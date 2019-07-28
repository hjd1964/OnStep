// -----------------------------------------------------------------------------------
// Settings

const char html_settingsStart[] PROGMEM =
"<form method='get' action='/settings.htm'>";

const char html_settingsRefine1[] PROGMEM =
"Refine Polar Alignment: <br />"
"<button name='rp' value='a' type='submit'>Refine PA</button><br />\r\n"
"Setup &amp; 3+ Star Align mount.  Goto bright star near NCP or SCP with Dec in 50 to 80&deg; range (N or S.) "
"Press [Refine PA] button.  Use mount's PA adjust controls to center the star again."
"Optionally align the mount again.</br><br />";

const char html_settingsPark1[] PROGMEM =
"Park: <br />"
"<button name='pk' value='s' type='submit'>Set-Park</button>\r\n";

const char html_settingsTrack1[] PROGMEM =
"</br></br>Tracking (<span id='tracking'>";
const char html_settingsTrack2[] = 
"</span>): <br />"
"<button name='tk' value='on' type='submit'>On</button>"
"<button name='tk' value='off' type='submit'>Off</button><br />";
const char html_settingsTrack3[] PROGMEM =
"<button name='tk' value='f' type='submit'>+ (0.02Hz faster)</button>"
"<button name='tk' value='-' type='submit'>- (0.02Hz slower)</button>"
"<button name='tk' value='r' type='submit'>Reset (default)</button>";

const char html_settingsTrackComp1[] PROGMEM =
"</br></br>Compensated Tracking Rate (Pointing Model/Refraction): </br>"
"<button name='rr' value='otk' type='submit'>Full</button>"
"<button name='rr' value='on' type='submit'>Refraction Only</button>"
"<button name='rr' value='off' type='submit'>Off</button>";
const char html_settingsTrackComp2[] PROGMEM =
"</br>"
"<button name='rr' value='don' type='submit'>Dual Axis</button>"
"<button name='rr' value='doff' type='submit'>Single Axis</button>\r\n";

const char html_settingsBuzzer1[] PROGMEM =
"<br /><br />Goto Alert, Buzzer (<span id='buzzer'>";
const char html_settingsBuzzer2[] PROGMEM =
"</span>): <br />"
"<button name='ab' value='on' type='submit'>On</button>"
"<button name='ab' value='off' type='submit'>Off</button>\r\n";

const char html_settingsMFAuto1[] PROGMEM =
"</br></br>Automatic Meridian Flip at Limit (<span id='autoFlip'>";
const char html_settingsMFAuto2[] PROGMEM =
"</span>):<br />"
"<button name='ma' value='now' type='submit'>Now</button>&nbsp;&nbsp;"
"<button name='ma' value='on' type='submit'>On</button>"
"<button name='ma' value='off' type='submit'>Off</button>";
const char html_settingsMFPause1[] PROGMEM =
"</br></br>Meridian Flip, Pause at Home (<span id='pause'>";
const char html_settingsMFPause2[] PROGMEM =
"</span>): <br />"
"<button name='mp' value='on' type='submit'>On</button>"
"<button name='mp' value='off' type='submit'>Off</button>\r\n";

const char html_settingsEnd[] PROGMEM =
"</form>\r\n";

#ifdef OETHS
void handleSettings(EthernetClient *client) {
#else
void handleSettings() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();

  char temp1[80]="";

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

  mountStatus.update(true);

  // finish the standard http response header
  data += FPSTR(html_onstep_header1);
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_links1N);
  data += FPSTR(html_links2N);
  data += FPSTR(html_links3N);
  sendHtml(data);
#ifdef ENCODERS_ON
  data += FPSTR(html_linksEncN);
#endif
  data += FPSTR(html_links4S);
  data += FPSTR(html_links5N);
#ifndef OETHS
  data += FPSTR(html_links6N);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);
  
  data+="<div style='width: 35em;'>";

  data += FPSTR(html_settingsStart);

  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_settingsRefine1);
  }

  data += FPSTR(html_settingsPark1);
  
  data += FPSTR(html_settingsTrack1);
  if (mountStatus.valid()) { if (mountStatus.tracking()) data+="On"; else data+="Off"; } else data+="?";
  data += FPSTR(html_settingsTrack2);
  data += FPSTR(html_settingsTrack3);
  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += FPSTR(html_settingsTrackComp1);
    data += FPSTR(html_settingsTrackComp2);
  }
  sendHtml(data);
  data += FPSTR(html_settingsBuzzer1);
  if (mountStatus.valid()) { if (mountStatus.buzzerEnabled()) data+="On"; else data+="Off"; } else data+="?";
  data += FPSTR(html_settingsBuzzer2);

  if (mountStatus.mountType()==MT_GEM) {
    data += FPSTR(html_settingsMFAuto1);
    if (mountStatus.valid()) { if (mountStatus.autoMeridianFlips()) data+="On"; else data+="Off"; } else data+="?";
    data += FPSTR(html_settingsMFAuto2);
    data += FPSTR(html_settingsMFPause1);
    if (mountStatus.valid()) { if (mountStatus.pauseAtHome()) data+="On"; else data+="Off"; } else data+="?";
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
  data += "tracking|"; if (mountStatus.valid()) { if (mountStatus.tracking()) data+="On"; else data+="Off"; } else data+="?"; data+="\n";
  data += "buzzer|"; if (mountStatus.valid()) { if (mountStatus.buzzerEnabled()) data+="On"; else data+="Off"; } else data+="?"; data+="\n";
  if (mountStatus.mountType()==MT_GEM) {
    data += "autoFlip|";  if (mountStatus.valid()) { if (mountStatus.autoMeridianFlips()) data+="On"; else data+="Off"; } else data+="?"; data+="\n";
    data += "pause|";  if (mountStatus.valid()) { if (mountStatus.pauseAtHome()) data+="On"; else data+="Off"; } else data+="?"; data+="\n";
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
  char temp[20]="";

  // refine polar align
  v=server.arg("rp");
  if (v!="") {
    if (v=="a") Ser.print(":MP#");
  }
  // set-park
  v=server.arg("pk");
  if (v!="") {
    if (v=="s") Ser.print(":hQ#");
    Ser.setTimeout(WebTimeout*4);
  }
  // Tracking control
  v=server.arg("tk");
  if (v!="") {
    if (v=="on") Ser.print(":Te#");
    if (v=="off") Ser.print(":Td#");
    if (v=="f") Ser.print(":T+#"); // 0.02hz faster
    if (v=="-") Ser.print(":T-#"); // 0.02hz slower
    if (v=="r") Ser.print(":TR#"); // reset
  }
  // Refraction Rate Tracking control
  v=server.arg("rr");
  if (v!="") {
    if (v=="otk") Ser.print(":To#");
    if (v=="on") Ser.print(":Tr#");
    if (v=="off") Ser.print(":Tn#");
    if (v=="don") Ser.print(":T2#");
    if (v=="doff") Ser.print(":T1#");
  }
  // Alert buzzer
  v=server.arg("ab");
  if (v!="") {
    if (v=="on") Ser.print(":SX97,1#");
    if (v=="off") Ser.print(":SX97,0#");
  }
  // Auto-continue
  v=server.arg("ma");
  if (v!="") {
    if (v=="now") Ser.print(":MN#");
    if (v=="on") Ser.print(":SX95,1#");
    if (v=="off") Ser.print(":SX95,0#");
  }
  // Pause at meridian flip
  v=server.arg("mp");
  if (v!="") {
    if (v=="on") Ser.print(":SX98,1#");
    if (v=="off") Ser.print(":SX98,0#");
  }

  // clear any possible response
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}
