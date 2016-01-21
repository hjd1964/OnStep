#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__) || defined(W5100_ON)

int  www_xmit_buffer_send_pos=0;
int  www_xmit_buffer_pos=0;
char www_xmit_buffer[1024] = "";

// provide the same functions as for serial

// Enter a MAC address and IP address for your controller below. MAC address will be on the sticker on the Tiva Connected Launchpad.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168,1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


// Network server port for command channel
EthernetServer cmd_server(9999);
EthernetClient cmd_client;
// Network server port for web channel
EthernetServer web_server(80);
EthernetClient www_client;
boolean alreadyConnected = false; // whether or not the client was connected previously


void Ethernet_Init() {
  // initialize the ethernet device
#if defined(ETHERNET_USE_DHCP_ON)
  Ethernet.begin(mac);
#else
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
#endif

  cmd_server.begin();
  web_server.begin();
  www_client = web_server.available(); // initialise www_client
}

void Ethernet_send(const char data[]) {
  if (!cmd_client)
   return;
   
  cmd_client.flush();
   
  cmd_client.print(data);
}

void Ethernet_print(const char data[]) {
  if (!cmd_client)
   return;
   
   cmd_server.print(data);
 }
 
 boolean Ethernet_transmit() {
   return false;
 }
 
 boolean Ethernet_available() {
   cmd_client = cmd_server.available();
   return cmd_client;
 }
 
 char Ethernet_read() {
   if (!cmd_client)
   return -1;
   
   return cmd_client.read();
 }

// -----------------------------------------------------------------------------------------
// web server

boolean currentLineIsBlank;
boolean responseStarted = false;
boolean clientNeedsToClose = false;
unsigned long responseFinish_ms;
unsigned long transactionStart_ms;
int html_page_step=0;

bool get_check=false;
bool get_name=false;
bool get_val=false;
int get_idx=0;
char get_names[11] = "";
char get_vals[11] = "";

// variables to support web-page request detection
const char index_page[] = "GET /index.htm"; bool index_page_found; byte index_page_count;
const char settings_page[] = "GET /settings.htm"; bool settings_page_found; byte settings_page_count;
const char control_page[] = "GET /control.htm"; bool control_page_found; byte control_page_count;
const char pec_page[] = "GET /pec.htm"; bool pec_page_found; byte pec_page_count;
const char config_page[] = "GET /config.htm"; bool config_page_found; byte config_page_count;
void reset_page_requests() {
  index_page_found=false; index_page_count=0;
  settings_page_found=false; settings_page_count=0;
  control_page_found=false; control_page_count=0;
  pec_page_found=false; pec_page_count=0;
  config_page_found=false; config_page_count=0;
  get_check=false; get_val=false; get_name=false;
}

void Ethernet_www() {
  // if a client doesn't already exist try to find a new one
  if (!www_client) { www_client = web_server.available(); currentLineIsBlank = true; responseStarted=false; clientNeedsToClose=false; reset_page_requests(); transactionStart_ms=millis(); }

  // active client?
  if (www_client) {
    if (www_client.connected()) {
      if (www_client.available()) {
        char c = www_client.read();

        //Serial_char(c);

        // record the get request name and value, then process the request
        if ((get_val) && ((c==' ') || (c=='&'))) { get_val=false; get_idx=0; Ethernet_get(); if (c=='&') { c='?'; get_check=true; } }
        if (get_val) { if (get_idx<10) { get_vals[get_idx]=c; get_vals[get_idx+1]=0; get_idx++; } }
        if ((get_name) && (c=='=')) { get_name=false; get_val=true; get_idx=0; }
        if (get_name) { if (get_idx<10) { get_names[get_idx]=c; get_names[get_idx+1]=0; get_idx++; } }
        if ((get_check) && (c=='?')) { get_name=true; get_idx=0; } get_check=false;
        
        // watch for page requests and flag them
        if (!index_page_found)    { if (c==index_page[index_page_count])       index_page_count++;    else index_page_count=0;    if (index_page_count==14)    { index_page_found=true; get_check=true; } }
        if (!settings_page_found) { if (c==settings_page[settings_page_count]) settings_page_count++; else settings_page_count=0; if (settings_page_count==17) { settings_page_found=true; get_check=true; } }
        if (!control_page_found)  { if (c==control_page[control_page_count])   control_page_count++;  else control_page_count=0;  if (control_page_count==16)  { control_page_found=true; get_check=true; } }
        if (!pec_page_found)      { if (c==pec_page[pec_page_count])           pec_page_count++;      else pec_page_count=0;      if (pec_page_count==12)      { pec_page_found=true; get_check=true; } }
        if (!config_page_found)   { if (c==config_page[config_page_count])     config_page_count++;   else config_page_count=0;   if (config_page_count==15)   { config_page_found=true; get_check=true; } }
        
        // if you've gotten to the end of the line (received a newline character) and the line is blank, the http request has ended, so you can send a reply
        if ((c == '\n') && currentLineIsBlank) { 
          responseStarted=true;
          html_page_step=0;
        } else {
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // any data ready to send gets processed here
      if (responseStarted) {
        if (index_page_found) index_html_page(); else
        if (settings_page_found) settings_html_page(); else
        if (control_page_found) control_html_page(); else
        if (pec_page_found) pec_html_page(); else
        if (config_page_found) config_html_page(); else
        index_html_page();
      }

      if (www_xmit_buffer_pos>0) {
        if (!www_send()) {
          clientNeedsToClose=true;
          responseFinish_ms=millis();
        } else {
          clientNeedsToClose=false;
        }
      }

    }
    // if data was sent give the web browser time to receive it then stop the client
    // if a transaction is taking more than five seconds, stop the client
    if ((clientNeedsToClose && (millis()-responseFinish_ms>100)) || (millis()-transactionStart_ms>5000)) {
      clientNeedsToClose=false;
      www_client.stop();
    }
    Ethernet.maintain();
  }
}

// quickly copies string to www server transmit buffer
// returns true if successful
// returns false if buffer is too full to accept the data (and copies no data into buffer)
boolean www_write(const char data[]) {
  int l=strlen(data);
  if (www_xmit_buffer_pos+l>1022) return false;
  strcpy((char *)&www_xmit_buffer[www_xmit_buffer_pos],data);
  www_xmit_buffer_pos+=l;
  return true;
}

// quickly writes up to 5 chars at a time from buffer to ethernet adapter
// returns true if data is still waiting for transmit
// returns false if data buffer is empty
boolean www_send() {
  char buf[11] = "";
  char c;
  
  // copy some data
  boolean buffer_empty=false;
  for (int l=0; l<5; l++) {
    c=www_xmit_buffer[www_xmit_buffer_send_pos];
    buf[l+1]=0;
    buf[l]=c;
    if ((c==0) || (www_xmit_buffer_send_pos>1022)) { buffer_empty=true; break; }
    www_xmit_buffer_send_pos++;
  }

  // send network data
  www_client.print(buf);

  // hit end of www_xmit_buffer? reset and start over
  if (buffer_empty) {
    www_xmit_buffer_pos=0;
    www_xmit_buffer_send_pos=0;
    www_xmit_buffer[0]=0;
    return false;
  }

  return true;
}

// The index.htm page --------------------------------------------------------------------------------------
// process GET requests 
// currently only the name and value are handled, the page sending the name isn't considered
// this lowers the processing overhead and is sufficient for our purposes here
double get_temp_float;
int get_temp_day,get_temp_month,get_temp_year;
int get_temp_hour,get_temp_minute,get_temp_second;
void Ethernet_get() {
  if (get_names[2]!=0) return; // only two char names for now

  // from the Settings.htm page -------------------------------------------------------------------
  // Slew Speed
  if ((get_names[0]=='s') && (get_names[1]=='s')) {
    if ((get_vals[0]=='v') && (get_vals[1]=='s') && (get_vals[2]==0)) maxRate=MaxRate*32L;
    if ((get_vals[0]=='s') && (get_vals[1]==0)) maxRate=MaxRate*24L;
    if ((get_vals[0]=='n') && (get_vals[1]==0)) maxRate=MaxRate*16L;
    if ((get_vals[0]=='f') && (get_vals[1]==0)) maxRate=MaxRate*11L;
    if ((get_vals[0]=='v') && (get_vals[1]=='f') && (get_vals[2]==0)) maxRate=MaxRate*8L;
    
    if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
    if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
    EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L));
  }
  // Overhead and Horizon Limits
  if ((get_names[0]=='o') && (get_names[1]=='l')) {
    if ( (atoi2(get_vals,&i)) && ((i>=60) && (i<=90))) { maxAlt=i; EEPROM.update(EE_maxAlt,maxAlt); }
  }
  if ((get_names[0]=='h') && (get_names[1]=='l')) {
    if ( (atoi2(get_vals,&i)) && ((i>=-30) && (i<=30))) { minAlt=i; EEPROM.update(EE_minAlt,minAlt+128); }
  }
  // Backlash Limits
  if (get_names[0]=='b') {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=999))) { 
      if (get_names[1]=='2') {
        backlashDec=(int)(((long)i*StepsPerDegreeDec)/3600L);
        EEPROM_writeInt(EE_backlashDec,backlashDec);
      } else
      if (get_names[1]=='1') {
        backlashHA =(int)(((long)i*StepsPerDegreeHA)/3600L);
        EEPROM_writeInt(EE_backlashHA,backlashHA);
      }
    }
  }
  // Location
  if ((get_names[0]=='g') && (get_names[1]=='1')) {
    if ( (atoi2(get_vals,&i)) && ((i>=-180) && (i<=180))) { get_temp_float=i; }
  }
  if ((get_names[0]=='g') && (get_names[1]=='2')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=60))) { if (get_temp_float<0.0) i=-i; longitude=get_temp_float+((double)i)/60.0; float f=longitude; EEPROM_writeQuad(EE_sites+(currentSite)*25+4,(byte*)&f); LST=jd2last(JD,UT1); update_lst(); }
  }
  if ((get_names[0]=='t') && (get_names[1]=='1')) {
    if ( (atoi2(get_vals,&i)) && ((i>=-180) && (i<=180))) { get_temp_float=i; }
  }
  if ((get_names[0]=='t') && (get_names[1]=='2')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=60))) {
      if (get_temp_float<0.0) i=-i;
      latitude=get_temp_float+((double)i)/60.0; float f=latitude;
      EEPROM_writeQuad(100+(currentSite)*25+0,(byte*)&f);
#ifdef MOUNT_TYPE_ALTAZM
      celestialPoleDec=fabs(latitude);
      if (latitude<0) celestialPoleHA=180L; else celestialPoleHA=0L;
#else
      if (latitude<0) celestialPoleDec=-90.0; else celestialPoleDec=90.0;
#endif
      cosLat=cos(latitude/Rad);
      sinLat=sin(latitude/Rad);
      if (latitude>0.0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
    }
  }
  if ((get_names[0]=='u') && (get_names[1]=='1')) {
    if ( (atoi2(get_vals,&i)) && ((i>=-13) && (i<=13))) { get_temp_float=i; }
  }
  if ((get_names[0]=='u') && (get_names[1]=='2')) {
    if ( (atoi2(get_vals,&i)) && ((i==00) || (i==30) || (i==45))) {
      if (get_temp_float<0.0) i=-i;
      timeZone=get_temp_float+((double)i)/60.0;
      b=encodeTimeZone(timeZone)+128;
      EEPROM.update(EE_sites+(currentSite)*25+8,b);
      UT1=LMT+timeZone;
      UT1_start  =UT1;
      UT1mS_start=millis();
      LST=jd2last(JD,UT1);
      update_lst();
    }
  }
  // from the Control.htm page -------------------------------------------------------------------
  // Date/Time
  if ((get_names[0]=='d') && (get_names[1]=='m')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=11))) { get_temp_month=i+1; }
  }
  if ((get_names[0]=='d') && (get_names[1]=='d')) {
    if ( (atoi2(get_vals,&i)) && ((i>=1) && (i<=31))) { get_temp_day=i; }
  }
  if ((get_names[0]=='d') && (get_names[1]=='y')) {
    if ( (atoi2(get_vals,&i)) && ((i>=2016) && (i<=9999))) {
      get_temp_year=i-2000;
      char temp[10];
      sprintf(temp,"%02d/%02d/%02d",get_temp_month,get_temp_day,get_temp_year);
      if (dateToDouble(&JD,temp)) { 
        float f=JD; EEPROM_writeQuad(EE_JD,(byte*)&f); 
        LST=jd2last(JD,UT1); 
        update_lst();
      }
    }
  }
  if ((get_names[0]=='t') && (get_names[1]=='h')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=23))) { get_temp_hour=i; }
  }
  if ((get_names[0]=='t') && (get_names[1]=='m')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=59))) { get_temp_minute=i; }
  }
  if ((get_names[0]=='t') && (get_names[1]=='s')) {
    if ( (atoi2(get_vals,&i)) && ((i>=0) && (i<=59))) {
      get_temp_second=i;
      char temp[10];
      sprintf(temp,"%02d:%02d:%02d",get_temp_hour,get_temp_minute,get_temp_second);
      i=highPrecision; highPrecision=true; 
      if (hmsToDouble(&LMT,temp)) {
        float f=LMT; EEPROM_writeQuad(EE_LMT,(byte*)&f); 
        UT1=LMT+timeZone; 
        UT1_start  =UT1;
        UT1mS_start=millis(); 
        LST=jd2last(JD,UT1); 
        update_lst(); 
      }
      highPrecision=i;
    }
  }
  // Align
  if ((get_names[0]=='a') && (get_names[1]=='l')) {
    if ((get_vals[0]=='1') && (get_vals[1]==0)) startAlign(get_vals[0]);
    if ((get_vals[0]=='2') && (get_vals[1]==0)) startAlign(get_vals[0]);
    if ((get_vals[0]=='3') && (get_vals[1]==0)) startAlign(get_vals[0]);
    if ((get_vals[0]=='n') && (get_vals[1]==0)) nextAlign();
  }
  // Home/Park
  if ((get_names[0]=='h') && (get_names[1]=='m')) {
    if ((get_vals[0]=='r') && (get_vals[1]==0)) setHome();
    if ((get_vals[0]=='f') && (get_vals[1]==0)) goHome();
  }
  if ((get_names[0]=='p') && (get_names[1]=='k')) {
    if ((get_vals[0]=='p') && (get_vals[1]==0)) park();
    if ((get_vals[0]=='u') && (get_vals[1]==0)) unpark();
    if ((get_vals[0]=='s') && (get_vals[1]==0)) setPark();
  }
  // Tracking control
  if ((get_names[0]=='t') && (get_names[1]=='k')) {
    if ((get_vals[0]=='o') && (get_vals[1]=='n') && (get_vals[2]==0)) trackingState=TrackingSidereal;
    if ((get_vals[0]=='o') && (get_vals[1]=='f') && (get_vals[2]=='f') && (get_vals[3]==0)) trackingState=TrackingNone;
    if (((get_vals[0]=='f') || (get_vals[0]=='-') || (get_vals[0]=='r')) && (get_vals[1]==0)) {
       if (get_vals[0]=='f') siderealInterval+=HzCf*(0.1);
       if (get_vals[0]=='-') siderealInterval-=HzCf*(0.1);
       if (get_vals[0]=='r') siderealInterval=15956313L;
       EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);
       SetSiderealClockRate(siderealInterval);
       cli(); SiderealRate=siderealInterval/StepsPerSecond; sei();
    }
    if ((get_vals[0]=='s') && (get_vals[1]==0)) SetTrackingRate(default_tracking_rate); // sidereal
    if ((get_vals[0]=='l') && (get_vals[1]==0)) SetTrackingRate(0.96236513150); // lunar
    if ((get_vals[0]=='h') && (get_vals[1]==0)) SetTrackingRate(0.99726956632); // solar
  }
  // PEC control
  if ((get_names[0]=='p') && (get_names[1]=='e')) {
    if ((get_vals[0]=='p') && (get_vals[1]=='l') && (get_vals[2]==0)) { if (PECrecorded) PECstatus=ReadyPlayPEC; } // play
    if ((get_vals[0]=='s') && (get_vals[1]=='t') && (get_vals[2]==0)) { PECstatus=IgnorePEC; }; // stop
    if ((get_vals[0]=='r') && (get_vals[1]=='e') && (get_vals[2]==0)) { PECstatus=ReadyRecordPEC; }; // record
    if ((get_vals[0]=='c') && (get_vals[1]=='l') && (get_vals[2]==0)) { // clear
      for (i=0; i<PECBufferSize; i++) PEC_buffer[i]=128;
      PECfirstRecord = true;
      PECstatus      = IgnorePEC;
      PECrecorded    = false;
      EEPROM.update(EE_PECstatus,PECstatus);
      EEPROM.update(EE_PECrecorded,PECrecorded);
      PECindex_record= 0;
      PECindex_sense = 0;
      EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
      EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
    };
    if ((get_vals[0]=='w') && (get_vals[1]=='r') && (get_vals[2]==0)) { // write to eeprom
      PECrecorded=true;
      PECstatus=IgnorePEC;
      EEPROM.update(EE_PECrecorded,PECrecorded);
      EEPROM.update(EE_PECstatus,PECstatus);
      EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
      // trigger recording of PEC buffer
      PECautoRecord=PECBufferSize;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------
// Web-site pages, the design of each xxxxx_html_page() function is now capable of sending a web-page of any size
// since it stops and waits should the local outgoing buffer become full
const char html_header1[] PROGMEM = "HTTP/1.1 200 OK\r\n";
const char html_header2[] PROGMEM = "Content-Type: text/html\r\n";
const char html_header3[] PROGMEM = "Connection: close\r\n";
const char html_header4[] PROGMEM = "Refresh: 5\r\n\r\n";
const char html_header4a[] PROGMEM = "\r\n";
const char html_header5[] PROGMEM = "<!DOCTYPE HTML>\r\n<html>\r\n";
const char html_header6[] PROGMEM = "<head>\r\n";
const char html_header7[] PROGMEM = "</head>\r\n";
const char html_header8[] PROGMEM = "<body bgcolor=\"#26262A\">\r\n";

const char html_main_css1[] PROGMEM = "<STYLE>\r\n";
const char html_main_css2[] PROGMEM = ".a { background-color: #111111; }\r\n";
const char html_main_css3[] PROGMEM = ".t { padding: 15px; border: 15px solid #551111;\r\n";
const char html_main_css4[] PROGMEM = " margin: 25px; color: #999999; background-color: #111111; }\r\n";
const char html_main_css5[] PROGMEM = ".b { padding: 30px; border: 2px solid #551111;\r\n";
const char html_main_css6[] PROGMEM = " margin: 30px; color: #999999; background-color: #111111; }\r\n";
const char html_main_css7[] PROGMEM = "h1 { text-align: right; }\r\n";
const char html_main_css8[] PROGMEM = "input { width:4em }\r\n";
const char html_main_css9[] PROGMEM = "</STYLE>\r\n";

const char html_links1[] PROGMEM = "<a href=\"/index.htm\">Status</a>&nbsp;&nbsp;&nbsp;<a href=\"/settings.htm\">";
const char html_links2[] PROGMEM = "Settings</a>&nbsp;&nbsp;&nbsp;<a href=\"/control.htm\">Control</a>&nbsp;&nbsp;&nbsp;";
const char html_links3[] PROGMEM = "<a href=\"/pec.htm\">PEC</a>&nbsp;&nbsp;&nbsp;<a href=\"/config.htm\">Configuration</a><br />";

// The index.htm page --------------------------------------------------------------------------------------
#ifdef MOUNT_TYPE_ALTAZM
  #define Axis1 "Az"
  #define Axis1a "Az"
  #define Axis2 "Azm"
#else
  #define Axis1 "HA"
  #define Axis1a "RA"
  #define Axis2 "Dec"
#endif
const char html_index1[] PROGMEM = "<div class=\"t\"><table width=\"100%\"><tr><td><b>" FirmwareName " " FirmwareNumber;
const char html_index2[] PROGMEM = "</b></td><td align=\"right\"><b><font size=\"5\">";
const char html_index2a[] PROGMEM = "STATUS</font></b></td></tr></table><br />";
const char html_index2b[] PROGMEM = "</div><div class=\"b\">Longitude = %s, Latitude = %s<br />";
const char html_index3[] PROGMEM = "%02d/%02d/%02d";
const char html_index4[] PROGMEM = "&nbsp;%s&nbsp;UT";
const char html_index4a[] PROGMEM = "&nbsp;(%s&nbsp; Local Apparent Sidereal Time)<br /><br />";
const char html_index5[] PROGMEM = "Current Position: " Axis1 "=%s, " Axis2 "=%s<br />";
const char html_index6[] PROGMEM = "Target Position: " Axis1 "=%s, " Axis2 "=%s<br /><br />";
const char html_index7[] PROGMEM = "Current MaxRate: %ld (Default MaxRate: %ld)<br /><br />";
const char html_index8[] PROGMEM = "Tracking: %s %s<br />";
const char html_index9[] PROGMEM = "Parking: %s<br />";
const char html_index10[] PROGMEM = "PEC: %s<br />";

void index_html_page() {
  char temp[256] = "";
  char temp1[80] = "";
  char temp2[20] = "";
  char temp3[20] = "";
  bool r=true;
  int stp=0;
  html_page_step++;
    
  // send a standard http response header
  if (html_page_step==++stp) strcpy_P(temp, html_header1); 
  if (html_page_step==++stp) strcpy_P(temp, html_header2);
  if (html_page_step==++stp) strcpy_P(temp, html_header3);
  if (html_page_step==++stp) strcpy_P(temp, html_header4);
  if (html_page_step==++stp) strcpy_P(temp, html_header5);
  if (html_page_step==++stp) strcpy_P(temp, html_header6);

  if (html_page_step==++stp) strcpy_P(temp, html_main_css1);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css2);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css3);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css4);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css5);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css6);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css7);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css8);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css9);
  
  if (html_page_step==++stp) strcpy_P(temp, html_header7);
  if (html_page_step==++stp) strcpy_P(temp, html_header8);

  // and the remainder of the page
  if (html_page_step==++stp) strcpy_P(temp, html_index1);
  if (html_page_step==++stp) strcpy_P(temp, html_index2);
  if (html_page_step==++stp) strcpy_P(temp, html_index2a);
  if (html_page_step==++stp) strcpy_P(temp, html_links1);
  if (html_page_step==++stp) strcpy_P(temp, html_links2);
  if (html_page_step==++stp) strcpy_P(temp, html_links3);
  if (html_page_step==++stp) {
    i=highPrecision; 
    highPrecision=false; 
    doubleToDms(temp2,&longitude,true,true);
    doubleToDms(temp3,&latitude,false,true);
    highPrecision=i;
    strcpy_P(temp1, html_index2b); sprintf(temp,temp1,temp2,temp3);
  }
  if (html_page_step==++stp) { 
    f1=JD;
    f=UT1; while (f>24.0) { f-=24.0; f1+=1; }
    
    greg(f1,&i2,&i,&i1); 
    i2=(i2/99.99999-floor(i2/99.99999))*100;

    strcpy_P(temp1, html_index3); sprintf(temp,temp1,i,i1,i2);
  }
  if (html_page_step==++stp) { 
    i=highPrecision; highPrecision=true; 
    f=UT1; while (f>24.0) f-=24.0;
    doubleToHms(temp2,&f);
    highPrecision=i;

    strcpy_P(temp1, html_index4); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) { 
    i=highPrecision; highPrecision=true; 
    doubleToHms(temp2,&LST);
    highPrecision=i;
    
    strcpy_P(temp1, html_index4a); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) {
    i=highPrecision; highPrecision=true; 
    cli();
    double ha=((double)posHA/StepsPerDegreeHA)/15.0; 
    doubleToHms(temp2,&ha);
    double dec=(double)posDec/StepsPerDegreeDec; 
    sei();
    doubleToDms(temp3,&dec,false,true);
    highPrecision=i;
    
    strcpy_P(temp1, html_index5); sprintf(temp,temp1,temp2,temp3); 
  }
  if (html_page_step==++stp) {
    i=highPrecision; highPrecision=true;
    cli();
    double ha=((double)((long int)targetHA.part.m)/StepsPerDegreeHA)/15.0; 
    doubleToHms(temp2,&ha);
    double dec=((double)(long int)targetDec.part.m)/StepsPerDegreeDec; 
    sei();
    doubleToDms(temp3,&dec,false,true);
    highPrecision=i;
    
    strcpy_P(temp1, html_index6); sprintf(temp,temp1,temp2,temp3); 
  }
  if (html_page_step==++stp) {
    strcpy_P(temp1, html_index7); sprintf(temp,temp1,maxRate/16L,(long int)MaxRate);
  }
  if (html_page_step==++stp) {
    if (trackingState==TrackingNone)     strcpy(temp2,"Off");
    if (trackingState==TrackingSidereal) strcpy(temp2,"On");
    if (trackingState==TrackingMoveTo)   strcpy(temp2,"Slewing");

    if ((PPSsynced) && (atHome)) strcpy(temp3,"(at home, PPS sync)");
    if ((PPSsynced) && (!atHome)) strcpy(temp3,"(PPS sync)");
    if ((!PPSsynced) && (atHome)) strcpy(temp3,"(at home)");
    if ((!PPSsynced) && (!atHome)) strcpy(temp3,"");
    
    strcpy_P(temp1, html_index8); sprintf(temp,temp1,temp2,temp3);
  }
  if (html_page_step==++stp) {
    if (parkStatus==NotParked)  strcpy(temp2,"Not Parked");
    if (parkStatus==Parked)     strcpy(temp2,"Parked");
    if (parkStatus==Parking)    strcpy(temp2,"Parking");
    if (parkStatus==ParkFailed) strcpy(temp2,"Park Failed");
    
    strcpy_P(temp1, html_index9); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) { 
    if (PECstatus==IgnorePEC)      strcpy(temp2,"Ignore");
    if (PECstatus==PlayPEC)        strcpy(temp2,"Playing");
    if (PECstatus==RecordPEC)      strcpy(temp2,"Recording");
    if (PECstatus==ReadyRecordPEC) strcpy(temp2,"Waiting to Record");
    if (PECstatus==ReadyPlayPEC)   strcpy(temp2,"Waiting to Play");
    
    strcpy_P(temp1, html_index10); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) strcpy(temp,"</div></body></html>");

  // stop sending this page
  if (html_page_step==++stp) { html_page_step=0; responseStarted=false; return; }

  // send the data
  r=www_write(temp);
  if (!r) html_page_step--; // repeat this step if www_write failed
}

// The settings.htm page ----------------------------------------------------------------------------------
const char html_settings1[] PROGMEM = "<div class=\"t\"><table width=\"100%\"><tr><td><b>" FirmwareName " " FirmwareNumber ;
const char html_settings2[] PROGMEM = "</b></td><td align=\"right\"><b><font size=\"5\">SETTINGS</font></b></td></tr></table><br />";
const char html_settings3[] PROGMEM = "</div><div class=\"b\">\r\n";
const char html_settings4[] PROGMEM = 
"Maximum Slew Speed: "
"<form method=\"get\" action=\"/settings.htm\">"
"<button name=\"ss\" value=\"vs\" type=\"submit\">Very Slow (0.5x)</button>"
"<button name=\"ss\" value=\"s\" type=\"submit\">Slow (0.75x)</button>";
const char html_settings5[] PROGMEM = 
"<button name=\"ss\" value=\"n\" type=\"submit\">Normal (1x) </button>"
"<button name=\"ss\" value=\"f\" type=\"submit\">Fast (1.5x) </button>"
"<button name=\"ss\" value=\"vf\" type=\"submit\">Very Fast (2x) </button>"
"</form><br />\r\n";
const char html_settings7[] PROGMEM = 
"Backlash: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"b1\" min=\"0\" max=\"999\">"
"<button type=\"submit\">Upload</button>"
" (" Axis1a ", in arc-seconds 0 to 999)"
"</form>"
"\r\n";
const char html_settings8[] PROGMEM = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"b2\" min=\"0\" max=\"999\">"
"<button type=\"submit\">Upload</button>"
" (" Axis2 ", in arc-seconds 0 to 999)"
"</form>"
"<br />\r\n";
const char html_settings9[] PROGMEM = 
"Limits: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"hl\" min=\"-30\" max=\"30\">"
"<button type=\"submit\">Upload</button>"
" (Horizon, in degrees +/- 30)"
"</form>"
"\r\n";
const char html_settings10[] PROGMEM = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%d\" type=\"number\" name=\"ol\" min=\"60\" max=\"90\">"
"<button type=\"submit\">Upload</button>"
" (Overhead, in degrees 60 to 90)"
"</form>"
"<br />\r\n";
const char html_settings11[] PROGMEM = 
"Location: <br />"
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"g1\" min=\"-180\" max=\"180\">&nbsp;&deg;&nbsp;";
const char html_settings12[] PROGMEM = 
" <input value=\"%s\" type=\"number\" name=\"g2\" min=\"0\" max=\"60\">&nbsp;'&nbsp;&nbsp;"
"<button type=\"submit\">Upload</button>"
" (Longitude, in deg. and min. +/- 180)"
"</form>"
"\r\n";
const char html_settings13[] PROGMEM = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"t1\" min=\"-90\" max=\"90\">&nbsp;&deg;&nbsp;";
const char html_settings14[] PROGMEM = 
" <input value=\"%s\" type=\"number\" name=\"t2\" min=\"0\" max=\"60\">&nbsp;'&nbsp;&nbsp;"
"<button type=\"submit\">Upload</button>"
" (Latitude, in deg. and min. +/- 90)"
"</form>"
"\r\n";
const char html_settings15[] PROGMEM = 
"<form method=\"get\" action=\"/settings.htm\">"
" <input value=\"%s\" type=\"number\" name=\"u1\" min=\"-12\" max=\"14\">&nbsp;hrs&nbsp;";
const char html_settings16[] PROGMEM = 
" <select name=\"u2\"><option value=\"0\" %s>00</option><option value=\"30\" %s>30</option><option value=\"45\" %s>45</option></select>&nbsp;min.&nbsp;"
"<button type=\"submit\">Upload</button>"
" (UTC Offset, in hours -12 to +14)"
"</form>"
"\r\n";

void settings_html_page() {
  char temp[256] = "";
  char temp1[256] = "";
  char temp2[20] = "";
  int stp=0;
  html_page_step++;

  // send a standard http response header
  if (html_page_step==++stp) strcpy_P(temp, html_header1);
  if (html_page_step==++stp) strcpy_P(temp, html_header2);
  if (html_page_step==++stp) strcpy_P(temp, html_header3);
  if (html_page_step==++stp) strcpy_P(temp, html_header4a);
  if (html_page_step==++stp) strcpy_P(temp, html_header5);
  if (html_page_step==++stp) strcpy_P(temp, html_header6);

  if (html_page_step==++stp) strcpy_P(temp, html_main_css1);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css2);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css3);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css4);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css5);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css6);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css7);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css8);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css9);
  
  if (html_page_step==++stp) strcpy_P(temp, html_header7);
  if (html_page_step==++stp) strcpy_P(temp, html_header8);

  // and the remainder of the page
  if (html_page_step==++stp) strcpy_P(temp, html_settings1);
  if (html_page_step==++stp) strcpy_P(temp, html_settings2);
  if (html_page_step==++stp) strcpy_P(temp, html_links1);
  if (html_page_step==++stp) strcpy_P(temp, html_links2);
  if (html_page_step==++stp) strcpy_P(temp, html_links3);
  if (html_page_step==++stp) strcpy_P(temp, html_settings3);
  if (html_page_step==++stp) strcpy_P(temp, html_settings4);
  if (html_page_step==++stp) strcpy_P(temp, html_settings5);
  if (html_page_step==++stp) { strcpy_P(temp1, html_settings7); i=(int)(((long)backlashHA*3600L)/StepsPerDegreeHA); if (i<0) i=0; if (i>999) i=999; sprintf(temp,temp1,i); }
  if (html_page_step==++stp) { strcpy_P(temp1, html_settings8); i=(int)(((long)backlashDec*3600L)/StepsPerDegreeDec); if (i<0) i=0; if (i>999) i=999; sprintf(temp,temp1,i); }
  if (html_page_step==++stp) { strcpy_P(temp1, html_settings9); sprintf(temp,temp1,minAlt); }
  if (html_page_step==++stp) { strcpy_P(temp1, html_settings10); sprintf(temp,temp1,maxAlt); }
  if (html_page_step==++stp) {
    i=highPrecision; 
    highPrecision=false; 
    doubleToDms(temp2,&longitude,true,true);
    temp2[4]=0; // deg. part only
    if (temp2[0]=='+') temp2[0]='0'; // remove +
    highPrecision=i;
    strcpy_P(temp1, html_settings11); sprintf(temp,temp1,temp2);
    }
  if (html_page_step==++stp) { 
    i=highPrecision; 
    highPrecision=false; 
    doubleToDms(temp2,&longitude,true,true);
    highPrecision=i;
    strcpy_P(temp1, html_settings12); sprintf(temp,temp1,(char*)&temp2[5]);
  }
  if (html_page_step==++stp) { 
    i=highPrecision; 
    highPrecision=false; 
    doubleToDms(temp2,&latitude,false,true);
    temp2[3]=0; // deg. part only
    if (temp2[0]=='+') temp2[0]='0'; // remove +
    highPrecision=i;
    strcpy_P(temp1, html_settings13); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) { 
    i=highPrecision; 
    highPrecision=false; 
    doubleToDms(temp2,&latitude,false,true);
    highPrecision=i;
    strcpy_P(temp1, html_settings14); sprintf(temp,temp1,(char*)&temp2[4]);
  }
  if (html_page_step==++stp) {
    timeZoneToHM(temp2,timeZone);
    temp2[3]=0; // deg. part only
    if (temp2[0]=='+') temp2[0]='0'; // remove +
    strcpy_P(temp1, html_settings15); sprintf(temp,temp1,temp2);
  }
  if (html_page_step==++stp) {
    timeZoneToHM(temp2,timeZone);
    strcpy_P(temp1, html_settings16);
    if (temp2[3]==0) sprintf(temp,temp1,"selected","",""); else
    if (temp2[4]=='3') sprintf(temp,temp1,"","selected",""); else
    if (temp2[4]=='4') sprintf(temp,temp1,"","","selected");
  }
  if (html_page_step==++stp) strcpy(temp,"</div></body></html>");

  // stop sending this page
  if (html_page_step==++stp) { html_page_step=0; responseStarted=false; return; }

  // send the data
  if (!www_write(temp)) html_page_step--; // repeat this step if www_write failed
}

// The control.htm page ----------------------------------------------------------------------------------
const char html_control1[] PROGMEM = "<div class=\"t\"><table width=\"100%\"><tr><td><b>" FirmwareName " " FirmwareNumber ;
const char html_control2[] PROGMEM = "</b></td><td align=\"right\"><b><font size=\"5\">CONTROL</font></b></td></tr></table><br />";
const char html_control3[] PROGMEM = "</div><div class=\"b\">\r\n";
const char html_control4a[] PROGMEM = 
"Date/Time: "
"<form method=\"get\" action=\"/control.htm\">"
"<button type=\"submit\" onclick=\"SetDateTime();\">Set</button>"
"<input id=\"dm\" type=\"hidden\" name=\"dm\">"
"<input id=\"dd\" type=\"hidden\" name=\"dd\">";
const char html_control4b[] PROGMEM = 
"<input id=\"dy\" type=\"hidden\" name=\"dy\">"
"<input id=\"th\" type=\"hidden\" name=\"th\">"
"<input id=\"tm\" type=\"hidden\" name=\"tm\">"
"<input id=\"ts\" type=\"hidden\" name=\"ts\">"
"</form><br />\r\n";
const char html_control4c[] PROGMEM =
"<script>\r\n"
"function SetDateTime() {"
"var d = new Date();"
"document.getElementById(\"dd\").value = d.getDate();"
"document.getElementById(\"dm\").value = d.getMonth();"
"document.getElementById(\"dy\").value = d.getFullYear();";
const char html_control4d[] PROGMEM =
"document.getElementById(\"th\").value = d.getHours();"
"document.getElementById(\"tm\").value = d.getMinutes();"
"document.getElementById(\"ts\").value = d.getSeconds();"
"}\r\n"
"</script>\r\n";
const char html_controlAlign1[] PROGMEM =
"Align: "
"<form method=\"get\" action=\"/control.htm\">"
"<button name=\"al\" value=\"1\" type=\"submit\">1 Star</button>";
const char html_controlAlign23[] PROGMEM = 
"<button name=\"al\" value=\"2\" type=\"submit\">2 Star</button>"
"<button name=\"al\" value=\"3\" type=\"submit\">3 Star</button>";
const char html_controlAlign4[] PROGMEM = 
"<br /><br /><button name=\"al\" value=\"n\" type=\"submit\">Accept</button>"
"</form><br />\r\n";
const char html_control6[] PROGMEM = 
"Home/Park: "
"<form method=\"get\" action=\"/control.htm\">"
"<button name=\"hm\" value=\"r\" type=\"submit\">Home (Reset)</button>&nbsp;&nbsp;&nbsp;"
"<button name=\"hm\" value=\"f\" type=\"submit\">Find Home</button><br /><br />";
const char html_control7[] PROGMEM = 
"<button name=\"pk\" value=\"p\" type=\"submit\">Park</button>&nbsp;&nbsp;&nbsp;"
"<button name=\"pk\" value=\"u\" type=\"submit\">Un-Park</button><br /><br />"
"<button name=\"pk\" value=\"s\" type=\"submit\">Set-Park</button>"
"</form><br />\r\n";
const char html_control8[] PROGMEM = 
"Tracking: "
"<form method=\"get\" action=\"/control.htm\">"
"<button name=\"tk\" value=\"on\" type=\"submit\">On</button>"
"<button name=\"tk\" value=\"off\" type=\"submit\">Off</button>&nbsp;&nbsp;&nbsp;";
const char html_control9[] PROGMEM = 
"<button name=\"tk\" value=\"f\" type=\"submit\">+ (0.1Hz faster)</button>"
"<button name=\"tk\" value=\"-\" type=\"submit\">- (0.1Hz slower)</button>"
"<button name=\"tk\" value=\"r\" type=\"submit\">Reset (default)</button><br /><br />";
const char html_control10[] PROGMEM = 
"<button name=\"tk\" value=\"s\" type=\"submit\">Sidereal</button>"
"<button name=\"tk\" value=\"l\" type=\"submit\">Lunar</button>"
"<button name=\"tk\" value=\"h\" type=\"submit\">Solar</button>"
"</form>\r\n";

void control_html_page() {
  char temp[256] = "";
  int stp=0;
  html_page_step++;

  // send a standard http response header
  if (html_page_step==++stp) strcpy_P(temp, html_header1);
  if (html_page_step==++stp) strcpy_P(temp, html_header2);
  if (html_page_step==++stp) strcpy_P(temp, html_header3);
  if (html_page_step==++stp) strcpy_P(temp, html_header4a);
  if (html_page_step==++stp) strcpy_P(temp, html_header5);
  if (html_page_step==++stp) strcpy_P(temp, html_header6);

  if (html_page_step==++stp) strcpy_P(temp, html_main_css1);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css2);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css3);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css4);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css5);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css6);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css7);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css8);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css9);
  
  if (html_page_step==++stp) strcpy_P(temp, html_header7);
  if (html_page_step==++stp) strcpy_P(temp, html_header8);

  // and the remainder of the page
  if (html_page_step==++stp) strcpy_P(temp, html_control1);
  if (html_page_step==++stp) strcpy_P(temp, html_control2);
  if (html_page_step==++stp) strcpy_P(temp, html_links1);
  if (html_page_step==++stp) strcpy_P(temp, html_links2);
  if (html_page_step==++stp) strcpy_P(temp, html_links3);
  if (html_page_step==++stp) strcpy_P(temp, html_control3);
  if (html_page_step==++stp) strcpy_P(temp, html_control4a);
  if (html_page_step==++stp) strcpy_P(temp, html_control4b);
  if (html_page_step==++stp) strcpy_P(temp, html_control4c);
  if (html_page_step==++stp) strcpy_P(temp, html_control4d);
  if (html_page_step==++stp) strcpy_P(temp, html_controlAlign1);
#if defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK_ALT)
  if (html_page_step==++stp) strcpy_P(temp, html_controlAlign23);
#endif
  if (html_page_step==++stp) strcpy_P(temp, html_controlAlign4);
  if (html_page_step==++stp) strcpy_P(temp, html_control6);
  if (html_page_step==++stp) strcpy_P(temp, html_control7);
  if (html_page_step==++stp) strcpy_P(temp, html_control8);
  if (html_page_step==++stp) strcpy_P(temp, html_control9);
  if (html_page_step==++stp) strcpy_P(temp, html_control10);
  if (html_page_step==++stp) strcpy(temp,"</div></body></html>");

  // stop sending this page
  if (html_page_step==++stp) { html_page_step=0; responseStarted=false; return; }

  // send the data
  if (!www_write(temp)) html_page_step--; // repeat this step if www_write failed
}

// The pec.htm page ----------------------------------------------------------------------------------
const char html_pec1[] PROGMEM = "<div class=\"t\"><table width=\"100%\"><tr><td><b>" FirmwareName " " FirmwareNumber ;
const char html_pec2[] PROGMEM = "</b></td><td align=\"right\"><b><font size=\"5\">PEC</font></b></td></tr></table><br />";
const char html_pec3[] PROGMEM = "</div><div class=\"b\">\r\n";
const char html_pec4[] PROGMEM = "PEC: <br /><br />";
const char html_pec5[] PROGMEM = "STATUS = %s<br /><br />";
const char html_pecControls1[] PROGMEM =
"<form method=\"get\" action=\"/pec.htm\">"
"<button name=\"pe\" value=\"pl\" type=\"submit\">Play</button>";
const char html_pecControls2[] PROGMEM = 
"<button name=\"pe\" value=\"st\" type=\"submit\">Stop</button><br /><br />"
"<button name=\"pe\" value=\"cl\" type=\"submit\">Clear</button>";
const char html_pecControls3[] PROGMEM = 
"<button name=\"pe\" value=\"re\" type=\"submit\">Record</button><br /><br />"
"<button name=\"pe\" value=\"wr\" type=\"submit\">Write to EEPROM</button></form>"
"</form><br />\r\n";

void pec_html_page() {
  char temp[256] = "";
  char temp1[80] = "";
  char temp2[80] = "";
  int stp=0;
  html_page_step++;

  // send a standard http response header
  if (html_page_step==++stp) strcpy_P(temp, html_header1);
  if (html_page_step==++stp) strcpy_P(temp, html_header2);
  if (html_page_step==++stp) strcpy_P(temp, html_header3);
  if (html_page_step==++stp) strcpy_P(temp, html_header4);
  if (html_page_step==++stp) strcpy_P(temp, html_header5);
  if (html_page_step==++stp) strcpy_P(temp, html_header6);

  if (html_page_step==++stp) strcpy_P(temp, html_main_css1);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css2);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css3);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css4);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css5);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css6);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css7);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css8);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css9);
  
  if (html_page_step==++stp) strcpy_P(temp, html_header7);
  if (html_page_step==++stp) strcpy_P(temp, html_header8);

  // and the remainder of the page
  if (html_page_step==++stp) strcpy_P(temp, html_pec1);
  if (html_page_step==++stp) strcpy_P(temp, html_pec2);
  if (html_page_step==++stp) strcpy_P(temp, html_links1);
  if (html_page_step==++stp) strcpy_P(temp, html_links2);
  if (html_page_step==++stp) strcpy_P(temp, html_links3);
  if (html_page_step==++stp) strcpy_P(temp, html_pec3);
  if (html_page_step==++stp) strcpy_P(temp, html_pec4);
  if (html_page_step==++stp) {
#ifndef MOUNT_TYPE_ALTAZM
    const char *PECstatusCh = PECStatusString;
    if (PECstatusCh[PECstatus]=='I') { strcpy(temp2,"Idle"); } else
    if (PECstatusCh[PECstatus]=='p') { strcpy(temp2,"Play waiting Idx"); } else
    if (PECstatusCh[PECstatus]=='P') { strcpy(temp2,"Playing"); } else
    if (PECstatusCh[PECstatus]=='r') { strcpy(temp2,"Record waiting Idx"); } else
    if (PECstatusCh[PECstatus]=='R') { strcpy(temp2,"Recording"); } else
      strcpy(temp2,"Unknown");
#endif
    strcpy_P(temp1, html_pec5); sprintf(temp,temp1,temp2);
  }

#ifdef MOUNT_TYPE_ALTAZM
  if (html_page_step==++stp) strcpy_P(temp, "PEC CONTROLS DISABLED");
#else
  if (html_page_step==++stp) strcpy_P(temp, html_pecControls1);
  if (html_page_step==++stp) strcpy_P(temp, html_pecControls2);
  if (html_page_step==++stp) strcpy_P(temp, html_pecControls3);
#endif
  if (html_page_step==++stp) strcpy(temp,"</div></body></html>");

  // stop sending this page
  if (html_page_step==++stp) { html_page_step=0; responseStarted=false; return; }

  // send the data
  if (!www_write(temp)) html_page_step--; // repeat this step if www_write failed
}

// The config.htm page --------------------------------------------------------------------------------------
const char html_config1[] PROGMEM = "<div class=\"t\"><table width=\"100%\"><tr><td><b>" FirmwareName " " FirmwareNumber;
const char html_config2[] PROGMEM = "</b></td><td align=\"right\"><b><font size=\"5\">";
const char html_config3[] PROGMEM = "CONFIGURATION</font></b></td></tr></table><br />";
const char html_config4[] PROGMEM = "</div><div class=\"b\">";
#if defined(MOUNT_TYPE_GEM)
const char html_configMount[] PROGMEM = "Mount type: GEM<br /><br />";
#elif defined(MOUNT_TYPE_FORK)
const char html_configMount[] PROGMEM = "Mount type: Fork or similar<br /><br />";
#elif defined(MOUNT_TYPE_FORKALT)
const char html_configMount[] PROGMEM = "Mount type: Fork or similar (Alternate)<br /><br />";
#elif defined(MOUNT_TYPE_ALTAZM)
const char html_configMount[] PROGMEM = "Mount type: AltAz<br /><br />";
#endif
#if defined(ST4_OFF)
const char html_configST4[] PROGMEM = "ST4 interface is OFF<br /><br />";
#elif defined(ST4_ON)
const char html_configST4[] PROGMEM = "ST4 interface is ON (no internal pullup resistors)<br /><br />";
#elif defined(ST4_PULLUP)
const char html_configST4[] PROGMEM = "ST4 interface is ON (with internal pullup resistors)<br /><br />";
#endif
#if defined(PPS_SENSE_OFF)
const char html_configPPS[] PROGMEM = "PPS sense is OFF<br />";
#elif defined(PPS_SENSE_ON)
const char html_configPPS[] PROGMEM = "PPS sense is ON<br />";
#endif
#if defined(PEC_SENSE_OFF)
const char html_configPEC[] PROGMEM = "PEC sense is OFF<br />";
#elif defined(PEC_SENSE_ON)
const char html_configPEC[] PROGMEM = "PEC sense is ON<br />";
#endif
#if defined(LIMIT_SENSE_OFF)
const char html_configLimit[] PROGMEM = "LIMIT sense is OFF<br />";
#elif defined(LIMIT_SENSE_ON)
const char html_configLimit[] PROGMEM = "LIMIT sense is ON<br />";
#endif

void config_html_page() {
  char temp[256] = "";
  char temp1[80] = "";
  char temp2[20] = "";
  char temp3[20] = "";
  int stp=0;
  html_page_step++;
    
  // send a standard http response header
  if (html_page_step==++stp) strcpy_P(temp, html_header1); 
  if (html_page_step==++stp) strcpy_P(temp, html_header2);
  if (html_page_step==++stp) strcpy_P(temp, html_header3);
  if (html_page_step==++stp) strcpy_P(temp, html_header4a);
  if (html_page_step==++stp) strcpy_P(temp, html_header5);
  if (html_page_step==++stp) strcpy_P(temp, html_header6);

  if (html_page_step==++stp) strcpy_P(temp, html_main_css1);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css2);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css3);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css4);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css5);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css6);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css7);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css8);
  if (html_page_step==++stp) strcpy_P(temp, html_main_css9);
  
  if (html_page_step==++stp) strcpy_P(temp, html_header7);
  if (html_page_step==++stp) strcpy_P(temp, html_header8);

  // and the remainder of the page
  if (html_page_step==++stp) strcpy_P(temp, html_config1);
  if (html_page_step==++stp) strcpy_P(temp, html_config2);
  if (html_page_step==++stp) strcpy_P(temp, html_config3);
  if (html_page_step==++stp) strcpy_P(temp, html_links1);
  if (html_page_step==++stp) strcpy_P(temp, html_links2);
  if (html_page_step==++stp) strcpy_P(temp, html_links3);
  if (html_page_step==++stp) strcpy_P(temp, html_config4);
  if (html_page_step==++stp) strcpy_P(temp, html_configMount);
  if (html_page_step==++stp) strcpy_P(temp, html_configST4);
  if (html_page_step==++stp) strcpy_P(temp, html_configPPS);
  if (html_page_step==++stp) strcpy_P(temp, html_configPEC);
  if (html_page_step==++stp) strcpy_P(temp, html_configLimit);
  if (html_page_step==++stp) strcpy(temp,"</div></body></html>");

  // stop sending this page
  if (html_page_step==++stp) { html_page_step=0; responseStarted=false; return; }

  // send the data
  if (!www_write(temp)) html_page_step--; // repeat this step if www_write failed
}

#endif
