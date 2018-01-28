// The guide.htm page

const char html_guide1[] = "<div class=\"t\"><table width=\"100%%\"><tr><td><b><font size=\"5\">%s</font></b></td><td align=\"right\"><b>" Product " " Version " (OnStep %s)</b>";
const char html_guide2[] = "</td></tr></table>";
const char html_guide3[] = "</div><div class=\"b\">\r\n";
const char html_guide4[] = 
"<script>function guide(dir,start) { var xhttp = new XMLHttpRequest(); "
"xhttp.open('GET', 'guide.txt?dr='+dir+start+'&x='+new Date().getTime(), true);"
"xhttp.send(); }</script>"
"Guide Rate:<br />";
const char html_guideControls1[] =
"<form method=\"get\" action=\"/guide.htm\">"
"<button name=\"gu\" value=\"0\" type=\"submit\">0.25x</button>"
"<button name=\"gu\" value=\"1\" type=\"submit\">0.5x</button>";
const char html_guideControls2[] = 
"<button name=\"gu\" value=\"2\" type=\"submit\">1x</button>"
"<button name=\"gu\" value=\"4\" type=\"submit\">Mid</button>";
const char html_guideControls3[] = 
"<button name=\"gu\" value=\"6\" type=\"submit\">Fast</button>"
"<button name=\"gu\" value=\"8\" type=\"submit\">VFast</button></form><br />Guide:<br />";
const char html_guideControls4[] =
"<div style=\"float: left; border: 2px solid #551111; background-color: #181818; text-align: center; padding: 15px; margin: 5px;\">"
"<button type=\"button\" style=\"width: 60px; height: 50px;\" onmousedown=\"guide('n','1')\" onmouseup=\"guide('n','0')\">North</button><br />";
const char html_guideControls5[] = 
"<button type=\"button\" style=\"width: 60px; height: 50px;\" onmousedown=\"guide('e','1')\" onmouseup=\"guide('e','0')\">East</button>";
const char html_guideControls6[] = 
"<button type=\"button\" style=\"width: 60px; height: 50px;\" onclick=\"guide('s','y')\">Sync</button>"
"<button type=\"button\" style=\"width: 60px; height: 50px;\" onmousedown=\"guide('w','1')\" onmouseup=\"guide('w','0')\">West</button><br />";
const char html_guideControls7[] = 
"<button type=\"button\" style=\"width: 60px; height: 50px;\" onmousedown=\"guide('s','1')\" onmouseup=\"guide('s','0')\">South</button>"
"</div><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br /><br />";
const char html_guideControls8[] = 
"<form method=\"get\" action=\"/guide.htm\">"
">&nbsp;<button name=\"gu\" value=\"q\" type=\"submit\" style=\"height: 40px;\">Stop Slew/Guide!</button>&nbsp;<";
const char html_guideControls9[] = 
"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<button name=\"mp\" value=\"co\" type=\"submit\" style=\"height: 40px;\">-> Continue Goto</button>";
const char html_guideControls10[] = 
"<br /><br /></form>\r\n";

const char html_guideRotate1[] = 
"&nbsp;<button type=\"button\" onmousedown=\"guide('r','e')\" style=\"height: 40px;\" >Reset</button>"
"&nbsp;<button type=\"button\" onmousedown=\"guide('h','o')\" style=\"height: 40px;\" >Home</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
const char html_guideRotate2[] = 
"&nbsp;<button type=\"button\" onmousedown=\"guide('b','2')\" style=\"height: 40px;\" >&lt;&lt;</button>"
"&nbsp;<button type=\"button\" onmousedown=\"guide('b','1')\" style=\"height: 40px;\" >&lt;</button>";
const char html_guideRotate3[] =
"&nbsp;<button type=\"button\" onmousedown=\"guide('f','1')\" style=\"height: 40px;\" >&gt;</button>"
"&nbsp;<button type=\"button\" onmousedown=\"guide('f','2')\" style=\"height: 40px;\" >&gt;&gt;</button><br />";
const char html_guideDeRotate1[] = 
"&nbsp;<button type=\"button\" onmousedown=\"guide('d','1')\" style=\"height: 40px;\" >De-Rotate On</button>&nbsp;&nbsp;&nbsp;"
"&nbsp;<button type=\"button\" onmousedown=\"guide('d','r')\" style=\"height: 40px;\" >Rev</button>";
const char html_guideDeRotate2[] = 
"&nbsp;<button type=\"button\" onmousedown=\"guide('d','p')\" style=\"height: 40px;\" >P</button>&nbsp;&nbsp;&nbsp;"
"&nbsp;<button type=\"button\" onmousedown=\"guide('d','0')\" style=\"height: 40px;\" >De-Rotate Off</button><br />";

const char html_guideControls11[] = 
"<br />\r\n";


void handleGuide(EthernetClient *client) {
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
    
  char temp[320] = "";
  char temp2[80]="";
  char temp3[80]="";

  processGuideGet();
  
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
  client->print(data); data="";

  // get status
  char stat[20] = "";
  Ser.print(":GU#");
  stat[Ser.readBytesUntil('#',stat,20)]=0;
  
  // finish the standard http response header
  Ser.print(":GVP#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; }
  Ser.print(":GVN#");
  temp3[Ser.readBytesUntil('#',temp3,20)]=0;
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_guide1,temp2,temp3);
  data += temp;
  data += html_guide2;
  data += html_links1gu;
  data += html_links2gu;
  data += html_links3gu;

  data += html_guide3;
  data += html_guide4;
  client->print(data); data="";

  data += html_guideControls1;
  data += html_guideControls2;
  data += html_guideControls3;
  data += html_guideControls4;
  data += html_guideControls5;
  client->print(data); data="";
  data += html_guideControls6;
  data += html_guideControls7;
  data += html_guideControls8;
  if (strstr(stat,"E")) {  // GEM only
    data += html_guideControls9;
  }
  data += html_guideControls10;

  // Rotate/De-Rotate controls
  boolean Rotate=false;
  boolean DeRotate=false;
  Ser.print(":GX98#");
  temp2[Ser.readBytesUntil('#',temp2,20)]=0; 
  if (temp2[0]=='R') { Rotate=true; DeRotate=false; }
  if (temp2[0]=='D') { Rotate=true; DeRotate=true; }
  if (Rotate) {
    data += "<br />Rotator:&nbsp;&nbsp;&nbsp;(Angle = ";
    Ser.print(":rG#");
    temp2[Ser.readBytesUntil('#',temp2,20)]=0;
    temp2[11]='\''; temp2[10]=temp2[6]; temp2[9]=temp2[5]; temp2[8]=';'; temp2[7]='g'; temp2[6]='e'; temp2[5]='d'; temp2[4]='&';
    data += temp2;
    data += " &larr; refresh page to update)<br />";
    data += html_guideRotate1;
    data += html_guideRotate2;
    data += html_guideRotate3;
  }
  if (DeRotate) {
    data += html_guideDeRotate1;
    data += html_guideDeRotate2;
  }

  data += html_guideControls11;

  data += "</div></body></html>";

  client->print(data);
}

void handleGuideAjax(EthernetClient *client) {
  processGuideGet();
  client->print("");
}

void processGuideGet() {
  String v;

  // GUIDE control -----------------------------------------------
  v=server.arg("gu");
  if (v!="") {
    if (v=="0") Ser.print(":R0#");
    if (v=="1") Ser.print(":R1#");
    if (v=="2") Ser.print(":R2#");
    if (v=="3") Ser.print(":R3#");
    if (v=="4") Ser.print(":R4#");
    if (v=="5") Ser.print(":R5#");
    if (v=="6") Ser.print(":R6#");
    if (v=="7") Ser.print(":R7#");
    if (v=="8") Ser.print(":R8#");
    if (v=="9") Ser.print(":R9#");
    if (v=="q") Ser.print(":Q#");
  }
  v=server.arg("dr");
  if (v!="") {
    if (v=="n1") Ser.print(":Mn#");
    if (v=="s1") Ser.print(":Ms#");
    if (v=="e1") Ser.print(":Me#");
    if (v=="w1") Ser.print(":Mw#");

    if (v=="n0") Ser.print(":Qn#");
    if (v=="s0") Ser.print(":Qs#");
    if (v=="e0") Ser.print(":Qe#");
    if (v=="w0") Ser.print(":Qw#");

    if (v=="sy") Ser.print(":CS#");
  }
  // Pause at meridian flip, continue
  v=server.arg("mp");
  if (v!="") {
    if (v=="co") Ser.print(":SX99,1#");
  }
  // Rotate/De-Rotate
  v=server.arg("dr");
  if (v!="") {
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
}


