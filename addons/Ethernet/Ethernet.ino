/*
 * Title       OnStep Ethernet Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2021 Howard Dutton
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
#define FirmwareVersionMajor  "2"
#define FirmwareVersionMinor  "1"
#define FirmwareVersionPatch  "u"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

// Enable debug and/or status messages to be passed to OnStep for display using its debug facilities
// default "DEBUG OFF", use "DEBUG ON" for background errors only, use "DEBUG VERBOSE" for all errors and status messages
#define DEBUG OFF

#include <limits.h>

// work around for some platform specific code
#if !defined(ESP8266) && !defined(ESP32)
  #define ICACHE_RAM_ATTR
  #define FPSTR
#endif
#ifdef ARDUINO_ARCH_SAMD
  #include <avr/dtostrf.h>
#endif
#define Ser Serial1  // Default=Serial1, This is the hardware serial port where OnStep is attached

#include <Ethernet.h>

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#define DISPLAY_RESET_CONTROLS OFF
#if AXIS1_ENC > 0 && AXIS2_ENC > 0
  #define ENCODERS ON
#endif

// get NV ready
#if defined(ARDUINO_ARCH_SAMD) && ENCODERS == ON
  #include <FlashAsEEPROM.h> // https://github.com/cmaglie/FlashStorage (Called "FlashStorage" in the Arduio IDE Library Manager)
#elif defined(_mk20dx128_h_) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #include <EEPROM.h>
#else
  #define EEPROM_DISABLED
#endif
#include "EEProm.h"

#include "Locale.h"
#include "Globals.h"

#include "CmdServer.h"
#include "WebServer.h"

// The settings in NV (EEPROM) are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD <= 28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

int webTimeout=TIMEOUT_WEB;
int cmdTimeout=TIMEOUT_CMD;

#include "Encoders.h"
#if ENCODERS == ON
  Encoders encoders;
#endif

// macros to help with sending webpage data
#define sendHtmlStart()
#define sendHtml(x) client->print(x); x=""
#define sendHtmlDone(x) client->print(x); x=""

WebServer server;
CmdServer cmdSvr;

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

#if LED_STATUS != OFF
  pinMode(LED_STATUS,OUTPUT);
#endif

// EEPROM Init
#ifndef EEPROM_DISABLED
  nv.init();

  if (nv.readInt(EE_KEY_HIGH) != 8266 || nv.readInt(EE_KEY_LOW) != 0) {
    nv.writeInt(EE_KEY_HIGH,8266);
    nv.writeInt(EE_KEY_LOW,0);
#if ENCODERS == ON
    nv.writeLong(EE_ENC_A1_DIFF_TO,AXIS1_ENC_DIFF_LIMIT_TO);
    nv.writeLong(EE_ENC_A2_DIFF_TO,AXIS2_ENC_DIFF_LIMIT_TO);
    nv.writeLong(EE_ENC_RC_STA,20);     // enc short term average samples
    nv.writeLong(EE_ENC_RC_LTA,200);    // enc long term average samples
    nv.writeLong(EE_ENC_RC_RCOMP,0);    // enc rate comp
    nv.writeLong(EE_ENC_RC_INTP_P,1);   // intpol phase
    nv.writeLong(EE_ENC_RC_INTP_M,0);   // intpol mag
    nv.writeLong(EE_ENC_RC_PROP,10);    // prop
    nv.writeLong(EE_ENC_MIN_GUIDE,100); // minimum guide duration
    nv.writeLong(EE_ENC_A1_ZERO,0);     // absolute Encoder Axis1 zero
    nv.writeLong(EE_ENC_A2_ZERO,0);     // absolute Encoder Axis2 zero
#endif
    nv.commit();
  }

#if ENCODERS == ON
  Axis1EncDiffTo=nv.readLong(EE_ENC_A1_DIFF_TO);
  Axis2EncDiffTo=nv.readLong(EE_ENC_A2_DIFF_TO);
  #if AXIS1_ENC_RATE_CONTROL == ON
    Axis1EncStaSamples=nv.readLong(EE_ENC_RC_STA);
    Axis1EncLtaSamples=nv.readLong(EE_ENC_RC_LTA);
    long l=nv.readLong(EE_ENC_RC_RCOMP); axis1EncRateComp=(float)l/1000000.0;
    #if AXIS1_ENC_INTPOL_COS == ON
      Axis1EncIntPolPhase=nv.readLong(EE_ENC_RC_INTP_P);
      Axis1EncIntPolMag=nv.readLong(EE_ENC_RC_INTP_M);
    #endif
    Axis1EncProp=nv.readLong(EE_ENC_RC_PROP);
    Axis1EncMinGuide=nv.readLong(EE_ENC_MIN_GUIDE);
  #endif
#endif

#endif

Again:
  clearSerialChannel();

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
    VLF("WEM: Ethernet, Connection established");
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
  
  clearSerialChannel();
  
  // say hello
  VF("WEM: Ethernet Addon "); V(FirmwareVersionMajor); V("."); V(FirmwareVersionMinor); VL(FirmwareVersionPatch);
  VF("WEM: MCU = "); VLF(MCU_STR);

  VF("WEM: Web Channel Timeout ms= "); VL(webTimeout);
  VF("WEM: Cmd Channel Timeout ms= "); VL(cmdTimeout);

  VF("WEM: Ethernet IP     = "); VL(ip.toString());
  VF("WEM: Ethernet GATEWAY= "); VL(gateway.toString());
  VF("WEM: Ethernet SUBNET = "); VL(subnet.toString());

#if W5500 == ON
  VLF("WEM: Resetting W5500 using pin 9");
  pinMode(9, OUTPUT); 
  digitalWrite(9, LOW);
  delayMicroseconds(500);
  digitalWrite(9, HIGH);
  delayMicroseconds(1000);
  delay(1000);
#endif

  VLF("WEM: Starting port 80 web svr");
  server.init();
  
  VLF("WEM: Connecting web-page handlers");
  server.on("/index.htm", handleRoot);
  server.on("/configuration.htm", handleConfiguration);
  server.on("/configurationA.txt", configurationAjaxGet);
  server.on("/settings.htm", handleSettings);
  server.on("/settingsA.txt", settingsAjaxGet);
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
  VLF("WEM: Starting port 9999 cmd svr");
  cmdSvr.init(9999,500);
  
  // allow time for the background servers to come up
  delay(2000);

  // clear the serial channel one last time
  clearSerialChannel();

#if ENCODERS == ON
  VLF("WEM: Starting Encoders");
  encoders.init();
#endif

  VLF("WEM: Ethernet Addon is ready");
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

void idle() {
  server.handleClient();
#if ENCODERS == ON
  encoders.poll();
#endif
}
