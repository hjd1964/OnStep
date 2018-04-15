// -----------------------------------------------------------------------------------
// Settings

const char html_settingsStart[] =
"<form method='get' action='/settings.htm'>";

const char html_settingsPark1[] =
"Park: <br />"
"<button name='pk' value='s' type='submit'>Set-Park</button>\r\n";

const char html_settingsTrack1[] = 
"</br></br>Tracking: <br />"
"<button name='tk' value='on' type='submit'>On</button>"
"<button name='tk' value='off' type='submit'>Off</button><br />";
const char html_settingsTrack2[] = 
"<button name='tk' value='f' type='submit'>+ (0.1Hz faster)</button>"
"<button name='tk' value='-' type='submit'>- (0.1Hz slower)</button>"
"<button name='tk' value='r' type='submit'>Reset (default)</button><br />";
const char html_settingsTrack3[] = 
"<button name='tk' value='s' type='submit'>Sidereal</button>"
"<button name='tk' value='l' type='submit'>Lunar</button>"
"<button name='tk' value='h' type='submit'>Solar</button>\r\n";

const char html_settingsTrackComp1[] = 
"</br></br>Compensated Tracking Rate (Pointing Model/Refraction): </br>"
"<button name='rr' value='otk' type='submit'>Full</button>"
"<button name='rr' value='on' type='submit'>Refraction Only</button>"
"<button name='rr' value='off' type='submit'>Off</button>";
const char html_settingsTrackComp2[] =
"</br>"
"<button name='rr' value='don' type='submit'>Dual Axis On</button>"
"<button name='rr' value='doff' type='submit'>Dual Axis Off</button>\r\n";

const char html_settingsBuzzer1[] =
"<br /><br />Goto Alert, Buzzer: <br />"
"<button name='ab' value='on' type='submit'>On</button>";
const char html_settingsBuzzer2[] =
"<button name='ab' value='off' type='submit'>Off</button>\r\n";

const char html_settingsMFAuto[] = 
"</br></br>Automatic Meridian Flip (at Limit):<br />"
"<button name='ma' value='on' type='submit'>On</button>"
"<button name='ma' value='off' type='submit'>Off</button>";
const char html_settingsMFPause1[] = 
"</br></br>Meridian Flip, Pause at Home: <br />"
"<button name='mp' value='on' type='submit'>On</button>";
const char html_settingsMFPause2[] =
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

  char temp2[80]="";

  processSettingsGet();
  Ser.setTimeout(WebTimeout);
 
  String data=html_headB;
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_cssE;
  data += html_headE;
#ifdef OETHS
  client->print(data); data="";
#endif

  data += html_bodyB;

  // get status
  char stat[20] = "";
  Ser.print(":GU#");
  stat[Ser.readBytesUntil('#',stat,20)]=0;

  // finish the standard http response header
  data += html_onstep_header1;
  Ser.print(":GVP#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; } data += temp2;
  data += html_onstep_header2;
  Ser.print(":GVN#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } data += temp2;
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
  data += html_links4S;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
#ifdef OETHS
  client->print(data); data="";
#endif
  
  data+="<div style='width: 35em;'>";

  data += html_settingsStart;

  data += html_settingsPark1;
  
  data += html_settingsTrack1;
  data += html_settingsTrack2;
  data += html_settingsTrack3;
  if (!strstr(stat,"A")) {  // not AltAzm
    data += html_settingsTrackComp1;
    data += html_settingsTrackComp2;
  }
  data += html_settingsBuzzer1;
  data += html_settingsBuzzer2;
  if (strstr(stat,"E")) {  // GEM only
    data += html_settingsMFAuto;
    data += html_settingsMFPause1;
    data += html_settingsMFPause2;
  }

  data += html_settingsEnd;

  data += "<br />";
  data += "</div></div></body></html>";
  
#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/html", data);
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
    if (v=="s") Ser.print(":TQ#"); // sidereal
    if (v=="l") Ser.print(":TL#"); // lunar
    if (v=="h") Ser.print(":TS#"); // solar
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

