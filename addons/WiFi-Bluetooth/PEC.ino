// -----------------------------------------------------------------------------------
// Controls for PEC

const char html_pec1[] PROGMEM = "<div class='b1' style='width: 27em'>";
const char html_pec2[] PROGMEM = "<div style='float: left'>PEC Status:</div><br/><div id='status'>?</div></div><br class='clear' />";

const char html_pecControls0[] PROGMEM =
"<div class='b1' style='width: 27em'><div style='float: left'>PEC Control:</div><br />";
const char html_pecControls1[] PROGMEM =
"<form method='get' action='/pec.htm'>"
"<button name='pe' value='pl' type='submit'>Play</button>";
const char html_pecControls2[] PROGMEM =
"<button name='pe' value='st' type='submit'>Stop</button><br /><br />"
"<button name='pe' value='cl' type='submit'>Clear</button><button name='pe' value='re' type='submit'>Record</button><br />";
const char html_pecControls3[] PROGMEM =
"Clear erases the memory buffer not EEPROM.  During recording corrections are averaged 3:1 favoring the buffer unless cleared in which case the full correction is used.<br />";
const char html_pecControls4[] PROGMEM =
"<br /><button name='pe' value='wr' type='submit'>Write to EEPROM</button><br />Writes PEC data to EEPROM so it's remembered if OnStep is restarted.  Writing the data can take a few seconds.<br /><br />";
const char html_pecControls4a[] PROGMEM =
"<br /><br />Writing to EEPROM disabled.<br /><br />";
const char html_pecControls5[] PROGMEM =
"</form></div><br class='clear' /><br />\r\n";

#ifdef OETHS
void handlePec(EthernetClient *client) {
#else
void handlePec() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
      
  char temp1[80] = "";

  processPecGet();

  sendHtmlStart();

  // send a standard http response header
  String data=FPSTR(html_headB);
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  data += FPSTR(html_main_css5);
  data += FPSTR(html_main_css6);
  sendHtml(data);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_css_control1);
  data += FPSTR(html_main_css_control2);
  data += FPSTR(html_main_css_control3);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  mountStatus.update();

  if (mountStatus.mountType()!=MT_ALTAZM) {
    // active ajax page is: pecAjax();
    data +="<script>var ajaxPage='pec.txt';</script>\n";
    data +=FPSTR(html_ajax_active);
    data +="<script>auto2Rate=2;</script>";
  }

  // finish the standard http response header
  data += FPSTR(html_onstep_header1);
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_links1N);
  data += FPSTR(html_links2N);
  data += FPSTR(html_links3S);
#ifdef ENCODERS_ON
  data += FPSTR(html_linksEncN);
#endif
  data += FPSTR(html_links4N);
  data += FPSTR(html_links5N);
#ifndef OETHS
  data += FPSTR(html_links6N);
#endif
  data += FPSTR(html_onstep_header4);
  data += FPSTR(html_pec1);
  sendHtml(data);

  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_pec2);
    data += FPSTR(html_pecControls0);
    data += FPSTR(html_pecControls1);
    data += FPSTR(html_pecControls2);
    data += FPSTR(html_pecControls3);
    data += FPSTR(html_pecControls4);
    data += FPSTR(html_pecControls5);
    data += "</div></body></html>";
  } else {
    data += "PEC CONTROLS DISABLED";
    data += "</div><br class='clear' /></body></html>";
  }

  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void pecAjax(EthernetClient *client) {
#else
void pecAjax() {
#endif
  String data="";
  char temp[80]="";
  
  data += "status|";
  if ((mountStatus.mountType()!=MT_ALTAZM) && (mountStatus.update()) && (sendCommand(":$QZ?#",temp))) {
    if (temp[0]=='I') data +="Idle"; else
    if (temp[0]=='p') data +="Play waiting to start"; else
    if (temp[0]=='P') data +="Playing"; else
    if (temp[0]=='r') data +="Record waiting for index to arrive"; else
    if (temp[0]=='R') data +="Recording"; else data += "Unknown";
    if (mountStatus.pecRecording()) data += " (writing to EEPROM)";
  } else { data += "?"; }
  data += "\n";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

void processPecGet() {
  String v;

  // PEC control
  v=server.arg("pe");
  if (v!="") {
    if (v=="pl") { // play
      Ser.print(":$QZ+#");
    }
    if (v=="st") { // stop
      Ser.print(":$QZ-#");
    }
    if (v=="re") { // record
      Ser.print(":$QZ/#");
    }
    if (v=="cl") { // clear
      Ser.print(":$QZZ#");
    };
    if (v=="wr") { // write to eeprom
      Ser.print(":$QZ!#");
    }
  }
}
