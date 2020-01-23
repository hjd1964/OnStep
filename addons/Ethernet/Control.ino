// -----------------------------------------------------------------------------------
// Telescope control related functions 

const char html_controlScript1[] PROGMEM =
"<script>\n"
"function s(key,v1) {\n"
  "var xhttp = new XMLHttpRequest();\n"
  "xhttp.open('GET', 'controlA.txt?'+key+'='+v1+'&x='+new Date().getTime(), true);\n"
  "xhttp.send();\n"
"}\n"
"function g(v1){s('dr',v1);}\n"
"function gf(v1){s('dr',v1);autoFastRun();}\n"
"function sf(key,v1){s(key,v1);autoFastRun();}\n"
"</script>\n";

const char html_controlScript2[] PROGMEM =
"<script>\r\n"
"function SetDateTime() {"
"var d1 = new Date();"
"var jan = new Date(d1.getFullYear(), 0, 1);"
"var d = new Date(d1.getTime()-(jan.getTimezoneOffset()-d1.getTimezoneOffset())*60*1000);";
const char html_controlScript3[] =
"document.getElementById('dd').value = d.getDate();"
"document.getElementById('dm').value = d.getMonth();"
"document.getElementById('dy').value = d.getFullYear();";
const char html_controlScript4[] =
"document.getElementById('th').value = d.getHours();"
"document.getElementById('tm').value = d.getMinutes();"
"document.getElementById('ts').value = d.getSeconds();"
"}\r\n"
"</script>\r\n";

const char html_controlQuick1[] PROGMEM =
"<div style='text-align: center; width: 30em'>"
"<form style='display: inline;' method='get' action='/control.htm'>"
"<button name='qb' class='bb' value='st' type='submit' onpointerdown='SetDateTime();' title='" L_HINT_CLOCK "'>" CLOCK_CH "</button>"
"&nbsp;&nbsp;";
const char html_controlQuick1a[] PROGMEM =
"<input id='dm' type='hidden' name='dm'><input id='dd' type='hidden' name='dd'><input id='dy' type='hidden' name='dy'>"
"<input id='th' type='hidden' name='th'><input id='tm' type='hidden' name='tm'><input id='ts' type='hidden' name='ts'>";
const char html_controlQuick2[] PROGMEM =
"</form>"
"<button type='button' class='bb' onpointerdown=\"g('qc')\" title='" L_HINT_CONTINUE_GOTO "'>" ARROW_R2 HOME_CH ARROW_R2 "</button>";
const char html_controlQuick3[] PROGMEM =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"g('qh')\" title='" L_HINT_FIND_HOME "'>" ARROW_DR HOME_CH "</button>"
"<button type='button' class='bb' onpointerdown=\"g('qr')\" title='" L_HINT_RESET_HOME "'>" CAUTION_CH HOME_CH "</button>";
const char html_controlQuick4[] PROGMEM =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"g('pu')\" title='" L_HINT_UNPARK "'>P" ARROW_UR "</button>"
"<button type='button' class='bb' onpointerdown=\"g('pk')\" title='" L_HINT_PARK "'>" ARROW_DR "P</button>";
const char html_controlQuick5[] PROGMEM =
"&nbsp;&nbsp;"
"<button type='button' class='bb' style=\"height: 2.2em; color: White;\" onpointerdown=\"g('qq')\" title='" L_HINT_EMERGENCY_STOP "'>Stop!</button>"
"</div><br class='clear' />\r\n";

const char html_controlTrack1[] PROGMEM = 
"<div class='b1' style='width: 8em'>"
"<div align='left'>Tracking:</div>"
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Ts')\" type='submit' title='" L_HINT_RATE_SIDEREAL "'>" SIDEREAL_CH "</button>";
const char html_controlTrack2[] PROGMEM =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Tl')\" type='submit' title='" L_HINT_RATE_LUNAR "'>" LUNAR_CH "</button>";
const char html_controlTrack3[] PROGMEM =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Th')\" type='submit' title='" L_HINT_RATE_SOLAR "'>" SOLAR_CH "</button>"
"</div>";
const char html_controlAlign1[] PROGMEM = 
"<div class='b1' style='width: 16.2em'>"
"<div align='left'>" L_ALIGN ":</div>"
"<form method='get' action='/control.htm'>";
const char html_controlAlign2[] PROGMEM =
"<button class='bbh' type='submit' name=\"al\" value='%d'>%d%s</button>";
const char html_controlAlign3[] PROGMEM =
"&nbsp;&nbsp;&nbsp;<button class='bbh' type='submit' name=\"al\" value=\"n\">" L_ACCEPT "</button></form>";
const char html_controlTrack4[] PROGMEM =
"</div><br class='clear' />\r\n";

const char html_controlGuide1[] PROGMEM =
"<div class='b1' style='width: 27em'>"
"<div align='left'>Guide:</div>"
"<button class='gb' type='button' onpointerdown=\"g('n1')\" onpointerup=\"g('n0')\">" BUTTON_N "</button><br />";
const char html_controlGuide2[] PROGMEM =
"<button class='gb' type='button' onpointerdown=\"g('e1')\" onpointerup=\"g('e0')\">" BUTTON_E "</button>";
const char html_controlGuide3[] PROGMEM =
"<button class='gb' type='button' onpointerdown=\"g('sy')\" title='Sync'>" BUTTON_SYNC "</button>"
"<button class='gb' type='button' onpointerdown=\"g('w1')\" onpointerup=\"g('w0')\">" BUTTON_W "</button><br />";
const char html_controlGuide4[] PROGMEM =
"<button class='gb' type='button' onpointerdown=\"g('s1')\" onpointerup=\"g('s0')\">" BUTTON_S "</button><br /><br />";
const char html_controlGuide5[] PROGMEM =
"<form method='get' action=\"/control.htm\">"
"<button class='bbh' type='button' onpointerdown=\"g('R0')\">0.25</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R1')\">0.5</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R2')\">1x</button>";
const char html_controlGuide6[] PROGMEM =
"<button class='bbh' type='button' onpointerdown=\"g('R5')\">" L_CENTER_RATE "</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R6')\">" L_FIND_RATE "</button>";
const char html_controlGuide7[] PROGMEM =
"<button class='bbh' type='button' onpointerdown=\"g('R7')\">" L_FAST_RATE "</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R8')\">" L_VFAST_RATE "</button>"
"</div><br class='clear' />\r\n";

const char html_controlFocus1[] PROGMEM =
"<div class='b1' style='width: 27em'>";
const char html_controlFocus2[] PROGMEM =
"<button class='bbh' type='button' onpointerdown=\"gf('F1')\" title='" L_HINT_FOC_SEL " 1'>1</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('F2')\" title='" L_HINT_FOC_SEL " 2'>2</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus3[] PROGMEM =
"<button class='bbh' type='button' onpointerdown=\"gf('Fh')\" title='" L_HINT_FOC_FIND_HOME "'>" ARROW_DR HOME_CH "</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('FH')\" title='" L_HINT_FOC_RESET_HOME "'>" CAUTION_CH HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus4[] PROGMEM =
"<button class='bbh' type='button' onpointerdown=\"gf('FI')\" onpointerup=\"g('Fq');\" >" ARROW_DD "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('Fi')\" onpointerup=\"g('Fq')\" >" ARROW_D "</button>";
const char html_controlFocus5[] PROGMEM =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('Fo')\" onpointerup=\"g('Fq')\" >" ARROW_U "</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('FO')\" onpointerup=\"g('Fq')\" >" ARROW_UU "</button>";
const char html_controlFocus6[] PROGMEM =
"</div><br class='clear' />\r\n";

const char html_controlRotate0[] PROGMEM =
"<div class='b1' style='width: 27em'>";
const char html_controlRotate1[] PROGMEM =
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('ho')\" title='" L_HINT_ROT_FIND_HOME "'>" ARROW_DR HOME_CH "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('re')\" title='" L_HINT_ROT_RESET_HOME "'>" CAUTION_CH HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlRotate2[] PROGMEM =
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('b2')\" >" ARROW_LL "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('b1')\" >" ARROW_L "</button>";
const char html_controlRotate3[] PROGMEM =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('f1')\" >" ARROW_R "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('f2')\" >" ARROW_RR "</button><br />";
const char html_controlDeRotate1[] PROGMEM =
"<button type='button' onpointerdown=\"gf('d1')\" >" L_HINT_DEROTATE_ON "</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"gf('dr')\" title='" L_HINT_ROTATOR_REV "'>Rev</button>";
const char html_controlDeRotate2[] PROGMEM =
"<button type='button' onpointerdown=\"gf('dp')\" title='" L_HINT_ROTATOR_PAR "'>P</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"gf('d0')\" >" L_HINT_DEROTATE_OFF "</button>";
const char html_controlRotate4[] PROGMEM =
"</div><br class='clear' />\r\n";

#if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8) || defined(AN3) || defined(AN4) || defined(AN5) || defined(AN6) || defined(AN7) || defined(AN8)
const char html_controlAuxB[] PROGMEM = "<div class='b1' style='width: 27em'><div align='left'>Aux:</div>";
#ifdef SW0
const char html_controlSwitch0[] PROGMEM = SW0 "<br /><button type='button' onpointerdown=\"s('sw0','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw0','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW1
const char html_controlSwitch1[] PROGMEM = SW1 "<br /><button type='button' onpointerdown=\"s('sw1','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw1','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW2
const char html_controlSwitch2[] PROGMEM = SW2 "<br /><button type='button' onpointerdown=\"s('sw2','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw2','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW3
const char html_controlSwitch3[] PROGMEM = SW3 "<br /><button type='button' onpointerdown=\"s('sw3','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw3','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW4
const char html_controlSwitch4[] PROGMEM = SW4 "<br /><button type='button' onpointerdown=\"s('sw4','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw4','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW5
const char html_controlSwitch5[] PROGMEM = SW5 "<br /><button type='button' onpointerdown=\"s('sw5','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw5','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW6
const char html_controlSwitch6[] PROGMEM = SW6 "<br /><button type='button' onpointerdown=\"s('sw6','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw6','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW7
const char html_controlSwitch7[] PROGMEM = SW7 "<br /><button type='button' onpointerdown=\"s('sw7','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw7','0')\" >" L_OFF "</button><br />";
#endif
#ifdef SW8
const char html_controlSwitch8[] PROGMEM = SW8 "<br /><button type='button' onpointerdown=\"s('sw8','255')\" >" L_ON "</button><button type='button' onpointerdown=\"s('sw8','0')\" >" L_OFF "</button><br />";
#endif
#ifdef AN3
const char html_controlAnalog3A[] PROGMEM = AN3 " <span id='an3v'>";
const char html_controlAnalog3B[] PROGMEM ="</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog3C[] PROGMEM = "' onchange=\"sf('an3',this.value)\"><br />";
#endif
#ifdef AN4
const char html_controlAnalog4A[] PROGMEM = AN4 " <span id='an4v'>";
const char html_controlAnalog4B[] PROGMEM ="</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog4C[] PROGMEM = "' onchange=\"sf('an4',this.value)\"><br />";
#endif
#ifdef AN5
const char html_controlAnalog5A[] PROGMEM = AN5 " <span id='an5v'>";
const char html_controlAnalog5B[] PROGMEM = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog5C[] PROGMEM = "' onchange=\"sf('an5',this.value)\"><br />";
#endif
#ifdef AN6
const char html_controlAnalog6A[] PROGMEM = AN6 " <span id='an6v'>";
const char html_controlAnalog6B[] PROGMEM = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog6C[] PROGMEM = "' onchange=\"sf('an6',this.value)\"><br />";
#endif
#ifdef AN7
const char html_controlAnalog7A[] PROGMEM = AN7 " <span id='an7v'>";
const char html_controlAnalog7B[] PROGMEM = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog7C[] PROGMEM = "' onchange=\"sf('an7',this.value)\"><br />";
#endif
#ifdef AN8
const char html_controlAnalog8A[] PROGMEM = AN8 " <span id='an8v'>";
const char html_controlAnalog8B[] PROGMEM = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog8C[] PROGMEM = "' onchange=\"sf('an8',this.value)\"><br />";
#endif
const char html_controlAuxE[] PROGMEM = "</div><br class='clear' />\r\n";
#endif

const char html_controlEnd[] PROGMEM = 
"<br />\r\n";

boolean Focuser1;
boolean Focuser2;
boolean Rotate;
boolean DeRotate;

#ifdef OETHS
void handleControl(EthernetClient *client) {
#else
void handleControl() {
#endif
  Ser.setTimeout(webTimeout);
  serialRecvFlush();
    
  char temp1[24]="";

  processControlGet();

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
  data += FPSTR(html_main_cssE);
  data += FPSTR(html_headE);
  data += FPSTR(html_bodyB);
  sendHtml(data);

  // get status
  mountStatus.update();

  // finish the standard http response header
  data += FPSTR(html_onstep_header1); data += "OnStep";
  data += FPSTR(html_onstep_header2);
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += FPSTR(html_onstep_header3);
  data += FPSTR(html_links1N);
  data += FPSTR(html_links2S);
  data += FPSTR(html_linksLibN);
#if ENCODERS == ON
  data += FPSTR(html_linksEncN);
#endif
  sendHtml(data);
  data += FPSTR(html_links3N);
  data += FPSTR(html_links4N);
  data += FPSTR(html_links5N);
#ifndef OETHS
  data += FPSTR(html_links6N);
#endif
  data += FPSTR(html_onstep_header4);
  sendHtml(data);

  // OnStep wasn't found, show warning and info.
  if (!mountStatus.valid()) { data+= FPSTR(html_bad_comms_message); sendHtml(data); sendHtmlDone(data); return; }

  // guide (etc) script
  data += FPSTR(html_controlScript1);
  // clock script
  data += FPSTR(html_controlScript2);
  data += FPSTR(html_controlScript3);
  data += FPSTR(html_controlScript4);

  // active ajax page is: controlAjax();
  data +="<script>var ajaxPage='control.txt';</script>\n";
  data +=FPSTR(html_ajax_active);
  sendHtml(data);

  // Quick controls ------------------------------------------
  data += FPSTR(html_controlQuick1);
  data += FPSTR(html_controlQuick1a);
  if (mountStatus.mountType()==MT_GEM) data += FPSTR(html_controlQuick2);
  data += FPSTR(html_controlQuick3);
  data += FPSTR(html_controlQuick4);
  data += FPSTR(html_controlQuick5);
  sendHtml(data);

  // Tracking control ----------------------------------------
  data += FPSTR(html_controlTrack1);
  data += FPSTR(html_controlTrack2);
  data += FPSTR(html_controlTrack3);
  sendHtml(data);

  // Get the align mode --------------------------------------
  data += FPSTR(html_controlAlign1);
  byte sc[3];
  int n=1;
  if (mountStatus.alignMaxStars()<3) { n=1; sc[0]=1; } else
  if (mountStatus.alignMaxStars()<4) { n=3; sc[0]=1; sc[1]=2; sc[2]=3; } else
  if (mountStatus.alignMaxStars()<6) { n=3; sc[0]=1; sc[1]=3; sc[2]=4; } else
  if (mountStatus.alignMaxStars()<8) { n=3; sc[0]=1; sc[1]=3; sc[2]=6; } else
                                     { n=3; sc[0]=1; sc[1]=3; sc[2]=9; }
  for (int i=0; i<n; i++) { char temp2[120]=""; sprintf_P(temp2,html_controlAlign2,sc[i],sc[i],SIDEREAL_CH); data+=temp2; }
  data += FPSTR(html_controlAlign3);
  sendHtml(data);
  
  // Tracking ------------------------------------------------
  data += FPSTR(html_controlTrack4);

  // Guiding -------------------------------------------------
  data += FPSTR(html_controlGuide1);
  data += FPSTR(html_controlGuide2);
  data += FPSTR(html_controlGuide3);
  data += FPSTR(html_controlGuide4);
  sendHtml(data);
  data += FPSTR(html_controlGuide5);
  data += FPSTR(html_controlGuide6);
  data += FPSTR(html_controlGuide7);
  sendHtml(data);

  // Focusing ------------------------------------------------
  if (commandBool(":FA#")) Focuser1=true; else Focuser1=false;
  if (commandBool(":fA#")) Focuser2=true; else Focuser2=false;
  if (Focuser1) {
    data += FPSTR(html_controlFocus1);
    data += "<div style='float: left;'>" L_FOCUSER ":</div><div style='float: right; text-align: right;' id='focuserpos'>?</div><br />";
    if (Focuser2) data += FPSTR(html_controlFocus2);
    data += FPSTR(html_controlFocus3);
    data += FPSTR(html_controlFocus4);
    data += FPSTR(html_controlFocus5);
    data += FPSTR(html_controlFocus6);
    sendHtml(data);
  }

  // Rotate/De-Rotate ----------------------------------------
  Rotate=false;
  DeRotate=false;
  if (command(":GX98#",temp1)) {
    if (temp1[0]=='R') Rotate=true;
    if (temp1[0]=='D') { Rotate=true; DeRotate=true; }
  }
  if (Rotate) {
    data += FPSTR(html_controlRotate0);
    data += "<div style='float: left;'>" L_ROTATOR ":</div><div style='float: right; text-align: right;' id='rotatorpos'>?</div><br />";
    data += FPSTR(html_controlRotate1);
    data += FPSTR(html_controlRotate2);
    data += FPSTR(html_controlRotate3);
    sendHtml(data);
  }
  if (DeRotate) {
    data += FPSTR(html_controlDeRotate1);
    data += FPSTR(html_controlDeRotate2);
    sendHtml(data);
  }
  if (Rotate) {
    data += FPSTR(html_controlRotate4);
    sendHtml(data);
  }

  // Aux -----------------------------------------------------
  #if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8) || defined(AN3) || defined(AN4) || defined(AN5) || defined(AN6) || defined(AN7) || defined(AN8)
    data += FPSTR(html_controlAuxB);
    // Digital Control
    int c=0;
    #ifdef SW0
    data += FPSTR(html_controlSwitch0); c++;
    #endif
    #ifdef SW1
    data += FPSTR(html_controlSwitch1); c++;
    #endif
    #ifdef SW2
    data += FPSTR(html_controlSwitch2); c++;
    #endif
    #ifdef SW3
    data += FPSTR(html_controlSwitch3); c++;
    #endif
    #ifdef SW4
    data += FPSTR(html_controlSwitch4); c++;
    #endif
    #ifdef SW5
    data += FPSTR(html_controlSwitch5); c++;
    #endif
    #ifdef SW6
    data += FPSTR(html_controlSwitch6); c++;
    #endif
    #ifdef SW7
    data += FPSTR(html_controlSwitch7); c++;
    #endif
    #ifdef SW8
    data += FPSTR(html_controlSwitch8); c++;
    #endif
    if (c>0) { data+="<br />"; sendHtml(data); }

    // Analog Control
    c=0;
    #ifdef AN3
    if (command(":GXG3#",temp1)) { data += FPSTR(html_controlAnalog3A); data += temp1; data += FPSTR(html_controlAnalog3B); data += temp1; data += FPSTR(html_controlAnalog3C); c++; }
    #endif
    #ifdef AN4
    if (command(":GXG4#",temp1)) { data += FPSTR(html_controlAnalog4A); data += temp1; data += FPSTR(html_controlAnalog4B); data += temp1; data += FPSTR(html_controlAnalog4C); c++; }
    #endif
    #ifdef AN5
    if (command(":GXG5#",temp1)) { data += FPSTR(html_controlAnalog5A); data += temp1; data += FPSTR(html_controlAnalog5B); data += temp1; data += FPSTR(html_controlAnalog5C); c++; }
    #endif
    #ifdef AN6
    if (command(":GXG6#",temp1)) { data += FPSTR(html_controlAnalog6A); data += temp1; data += FPSTR(html_controlAnalog6B); data += temp1; data += FPSTR(html_controlAnalog6C); c++; }
    #endif
    #ifdef AN7
    if (command(":GXG7#",temp1)) { data += FPSTR(html_controlAnalog7A); data += temp1; data += FPSTR(html_controlAnalog7B); data += temp1; data += FPSTR(html_controlAnalog7C); c++; }
    #endif
    #ifdef AN8
    if (command(":GXG8#",temp1)) { data += FPSTR(html_controlAnalog8A); data += temp1; data += FPSTR(html_controlAnalog8B); data += temp1; data += FPSTR(html_controlAnalog8C); c++; } 
    #endif
    if (c>0) sendHtml(data);

    data += FPSTR(html_controlAuxE);
  #endif

  data += FPSTR(html_controlEnd);
  
  data += "</div></body></html>";

  sendHtml(data);
  sendHtmlDone(data);
}

#ifdef OETHS
void controlAjaxGet(EthernetClient *client) {
#else
void controlAjaxGet() {
#endif
  processControlGet();
#ifdef OETHS
  client->print("");
#else
  server.send(200, "text/html","");
#endif
}

#ifdef OETHS
void controlAjax(EthernetClient *client) {
#else
void controlAjax() {
#endif
  String data="";
  char temp[120]="";

  if (Focuser1) {
    data += "focuserpos|";
    if (command(":FG#",temp)) { data += temp; data += " microns\n"; } else { data += "?\n"; }
  }

  if (Rotate) {
    data += "rotatorpos|";
    if (command(":rG#",temp)) { temp[9]=temp[5]; temp[10]=temp[6]; temp[11]=0; temp[4]='&'; temp[5]='d'; temp[6]='e'; temp[7]='g'; temp[8]=';'; data += temp; data += "&#39;\n"; } else { data += "?\n"; }
  }

  #ifdef AN3
    data += "an3v|"; if (command(":GXG3#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN4
    data += "an4v|"; if (command(":GXG4#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN5
    data += "an5v|"; if (command(":GXG5#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN6
    data += "an6v|"; if (command(":GXG6#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN7
    data += "an7v|"; if (command(":GXG7#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN8
    data += "an8v|"; if (command(":GXG8#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

int get_temp_month;
int get_temp_day;
int get_temp_year;
int get_temp_hour;
int get_temp_minute;
int get_temp_second;

void processControlGet() {
  String v;
  int i;
  char temp[20]="";

  // Quick bar
  v=server.arg("qb");
  if (v!="") {
    if (v=="q") commandBlind(":Q#");       // stop goto/guide
    if (v=="co") commandBool(":SX99,1#");  // meridian flip, pause->continue
    if (v=="hf") commandBlind(":hF#");     // home, reset
    if (v=="hc") commandBlind(":hC#");     // home, find
    if (v=="pk") commandBool(":hP#");      // park
    if (v=="pu") commandBool(":hR#");      // un-park
  }

  // Align
  v=server.arg("al");
  if (v!="") {
    if (v=="1") commandBool(":A1#");
    if (v=="2") commandBool(":A2#");
    if (v=="3") commandBool(":A3#");
    if (v=="4") commandBool(":A4#");
    if (v=="5") commandBool(":A5#");
    if (v=="6") commandBool(":A6#");
    if (v=="7") commandBool(":A7#");
    if (v=="8") commandBool(":A8#");
    if (v=="9") commandBool(":A9#");
    if (v=="n") commandBool(":A+#");
    if (v=="q") commandBlind(":Q#");
  }

  // Set DATE/TIME
  v=server.arg("dm");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=0) && (i<=11))) { get_temp_month=i+1; }
  }
  v=server.arg("dd");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=1) && (i<=31))) { get_temp_day=i; }
  }
  v=server.arg("dy");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=2016) && (i<=9999))) {
      get_temp_year=i-2000;
      char temp[10];
      sprintf(temp,":SC%02d/%02d/%02d#",get_temp_month,get_temp_day,get_temp_year);
      commandBool(temp);
    }
  }
  v=server.arg("th");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=0) && (i<=23))) { get_temp_hour=i; }
  }
  v=server.arg("tm");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=0) && (i<=59))) { get_temp_minute=i; }
  }
  v=server.arg("ts");
  if (v!="") {
    if ( (atoi2((char *)v.c_str(),&i)) && ((i>=0) && (i<=59))) {
      get_temp_second=i;
      char temp[10];
      sprintf(temp,":SL%02d:%02d:%02d#",get_temp_hour,get_temp_minute,get_temp_second);
      commandBool(temp);
    }
  }

  v=server.arg("dr");
  if (v!="") {
    // Tracking control
    if (v=="Ts") commandBlind(":TQ#");    // sidereal
    if (v=="Tl") commandBlind(":TL#");    // lunar
    if (v=="Th") commandBlind(":TS#");    // solar

    // quick
    if (v=="qq") commandBlind(":Q#");     // stop goto/guide
    if (v=="qc") commandBool(":SX99,1#"); // meridian flip, pause->continue
    if (v=="qr") commandBlind(":hF#");    // home, reset
    if (v=="qh") commandBlind(":hC#");    // home, find
    if (v=="pk") commandBool(":hP#");     // park
    if (v=="pu") commandBool(":hR#");     // un-park

    // GUIDE control direction
    if (v=="n1") commandBlind(":Mn#");    // move n... s,e,w
    if (v=="s1") commandBlind(":Ms#");
    if (v=="e1") commandBlind(":Me#");
    if (v=="w1") commandBlind(":Mw#");

    if (v=="n0") commandBlind(":Qn#");    // quit n... s,e,w
    if (v=="s0") commandBlind(":Qs#");
    if (v=="e0") commandBlind(":Qe#");
    if (v=="w0") commandBlind(":Qw#");

    if (v=="sy") commandBool(":CS#");     // sync

    // GUIDE control rate
    if (v=="R0") commandBlind(":R0#");    // guide rate 0..9
    if (v=="R1") commandBlind(":R1#");
    if (v=="R2") commandBlind(":R2#");
    if (v=="R3") commandBlind(":R3#");
    if (v=="R4") commandBlind(":R4#");
    if (v=="R5") commandBlind(":R5#");
    if (v=="R6") commandBlind(":R6#");
    if (v=="R7") commandBlind(":R7#");
    if (v=="R8") commandBlind(":R8#");
    if (v=="R9") commandBlind(":R9#");

    // Focuser
    if (v=="F1") commandBool(":FA1#");      // select focuser 1
    if (v=="F2") commandBool(":FA2#");      // select focuser 2
    if (v=="FH") commandBlind(":FH#");      // reset focuser at home (half travel)
    if (v=="Fh") commandBlind(":Fh#");      // move focuser to home (half travel)
    if (v=="FI") commandBlind(":FF#:F-#");  // rate fast, move in
    if (v=="Fi") commandBlind(":FS#:F-#");  // rate slow, move in
    if (v=="Fo") commandBlind(":FS#:F+#");  // rate fast, move out
    if (v=="FO") commandBlind(":FF#:F+#");  // rate slow, move out
    if (v=="Fq") commandBlind(":FQ#");
   
    // Rotate/De-Rotate
    if (v=="b2") commandBlind(":r3#:r<#");  // rate 3, move ccw
    if (v=="b1") commandBlind(":r1#:r<#");  // rate 1, move ccw
    if (v=="f1") commandBlind(":r1#:r>#");  // rate 3, move cw
    if (v=="f2") commandBlind(":r3#:r>#");  // rate 1, move cw
    if (v=="re") commandBlind(":rF#");      // reset rotator at home
    if (v=="ho") commandBlind(":rC#");      // move rotator to home
    if (v=="d0") commandBlind(":r-#");      // disable de-rotator
    if (v=="d1") commandBlind(":r+#");      // enable de-rotator
    if (v=="dr") commandBlind(":rR#");      // reverse rotator
    if (v=="dp") commandBlind(":rP#");      // move rotator to parallactic angle
  }

  // General purpose switches
  #ifdef SW0
  v=server.arg("sw0"); if (v != "") commandBool(":SXG0,"+v+"#");
  #endif
  #ifdef SW1
  v=server.arg("sw1"); if (v != "") commandBool(":SXG1,"+v+"#");
  #endif
  #ifdef SW2
  v=server.arg("sw2"); if (v != "") commandBool(":SXG2,"+v+"#");
  #endif
  #ifdef SW3
  v=server.arg("sw3"); if (v != "") commandBool(":SXG3,"+v+"#");
  #endif
  #ifdef SW4
  v=server.arg("sw4"); if (v != "") commandBool(":SXG4,"+v+"#");
  #endif
  #ifdef SW5
  v=server.arg("sw5"); if (v != "") commandBool(":SXG5,"+v+"#");
  #endif
  #ifdef SW6
  v=server.arg("sw6"); if (v != "") commandBool(":SXG6,"+v+"#");
  #endif
  #ifdef SW7
  v=server.arg("sw7"); if (v != "") commandBool(":SXG7,"+v+"#");
  #endif
  #ifdef SW8
  v=server.arg("sw8"); if (v != "") commandBool(":SXG8,"+v+"#");
  #endif

  // General purpose analog
  strcpy(temp,"");
  #ifdef AN3
  v=server.arg("an3"); if (v != "") sprintf(temp,":SXG3,%ld#",(v.toInt()*255L)/100L);
  #endif
  #ifdef AN4
  v=server.arg("an4"); if (v != "") sprintf(temp,":SXG4,%ld#",(v.toInt()*255L)/100L);
  #endif
  #ifdef AN5
  v=server.arg("an5"); if (v != "") sprintf(temp,":SXG5,%ld#",(v.toInt()*255L)/100L);
  #endif
  #ifdef AN6
  v=server.arg("an6"); if (v != "") sprintf(temp,":SXG6,%ld#",(v.toInt()*255L)/100L);
  #endif
  #ifdef AN7
  v=server.arg("an7"); if (v != "") sprintf(temp,":SXG7,%ld#",(v.toInt()*255L)/100L);
  #endif
  #ifdef AN8
  v=server.arg("an8"); if (v != "") sprintf(temp,":SXG8,%ld#",(v.toInt()*255L)/100L);
  #endif
  if (strlen(temp) > 0) commandBool(temp);
}
