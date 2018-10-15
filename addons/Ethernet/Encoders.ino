// -----------------------------------------------------------------------------------
// Encoders

#ifdef ENCODERS_ON

const char html_encBeg[] = 
"";

const char html_encStart[] =
"";

const char html_syncOnStepNow[] =
"Sync OnStep to Encoders: <br />"
"<form method='get' action='/enc.htm'>"
"<button name='ms' value='ons' type='submit'>Now</button>"
"<br />\r\n";

const char html_syncEncodersNow[] =
"Sync Encoders to OnStep: <br />"
"<button name='ms' value='enc' type='submit'>Now</button>"
"</form><br />\r\n";

const char html_encEn1[] =
"Automatically sync. OnStep to Encoders (";
const char html_encEn2[] =
"):<br />"
"<form method='get' action='/enc.htm'>";
const char html_encEn3[] =
"<button name='as' value='on' type='submit'>On</button>"
"<button name='as' value='off' type='submit'>Off</button>"
"</form><br />\r\n";

const char html_encMxAxis1[] =
"Maximum allowable angular distance (between Encoders and OnStep): <br />"
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='d1' min='0' max='9999'>"
"<button type='submit'>Upload</button>"
" (Axis1, in arc-seconds 0 to 9999)"
"</form>"
"\r\n";
const char html_encMxAxis2[] = 
"<form method='get' action='/enc.htm'>"
" <input value='%ld' type='number' name='d2' min='0' max='9999'>"
"<button type='submit'>Upload</button>"
" (Axis2, in arc-seconds 0 to 9999)"
"</form>"
"<br />\r\n";

const char html_encEnd[] = 
"</form>"
"\r\n";
#ifdef OETHS
void handleEncoders(EthernetClient *client) {
#else
void handleEncoders() {
#endif
  Ser.setTimeout(WebTimeout);
  serialRecvFlush();
  
  char temp[320]="";
  char temp1[80]="";
  
  processEncodersGet();

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
  data += html_main_cssE;
  data += html_headE;
#ifdef OETHS
  client->print(data); data="";
#endif

  data += html_bodyB;

  // get status
  mountStatus.update();

  // finish the standard http response header
  data += html_onstep_header1;
  if (mountStatus.getId(temp1)) data += temp1; else data += "?";
  data += html_onstep_header2;
  if (mountStatus.getVer(temp1)) data += temp1; else data += "?";
  data += html_onstep_header3;
  data += html_links1N;
  data += html_links2N;
  data += html_links3N;
#ifdef ENCODERS_ON
  data += html_linksEncS;
#endif
  data += html_links4N;
  data += html_links5N;
#ifndef OETHS
  data += html_links6N;
#endif
  data += html_onstep_header4;
  data += html_encBeg;

  data+="<div style='width: 35em;'>";

  data += html_syncOnStepNow;
  data += html_syncEncodersNow;

  data += html_encEn1;
  if (encAutoSync) data+="On"; else data+="Off";
  data += html_encEn2;
  data += html_encEn3;
#ifdef OETHS
  client->print(data); data="";
#endif
  
  sprintf(temp,html_encMxAxis1,Axis1EncDiffLimit);
  data += temp;
  sprintf(temp,html_encMxAxis2,Axis2EncDiffLimit);
  data += temp;

  // end of page
  sprintf(temp,html_encEnd);
  data += temp;

  strcpy(temp,"</div></div></body></html>");
  data += temp;

#ifdef OETHS
  client->print(data); data="";
#else
  server.send(200, "text/html",data);
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

#ifndef EEPROM_COMMIT_DISABLED
  if (EEwrite) EEPROM.commit();
#endif

  // clear any possible response
  temp[Ser.readBytesUntil('#',temp,20)]=0;
}

#endif

