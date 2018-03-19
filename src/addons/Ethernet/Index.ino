// -----------------------------------------------------------------------------------
// The home page, status information

#ifdef ADVANCED_CHARS_ON
  #define Axis1 "&alpha;"
  #define Axis1A "&alpha;"
  #define Axis2 "&delta;"
#else
  #define Axis1 "RA"
  #define Axis1A "RA"
  #define Axis2 "DEC"
#endif

const char html_settingsBrowserTime[] =
"&nbsp;&nbsp;<span id=\"datetime\"></span> UT (web browser)"
"<script> "
"function pad(num, size) { var s = '000000000' + num; return s.substr(s.length-size); }"
"var now = new Date(); document.getElementById('datetime').innerHTML = (now.getUTCMonth()+1)+'/'+"
"pad(now.getUTCDate().toString(),2)+'/'+pad(now.getUTCFullYear().toString().substr(-2),2)+"
"' '+pad(now.getUTCHours().toString(),2)+':'+pad(now.getUTCMinutes().toString(),2)+':'+pad(now.getUTCSeconds().toString(),2); "
"</script><br />\r\n";

const char* html_indexDate = "&nbsp;&nbsp;<font class='c'>%s</font>";
const char* html_indexTime = "&nbsp;<font class='c'>%s</font>&nbsp;UT";
const char* html_indexSidereal = "&nbsp;(<font class='c'>%s</font>&nbsp; LST)<br />";
const char* html_indexSite = "&nbsp;&nbsp;Long. = <font class='c'>%s</font>, Lat. = <font class='c'>%s</font><br />";

const char* html_indexTrue = "&nbsp;&nbsp;Absolute Position: " Axis1 "=<font class='c'>%ld</font> steps, " Axis2 "=<font class='c'>%ld</font> steps<br />";
const char* html_indexIndex = "&nbsp;&nbsp;IHS=<font class='c'>%ld</font> steps, IDS=<font class='c'>%ld</font> steps<br />";
const char* html_indexCorIdx = "&nbsp;&nbsp;IH=<font class='c'>%ld</font>\", ID=<font class='c'>%ld</font>\"<br />";
const char* html_indexCorPole = "&nbsp;&nbsp;altCor=<font class='c'>%ld</font>\", azmCor=<font class='c'>%ld</font>\"<br />";
const char* html_indexCorPolar = "&nbsp;&nbsp;Polar Offset: &Delta; Alt=<font class='c'>%ld</font>\", &Delta; Azm=<font class='c'>%ld</font>\"<br />";
const char* html_indexCorOrtho = "&nbsp;&nbsp;doCor=<font class='c'>%ld</font>\", pdCor=<font class='c'>%ld</font>\"<br />";
const char* html_indexRateDeltas = "&nbsp;&nbsp;&Delta; Axis1=<font class='c'>%s</font>\"/s, &Delta; Axis2=<font class='c'>%s</font>\"/s<br /><br />";

const char* html_indexPosition = "&nbsp;&nbsp;Current: " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char* html_indexTarget = "&nbsp;&nbsp;Target:&nbsp;&nbsp; " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char* html_indexAz1 = "&nbsp;&nbsp;Az1: " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char* html_indexAz2 = "&nbsp;&nbsp;Az2: " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char* html_indexPier = "&nbsp;&nbsp;Pier Side=<font class='c'>%s</font> (meridian flips <font class='c'>%s</font>)<br />";
const char* html_indexTracking = "&nbsp;&nbsp;Tracking: <font class='c'>%s %s</font><br />";
const char* html_indexPark = "&nbsp;&nbsp;Parking: <font class='c'>%s</font><br />";
const char* html_indexLastError = "&nbsp;&nbsp;Last Error: <font class='c'>%s</font><br />";
const char* html_indexMaxSpeed = "&nbsp;&nbsp;Maximum slew speed: <font class='c'>%s</font>&deg;/s<br />";
const char* html_indexMaxRate = "&nbsp;&nbsp;Current MaxRate: <font class='c'>%ld</font> (Default MaxRate: <font class='c'>%ld</font>)<br />";
const char* html_indexWorkload = "&nbsp;&nbsp;Workload: <font class='c'>%s</font><br />";

#ifdef OETHS
void handleRoot(EthernetClient *client) {
#else
void handleRoot() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();

  char temp[320]="";
  char temp1[80]="";
  char temp2[80]="";
  char temp3[80]="";
  char temp4[40]="";

  String data=html_headB;
  data += html_headerIdx; // page refresh
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  data += html_main_css6;
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_css_control1;
  data += html_main_css_control2;
  data += html_main_css_control3;
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
  data += html_links1S;
  data += html_links2N;
  data += html_links3N;
  data += html_links4N;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
#ifdef OETHS
  client->print(data); data="";
#endif

  data+="<div style='width: 27em;'>";

  data+="<b>Site:</b><br />";
  // Browser time
  data += html_settingsBrowserTime;

  // UTC Date
  Ser.print(":GX81#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexDate,temp2);
  data += temp;

  // UTC Time
  Ser.print(":GX80#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexTime,temp2);
  data += temp;

  // LST
  Ser.print(":GS#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexSidereal,temp2);
  data += temp;

  // Longitude and Latitude
  Ser.print(":Gg#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Ser.print(":Gt#");
  temp3[Ser.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexSite,temp2,temp3);
  data += temp;
#ifdef OETHS
  client->print(data); data="";
#endif

  data+="<br /><b>Coordinates:</b><br />";

  // RA,Dec current
  Ser.print(":GR#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Ser.print(":GD#");
  temp3[Ser.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexPosition,temp2,temp3); 
  data += temp;

  // RA,Dec target
  Ser.print(":Gr#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Ser.print(":Gd#");
  temp3[Ser.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexTarget,temp2,temp3); 
  data += temp;

  // pier side
  Ser.print(":GX94#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
  bool meridianFlipsDisabled=strstr(temp2, "N");
  pierSide=strtol(&temp2[0],NULL,10);
  if ((pierSide==PierSideFlipWE1) ||
      (pierSide==PierSideFlipWE2) ||
      (pierSide==PierSideFlipWE3)) strcpy(temp2,"Meridian Flip, West to East"); else
  if ((pierSide==PierSideFlipEW1) ||
      (pierSide==PierSideFlipEW2) ||
      (pierSide==PierSideFlipEW3)) strcpy(temp2,"Meridian Flip, East to West"); else
  if (pierSide==PierSideWest) strcpy(temp2,"West"); else
  if (pierSide==PierSideEast) strcpy(temp2,"East"); else
  if (pierSide==PierSideNone) strcpy(temp2,"None"); else strcpy(temp2,"Unknown");

  // meridian flips
  if (meridianFlipsDisabled) strcpy(temp3,"Off"); else {
    strcpy(temp3,"On");
    // automatic meridian flips
    if (!strstr(stat,"A") && (!meridianFlipsDisabled)) { // not AltAzm and Enabled
      Ser.print(":GX95#");
      temp4[Ser.readBytesUntil('#',temp4,20)]=0;
      if (strlen(temp4)>0) {
        if (!strstr(temp4, "0")) strcat(temp3,"</font>, <font class=\"c\">Auto. @Limit"); 
      }
    }
  }
  sprintf(temp,html_indexPier,temp2,temp3);
  data += temp;
#ifdef OETHS
  client->print(data); data="";
#endif

  data+="<br /><b>Alignment:</b><br />";

  if ((!strstr(stat,"A")) && (!strstr(stat,"k"))) {  // not AltAzm and not Fork_Alt
    Ser.print(":GX02#");
    temp2[Ser.readBytesUntil('#',temp2,20)]=0;
    if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
    long altCor=strtol(&temp2[0],NULL,10);
    Ser.print(":GX03#");
    temp2[Ser.readBytesUntil('#',temp2,20)]=0;
    if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
    long azmCor=strtol(&temp2[0],NULL,10);
    sprintf(temp,html_indexCorPolar,(long)(altCor),(long)(azmCor)); 
    data += temp;
  }
#ifdef OETHS
  client->print(data); data="";
#endif

  data+="<br /><b>Operations:</b><br />";

  // Park
  if (strstr(stat,"p")) strcpy(temp2,"Not Parked"); else
  if (strstr(stat,"P")) strcpy(temp2,"Parked"); else
  if (strstr(stat,"I")) strcpy(temp2,"Parking"); else
  if (strstr(stat,"F")) strcpy(temp2,"Park Failed"); else strcpy(temp2,"Unknown");
  if (strstr(stat,"H")) strcat(temp2," </font>(<font class=\"c\">At Home</font>)<font class=\"c\">");
  sprintf(temp,html_indexPark,temp2);
  data += temp;

  // Tracking
  if (strstr(stat,"N") && strstr(stat,"n")) strcpy(temp2,"Off"); else
  if (!strstr(stat,"n")) strcpy(temp2,"On"); else
  if (!strstr(stat,"N")) strcpy(temp2,"Slewing");
  
  strcpy(temp3,"</font>(<font class=\"c\">");
  if (strstr(stat,"S")) strcat(temp3,"PPS Sync, ");
  if (strstr(stat,"r") && !strstr(temp2,"t")) strcat(temp3,"Refr. Compensation, ");
  if (strstr(stat,"r") && strstr(temp2,"t")) strcat(temp3,"Full Compensation, ");
  if (temp3[strlen(temp3)-2]==',') { temp3[strlen(temp3)-2]=0; strcat(temp3,"</font>)<font class=\"c\">"); } else strcpy(temp3,"");
  sprintf(temp,html_indexTracking,temp2,temp3);
  data += temp;

  // Tracking rate
  Ser.print(":GT#");
  temp4[Ser.readBytesUntil('#',temp4,20)]=0;
  if (strlen(temp4)>6) {
    double tr=atof(temp4);
    sprintf(temp,"Tracking Rate: <font class=\"c\">%5.3f</font>Hz<br />",tr);
    data += temp;
  }

  // Slew speed
  Ser.print(":GX97#");
  temp1[Ser.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)>2) {
    sprintf(temp,html_indexMaxSpeed,temp1);
    data += temp;
  } else {
    // fall back to MaxRate display if not supported
    Ser.print(":GX92#");
    temp1[Ser.readBytesUntil('#',temp1,20)]=0;
    if (strlen(temp1)<=0) { strcpy(temp1,"0"); }
    long maxRate=strtol(&temp1[0],NULL,10);
    Ser.print(":GX93#");
    temp1[Ser.readBytesUntil('#',temp1,20)]=0;
    if (strlen(temp1)<=0) { strcpy(temp1,"0"); }
    long MaxRate=strtol(&temp1[0],NULL,10);;
    sprintf(temp,html_indexMaxRate,maxRate,MaxRate);
    data += temp;
  }
#ifdef OETHS
  client->print(data); data="";
#endif

  data+="<br /><b>Errors:</b><br />";

  // Last Error
  strcpy(temp2,"None");
  lastError=(Errors)(stat[strlen(stat)-1]-'0');
  if (lastError==ERR_NONE) strcpy(temp2,"None"); else
  if (lastError==ERR_MOTOR_FAULT) strcpy(temp2,"</font><font class=\"y\">Motor Fault"); else
  if (lastError==ERR_ALT) strcpy(temp2,"</font><font class=\"y\">Altitude Min/Max"); else
  if (lastError==ERR_LIMIT_SENSE) strcpy(temp2,"</font><font class=\"y\">Limit Sense"); else
  if (lastError==ERR_DEC) strcpy(temp2,"</font><font class=\"y\">Dec Limit Exceeded"); else
  if (lastError==ERR_AZM) strcpy(temp2,"</font><font class=\"y\">Azm Limit Exceeded"); else
  if (lastError==ERR_UNDER_POLE) strcpy(temp2,"</font><font class=\"y\">Under Pole Limit Exceeded"); else
  if (lastError==ERR_MERIDIAN) strcpy(temp2,"</font><font class=\"y\">Meridian Limit (W) Exceeded"); else
  if (lastError==ERR_SYNC) strcpy(temp2,"</font><font class=\"y\">Sync. ignored >30&deg;");
  sprintf(temp,html_indexLastError,temp2);
  data += temp;

  // Loop time
  Ser.print(":GXFA#");
  temp1[Ser.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)<=0) { strcpy(temp1,"?%"); }
  sprintf(temp,html_indexWorkload,temp1);
  data += temp;

  data += "<br />";
  data += "</div><br class=\"clear\" />\r\n";
  data += "</div></body></html>";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/html",data);
#endif
}

