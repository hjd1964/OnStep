// -----------------------------------------------------------------------------------
// Controls for PEC

const char html_pec1[] = "<div class='b1' style='width: 27em'>";
const char html_pec2[] = "<div style='float: left'>PEC Status:</div><br/><div id='status'>?</div></div><br class='clear' />";

const char html_pecControls0[] = 
"<div class='b1' style='width: 27em'><div style='float: left'>PEC Control:</div><br />";
const char html_pecControls1[] =
"<form method='get' action='/pec.htm'>"
"<button name='pe' value='pl' type='submit'>Play</button>";
const char html_pecControls2[] = 
"<button name='pe' value='st' type='submit'>Stop</button><br /><br />"
"<button name='pe' value='cl' type='submit'>Clear</button><button name='pe' value='re' type='submit'>Record</button><br />";
const char html_pecControls3[] = 
"Clear erases the memory buffer not EEPROM.  During recording corrections are averaged 3:1 favoring the buffer unless cleared in which case the full correction is used.<br />";
const char html_pecControls4[] = 
"<br /><button name='pe' value='wr' type='submit'>Write to EEPROM</button><br />Writes PEC data to EEPROM so it's remembered if OnStep is restarted.  Writing the data can take a few seconds.<br /><br />";
const char html_pecControls4a[] = 
"<br /><br />Writing to EEPROM disabled.<br /><br />";
const char html_pecControls5[] = 
"</form></div><br class='clear' /><br />\r\n";

#ifdef OETHS
void handlePec(EthernetClient *client) {
#else
void handlePec() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
      
  char temp2[80] = "";

  processPecGet();

  // send a standard http response header
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

  char stat[20] = "";
  Ser.print(":GU#");
  stat[Ser.readBytesUntil('#',stat,20)]=0;

  if (!strstr(stat,"A")) {  // not AltAzm
    // active ajax page is: pecAjax();
    data +="<script>var ajaxPage='pec.txt';</script>\n";
    data +=html_ajax_active;
    data +="<script>auto2Rate=2;</script>";
  }

  // finish the standard http response header
  data += html_onstep_header1;
  Ser.print(":GVP#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; } data += temp2;
  data += html_onstep_header2;
  Ser.print(":GVN#"); temp2[Ser.readBytesUntil('#',temp2,20)]=0; if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } data += temp2;
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3S;
  data += html_links4N;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  data += html_pec1;
#ifdef OETHS
  client->print(data); data="";
#endif

  if (!strstr(stat,"A")) {  // not AltAzm
    data += html_pec2;
    data += html_pecControls0;
    data += html_pecControls1;
    data += html_pecControls2;
    data += html_pecControls3;

    // disable recording if pecAutoRecord is active
    if (!strstr(stat,"W")) data += html_pecControls4;

    data += html_pecControls5;
  } else {
    data += "PEC CONTROLS DISABLED";
  }

  data += "</div></body></html>";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/html",data);
#endif
}

#ifdef OETHS
void pecAjax(EthernetClient *client) {
#else
void pecAjax() {
#endif
  String data="";
  char temp[80]="";
  
  data += "status|";

  char stat[20] = "";
  Ser.print(":GU#");
  stat[Ser.readBytesUntil('#',stat,20)]=0;

  if (!strstr(stat,"A")) {  // not AltAzm
    Ser.print(":$QZ?#");
    temp[Ser.readBytesUntil('#',temp,20)]=0;
  
    if (temp[0]=='I') data +="Idle"; else
    if (temp[0]=='p') data +="Play waiting to start"; else
    if (temp[0]=='P') data +="Playing"; else
    if (temp[0]=='r') data +="Record waiting for index to arrive"; else
    if (temp[0]=='R') data +="Recording"; else data += "Unknown";
    if (strstr(stat,"W")) data += " (writing to EEPROM)";
  } else { data += "?"; }
  data += "\n";

#ifdef OETHS
  client->print(data);
#else
  server.send(200, "text/plain",data);
#endif
}

void processPecGet() {
  String v;

  // PEC control
  v=server.arg("pe");
  if (v!="") {
    if (v=="pl") { // play
      Ser.print(":$QZ+#");
    }
    if (v=="st") { // stop
      Ser.print(":$QZ-#");
    }
    if (v=="re") { // record
      Ser.print(":$QZ/#");
    }
    if (v=="cl") { // clear
      Ser.print(":$QZZ#");
    };
    if (v=="wr") { // write to eeprom
      Ser.print(":$QZ!#");
    }
  }
}


