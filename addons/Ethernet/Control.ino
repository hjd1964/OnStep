// -----------------------------------------------------------------------------------
// Telescope control related functions 

#ifdef SPECIAL_CHARS_ON
  #define RESET_CH "@"
  #define HOME_CH "&#x1F3E0;"
  #define ARROW_DR "&#x27A5;"
  #define ARROW_UR "&#x27A6;"
  #define ARROW_R2 "&#x27A4;"
  #define CAUTION_CH "&#9888;"
  #define CLOCK_CH "&#x1F565;"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "&#9733;"
  #define LUNAR_CH "&#9790;"
  #define SOLAR_CH "&#9737;"
#else
  #define RESET_CH "@"
  #define HOME_CH "H"
  #define ARROW_DR "-&gt;"
  #define ARROW_UR "-&gt;"
  #define ARROW_R2 "&gt;"
  #define CAUTION_CH "/!\\"
  #define CLOCK_CH "T";
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "*"
  #define LUNAR_CH "("
  #define SOLAR_CH "O"
#endif

#define BUTTON_N "N"
#define BUTTON_S "S"
#define BUTTON_E "E"
#define BUTTON_W "W"
#define BUTTON_SYNC "@"

const char html_controlScript1[] =
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

const char html_controlScript2[] =
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

const char html_controlQuick1[] =
"<div style='text-align: center; width: 30em'>"
"<form style='display: inline;' method='get' action='/control.htm'>"
"<button name='qb' class='bb' value='st' type='submit' onpointerdown='SetDateTime();'>" CLOCK_CH "</button>"
"&nbsp;&nbsp;";
const char html_controlQuick1a[] =
"<input id='dm' type='hidden' name='dm'><input id='dd' type='hidden' name='dd'><input id='dy' type='hidden' name='dy'>"
"<input id='th' type='hidden' name='th'><input id='tm' type='hidden' name='tm'><input id='ts' type='hidden' name='ts'>";
const char html_controlQuick2[] =
"</form>"
"<button type='button' class='bb' onpointerdown=\"g('qc')\">" ARROW_R2 HOME_CH ARROW_R2 "</button>";
const char html_controlQuick3[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"g('qh')\">" ARROW_DR HOME_CH "</button>"
"<button type='button' class='bb' onpointerdown=\"g('qr')\">" CAUTION_CH HOME_CH "</button>";
const char html_controlQuick4[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"g('pu')\">P" ARROW_UR "</button>"
"<button type='button' class='bb' onpointerdown=\"g('pk')\">" ARROW_DR "P</button>";
const char html_controlQuick5[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' style=\"height: 2.2em; color: White;\" onpointerdown=\"g('qq')\">Stop!</button>"
"</div><br class='clear' />\r\n";

const char html_controlTrack1[] = 
"<div class='b1' style='width: 8em'>"
"<div align='left'>Tracking:</div>"
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Ts')\" type='submit'>" SIDEREAL_CH "</button>";
const char html_controlTrack2[] =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Tl')\" type='submit'>" LUNAR_CH "</button>";
const char html_controlTrack3[] =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"g('Th')\" type='submit'>" SOLAR_CH "</button>"
"</div>";
const char html_controlAlign1[] = 
"<div class='b1' style='width: 16.2em'>"
"<div align='left'>Align:</div>"
"<form method='get' action='/control.htm'>";
const char html_controlAlign2[] = 
"<button class='bbh' type='submit' name=\"al\" value='%d'>%d%s</button>";
const char html_controlAlign3[] = 
"&nbsp;&nbsp;&nbsp;<button class='bbh' type='submit' name=\"al\" value=\"n\">Accept</button></form>";
const char html_controlTrack4[] = 
"</div><br class='clear' />\r\n";

const char html_controlGuide1[] =
"<div class='b1' style='width: 27em'>"
"<div align='left'>Guide:</div>"
"<button class='gb' type='button' onpointerdown=\"g('n1')\" onpointerup=\"g('n0')\">" BUTTON_N "</button><br />";
const char html_controlGuide2[] = 
"<button class='gb' type='button' onpointerdown=\"g('e1')\" onpointerup=\"g('e0')\">" BUTTON_E "</button>";
const char html_controlGuide3[] = 
"<button class='gb' type='button' onpointerdown=\"g('sy')\">" BUTTON_SYNC "</button>"
"<button class='gb' type='button' onpointerdown=\"g('w1')\" onpointerup=\"g('w0')\">" BUTTON_W "</button><br />";
const char html_controlGuide4[] = 
"<button class='gb' type='button' onpointerdown=\"g('s1')\" onpointerup=\"g('s0')\">" BUTTON_S "</button><br /><br />";
const char html_controlGuide5[] =
"<form method='get' action=\"/control.htm\">"
"<button class='bbh' type='button' onpointerdown=\"g('R0')\">0.25x</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R1')\">0.5x</button>";
const char html_controlGuide6[] = 
"<button class='bbh' type='button' onpointerdown=\"g('R2')\">1x</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R4')\">Mid</button>";
const char html_controlGuide7[] = 
"<button class='bbh' type='button' onpointerdown=\"g('R6')\">Fast</button>"
"<button class='bbh' type='button' onpointerdown=\"g('R8')\">VFast</button>"
"</div><br class='clear' />\r\n";

const char html_controlFocus1[] =
"<div class='b1' style='width: 27em'>";
const char html_controlFocus2[] =
"<button class='bbh' type='button' onpointerdown=\"gf('F1')\" >1</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('F2')\" >2</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus3[] = 
"<button class='bbh' type='button' onpointerdown=\"gf('Fz')\" >" RESET_CH "</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('Fh')\" >" ARROW_DR HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus4[] =
"<button class='bbh' type='button' onpointerdown=\"gf('FI')\" onpointerup=\"g('Fq');\" >" ARROW_DD "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('Fi')\" onpointerup=\"g('Fq')\" >" ARROW_D "</button>";
const char html_controlFocus5[] =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('Fo')\" onpointerup=\"g('Fq')\" >" ARROW_U "</button>"
"<button class='bbh' type='button' onpointerdown=\"gf('FO')\" onpointerup=\"g('Fq')\" >" ARROW_UU "</button>";
const char html_controlFocus6[] =
"</div><br class='clear' />\r\n";

const char html_controlRotate0[] = 
"<div class='b1' style='width: 27em'>";
const char html_controlRotate1[] = 
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('re')\" >" RESET_CH "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('ho')\" >" ARROW_DR HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlRotate2[] = 
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('b2')\" >" ARROW_LL "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('b1')\" >" ARROW_L "</button>";
const char html_controlRotate3[] =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"gf('f1')\" >" ARROW_R "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"gf('f2')\" >" ARROW_RR "</button><br />";
const char html_controlDeRotate1[] = 
"<button type='button' onpointerdown=\"gf('d1')\" >De-Rotate On</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"gf('dr')\" >Rev</button>";
const char html_controlDeRotate2[] = 
"<button type='button' onpointerdown=\"gf('dp')\" >P</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"gf('d0')\" >De-Rotate Off</button>";
const char html_controlRotate4[] =
"</div><br class='clear' />\r\n";

#if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8) || defined(SW9) || defined(SW10) || defined(SW11) || defined(SW12) || defined(SW13) || defined(SW14) || defined(SW15) || defined(AN3) || defined(AN4) || defined(AN5) || defined(AN6) || defined(AN7) || defined(AN8)
const char html_controlAuxB[] = "<div class='b1' style='width: 27em'><div align='left'>Aux:</div>";
#ifdef SW0
const char html_controlSwitch0[] = SW0 "<br /><button type='button' onpointerdown=\"s('sw0','255')\" >On</button><button type='button' onpointerdown=\"s('sw0','0')\" >Off</button><br />";
#endif
#ifdef SW1
const char html_controlSwitch1[] = SW1 "<br /><button type='button' onpointerdown=\"s('sw1','255')\" >On</button><button type='button' onpointerdown=\"s('sw1','0')\" >Off</button><br />";
#endif
#ifdef SW2
const char html_controlSwitch2[] = SW2 "<br /><button type='button' onpointerdown=\"s('sw2','255')\" >On</button><button type='button' onpointerdown=\"s('sw2','0')\" >Off</button><br />";
#endif
#ifdef SW3
const char html_controlSwitch3[] = SW3 "<br /><button type='button' onpointerdown=\"s('sw3','255')\" >On</button><button type='button' onpointerdown=\"s('sw3','0')\" >Off</button><br />";
#endif
#ifdef SW4
const char html_controlSwitch4[] = SW4 "<br /><button type='button' onpointerdown=\"s('sw4','255')\" >On</button><button type='button' onpointerdown=\"s('sw4','0')\" >Off</button><br />";
#endif
#ifdef SW5
const char html_controlSwitch5[] = SW5 "<br /><button type='button' onpointerdown=\"s('sw5','255')\" >On</button><button type='button' onpointerdown=\"s('sw5','0')\" >Off</button><br />";
#endif
#ifdef SW6
const char html_controlSwitch6[] = SW6 "<br /><button type='button' onpointerdown=\"s('sw6','255')\" >On</button><button type='button' onpointerdown=\"s('sw6','0')\" >Off</button><br />";
#endif
#ifdef SW7
const char html_controlSwitch7[] = SW7 "<br /><button type='button' onpointerdown=\"s('sw7','255')\" >On</button><button type='button' onpointerdown=\"s('sw7','0')\" >Off</button><br />";
#endif
#ifdef SW8
const char html_controlSwitch8[] = SW8 "<br /><button type='button' onpointerdown=\"s('sw8','255')\" >On</button><button type='button' onpointerdown=\"s('sw8','0')\" >Off</button><br />";
#endif
#ifdef SW9
const char html_controlSwitch9[] = SW9 "<br /><button type='button' onpointerdown=\"s('sw9','255')\" >On</button><button type='button' onpointerdown=\"s('sw9','0')\" >Off</button><br />";
#endif
#ifdef SW10
const char html_controlSwitch10[] = SW10 "<br /><button type='button' onpointerdown=\"s('swA','255')\" >On</button><button type='button' onpointerdown=\"s('swA','0')\" >Off</button><br />";
#endif
#ifdef SW11
const char html_controlSwitch11[] = SW11 "<br /><button type='button' onpointerdown=\"s('swB','255')\" >On</button><button type='button' onpointerdown=\"s('swB','0')\" >Off</button><br />";
#endif
#ifdef SW12
const char html_controlSwitch12[] = SW12 "<br /><button type='button' onpointerdown=\"s('swC','255')\" >On</button><button type='button' onpointerdown=\"s('swC','0')\" >Off</button><br />";
#endif
#ifdef SW13
const char html_controlSwitch13[] = SW13 "<br /><button type='button' onpointerdown=\"s('swD','255')\" >On</button><button type='button' onpointerdown=\"s('swD','0')\" >Off</button><br />";
#endif
#ifdef SW14
const char html_controlSwitch14[] = SW14 "<br /><button type='button' onpointerdown=\"s('swE','255')\" >On</button><button type='button' onpointerdown=\"s('swE','0')\" >Off</button><br />";
#endif
#ifdef SW15
const char html_controlSwitch15[] = SW15 "<br /><button type='button' onpointerdown=\"s('swF','255')\" >On</button><button type='button' onpointerdown=\"s('swF','0')\" >Off</button><br />";
#endif
#ifdef AN3
const char html_controlAnalog3A[] = AN3 " <span id='an3v'>";
const char html_controlAnalog3B[] ="</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog3C[] = "' onchange=\"sf('an3',this.value)\"><br />";
#endif
#ifdef AN4
const char html_controlAnalog4A[] = AN4 " <span id='an4v'>";
const char html_controlAnalog4B[] ="</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog4C[] = "' onchange=\"sf('an4',this.value)\"><br />";
#endif
#ifdef AN5
const char html_controlAnalog5A[] = AN5 " <span id='an5v'>";
const char html_controlAnalog5B[] = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog5C[] = "' onchange=\"sf('an5',this.value)\"><br />";
#endif
#ifdef AN6
const char html_controlAnalog6A[] = AN6 " <span id='an6v'>";
const char html_controlAnalog6B[] = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog6C[] = "' onchange=\"sf('an6',this.value)\"><br />";
#endif
#ifdef AN7
const char html_controlAnalog7A[] = AN7 " <span id='an7v'>";
const char html_controlAnalog7B[] = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog7C[] = "' onchange=\"sf('an7',this.value)\"><br />";
#endif
#ifdef AN8
const char html_controlAnalog8A[] = AN8 " <span id='an8v'>";
const char html_controlAnalog8B[] = "</span>%<br /><input style='width: 80%; background: #111' type='range' value='";
const char html_controlAnalog8C[] = "' onchange=\"sf('an8',this.value)\"><br />";
#endif
const char html_controlAuxE[] = "</div><br class='clear' />\r\n";
#endif


const char html_controlEnd[] = 
"<br />\r\n";

#ifdef OETHS
void handleControl(EthernetClient *client) {
#else
void handleControl() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
    
  char temp1[24]="";

  processControlGet();

  sendHtmlStart();
  
  String data=html_headB;
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

  // get status
  mountStatus.update();

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2S;
  data += html_links3N;
#ifdef ENCODERS_ON
  data += html_linksEncN;
#endif
  sendHtml(data);
  data += html_links4N;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  sendHtml(data);

  // guide (etc) script
  data += html_controlScript1;
  // clock script
  data += html_controlScript2;
  data += html_controlScript3;
  data += html_controlScript4;

  // active ajax page is: controlAjax();
  data +="<script>var ajaxPage='control.txt';</script>\n";
  data +=html_ajax_active;
  sendHtml(data);

  // Quick controls ------------------------------------------
  data += html_controlQuick1;
  data += html_controlQuick1a;
  if (mountStatus.mountType()==MT_GEM) data += html_controlQuick2;
  data += html_controlQuick3;
  data += html_controlQuick4;
  data += html_controlQuick5;
  sendHtml(data);

  // Tracking control ----------------------------------------
  data += html_controlTrack1;
  data += html_controlTrack2;
  data += html_controlTrack3;
  sendHtml(data);

  // Get the align mode --------------------------------------
  data += html_controlAlign1;
  byte sc[3];
  int n=1;
  if (mountStatus.alignMaxStars()<3) { n=1; sc[0]=1; } else
  if (mountStatus.alignMaxStars()<4) { n=3; sc[0]=1; sc[1]=2; sc[2]=3; } else
  if (mountStatus.alignMaxStars()<6) { n=3; sc[0]=1; sc[1]=3; sc[2]=4; } else
  if (mountStatus.alignMaxStars()<8) { n=3; sc[0]=1; sc[1]=3; sc[2]=6; } else
                                     { n=3; sc[0]=1; sc[1]=3; sc[2]=9; }
  for (int i=0; i<n; i++) { char temp2[120]=""; sprintf(temp2,html_controlAlign2,sc[i],sc[i],SIDEREAL_CH); data+=temp2; }
  data += html_controlAlign3;
  sendHtml(data);
  
  // Tracking ------------------------------------------------
  data += html_controlTrack4;

  // Guiding -------------------------------------------------
  data += html_controlGuide1;
  data += html_controlGuide2;
  data += html_controlGuide3;
  data += html_controlGuide4;
  sendHtml(data);
  data += html_controlGuide5;
  data += html_controlGuide6;
  data += html_controlGuide7;
  sendHtml(data);

  // Focusing ------------------------------------------------
  boolean Focuser1; if (sendCommand(":FA#",temp1,R_BOOL)) Focuser1=true; else Focuser1=false;
  boolean Focuser2; if (sendCommand(":fA#",temp1,R_BOOL)) Focuser2=true; else Focuser2=false;
  if (Focuser1) {
    data += html_controlFocus1;
    data += "<div style='float: left;'>Focuser:</div><div style='float: right; text-align: right;' id='focuserpos'>?</div><br />";
    if (Focuser2) data += html_controlFocus2;
    data += html_controlFocus3;
    data += html_controlFocus4;
    data += html_controlFocus5;
    data += html_controlFocus6;
  sendHtml(data);
  }

  // Rotate/De-Rotate ----------------------------------------
  boolean Rotate=false;
  boolean DeRotate=false;
  if (sendCommand(":GX98#",temp1)) {
    if (temp1[0]=='R') Rotate=true;
    if (temp1[0]=='D') { Rotate=true; DeRotate=true; }
  }
  if (Rotate) {
    data += html_controlRotate0;
    data += "<div style='float: left;'>Rotator:</div><div style='float: right; text-align: right;' id='rotatorpos'>?</div><br />";
    data += html_controlRotate1;
    data += html_controlRotate2;
    data += html_controlRotate3;
  }
  if (DeRotate) {
    data += html_controlDeRotate1;
    data += html_controlDeRotate2;
  }
  if (Rotate) {
    data += html_controlRotate4;
  }
  sendHtml(data);

  // Aux -----------------------------------------------------
  #if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8) || defined(SW9) || defined(SW10) || defined(SW11) || defined(SW12) || defined(SW13) || defined(SW14) || defined(SW15) || defined(AN3) || defined(AN4) || defined(AN5) || defined(AN6) || defined(AN7) || defined(AN8)
    data += html_controlAuxB;
    // Digital Control
    int c=0;
    #ifdef SW0
    data += html_controlSwitch0; c++;
    #endif
    #ifdef SW1
    data += html_controlSwitch1; c++;
    #endif
    #ifdef SW2
    data += html_controlSwitch2; c++;
    #endif
    #ifdef SW3
    data += html_controlSwitch3; c++;
    #endif
    #ifdef SW4
    data += html_controlSwitch4; c++;
    #endif
    #ifdef SW5
    data += html_controlSwitch5; c++;
    #endif
    #ifdef SW6
    data += html_controlSwitch6; c++;
    #endif
    #ifdef SW7
    data += html_controlSwitch7; c++;
    #endif
    #ifdef SW8
    data += html_controlSwitch8; c++;
    #endif
    #ifdef SW9
    data += html_controlSwitch9; c++;
    #endif
    #ifdef SW10
    data += html_controlSwitch10; c++;
    #endif
    #ifdef SW11
    data += html_controlSwitch11; c++;
    #endif
    #ifdef SW12
    data += html_controlSwitch12; c++;
    #endif
    #ifdef SW13
    data += html_controlSwitch13; c++;
    #endif
    #ifdef SW14
    data += html_controlSwitch14; c++;
    #endif
    #ifdef SW15
    data += html_controlSwitch15; c++;
    #endif
    if (c>0) data+="<br />";
    sendHtml(data);

    // Analog Control
    #ifdef AN3
    if (sendCommand(":GXG3#",temp1)) { data += html_controlAnalog3A; data += temp1; data += html_controlAnalog3B; data += temp1; data += html_controlAnalog3C; }
    #endif
    #ifdef AN4
    if (sendCommand(":GXG4#",temp1)) { data += html_controlAnalog4A; data += temp1; data += html_controlAnalog4B; data += temp1; data += html_controlAnalog4C; }
    #endif
    #ifdef AN5
    if (sendCommand(":GXG5#",temp1)) { data += html_controlAnalog5A; data += temp1; data += html_controlAnalog5B; data += temp1; data += html_controlAnalog5C; }
    #endif
    #ifdef AN6
    if (sendCommand(":GXG6#",temp1)) { data += html_controlAnalog6A; data += temp1; data += html_controlAnalog6B; data += temp1; data += html_controlAnalog6C; }
    #endif
    #ifdef AN7
    if (sendCommand(":GXG7#",temp1)) { data += html_controlAnalog7A; data += temp1; data += html_controlAnalog7B; data += temp1; data += html_controlAnalog7C; }
    #endif
    #ifdef AN8
    if (sendCommand(":GXG8#",temp1)) { data += html_controlAnalog8A; data += temp1; data += html_controlAnalog8B; data += temp1; data += html_controlAnalog8C; }
    #endif
    sendHtml(data);

    data += html_controlAuxE;
  #endif

  data += html_controlEnd;
  
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
  char temp[80]="";

  data += "focuserpos|";
  if (sendCommand(":FG#",temp)) { data += temp; data += " microns\n"; } else { data += "?\n"; }

  data += "rotatorpos|";
  if (sendCommand(":rG#",temp)) { temp[9]=temp[5]; temp[10]=temp[6]; temp[11]=0; temp[4]='&'; temp[5]='d'; temp[6]='e'; temp[7]='g'; temp[8]=';'; data += temp; data += "&#39;\n"; } else { data += "?\n"; }

  #ifdef AN3
    data += "an3v|"; if (sendCommand(":GXG3#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN4
    data += "an4v|"; if (sendCommand(":GXG4#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN5
    data += "an5v|"; if (sendCommand(":GXG5#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN6
    data += "an6v|"; if (sendCommand(":GXG6#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN7
    data += "an7v|"; if (sendCommand(":GXG7#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
  #endif
  #ifdef AN8
    data += "an8v|"; if (sendCommand(":GXG8#",temp)) { data += temp; data += "\n"; } else { data += "?\n"; }
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
    if (v=="q") Ser.print(":Q#");       // stop goto/guide
    if (v=="co") Ser.print(":SX99,1#"); // meridian flip, pause->continue
    if (v=="hf") Ser.print(":hF#");     // home, reset
    if (v=="hc") Ser.print(":hC#");     // home, find
    if (v=="pk") Ser.print(":hP#");     // park
    if (v=="pu") Ser.print(":hR#");     // un-park
    Ser.setTimeout(WebTimeout*8);

    // clear any possible response
    temp[Ser.readBytesUntil('#',temp,20)]=0;
  }

  // Align
  v=server.arg("al");
  if (v!="") {
    if (v=="1") Ser.print(":A1#");
    if (v=="2") Ser.print(":A2#");
    if (v=="3") Ser.print(":A3#");
    if (v=="4") Ser.print(":A4#");
    if (v=="5") Ser.print(":A5#");
    if (v=="6") Ser.print(":A6#");
    if (v=="7") Ser.print(":A7#");
    if (v=="8") Ser.print(":A8#");
    if (v=="9") Ser.print(":A9#");
    if (v=="n") Ser.print(":A+#");
    if (v=="q") Ser.print(":Q#");
    Ser.setTimeout(1000);

    // clear any possible response
    temp[Ser.readBytesUntil('#',temp,20)]=0;
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
      Ser.print(temp);
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
      Ser.print(temp);
    }
  }

  v=server.arg("dr");
  if (v!="") {
    // Tracking control
    if (v=="Ts") Ser.print(":TQ#"); // sidereal
    if (v=="Tl") Ser.print(":TL#"); // lunar
    if (v=="Th") Ser.print(":TS#"); // solar

    // quick
    if (v=="qq") { Ser.print(":Q#");  cl(); }     // stop goto/guide
    if (v=="qc") { Ser.print(":SX99,1#"); cl(); } // meridian flip, pause->continue
    if (v=="qr") { Ser.print(":hF#"); cl(); }     // home, reset
    if (v=="qh") { Ser.print(":hC#"); cl(); }     // home, find
    if (v=="pk") { Ser.print(":hP#"); cl(); }     // park
    if (v=="pu") { Ser.print(":hR#"); cl(); }     // un-park

    // GUIDE control direction
    if (v=="n1") Ser.print(":Mn#");
    if (v=="s1") Ser.print(":Ms#");
    if (v=="e1") Ser.print(":Me#");
    if (v=="w1") Ser.print(":Mw#");

    if (v=="n0") Ser.print(":Qn#");
    if (v=="s0") Ser.print(":Qs#");
    if (v=="e0") Ser.print(":Qe#");
    if (v=="w0") Ser.print(":Qw#");

    if (v=="sy") Ser.print(":CS#");

    // GUIDE control rate
    if (v=="R0") Ser.print(":R0#");
    if (v=="R1") Ser.print(":R1#");
    if (v=="R2") Ser.print(":R2#");
    if (v=="R3") Ser.print(":R3#");
    if (v=="R4") Ser.print(":R4#");
    if (v=="R5") Ser.print(":R5#");
    if (v=="R6") Ser.print(":R6#");
    if (v=="R7") Ser.print(":R7#");
    if (v=="R8") Ser.print(":R8#");
    if (v=="R9") Ser.print(":R9#");

    // Focuser
    if (v=="F1") { Ser.print(":FA1#"); temp[Ser.readBytesUntil('#',temp,20)]=0; }
    if (v=="F2") { Ser.print(":FA2#"); temp[Ser.readBytesUntil('#',temp,20)]=0; }
    if (v=="Fz") Ser.print(":FZ#");
    if (v=="Fh") Ser.print(":FS0#");
    if (v=="FI") Ser.print(":FF#:F-#");
    if (v=="Fi") Ser.print(":FS#:F-#");
    if (v=="Fo") Ser.print(":FS#:F+#");
    if (v=="FO") Ser.print(":FF#:F+#");
    if (v=="Fq") Ser.print(":FQ#");
   
    // Rotate/De-Rotate
    if (v=="b2") Ser.print(":r3#:r<#");
    if (v=="b1") Ser.print(":r1#:r<#");
    if (v=="f1") Ser.print(":r1#:r>#");
    if (v=="f2") Ser.print(":r3#:r>#");
    if (v=="ho") Ser.print(":rC#");
    if (v=="re") Ser.print(":rF#");
    if (v=="d0") Ser.print(":r-#");
    if (v=="d1") Ser.print(":r+#");
    if (v=="dr") Ser.print(":rR#");
    if (v=="dp") Ser.print(":rP#");
  }

  // General purpose switches
  #ifdef SW0
  v=server.arg("sw0"); if (v!="") { Ser.print(":SXG0,"+v+"#"); cl(); }
  #endif
  #ifdef SW1
  v=server.arg("sw1"); if (v!="") { Ser.print(":SXG1,"+v+"#"); cl(); }
  #endif
  #ifdef SW2
  v=server.arg("sw2"); if (v!="") { Ser.print(":SXG2,"+v+"#"); cl(); }
  #endif
  #ifdef SW3
  v=server.arg("sw3"); if (v!="") { Ser.print(":SXG3,"+v+"#"); cl(); }
  #endif
  #ifdef SW4
  v=server.arg("sw4"); if (v!="") { Ser.print(":SXG4,"+v+"#"); cl(); }
  #endif
  #ifdef SW5
  v=server.arg("sw5"); if (v!="") { Ser.print(":SXG5,"+v+"#"); cl(); }
  #endif
  #ifdef SW6
  v=server.arg("sw6"); if (v!="") { Ser.print(":SXG6,"+v+"#"); cl(); }
  #endif
  #ifdef SW7
  v=server.arg("sw7"); if (v!="") { Ser.print(":SXG7,"+v+"#"); cl(); }
  #endif
  #ifdef SW8
  v=server.arg("sw8"); if (v!="") { Ser.print(":SXG8,"+v+"#"); cl(); }
  #endif
  #ifdef SW9
  v=server.arg("sw9"); if (v!="") { Ser.print(":SXG9,"+v+"#"); cl(); }
  #endif
  #ifdef SW10
  v=server.arg("swA"); if (v!="") { Ser.print(":SXGA,"+v+"#"); cl(); }
  #endif
  #ifdef SW11
  v=server.arg("swB"); if (v!="") { Ser.print(":SXGB,"+v+"#"); cl(); }
  #endif
  #ifdef SW12
  v=server.arg("swC"); if (v!="") { Ser.print(":SXGC,"+v+"#"); cl(); }
  #endif
  #ifdef SW13
  v=server.arg("swD"); if (v!="") { Ser.print(":SXGD,"+v+"#"); cl(); }
  #endif
  #ifdef SW14
  v=server.arg("swE"); if (v!="") { Ser.print(":SXGE,"+v+"#"); cl(); }
  #endif
  #ifdef SW15
  v=server.arg("swF"); if (v!="") { Ser.print(":SXGF,"+v+"#"); cl(); }
  #endif

  // General purpose analog
  #ifdef AN3
  v=server.arg("an3"); if (v!="") { Ser.printf(":SXG3,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif
  #ifdef AN4
  v=server.arg("an4"); if (v!="") { Ser.printf(":SXG4,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif
  #ifdef AN5
  v=server.arg("an5"); if (v!="") { Ser.printf(":SXG5,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif
  #ifdef AN6
  v=server.arg("an6"); if (v!="") { Ser.printf(":SXG6,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif
  #ifdef AN7
  v=server.arg("an7"); if (v!="") { Ser.printf(":SXG7,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif
  #ifdef AN8
  v=server.arg("an8"); if (v!="") { Ser.printf(":SXG8,%ld#",(v.toInt()*255L)/100L); cl(); }
  #endif

}

// clear any possible response
void cl() {
  char temp[20]="";
  Ser.setTimeout(WebTimeout*8);
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}
