// -----------------------------------------------------------------------------------
// Command processing

boolean serial_zero_ready = false;
boolean serial_one_ready = false;

// process commands
void processCommands() {
    if ((Serial.available() > 0) && (!serial_zero_ready)) { serial_zero_ready = buildCommand(Serial.read()); }
    #ifdef MEGA2560_ON
    if ((Serial1.available() > 0) && (!serial_one_ready)) { serial_one_ready = buildCommand_serial_one(Serial1.read()); }
    #endif

    
    byte process_command = 0;
    if (serial_zero_ready)     { strcpy(command,command_serial_zero); strcpy(parameter,parameter_serial_zero); serial_zero_ready=false; clearCommand_serial_zero(); process_command=1; } 
    #ifdef MEGA2560_ON
    else if (serial_one_ready) { strcpy(command,command_serial_one);  strcpy(parameter,parameter_serial_one);  serial_one_ready=false;  clearCommand_serial_one();  process_command=2; }
    #endif
    else return;

    if (process_command) {
// Command is two chars followed by an optional parameter...
      commandError=false;
// Handles empty and one char replies
      reply[0]=0; reply[1]=0;

//   A - Alignment Commands
      if (command[0]=='A') {
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
      if ((command[1]=='1') || (command[1]=='2') || (command[1]=='3')) {
        // set current time and date before calling this routine

        // telescope should be set in the polar home (CWD) for a starting point
        // this command sets IH, ID, azmCor=0; altCor=0;
        setHome();

        // newTargetRA =timeRange(LST);
        // newTargetDec=90.0;  

        // start tracking
        trackingState=TrackingSidereal;

        // start align... AlignOneStar1=01, AlignTwoStar1=11, AlignThreeStar1=21
        alignMode=AlignOneStar1+(command[1]-'1')*10;

        commandError=false;
      } else
//  :A+#  Manual Alignment, set target location
//         Returns:
//         1: If correction is accepted
//         0: Failure, Manual align mode not set or distance too far 
        if (command[1]=='+') { 
          if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) {
            alignMode++;
            // Move to star near (Zenith(W) or, for one-star near where you're working) 
            //                   (PAlt~Dec ,PAzm~None)
            // set the IH offset
            // set the ID offset
            if (!syncEqu(newTargetRA,newTargetDec)) { commandError=true; }
            
          } else 
#ifdef ALIGN_TWO_AND_THREE_STAR_ON
          if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2)) {
            alignMode++;
            // Move to star near Zenith(E)  (PAlt~Dec ,PAzm~None)
            // refine the IH offset
            // get the ID2 offset           ID1 =ID
            // set the ID offset            ID  =(ID1-ID2)/2
            // set the PAlt offset          PAlt=ID-ID1
     
            // calculate the Alt1 and Azm1 for the target object
            // calculate the Alt2 and Azm2 for the current position
            // AltCorr=Alt1-Alt2 is the polar offset in Alt  (up is +)
            // EQU->HOR->Add this to Latitude->HOR->EQU conversion
            
            float ID1 = ID;
            float IH1 = IH;
            
            if (syncEqu(newTargetRA,newTargetDec)) {
              float ID2=ID;
              IH  =(IH1+IH)/2.0;
              ID  =(ID1-ID2)/2.0;   // West of pier add to actual Dec to get corrected Dec (subtract when East)
              altCor=-(ID-ID1);     // Negative when pointed below the pole, add to Latitude to go from actual HA/Dec to corrected HA/Dec
            } else commandError=true;
          } else 
          if (alignMode==AlignThreeStar3) {
            alignMode++;
            
            // Get hour angle of the actual RA
            f=LST-newTargetRA;
            while (f>+12.0) f=f-24.0;
            while (f<-12.0) f=f+24.0;
    
            // f/f1 are the actual Alt/Azm
            EquToHor(latitude,f,newTargetDec,&f,&f1);
            // Now for where we think we are
            cli(); long t=targetHA; long t1=targetDec; sei();
            
            f2=(t/(double)StepsPerDegreeHA)/15.0;
            f3=t1/(double)StepsPerDegreeDec;
            // corrected to actual HA/Dec (for offset HA/Dec and Alt)
            CEquToEqu(latitude,f2,f3,&f2,&f3);
            // f2/f3 are the Alt/Azm of where we think we are
            EquToHor(latitude,f2,f3,&f2,&f3);
            // Polar offset in azimuth, degrees ie. at 180, measures 185, azmCor=+5
            azmCor=(f3-f1); 
  
          } else
#endif
          commandError=true; 
        } else commandError=true; 
      } else
//   $B - Backlash Compensation
//  :$BDdd# Set Altitude/Dec Antibacklash
//          Return: 0 on failure
//                  1 on success
//  :$BRdd# Set Azimuth/RA Antibacklash
//          Return: Nothing
//         Set the Backlash values.  Units are arc-seconds
//         For HA  when slewing east takes this many additional steps
//         For Dec when slewing takes this many additional steps
//         for this reason, calls to the GetEqu command need to have a correction applied to remove
//         this effect.
//          Return: 0 on failure
//                  1 on success
      if ((command[0]=='$') && (command[1]=='B')) {
        if ((strlen(parameter)>1) && (strlen(parameter)<5)) {
          i=atoi((char*)&parameter[1]);
          if ( (!errno) && ((i>=0) && (i<=999))) { 
            if (parameter[0]=='D') {
              backlashDec=(i*StepsPerDegreeDec)/3600;
              EEPROM_writeInt(EE_backlashDec,backlashDec);
            } else
            if (parameter[0]=='R') {
              backlashHA =(i*StepsPerDegreeHA)/3600;
              EEPROM_writeInt(EE_backlashHA,backlashHA);
            } else commandError=true;
          } else commandError=true;
        } else commandError=true;
      } else
      
//   C - Sync Control
//  :CS#   Synchonize the telescope with the current right ascension and declination coordinates
//         Returns: Nothing
      if ((command[0]=='C') && (command[1]=='S'))  {
        if (trackingState!=TrackingMoveTo) {
          syncEqu(newTargetRA,newTargetDec);
          quietReply=true;
        }
      } else 

      
//   D - Distance Bars
//  :D#    returns an "\0x7f#" if the mount is moving, otherwise returns "#".
      if ((command[0]=='D') && (command[1]==0))  { if (trackingState==TrackingMoveTo) { reply[0]='\0x7f'; reply[1]=0; } else { reply[0]=0; } quietReply=true; } else 
 
//   G - Get Telescope Information
      if (command[0]=='G') {

//  :GA#   Get Telescope Altitude
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
//         The current scope altitude
      if (command[1]=='A')  { getHor(&f,&f1); if (!doubleToDms(reply,&f,false,true)) commandError=true; else quietReply=true; } else
//  :GC#   Get the current date
//         Returns: MM/DD/YY#
//         The current local calendar date
      if (command[1]=='C') { 
        LMT=UT1-timeZone;
        greg(JD,&i2,&i,&i1); 
        i2=(i2/99.99999-floor(i2/99.99999))*100;

        // correct for day moving forward/backward... this works for multipule days of up-time, but eventually
        // (after several days) the time keeping will loose precision as the larger values start to consume the significant digits
        while (LMT>=24.0) { LMT=LMT-24.0; i1--; } 
        if    (LMT<0.0)   { LMT=LMT+24.0; i1++; }
        sprintf(reply,"%02d/%02d/%02d",i,i1,i2); 
        quietReply=true; 
      } else 
//  :GD#   Get Telescope Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
      if (command[1]=='D')  { getEqu(&f,&f1,true); if (!doubleToDms(reply,&f1,false,true)) commandError=true; else quietReply=true; } else 
//  :Gd#   Get Currently Selected Target Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
      if (command[1]=='d')  { if (!doubleToDms(reply,&newTargetDec,false,true)) commandError=true; else quietReply=true; } else 
//  :GG#   Get UTC offset time
//         Returns: sHH#
//         The number of decimal hours to add to local time to convert it to UTC
      if (command[1]=='G')  { sprintf(reply,"%+03d",timeZone); quietReply=true; } else 
//  :Gg#   Get Current Site Longitude
//         Returns: sDDD*MM#
//         The current site Longitude. East Longitudes are negative
      if (command[1]=='g')  { i=highPrecision; highPrecision=false; if (!doubleToDms(reply,&longitude,true,true)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :Gh#   Get Horizon Limit
//         Returns: sDD*#
//         The minimum elevation of an object above the horizon required for a mount goto
      if (command[1]=='h')  { sprintf(reply,"%+02d*",minAlt); quietReply=true; } else
//  :GH#   Get Telescope HA
//         Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
      if (command[1]=='H')  { getEqu(&f,&f1,true); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true; } else 
//  :GL#   Get Local Time in 24 hour format
//         Returns: HH:MM:SS#
      if (command[1]=='L')  { i=highPrecision; highPrecision=true; LMT=timeRange(UT1-timeZone); if (!doubleToHms(reply,&LMT)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GM#   Get Site 1 Name
//  :GN#   Get Site 2 Name
//  :GO#   Get Site 3 Name
//  :GP#   Get Site 4 Name
//         Returns: <string>#
//         A # terminated string with the name of the requested site.
      if ((command[1]=='M') || (command[1]=='N') || (command[1]=='O') || (command[1]=='P'))  { 
        i=(byte)command[1]-(byte)'L';
        EEPROM_readString(EE_sites+i*25+9,reply); quietReply=true; 
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
      if (command[1]=='R')  { getEqu(&f,&f1,false); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true; } else 
//  :Gr#   Get current/target object RA
//         Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
      if (command[1]=='r')  { if (!doubleToHms(reply,&newTargetRA)) commandError=true; else quietReply=true; } else 
//  :GS#   Get the Sidereal Time
//         Returns: HH:MM:SS#
//         The Sidereal Time as an ASCII Sexidecimal value in 24 hour format
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; if (!doubleToHms(reply,&LST)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GT#   Get tracking rate
//         Returns: TT#
//         Returns "60#" if the telescope is tracking at the sidereal rate, "0#" otherwise
      if (command[1]=='T')  { if (trackingState==TrackingSidereal) { reply[0]='6'; reply[1]='0'; reply[2]=0; } else { reply[0]='0'; reply[1]=0; } quietReply=true; } else 
//  :Gt#   Get Current Site Latitude
//         Returns: sDD*MM#
//         The latitude of the current site. Positive for North latitudes
      if (command[1]=='t')  { i=highPrecision; highPrecision=false; if (!doubleToDms(reply,&latitude,false,true)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GU#   Get telescope Status
//         Returns: SS#
      if (command[1]=='U')  { 
        i=0;
        if (trackingState!=TrackingMoveTo)     reply[i++]='N';
        char *parkStatusCh = "pIPF";           reply[i++]=parkStatusCh[parkStatus]; // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (PECrecorded)                       reply[i++]='R';
        if (atHome)                            reply[i++]='H'; 
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
#ifdef MEGA2560_ON
//  :GXnn#   Get OnStep value
//         Returns: value
      if (command[1]=='X')  { 
        if (parameter[2]==(char)0) {
          if (parameter[0]=='0') {
            if (parameter[1]=='0') { // ID, Dec offset
              if (!doubleToDms(reply,&ID,false,true)) commandError=true; else quietReply=true;
            } else commandError=true;
            if (parameter[1]=='1') { // IH, HA offset
              if (!doubleToHms(reply,&IH)) commandError=true; else quietReply=true;
            } else commandError=true;
          } else commandError=true;
        } else commandError=true;
      getEqu(&f,&f1,false);  
    } else 
#endif
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
      if (command[1]=='C')  { goHome(); quietReply=true; } else 
//  :hP#   Goto the Park Position
//          Return: 0 on failure
//                  1 on success
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

//   M - Telescope Movement Commands
      if (command[0]=='M') {
#ifdef ALT_AZM_GOTO_ON
//  :MA#   Goto the target Alt and Az
//         Returns:
//         0#=Goto is Possible
//         1#=Object below horizon
//         2#=No object selected
//         4#=Position unreachable
//         6#=Outside limits
      if (command[1]=='A') { 
        quietReply=true; 
        i=goToHor(&newTargetAlt, &newTargetAzm);
        reply[0]=i+'0'; reply[1]=0;
      } else 
#endif
//  :Mgdnnnn# Pulse guide command
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='g') {
        i=atoi((char *)&parameter[1]); 
        if ( (!errno) && ((i>=0) && (i<=16399)) && (parkStatus==NotParked)) { 
          if ((parameter[0]=='e') || (parameter[0]=='w')) { 
            if (moveDirHA) { cli(); skipCountHA=lastSkipCountHA; sei(); }
            moveDirHA=parameter[0];  moveDurationHA=i;  cli(); lastSkipCountHA=skipCountHA; skipCountHA=moveSkipCountHA; sei();
          } else
            if ((parameter[0]=='n') || (parameter[0]=='s')) { 
              if (moveDirDec) { cli(); skipCountDec=lastSkipCountDec; sei(); }
              moveDirDec=parameter[0]; 
              moveDurationDec=i; cli(); 
              lastSkipCountDec=skipCountDec; 
 #ifdef DEC_RATIO_ON
             skipCountDec=moveSkipCountDec*SkipCountRateRatio;
 #else
             skipCountDec=moveSkipCountDec;
 #endif
              sei();
            } else commandError=true;
        } else commandError=true;
      } else
//  :Me# & :Mw#      Move Telescope East or West at current slew rate
//         Returns: Nothing
      if ((command[1]=='e') || (command[1]=='w')) { 
        if (parkStatus==NotParked) {
          cli(); 
          if (moveDirHA) skipCountHA=lastSkipCountHA;
          moveDirHA=command[1];  
          lastSkipCountHA=skipCountHA; skipCountHA=moveSkipCountHA; 
          sei();
        }
        quietReply=true;
      } else
//  :Mn# & :Ms#      Move Telescope North or South at current slew rate
//         Returns: Nothing
      if ((command[1]=='n') || (command[1]=='s')) { 
        if (parkStatus==NotParked) {
          cli();
          if (moveDirDec) skipCountDec=lastSkipCountDec;
          moveDirDec=command[1];
          lastSkipCountDec=skipCountDec; 
 #ifdef DEC_RATIO_ON
          skipCountDec=moveSkipCountDec*SkipCountRateRatio; 
 #else
          skipCountDec=moveSkipCountDec; 
 #endif
          sei();
        }
        quietReply=true;
      } else

//  :MS#   Goto the Target Object
//         Returns:
//         0#=Goto is Possible
//         1#=Object below horizon    Outside limits, below the Horizon limit
//         2#=No object selected      Failure to resolve coordinates
//         4#=Position unreachable    Not unparked
//         5#=Busy                    Goto already active
//         6#=Outside limits          Outside limits, above the Zenith limit
      if (command[1]=='S')  {
        quietReply=true; i=goToEqu(newTargetRA,newTargetDec);
        reply[0]=i+'0'; reply[1]=0;
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
          if ((parameter[1]=='+') && (trackingState==TrackingSidereal)) { if (PECrecorded) PECstatus=ReadyPlayPEC; } else
          if ((parameter[1]=='-') && (trackingState==TrackingSidereal)) { PECstatus=IgnorePEC; } else
          if ((parameter[1]=='/') && (trackingState==TrackingSidereal)) { PECstatus=ReadyRecordPEC; } else
          if (parameter[1]=='Z') { 
            for (i=0; i<PECBufferSize; i++) PEC_buffer[i]=128;
              PECfirstRecord=true;
              PECrecord_index=0;
          } else
          if (parameter[1]=='!') {
            for (i=0; i<PECBufferSize; i++) EEPROM.write(EE_PECindex+i,PEC_buffer[i]);
            EEPROM.write(EE_PECrecorded,PECrecorded);
            EEPROM.write(EE_PECstatus,PECstatus);
            EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECrecord_index); 
          } else
          // Status is one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording
          if (parameter[1]=='?') { char *PECstatusCh = PECStatusString; reply[0]=PECstatusCh[PECstatus]; reply[1]=0; } else { quietReply=false; commandError=true; }
        } else commandError=true;
      } else

//   Q - Movement Commands
//  :Q#    Halt all slews
//         Returns: Nothing
      if (command[0]=='Q') {
        if (command[1]==0) { 
          if (parkStatus==NotParked) {
            cli();
            if (moveDirDec) skipCountDec=lastSkipCountDec; 
            if (moveDirHA)  skipCountHA =lastSkipCountHA; 
            sei();
            moveDirDec=0;
            moveDirHA =0;
          }
          quietReply=true; 
        } else
//  :Qe# & Qw#   Halt east/westward Slews
//         Returns: Nothing
        if ((command[1]=='e') || (command[1]=='w')) { 
          if (parkStatus==NotParked) {
            moveDirHA=0; cli(); skipCountHA=lastSkipCountHA; sei();
          }
          quietReply=true; 
        } else
//  :Qn# & Qs#   Halt north/southward Slews
//         Returns: Nothing
        if ((command[1]=='n') || (command[1]=='s')) {
          if (parkStatus==NotParked) {
            moveDirDec=0; cli(); skipCountDec=lastSkipCountDec; sei(); 
            quietReply=true; 
          }
        } else commandError=true;
      } else

//   R - Slew Rate Commands
      if (command[0]=='R') {
//  :RG#   Set Slew rate to Guiding Rate (slowest)
//  :RC#   Set Slew rate to Centering rate (2nd slowest)
//  :RM#   Set Slew rate to Find Rate (2nd Fastest)
//  :RS#   Set Slew rate to max (fastest)
//  :Rn#   Set Slew rate to n, where n=1..9
//         Returns: Nothing
      if ((command[1]=='G') || (command[1]=='C') || (command[1]=='M') || (command[1]=='S') || ((command[1]>='1') && (command[1]<='9'))) {
        if (command[1]=='G') i=1; else
        if (command[1]=='C') i=3; else
        if (command[1]=='M') i=4; else
        if (command[1]=='S') i=6; else i=command[1]-'1';
        setGuideRate(i); // 0..8
        quietReply=true; 
      } else commandError=true;
     } else

//   S - Telescope Set Commands
      if (command[0]=='S') {
#ifdef ALT_AZM_GOTO_ON
//  :SasDD*MM#
//         Set target object altitude to sDD*MM# or sDD*MM'SS# (based on precision setting)
//         Returns:
//         0 if Object is within slew range, 1 otherwise
      if (command[1]=='a')  { if (!dmsToDouble(&newTargetAlt,parameter,true)) commandError=true; } else 
#endif
//  :SBn#  Set Baud Rate n for Serial-0, where n is an ASCII digit (1..9) with the following interpertation
//         1=56.7K, 2=38.4K, 3=28.8K, 4=19.2K, 5=14.4K, 6=9600, 7=4800, 8=2400, 9=1200
//         Returns: "1" At the current baud rate and then changes to the new rate for further communication
      if (command[1]=='B') {
        i=(int)(parameter[0]-'0');
        if (((i>0) && (i<10)) && (process_command==1)) { Serial.print("1\n"); Serial.end(); Serial.begin(baudRate[i]); quietReply=true; } else commandError=true;
      } else 
//  :SCMM/DD/YY#
//          Change Date to MM/DD/YY
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='C')  { if (!dateToDouble(&JD,parameter)) commandError=true; else { EEPROM_writeQuad(EE_JD,(byte*)&JD); LST=(JD,UT1); update_lst(); } } else 
//  :SdsDD*MM#
//          Set target object declination to sDD*MM or sDD*MM:SS depending on the current precision setting
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='d')  { if (!dmsToDouble(&newTargetDec,parameter,true)) commandError=true; } else 
//  :SgsDDD*MM#
//          Set current sites longitude to sDDD*MM an ASCII position string, East longitudes are expressed as negative
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='g')  { i=highPrecision; highPrecision=false; if (!dmsToDouble(&longitude,(char *)&parameter[1],false)) commandError=true; else { if (parameter[0]=='-') longitude=-longitude; EEPROM_writeQuad(EE_sites+(currentSite)*25+4,(byte*)&longitude); } LST=jd2last(JD,UT1); update_lst(); highPrecision=i; } else 
//  :SGsHH#
//          Set the number of hours added to local time to yield UTC
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='G')  { 
        if ((parameter[0]!=0) && (strlen(parameter)<4)) { 
          i=atoi(parameter); 
          if ( (!errno) && ((i>=-24) && (i<=24))) { 
            timeZone=i; 
            b=timeZone+128; 
            EEPROM.write(EE_sites+(currentSite)*25+8,b);
            UT1=LMT+timeZone; 
            UT1_start  =UT1;
            UT1mS_start=millis(); 
            LST=jd2last(JD,UT1); 
            update_lst(); 
          } else commandError=true; 
        } else commandError=true; 
      } else
//  :Sh+DD#
//          Set the lowest elevation to which the telescope will goTo
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='h')  { if ((parameter[0]!=0) && (strlen(parameter)<4)) { i=atoi(parameter); if ( (!errno) && ((i>=-30) && (i<=30))) { minAlt=i; EEPROM.write(EE_minAlt,minAlt-128); } else commandError=true; } else commandError=true; } else
//  :SLHH:MM:SS#
//          Set the local Time
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='L')  {  
        i=highPrecision; highPrecision=true; 
        if (!hmsToDouble(&LMT,parameter)) commandError=true; else {
          EEPROM_writeQuad(EE_LMT,(byte*)&LMT); 
          UT1=LMT+timeZone; 
          UT1_start  =UT1;
          UT1mS_start=millis(); 
          LST=jd2last(JD,UT1); 
          update_lst(); 
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
        i=command[1]-'L';
        if (strlen(parameter)>15) commandError=true; else EEPROM_writeString(EE_sites+i*25+9,parameter);
      } else
//  :SoDD#
//          Set the overhead elevation limit to DD#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='o')  { if ((parameter[0]!=0) && (strlen(parameter)<3)) { i=atoi(parameter); if ( (!errno) && ((i>=60) && (i<=90))) { maxAlt=i; EEPROM.write(EE_maxAlt,maxAlt); } else commandError=true; } else commandError=true; } else
//  :SrHH:MM.T#
//  :SrHH:MM:SS#
//          Set target object RA to HH:MM.T or HH:MM:SS (based on precision setting)
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='r')  { if (!hmsToDouble(&newTargetRA,parameter)) commandError=true; } else 
//  :SSHH:MM:SS#
//          Sets the local (apparent) sideral time to HH:MM:SS
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; if (!hmsToDouble(&LST,parameter)) commandError=true; else update_lst(); highPrecision=i; } else 
//  :StsDD*MM#
//          Sets the current site latitude to sDD*MM#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='t')  { i=highPrecision; highPrecision=false; if (!dmsToDouble(&latitude,parameter,true)) commandError=true; else EEPROM_writeQuad(100+(currentSite)*25+0,(byte*)&latitude); highPrecision=i; } else 
//  :STtt#
//          Sets the current tracking rate to tt hertz, two values are valid "60" and "0".
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='T')  { 
        if ((trackingState==TrackingSidereal) || (trackingState==TrackingNone)) {
          if ((parameter[2]==0) && (parameter[0]=='6') && (parameter[1]=='0')) trackingState = TrackingSidereal; else
            if ((parameter[1]==0) && (parameter[0]=='0')) trackingState = TrackingNone; else
              commandError=true;
        } else commandError=true;
      } else
//  :SzDDD*MM#
//          Sets the target Object Azimuth
//          Return: 0 on failure
//                  1 on success
#ifdef ALT_AZM_GOTO_ON
      if (command[1]=='z')  { if (!dmsToDouble(&newTargetAzm,parameter,false)) commandError=true; } else
#endif
      commandError=true;
      } else 
//   T - Tracking Commands
//  :T+#   Track faster by 1 part in 300(ish)
//  :T-#   Track slower by 1 part in 300(ish) this is the speed the sidereal timer runs at
//         Returns: Nothing
     if (command[0]=='T') {
       if (command[1]=='+') masterSiderealInterval--;
       if (command[1]=='-') masterSiderealInterval++;
       EEPROM_writeInt(EE_siderealInterval,masterSiderealInterval);
       quietReply=true;
     } else
     
//   U - Precision Toggle
//  :U#    Toggle between low/hi precision positions
//         Low -  RA/Dec/etc. displays and accepts HH:MM.M sDD*MM
//         High - RA/Dec/etc. displays and accepts HH:MM:SS sDD*MM:SS
//         Returns Nothing
#ifdef MEGA2560_ON
     if ((command[0]=='U') && (command[1]==0)) { highPrecision=!highPrecision; quietReply=true; } else
#endif
//   V - PEC Readout
//  :VRNNNN#
//         Read out RA PEC Table Entry
//         Returns: sDDD#
//         Rate Adjustment factor for worm segment NNNN. PecRate = Steps +/- for this 1 second segment
//         If NNNN is omitted, returns the currently playing segment
      if ((command[0]=='V') && (command[1]=='R')) {
       if (parameter[0]==0) i=PECindex; else i=atoi(parameter);
       if ( (!errno) && ((i>=0) && (i<PECBufferSize))) {
         if (parameter[0]==0) { 
           i=(i-1)%(StepsPerWormRotation/StepsPerSecond); 
           i1=PEC_buffer[i]-128; sprintf(reply,"%+04i,%03i",i1,i); 
         } else { 
           i1=PEC_buffer[i]-128; sprintf(reply,"%+04i",i1);
         }
       } else commandError=true;
       quietReply=true;
     } else
//   V - PEC Readout StepsPerWormRotation
//  :VW#
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='W')) {
       if (parameter[0]==0) {
         sprintf(reply,"%06i",StepsPerWormRotation);
         quietReply=true;
       } else commandError=true;
     } else
//   V - PEC Readout StepsPerSecond
//  :VS#
//         Returns: DDD#
     if ((command[0]=='V') && (command[1]=='S')) {
       if (parameter[0]==0) {
         sprintf(reply,"%03i",StepsPerSecond);
         quietReply=true;
       } else commandError=true;
     } else
//  :VI#
//         Read RA PEC record index start
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='I')) {
         sprintf(reply,"%06i",PECrecord_index);
         quietReply=true;
     } else

//  :WIDDDDDD#
//         Write RA PEC index start
     if ((command[0]=='W') && (command[1]=='I')) {
       commandError=true;
#ifdef MEGA2560_ON
       // the correct way...
       long int l=atol(parameter);
       // see if it converted and is in range
       if ( (!errno) && ((l>=0) && (l<StepsPerWormRotation))) {
         PECrecord_index=l;
         commandError=false;
       }
#else
       // good enough for my purposes, smaller code limits index offset to maximum of 32767 (mine is 11520) other mounts
       i=atoi(parameter);
       // see if it converted and is in range
       if ( (!errno) && ((i>=0) && (i<PECBufferSize*StepsPerSecond))) {
         PECrecord_index=i;
         commandError=false;
       }
#endif
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
           i=PEC_buffer[StepsPerWormRotation/StepsPerSecond-1];
           memmove((byte *)&PEC_buffer[1],(byte *)&PEC_buffer[0],StepsPerWormRotation/StepsPerSecond-1);
           PEC_buffer[0]=i;
           commandError=false;
         } else
         if (parameter[0]=='-') {
           i=PEC_buffer[0];
           memmove((byte *)&PEC_buffer[0],(byte *)&PEC_buffer[1],StepsPerWormRotation/StepsPerSecond-1);
           PEC_buffer[StepsPerWormRotation/StepsPerSecond-1]=i;
           commandError=false;
         }
       } else {
         // it should be an int
         i=atoi(parameter);
         // see if it converted and is in range
         if ( (!errno) && ((i>=0) && (i<PECBufferSize))) {
           // should be another int here
           i2=atoi((char*)&parameter[5]);
           // see if it converted and is in range
           if ( (!errno) && ((i2>=-128) && (i2<=127))) {
             PEC_buffer[i]=i2+128;
             PECrecorded =true;
             commandError=false;
           }
         }
         quietReply=true;
       }
     } else
#ifdef MEGA2560_ON

//   W - Site Select/Site get
//  :Wn#
//         Sets current site to n, 0..3 or queries site with ?
//         Returns: Nothing or current site ?#
      if (command[0]=='W') {
        if ((command[1]>='0') && (command[1]<='3')) {
          currentSite=command[1]-'0'; EEPROM.write(EE_currentSite,currentSite); quietReply=true;
          EEPROM_readQuad(EE_sites+(currentSite*25+0),(byte*)&latitude);
          EEPROM_readQuad(EE_sites+(currentSite*25+4),(byte*)&longitude);
          b=EEPROM.read(EE_sites+(currentSite*25+8)); timeZone=b-128;
        } else 
        if (command[1]=='?') {
          quietReply=true;
          sprintf(reply,"%i",currentSite);
        } else
          commandError=true;
      } else 
#endif
        commandError=true;
      
      if (!quietReply) {
        if (commandError) reply[0]='0'; else reply[0]='1';
        reply[1]=0; 
      }
      
      if (strlen(reply)>0) {

      if (process_command==1) {
#ifdef CHKSUM0_ON
        // calculate the checksum
        byte cks=0; for (int cksCount0=0; cksCount0<strlen(reply); cksCount0++) {  cks+=reply[cksCount0]; }
#endif

        Serial.print(reply);
#ifdef CHKSUM0_ON
        Serial.print(cks,HEX);
#endif
        Serial.print("#");
      } 

#ifdef MEGA2560_ON
      if (process_command==2) {
#ifdef CHKSUM1_ON
        // calculate the checksum
        byte cks=0; for (int cksCount0=0; cksCount0<strlen(reply); cksCount0++) {  cks+=reply[cksCount0]; }
#endif
//        Serial.print(reply);
        Serial1.print(reply);
#ifdef CHKSUM1_ON
//        Serial.print(cks,HEX);
//        Serial.print("#");
        Serial1.print(cks,HEX);
        Serial1.print("#");
#endif
//        Serial.print("\r\n");
        Serial1.print("\r\n");
      }
#endif
      }
      quietReply=false;
   }
}

// Build up a command
boolean buildCommand(char c) {

#ifdef MEGA2560_ON
  // (chr)6 is a special status command for the LX200 protocol
  if ((c==(char)6) && (bufferPtr_serial_zero==0)) {
    Serial.print('G#');
  }
#endif

  // ignore spaces/lf/cr, dropping spaces is another tweek to allow compatibility with LX200 protocol
  if ((c!=(char)32) && (c!=(char)10) && (c!=(char)13)) {
    command_serial_zero[bufferPtr_serial_zero]=c;
    bufferPtr_serial_zero++;
    command_serial_zero[bufferPtr_serial_zero]=(char)0;
    if (bufferPtr_serial_zero>22) { bufferPtr_serial_zero=22; }  // limit maximum command length to avoid overflow, c2+p16+cc2+eol2+eos1=23 bytes max ranging from 0..22
  }
  
  if (c=='#') {

    // validate the command frame, normal command
    if ((bufferPtr_serial_zero>1) && (command_serial_zero[0]==':') && (command_serial_zero[bufferPtr_serial_zero-1]=='#')) { command_serial_zero[bufferPtr_serial_zero-1]=0; } else { clearCommand_serial_zero(); return false; }

#ifdef CHKSUM0_ON
    // checksum the data, for example ":11111126".  I don't include the command frame in the checksum.  The error response is a checksumed null string "00#\r\n" which means re-transmit.
    byte len=strlen(command_serial_zero);
    byte cks=0; for (int cksCount0=1; cksCount0<len-2; cksCount0++) {  cks+=command_serial_zero[cksCount0]; }
    char chkSum[3]; sprintf(chkSum,"%02X",cks); if (!((chkSum[0]==command_serial_zero[len-2]) && (chkSum[1]==command_serial_zero[len-1]))) { clearCommand_serial_zero();  Serial.print("00#\r\n"); return false; }
    --len; command_serial_zero[--len]=0;
#endif

    // break up the command into a two char command and the remaining parameter
    
    // the parameter can be up to 16 chars in length
    memmove(parameter_serial_zero,(char *)&command_serial_zero[3],17);

    // the command is either one or two chars in length
    command_serial_zero[3]=0;  memmove(command_serial_zero,(char *)&command_serial_zero[1],3);

//    Serial.println(command);
//    Serial.println(parameter);

    return true;
  } else {
    return false;
  }
}

// clear commands
boolean clearCommand_serial_zero() {
  bufferPtr_serial_zero=0;
  command_serial_zero[bufferPtr_serial_zero]=(char)0;
  return true;
}

#ifdef MEGA2560_ON
// Build up a command
boolean buildCommand_serial_one(char c) {
  // ignore lf/cr
  if ((c!=(char)10) && (c!=(char)13)) {
    command_serial_one[bufferPtr_serial_one]=c;
    bufferPtr_serial_one++;
    command_serial_one[bufferPtr_serial_one]=(char)0;
    if (bufferPtr_serial_one>22) { bufferPtr_serial_one=22; }  // limit maximum command length to avoid overflow, c2+p16+cc2+eol2+eos1=23 bytes max ranging from 0..22
  }

  if (c=='#') {
    //Serial.println(command_serial_one);
    // validate the command frame, normal command
    if ((bufferPtr_serial_one>1) && (command_serial_one[0]==':') && (command_serial_one[bufferPtr_serial_one-1]=='#')) { command_serial_one[bufferPtr_serial_one-1]=0; } else { clearCommand_serial_one(); return false; }
    
#ifdef CHKSUM1_ON
    // checksum the data, as above.  
    byte len=strlen(command_serial_one);
    byte cks=0; for (int cksCount0=1; cksCount0<len-2; cksCount0++) { cks=cks+command_serial_one[cksCount0]; }
    char chkSum[3]; sprintf(chkSum,"%02X",cks); if (!((chkSum[0]==command_serial_one[len-2]) && (chkSum[1]==command_serial_one[len-1]))) { clearCommand_serial_one(); Serial1.print("00#\r\n"); Serial.println(" failed"); return false; }
    --len; command_serial_one[--len]=0;
#endif

    // break up the command into a two char command and the remaining parameter
    
    // the parameter can be up to 16 chars in length
    memmove(parameter_serial_one,(char *)&command_serial_one[3],17);

    // the command is either one or two chars in length
    command_serial_one[3]=0;  memmove(command_serial_one,(char *)&command_serial_one[1],3);

//    Serial.println(command);
//    Serial.println(parameter);

    return true;
  } else {
    return false;
  }
}

// clear commands
boolean clearCommand_serial_one() {
  bufferPtr_serial_one=0;
  command_serial_one[bufferPtr_serial_one]=(char)0;
  return true;
}
#endif

// calculates the tracking speed for move commands
void setGuideRate(int g) {
  currentGuideRate=g;
  
  // moveRates[9]={7,15,30,60,120,240,360,600,900}; 

  // this sets how fast the drives CAN move

  // use siderealRate as an basis, it really runs at 2 times the speed (of the sky, half the counts)
  // so mult. by 2 gets us back to the actual (in the sky) sidereal rate, and mult. by another 15 gets us to a count for arc-seconds/second
  // and finally, subtract just a little bit to make sure the timer moves the stepper just a hair faster than the target position it chases
  moveSkipCountHA =((SiderealRate*2*15)/moveRates[g])-1;

  #ifdef DEC_RATIO_ON
  moveSkipCountDec=round(moveSkipCountHA*SkipCountRateRatio);
  #else
  moveSkipCountDec=moveSkipCountHA;
  #endif
  
  // at low speeds, fall back to guide rates based on the 2 x sidereal rate so we can step fast enough to add in the extra guide/PEC steps
  if (g<=3) moveSkipCountHA /=2;
  
  // moveRates[9]={7,15,30,60,120,240,360,600,900}; 
  if (g<=2) { amountMoveHA=1;  } else
  if (g<=5) { amountMoveHA=8;  } else
  if (g<=7) { amountMoveHA=24; } else
            { amountMoveHA=36; }
  amountMoveDec=amountMoveHA;
  
  // time between steps for a given rate, for example:
  // worst case is 10mS (at my stepsPerDegree, this will vary a bit depending on gear ratios on other mounts, but should do as-is for most cases)
  // g=0:45mS;  g=1:21mS;  g=2:10mS;  g=3:42mS;  g=4:21mS;  g=5:10mS;  g=6:21mS;  g=7:13mS;  g=8:13mS
  //
  //              1000/(stepsPerDegreeHA*moveRates[g]/amountMoveHA/3600)
  //              1000/(    11520       *    60      /      8     /3600) = 41mS per step (a little round off error)
  //
  msMoveHA =1000/(((StepsPerDegreeHA *moveRates[g])/amountMoveHA )/3600);
  msMoveDec=1000/(((StepsPerDegreeDec*moveRates[g])/amountMoveDec)/3600);
}




