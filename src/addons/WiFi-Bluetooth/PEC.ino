// The pec.htm page

const char html_pec1[] = "<div class=\"t\"><table width=\"100%%\"><tr><td><b><font size=\"5\">%s</font></b></td><td align=\"right\"><b>" Product " " Version " (OnStep %s)</b>";
const char html_pec2[] = "</td></tr></table>";
const char html_pec3[] = "</div><div class=\"b\">\r\n";
const char html_pec4[] = "PEC: <br /><br />";
const char html_pec5[] = "STATUS = <font class=\"c\">%s%s</font><br /><br />";
const char html_pecControls1[] =
"<form method=\"get\" action=\"/pec.htm\">"
"<button name=\"pe\" value=\"pl\" type=\"submit\">Play</button>";
const char html_pecControls2[] = 
"<button name=\"pe\" value=\"st\" type=\"submit\">Stop</button><br /><br />"
"<button name=\"pe\" value=\"cl\" type=\"submit\">Clear</button><button name=\"pe\" value=\"re\" type=\"submit\">Record</button><br />";
const char html_pecControls3[] = 
"Clear erases the memory buffer not EEPROM.  During recording corrections are averaged 3:1 favoring the buffer unless the buffer was cleared in which case the full correction is used.<br />";
const char html_pecControls4[] = 
"<br /><button name=\"pe\" value=\"wr\" type=\"submit\">Write to EEPROM</button><br />Writes PEC data to EEPROM so it's remembered if OnStep is restarted.  Writing the data can take a few seconds.<br /><br />";
const char html_pecControls4a[] = 
"<br /><br />Writing to EEPROM disabled.<br /><br />";
const char html_pecControls5[] = 
"</form>\r\n";

void handlePec() {
  Serial.setTimeout(WebTimeout);
  serialRecvFlush();
      
  char temp[320] = "";
  char temp1[80] = "";
  char temp2[80] = "";
  char temp3[80] = "";

  processPecGet();

  // send a standard http response header
  String data=html_head1;
  data += html_headerPec; // page refresh
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

  // finish the standard http response header
  Serial.print(":GVP#");
  temp2[Serial.readBytesUntil('#',temp2,20)]=0; 
  if (strlen(temp2)<=0) { strcpy(temp2,"N/A"); } else { for (int i=2; i<7; i++) temp2[i]=temp2[i+1]; }
  Serial.print(":GVN#");
  temp3[Serial.readBytesUntil('#',temp3,20)]=0; 
  if (strlen(temp3)<=0) { strcpy(temp3,"N/A"); }
  sprintf(temp,html_pec1,temp2,temp3);
  data += temp;
  data += html_pec2;
  data += html_links1pe;
  data += html_links2pe;
  data += html_links3pe;
  data += html_pec3;
  data += html_pec4;

  char stat[20] = "";
  Serial.print(":GU#");
  stat[Serial.readBytesUntil('#',stat,20)]=0;

  if (!strstr(stat,"A")) {  // not AltAzm
    strcpy(temp1,"");
    Serial.print(":$QZ?#");
    temp1[Serial.readBytesUntil('#',temp1,20)]=0;

    if (strstr(stat,"W")) { strcpy(temp3," (writing to EEPROM)"); } else { strcpy(temp3,""); }
    if (temp1[0]=='I') { strcpy(temp2,"Idle"); } else
    if (temp1[0]=='p') { strcpy(temp2,"Play waiting to start"); } else
    if (temp1[0]=='P') { strcpy(temp2,"Playing"); } else
    if (temp1[0]=='r') { strcpy(temp2,"Record waiting for the index to arrive"); } else
    if (temp1[0]=='R') { strcpy(temp2,"Recording"); } else strcpy(temp2,"Unknown");
    sprintf(temp,html_pec5,temp2,temp3);
    data += temp;

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

  server.send(200, "text/html",data);
}

void processPecGet() {
  String v;

  // PEC control
  v=server.arg("pe");
  if (v!="") {
    if (v=="pl") { // play
      Serial.print(":$QZ+#");
    }
    if (v=="st") { // stop
      Serial.print(":$QZ-#");
    }
    if (v=="re") { // record
      Serial.print(":$QZ/#");
    }
    if (v=="cl") { // clear
      Serial.print(":$QZZ#");
    };
    if (v=="wr") { // write to eeprom
      Serial.print(":$QZ!#");
    }
  }
}


