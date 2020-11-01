// -----------------------------------------------------------------------------------
// Settings

const char html_settingsScript[] PROGMEM =
"<script>\n"
"function s(key,v1) {\n"
  "var xhttp = new XMLHttpRequest();\n"
  "xhttp.open('GET', 'settingsA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);\n"
  "xhttp.send();\n"
"}\n"
"</script>\n";

const char html_settingsStart[] PROGMEM =
"<form method='get' action='/settings.htm'>";

const char html_settingsSlewSpeed1[] PROGMEM =
L_MAX_SLEW_SPEED ": <br />"
"<button id='sr_vf' class='btns_right' onpointerdown=\"s('ss','vf')\" type='button' disabled>" L_VFAST "</button>"
"<button id='sr_f'  class='btns_mid'   onpointerdown=\"s('ss','f')\"  type='button' disabled>" L_FAST "</button>"
"<button id='sr_n'  class='btns_mid'   onpointerdown=\"s('ss','n')\"  type='button' disabled>" L_NORMAL "</button>";
const char html_settingsSlewSpeed2[] PROGMEM =
"<button id='sr_s'  class='btns_mid'   onpointerdown=\"s('ss','s')\"  type='button' disabled>" L_SLOW "</button>"
"<button id='sr_vs' class='btns_left'  onpointerdown=\"s('ss','vs')\" type='button' disabled>" L_VSLOW "</button>"
"<br /><br />\r\n";

const char html_settingsTrackComp1[] PROGMEM =
L_TRK_COMP ": <br />";
const char html_settingsTrackComp2[] PROGMEM =
"<button id='ot_on'  class='btns_right' onpointerdown=\"s('rr','otk')\"  type='button' disabled>" L_TRK_FULL "</button>"
"<button id='ot_ref' class='btns_mid'   onpointerdown=\"s('rr','on')\"   type='button' disabled>" L_TRK_REFR "</button>"
"<button id='ot_off' class='btns_left'  onpointerdown=\"s('rr','off')\"  type='button' disabled>" L_OFF "</button><br />\r\n";
const char html_settingsTrackComp3[] PROGMEM =
"<button id='ot_dul' class='btns_right' onpointerdown=\"s('rr','don')\"  type='button' disabled>" L_TRK_DUAL "</button>"
"<button id='ot_sgl' class='btns_left'  onpointerdown=\"s('rr','doff')\" type='button' disabled>" L_TRK_SINGLE "</button>"
"<br /><br />\r\n";

const char html_settingsPark1[] PROGMEM =
L_PARK ": <br />"
"<button onpointerdown=\"s('pk','s')\" type='button'>" L_SET_PARK "</button>"
"<br /><br />\r\n";

const char html_settingsTrack1[] PROGMEM =
L_TRACKING_RATE ": <br />";
const char html_settingsTrack2[] PROGMEM =
"<button onpointerdown=\"s('tk','f')\" type='button'>+ (0.02Hz " L_TRK_FASTER ")</button>"
"<button onpointerdown=\"s('tk','-')\" type='button'>- (0.02Hz " L_TRK_SLOWER ")</button>"
"<button onpointerdown=\"s('tk','r')\" type='button'>" L_TRK_RESET "</button>"
"<br /><br />\r\n";

const char html_settingsBuzzer1[] PROGMEM =
L_BUZZER ": <br />";
const char html_settingsBuzzer2[] PROGMEM =
"<button id='bzr_on'  class='btns_right' onpointerdown=\"s('ab','on')\"  type='button' disabled>" L_ON "</button>"
"<button id='bzr_off' class='btns_left'  onpointerdown=\"s('ab','off')\" type='button' disabled>" L_OFF "</button>"
"<br /><br />\r\n";

const char html_settingsMFAuto1[] PROGMEM =
L_MERIDIAN_FLIP_AUTO ":<br />";
const char html_settingsMFAuto2[] PROGMEM =
"<button id='mfa_on' class='btns_right' onpointerdown=\"s('ma','on')\"  type='button' disabled>" L_ON "</button>"
"<button id='mfa_off' class='btns_left' onpointerdown=\"s('ma','off')\" type='button' disabled>" L_OFF "</button>&nbsp;&nbsp;"
"<button name='ma' value='now' type='submit'>" L_MERIDIAN_FLIP_NOW "</button>"
"<br /><br />\r\n";

const char html_settingsMFPause1[] PROGMEM =
L_MERIDIAN_FLIP_PAUSE ": <br />";
const char html_settingsMFPause2[] PROGMEM =
"<button id='mfp_on' class='btns_right' onpointerdown=\"s('mp','on')\"  type='button' disabled>" L_ON "</button>"
"<button id='mfp_off' class='btns_left' onpointerdown=\"s('mp','off')\" type='button' disabled>" L_OFF "</button>"
"<br /><br />\r\n";

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
  data += FPSTR(html_main_css_btns1);
  sendHtml(data);
  data += FPSTR(html_main_css_btns2);
  data += FPSTR(html_main_css_btns3);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);

  data += FPSTR(html_bodyB);
  sendHtml(data);
  
  // active ajax page is: settingsAjax();
  data += "<script>var ajaxPage='settings.txt';</script>\n";
  data += FPSTR(html_ajax_active);
  data += "<script>auto2Rate=2;</script>";
  data += FPSTR(html_settingsScript);

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

  // Slew speed
  data += FPSTR(html_settingsSlewSpeed1);
  data += FPSTR(html_settingsSlewSpeed2);

  sendHtml(data);

  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_settingsTrackComp1);
    data += FPSTR(html_settingsTrackComp2);
    data += FPSTR(html_settingsTrackComp3);
  }

  data += FPSTR(html_settingsTrack1);
  data += FPSTR(html_settingsTrack2);
  
  data += FPSTR(html_settingsPark1);
    
  sendHtml(data);

  data += FPSTR(html_settingsBuzzer1);
  data += FPSTR(html_settingsBuzzer2);

  if (mountStatus.mountType()==MT_GEM) {
    data += FPSTR(html_settingsMFAuto1);
    data += FPSTR(html_settingsMFAuto2);
    data += FPSTR(html_settingsMFPause1);
    data += FPSTR(html_settingsMFPause2);
  }

  data += FPSTR(html_settingsEnd);

  data += "<br />";
  data += "</div></div></body></html>";
  
  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void settingsAjaxGet(EthernetClient *client) {
#else
void settingsAjaxGet() {
#endif
  processSettingsGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

#ifdef OETHS
void settingsAjax(EthernetClient *client) {
#else
void settingsAjax() {
#endif
  String data="";
  mountStatus.update();
  if (mountStatus.valid()) {
    data += "bzr_on|";  if (mountStatus.buzzerEnabled()) data+="disabled"; else data+="enabled"; data+="\n";
    data += "bzr_off|"; if (mountStatus.buzzerEnabled()) data+="enabled"; else data+="disabled"; data+="\n";
    if (mountStatus.mountType() == MT_GEM) {
      data += "mfa_on|";  if (mountStatus.autoMeridianFlips()) data+="disabled"; else data+="enabled"; data+="\n";
      data += "mfa_off|"; if (mountStatus.autoMeridianFlips()) data+="enabled"; else data+="disabled"; data+="\n";
      data += "mfp_on|";  if (mountStatus.pauseAtHome()) data+="disabled"; else data+="enabled"; data+="\n";
      data += "mfp_off|"; if (mountStatus.pauseAtHome()) data+="enabled"; else data+="disabled"; data+="\n";
    }
    if (mountStatus.mountType() != MT_ALTAZM) {
      // RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH
      if (mountStatus.rateCompensation() == RC_NONE) {
        data += "ot_on|";  data+="enabled";  data+="\n";
        data += "ot_ref|"; data+="enabled";  data+="\n";
        data += "ot_off|"; data+="disabled"; data+="\n";
        data += "ot_dul|"; data+="disabled"; data+="\n";
        data += "ot_sgl|"; data+="disabled"; data+="\n";
      } else
      if (mountStatus.rateCompensation() == RC_REFR_RA) {
        data += "ot_on|";  data+="enabled";  data+="\n";
        data += "ot_ref|"; data+="disabled"; data+="\n";
        data += "ot_off|"; data+="enabled";  data+="\n";
        data += "ot_dul|"; data+="enabled";  data+="\n";
        data += "ot_sgl|"; data+="disabled"; data+="\n";
      } else
      if (mountStatus.rateCompensation() == RC_REFR_BOTH) {
        data += "ot_on|";  data+="enabled";  data+="\n";
        data += "ot_ref|"; data+="disabled"; data+="\n";
        data += "ot_off|"; data+="enabled";  data+="\n";
        data += "ot_dul|"; data+="disabled"; data+="\n";
        data += "ot_sgl|"; data+="enabled";  data+="\n";
      } else
      if (mountStatus.rateCompensation() == RC_FULL_RA) {
        data += "ot_on|";  data+="disabled"; data+="\n";
        data += "ot_ref|"; data+="enabled";  data+="\n";
        data += "ot_off|"; data+="enabled";  data+="\n";
        data += "ot_dul|"; data+="enabled";  data+="\n";
        data += "ot_sgl|"; data+="disabled"; data+="\n";
      } else
      if (mountStatus.rateCompensation() == RC_FULL_BOTH) {
        data += "ot_on|";  data+="disabled"; data+="\n";
        data += "ot_ref|"; data+="enabled";  data+="\n";
        data += "ot_off|"; data+="enabled";  data+="\n";
        data += "ot_dul|"; data+="disabled"; data+="\n";
        data += "ot_sgl|"; data+="enabled";  data+="\n";
      }
    }
  } else {
    data += "trk_on|";  data+="disabled"; data+="\n";
    data += "trk_off|"; data+="disabled"; data+="\n";
    data += "bzr_on|";  data+="disabled"; data+="\n";
    data += "bzr_off|"; data+="disabled"; data+="\n";
    data += "mfa_on|";  data+="disabled"; data+="\n";
    data += "mfa_off|"; data+="disabled"; data+="\n";
    data += "mfp_on|";  data+="disabled"; data+="\n";
    data += "mfp_off|"; data+="disabled"; data+="\n";
    data += "ot_on|";   data+="disabled"; data+="\n";
    data += "ot_ref|";  data+="disabled"; data+="\n";
    data += "ot_off|";  data+="disabled"; data+="\n";
    data += "ot_dul|";  data+="disabled"; data+="\n";
    data += "ot_sgl|";  data+="disabled"; data+="\n";
  }

  String temp = commandString(":GX92#");
  float nominalRate = temp.toFloat();
  temp = commandString(":GX93#");
  float currentRate = temp.toFloat();
  if (nominalRate > 0.001 && nominalRate < 180.0 && currentRate > 0.001 && currentRate < 180.0) {
    double rateRatio=currentRate/nominalRate;

    if (rateRatio > 1.75) {
      data += "sr_vf|"; data+="disabled"; data+="\n";
      data += "sr_f|";  data+="enabled";  data+="\n";
      data += "sr_n|";  data+="enabled";  data+="\n";
      data += "sr_s|";  data+="enabled";  data+="\n";
      data += "sr_vs|"; data+="enabled";  data+="\n";
    } else
    if (rateRatio > 1.25) {
      data += "sr_vf|"; data+="enabled";  data+="\n";
      data += "sr_f|";  data+="disabled"; data+="\n";
      data += "sr_n|";  data+="enabled";  data+="\n";
      data += "sr_s|";  data+="enabled";  data+="\n";
      data += "sr_vs|"; data+="enabled";  data+="\n";
    } else
    if (rateRatio > 0.875) {
      data += "sr_vf|"; data+="enabled";  data+="\n";
      data += "sr_f|";  data+="enabled";  data+="\n";
      data += "sr_n|";  data+="disabled"; data+="\n";
      data += "sr_s|";  data+="enabled";  data+="\n";
      data += "sr_vs|"; data+="enabled";  data+="\n";
    } else
    if (rateRatio > 0.625) {
      data += "sr_vf|"; data+="enabled";  data+="\n";
      data += "sr_f|";  data+="enabled";  data+="\n";
      data += "sr_n|";  data+="enabled";  data+="\n";
      data += "sr_s|";  data+="disabled"; data+="\n";
      data += "sr_vs|"; data+="enabled";  data+="\n";
    } else {
      data += "sr_vf|"; data+="enabled";  data+="\n";
      data += "sr_f|";  data+="enabled";  data+="\n";
      data += "sr_n|";  data+="enabled";  data+="\n";
      data += "sr_s|";  data+="enabled";  data+="\n";
      data += "sr_vs|"; data+="disabled"; data+="\n";
    }
  } else {
    data += "sr_vf|"; data+="disabled"; data+="\n";
    data += "sr_f|";  data+="disabled"; data+="\n";
    data += "sr_n|";  data+="disabled"; data+="\n";
    data += "sr_s|";  data+="disabled"; data+="\n";
    data += "sr_vs|"; data+="disabled"; data+="\n";
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

  // Slew Speed
  v=server.arg("ss");
  if (v!="") {
    if (v=="vs") commandBool(":SX93,5#"); // very slow, 0.5 x
    if (v=="s")  commandBool(":SX93,4#"); // slow,      0.75x
    if (v=="n")  commandBool(":SX93,3#"); // normal,    1.0 x
    if (v=="f")  commandBool(":SX93,2#"); // fast,      1.5 x
    if (v=="vf") commandBool(":SX93,1#"); // very fast, 2.0 x
  }

  // set-park
  v=server.arg("pk");
  if (v!="") {
    if (v=="s") commandBool(":hQ#");
  }
  // Tracking control
  v=server.arg("tk");
  if (v!="") {
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
