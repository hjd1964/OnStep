import { API } from "./api";
import { WifiConfig, WifiSettingsResponse } from "../types";
import { parseWifiSettingsResponse } from "./parseResponse";

function splitIp(ip: string, keyPrefix: string) {
  return Object.fromEntries(
    ip.split(".").map((num, index) => [`${keyPrefix}${index + 1}`, num])
  );
}

function bTo10(b: boolean) {
  return b ? 1 : 0;
}

export class WifiApi {
  constructor(private api: API) {}
  wifiLogout() {
    return this.api.get("ajax/wifi", {
      logout: true,
    });
  }

  async scanForWifiNetworks() {
    return this.api.get<Record<string, "open" | "secured">>("ajax/wifi/scan");
  }

  async getWifiSettings(login?: string) {
    return parseWifiSettingsResponse(
      await this.api.get<WifiSettingsResponse>("ajax/wifi", {
        login,
      })
    );
  }

  async updateStation(config: WifiConfig["station"], password?: string) {
    const updates = {
      ...(password?.length ? { stpwd: password } : {}),
      stssid: config.network.ssid,
      staen: bTo10(config.enabled),
      stadhcp: bTo10(config.network.dhcp),
      ...splitIp(config.network.ip, "staip"),
      ...splitIp(config.network.sn, "stasn"),
      ...splitIp(config.network.gw, "stagw"),
    };
    await this.api.get("wifi.htm", updates);
    return this.getWifiSettings();
  }

  async updateAp(config: WifiConfig["accessPoint"], password?: string) {
    const updates = {
      ...(password?.length ? { appwd: password } : {}),
      apen: bTo10(config.enabled),
      apssid: config.network.ssid,
      ...splitIp(config.network.ip, "apip"),
      ...splitIp(config.network.sn, "apsn"),
      ...splitIp(config.network.gw, "apgw"),
    };

    await this.api.get("wifi.htm", updates);
    return this.getWifiSettings();
  }
}
