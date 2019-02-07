// -----------------------------------------------------------------------------------
// Configuration

const char html_config1[] = 
"Maximum Slew Speed: "
"<form method='get' action='/configuration.htm'>"
"<button name='ss' value='vs' type='submit'>Very Slow (0.5x)</button>"
"<button name='ss' value='s' type='submit'>Slow (0.75x)</button>";
const char html_configMaxRate[] =
"<button name='ss' value=\"n\" type='submit'>Normal (1x)</button>"
"<button name='ss' value=\"f\" type='submit'>Fast (1.5x)</button>"
"<button name='ss' value=\"vf\" type='submit'>Very Fast (2x)</button>"
"</form><br /><br />\r\n";
const char html_configBlAxis1[] =
"Backlash: <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='b1' min='0' max='999'>"
"<button type='submit'>Upload</button>"
" (Axis1, in arc-seconds 0 to 999)"
"</form>"
"\r\n";
const char html_configBlAxis2[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='b2' min='0' max='999'>"
"<button type='submit'>Upload</button>"
" (Axis2, in arc-seconds 0 to 999)"
"</form>"
"<br />\r\n";
const char html_configMinAlt[] = 
"Limits: <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='hl' min='-30' max='30'>"
"<button type='submit'>Upload</button>"
" (Horizon, in degrees +/- 30)"
"</form>"
"\r\n";
const char html_configMaxAlt[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='ol' min='60' max='90'>"
"<button type='submit'>Upload</button>"
" (Overhead, in degrees 60 to 90)"
"</form>"
"\r\n";
const char html_configPastMerE[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='el' min='-45' max='45'>"
"<button type='submit'>Upload</button>"
" (Past Meridian when East of the pier, in degrees +/-45)"
"</form>"
"\r\n";
const char html_configPastMerW[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%d' type='number' name='wl' min='-45' max='45'>"
"<button type='submit'>Upload</button>"
" (Past Meridian when West of the pier, in degrees +/-45)"
"</form>"
"<br />\r\n";
const char html_configLongDeg[] = 
"Location: <br />"
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='g1' min='-180' max='180'>&nbsp;&deg;&nbsp;";
const char html_configLongMin[] = 
" <input value='%s' type='number' name='g2' min='0' max='60'>&nbsp;'&nbsp;&nbsp;"
"<button type='submit'>Upload</button>"
" (Longitude, in deg. and min. +/- 180, W is +)"
"</form>"
"\r\n";
const char html_configLatDeg[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='t1' min='-90' max='90'>&nbsp;&deg;&nbsp;";
const char html_configLatMin[] = 
" <input value='%s' type='number' name='t2' min='0' max='60'>&nbsp;'&nbsp;&nbsp;"
"<button type='submit'>Upload</button>"
" (Latitude, in deg. and min. +/- 90, N is +)"
"</form>"
"\r\n";
const char html_configOffsetDeg[] = 
"<form method='get' action='/configuration.htm'>"
" <input value='%s' type='number' name='u1' min='-12' max='14'>&nbsp;hrs&nbsp;";
const char html_configOffsetMin[] = 
" <select name='u2'><option value='0' %s>00</option><option value='30' %s>30</option><option value='45' %s>45</option></select>&nbsp;min.&nbsp;"
"<button type='submit'>Upload</button>"
" (UTC Offset, in hours -12 to +14)<br />"
"Opposite of a time-zone value, this is for Standard Time (not Daylight Time.)<br /><br />"
"</form>"
"\r\n";
#ifdef OETHS
void handleConfiguration(EthernetClient *client) {
#else
void handleConfiguration() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
  
  char temp[320]="";
  char temp1[80]="";
  char temp2[80]="";
  
  processConfigurationGet();

  sendHtmlStart();

  // send a standard http response header
  String data=html_headB;
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  sendHtml(data);
  data += html_main_css5;
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_cssE;
  data += html_headE;
  data += html_bodyB;
  sendHtml(data);

  // get status
  mountStatus.update();

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
#ifdef ENCODERS_ON
  data += html_linksEncN;
#endif
  sendHtml(data);
  data += html_links4N;
  data += html_links5S;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  data += html_config1;

  data+="<div style='width: 35em;'>";

  // Slew speed
  data += html_configMaxRate;
  sendHtml(data);

  // Backlash
  if (!sendCommand(":%BR#",temp1)) strcpy(temp1,"0"); int backlashAxis1=(int)strtol(&temp1[0],NULL,10);
  sprintf(temp,html_configBlAxis1,backlashAxis1);
  data += temp;
  if (!sendCommand(":%BD#",temp1)) strcpy(temp1,"0"); int backlashAxis2=(int)strtol(&temp1[0],NULL,10);
  sprintf(temp,html_configBlAxis2,backlashAxis2);
  data += temp;
  sendHtml(data);

  // Overhead and Horizon Limits
  if (!sendCommand(":Gh#",temp1)) strcpy(temp1,"0"); int minAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf(temp,html_configMinAlt,minAlt);
  data += temp;
  if (!sendCommand(":Go#",temp1)) strcpy(temp1,"0"); int maxAlt=(int)strtol(&temp1[0],NULL,10);
  sprintf(temp,html_configMaxAlt,maxAlt);
  data += temp;

  // Meridian Limits
  if ((sendCommand(":GXE9#",temp1)) && (sendCommand(":GXEA#",temp2))) {
    int degPastMerE=(int)strtol(&temp1[0],NULL,10);
    degPastMerE=round((degPastMerE*15.0)/60.0);
    sprintf(temp,html_configPastMerE,degPastMerE);
    data += temp;
    int degPastMerW=(int)strtol(&temp2[0],NULL,10);
    degPastMerW=round((degPastMerW*15.0)/60.0);
    sprintf(temp,html_configPastMerW,degPastMerW);
    data += temp;
  } else data += "<br />\r\n";
  sendHtml(data);

  // Longitude
  if (!sendCommand(":Gg#",temp1)) strcpy(temp1,"+000*00");
  temp1[4]=0; // deg. part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  sprintf(temp,html_configLongDeg,temp1);
  data += temp;
  sprintf(temp,html_configLongMin,(char*)&temp1[5]);
  data += temp;
  sendHtml(data);

  // Latitude
  if (!sendCommand(":Gt#",temp1)) strcpy(temp1,"+00*00");
  temp1[3]=0; // deg. part only
  if (temp1[0]=='+') temp1[0]='0'; // remove +
  sprintf(temp,html_configLatDeg,temp1);
  data += temp;
  sprintf(temp,html_configLatMin,(char*)&temp1[4]);
  data += temp;
  sendHtml(data);

  // UTC Offset
  if (!sendCommand(":GG#",temp1)) strcpy(temp1,"+00");
  strcpy(temp2,temp1);
  temp2[3]=0; // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  sprintf(temp,html_configOffsetDeg,temp2);
  data += temp;
  strcpy(temp2,temp1);
  if (temp2[3]==0) sprintf(temp,html_configOffsetMin,"selected","",""); else
  if (temp2[4]=='3') sprintf(temp,html_configOffsetMin,"","selected",""); else
  if (temp2[4]=='4') sprintf(temp,html_configOffsetMin,"","","selected");
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
    if (v=="vs") Ser.print(":SX93,5#");
    if (v=="s") Ser.print(":SX93,4#");
    if (v=="n") Ser.print(":SX93,3#");
    if (v=="f") Ser.print(":SX93,2#");
    if (v=="vf") Ser.print(":SX93,1#");
  }
  
  // Overhead and Horizon Limits
  v=server.arg("ol");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=60) && (i<=90))) { 
      sprintf(temp,":So%d#",i);
      Ser.print(temp);
    }
  }
  v=server.arg("hl");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-30) && (i<=30))) { 
      sprintf(temp,":Sh%d#",i);
      Ser.print(temp);
    }
  }

  // Meridian Limits
  v=server.arg("el");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-45) && (i<=45))) { 
      i=round((i*60.0)/15.0);
      sprintf(temp,":SXE9,%d#",i);
      Ser.print(temp);
    }
  }
  v=server.arg("wl");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-45) && (i<=45))) { 
      i=round((i*60.0)/15.0);
      sprintf(temp,":SXEA,%d#",i);
      Ser.print(temp);
    }
  }

  // Backlash Limits
  v=server.arg("b1");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=999))) { 
      sprintf(temp,":$BR%d#",i);
      Ser.print(temp);
    }
  }
  v=server.arg("b2");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=999))) { 
      sprintf(temp,":$BD%d#",i);
      Ser.print(temp);
    }
  }

  // Location
  int long_deg=-999;
  v=server.arg("g1");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-180) && (i<=180))) { long_deg=i; }
  }
  v=server.arg("g2");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=60))) { 
      if ((long_deg>=-180) && (long_deg<=180)) {
        sprintf(temp,":Sg%+04d*%02d#",long_deg,i);
        Ser.print(temp);
      }
    }
  }
  int lat_deg=-999;
  v=server.arg("t1");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-90) && (i<=90))) { lat_deg=i; }
  }
  v=server.arg("t2");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=60))) {
      if ((lat_deg>=-90) && (lat_deg<=90)) {
        sprintf(temp,":St%+03d*%02d#",lat_deg,i);
        Ser.print(temp);
      }
    }
  }
  int ut_hrs=-999;
  v=server.arg("u1");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=-13) && (i<=13))) { ut_hrs=i; }
  }
  v=server.arg("u2");
  if (v!="") {
    if ( (atoi2((char*)v.c_str(),&i)) && ((i==00) || (i==30) || (i==45))) {
      if ((ut_hrs>=-13) && (ut_hrs<=13)) {
        sprintf(temp,":SG%+03d:%02d#",ut_hrs,i);
        Ser.print(temp);
      }
    }
  }

  // clear any possible response
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}
