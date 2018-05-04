/*
 * Title       OnStep Ethernet Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2018 Howard Dutton
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
 * ESP8266-01 OnStep control
 *
 */
 
#define Product "OnEth"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "3"
#define FirmwareVersionPatch  "a"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

#define OETHS

#include <Ethernet.h>
#include "Config.h"
#include "CmdServer.h"
#include "WebServer.h"
#include "Constants.h"

#include "Encoders.h"
Encoders encoders;

#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

// base timeouts
int WebTimeout=TIMEOUT_WEB;  // default 15
int CmdTimeout=TIMEOUT_CMD;  // default 30

WebServer server;
CmdServer cmdSvr;

unsigned long clientTime = 0;

char writeBuffer[40]="";
int writeBufferPos=0;
byte tb=0;

enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22
byte pierSide = PierSideNone;
int AlignMaxNumStars = -1;

void handleNotFound(EthernetClient *client) {
  String message = "File Not Found\n\n";
  client->print(message);
//  server.send(404, "text/plain", message);
}

void setup(void){
  SerialUSB.begin(9600);
  Ser.begin(SERIAL_BAUD_DEFAULT);
  delay(3000);
  
Again:
  char c=0;

  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
    delay(500);
    c=serialRecvFlush();
  }

  if (SERIAL_BAUD!=SERIAL_BAUD_DEFAULT) {

    // switch OnStep Serial up to ? baud
    Ser.print(HighSpeedCommsStr(SERIAL_BAUD));
    delay(100);
    int count=0; c=0;
    while (Ser.available()>0) { count++; if (count==1) c=Ser.read(); }
    if (c=='1') {
      Ser.begin(SERIAL_BAUD);
    } else {
      // got nothing back, toggle baud rate and try again
      tb++;
      if (tb==7) tb=1;
      if (tb==1) Serial.begin(SERIAL_BAUD_DEFAULT);
      if (tb==4) Serial.begin(SERIAL_BAUD);
  
      delay(1000);
#ifndef DEBUG_ON
      goto Again;
#else
      Ser.begin(9600);
#endif
    }
    
    // clear the buffers and any noise on the serial lines
    for (int i=0; i<3; i++) {
      Ser.print(":#");
      delay(50);
      serialRecvFlush();
    }
  }

  // Initialize the www server
  server.init();
  server.on("index.htm", handleRoot);
  server.on("configuration.htm", handleConfiguration);
  server.on("settings.htm", handleSettings);
  server.on("control.htm", handleControl);
  server.on("control.txt", controlAjax);
  server.on("guide.txt", guideAjax);
  server.on("pec.htm", handlePec);
  server.on("pec.txt", pecAjax);
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  // Initialize the cmd server, timeout after 500ms
  cmdSvr.init(9999,500);

  encoders.init();
}

void loop(void){
  server.handleClient();
  cmdSvr.handleClient();

  // check clients for data, if found get the command, send cmd and pickup the response, then return the response
  while (cmdSvr.available()) {
    // get the data
    byte b=cmdSvr.read();
    writeBuffer[writeBufferPos]=b; writeBufferPos++; if (writeBufferPos>39) writeBufferPos=39; writeBuffer[writeBufferPos]=0;

    // send cmd and pickup the response
    if (b=='#' || ((strlen(writeBuffer)==1) && (b==(char)6))) {
      char readBuffer[40]="";
      readLX200Bytes(writeBuffer,readBuffer,CmdTimeout); writeBuffer[0]=0; writeBufferPos=0;

      // return the response, if we have one
      if (strlen(readBuffer)>0) {
        cmdSvr.print(readBuffer);
        delay(2);
      }

    } else server.handleClient();
  }

  encoders.poll();
}

const char* HighSpeedCommsStr(long baud) {
  if (baud==115200) { return ":SB0#"; }
  if (baud==57600) { return ":SB1#"; }
  if (baud==38400) { return ":SB2#"; }
  if (baud==28800) { return ":SB3#"; }
  if (baud==19200) { return ":SB4#"; } else { return ":SB5#"; }
}


