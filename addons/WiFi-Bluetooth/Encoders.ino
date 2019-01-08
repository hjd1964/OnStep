// -----------------------------------------------------------------------------------
// Encoders

#ifdef ENCODERS_ON

const char html_encBeg[] = 
"";

const char html_encStart[] =
"";

const char html_encScript1[] =
"<script>"
"function s(key,v1) {"
  "var xhttp = new XMLHttpRequest();"
  "xhttp.open('GET', 'encA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);"
  "xhttp.send();"
"}"
"</script>\n";

const char html_encScript2[] =
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

const char html_syncOnStepNow[] =
"Sync OnStep to Encoders: <br />"
"<form method='get' action='/enc.htm'>"
"<button name='ms' value='ons' type='submit'>Now</button>"
"<br />";

const char html_syncEncodersNow[] =
"Sync Encoders to OnStep: <br />"
"<button name='ms' value='enc' type='submit'>Now</button>"
"</form><br />";

const char html_encEn1[] =
"Automatically sync. OnStep to Encoders (<span id='aste'>?</span>):<br />";
const char html_encEn2[] =
"<button type='button' onpointerdown=\"s('as','on')\" >On</button>"
"<button type='button' onpointerdown=\"s('as','off')\" >Off</button>"
"<br /><br />";

const char html_encMxAxis0[] =
"Max angular distance (Encoders vs. OnStep): <br />"
"<form method='get' action='/enc.htm'>";
const char html_encMxAxis1[] =
" <input value='%ld' type='number' name='d1' min='0' max='9999'>"
"<button type='submit'>Upload</button>"
" (Axis1, arc-sec 0 to 9999)"
"</form>";
const char html_encMxAxis2[] = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='d2' min='0' max='9999'>"
"<button type='submit'>Upload</button>"
" (Axis2, arc-sec 0 to 9999)"
"</form><br />";

const char html_encRateEn1[] =
"OnStep rate control (<span id='orc'>?</span>):<br />";
const char html_encRateEn2[] =
"<button type='button' onpointerdown=\"s('rc','on')\" >On</button>"
"<button type='button' onpointerdown=\"s('rc','off')\" >Off</button>"
"<br /><br />";

const char html_encStaAxis1[] =
"Encoder rate averaging, samples: <br />"
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='sa' min='1' max='999'>"
"<button type='submit'>Upload</button>"
" (STA 1 to 999)"
"</form>";
const char html_encLtaAxis1[] = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='la' min='1' max='999'>"
"<button type='submit'>Upload</button>"
" (LTA 1 to 999)"
"</form><br />";

const char html_encPropAxis1[] = 
"Encoder proportional response: <br />"
"<form method='get' action='/enc.htm'>"
" <input 'width: 75px' value='%ld' type='number' name='pr' min='50' max='5000'>"
"<button type='submit'>Upload</button>"
" (P 50 to 5000&#x25;)"
"</form><br />";

#ifndef AXIS1_ENC_AUTO_RATE_COMP_ON
const char html_encErc2Axis1[] = 
"Encoder rate comp: <br />"
"<form method='get' action='/enc.htm'>"
" <input style='width: 75px' value='%ld' type='number' name='er' min='-99999' max='99999'>"
"<button type='submit'>Upload</button>"
" (ERC &#xb1;99999 PPM, &#xb1;10&#x25;)"
"</form><br />";
#endif

#ifdef AXIS1_ENC_INTPOL_COS_ON
const char html_encIntPolPhaseAxis1[] = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='ip' min='0' max='255'>"
"<button type='submit'>Upload</button>"
" (Phase 1 to 256 counts)"
"</form>";

const char html_encIntPolMagAxis1[] = 
"<form method='get' action='/enc.htm'>"
" <input style='width: 75px' value='%ld' type='number' name='im' min='0' max='29999'>"
"<button type='submit'>Upload</button>"
" (Magnitude 0 to 29999 PPM, 0 to 3&#x25;)"
"</form><br />";
#endif

const char html_encSweepEn1[] =
"<br />&nbsp; Sweep (<span id='osc'>?</span>):<br />";
const char html_encSweepEn2[] =
"&nbsp; <button type='button' onpointerdown=\"s('sw','on')\" >On</button>"
"<button type='button' onpointerdown=\"s('sw','off')\" >Off</button>"
"<br /><br />";

const char html_encEnd[] = 
"</form>";

#ifdef OETHS
void handleEncoders(EthernetClient *client) {
#else
void handleEncoders() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
  
  char temp[300]="";
  
  processEncodersGet();

  sendHtmlStart();

  String data=html_headB;

  // active ajax page is: encAjax();
  data +="<script>var ajaxPage='enc.txt';</script>\n";
  data +=html_ajax_active;
  data +="<script>auto2Rate=2;</script>";
  sendHtml(data);

  data += html_encScript1;
  sendHtml(data);

#ifdef AXIS1_ENC_RATE_CONTROL_ON
  data += html_encScript2;
  sendHtml(data);
#endif

  // send a standard http response header
  data += html_main_cssB;
  data += html_main_css1;
  data += html_main_css2;
  data += html_main_css3;
  data += html_main_css4;
  data += html_main_css5;
  sendHtml(data);
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
  {
    char temp1[40]="";
    data += html_onstep_header1;
    if (mountStatus.getId(temp1)) data += temp1; else data += "?";
    data += html_onstep_header2;
    if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
    data += html_onstep_header3;
  }
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
#ifdef ENCODERS_ON
  data += html_linksEncS;
#endif
  data += html_links4N;
  sendHtml(data);
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  data += html_encBeg;

  data+="<div style='width: 35em;'>";

  data += html_syncOnStepNow;
  data += html_syncEncodersNow;

  // Autosync
  data += html_encEn1;
  data += html_encEn2;
  sendHtml(data);
  
  // Encoder sync thresholds
  sprintf(temp,html_encMxAxis0);
  data += temp;
  sprintf(temp,html_encMxAxis1,Axis1EncDiffLimit);
  data += temp;
  sprintf(temp,html_encMxAxis2,Axis2EncDiffLimit);
  data += temp;
  sendHtml(data);
  
#ifdef AXIS1_ENC_RATE_CONTROL_ON
  // OnStep rate control
  data+="<br />";
  data += html_encRateEn1;
  data += html_encRateEn2;

  // Encoder averaging (integration) samples
  sprintf(temp,html_encStaAxis1,Axis1EncStaSamples);
  data += temp;
  sprintf(temp,html_encLtaAxis1,Axis1EncLtaSamples);
  data += temp;
  sendHtml(data);

  // Encoder poportional response
  sprintf(temp,html_encPropAxis1,Axis1EncProp);
  data += temp;

  // Encoder rate compensation
#ifndef AXIS1_ENC_AUTO_RATE_COMP_ON
  long l=round((Axis1EncRateComp-1.0)*1000000.0);
  sprintf(temp,html_encErc2Axis1,l);
  data += temp;
#endif

#ifdef AXIS1_ENC_INTPOL_COS_ON
  // Encoder interpolation compensation
  sprintf(temp,html_encIntPolPhaseAxis1,Axis1EncIntPolPhase);
  data += temp;

  sprintf(temp,html_encIntPolMagAxis1,Axis1EncIntPolMag);
  data += temp;
#endif
  sendHtml(data);

  // Encoder status display
  sprintf(temp,"Axis1 rates (x Sidereal):<br />");
  data += temp;
  sprintf(temp,"&nbsp; OnStep = <span id='stO'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Delta &nbsp;= <span id='stD'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Guide &nbsp;= <span id='rtF'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Encoder STA = <span id='stS'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Encoder LTA = <span id='stL'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Intpol Comp = <span id='ipC'>?</span><br />");
  data += temp;
  sprintf(temp,"&nbsp; Intpol Phase = <span id='ipP'>?</span><br />");
  data += temp;
  sendHtml(data);

  data += "<br /><canvas id='myCanvas' width='600' height='300' style='margin-left: 8px; border:2px solid #999999;'></canvas>";
  data += "&nbsp; Center: OnStep rate, Blue: STA (range &#xb1;0.1), Green: LTA (range &#xb1;0.01)<br />";

  data += html_encSweepEn1;
  data += html_encSweepEn2;

  encoders.clearAverages();
#endif

  // end of page
  sprintf(temp,html_encEnd);
  data += temp;
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
  
#ifdef AXIS1_ENC_RATE_CONTROL_ON
  char temp[20]="";
  data += "stO|"; sprintf(temp,"%+1.4f\n",axis1Rate); data += temp;
  data += "stD|"; sprintf(temp,"%+1.4f\n",axis1Rate-axis1EncRateSta); data += temp;
  data += "stS|"; sprintf(temp,"%+1.4f\n",axis1EncRateSta); data += temp;
  data += "stL|"; sprintf(temp,"%+1.4f\n",axis1EncRateLta); data += temp;
  data += "ipC|"; sprintf(temp,"%+1.4f\n",intpolComp); data += temp;
  data += "ipP|"; sprintf(temp,"%d\n",(int)intpolPhase); data += temp;

if (finalCorrection==0) {
  data += "rtF|"; sprintf(temp,"None\n"); data += temp;
} else
if (finalCorrection>0) {
  data += "rtF|"; sprintf(temp,"West %ld ms\n",finalCorrection); data += temp;
} else
if (finalCorrection<0) {
  data += "rtF|"; sprintf(temp,"East %ld ms\n",-finalCorrection); data += temp;
}

  data += "orc|"; if (encRateControl) data+="On\n"; else data+="Off\n";
  data += "osc|"; if (encSweep) data+="On\n"; else data+="Off\n";
#endif
  data += "aste|";  if (encAutoSync) data+="On\n"; else data+="Off\n";

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
  char temp[20]="";
  
  v=server.arg("ms");
  if (v!="") {
    if (v=="ons") encoders.syncToOnStep();
    if (v=="enc") encoders.syncFromOnStep();
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
      Axis1EncDiffLimit=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(600,Axis1EncDiffLimit);
#endif
      EEwrite=true;
    }
  }
  v=server.arg("d2");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=9999))) { 
      Axis2EncDiffLimit=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(604,Axis2EncDiffLimit);
#endif
      EEwrite=true;
    }
  }

#ifdef AXIS1_ENC_RATE_CONTROL_ON
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
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(608,Axis1EncStaSamples);
#endif
      EEwrite=true;
    }
  }
  v=server.arg("la");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=1) && (i<=999))) { 
      Axis1EncLtaSamples=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(612,Axis1EncLtaSamples);
#endif
      EEwrite=true;
    }
  }

  // Encoder proportional response
  v=server.arg("pr");
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=50) && (i<=5000))) { 
      Axis1EncProp=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(632,Axis1EncProp);
#endif
      EEwrite=true;
    }
  }

  // Encoder rate compensation
  v=server.arg("er");
  if (v!="") {
    int l=0;
    l=strtol(v.c_str(),NULL,10);
    if ((l>=-99999) && (l<=99999)) {
      Axis1EncRateComp=1.0+(float)l/1000000.0;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(616,l);
#endif
      EEwrite=true;
    }
  }

#ifdef AXIS1_ENC_INTPOL_COS_ON
  // Encoder interpolation compensation
  v=server.arg("ip"); // phase
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=255))) { 
      Axis1EncIntPolPhase=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(624,i);
#endif
      EEwrite=true;
    }
  }
  v=server.arg("im"); // magnitude
  if (v!="") {
    int i;
    if ( (atoi2((char*)v.c_str(),&i)) && ((i>=0) && (i<=29000))) { 
      Axis1EncIntPolMag=i;
#ifndef EEPROM_DISABLED
      EEPROM_writeLong(628,i);
#endif
      EEwrite=true;
    }
  }
#endif // AXIS1_ENC_INTPOL_COS_ON

  // Sweep control
  v=server.arg("sw");
  if (v!="") {
    if (v=="on") encSweep=true;
    if (v=="off") encSweep=false;
  }

#endif // AXIS1_ENC_RATE_CONTROL_ON

#ifndef EEPROM_COMMIT_DISABLED
  if (EEwrite) EEPROM.commit();
#endif

  // clear any possible response
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}

#endif
