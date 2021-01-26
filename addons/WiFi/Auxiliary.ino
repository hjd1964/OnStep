// -----------------------------------------------------------------------------------
// Telescope auxiliary feature related functions 

const char html_auxScript1[] PROGMEM =
"<script>\n"
"function s(key,v1) {\n"
  "var xhttp = new XMLHttpRequest();\n"
  "xhttp.open('GET', 'auxiliaryA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);\n"
  "xhttp.send();\n"
"}\n"
"function g(v1){s('dr',v1);}\n"
"function gf(v1){s('dr',v1);autoFastRun();}\n"
"function sf(key,v1){s(key,v1);autoFastRun();}\n"
"</script>\n";

const char html_auxAuxB[] PROGMEM = "<div class='b1' style='width: 27em'>\r\n<div align='left'>" L_AUX_FEATURES ":<br /><br /></div>\r\n";
const char html_auxOnSwitch[] PROGMEM = 
"<button id='sw%d_on' class='btns_right' style='line-height: 1.2rem' onpointerdown=\"s('x%dv1','1')\" type='button' disabled>" L_ON "</button>";
const char html_auxOffSwitch[] PROGMEM = 
"<button id='sw%d_off' class='btns_left' style='line-height: 1.2rem' onpointerdown=\"s('x%dv1','0')\" type='button' disabled>" L_OFF "</button>\r\n";

const char html_auxStartStop1[] PROGMEM = "<button onpointerdown=\"s('";
const char html_auxStartStop2[] PROGMEM = "','1')\" type='button'>" L_START "</button><button onpointerdown=\"s('";
const char html_auxStartStop3[] PROGMEM = "','0')\" type='button'>" L_STOP "</button>\r\n";

const char html_auxAnalog[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='0' max='255' value='";
const char html_auxHeater[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='-50' max='200' value='";
const char html_auxExposure[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='0' max='255' value='";
const char html_auxDelay[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='10' max='255' value='";
const char html_auxCount[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='1' max='255' value='";
const char html_auxAuxE[] PROGMEM = "</div><br class='clear' />\r\n";

const char html_auxEnd[] PROGMEM = 
"<br />\r\n";

#ifdef OETHS
void handleAux(EthernetClient *client) {
#else
void handleAux() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();

  mountStatus.update(true);
    
  char temp1[240]="";
  char temp2[80]="";

  processAuxGet();

  sendHtmlStart();
  
  String data=FPSTR(html_headB);
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
  data += FPSTR(html_main_css_btns1);
  sendHtml(data);
  data += FPSTR(html_main_css_btns2);
  data += FPSTR(html_main_css_btns3);
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
  if (mountStatus.featureFound()) data += FPSTR(html_linksAuxS);
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

  // ajax scripts
  data += FPSTR(html_auxScript1);

  // active ajax page is: auxAjax();
  data +="<script>var ajaxPage='auxiliary.txt';</script>\n";
  data +=FPSTR(html_ajax_active);
  sendHtml(data);

  // Auxiliary Features --------------------------------------
  int j = 0;
  if (mountStatus.featureFound()) {
    data += FPSTR(html_auxAuxB);

    for (int i=0; i<8; i++) {
      mountStatus.selectFeature(i);

      if (mountStatus.featurePurpose() != 0 && j > 0) {
        data += F("<br/><div style='float: left; width: 26em'><hr></div>\r\n");
      }
      if (mountStatus.featurePurpose() == SWITCH) {
        data += F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>&bull;");
        data += mountStatus.featureName();
        data += "&bull;";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        sprintf_P(temp1,html_auxOnSwitch,i+1,i+1); data += temp1;
        sprintf_P(temp1,html_auxOffSwitch,i+1,i+1); data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        data += "</div>\r\n";
        sendHtml(data);
        j++;
      } else
      if (mountStatus.featurePurpose() == ANALOG_OUTPUT) {
        data += F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>&bull;");
        data += mountStatus.featureName();
        data += "&bull;";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxAnalog);
        sprintf(temp1,"%d' onchange=\"sf('x%dv1',this.value)\">",mountStatus.featureValue1(),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        sprintf(temp1,"<span id='x%dv1'>%d</span>%%",i+1,(int)lround((mountStatus.featureValue1()/255.0)*100.0));
        data += temp1;
        data += "</div>\r\n";
        sendHtml(data);
        j++;
      } else
      if (mountStatus.featurePurpose() == DEW_HEATER) {
        data += F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>&bull;");
        data += mountStatus.featureName();
        data += "&bull;";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        sprintf_P(temp1,html_auxOnSwitch,i+1,i+1); data += temp1;
        sprintf_P(temp1,html_auxOffSwitch,i+1,i+1); data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        dtostrf(mountStatus.featureValue4(),3,1,temp2);
        sprintf(temp1,"&Delta;<span id='x%dv4'>%s</span>&deg;C\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";

        data += F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>");
        data += L_ZERO " (100% " L_POWER ")";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxHeater);
        sprintf(temp1,"%d' onchange=\"sf('x%dv2',this.value)\">",(int)lround(mountStatus.featureValue2()*10.0),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        dtostrf(mountStatus.featureValue2(),3,1,temp2);
        sprintf(temp1,"<span id='x%dv2'>%s</span>&deg;C\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";
        
        data += F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>");
        data += L_SPAN " (0% " L_POWER ")";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxHeater);
        sprintf(temp1,"%d' onchange=\"sf('x%dv3',this.value)\">",(int)lround(mountStatus.featureValue3()*10.0),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        dtostrf(mountStatus.featureValue3(),3,1,temp2);
        sprintf(temp1,"<span id='x%dv3'>%s</span>&deg;C\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";

        sendHtml(data);
        j++;
      } else
      if (mountStatus.featurePurpose() == INTERVALOMETER) {
        data += F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>&bull;");
        data += mountStatus.featureName();
        data += "&bull;";
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxStartStop1);
        sprintf(temp1,"x%dv1",i+1);
        data += temp1;
        data += FPSTR(html_auxStartStop2);
        sprintf(temp1,"x%dv1",i+1);
        data += temp1;
        data += FPSTR(html_auxStartStop3);
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        sprintf(temp1,"<span id='x%dv1'>-</span>\r\n",i+1);
        data += temp1;
        data += "</div>\r\n";

        data += F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>");
        data += L_CAMERA_COUNT;
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxCount);
        sprintf(temp1,"%d' onchange=\"sf('x%dv4',this.value)\">",(int)mountStatus.featureValue4(),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        dtostrf(mountStatus.featureValue4(),0,0,temp2);
        sprintf(temp1,"<span id='x%dv4'>%s</span> x\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";

        data += F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>");
        data += L_CAMERA_EXPOSURE;
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxExposure);
        sprintf(temp1,"%d' onchange=\"sf('x%dv2',this.value)\">",(int)timeToByte(mountStatus.featureValue2()),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        float v; int d;
        v=mountStatus.featureValue2(); if (v < 1.0) d=3; else if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,temp2);
        sprintf(temp1,"<span id='x%dv2'>%s</span> sec\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";

        data += F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>");
        data += L_CAMERA_DELAY;
        data += F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>");
        data += FPSTR(html_auxDelay);
        sprintf(temp1,"%d' onchange=\"sf('x%dv3',this.value)\">",(int)timeToByte(mountStatus.featureValue3()),i+1);
        data += temp1;
        data += F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>");
        v=mountStatus.featureValue3(); if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,temp2);
        sprintf(temp1,"<span id='x%dv3'>%s</span> sec\r\n",i+1,temp2);
        data += temp1;
        data += "</div>\r\n";

        sendHtml(data);
        j++;
      }
    }

    data += FPSTR(html_auxAuxE);
  }

  data += FPSTR(html_auxEnd);
  
  data += "</div></body></html>";

  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void auxAjaxGet(EthernetClient *client) {
#else
void auxAjaxGet() {
#endif
  processAuxGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

#ifdef OETHS
void auxAjax(EthernetClient *client) {
#else
void auxAjax() {
#endif
  String data="";
  char temp[120]="";

  // update auxiliary feature values
  if (mountStatus.featureFound()) {
    mountStatus.featureUpdate();
    for (int i=0; i<8; i++) {
      mountStatus.selectFeature(i);
      if (mountStatus.featurePurpose() == SWITCH) {
        if (mountStatus.featureValue1() == 0) {
          sprintf(temp,"sw%d_on|%s\n",i+1,"enabled"); data += temp;
          sprintf(temp,"sw%d_off|%s\n",i+1,"disabled"); data += temp;
        } else {
          sprintf(temp,"sw%d_on|%s\n",i+1,"disabled"); data += temp;
          sprintf(temp,"sw%d_off|%s\n",i+1,"enabled"); data += temp;
        }
      } else
      if (mountStatus.featurePurpose() == ANALOG_OUTPUT) {
        sprintf(temp,"x%dv1|%d\n",i+1,(int)lround((mountStatus.featureValue1()/255.0)*100.0)); data += temp;
      } else
      if (mountStatus.featurePurpose() == DEW_HEATER) {
        char s[20];
        if (mountStatus.featureValue1() == 0) {
          sprintf(temp,"sw%d_on|%s\n",i+1,"enabled"); data += temp;
          sprintf(temp,"sw%d_off|%s\n",i+1,"disabled"); data += temp;
        } else {
          sprintf(temp,"sw%d_on|%s\n",i+1,"disabled"); data += temp;
          sprintf(temp,"sw%d_off|%s\n",i+1,"enabled"); data += temp;
        }
        dtostrf(mountStatus.featureValue2(),3,1,s); sprintf(temp,"x%dv2|%s\n",i+1,s); data += temp;
        dtostrf(mountStatus.featureValue3(),3,1,s); sprintf(temp,"x%dv3|%s\n",i+1,s); data += temp;
        dtostrf(mountStatus.featureValue4(),3,1,s); sprintf(temp,"x%dv4|%s\n",i+1,s); data += temp;
      } else
      if (mountStatus.featurePurpose() == INTERVALOMETER) {
        char s[20];
        float v; int d;
        
        v=mountStatus.featureValue1();
        if (abs(v) < 0.001) sprintf(temp,"x%dv1|-\n",i+1); else sprintf(temp,"x%dv1|%d\n",i+1,(int)v); data += temp;
        v=mountStatus.featureValue2(); if (v < 1.0) d=3; else if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,s); sprintf(temp,"x%dv2|%s\n",i+1,s); data += temp;
        v=mountStatus.featureValue3(); if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,s); sprintf(temp,"x%dv3|%s\n",i+1,s); data += temp;
        sprintf(temp,"x%dv4|%d\n",i+1,(int)mountStatus.featureValue4()); data += temp;
      }
    }
  }

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

void processAuxGet() {
  String v;
  char temp[40]="";
  char temp1[40]="";

  // Auxiliary Feature set Value1 to Value4
  for (char c='1'; c<='8'; c++) {
    mountStatus.selectFeature(c-'1');
    sprintf(temp,"x%cv1",c); v=server.arg(temp);
    if (v != "") { sprintf(temp,":SXX%c,V%s#",c,v.c_str()); commandBool(temp); }
    if (mountStatus.featurePurpose() == DEW_HEATER) {
      sprintf(temp,"x%cv2",c); v=server.arg(temp);
      if (v != "") { dtostrf(v.toFloat()/10.0,0,1,temp1); sprintf(temp,":SXX%c,Z%s#",c,temp1); commandBool(temp); }
      sprintf(temp,"x%cv3",c); v=server.arg(temp);
      if (v != "") { dtostrf(v.toFloat()/10.0,0,1,temp1); sprintf(temp,":SXX%c,S%s#",c,temp1); commandBool(temp); }
    } else
    if (mountStatus.featurePurpose() == INTERVALOMETER) {
      sprintf(temp,"x%cv2",c); v=server.arg(temp);
      if (v != "") { dtostrf(byteToTime(v.toInt()),0,3,temp1); sprintf(temp,":SXX%c,E%s#",c,temp1); commandBool(temp); }
      sprintf(temp,"x%cv3",c); v=server.arg(temp);
      if (v != "") { dtostrf(byteToTime(v.toInt()),0,2,temp1); sprintf(temp,":SXX%c,D%s#",c,temp1); commandBool(temp); }
      sprintf(temp,"x%cv4",c); v=server.arg(temp);
      if (v != "") { dtostrf(v.toFloat(),0,0,temp1); sprintf(temp,":SXX%c,C%s#",c,temp1); commandBool(temp); }
    }
  }
}
