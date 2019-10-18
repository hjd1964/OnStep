/*
 * Title       OnStep WiFi Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2019 Howard Dutton
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

#define Product "OnEsp"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "8"
#define FirmwareVersionPatch  "f"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

#include <limits.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiAP.h>
#include <EEPROM.h>

#define DEBUG_OFF   // Turn _ON to allow Ethernet startup without OnStep (Serial port for debug at 9600 baud)

#include "Constants.h"
#include "Config.h"

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

#define AXIS1_ENC_A_PIN 14 // pin# for Axis1 encoder, for A or CW
#define AXIS1_ENC_B_PIN 12 // pin# for Axis1 encoder, for B or CCW
#define AXIS2_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
#define AXIS2_ENC_B_PIN 4  // pin# for Axis1 encoder, for B or CCW
#include "Encoders.h"
#if ENCODERS == ON
Encoders encoders;
#endif

#include "MountStatus.h"

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

int WebTimeout=TIMEOUT_WEB;
int CmdTimeout=TIMEOUT_CMD;

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

ESP8266WebServer server(80);

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

    EEPROM_writeInt(10,(int)WebTimeout);
    EEPROM_writeInt(12,(int)CmdTimeout);

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

    WebTimeout=EEPROM_readInt(10);
    CmdTimeout=EEPROM_readInt(12);

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
  Ser.begin(SERIAL_BAUD_DEFAULT);
#if SERIAL_SWAP == ON
  Ser.swap();
#endif
  delay(2000);

  byte tb=0;
Again:
#if LED_STATUS != OFF
  digitalWrite(LED_STATUS,LOW);
#endif
  char c=0;

  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
#if LED_STATUS != OFF
    digitalWrite(LED_STATUS,HIGH);
#endif
    delay(500);
    Ser.flush();
    c=serialRecvFlush();
#if LED_STATUS != OFF
    digitalWrite(LED_STATUS,LOW);
#endif
    delay(500);
  }

  // safety net
  if ((c=='R') || (!accessPointEnabled && !stationEnabled)) {
    // reset EEPROM values, triggers an init
    EEPROM_writeInt(0,0); EEPROM_writeInt(2,0);
    accessPointEnabled=true;
    EEPROM.commit();
    Ser.println();
    Ser.println("Cycle power for reset to defaults.");
    Ser.println();
  }
 
  if (SERIAL_BAUD != SERIAL_BAUD_DEFAULT) {

    // switch OnStep Serial1 up to ? baud
    Ser.print(HighSpeedCommsStr(SERIAL_BAUD));
    delay(200);
    int count=0; c=0; while (Ser.available() > 0) { count++; if (count == 1) c=Ser.read(); }
    if (c == '1') {
        Ser.begin(SERIAL_BAUD);
#if SERIAL_SWAP == ON
        Ser.swap();
#endif
        delay(2000);
    } else {
#if LED_STATUS != OFF
      digitalWrite(LED_STATUS,HIGH);
#endif
      // got nothing back, toggle baud rate and try again
      tb++;
      if (tb == 11) tb=1;
      if (tb == 1) Ser.begin(SERIAL_BAUD_DEFAULT);
      if (tb == 6) Ser.begin(SERIAL_BAUD);
      
      if (tb == 1 || tb == 6) {
#if SERIAL_SWAP == ON
        Ser.swap();
#endif
        delay(2000);
      }
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
  server.on("/control.htm", handleControl);
#if ENCODERS == ON
  server.on("/enc.htm", handleEncoders);
  server.on("/encA.txt", encAjaxGet);
  server.on("/enc.txt", encAjax);
#endif
  server.on("/control.txt", controlAjax);
  server.on("/controlA.txt", controlAjaxGet);
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
    static char writeBuffer[40]="";
    static int writeBufferPos=0;

    // get the data
    byte b=cmdSvrClient.read();

    writeBuffer[writeBufferPos]=b; writeBufferPos++; if (writeBufferPos>39) writeBufferPos=39; writeBuffer[writeBufferPos]=0;

    // send cmd and pickup the response
    if ((b=='#') || ((strlen(writeBuffer)==1) && (b==(char)6))) {
      char readBuffer[40]="";
      readLX200Bytes(writeBuffer,readBuffer,CmdTimeout); writeBuffer[0]=0; writeBufferPos=0;

      // return the response, if we have one
      if (strlen(readBuffer)>0) {
        if (cmdSvrClient && cmdSvrClient.connected()) {
          cmdSvrClient.print(readBuffer);
          delay(2);
        }
      }

    } else {
      server.handleClient(); 
  #if ENCODERS == ON
      encoders.poll();
  #endif
    }
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
    static char writeBuffer[40]="";
    static int writeBufferPos=0;

    // still active? push back disconnect by 2 minutes
    persistentClientTime=millis()+120000UL;

    // get the data
    byte b=persistentCmdSvrClient.read();

    writeBuffer[writeBufferPos]=b; writeBufferPos++; if (writeBufferPos>39) writeBufferPos=39; writeBuffer[writeBufferPos]=0;

    // send cmd and pickup the response
    if ((b=='#') || ((strlen(writeBuffer)==1) && (b==(char)6))) {
      char readBuffer[40]="";
      readLX200Bytes(writeBuffer,readBuffer,CmdTimeout); writeBuffer[0]=0; writeBufferPos=0;

      // return the response, if we have one
      if (strlen(readBuffer)>0) {
        if (persistentCmdSvrClient && persistentCmdSvrClient.connected()) {
          persistentCmdSvrClient.print(readBuffer);
          delay(2);
        }
      }

    } else {
      server.handleClient(); 
  #if ENCODERS == ON
      encoders.poll();
  #endif
    }
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
