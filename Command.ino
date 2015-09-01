// -----------------------------------------------------------------------------------
// Command processing

boolean serial_zero_ready = false;
boolean serial_one_ready = false;
// for align
double avgDec = 0.0;
double avgHA  = 0.0;
// scratch-pad variables
double f,f1,f2,f3; 
int    i,i1,i2;
byte   b;

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

//   A - Alignment Commands
      if (command[0]=='A') {
//  :AW#  Align Write to EEPROM
//         Returns: 1 on success
        if (command[1]=='W') {
              saveAlignModel();
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
        if ((command[1]=='1') || (command[1]=='2') || (command[1]=='3')) {
          // set current time and date before calling this routine

          // :A2 and :A3 arent supported with Fork mounts in alternate mode
          #if defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
          if (command[1]=='1') {
          #endif

          // telescope should be set in the polar home (CWD) for a starting point
          // this command sets IH, ID, azmCor=0; altCor=0;
          setHome();
          
          // enable the stepper drivers
          digitalWrite(HA_EN,HA_Enabled);
          digitalWrite(DE_EN,DE_Enabled);
          delay(10);
  
          // newTargetRA =timeRange(LST);
          // newTargetDec=90.0;  
  
          // start tracking
          trackingState=TrackingSidereal;
  
          // start align... AlignOneStar1=01, AlignTwoStar1=11, AlignThreeStar1=21
          alignMode=AlignOneStar1+(command[1]-'1')*10;
  
          commandError=false;
          
          #if defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
          } else commandError=true;
          #endif

        } else
//  :A+#  Manual Alignment, set target location
//         Returns:
//         1: If correction is accepted
//         0: Failure, Manual align mode not set or distance too far 
/*
Alignment Logic:
Near the celestial equator (Dec=0, HA=0)...
the azmCor term is 0 in Dec
the altCor term is 1 in Dec
the doCor  term is 1 in HA
the pdCor  term is 0 in HA

Near HA=6 and Dec=45...
the azmCor term is 1 in Dec
the altCor term is 0 in Dec
the doCor  term is 0 in HA
the pdCor  term is 1 in HA
*/
        if (command[1]=='+') { 

          // First star:
          // Near the celestial equator (Dec=0, HA=0), telescope West of the pier if multi-star align
          if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) {
            if ((alignMode==AlignOneStar1) && (meridianFlip==MeridianFlipAlign)) meridianFlip=MeridianFlipNever;
            alignMode++;
            // set the IH offset
            // set the ID offset
            if (!syncEqu(newTargetRA,newTargetDec)) { commandError=true; }
            IHS=IH*15.0*StepsPerDegreeHA;
            avgDec=newTargetDec;
            avgHA =haRange(LST-newTargetRA);
          } else 
          // Second star:
          // Near the celestial equator (Dec=0, HA=0), telescope East of the pier
          if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2)) {
            if ((alignMode==AlignTwoStar2) && (meridianFlip==MeridianFlipAlign)) meridianFlip=MeridianFlipNever;
            alignMode++;
            double ID1 = -ID;  // last offset Dec is negative because we flipped the meridian
            double IH1 = IH;

            avgDec=(avgDec+newTargetDec)/2.0;
            avgHA =(-avgHA+haRange(LST-newTargetRA))/2.0; // last HA is negative because we were on the other side of the meridian
            if (syncEqu(newTargetRA,newTargetDec)) {
              double ID2=ID;
              double IH2=IH;

              IH    = (IH2+IH1)/2.0;                    // average offset in HA
              IHS=IH*15.0*StepsPerDegreeHA;
              ID    = (ID2-ID1)/2.0;                    // new offset in Dec
              double IH3=IH;
              double ID3=ID;
            
              altCor=-(ID2+ID1)/2.0;                    // Negative when pointed below the pole
              altCor= altCor/cos((avgHA*15.0)/Rad);     // correct for measurements being away from the Meridian

              // allow the altCor to be applied
              if (syncEqu(newTargetRA,newTargetDec)) {
                ID2=ID;
                IH2=IH;

                doCor =-((IH2-IH1)/2.0)*15.0;            // the difference of these two values should be a decent approximation of the optical axis to Dec axis error (aka cone error)
                doCor = doCor*cos(avgDec/Rad);           // correct for measurement being away from the Celestial Equator

                IH=IH3;
                IHS=IH*15.0*StepsPerDegreeHA;
                ID=ID3;
              } else commandError=true;
            } else commandError=true;
          } else 
          // Third star:
          // Near (Dec=45, HA=6), telescope East of the pier
          if (alignMode==AlignThreeStar3) {
            #ifndef MOUNT_TYPE_GEM
            meridianFlip=MeridianFlipNever;
            #endif
            alignMode++;
            
            double ID1 = ID;
            double IH1 = IH;
            if (syncEqu(newTargetRA,newTargetDec)) {
              double ID2=ID;
              double IH2=IH;

              azmCor = -(ID2-ID1);                      // offset in declination is largely due to polar align Azm error
              azmCor = azmCor/sin((haRange(LST-newTargetRA)*15.0)/Rad);  // correct for HA of measurement location

              // allow the azmCor to be applied
              if (syncEqu(newTargetRA,newTargetDec)) {
                ID2=ID;
                IH2=IH;
                // only apply Dec axis flexture term on GEMs
                #ifdef MOUNT_TYPE_GEM
                pdCor =  (IH2-IH1)*15.0;                // the Dec axis to RA axis perp. error should be the only major source of error left effecting the HA
                pdCor = pdCor/tan(newTargetDec/Rad);    // correct for Dec of measurement location
                #else
                pdCor = 0.0;
                #endif

                ID=ID1;
                IH=IH1;
              } else commandError=true;
            } else commandError=true;
          } else commandError=true; 
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
              backlashDec=(int)(((long)i*StepsPerDegreeDec)/3600L);
              EEPROM_writeInt(EE_backlashDec,backlashDec);
            } else
            if (parameter[0]=='R') {
              backlashHA =(int)(((long)i*StepsPerDegreeHA)/3600L);
              EEPROM_writeInt(EE_backlashHA,backlashHA);
            } else commandError=true;
          } else commandError=true;
        } else commandError=true;
      } else
      
//   % - Return parameter
//  :%BD# Get Dec Antibacklash
//          Return: ddd#
//  :%BR# Get RA Antibacklash
//          Return: ddd#
//          Get the Backlash values.  Units are arc-seconds
      if ((command[0]=='%') && (command[1]=='B')) {
        if (parameter[0]=='D') {
            i=(int)(((long)backlashDec*3600L)/StepsPerDegreeDec);
            if (i<0) i=0; if (i>999) i=999;
            sprintf(reply,"%03d",i);
            quietReply=true;
        } else
        if (parameter[0]=='R') {
            i=(int)(((long)backlashHA*3600L)/StepsPerDegreeHA);
            if (i<0) i=0; if (i>999) i=999;
            sprintf(reply,"%03d",i);
            quietReply=true;
        } else commandError=true;
      } else
      
//   C - Sync Control
//  :CS#   Synchonize the telescope with the current right ascension and declination coordinates
//         Returns: Nothing
//  :CM#   Synchonize the telescope with the current database object (as above)
//         Returns: "N/A#"
      if ((command[0]=='C') && ((command[1]=='S') || command[1]=='M'))  {
        if (trackingState!=TrackingMoveTo) {
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
        // (after several days) the time keeping will loose precision as the larger values start to consume the significant digits
        while (LMT>=24.0) { LMT=LMT-24.0; i1--; } 
        if    (LMT<0.0)   { LMT=LMT+24.0; i1++; }
        sprintf(reply,"%02d/%02d/%02d",i,i1,i2); 
        quietReply=true; 
      } else 
//  :GD#   Get Telescope Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
      if (command[1]=='D')  { getEqu(&f,&f1,true,true); if (!doubleToDms(reply,&f1,false,true)) commandError=true; else quietReply=true; } else 
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
      if (command[1]=='H')  { getEqu(&f,&f1,true,true); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true; } else 
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
      if (command[1]=='R')  { getEqu(&f,&f1,false,true); if (!doubleToHms(reply,&f)) commandError=true; else quietReply=true;  } else 
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
        if (trackingState!=TrackingMoveTo)       reply[i++]='N';
        const char *parkStatusCh = "pIPF";       reply[i++]=parkStatusCh[parkStatus]; // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (PECrecorded)                         reply[i++]='R';
        if (atHome)                              reply[i++]='H'; 
        if (PPSsynced)                           reply[i++]='S';
        if ((guideDirHA) || (guideDirDec))       reply[i++]='G';
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
              case '0': sprintf(reply,"%ld",(long)(IH*15.0*3600.0)); quietReply=true; break;  // IH
              case '1': sprintf(reply,"%ld",(long)(ID*3600.0)); quietReply=true; break;       // ID
              case '2': sprintf(reply,"%ld",(long)(altCor*3600.0)); quietReply=true; break;   // altCor
              case '3': sprintf(reply,"%ld",(long)(azmCor*3600.0)); quietReply=true; break;   // azmCor
              case '4': sprintf(reply,"%ld",(long)(doCor*3600.0)); quietReply=true; break;    // doCor
              case '5': sprintf(reply,"%ld",(long)(pdCor*3600.0)); quietReply=true; break;    // pdCor
            }
          } else
          if (parameter[0]=='9') { // 9n: Misc.
            switch (parameter[1]) {
              case '0': dtostrf(guideRates[currentPulseGuideRate]/15.0,2,2,reply); quietReply=true; break;  // pulse-guide rate
              case '1': sprintf(reply,"%i",PECav); quietReply=true; break;                                  // pec analog value
              case '2': sprintf(reply,"%ld",(long)(maxRate/16L)); quietReply=true; break;                   // MaxRate
              case '3': sprintf(reply,"%ld",(long)(MaxRate)); quietReply=true; break;                       // MaxRate (default)
#if defined(__AVR__)
              case '9': sprintf(reply,"%ld",(long)(freeRam())); quietReply=true; break;                     // Available RAM
#endif
            }
          } else
          if (parameter[0]=='F') { // Fn: Debug
            long temp;
            switch (parameter[1]) {
//              case '0': cli(); temp=(long)(posHA-((long)targetHA.part.m+PEC_HA)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;  // Debug0, true vs. target RA position          
              case '0': cli(); temp=(long)(posDec-((long)targetDec.part.m)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;  // Debug0, true vs. target RA position          
//              case '1': sprintf(reply,"%ld",(long)(debugv1/1.00273790935)); quietReply=true; break;                                              // Debug1, RA tracking rate
              case '1': cli(); sprintf(reply,"%ld",(long)(posDec)); sei(); quietReply=true; break;                                              // Debug1, RA tracking rate
            }
          } else commandError=true;
        } else commandError=true;
        getEqu(&f,&f1,false,true);  
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
          doubleToHms(ws,&newTargetRA); strcat(reply,","); strcat(reply,ws);
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
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            enableGuideRate(currentPulseGuideRate);
#else
            enableGuideRate(currentGuideRate);
#endif
            guideDirHA=parameter[0];
            guideDurationLastHA=micros();
            guideDurationHA=(long)i*1000L;
            cli(); if (guideDirHA=='e') guideTimerRateHA=-guideTimerRate; else guideTimerRateHA=guideTimerRate; sei();
            quietReply=true;
         } else
            if ((parameter[0]=='n') || (parameter[0]=='s')) { 
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            enableGuideRate(currentPulseGuideRate);
#else
            enableGuideRate(currentGuideRate);
#endif
              guideDirDec=parameter[0]; 
              guideDurationLastDec=micros();
              guideDurationDec=(long)i*1000L; 
              cli(); guideTimerRateDec=guideTimerRate; sei();
              quietReply=true;
            } else commandError=true;
        } else commandError=true;
      } else
//  :Me# & :Mw#      Move Telescope East or West at current slew rate
//         Returns: Nothing
      if ((command[1]=='e') || (command[1]=='w')) { 
        if (parkStatus==NotParked) {
          // block user from changing direction at high rates, just stop the guide instead
          if ((guideDirHA!=0) && (command[1]!=guideDirHA) && (fabs(guideTimerRateHA)>2)) { 
            guideDirHA='b';
          } else {
            enableGuideRate(currentGuideRate);
            guideDirHA=command[1];
            guideDurationHA=-1;
            cli(); if (guideDirHA=='e') guideTimerRateHA=-guideTimerRate; else guideTimerRateHA=guideTimerRate; sei();
          }
        }
        quietReply=true;
      } else
//  :Mn# & :Ms#      Move Telescope North or South at current slew rate
//         Returns: Nothing
      if ((command[1]=='n') || (command[1]=='s')) { 
        if (parkStatus==NotParked) {
          // block user from changing direction at high rates, just stop the guide instead
          if ((guideDirDec!=0) && (command[1]!=guideDirDec) && (fabs(guideTimerRateDec)>2)) { 
            guideDirDec='b';
          } else {
            enableGuideRate(currentGuideRate);
            guideDirDec=command[1];
            guideDurationDec=-1;
            cli(); guideTimerRateDec=guideTimerRate; sei();
          }
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
            PECfirstRecord = true;
            PECstatus      = IgnorePEC;
            PECrecorded    = false;
            EEPROM.update(EE_PECstatus,PECstatus);
            EEPROM.update(EE_PECrecorded,PECrecorded);
            PECindex_record= 0;
            PECindex_sense = 0;
            EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
            EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
          } else
          if (parameter[1]=='!') {
            PECrecorded=true;
            PECstatus=IgnorePEC;
            EEPROM.update(EE_PECrecorded,PECrecorded);
            EEPROM.update(EE_PECstatus,PECstatus);
           // EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
            EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
            // trigger recording of PEC buffer
            PECautoRecord=PECBufferSize;
          } else
          // Status is one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording.  Or an optional (.) to indicate an index detect.
          if (parameter[1]=='?') { const char *PECstatusCh = PECStatusString; reply[0]=PECstatusCh[PECstatus]; reply[1]=0; reply[2]=0; if (PECindexDetected) { reply[1]='.'; PECindexDetected=false; } } else { quietReply=false; commandError=true; }
        } else commandError=true;
      } else

//   Q - Movement Commands
//  :Q#    Halt all slews, stops goto
//         Returns: Nothing
      if (command[0]=='Q') {
        if (command[1]==0) {
          if (parkStatus==NotParked) {
            if (guideDirHA)  guideDirHA='b'; // break
            if (guideDirDec) guideDirDec='b'; // break
            if (trackingState==TrackingMoveTo) { abortSlew=true; }
          }
          quietReply=true; 
        } else
//  :Qe# & Qw#   Halt east/westward Slews
//         Returns: Nothing
        if ((command[1]=='e') || (command[1]=='w')) { 
          if (parkStatus==NotParked) {
            if (guideDirHA) guideDirHA='b'; // break
          }
          quietReply=true; 
        } else
//  :Qn# & Qs#   Halt north/southward Slews
//         Returns: Nothing
        if ((command[1]=='n') || (command[1]=='s')) {
          if (parkStatus==NotParked) {
            if (guideDirDec) guideDirDec='b'; // break
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
      if (command[1]=='C')  { if (!dateToDouble(&JD,parameter)) commandError=true; else { float f=JD; EEPROM_writeQuad(EE_JD,(byte*)&f); LST=jd2last(JD,UT1); update_lst(); } } else 
//  :SdsDD*MM#
//          Set target object declination to sDD*MM or sDD*MM:SS depending on the current precision setting
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='d')  { if (!dmsToDouble(&newTargetDec,parameter,true)) commandError=true; } else 
//  :SgsDDD*MM#
//          Set current sites longitude to sDDD*MM an ASCII position string, East longitudes are expressed as negative
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='g')  { i=highPrecision; highPrecision=false; if (!dmsToDouble(&longitude,(char *)&parameter[1],false)) commandError=true; else { if (parameter[0]=='-') longitude=-longitude; float f=longitude; EEPROM_writeQuad(EE_sites+(currentSite)*25+4,(byte*)&f); } LST=jd2last(JD,UT1); update_lst(); highPrecision=i; } else 
//  :SGsHH#
//          Set the number of hours added to local time to yield UTC
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='G')  { 
        if ((parameter[0]!=0) && (strlen(parameter)<4)) { 
          if ( (atoi2(parameter,&i)) && ((i>=-24) && (i<=24))) { 
            timeZone=i;
            b=timeZone+128; 
            EEPROM.update(EE_sites+(currentSite)*25+8,b);
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
        if ( (atoi2(parameter,&i)) && ((i>=-30) && (i<=30))) { minAlt=i; EEPROM.update(EE_minAlt,minAlt+128); } else commandError=true; 
      } else commandError=true; } else
//  :SLHH:MM:SS#
//          Set the local Time
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='L')  {  
        i=highPrecision; highPrecision=true; 
        if (!hmsToDouble(&LMT,parameter)) commandError=true; else {
          float f=LMT; EEPROM_writeQuad(EE_LMT,(byte*)&f); 
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
        i=command[1]-'M';
        if (strlen(parameter)>15) commandError=true; else EEPROM_writeString(EE_sites+i*25+9,parameter);
      } else
//  :SoDD#
//          Set the overhead elevation limit to DD#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='o')  { if ((parameter[0]!=0) && (strlen(parameter)<3)) { 
        if ( (atoi2(parameter,&i)) && ((i>=60) && (i<=90))) { maxAlt=i; EEPROM.update(EE_maxAlt,maxAlt); } else commandError=true; 
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
          float f=latitude; EEPROM_writeQuad(100+(currentSite)*25+0,(byte*)&f);
          if (latitude<0) celestialPoleDec=-90L; else celestialPoleDec=90L;
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (celestialPoleDec>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
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
              siderealInterval=HzCf*((60.0/f)*60.0);
              EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);
              SetSiderealClockRate(siderealInterval);
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
            case '0': IH=(double)strtol(&parameter[3],NULL,10)/3600.0/15.0; break;  // IH
            case '1': ID=(double)strtol(&parameter[3],NULL,10)/3600.0; break;       // ID
            case '2': altCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;   // altCor
            case '3': azmCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;   // azmCor
            case '4': doCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // doCor
            case '5': pdCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // pdCor
          }
        } else
        if (parameter[0]=='9') { // 9n: Misc.
          switch (parameter[1]) {
            case '2': 
              maxRate=strtol(&parameter[3],NULL,10)*16L;
              if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
              if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
              EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L));
            break; // maxRate
          }
        } else commandError=true;
        getEqu(&f,&f1,false,true);  
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
//  :T+#   Track faster by 0.1 Hertz (I use a fifth of the LX200 standard, stored in EEPROM)
//  :T-#   Track slower by 0.1 Hertz (stored in EEPROM)
//  :TS#   Track rage solar
//  :TL#   Track rate lunar
//  :TQ#   Track rate custom (stored in EEPROM)
//  :TR#   Track rate reset custom (to calculated sidereal rate, stored in EEPROM)
//  :TK#   Track rate king
//  :Te#   Tracking enable  (OnStep only, replies 0/1)
//  :Td#   Tracking disable (OnStep only, replies 0/1)
//  :Tr#   Track refraction enable  (OnStep only, replies 0/1)
//  :Tn#   Track refraction disable (OnStep only, replies 0/1)
//         Returns: Nothing
     if (command[0]=='T') {
       if (command[1]=='+') { siderealInterval-=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='-') { siderealInterval+=HzCf*(0.02); quietReply=true; } else
       if (command[1]=='S') { siderealInterval =HzCf*(60.0); customRateActive=false; quietReply=true; } else                       // solar tracking rate
       if (command[1]=='L') { siderealInterval =HzCf*((60.0/57.9)*60.0); customRateActive=false; quietReply=true; } else           // lunar tracking rate
       if (command[1]=='Q') { EEPROM_readQuad(EE_siderealInterval,(byte*)&siderealInterval); customRateActive=true; quietReply=true; } else  // custom tracking rate
       if (command[1]=='R') { siderealInterval =HzCf*((60.0/60.16427479)*60.0); EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval); customRateActive=true; quietReply=true; } else // default tracking rate
       if (command[1]=='K') { siderealInterval =HzCf*((60.0/60.136)*60.0); customRateActive=false; quietReply=true; } else         // king tracking rate
       if ((command[1]=='e') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone))) trackingState=TrackingSidereal; else
       if ((command[1]=='d') && ((trackingState==TrackingSidereal) || (trackingState==TrackingNone))) trackingState=TrackingNone; else
       if ((command[1]=='r') && (!refraction)) refraction=true; else
       if ((command[1]=='n') && (refraction)) refraction=false; else
         commandError=true;
       // Only burn the new rate if changing the custom tracking rate 
       if ((customRateActive) && (!commandError) && ((command[1]=='+') || (command[1]=='-'))) EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);

       SetSiderealClockRate(siderealInterval);
       cli(); SiderealRate=siderealInterval/StepsPerSecond; sei();
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
       if (parameter[0]==0) { i=PECindex1; } else conv_result=atoi2(parameter,&i);
       if ((conv_result) && ((i>=0) && (i<PECBufferSize))) {
         if (parameter[0]==0) { 
           i=(i-1)%SecondsPerWormRotation; 
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
//         Returns: DDD.DDDDDD#
     if ((command[0]=='V') && (command[1]=='S')) {
       if (parameter[0]==0) {
         char temp[12];
         dtostrf(StepsPerSecond,0,6,temp);
         strcpy(reply,temp);
//         sprintf(reply,"%03i",StepsPerSecond);
         quietReply=true;
       } else commandError=true;
     } else
//  :VH#
//         Read RA PEC hall sensor index start (seconds)
//         Returns: DDDDD#
     if ((command[0]=='V') && (command[1]=='H')) {
         sprintf(reply,"%05ld",PECindex_sense);
         quietReply=true;
     } else
//  :VI#
//         Read RA PEC record index start (steps)
//         Returns: DDDDDD#
     if ((command[0]=='V') && (command[1]=='I')) {
         sprintf(reply,"%06ld",PECindex_record);
         quietReply=true;
     } else

//  :WIDDDDDD#
//         Write RA PEC index start (steps)
     if ((command[0]=='W') && (command[1]=='I')) {
       commandError=true;
       long l=strtol(parameter,&conv_end,10);
       // see if it converted and is in range
       if ( (&parameter[0]!=conv_end) && ((l>=0) && (l<StepsPerWormRotation))) {
         PECindex_record=l;
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
           i=PEC_buffer[SecondsPerWormRotation-1];
           memmove((byte *)&PEC_buffer[1],(byte *)&PEC_buffer[0],SecondsPerWormRotation-1);
           PEC_buffer[0]=i;
           commandError=false;
         } else
         if (parameter[0]=='-') {
           i=PEC_buffer[0];
           memmove((byte *)&PEC_buffer[0],(byte *)&PEC_buffer[1],SecondsPerWormRotation-1);
           PEC_buffer[SecondsPerWormRotation-1]=i;
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
          currentSite=command[1]-'0'; EEPROM.update(EE_currentSite,currentSite); quietReply=true;
          float f; 
          EEPROM_readQuad(EE_sites+(currentSite*25+0),(byte*)&f); latitude=f;
          if (latitude<0) celestialPoleDec=-90L; else celestialPoleDec=90L;
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (celestialPoleDec>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
          EEPROM_readQuad(EE_sites+(currentSite*25+4),(byte*)&f); longitude=f;
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
//    Serial_print("G#");
    #ifdef MOUNT_TYPE_ALTAZM
    Serial_print("A");
    #else
    Serial_print("P");
    #endif
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
//    Serial1_print("G#");
    #ifdef MOUNT_TYPE_ALTAZM
    Serial_print("A");
    #else
    Serial_print("P");
    #endif
  }

  // ignore spaces/lf/cr, dropping spaces is another tweek to allow compatibility with LX200 protocol
  if ((c!=(char)32) && (c!=(char)10) && (c!=(char)13) && (c!=(char)6)) {
    command_serial_one[bufferPtr_serial_one]=c;
    bufferPtr_serial_one++;
    command_serial_one[bufferPtr_serial_one]=(char)0;
    if (bufferPtr_serial_one>22) { bufferPtr_serial_one=22; }  // limit maximum command length to avoid overflow, c2+p16+cc2+eol2+eos1=23 bytes max ranging from 0..22
  }

  if (c=='#') {
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
  if ((g<=GuideRate1x) && (currentPulseGuideRate!=g)) { currentPulseGuideRate=g; EEPROM.update(EE_pulseGuideRate,g); }
}

void enableGuideRate(int g) {
  // don't do these lengthy calculations unless we have to
  if (activeGuideRate==g) return;
  
  activeGuideRate=g;
  
  // this enables the guide rate
  guideTimerRate=(double)guideRates[g]/15.0;

  cli();
  amountGuideHA.fixed =doubleToFixed((guideTimerRate*StepsPerSecond)/100.0);
  amountGuideDec.fixed=doubleToFixed((guideTimerRate*StepsPerSecondDec)/100.0);
  sei();
}

#if defined(__AVR__)
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif
