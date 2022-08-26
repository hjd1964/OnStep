import {
  MountStatusResponse,
  TrackingModes,
  MountStatus,
  MaxSlewSpeed,
  OneZero,
  Mount,
  WifiSettingsResponse,
  WifiConfig,
  Coordinate,
} from "../types";

export function oneZeroBool(v: OneZero) {
  return v === "1";
}

export function getMount(status: MountStatusResponse): Mount {
  const rateComp = {
    rate_comp: status.rate_comp,
    rate_comp_type:
      status.rate_comp === "none"
        ? "none"
        : ["full_ra", "full_both"].includes(status.rate_comp)
        ? "full"
        : "refraction_only",
    rate_comp_axes: ["refr_both", "full_both"].includes(status.rate_comp)
      ? "dual"
      : "single",
  } as const;

  const baseMount = {
    backlash_ra: status.backlash_ra,
    backlash_dec: status.backlash_dec,
    min_alt: status.min_alt,
    max_alt: status.max_alt,
  } as const;

  switch (status.mount_type) {
    case "gem":
      return {
        type: "gem",
        auto_meridian: oneZeroBool(status.auto_meridian),
        pause_at_home: oneZeroBool(status.pause_at_home),
        deg_past_mer_e: status.deg_past_mer_e,
        deg_past_mer_w: status.deg_past_mer_w,
        ...rateComp,
        ...baseMount,
      };

    case "alt_az":
      return {
        type: "alt_az",
        ...baseMount,
      };
  }

  return {
    type: status.mount_type,
    ...rateComp,
    ...baseMount,
  };
}

export function parseResponse(
  status: MountStatusResponse,
  tracking: TrackingModes,
  settings: {
    speed: number;
  }
): MountStatus {
  if (status.type !== "valid") {
    return {
      type: "invalid",
      lastError: status.last_error === "0" ? false : status.last_error,
    };
  }

  const rateRatio =
    parseFloat(status.current_rate) / parseFloat(status.nominal_rate);
  let trackSpeed: MaxSlewSpeed = "vs";

  if (rateRatio > 1.75) {
    trackSpeed = "vf";
  } else if (rateRatio > 1.25) {
    trackSpeed = "f";
  } else if (rateRatio > 0.875) {
    trackSpeed = "n";
  } else if (rateRatio > 0.625) {
    trackSpeed = "s";
  }

  const [maxStars, currentStar, lastRequiredStar] = status.align_stars
    .split("")
    .map((v) => parseInt(v));

  function getDate(date: string, time: string) {
    const [_m, _d, _y] = date.split("/");

    const times = time.split(":");

    const formattedDate = [
      `20${_y.padStart(2, "0")}`,
      _m.padStart(2, "0"),
      _d.padStart(2, "0"),
    ].join("-");

    const formattedTime = times.map((time) => time.padStart(2, "0")).join(":");

    return [[formattedDate, formattedTime].join("T"), "Z"].join("");
  }

  const dateTime = {
    // utc_date|8/13/22
    // utc_time|6:19:31
    // sidereal|22:24:17
    utc: new Date(getDate(status.utc_date, status.utc_time)),
    utcOffset: status.utc_offset,
    browser: new Date(),
  };

  const dateDiff = Math.abs(
    dateTime.browser.getTime() - dateTime.utc.getTime()
  );

  const datesAreOutOfSync = dateDiff > 60000;

  function getLocation(loc: string): Coordinate {
    const nums = loc
      .split(/[:*]/)
      .map((v) => parseInt(v))
      .map((v) => String(v));
    const keys = ["deg", "min", "sec"] as const;

    return Object.fromEntries(
      keys.map((key, i) => [key, nums[i]])
    ) as Coordinate;
  }

  return {
    type: status.type,
    lastError: status.last_error === "0" ? false : status.last_error,
    version: {
      onstep: status.mount_version,
      wifi: status.server_version,
    },
    mount: getMount(status),
    dateTime: {
      ...dateTime,
      datesAreOutOfSync,
    },
    features: {
      buzzer: oneZeroBool(status.buzzer),
    },
    position: {
      current: {
        ra: status.current_ra,
        dec: status.current_dec,
      },
      target: {
        ra: status.target_ra,
        dec: status.target_dec,
      },
    },
    location: {
      long: getLocation(status.long),
      lat: getLocation(status.lat),
    },
    alignment: {
      maxStars,
      currentStar,
      lastRequiredStar,
    },
    status: {
      waitingAtHome: oneZeroBool(status.waiting_at_home),
      tracking: oneZeroBool(status.tracking),
      parked: oneZeroBool(status.parked),
      parkFail: status.park_fail === "1",
      slewing: oneZeroBool(status.slewing),
      home: oneZeroBool(status.home),
      aligning: oneZeroBool(status.aligning),
    },
    slewing: {
      maxSpeed: trackSpeed,
      speed: settings.speed,
    },
    tracking: {
      type: tracking,
    },
  };
}

export function parseWifiSettingsResponse(
  response: WifiSettingsResponse
): WifiConfig {
  return {
    restartRequired: oneZeroBool(response.restart_required),
    station: {
      network: {
        ip: response.sta_ip,
        ssid: response.sta_ssid,
        mac: response.sta_mac,
        gw: response.sta_gw,
        sn: response.sta_sn,
        secured: oneZeroBool(response.sta_secured),
        dhcp: oneZeroBool(response.sta_dhcp),
      },
      enabled: oneZeroBool(response.sta),
    },
    accessPoint: {
      network: {
        ip: response.ap_ip,
        ssid: response.ap_ssid,
        mac: response.ap_mac,
        gw: response.ap_gw,
        sn: response.ap_sn,
      },
      enabled: oneZeroBool(response.ap),
    },
  };
}
