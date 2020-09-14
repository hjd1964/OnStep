// -----------------------------------------------------------------------------------
// Controls for PEC

const char html_pec1[] PROGMEM = "<div class='b1' style='width: 27em'>";
const char html_pec2[] PROGMEM = "<div style='float: left'>" L_PEC_STATUS ":</div><br/><div id='status'>?</div></div><br class='clear' />";

const char html_pecControls0[] PROGMEM =
"<div class='b1' style='width: 27em'><div style='float: left'>" L_PEC_CONTROL ":</div><br />";
const char html_pecControls1[] PROGMEM =
"<form method='get' action='/pec.htm'>"
"<button name='pe' value='pl' type='submit'>" L_PEC_PLAY "</button>";
const char html_pecControls2[] PROGMEM =
"<button name='pe' value='st' type='submit'>" L_STOP "</button><br /><br />"
"<button name='pe' value='cl' type='submit'>" L_PEC_CLEAR "</button>"
"<button name='pe' value='re' type='submit'>" L_PEC_REC "</button><br />";
const char html_pecControls3[] PROGMEM =
L_PEC_CLEAR_MESSAGE "<br />";
const char html_pecControls4[] PROGMEM =
"<br /><button name='pe' value='wr' type='submit'>" L_PEC_EEWRITE "</button><br />" L_PEC_EEWRITE_MESSAGE "<br /><br />";
const char html_pecControls5[] PROGMEM =
"</form></div><br class='clear' /><br />\r\n";

#ifdef OETHS
void handlePec(EthernetClient *client) {
#else
void handlePec() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
      
  mountStatus.update();

  char temp1[120] = "";

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

  if (mountStatus.pecEnabled()) {
    // active ajax page is: pecAjax();
    data +="<script>var ajaxPage='pec.txt';</script>\n";
    data +=FPSTR(html_ajax_active);
    data +="<script>auto2Rate=2;</script>";
  }

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
  data += FPSTR(html_linksPecS);
  data += FPSTR(html_linksSetN);
  data += FPSTR(html_linksCfgN);
#ifndef OETHS
  data += FPSTR(html_linksWifiN);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data += FPSTR(html_pec1);

  if (mountStatus.pecEnabled()) {
    data += FPSTR(html_pec2);
    data += FPSTR(html_pecControls0);
    data += FPSTR(html_pecControls1);
    data += FPSTR(html_pecControls2);
    data += FPSTR(html_pecControls3);
    data += FPSTR(html_pecControls4);
    data += FPSTR(html_pecControls5);
    data += "</div></body></html>";
  } else {
    data += L_PEC_NO_PEC_MESSAGE;
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
  if ((mountStatus.mountType()!=MT_ALTAZM) && (mountStatus.update()) && (command(":$QZ?#",temp))) {
    if (temp[0]=='I') data += L_PEC_IDLE; else
    if (temp[0]=='p') data += L_PEC_WAIT_PLAY; else
    if (temp[0]=='P') data += L_PEC_PLAYING; else
    if (temp[0]=='r') data += L_PEC_WAIT_REC; else
    if (temp[0]=='R') data += L_PEC_RECORDING; else data += L_PEC_UNK;
    if (mountStatus.pecRecording()) data += " (" L_PEC_EEWRITING ")";
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
  if (v != "") {
    if (v == "pl") commandBlind(":$QZ+#"); // play
    if (v == "st") commandBlind(":$QZ-#"); // stop
    if (v == "re") commandBlind(":$QZ/#"); // record
    if (v == "cl") commandBlind(":$QZZ#"); // clear
    if (v == "wr") commandBlind(":$QZ!#"); // write to eeprom
  }
}
