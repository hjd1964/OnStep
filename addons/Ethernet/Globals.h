// -----------------------------------------------------------------------------------
// Global variables / pin maps

#if DISPLAY_RESET_CONTROLS == FWU
  #if SERIAL_SWAP != OFF
    #error "Configuration (Config.h): Setting DISPLAY_RESET_CONTROLS FWU must be used with SERIAL_SWAP OFF only"
  #endif
#endif

#ifdef ESP32
  #define AXIS1_ENC_A_PIN 18 // pin# for Axis1 encoder, for A or CW
  #define AXIS1_ENC_B_PIN 19 // pin# for Axis1 encoder, for B or CCW
  #define AXIS2_ENC_A_PIN 22 // pin# for Axis1 encoder, for A or CW
  #define AXIS2_ENC_B_PIN 21 // pin# for Axis1 encoder, for B or CCW
  #ifndef LED_STATUS_ON_STATE
    #define LED_STATUS_ON_STATE HIGH
  #endif
#elif ESP8266
  #if DISPLAY_RESET_CONTROLS == FWU
    #define BOOT0_PIN     13 // pin D7, GPIO13 to Boot0 of STM32 (no swapped serial if active)
  #endif
  #define AXIS1_ENC_A_PIN 14 // pin# for Axis1 encoder, for A or CW
  #define AXIS1_ENC_B_PIN 12 // pin# for Axis1 encoder, for B or CCW
  #define AXIS2_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
  #define AXIS2_ENC_B_PIN 4  // pin# for Axis1 encoder, for B or CCW
#else // Teensy's etc.
  #define AXIS1_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
  #define AXIS1_ENC_B_PIN 6  // pin# for Axis1 encoder, for B or CCW
  #define AXIS2_ENC_A_PIN 7  // pin# for Axis2 encoder, for A or CW
  #define AXIS2_ENC_B_PIN 8  // pin# for Axis2 encoder, for B or CCW
#endif

#if !defined(LED_STATUS_ON_STATE)
  #define LED_STATUS_ON_STATE LOW
  #define LED_STATUS_OFF_STATE HIGH
#elif LED_STATUS_ON_STATE == LOW
  #define LED_STATUS_OFF_STATE HIGH
#elif LED_STATUS_ON_STATE == HIGH
  #define LED_STATUS_OFF_STATE LOW
#endif

const char html_headB[] PROGMEM = "<!DOCTYPE HTML>\r\n"
                                  "<html>\r\n"
                                  "<head>\r\n"
                                  "<meta charset='utf-8'/>\r\n"
                                  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\r\n";
const char html_headerPec[] PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/pec.htm\">\r\n";
const char html_headerIdx[] PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/index.htm\">\r\n";
const char html_headE[] PROGMEM = "</head>\r\n";
const char html_bodyB[] PROGMEM = "<body bgcolor='#26262A'>\r\n";

const char html_main_cssB[] PROGMEM = "<STYLE>";
const char html_main_css1[] PROGMEM = ".clear { clear: both; } .a { background-color: #111111; } .t { padding: 10px 10px 20px 10px; width: 600px; border: 5px solid #551111; margin: 25px 25px 0px 25px;";
const char html_main_css2[] PROGMEM = "color: #999999; background-color: #111111; min-width: 30em; } input { text-align:center; padding: 2px; margin: 3px; font-weight: bold; width:5em; background-color: #B02020}";
const char html_main_css3[] PROGMEM = ".b { padding: 10px; border-left: 5px solid #551111; border-right: 5px solid #551111; border-bottom: 5px solid #551111; margin: 0px 25px 25px 25px; width: 600px; color: #999999;";
const char html_main_css4[] PROGMEM = "background-color: #111111; min-width: 30em; } select { width:4em; font-weight: bold; background-color: #B02020; padding: 2px 2px; } .c { color: #B02020; font-weight: bold; }";
const char html_main_css5[] PROGMEM = "h1 { text-align: right; } a:hover, a:active { background-color: red; } .y { color: #FFFF00; font-weight: bold; }";
const char html_main_css6[] PROGMEM = "a:link, a:visited { background-color: #332222; color: #A07070; border:1px solid red; padding: 5px 10px;";
const char html_main_css7[] PROGMEM = " margin: none; text-align: center; text-decoration: none; display: inline-block; }";
const char html_main_css8[] PROGMEM = "button:disabled { background-color: #733; color: #000; } button { background-color: #B02020; font-weight: bold; border-radius: 5px; margin: 2px; padding: 4px 8px; }";
const char html_main_css_btns1[] PROGMEM = ".btns_left { margin-left: -1px; border-top-left-radius: 0px; border-bottom-left-radius: 0px; }";
const char html_main_css_btns2[] PROGMEM = ".btns_mid { margin-left: -1px; margin-right: -1px; border-top-left-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 0px; border-bottom-right-radius: 0px; }";
const char html_main_css_btns3[] PROGMEM = ".btns_right { margin-right: -1px; border-top-right-radius: 0px; border-bottom-right-radius: 0px; }";
const char html_main_css_control1[] PROGMEM = ".b1 { float: left; border: 2px solid #551111; background-color: #181818; text-align: center; margin: 5px; padding: 15px; padding-top: 3px; }";
const char html_main_css_control2[] PROGMEM = ".gb {  font-weight: bold; font-size: 150%; font-family: 'Times New Roman', Times, serif; width: 60px; height: 50px; padding: 0px; }";
const char html_main_css_control3[] PROGMEM = ".bb {  font-weight: bold; font-size: 105%; } .bbh {  font-weight: bold; font-size: 100%; height: 2.1em; }";
const char html_main_css_collapse1[] PROGMEM = ".collapsible { background-color: #500808; color: #999; cursor: pointer; padding: 7px; width: 80%; border: none; text-align: left; outline: none; font-size: 14px; }";
const char html_main_css_collapse2[] PROGMEM = ".active, .collapsible:hover { background-color: #661111; } .content { padding: 0px 18px; display: none; overflow: hidden; background-color: #301212; }";
const char html_main_cssE[] PROGMEM = "</STYLE>";

const char html_bad_comms_message[] PROGMEM =
  "<br /><bigger><font class=\"y\">" L_DOWN_TITLE "</font></bigger><br /><br />"
  L_DOWN_MESSAGE1 L_DOWN_MESSAGE2
  " <li>" L_DOWN_MESSAGE3 "</li><br />"
  " <li>" L_DOWN_MESSAGE4 "</li><br />"
  " <li>" L_DOWN_MESSAGE5 "</li><br />"
  " <li>" L_DOWN_MESSAGE6 "</li><br />"
  " <li>" L_DOWN_MESSAGE7 "</li><br />"
#if DISPLAY_RESET_CONTROLS != OFF
  "<br /><form method='get' action='/configuration.htm'>"
  "<button name='advanced' type='submit' value='reset' onclick=\"return confirm('" L_ARE_YOU_SURE "?');\" >" L_RESET "</button></form>\r\n"
#endif
  "</ul></div><br class=\"clear\" />\r\n"
  "</div></body></html>";

const char html_onstep_header1[] PROGMEM = "<div class='t'><table width='100%%'><tr><td><b><font size='5'>";
const char html_onstep_header2[] PROGMEM = "</font></b></td><td align='right'><b>" Product " " Version " (OnStep ";
const char html_onstep_header3[] PROGMEM = ")</b></td></tr></table>";
const char html_onstep_header4[] PROGMEM = "</div><div class='b'>\r\n";

const char html_linksStatS[] PROGMEM = "<a href='/index.htm' style='background-color: #552222;'>" L_PAGE_STATUS "</a>";
const char html_linksStatN[] PROGMEM = "<a href='/index.htm'>" L_PAGE_STATUS "</a>";
const char html_linksCtrlS[] PROGMEM = "<a href='/control.htm' style='background-color: #552222;'>" L_PAGE_CONTROL "</a>";
const char html_linksCtrlN[] PROGMEM = "<a href='/control.htm'>" L_PAGE_CONTROL "</a>";
const char html_linksAuxS[] PROGMEM = "<a href='/auxiliary.htm' style='background-color: #552222;'>" L_PAGE_AUX "</a>";
const char html_linksAuxN[] PROGMEM = "<a href='/auxiliary.htm'>" L_PAGE_AUX "</a>";
const char html_linksLibS[] PROGMEM = "<a href='/library.htm' style='background-color: #552222;'>" L_PAGE_LIBRARY "</a>";
const char html_linksLibN[] PROGMEM = "<a href='/library.htm'>" L_PAGE_LIBRARY "</a>";
const char html_linksEncS[] PROGMEM = "<a href='/enc.htm' style='background-color: #552222;'>" L_PAGE_ENCODERS "</a>";
const char html_linksEncN[] PROGMEM = "<a href='/enc.htm'>" L_PAGE_ENCODERS "</a>";
const char html_linksPecS[] PROGMEM = "<a href='/pec.htm' style='background-color: #552222;'>" L_PAGE_PEC "</a>";
const char html_linksPecN[] PROGMEM = "<a href='/pec.htm'>" L_PAGE_PEC "</a>";
const char html_linksSetS[] PROGMEM = "<a href='/settings.htm' style='background-color: #552222;'>" L_PAGE_SETTINGS "</a>";
const char html_linksSetN[] PROGMEM = "<a href='/settings.htm'>" L_PAGE_SETTINGS "</a>";
const char html_linksCfgS[] PROGMEM = "<a href='/configuration.htm' style='background-color: #552222;'>" L_PAGE_CONFIG "</a>";
const char html_linksCfgN[] PROGMEM = "<a href='/configuration.htm'>" L_PAGE_CONFIG "</a>";
const char html_linksWifiS[] PROGMEM = "<a href='/wifi.htm' style='background-color: #552222;'>" L_PAGE_WIFI "</a><br />";
const char html_linksWifiN[] PROGMEM = "<a href='/wifi.htm'>" L_PAGE_WIFI "</a><br />";

// Javascript for Collapsibles
const char html_collapseScript[] PROGMEM =
"<script>"
"var cc = document.getElementsByClassName('collapsible');"
"var i;"
"for (i = 0; i < cc.length; i++) {"
  "cc[i].addEventListener('click', function() {"
    "this.classList.toggle('active');"
    "var ct = this.nextElementSibling;"
    "if (ct.style.display === 'block') { ct.style.display = 'none'; } else { ct.style.display = 'block'; }"
  "});"
"}"
"</script>\n";

// Javascript for Ajax
// be sure to define "var ajaxPage='control.txt';" etc.
const char html_ajax_active[] PROGMEM =
"<script>\n"
"var auto1Tick=-1;\n"
"var auto2Tick=0;\n"
"var auto2Rate=" DEFAULT_AJAX_RATE ";\n"
"var auto1=setInterval(autoRun1s,1000);\n"
// update once a second for 15 seconds then drop back to once every 5 seconds
"function autoFastRun() {\n"
  "auto2Rate=" DEFAULT_FAST_AJAX_RATE "\n"
  "auto2Tick=" DEFAULT_AJAX_SHED_TIME ";\n"
"}\n"
"function autoRun1s() {\n"
  "auto1Tick++;\n"
  "var i;\n"
  "if (auto2Tick>=0) auto2Tick--;\n"
  "if (auto2Tick==0) auto2Rate=" DEFAULT_AJAX_RATE ";\n"
  "if (auto1Tick%auto2Rate==0) {\n"
    "nocache='?nocache='+Math.random()*1000000;\n"
    "var request = new XMLHttpRequest();\n"
    "request.onreadystatechange = pageReady(ajaxPage);\n"
    "request.open('GET',ajaxPage.toLowerCase()+nocache,true); request.send(null);\n"
  "}"
"}\n"
"function pageReady(aPage) {\n"
  "return function() {\n"
    "if ((this.readyState==4)&&(this.status==200)) {\n"
      "lines=this.responseText.split('\\n');\n"
      "for (var i=0; i<lines.length; i++) {\n"
        "j=lines[i].indexOf('|');m=0;\n"
        "if (j==-1) {j=lines[i].indexOf('&');m=1;}\n"
        "v=lines[i].slice(j+1);\n"
        "k=lines[i].slice(0,j);\n"
        "if (k!=''&&document.getElementById(k)!=null) {"
        " if (m==1) document.getElementById(k).value=v; else "
        " if (v=='disabled') document.getElementById(k).disabled=true; else"
        " if (v=='enabled') document.getElementById(k).disabled=false; else"
        " document.getElementById(k).innerHTML=v;"
        "}\n"
      "}\n"
    "}"
  "}"
"}\n"
"</script>\n";
