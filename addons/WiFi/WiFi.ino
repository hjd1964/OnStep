/*
 * Title       OnStep WiFi Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2020 Howard Dutton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 *
 * Revision History, see GitHub
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * ESP8266 OnStep control
 *
 */

#define Product "WiFi Server"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "13"
#define FirmwareVersionPatch  "f"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

#include <limits.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
  #include <WiFiAP.h>
#else
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266WiFiAP.h>
#endif
#include <EEPROM.h>

#define DEBUG_OFF   // Turn _ON to allow web and cmd channel servers to startup without OnStep (Serial port for debug at 115200 baud)

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Globals.h"

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif
bool serialSwap=false;

int webTimeout=TIMEOUT_WEB;
int cmdTimeout=TIMEOUT_CMD;

#define AXIS1_ENC_A_PIN 14 // pin# for Axis1 encoder, for A or CW
#define AXIS1_ENC_B_PIN 12 // pin# for Axis1 encoder, for B or CCW
#define AXIS2_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
#define AXIS2_ENC_B_PIN 4  // pin# for Axis1 encoder, for B or CCW
#include "Encoders.h"
#if ENCODERS == ON
Encoders encoders;
#endif

#ifndef LEGACY_TRANSMIT_ON
  // macros to help with sending webpage data, chunked
  #define sendHtmlStart() server.setContentLength(CONTENT_LENGTH_UNKNOWN); server.sendHeader("Cache-Control","no-cache"); server.send(200, "text/html", String());
  #define sendHtml(x) server.sendContent(x); x=""
  #define sendHtmlDone(x) server.sendContent("");
#else
  // macros to help with sending webpage data, normal method
  #define sendHtmlStart()
  #define sendHtml(x)
  #define sendHtmlDone(x) server.send(200, "text/html", x)
#endif

bool errorMonitorOn=false;
struct errors {
  char cmd[40];
  byte err;
};
struct errors cmdErrorList[10] = { {"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0} };

#define Default_Password "password"
char masterPassword[40]=Default_Password;

bool accessPointEnabled=true;
bool stationEnabled=false;
bool stationDhcpEnabled=true;

char wifi_sta_ssid[40]="";
char wifi_sta_pwd[40]="";

IPAddress wifi_sta_ip = IPAddress(192,168,0,1);
IPAddress wifi_sta_gw = IPAddress(192,168,0,1);
IPAddress wifi_sta_sn = IPAddress(255,255,255,0);

char wifi_ap_ssid[40]="ONSTEP";
char wifi_ap_pwd[40]="password";
byte wifi_ap_ch=7;

IPAddress wifi_ap_ip = IPAddress(192,168,0,1);
IPAddress wifi_ap_gw = IPAddress(192,168,0,1);
IPAddress wifi_ap_sn = IPAddress(255,255,255,0);

#ifdef ESP32
  WebServer server(80);
#else
  ESP8266WebServer server(80);
#endif

#if STANDARD_COMMAND_CHANNEL == ON
  WiFiServer cmdSvr(9999);
  WiFiClient cmdSvrClient;
#endif

#if PERSISTENT_COMMAND_CHANNEL == ON
  WiFiServer persistentCmdSvr(9998);
  WiFiClient persistentCmdSvrClient;
#endif

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

#include "Accessories.h"
#include "MountStatus.h"

void setup(void){
#if LED_STATUS != OFF
  pinMode(LED_STATUS,OUTPUT);
#endif
  EEPROM.begin(1024);

  // EEPROM Init
  if ((EEPROM_readInt(0)!=8266) || (EEPROM_readInt(2)!=0)) {
    EEPROM_writeInt(0,8266);
    EEPROM_writeInt(2,0);

    EEPROM_writeInt(4,(int)accessPointEnabled);
    EEPROM_writeInt(6,(int)stationEnabled);
    EEPROM_writeInt(8,(int)stationDhcpEnabled);

    EEPROM_writeInt(10,(int)webTimeout);
    EEPROM_writeInt(12,(int)cmdTimeout);

#if ENCODERS == ON
    EEPROM_writeLong(600,Axis1EncDiffLimit);
    EEPROM_writeLong(604,Axis2EncDiffLimit);
    EEPROM_writeLong(608,20);  // enc short term average samples
    EEPROM_writeLong(612,200); // enc long term average samples
    EEPROM_writeLong(616,0);   // enc rate comp
    EEPROM_writeLong(624,1);   // intpol phase
    EEPROM_writeLong(628,0);   // intpol mag
    EEPROM_writeLong(632,10);  // prop
#endif

    EEPROM_writeString(100,wifi_sta_ssid);
    EEPROM_writeString(150,wifi_sta_pwd);
    EEPROM_writeString(200,masterPassword);
    EEPROM.write(20,wifi_sta_ip[0]); EEPROM.write(21,wifi_sta_ip[1]); EEPROM.write(22,wifi_sta_ip[2]); EEPROM.write(23,wifi_sta_ip[3]);
    EEPROM.write(24,wifi_sta_gw[0]); EEPROM.write(25,wifi_sta_gw[1]); EEPROM.write(26,wifi_sta_gw[2]); EEPROM.write(27,wifi_sta_gw[3]);
    EEPROM.write(28,wifi_sta_sn[0]); EEPROM.write(29,wifi_sta_sn[1]); EEPROM.write(30,wifi_sta_sn[2]); EEPROM.write(31,wifi_sta_sn[3]);

    EEPROM_writeString(500,wifi_ap_ssid);
    EEPROM_writeString(550,wifi_ap_pwd);
    EEPROM_writeInt(50,(int)wifi_ap_ch);
    EEPROM.write(60,wifi_ap_ip[0]); EEPROM.write(61,wifi_ap_ip[1]); EEPROM.write(62,wifi_ap_ip[2]); EEPROM.write(63,wifi_ap_ip[3]);
    EEPROM.write(70,wifi_ap_gw[0]); EEPROM.write(71,wifi_ap_gw[1]); EEPROM.write(72,wifi_ap_gw[2]); EEPROM.write(73,wifi_ap_gw[3]);
    EEPROM.write(80,wifi_ap_sn[0]); EEPROM.write(81,wifi_ap_sn[1]); EEPROM.write(82,wifi_ap_sn[2]); EEPROM.write(83,wifi_ap_sn[3]);
    EEPROM.commit();
  } else {  
    accessPointEnabled=EEPROM_readInt(4);
    stationEnabled=EEPROM_readInt(6);
    stationDhcpEnabled=EEPROM_readInt(8);

    webTimeout=EEPROM_readInt(10);
    cmdTimeout=EEPROM_readInt(12);
    if (cmdTimeout > 100) cmdTimeout=30;

#if ENCODERS == ON
    Axis1EncDiffLimit=EEPROM_readLong(600);
    Axis2EncDiffLimit=EEPROM_readLong(604);
#if AXIS1_ENC_RATE_CONTROL == ON
    Axis1EncStaSamples=EEPROM_readLong(608);
    Axis1EncLtaSamples=EEPROM_readLong(612);
    long l=EEPROM_readLong(616); axis1EncRateComp=(float)l/1000000.0;
#if AXIS1_ENC_INTPOL_COS == ON
    Axis1EncIntPolPhase =EEPROM_readLong(624);
    Axis1EncIntPolMag   =EEPROM_readLong(628);
#endif
    Axis1EncProp        =EEPROM_readLong(632);
    Axis1EncMinGuide    =EEPROM_readLong(636);
#endif
#endif

    EEPROM_readString(100,wifi_sta_ssid);
    EEPROM_readString(150,wifi_sta_pwd);
    EEPROM_readString(200,masterPassword);
    wifi_sta_ip[0]=EEPROM.read(20); wifi_sta_ip[1]=EEPROM.read(21); wifi_sta_ip[2]=EEPROM.read(22); wifi_sta_ip[3]=EEPROM.read(23);
    wifi_sta_gw[0]=EEPROM.read(24); wifi_sta_gw[1]=EEPROM.read(25); wifi_sta_gw[2]=EEPROM.read(26); wifi_sta_gw[3]=EEPROM.read(27);
    wifi_sta_sn[0]=EEPROM.read(28); wifi_sta_sn[1]=EEPROM.read(29); wifi_sta_sn[2]=EEPROM.read(30); wifi_sta_sn[3]=EEPROM.read(31);

    EEPROM_readString(500,wifi_ap_ssid);
    EEPROM_readString(550,wifi_ap_pwd);
    wifi_ap_ch=EEPROM_readInt(50);
    wifi_ap_ip[0]=EEPROM.read(60); wifi_ap_ip[1]=EEPROM.read(61); wifi_ap_ip[2]=EEPROM.read(62); wifi_ap_ip[3]=EEPROM.read(63);
    wifi_ap_gw[0]=EEPROM.read(70); wifi_ap_gw[1]=EEPROM.read(71); wifi_ap_gw[2]=EEPROM.read(72); wifi_ap_gw[3]=EEPROM.read(73);
    wifi_ap_sn[0]=EEPROM.read(80); wifi_ap_sn[1]=EEPROM.read(81); wifi_ap_sn[2]=EEPROM.read(82); wifi_ap_sn[3]=EEPROM.read(83);    
  }

#ifndef DEBUG_ON
  long serial_baud = SERIAL_BAUD;
  Ser.begin(SERIAL_BAUD_DEFAULT); if (serialSwap) swapSerial(); delay(2000);
  byte tb=1;

Again:
#if LED_STATUS != OFF
  digitalWrite(LED_STATUS,LOW);
#endif

  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
#if LED_STATUS != OFF
    digitalWrite(LED_STATUS,HIGH);
#endif
    delay(300);
    Ser.flush(); serialRecvFlush();
#if LED_STATUS != OFF
    digitalWrite(LED_STATUS,LOW);
#endif
    delay(300);
  }

  // look for On-Step
  Ser.print(":GVP#"); delay(100);
  // make sure response is good
  if (Ser.available() == 8 && 
      Ser.read() == 'O' && Ser.read() == 'n' && Ser.read() == '-' && Ser.read() == 'S' &&
      Ser.read() == 't' && Ser.read() == 'e' && Ser.read() == 'p' && Ser.read() == '#') {

    // check fastest baud rate
    Ser.print(":GB#"); delay(100);
    if (Ser.available() != 1) { serialRecvFlush(); goto Again; }
    if (Ser.read() == '4' && serial_baud > 19200) serial_baud = 19200; // Mega2560 returns '4' for 19200 baud recommended

    // set fastest baud rate
    Ser.print(HighSpeedCommsStr(serial_baud)); delay(100);
    if (Ser.available() != 1) { serialRecvFlush(); goto Again; }
    if (Ser.read() != '1') goto Again;
    
    // we're all set, just change the baud rate to match OnStep
    Ser.begin(serial_baud); if (serialSwap) swapSerial(); delay(2000);
  } else {
#if LED_STATUS != OFF
    digitalWrite(LED_STATUS,HIGH);
#endif
    // got nothing back, toggle baud rate and/or swap ports
    serialRecvFlush();
    tb++;
    if (tb == 16) { tb=1; serialSwap=!serialSwap; }
    if (tb == 1) { Ser.begin(SERIAL_BAUD_DEFAULT); if (serialSwap) swapSerial(); delay(2000); }
    if (tb == 6) { Ser.begin(serial_baud); if (serialSwap) swapSerial(); delay(2000); }
    if (tb == 11) { Ser.begin(19200); if (serialSwap) swapSerial(); delay(2000); }
    goto Again;
  }
#else
  Ser.begin(115200);
  delay(10000);
  
  Ser.println(accessPointEnabled);
  Ser.println(stationEnabled);
  Ser.println(stationDhcpEnabled);

  Ser.println(webTimeout);
  Ser.println(cmdTimeout);

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

TryAgain:
  if ((stationEnabled) && (!stationDhcpEnabled)) WiFi.config(wifi_sta_ip, wifi_sta_gw, wifi_sta_sn);
  if (accessPointEnabled) WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gw, wifi_ap_sn);
  
  if (accessPointEnabled && !stationEnabled) {
    WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
    WiFi.mode(WIFI_AP);
  } else
  if (!accessPointEnabled && stationEnabled) {
    WiFi.softAPdisconnect(true);
    WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
    WiFi.mode(WIFI_STA);
  } else
  if (accessPointEnabled && stationEnabled) {
    WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
    WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
    WiFi.mode(WIFI_AP_STA);
  }

  // wait for connection in station mode, if it fails fall back to access-point mode
  if (!accessPointEnabled && stationEnabled) {
    for (int i=0; i<5; i++)
      if (WiFi.status() != WL_CONNECTED) delay(1000); else break;
    if (WiFi.status() != WL_CONNECTED) {
      stationEnabled=false;
      accessPointEnabled=true;
      goto TryAgain;
    }
  }

  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
    delay(50);
    serialRecvFlush();
  }

  server.on("/", handleRoot);
  server.on("/index.htm", handleRoot);
  server.on("/configuration.htm", handleConfiguration);
  server.on("/settings.htm", handleSettings);
  server.on("/settings.txt", settingsAjax);
#if ENCODERS == ON
  server.on("/enc.htm", handleEncoders);
  server.on("/encA.txt", encAjaxGet);
  server.on("/enc.txt", encAjax);
#endif
  server.on("/library.htm", handleLibrary);
  server.on("/libraryA.txt", libraryAjaxGet);
  server.on("/library.txt", libraryAjax);
  server.on("/control.htm", handleControl);
  server.on("/controlA.txt", controlAjaxGet);
  server.on("/control.txt", controlAjax);
  server.on("/auxiliary.htm", handleAux);
  server.on("/auxiliaryA.txt", auxAjaxGet);
  server.on("/auxiliary.txt", auxAjax);
  server.on("/pec.htm", handlePec);
  server.on("/pec.txt", pecAjax);
  server.on("/wifi.htm", handleWifi);
  
  server.onNotFound(handleNotFound);

#if STANDARD_COMMAND_CHANNEL == ON
  cmdSvr.begin();
  cmdSvr.setNoDelay(true);
#endif

#if PERSISTENT_COMMAND_CHANNEL == ON
  persistentCmdSvr.begin();
  persistentCmdSvr.setNoDelay(true);
#endif

  server.begin();

#ifdef DEBUG_ON
  Ser.println("HTTP server started");
#endif

#if ENCODERS == ON
  encoders.init();
#endif
}

void loop(void) {
  server.handleClient();
#if ENCODERS == ON
  encoders.poll();
#endif

#if STANDARD_COMMAND_CHANNEL == ON
  // -------------------------------------------------------------------------------------------------------------------------------
  // Standard IP connections on port 9999

  // disconnect client
  static unsigned long clientTime = 0;
  if (cmdSvrClient && (!cmdSvrClient.connected())) cmdSvrClient.stop();
  if (cmdSvrClient && ((long)(clientTime-millis())<0)) cmdSvrClient.stop();

  // new client
  if (!cmdSvrClient && (cmdSvr.hasClient())) {
    // find free/disconnected spot
    cmdSvrClient = cmdSvr.available();
    clientTime=millis()+2000UL;
  }

  // check clients for data, if found get the command, pass to OnStep and pickup the response, then return the response to client
  while (cmdSvrClient && cmdSvrClient.connected() && (cmdSvrClient.available()>0)) {
    static char cmdBuffer[40]="";
    static int cmdBufferPos=0;

    // get the data
    byte b=cmdSvrClient.read();
    cmdBuffer[cmdBufferPos]=b; cmdBufferPos++; if (cmdBufferPos>39) cmdBufferPos=39; cmdBuffer[cmdBufferPos]=0;

    // send cmd and pickup the response
    if (b == '#' || (strlen(cmdBuffer) == 1 && b == (char)6)) {
      char result[40]="";
      processCommand(cmdBuffer,result,cmdTimeout);               // send cmd to OnStep, pickup response
      if (strlen(result) > 0) { if (cmdSvrClient && cmdSvrClient.connected()) { cmdSvrClient.print(result); delay(2); } } // client response
      if (errorMonitorOn) logCommandErrors(cmdBuffer,result);    // log any errors
      cmdBuffer[0]=0; cmdBufferPos=0;
    } else idle();
  }
  // -------------------------------------------------------------------------------------------------------------------------------
#endif

#if PERSISTENT_COMMAND_CHANNEL == ON
  // -------------------------------------------------------------------------------------------------------------------------------
  // Persistent IP connections on port 9998

  // disconnect client
  static unsigned long persistentClientTime = 0;
  if (persistentCmdSvrClient && (!persistentCmdSvrClient.connected())) persistentCmdSvrClient.stop();
  if (persistentCmdSvrClient && ((long)(persistentClientTime-millis())<0)) persistentCmdSvrClient.stop();

  // new client
  if (!persistentCmdSvrClient && (persistentCmdSvr.hasClient())) {
    // find free/disconnected spot
    persistentCmdSvrClient = persistentCmdSvr.available();
    persistentClientTime=millis()+120000UL;
  }

  // check clients for data, if found get the command, pass to OnStep and pickup the response, then return the response to client
  while (persistentCmdSvrClient && persistentCmdSvrClient.connected() && (persistentCmdSvrClient.available()>0)) {
    static char cmdBuffer[40]="";
    static int cmdBufferPos=0;

    // still active? push back disconnect by 2 minutes
    persistentClientTime=millis()+120000UL;

    // get the data
    byte b=persistentCmdSvrClient.read();
    cmdBuffer[cmdBufferPos]=b; cmdBufferPos++; if (cmdBufferPos>39) cmdBufferPos=39; cmdBuffer[cmdBufferPos]=0;

    // send cmd and pickup the response
    if (b == '#' || (strlen(cmdBuffer) == 1 && b == (char)6)) {
      char result[40]="";
      processCommand(cmdBuffer,result,cmdTimeout);               // send cmd to OnStep, pickup response
      if (strlen(result) > 0) { if (persistentCmdSvrClient && persistentCmdSvrClient.connected()) { persistentCmdSvrClient.print(result); delay(2); } } // client response
      if (errorMonitorOn) logCommandErrors(cmdBuffer,result);    // log any errors
      cmdBuffer[0]=0; cmdBufferPos=0;
    } else idle();
  }
  // -------------------------------------------------------------------------------------------------------------------------------
#endif

}

const char* HighSpeedCommsStr(long baud) {
  if (baud==115200) { return ":SB0#"; }
  if (baud==57600) { return ":SB1#"; }
  if (baud==38400) { return ":SB2#"; }
  if (baud==28800) { return ":SB3#"; }
  if (baud==19200) { return ":SB4#"; } else { return ":SB5#"; }
}

void logCommandErrors(char *cmdBuffer, char *result) {
#if MONITOR_GUIDE_COMMANDS == ON
  if (strstr(cmdBuffer,":Me") || strstr(cmdBuffer,":Mw") || strstr(cmdBuffer,":Mn") || strstr(cmdBuffer,":Ms") || strstr(cmdBuffer,":Mg") ||
      strstr(cmdBuffer,":Qe") || strstr(cmdBuffer,":Qw") || strstr(cmdBuffer,":Qn") || strstr(cmdBuffer,":Qs") || strstr(cmdBuffer,":Q") ||
      strstr(cmdBuffer,";Me") || strstr(cmdBuffer,";Mw") || strstr(cmdBuffer,";Mn") || strstr(cmdBuffer,";Ms") || strstr(cmdBuffer,";Mg") ||
      strstr(cmdBuffer,";Qe") || strstr(cmdBuffer,";Qw") || strstr(cmdBuffer,";Qn") || strstr(cmdBuffer,";Qs") || strstr(cmdBuffer,";Q")) {
    for (int i=8; i>=0; i--) { strcpy(cmdErrorList[i+1].cmd,cmdErrorList[i].cmd); cmdErrorList[i+1].err=cmdErrorList[i].err; }
    strcpy(cmdErrorList[0].cmd,cmdBuffer); cmdErrorList[0].err=CE_NULL;
  }
#endif
  char cmd[]=":GE#";
  processCommand(cmd,result,cmdTimeout);
  int e=CE_REPLY_UNKNOWN;
  if (strlen(result) == 3) e=atoi(result); else strcat(cmdBuffer,"(:GE#)");
  if (e>1) {
    for (int i=8; i>=0; i--) { strcpy(cmdErrorList[i+1].cmd,cmdErrorList[i].cmd); cmdErrorList[i+1].err=cmdErrorList[i].err; }
    strcpy(cmdErrorList[0].cmd,cmdBuffer); cmdErrorList[0].err=e;
  }
}

void idle() {
  server.handleClient(); 
#if ENCODERS == ON
  encoders.poll();
#endif
}

void swapSerial() {
#ifndef ESP32
  Ser.swap();
#endif
}
