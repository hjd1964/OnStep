// The config.htm page ----------------------------------------------------------------------------------

const char html_config1[] = "<div class=\"t\"><table width=\"100%%\"><tr><td><b><font size=\"5\">%s</font></b></td><td align=\"right\"><b>" Product " " Version " (OnStep %s)</b>";
const char html_config2[] = "</td></tr></table>";
const char html_config3[] = "</div><div class=\"b\">\r\n";

const char html_debug[] = "Debug Mode: <font class=\"c\">%s</font><br /><br />";
const char html_sync[] = "Sync Anywhere: <font class=\"c\">%s</font><br /><br />";
const char html_configMount[] = "Mount type: <font class=\"c\">%s</font><br /><br />";
const char html_configST4[] = "ST4 interface <font class=\"c\">%s</font> (";
const char html_pinsST4[] = "at <font class=\"c\">%s</font>)<br /><br />";

const char html_configPPS[] = "PPS sense is <font class=\"c\">%s</font>, ";
const char html_configPEC[] = "PEC sense is <font class=\"c\">%s</font>, ";
const char html_configLimit[] = "LIMIT sense is <font class=\"c\">%s</font><br /><br />";
const char html_configLed1[] = "Status LED 1 <font class=\"c\">%s</font>, ";
const char html_configLed2[] = "Status LED 2 <font class=\"c\">%s</font>, ";
const char html_configLed3[] = "Reticule LED <font class=\"c\">%s</font><br /><br />";
const char html_configPwr[] = "Optional Logic Level Power to Stepper drivers <font class=\"c\">%s</font><br />";
const char html_configA1En[] = "Stepper driver Enable line support Axis1 (RA/Az) disabled <font class=\"c\">%s";
const char html_configA2En[] = "</font>, Axis2 (Dec/Alt) disabled <font class=\"c\">%s</font><br />";
const char html_configA1Flt[] = "Stepper driver Fault on Axis1 (RA/Az) <font class=\"c\">%s</font>, ";
const char html_configA2Flt[] = "Axis2 (Dec/Alt) <font class=\"c\">%s</font><br /><br />";
const char html_configTrack[] = "Tracking compensation for refraction effect, defaults to <font class=\"c\">%s</font><br /><br />";
const char html_configGuide[] = "Separate pulse guide rate <font class=\"c\">%s</font><br /><br />";
const char html_configRemMax[] = "Remember MaxRate setting changes <font class=\"c\">%s</font><br /><br />";

const char html_configMaxRate[]  = "MaxRate <font class=\"c\">%s</font>uS<br /><br />";
const char html_configDegAcc[] = "Degrees for acceleration/deceleration <font class=\"c\">%s</font><br />";
const char html_configBacklash[] = "Backlash takeup rate <font class=\"c\">%s</font> x Sidereal<br /><br />";
const char html_configStepsDegAx1[] = "Steps per degree for Axis1 (RA/Az) <font class=\"c\">%s</font><br />";
const char html_configStepsDegAx2[] = "Steps per degree for Axis2 (Dec/Alt) <font class=\"c\">%s</font><br />";
const char html_configStepsPerSec[] = "Steps per second for tracking <font class=\"c\">%s</font><br /><br />";
const char html_configStepsPerWR[] = "Steps per worm rotation for PEC <font class=\"c\">%s</font>, ";
const char html_configPecBufSize[] = "PEC buffer size <font class=\"c\">%s</font> seconds<br /><br />";
const char html_configUPL[] = "Under pole limit <font class=\"c\">%s</font> hours<br />";
const char html_configMinDec[] = "Minimum Declination <font class=\"c\">%s</font> degrees<br />";
const char html_configMaxDec[] = "Maximum Declination <font class=\"c\">%s</font> degrees<br /><br />";

void handleConfig() {
  Serial.setTimeout(WebTimeout);
  serialRecvFlush();
    
  char temp[320] = "";
  char temp1[80] = "";
  char temp2[80] = "";
  char temp3[80] = "";

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

  // finish the standard http response header
  Serial.print(":GVP#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; }
  Serial.print(":GVN#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0; 
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_config1,temp2,temp3);
  data += temp;
  data += html_config2;
  data += html_links1co;
  data += html_links2co;
  data += html_links3co;

  data += html_config3;

  Serial.print(":GXE0#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)<=0) { strcpy(temp1,"0000000000000000000"); }
  
  if (temp1[0]=='0') strcpy(temp2,"OFF"); else if (temp1[0]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_debug,temp2);
  data += temp;
  if (temp1[1]=='0') strcpy(temp2,"OFF"); else if (temp1[1]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_sync,temp2);
  data += temp;
  if (temp1[2]=='0') strcpy(temp2,"GEM"); else
  if (temp1[2]=='1') strcpy(temp2,"Fork"); else
  if (temp1[2]=='2') strcpy(temp2,"Fork (Alternate)"); else
  if (temp1[2]=='3') strcpy(temp2,"AltAzm"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configMount,temp2);
  data += temp;
  if (temp1[3]=='0') strcpy(temp2,"OFF"); else 
  if (temp1[3]=='1') strcpy(temp2,"ON"); else
  if (temp1[3]=='2') strcpy(temp2,"ON (internal pullups)"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configST4,temp2);
  data += temp;
  if (temp1[4]=='0') strcpy(temp2,"Default Pins"); else if (temp1[4]=='1') strcpy(temp2,"Alternate Pins"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_pinsST4,temp2);
  data += temp;
  if (temp1[5]=='0') strcpy(temp2,"OFF"); else
  if (temp1[5]=='1') strcpy(temp2,"ON"); else
  if (temp1[5]=='2') strcpy(temp2,"ON (internal pullup)"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configPPS,temp2);
  data += temp;
  if (temp1[6]=='0') strcpy(temp2,"OFF"); else
  if (temp1[6]=='1') strcpy(temp2,"ON"); else
  if (temp1[6]=='2') strcpy(temp2,"ON (internal pullup)"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configPEC,temp2);
  data += temp;
  if (temp1[7]=='0') strcpy(temp2,"OFF"); else
  if (temp1[7]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configLimit,temp2);
  data += temp;
  if (temp1[8]=='0') strcpy(temp2,"OFF"); else
  if (temp1[8]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configLed1,temp2);
  data += temp;
  if (temp1[9]=='0') strcpy(temp2,"OFF"); else 
  if (temp1[9]=='2') strcpy(temp2,"(Reticule)"); else
  if (temp1[9]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configLed2,temp2);
  data += temp;
  if (temp1[10]=='0') strcpy(temp2,"OFF"); else if (temp1[10]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configLed3,temp2);
  data += temp;
  if (temp1[11]=='0') strcpy(temp2,"OFF"); else if (temp1[11]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configPwr,temp2);
  data += temp;
  if (temp1[12]=='0') strcpy(temp2,"LOW"); else if (temp1[12]=='1') strcpy(temp2,"HIGH"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configA1En,temp2);
  data += temp;
  if (temp1[13]=='0') strcpy(temp2,"LOW"); else if (temp1[13]=='1') strcpy(temp2,"HIGH"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configA2En,temp2);
  data += temp;
  if (temp1[14]=='0') strcpy(temp2,"</font>when <font class=\"c\">LOW"); else 
  if (temp1[14]=='1') strcpy(temp2,"</font>when <font class=\"c\">HIGH"); else 
  if (temp1[14]=='2') strcpy(temp2,"</font>is <font class=\"c\">OFF"); else strcpy(temp2,"</font>is <font class=\"c\">UNKNOWN");
  sprintf(temp,html_configA1Flt,temp2);
  data += temp;
  if (temp1[15]=='0') strcpy(temp2,"</font>when <font class=\"c\">LOW"); else 
  if (temp1[15]=='1') strcpy(temp2,"</font>when <font class=\"c\">HIGH"); else 
  if (temp1[15]=='2') strcpy(temp2,"</font>is <font class=\"c\">OFF"); else strcpy(temp2,"</font>is <font class=\"c\">UNKNOWN");
  sprintf(temp,html_configA2Flt,temp2);
  data += temp;
  if (temp1[16]=='0') strcpy(temp2,"OFF"); else if (temp1[16]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configTrack,temp2);
  data += temp;
  if (temp1[17]=='0') strcpy(temp2,"OFF"); else if (temp1[17]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configGuide,temp2);
  data += temp;
  if (temp1[18]=='0') strcpy(temp2,"OFF"); else if (temp1[18]=='1') strcpy(temp2,"ON"); else strcpy(temp2,"UNKNOWN");
  sprintf(temp,html_configRemMax,temp2);
  data += temp;

  // MaxRate
  Serial.print(":GXE1#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0;
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configMaxRate,temp1);
  data += temp;
    
  // Deg. for Acc
  Serial.print(":GXE2#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configDegAcc,temp1);
  data += temp;

  // Backlash takeup rate
  Serial.print(":GXE3#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configBacklash,temp1);
  data += temp;

  // Steps per Degree Axis1
  Serial.print(":GXE4#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configStepsDegAx1,temp1);
  data += temp;

  // Steps per Degree Axis2
  Serial.print(":GXE5#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configStepsDegAx2,temp1);
  data += temp;

  // Steps per Second
  Serial.print(":GXE6#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configStepsPerSec,temp1);
  data += temp;

  // Steps per WR
  Serial.print(":GXE7#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configStepsPerWR,temp1);
  data += temp;

  // PEC Buffer size
  Serial.print(":GXE8#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configPecBufSize,temp1);
  data += temp;

  // Limit Underpole
  Serial.print(":GXEB#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configUPL,temp1);
  data += temp;

  // Limit MinDec
  Serial.print(":GXEC#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configMinDec,temp1);
  data += temp;

  // Limit MaxDec
  Serial.print(":GXED#");
  temp1[Serial.readBytesUntil('#',temp1,20)]=0; 
  if (strlen(temp1)<=0) { strcpy(temp1,"N/A"); }
  sprintf(temp,html_configMaxDec,temp1);
  data += temp;

  data += "</div></body></html>";
  server.send(200, "text/html",data);
}

