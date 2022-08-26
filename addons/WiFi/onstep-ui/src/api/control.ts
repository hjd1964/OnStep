import { toast } from "../utils/toast";
import { API } from "./api";
import {
  MountStatusResponse,
  TrackingModes,
  MountStatus,
  MaxSlewSpeed,
  Direction,
  ValidMountStatus,
  MountWithCompensation,
  mountHasCompensation,
  WifiSettingsResponse,
  Mount,
} from "../types";
import { Star } from "../utils/stars";
import { objectsEqual } from "../utils/compareObjects";
import { parseResponse, parseWifiSettingsResponse } from "./parseResponse";
import { Mutex } from "async-mutex";

export class MountStatusApi {
  private heartbeat?: number;
  private onStatus?: (status: MountStatus) => void;
  private status?: MountStatus;
  private mutex;
  // todo: there isn't a convenient way to get this from the API.
  // store the state of the speed locally whenever someone changes it.
  private speed: number = 0;
  public commandLogs: { command: string; response: string }[] = [];

  /**
   * todo: it doesn't seem you can query onstep to see what
   * the current tracking mode is.. however, when
   * onstep boots up, it's in 'sidereal' - so track
   * this value internally for the sake of UI feedback.
   */
  private trackingMode: TrackingModes = "sidereal";

  constructor(private api: API) {
    this.mutex = new Mutex();
  }

  startHeartbeat(
    onStatus: (status: MountStatus) => void,
    onLoading: (loading: boolean) => void
  ) {
    if (this.heartbeat) {
      throw new Error("Heartbeat is already ticking.");
    }

    this.onStatus = onStatus;

    this.heartbeat = setInterval(
      () => this.getStatus(onLoading),
      4000
    ) as any as number;
    return this;
  }

  stopHeartbeat() {
    if (this.heartbeat) {
      clearInterval(this.heartbeat);
      this.heartbeat = undefined;
    }
  }

  private async getStatus(onLoading?: (loading: boolean) => void) {
    onLoading?.(true);

    await this.mutex.runExclusive(async () => {
      const status = await this.api.get<MountStatusResponse>("ajax/status");

      this.status = parseResponse(status, this.trackingMode, {
        speed: this.speed,
      });

      if (this.status.lastError) {
        toast(`A background error occured! ${this.status.lastError}`, "error");
      }

      this.onStatus?.(this.status);
      onLoading?.(false);
    });
  }

  slew(dir: Direction, startStop: boolean) {
    return this.api.get("controlA.txt", {
      dr: `${dir}${startStop ? 1 : 0}`,
    });
  }

  async updateMountSettings(mount: Mount) {
    const updates: Record<string, any> = {};

    const compareKeys: string[] = [
      "max_alt",
      "min_alt",
      "backlash_ra",
      "backlash_dec",
    ];

    updates["ol"] = mount.max_alt;
    updates["hl"] = mount.min_alt;
    updates["b1"] = mount.backlash_ra;
    updates["b2"] = mount.backlash_dec;

    if (mount.type === "gem") {
      updates["el"] = mount.deg_past_mer_e;
      updates["wl"] = mount.deg_past_mer_w;
      compareKeys.push(...["deg_past_mer_e", "deg_past_mer_w"]);
    }

    const compareUpdate = Object.fromEntries(
      compareKeys.map((k) => [k, mount[k as keyof typeof mount]])
    );

    await this.api.get("configurationA.txt", updates);

    await this.waitForStatus({
      test: (status) => {
        const thisStatus = Object.fromEntries(
          compareKeys.map((k) => [
            k,
            status.mount[k as keyof typeof status.mount],
          ])
        );

        return objectsEqual(thisStatus, compareUpdate);
      },
      onTimeout: (s) =>
        toast(`Failed to update mount settings after ${s} seconds.`),
    });
  }

  async setLocation(location: ValidMountStatus["location"]) {
    await this.api.get("configurationA.txt", {
      g1: location.long.deg,
      g2: location.long.min,
      g3: location.long.sec,

      t1: location.lat.deg,
      t2: location.lat.min,
      t3: location.lat.sec,
    });

    await this.waitForStatus({
      test: (status) => objectsEqual(status.location, location),
      maxSeconds: 5,
      onTimeout: () => toast(`Failed to update location after 5 seconds.`),
    });
  }

  async setDateTime(offset?: string) {
    var d = new Date();

    const localUpdate = {
      dd: d.getDate(),
      dm: d.getMonth(),
      dy: d.getFullYear(),
      th: d.getHours(),
      tm: d.getMinutes(),
      ts: d.getSeconds(),
    };

    const utcOffset =
      typeof offset === "string"
        ? parseFloat(offset) * 60
        : new Date().getTimezoneOffset();
    const utcOffsetHours = Math.floor(utcOffset / 60);
    const utcOffsetMinutes = (utcOffset / 60 - utcOffsetHours) * 60;

    // set the timezone first
    await this.api.get("configurationA.txt", {
      u1: utcOffsetHours,
      u2: utcOffsetMinutes,
    });

    // then set the local date/time
    await this.api.get("controlA.txt", {
      qb: "st",
      ...localUpdate,
    });

    await this.waitForStatus({
      test: (status) => !status.dateTime.datesAreOutOfSync,
      maxSeconds: 5,
      onTimeout: () =>
        toast(
          `After 5 seconds, the date/time still appears to be out of sync.`,
          "error"
        ),
    });
  }

  async setNewHome() {
    await this.api.get("controlA.txt", {
      dr: "qr",
    });

    await this.getStatus();
  }

  async goHome() {
    await this.api.get("controlA.txt", {
      dr: "qh",
    });

    await this.waitForStatus({
      test: (status) => {
        return status.status.slewing || status.status.home;
      },
      maxSeconds: 5,
      onTimeout(v) {
        toast(`Failed to start homing after ${v} seconds`, "error");
      },
    });

    await this.waitForStatus({
      test: (status) => {
        return status.status.home;
      },
      maxSeconds: 100,
    });
  }

  async doMeridianFlip() {
    await this.api.get("controlA.txt", {
      qb: "co",
    });

    await this.getStatus();
  }

  async setMeridianAutoFlipNow() {
    await this.api.get("settingsA.txt", {
      ma: "now",
    });
  }

  async setMeridianAutoFlip(enable: boolean) {
    await this.api.get("settingsA.txt", {
      ma: enable ? "on" : "off",
    });

    await this.waitForStatus({
      test: (status) =>
        status.mount.type === "gem" && status.mount.auto_meridian === enable,
      maxSeconds: 5,
      onTimeout: (s) =>
        toast(
          `Failed to ${
            enable ? "enable" : "disable"
          } meridian flip after ${s} seconds.`,
          "error"
        ),
    });
  }

  async setMeridianPauseAtHome(enable: boolean) {
    await this.api.get("settingsA.txt", {
      mp: enable ? "on" : "off",
    });

    await this.waitForStatus({
      test: (status) =>
        status.mount.type === "gem" && status.mount.pause_at_home === enable,
      maxSeconds: 5,
      onTimeout: (s) =>
        toast(
          `Failed to ${
            enable ? "enable" : "disable"
          } auto pause at home after ${s} seconds.`,
          "error"
        ),
    });
  }

  async setBuzzer(enable: boolean) {
    await this.api.get("settingsA.txt", {
      ab: enable ? "on" : "off",
    });

    await this.waitForStatus({
      test: (status) => status.features.buzzer === enable,
      maxSeconds: 5,
      onTimeout: (s) =>
        toast(`Failed to change buzzer status after ${s} seconds.`, "error"),
    });
  }

  async changeSpeed(speed: number) {
    await this.api.get("controlA.txt", {
      dr: `R${speed}`,
    });

    this.speed = speed;
    await this.getStatus();
  }

  async changeMaxSlewSpeed(speed: MaxSlewSpeed) {
    await this.api.get("settingsA.txt", {
      ss: speed,
    });

    await this.waitForStatus({
      test: (status) => status.slewing.maxSpeed === speed,
      maxSeconds: 5,
      onTimeout: (s) =>
        toast(
          `Failed to change slewing speed to ${speed} after ${s} seconds`,
          "error"
        ),
    });
  }

  async setTrackingRateComp(
    rate: MountWithCompensation["rate_comp_type"],
    axes: MountWithCompensation["rate_comp_axes"]
  ) {
    // todo: this didn't really come out so great.
    const apiRate = {
      full: {
        dual: {
          status: "full_both",
          command: ["otk", "don"],
        },
        single: {
          status: "full_ra",
          command: ["otk", "doff"],
        },
      },
      refraction_only: {
        dual: {
          status: "refr_both",
          command: ["on", "don"],
        },
        single: {
          status: "refr_ra",
          command: ["on", "doff"],
        },
      },
      none: {
        dual: {
          status: "none",
          command: ["off"],
        },
        single: {
          status: "none",
          command: ["off"],
        },
      },
    }[rate][axes];

    for (const rr of apiRate.command) {
      await this.api.get("settingsA.txt", {
        rr,
      });
    }

    await this.waitForStatus({
      test: (status) => {
        const mount = status.mount;

        if (!mountHasCompensation(mount)) {
          return false;
        }

        return mount.rate_comp === apiRate.status;
      },
    });
  }

  async syncWith() {
    await this.api.get("controlA.txt", {
      dr: "sy",
    });
  }
  async startTracking() {
    await this.unPark();
    await this.api.get("controlA.txt", {
      dr: "T1",
    });

    await this.getStatus();

    await this.waitForStatus({
      test: ({ status }) => status.tracking,
      onTimeout: () => toast("Failed to start tracking.", "error"),
    });
  }

  async adjustTrackingRate(type: "faster" | "slower" | "reset") {
    await this.api.get("settingsA.txt", {
      tk: {
        faster: "f",
        slower: "-",
        reset: "r",
      }[type],
    });

    await this.getStatus();
  }

  async continueGoTo() {
    await this.api.get("controlA.txt", {
      dr: "qc",
    });

    await this.waitForStatus({
      test: (status) => !status.status.waitingAtHome,
      onTimeout: (s) => toast(`Failed to continue GoTo after ${s} seconds.`),
    });
  }
  /**
   * TODO: it would be nice to be able to store the star name in the firmware
   * somewhere so that we could show the user "going to star xyz"
   */
  async goToStar(star: Star) {
    await this.sendCommand(`:Sr${star.ra}#`);
    await this.sendCommand(`:Sd${star.dec}#`);
    const result = String(await this.sendCommand(":MS#"));

    if (result !== "0") {
      const message =
        {
          // error messages taken from source code
          "1": "the target is below the horizon limit",
          "2": "the target is above the overhead limit",
          "3": "the controller is in standby",
          "4": "the mount is parked",
          "5": "a GoTo already in progress",
          "6": "the move is outside the mount limits",
          "7": "there was a hardware fault",
          "8": "the mount is already in motion",
        }[result] ?? `an unknown error occured ${result}`;

      toast(`An error occured: ${message}.`, "error");
      return;
    }

    await this.waitForStatus({
      test: (status) => status.status.slewing,
      maxSeconds: 15,
      onTimeout: () =>
        toast(
          `Failed to initiate GoTo after 15 seconds. The target may be out of bounds for your scope.`,
          "error"
        ),
    });
  }

  async sendCommand(cmd: string, log = false) {
    const response = (
      await this.api.getWithoutParse("ajax/cmd", {
        cmd,
      })
    ).data;

    if (log) {
      this.commandLogs.unshift({
        command: cmd,
        response,
      });
    }

    return response;
  }

  async stopTracking() {
    await this.api.get("controlA.txt", {
      dr: "T0",
    });

    await this.getStatus();

    await this.waitForStatus({
      test: ({ status }) => !status.tracking,
      onTimeout: () => toast("Failed to stop tracking.", "error"),
    });
  }

  async startAlignment(stars: number) {
    await this.api.get("controlA.txt", {
      al: stars.toString(),
    });

    await this.waitForStatus({
      test: (status) => status.status.aligning,
    });
  }

  async nextStarAlignment() {
    if (this.status?.type !== "valid") {
      throw new Error("Mount is not valid.");
    }

    const currentStar = this.status.alignment.currentStar;

    await this.api.get("controlA.txt", {
      dr: "sy",
    });

    await this.waitForStatus({
      test: (status) => {
        // if (currentStar + 1 === status.alignment.lastRequiredStar
        return status.alignment.currentStar !== currentStar;
      },
    });
  }

  async acceptAlignment() {
    await this.api.get("controlA.txt", {
      al: "n",
    });

    await this.waitForStatus({
      test: (status) => !status.status.aligning,
    });
  }

  async setParkingLocation() {
    await this.api.get("settingsA.txt", {
      pk: "s",
    });

    await this.getStatus();
  }

  async park() {
    await this.api.get("controlA.txt", {
      dr: "pk",
    });

    await this.getStatus();

    await this.waitForStatus({
      test: ({ status }) => {
        return status.slewing;
      },
      maxSeconds: 5,
      onTimeout(seconds) {
        toast(`Failed to start parking after ${seconds} seconds.`, "error");
      },
    });

    await this.waitForStatus({
      test: ({ status }) => {
        return status.parked;
      },
      onTimeout(seconds) {
        toast(`Failed to park after ${seconds} seconds.`, "error");
      },
    });
  }

  async unPark() {
    await this.api.get("controlA.txt", {
      dr: "pu",
    });

    await this.getStatus();

    await this.waitForStatus({
      test: ({ status }) => !status.parked,
      maxSeconds: 2,
      onTimeout(s) {
        toast(`Failed to unpark after ${s} seconds.`);
      },
    });
  }

  async changeTrackingType(mode: TrackingModes) {
    const dr = {
      sidereal: "Ts",
      lunar: "Tl",
      solar: "Th",
    }[mode];

    await this.api.get("controlA.txt", {
      dr,
    });

    this.trackingMode = mode;
    await this.getStatus();
  }

  async estop() {
    await this.api.get("controlA.txt", { dr: "qq" });
    toast(
      "An E-Stop command has been sent to the mount. Hope everything's ok."
    );
  }

  private waitForStatus(params: {
    test: (status: ValidMountStatus) => boolean;
    maxSeconds?: number;
    onTimeout?: (seconds: number) => void;
  }) {
    const { test, maxSeconds, onTimeout } = params;
    let counter = 0;
    const _maxSeconds = maxSeconds ?? 30;

    const waitFor = (
      resolve: (v: boolean) => void,
      reject: (v: number) => void
    ) => {
      if (counter++ > _maxSeconds) {
        onTimeout?.(_maxSeconds);
        reject(_maxSeconds);
        return;
      }

      const status = this.status;

      if (status?.type !== "valid") {
        setTimeout(() => waitFor(resolve, reject), 1000);
        return;
      }

      if (test(status)) {
        resolve(true);
        return;
      }

      setTimeout(() => waitFor(resolve, reject), 1000);
    };

    return new Promise((resolve, reject) => {
      waitFor(resolve, reject);
    });
  }

  private checkStatus(fn: (status: ValidMountStatus) => void) {
    const status = this.status;
    if (!status || status?.type !== "valid") {
      return toast("Failed to get status.", "error");
    }

    fn(status);
  }
}
