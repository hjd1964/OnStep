// -----------------------------------------------------------------------------------
// The home page, status information

#define leftTri  "&#x25c4;"
#define rightTri "&#x25ba;"
#define upTri    "&#x25b2;"
#define downTri  "&#x25bc;"

#if DISPLAY_ADVANCED_CHARS == ON
  #define Axis1 "&alpha;"
  #define Axis1A "&alpha;"
  #define Axis2 "&delta;"
#else
  #define Axis1 "RA"
  #define Axis1A "RA"
  #define Axis2 "DEC"
#endif

const char html_settingsBrowserTime[] PROGMEM =
"&nbsp;&nbsp;<span id=\"datetime\"></span> " L_TIME_BROWSER_UT
"<script> "
"function pad(num, size) { var s = '000000000' + num; return s.substr(s.length-size); }"
"var now = new Date(); document.getElementById('datetime').innerHTML = (now.getUTCMonth()+1)+'/'+"
"pad(now.getUTCDate().toString(),2)+'/'+pad(now.getUTCFullYear().toString().substr(-2),2)+"
"' '+pad(now.getUTCHours().toString(),2)+':'+pad(now.getUTCMinutes().toString(),2)+':'+pad(now.getUTCSeconds().toString(),2); "
"</script><br />\r\n";

const char html_indexDate[] PROGMEM = "&nbsp;&nbsp;<font class='c'>%s</font>";
const char html_indexTime[] PROGMEM = "&nbsp;<font class='c'>%s</font>&nbsp;" L_UT;
const char html_indexSidereal[] PROGMEM = "&nbsp;(<font class='c'>%s</font>&nbsp; " L_LST ")<br />";
const char html_indexSite[] PROGMEM = "&nbsp;&nbsp;" L_LONG " = <font class='c'>%s</font>, " L_LAT " = <font class='c'>%s</font><br />";

const char html_indexPosition[] PROGMEM = "&nbsp;&nbsp;" L_CURRENT ": " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
const char html_indexTarget[] PROGMEM = "&nbsp;&nbsp;" L_TARGET ": " Axis1 "=<font class='c'>%s</font>, " Axis2 "=<font class='c'>%s</font><br />";
#if ENCODERS == ON
const char html_indexEncoder1[] PROGMEM = "&nbsp;&nbsp;OnStep: Ax1=<font class='c'>%s</font>, Ax2=<font class='c'>%s</font><br />";
const char html_indexEncoder2[] PROGMEM = "&nbsp;&nbsp;" L_ENCODER ": Ax1=<font class='c'>%s</font>, Ax2=<font class='c'>%s</font><br />";
#endif
const char html_indexPier[] PROGMEM = "&nbsp;&nbsp;" L_PIER_SIDE "=<font class='c'>%s</font> (" L_MERIDIAN_FLIPS " <font class='c'>%s</font>)<br />";

const char html_indexCorPolar[] PROGMEM = "&nbsp;&nbsp;%s <font class='c'>%ld</font>%c &nbsp; %s <font class='c'>%ld</font>%c &nbsp;(" L_ALIGN_MESSAGE " %s)<br />";

const char html_indexPark[] PROGMEM = "&nbsp;&nbsp;" L_PARKING ": <font class='c'>%s</font><br />";
const char html_indexTracking[] PROGMEM = "&nbsp;&nbsp;" L_TRACKING ": <font class='c'>%s %s</font><br />";
const char html_indexMaxRate[] PROGMEM = "&nbsp;&nbsp;" L_CURRENT_MAXRATE ": <font class='c'>%ld</font> (" L_DEFAULT_MAXRATE ": <font class='c'>%ld</font>)<br />";
const char html_indexMaxSpeed[] PROGMEM = "&nbsp;&nbsp;" L_CURRENT_MAXSLEW ": <font class='c'>%s</font>&deg;/s<br />";

const char html_indexTPHD[] PROGMEM = "&nbsp;&nbsp;%s <font class='c'>%s</font>%s<br />";

const char html_indexDriverStatus[] PROGMEM = " " L_DRIVER_STATUS ": <font class='c'>%s</font><br />";
const char html_indexGeneralError[] PROGMEM = "&nbsp;&nbsp;" L_LAST_GENERAL_ERROR ": <font class='c'>%s</font><br />";
const char html_indexCmdErrorLog[] PROGMEM = "&nbsp;&nbsp;&nbsp;&nbsp;%s %s<br />";
const char html_indexWorkload[] PROGMEM = "&nbsp;&nbsp;" L_WORKLOAD ": <font class='c'>%s</font><br />";
#if DISPLAY_WIFI_SIGNAL_STRENGTH == ON
const char html_indexSignalStrength[] PROGMEM = "&nbsp;&nbsp;" L_WIRELESS_SIGNAL_STRENGTH ": <font class=\"c\">%s</font><br />";
#endif

#ifdef OETHS
void handleRoot(EthernetClient *client) {
#else
void handleRoot() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();

  mountStatus.update(true);

  char temp[400]="";
  char temp1[120]="";
  char temp2[120]="";

  sendHtmlStart();

  String data=FPSTR(html_headB);
  data += FPSTR(FPSTR(html_headerIdx)); // page refresh
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  data += FPSTR(html_main_css5);
  data += FPSTR(html_main_css6);
  sendHtml(data);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_css_control1);
  data += FPSTR(html_main_css_control2);
  data += FPSTR(html_main_css_control3);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  // finish the standard http response header
  data += FPSTR(html_onstep_header1); data += "OnStep";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_linksStatS);
  data += FPSTR(html_linksCtrlN);
  if (mountStatus.featureFound()) data += FPSTR(html_linksAuxN);
  data += FPSTR(html_linksLibN);
#if ENCODERS == ON
  data += FPSTR(html_linksEncN);
#endif
  sendHtml(data);
  data += FPSTR(html_linksPecN);
  data += FPSTR(html_linksSetN);
  data += FPSTR(html_linksCfgN);
#ifndef OETHS
  data += FPSTR(html_linksWifiN);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  data+="<div style='width: 27em;'>";

  data+="<b>" L_SITE ":</b><br />";
  // Browser time
  data += FPSTR(html_settingsBrowserTime);

  // UTC Date
  if (!command(":GX81#",temp1)) strcpy(temp1,"?");
  stripNum(temp1);
  sprintf_P(temp,html_indexDate,temp1);
  data += temp;

  // UTC Time
  if (!command(":GX80#",temp1)) strcpy(temp1,"?");
  sprintf_P(temp,html_indexTime,temp1);
  data += temp;

  // LST
  if (!command(":GS#",temp1)) strcpy(temp1,"?");
  sprintf_P(temp,html_indexSidereal,temp1);
  data += temp;

  // Longitude and Latitude
  if (!command(":GgH#",temp1)) strcpy(temp1,"?"); temp1[10]=0;
  if (!command(":GtH#",temp2)) strcpy(temp2,"?"); temp2[9]=0;
  sprintf_P(temp,html_indexSite,temp1,temp2);
  data += temp;
  sendHtml(data);

  // Ambient conditions
#if DISPLAY_WEATHER == ON
  if (!command(":GX9A#",temp1)) strcpy(temp1,"?"); else localeTemperature(temp1,temp2); sprintf_P(temp,html_indexTPHD,L_TEMPERATURE ":",temp1,temp2); data+=temp;
  if (!command(":GX9B#",temp1)) strcpy(temp1,"?"); else localePressure(temp1,temp2); sprintf_P(temp,html_indexTPHD,L_PRESSURE ":",temp1,temp2); data+=temp;
  if (!command(":GX9C#",temp1)) strcpy(temp1,"?"); sprintf_P(temp,html_indexTPHD,L_HUMIDITY ":",temp1,"%"); data+=temp;
  if (!command(":GX9E#",temp1)) strcpy(temp1,"?"); else localeTemperature(temp1,temp2); sprintf_P(temp,html_indexTPHD,L_DEW_POINT ":",temp1,temp2); data+=temp;
#endif

  // Focuser/telescope temperature
  if (mountStatus.focuserPresent()) {
    if (!command(":Ft#",temp1)) strcpy(temp1,"?"); else localeTemperature(temp1,temp2); sprintf_P(temp,html_indexTPHD,L_TELE_TEMPERATURE ":",temp1,temp2); data+=temp;
  }
  
  data+="<br /><b>" L_COORDINATES ":</b><br />";

#if DISPLAY_HIGH_PRECISION_COORDS == ON
  // RA,Dec current
  if (!command(":GRa#",temp1)) strcpy(temp1,"?");
  if (!command(":GDe#",temp2)) strcpy(temp2,"?");
  sprintf_P(temp,html_indexPosition,temp1,temp2); 
  data += temp;

  // RA,Dec target
  if (!command(":Gra#",temp1)) strcpy(temp1,"?");
  if (!command(":Gde#",temp2)) strcpy(temp2,"?");
  sprintf_P(temp,html_indexTarget,temp1,temp2); 
  data += temp;
#else
  // RA,Dec current
  if (!command(":GR#",temp1)) strcpy(temp1,"?");
  if (!command(":GD#",temp2)) strcpy(temp2,"?");
  sprintf_P(temp,html_indexPosition,temp1,temp2); 
  data += temp;

  // RA,Dec target
  if (!command(":Gr#",temp1)) strcpy(temp1,"?");
  if (!command(":Gd#",temp2)) strcpy(temp2,"?");
  sprintf_P(temp,html_indexTarget,temp1,temp2); 
  data += temp;
#endif

#if ENCODERS == ON
  // RA,Dec OnStep position
  double f;
  f=encoders.getOnStepAxis1(); doubleToDms(temp1,&f,true,true);
  f=encoders.getOnStepAxis2(); doubleToDms(temp2,&f,true,true);
  sprintf_P(temp,html_indexEncoder1,temp1,temp2);
  data += temp;

  // RA,Dec encoder position
  if (encoders.validAxis1()) { f=encoders.getAxis1(); doubleToDms(temp1,&f,true,true); } else strcpy(temp1," ** " L_FAULT " ** ");
  if (encoders.validAxis2()) { f=encoders.getAxis2(); doubleToDms(temp2,&f,true,true); } else strcpy(temp2," ** " L_FAULT " ** ");
  sprintf_P(temp,html_indexEncoder2,temp1,temp2);
  data += temp;
#endif

  // pier side and meridian flips
  if ((mountStatus.pierSide()==PierSideFlipWE1) || (mountStatus.pierSide()==PierSideFlipWE2) || (mountStatus.pierSide()==PierSideFlipWE3)) strcpy(temp1,L_MERIDIAN_FLIP_W_TO_E); else
  if ((mountStatus.pierSide()==PierSideFlipEW1) || (mountStatus.pierSide()==PierSideFlipEW2) || (mountStatus.pierSide()==PierSideFlipEW3)) strcpy(temp1,L_MERIDIAN_FLIP_E_TO_W); else
  if (mountStatus.pierSide()==PierSideWest) strcpy(temp1,L_WEST); else
  if (mountStatus.pierSide()==PierSideEast) strcpy(temp1,L_EAST); else
  if (mountStatus.pierSide()==PierSideNone) strcpy(temp1,L_NONE); else strcpy(temp1,L_UNKNOWN);
  if (!mountStatus.valid()) strcpy(temp1,"?");
  if (mountStatus.meridianFlips()) {
    strcpy(temp2,"On");
    if (mountStatus.autoMeridianFlips()) strcat(temp2,"</font>, <font class=\"c\">" L_AUTO);
  } else strcpy(temp2,"Off");
  if (!mountStatus.valid()) strcpy(temp2,"?");
  sprintf_P(temp,html_indexPier,temp1,temp2);
  data += temp;
  sendHtml(data);

  long lat=LONG_MIN; if (command(":Gt#",temp1)) { temp1[3]=0; if (temp1[0]=='+') temp1[0]='0'; lat=strtol(temp1,NULL,10); }
  if (abs(lat)<=89) {
    long ud=LONG_MIN; if (command(":GX02#",temp1)) { ud=strtol(&temp1[0],NULL,10); if (lat<0) ud=-ud; }
    long lr=LONG_MIN; if (command(":GX03#",temp1)) { lr=strtol(&temp1[0],NULL,10); lr=lr/cos(lat/57.295); }

    if ((lat!=LONG_MIN) && (ud!=LONG_MIN) && (lr!=LONG_MIN)) {
      data+="<br /><b>" L_POLAR_ALIGN ":</b><br />";

      // default to arc-minutes unless we get close, then arc-seconds
      char units='"';
      if ((abs(ud)>=300) || (abs(lr)>=300)) { 
        ud=ud/60.0; lr=lr/60.0;
        units='\'';
      }

      if (mountStatus.mountType()==MT_ALTAZM) {
        strcpy(temp1,L_ZENITH);
      } else {
        if (lat<0) strcpy(temp1,L_SCP); else strcpy(temp1,L_NCP);
      }

      // show direction
      if ((ud< 0) && (lr< 0)) sprintf_P(temp,html_indexCorPolar,rightTri,(long)(abs(lr)),units,downTri,(long)(abs(ud)),units,temp1); else
      if ((ud>=0) && (lr< 0)) sprintf_P(temp,html_indexCorPolar,rightTri,(long)(abs(lr)),units,upTri  ,(long)(abs(ud)),units,temp1); else
      if ((ud< 0) && (lr>=0)) sprintf_P(temp,html_indexCorPolar,leftTri ,(long)(abs(lr)),units,downTri,(long)(abs(ud)),units,temp1); else
      if ((ud>=0) && (lr>=0)) sprintf_P(temp,html_indexCorPolar,leftTri ,(long)(abs(lr)),units,upTri  ,(long)(abs(ud)),units,temp1);

      data += temp;
    }
  }
  sendHtml(data);

  data+="<br /><b>" L_OPERATIONS ":</b><br />";

  // Park
  if (mountStatus.parked()) strcpy(temp1,L_PARKED); else strcpy(temp1,L_NOT_PARKED);
  if (mountStatus.parking()) strcpy(temp1,L_PARKING); else
  if (mountStatus.parkFail()) strcpy(temp1,L_PARK_FAILED);
  if (mountStatus.atHome()) strcat(temp1," </font>(<font class=\"c\">" L_AT_HOME "</font>)<font class=\"c\">");
  if (!mountStatus.valid()) strcpy(temp1,"?");
  sprintf_P(temp,html_indexPark,temp1);
  data += temp;

  // Tracking
  if (mountStatus.tracking()) strcpy(temp1,L_ON); else strcpy(temp1,L_OFF);
  if (mountStatus.slewing()) strcpy(temp1,L_SLEWING);
  if (!mountStatus.valid()) strcpy(temp1,"?");
  
  strcpy(temp2,"</font>(<font class=\"c\">");
  if (mountStatus.ppsSync()) strcat(temp2,L_PPS_SYNC ", ");
  if (mountStatus.rateCompensation()==RC_REFR_RA)   strcat(temp2,L_REFR_COMP_RA ", ");
  if (mountStatus.rateCompensation()==RC_REFR_BOTH) strcat(temp2,L_REFR_COMP_BOTH ", ");
  if (mountStatus.rateCompensation()==RC_FULL_RA)   strcat(temp2,L_FULL_COMP_RA ", ");
  if (mountStatus.rateCompensation()==RC_FULL_BOTH) strcat(temp2,L_FULL_COMP_BOTH ", ");
  if (!mountStatus.valid()) strcpy(temp2,"?");
  if (temp2[strlen(temp2)-2]==',') { temp2[strlen(temp2)-2]=0; strcat(temp2,"</font>)<font class=\"c\">"); } else strcpy(temp2,"");
  sprintf_P(temp,html_indexTracking,temp1,temp2);
  data += temp;
  sendHtml(data);

  // Tracking rate
  if ((command(":GT#",temp1)) && (strlen(temp1)>6)) {
    double tr=atof(temp1);
    dtostrf(tr,5,3,temp1);
    sprintf(temp,"&nbsp;&nbsp;" L_TRACKING_RATE ": <font class=\"c\">%s</font>Hz<br />",temp1);
    data += temp;
  }

  // Slew speed
  if ((command(":GX97#",temp1)) && (strlen(temp1)>2)) {
    sprintf_P(temp,html_indexMaxSpeed,temp1);
    data += temp;
  } else {
    // fall back to MaxRate display if not supported
    if ((command(":GX92#",temp1)) && (command(":GX93#",temp2))) { 
      long maxRate=strtol(&temp1[0],NULL,10);
      long MaxRate=strtol(&temp2[0],NULL,10);
      sprintf_P(temp,html_indexMaxRate,maxRate,MaxRate);
    } else sprintf_P(temp,html_indexMaxSpeed,"?");
    data += temp;
  }
  sendHtml(data);

  data+="<br /><b>" L_STATE ":</b><br />";

  if (mountStatus.axisStatusValid()) {
    // Stepper driver status Axis1
    strcpy(temp1,"");
    if (mountStatus.axis1Comms()) strcat(temp1,L_COMMS_FAILURE  ", ");
    if (mountStatus.axis1StSt()) strcat(temp1,L_STANDSTILL ", "); else {
      if (mountStatus.axis1OLa() || mountStatus.axis1OLb()) {
        strcat(temp1,L_OPEN_LOAD " ");
        if (mountStatus.axis1OLa()) strcat(temp1,"A");
        if (mountStatus.axis1OLb()) strcat(temp1,"B");
        strcat(temp1,", ");
      }
    }
    if (mountStatus.axis1S2Ga() || mountStatus.axis1S2Ga()) {
      strcat(temp1,L_SHORT_GND " ");
      if (mountStatus.axis1S2Ga()) strcat(temp1,"A");
      if (mountStatus.axis1S2Gb()) strcat(temp1,"B");
      strcat(temp1,", ");
    }
    if (mountStatus.axis1OT()) strcat(temp1,L_SHUTDOWN_OVER " 150C, ");
    if (mountStatus.axis1OTPW()) strcat(temp1,L_PRE_WARNING " &gt;120C, ");
    if (strlen(temp1)>2) temp1[strlen(temp1)-2]=0;
    if (strlen(temp1)==0) strcpy(temp1,"Ok");
    sprintf_P(temp,html_indexDriverStatus,temp1);
    data += "&nbsp;&nbsp;Axis1";
    data += temp;
  
    // Stepper driver status Axis2
    strcpy(temp1,"");
    if (mountStatus.axis2Comms()) strcat(temp1,L_COMMS_FAILURE ", ");
    if (mountStatus.axis2StSt()) strcat(temp1,L_STANDSTILL ", "); else {
      if (mountStatus.axis2OLa() || mountStatus.axis2OLb()) {
        strcat(temp1,L_OPEN_LOAD " ");
        if (mountStatus.axis2OLa()) strcat(temp1,"A");
        if (mountStatus.axis2OLb()) strcat(temp1,"B");
        strcat(temp1,", ");
      }
    }
    if (mountStatus.axis2S2Ga() || mountStatus.axis2S2Ga()) {
      strcat(temp1,L_SHORT_GND " ");
      if (mountStatus.axis2S2Ga()) strcat(temp1,"A");
      if (mountStatus.axis2S2Gb()) strcat(temp1,"B");
      strcat(temp1,", ");
    }
    if (mountStatus.axis2OT()) strcat(temp1,L_SHUTDOWN_OVER " 150C, ");
    if (mountStatus.axis2OTPW()) strcat(temp1,L_PRE_WARNING " &gt;120C, ");
    if (strlen(temp1)>2) temp1[strlen(temp1)-2]=0;
    if (strlen(temp1)==0) strcpy(temp1,"Ok");
    sprintf_P(temp,html_indexDriverStatus,temp1);
    data += "&nbsp;&nbsp;Axis2";
    data += temp;
  }

#if DISPLAY_INTERNAL_TEMPERATURE == ON
  if (!command(":GX9F#",temp1)) strcpy(temp1,"?"); else localeTemperature(temp1,temp2); sprintf_P(temp,html_indexTPHD,L_INTERNAL_TEMP ":",temp1,temp2); data+=temp;
#endif

  // General Error
  if (mountStatus.lastError()!=ERR_NONE) strcpy(temp1,"</font><font class=\"y\">"); else strcpy(temp1,"");
  mountStatus.getLastErrorMessage(temp2);
  strcat(temp1,temp2);
  if (!mountStatus.valid()) strcpy(temp1,"?");
  sprintf_P(temp,html_indexGeneralError,temp1);
  data += temp;

  // Loop time
  if (!command(":GXFA#",temp1)) strcpy(temp1,"?%");
  sprintf_P(temp,html_indexWorkload,temp1);
  data += temp;

#if DISPLAY_WIFI_SIGNAL_STRENGTH == ON
  long signal_strength_dbm=WiFi.RSSI();
  int signal_strength_qty=2*(signal_strength_dbm+100);
  if (signal_strength_qty>100) signal_strength_qty=100; 
  else if (signal_strength_qty<0) signal_strength_qty=0;
  sprintf(temp1,"%idBm (%i%%)",signal_strength_dbm,signal_strength_qty);
  sprintf_P(temp,html_indexSignalStrength,temp1);
  data += temp;
#endif
  data += "</div><br class=\"clear\" />\r\n";
  data += "</div></body></html>";

  sendHtml(data);
  sendHtmlDone(data);
}
