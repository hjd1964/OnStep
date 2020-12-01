// -----------------------------------------------------------------------------------
// Encoders

#if ENCODERS == ON

const char html_encScript1[] PROGMEM =
"<script>"
"function s(key,v1) {"
  "var xhttp = new XMLHttpRequest();"
  "xhttp.open('GET', 'encA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);"
  "xhttp.send();"
"}"
"</script>\n";

const char html_encScript2[] PROGMEM =
"<script>"
  "var autoG=setInterval(autoRunG,1000);"
  "var x=0;"
  "function autoRunG() {"
    "x=x+1; if (x>599) x=0;"
    "var c = document.getElementById('myCanvas');"
    "var ctx = c.getContext('2d');"
    
    "ctx.beginPath();"
    "ctx.strokeStyle='#666666';"
    "ctx.moveTo(0, 150); ctx.lineTo(600, 150);"
    "ctx.stroke();"

    "if (document.getElementById('osc').innerText=='On') {"
      "ctx.beginPath();"
      "ctx.strokeStyle='#111111';"
      "x1=x+1; if (x1>599) x1=0;"
      "ctx.moveTo(x1, 0); ctx.lineTo(x1, 300);"
      "ctx.stroke();"
    "};"

    "var stO=document.getElementById('stO').innerText;"

    "var Ylta=document.getElementById('stL').innerText;"
    "Ylta=(stO-Ylta)*10000.0;"
    "ctx.beginPath();"
    "ctx.fillStyle='#66ff66';"
    "ctx.fillRect(x,Ylta+150,3,3);"
    "ctx.stroke();"

    "var Ysta=document.getElementById('stS').innerText;"
    "Ysta=(stO-Ysta)*1000.0;"
    "ctx.beginPath();"
    "ctx.fillStyle='#6666ff';"
    "ctx.fillRect(x,Ysta+150,3,3);"
    "ctx.stroke();"

  "};"
"</script>\n";

const char html_syncOnStepNow[] PROGMEM =
L_ENC_SYNC_TO_ENC ": <br />"
"<form method='get' action='/enc.htm'>"
"<button name='ms' value='ons' type='submit'>" L_ENC_SYNC_NOW "</button>"
"<br />";

const char html_syncEncodersNow[] PROGMEM =
L_ENC_SYNC_TO_ONS ": <br />"
"<button name='ms' value='enc' type='submit'>" L_ENC_SYNC_NOW "</button>"
"<br />";

#ifdef ENC_HAS_ABSOLUTE
const char html_zeroEncodersNow[] PROGMEM =
L_ENC_ZERO_TO_ONS ": <br />"
"<button name='ms' value='zro' type='submit'>" L_ENC_SYNC_NOW "</button>"
"<br />";
#endif

const char html_encEn1[] PROGMEM = "<br /><br />"
L_ENC_AUTO_SYNC ": <br />";
const char html_encEn2[] PROGMEM =
"<button id='eas_on'  class='btns_right' onpointerdown=\"s('as','on')\"  type='button' disabled>" L_ON "</button>"
"<button id='eas_off' class='btns_left'  onpointerdown=\"s('as','off')\" type='button' disabled>" L_OFF "</button>"
"<br /><br />";

const char html_encMxAxis0[] PROGMEM =
L_ENC_MAX_ANGLE ": <br />"
"<form method='get' action='/enc.htm'>";
const char html_encMxAxis1[] PROGMEM =
" <input value='%ld' type='number' name='d1' min='0' max='9999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" " L_ENC_MAX_ANGLE_AXIS1
"</form>";
const char html_encMxAxis2[] PROGMEM = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='d2' min='0' max='9999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" " L_ENC_MAX_ANGLE_AXIS2
"</form><br />";

const char html_encRateEn1[] PROGMEM =
L_ENC_AUTO_RATE " (<span id='orc'>?</span>):<br />";
const char html_encRateEn2[] PROGMEM =
"<button type='button' onpointerdown=\"s('rc','on')\" >" L_ON "</button>"
"<button type='button' onpointerdown=\"s('rc','off')\" >" L_OFF "</button>"
"<br /><br />";

const char html_encStaAxis1[] PROGMEM =
#if AXIS1_ENC_BIN_AVG > 0
L_ENC_BINNED_SAMPLES ": <br />"
#else
L_ENC_SAMPLES ": <br />"
#endif
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='sa' min='1' max='999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (STA 1 to 999)"
"</form>";
const char html_encLtaAxis1[] PROGMEM = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='la' min='1' max='999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (LTA 1 to 999)"
"</form><br />";

const char html_encPropAxis1[] PROGMEM = 
L_ENC_PROPORTIONAL ": <br />"
"<form method='get' action='/enc.htm'>"
" <input 'width: 75px' value='%ld' type='number' name='pr' min='50' max='5000'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (P 50 to 5000&#x25;)"
"</form><br />";

const char html_encMinGuideAxis1[] PROGMEM = 
L_ENC_MIN_GUIDE ": <br />"
"<form method='get' action='/enc.htm'>"
" <input 'width: 75px' value='%ld' type='number' name='mg' min='25' max='1000'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (25 to 1000mS)"
"</form><br />";

#if AXIS1_ENC_RATE_AUTO == OFF
const char html_encErc2Axis1[] PROGMEM = 
L_ENC_RATE_COMP ": <br />"
"<form method='get' action='/enc.htm'>"
" <input style='width: 75px' value='%ld' type='number' name='er' min='-99999' max='99999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (ERC &#xb1;99999 PPM, &#xb1;10&#x25;)"
"</form><br />";
#endif

#if AXIS1_ENC_INTPOL_COS == ON
const char html_encIntPolPhaseAxis1[] PROGMEM = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='ip' min='0' max='255'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_ENC_PHASE " 1 to 256 " L_ENC_COUNTS ")"
"</form>";

const char html_encIntPolMagAxis1[] PROGMEM = 
"<form method='get' action='/enc.htm'>"
" <input style='width: 75px' value='%ld' type='number' name='im' min='0' max='29999'>"
"<button type='submit'>" L_UPLOAD "</button>"
" (" L_ENC_MAGN " 0 to 29999 PPM, 0 to 3&#x25;)"
"</form><br />";
#endif

const char html_encSweepEn1[] PROGMEM =
"<br />&nbsp; Sweep (<span id='osc'>?</span>):<br />";
const char html_encSweepEn2[] PROGMEM =
"&nbsp; <button type='button' onpointerdown=\"s('sw','on')\" >" L_ON "</button>"
"<button type='button' onpointerdown=\"s('sw','off')\" >" L_OFF "</button>"
"<br /><br />";

const char html_encEnd[] PROGMEM = 
"</form>";

#ifdef OETHS
void handleEncoders(EthernetClient *client) {
#else
void handleEncoders() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
  
  mountStatus.update();

  char temp[400]="";
  
  processEncodersGet();

  sendHtmlStart();

  String data=FPSTR(html_headB);

  // active ajax page is: encAjax();
  data +="<script>var ajaxPage='enc.txt';</script>\n";
  data +=FPSTR(html_ajax_active);
  data +="<script>auto2Rate=2;</script>";
  sendHtml(data);

  data += html_encScript1;
  sendHtml(data);

#if AXIS1_ENC_RATE_CONTROL == ON
  data += html_encScript2;
  sendHtml(data);
#endif

  // send a standard http response header
  data += FPSTR(html_main_cssB);
  data += FPSTR(html_main_css1);
  data += FPSTR(html_main_css2);
  data += FPSTR(html_main_css3);
  data += FPSTR(html_main_css4);
  data += FPSTR(html_main_css5);
  sendHtml(data);
  data += FPSTR(html_main_css6);
  data += FPSTR(html_main_css7);
  data += FPSTR(html_main_css8);
  data += FPSTR(html_main_css_btns1);
  sendHtml(data);
  data += FPSTR(html_main_css_btns2);
  data += FPSTR(html_main_css_btns3);
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  // finish the standard http response header
  {
    char temp1[40]="";
    data += FPSTR(html_onstep_header1); data += "OnStep";
    data += FPSTR(html_onstep_header2);
    if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
    data += FPSTR(html_onstep_header3);
  }
  data += FPSTR(html_linksStatN);
  data += FPSTR(html_linksCtrlN);
  if (mountStatus.featureFound()) data += FPSTR(html_linksAuxN);
  data += FPSTR(html_linksLibN);
#if ENCODERS == ON
  data += FPSTR(html_linksEncS);
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

  data+="<div style='width: 35em;'>";

  data += FPSTR(html_syncOnStepNow);
  data += FPSTR(html_syncEncodersNow);
#ifdef ENC_HAS_ABSOLUTE
  data += FPSTR(html_zeroEncodersNow);
#endif
  data += "</form>";

  // Autosync
  data += FPSTR(html_encEn1);
  data += FPSTR(html_encEn2);
  sendHtml(data);
  
  // Encoder sync thresholds
  data += FPSTR(html_encMxAxis0);
  sprintf_P(temp,html_encMxAxis1,Axis1EncDiffTo);
  data += temp;
  sprintf_P(temp,html_encMxAxis2,Axis2EncDiffTo);
  data += temp;
  sendHtml(data);
  
#if AXIS1_ENC_RATE_CONTROL == ON
  // OnStep rate control
  data+="<br />";
  data += FPSTR(html_encRateEn1);
  data += FPSTR(html_encRateEn2);

  // Encoder averaging (integration) samples
  sprintf_P(temp,html_encStaAxis1,Axis1EncStaSamples);
  data += temp;
  sprintf_P(temp,html_encLtaAxis1,Axis1EncLtaSamples);
  data += temp;
  sendHtml(data);

  // Encoder poportional response
  sprintf_P(temp,html_encPropAxis1,Axis1EncProp);
  data += temp;

  // Encoder minimum guide
  sprintf_P(temp,html_encMinGuideAxis1,Axis1EncMinGuide);
  data += temp;

  // Encoder rate compensation
#if AXIS1_ENC_RATE_AUTO == OFF
  long l=round(axis1EncRateComp*1000000.0);
  sprintf_P(temp,html_encErc2Axis1,l);
  data += temp;
#endif

#if AXIS1_ENC_INTPOL_COS == ON
  // Encoder interpolation compensation
  sprintf_P(temp,html_encIntPolPhaseAxis1,Axis1EncIntPolPhase);
  data += temp;

  sprintf_P(temp,html_encIntPolMagAxis1,Axis1EncIntPolMag);
  data += temp;
#endif
  sendHtml(data);

  // Encoder status display
  sprintf(temp,L_ENC_STAT_RATE_AXIS1 ":<br />");
  data += temp;
  sprintf(temp,"&nbsp; OnStep = <span id='stO'>?</span><br />");
  data += temp;
#if AXIS1_ENC_INTPOL_COS == ON
  sprintf(temp,"&nbsp; " L_ENC_STAT_INTPOL_COMP " = <span id='ipC'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; " L_ENC_STAT_INTPOL_PHASE " = <span id='ipP'>?</span><br />");
  data += temp;
#endif
#if AXIS1_ENC_RATE_AUTO > 0
  sprintf(temp,"&nbsp; " L_ENCODER " ARC = <span id='erA'>?</span><br />");
  data += temp;
#endif
  sprintf(temp,"&nbsp; " L_ENCODER " STA = <span id='stS'>?</span> x<br />");
  data += temp;
  sprintf(temp,"&nbsp; " L_ENCODER " LTA = <span id='stL'>?</span> x<br />");
  data += temp;
  sprintf(temp,"&nbsp; " L_DELTA " &nbsp;= <span id='stD'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; " L_GUIDE " &nbsp;= <span id='rtF'>?</span><br />");
  data += temp;

  sendHtml(data);

  data += "<br /><canvas id='myCanvas' width='600' height='300' style='margin-left: -2px; border:2px solid #999999;'></canvas>";
  data += "&nbsp; " L_CENTER ": OnStep " L_RATE ", " L_BLUE ": STA (" L_RANGE " &#xb1;0.1), " L_GREEN ": LTA (" L_RANGE " &#xb1;0.01)<br />";

  data += FPSTR(html_encSweepEn1);
  data += FPSTR(html_encSweepEn2);

  encoders.clearAverages();
#endif

  // end of page
  data+="<br />";

  strcpy(temp,"</div></div></body></html>");
  data += temp;
  sendHtml(data);

  sendHtmlDone(data);
}

#ifdef OETHS
void encAjax(EthernetClient *client) {
#else
void encAjax() {
#endif
  String data="";
  
#if AXIS1_ENC_RATE_CONTROL == ON
  char temp[20]="";
  data += "stO|"; sprintf(temp,"%+1.4f\n",axis1Rate); data += temp;
  data += "stD|"; sprintf(temp,"%+1.4f\n",axis1Rate-axis1EncRateSta); data += temp;
  data += "stS|"; sprintf(temp,"%+1.4f\n",axis1EncRateSta); data += temp;
  data += "stL|"; sprintf(temp,"%+1.4f\n",axis1EncRateLta); data += temp;
#if AXIS1_ENC_INTPOL_COS == ON
  data += "ipC|"; sprintf(temp,"%+1.4f\n",intpolComp); data += temp;
  data += "ipP|"; sprintf(temp,"%d\n",(int)intpolPhase); data += temp;
#endif
#if AXIS1_ENC_RATE_AUTO > 0
  data += "erA|"; sprintf(temp,"%+1.5f\n",axis1EncRateComp); data += temp;
#endif

if (guideCorrectionMillis==0) {
  data += "rtF|"; sprintf(temp,L_NONE "\n"); data += temp;
} else
if (guideCorrectionMillis>0) {
  data += "rtF|"; sprintf(temp,L_WEST " %ld ms\n",guideCorrectionMillis); data += temp;
} else
if (guideCorrectionMillis<0) {
  data += "rtF|"; sprintf(temp,L_EAST " %ld ms\n",-guideCorrectionMillis); data += temp;
}

  data += "orc|"; if (encRateControl) data+=L_ON "\n"; else data+=L_OFF "\n";
  data += "osc|"; if (encSweep) data+=L_ON "\n"; else data+=L_OFF "\n";
#endif

  data += "eas_on|";  if (encAutoSync) data+="disabled"; else data+="enabled"; data+="\n";
  data += "eas_off|"; if (encAutoSync) data+="enabled"; else data+="disabled"; data+="\n";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

#ifdef OETHS
void encAjaxGet(EthernetClient *client) {
#else
void encAjaxGet() {
#endif
  processEncodersGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

void processEncodersGet() {
  boolean EEwrite=false;
  String v;
  
  v=server.arg("ms");
  if (v!="") {
    if (v=="ons") encoders.syncToOnStep();
    if (v=="enc") encoders.syncFromOnStep();
#ifdef ENC_HAS_ABSOLUTE
    if (v=="zro") encoders.zeroFromOnStep();
#endif
  }

  // Autosync
  v=server.arg("as");
  if (v!="") {
    if (v=="on") encAutoSync=true;
    if (v=="off") encAutoSync=false;
  }

  // Max. limits
  v=server.arg("d1");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=9999))) { 
      Axis1EncDiffTo=i;
      nv.writeLong(EE_ENC_A1_DIFF_TO,Axis1EncDiffTo);
      EEwrite=true;
    }
  }
  v=server.arg("d2");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=9999))) { 
      Axis2EncDiffTo=i;
      nv.writeLong(EE_ENC_A2_DIFF_TO,Axis2EncDiffTo);
      EEwrite=true;
    }
  }

#if AXIS1_ENC_RATE_CONTROL == ON
  // OnStep rate control
  v=server.arg("rc");
  if (v!="") {
    if (v=="on") encRateControl=true;
    if (v=="off") encRateControl=false;
  }

  // Encoder averaging samples
  v=server.arg("sa");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=1) && (i<=999))) { 
      Axis1EncStaSamples=i;
      nv.writeLong(EE_ENC_RC_STA,Axis1EncStaSamples);
      EEwrite=true;
    }
  }
  v=server.arg("la");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=1) && (i<=999))) { 
      Axis1EncLtaSamples=i;
      nv.writeLong(EE_ENC_RC_LTA,Axis1EncLtaSamples);
      EEwrite=true;
    }
  }

  // Encoder proportional response
  v=server.arg("pr");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=50) && (i<=5000))) { 
      Axis1EncProp=i;
      nv.writeLong(EE_ENC_RC_PROP,Axis1EncProp);
      EEwrite=true;
    }
  }
  
  // Encoder minimum guide
  v=server.arg("mg");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=25) && (i<=1000))) { 
      Axis1EncMinGuide=i;
      nv.writeLong(EE_ENC_MIN_GUIDE,Axis1EncMinGuide);
      EEwrite=true;
    }
  }

  // Encoder rate compensation
  v=server.arg("er");
  if (v!="") {
    int l=0;
    l=strtol(v.c_str(),NULL,10);
    if ((l>=-99999) && (l<=99999)) {
      axis1EncRateComp=(float)l/1000000.0;
      nv.writeLong(EE_ENC_RC_RCOMP,l);
      EEwrite=true;
    }
  }

#if AXIS1_ENC_INTPOL_COS == ON
  // Encoder interpolation compensation
  v=server.arg("ip"); // phase
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=255))) { 
      Axis1EncIntPolPhase=i;
      nv.writeLong(EE_ENC_RC_INTP_P,i);
      EEwrite=true;
    }
  }
  v=server.arg("im"); // magnitude
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=29000))) { 
      Axis1EncIntPolMag=i;
      nv.writeLong(EE_ENC_RC_INTP_M,i);
      EEwrite=true;
    }
  }
#endif // AXIS1_ENC_INTPOL_COS == ON

  // Sweep control
  v=server.arg("sw");
  if (v!="") {
    if (v=="on") encSweep=true;
    if (v=="off") encSweep=false;
  }

#endif // AXIS1_ENC_RATE_CONTROL == ON

  nv.commit();
}

#endif
