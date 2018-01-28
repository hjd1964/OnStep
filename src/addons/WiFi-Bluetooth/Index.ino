// The index.htm page

#define Axis1 "RA"
#define Axis1A "RA"
#define Axis2 "Dec"
const char html_index1[] = "<div class=\"t\"><table width=\"100%%\"><tr><td><b><font size=\"5\">%s</font></b></td><td align=\"right\"><b>" Product " " Version " (OnStep %s)</b>";
const char html_index2[] = "</td></tr></table>";
const char html_index3[] = "</div><div class=\"b\">";
const char* html_indexDate = "<font class=\"c\">%s</font>";
const char* html_indexTime= "&nbsp;<font class=\"c\">%s</font>&nbsp;UT";
const char* html_indexSidereal = "&nbsp;(<font class=\"c\">%s</font>&nbsp; Local Apparent Sidereal Time)<br /><br />";
const char* html_indexSite = "Longitude = <font class=\"c\">%s</font>, Latitude = <font class=\"c\">%s</font><br /><br />";
const char* html_indexTrue = "Absolute Position: " Axis1 "=<font class=\"c\">%ld</font> steps, " Axis2 "=<font class=\"c\">%ld</font> steps<br />";
const char* html_indexIndex = "IHS=<font class=\"c\">%ld</font> steps, IDS=<font class=\"c\">%ld</font> steps<br />";
const char* html_indexCorIdx = "IH=<font class=\"c\">%ld</font>\", ID=<font class=\"c\">%ld</font>\"<br />";
const char* html_indexCorPole = "altCor=<font class=\"c\">%ld</font>\", azmCor=<font class=\"c\">%ld</font>\"<br />";
const char* html_indexCorPolar = "Polar Alignment Correction: Alt=<font class=\"c\">%ld</font>\", Azm=<font class=\"c\">%ld</font>\"<br /><br />";
const char* html_indexCorOrtho = "doCor=<font class=\"c\">%ld</font>\", pdCor=<font class=\"c\">%ld</font>\"<br />";
const char* html_indexRateDeltas = "&Delta; Axis1=<font class=\"c\">%s</font>\"/s, &Delta; Axis2=<font class=\"c\">%s</font>\"/s<br /><br />";
const char* html_indexPosition = "Current Coordinates: " Axis1 "=<font class=\"c\">%s</font>, " Axis2 "=<font class=\"c\">%s</font><br />";
const char* html_indexTarget = "Target Coordinates: " Axis1 "=<font class=\"c\">%s</font>, " Axis2 "=<font class=\"c\">%s</font><br />";
const char* html_indexAz1 = "Az1: " Axis1 "=<font class=\"c\">%s</font>, " Axis2 "=<font class=\"c\">%s</font><br />";
const char* html_indexAz2 = "Az2: " Axis1 "=<font class=\"c\">%s</font>, " Axis2 "=<font class=\"c\">%s</font><br />";
const char* html_indexPier = "Pier Side=<font class=\"c\">%s</font> (meridian flips <font class=\"c\">%s</font>)<br /><br />";
const char* html_indexTracking = "Tracking: <font class=\"c\">%s %s</font><br />";
const char* html_indexPark = "Park: <font class=\"c\">%s</font><br /><br />";
const char* html_indexLastError = "Last Error: <font class=\"c\">%s</font><br />";
const char* html_indexMaxSpeed = "Maximum slew speed: <font class=\"c\">%s</font>&deg;/s<br /><br />";
const char* html_indexMaxRate = "Current MaxRate: <font class=\"c\">%ld</font> (Default MaxRate: <font class=\"c\">%ld</font>)<br /><br />";
const char* html_indexWorkload = "Workload: <font class=\"c\">%s</font><br />";

void handleRoot() {
  Serial.setTimeout(WebTimeout);
  serialRecvFlush();

  char temp[320]="";
  char temp1[80]="";
  char temp2[80]="";
  char temp3[80]="";
  char temp4[40]="";

  String data=html_head1;
  data += html_headerIdx; // page refresh
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

  // get status
  char stat[20] = "";
  Serial.print(":GU#");
  stat[Serial.readBytesUntil('#',stat,20)]=0;

  // finish the standard http response header
  Serial.print(":GVP#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; }
  Serial.print(":GVN#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0; 
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_index1,temp2,temp3);

  data += temp;
  data += html_index2;
  data += html_links1in;
  data += html_links2in;
  data += html_links3in;
  data += html_index3;
  
  // UTC Date
  Serial.print(":GX81#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexDate,temp2);
  data += temp;

  // UTC Time
  Serial.print(":GX80#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexTime,temp2);
  data += temp;

  // LST
  Serial.print(":GS#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  sprintf(temp,html_indexSidereal,temp2);
  data += temp;

  // Longitude and Latitude
  Serial.print(":Gg#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Serial.print(":Gt#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexSite,temp2,temp3);
  data += temp;

  if ((!strstr(stat,"A")) && (!strstr(stat,"k"))) {  // not AltAzm and not Fork_Alt
    Serial.print(":GX02#");
    temp2[Serial.readBytesUntil('#',temp2,20)]=0;
    if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
    long altCor=strtol(&temp2[0],NULL,10);
    Serial.print(":GX03#");
    temp2[Serial.readBytesUntil('#',temp2,20)]=0;
    if (strlen(temp2)<=0) { strcpy(temp2,"0"); }
    long azmCor=strtol(&temp2[0],NULL,10);
    sprintf(temp,html_indexCorPolar,(long)(altCor),(long)(azmCor)); 
    data += temp;
  }

  // RA,Dec current
  Serial.print(":GR#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Serial.print(":GD#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexPosition,temp2,temp3); 
  data += temp;

  // RA,Dec target
  Serial.print(":Gr#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); }
  Serial.print(":Gd#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_indexTarget,temp2,temp3); 
  data += temp;

  // pierSide
  Serial.print(":GX94#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0;
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
  if (meridianFlipsDisabled) strcpy(temp3,"Disabled"); else {
    strcpy(temp3,"Enabled");
    // automatic meridian flips
    if (!strstr(stat,"A") && (!meridianFlipsDisabled)) { // not AltAzm and Enabled
      Serial.print(":GX95#");
      temp4[Serial.readBytesUntil('#',temp4,20)]=0;
      if (strlen(temp4)>0) {
        if (strstr(temp4, "0")) strcat(temp3,"</font>, <font class=\"c\">not Automatic"); else strcat(temp3,"</font>, <font class=\"c\">Automatic"); 
      }
    }
  }
  sprintf(temp,html_indexPier,temp2,temp3);
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
  Serial.print(":GT#");
  temp4[Serial.readBytesUntil('#',temp4,20)]=0;
  if (strlen(temp4)>6) {
    double tr=atof(temp4);
    dtostrf(tr,5,3,temp4);
    sprintf(temp,"Tracking Rate: <font class=\"c\">%s</font>Hz<br />",temp4);
    data += temp;
  }

  // Park
  if (strstr(stat,"p")) strcpy(temp2,"Not Parked"); else
  if (strstr(stat,"P")) strcpy(temp2,"Parked"); else
  if (strstr(stat,"I")) strcpy(temp2,"Parking"); else
  if (strstr(stat,"F")) strcpy(temp2,"Park Failed"); else strcpy(temp2,"Unknown");
  if (strstr(stat,"H")) strcat(temp2," </font>(<font class=\"c\">At Home</font>)<font class=\"c\">");
  sprintf(temp,html_indexPark,temp2);
  data += temp;

  // Last Error
  strcpy(temp2,"</font><font class=\"g\">None");
  lastError=(Errors)(stat[strlen(stat)-1]-'0');
  if (lastError==ERR_NONE) strcpy(temp2,"</font><font class=\"g\">None"); else
  if (lastError==ERR_MOTOR_FAULT) strcpy(temp2,"Motor Fault"); else
  if (lastError==ERR_ALT) strcpy(temp2,"Altitude Min/Max"); else
  if (lastError==ERR_LIMIT_SENSE) strcpy(temp2,"Limit Sense"); else
  if (lastError==ERR_DEC) strcpy(temp2,"Dec Limit Exceeded"); else
  if (lastError==ERR_AZM) strcpy(temp2,"Azm Limit Exceeded"); else
  if (lastError==ERR_UNDER_POLE) strcpy(temp2,"Under Pole Limit Exceeded"); else
  if (lastError==ERR_MERIDIAN) strcpy(temp2,"Meridian Limit (W) Exceeded"); else
  if (lastError==ERR_SYNC) strcpy(temp2,"Sync. ignored >30&deg;");
  sprintf(temp,html_indexLastError,temp2);
  data += temp;

  // Slew speed
  Serial.print(":GX97#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)>2) {
    sprintf(temp,html_indexMaxSpeed,temp1);
    data += temp;
  } else {
    // fall back to MaxRate display if not supported
    Serial.print(":GX92#");
    temp1[Serial.readBytesUntil('#',temp1,20)]=0;
    if (strlen(temp1)<=0) { strcpy(temp1,"0"); }
    long maxRate=strtol(&temp1[0],NULL,10);
    Serial.print(":GX93#");
    temp1[Serial.readBytesUntil('#',temp1,20)]=0;
    if (strlen(temp1)<=0) { strcpy(temp1,"0"); }
    long MaxRate=strtol(&temp1[0],NULL,10);;
    sprintf(temp,html_indexMaxRate,maxRate,MaxRate);
    data += temp;
  }

  // Loop time
  Serial.print(":GXFA#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)<=0) { strcpy(temp1,"?%"); }
  sprintf(temp,html_indexWorkload,temp1);
  data += temp;

  data+="</div></body></html>";

  server.send(200, "text/html",data);
}

