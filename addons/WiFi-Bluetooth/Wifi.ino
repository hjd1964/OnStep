// -----------------------------------------------------------------------------------
// Wifi setup

const char html_wifiSerial[] = 
"<b>Performance and compatibility:</b><br/>"
"<form method='post' action='/wifi.htm'>"
"Command channel serial read time-out: <input style='width:4em' name='ccto' value='%d' type='number' min='5' max='100'> ms<br/>"
"Web channel serial read time-out: <input style='width:4em' name='wcto' value='%d' type='number' min='5' max='100'> ms<br/>"
"<button type='submit'>Upload</button></form><br />\r\n";
const char html_wifiSSID1[] = 
"<br/><b>Station mode (connect to an Access-Point):</b><br/>"
"<form method='post' action='/wifi.htm'>"
"SSID: <input style='width:6em' name='stssid' type='text' value='%s' maxlength='32'>&nbsp;&nbsp;&nbsp;"
"Password: <input style='width:8em' name='stpwd' type='password' value='%s' maxlength='39'> (8 char min.)<br/>";
const char html_wifiSSID2[] =
"Enable DHCP: <input type='checkbox' name='stadhcp' value='1' %s> (Note: above addresses are ignored if DHCP is enabled)<br/>"
"Enable Station Mode: <input type='checkbox' name='staen' value='1' %s><br/>"
"<button type='submit'>Upload</button></form><br />\r\n";

const char html_wifiMAC[] = 
"MAC: <input style='width:10em' name='stmac' type='text' value='%s' maxlength='17' disabled><br/>";

const char html_wifiSTAIP[] =
"<table><tr><td>IP Address: </td><td>"
"<input name='staip1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='staip4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSTAGW[] =
"<tr><td>Gateway: </td><td>"
"<input name='stagw1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stagw4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSTASN[] =
"<tr><td>Subnet: </td><td>"
"<input name='stasn1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='stasn4' value='%d' type='number' min='0' max='255'></td></tr></table>";

const char html_wifiSSID3[] = 
"<br/><b>Access-Point mode:</b><br/>"
"<form method='post' action='/wifi.htm'>"
"SSID: <input style='width:6em' name='apssid' type='text' value='%s' maxlength='32'>&nbsp;&nbsp;&nbsp;"
"Password: <input style='width:8em' name='appwd' type='password' value='%s' maxlength='39'> (8 char min.)&nbsp;&nbsp;&nbsp;"
"Channel: <input style='width:2em' name='apch' value='%d' type='number' min='1' max='11'><br/>";

const char html_wifiApMAC[] = 
"MAC: <input style='width:10em' name='apmac' type='text' value='%s' maxlength='17' disabled><br/>";

const char html_wifiSSID4[] =
"<table><tr><td>IP Address: </td><td>"
"<input name='apip1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apip4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSSID5[] =
"<tr><td>Gateway: </td><td>"
"<input name='apgw1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apgw4' value='%d' type='number' min='0' max='255'></td>";
const char html_wifiSSID6[] =
"<tr><td>Subnet: </td><td>"
"<input name='apsn1' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn2' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn3' value='%d' type='number' min='0' max='255'>&nbsp;.&nbsp;"
"<input name='apsn4' value='%d' type='number' min='0' max='255'></td></tr></table>";

const char html_wifiSSID7[] =
"Enable Access-Point Mode: <input type='checkbox' name='apen' value='1' %s> (Note: auto-enabled if Station Mode fails to connect)<br/>"
"<button type='submit'>Upload</button></form><br />\r\n";

const char html_logout[] = 
"<br/><b>WiFi Configuration Security:</b><br/>"
"<form method='post' action='/wifi.htm'>"
"Password: <input style='width:8em' name='webpwd' type='password' maxlength='39'> "
"<button type='submit'>Upload</button></form>"
"<form method='post' action='/wifi.htm'>"
"<button type='submit' name='logout' value='1'>Logout</button></form><br />\r\n";

const char html_reboot[] = 
"<br/><br/><br/><br/><br/><form method='get' action='/wifi.htm'>"
"<b>You must <u>manually</u> restart for changes to take effect.</b><br/><br/>"
"<button type='submit'>Continue</button>"
"</form><br/><br/><br/><br/>"
"\r\n";

const char html_login[] = 
"<br/><form method='post' action='/wifi.htm'>"
"<br/>Enter password to change WiFi configuration:<br />"
"<input style='width:8em' name='login' type='password' maxlength='39'>"
"<button type='submit'>Ok</button>"
"</form><br/><br/><br/>"
"Setup:<br/><br/>"
"Enable either station <b>OR</b> AP mode, both enabled can cause performance issues.&nbsp;&nbsp;"
"However, if just setting up or testing it can be desirable to enable both modes temporarily to guard against being locked out.<br/>"
"If locked out of the ESP8266, a Sketch uploaded to the MCU (Teensy3.x, STM32, Mega2560, etc.) which sends an 'R' at 9600 baud on the serial interface "
"in reply to any '#' received will cause a reset to AP only enabled and the default SSID/Password.<br/><br/>"
"\r\n";

bool restartRequired=false;
bool loginRequired=true;

void handleWifi() {
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
  
  char temp[320]="";
  char temp1[80]="";
  
  processWifiGet();

  sendHtmlStart();

  // send a standard http response header
  String data=html_headB;
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  sendHtml(data);
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_cssE;
  data += html_headE;
  data += html_bodyB;
  sendHtml(data);

  mountStatus.update(true);

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
  sendHtml(data);
#ifdef ENCODERS_ON
  data += html_linksEncN;
#endif
  data += html_links4N;
  data += html_links5N;
  data += html_links6S;
  data += html_onstep_header4;
  sendHtml(data);

  data+="<div style='width: 40em;'>";

  if (restartRequired) {
    restartRequired=false;
    data+=html_reboot;
  } else
  if (loginRequired) {
    restartRequired=false;
    data+=html_login;
  } else {
    EEPROM_readString(100,wifi_sta_ssid);
    EEPROM_readString(150,wifi_sta_pwd);
      
    sprintf(temp,html_wifiSerial,CmdTimeout,WebTimeout); data += temp;
    sprintf(temp,html_wifiSSID1,wifi_sta_ssid,""); data += temp;
    
    uint8_t mac[6] = {0,0,0,0,0,0}; WiFi.macAddress(mac);
    char wifi_sta_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_sta_mac,"%s%02x:",wifi_sta_mac,mac[i]); } wifi_sta_mac[strlen(wifi_sta_mac)-1]=0;
    sprintf(temp,html_wifiMAC,wifi_sta_mac); data += temp;
  
    sprintf(temp,html_wifiSTAIP,wifi_sta_ip[0],wifi_sta_ip[1],wifi_sta_ip[2],wifi_sta_ip[3]); data += temp;
    sprintf(temp,html_wifiSTAGW,wifi_sta_gw[0],wifi_sta_gw[1],wifi_sta_gw[2],wifi_sta_gw[3]); data += temp;
    sprintf(temp,html_wifiSTASN,wifi_sta_sn[0],wifi_sta_sn[1],wifi_sta_sn[2],wifi_sta_sn[3]); data += temp;
    sprintf(temp,html_wifiSSID2,stationDhcpEnabled?"checked":"",stationEnabled?"checked":""); data += temp;
    sprintf(temp,html_wifiSSID3,wifi_ap_ssid,"",wifi_ap_ch); data += temp;
    sendHtml(data);
  
    uint8_t macap[6] = {0,0,0,0,0,0}; WiFi.softAPmacAddress(macap);
    char wifi_ap_mac[80]="";
    for (int i=0; i<6; i++) { sprintf(wifi_ap_mac,"%s%02x:",wifi_ap_mac,macap[i]); } wifi_ap_mac[strlen(wifi_ap_mac)-1]=0;
    sprintf(temp,html_wifiApMAC,wifi_ap_mac); data += temp;
    
    sprintf(temp,html_wifiSSID4,wifi_ap_ip[0],wifi_ap_ip[1],wifi_ap_ip[2],wifi_ap_ip[3]); data += temp;
    sprintf(temp,html_wifiSSID5,wifi_ap_gw[0],wifi_ap_gw[1],wifi_ap_gw[2],wifi_ap_gw[3]); data += temp;
    sprintf(temp,html_wifiSSID6,wifi_ap_sn[0],wifi_ap_sn[1],wifi_ap_sn[2],wifi_ap_sn[3]); data += temp;
    sprintf(temp,html_wifiSSID7,accessPointEnabled?"checked":""); data += temp;
    data += html_logout;
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
    EEPROM_writeString(200,masterPassword);
    EEwrite=true;
  }

  // Timeouts -----------------------------------------------------------------
  // Cmd channel timeout
  v=server.arg("ccto");
  if (v!="") {
    CmdTimeout=v.toInt();
    EEPROM_writeInt(12,(int)CmdTimeout);
    EEwrite=true;
  }

  // Web channel timeout
  v=server.arg("wcto");
  if (v!="") {
    WebTimeout=v.toInt();
    EEPROM_writeInt(10,(int)WebTimeout);
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
    EEPROM_writeString(100,wifi_sta_ssid);
    EEPROM_writeString(150,wifi_sta_pwd);
    EEPROM_writeInt(8,(int)stationDhcpEnabled);
    EEPROM_writeInt(6,(int)stationEnabled);
    EEPROM.write(20,wifi_sta_ip[0]); EEPROM.write(21,wifi_sta_ip[1]); EEPROM.write(22,wifi_sta_ip[2]); EEPROM.write(23,wifi_sta_ip[3]);
    EEPROM.write(24,wifi_sta_gw[0]); EEPROM.write(25,wifi_sta_gw[1]); EEPROM.write(26,wifi_sta_gw[2]); EEPROM.write(27,wifi_sta_gw[3]);
    EEPROM.write(28,wifi_sta_sn[0]); EEPROM.write(29,wifi_sta_sn[1]); EEPROM.write(30,wifi_sta_sn[2]); EEPROM.write(31,wifi_sta_sn[3]);
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
    EEPROM_writeString(500,wifi_ap_ssid);
    EEPROM_writeString(550,wifi_ap_pwd);
    EEPROM_writeInt(50,(int)wifi_ap_ch);
    EEPROM_writeInt(4,(int)accessPointEnabled);
    EEPROM.write(60,wifi_ap_ip[0]); EEPROM.write(61,wifi_ap_ip[1]); EEPROM.write(62,wifi_ap_ip[2]); EEPROM.write(63,wifi_ap_ip[3]);
    EEPROM.write(70,wifi_ap_gw[0]); EEPROM.write(71,wifi_ap_gw[1]); EEPROM.write(72,wifi_ap_gw[2]); EEPROM.write(73,wifi_ap_gw[3]);
    EEPROM.write(80,wifi_ap_sn[0]); EEPROM.write(81,wifi_ap_sn[1]); EEPROM.write(82,wifi_ap_sn[2]); EEPROM.write(83,wifi_ap_sn[3]);
    EEwrite=true;
    restartRequired=true;
  }

  if (EEwrite) EEPROM.commit();
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
