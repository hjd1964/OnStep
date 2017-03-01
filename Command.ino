// -----------------------------------------------------------------------------------
// Command processing

// scratch-pad variables
double f,f1,f2,f3; 
int    i,i1,i2;
byte   b;
unsigned long _coord_t=0;
double _dec,_ra;

// help with commands
enum Command {COMMAND_NONE, COMMAND_SERIAL, COMMAND_SERIAL1, COMMAND_ETHERNET, COMMAND_SPI};
char reply[50];
char command[3];
char parameter[25];
boolean commandError = false;
boolean quietReply   = false;
cb cmd;  // serial
cb cmd1; // serial1
cb cmde; // ethernet

// process commands
void processCommands() {
    boolean supress_frame = false;
    char *conv_end;

    // accumulate the command
    if ((PSerial.available()>0) && (!cmd.ready())) cmd.add(PSerial.read());
    if ((PSerial1.available()>0) && (!cmd1.ready())) cmd1.add(PSerial1.read());
#ifdef ETHERNET_ON
    if ((Ethernet_available()>0) && (!cmde.ready())) cmde.add(Ethernet_read());
#endif

    // send any reply
    if (PSerial.transmit() || PSerial1.transmit() 
#ifdef ETHERNET_ON
    || Ethernet_transmit()
#endif
    ) return;

    // if a command is ready, process it
    Command process_command = COMMAND_NONE;
    if (cmd.ready()) { strcpy(command,cmd.getCmd()); strcpy(parameter,cmd.getParameter()); cmd.flush(); process_command=COMMAND_SERIAL; }
    else if (cmd1.ready()) { strcpy(command,cmd1.getCmd()); strcpy(parameter,cmd1.getParameter()); cmd1.flush(); process_command=COMMAND_SERIAL1; }
#ifdef ETHERNET_ON
    else if (cmde.ready()) { strcpy(command,cmde.getCmd()); strcpy(parameter,cmde.getParameter()); cmde.flush(); process_command=COMMAND_ETHERNET; }
#endif
    else {

    // if a command isn't ready, give the www interface some time
#ifdef ETHERNET_ON
#if !(defined(__ARM_Teensy3__))
      if (!Ethernet_cmd_busy())
#endif
      Ethernet_www();
#endif
      return;
    }

    if (process_command) {
// Command is two chars followed by an optional parameter...
      commandError=false;
// Handles empty and one char replies
      reply[0]=0; reply[1]=0;

//   (char)6 - Special
      if (command[0]==(char)6) {
        if (command[1]=='0') {
          reply[0]=command[1]; reply[1]=command[1]; reply[2]=0; // last cmd checksum failed, 00#
        } else {
          reply[0]=command[1]; reply[1]=0; // Equatorial or Horizon mode, A or P
          supress_frame=true;
        }
        quietReply=true;
      } else

//   A - Alignment Commands
      if (command[0]=='A') {
//  :AW#  Align Write to EEPROM
//         Returns: 1 on success
        if (command[1]=='W') {
          saveAlignModel();
        } else
//  :A?#  Align status
//         Returns: mno#
//         where m is the maximum number of alignment stars
//               n is the current alignment star (0 otherwise)
//               o is the last required alignment star when an alignment is in progress (0 otherwise)
        if (command[1]=='?') {
          reply[0]=MAX_NUM_ALIGN_STARS;
          reply[1]='0'+alignThisStar;
          reply[2]='0'+alignNumStars;
          reply[3]=0;
          quietReply=true;
        } else
//  :An#  Start Telescope Manual Alignment Sequence
//         This is to initiate a one or two-star alignment:
//         1) Before calling this function, the telescope should be in the polar-home position
//         2) Call this function
//         3) Set the target location (RA/Dec) to a bright star, etc. (near the celestial equator in the western sky)
//         4) Issue a goto command
//         5) Center the star/object using the guide commands (as needed)
//         6) Call :A+# command to accept the correction
//         ( for two-star alignment )
//         7) Set the target location (RA/Dec) to a bright star, etc. (near the celestial equator in the southern sky)
//         8) Issue a goto command
//         9) Center the star/object using the guide commands (as needed)
//         10) Call :A+# command to accept the correction
//         Returns:
//         1: When ready for your goto commands
//         0: If mount is busy
        if ((command[1]>='1') && (command[1]<='9')) {
          // set current time and date before calling this routine

          // Two star and three star align not supported with Fork mounts in alternate mode
#ifdef MOUNT_TYPE_FORK_ALT
          if (n==1) {
#endif
        
          // telescope should be set in the polar home (CWD) for a starting point
          // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
          setHome();
          
          // enable the stepper drivers
          digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
          digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
          delay(10);
        
          // start tracking
          trackingState=TrackingSidereal;
        
          // start align...
          alignNumStars=command[1]-'0';
          alignThisStar=1;
       
#if defined(MOUNT_TYPE_FORK_ALT)
          } else commandError=true;
#endif

          if (commandError) { alignNumStars=0; alignThisStar=1; }
        } else
//  :A+#  Manual Alignment, set target location
//         Returns:
//         1: If correction is accepted
//         0: Failure, Manual align mode not set or distance too far 
        if (command[1]=='+') {
            // after last star turn meridian flips off when align is done
            if ((alignNumStars==alignThisStar) && (meridianFlip==MeridianFlipAlign)) meridianFlip=MeridianFlipNever;
          
#ifdef MOUNT_TYPE_ALTAZM
            // AltAz Taki method
            if ((alignNumStars>1) && (alignThisStar<=alignNumStars)) {
              cli();
              // get the Azm/Alt
              double F=(double)(posAxis1+indexAxis1Steps)/(double)StepsPerDegreeAxis1;
              double H=(double)(posAxis2+indexAxis2Steps)/(double)StepsPerDegreeAxis2;
              sei();
              // B=RA, D=Dec, H=Elevation (instr), F=Azimuth (instr), all in degrees
              Align.addStar(alignThisStar,alignNumStars,haRange(LST()*15.0-newTargetRA),newTargetDec,H,F);
              alignThisStar++;
            } else
#endif
            if (alignThisStar<=alignNumStars) {
              // RA, Dec (in degrees)
              if (GeoAlign.addStar(alignThisStar,alignNumStars,newTargetRA,newTargetDec)) alignThisStar++; else commandError=true;
            } else commandError=true;

          if (commandError) { alignNumStars=0; alignThisStar=0; }
        } else commandError=true; 
      } else
      
//   $ - Set parameter
//  :$BDddd# Set Dec Antibacklash
//          Return: 0 on failure
//                  1 on success
//  :$BRddd# Set RA Antibacklash
//          Return: 0 on failure
//                  1 on success
//         Set the Backlash values.  Units are arc-seconds
      if ((command[0]=='$') && (command[1]=='B')) {
        if ((strlen(parameter)>1) && (strlen(parameter)<5)) {
          if ( (atoi2((char*)&parameter[1],&i)) && ((i>=0) && (i<=999))) { 
            if (parameter[0]=='D') {
              backlashAxis2=(int)round(((double)i*(double)StepsPerDegreeAxis2)/3600.0);
              EEPROM_writeInt(EE_backlashAxis2,backlashAxis2);
            } else
            if (parameter[0]=='R') {
              backlashAxis1 =(int)round(((double)i*(double)StepsPerDegreeAxis1)/3600.0);
              EEPROM_writeInt(EE_backlashAxis1,backlashAxis1);
            } else commandError=true;
          } else commandError=true;
        } else commandError=true;
      } else
      
//   % - Return parameter
//  :%BD# Get Dec Antibacklash
//          Return: d#
//  :%BR# Get RA Antibacklash
//          Return: d#
//          Get the Backlash values.  Units are arc-seconds
      if ((command[0]=='%') && (command[1]=='B')) {
        if (parameter[0]=='D') {
            i=(int)round(((double)backlashAxis2*3600.0)/(double)StepsPerDegreeAxis2);
            if (i<0) i=0; if (i>999) i=999;
            sprintf(reply,"%d",i);
            quietReply=true;
        } else
        if (parameter[0]=='R') {
            i=(int)round(((double)backlashAxis1*3600.0)/(double)StepsPerDegreeAxis1);
            if (i<0) i=0; if (i>999) i=999;
            sprintf(reply,"%d",i);
            quietReply=true;
        } else commandError=true;
      } else
      
//   B - Reticule/Accessory Control
//  :B+#   Increase reticule Brightness
//         Returns: Nothing
//  :B-#   Decrease Reticule Brightness
//         Returns: Nothing
      if ((command[0]=='B') && ((command[1]=='+') || (command[1]=='-')))  {
#ifdef RETICULE_LED_PINS
        int scale;
        if (reticuleBrightness>255-8) scale=1; else
        if (reticuleBrightness>255-32) scale=4; else
        if (reticuleBrightness>255-64) scale=12; else
        if (reticuleBrightness>255-128) scale=32; else scale=64;
        if (command[1]=='-') reticuleBrightness+=scale;  if (reticuleBrightness>255) reticuleBrightness=255;
        if (command[1]=='+') reticuleBrightness-=scale;  if (reticuleBrightness<0)   reticuleBrightness=0;
        analogWrite(ReticulePin,reticuleBrightness);
#endif
        quietReply=true;
      } else 

//   C - Sync Control
//  :CS#   Synchonize the telescope with the current right ascension and declination coordinates
//         Returns: Nothing
//  :CM#   Synchonize the telescope with the current database object (as above)
//         Returns: "N/A#"
      if ((command[0]=='C') && ((command[1]=='S') || command[1]=='M'))  {
        if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
          syncEqu(newTargetRA,newTargetDec);
          if (command[1]=='M') strcpy(reply,"N/A");
          quietReply=true;
        }
      } else 

      
//   D - Distance Bars
//  :D#    returns an "\0x7f#" if the mount is moving, otherwise returns "#".
      if ((command[0]=='D') && (command[1]==0))  { if (trackingState==TrackingMoveTo) { reply[0]=(char)127; reply[1]=0; } else { reply[0]=0; } quietReply=true; } else 
 
//   G - Get Telescope Information
      if (command[0]=='G') {

//  :GA#   Get Telescope Altitude
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
//         The current scope altitude
      if (command[1]=='A')  { getHor(&f,&f1); if (!doubleToDms(reply,&f,false,true)) commandError=true; else quietReply=true; } else
//  :Ga#   Get Local Time in 12 hour format
//         Returns: HH:MM:SS#
      if (command[1]=='a')  { i=highPrecision; highPrecision=true; LMT=timeRange(UT1-timeZone); if (LMT>12.0) LMT-=12.0; if (!doubleToHms(reply,&LMT)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GC#   Get the current date
//         Returns: MM/DD/YY#
//         The current local calendar date
      if (command[1]=='C') { 
        LMT=UT1-timeZone;
        greg(JD,&i2,&i,&i1); 
        i2=(i2/99.99999-floor(i2/99.99999))*100;

        // correct for day moving forward/backward... this works for multipule days of up-time, but eventually
        while (LMT>=24.0) { LMT=LMT-24.0; i1--; } 
        if    (LMT<0.0)   { LMT=LMT+24.0; i1++; }
        sprintf(reply,"%02d/%02d/%02d",i,i1,i2); 
        quietReply=true; 
      } else 
//  :Gc#   Get the current time format
//         Returns: 24#
//         The current local time format
      if (command[1]=='c') {
        strcpy(reply,"24");
        quietReply=true; 
       } else
//  :GD#   Get Telescope Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
      if (command[1]=='D')  { 
        if (millis()-_coord_t<100) { f=_ra; f1=_dec; } else {
          getEqu(&f,&f1,false); f/=15.0;
          _ra=f; _dec=f1; _coord_t=millis(); 
        }
        if (!doubleToDms(reply,&f1,false,true)) commandError=true; else quietReply=true; 
      } else 
//  :Gd#   Get Currently Selected Target Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
      if (command[1]=='d')  { if (!doubleToDms(reply,&newTargetDec,false,true)) commandError=true; else quietReply=true; } else 
//  :GG#   Get UTC offset time
//         Returns: sHH#
//         The number of decimal hours to add to local time to convert it to UTC
      if (command[1]=='G')  { 
        timeZoneToHM(reply,timeZone);
        quietReply=true; 
      } else 
//  :Gg#   Get Current Site Longitude
//         Returns: sDDD*MM#
//         The current site Longitude. East Longitudes are negative
      if (command[1]=='g')  { i=highPrecision; highPrecision=false; if (!doubleToDms(reply,&longitude,true,true)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :Gh#   Get Horizon Limit
//         Returns: sDD*#
//         The minimum elevation of an object above the horizon required for a mount goto
      if (command[1]=='h')  { sprintf(reply,"%+02d*",minAlt); quietReply=true; } else
//  :GL#   Get Local Time in 24 hour format
//         Returns: HH:MM:SS#
//         On devices with single precision fp several days up-time will cause loss of precision as additional mantissa digits are needed to represent hours
//         Devices with double precision fp are limitated by sidereal clock overflow which takes 249 days
      if (command[1]=='L')  { i=highPrecision; highPrecision=true; LMT=timeRange(UT1-timeZone); if (!doubleToHms(reply,&LMT)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GM#   Get Site 1 Name
//  :GN#   Get Site 2 Name
//  :GO#   Get Site 3 Name
//  :GP#   Get Site 4 Name
//         Returns: <string>#
//         A # terminated string with the name of the requested site.
      if ((command[1]=='M') || (command[1]=='N') || (command[1]=='O') || (command[1]=='P'))  { 
        i=command[1]-'M';
        EEPROM_readString(EE_sites+i*25+9,reply); 
        if (reply[0]==0) { strcat(reply,"None"); }
        quietReply=true; 
      } else
//  :Gm#   Gets the meridian pier-side
//         Returns: E#, W#, N# (none/parked), ?# (Meridian flip in progress) 
//         A # terminated string with the pier side.
      if (command[1]=='m')  {
        reply[0]='?'; reply[1]=0;
        if (pierSide==PierSideNone) reply[0]='N';
        if (pierSide==PierSideEast) reply[0]='E';
        if (pierSide==PierSideWest) reply[0]='W';
        quietReply=true; } else
//  :Go#   Get Overhead Limit
//         Returns: DD*#
//         The highest elevation above the horizon that the telescope will goto
      if (command[1]=='o')  { sprintf(reply,"%02d*",maxAlt); quietReply=true; } else
//  :GR#   Get Telescope RA
//         Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
      if (command[1]=='R')  { 
        if (millis()-_coord_t<100) { f=_ra; f1=_dec; } else {
          getEqu(&f,&f1,false); f/=15.0;
          _ra=f; _dec=f1; _coord_t=millis(); 
        }
        if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true;  
      } else 
//  :Gr#   Get current/target object RA
//         Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
      if (command[1]=='r')  { f=newTargetRA; f/=15.0; if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true; } else 
//  :GS#   Get the Sidereal Time
//         Returns: HH:MM:SS#
//         The Sidereal Time as an ASCII Sexidecimal value in 24 hour format
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; f=LST(); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GT#   Get tracking rate
//         Returns: dd.ddddd# (OnStep returns more decimal places than LX200 standard)
//         Returns the tracking rate if siderealTracking, 0.0 otherwise
      if (command[1]=='T')  {
        if (trackingState==TrackingSidereal) {
#ifdef MOUNT_TYPE_ALTAZM
          f=GetTrackingRate()*1.00273790935*60.0; 
#else
          f=(trackingTimerRateAxis1*1.00273790935)*60.0; 
#endif
        }
        else f=0.0;
        char temp[10];
        dtostrf(f,0,5,temp);
        strcpy(reply,temp);
        quietReply=true;
      } else 
//  :Gt#   Get Current Site Latitude
//         Returns: sDD*MM#
//         The latitude of the current site. Positive for North latitudes
      if (command[1]=='t')  { i=highPrecision; highPrecision=false; if (!doubleToDms(reply,&latitude,false,true)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GU#   Get telescope Status
//         Returns: SS#
      if (command[1]=='U')  { 
        i=0;
        if (trackingState!=TrackingSidereal)     reply[i++]='n';
        if (trackingState!=TrackingMoveTo)       reply[i++]='N';
        const char *parkStatusCh = "pIPF";       reply[i++]=parkStatusCh[parkStatus]; // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (pecRecorded)                         reply[i++]='R';
        if (pecAutoRecord)                       reply[i++]='W';
        if (atHome)                              reply[i++]='H'; 
        if (PPSsynced)                           reply[i++]='S';
        if ((guideDirAxis1) || (guideDirAxis2))  reply[i++]='G';
        if (faultAxis1 || faultAxis2)            reply[i++]='f';
        if (refraction)                          reply[i++]='r'; else reply[i++]='s';
        if (onTrack)                             reply[i++]='t';
        // provide mount type
        #ifdef MOUNT_TYPE_GEM
        reply[i++]='E';
        #endif
        #ifdef MOUNT_TYPE_FORK
        reply[i++]='K';
        #endif
        #ifdef MOUNT_TYPE_FORK_ALT
        reply[i++]='k';
        #endif
        #ifdef MOUNT_TYPE_ALTAZM
        reply[i++]='A';
        #endif
        reply[i++]='0'+lastError;
        reply[i++]=0;
        quietReply=true;
      } else 
//  :GVD# Get Telescope Firmware Date
//         Returns: mm dd yyyy#
//  :GVN# Get Telescope Firmware Number
//         Returns: dd.d#
//  :GVP# Get Telescope Product Name
//         Returns: <string>#
//  :GVT# Get Telescope Firmware Time
//         returns: HH:MM:SS#
      if (command[1]=='V') {
        if (parameter[1]==(char)0) {
          if (parameter[0]=='D') strcpy(reply,FirmwareDate); else
          if (parameter[0]=='N') strcpy(reply,FirmwareNumber); else
          if (parameter[0]=='P') strcpy(reply,FirmwareName); else
          if (parameter[0]=='T') strcpy(reply,FirmwareTime); else commandError=true;
        } else commandError=true;
        
        quietReply=true; 
      } else 
//  :GXnn#   Get OnStep value
//         Returns: value
      if (command[1]=='X')  { 
        if (parameter[2]==(char)0) {
          if (parameter[0]=='0') { // 0n: Align Model
            switch (parameter[1]) {
              case '0': sprintf(reply,"%ld",(long)(indexAxis1*3600.0)); quietReply=true; break;        // indexAxis1
              case '1': sprintf(reply,"%ld",(long)(indexAxis2*3600.0)); quietReply=true; break;        // indexAxis2
              case '2': sprintf(reply,"%ld",(long)(GeoAlign.altCor*3600.0)); quietReply=true; break;   // altCor
              case '3': sprintf(reply,"%ld",(long)(GeoAlign.azmCor*3600.0)); quietReply=true; break;   // azmCor
              case '4': sprintf(reply,"%ld",(long)(GeoAlign.doCor*3600.0)); quietReply=true; break;    // doCor
              case '5': sprintf(reply,"%ld",(long)(GeoAlign.pdCor*3600.0)); quietReply=true; break;    // pdCor
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
              case '6': sprintf(reply,"%ld",(long)(GeoAlign.dfCor*3600.0)); quietReply=true; break;    // ffCor
              case '7': sprintf(reply,"%ld",(long)(0)); quietReply=true; break;                        // dfCor
#else
              case '6': sprintf(reply,"%ld",(long)(0)); quietReply=true; break;                        // ffCor
              case '7': sprintf(reply,"%ld",(long)(GeoAlign.dfCor*3600.0)); quietReply=true; break;    // dfCor
#endif
              case '8': sprintf(reply,"%ld",(long)(GeoAlign.tfCor*3600.0)); quietReply=true; break;    // tfCor
            }
          } else
          if (parameter[0]=='8') { // 8n: Date/Time
            switch (parameter[1]) {
              case '0': i=highPrecision; highPrecision=true; f=timeRange(UT1); doubleToHms(reply,&f); highPrecision=i; quietReply=true; break;  // UTC time
              case '1': f1=JD; f=UT1; while (f>=24.0) { f-=24.0; f1+=1; } while (f<0.0) { f+=24.0; f1-=1; } greg(f1,&i2,&i,&i1); i2=(i2/99.99999-floor(i2/99.99999))*100; sprintf(reply,"%d/%d/%d",i,i1,i2); quietReply=true; break; // UTC date
            }
          } else
          if (parameter[0]=='9') { // 9n: Misc.
            switch (parameter[1]) {
              case '0': dtostrf(guideRates[currentPulseGuideRate]/15.0,2,2,reply); quietReply=true; break;  // pulse-guide rate
              case '1': sprintf(reply,"%i",pecAnalogValue); quietReply=true; break;                         // pec analog value
              case '2': sprintf(reply,"%ld",(long)(maxRate/16L)); quietReply=true; break;                   // MaxRate
              case '3': sprintf(reply,"%ld",(long)(MaxRate)); quietReply=true; break;                       // MaxRate (default)
              case '4': if (meridianFlip==MeridianFlipNever) { sprintf(reply,"%d N",(int)(pierSide)); } else { sprintf(reply,"%d",(int)(pierSide)); } quietReply=true; break; // pierSide (N if never)
              case '5': sprintf(reply,"%i",(int)autoContinue); quietReply=true; break;                      // autoContinue
              case '6': 
                if (preferredPierSide==PPS_EAST) strcpy(reply,"E"); else
                if (preferredPierSide==PPS_WEST) strcpy(reply,"W"); else strcpy(reply,"B");
                quietReply=true; break;                 // preferred pier side
            }
          } else
          if (parameter[0]=='E') { // En: Get config
            if (parameter[1]=='0') { // simple values
              String c;
              #ifdef DEBUG_ON
                c="1";
              #else
                c="0";
              #endif
              #ifdef SYNC_ANYWHERE_ON
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(MOUNT_TYPE_GEM)
                c+="0";
              #elif defined(MOUNT_TYPE_FORK)
                c+="1";
              #elif defined(MOUNT_TYPE_FORK_ALT)
                c+="2";
              #elif defined(MOUNT_TYPE_ALTAZM)
                c+="3";
              #else
                c+="9";
              #endif
              #if defined(ST4_OFF)
                c+="0";
              #elif defined(ST4_ON)
                c+="1";
              #elif defined(ST4_PULLUP)
                c+="2";
              #else
                c+="0";
              #endif
              #if defined(ST4_ALTERNATE_PINS_ON)
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(PPS_SENSE_ON)
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(PEC_SENSE_ON)
                c+="1";
              #elif defined(PEC_SENSE_PULLUP)
                c+="2";
              #else
                c+="0";
              #endif
              #ifdef LIMIT_SENSE_ON
                c+="1";
              #else
                c+="0";
              #endif
              #ifdef STATUS_LED_PINS_ON
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(STATUS_LED2_PINS_ON)
                c+="1";
              #elif defined(STATUS_LED2_PINS)
                c+="2";
              #else
                c+="0";
              #endif
              #ifdef RETICULE_LED_PINS
                c+="1";
              #else
                c+="0";
              #endif
              #ifdef POWER_SUPPLY_PINS_ON
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(AXIS1_DISABLED_HIGH)
                c+="1";
              #elif defined(AXIS1_DISABLED_LOW)
                c+="0";
              #else
                c+="9";
              #endif
              #if defined(AXIS2_DISABLED_HIGH)
                c+="1";
              #elif defined(AXIS2_DISABLED_LOW)
                c+="0";
              #else
                c+="9";
              #endif
              #if defined(AXIS1_FAULT_LOW)
                c+="0";
              #elif defined(AXIS1_FAULT_HIGH)
                c+="1";
              #elif defined(AXIS1_FAULT_OFF)
                c+="2";
              #endif
              #if defined(AXIS2_FAULT_LOW)
                c+="0";
              #elif defined(AXIS2_FAULT_HIGH)
                c+="1";
              #elif defined(AXIS2_FAULT_OFF)
                c+="2";
              #endif
              #ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
                c+="1";
              #else
                c+="0";
              #endif
              #ifdef SEPERATE_PULSE_GUIDE_RATE_ON
                c+="1";
              #else
                c+="0";
              #endif
              #if defined(RememberMaxRate_ON) || defined(REMEMBER_MAX_RATE_ON)
                c+="1";
              #else
                c+="0";
              #endif
              strcpy(reply,(char *)c.c_str());
              quietReply=true;
            } else
            if (parameter[1]=='1') { sprintf(reply,"%ld",(long)MaxRate); quietReply=true; } else
            if (parameter[1]=='2') { dtostrf(DegreesForAcceleration,2,1,reply); quietReply=true; } else
            if (parameter[1]=='3') { sprintf(reply,"%ld",(long)round(BacklashTakeupRate)); quietReply=true; } else
            if (parameter[1]=='4') { sprintf(reply,"%ld",(long)round(StepsPerDegreeAxis1)); quietReply=true; } else
            if (parameter[1]=='5') { sprintf(reply,"%ld",(long)round(StepsPerDegreeAxis2)); quietReply=true; } else
            if (parameter[1]=='6') { dtostrf(StepsPerSecondAxis1,3,6,reply); quietReply=true; } else
            if (parameter[1]=='7') { sprintf(reply,"%ld",(long)round(StepsPerWormRotationAxis1)); quietReply=true; } else
            if (parameter[1]=='8') { sprintf(reply,"%ld",(long)round(PECBufferSize)); quietReply=true; } else
            if (parameter[1]=='9') { sprintf(reply,"%ld",(long)round(MinutesPastMeridianE)); quietReply=true; } else
            if (parameter[1]=='A') { sprintf(reply,"%ld",(long)round(MinutesPastMeridianW)); quietReply=true; } else
            if (parameter[1]=='B') { sprintf(reply,"%ld",(long)round(UnderPoleLimit)); quietReply=true; } else
            if (parameter[1]=='C') { sprintf(reply,"%ld",(long)round(MinDec)); quietReply=true; } else
            if (parameter[1]=='D') { sprintf(reply,"%ld",(long)round(MaxDec)); quietReply=true; } else commandError=true;
          } else
          if (parameter[0]=='F') { // Fn: Debug
            long temp;
            switch (parameter[1]) {
              case '0': cli(); temp=(long)(posAxis1-((long)targetAxis1.part.m)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;        // Debug0, true vs. target RA position
              case '1': cli(); temp=(long)(posAxis2-((long)targetAxis2.part.m)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;        // Debug1, true vs. target Dec position
//              case '0': cli(); temp=(long)(((az_Azm1-az_Azm2)*2.0)*1000); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;               // Debug0, true vs. target RA position
//              case '1': cli(); temp=(long)(az_Azm1); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                                    // Debug1, true vs. target Dec position
              case '2': sprintf(reply,"%ld",(long)((debugv1/53333.3333333333)*15000)); quietReply=true; break;                                    // Debug2, RA tracking rate
              case '3': sprintf(reply,"%ld",(long)(az_deltaAxis1*1000.0*1.00273790935)); quietReply=true; break;                                  // Debug3, RA refraction tracking rate
              case '4': sprintf(reply,"%ld",(long)(az_deltaAxis2*1000.0*1.00273790935)); quietReply=true; break;                                  // Debug4, Dec refraction tracking rate
              case '5': sprintf(reply,"%ld",(long)(ZenithTrackingRate()*1000.0*1.00273790935)); quietReply=true; break;                           // Debug5, Alt RA refraction tracking rate
              case '6': cli(); temp=(long)(targetAxis1.part.m);  sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                        // Debug6, HA target position
              case '7': cli(); temp=(long)(targetAxis2.part.m); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                         // Debug7, Dec target position
              case '8': cli(); temp=(long)(posAxis1);     sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                               // Debug8, HA motor position
              case '9': cli(); temp=(long)(posAxis2);    sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                                // Debug9, Dec motor position
              case 'A': sprintf(reply,"%ld%%",(worst_loop_time*100L)/9970L); worst_loop_time=0; quietReply=true; break;                           // DebugA, Workload
              
            }
          } else commandError=true;
        } else commandError=true;
        getEqu(&f,&f1,false);  
      } else 
//  :GZ#   Get telescope azimuth
//         Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
      if (command[1]=='Z')  { getHor(&f,&f1); if (!doubleToDms(reply,&f1,true,false)) commandError=true; else quietReply=true; } else commandError=true;
      } else

//  h - Home Position Commands
      if (command[0]=='h') {
//  :hF#   Reset telescope at the home position.  This position is required for a Cold Start.
//         Point to the celestial pole with the counterweight pointing downwards (CWD position).
//         Returns: Nothing
      if (command[1]=='F')  { setHome(); quietReply=true; } else 
//  :hC#   Moves telescope to the home position
//         Returns: Nothing
      if (command[1]=='C')  {  goHome(); quietReply=true; } else 
//  :hP#   Goto the Park Position
//         Returns: Nothing
      if (command[1]=='P')  { if (park()) commandError=true; } else 
//  :hQ#   Set the park position
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='Q')  { if (!setPark()) commandError=true; } else 
//  :hR#   Restore parked telescope to operation
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='R')  { if (!unpark()) commandError=true; }
      else commandError=true; 

      } else
      
//   L - Object Library Commands
      if (command[0]=='L') {

// :LB#    Find previous object and set it as the current target object.
//          Returns: Nothing
      if (command[1]=='B') { 
          Lib.prevRec(); quietReply=true;
      } else 

// :LCNNNN#
//         Set current target object to deep sky catalog object number NNNN
//          Returns : Nothing
      if (command[1]=='C') {
        if ( (atoi2((char *)&parameter[0],&i)) && ((i>=0) && (i<=32767))) {
          Lib.gotoRec(i); quietReply=true;
        } else commandError=true;
      } else 

// :LI#    Get Object Information
//          Returns: <string>#
//          Returns a string containing the current target object’s name and object type.
      if (command[1]=='I') {
        Lib.readVars(reply,&i,&newTargetRA,&newTargetDec);

        char const * objType=objectStr[i];
        strcat(reply,",");
        strcat(reply,objType);
        quietReply=true;
          
      } else 

// :LR#    Get Object Information including RA and Dec, with advance to next Record
//          Returns: <string>#
//          Returns a string containing the current target object’s name, type, RA, and Dec.
      if (command[1]=='R') {
        Lib.readVars(reply,&i,&newTargetRA,&newTargetDec);

        char const * objType=objectStr[i];
        char ws[20];

        strcat(reply,",");
        strcat(reply,objType);
        if (strcmp(reply,",UNK")!=0) {
          f=newTargetRA; f/=15.0; doubleToHms(ws,&f); strcat(reply,","); strcat(reply,ws);
          doubleToDms(ws,&newTargetDec,false,true); strcat(reply,","); strcat(reply,ws);
        }
        
        Lib.nextRec();

        quietReply=true;
          
      } else 

// :LWss#  Write Object Information including current target RA,Dec to next available empty record
//         If at the end of the object list (:LI# command returns an empty string "#") a new item is automatically added
//         ss is a string of up to eleven chars followed by a comma and a type designation for ex. ":LWM31 AND,GAL#"
//          Return: 0 on failure (memory full, for example)
//                  1 on success
      if (command[1]=='W') {
        
        char name[12];
        char objType[4];

        // extract object name
        int l=0;
        do {
          name[l]=0;
          if (parameter[l]==',') break;
          name[l]=parameter[l]; name[l+1]=0;
          l++;
        } while (l<12);

        // extract object type
        i=0; if (parameter[l]==',') {
          l++; int m=0;
          do {
            objType[m+1]=0;
            objType[m]=parameter[l];
            l++; m++;
          } while (parameter[l]!=0);
          // encode
          //   0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15    
          // "UNK",  "OC",  "GC",  "PN",  "DN",  "SG",  "EG",  "IG", "KNT", "SNR", "GAL",  "CN", "STR", "PLA", "CMT", "AST"
          for (l=0; l<=15; l++) { if (strcmp(objType,objectStr[l])==0) i=l; }
        }
        
        if (Lib.firstFreeRec()) Lib.writeVars(name,i,newTargetRA,newTargetDec); else commandError=true;
      } else 

// :LN#    Find next deep sky target object subject to the current constraints.
//          Returns: Nothing
      if (command[1]=='N') { 
          Lib.nextRec();
          quietReply=true;
      } else 

// :L$#    Move to catalog name record
//          Returns: 1
      if (command[1]=='$') { 
          Lib.nameRec();
          quietReply=false;
      } else 

// :LD#    Clear current record
//          Returns: Nothing
      if (command[1]=='D') { 
          Lib.clearCurrentRec();
          quietReply=true;
      } else 

// :LL#    Clear current catalog
//          Returns: Nothing
      if (command[1]=='L') { 
          Lib.clearLib();
          quietReply=true;
      } else 

// :L!#    Clear library (all catalogs)
//          Returns: Nothing
      if (command[1]=='!') { 
          Lib.clearAll();
          quietReply=true;
      } else 

// :Lonn#  Select Library catalog where nn specifies user catalog number
//         in OnStep catalog# range from 0-14. Catalogs 0-6 are user defined, the remainder are reserved.
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='o') {
        if ( (atoi2((char *)&parameter[0],&i)) && ((i>=0) && (i<=14))) {
          Lib.setCatalog(i);
        } else commandError=true;
      } else commandError=true;
        
      } else

//   M - Telescope Movement Commands
      if (command[0]=='M') {
//  :MA#   Goto the target Alt and Az
//         Returns:
//         0=Goto is Possible
//         1=Object below horizon
//         2=No object selected
//         4=Position unreachable
//         6=Outside limits
      if (command[1]=='A') { 
        i=goToHor(&newTargetAlt, &newTargetAzm);
        reply[0]=i+'0'; reply[1]=0; 
        quietReply=true; 
        supress_frame=true;
      } else 
//  :Mgdnnnn# Pulse guide command
//          Returns: Nothing
      if (command[1]=='g') {
        if ( (atoi2((char *)&parameter[1],&i)) && ((i>=0) && (i<=16399)) && (parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) { 
          if ((parameter[0]=='e') || (parameter[0]=='w')) {
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            enableGuideRate(currentPulseGuideRate);
#else
            enableGuideRate(currentGuideRate);
#endif
            guideDirAxis1=parameter[0];
            guideDurationLastHA=micros();
            guideDurationHA=(long)i*1000L;
            cli();
            guideStartTimeAxis1=millis();
            if (guideDirAxis1=='e') guideTimerRateAxis1=-guideTimerBaseRate; else guideTimerRateAxis1=guideTimerBaseRate; 
            sei();
            quietReply=true;
          } else
          if ((parameter[0]=='n') || (parameter[0]=='s')) { 
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            enableGuideRate(currentPulseGuideRate);
#else
            enableGuideRate(currentGuideRate);
#endif
            guideDirAxis2=parameter[0]; 
            guideDurationLastDec=micros();
            guideDurationDec=(long)i*1000L; 
            cli();
            guideStartTimeAxis2=millis();
            if (guideDirAxis2=='s') guideTimerRateAxis2=-guideTimerBaseRate; else guideTimerRateAxis2=guideTimerBaseRate; 
            sei();
            quietReply=true;
          } else commandError=true;
        } else commandError=true;
      } else
//  :Me# & :Mw#      Move Telescope East or West at current slew rate
//         Returns: Nothing
      if ((command[1]=='e') || (command[1]=='w')) { 
        if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
          // block user from changing direction at high rates, just stop the guide instead
          if ((guideDirAxis1) && (command[1]!=guideDirAxis1) && (fabs(guideTimerRateAxis1)>2)) { 
            guideDirAxis1='b';
          } else {
            enableGuideRate(currentGuideRate);
            guideDirAxis1=command[1];
            guideDurationHA=-1;
            cli();
            guideStartTimeAxis1=millis();
            if (guideDirAxis1=='e') guideTimerRateAxis1=-guideTimerBaseRate; else guideTimerRateAxis1=guideTimerBaseRate; 
            sei();
          }
        }
        quietReply=true;
      } else
//  :Mn# & :Ms#      Move Telescope North or South at current slew rate
//         Returns: Nothing
      if ((command[1]=='n') || (command[1]=='s')) { 
        if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
          // block user from changing direction at high rates, just stop the guide instead
          if ((guideDirAxis2) && (command[1]!=guideDirAxis2) && (fabs(guideTimerRateAxis2)>2)) { 
            guideDirAxis2='b';
          } else {
            enableGuideRate(currentGuideRate);
            guideDirAxis2=command[1];
            guideDurationDec=-1;
            cli();
            guideStartTimeAxis2=millis();
            if (guideDirAxis2=='s') guideTimerRateAxis2=-guideTimerBaseRate; else guideTimerRateAxis2=guideTimerBaseRate; 
            sei();
          }
        }
        quietReply=true;
      } else

//  :MP#   Goto the Current Position for Polar Align
//         Returns:
//         0=Goto is Possible
//         1=Object below horizon    Outside limits, below the Horizon limit
//         2=No object selected      Failure to resolve coordinates
//         4=Position unreachable    Not unparked
//         5=Busy                    Goto already active
//         6=Outside limits          Outside limits, above the Zenith limit
      if (command[1]=='P')  {
        double r,d;
        getEqu(&r,&d,false);
        GeoAlign.altCor=0.0;
        GeoAlign.azmCor=0.0;
        i=goToEqu(r,d);
        reply[0]=i+'0'; reply[1]=0;
        quietReply=true;
        supress_frame=true;
      } else

//  :MS#   Goto the Target Object
//         Returns:
//         0=Goto is Possible
//         1=Object below horizon    Outside limits, below the Horizon limit
//         2=No object selected      Failure to resolve coordinates
//         4=Position unreachable    Not unparked
//         5=Busy                    Goto already active
//         6=Outside limits          Outside limits, above the Zenith limit
      if (command[1]=='S')  {
        i=goToEqu(newTargetRA,newTargetDec);
        reply[0]=i+'0'; reply[1]=0;
        quietReply=true;
        supress_frame=true; 
      } else commandError=true;
      
      } else
//   $Q - PEC Control
//  :$QZ+  Enable RA PEC compensation 
//         Returns: nothing
//  :$QZ-  Disable RA PEC Compensation
//         Returns: nothing
//  :$QZZ  Clear the PEC data buffer
//         Return: Nothing
//  :$QZ/  Ready Record PEC
//         Returns: nothing
//  :$QZ!  Write PEC data to EEPROM
//         Returns: nothing
//  :$QZ?  Get PEC status
//         Returns: S#
      if ((command[0]=='$') && (command[1]=='Q')) {
        if ((parameter[2]==0) && (parameter[0]=='Z')) {
          quietReply=true; 
#ifndef MOUNT_TYPE_ALTAZM
          if (parameter[1]=='+') { if (pecRecorded) pecStatus=ReadyPlayPEC; EEPROM.update(EE_pecStatus,pecStatus); } else
          if (parameter[1]=='-') { pecStatus=IgnorePEC; EEPROM.update(EE_pecStatus,pecStatus); } else
          if ((parameter[1]=='/') && (trackingState==TrackingSidereal)) { pecStatus=ReadyRecordPEC; EEPROM.update(EE_pecStatus,IgnorePEC); } else
          if (parameter[1]=='Z') { 
            for (i=0; i<PECBufferSize; i++) pecBuffer[i]=128;
            pecFirstRecord = true;
            pecStatus      = IgnorePEC;
            pecRecorded    = false;
            EEPROM.update(EE_pecStatus,pecStatus);
            EEPROM.update(EE_pecRecorded,pecRecorded);
          } else
          if (parameter[1]=='!') {
            pecRecorded=true;
            EEPROM.update(EE_pecRecorded,pecRecorded);
            EEPROM_writeLong(EE_wormSensePos,wormSensePos);
            // trigger recording of PEC buffer
            pecAutoRecord=PECBufferSize;
          } else
#endif
          // Status is one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording.  Or an optional (.) to indicate an index detect.
          if (parameter[1]=='?') { const char *pecStatusCh = PECStatusString; reply[0]=pecStatusCh[pecStatus]; reply[1]=0; reply[2]=0; if (wormSensedAgain) { reply[1]='.'; wormSensedAgain=false; } } else { quietReply=false; commandError=true; }
        } else commandError=true;
      } else

//   Q - Movement Commands
//  :Q#    Halt all slews, stops goto
//         Returns: Nothing
      if (command[0]=='Q') {
        if (command[1]==0) {
          if ((parkStatus==NotParked) || (parkStatus==Parking)) {
            cli();
            if (guideDirAxis1) guideDirAxis1='b'; // break
            if (guideDirAxis2) guideDirAxis2='b'; // break
            guideBreakTimeAxis1=millis();
            guideBreakTimeAxis2=millis();
            sei();
            if (trackingState==TrackingMoveTo) { abortSlew=true; }
          }
          quietReply=true; 
        } else
//  :Qe# & Qw#   Halt east/westward Slews
//         Returns: Nothing
        if ((command[1]=='e') || (command[1]=='w')) { 
          if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
            cli();
            if (guideDirAxis1) guideDirAxis1='b'; // break
            guideBreakTimeAxis1=millis();
            sei();
          }
          quietReply=true; 
        } else
//  :Qn# & Qs#   Halt north/southward Slews
//         Returns: Nothing
        if ((command[1]=='n') || (command[1]=='s')) {
          if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
            cli();
            if (guideDirAxis2) guideDirAxis2='b'; // break
            guideBreakTimeAxis2=millis();
            sei();
          }
          quietReply=true; 
        } else commandError=true;
      } else

//   R - Slew Rate Commands
      if (command[0]=='R') {
//  :RG#   Set Slew rate to Guiding Rate (slowest) 1X
//  :RC#   Set Slew rate to Centering rate (2nd slowest) 8X
//  :RM#   Set Slew rate to Find Rate (2nd Fastest) 24X
//  :RS#   Set Slew rate to max (fastest) ?X (1/2 of MaxRate)
//  :Rn#   Set Slew rate to n, where n=0..9
//         Returns: Nothing
      if ((command[1]=='G') || (command[1]=='C') || (command[1]=='M') || (command[1]=='S') || ((command[1]>='0') && (command[1]<='9'))) {
        if (command[1]=='G') i=2; else
        if (command[1]=='C') i=5; else
        if (command[1]=='M') i=6; else
        if (command[1]=='S') i=8; else i=command[1]-'0';
        setGuideRate(i);
        quietReply=true; 
      } else commandError=true;
     } else

//   S - Telescope Set Commands
      if (command[0]=='S') {
//  :SasDD*MM#
//         Set target object altitude to sDD*MM# or sDD*MM'SS# (based on precision setting)
//         Returns:
//         0 if Object is within slew range, 1 otherwise
      if (command[1]=='a')  { if (!dmsToDouble(&newTargetAlt,parameter,true)) commandError=true; } else 
//  :SBn#  Set Baud Rate n for Serial-0, where n is an ASCII digit (1..9) with the following interpertation
//         0=115.2K, 1=56.7K, 2=38.4K, 3=28.8K, 4=19.2K, 5=14.4K, 6=9600, 7=4800, 8=2400, 9=1200
//         Returns: "1" At the current baud rate and then changes to the new rate for further communication
      if (command[1]=='B') {
        i=(int)(parameter[0]-'0');
        if ((i>=0) && (i<10)) {
          if (process_command==COMMAND_SERIAL) {
            PSerial.print("1"); while (PSerial.transmit()); delay(20); PSerial.begin(baudRate[i]);
          } else if (process_command==COMMAND_ETHERNET) {
#ifdef ETHERNET_ON
             Ethernet_print("1"); while (Ethernet_transmit()); delay(20);
#endif
          } else  {
            PSerial1.print("1"); while (PSerial1.transmit()); delay(20); PSerial1.begin(baudRate[i]); 
          }
          quietReply=true; 
        } else commandError=true;
      } else
//  :SCMM/DD/YY#
//          Change Date to MM/DD/YY
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='C')  { if (!dateToDouble(&JD,parameter)) commandError=true; else { EEPROM_writeFloat(EE_JD,JD); update_lst(jd2last(JD,UT1)); } } else 
//  :SdsDD*MM#
//          Set target object declination to sDD*MM or sDD*MM:SS depending on the current precision setting
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='d')  { if (!dmsToDouble(&newTargetDec,parameter,true)) commandError=true; } else 
//  :SgsDDD*MM# or :SgDDD*MM#
//          Set current sites longitude to sDDD*MM an ASCII position string, East longitudes can be as negative or >180 degrees
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='g')  {
        i=highPrecision;
        highPrecision=false;
        if ((parameter[0] == '-') || (parameter[0] == '+')) i1=1; else i1=0;
        if (!dmsToDouble(&longitude,(char *)&parameter[i1],false))
          commandError=true;
        else {
          if (parameter[0]=='-') longitude=-longitude;
          EEPROM_writeFloat(EE_sites+(currentSite)*25+4,longitude);
        }
        update_lst(jd2last(JD,UT1));
        highPrecision=i;
        } else
//  :SGsHH#
//  :SGsHH:MM# (where MM is 30 or 45)
//          Set the number of hours added to local time to yield UTC
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='G')  { 
        if (strlen(parameter)<7) {
          double f=0.0;
          char *temp=strchr(parameter,':');
          if (temp) {
            if ((temp[0]==':') && (temp[1]=='4') && temp[2]=='5') { temp[0]=0; f=0.75; } else
            if ((temp[0]==':') && (temp[1]=='3') && temp[2]=='0') { temp[0]=0; f=0.5; } else 
            if ((temp[0]==':') && (temp[1]=='0') && temp[2]=='0') { temp[0]=0; f=0.0; } else { i=-999; } // force error if not :00 or :30 or :45
          }

          if ( (atoi2(parameter,&i)) && ((i>=-24) && (i<=24))) {
            if (i<0) timeZone=i-f; else timeZone=i+f;
            b=encodeTimeZone(timeZone)+128;
            EEPROM.update(EE_sites+(currentSite)*25+8,b);
            update_lst(jd2last(JD,UT1));
          } else commandError=true; 
        } else commandError=true; 
      } else
//  :Sh+DD#
//          Set the lowest elevation to which the telescope will goTo
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='h')  { if ((parameter[0]!=0) && (strlen(parameter)<4)) {
        if ( (atoi2(parameter,&i)) && ((i>=-30) && (i<=30))) { minAlt=i; EEPROM.update(EE_minAlt,minAlt+128); } else commandError=true; 
      } else commandError=true; } else
//  :SLHH:MM:SS#
//          Set the local Time
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='L')  {  
        i=highPrecision; highPrecision=true; 
        if (!hmsToDouble(&LMT,parameter)) commandError=true; else {
          EEPROM_writeFloat(EE_LMT,LMT); 
          UT1=LMT+timeZone; 
          UT1_start=UT1;
          update_lst(jd2last(JD,UT1));
        }
        highPrecision=i;
      } else 
//  :SM<string>#
//  :SN<string>#
//  :SO<string>#
//  :SP<string>#
//          Set site name to be <string>, up to 15 characters.
//          Return: 0 on failure
//                  1 on success
      if ((command[1]=='M') || (command[1]=='N') || (command[1]=='O') || (command[1]=='P')) {
        i=command[1]-'M';
        if (strlen(parameter)>15) commandError=true; else EEPROM_writeString(EE_sites+i*25+9,parameter);
      } else
//  :SoDD#
//          Set the overhead elevation limit to DD#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='o')  { if ((parameter[0]!=0) && (strlen(parameter)<3)) { 
        if ( (atoi2(parameter,&i)) && ((i>=60) && (i<=90))) { 
          maxAlt=i;
#ifdef MOUNT_TYPE_ALTAZM
          if (maxAlt>87) maxAlt=87;
#endif
          EEPROM.update(EE_maxAlt,maxAlt); 
        } else commandError=true; 
      } else commandError=true; } else
//  :SrHH:MM.T#
//  :SrHH:MM:SS#
//          Set target object RA to HH:MM.T or HH:MM:SS (based on precision setting)
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='r')  { if (!hmsToDouble(&newTargetRA,parameter)) commandError=true; else newTargetRA*=15.0; } else 
//  :SSHH:MM:SS#
//          Sets the local (apparent) sideral time to HH:MM:SS
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; if (!hmsToDouble(&f,parameter)) commandError=true; else update_lst(f); highPrecision=i; } else 
//  :StsDD*MM#
//          Sets the current site latitude to sDD*MM#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='t')  { 
        i=highPrecision; highPrecision=false; 
        if (!dmsToDouble(&latitude,parameter,true)) { commandError=true; } else {
          EEPROM_writeFloat(100+(currentSite)*25+0,latitude);
#ifdef MOUNT_TYPE_ALTAZM
          celestialPoleAxis2=AltAzmDecStartPos;
          if (latitude<0) celestialPoleAxis1=180L; else celestialPoleAxis1=0L;
#else
          if (latitude<0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (latitude>0.0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
        }
        highPrecision=i;
      } else 
//  :STdd.ddddd#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='T')  { 
        if ((trackingState==TrackingSidereal) || (trackingState==TrackingNone)) {
          f=strtod(parameter,&conv_end);
          if ( (&parameter[0]!=conv_end) && (((f>=30.0) && (f<90.0)) || (abs(f)<0.1))) {
            if (abs(f)<0.1) { 
              trackingState = TrackingNone; 
            } else {
              SetTrackingRate((f/60.0)/1.00273790935);
            }
          } else commandError=true;
        } else commandError=true;
      } else
//  :SXnn,VVVVVV...#   Set OnStep value
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='X')  { 
        if (parameter[0]=='0') { // 0n: Align Model
          switch (parameter[1]) {
            case '0': indexAxis1=(double)strtol(&parameter[3],NULL,10)/3600.0; break;        // indexAxis1
            case '1': indexAxis2=(double)strtol(&parameter[3],NULL,10)/3600.0; break;        // indexAxis2
            case '2': GeoAlign.altCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;   // altCor
            case '3': GeoAlign.azmCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;   // azmCor
            case '4': GeoAlign.doCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // doCor
            case '5': GeoAlign.pdCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // pdCor
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
            case '6': GeoAlign.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // ffCor
            case '7': break;                                                                 // dfCor
#else
            case '6': break;                                                                 // ffCor
            case '7': GeoAlign.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // dfCor
#endif
            case '8': GeoAlign.tfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // tfCor
          }
        } else
        if (parameter[0]=='9') { // 9n: Misc.
          switch (parameter[1]) {
            case '2': // set new acceleration rate
              maxRate=strtol(&parameter[3],NULL,10)*16L;
              if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
              if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
              EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L));
              SetAccelerationRates(maxRate);
            break;
            case '3': // acceleration rate preset
              quietReply=true;
              switch (parameter[3]) {
                case '5': maxRate=MaxRate*32L; break; // 50%
                case '4': maxRate=MaxRate*24L; break; // 75%
                case '3': maxRate=MaxRate*16L; break; // 100%
                case '2': maxRate=MaxRate*12L; break; // 150%
                case '1': maxRate=MaxRate*8L;  break; // 200%
              break;
              }
              SetAccelerationRates(maxRate);
            break;
            case '5': // autoContinue
              if ((parameter[3]=='0') || (parameter[3]=='1')) { i=parameter[3]-'0'; if ((i==0) || (i==1)) { autoContinue=i;  EEPROM.write(EE_autoContinue,autoContinue); } } 
            break; 
            case '6': // preferred pier side 
              switch (parameter[3]) {
                case 'E': preferredPierSide=PPS_EAST; break;
                case 'W': preferredPierSide=PPS_WEST; break;
                case 'B': preferredPierSide=PPS_BEST; break;
                default: commandError=true;
              }
            break;
          }
        } else commandError=true;
        //getEqu(&f,&f1,false);  
      } else 
//  :SzDDD*MM#
//          Sets the target Object Azimuth
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='z')  { if (!dmsToDouble(&newTargetAzm,parameter,false)) commandError=true; } else
      commandError=true;
      } else 
//   T - Tracking Commands
//  :T+#   Master sidereal clock faster by 0.1 Hertz (I use a fifth of the LX200 standard, stored in EEPROM)
//  :T-#   Master sidereal clock slower by 0.1 Hertz (stored in EEPROM)
//  :TS#   Track rate solar
//  :TL#   Track rate lunar
//  :TQ#   Track rate sidereal
//  :TR#   Master sidereal clock reset (to calculated sidereal rate, stored in EEPROM)
//  :TK#   Track rate king
//  :Te#   Tracking enable  (OnStep only, replies 0/1)
//  :Td#   Tracking disable (OnStep only, replies 0/1)
//  :To#   OnTrack enable   (OnStep only, replies 0/1)
//  :Tr#   Track refraction enable  (OnStep only, replies 0/1)
//  :Tn#   Track refraction disable (OnStep only, replies 0/1)
//         Returns: Nothing

     if (command[0]=='T') {
       if (command[1]=='+') { siderealInterval-=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='-') { siderealInterval+=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='S') { SetTrackingRate(0.99726956632); refraction=false; quietReply=true; } else      // solar tracking rate 60Hz
       if (command[1]=='L') { SetTrackingRate(0.96236513150); refraction=false; quietReply=true; } else      // lunar tracking rate 57.9Hz
       if (command[1]=='Q') { SetTrackingRate(default_tracking_rate); quietReply=true; } else                // sidereal tracking rate
       if (command[1]=='R') { siderealInterval=15956313L; quietReply=true; } else                            // reset master sidereal clock interval
       if (command[1]=='K') { SetTrackingRate(0.99953004401); refraction=false; quietReply=true; } else      // king tracking rate 60.136Hz
       if ((command[1]=='e') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone)) && (parkStatus==NotParked)) trackingState=TrackingSidereal; else
       if ((command[1]=='d') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone))) trackingState=TrackingNone; else
       if (command[1]=='o') { refraction=refraction_enable; onTrack=true;  SetTrackingRate(default_tracking_rate); } else  // turn full compensation on, defaults to base sidereal tracking rate
       if (command[1]=='r') { refraction=refraction_enable; onTrack=false; SetTrackingRate(default_tracking_rate); } else  // turn refraction compensation on, defaults to base sidereal tracking rate
       if (command[1]=='n') { refraction=false; onTrack=false; SetTrackingRate(default_tracking_rate); } else              // turn refraction off, sidereal tracking rate resumes
         commandError=true;

       // Only burn the new rate if changing the sidereal interval
       if ((!commandError) && ((command[1]=='+') || (command[1]=='-') || (command[1]=='R'))) {
         EEPROM_writeLong(EE_siderealInterval,siderealInterval);
         SetSiderealClockRate(siderealInterval);
         cli(); SiderealRate=siderealInterval/StepsPerSecondAxis1; sei();
       }

     } else
     
//   U - Precision Toggle
//  :U#    Toggle between low/hi precision positions
//         Low -  RA/Dec/etc. displays and accepts HH:MM.M sDD*MM
//         High - RA/Dec/etc. displays and accepts HH:MM:SS sDD*MM:SS
//         Returns Nothing
     if ((command[0]=='U') && (command[1]==0)) { highPrecision=!highPrecision; quietReply=true; } else

#ifndef MOUNT_TYPE_ALTAZM
//   V - PEC Readout
//  :VRNNNN#
//         Read out RA PEC Table Entry
//         Returns: sDDD#
//         Rate Adjustment factor for worm segment NNNN. PecRate = Steps +/- for this 1 second segment
//         If NNNN is omitted, returns the currently playing segment
     if ((command[0]=='V') && (command[1]=='R')) {
       boolean conv_result=true;
       if (parameter[0]==0) { i=pecIndex1; } else conv_result=atoi2(parameter,&i);
       if ((conv_result) && ((i>=0) && (i<PECBufferSize))) {
         if (parameter[0]==0) {
           i-=1; if (i<0) i+=SecondsPerWormRotationAxis1; if (i>=SecondsPerWormRotationAxis1) i-=SecondsPerWormRotationAxis1;
           i1=pecBuffer[i]-128; sprintf(reply,"%+04i,%03i",i1,i);
         } else {
           i1=pecBuffer[i]-128; sprintf(reply,"%+04i",i1);
         }
       } else commandError=true;
       quietReply=true;
     } else
//  :VrNNNN#
//         Read out RA PEC ten byte frame in hex format starting at worm segment NNNN
//         Returns: x0x1x2x3x4x5x6x7x8x9#
//         Rate Adjustment factor for worm segments. PecRate = Steps +/- for each 1 second segment, hex one byte integer (PecRate=b-128)
//         leave a delay of about 10ms between calls
     if ((command[0]=='V') && (command[1]=='r')) {
       if ( (atoi2(parameter,&i)) && ((i>=0) && (i<PECBufferSize))) {
         int j=0;
         byte b;
         char x[3]="  ";
         for (j=0; j<10; j++) {
           if (i+j<PECBufferSize) b=pecBuffer[i+j]; else b=128;
           
           sprintf(x,"%02X",b);
           strcat(reply,x);
         }
       } else commandError=true;
       quietReply=true;
     } else
//   V - PEC Readout StepsPerWormRotationAxis1
//  :VW#
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='W')) {
       if (parameter[0]==0) {
         sprintf(reply,"%06ld",(long)StepsPerWormRotationAxis1);
         quietReply=true;
       } else commandError=true;
     } else
//   V - PEC Readout StepsPerSecondAxis1
//  :VS#
//         Returns: DDD.DDDDDD#
     if ((command[0]=='V') && (command[1]=='S')) {
       if (parameter[0]==0) {
         char temp[12];
         dtostrf(StepsPerSecondAxis1,0,6,temp);
         strcpy(reply,temp);
//         sprintf(reply,"%03i",StepsPerSecondAxis1);
         quietReply=true;
       } else commandError=true;
     } else
//  :VH#
//         Read RA PEC sense index (seconds)
//         Returns: DDDDD#
     if ((command[0]=='V') && (command[1]=='H')) {
       long s=(long)((double)wormSensePos/(double)StepsPerSecondAxis1);
       while (s>SecondsPerWormRotationAxis1) s-=SecondsPerWormRotationAxis1;
       while (s<0) s+=SecondsPerWormRotationAxis1;
       sprintf(reply,"%05ld",s); // indexWorm_sense
       quietReply=true;
     } else
//  :VI#
//         Read RA PEC record index start (steps)
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='I')) {
       sprintf(reply,"%06ld",0L);  // indexWorm_record
       quietReply=true;
     } else

//  :WIDDDDDD#
//         Write RA PEC index start (steps)
     if ((command[0]=='W') && (command[1]=='I')) {
       commandError=true;
       long l=strtol(parameter,&conv_end,10);
       // see if it converted and is in range
       if ( (&parameter[0]!=conv_end) && ((l>=0) && (l<(long)StepsPerWormRotationAxis1))) {
// ignored
         commandError=false;
       }
    } else
//  :WR+#
//         Move PEC Table ahead by one second
//  :WR-#
//         Move PEC Table back by one second
//  :WRNNNN,sDDD#
//         Write RA PEC Table Entry
//         Returns: Nothing
//         Rate Adjustment factor for worm segment NNNN. PecRate = Steps +/- for this 1 second segment
     if ((command[0]=='W') && (command[1]=='R')) { 
       commandError=true;
       if (parameter[1]==0) {
         if (parameter[0]=='+') {
           i=pecBuffer[SecondsPerWormRotationAxis1-1];
           memmove((byte *)&pecBuffer[1],(byte *)&pecBuffer[0],SecondsPerWormRotationAxis1-1);
           pecBuffer[0]=i;
           commandError=false;
         } else
         if (parameter[0]=='-') {
           i=pecBuffer[0];
           memmove((byte *)&pecBuffer[0],(byte *)&pecBuffer[1],SecondsPerWormRotationAxis1-1);
           pecBuffer[SecondsPerWormRotationAxis1-1]=i;
           pecRecorded=true;
           commandError=false;
         }
       } else {
         // it should be an int
         // see if it converted and is in range
         if ( (atoi2(parameter,&i)) && ((i>=0) && (i<PECBufferSize))) {
           // should be another int here
           // see if it converted and is in range
           if ( (atoi2((char*)&parameter[5],&i2)) && ((i2>=-128) && (i2<=127))) {
             pecBuffer[i]=i2+128;
             pecRecorded =true;
             commandError=false;
           }
         }
         quietReply=true;
       }
     } else
#endif

//   W - Site Select/Site get
//  :Wn#
//         Sets current site to n, 0..3 or queries site with ?
//         Returns: Nothing or current site ?#
      if (command[0]=='W') {
        if ((command[1]>='0') && (command[1]<='3')) {
          currentSite=command[1]-'0'; EEPROM.update(EE_currentSite,currentSite); quietReply=true;
          latitude=EEPROM_readFloat(EE_sites+(currentSite*25+0));
#ifdef MOUNT_TYPE_ALTAZM
          celestialPoleAxis2=AltAzmDecStartPos;
#else
          if (latitude<0.0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (latitude>0.0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
          longitude=EEPROM_readFloat(EE_sites+(currentSite*25+4));
          timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
          timeZone=decodeTimeZone(timeZone);
        } else 
        if (command[1]=='?') {
          quietReply=true;
          sprintf(reply,"%i",currentSite);
        } else
          commandError=true;
      } else 
        commandError=true;
      
      if (!quietReply) {
        if (commandError) reply[0]='0'; else reply[0]='1';
        reply[1]=0;
        supress_frame=true;
      }
      
      if (strlen(reply)>0) {
        if (process_command==COMMAND_SERIAL) {
          if (cmd.checksum) checksum(reply);
          if (!supress_frame) strcat(reply,"#");
          PSerial.print(reply);
        } 
  
        if (process_command==COMMAND_SERIAL1) {
          if (cmd1.checksum) checksum(reply);
          if (!supress_frame) strcat(reply,"#");
          PSerial1.print(reply);
        }
  
#ifdef ETHERNET_ON
        if (process_command==COMMAND_ETHERNET) {
          if (cmde.checksum) checksum(reply);
          if (!supress_frame) strcat(reply,"#");
          Ethernet_print(reply);
        }
#endif       
      }
      quietReply=false;
   }
}

// calculates the tracking speed for move commands
void setGuideRate(int g) {
  currentGuideRate=g;
  if ((g<=GuideRate1x) && (currentPulseGuideRate!=g)) { currentPulseGuideRate=g; EEPROM.update(EE_pulseGuideRate,g); }
}

void enableGuideRate(int g) {
  // don't do these lengthy calculations unless we have to
  if (activeGuideRate==g) return;
  
  activeGuideRate=g;

  // this enables the guide rate
  guideTimerBaseRate=(double)(guideRates[g]/15.0);

  cli();
  amountGuideHA.fixed =doubleToFixed((guideTimerBaseRate*StepsPerSecondAxis1)/100.0);
  amountGuideDec.fixed=doubleToFixed((guideTimerBaseRate*StepsPerSecondAxis2)/100.0);
  sei();
}

// calculate the checksum and add to string
void checksum(char s[]) {
  char HEXS[3]="";
  byte cks=0; for (unsigned int cksCount0=0; cksCount0<strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
  sprintf(HEXS,"%02X",cks);
  strcat(s,HEXS);
}

