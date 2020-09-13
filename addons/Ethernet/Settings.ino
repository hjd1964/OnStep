// -----------------------------------------------------------------------------------
// Settings

const char html_settingsStart[] PROGMEM =
"<form method='get' action='/settings.htm'>";

const char html_settingsRefine1[] PROGMEM =
L_REFINE_POLAR_ALIGN ": <br />"
"<button name='rp' value='a' type='submit'>" L_REFINE_PA "</button><br />\r\n"
L_REFINE_MESSAGE1 L_REFINE_MESSAGE2 L_REFINE_MESSAGE3 "</br><br />";

const char html_settingsPark1[] PROGMEM =
L_PARK ": <br />"
"<button name='pk' value='s' type='submit'>" L_SET_PARK "</button>\r\n";

const char html_settingsTrack1[] PROGMEM =
"</br></br>" L_TRACKING " (<span id='tracking'>";
const char html_settingsTrack2[] = 
"</span>): <br />"
"<button name='tk' value='on' type='submit'>" L_ON "</button>"
"<button name='tk' value='off' type='submit'>" L_OFF "</button><br />";
const char html_settingsTrack3[] PROGMEM =
"<button name='tk' value='f' type='submit'>+ (0.02Hz " L_TRK_FASTER ")</button>"
"<button name='tk' value='-' type='submit'>- (0.02Hz " L_TRK_SLOWER ")</button>"
"<button name='tk' value='r' type='submit'>" L_TRK_RESET "</button>";

const char html_settingsTrackComp1[] PROGMEM =
"</br></br>" L_TRK_COMP ": </br>"
"<button name='rr' value='otk' type='submit'>" L_TRK_FULL "</button>"
"<button name='rr' value='on' type='submit'>" L_TRK_REFR "</button>"
"<button name='rr' value='off' type='submit'>" L_OFF "</button>";
const char html_settingsTrackComp2[] PROGMEM =
"</br>"
"<button name='rr' value='don' type='submit'>" L_TRK_DUAL "</button>"
"<button name='rr' value='doff' type='submit'>" L_TRK_SINGLE "</button>\r\n";

const char html_settingsBuzzer1[] PROGMEM =
"<br /><br />" L_BUZZER " (<span id='buzzer'>";
const char html_settingsBuzzer2[] PROGMEM =
"</span>): <br />"
"<button name='ab' value='on' type='submit'>" L_ON "</button>"
"<button name='ab' value='off' type='submit'>" L_OFF "</button>\r\n";

const char html_settingsMFAuto1[] PROGMEM =
"</br></br>" L_MERIDIAN_FLIP_AUTO " (<span id='autoFlip'>";
const char html_settingsMFAuto2[] PROGMEM =
"</span>):<br />"
"<button name='ma' value='now' type='submit'>" L_MERIDIAN_FLIP_NOW "</button>&nbsp;&nbsp;"
"<button name='ma' value='on' type='submit'>" L_ON "</button>"
"<button name='ma' value='off' type='submit'>" L_OFF "</button>";
const char html_settingsMFPause1[] PROGMEM =
"</br></br>" L_MERIDIAN_FLIP_PAUSE " (<span id='pause'>";
const char html_settingsMFPause2[] PROGMEM =
"</span>): <br />"
"<button name='mp' value='on' type='submit'>" L_ON "</button>"
"<button name='mp' value='off' type='submit'>" L_OFF "</button>\r\n";

const char html_settingsEnd[] PROGMEM =
"</form>\r\n";

#ifdef OETHS
void handleSettings(EthernetClient *client) {
#else
void handleSettings() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();

  mountStatus.update(true);

  char temp1[120]="";

  processSettingsGet();
  
  sendHtmlStart();
 
  String data=FPSTR(html_headB);
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  data += FPSTR(html_main_css5);
  sendHtml(data);
  data += FPSTR(html_main_css6);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);
  
  // active ajax page is: settingsAjax();
  data +="<script>var ajaxPage='settings.txt';</script>\n";
  data +=FPSTR(html_ajax_active);
  data +="<script>auto2Rate=2;</script>";

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
  data += FPSTR(html_linksPecN);
  data += FPSTR(html_linksSetS);
  data += FPSTR(html_linksCfgN);
#ifndef OETHS
  data += FPSTR(html_linksWifiN);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);
  
  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data+="<div style='width: 35em;'>";

  data += FPSTR(html_settingsStart);

  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_settingsRefine1);
  }

  data += FPSTR(html_settingsPark1);
  
  data += FPSTR(html_settingsTrack1);
  if (mountStatus.valid()) { if (mountStatus.tracking()) data+=L_ON; else data+=L_OFF; } else data+="?";
  data += FPSTR(html_settingsTrack2);
  data += FPSTR(html_settingsTrack3);
  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_settingsTrackComp1);
    data += FPSTR(html_settingsTrackComp2);
  }
  sendHtml(data);

  data += FPSTR(html_settingsBuzzer1);
  if (mountStatus.valid()) { if (mountStatus.buzzerEnabled()) data+=L_ON; else data+=L_OFF; } else data+="?";
  data += FPSTR(html_settingsBuzzer2);

  if (mountStatus.mountType()==MT_GEM) {
    data += FPSTR(html_settingsMFAuto1);
    if (mountStatus.valid()) { if (mountStatus.autoMeridianFlips()) data+=L_ON; else data+=L_OFF; } else data+="?";
    data += FPSTR(html_settingsMFAuto2);
    data += FPSTR(html_settingsMFPause1);
    if (mountStatus.valid()) { if (mountStatus.pauseAtHome()) data+=L_ON; else data+=L_OFF; } else data+="?";
    data += FPSTR(html_settingsMFPause2);
  }

  data += FPSTR(html_settingsEnd);

  data += "<br />";
  data += "</div></div></body></html>";
  
  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void settingsAjax(EthernetClient *client) {
#else
void settingsAjax() {
#endif
  String data="";
  mountStatus.update();
  data += "tracking|";   if (mountStatus.valid()) { if (mountStatus.tracking())          data+=L_ON; else data+=L_OFF; } else data+="?"; data+="\n";
  data += "buzzer|";     if (mountStatus.valid()) { if (mountStatus.buzzerEnabled())     data+=L_ON; else data+=L_OFF; } else data+="?"; data+="\n";
  if (mountStatus.mountType()==MT_GEM) {
    data += "autoFlip|"; if (mountStatus.valid()) { if (mountStatus.autoMeridianFlips()) data+=L_ON; else data+=L_OFF; } else data+="?"; data+="\n";
    data += "pause|";    if (mountStatus.valid()) { if (mountStatus.pauseAtHome())       data+=L_ON; else data+=L_OFF; } else data+="?"; data+="\n";
  }
#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

void processSettingsGet() {
  // from the Settings.htm page -------------------------------------------------------------------
  String v;

  // refine polar align
  v=server.arg("rp");
  if (v!="") {
    if (v=="a") commandBool(":MP#");
  }
  // set-park
  v=server.arg("pk");
  if (v!="") {
    if (v=="s") commandBool(":hQ#");
  }
  // Tracking control
  v=server.arg("tk");
  if (v!="") {
    if (v=="on")   commandBool(":Te#");  // enable tracking
    if (v=="off")  commandBool(":Td#");  // disable tracking
    if (v=="f")    commandBlind(":T+#"); // 0.02hz faster
    if (v=="-")    commandBlind(":T-#"); // 0.02hz slower
    if (v=="r")    commandBlind(":TR#"); // reset
  }
  // Refraction Rate Tracking control
  v=server.arg("rr");
  if (v!="") {
    if (v=="otk")  commandBool(":To#"); // pointing model compensated
    if (v=="on")   commandBool(":Tr#"); // compensated on
    if (v=="off")  commandBool(":Tn#"); // compensated off
    if (v=="don")  commandBool(":T2#"); // compensated 2 axis
    if (v=="doff") commandBool(":T1#"); // compensated 1 axis
  }
  // Alert buzzer
  v=server.arg("ab");
  if (v!="") {
    if (v=="on")   commandBool(":SX97,1#");
    if (v=="off")  commandBool(":SX97,0#");
  }
  // Auto-continue
  v=server.arg("ma");
  if (v!="") {
    if (v=="now")  commandBool(":MN#");
    if (v=="on")   commandBool(":SX95,1#");
    if (v=="off")  commandBool(":SX95,0#");
  }
  // Pause at meridian flip
  v=server.arg("mp");
  if (v!="") {
    if (v=="on")   commandBool(":SX98,1#");
    if (v=="off")  commandBool(":SX98,0#");
  }
}
