// The settings.htm page

const char html_settings1[] = "<div class=\"t\"><table width=\"100%%\"><tr><td><b><font size=\"5\">%s</font></b></td><td align=\"right\"><b>" Product " " Version " (OnStep %s)</b>";
const char html_settings2[] = "</td></tr></table>";
const char html_settings3[] = "</div><div class=\"b\">\r\n";
const char html_settings4[] = 
"Maximum Slew Speed: "
"<form method=\"get\" action=\"/settings.htm\">"
"<button name=\"ss\" value=\"vs\" type=\"submit\">Very Slow (0.5x)</button>"
"<button name=\"ss\" value=\"s\" type=\"submit\">Slow (0.75x)</button>";
const char html_settingsMaxRate[] =
"<button name=\"ss\" value=\"n\" type=\"submit\">Normal (1x) </button>"
"<button name=\"ss\" value=\"f\" type=\"submit\">Fast (1.5x) </button>"
"<button name=\"ss\" value=\"vf\" type=\"submit\">Very Fast (2x) </button>"
"</form><br />\r\n";
const char html_settingsBlAxis1[] =
"Backlash: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"b1\" min=\"0\" max=\"999\">"
"<button type=\"submit\">Upload</button>"
" (Axis1, in arc-seconds 0 to 999)"
"</form>"
"\r\n";
const char html_settingsBlAxis2[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"b2\" min=\"0\" max=\"999\">"
"<button type=\"submit\">Upload</button>"
" (Axis2, in arc-seconds 0 to 999)"
"</form>"
"<br />\r\n";
const char html_settingsMinAlt[] = 
"Limits: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"hl\" min=\"-30\" max=\"30\">"
"<button type=\"submit\">Upload</button>"
" (Horizon, in degrees +/- 30)"
"</form>"
"\r\n";
const char html_settingsMaxAlt[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"ol\" min=\"60\" max=\"90\">"
"<button type=\"submit\">Upload</button>"
" (Overhead, in degrees 60 to 90)"
"</form>"
"\r\n";
const char html_settingsPastMerE[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"el\" min=\"-45\" max=\"45\">"
"<button type=\"submit\">Upload</button>"
" (Past Meridian when East of the pier, in degrees +/-45)"
"</form>"
"\r\n";
const char html_settingsPastMerW[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"wl\" min=\"-45\" max=\"45\">"
"<button type=\"submit\">Upload</button>"
" (Past Meridian when West of the pier, in degrees +/-45)"
"</form>"
"<br />\r\n";
const char html_settingsLongDeg[] = 
"Location: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"g1\" min=\"-180\" max=\"180\">&nbsp;&deg;&nbsp;";
const char html_settingsLongMin[] = 
" <input value=\"%s\" type=\"number\" name=\"g2\" min=\"0\" max=\"60\">&nbsp;'&nbsp;&nbsp;"
"<button type=\"submit\">Upload</button>"
" (Longitude, in deg. and min. +/- 180)"
"</form>"
"\r\n";
const char html_settingsLatDeg[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"t1\" min=\"-90\" max=\"90\">&nbsp;&deg;&nbsp;";
const char html_settingsLatMin[] = 
" <input value=\"%s\" type=\"number\" name=\"t2\" min=\"0\" max=\"60\">&nbsp;'&nbsp;&nbsp;"
"<button type=\"submit\">Upload</button>"
" (Latitude, in deg. and min. +/- 90)"
"</form>"
"\r\n";

const char html_settingsOffsetDeg[] = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"u1\" min=\"-12\" max=\"14\">&nbsp;hrs&nbsp;";
const char html_settingsOffsetMin1[] = 
" <select name=\"u2\"><option value=\"0\" %s>00</option><option value=\"30\" %s>30</option><option value=\"45\" %s>45</option></select>&nbsp;min.&nbsp;"
"<button type=\"submit\">Upload</button>";
const char html_settingsOffsetMin2[] = 
" (UTC Offset, in hours -12 to +14)<br />"
"Opposite of a time-zone value, this is for Standard Time (not Daylight Time.)<br /><br />"
"</form><br />"
"\r\n";

void handleSettings(EthernetClient *client) {
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
  
  char temp[320]="";
  char temp1[80]="";
  char temp2[80]="";
  char temp3[80]="";
  
  processSettingsGet();

  // send a standard http response header
  String data=html_head1;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_css9;
  data += html_head2;
  client->print(data); data="";


  // finish the standard http response header
  Ser.print(":GVP#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; }
  Ser.print(":GVN#");
  temp3[Ser.readBytesUntil('#',temp3,20)]=0; 
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_settings1,temp2,temp3);
  data += temp;
  data += html_settings2;
  data += html_links1se;
  data += html_links2se;
  data += html_links3se;
  data += html_settings3;
  data += html_settings4;

  // Slew speed
  data += html_settingsMaxRate;
  client->print(data); data="";

  // Backlash
  Ser.print(":%BR#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  int backlashAxis1=(int)strtol(&temp2[0],NULL,10);
  sprintf(temp,html_settingsBlAxis1,backlashAxis1);
  data += temp;
  Ser.print(":%BD#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  int backlashAxis2=(int)strtol(&temp2[0],NULL,10);
  sprintf(temp,html_settingsBlAxis2,backlashAxis2);
  data += temp;

  // Limits
  strcpy(temp2,"");
  Ser.print(":Gh#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  int minAlt=(int)strtol(&temp2[0],NULL,10);
  sprintf(temp,html_settingsMinAlt,minAlt);
  data += temp;
  Ser.print(":Go#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  int maxAlt=(int)strtol(&temp2[0],NULL,10);
  sprintf(temp,html_settingsMaxAlt,maxAlt);
  data += temp;
  client->print(data); data="";

  boolean foundHash;
  strcpy(temp2,"");
  Ser.print(":GXE9#");
  temp2[readBytesUntil2('#',temp2,20,&foundHash,WebTimeout)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  if (foundHash) {
    int degPastMerE=(int)strtol(&temp2[0],NULL,10);
    degPastMerE=round((degPastMerE*15.0)/60.0);
    sprintf(temp,html_settingsPastMerE,degPastMerE);
    data += temp;
    strcpy(temp2,"");
    Ser.print(":GXEA#");
    temp2[Ser.readBytesUntil('#',temp2,20)]=0;
    if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
    int degPastMerW=(int)strtol(&temp2[0],NULL,10);
    degPastMerW=round((degPastMerW*15.0)/60.0);
    sprintf(temp,html_settingsPastMerW,degPastMerW);
    data += temp;
  } else data += "<br />\r\n";

  // Longitude
  Ser.print(":Gg#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"+000*00"); }
  temp2[4]=0; // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  sprintf(temp,html_settingsLongDeg,temp2);
  data += temp;
  sprintf(temp,html_settingsLongMin,(char*)&temp2[5]);
  data += temp;

  // Latitude
  Ser.print(":Gt#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"+00*00"); }
  temp2[3]=0; // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  sprintf(temp,html_settingsLatDeg,temp2);
  data += temp;
  sprintf(temp,html_settingsLatMin,(char*)&temp2[4]);
  data += temp;

  // UTC Offset
  Ser.print(":GG#");
  temp1[Ser.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)<=0) { strcpy(temp1,"+00"); }
  strcpy(temp2,temp1);
  temp2[3]=0; // deg. part only
  if (temp2[0]=='+') temp2[0]='0'; // remove +
  sprintf(temp,html_settingsOffsetDeg,temp2);
  data += temp;
  strcpy(temp2,temp1);
  if (temp2[3]==0) sprintf(temp,html_settingsOffsetMin1,"selected","",""); else
  if (temp2[4]=='3') sprintf(temp,html_settingsOffsetMin1,"","selected",""); else
  if (temp2[4]=='4') sprintf(temp,html_settingsOffsetMin1,"","","selected");
  data += temp;
  data += html_settingsOffsetMin2;
  
  strcpy(temp,"</div></body></html>");
  data += temp;

  client->print(data);
}

void processSettingsGet() {
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

