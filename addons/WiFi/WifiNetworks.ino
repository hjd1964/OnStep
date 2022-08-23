// @todo: global state here not ideal
// would be better to return a token or something to that effect.
// code taken from Setup.ino
bool checkLogin() {
    String data = "";

    data=server.arg("login");
    if (data!="") {
        if (!strcmp(masterPassword,(char*)data.c_str())) loginRequired=false;
    }

    data=server.arg("logout");
    if (data!="") loginRequired=true;

    if (loginRequired) {
        server.send(401, "text/plain", "");
        return false;
    }

    return true;
}

void ajaxWifiNetworks()
{
    String extended;
    char tmp[420] = "";
    String data = "";

    if (!checkLogin()) {
        return;
    }

    sprintf(tmp, "sta_ssid|%s\n", wifi_sta_ssid);
    data += tmp;

    sprintf(tmp, "ap_ssid|%s\n", wifi_ap_ssid);
    data += tmp;

    if (strlen(wifi_sta_pwd) > 0) {
        data += "sta_secured|1\n";
    } else {
        data += "sta_secured|0\n";
    }

    sprintf(tmp, "ap_ssid|%s\n", wifi_ap_ssid);

    uint8_t macap[6] = {0,0,0,0,0,0}; WiFi.softAPmacAddress(macap);
    char wifi_ap_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_ap_mac,"%s%02x:",wifi_ap_mac,macap[i]); } wifi_ap_mac[strlen(wifi_ap_mac)-1]=0;
  
    sprintf(tmp, "ap_mac|%s\n", wifi_ap_mac); data += tmp;
    sprintf(tmp, "ap_ip|%d.%d.%d.%d\n", wifi_ap_ip[0],wifi_ap_ip[1],wifi_ap_ip[2],wifi_ap_ip[3]); data += tmp;
    sprintf(tmp, "ap_gw|%d.%d.%d.%d\n", wifi_ap_gw[0],wifi_ap_gw[1],wifi_ap_gw[2],wifi_ap_gw[3]); data += tmp;
    sprintf(tmp, "ap_sn|%d.%d.%d.%d\n", wifi_ap_sn[0],wifi_ap_sn[1],wifi_ap_sn[2],wifi_ap_sn[3]); data += tmp;

    uint8_t mac[6] = {0,0,0,0,0,0}; WiFi.macAddress(mac);
    char wifi_sta_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_sta_mac,"%s%02x:",wifi_sta_mac,mac[i]); } wifi_sta_mac[strlen(wifi_sta_mac)-1]=0;
    sprintf(tmp, "sta_mac|%s\n",wifi_sta_mac); data += tmp;
    sprintf(tmp, "sta_ip|%d.%d.%d.%d\n", wifi_sta_ip[0],wifi_sta_ip[1],wifi_sta_ip[2],wifi_sta_ip[3]); data += tmp;
    sprintf(tmp, "sta_gw|%d.%d.%d.%d\n", wifi_sta_gw[0],wifi_sta_gw[1],wifi_sta_gw[2],wifi_sta_gw[3]); data += tmp;
    sprintf(tmp, "sta_sn|%d.%d.%d.%d\n", wifi_sta_sn[0],wifi_sta_sn[1],wifi_sta_sn[2],wifi_sta_sn[3]); data += tmp;

    if (accessPointEnabled) {
        data += "ap|1\n";
    } else {
        data += "ap|0\n";
    }

    if (stationEnabled) {
        data += "sta|1\n";
    } else {
        data += "sta|0\n";
    }

    if (stationDhcpEnabled) {
        data += "sta_dhcp|1\n";
    } else {
        data += "sta_dhcp|0\n";
    }

    if (restartRequired) {
        data += "restart_required|1\n";
    } else {
        data += "restart_required|0\n";
    }

    server.send(200, "text/plain", data);
}

void ajaxScanNetworks()
{
    String data = "";

    if (!checkLogin()) {
        return;
    }

    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        data += WiFi.SSID(i);
        data += "|";
        data += WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "secured";
        data += "\n";
    }

    server.send(200, "text/plain", data);
}
