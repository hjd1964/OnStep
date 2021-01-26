// -----------------------------------------------------------------------------------
// Configuration

const char html_configScript1[] PROGMEM =
"<script>\n"
"function s(key,v1) {"
  "var xhttp = new XMLHttpRequest();"
  "xhttp.open('GET', 'configurationA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);"
  "xhttp.send();"
"}</script>\n";

// Misc
const char html_configFormBegin[] PROGMEM = "<div class='content'><br />\r\n<form method='get' action='/configuration.htm'>";
const char html_configFormEnd[] PROGMEM = "\r\n</form><br />\r\n</div><br />\r\n";

// Site
const char html_configLongDeg[] PROGMEM =
"<input style='width: 4em;' value='%s' type='number' name='g1' min='-180' max='180'>&nbsp;&deg;&nbsp;";
const char html_configLongMin[] PROGMEM =
"<input style='width: 3.5em;' value='%s' type='number' name='g2' min='0' max='60'>&nbsp;'&nbsp;";
const char html_configLongSec[] PROGMEM =
"<input style='width: 3.5em;' value='%s' type='number' name='g3' min='0' max='60'>&nbsp;\"&nbsp;&nbsp; &nbsp;" L_LOCATION_LONG "<br />\r\n";
const char html_configLatDeg[] PROGMEM =
"<input style='width: 4em;' value='%s' type='number' name='t1' min='-90' max='90'>&nbsp;&deg;&nbsp;";
const char html_configLatMin[] PROGMEM =
"<input style='width: 3.5em;' value='%s' type='number' name='t2' min='0' max='60'>&nbsp;'&nbsp;";
const char html_configLatSec[] PROGMEM =
"<input style='width: 3.5em;' value='%s' type='number' name='t3' min='0' max='60'>&nbsp;\"&nbsp;&nbsp; &nbsp;" L_LOCATION_LAT "<br />\r\n";
const char html_configOffsetHrs[] PROGMEM =
"<input style='width: 4em;' value='%s' type='number' name='u1' min='-14' max='12'>&nbsp;h&nbsp;";
const char html_configOffsetMin[] PROGMEM =
"<select name='u2'><option value='0' %s>00</option><option value='30' %s>30</option><option value='45' %s>45</option></select>&nbsp;m&nbsp;"
"&nbsp;&nbsp;" L_LOCATION_RANGE_UTC_OFFSET "<br />" L_LOCATION_MESSAGE_UTC_OFFSET "<br /><br />";

// Overhead and Horizon limits
const char html_configMinAlt[] PROGMEM =
"<input value='%d' type='number' name='hl' min='-30' max='30'>&nbsp;" L_LIMITS_RANGE_HORIZON "<br />\r\n";
const char html_configMaxAlt[] PROGMEM =
"<input value='%d' type='number' name='ol' min='60' max='90'>&nbsp;" L_LIMITS_RANGE_OVERHEAD "<br /><br />";

// Axis1
const char html_configBlAxis1[] PROGMEM =
"<input value='%d' type='number' name='b1' min='0' max='3600'>&nbsp;" L_BACKLASH_RANGE_AXIS1 "<br />\r\n";
const char html_configPastMerE[] PROGMEM =
"<input value='%d' type='number' name='el' min='-270' max='270'>&nbsp;" L_LIMITS_RANGE_MERIDIAN_E "<br />\r\n";
const char html_configPastMerW[] PROGMEM =
"<input value='%d' type='number' name='wl' min='-270' max='270'>&nbsp;" L_LIMITS_RANGE_MERIDIAN_W "<br /><br />";

// Axis2
const char html_configBlAxis2[] PROGMEM =
"<input value='%d' type='number' name='b2' min='0' max='3600'>&nbsp;" L_BACKLASH_RANGE_AXIS2 "<br /><br />";

// Axis3
const char html_configBlAxis3[] PROGMEM =
"<input value='%d' type='number' name='b3' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS345 "<br /><br />";

// Axis4
const char html_configBlAxis4[] PROGMEM =
"<input value='%d' type='number' name='b4' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS345 "<br /><br />";
const char html_configTcfEnAxis4[] PROGMEM =
"<input value='%d' type='number' name='en4' min='0' max='1' step='1'>&nbsp;" L_TCF_COEF_EN_AXIS45 "<br />";
const char html_configDbAxis4[] PROGMEM =
"<input value='%d' type='number' name='d4' min='1' max='32767'>&nbsp;" L_DEADBAND_RANGE_AXIS45 "<br />";
const char html_configTcfCoefAxis4[] PROGMEM =
"<input value='%s' type='number' name='tc4' min='-999.0' max='999.0' step='any'>&nbsp;" L_TCF_COEF_RANGE_AXIS45 "<br /><br />";

// Axis5
const char html_configBlAxis5[] PROGMEM =
"<input value='%d' type='number' name='b5' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS345 "<br /><br />";
const char html_configTcfEnAxis5[] PROGMEM =
"<input value='%d' type='number' name='en5' min='0' max='1' step='1'>&nbsp;" L_TCF_COEF_EN_AXIS45 "<br />";
const char html_configDbAxis5[] PROGMEM =
"<input value='%d' type='number' name='d5' min='1' max='32767'>&nbsp;" L_DEADBAND_RANGE_AXIS45 "<br />";
const char html_configTcfCoefAxis5[] PROGMEM =
"<input value='%s' type='number' name='tc5' min='-999.0' max='999.0' step='any'>&nbsp;" L_TCF_COEF_RANGE_AXIS45 "<br /><br />";

const char html_configAdvanced[] PROGMEM =
"<hr>" L_ADV_SET_TITLE " (<i>" L_ADV_SET_HEADER_MSG "</i>)<br /><br />";
const char html_configMountType[] PROGMEM =
" <input style='width: 7em;' value='%d' type='number' name='mountt' min='1' max='3' step='1'>&nbsp; " L_ADV_MOUNT_TYPE "<br /><br />\r\n";

// Reset
const char html_resetNotes[] PROGMEM =
"<br />Notes:<ul>"
"<li>" L_RESET_MSG1 "</li>"
"<li>" L_DOWN_MESSAGE1 "</li>"
"</ul>";

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
  
  bool success=processConfigurationGet();

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
  data += FPSTR(html_main_css_collapse1);
  data += FPSTR(html_main_css_collapse2);
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
  if (!mountStatus.valid() || !success) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }
  
  // ajax scripts
  data += FPSTR(html_configScript1);
  
  data+="<div style='width: 35em;'>";
  data += L_BASIC_SET_TITLE "<br /><br />";
  sendHtml(data);

  data += "<button type='button' class='collapsible'>" L_LOCATION_TITLE "</button>";
  data += FPSTR(html_configFormBegin);
  // Longitude
  if (!command(":GgH#",temp1)) strcpy(temp1,"+000*00:00"); temp1[10]=0;
  temp1[4]=0;                      // deg part only
  temp1[7]=0;                      // min part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  stripNum(temp1);
  while (temp1[0] == '0' && strlen(temp1) > 1) memmove(&temp1[0],&temp1[1],strlen(temp1)); // remove leading 0's
  sprintf_P(temp,html_configLongDeg,temp1);
  data += temp;
  sprintf_P(temp,html_configLongMin,(char*)&temp1[5]);
  data += temp;
  sprintf_P(temp,html_configLongSec,(char*)&temp1[8]);
  data += temp;
  sendHtml(data);

  // Latitude
  if (!command(":GtH#",temp1)) strcpy(temp1,"+00*00:00"); temp1[9]=0;
  temp1[3]=0;                      // deg part only
  temp1[6]=0;                      // min part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  stripNum(temp1);
  sprintf_P(temp,html_configLatDeg,temp1);
  data += temp;
  sprintf_P(temp,html_configLatMin,(char*)&temp1[4]);
  data += temp;
  sprintf_P(temp,html_configLatSec,(char*)&temp1[7]);
  data += temp;
  sendHtml(data);

  // UTC Offset
  if (!command(":GG#",temp1)) strcpy(temp1,"+00");
  strcpy(temp2,temp1);
  temp2[3]=0;                      // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  stripNum(temp2);
  sprintf_P(temp,html_configOffsetHrs,temp2);
  data += temp;
  strcpy(temp2,temp1);
  if (temp2[3]==0) sprintf_P(temp,html_configOffsetMin,"selected","",""); else
  if (temp2[4]=='3') sprintf_P(temp,html_configOffsetMin,"","selected",""); else
  if (temp2[4]=='4') sprintf_P(temp,html_configOffsetMin,"","","selected");
  data += temp;
  data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
  data += FPSTR(html_configFormEnd);
  sendHtml(data);

  // Overhead and Horizon Limits
  data += "<button type='button' class='collapsible'>" L_LIMITS_TITLE "</button>";
  data += FPSTR(html_configFormBegin);
  if (!command(":Gh#",temp1)) strcpy(temp1,"0"); int minAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configMinAlt,minAlt);
  data += temp;
  if (!command(":Go#",temp1)) strcpy(temp1,"0"); int maxAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configMaxAlt,maxAlt);
  data += temp;
  data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
  data += FPSTR(html_configFormEnd);
  sendHtml(data);

  // Axis1 RA/Azm
  data += "<br /><button type='button' class='collapsible'>Axis1 RA/Azm</button>";
  data += FPSTR(html_configFormBegin);
  // Backlash
  if (!command(":%BR#",temp1)) strcpy(temp1,"0"); int backlashAxis1=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configBlAxis1,backlashAxis1);
  data += temp;
  sendHtml(data);
  // Meridian Limits
  if (mountStatus.mountType() == MT_GEM && (command(":GXE9#",temp1)) && (command(":GXEA#",temp2))) {
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
  data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
  data += FPSTR(html_configFormEnd);
  sendHtml(data);

  // Axis2 Dec/Alt
  data += "<button type='button' class='collapsible'>Axis2 Dec/Alt</button>";
  data += FPSTR(html_configFormBegin);
  // Backlash
  if (!command(":%BD#",temp1)) strcpy(temp1,"0"); int backlashAxis2=(int)strtol(&temp1[0],NULL,10);
  sprintf_P(temp,html_configBlAxis2,backlashAxis2);
  data += temp;
  data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
  data += FPSTR(html_configFormEnd);
  sendHtml(data);

  // Axis3 Rotator
  int i = 0;
  static int rotatorPresent = -1;
  if (rotatorPresent == -1) { command(":rT#",temp1); if (temp1[0] == '0') rotatorPresent=false; else rotatorPresent=true; }
  
  if (rotatorPresent) {
    data += "<button type='button' class='collapsible'>Axis3 " L_ROTATOR "</button>";
    data += FPSTR(html_configFormBegin);
    // Backlash
    if (!command(":rb#",temp1)) strcpy(temp1,"0"); i=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,html_configBlAxis3,i);
    data += temp;
    data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
    data += FPSTR(html_configFormEnd);
    sendHtml(data);
  }

  // Axis4 Focuser1
  static int focuser1Present = -1;
  if (focuser1Present == -1) { command(":FT#",temp1); if (temp1[0] == '0') focuser1Present=false; else focuser1Present=true; }
  if (focuser1Present) {
    commandBool(":FA1#");

    data += "<button type='button' class='collapsible'>Axis4 " L_FOCUSER " 1</button>";
    data += FPSTR(html_configFormBegin);
    // Backlash
    if (!command(":Fb#",temp1)) strcpy(temp1,"0"); i=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,html_configBlAxis4,i);
    data += temp;
    // TCF Enable
    if (commandBool(":Fc#")) sprintf_P(temp,html_configTcfEnAxis4,1); else sprintf_P(temp,html_configTcfEnAxis4,0);
    data += temp;
    // TCF Deadband
    if (!command(":Fd#",temp1)) strcpy(temp1,"0"); i=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,html_configDbAxis4,i);
    data += temp;
    sendHtml(data);
    // TCF Coef
    if (!command(":FC#",temp1)) strcpy(temp1,"0");
    char *conv_end;
    double f=strtod(temp1,&conv_end); if (&temp1[0] == conv_end) f=0.0;
    dtostrf(f,1,5,temp1); stripNum(temp1);
    sprintf_P(temp,html_configTcfCoefAxis4,temp1);
    data += temp;
    data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
    data += FPSTR(html_configFormEnd);
    sendHtml(data);
  }

  // Axis5 Focuser2
  static int focuser2Present = -1;
  if (focuser2Present == -1) { command(":fT#",temp1); if (temp1[0] == '0') focuser2Present=false; else focuser2Present=true; }
  if (focuser2Present) {
    data += "<button type='button' class='collapsible'>Axis5 " L_FOCUSER " 2</button>";
    data += FPSTR(html_configFormBegin);
    // Backlash
    if (!command(":fb#",temp1)) strcpy(temp1,"0"); i=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,html_configBlAxis5,i);
    data += temp;
    // TCF Enable
    if (commandBool(":Fc#")) sprintf_P(temp,html_configTcfEnAxis5,1); else sprintf_P(temp,html_configTcfEnAxis5,0);
    data += temp;
    // TCF Deadband
    if (!command(":fd#",temp1)) strcpy(temp1,"0"); i=(int)strtol(&temp1[0],NULL,10);
    sprintf_P(temp,html_configDbAxis5,i);
    data += temp;
    sendHtml(data);
    // TCF Coef
    if (!command(":fC#",temp1)) strcpy(temp1,"0");
    char *conv_end;
    double f=strtod(temp1,&conv_end); if (&temp1[0] == conv_end) f=0.0;
    dtostrf(f,1,5,temp1); stripNum(temp1);
    sprintf_P(temp,html_configTcfCoefAxis5,temp1);
    data += temp;
    data += "<button type='submit'>" L_UPLOAD "</button>\r\n";
    data += FPSTR(html_configFormEnd);
    sendHtml(data);
  }
  
  data += "<br />\r\n";

  int numShown = 0;
  
  // Mount type
  if (!command(":GXEM#",temp1)) strcpy(temp1,"0");
  int mt=atoi(temp1);
  if (mt != 0) data += FPSTR(html_configAdvanced);
  if (mt >= 1 && mt <= 3) {
    data += "<button type='button' class='collapsible'>Mount Type</button>";
    data += FPSTR(html_configFormBegin);
    sprintf_P(temp,html_configMountType,mt); data += temp;
    data += "<button type='submit'>" L_UPLOAD "</button> ";
    data += "<button name='revert' value='0' type='submit'>" L_REVERT "</button>\r\n";
    data += FPSTR(html_configFormEnd);
    data += "<br />";
    numShown++;
  }

#if DISPLAY_RESET_CONTROLS != OFF
  sendHtml(data);
  data += "<hr>" L_RESET_TITLE "<br/><br/>";
  data += "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('advanced','reset')\" type='button'>" L_RESET "!</button>";
  #ifdef BOOT0_PIN
    data += " &nbsp;&nbsp;<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('advanced','fwu')\" type='button'>" L_RESET_FWU "!</button>";
  #endif
  data += "<br/>\r\n";
  data += FPSTR(html_resetNotes);
#endif

  // collapsible script
  data += FPSTR(html_collapseScript);

  data += "<br />";
  
  strcpy(temp,"</div></div></body></html>");
  data += temp;

  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void configurationAjaxGet(EthernetClient *client) {
#else
void configurationAjaxGet() {
#endif
  processConfigurationGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

bool processConfigurationGet() {
  String v,v1,v2;
  char temp[20]="";

  // Overhead limit
  v=server.arg("ol");
  if (v!="") {
    if (v.toInt() >= 60 && v.toInt() <= 90) { 
      sprintf(temp,":So%d#",(int16_t)v.toInt());
      commandBool(temp);
    }
  }

  // Horizon limit
  v=server.arg("hl");
  if (v!="") {
    if (v.toInt() >= -30 && v.toInt() <= 30) { 
      sprintf(temp,":Sh%d#",(int16_t)v.toInt());
      commandBool(temp);
    }
  }

  // Meridian limit E
  v=server.arg("el");
  if (v!="") {
    if (v.toInt() >= -270 && v.toInt() <= 270) { 
      sprintf(temp,":SXE9,%d#",(int16_t)round((v.toInt()*60.0)/15.0));
      commandBool(temp);
    }
  }

  // Meridian limit W
  v=server.arg("wl");
  if (v!="") {
    if (v.toInt() >= -270 && v.toInt() <= 270) { 
      sprintf(temp,":SXEA,%d#",(int16_t)round((v.toInt()*60.0)/15.0));
      commandBool(temp);
    }
  }

  // Backlash
  v=server.arg("b1");
  if (v!="") {
    if (v.toInt() >= 0 && v.toInt() <= 3600) { 
      sprintf(temp,":$BR%d#",(int16_t)v.toInt());
      commandBool(temp);
    }
  }
  v=server.arg("b2");
  if (v!="") {
    if (v.toInt() >= 0 && v.toInt() <= 3600) { 
      sprintf(temp,":$BD%d#",(int16_t)v.toInt());
      commandBool(temp);
    }
  }
  v=server.arg("b3");
  if (v!="") {
    if (v.toInt() >= 0 && v.toInt() <= 32767) { 
      sprintf(temp,":rb%d#",(int16_t)v.toInt());
      commandBool(temp);
    }
  }
  v=server.arg("b4");
  if (v!="") {
    if (v.toInt() >= 0 && v.toInt() <= 32767) { 
      sprintf(temp,":Fb%d#",(int16_t)v.toInt());
      commandBool(":FA1#"); commandBool(temp);
    }
  }
  v=server.arg("b5");
  if (v!="") {
    if (v.toInt() >= 0 && v.toInt() <= 32767) { 
      sprintf(temp,":fb%d#",(int16_t)v.toInt());
      commandBool(":FA1#"); commandBool(temp);
    }
  }

  // TCF deadband
  v=server.arg("d4");
  if (v!="") {
    if (v.toInt() >= 1 && v.toInt() <= 32767) { 
      sprintf(temp,":Fd%d#",(int16_t)v.toInt());
      commandBool(":FA1#"); commandBool(temp);
    }
  }
  v=server.arg("d5");
  if (v!="") {
    if (v.toInt() >= 1 && v.toInt() <= 32767) { 
      sprintf(temp,":fd%d#",(int16_t)v.toInt());
      commandBool(":FA1#"); commandBool(temp);
    }
  }

  // TCF Coef
  v=server.arg("tc4");
  if (v!="") {
    if (v.toFloat() >= -999.0 && v.toFloat() <= 999.0) { 
      sprintf(temp,":FC%s#",v.c_str());
      commandBool(":FA1#"); commandBool(temp);
    }
  }
  v=server.arg("tc5");
  if (v!="") {
    if (v.toFloat() >= -999.0 && v.toFloat() <= 999.0) { 
      sprintf(temp,":fC%s#",v.c_str());
      commandBool(":FA1#"); commandBool(temp);
    }
  }

  // TCF Enable
  v=server.arg("en4");
  if (v == "0" || v == "1") {
    sprintf(temp,":Fc%s#",v.c_str());
    commandBool(":FA1#"); commandBool(temp);
  }
  v=server.arg("en5");
  if (v == "0" || v == "1") {
    sprintf(temp,":fc%s#",v.c_str());
    commandBool(":FA1#"); commandBool(temp);
  }

  // Location
  v=server.arg("g1"); // long deg
  v1=server.arg("g2"); // long min
  v2=server.arg("g3"); // long sec
  if (v != "" && v1 != "" && v2 != "") {
    if (v.toInt() >= -180 && v.toInt() <= 180 && v1.toInt() >= 0 && v1.toInt() <= 60 && v2.toInt() >= 0 && v2.toInt() <= 60) {
      sprintf(temp,":Sg%+04d*%02d:%02d#",(int16_t)v.toInt(),(int16_t)v1.toInt(),(int16_t)v2.toInt());
      commandBool(temp);
    }
  }
  v=server.arg("t1"); // lat deg
  v1=server.arg("t2"); // lat min
  v2=server.arg("t3"); // lat sec
  if (v != "" && v1 != "" && v2 != "") {
    if (v.toInt() >= -90 && v.toInt() <= 90 && v1.toInt() >= 0 && v1.toInt() <= 60 && v2.toInt() >= 0 && v2.toInt() <= 60) {
      sprintf(temp,":St%+03d*%02d:%02d#",(int16_t)v.toInt(),(int16_t)v1.toInt(),(int16_t)v2.toInt());
      commandBool(temp);
    }
  }
  v=server.arg("u1"); // UT hrs
  v1=server.arg("u2"); // UT min
  if (v != "" && v1 != "") {
    if (v.toInt() >= -14 && v.toInt() <= 12 && (v1.toInt() == 0 || v1.toInt() == 30 || v1.toInt() == 45)) {
      sprintf(temp,":SG%+03d:%02d#",(int16_t)v.toInt(),(int16_t)v1.toInt());
      commandBool(temp);
    }
  }

  String ssa=server.arg("advanced");
#if DISPLAY_RESET_CONTROLS != OFF
  if (ssa.equals("reset")) { commandBlind(":ERESET#"); return false; }
  #ifdef BOOT0_PIN
    if (ssa.equals("fwu")) { pinMode(BOOT0_PIN,OUTPUT); digitalWrite(BOOT0_PIN,HIGH); commandBlind(":ERESET#"); delay(500); pinMode(BOOT0_PIN,INPUT); return false; }
  #endif
#endif
  String ssm=server.arg("mountt"); if (!ssm.equals("")) { sprintf(temp,":SXEM,%s#",ssm.c_str()); commandBool(temp); }
  String ssr=server.arg("revert"); if (!ssr.equals("")) commandBool(":SXEM,0#");

  return true;
}
