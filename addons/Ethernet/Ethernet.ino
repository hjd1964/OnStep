/*
 * Title       OnStep Ethernet Server
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
 * W5100 and W5500 Ethernet for OnStep
 *
 */
 
#define OETHS
#define Product "Ethernet Server"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "13"
#define FirmwareVersionPatch  "d"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

// work around PROGMEM use on Teensy3.2 etc: FPSTR() gets ignored
#if !defined(ESP8266) && !defined(ESP32)
  #define ICACHE_RAM_ATTR
  #define FPSTR
#endif

#include <limits.h>

// pretty sure this wasn't being compiled in even if W5500 was ON (ahead of #include Config.h) so commented out for now.
//#ifdef W5500_ON
//  #include <Ethernet3.h>  // https://github.com/PaulStoffregen/Ethernet
//#else
//  #include <Ethernet.h>
//#endif

#define DEBUG_OFF   // Turn _ON to allow Ethernet startup without OnStep (Serial port for debug at 9600 baud)

#include <Ethernet.h>
#include "CmdServer.h"

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Globals.h"

#include "WebServer.h"

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

int webTimeout=TIMEOUT_WEB;
int cmdTimeout=TIMEOUT_CMD;

#define AXIS1_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
#define AXIS1_ENC_B_PIN 6  // pin# for Axis1 encoder, for B or CCW
#define AXIS2_ENC_A_PIN 7  // pin# for Axis2 encoder, for A or CW
#define AXIS2_ENC_B_PIN 8  // pin# for Axis2 encoder, for B or CCW
#include "Encoders.h"
#if ENCODERS == ON
Encoders encoders;
#endif

// macros to help with sending webpage data
#define sendHtmlStart()
#define sendHtml(x) client->print(x); x=""
#define sendHtmlDone(x) client->print(x); x=""

bool errorMonitorOn=false;
struct errors {
  char cmd[40];
  byte err;
};
struct errors cmdErrorList[10] = { {"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0} };

WebServer server;
CmdServer cmdSvr;

#if defined(_mk20dx128_h_) || defined(__MK20DX128__) || defined(__MK20DX256__)
  #include <EEPROM.h>
#else
  #define EEPROM_DISABLED
#endif
#define EEPROM_COMMIT_DISABLED

void handleNotFound(EthernetClient *client) {
  String message = "File Not Found\n\n";
  client->print(message);
//  server.send(404, "text/plain", message);
}

#include "Accessories.h"
#include "MountStatus.h"

void setup(void){
  long serial_baud = SERIAL_BAUD;
  Ser.begin(SERIAL_BAUD_DEFAULT);
  byte tb=1;

// EEPROM Init
#ifndef EEPROM_DISABLED
  if ((EEPROM_readInt(0)!=8267) || (EEPROM_readInt(2)!=0)) {
    EEPROM_writeInt(0,8267);
    EEPROM_writeInt(2,0);
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
  } else {  
#if ENCODERS == ON
    Axis1EncDiffLimit=EEPROM_readLong(600);
    Axis2EncDiffLimit=EEPROM_readLong(604);
#if AXIS1_ENC_RATE_CONTROL == ON
    Axis1EncStaSamples=EEPROM_readLong(608);
    Axis1EncLtaSamples=EEPROM_readLong(612);
    long l=EEPROM_readLong(616); Axis1EncRateComp=(float)l/1000000.0;
    Axis1EncIntPolPeriod=EEPROM_readLong(620);
    Axis1EncIntPolPhase =EEPROM_readLong(624);
    Axis1EncIntPolMag   =EEPROM_readLong(628);
    Axis1EncProp        =EEPROM_readLong(632);
    Axis1EncMinGuide    =EEPROM_readLong(636);
#endif
#endif
  }
#endif

Again:
  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#"); delay(500);
    serialRecvFlush();
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
    Ser.begin(serial_baud); delay(2000);
  } else {
    // got nothing back, toggle baud rate and/or swap ports
    serialRecvFlush();
    tb++;
    if (tb == 16) tb=1;
    if (tb == 1) { Ser.begin(SERIAL_BAUD_DEFAULT); delay(2000); }
    if (tb == 6) { Ser.begin(serial_baud); delay(2000); }
    if (tb == 11) { Ser.begin(19200); delay(2000); }
    goto Again;
  }
  
  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
    delay(50);
    serialRecvFlush();
  }

#if W5500 == ON
  // reset a W5500
  pinMode(9, OUTPUT); 
  digitalWrite(9, LOW);
  delayMicroseconds(500);
  digitalWrite(9, HIGH);
  delayMicroseconds(1000);
  delay(1000);
#endif

  // Initialize the www server
  server.init();
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
  server.on("/control.txt", controlAjax);
  server.on("/controlA.txt", controlAjaxGet);
  server.on("/auxiliary.htm", handleAux);
  server.on("/auxiliaryA.txt", auxAjaxGet);
  server.on("/auxiliary.txt", auxAjax);
  server.on("/pec.htm", handlePec);
  server.on("/pec.txt", pecAjax);
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  // Initialize the cmd server, timeout after 500ms
  cmdSvr.init(9999,500);

#if ENCODERS == ON
  encoders.init();
#endif
}

void loop(void){
  server.handleClient();
  cmdSvr.handleClient();
#if ENCODERS == ON
  encoders.poll();
#endif

  // check clients for data, if found get the command, send cmd and pickup the response, then return the response
  static char cmdBuffer[40]="";
  static int cmdBufferPos=0;
  while (cmdSvr.available()) {
    // get the data
    byte b=cmdSvr.read();
    cmdBuffer[cmdBufferPos]=b; cmdBufferPos++; if (cmdBufferPos>39) cmdBufferPos=39; cmdBuffer[cmdBufferPos]=0;

    if (b == '#' || (strlen(cmdBuffer) == 1 && b == (char)6)) {
      char result[40]="";
      processCommand(cmdBuffer,result,cmdTimeout);                 // send cmd to OnStep, pickup response
      if (strlen(result) > 0) { cmdSvr.print(result); delay(2); }  // return any response to client
      if (errorMonitorOn) logCommandErrors(cmdBuffer,result);      // log any errors
      cmdBuffer[0]=0; cmdBufferPos=0;
    } else idle();
  }
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
  if (strlen(result) == 3) e=atoi(result); else strcpy(cmdBuffer,":GE#");
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
