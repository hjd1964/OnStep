// -----------------------------------------------------------------------------------
// Command processing

boolean serial_zero_ready = false;
boolean serial_one_ready = false;

// process commands
void processCommands() {
    boolean supress_frame = false;
    char *conv_end;

    if ((Serial_available() > 0) && (!serial_zero_ready)) { serial_zero_ready = buildCommand(Serial_read()); }
    if ((Serial1_available() > 0) && (!serial_one_ready)) { serial_one_ready = buildCommand_serial_one(Serial1_read()); }
    
    if (Serial_transmit()) return;
    if (Serial1_transmit()) return;
    
    byte process_command = 0;
    if (serial_zero_ready)     { strcpy(command,command_serial_zero); strcpy(parameter,parameter_serial_zero); serial_zero_ready=false; clearCommand_serial_zero(); process_command=1; } 
    else if (serial_one_ready) { strcpy(command,command_serial_one);  strcpy(parameter,parameter_serial_one);  serial_one_ready=false;  clearCommand_serial_one();  process_command=2; }
    else return;

    if (process_command) {
// Command is two chars followed by an optional parameter...
      commandError=false;
// Handles empty and one char replies
      reply[0]=0; reply[1]=0;

//   @ - Parameter read/write
        if (command[0]=='@') {
//  :@W??SSSSS#  Write value, address ??
//         Returns:
//         1: If value accepted
//         0: Failure 
/*          if (command[1]=='W') {
            if ((parameter[0]=='D') && (parameter[1]=='O')) {
              _DO=atoi((char*)parameter[2]);
            } else
            if (parameter[0]=='P') && (parameter[l]=='D')) {
              _PD=atoi((char*)parameter[2]);
            } else commandError=true;
          } else
          if (command[1]=='R') {
          }  else commandError=true;
*/        } else

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
//   $ - Set parameter
//  :$BDddd# Set Dec Antibacklash
//          Return: 0 on failure
//                  1 on success
//  :$BRddd# Set RA Antibacklash
//          Return: 0 on failure
//                  1 on success
//          Set the Backlash values.  Units are arc-seconds
//          For HA  when slewing east takes this many additional steps
//          For Dec when slewing takes this many additional steps
//          for this reason, calls to the GetEqu command need to have a correction applied to remove
//          this effect.
      if ((command[0]=='$') && (command[1]=='B')) {
        if ((strlen(parameter)>1) && (strlen(parameter)<5)) {
          if ( (atoi2((char*)&parameter[1],&i)) && ((i>=0) && (i<=999))) { 
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
//   % Return parameter
//  :%BD# Get Dec Antibacklash
//          Return: ddd#
//  :%BR# Get RA Antibacklash
//          Return: ddd#
//          Get the Backlash values.  Units are arc-seconds
      if ((command[0]=='%') && (command[1]=='B')) {
        if (parameter[0]=='D') {
            i=((backlashDec*3600)/StepsPerDegreeDec);
            sprintf(reply,"%03d",i);
            quietReply=true; 
        } else
        if (parameter[0]=='R') {
            i=((backlashHA*3600)/StepsPerDegreeHA);
            sprintf(reply,"%03d",i);
            quietReply=true; 
        } else commandError=true;
      } else
//   C - Sync Control
//  :CS#   Synchonize the telescope with the current right ascension and declination coordinates
//  :CM#   Synchonize the telescope with the current database object (as above)
//         Returns: Nothing
      if ((command[0]=='C') && ((command[1]=='S') || command[1]=='M'))  {
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
      if (command[1]=='R')  { getEqu(&f,&f1,false); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true;  } else 
//  :Gr#   Get current/target object RA
//         Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
      if (command[1]=='r')  { if (!doubleToHms(reply,&newTargetRA)) commandError=true; else quietReply=true; } else 
//  :GS#   Get the Sidereal Time
//         Returns: HH:MM:SS#
//         The Sidereal Time as an ASCII Sexidecimal value in 24 hour format
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; if (!doubleToHms(reply,&LST)) commandError=true; else quietReply=true; highPrecision=i; } else 
//  :GT#   Get tracking rate
//         Returns: dd.ddddd# (OnStep returns more decimal places than LX200 standard)
//         Returns the tracking rate if siderealTracking, 0.0 otherwise
      if (command[1]=='T')  {
        if (trackingState==TrackingSidereal) f=(60.0/(siderealInterval/HzCf))*60.0; else f=0.0;
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
        if (trackingState!=TrackingMoveTo)     reply[i++]='N';
        const char *parkStatusCh = "pIPF";     reply[i++]=parkStatusCh[parkStatus]; // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (PECrecorded)                       reply[i++]='R';
        if (atHome)                            reply[i++]='H'; 
        if ((moveDirHA!=0) || (moveDirDec!=0)) reply[i++]='G';
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
#endif
//  :Mgdnnnn# Pulse guide command
//          Returns: Nothing
      if (command[1]=='g') {
        if ( (atoi2((char *)&parameter[1],&i)) && ((i>=0) && (i<=16399)) && (parkStatus==NotParked)) { 
          if ((parameter[0]=='e') || (parameter[0]=='w')) {
            if (moveDirHA) { cli(); moveTimerRateHA=0; sei(); }
            moveDirHA=parameter[0];  
            moveDurationHA=i;
            cli(); 
            if (moveDirHA=='e') moveTimerRateHA=-moveTimerRate; else moveTimerRateHA=moveTimerRate; 
            sei();
            quietReply=true;
         } else
            if ((parameter[0]=='n') || (parameter[0]=='s')) { 
              if (moveDirDec) { cli(); moveTimerRateDec=0; sei(); }
              moveDirDec=parameter[0]; 
              moveDurationDec=i; 
              cli(); moveTimerRateDec=moveTimerRate; sei();
              quietReply=true;
            } else commandError=true;
        } else commandError=true;
      } else
//  :Me# & :Mw#      Move Telescope East or West at current slew rate
//         Returns: Nothing
      if ((command[1]=='e') || (command[1]=='w')) { 
        if (parkStatus==NotParked) {
          if (moveDirHA) { cli(); moveTimerRateHA=0; sei(); }
          moveDirHA=command[1];
          cli(); 
          if (moveDirHA=='e') moveTimerRateHA=-moveTimerRate; else moveTimerRateHA=moveTimerRate; 
          sei();
        }
        quietReply=true;
      } else
//  :Mn# & :Ms#      Move Telescope North or South at current slew rate
//         Returns: Nothing
      if ((command[1]=='n') || (command[1]=='s')) { 
        if (parkStatus==NotParked) {
          if (moveDirDec) { cli(); moveTimerRateDec=0; sei(); }
          moveDirDec=command[1];
          cli(); moveTimerRateDec=moveTimerRate; sei();
        }
        quietReply=true;
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
          if (parameter[1]=='?') { const char *PECstatusCh = PECStatusString; reply[0]=PECstatusCh[PECstatus]; reply[1]=0; } else { quietReply=false; commandError=true; }
        } else commandError=true;
      } else

//   Q - Movement Commands
//  :Q#    Halt all slews
//         Returns: Nothing
      if (command[0]=='Q') {
        if (command[1]==0) { 
          if (parkStatus==NotParked) {
            if (moveDirHA!=0) { cli(); moveTimerRateHA=0; sei(); moveDirHA=0; }
            if (moveDirDec!=0) { cli(); moveTimerRateDec=0; sei(); moveDirDec=0; }
          }
          quietReply=true; 
        } else
//  :Qe# & Qw#   Halt east/westward Slews
//         Returns: Nothing
        if ((command[1]=='e') || (command[1]=='w')) { 
          if (parkStatus==NotParked) {
            if (moveDirHA!=0) { cli(); moveTimerRateHA=0; sei(); moveDirHA=0; }
          }
          quietReply=true; 
        } else
//  :Qn# & Qs#   Halt north/southward Slews
//         Returns: Nothing
        if ((command[1]=='n') || (command[1]=='s')) {
          if (parkStatus==NotParked) {
            if (moveDirDec!=0) { cli(); moveTimerRateDec=0; sei(); moveDirDec=0; }
          }
          quietReply=true; 
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
        if ((i>0) && (i<10)) {
          if (process_command==1) { 
            Serial_print("1"); while (Serial_transmit()); delay(20); Serial_Init(baudRate[i]);
          } else {
            Serial1_print("1"); while (Serial1_transmit()); delay(20); Serial1_Init(baudRate[i]); 
          }
          quietReply=true; 
        } else commandError=true;
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
          if ( (atoi2(parameter,&i)) && ((i>=-24) && (i<=24))) { 
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
      if (command[1]=='h')  { if ((parameter[0]!=0) && (strlen(parameter)<4)) {
        if ( (atoi2(parameter,&i)) && ((i>=-30) && (i<=30))) { minAlt=i; EEPROM.write(EE_minAlt,minAlt+128); } else commandError=true; 
      } else commandError=true; } else
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
      if (command[1]=='o')  { if ((parameter[0]!=0) && (strlen(parameter)<3)) { 
        if ( (atoi2(parameter,&i)) && ((i>=60) && (i<=90))) { maxAlt=i; EEPROM.write(EE_maxAlt,maxAlt); } else commandError=true; 
      } else commandError=true; } else
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
      if (command[1]=='t')  { 
        i=highPrecision; highPrecision=false; 
        if (!dmsToDouble(&latitude,parameter,true)) { commandError=true; } else { 
          EEPROM_writeQuad(100+(currentSite)*25+0,(byte*)&latitude); 
        }
        highPrecision=i;
      } else 
//  :STdd.ddddd#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='T')  { 
        if ((trackingState==TrackingSidereal) || (trackingState==TrackingNone)) {
          f=strtod(parameter,&conv_end);
          if ( (&parameter[0]!=conv_end) && ((f>=30.0) && (f<90.0) || (abs(f)<0.1))) {
            if (abs(f)<0.1) { 
              trackingState = TrackingNone; 
            } else {
              siderealInterval=HzCf*((60.0/f)*60.0);
              EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);
              Timer1SetRate(siderealInterval/100);
            }
          } else commandError=true;
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
//  :T+#   Track faster by 0.1 Hertz (I use a fifth of the LX200 standard)
//  :T-#   Track slower by 0.1 Hertz
//  :TS#   Track rage solar
//  :TL#   Track rate lunar
//  :TQ#   Track rate custom (stored in EEPROM)
//  :TR#   Track rate reset custom (to calculated sidereal rate)
//  :TK#   Track rate king
//  :Te#   Tracking enable  (OnStep only, replies 0/1)
//  :Td#   Tracking disable (OnStep only, replies 0/1)
//         Returns: Nothing
     if (command[0]=='T') {
       if (command[1]=='+') { siderealInterval-=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='-') { siderealInterval+=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='S') { siderealInterval =HzCf*(60.0); customRateActive=false; quietReply=true; } else                       // solar tracking rate
       if (command[1]=='L') { siderealInterval =HzCf*((60.0/57.9)*60.0); customRateActive=false; quietReply=true; } else           // lunar tracking rate
       if (command[1]=='Q') { EEPROM_readQuad(EE_siderealInterval,(byte*)&siderealInterval); customRateActive=true; quietReply=true; } else  // custom tracking rate
       if (command[1]=='R') { siderealInterval =HzCf*((60.0/60.16427479)*60.0); customRateActive=false; quietReply=true; } else    // default tracking rate
       if (command[1]=='K') { siderealInterval =HzCf*((60.0/60.136)*60.0); customRateActive=false; quietReply=true; } else         // king tracking rate
       if ((command[1]=='e') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone))) trackingState=TrackingSidereal; else
       if ((command[1]=='d') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone))) trackingState=TrackingNone; else
         commandError=true;

       // Only burn the new rate if changing the custom tracking rate 
       if ((customRateActive) && (!commandError) && ((command[1]=='+') || (command[1]=='-'))) EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);

       Timer1SetRate(siderealInterval/100);
       SiderealRate=siderealInterval/StepsPerSecond;
     } else
     
//   U - Precision Toggle
//  :U#    Toggle between low/hi precision positions
//         Low -  RA/Dec/etc. displays and accepts HH:MM.M sDD*MM
//         High - RA/Dec/etc. displays and accepts HH:MM:SS sDD*MM:SS
//         Returns Nothing
     if ((command[0]=='U') && (command[1]==0)) { highPrecision=!highPrecision; quietReply=true; } else
//   V - PEC Readout
//  :VRNNNN#
//         Read out RA PEC Table Entry
//         Returns: sDDD#
//         Rate Adjustment factor for worm segment NNNN. PecRate = Steps +/- for this 1 second segment
//         If NNNN is omitted, returns the currently playing segment
     if ((command[0]=='V') && (command[1]=='R')) {
       boolean conv_result=true;
       if (parameter[0]==0) { i=PECindex; } else conv_result=atoi2(parameter,&i);
       if ((conv_result) && ((i>=0) && (i<PECBufferSize))) {
         if (parameter[0]==0) { 
           i=(i-1)%(StepsPerWormRotation/StepsPerSecond); 
           i1=PEC_buffer[i]-128; sprintf(reply,"%+04i,%03i",i1,i); 
         } else { 
           i1=PEC_buffer[i]-128; sprintf(reply,"%+04i",i1);
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
           if (i+j<PECBufferSize) b=PEC_buffer[i+j]; else b=128;
           
           sprintf(x,"%02X",b);
           strcat(reply,x);
         }
//         strcat(reply,"#");
       } else commandError=true;
       quietReply=true;
     } else
//   V - PEC Readout StepsPerWormRotation
//  :VW#
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='W')) {
       if (parameter[0]==0) {
         sprintf(reply,"%06ld",StepsPerWormRotation);
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
         sprintf(reply,"%06ld",PECrecord_index);
         quietReply=true;
     } else

//  :WIDDDDDD#
//         Write RA PEC index start
     if ((command[0]=='W') && (command[1]=='I')) {
       commandError=true;
       long l=strtol(parameter,&conv_end,10);
       // see if it converted and is in range
       if ( (&parameter[0]!=conv_end) && ((l>=0) && (l<StepsPerWormRotation))) {
         PECrecord_index=l;
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
         // see if it converted and is in range
         if ( (atoi2(parameter,&i)) && ((i>=0) && (i<PECBufferSize))) {
           // should be another int here
           // see if it converted and is in range
           if ( (atoi2((char*)&parameter[5],&i2)) && ((i2>=-128) && (i2<=127))) {
             PEC_buffer[i]=i2+128;
             PECrecorded =true;
             commandError=false;
           }
         }
         quietReply=true;
       }
     } else

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
        commandError=true;
      
      if (!quietReply) {
        if (commandError) reply[0]='0'; else reply[0]='1';
        reply[1]=0;
        supress_frame=true;
      }
      
      if (strlen(reply)>0) {

      if (process_command==1) {
#ifdef CHKSUM0_ON
        // calculate the checksum
        char HEXS[3]="";
        byte cks=0; for (int cksCount0=0; cksCount0<strlen(reply); cksCount0++) {  cks+=reply[cksCount0]; }
        sprintf(HEXS,"%02X",cks);
        strcat(reply,HEXS);
#endif
        if (!supress_frame) strcat(reply,"#");
        Serial_print(reply);
      } 

      if (process_command==2) {
#ifdef CHKSUM1_ON
        // calculate the checksum
        char HEXS[3]="";
        byte cks=0; for (int cksCount0=0; cksCount0<strlen(reply); cksCount0++) {  cks+=reply[cksCount0]; }
        sprintf(HEXS,"%02X",cks);
        strcat(reply,HEXS);
#endif
        if (!supress_frame) strcat(reply,"#");
//        strcat(reply,"\r");
        Serial1_print(reply);
      }
      }
      quietReply=false;
   }
}

// Build up a command
boolean buildCommand(char c) {
  // (chr)6 is a special status command for the LX200 protocol
  if ((c==(char)6) && (bufferPtr_serial_zero==0)) {
    Serial_print("G#");
  }

  // ignore spaces/lf/cr, dropping spaces is another tweek to allow compatibility with LX200 protocol
  if ((c!=(char)32) && (c!=(char)10) && (c!=(char)13) && (c!=(char)6)) {
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
    char chkSum[3]; sprintf(chkSum,"%02X",cks); if (!((chkSum[0]==command_serial_zero[len-2]) && (chkSum[1]==command_serial_zero[len-1]))) { clearCommand_serial_zero();  Serial_print("00#"); return false; }
    --len; command_serial_zero[--len]=0;
#endif

    // break up the command into a two char command and the remaining parameter
    
    // the parameter can be up to 16 chars in length
    memmove(parameter_serial_zero,(char *)&command_serial_zero[3],17);

    // the command is either one or two chars in length
    command_serial_zero[3]=0;  memmove(command_serial_zero,(char *)&command_serial_zero[1],3);

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

// Build up a command
boolean buildCommand_serial_one(char c) {
  // (chr)6 is a special status command for the LX200 protocol
  if ((c==(char)6) && (bufferPtr_serial_zero==0)) {
    Serial1_print("G#");
//    Serial1_print("\r");
  }

  // ignore spaces/lf/cr, dropping spaces is another tweek to allow compatibility with LX200 protocol
  if ((c!=(char)32) && (c!=(char)10) && (c!=(char)13) && (c!=(char)6)) {
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
    char chkSum[3]; sprintf(chkSum,"%02X",cks); if (!((chkSum[0]==command_serial_one[len-2]) && (chkSum[1]==command_serial_one[len-1]))) { clearCommand_serial_one(); Serial1_print("00#"); return false; }
    --len; command_serial_one[--len]=0;
#endif

    // break up the command into a two char command and the remaining parameter
    
    // the parameter can be up to 16 chars in length
    memmove(parameter_serial_one,(char *)&command_serial_one[3],17);

    // the command is either one or two chars in length
    command_serial_one[3]=0;  memmove(command_serial_one,(char *)&command_serial_one[1],3);

// For debugging:
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

// calculates the tracking speed for move commands
void setGuideRate(int g) {
  currentGuideRate=g;
  
  // moveRates[9]={7,15,30,60,120,240,360,600,900}; 

  // this sets the guide rate
  moveTimerRate=(double)moveRates[g]/15.0;
  
  cli();
  // moveRates[9]={7,15,30,60,120,240,360,600,900}; 
  if (g<=1) { amountMoveHA=1; } else // 7,   15
  if (g<=3) { amountMoveHA=4; } else // 30,  60
  if (g<=5) { amountMoveHA=8; } else // 120, 240
  if (g<=7) { amountMoveHA=16; } else // 360, 600
            { amountMoveHA=24; }     // 900
  sei();
  amountMoveDec=amountMoveHA;

  msMoveHA =((3600000.0/(double)StepsPerDegreeHA) /((double)moveRates[g]))*amountMoveHA;
  msMoveDec=((3600000.0/(double)StepsPerDegreeDec)/((double)moveRates[g]))*amountMoveDec;
}
