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
"function guide(dir,start) {\n"
  "var xhttp = new XMLHttpRequest();\n"
  "xhttp.open('GET', 'guide.txt?dr='+dir+start+'&x='+new Date().getTime(), true);\n"
  "xhttp.send();\n"
"}\n"
"function guidef(dir,start) { guide(dir,start); autoFastRun(); }\n"
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
"<button type='button' class='bb' onpointerdown=\"guide('q','c')\">" ARROW_R2 HOME_CH ARROW_R2 "</button>";
const char html_controlQuick3[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"guide('q','h')\">" ARROW_DR HOME_CH "</button>"
"<button type='button' class='bb' onpointerdown=\"guide('q','r')\">" CAUTION_CH HOME_CH "</button>";
const char html_controlQuick4[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' onpointerdown=\"guide('p','u')\">P" ARROW_UR "</button>"
"<button type='button' class='bb' onpointerdown=\"guide('p','k')\">" ARROW_DR "P</button>";
const char html_controlQuick5[] =
"&nbsp;&nbsp;"
"<button type='button' class='bb' style=\"height: 2.2em; color: White;\" onpointerdown=\"guide('q','q')\">Stop!</button>"
"</div><br class='clear' />\r\n";

const char html_controlTrack1[] = 
"<div class='b1' style='width: 8em'>"
"<div align='left'>Tracking:</div>"
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"guide('T','s')\" type='submit'>" SIDEREAL_CH "</button>";
const char html_controlTrack2[] =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"guide('T','l')\" type='submit'>" LUNAR_CH "</button>";
const char html_controlTrack3[] =
"<button type='button' class='bbh' style='width: 2em' onpointerdown=\"guide('T','h')\" type='submit'>" SOLAR_CH "</button>"
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
"<button class='gb' type='button' onpointerdown=\"guide('n','1')\" onpointerup=\"guide('n','0')\">" BUTTON_N "</button><br />";
const char html_controlGuide2[] = 
"<button class='gb' type='button' onpointerdown=\"guide('e','1')\" onpointerup=\"guide('e','0')\">" BUTTON_E "</button>";
const char html_controlGuide3[] = 
"<button class='gb' type='button' onpointerdown=\"guide('s','y')\">" BUTTON_SYNC "</button>"
"<button class='gb' type='button' onpointerdown=\"guide('w','1')\" onpointerup=\"guide('w','0')\">" BUTTON_W "</button><br />";
const char html_controlGuide4[] = 
"<button class='gb' type='button' onpointerdown=\"guide('s','1')\" onpointerup=\"guide('s','0')\">" BUTTON_S "</button><br /><br />";
const char html_controlGuide5[] =
"<form method='get' action=\"/control.htm\">"
"<button class='bbh' type='button' onpointerdown=\"guide('R','0')\">0.25x</button>"
"<button class='bbh' type='button' onpointerdown=\"guide('R','1')\">0.5x</button>";
//"<button name='gu' value=\"0\" type='submit'>0.25x</button>"
//"<button name='gu' value=\"1\" type='submit'>0.5x</button>";
const char html_controlGuide6[] = 
"<button class='bbh' type='button' onpointerdown=\"guide('R','2')\">1x</button>"
"<button class='bbh' type='button' onpointerdown=\"guide('R','4')\">Mid</button>";
//"<button name='gu' value=\"2\" type='submit'>1x</button>"
//"<button name='gu' value=\"4\" type='submit'>Mid</button>";
const char html_controlGuide7[] = 
"<button class='bbh' type='button' onpointerdown=\"guide('R','6')\">Fast</button>"
"<button class='bbh' type='button' onpointerdown=\"guide('R','8')\">VFast</button>"
//"<button name='gu' value=\"6\" type='submit'>Fast</button>"
//"<button name='gu' value=\"8\" type='submit'>VFast</button></form>"
"</div><br class='clear' />\r\n";

const char html_controlFocus1[] =
"<div class='b1' style='width: 27em'>";
const char html_controlFocus2[] =
"<button class='bbh' type='button' onpointerdown=\"guidef('F','1')\" >1</button>"
"<button class='bbh' type='button' onpointerdown=\"guidef('F','2')\" >2</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus3[] = 
"<button class='bbh' type='button' onpointerdown=\"guidef('F','z')\" >" RESET_CH "</button>"
"<button class='bbh' type='button' onpointerdown=\"guidef('F','h')\" >" ARROW_DR HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlFocus4[] =
"<button class='bbh' type='button' onpointerdown=\"guidef('F','I')\" onpointerup=\"guide('F','q');\" >" ARROW_DD "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"guidef('F','i')\" onpointerup=\"guide('F','q')\" >" ARROW_D "</button>";
const char html_controlFocus5[] =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"guidef('F','o')\" onpointerup=\"guide('F','q')\" >" ARROW_U "</button>"
"<button class='bbh' type='button' onpointerdown=\"guidef('F','O')\" onpointerup=\"guide('F','q')\" >" ARROW_UU "</button>";
const char html_controlFocus6[] =
"</div><br class='clear' />\r\n";

const char html_controlRotate0[] = 
"<div class='b1' style='width: 27em'>";
const char html_controlRotate1[] = 
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"guidef('r','e')\" >" RESET_CH "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"guidef('h','o')\" >" ARROW_DR HOME_CH "</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_controlRotate2[] = 
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"guidef('b','2')\" >" ARROW_LL "</button>"
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"guidef('b','1')\" >" ARROW_L "</button>";
const char html_controlRotate3[] =
"<button class='bbh' type='button' style='width: 2em' onpointerdown=\"guidef('f','1')\" >" ARROW_R "</button>"
"<button class='bbh' type='button' style='height: 2.1em' onpointerdown=\"guidef('f','2')\" >" ARROW_RR "</button><br />";
const char html_controlDeRotate1[] = 
"<button type='button' onpointerdown=\"guidef('d','1')\" >De-Rotate On</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"guidef('d','r')\" >Rev</button>";
const char html_controlDeRotate2[] = 
"<button type='button' onpointerdown=\"guidef('d','p')\" >P</button>&nbsp;&nbsp;&nbsp;"
"<button type='button' onpointerdown=\"guidef('d','0')\" >De-Rotate Off</button>";
const char html_controlRotate4[] =
"</div><br class='clear' />\r\n";

#if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8)
const char html_controlSwitchB[] = 
"<div class='b1' style='width: 27em'>"
"<div align='left'>Switches:</div>";
#ifdef SW0
const char html_controlSwitch0[] = 
"<button type='button' onpointerdown=\"guidef('o','0')\" >" SW0 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','0')\" >" SW0 " Off</button><br />";
#endif
#ifdef SW1
const char html_controlSwitch1[] = 
"<button type='button' onpointerdown=\"guidef('o','1')\" >" SW1 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','1')\" >" SW1 " Off</button><br />";
#endif
#ifdef SW2
const char html_controlSwitch2[] = 
"<button type='button' onpointerdown=\"guidef('o','2')\" >" SW2 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','2')\" >" SW2 " Off</button><br />";
#endif
#ifdef SW3
const char html_controlSwitch3[] = 
"<button type='button' onpointerdown=\"guidef('o','3')\" >" SW3 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','3')\" >" SW3 " Off</button><br />";
#endif
#ifdef SW4
const char html_controlSwitch4[] = 
"<button type='button' onpointerdown=\"guidef('o','4')\" >" SW4 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','4')\" >" SW4 " Off</button><br />";
#endif
#ifdef SW5
const char html_controlSwitch5[] = 
"<button type='button' onpointerdown=\"guidef('o','5')\" >" SW5 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','5')\" >" SW5 " Off</button><br />";
#endif
#ifdef SW6
const char html_controlSwitch6[] = 
"<button type='button' onpointerdown=\"guidef('o','6')\" >" SW6 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','6')\" >" SW6 " Off</button><br />";
#endif
#ifdef SW7
const char html_controlSwitch7[] = 
"<button type='button' onpointerdown=\"guidef('o','7')\" >" SW7 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','7')\" >" SW7 " Off</button><br />";
#endif
#ifdef SW8
const char html_controlSwitch8[] = 
"<button type='button' onpointerdown=\"guidef('o','8')\" >" SW8 " On</button>"
"<button type='button' onpointerdown=\"guidef('x','8')\" >" SW8 " Off</button><br />";
#endif
const char html_controlSwitchE[] =
"</div><br class='clear' />\r\n";
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
    
  char temp1[120]="";
  char temp2[120]="";

  processControlGet();
  
  String data=html_headB;
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
  char stat[22] = "";
  Ser.print(":GU#");
  stat[Ser.readBytesUntil('#',stat,20)]=0;

  // finish the standard http response header
  data += html_onstep_header1;
  Ser.print(":GVP#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; } data += temp2;
  data += html_onstep_header2;
  Ser.print(":GVN#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } data += temp2;
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2S;
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

  // guide (etc) script
  data += html_controlScript1;
  // clock script
  data += html_controlScript2;
  data += html_controlScript3;
  data += html_controlScript4;

  // active ajax page is: controlAjax();
  data +="<script>var ajaxPage='control.txt';</script>\n";
  data +=html_ajax_active;
#ifdef OETHS
  client->print(data); data="";
#endif

  // Quick controls ------------------------------------------
  data += html_controlQuick1;
  data += html_controlQuick1a;
  if (strstr(stat,"E")) data += html_controlQuick2; // GEM only
  data += html_controlQuick3;
  data += html_controlQuick4;
  data += html_controlQuick5;
#ifdef OETHS
  client->print(data); data="";
#endif

  // Tracking control ----------------------------------------
  data += html_controlTrack1;
  data += html_controlTrack2;
  data += html_controlTrack3;
#ifdef OETHS
  client->print(data); data="";
#endif

  // Get the align mode --------------------------------------
  if (AlignMaxNumStars==-1) {
    Ser.print(":A?#");
    temp1[Ser.readBytesUntil('#',temp1,20)]=0;
    AlignMaxNumStars=3;
    if (temp1[0]!=0) {
      if ((temp1[0]>'0') && (temp1[0]<'9')) AlignMaxNumStars=temp1[0]-'0';
    }
    if (AlignMaxNumStars>9) AlignMaxNumStars=9;
    if (AlignMaxNumStars<1) AlignMaxNumStars=1;
  }
  data += html_controlAlign1;
  if (AlignMaxNumStars<=3) {
    for (int i=1; i<=AlignMaxNumStars; i++) { sprintf(temp2,html_controlAlign2,i,i,SIDEREAL_CH); data+=temp2; }
  } else {
    sprintf(temp2,html_controlAlign2,1,1,SIDEREAL_CH); data+=temp2;
    sprintf(temp2,html_controlAlign2,4,4,SIDEREAL_CH); data+=temp2;
    sprintf(temp2,html_controlAlign2,6,6,SIDEREAL_CH); data+=temp2;
  }
  data += html_controlAlign3;
#ifdef OETHS
  client->print(data); data="";
#endif
  
  // Tracking ------------------------------------------------
  data += html_controlTrack4;

  // Guiding -------------------------------------------------
  data += html_controlGuide1;
  data += html_controlGuide2;
  data += html_controlGuide3;
  data += html_controlGuide4;
  data += html_controlGuide5;
  data += html_controlGuide6;
  data += html_controlGuide7;
#ifdef OETHS
  client->print(data); data="";
#endif

  // Focusing ------------------------------------------------
  boolean Focuser1 = false;
  boolean Focuser2 = false;
  Ser.print(":FA#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (temp2[0]=='1') Focuser1=true; else Focuser1=false;
  Ser.print(":fA#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (temp2[0]=='1') Focuser2=true; else Focuser2=false;

  if (Focuser1) {
    data += html_controlFocus1;
    data += "<div style='float: left;'>Focuser:</div><div style='float: right; text-align: right;' id='focuserpos'>?</div><br />";
    if (Focuser2) data += html_controlFocus2;
    data += html_controlFocus3;
    data += html_controlFocus4;
    data += html_controlFocus5;
    data += html_controlFocus6;
#ifdef OETHS
    client->print(data); data="";
#endif
  }

  // Rotate/De-Rotate ----------------------------------------
  boolean Rotate=false;
  boolean DeRotate=false;
  Ser.print(":GX98#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0;
  if (temp2[0]=='R') { Rotate=true; DeRotate=false; }
  if (temp2[0]=='D') { Rotate=true; DeRotate=true; }
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
#ifdef OETHS
    client->print(data); data="";
#endif
  }

  // Rotate/De-Rotate ----------------------------------------
  #if defined(SW0) || defined(SW1) || defined(SW2) || defined(SW3) || defined(SW4) || defined(SW5) || defined(SW6) || defined(SW7) || defined(SW8)
    data += html_controlSwitchB;
    #ifdef SW0
    data += html_controlSwitch0;
    #endif
    #ifdef SW1
    data += html_controlSwitch1;
    #endif
    #ifdef SW2
    data += html_controlSwitch2;
    #endif
    #ifdef SW3
    data += html_controlSwitch3;
    #endif
    #ifdef SW4
    data += html_controlSwitch4;
    #endif
    #ifdef SW5
    data += html_controlSwitch5;
    #endif
    #ifdef SW6
    data += html_controlSwitch6;
    #endif
    #ifdef SW7
    data += html_controlSwitch7;
    #endif
    #ifdef SW8
    data += html_controlSwitch8;
    #endif
    data += html_controlSwitchE;
  #endif

  data += html_controlEnd;
  
  data += "</div></body></html>";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/html",data);
#endif
}

#ifdef OETHS
void guideAjax(EthernetClient *client) {
#else
void guideAjax() {
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
  char temp[40]="";
  
  Ser.print(":FG#");
  temp[Ser.readBytesUntil('#',temp,20)]=0;
  data += "focuserpos|";
  if (temp[0]!=0) { data += temp; data += " microns\n"; } else { data += "?\n"; }

  Ser.print(":rG#");
  temp[Ser.readBytesUntil('#',temp,20)]=0;
  data += "rotatorpos|";
  if (temp[0]!=0) { temp[9]=temp[5]; temp[10]=temp[6]; temp[11]=0; temp[4]='&'; temp[5]='d'; temp[6]='e'; temp[7]='g'; temp[8]=';'; data += temp; data += "&#39;\n"; } else { data += "?\n"; }

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
    Ser.setTimeout(WebTimeout*4);

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

    // Control general purpose switches
    #ifdef SW0
    if (v=="o0") { Ser.print(":SXG0,255#"); cl(); }
    if (v=="x0") { Ser.print(":SXG0,0#"); cl(); }
    #endif
    #ifdef SW1
    if (v=="o1") { Ser.print(":SXG1,255#"); cl(); }
    if (v=="x1") { Ser.print(":SXG1,0#"); cl(); }
    #endif
    #ifdef SW2
    if (v=="o2") { Ser.print(":SXG2,255#"); cl(); }
    if (v=="x2") { Ser.print(":SXG2,0#"); cl(); }
    #endif
    #ifdef SW3
    if (v=="o3") { Ser.print(":SXG3,255#"); cl(); }
    if (v=="x3") { Ser.print(":SXG3,0#"); cl(); }
    #endif
    #ifdef SW4
    if (v=="o4") { Ser.print(":SXG4,255#"); cl(); }
    if (v=="x4") { Ser.print(":SXG4,0#"); cl(); }
    #endif
    #ifdef SW5
    if (v=="o5") { Ser.print(":SXG5,255#"); cl(); }
    if (v=="x5") { Ser.print(":SXG5,0#"); cl(); }
    #endif
    #ifdef SW6
    if (v=="o6") { Ser.print(":SXG6,255#"); cl(); }
    if (v=="x6") { Ser.print(":SXG6,0#"); cl(); }
    #endif
    #ifdef SW7
    if (v=="o7") { Ser.print(":SXG7,255#"); cl(); }
    if (v=="x7") { Ser.print(":SXG7,0#"); cl(); }
    #endif
    #ifdef SW8
    if (v=="o8") { Ser.print(":SXG8,255#"); cl(); }
    if (v=="x8") { Ser.print(":SXG8,0#"); cl(); }
    #endif

  }
}

// clear any possible response
void cl() {
  char temp[20]="";
  Ser.setTimeout(WebTimeout*8);
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}

