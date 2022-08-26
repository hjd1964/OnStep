import { OneZero } from "./responses";
export type WifiScan = Record<string, "open" | "secured">;
export type WifiSettingsResponse = {
  ap_ssid: string;
  ap_mac: string;
  ap_ip: string;
  ap_gw: string;
  ap_sn: string;

  sta_secured: OneZero;
  sta_ssid: string;
  sta_mac: string;
  sta_ip: string;
  sta_gw: string;
  sta_sn: string;

  ap: OneZero;
  sta: OneZero;
  sta_dhcp: OneZero;

  restart_required: OneZero;
};

export type NetworkConfig = {
  ip: string;
  ssid: string;
  mac: string;
  gw: string;
  sn: string;
};

export type WifiConfig = {
  station: {
    network: NetworkConfig & {
      dhcp: boolean;
      secured: boolean;
    };
    enabled: boolean;
  };
  accessPoint: {
    network: NetworkConfig;
    enabled: boolean;
  };
  restartRequired: boolean;
};
