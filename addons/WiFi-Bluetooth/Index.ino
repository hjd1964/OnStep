// -----------------------------------------------------------------------------------
// The home page, status information

#define HIGH_PRECISION_COORDS_ON

#define leftTri  "&#x25c4;"
#define rightTri "&#x25ba;"
#define upTri    "&#x25b2;"
#define downTri  "&#x25bc;"

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

const char* html_indexPosition = "&nbsp;&nbsp;Current: " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char* html_indexTarget = "&nbsp;&nbsp;Target:&nbsp;&nbsp; " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
#ifdef ENCODERS_ON
const char* html_indexEncoder1 = "&nbsp;&nbsp;OnStep: Ax1=<font class='c'>%s</font>, Ax2=<font class='c'>%s</font><br />";
const char* html_indexEncoder2 = "&nbsp;&nbsp;Encodr: Ax1=<font class='c'>%s</font>, Ax2=<font class='c'>%s</font><br />";
#endif
const char* html_indexPier = "&nbsp;&nbsp;Pier Side=<font class='c'>%s</font> (meridian flips <font class='c'>%s</font>)<br />";

const char* html_indexCorPolar = "&nbsp;&nbsp;%s <font class='c'>%ld</font>%c &nbsp; %s <font class='c'>%ld</font>%c &nbsp;(Mount relative to %s)<br />";

const char* html_indexPark = "&nbsp;&nbsp;Parking: <font class='c'>%s</font><br />";
const char* html_indexTracking = "&nbsp;&nbsp;Tracking: <font class='c'>%s %s</font><br />";
const char* html_indexMaxRate = "&nbsp;&nbsp;Current MaxRate: <font class='c'>%ld</font> (Default MaxRate: <font class='c'>%ld</font>)<br />";
const char* html_indexMaxSpeed = "&nbsp;&nbsp;Maximum slew speed: <font class='c'>%s</font>&deg;/s<br />";

const char* html_indexTPHD = "&nbsp;&nbsp;%s <font class='c'>%s</font>%s<br />";

const char* html_indexDriverStatus = " Driver: <font class='c'>%s</font><br />";
const char* html_indexLastError = "&nbsp;&nbsp;Last Error: <font class='c'>%s</font><br />";
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

  sendHtmlStart();

  String data=html_headB;
  data += html_headerIdx; // page refresh
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  data += html_main_css6;
  sendHtml(data);
  data += html_main_css7;
  data += html_main_css8;
  data += html_main_css_control1;
  data += html_main_css_control2;
  data += html_main_css_control3;
  data += html_main_cssE;
  data += html_headE;
  data += html_bodyB;
  sendHtml(data);

  // get status (all)
  mountStatus.update(true);

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1S;
  data += html_links2N;
  data += html_links3N;
  sendHtml(data);
#ifdef ENCODERS_ON
  data += html_linksEncN;
#endif
  data += html_links4N;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  sendHtml(data);

  data+="<div style='width: 27em;'>";

  data+="<b>Site:</b><br />";
  // Browser time
  data += html_settingsBrowserTime;

  // UTC Date
  if (!sendCommand(":GX81#",temp1)) strcpy(temp1,"?");
  sprintf(temp,html_indexDate,temp1);
  data += temp;

  // UTC Time
  if (!sendCommand(":GX80#",temp1)) strcpy(temp1,"?");
  sprintf(temp,html_indexTime,temp1);
  data += temp;

  // LST
  if (!sendCommand(":GS#",temp1)) strcpy(temp1,"?");
  sprintf(temp,html_indexSidereal,temp1);
  data += temp;

  // Longitude and Latitude
  if (!sendCommand(":Gg#",temp1)) strcpy(temp1,"?");
  if (!sendCommand(":Gt#",temp2)) strcpy(temp2,"?");
  sprintf(temp,html_indexSite,temp1,temp2);
  data += temp;
  sendHtml(data);

#ifdef AMBIENT_CONDITIONS_ON
  if (!sendCommand(":GX9A#",temp1)) strcpy(temp1,"?"); sprintf(temp,html_indexTPHD,"Temperature:",temp1,"&deg;C"); data+=temp;
  if (!sendCommand(":GX9B#",temp1)) strcpy(temp1,"?"); sprintf(temp,html_indexTPHD,"Barometric Pressure:",temp1,"mb"); data+=temp;
  if (!sendCommand(":GX9C#",temp1)) strcpy(temp1,"?"); sprintf(temp,html_indexTPHD,"Relative Humidity:",temp1,"%"); data+=temp;
  if (!sendCommand(":GX9E#",temp1)) strcpy(temp1,"?"); sprintf(temp,html_indexTPHD,"Dew Point Temperature:",temp1,"&deg;C"); data+=temp;
#endif

  data+="<br /><b>Coordinates:</b><br />";

#ifdef HIGH_PRECISION_COORDS_ON
  // RA,Dec current
  if (!sendCommand(":GRa#",temp1)) strcpy(temp1,"?");
  if (!sendCommand(":GDe#",temp2)) strcpy(temp2,"?");
  sprintf(temp,html_indexPosition,temp1,temp2); 
  data += temp;

  // RA,Dec target
  if (!sendCommand(":Gra#",temp1)) strcpy(temp1,"?");
  if (!sendCommand(":Gde#",temp2)) strcpy(temp2,"?");
  sprintf(temp,html_indexTarget,temp1,temp2); 
  data += temp;
#else
  // RA,Dec current
  if (!sendCommand(":GR#",temp1)) strcpy(temp1,"?");
  if (!sendCommand(":GD#",temp2)) strcpy(temp2,"?");
  sprintf(temp,html_indexPosition,temp1,temp2); 
  data += temp;

  // RA,Dec target
  if (!sendCommand(":Gr#",temp1)) strcpy(temp1,"?");
  if (!sendCommand(":Gd#",temp2)) strcpy(temp2,"?");
  sprintf(temp,html_indexTarget,temp1,temp2); 
  data += temp;
#endif

#ifdef ENCODERS_ON
  // RA,Dec OnStep position
  double f;
  f=encoders.getOnStepAxis1(); doubleToDms(temp1,&f,true,true);
  f=encoders.getOnStepAxis2(); doubleToDms(temp2,&f,true,true);
  sprintf(temp,html_indexEncoder1,temp1,temp2);
  data += temp;

  // RA,Dec encoder position
  f=encoders.getAxis1(); doubleToDms(temp1,&f,true,true);
  f=encoders.getAxis2(); doubleToDms(temp2,&f,true,true);
  sprintf(temp,html_indexEncoder2,temp1,temp2);
  data += temp;
#endif

  // pier side and meridian flips
  if ((mountStatus.pierSide()==PierSideFlipWE1) || (mountStatus.pierSide()==PierSideFlipWE2) || (mountStatus.pierSide()==PierSideFlipWE3)) strcpy(temp1,"Meridian Flip, West to East"); else
  if ((mountStatus.pierSide()==PierSideFlipEW1) || (mountStatus.pierSide()==PierSideFlipEW2) || (mountStatus.pierSide()==PierSideFlipEW3)) strcpy(temp1,"Meridian Flip, East to West"); else
  if (mountStatus.pierSide()==PierSideWest) strcpy(temp1,"West"); else
  if (mountStatus.pierSide()==PierSideEast) strcpy(temp1,"East"); else
  if (mountStatus.pierSide()==PierSideNone) strcpy(temp1,"None"); else strcpy(temp1,"Unknown");
  if (!mountStatus.valid()) strcpy(temp1,"?");
  if (mountStatus.meridianFlips()) {
    strcpy(temp2,"On");
    if (mountStatus.autoMeridianFlips()) strcat(temp2,"</font>, <font class=\"c\">Auto");
  } else strcpy(temp2,"Off");
  if (!mountStatus.valid()) strcpy(temp2,"?");
  sprintf(temp,html_indexPier,temp1,temp2);
  data += temp;
  sendHtml(data);

  long lat=LONG_MIN; if (sendCommand(":Gt#",temp1)) { temp1[3]=0; if (temp1[0]=='+') temp1[0]='0'; lat=strtol(temp1,NULL,10); }
  if (abs(lat)<=89) {
    long ud=LONG_MIN; if (sendCommand(":GX02#",temp1)) { ud=strtol(&temp1[0],NULL,10); if (lat<0) ud=-ud; }
    long lr=LONG_MIN; if (sendCommand(":GX03#",temp1)) { lr=strtol(&temp1[0],NULL,10); lr=lr/cos(lat/57.295); }

    if ((lat!=LONG_MIN) && (ud!=LONG_MIN) && (lr!=LONG_MIN)) {
      data+="<br /><b>Polar Alignment:</b><br />";

      // default to arc-minutes unless we get close, then arc-seconds
      char units='"';
      if ((abs(ud)>=300) || (abs(lr)>=300)) { 
        ud=ud/60.0; lr=lr/60.0;
        units='\'';
      }

      if (mountStatus.mountType()==MT_ALTAZM) {
        strcpy(temp1,"Zenith");
      } else {
        if (lat<0) strcpy(temp1,"SCP"); else strcpy(temp1,"NCP");
      }

      // show direction
      if ((ud< 0) && (lr< 0)) sprintf(temp,html_indexCorPolar,rightTri,(long)(abs(lr)),units,downTri,(long)(abs(ud)),units,temp1); else
      if ((ud>=0) && (lr< 0)) sprintf(temp,html_indexCorPolar,rightTri,(long)(abs(lr)),units,upTri  ,(long)(abs(ud)),units,temp1); else
      if ((ud< 0) && (lr>=0)) sprintf(temp,html_indexCorPolar,leftTri ,(long)(abs(lr)),units,downTri,(long)(abs(ud)),units,temp1); else
      if ((ud>=0) && (lr>=0)) sprintf(temp,html_indexCorPolar,leftTri ,(long)(abs(lr)),units,upTri  ,(long)(abs(ud)),units,temp1);

      data += temp;
    }
  }
  sendHtml(data);

  data+="<br /><b>Operations:</b><br />";

  // Park
  if (mountStatus.parked()) strcpy(temp1,"Parked"); else strcpy(temp1,"Not Parked");
  if (mountStatus.parking()) strcpy(temp1,"Parking"); else
  if (mountStatus.parkFail()) strcpy(temp1,"Park Failed");
  if (mountStatus.atHome()) strcat(temp1," </font>(<font class=\"c\">At Home</font>)<font class=\"c\">");
  if (!mountStatus.valid()) strcpy(temp1,"?");
  sprintf(temp,html_indexPark,temp1);
  data += temp;

  // Tracking
  if (mountStatus.tracking()) strcpy(temp1,"On"); else strcpy(temp1,"Off");
  if (mountStatus.slewing()) strcpy(temp1,"Slewing");
  if (!mountStatus.valid()) strcpy(temp1,"?");
  
  strcpy(temp2,"</font>(<font class=\"c\">");
  if (mountStatus.ppsSync()) strcat(temp2,"PPS Sync, ");
  if (mountStatus.rateCompensation()==RC_REFR_RA)   strcat(temp2,"Refr Comp RA Axis, ");
  if (mountStatus.rateCompensation()==RC_REFR_BOTH) strcat(temp2,"Refr Comp Both Axis, ");
  if (mountStatus.rateCompensation()==RC_FULL_RA)   strcat(temp2,"Full Comp RA Axis, ");
  if (mountStatus.rateCompensation()==RC_FULL_BOTH) strcat(temp2,"Full Comp Both Axis, ");
  if (!mountStatus.valid()) strcpy(temp2,"?");
  if (temp2[strlen(temp2)-2]==',') { temp2[strlen(temp2)-2]=0; strcat(temp2,"</font>)<font class=\"c\">"); } else strcpy(temp2,"");
  sprintf(temp,html_indexTracking,temp1,temp2);
  data += temp;
  sendHtml(data);

  // Tracking rate
  if ((sendCommand(":GT#",temp1)) && (strlen(temp1)>6)) {
    double tr=atof(temp1);
    sprintf(temp,"&nbsp;&nbsp;Tracking Rate: <font class=\"c\">%5.3f</font>Hz<br />",tr);
    data += temp;
  }

  // Slew speed
  if ((sendCommand(":GX97#",temp1)) && (strlen(temp1)>2)) {
    sprintf(temp,html_indexMaxSpeed,temp1);
    data += temp;
  } else {
    // fall back to MaxRate display if not supported
    if ((sendCommand(":GX92#",temp1)) && (sendCommand(":GX93#",temp2))) { 
      long maxRate=strtol(&temp1[0],NULL,10);
      long MaxRate=strtol(&temp2[0],NULL,10);
      sprintf(temp,html_indexMaxRate,maxRate,MaxRate);
    } else sprintf(temp,html_indexMaxSpeed,"?");
    data += temp;
  }
  sendHtml(data);

  data+="<br /><b>State:</b><br />";

  if (mountStatus.axisStatusValid()) {
    // Stepper driver status Axis1
    strcpy(temp1,"");
    if (mountStatus.axis1StSt()) strcat(temp1,"Standstill, "); else {
      if (mountStatus.axis1OLa() || mountStatus.axis1OLb()) {
        strcat(temp1,"Open Load ");
        if (mountStatus.axis1OLa()) strcat(temp1,"A");
        if (mountStatus.axis1OLb()) strcat(temp1,"B");
        strcat(temp1,", ");
      }
    }
    if (mountStatus.axis1S2Ga() || mountStatus.axis1S2Ga()) {
      strcat(temp1,"Short Gnd ");
      if (mountStatus.axis1S2Ga()) strcat(temp1,"A");
      if (mountStatus.axis1S2Gb()) strcat(temp1,"B");
      strcat(temp1,", ");
    }
    if (mountStatus.axis1OT()) strcat(temp1,"Shutdown Over 150C, ");
    if (mountStatus.axis1OTPW()) strcat(temp1,"Pre-warning &gt;120C, ");
    if (strlen(temp1)>2) temp1[strlen(temp1)-2]=0;
    if (strlen(temp1)==0) strcpy(temp1,"Ok");
    sprintf(temp,html_indexDriverStatus,temp1);
    data += "&nbsp;&nbsp;Axis1";
    data += temp;
  
    // Stepper driver status Axis2
    strcpy(temp1,"");
    if (mountStatus.axis2StSt()) strcat(temp1,"Standstill, "); else {
      if (mountStatus.axis2OLa() || mountStatus.axis2OLb()) {
        strcat(temp1,"Open Load ");
        if (mountStatus.axis2OLa()) strcat(temp1,"A");
        if (mountStatus.axis2OLb()) strcat(temp1,"B");
        strcat(temp1,", ");
      }
    }
    if (mountStatus.axis2S2Ga() || mountStatus.axis2S2Ga()) {
      strcat(temp1,"Short Gnd ");
      if (mountStatus.axis2S2Ga()) strcat(temp1,"A");
      if (mountStatus.axis2S2Gb()) strcat(temp1,"B");
      strcat(temp1,", ");
    }
    if (mountStatus.axis2OT()) strcat(temp1,"Shutdown Over 150C, ");
    if (mountStatus.axis2OTPW()) strcat(temp1,"Pre-warning &gt;120C, ");
    if (strlen(temp1)>2) temp1[strlen(temp1)-2]=0;
    if (strlen(temp1)==0) strcpy(temp1,"Ok");
    sprintf(temp,html_indexDriverStatus,temp1);
    data += "&nbsp;&nbsp;Axis2";
    data += temp;
  }

#ifdef INTERNAL_TEMPERATURE_ON
  if (!sendCommand(":GX9F#",temp1)) strcpy(temp1,"?"); sprintf(temp,html_indexTPHD,"Controller Internal Temperature:",temp1,"&deg;C"); data+=temp;
#endif

  // Last Error
  if (mountStatus.lastError()!=ERR_NONE) strcpy(temp1,"</font><font class=\"y\">"); else strcpy(temp1,"");
  mountStatus.getLastErrorMessage(temp2);
  strcat(temp1,temp2);
  if (!mountStatus.valid()) strcpy(temp1,"?");
  sprintf(temp,html_indexLastError,temp1);
  data += temp;

  // Loop time
  if (!sendCommand(":GXFA#",temp1)) strcpy(temp1,"?%");
  sprintf(temp,html_indexWorkload,temp1);
  data += temp;

  data += "</div><br class=\"clear\" />\r\n";
  data += "</div></body></html>";

  sendHtml(data);
  sendHtmlDone(data);
}
