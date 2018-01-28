/*
 * Title       OnStepEthernetServer
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
#define FirmwareVersionMinor  "0"
#define FirmwareVersionPatch  "b"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

#include <Ethernet.h>
#include "Config.h"
#include "CmdServer.h"
#include "WebServer.h"

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

const char* html_head1 = "<!DOCTYPE HTML>\r\n<html>\r\n<head>\r\n";
const char* html_headerPec = "<meta http-equiv=\"refresh\" content=\"5; URL=/pec.htm\">\r\n";
const char* html_headerIdx = "<meta http-equiv=\"refresh\" content=\"5; URL=/index.htm\">\r\n";
const char* html_head2 = "</head>\r\n<body bgcolor=\"#26262A\">\r\n";

const char* html_main_css1 = "<STYLE>";
const char* html_main_css2 = ".a { background-color: #111111; } .t { padding: 10px 10px 20px 10px; border: 5px solid #551111;";
const char* html_main_css3 = " margin: 25px 25px 0px 25px; color: #999999; background-color: #111111; } input { width:4em; font-weight: bold; background-color: #A01010; padding: 2px 2px; }";
const char* html_main_css4 = ".b { padding: 10px; border-left: 5px solid #551111; border-right: 5px solid #551111; border-bottom: 5px solid #551111; margin: 0px 25px 25px 25px; color: #999999;";
const char* html_main_css5 = "background-color: #111111; } select { width:4em; font-weight: bold; background-color: #A01010; padding: 2px 2px; } .c { color: #A01010; font-weight: bold; }";
const char* html_main_css6 = "h1 { text-align: right; } a:hover, a:active { background-color: red; } .g { color: #105010; font-weight: bold; }";
const char* html_main_css7 = "a:link, a:visited { background-color: #332222; color: #a07070; border:1px solid red; padding: 5px 10px;";
const char* html_main_css8 = " margin: none; text-align: center; text-decoration: none; display: inline-block; }";
const char* html_main_css9 = "button { background-color: #A01010; font-weight: bold; border-radius: 5px; font-size: 12px; margin: 2px; padding: 4px 8px; }</STYLE>";

const char* html_links1in = "<a href=\"/index.htm\" style=\"background-color: #552222;\">Status</a><a href=\"/control.htm\">Control</a>";
const char* html_links2in = "<a href=\"/guide.htm\">Guide</a><a href=\"/pec.htm\">PEC</a><a href=\"/settings.htm\">Settings</a>";
const char* html_links3in = "<a href=\"/config.htm\">Config.h</a><br />";

const char* html_links1ct = "<a href=\"/index.htm\">Status</a><a href=\"/control.htm\" style=\"background-color: #552222;\">Control</a>";
const char* html_links2ct = "<a href=\"/guide.htm\">Guide</a><a href=\"/pec.htm\">PEC</a><a href=\"/settings.htm\">Settings</a>";
const char* html_links3ct = "<a href=\"/config.htm\">Config.h</a><br />";

const char* html_links1gu = "<a href=\"/index.htm\">Status</a><a href=\"/control.htm\">Control</a>";
const char* html_links2gu = "<a href=\"/guide.htm\" style=\"background-color: #552222;\">Guide</a><a href=\"/pec.htm\">PEC</a><a href=\"/settings.htm\">Settings</a>";
const char* html_links3gu = "<a href=\"/config.htm\">Config.h</a><br />";

const char* html_links1pe = "<a href=\"/index.htm\">Status</a><a href=\"/control.htm\">Control</a>";
const char* html_links2pe = "<a href=\"/guide.htm\">Guide</a><a href=\"/pec.htm\" style=\"background-color: #552222;\">PEC</a><a href=\"/settings.htm\">Settings</a>";
const char* html_links3pe = "<a href=\"/config.htm\">Config.h</a><br />";

const char* html_links1se = "<a href=\"/index.htm\">Status</a><a href=\"/control.htm\">Control</a>";
const char* html_links2se = "<a href=\"/guide.htm\">Guide</a><a href=\"/pec.htm\">PEC</a><a href=\"/settings.htm\" style=\"background-color: #552222;\">Settings</a>";
const char* html_links3se = "<a href=\"/config.htm\">Config.h</a><br />";

const char* html_links1co = "<a href=\"/index.htm\">Status</a><a href=\"/control.htm\">Control</a>";
const char* html_links2co = "<a href=\"/guide.htm\">Guide</a><a href=\"/pec.htm\">PEC</a><a href=\"/settings.htm\">Settings</a>";
const char* html_links3co = "<a href=\"/config.htm\" style=\"background-color: #552222;\">Config.h</a><br />";

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

  // switch OnStep Serial up to ? baud
  Ser.print(SERIAL_BAUD);
  delay(100);
  int count=0; c=0;
  while (Ser.available()>0) { count++; if (count==1) c=Ser.read(); }
  if (c=='1') {
    HighSpeedComms();
  } else {
    // got nothing back, toggle baud rate and try again
    tb++;
    if (tb==7) tb=1;
    if (tb==1) Serial.begin(SERIAL_BAUD_DEFAULT);
    if (tb==4) HighSpeedComms();

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

  // Initialize the www server
  server.init();
  server.on("index.htm", handleRoot);
  server.on("settings.htm", handleSettings);
  server.on("control.htm", handleControl);
  server.on("guide.htm", handleGuide);
  server.on("guide.txt", handleGuideAjax);
  server.on("pec.htm", handlePec);
  server.on("config.htm", handleConfig);
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // Initialize the cmd server, timeout after 500ms
  cmdSvr.init(9999,500);
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
}

void HighSpeedComms() {
  if (!strcmp(SERIAL_BAUD,":SB0#")) Ser.begin(115200); else
  if (!strcmp(SERIAL_BAUD,":SB1#")) Ser.begin(57600); else
  if (!strcmp(SERIAL_BAUD,":SB2#")) Ser.begin(38400); else
  if (!strcmp(SERIAL_BAUD,":SB3#")) Ser.begin(28800); else
  if (!strcmp(SERIAL_BAUD,":SB4#")) Ser.begin(19200); else Ser.begin(9600);
}

