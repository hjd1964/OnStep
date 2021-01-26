// -----------------------------------------------------------------------------------
// Wifi Setup

const char html_wifiSerial[] PROGMEM =
"<b>" L_WIFI_PERFORMANCE ":</b><br/>"
"<form method='post' action='/wifi.htm'>"
L_WIFI_CMD_TO ": <input style='width:4em' name='ccto' value='%d' type='number' min='5' max='100'> ms<br/>"
L_WIFI_WWW_TO ": <input style='width:4em' name='wcto' value='%d' type='number' min='5' max='100'> ms<br/>"
"<button type='submit'>" L_UPLOAD "</button></form><br />\r\n";
const char html_wifiSSID1[] PROGMEM =
"<br/><b>" L_WIFI_STA_TITLE ":</b><br/>"
"<form method='post' action='/wifi.htm'>"
"SSID: <input style='width:6em' name='stssid' type='text' value='%s' maxlength='32'>&nbsp;&nbsp;&nbsp;"
L_WIFI_PWD ": <input style='width:8em' name='stpwd' type='password' value='%s' minlength='8' maxlength='39'> (" L_WIFI_PWD_MSG ")<br/>";
const char html_wifiSSID2[] PROGMEM =
L_WIFI_EN_DHCP ": <input type='checkbox' name='stadhcp' value='1' %s> (" L_WIFI_EN_DHCP_MSG ")<br/>"
L_WIFI_EN_STA ": <input type='checkbox' name='staen' value='1' %s><br/>"
"<button type='submit'>" L_UPLOAD "</button></form><br />\r\n";

const char html_wifiMAC[] PROGMEM =
"MAC: <input style='width:10em' name='stmac' type='text' value='%s' maxlength='17' disabled><br/>";

const char html_wifiSTAIP[] PROGMEM =
"<table><tr><td>" L_IP_ADDRESS ": </td><td>"
"<input name='staip1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSTAGW[] PROGMEM =
"<tr><td>" L_GATEWAY ": </td><td>"
"<input name='stagw1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSTASN[] PROGMEM =
"<tr><td>" L_SUBNET ": </td><td>"
"<input name='stasn1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn4' value='%d' type='number' min='0' max='255'></td></tr></table>";

const char html_wifiSSID3A[] PROGMEM =
"<br/><b>" L_WIFI_AP ":</b><br/>"
"<form method='post' action='/wifi.htm'>"
"SSID: <input style='width:6em' name='apssid' type='text' ";
const char html_wifiSSID3B[] PROGMEM =
"value='%s' maxlength='32'>&nbsp;&nbsp;&nbsp;"
L_WIFI_PWD ": <input style='width:8em' name='appwd' type='password' value='%s' minlength='8' maxlength='39'> " L_WIFI_PWD_MSG "&nbsp;&nbsp;&nbsp;"
L_WIFI_CHA ": <input style='width:3em' name='apch' value='%d' type='number' min='1' max='11'><br/>";

const char html_wifiApMAC[] PROGMEM =
"MAC: <input style='width:10em' name='apmac' type='text' value='%s' maxlength='17' disabled><br/>";

const char html_wifiSSID4[] PROGMEM =
"<table><tr><td>" L_IP_ADDRESS ": </td><td>"
"<input name='apip1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSSID5[] PROGMEM =
"<tr><td>" L_GATEWAY ": </td><td>"
"<input name='apgw1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSSID6[] PROGMEM =
"<tr><td>" L_SUBNET ": </td><td>"
"<input name='apsn1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn4' value='%d' type='number' min='0' max='255'></td></tr></table>";

const char html_wifiSSID7[] PROGMEM =
L_WIFI_EN_AP_MODE ": <input type='checkbox' name='apen' value='1' %s> (Note: auto-enabled if Station Mode fails to connect)<br/>"
"<button type='submit'>" L_UPLOAD "</button></form><br />\r\n";

const char html_logout[] PROGMEM =
"<br/><b>" L_WIFI_CONFIG_SECURITY ":</b><br/>"
"<form method='post' action='/wifi.htm'>"
L_WIFI_PWD ": <input style='width:8em' name='webpwd' type='password' minlength='3' maxlength='39'> "
"<button type='submit'>" L_UPLOAD "</button></form>"
"<form method='post' action='/wifi.htm'>"
"<button type='submit' name='logout' value='1'>" L_LOGOUT "</button></form><br />\r\n";

const char html_reboot[] PROGMEM =
"<br/><br/><br/><br/><br/><form method='get' action='/wifi.htm'>"
"<b>" L_WIFI_RESTART_MSG "</b><br/><br/>"
"<button type='submit'>" L_CONTINUE "</button>"
"</form><br/><br/><br/><br/>"
"\r\n";

const char html_login[] PROGMEM =
"<br/><form method='post' action='/wifi.htm'>"
"<br/>" L_WIFI_TITLE "<br />"
"<input style='width:8em' name='login' type='password' minlength='3' maxlength='39'>"
"<button type='submit'>" L_OK "</button>"
"</form><br/><br/><br/>"
L_WIFI_ADVICE1 L_WIFI_ADVICE2 L_WIFI_ADVICE3 L_WIFI_ADVICE4 L_WIFI_ADVICE5 L_WIFI_ADVICE6 L_WIFI_ADVICE7 L_WIFI_ADVICE8 L_WIFI_ADVICE9 
"<br/><br/>\r\n";

bool restartRequired=false;
bool loginRequired=true;

void handleWifi() {
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
  
  mountStatus.update(true);

  char temp[420]="";
  char temp1[140]="";
  
  processWifiGet();

  sendHtmlStart();

  // send a standard http response header
  String data=FPSTR(html_headB);
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  data += FPSTR(html_main_css5);
  sendHtml(data);
  data += FPSTR(html_main_css6);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  // finish the standard http response header
  data += FPSTR(html_onstep_header1); data += "OnStep";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_linksStatN);
  data += FPSTR(html_linksCtrlN);
  if (mountStatus.featureFound()) data += FPSTR(html_linksAuxN);
  data += FPSTR(html_linksLibN);
#if ENCODERS == ON
  data += FPSTR(html_linksEncN);
#endif
  sendHtml(data);
  data += FPSTR(html_linksPecN);
  data += FPSTR(html_linksSetN);
  data += FPSTR(html_linksCfgN);
  data += FPSTR(html_linksWifiS);
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data+="<div>";

  if (restartRequired) {
    restartRequired=false;
    data+=FPSTR(html_reboot);
  } else
  if (loginRequired) {
    restartRequired=false;
    data+=FPSTR(html_login);
  } else {
    nv.readString(EE_STA_SSID,wifi_sta_ssid);
    nv.readString(EE_STA_PWD,wifi_sta_pwd);
      
    sprintf_P(temp,html_wifiSerial,cmdTimeout,webTimeout); data += temp;
    sprintf_P(temp,html_wifiSSID1,wifi_sta_ssid,""); data += temp;
    
    uint8_t mac[6] = {0,0,0,0,0,0}; WiFi.macAddress(mac);
    char wifi_sta_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_sta_mac,"%s%02x:",wifi_sta_mac,mac[i]); } wifi_sta_mac[strlen(wifi_sta_mac)-1]=0;
    sprintf_P(temp,html_wifiMAC,wifi_sta_mac); data += temp;
  
    sprintf_P(temp,html_wifiSTAIP,wifi_sta_ip[0],wifi_sta_ip[1],wifi_sta_ip[2],wifi_sta_ip[3]); data += temp;
    sprintf_P(temp,html_wifiSTAGW,wifi_sta_gw[0],wifi_sta_gw[1],wifi_sta_gw[2],wifi_sta_gw[3]); data += temp;
    sprintf_P(temp,html_wifiSTASN,wifi_sta_sn[0],wifi_sta_sn[1],wifi_sta_sn[2],wifi_sta_sn[3]); data += temp;
    sprintf_P(temp,html_wifiSSID2,stationDhcpEnabled?"checked":"",stationEnabled?"checked":""); data += temp;
    data += FPSTR(html_wifiSSID3A);
    sprintf_P(temp,html_wifiSSID3B,wifi_ap_ssid,"",wifi_ap_ch); data += temp;
    sendHtml(data);
  
    uint8_t macap[6] = {0,0,0,0,0,0}; WiFi.softAPmacAddress(macap);
    char wifi_ap_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_ap_mac,"%s%02x:",wifi_ap_mac,macap[i]); } wifi_ap_mac[strlen(wifi_ap_mac)-1]=0;
    sprintf_P(temp,html_wifiApMAC,wifi_ap_mac); data += temp;
    
    sprintf_P(temp,html_wifiSSID4,wifi_ap_ip[0],wifi_ap_ip[1],wifi_ap_ip[2],wifi_ap_ip[3]); data += temp;
    sprintf_P(temp,html_wifiSSID5,wifi_ap_gw[0],wifi_ap_gw[1],wifi_ap_gw[2],wifi_ap_gw[3]); data += temp;
    sprintf_P(temp,html_wifiSSID6,wifi_ap_sn[0],wifi_ap_sn[1],wifi_ap_sn[2],wifi_ap_sn[3]); data += temp;
    sprintf_P(temp,html_wifiSSID7,accessPointEnabled?"checked":""); data += temp;
    data += FPSTR(html_logout);
  }
  
  strcpy(temp,"</div></div></body></html>");
  data += temp;

  sendHtml(data);
  sendHtmlDone(data);
}

void processWifiGet() {
  String v,v1;
  
  boolean EEwrite=false;

  // Login --------------------------------------------------------------------
  v=server.arg("login");
  if (v!="") {
    if (!strcmp(masterPassword,(char*)v.c_str())) loginRequired=false;
  }
  v=server.arg("logout");
  if (v!="") loginRequired=true;
  if (loginRequired) return;
  v=server.arg("webpwd");
  if (v!="") {
    strcpy(masterPassword,(char*)v.c_str());
    nv.writeString(EE_PASSWORD,masterPassword);
    EEwrite=true;
  }

  // Timeouts -----------------------------------------------------------------
  // Cmd channel timeout
  v=server.arg("ccto");
  if (v!="") {
    cmdTimeout=v.toInt();
    nv.writeInt(EE_TIMEOUT_CMD,(int)cmdTimeout);
    EEwrite=true;
  }

  // Web channel timeout
  v=server.arg("wcto");
  if (v!="") {
    webTimeout=v.toInt();
    nv.writeInt(EE_TIMEOUT_WEB,(int)webTimeout);
    EEwrite=true;
  }

  // --------------------------------------------------------------------------------------------------------

  // Station MAC
  v=server.arg("stmac");
  if (v!="") {
    // 5c:cf:7f:0f:ad:85
    // first the length should be 17
    if (v.length()==17) {
      // seperators all in place
      if ((v.charAt(2)==':') && (v.charAt(5)==':') && (v.charAt(8)==':') && (v.charAt(11)==':') && (v.charAt(14)==':')) {
        // digits all in 0..9,A..F and validate
        v.toUpperCase();
        uint8_t mac[6];
        int imac[6];
        imac[0]=hexToInt(v.substring(0,2)); imac[1]=hexToInt(v.substring(3,2)); imac[2]=hexToInt(v.substring(6,2));
        imac[3]=hexToInt(v.substring(9,2)); imac[4]=hexToInt(v.substring(12,2)); imac[5]=hexToInt(v.substring(15,2));
        if ((imac[0]>=0) && (imac[1]>=0) && (imac[2]>=0) && (imac[3]>=0) && (imac[4]>=0) && (imac[5]>=0)) {
          mac[0]=imac[0]; mac[1]=imac[1]; mac[2]=imac[2]; mac[3]=imac[3]; mac[4]=imac[4]; mac[5]=imac[5]; 
          WiFi.macAddress(mac); restartRequired=true; 
        }
      }
    }
  }

  // Station SSID
  v=server.arg("stssid"); v1=v;
  if (v!="") {
    if (!strcmp(wifi_sta_ssid,(char*)v.c_str())) restartRequired=true;
    strcpy(wifi_sta_ssid,(char*)v.c_str());

    // if this section was submitted set the stationEnabled default to false
    stationDhcpEnabled=false;
    stationEnabled=false;
  }

  // Station password
  v=server.arg("stpwd");
  if (v!="") {
    if (!strcmp(wifi_sta_pwd,(char*)v.c_str())) restartRequired=true;
    strcpy(wifi_sta_pwd,(char*)v.c_str());
  }

  // Station dhcp enabled
  v=server.arg("stadhcp");
  if (v!="") {
    stationDhcpEnabled=v.toInt();
  }

  // Station enabled
  v=server.arg("staen");
  if (v!="") {
    stationEnabled=v.toInt();
  }

  // Access-Point ip
  IPAddress old_ip=wifi_sta_ip;
  v=server.arg("staip1"); if (v!="") wifi_sta_ip[0]=v.toInt();
  v=server.arg("staip2"); if (v!="") wifi_sta_ip[1]=v.toInt();
  v=server.arg("staip3"); if (v!="") wifi_sta_ip[2]=v.toInt();
  v=server.arg("staip4"); if (v!="") wifi_sta_ip[3]=v.toInt();

  // Access-Point SubNet
  old_ip=wifi_sta_sn;
  v=server.arg("stasn1"); if (v!="") wifi_sta_sn[0]=v.toInt();
  v=server.arg("stasn2"); if (v!="") wifi_sta_sn[1]=v.toInt();
  v=server.arg("stasn3"); if (v!="") wifi_sta_sn[2]=v.toInt();
  v=server.arg("stasn4"); if (v!="") wifi_sta_sn[3]=v.toInt();

  // Access-Point Gateway
  old_ip=wifi_sta_gw;
  v=server.arg("stagw1"); if (v!="") wifi_sta_gw[0]=v.toInt();
  v=server.arg("stagw2"); if (v!="") wifi_sta_gw[1]=v.toInt();
  v=server.arg("stagw3"); if (v!="") wifi_sta_gw[2]=v.toInt();
  v=server.arg("stagw4"); if (v!="") wifi_sta_gw[3]=v.toInt();
    
  if (v1!="") {
    nv.writeString(EE_STA_SSID,wifi_sta_ssid);
    nv.writeString(EE_STA_PWD,wifi_sta_pwd);
    nv.writeInt(EE_DHCP_EN,(int)stationDhcpEnabled);
    nv.writeInt(EE_STA_EN,(int)stationEnabled);
    for (int i=0;i<4;i++) nv.write(EE_STA_IP+i,wifi_sta_ip[i]);
    for (int i=0;i<4;i++) nv.write(EE_STA_GW+i,wifi_sta_gw[i]);
    for (int i=0;i<4;i++) nv.write(EE_STA_SN+i,wifi_sta_sn[i]);
    EEwrite=true;
    restartRequired=true;
  }

  // -------------------------------------------------------------------------------------------
  
  // Access-Point MAC
  v=server.arg("apmac");
  if (v!="") {
    // 5c:cf:7f:0f:ad:85
    // first the length should be 17
    if (v.length()==17) {
      // seperators all in place
      if ((v.charAt(2)==':') && (v.charAt(5)==':') && (v.charAt(8)==':') && (v.charAt(11)==':') && (v.charAt(14)==':')) {
        // digits all in 0..9,A..F and validate
        v.toUpperCase();
        uint8_t mac[6];
        int imac[6];
        imac[0]=hexToInt(v.substring(0,2)); imac[1]=hexToInt(v.substring(3,2)); imac[2]=hexToInt(v.substring(6,2));
        imac[3]=hexToInt(v.substring(9,2)); imac[4]=hexToInt(v.substring(12,2)); imac[5]=hexToInt(v.substring(15,2));
        if ((imac[0]>=0) && (imac[1]>=0) && (imac[2]>=0) && (imac[3]>=0) && (imac[4]>=0) && (imac[5]>=0)) {
          mac[0]=imac[0]; mac[1]=imac[1]; mac[2]=imac[2]; mac[3]=imac[3]; mac[4]=imac[4]; mac[5]=imac[5];
          WiFi.softAPmacAddress(mac); restartRequired=true; 
        }
      }
    }
  }

  // Access-Point SSID
  v=server.arg("apssid");
  if (v!="") {
    if (!strcmp(wifi_ap_ssid,(char*)v.c_str())) restartRequired=true;
    strcpy(wifi_ap_ssid,(char*)v.c_str());

    // if this section was submitted set the accessPointEnabled default to false
    accessPointEnabled=false;
  }

  // Access-Point password
  v=server.arg("appwd");
  if (v!="") {
    if (!strcmp(wifi_ap_pwd,(char*)v.c_str())) restartRequired=true;
    strcpy(wifi_ap_pwd,(char*)v.c_str());
  }

  // Access-Point channel
  v=server.arg("apch");
  if (v!="") {
    if (wifi_ap_ch!=v.toInt()) restartRequired=true;
    wifi_ap_ch=v.toInt();
  }

  // Access-Point enabled
  v=server.arg("apen");
  if (v!="") {
    accessPointEnabled=v.toInt();
    EEwrite=true;
  }

  // Access-Point ip
  old_ip=wifi_ap_ip;
  v=server.arg("apip1"); if (v!="") wifi_ap_ip[0]=v.toInt();
  v=server.arg("apip2"); if (v!="") wifi_ap_ip[1]=v.toInt();
  v=server.arg("apip3"); if (v!="") wifi_ap_ip[2]=v.toInt();
  v=server.arg("apip4"); if (v!="") wifi_ap_ip[3]=v.toInt();

  // Access-Point SubNet
  old_ip=wifi_ap_sn;
  v=server.arg("apsn1"); if (v!="") wifi_ap_sn[0]=v.toInt();
  v=server.arg("apsn2"); if (v!="") wifi_ap_sn[1]=v.toInt();
  v=server.arg("apsn3"); if (v!="") wifi_ap_sn[2]=v.toInt();
  v=server.arg("apsn4"); if (v!="") wifi_ap_sn[3]=v.toInt();

  // Access-Point Gateway
  old_ip=wifi_ap_gw;
  v=server.arg("apgw1"); if (v!="") wifi_ap_gw[0]=v.toInt();
  v=server.arg("apgw2"); if (v!="") wifi_ap_gw[1]=v.toInt();
  v=server.arg("apgw3"); if (v!="") wifi_ap_gw[2]=v.toInt();
  v=server.arg("apgw4"); if (v!="") wifi_ap_gw[3]=v.toInt();

  if (v!="") {
    nv.writeString(EE_AP_SSID,wifi_ap_ssid);
    nv.writeString(EE_AP_PWD,wifi_ap_pwd);
    nv.writeInt(EE_AP_CH,(int)wifi_ap_ch);
    nv.writeInt(EE_AP_EN,(int)accessPointEnabled);
    for (int i=0;i<4;i++) nv.write(EE_AP_IP+i,wifi_ap_ip[i]);
    for (int i=0;i<4;i++) nv.write(EE_AP_GW+i,wifi_ap_gw[i]);
    for (int i=0;i<4;i++) nv.write(EE_AP_SN+i,wifi_ap_sn[i]);
    EEwrite=true;
    restartRequired=true;
  }

  if (EEwrite) nv.commit();
}

// convert hex to int with error checking
// returns -1 on error
int hexToInt(String s) {
  int i0;
  int i1;
  if (s.length()!=2) return -1;
  char c0=s.charAt(0);
  char c1=s.charAt(1);
  if ( (((c0>='0') && (c0<='9')) || ((c0>='A') && (c0<='F'))) &&
       (((c1>='0') && (c1<='9')) || ((c1>='A') && (c1<='F'))) ) {
    if ((c0>='0') && (c0<='9')) { i0=c0-'0'; } else { i0=(c0-'A')+10; }
    if ((c1>='0') && (c1<='9')) { i1=c1-'0'; } else { i1=(c1-'A')+10; }
    return i0*16+i1;
  } else return -1;
}
