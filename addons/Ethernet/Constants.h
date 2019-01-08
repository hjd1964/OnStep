// -----------------------------------------------------------------------------------
// Constants

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

#define DEFAULT_AJAX_RATE "5"        // normally 5 seconds between updates
#define DEFAULT_FAST_AJAX_RATE "1"   // fast update is 1 second/update
#define DEFAULT_AJAX_SHED_TIME "15"  // time before return to normal update rate

const char* html_headB PROGMEM = "<!DOCTYPE HTML>\r\n<html>\r\n<head>\r\n";
const char* html_headerPec PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/pec.htm\">\r\n";
const char* html_headerIdx PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/index.htm\">\r\n";
const char* html_headE PROGMEM = "</head>\r\n";
const char* html_bodyB PROGMEM = "<body bgcolor='#26262A'>\r\n";

const char* html_main_cssB PROGMEM = "<STYLE>";
const char* html_main_css1 PROGMEM = ".clear { clear: both; } .a { background-color: #111111; } .t { padding: 10px 10px 20px 10px; border: 5px solid #551111;";
const char* html_main_css2 PROGMEM = " margin: 25px 25px 0px 25px; color: #999999; background-color: #111111; min-width: 30em; } input { font-weight: bold; width:4em; background-color: #A01010; padding: 2px 2px; }";
const char* html_main_css3 PROGMEM = ".b { padding: 10px; border-left: 5px solid #551111; border-right: 5px solid #551111; border-bottom: 5px solid #551111; margin: 0px 25px 25px 25px; color: #999999;";
const char* html_main_css4 PROGMEM = "background-color: #111111; min-width: 30em; } select { width:4em; font-weight: bold; background-color: #A01010; padding: 2px 2px; } .c { color: #A01010; font-weight: bold; }";
const char* html_main_css5 PROGMEM = "h1 { text-align: right; } a:hover, a:active { background-color: red; } .y { color: #FFFF00; font-weight: bold; }";
const char* html_main_css6 PROGMEM = "a:link, a:visited { background-color: #332222; color: #a07070; border:1px solid red; padding: 5px 10px;";
const char* html_main_css7 PROGMEM = " margin: none; text-align: center; text-decoration: none; display: inline-block; }";
const char* html_main_css8 PROGMEM = "button { background-color: #A01010; font-weight: bold; border-radius: 5px; margin: 2px; padding: 4px 8px; }";
const char* html_main_css_control1 PROGMEM = ".b1 { float: left; border: 2px solid #551111; background-color: #181818; text-align: center; margin: 5px; padding: 15px; padding-top: 3px; }";
const char* html_main_css_control2 PROGMEM = ".gb {  font-weight: bold; font-size: 150%; font-family: 'Times New Roman', Times, serif; width: 60px; height: 50px; padding: 0px; }";
const char* html_main_css_control3 PROGMEM = ".bb {  font-weight: bold; font-size: 105%; } .bbh {  font-weight: bold; font-size: 100%; height: 2.1em; }";
const char* html_main_cssE PROGMEM = "</STYLE>";

const char html_onstep_header1[] = "<div class='t'><table width='100%%'><tr><td><b><font size='5'>";
const char html_onstep_header2[] = "</font></b></td><td align='right'><b>" Product " " Version " (OnStep ";
const char html_onstep_header3[] = ")</b></td></tr></table>";
const char html_onstep_header4[] = "</div><div class='b'>\r\n";

const char* html_links1S = "<a href='/index.htm' style='background-color: #552222;'>Status</a>";
const char* html_links1N = "<a href='/index.htm'>Status</a>";
const char* html_links2S = "<a href='/control.htm' style='background-color: #552222;'>Control</a>";
const char* html_links2N = "<a href='/control.htm'>Control</a>";
const char* html_links3S = "<a href='/pec.htm' style='background-color: #552222;'>PEC</a>";
const char* html_links3N = "<a href='/pec.htm'>PEC</a>";
const char* html_linksEncS = "<a href='/enc.htm' style='background-color: #552222;'>Encoders</a>";
const char* html_linksEncN = "<a href='/enc.htm'>Encoders</a>";
const char* html_links4S = "<a href='/settings.htm' style='background-color: #552222;'>Settings</a>";
const char* html_links4N = "<a href='/settings.htm'>Settings</a>";
const char* html_links5S = "<a href='/configuration.htm' style='background-color: #552222;'>Configuration</a>";
const char* html_links5N = "<a href='/configuration.htm'>Configuration</a>";
const char* html_links6S = "<a href='/wifi.htm' style='background-color: #552222;'>WiFi</a><br />";
const char* html_links6N = "<a href='/wifi.htm'>WiFi</a><br />";

// Javascript for Ajax
// be sure to define "var ajaxPage='control.txt';" etc.
const char html_ajax_active[] =
"<script>\n"
"var auto1Tick=0;\n"
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
        "v=lines[i].slice(lines[i].indexOf('|')+1);\n"
        "k=lines[i].slice(0,lines[i].indexOf('|'));\n"
        "if (k!='') document.getElementById(k).innerHTML=v;\n"
      "}\n"
    "}"
  "}"
"}\n"
"</script>\n";
