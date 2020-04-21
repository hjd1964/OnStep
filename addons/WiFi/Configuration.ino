// -----------------------------------------------------------------------------------
// Configuration

const char html_config1[] PROGMEM =
L_MAX_SLEW_SPEED ": "
"<form method='get' action='/configuration.htm'>"
"<button name='ss' value='vs' type='submit'>" L_VSLOW "</button>"
"<button name='ss' value='s' type='submit'>" L_SLOW "</button>";
const char html_configMaxRate[] PROGMEM =
"<button name='ss' value=\"n\" type='submit'>" L_NORMAL "</button>"
"<button name='ss' value=\"f\" type='submit'>" L_FAST "</button>"
"<button name='ss' value=\"vf\" type='submit'>" L_VFAST "</button>"
"</form><br /><br />\r\n";
const char html_configBlAxis1[] PROGMEM =
L_BACKLASH_TITLE ": <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='b1' min='0' max='3600'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_BACKLASH_RANGE_AXIS1 ")"
"</form>"
"\r\n";
const char html_configBlAxis2[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='b2' min='0' max='3600'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_BACKLASH_RANGE_AXIS2 ")"
"</form>"
"<br />\r\n";
const char html_configMinAlt[] PROGMEM =
L_LIMITS_TITLE ": <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='hl' min='-30' max='30'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LIMITS_RANGE_HORIZON ")"
"</form>"
"\r\n";
const char html_configMaxAlt[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='ol' min='60' max='90'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LIMITS_RANGE_OVERHEAD ")"
"</form>"
"\r\n";
const char html_configPastMerE[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='el' min='-180' max='180'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LIMITS_RANGE_MERIDIAN_E ")"
"</form>"
"\r\n";
const char html_configPastMerW[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='wl' min='-180' max='180'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LIMITS_RANGE_MERIDIAN_W ")"
"</form>"
"<br />\r\n";
const char html_configLongDeg[] PROGMEM =
L_LOCATION_TITLE ": <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='g1' min='-180' max='180'>&nbsp;&deg;&nbsp;";
const char html_configLongMin[] PROGMEM =
" <input value='%s' type='number' name='g2' min='0' max='60'>&nbsp;'&nbsp;&nbsp;"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LOCATION_LONG ")"
"</form>"
"\r\n";
const char html_configLatDeg[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='t1' min='-90' max='90'>&nbsp;&deg;&nbsp;";
const char html_configLatMin[] PROGMEM =
" <input value='%s' type='number' name='t2' min='0' max='60'>&nbsp;'&nbsp;&nbsp;"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LOCATION_LAT ")"
"</form>"
"\r\n";
const char html_configOffsetDeg[] PROGMEM =
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='u1' min='-12' max='14'>&nbsp;hrs&nbsp;";
const char html_configOffsetMin[] PROGMEM =
" <select name='u2'><option value='0' %s>00</option><option value='30' %s>30</option><option value='45' %s>45</option></select>&nbsp;min.&nbsp;"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_LOCATION_RANGE_UTC_OFFSET ")<br />"
L_LOCATION_MESSAGE_UTC_OFFSET "<br /><br />"
"</form>"
"\r\n";
#ifdef OETHS
void handleConfiguration(EthernetClient *client) {
#else
void handleConfiguration() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
  
  mountStatus.update();

  char temp[400]="";
  char temp1[120]="";
  char temp2[120]="";
  
  processConfigurationGet();

  sendHtmlStart();

  // send a standard http response header
  String data=FPSTR(html_headB);
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  sendHtml(data);
  data += FPSTR(html_main_css5);
  data += FPSTR(html_main_css6);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

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
  data += FPSTR(html_linksSetN);
  data += FPSTR(html_linksCfgS);
#ifndef OETHS
  data += FPSTR(html_linksWifiN);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data += FPSTR(html_config1);

  data+="<div style='width: 35em;'>";

  // Slew speed
  data += FPSTR(html_configMaxRate);
  sendHtml(data);

  // Backlash
  if (!command(":%BR#",temp1)) strcpy(temp1,"0"); int backlashAxis1=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configBlAxis1,backlashAxis1);
  data += temp;
  if (!command(":%BD#",temp1)) strcpy(temp1,"0"); int backlashAxis2=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configBlAxis2,backlashAxis2);
  data += temp;
  sendHtml(data);

  // Overhead and Horizon Limits
  if (!command(":Gh#",temp1)) strcpy(temp1,"0"); int minAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configMinAlt,minAlt);
  data += temp;
  if (!command(":Go#",temp1)) strcpy(temp1,"0"); int maxAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configMaxAlt,maxAlt);
  data += temp;

  // Meridian Limits
  if ((command(":GXE9#",temp1)) && (command(":GXEA#",temp2))) {
    int degPastMerE=(int)strtol(&temp1[0],NULL,10);
    degPastMerE=round((degPastMerE*15.0)/60.0);
    sprintf_P(temp,html_configPastMerE,degPastMerE);
    data += temp;
    int degPastMerW=(int)strtol(&temp2[0],NULL,10);
    degPastMerW=round((degPastMerW*15.0)/60.0);
    sprintf_P(temp,html_configPastMerW,degPastMerW);
    data += temp;
  } else data += "<br />\r\n";
  sendHtml(data);

  // Longitude
  if (!command(":Gg#",temp1)) strcpy(temp1,"+000*00");
  temp1[4]=0; // deg. part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  sprintf_P(temp,html_configLongDeg,temp1);
  data += temp;
  sprintf_P(temp,html_configLongMin,(char*)&temp1[5]);
  data += temp;
  sendHtml(data);

  // Latitude
  if (!command(":Gt#",temp1)) strcpy(temp1,"+00*00");
  temp1[3]=0; // deg. part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  sprintf_P(temp,html_configLatDeg,temp1);
  data += temp;
  sprintf_P(temp,html_configLatMin,(char*)&temp1[4]);
  data += temp;
  sendHtml(data);

  // UTC Offset
  if (!command(":GG#",temp1)) strcpy(temp1,"+00");
  strcpy(temp2,temp1);
  temp2[3]=0; // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  sprintf_P(temp,html_configOffsetDeg,temp2);
  data += temp;
  strcpy(temp2,temp1);
  if (temp2[3]==0) sprintf_P(temp,html_configOffsetMin,"selected","",""); else
  if (temp2[4]=='3') sprintf_P(temp,html_configOffsetMin,"","selected",""); else
  if (temp2[4]=='4') sprintf_P(temp,html_configOffsetMin,"","","selected");
  data += temp;
  sendHtml(data);

  strcpy(temp,"</div></div></body></html>");
  data += temp;

  sendHtml(data);
  sendHtmlDone(data);
}

void processConfigurationGet() {
  String v;
  int i;
  char temp[20]="";

  // Slew Speed
  v=server.arg("ss");
  if (v!="") {
    if (v=="vs") commandBool(":SX93,5#"); // very slow, 0.5 x
    if (v=="s")  commandBool(":SX93,4#"); // slow,      0.75x
    if (v=="n")  commandBool(":SX93,3#"); // normal,    1.0 x
    if (v=="f")  commandBool(":SX93,2#"); // fast,      1.5 x
    if (v=="vf") commandBool(":SX93,1#"); // very fast, 2.0 x
  }
  
  // Overhead and Horizon Limits
  v=server.arg("ol");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= 60 && i <= 90)) { 
      sprintf(temp,":So%d#",i);
      commandBool(temp);
    }
  }
  v=server.arg("hl");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -30 && i <= 30)) { 
      sprintf(temp,":Sh%d#",i);
      commandBool(temp);
    }
  }

  // Meridian Limits
  v=server.arg("el");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -180 && i <= 180)) { 
      i=round((i*60.0)/15.0);
      sprintf(temp,":SXE9,%d#",i);
      commandBool(temp);
    }
  }
  v=server.arg("wl");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -180 && i <= 180)) { 
      i=round((i*60.0)/15.0);
      sprintf(temp,":SXEA,%d#",i);
      commandBool(temp);
    }
  }

  // Backlash Limits
  v=server.arg("b1");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= 0 && i <= 3600)) { 
      sprintf(temp,":$BR%d#",i);
      commandBool(temp);
    }
  }
  v=server.arg("b2");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= 0 && i <= 3600)) { 
      sprintf(temp,":$BD%d#",i);
      commandBool(temp);
    }
  }

  // Location
  int long_deg=-999;
  v=server.arg("g1");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -180 && i <= 180)) long_deg = i;
  }
  v=server.arg("g2");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= 0 && i <= 60)) { 
      if (long_deg >= -180 && long_deg <= 180) {
        sprintf(temp,":Sg%+04d*%02d#",long_deg,i);
        commandBool(temp);
      }
    }
  }
  int lat_deg=-999;
  v=server.arg("t1");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -90 && i <= 90)) lat_deg = i;
  }
  v=server.arg("t2");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= 0 && i <= 60)) {
      if (lat_deg >= -90 && lat_deg <= 90) {
        sprintf(temp,":St%+03d*%02d#",lat_deg,i);
        commandBool(temp);
      }
    }
  }
  int ut_hrs=-999;
  v=server.arg("u1");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i >= -13 && i <= 13)) ut_hrs = i;
  }
  v=server.arg("u2");
  if (v!="") {
    if (atoi2((char*)v.c_str(),&i) && (i == 0 || i == 30 || i == 45)) {
      if (ut_hrs >= -13 && ut_hrs <= 13) {
        sprintf(temp,":SG%+03d:%02d#",ut_hrs,i);
        commandBool(temp);
      }
    }
  }
}
