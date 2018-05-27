#include "WifiBluetooth.h"
#include "config.h"


const char* html_headB = "<!DOCTYPE HTML>\r\n<html>\r\n<head>\r\n";
const char* html_headerPec = "<meta http-equiv=\"refresh\" content=\"5; URL=/pec.htm\">\r\n";
const char* html_headerIdx = "<meta http-equiv=\"refresh\" content=\"5; URL=/index.htm\">\r\n";
const char* html_headE = "</head>\r\n";
const char* html_bodyB = "<body bgcolor='#26262A'>\r\n";

const char* html_main_cssB = "<STYLE>";
const char* html_main_css1 = ".clear { clear: both; } .a { background-color: #111111; } .t { padding: 10px 10px 20px 10px; border: 5px solid #551111;";
const char* html_main_css2 = " margin: 25px 25px 0px 25px; color: #999999; background-color: #111111; min-width: 30em; } input { font-weight: bold; width:4em; background-color: #A01010; padding: 2px 2px; }";
const char* html_main_css3 = ".b { padding: 10px; border-left: 5px solid #551111; border-right: 5px solid #551111; border-bottom: 5px solid #551111; margin: 0px 25px 25px 25px; color: #999999;";
const char* html_main_css4 = "background-color: #111111; min-width: 30em; } select { width:4em; font-weight: bold; background-color: #A01010; padding: 2px 2px; } .c { color: #A01010; font-weight: bold; }";
const char* html_main_css5 = "h1 { text-align: right; } a:hover, a:active { background-color: red; } .y { color: #FFFF00; font-weight: bold; }";
const char* html_main_css6 = "a:link, a:visited { background-color: #332222; color: #a07070; border:1px solid red; padding: 5px 10px;";
const char* html_main_css7 = " margin: none; text-align: center; text-decoration: none; display: inline-block; }";
const char* html_main_css8 = "button { background-color: #A01010; font-weight: bold; border-radius: 5px; margin: 2px; padding: 4px 8px; }";
const char* html_main_css_control1 = ".b1 { float: left; border: 2px solid #551111; background-color: #181818; text-align: center; margin: 5px; padding: 15px; padding-top: 3px; }";
const char* html_main_css_control2 = ".gb {  font-weight: bold; font-size: 150%; font-family: 'Times New Roman', Times, serif; width: 60px; height: 50px; padding: 0px; }";
const char* html_main_css_control3 = ".bb {  font-weight: bold; font-size: 105%; } .bbh {  font-weight: bold; font-size: 100%; height: 2.1em; }";
const char* html_main_cssE = "</STYLE>";

const char* html_onstep_header1 = "<div class='t'><table width='100%%'><tr><td><b><font size='5'>";
const char* html_onstep_header2 = "</font></b></td><td align='right'><b>" Product " " Version " (OnStep ";
const char* html_onstep_header3 = ")</b></td></tr></table>";
const char* html_onstep_header4 = "</div><div class='b'>\r\n";

const char* html_links1S = "<a href='/index.htm' style='background-color: #552222;'>Status</a>";
const char* html_links1N = "<a href='/index.htm'>Status</a>";
const char* html_links2S = "<a href='/control.htm' style='background-color: #552222;'>Control</a>";
const char* html_links2N = "<a href='/control.htm'>Control</a>";
const char* html_links3S = "<a href='/pec.htm' style='background-color: #552222;'>PEC</a>";
const char* html_links3N = "<a href='/pec.htm'>PEC</a>";
const char* html_links4S = "<a href='/settings.htm' style='background-color: #552222;'>Settings</a>";
const char* html_links4N = "<a href='/settings.htm'>Settings</a>";
const char* html_links5S = "<a href='/configuration.htm' style='background-color: #552222;'>Configuration</a>";
const char* html_links5N = "<a href='/configuration.htm'>Configuration</a>";
const char* html_links6S = "<a href='/wifi.htm' style='background-color: #552222;'>WiFi</a><br />";
const char* html_links6N = "<a href='/wifi.htm'>WiFi</a><br />";

// Javascript for Ajax
// be sure to define "var ajaxPage='control.txt';" etc.
const char* html_ajax_active =
"<script>\n"
"var auto1Tick=0;\n"
"var auto2Tick=0;\n"
"var auto2Rate=" DEFAULT_AJAX_RATE ";\n"
"var auto1=setInterval(autoRun1s,1000);\n"
// update once a second for 15 seconds then drop back to once every 5 seconds
"function autoFastRun() {\n"
"auto2Rate=" DEFAULT_FAST_AJAX_RATE "\n"
"auto2Tick=" DEFAULT_AJAX_SHED_TIME ";\n"
"}\n"
"function autoRun1s() {\n"
"auto1Tick++;\n"
"var i;\n"
"if (auto2Tick>=0) auto2Tick--;\n"
"if (auto2Tick==0) auto2Rate=" DEFAULT_AJAX_RATE ";\n"
"if (auto1Tick%auto2Rate==0) {\n"
"nocache='?nocache='+Math.random()*1000000;\n"
"var request = new XMLHttpRequest();\n"
"request.onreadystatechange = pageReady(ajaxPage);\n"
"request.open('GET',ajaxPage.toLowerCase()+nocache,true); request.send(null);\n"
"}"
"}\n"
"function pageReady(aPage) {\n"
"return function() {\n"
"if ((this.readyState==4)&&(this.status==200)) {\n"
"lines=this.responseText.split('\\n');\n"
"for (var i=0; i<lines.length; i++) {\n"
"v=lines[i].slice(lines[i].indexOf('|')+1);\n"
"k=lines[i].slice(0,lines[i].indexOf('|'));\n"
"if (k!='') document.getElementById(k).innerHTML=v;\n"
"}\n"
"}"
"}"
"}\n"
"</script>\n";


int wifibluetooth::WebTimeout = TIMEOUT_WEB;
int wifibluetooth::CmdTimeout = TIMEOUT_CMD;

char wifibluetooth::masterPassword[40] = Default_Password;

bool wifibluetooth::accessPointEnabled = true;
bool wifibluetooth::stationEnabled = false;
bool wifibluetooth::stationDhcpEnabled = true;

char wifibluetooth::wifi_sta_ssid[40] = "";
char wifibluetooth::wifi_sta_pwd[40] = "";

IPAddress wifibluetooth::wifi_sta_ip = IPAddress(192, 168, 0, 1);
IPAddress wifibluetooth::wifi_sta_gw = IPAddress(192, 168, 0, 1);
IPAddress wifibluetooth::wifi_sta_sn = IPAddress(255, 255, 255, 0);

char wifibluetooth::wifi_ap_ssid[40] = "ONSTEP";
char wifibluetooth::wifi_ap_pwd[40] = "password";
byte wifibluetooth::wifi_ap_ch = 7;

IPAddress wifibluetooth::wifi_ap_ip = IPAddress(192, 168, 0, 1);
IPAddress wifibluetooth::wifi_ap_gw = IPAddress(192, 168, 0, 1);
IPAddress wifibluetooth::wifi_ap_sn = IPAddress(255, 255, 255, 0);

MountStatus wifibluetooth::mountStatus;
ESP8266WebServer wifibluetooth::server;
WiFiServer* wifibluetooth::cmdSvr = NULL;
WiFiClient wifibluetooth::cmdSvrClient;

// -----------------------------------------------------------------------------------
// EEPROM related functions 

// write int numbers into EEPROM at position i (2 bytes)
void wifibluetooth::EEPROM_writeInt(int i, int j) {
  uint8_t *k = (uint8_t*)&j;
  EEPROM.write(i + 0, *k); k++;
  EEPROM.write(i + 1, *k);
}

// read int numbers from EEPROM at position i (2 bytes)
int wifibluetooth::EEPROM_readInt(int i) {
  uint16_t j;
  uint8_t *k = (uint8_t*)&j;
  *k = EEPROM.read(i + 0); k++;
  *k = EEPROM.read(i + 1);
  return j;
}

// write 4 byte variable into EEPROM at position i (4 bytes)
void wifibluetooth::EEPROM_writeQuad(int i, byte *v) {
  EEPROM.write(i + 0, *v); v++;
  EEPROM.write(i + 1, *v); v++;
  EEPROM.write(i + 2, *v); v++;
  EEPROM.write(i + 3, *v);
}

// read 4 byte variable from EEPROM at position i (4 bytes)
void wifibluetooth::EEPROM_readQuad(int i, byte *v) {
  *v = EEPROM.read(i + 0); v++;
  *v = EEPROM.read(i + 1); v++;
  *v = EEPROM.read(i + 2); v++;
  *v = EEPROM.read(i + 3);
}

// write String into EEPROM at position i (40 bytes)
void wifibluetooth::EEPROM_writeString(int i, char l[]) {
  for (int l1 = 0; l1<40; l1++) {
    EEPROM.write(i + l1, *l); l++;
  }
}

// read String from EEPROM at position i (40 bytes)
void wifibluetooth::EEPROM_readString(int i, char l[]) {
  for (int l1 = 0; l1<40; l1++) {
    *l = EEPROM.read(i + l1); l++;
  }
}

// write 4 byte float into EEPROM at position i (4 bytes)
void wifibluetooth::EEPROM_writeFloat(int i, float f) {
  EEPROM_writeQuad(i, (byte*)&f);
}

// read 4 byte float from EEPROM at position i (4 bytes)
float wifibluetooth::EEPROM_readFloat(int i) {
  float f;
  EEPROM_readQuad(i, (byte*)&f);
  return f;
}

// write 4 byte long into EEPROM at position i (4 bytes)
void wifibluetooth::EEPROM_writeLong(int i, long l) {
  EEPROM_writeQuad(i, (byte*)&l);
}

// read 4 byte long from EEPROM at position i (4 bytes)
long wifibluetooth::EEPROM_readLong(int i) {
  long l;
  EEPROM_readQuad(i, (byte*)&l);
  return l;
}


const char* wifibluetooth::HighSpeedCommsStr(long baud)
{
  if (baud == 115200) { return ":SB0#"; }
  if (baud == 57600) { return ":SB1#"; }
  if (baud == 38400) { return ":SB2#"; }
  if (baud == 28800) { return ":SB3#"; }
  if (baud == 19200) { return ":SB4#"; }
  else { return ":SB5#"; }
}

void wifibluetooth::handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void wifibluetooth::setup()
{
  cmdSvr = new WiFiServer(9999);


#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif
  EEPROM.begin(1024);

  // EEPROM Init
  if ((EEPROM_readInt(0) != 8265) || (EEPROM_readInt(2) != 0)) {
    EEPROM_writeInt(0, 8266);
    EEPROM_writeInt(2, 0);

    EEPROM_writeInt(4, (int)accessPointEnabled);
    EEPROM_writeInt(6, (int)stationEnabled);
    EEPROM_writeInt(8, (int)stationDhcpEnabled);

    EEPROM_writeInt(10, (int)WebTimeout);
    EEPROM_writeInt(12, (int)CmdTimeout);

    EEPROM_writeString(100, wifi_sta_ssid);
    EEPROM_writeString(150, wifi_sta_pwd);
    EEPROM_writeString(200, masterPassword);
    EEPROM.write(20, wifi_sta_ip[0]); EEPROM.write(21, wifi_sta_ip[1]); EEPROM.write(22, wifi_sta_ip[2]); EEPROM.write(23, wifi_sta_ip[3]);
    EEPROM.write(24, wifi_sta_gw[0]); EEPROM.write(25, wifi_sta_gw[1]); EEPROM.write(26, wifi_sta_gw[2]); EEPROM.write(27, wifi_sta_gw[3]);
    EEPROM.write(28, wifi_sta_sn[0]); EEPROM.write(29, wifi_sta_sn[1]); EEPROM.write(30, wifi_sta_sn[2]); EEPROM.write(31, wifi_sta_sn[3]);

    EEPROM_writeString(500, wifi_ap_ssid);
    EEPROM_writeString(550, wifi_ap_pwd);
    EEPROM_writeInt(50, (int)wifi_ap_ch);
    EEPROM.write(60, wifi_ap_ip[0]); EEPROM.write(61, wifi_ap_ip[1]); EEPROM.write(62, wifi_ap_ip[2]); EEPROM.write(63, wifi_ap_ip[3]);
    EEPROM.write(70, wifi_ap_gw[0]); EEPROM.write(71, wifi_ap_gw[1]); EEPROM.write(72, wifi_ap_gw[2]); EEPROM.write(73, wifi_ap_gw[3]);
    EEPROM.write(80, wifi_ap_sn[0]); EEPROM.write(81, wifi_ap_sn[1]); EEPROM.write(82, wifi_ap_sn[2]); EEPROM.write(83, wifi_ap_sn[3]);
    EEPROM.commit();
  }
  else {
    accessPointEnabled = EEPROM_readInt(4);
    stationEnabled = EEPROM_readInt(6);
    stationDhcpEnabled = EEPROM_readInt(8);

    WebTimeout = EEPROM_readInt(10);
    CmdTimeout = EEPROM_readInt(12);

    EEPROM_readString(100, wifi_sta_ssid);
    EEPROM_readString(150, wifi_sta_pwd);
    EEPROM_readString(200, masterPassword);
    wifi_sta_ip[0] = EEPROM.read(20); wifi_sta_ip[1] = EEPROM.read(21); wifi_sta_ip[2] = EEPROM.read(22); wifi_sta_ip[3] = EEPROM.read(23);
    wifi_sta_gw[0] = EEPROM.read(24); wifi_sta_gw[1] = EEPROM.read(25); wifi_sta_gw[2] = EEPROM.read(26); wifi_sta_gw[3] = EEPROM.read(27);
    wifi_sta_sn[0] = EEPROM.read(28); wifi_sta_sn[1] = EEPROM.read(29); wifi_sta_sn[2] = EEPROM.read(30); wifi_sta_sn[3] = EEPROM.read(31);

    EEPROM_readString(500, wifi_ap_ssid);
    EEPROM_readString(550, wifi_ap_pwd);
    wifi_ap_ch = EEPROM_readInt(50);
    wifi_ap_ip[0] = EEPROM.read(60); wifi_ap_ip[1] = EEPROM.read(61); wifi_ap_ip[2] = EEPROM.read(62); wifi_ap_ip[3] = EEPROM.read(63);
    wifi_ap_gw[0] = EEPROM.read(70); wifi_ap_gw[1] = EEPROM.read(71); wifi_ap_gw[2] = EEPROM.read(72); wifi_ap_gw[3] = EEPROM.read(73);
    wifi_ap_sn[0] = EEPROM.read(80); wifi_ap_sn[1] = EEPROM.read(81); wifi_ap_sn[2] = EEPROM.read(82); wifi_ap_sn[3] = EEPROM.read(83);
  }

#ifndef DEBUG_ON
  Ser.begin(SERIAL_BAUD_DEFAULT);
#ifdef SERIAL_SWAP_ON
  Ser.swap();
#endif

  byte tb = 0;
Again:
#ifdef LED_PIN
  digitalWrite(LED_PIN, LOW);
#endif
  char c = 0;

  // clear the buffers and any noise on the serial lines
  for (int i = 0; i < 3; i++) {
    Ser.print(":#");
#ifdef LED_PIN
    digitalWrite(LED_PIN, HIGH);
#endif
    delay(500);
    Ser.flush();
    c = serialRecvFlush();
#ifdef LED_PIN
    digitalWrite(LED_PIN, LOW);
#endif
    delay(500);
  }

  // safety net
  if ((c == 'R') || (!accessPointEnabled && !stationEnabled)) {
    // reset EEPROM values, triggers an init
    EEPROM_writeInt(0, 0); EEPROM_writeInt(2, 0);
    accessPointEnabled = true;
    EEPROM.commit();
    Ser.println();
    Ser.println("Cycle power for reset to defaults.");
    Ser.println();
  }

  if (SERIAL_BAUD != SERIAL_BAUD_DEFAULT) {

    // switch OnStep Serial1 up to ? baud
    Ser.print(HighSpeedCommsStr(SERIAL_BAUD));
    delay(100);
    int count = 0; c = 0;
    while (Ser.available() > 0) { count++; if (count == 1) c = Ser.read(); }
    if (c == '1') {
      Ser.begin(SERIAL_BAUD);
#ifdef SERIAL_SWAP_ON
      Ser.swap();
#endif
    }
    else {
#ifdef LED_PIN
      digitalWrite(LED_PIN, HIGH);
#endif
      // got nothing back, toggle baud rate and try again
      tb++;
      if (tb == 7) tb = 1;
      if (tb == 1) Ser.begin(SERIAL_BAUD_DEFAULT);
      if (tb == 4) Ser.begin(SERIAL_BAUD);

#ifdef SERIAL_SWAP_ON
      Ser.swap();
#endif
      delay(1000);
      goto Again;
    }
  }
#else
  Ser.begin(115200);
  delay(10000);

  Ser.println(accessPointEnabled);
  Ser.println(stationEnabled);
  Ser.println(stationDhcpEnabled);

  Ser.println(WebTimeout);
  Ser.println(CmdTimeout);

  Ser.println(wifi_sta_ssid);
  Ser.println(wifi_sta_pwd);
  Ser.println(wifi_sta_ip.toString());
  Ser.println(wifi_sta_gw.toString());
  Ser.println(wifi_sta_sn.toString());

  Ser.println(wifi_ap_ssid);
  Ser.println(wifi_ap_pwd);
  Ser.println(wifi_ap_ch);
  Ser.println(wifi_ap_ip.toString());
  Ser.println(wifi_ap_gw.toString());
  Ser.println(wifi_ap_sn.toString());

#endif

  if ((stationEnabled) && (!stationDhcpEnabled)) WiFi.config(wifi_sta_ip, wifi_sta_gw, wifi_sta_sn);
  if (accessPointEnabled) WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gw, wifi_ap_sn);

  if (accessPointEnabled && !stationEnabled) {
    WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
    WiFi.mode(WIFI_AP);
  }
  else
    if (!accessPointEnabled && stationEnabled) {
      WiFi.softAPdisconnect(true);
      WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
      WiFi.mode(WIFI_STA);
    }
    else
      if (accessPointEnabled && stationEnabled) {
        WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
        WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
        WiFi.mode(WIFI_AP_STA);
      }

  // clear the buffers and any noise on the serial lines
  for (int i = 0; i < 3; i++) {
    Ser.print(":#");
    delay(50);
    serialRecvFlush();
  }

  // Wait for connection
  if (stationEnabled) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }
  server.on("/", handleRoot);
  server.on("/index.htm", handleRoot);
  server.on("/configuration.htm", handleConfiguration);
  server.on("/settings.htm", handleSettings);
  server.on("/settings.txt", settingsAjax);
  server.on("/control.htm", handleControl);
  server.on("/control.txt", controlAjax);
  server.on("/guide.txt", guideAjax);
  server.on("/pec.htm", handlePec);
  server.on("/pec.txt", pecAjax);
  server.on("/wifi.htm", handleWifi);

  server.onNotFound(handleNotFound);

  cmdSvr->begin();
  cmdSvr->setNoDelay(true);
  server.begin();

#ifdef DEBUG_ON
  Ser.println("HTTP server started");
#endif

  //encoders.init();
};

void wifibluetooth::update()
{
  server.handleClient();

  // disconnect client
  static unsigned long clientTime = 0;
  if (cmdSvrClient && (!cmdSvrClient.connected())) cmdSvrClient.stop();
  if (cmdSvrClient && ((long)(clientTime - millis()) < 0)) cmdSvrClient.stop();

  // new client
  if (!cmdSvrClient && (cmdSvr->hasClient())) {
    // find free/disconnected spot
    cmdSvrClient = cmdSvr->available();
    clientTime = millis() + 2000UL;
  }

  static char writeBuffer[40] = "";
  static int writeBufferPos = 0;
  // check clients for data, if found get the command, send cmd and pickup the response, then return the response
  while (cmdSvrClient && cmdSvrClient.connected() && (cmdSvrClient.available() > 0)) {
    // get the data
    byte b = cmdSvrClient.read();
    writeBuffer[writeBufferPos] = b; writeBufferPos++; if (writeBufferPos > 39) writeBufferPos = 39; writeBuffer[writeBufferPos] = 0;

    // send cmd and pickup the response
    if ((b == '#') || ((strlen(writeBuffer) == 1) && (b == (char)6))) {
      char readBuffer[40] = "";
      readLX200Bytes(writeBuffer, readBuffer, CmdTimeout); writeBuffer[0] = 0; writeBufferPos = 0;

      // return the response, if we have one
      if (strlen(readBuffer) > 0) {
        if (cmdSvrClient && cmdSvrClient.connected()) {
          cmdSvrClient.print(readBuffer);
          delay(2);
        }
      }

    }
    else server.handleClient();
  }

  // encoders.poll();

}

