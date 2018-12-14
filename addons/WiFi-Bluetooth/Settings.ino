// -----------------------------------------------------------------------------------
// Settings

const char html_settingsStart[] =
"<form method='get' action='/settings.htm'>";

const char html_settingsPark1[] =
"Park: <br />"
"<button name='pk' value='s' type='submit'>Set-Park</button>\r\n";

const char html_settingsTrack1[] = 
"</br></br>Tracking (<span id='tracking'>";
const char html_settingsTrack2[] = 
"</span>): <br />"
"<button name='tk' value='on' type='submit'>On</button>"
"<button name='tk' value='off' type='submit'>Off</button><br />";
const char html_settingsTrack3[] = 
"<button name='tk' value='f' type='submit'>+ (0.1Hz faster)</button>"
"<button name='tk' value='-' type='submit'>- (0.1Hz slower)</button>"
"<button name='tk' value='r' type='submit'>Reset (default)</button>";

const char html_settingsTrackComp1[] = 
"</br></br>Compensated Tracking Rate (Pointing Model/Refraction): </br>"
"<button name='rr' value='otk' type='submit'>Full</button>"
"<button name='rr' value='on' type='submit'>Refraction Only</button>"
"<button name='rr' value='off' type='submit'>Off</button>";
const char html_settingsTrackComp2[] =
"</br>"
"<button name='rr' value='don' type='submit'>Dual Axis</button>"
"<button name='rr' value='doff' type='submit'>Single Axis</button>\r\n";

const char html_settingsBuzzer1[] =
"<br /><br />Goto Alert, Buzzer (<span id='buzzer'>";
const char html_settingsBuzzer2[] =
"</span>): <br />"
"<button name='ab' value='on' type='submit'>On</button>"
"<button name='ab' value='off' type='submit'>Off</button>\r\n";

const char html_settingsMFAuto1[] = 
"</br></br>Automatic Meridian Flip at Limit (<span id='autoFlip'>";
const char html_settingsMFAuto2[] = 
"</span>):<br />"
"<button name='ma' value='now' type='submit'>Now</button>&nbsp;&nbsp;"
"<button name='ma' value='on' type='submit'>On</button>"
"<button name='ma' value='off' type='submit'>Off</button>";
const char html_settingsMFPause1[] = 
"</br></br>Meridian Flip, Pause at Home (<span id='pause'>";
const char html_settingsMFPause2[] = 
"</span>): <br />"
"<button name='mp' value='on' type='submit'>On</button>"
"<button name='mp' value='off' type='submit'>Off</button>\r\n";

const char html_settingsEnd[] =
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
 
  String data=html_headB;
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  sendHtml(data);
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_cssE;
  data += html_headE;
  data += html_bodyB;
  sendHtml(data);
  
  // active ajax page is: settingsAjax();
  data +="<script>var ajaxPage='settings.txt';</script>\n";
  data +=html_ajax_active;
  data +="<script>auto2Rate=2;</script>";

  mountStatus.update(true);

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
  sendHtml(data);
#ifdef ENCODERS_ON
  data += html_linksEncN;
#endif
  data += html_links4S;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  sendHtml(data);
  
  data+="<div style='width: 35em;'>";

  data += html_settingsStart;

  data += html_settingsPark1;
  
  data += html_settingsTrack1;
  if (mountStatus.valid()) { if (mountStatus.tracking()) data+="On"; else data+="Off"; } else data+="?";
  data += html_settingsTrack2;
  data += html_settingsTrack3;
  if (mountStatus.mountType()!=MT_ALTAZM) {
    data += html_settingsTrackComp1;
    data += html_settingsTrackComp2;
  }
  sendHtml(data);
  data += html_settingsBuzzer1;
  if (mountStatus.valid()) { if (mountStatus.buzzerEnabled()) data+="On"; else data+="Off"; } else data+="?";
  data += html_settingsBuzzer2;

  if (mountStatus.mountType()==MT_GEM) {
    data += html_settingsMFAuto1;
    if (mountStatus.valid()) { if (mountStatus.autoMeridianFlips()) data+="On"; else data+="Off"; } else data+="?";
    data += html_settingsMFAuto2;
    data += html_settingsMFPause1;
    if (mountStatus.valid()) { if (mountStatus.pauseAtHome()) data+="On"; else data+="Off"; } else data+="?";
    data += html_settingsMFPause2;
  }

  data += html_settingsEnd;

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
