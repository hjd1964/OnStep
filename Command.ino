// -----------------------------------------------------------------------------------
// Command processing

// last RA/Dec time
unsigned long _coord_t=0;

// help with commands
enum Command {COMMAND_NONE, COMMAND_SERIAL_A, COMMAND_SERIAL_B, COMMAND_SERIAL_C, COMMAND_SERIAL_D, COMMAND_SERIAL_E, COMMAND_SERIAL_ST4, COMMAND_SERIAL_X};
cb cmdA;  // the first Serial is always enabled
#ifdef HAL_SERIAL_B_ENABLED
cb cmdB;
#endif
#ifdef HAL_SERIAL_C_ENABLED
cb cmdC;
#endif
#ifdef HAL_SERIAL_D_ENABLED
cb cmdD;
#endif
#ifdef HAL_SERIAL_E_ENABLED
cb cmdE;
#endif
#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
cb cmdST4;
#endif
char _replyX[50]=""; cb cmdX;  // virtual command channel for internal use

// process commands
void processCommands() {
    // scratch-pad variables
    double f,f1; 
    int    i,i1,i2;
    byte   b;
    
    // last RA/Dec
    static double _dec,_ra;

    // command processing
    static char reply[50];
    static char command[3];
    static char parameter[45];
    static bool boolReply = true;

    bool supress_frame = false;
    char *conv_end;
#if FOCUSER1 == ON
    static char primaryFocuser = 'F';
    static char secondaryFocuser = 'f';
#endif

    // accumulate the command
    if (SerialA.available() > 0 && !cmdA.ready()) cmdA.add(SerialA.read());
#ifdef HAL_SERIAL_B_ENABLED
    if (SerialB.available() > 0 && !cmdB.ready()) cmdB.add(SerialB.read());
#endif
#ifdef HAL_SERIAL_C_ENABLED
    if (SerialC.available() > 0 && !cmdC.ready()) cmdC.add(SerialC.read());
#endif
#ifdef HAL_SERIAL_D_ENABLED
    if (SerialD.available() > 0 && !cmdD.ready()) cmdD.add(SerialD.read());
#endif
#ifdef HAL_SERIAL_E_ENABLED
    if (SerialE.available() > 0 && !cmdE.ready()) cmdE.add(SerialE.read());
#endif
#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
    if (SerialST4.available() > 0 && !cmdST4.ready()) cmdST4.add(SerialST4.read());
#endif

    // send any reply
#ifdef HAL_SERIAL_TRANSMIT
    if (SerialA.transmit()) return;
  #ifdef HAL_SERIAL_B_ENABLED
    if (SerialB.transmit()) return;
  #endif
  #ifdef HAL_SERIAL_C_ENABLED
    if (SerialC.transmit()) return;
  #endif
  #ifdef HAL_SERIAL_D_ENABLED
    if (SerialD.transmit()) return;
  #endif
#endif

    // if a command is ready, process it
    Command process_command = COMMAND_NONE;
    if (cmdA.ready()) { strcpy(command,cmdA.getCmd()); strcpy(parameter,cmdA.getParameter()); cmdA.flush(); process_command=COMMAND_SERIAL_A; }
#ifdef HAL_SERIAL_B_ENABLED
    else if (cmdB.ready()) { strcpy(command,cmdB.getCmd()); strcpy(parameter,cmdB.getParameter()); cmdB.flush(); process_command=COMMAND_SERIAL_B; }
#endif
#ifdef HAL_SERIAL_C_ENABLED
    else if (cmdC.ready()) { strcpy(command,cmdC.getCmd()); strcpy(parameter,cmdC.getParameter()); cmdC.flush(); process_command=COMMAND_SERIAL_C; }
#endif
#ifdef HAL_SERIAL_D_ENABLED
    else if (cmdD.ready()) { strcpy(command,cmdD.getCmd()); strcpy(parameter,cmdD.getParameter()); cmdD.flush(); process_command=COMMAND_SERIAL_D; }
#endif
#ifdef HAL_SERIAL_E_ENABLED
    else if (cmdE.ready()) { strcpy(command,cmdE.getCmd()); strcpy(parameter,cmdE.getParameter()); cmdE.flush(); process_command=COMMAND_SERIAL_E; }
#endif
#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
    else if (cmdST4.ready()) { strcpy(command,cmdST4.getCmd()); strcpy(parameter,cmdST4.getParameter()); cmdST4.flush(); process_command=COMMAND_SERIAL_ST4; }
#endif
    else if (cmdX.ready()) { strcpy(command,cmdX.getCmd()); strcpy(parameter,cmdX.getParameter()); cmdX.flush(); process_command=COMMAND_SERIAL_X; }
    else return;

    if (process_command) {
// Command is two chars followed by an optional parameter...
      commandError=CE_NONE;
// Handles empty and one char replies
      reply[0]=0; reply[1]=0;

//   (char)6 - Special
      if (command[0] == (char)6) {
        if (command[1] == '0') {
          reply[0]=command[1]; strcpy(reply,"CK_FAIL");  // last cmd checksum failed
        } else {
          reply[0]=command[1]; reply[1]=0;               // Equatorial or Horizon mode, A or P
          supress_frame=true;
        }
        boolReply=false;
      } else

// A - Alignment Commands
      if (command[0] == 'A') {
// :AW#       Align Write to EEPROM
//            Returns: 1 on success
        if (command[1] == 'W' && parameter[0] == 0) {
          saveAlignModel();
        } else
// :A?#       Align status
//            Returns: mno#
//            where m is the maximum number of alignment stars
//                  n is the current alignment star (0 otherwise)
//                  o is the last required alignment star when an alignment is in progress (0 otherwise)
        if (command[1] == '?' && parameter[0] == 0) {
          reply[0]=MAX_NUM_ALIGN_STARS;
          reply[1]='0'+alignThisStar;
          reply[2]='0'+alignNumStars;
          reply[3]=0;
          boolReply=false;
        } else
// :A[n]#     Start Telescope Manual Alignment Sequence
//            This is to initiate a n-star alignment for 1..MAX_NUM_ALIGN_STARS:
//            1) Before calling this function, the telescope should be in the polar-home position
//            2) Call this function with the # of align stars you'd like to use
//            3) Set the target location (RA/Dec) to a bright star, etc. (not too near the NCP/SCP)
//            4) Issue a goto command
//            5) Center the star/object using the guide commands (as needed)
//            6) Call :A+# command to accept the correction
//            ( for two+ star alignment )
//            7) Back to #3 above until done, except where possible choose at least one star on both meridian sides
//            Return: 0 on failure
//                    1 on success
        if (command[1] >= '1' && command[1] <= MAX_NUM_ALIGN_STARS && parameter[0] == 0) {
          // set current time and date before calling this routine

          // telescope should be set in the polar home (CWD) for a starting point
          // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
          setHome();

          // start tracking
          trackingState=TrackingSidereal;
          enableStepperDrivers();

          // start align...
          alignNumStars=command[1]-'0';
          alignThisStar=1;
        } else
// :A+#       Align accept target location
//            Return: 0 on failure
//                    1 on success
        if (command[1] == '+' && parameter[0] == 0) {
          if (alignActive()) {
            CommandErrors e=alignStar();
            if (e != CE_NONE) { alignNumStars=0; alignThisStar=0; commandError=e; }
          } else commandError=CE_ALIGN_NOT_ACTIVE;
        } else commandError=CE_CMD_UNKNOWN;
      }
      else

//  $ - Set parameter
// :$BD[n]#   Set Dec/Alt backlash in arc-seconds
//            Return: 0 on failure
//                    1 on success
// :$BR[n]#   Set RA/Azm backlash in arc-seconds
//            Return: 0 on failure
//                    1 on success
//        Set the Backlash values.  Units are arc-seconds
      if (command[0] == '$' && command[1] == 'B') {
        if (atoi2((char*)&parameter[1],&i)) {
          if (i >= 0 && i <= 3600) {
            if (parameter[0] == 'D') {
              reactivateBacklashComp();
              cli(); backlashAxis2=(int)round(((double)i*axis2Settings.stepsPerMeasure)/3600.0); sei();
              nv.writeInt(EE_backlashAxis2,backlashAxis2);
            } else
            if (parameter[0] == 'R') {
              reactivateBacklashComp();
              cli(); backlashAxis1 =(int)round(((double)i*axis1Settings.stepsPerMeasure)/3600.0); sei();
              nv.writeInt(EE_backlashAxis1,backlashAxis1);
            } else commandError=CE_CMD_UNKNOWN;
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else
      
//  % - Return parameter
// :%BD#      Get Dec/Alt Antibacklash value in arc-seconds
//            Return: n#
// :%BR#      Get RA/Azm Antibacklash value in arc-seconds
//            Return: n#
      if (command[0] == '%' && command[1] == 'B') {
        if (parameter[0] == 'D' && parameter[1] == 0) {
            reactivateBacklashComp();
            i=(int)round(((double)backlashAxis2*3600.0)/axis2Settings.stepsPerMeasure);
            if (i < 0) i=0; if (i > 3600) i=3600;
            sprintf(reply,"%d",i);
            boolReply=false;
        } else
        if (parameter[0] == 'R' && parameter[1] == 0) {
            reactivateBacklashComp();
            i=(int)round(((double)backlashAxis1*3600.0)/axis1Settings.stepsPerMeasure);
            if (i < 0) i=0; if (i > 3600) i=3600;
            sprintf(reply,"%d",i);
            boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else
      
//  B - Reticule/Accessory Control
// :B+#       Increase reticule Brightness
//            Returns: Nothing
// :B-#       Decrease Reticule Brightness
//            Returns: Nothing
      if (command[0] == 'B' && (command[1] == '+' || command[1] == '-') && parameter[0] == 0)  {
#if LED_RETICLE >= 0
        int scale;
        if (reticuleBrightness > 255-8) scale=1; else
        if (reticuleBrightness > 255-32) scale=4; else
        if (reticuleBrightness > 255-64) scale=12; else
        if (reticuleBrightness > 255-128) scale=32; else scale=64;
        if (command[1] == '-') reticuleBrightness+=scale;  if (reticuleBrightness > 255) reticuleBrightness=255;
        if (command[1] == '+') reticuleBrightness-=scale;  if (reticuleBrightness < 0)   reticuleBrightness=0;
        analogWrite(ReticlePin,reticuleBrightness);
#endif
        boolReply=false;
      } else 

//  C - Sync Control
// :CS#       Synchonize the telescope with the current right ascension and declination coordinates
//            Returns: Nothing (Sync's fail silently)
// :CM#       Synchonize the telescope with the current database object (as above)
//            Returns: "N/A#" on success, "En#" on failure where n is the error code per the :MS# command
      if (command[0] == 'C' && (command[1] == 'S' || command[1] == 'M') && parameter[0] == 0)  {
        if (parkStatus == NotParked && trackingState != TrackingMoveTo) {

          newTargetRA=origTargetRA; newTargetDec=origTargetDec;
#if TELESCOPE_COORDINATES == TOPOCENTRIC
          topocentricToObservedPlace(&newTargetRA,&newTargetDec);
#endif

          CommandErrors e;
          if (alignActive()) {
            e=alignStar();
            if (e != CE_NONE) { alignNumStars=0; alignThisStar=0; commandError=e; }
          } else { 
            e=syncEqu(newTargetRA,newTargetDec);
          }

          if (command[1] == 'M') {
              if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) { reply[0]='E'; reply[1]=(char)(e-CE_GOTO_ERR_BELOW_HORIZON)+'1'; reply[2]=0; }
              if (e == CE_NONE) strcpy(reply,"N/A");
          }

          boolReply=false;
        }
      } else 

//  D - Distance Bars
// :D#        Return: "\0x7f#" if the mount is moving, otherwise "#".
      if (command[0] == 'D' && command[1] == 0)  { if (trackingState == TrackingMoveTo) { reply[0]=(char)127; reply[1]=0; } else { reply[0]='#'; reply[1]=0; supress_frame=true; } boolReply=false; } else 

//  E - Enter special mode
      if (command[0] == 'E') {
// :EC[s]# Echo string [c] on DebugSer.
//            Return: Nothing
        if (command[1] == 'C') {
          // spaces are encoded as '_'
          for (unsigned int i=0; i < strlen(parameter); i++) if (parameter[i]=='_') parameter[i]=' ';
          // a newline is encoded as '&' in the last char of message
          int l=strlen(parameter);
          if (l > 0 && parameter[l-1] == '&') { parameter[l-1]=0; DL(parameter); } else D(parameter);
          boolReply=false;
        } else
// :ENVRESET# Wipe flash.  OnStep must be at home and tracking turned off for this command to work.
        if (command[1] == 'N' && parameter[0] == 'V' && parameter[1] == 'R' && parameter[2] == 'E' && parameter[3] == 'S' && parameter[4] == 'E' && parameter[5] == 'T' && parameter[6] == 0) {
          if (atHome || parkStatus == Parked) {
            nv.writeLong(EE_autoInitKey,0);
            strcpy(reply,"NV will be wiped on next boot.");
            boolReply=false;
          } else commandError=CE_NOT_PARKED_OR_AT_HOME; } else
#if SERIAL_B_ESP_FLASHING == ON
// :ESPFLASH# ESP8266 device flash mode.  OnStep must be at home and tracking turned off for this command to work.
//            Return: 1 on completion (after up to one minute from start of command.)
        if (command[1] == 'S' && parameter[0] == 'P' && parameter[1] == 'F' && parameter[2] == 'L' && parameter[3] == 'A' && parameter[4] == 'S' && parameter[5] == 'H' && parameter[6] == 0) {
          if (atHome || parkStatus == Parked) {
            SerialA.println("The ESP8266 will now be placed in flash upload mode (at 115200 Baud.)");
            SerialA.println("Arduino's 'Tools -> Upload Speed' should be set to 115200 Baud.");
            SerialA.println("Waiting for data, you have one minute to start the upload.");
            delay(1000);
            fa.go(false); // flash the addon
            SerialA.println("ESP8266 reset and in run mode, resuming OnStep operation...");
            delay(1000);
          } else commandError=CE_NOT_PARKED_OR_AT_HOME;
        } else
#endif
        commandError=CE_CMD_UNKNOWN;
      } else

// :FA#       Active?
//            Return: 0 on failure
//                    1 on success
        if (command[0] == 'F' && command[1] == 'A' && parameter[0] == 0) {
#if FOCUSER1 != ON
          commandError=CE_0;
#endif
        } else
// :fA#       Active?
//            Return: 0 on failure
//                    1 on success
        if (command[0] == 'f' && command[1] == 'A' && parameter[0] == 0) {
#if FOCUSER2 != ON
          commandError=CE_0;
#endif
        } else
// F,f - Focuser1 and Focuser2 Commands
#if FOCUSER1 == ON
      if (command[0] == 'F' || command[0]=='f') {

        focuser *foc = NULL;
        if (command[0] == primaryFocuser) foc = &foc1;
#if FOCUSER2 == ON
        else if (command[0] == secondaryFocuser) foc = &foc2;
#endif

        // check that we have a focuser selected and for commands that shouldn't have a parameter
        if (foc != NULL && !(strchr("TpIMtuQF1234+-GZHh",command[1]) && parameter[0] != 0)) {

        // get ready for commands that convert to microns or steps (these commands are upper-case for microns OR lower-case for steps)
        double spm = foc->getStepsPerMicro(); if (strchr("bdgimrs",command[1])) spm = 1.0;

// :FA[n]#    Select primary focuser where [n] = 1 or 2
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'A') {
          if (parameter[0] == '1' && parameter[1] == 0) { primaryFocuser=toupper(secondaryFocuser); secondaryFocuser=tolower(primaryFocuser); }
#if FOCUSER2 == ON
          else if (parameter[0] == '2' && parameter[1] == 0) { primaryFocuser=tolower(secondaryFocuser); secondaryFocuser=toupper(primaryFocuser); }
#endif
          else commandError=CE_PARAM_RANGE;
        } else
// :Fa#       Get primary focuser
//            Returns: 1 if primary focuser is focuser 1, 0 otherwise
        if (command[1] == 'a') { if (primaryFocuser != 'F') commandError=CE_0; } else

// :FT#       Get status
//            Returns: M# (for moving) or S# (for stopped)
        if (command[1] == 'T') { if (foc->moving()) strcpy(reply,"M"); else strcpy(reply,"S"); boolReply=false; } else
// :Fp#       Get mode
//            Return: 0 for absolute
//                    1 for pseudo absolute
        if (command[1] == 'p') { if (!foc->isDcFocuser()) commandError=CE_0; } else

// :FI#       Get full in position (in microns or steps)
//            Returns: n#
        if (toupper(command[1]) == 'I') { sprintf(reply,"%ld",(long)round(foc->getMin()/spm)); boolReply=false; } else
// :FM#       Get max position (in microns or steps)
//            Returns: n#
        if (toupper(command[1]) == 'M') { sprintf(reply,"%ld",(long)round(foc->getMax()/spm)); boolReply=false; } else

// :Fe#       Get focuser temperature differential
//            Returns: n# temperature in deg. C
        if (command[1] == 'e') { if (foc->getTcfEnable()) dtostrf(ambient.getTelescopeTemperature()-foc->getTcfT0(),3,1,reply); else dtostrf(0.0,3,1,reply); boolReply=false; } else
// :Ft#       Get focuser temperature
//            Returns: n# temperature in deg. C
        if (command[1] == 't') { dtostrf(ambient.getTelescopeTemperature(),3,1,reply); boolReply=false; } else
// :Fu#       Get focuser microns per step
//            Returns: n.n#
        if (command[1] == 'u') { dtostrf(1.0/foc->getStepsPerMicro(),7,5,reply); boolReply=false; } else
// :FB#       Get focuser backlash amount (in steps or microns)
//            Return: n#
        if (toupper(command[1]) == 'B' && parameter[0] == 0) { sprintf(reply,"%ld",(long)round(foc->getBacklash()/spm)); boolReply=false; } else
// :FB[n]#    Set focuser backlash amount (in steps or microns)
//            Return: 0 on failure
//                    1 on success
        if (toupper(command[1]) == 'B') { long l = atol(parameter)*spm; if (!foc->setBacklash(l)) commandError=CE_PARAM_RANGE; } else
// :FC#       Get focuser temperature compensation coefficient
//            Return: n.n#
        if (command[1] == 'C' && parameter[0] == 0) { dtostrf(foc->getTcfCoef(),7,5,reply); boolReply=false; } else
// :FC[sn.n]# Set focuser temperature compensation coefficient in um per deg. C (+ moves out as temperature falls)
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'C') { f = atof(parameter); if (!foc->setTcfCoef(f)) commandError=CE_PARAM_RANGE; } else
// :Fc#       Get focuser temperature compensation enable status
//            Return: 0 if disabled
//                    1 if enabled
        if (command[1] == 'c' && parameter[0] == 0) { if (!foc->getTcfEnable()) commandError=CE_0; } else
// :Fc[n]#    Enable/disable focuser temperature compensation where [n] = 0 or 1
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'c' && parameter[1] == 0) { foc->setTcfEnable(parameter[0] != '0'); } else
// :FD#       Get focuser temperature compensation deadband amount (in steps or microns)
//            Return: n#
        if (toupper(command[1]) == 'D' && parameter[0] == 0) { sprintf(reply,"%ld",(long)round(foc->getTcfDeadband()/spm)); boolReply=false; } else
// :FD[n]#    Set focuser temperature compensation deadband amount (in steps or microns)
//            Return: 0 on failure
//                    1 on success
        if (toupper(command[1]) == 'D') { long l = atol(parameter)*spm; if (!foc->setTcfDeadband(l)) commandError=CE_PARAM_RANGE; } else

// :FP#       Get focuser DC Motor Power Level (in %)
//            Returns: n#
// :FP[n]#    Set focuser DC Motor Power Level (in %)
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'P') {
          if (foc->isDcFocuser()) {
            if (parameter[0] == 0) {
              sprintf(reply,"%d",(int)foc->getDcPower()); boolReply=false; 
            } else {
              i=atol(parameter);
              if (!foc->setDcPower(i)) commandError=CE_PARAM_RANGE; 
            }
          } else commandError=CE_CMD_UNKNOWN;
        } else

// :FQ#       Stop the focuser
//            Returns: Nothing
        if (command[1] == 'Q') { foc->stopMove(); boolReply=false; } else

// :FF#       Set focuser for fast motion (1mm/s)
//            Returns: Nothing
        if (command[1] == 'F') { foc->setMoveRate(1000); boolReply=false; } else
// :FS#       Set focuser for slow motion (0.01mm/s)
//            Returns: Nothing
        if (command[1] == 'S' && parameter[0] == 0) { foc->setMoveRate(10); boolReply=false; } else
// :F[n]#     Set focuser move rate, where n = 1 for finest, 2 for 0.01mm/second, 3 for 0.1mm/second, 4 for 1mm/second
//            Returns: Nothing
        if (command[1] >= '1' && command[1] <= '4') { int p[] = {1,10,100,1000}; foc->setMoveRate(p[command[1] - '1']); boolReply=false; } else
// :F+#       Move focuser in (toward objective)
//            Returns: Nothing
        if (command[1] == '+') { foc->startMoveIn(); boolReply=false; } else
// :F-#       Move focuser out (away from objective)
//            Returns: Nothing
        if (command[1] == '-') { foc->startMoveOut(); boolReply=false; } else

// :FG#       Get focuser current position (in microns or steps)
//            Returns: sn#
        if (toupper(command[1]) == 'G') { sprintf(reply,"%ld",(long)round(foc->getPosition()/spm)); boolReply=false; } else
// :FR[sn]#   Set focuser target position relative (in microns or steps)
//            Returns: Nothing
        if (toupper(command[1]) == 'R') { foc->relativeTarget((double)atol(parameter)*spm); boolReply=false; } else
// :FS[n]#    Set focuser target position (in microns or steps)
//            Return: 0 on failure
//                    1 on success
        if (toupper(command[1]) == 'S') { if (!foc->setTarget((double)atol(parameter)*spm)) commandError=CE_SLEW_ERR_IN_STANDBY; } else
// :FZ#       Set focuser position as zero
//            Returns: Nothing
        if (command[1] == 'Z') { foc->setPosition(0); boolReply=false; } else
// :FH#       Set focuser position as half-travel
//            Returns: Nothing
        if (command[1] == 'H') { foc->setPosition((foc->getMax()+foc->getMin())/2.0); boolReply=false; } else
// :Fh#       Set focuser target position at half-travel
//            Returns: Nothing
        if (command[1] == 'h') { foc->setTarget((foc->getMax()+foc->getMin())/2.0); boolReply=false; } else commandError=CE_CMD_UNKNOWN;
        
        } else commandError=CE_CMD_UNKNOWN;
      } else
#endif

// G - Get Telescope Information
      if (command[0] == 'G') {

// :GA#       Get Telescope Altitude
//            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
//            The current scope altitude
      if (command[1] == 'A' && parameter[0] == 0)  { getHor(&f,&f1); doubleToDms(reply,&f,false,true,precision); boolReply=false; } else
// :GB#       Get Fastest Recommended Baud rate
//            Returns: n
//            The baud rate code
      if (command[1] == 'B' && parameter[0] == 0)  { 
#ifdef HAL_SLOW_PROCESSOR
        strcpy(reply,"4");
#else
        strcpy(reply,"0");
#endif
        boolReply=false;
        supress_frame=true;
      } else
// :Ga#       Get Local Time in 12 hour format
//            Returns: HH:MM:SS#
      if (command[1] == 'a' && parameter[0] == 0)  { LMT=timeRange(UT1-timeZone); if (LMT > 12.0) LMT-=12.0; doubleToHms(reply,&LMT,PM_HIGH); boolReply=false; } else 
// :GC#       Get the current local calendar date
//            Returns: MM/DD/YY#
      if (command[1] == 'C' && parameter[0] == 0) { 
        LMT=UT1-timeZone;
        // correct for day moving forward/backward... this works for multipule days of up-time
        double J=JD;
        int y,m,d;
        while (LMT >= 24.0) { LMT=LMT-24.0; J=J-1.0; } 
        if    (LMT<0.0)   { LMT=LMT+24.0; J=J+1.0; }
        greg(J,&y,&m,&d); y-=2000; if (y >= 100) y-=100;
        sprintf(reply,"%02d/%02d/%02d",m,d,y); 
        boolReply=false; 
      } else 
// :Gc#       Get the current local time format
//            Returns: 24#
      if (command[1] == 'c' && parameter[0] == 0) {
        strcpy(reply,"24");
        boolReply=false; 
       } else
// :GD#       Get Telescope Declination
//            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
// :GDH#      Get Telescope Declination
//            Returns: sDD*MM:SS.SSSS# (high precision)
      if (command[1] == 'D')  {
#ifdef HAL_SLOW_PROCESSOR
        if ((long)(millis()-_coord_t) > 500)
#else
        if ((long)(millis()-_coord_t) > 50)
#endif
        {
          getEqu(&f,&f1,false);
#if TELESCOPE_COORDINATES == TOPOCENTRIC
          observedPlaceToTopocentric(&f,&f1);
#endif
          _ra=f/15.0; _dec=f1; _coord_t=millis(); 
        }
        if (parameter[0] == 0) {
          doubleToDms(reply,&_dec,false,true,precision); boolReply=false; 
        } else
        if ((parameter[0] == 'e' || parameter[0] == 'H') && parameter[1] == 0) {
          doubleToDms(reply,&_dec,false,true,PM_HIGHEST); boolReply=false; 
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// :Gd#       Get Currently Selected Target Declination
//            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
// :GdH#      Get Currently Selected Target Declination
//            Returns: sDD*MM:SS.SSS# (high precision)
      if (command[1] == 'd')  {
        if (parameter[0] == 0) {
          doubleToDms(reply,&origTargetDec,false,true,precision); boolReply=false; 
        } else
        if ((parameter[0] == 'e' || parameter[0] == 'H') && parameter[1] == 0) {
          doubleToDms(reply,&origTargetDec,false,true,PM_HIGHEST); boolReply=false; 
        } else commandError=CE_CMD_UNKNOWN;
      } else
// :GE#       Get last command error numeric code
//            Returns: CC#
      if (command[1] == 'E' && parameter[0] == 0) {
        CommandErrors e=CE_REPLY_UNKNOWN;
        if (process_command == COMMAND_SERIAL_A) e=cmdA.lastError; else
#ifdef HAL_SERIAL_B_ENABLED
        if (process_command == COMMAND_SERIAL_B) e=cmdB.lastError; else
#endif
#ifdef HAL_SERIAL_C_ENABLED
        if (process_command == COMMAND_SERIAL_C) e=cmdC.lastError; else
#endif
#ifdef HAL_SERIAL_D_ENABLED
        if (process_command == COMMAND_SERIAL_D) e=cmdD.lastError; else
#endif
#ifdef HAL_SERIAL_E_ENABLED
        if (process_command == COMMAND_SERIAL_E) e=cmdE.lastError; else
#endif
#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
        if (process_command == COMMAND_SERIAL_ST4) e=cmdST4.lastError; else
#endif
        if (process_command == COMMAND_SERIAL_X) e=cmdX.lastError;
        sprintf(reply,"%02d",e);
        commandError=CE_NULL;
        boolReply=false; 
      } else
// :GG#       Get UTC offset time, the number of decimal hours to add to local time to convert to UTC
//            Returns: sHH#
      if (command[1] == 'G' && parameter[0] == 0)  { 
        timeZoneToHM(reply,timeZone);
        boolReply=false; 
      } else 
// :Gg#       Get Current Site Longitude, east is negative
//            Returns: sDDD*MM#
// :GgH#      Get current site Longitude
//            Returns: sDD*MM:SS.SSS# (high precision)
      if (command[1] == 'g')  {
        if (parameter[0] == 0) {
          doubleToDms(reply,&longitude,true,true,PM_LOW); boolReply=false;
        } else
        if (parameter[0] == 'H' && parameter[1] == 0) {
          doubleToDms(reply,&longitude,true,true,PM_HIGHEST); boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// :Gh#       Get Horizon Limit, the minimum elevation of the mount relative to the horizon
//            Returns: sDD*#
      if (command[1] == 'h' && parameter[0] == 0)  { sprintf(reply,"%+02d*",minAlt); boolReply=false; } else
// :GL#       Get Local Time in 24 hour format
//            Returns: HH:MM:SS#
// :GLH#      Get Local Time in 24 hour format
//            Returns: HH:MM:SS.SSSS# (high precision)
//            On devices with single precision fp several days up-time will cause loss of precision as additional mantissa digits are needed to represent hours
//            Devices with double precision fp are limitated by sidereal clock overflow which takes 249 days
      if (command[1] == 'L') {
        LMT=timeRange(UT1-timeZone);
        if ( parameter[0] == 0)  {
          doubleToHms(reply,&LMT,PM_HIGH); boolReply=false;
        } else 
        if ((parameter[0] == 'a' || parameter[0] == 'H') && parameter[1] == 0) {
          doubleToHms(reply,&LMT,PM_HIGHEST); boolReply=false;
        }
      }
      else
// :GM#       Get site 1 name
// :GN#       Get site 2 name
// :GO#       Get site 3 name
// :GP#       Get site 4 name
//            Returns: s#
      if ((command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P') && parameter[0] == 0)  {
        i=command[1]-'M';
        nv.readString(EE_sites+i*25+9,reply); 
        if (reply[0] == 0) { strcat(reply,"None"); }
        boolReply=false; 
      } else
// :Gm#       Gets the meridian pier-side
//            Returns: E#, W#, N# (none/parked)
      if (command[1] == 'm' && parameter[0] == 0)  {
        reply[0]='?'; reply[1]=0;
        if (getInstrPierSide() == PierSideNone) reply[0]='N';
        if (getInstrPierSide() == PierSideEast) reply[0]='E';
        if (getInstrPierSide() == PierSideWest) reply[0]='W';
        boolReply=false; } else
// :Go#       Get Overhead Limit
//            Returns: DD*#
//            The highest elevation above the horizon that the telescope will goto
      if (command[1] == 'o' && parameter[0] == 0)  { sprintf(reply,"%02d*",maxAlt); boolReply=false; } else
// :GR#       Get Telescope RA
//            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
// :GRH#      Get Telescope RA High Precision
//            Returns: HH:MM:SS.SSSS#
      if (command[1] == 'R')  {
#ifdef HAL_SLOW_PROCESSOR
        if ((long)(millis()-_coord_t) > 500)
#else
        if ((long)(millis()-_coord_t) > 50)
#endif
        {
          getEqu(&f,&f1,false);
#if TELESCOPE_COORDINATES == TOPOCENTRIC
          observedPlaceToTopocentric(&f,&f1);
#endif
          _ra=f/15.0; _dec=f1; _coord_t=millis(); 
        }
        if (parameter[0] == 0) {
           doubleToHms(reply,&_ra,precision); boolReply=false;  
        } else
        if ((parameter[0] == 'a' || parameter[0] == 'H') && parameter[1] == 0) {
          doubleToHms(reply,&_ra,PM_HIGHEST); boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// :Gr#       Get current/target object RA
//            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
// :GrH#      Get Telescope RA
//            Returns: HH:MM:SS.SSSS# (high precision)
      if (command[1] == 'r')  {
        f=origTargetRA; f/=15.0;
        if (parameter[0] == 0) {
           doubleToHms(reply,&f,precision); boolReply=false;
        } else
        if ((parameter[0] == 'a' || parameter[0] == 'H') && parameter[1] == 0) {
          doubleToHms(reply,&f,PM_HIGHEST); boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// :GS#       Get the Sidereal Time as sexagesimal value in 24 hour format
//            Returns: HH:MM:SS#
// :GSa#      Get the Sidereal Time as sexagesimal value in 24 hour format, with high precision
//            Returns HH:MM:SS.ss#
      if (command[1] == 'S') {
        f = LST();
        if (parameter[0] == 0) {
          doubleToHms(reply,&f,PM_HIGH); boolReply=false;
        } else 
        if (parameter[0] == 'a' && parameter[1] == 0) {
          doubleToHms(reply,&f,PM_HIGHEST); boolReply=false;
        }
      } else
// :GT#       Get tracking rate, 0.0 unless TrackingSidereal
//            Returns: n.n# (OnStep returns more decimal places than LX200 standard)
      if (command[1] == 'T' && parameter[0] == 0)  {
        char temp[10];
        f=getTrackingRate60Hz();
        dtostrf(f,0,5,temp);
        strcpy(reply,temp);
        boolReply=false;
      } else 
// :Gt#       Get current site Latitude, positive for North latitudes
//            Returns: sDD*MM#
// :GtH#      Get current site Latitude, positive for North latitudes
//            Returns: sDD*MM:SS.SSS# (high precision)
      if (command[1] == 't')  {
        if (parameter[0] == 0) {
          doubleToDms(reply,&latitude,false,true,PM_LOW); boolReply=false;
        } else
        if (parameter[0] == 'H' && parameter[1] == 0) {
          doubleToDms(reply,&latitude,false,true,PM_HIGHEST); boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// :GU#       Get telescope Status
//            Returns: s#
      if (command[1] == 'U' && parameter[0] == 0)  {
        i=0;
        if (trackingState != TrackingSidereal &&
          !(trackingState == TrackingMoveTo && lastTrackingState == TrackingSidereal)) reply[i++]='n';       // [n]ot tracking
        if (trackingState != TrackingMoveTo && !trackingSyncInProgress())  reply[i++]='N';                   // [N]o goto
        const char *parkStatusCh = "pIPF";       reply[i++]=parkStatusCh[parkStatus];                        // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (pecRecorded)                         reply[i++]='R';                                             // PEC data has been [R]ecorded
        if (syncToEncodersOnly)                  reply[i++]='e';                                             // sync to [e]ncoders only
        if (atHome)                              reply[i++]='H';                                             // at [H]ome
        if (ppsSynced)                           reply[i++]='S';                                             // PPS [S]ync
        if (isPulseGuiding())                    reply[i++]='G';                                             // pulse [G]uide active
#if MOUNT_TYPE != ALTAZM
        if (rateCompensation == RC_REFR_RA)      { reply[i++]='r'; reply[i++]='s'; }                         // [r]efr enabled [s]ingle axis
        if (rateCompensation == RC_REFR_BOTH)    { reply[i++]='r'; }                                         // [r]efr enabled
        if (rateCompensation == RC_FULL_RA)      { reply[i++]='t'; reply[i++]='s'; }                         // on[t]rack enabled [s]ingle axis
        if (rateCompensation == RC_FULL_BOTH)    { reply[i++]='t'; }                                         // on[t]rack enabled
#endif
        if (waitingHome)                         reply[i++]='w';                                             // [w]aiting at home 
        if (pauseHome)                           reply[i++]='u';                                             // pa[u]se at home enabled?
        if (soundEnabled)                        reply[i++]='z';                                             // bu[z]zer enabled?
#if MOUNT_TYPE == GEM
        if (autoMeridianFlip)                    reply[i++]='a';                                             // [a]uto meridian flip
#endif
#if AXIS1_PEC == ON
        const char *pch = PECStatusStringAlt; reply[i++]=pch[pecStatus];                                     // PEC Status one of "/,~;^" (/)gnore, ready to (,)lay, (~)laying, ready to (;)ecord, (^)ecording
#endif
        // provide mount type
#if MOUNT_TYPE == GEM
        reply[i++]='E';
#elif MOUNT_TYPE == FORK
        reply[i++]='K';
#elif MOUNT_TYPE == ALTAZM
        reply[i++]='A';
#endif

        // provide pier side info.
        if (getInstrPierSide() == PierSideNone) reply[i++]='o'; else                                         // pier side n[o]ne
        if (getInstrPierSide() == PierSideEast) reply[i++]='T'; else                                         // pier side eas[T]
        if (getInstrPierSide() == PierSideWest) reply[i++]='W';                                              // pier side [W]est

        // provide pulse-guide rate
        reply[i++]='0'+getPulseGuideRate();

        // provide guide rate
        reply[i++]='0'+getGuideRate();

        // provide general error
        reply[i++]='0'+generalError;
        reply[i++]=0;
        boolReply=false;

      } else
// :Gu#       Get bit packed telescope status
//            Returns: s#
      if (command[1] == 'u' && parameter[0] == 0)  {
        memset(reply,(char)0b10000000,9);
        if (trackingState != TrackingSidereal &&
          !(trackingState == TrackingMoveTo && lastTrackingState == TrackingSidereal)) reply[0]|=0b10000001; // Not tracking
        if (trackingState != TrackingMoveTo && !trackingSyncInProgress())  reply[0]|=0b10000010;             // No goto
        if (ppsSynced)                               reply[0]|=0b10000100;                                   // PPS sync
        if (isPulseGuiding())                        reply[0]|=0b10001000;                                   // pulse guide active
#if MOUNT_TYPE != ALTAZM
        if (rateCompensation == RC_REFR_RA)          reply[0]|=0b11010000;                                   // Refr enabled Single axis
        if (rateCompensation == RC_REFR_BOTH)        reply[0]|=0b10010000;                                   // Refr enabled
        if (rateCompensation == RC_FULL_RA)          reply[0]|=0b11100000;                                   // OnTrack enabled Single axis
        if (rateCompensation == RC_FULL_BOTH)        reply[0]|=0b10100000;                                   // OnTrack enabled
#endif
        if (rateCompensation == RC_NONE) {
          double tr=getTrackingRate60Hz();
          if (fabs(tr-57.900)<0.001)                 reply[1]|=0b10000001; else                              // Lunar rate selected
          if (fabs(tr-60.000)<0.001)                 reply[1]|=0b10000010; else                              // Solar rate selected
          if (fabs(tr-60.136)<0.001)                 reply[1]|=0b10000011;                                   // King rate selected
        }
        
        if (syncToEncodersOnly)                      reply[1]|=0b10000100;                                   // sync to encoders only
        if (atHome)                                  reply[2]|=0b10000001;                                   // At home
        if (waitingHome)                             reply[2]|=0b10000010;                                   // Waiting at home
        if (pauseHome)                               reply[2]|=0b10000100;                                   // Pause at home enabled?
        if (soundEnabled)                            reply[2]|=0b10001000;                                   // Buzzer enabled?
#if MOUNT_TYPE == GEM
        if (autoMeridianFlip)                        reply[2]|=0b10010000;                                   // Auto meridian flip
#endif
        if (pecRecorded)                             reply[2]|=0b10100000;                                   // PEC data has been recorded

        // provide mount type
#if MOUNT_TYPE == GEM
                                                     reply[3]|=0b10000001;                                   // GEM
#elif MOUNT_TYPE == FORK
                                                     reply[3]|=0b10000010;                                   // FORK
#elif MOUNT_TYPE == ALTAZM
                                                     reply[3]|=0b10001000;                                   // ALTAZM
#endif

        // provide pier side info.
        if (getInstrPierSide() == PierSideNone)      reply[3]|=0b10010000; else                              // Pier side none
        if (getInstrPierSide() == PierSideEast)      reply[3]|=0b10100000; else                              // Pier side east
        if (getInstrPierSide() == PierSideWest)      reply[3]|=0b11000000;                                   // Pier side west

#if AXIS1_PEC == ON
        reply[4]=pecStatus|0b10000000;                                                                       // PEC status: 0 ignore, 1 ready play, 2 playing, 3 ready record, 4 recording
#endif
        reply[5]=parkStatus|0b10000000;                                                                      // Park status: 0 not parked, 1 parking in-progress, 2 parked, 3 park failed
        reply[6]=getPulseGuideRate()|0b10000000;                                                             // Pulse-guide rate
        reply[7]=getGuideRate()|0b10000000;                                                                  // Guide rate
        reply[8]=generalError|0b10000000;                                                                    // General error
        reply[9]=0;
        boolReply=false;
      } else
// :GVD#      Get Telescope Firmware Date
//            Returns: MTH DD YYYY#
// :GVM#      General Message
//            Returns: s# (where s is a string up to 16 chars)
// :GVN#      Get Telescope Firmware Number
//            Returns: M.mp#
// :GVP#      Get Telescope Product Name
//            Returns: s#
// :GVT#      Get Telescope Firmware Time
//            Returns: HH:MM:SS#
      if (command[1] == 'V') {
        if (parameter[1] == 0) {
          if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
          if (parameter[0] == 'M') sprintf(reply,"OnStep %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
          if (parameter[0] == 'N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
          if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
          if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else commandError=CE_CMD_UNKNOWN;
        } else commandError=CE_CMD_UNKNOWN;
        boolReply=false; 
      } else
// :GW#       Get alignment status
//            Returns: [mount][tracking][alignment]#
//            Where mount: A-AltAzm, P-Fork, G-GEM
//                  tracking: T-tracking, N-not tracking
//                  alignment: 0-needs alignment, 1-one star aligned, 2-two star aligned, >= 3-three star aligned
       if (command[1] == 'W' && parameter[0] == 0) {
        // mount type
#if MOUNT_TYPE == GEM
        reply[0]='G';
#elif MOUNT_TYPE == FORK
        reply[0]='P';
#elif MOUNT_TYPE == ALTAZM
        reply[0]='A';
#endif
        // tracking
        if (trackingState != TrackingSidereal || trackingSyncInProgress()) reply[1]='N'; else reply[1]='T';
        // align status
        i=alignThisStar-1; if (i<0) i=0; if (i > 3) i=3; reply[2]='0'+i;
        reply[3]=0;
        boolReply=false;
       } else
// :GX[II]#   Get OnStep value where II is the numeric index
//            Returns: n (numeric value, possibly floating point)
      if (command[1] == 'X')  {
        if (parameter[2] == (char)0) {
          if (parameter[0] == '0') { // 0n: Align Model
            static int star=0;
            switch (parameter[1]) {
              case '0': sprintf(reply,"%ld",(long)(Align.ax1Cor*3600.0)); boolReply=false; break;         // ax1Cor
              case '1': sprintf(reply,"%ld",(long)(Align.ax2Cor*3600.0)); boolReply=false; break;         // ax2Cor
              case '2': sprintf(reply,"%ld",(long)(Align.altCor*3600.0)); boolReply=false; break;         // altCor
              case '3': sprintf(reply,"%ld",(long)(Align.azmCor*3600.0)); boolReply=false; break;         // azmCor
              case '4': sprintf(reply,"%ld",(long)(Align.doCor*3600.0)); boolReply=false; break;          // doCor
              case '5': sprintf(reply,"%ld",(long)(Align.pdCor*3600.0)); boolReply=false; break;          // pdCor
#if MOUNT_TYPE == FORK || MOUNT_TYPE == ALTAZM
              case '6': sprintf(reply,"%ld",(long)(Align.dfCor*3600.0)); boolReply=false; break;          // ffCor
              case '7': sprintf(reply,"%ld",(long)(0)); boolReply=false; break;                           // dfCor
#else
              case '6': sprintf(reply,"%ld",(long)(0)); boolReply=false; break;                           // ffCor
              case '7': sprintf(reply,"%ld",(long)(Align.dfCor*3600.0)); boolReply=false; break;          // dfCor
#endif
              case '8': sprintf(reply,"%ld",(long)(Align.tfCor*3600.0)); boolReply=false; break;          // tfCor

              // Number of stars, reset to first star
              case '9': { int n=0; if (alignThisStar > alignNumStars) n=alignNumStars; sprintf(reply,"%ld",(long)(n)); star=0; boolReply=false; } break;
              case 'A': { double f=(Align.actual[star].ha*Rad)/15.0; doubleToHms(reply,&f,PM_HIGH); boolReply=false; } break;           // Star  #n HA
              case 'B': { double f=(Align.actual[star].dec*Rad); doubleToDms(reply,&f,false,true,precision);  boolReply=false; } break; // Star  #n Dec
              case 'C': { double f=(Align.mount[star].ha*Rad)/15.0;  doubleToHms(reply,&f,PM_HIGH); boolReply=false; } break;           // Mount #n HA
              case 'D': { double f=(Align.mount[star].dec*Rad);  doubleToDms(reply,&f,false,true,precision);  boolReply=false; } break; // Mount #n Dec
              case 'E': sprintf(reply,"%ld",(long)(Align.mount[star].side)); star++; boolReply=false; break;                            // Mount PierSide (and increment n)
              default: commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == '4') { // 4n: Encoder
            switch (parameter[1]) {
              case '0': getEnc(&f,&f1); doubleToDms(reply,&f,true,true,precision); boolReply=false; break;  // Get formatted absolute Axis1 angle
              case '1': getEnc(&f,&f1); doubleToDms(reply,&f1,true,true,precision); boolReply=false; break; // Get formatted absolute Axis2 angle 
              case '2': getEnc(&f,&f1); dtostrf(f,0,6,reply); boolReply=false; break;                       // Get absolute Axis1 angle in degrees
              case '3': getEnc(&f,&f1); dtostrf(f1,0,6,reply); boolReply=false; break;                      // Get absolute Axis2 angle in degrees
              case '9': cli(); dtostrf(trackingTimerRateAxis1,1,8,reply); sei(); boolReply=false; break;    // Get current tracking rate
              default:  commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == '8') { // 8n: Date/Time
            switch (parameter[1]) {
              case '0': f=timeRange(UT1); doubleToHms(reply,&f,PM_HIGH); boolReply=false; break;          // UTC time
              case '1': f1=JD; f=UT1; while (f >= 24.0) { f-=24.0; f1+=1; } while (f < 0.0) { f+=24.0; f1-=1; } greg(f1,&i2,&i,&i1); i2=(i2/99.99999-floor(i2/99.99999))*100; sprintf(reply,"%02d/%02d/%02d",i,i1,i2); boolReply=false; break; // UTC date
              case '9': if (dateWasSet && timeWasSet) commandError=CE_0; break;                              // Get Date/Time status, returns 0=known or 1=unknown
              default:  commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == '9') { // 9n: Misc.
            switch (parameter[1]) {
              case '0': dtostrf(guideRates[currentPulseGuideRate]/15.0,2,2,reply); boolReply=false; break;// pulse-guide rate
              case '1': sprintf(reply,"%i",pecValue); boolReply=false; break;                             // pec analog value
              case '2': dtostrf(maxRate/16.0,3,3,reply); boolReply=false; break;                          // MaxRate (current)
              case '3': dtostrf((double)maxRateBaseActual,3,3,reply); boolReply=false; break;             // maxRateBaseActual (default)
              case '4': if (meridianFlip == MeridianFlipNever) { sprintf(reply,"%d N",getInstrPierSide()); } else { sprintf(reply,"%d",getInstrPierSide()); } boolReply=false; break; // pierSide (N if never)
              case '5': sprintf(reply,"%i",(int)autoMeridianFlip); boolReply=false; break;                // autoMeridianFlip
              case '6':                                                                                   // preferred pier side
                if (preferredPierSide == EAST) strcpy(reply,"E"); else
                if (preferredPierSide == WEST) strcpy(reply,"W"); else strcpy(reply,"B");
                boolReply=false; break;
              case '7': dtostrf(slewSpeed,3,1,reply); boolReply=false; break;                             // slew speed
              case '8':
#if ROTATOR == ON
  #if MOUNT_TYPE == ALTAZM
                strcpy(reply,"D");
  #else
                strcpy(reply,"R");
  #endif
#else
                strcpy(reply,"N");
#endif
                boolReply=false; break; // rotator availablity 2=rotate/derotate, 1=rotate, 0=off
              case '9': dtostrf(maxRateLowerLimit()/16.0,3,3,reply); boolReply=false; break;              // MaxRate (fastest/lowest)
              case 'A': dtostrf(ambient.getTemperature(),3,1,reply); boolReply=false; break;              // temperature in deg. C
              case 'B': dtostrf(ambient.getPressure(),3,1,reply); boolReply=false; break;                 // pressure in mb
              case 'C': dtostrf(ambient.getHumidity(),3,1,reply); boolReply=false; break;                 // relative humidity in %
              case 'D': dtostrf(ambient.getAltitude(),3,1,reply); boolReply=false; break;                 // altitude in meters
              case 'E': dtostrf(ambient.getDewPoint(),3,1,reply); boolReply=false; break;                 // dew point in deg. C
              case 'F': { float t=HAL_MCU_Temperature(); if (t > -999) { dtostrf(t,1,0,reply); boolReply=false; } else commandError=CE_0; } break; // internal MCU temperature in deg. C
              default:  commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == 'A') { // :GXAn: Get axis settings
            int p=parameter[1]-'1';
            if (p >= 0 && p <= 5) {
              uint8_t state=nv.read(EE_settingsRuntime);
              // check for all axes set to revert
              if (state&1) {
                // check for this axis set to revert
                if (!(state&(1<<(p+1)))) {
                  if (p == 0 || p == 1 || (p == 2 && ROTATOR == ON) || (p == 3 && FOCUSER1 == ON) || (p == 4 && FOCUSER2 == ON)) {
                    axisSettings axis;
                    nv.readBytes(EE_settingsAxis1+(p*17),(byte*)&axis,sizeof(axis));
                    sprintf(reply,"%ld.%03ld,%d,%d,%d,%d,%d",(long)axis.stepsPerMeasure,(long)(axis.stepsPerMeasure*1000)%1000,axis.microsteps,axis.IRUN,axis.reverse,axis.min,axis.max);
                    boolReply=false;
                  } else commandError=CE_0;
                } else commandError=CE_0;
              } else commandError=CE_0;
            } else commandError=CE_CMD_UNKNOWN;
          } else
          if (parameter[0] == 'U') { // Un: Get stepper driver statUs
            switch (parameter[1]) {
              case '1':
#if (AXIS1_DRIVER_STATUS == TMC_SPI)
                tmcAxis1.refresh_DRVSTATUS();
                strcat(reply,tmcAxis1.get_DRVSTATUS_STST() ? "ST," : ",");                                   // Standstill
                strcat(reply,tmcAxis1.get_DRVSTATUS_OLa() ? "OA," : ",");                                    // Open Load A
                strcat(reply,tmcAxis1.get_DRVSTATUS_OLb() ? "OB," : ",");                                    // Open Load B
                strcat(reply,tmcAxis1.get_DRVSTATUS_S2Ga() ? "GA," : ",");                                   // Short to Ground A
                strcat(reply,tmcAxis1.get_DRVSTATUS_S2Gb() ? "GB," : ",");                                   // Short to Ground B
                strcat(reply,tmcAxis1.get_DRVSTATUS_OT() ? "OT," : ",");                                     // Overtemp Shutdown 150C
                strcat(reply,tmcAxis1.get_DRVSTATUS_OTPW() ? "PW" : "");                                     // Overtemp Pre-warning 120C
                boolReply=false;
#else
                commandError=CE_0;
#endif
              break;
              case '2':
#if (AXIS2_DRIVER_STATUS == TMC_SPI)
                tmcAxis2.refresh_DRVSTATUS();
                strcat(reply,tmcAxis2.get_DRVSTATUS_STST() ? "ST," : ",");                                   // Standstill
                strcat(reply,tmcAxis2.get_DRVSTATUS_OLa() ? "OA," : ",");                                    // Open Load A
                strcat(reply,tmcAxis2.get_DRVSTATUS_OLb() ? "OB," : ",");                                    // Open Load B
                strcat(reply,tmcAxis2.get_DRVSTATUS_S2Ga() ? "GA," : ",");                                   // Short to Ground A
                strcat(reply,tmcAxis2.get_DRVSTATUS_S2Gb() ? "GB," : ",");                                   // Short to Ground B
                strcat(reply,tmcAxis2.get_DRVSTATUS_OT() ? "OT," : ",");                                     // Overtemp Shutdown 150C
                strcat(reply,tmcAxis2.get_DRVSTATUS_OTPW() ? "PW" : "");                                     // Overtemp Pre-warning 120C
                boolReply=false;
#else
                commandError=CE_0;
#endif
              break;
              default: commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == 'E') { // En: Get settings
            switch (parameter[1]) {
              case '1': dtostrf((double)maxRateBaseActual,3,3,reply); boolReply=false; break;
              case '2': dtostrf(SLEW_ACCELERATION_DIST,2,1,reply); boolReply=false; break;
              case '3': sprintf(reply,"%ld",(long)round(TRACK_BACKLASH_RATE)); boolReply=false; break;
              case '4': sprintf(reply,"%ld",(long)round(axis1Settings.stepsPerMeasure)); boolReply=false; break;
              case '5': sprintf(reply,"%ld",(long)round(axis2Settings.stepsPerMeasure)); boolReply=false; break;
              case '6': dtostrf(stepsPerSecondAxis1,3,6,reply); boolReply=false; break;
              case '7': sprintf(reply,"%ld",nv.readLong(EE_stepsPerWormRotAxis1)); boolReply=false; break;
              case '8': sprintf(reply,"%ld",(long)round(pecBufferSize)); boolReply=false; break;
#if MOUNT_TYPE == GEM
              case '9': sprintf(reply,"%ld",(long)round(degreesPastMeridianE*4.0)); boolReply=false; break;    // minutes past meridianE
              case 'A': sprintf(reply,"%ld",(long)round(degreesPastMeridianW*4.0)); boolReply=false; break;    // minutes past meridianW
#endif
              case 'e': sprintf(reply,"%ld",(long)round(axis1Settings.min)); boolReply=false; break;           // RA east or -Az limit, in degrees
              case 'w': sprintf(reply,"%ld",(long)round(axis1Settings.max)); boolReply=false; break;           // RA west or +Az limit, in degrees
              case 'B': sprintf(reply,"%ld",(long)round(axis1Settings.max/15.0)); boolReply=false; break;      // RA west or +Az limit, in hours
              case 'C': sprintf(reply,"%ld",(long)round(axis2Settings.min)); boolReply=false; break;
              case 'D': sprintf(reply,"%ld",(long)round(axis2Settings.max)); boolReply=false; break;
              case 'E':
                // coordinate mode for getting and setting RA/Dec
                // 0 = OBSERVED_PLACE
                // 1 = TOPOCENTRIC (does refraction)
                // 2 = ASTROMETRIC_J2000
                reply[0]='0'+(TELESCOPE_COORDINATES-1);
                boolReply=false;
                supress_frame=true;
              break;
              case 'F':
#if AXIS2_TANGENT_ARM == ON
                reply[0]='1';
#else
                reply[0]='0';
#endif
                boolReply=false;
                supress_frame=true;
              break;
              default: commandError=CE_CMD_UNKNOWN;
            }
          } else
          if (parameter[0] == 'F') { // Fn: Debug
            long temp;
            switch (parameter[1]) {
              case '0': cli(); temp=(long)(posAxis1-((long)targetAxis1.part.m)); sei(); sprintf(reply,"%ld",temp); boolReply=false; break; // Debug0, true vs. target RA position
              case '1': cli(); temp=(long)(posAxis2-((long)targetAxis2.part.m)); sei(); sprintf(reply,"%ld",temp); boolReply=false; break; // Debug1, true vs. target Dec position
              case '2': cli(); temp=(long)trackingState; sei(); sprintf(reply,"%ld",temp); boolReply=false; break;                         // Debug2, trackingState
              case '3': dtostrf(getFrequencyHzAxis1(),3,6,reply); boolReply=false; break;                                                  // Axis1 final tracking rate Hz
              case '4': dtostrf(getFrequencyHzAxis2(),3,6,reply); boolReply=false; break;                                                  // Axis2 final tracking rate Hz
              case '6': cli(); temp=(long)(targetAxis1.part.m); sei(); sprintf(reply,"%ld",temp); boolReply=false; break;                  // Debug6, HA target position
              case '7': cli(); temp=(long)(targetAxis2.part.m); sei(); sprintf(reply,"%ld",temp); boolReply=false; break;                  // Debug7, Dec target position
              case '8': cli(); temp=(long)(posAxis1); sei(); sprintf(reply,"%ld",temp); boolReply=false; break;                            // Debug8, HA motor position
              case '9': cli(); temp=(long)(posAxis2); sei(); sprintf(reply,"%ld",temp); boolReply=false; break;                            // Debug9, Dec motor position
              case 'A': sprintf(reply,"%ld%%",((long)worst_loop_time*100L)/9970L); worst_loop_time=0; boolReply=false; break;              // DebugA, Workload
              case 'B': cli(); temp=(long)(trackingTimerRateAxis1*1000.0); sei(); sprintf(reply,"%ld",temp); boolReply=false; break;       // DebugB, trackingTimerRateAxis1
              case 'C': sprintf(reply,"%ldus",average_loop_time); boolReply=false; break;                                                  // DebugC, Workload average
              case 'E': double ra, de; cli(); getEqu(&ra,&de,false); sei(); sprintf(reply,"%f,%f",ra,de); boolReply=false; break;          // DebugE, equatorial coordinates degrees (no division by 15)
#if DEBUG != OFF // DebugF, EEPROM dump to DebugSer
              case 'F':
                for (int x=0; x <= E2END+16; x++) {
                  if (x < 8 || x > E2END+8) {
                    if (x%8 == 0) DF("-----------");
                    DF("--");
                    if (x%8 == 7) DL();
                  } else {
                    if (x%8 == 0) { DF(":SXFF,"); char s[8]; sprintf(s,"%04d=",x/8); D(s); }
                    int v=nv.read(x-8);
                    if (v < 16) D(0); DebugSer.print(v,HEX);
                    if (x%8 == 7) DL("#");
                  }
                }
              break;
#endif
              case 'G': dtostrf(getIndexAxis2(),3,6,reply); boolReply=false; break;                                                        // DebugG, return index position
              default:  commandError=CE_CMD_UNKNOWN;
            }
          } else
#ifdef FEATURES_PRESENT
          if (parameter[0] == 'X') { // Xn: get auXiliary feature
            featuresGetCommand(parameter,reply,boolReply);
          } else
          if (parameter[0] == 'Y') { // Yn: get auXiliary feature temperature
            featuresGetInfoCommand(parameter,reply,boolReply);
          } else
#else
          if (parameter[0] == 'X' || parameter[0] == 'Y') commandError=CE_0; else // silent errors for feature detection
#endif
            commandError=CE_CMD_UNKNOWN;
        } else commandError=CE_CMD_UNKNOWN;
      } else
// :GZ#       Get telescope azimuth
//            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
      if (command[1] == 'Z' && parameter[0] == 0)  { getHor(&f,&f1); f1=degRange(f1); doubleToDms(reply,&f1,true,false,precision); boolReply=false; } else commandError=CE_CMD_UNKNOWN;
      } else

//  h - Home Position Commands
      if (command[0] == 'h') {
// :hF#       Reset telescope at the home position.  This position is required for a cold Start.
//            Point to the celestial pole.  GEM w/counterweights pointing downwards (CWD position).  Equatorial fork mounts at HA = 0.
//            Returns: Nothing
      if (command[1] == 'F' && parameter[0] == 0)  {
        focuserRotatorSave();
        commandError=setHome(); boolReply=false;
        if (commandError == CE_MOUNT_IN_MOTION) stopSlewingAndTracking(SS_ALL_FAST);
      } else 
// :hC#       Moves telescope to the home position
//            Returns: Nothing
      if (command[1] == 'C' && parameter[0] == 0)  {
        focuserRotatorSave();
        commandError=goHome(true); boolReply=false;
        if (commandError == CE_MOUNT_IN_MOTION) stopSlewingAndTracking(SS_ALL_FAST);
      } else 
// :hP#       Goto the Park Position
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'P' && parameter[0] == 0)  {
        focuserRotatorSave();
        commandError=park();
        } else 
// :hQ#       Set the park position
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'Q' && parameter[0] == 0)  commandError=setPark(); else 
// :hR#       Restore parked telescope to operation
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'R' && parameter[0] == 0)  commandError=unPark(true); else

        commandError=CE_CMD_UNKNOWN;
      } else

//   L - Object Library Commands
      if (command[0] == 'L') {

// :LB#       Find previous object and set it as the current target object
//            Returns: Nothing
      if (command[1] == 'B' && parameter[0] == 0) { 
          Lib.prevRec(); boolReply=false;
      } else 

// :LC[n]#    Set current target object to catalog object number
//            Returns: Nothing
      if (command[1] == 'C') {
        if (atoi2(parameter,&i)) {
          if (i >= 0 && i <= 32767) {
            Lib.gotoRec(i); boolReply=false;
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else 

// :LI#       Get Object Information
//            Returns: s# (string containing the current target object’s name and object type)
      if (command[1] == 'I' && parameter[0] == 0) {
        Lib.readVars(reply,&i,&origTargetRA,&origTargetDec);

        char const * objType=objectStr[i];
        strcat(reply,",");
        strcat(reply,objType);
        boolReply=false;
      } else 

// :LIG#      Get catalog object information and goto
//            Returns: Nothing
      if (command[1] == 'I' && parameter[0] == 'G' && parameter[1] == 0) {
        Lib.readVars(reply,&i,&origTargetRA,&origTargetDec);

        newTargetRA=origTargetRA; newTargetDec=origTargetDec;
#if TELESCOPE_COORDINATES == TOPOCENTRIC
        topocentricToObservedPlace(&newTargetRA,&newTargetDec);
#endif

        commandError=goToEqu(newTargetRA,newTargetDec);
        boolReply=false;
      } else 

// :LR#       Get catalog object information including RA and Dec, with advance to next record
//            Returns: s# (string containing the current target object’s name, type, RA, and Dec)
      if (command[1] == 'R' && parameter[0] == 0) {
        Lib.readVars(reply,&i,&origTargetRA,&origTargetDec);

        char const * objType=objectStr[i];
        char ws[20];

        strcat(reply,",");
        strcat(reply,objType);
        if (strcmp(reply,",UNK") != 0) {
          f=origTargetRA; f/=15.0; doubleToHms(ws,&f,precision); strcat(reply,","); strcat(reply,ws);
          doubleToDms(ws,&origTargetDec,false,true,precision); strcat(reply,","); strcat(reply,ws);
        }
        
        Lib.nextRec();

        boolReply=false;
          
      } else 

// :LW[s]#    Write catalog object information including current target RA,Dec to next available empty record
//            If at the end of the object list (:LI# command returns an empty string "#") a new item is automatically added
//            [s] is a string of up to eleven chars followed by a comma and a type designation for ex. ":LWM31 AND,GAL#"
//            Returns: 0 on failure (memory full, for example)
//                     1 on success
      if (command[1] == 'W') {
        
        char name[12];
        char objType[4];

        // extract object name
        int l=0;
        do {
          name[l]=0;
          if (parameter[l] == ',') break;
          name[l]=parameter[l]; name[l+1]=0;
          l++;
        } while (l<12);

        // extract object type
        i=0;
        if (parameter[l] == ',') {
          l++; int m=0;
          do {
            objType[m+1]=0;
            objType[m]=parameter[l];
            l++; m++;
          } while (parameter[l] != 0);
          // encode
          //   0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15    
          // "UNK",  "OC",  "GC",  "PN",  "DN",  "SG",  "EG",  "IG", "KNT", "SNR", "GAL",  "CN", "STR", "PLA", "CMT", "AST"
          for (l=0; l <= 15; l++) { if (strcmp(objType,objectStr[l]) == 0) i=l; }
        }
        
        if (Lib.firstFreeRec()) Lib.writeVars(name,i,origTargetRA,origTargetDec); else commandError=CE_LIBRARY_FULL;
      } else 

// :LN#       Find next deep sky target object subject to the current constraints.
//            Returns: Nothing
      if (command[1] == 'N' && parameter[0] == 0) { 
          Lib.nextRec();
          boolReply=false;
      } else 

// :L$#       Move to catalog name record
//            Returns: 1
      if (command[1] == '$' && parameter[0] == 0) { 
          Lib.nameRec();
          boolReply=true;
      } else 

// :LD#       Clear current record
//            Returns: Nothing
      if (command[1] == 'D' && parameter[0] == 0) { 
          Lib.clearCurrentRec();
          boolReply=false;
      } else 

// :LL#       Clear current catalog
//            Returns: Nothing
      if (command[1] == 'L' && parameter[0] == 0) { 
          Lib.clearLib();
          boolReply=false;
      } else 

// :L!#       Clear library (all catalogs)
//            Returns: Nothing
      if (command[1] == '!' && parameter[0] == 0) { 
          Lib.clearAll();
          boolReply=false;
      } else 

// :L?#       Get library free records (all catalogs)
//            Returns: n#
      if (command[1] == '?' && parameter[0] == 0) { 
          sprintf(reply,"%ld",Lib.recFreeAll());
          boolReply=false;
      } else 

// :Lo[n]#    Select Library catalog by catalog number n
//            Catalog number ranges from 0..14, catalogs 0..6 are user defined, the remainder are reserved
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'o') {
        if (atoi2(parameter,&i)) {
          if (i >= 0 && i <= 14) {
            Lib.setCatalog(i);
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else commandError=CE_CMD_UNKNOWN;
        
      } else

// M - Telescope Movement Commands
      if (command[0] == 'M') {
// :MA#       Goto the target Alt and Az
//            Returns: 0..9, see :MS#
      if (command[1] == 'A' && parameter[0] == 0) {
        CommandErrors e=goToHor(&newTargetAlt, &newTargetAzm);
        if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) reply[0]=(char)(e-CE_GOTO_ERR_BELOW_HORIZON)+'1';
        if (e == CE_NONE) reply[0]='0';
        reply[1]=0;
        boolReply=false; 
        supress_frame=true;
        commandError=e;
      } else
// :Mgd[n]#   Pulse guide command where n is the guide time in milliseconds
//            Returns: Nothing
// :MGd[n]#   Pulse guide command where n is the guide time in milliseconds
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'g' || command[1] == 'G') {
        if (atoi2((char *)&parameter[1],&i)) {
          if (i >= 0 && i <= 16399) {
            if ((parameter[0] == 'e' || parameter[0] == 'w') && guideDirAxis1 == 0) {
#if SEPARATE_PULSE_GUIDE_RATE == ON
              commandError=startGuideAxis1(parameter[0],currentPulseGuideRate,i,true);
#else
              commandError=startGuideAxis1(parameter[0],currentGuideRate,i,true);
#endif
              if (command[1] == 'g') boolReply=false;
            } else
            if ((parameter[0] == 'n' || parameter[0] == 's') && guideDirAxis2 == 0) { 
#if SEPARATE_PULSE_GUIDE_RATE == ON
              commandError=startGuideAxis2(parameter[0],currentPulseGuideRate,i,true);
#else
              commandError=startGuideAxis2(parameter[0],currentGuideRate,i,true);
#endif
              if (command[1] == 'g') boolReply=false;
            } else commandError=CE_CMD_UNKNOWN;
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else
// :Me# :Mw#  Move Telescope East or West at current guide rate
//            Returns: Nothing
      if ((command[1] == 'e' || command[1] == 'w') && parameter[0] == 0) {
        commandError=startGuideAxis1(command[1],currentGuideRate,GUIDE_TIME_LIMIT*1000,false);
        boolReply=false;
      } else
// :Mn# :Ms#  Move Telescope North or South at current guide rate
//            Returns: Nothing
      if ((command[1] == 'n' || command[1] == 's') && parameter[0] == 0) {
        commandError=startGuideAxis2(command[1],currentGuideRate,GUIDE_TIME_LIMIT*1000,false);
        boolReply=false;
      } else
// :Mp#  Move Telescope for sPiral search at current guide rate
//            Returns: Nothing
      if ((command[1] == 'p') && parameter[0] == 0) {
        commandError=startGuideSpiral(currentGuideRate,GUIDE_SPIRAL_TIME_LIMIT*1000);
        boolReply=false;
      } else

// :MP#       Goto the Current Position for Polar Align
//            Returns: 0..9, see :MS#
      if (command[1] == 'P' && parameter[0] == 0)  {
        double r,d;
        getEqu(&r,&d,false);
        CommandErrors e=validateGoToEqu(r,d);
        if (e == CE_NONE) {
          Align.altCor=0.0;
          Align.azmCor=0.0;
          e=goToEqu(r,d);
        }
        if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) reply[0]=(char)(e-CE_GOTO_ERR_BELOW_HORIZON)+'1';
        if (e == CE_NONE) reply[0]='0';
        reply[1]=0;
        boolReply=false;
        supress_frame=true;
        commandError=e;
      } else

// :MS#       Goto the Target Object
//            Returns:
//              0=Goto is possible
//              1=below the horizon limit
//              2=above overhead limit
//              3=controller in standby
//              4=mount is parked
//              5=Goto in progress
//              6=outside limits (AXIS2_LIMIT_MAX, AXIS2_LIMIT_MIN, AXIS1_LIMIT_MIN/MAX, MERIDIAN_E/W)
//              7=hardware fault
//              8=already in motion
//              9=unspecified error
      if (command[1] == 'S' && parameter[0] == 0)  {
        newTargetRA=origTargetRA; newTargetDec=origTargetDec;
#if TELESCOPE_COORDINATES == TOPOCENTRIC
        topocentricToObservedPlace(&newTargetRA,&newTargetDec);
#endif
        CommandErrors e=goToEqu(newTargetRA,newTargetDec);
        if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) reply[0]=(char)(e-CE_GOTO_ERR_BELOW_HORIZON)+'1';
        if (e == CE_NONE) reply[0]='0';
        reply[1]=0;
        boolReply=false;
        supress_frame=true;
        commandError=e;
      } else

//  :MN#   Goto current RA/Dec but East of the Pier (within meridian limit overlap for GEM mounts)
//         Returns: 0..9, see :MS#
      if (command[1] == 'N' && parameter[0] == 0)  {
        CommandErrors e=goToHere(true);
        if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) reply[0]=(char)(e-CE_GOTO_ERR_BELOW_HORIZON)+'1';
        if (e == CE_NONE) reply[0]='0';
        reply[1]=0;
        boolReply=false;
        supress_frame=true; 
      } else commandError=CE_CMD_UNKNOWN;
      
      } else
// $Q - PEC Control
// :$QZ+      Enable RA PEC compensation 
//            Returns: nothing
// :$QZ-      Disable RA PEC Compensation
//            Returns: nothing
// :$QZZ      Clear the PEC data buffer
//            Return: Nothing
// :$QZ/      Ready Record PEC
//            Returns: nothing
// :$QZ!      Write PEC data to EEPROM
//            Returns: nothing
// :$QZ?      Get PEC status
//            Returns: s#
      if (command[0] == '$' && command[1] == 'Q') {
        if (parameter[0] == 'Z' && parameter[2] == 0) {
          boolReply=false;
#if AXIS1_PEC == ON
          if (parameter[1] == '+') { if (pecRecorded) pecStatus=ReadyPlayPEC; nv.update(EE_pecStatus,pecStatus); } else
          if (parameter[1] == '-') { pecStatus=IgnorePEC; nv.update(EE_pecStatus,pecStatus); } else
          if (parameter[1] == '/' && trackingState == TrackingSidereal) { pecStatus=ReadyRecordPEC; nv.update(EE_pecStatus,IgnorePEC); } else
          if (parameter[1] == 'Z') { 
            for (i=0; i<pecBufferSize; i++) pecBuffer[i]=128;
            pecFirstRecord = true;
            pecStatus      = IgnorePEC;
            pecRecorded    = false;
            nv.update(EE_pecStatus,pecStatus);
            nv.update(EE_pecRecorded,pecRecorded);
          } else
          if (parameter[1] == '!') {
            pecRecorded=true;
            nv.update(EE_pecRecorded,pecRecorded);
            nv.writeLong(EE_wormSensePos,wormSensePos);
            // trigger recording of PEC buffer
            pecAutoRecord=pecBufferSize;
          } else
#endif
          // Status is one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording.  Or an optional (.) to indicate an index detect.
          if (parameter[1] == '?') {
            const char *pecStatusCh = PECStatusString;
            reply[0]=pecStatusCh[pecStatus];
            reply[1]=0; reply[2]=0;
            if (wormSensedAgain) { reply[1]='.'; wormSensedAgain=false; }
          } else {
            boolReply=true;
            commandError=CE_CMD_UNKNOWN;
          }
        } else commandError=CE_CMD_UNKNOWN;
      } else

// Q - Movement Commands
// :Q#        Halt all slews, stops goto
//            Returns: Nothing
      if (command[0] == 'Q') {
        if (command[1] == 0) {
          stopSlewingAndTracking(SS_ALL_FAST);
          boolReply=false; 
        } else
// :Qe# Qw#   Halt east/westward Slews
//            Returns: Nothing
        if ((command[1] == 'e' || command[1] == 'w') && parameter[0] == 0) {
          stopGuideAxis1();
          boolReply=false;
        } else
// :Qn# Qs#   Halt north/southward Slews
//            Returns: Nothing
        if ((command[1] == 'n' || command[1] == 's') && parameter[0] == 0) {
          stopGuideAxis2();
          boolReply=false;
        } else commandError=CE_CMD_UNKNOWN;
      } else

// R - Slew Rate Commands
      if (command[0] == 'R') {

// :RA[n.n]#  Set Axis1 Guide rate to n.n degrees per sidereal second
//            Returns: Nothing
      if (command[1] == 'A') {
        f=strtod(parameter,&conv_end);
        double maxStepsPerSecond=1000000.0/(maxRate/16.0);
        if (&parameter[0] != conv_end) {
          if (f < 0.001/60.0/60.0) f=0.001/60.0/60.0;
          if (f > maxStepsPerSecond/axis1Settings.stepsPerMeasure) f=maxStepsPerSecond/axis1Settings.stepsPerMeasure;
          customGuideRateAxis1(f*240.0,GUIDE_TIME_LIMIT*1000);
        }
        boolReply=false; 
      } else
// :RE[n.n]#  Set Axis2 Guide rate to n.n degrees per sidereal second
//            Returns: Nothing
      if (command[1] == 'E') {
        f=strtod(parameter,&conv_end);
        double maxStepsPerSecond=1000000.0/(maxRate/16.0);
        if (&parameter[0] != conv_end) {
          if (f < 0.001/60.0/60.0) f=0.001/60.0/60.0;
          if (f > maxStepsPerSecond/axis2Settings.stepsPerMeasure) f=maxStepsPerSecond/axis2Settings.stepsPerMeasure;
          customGuideRateAxis2(f*240.0,GUIDE_TIME_LIMIT*1000);
        }
        boolReply=false; 
      } else

// :RG#       Set slew rate to Guiding Rate   1X
// :RC#       Set slew rate to Centering rate 8X
// :RM#       Set slew rate to Find Rate     20X
// :RF#       Set slew rate to Fast Rate     48X
// :RS#       Set slew rate to Half Max (VF)  ?X (1/2 of maxRate)
// :Rn#       Set slew rate to n, where n = 0..9
//            Returns: Nothing
      if ((command[1] == 'G' || command[1] == 'C' || command[1] == 'M' || command[1] == 'S' || (command[1] >= '0' && command[1] <= '9')) && parameter[0] == 0) {
        if (command[1] == 'G') i=2; else // 1x
        if (command[1] == 'C') i=5; else // 8x
        if (command[1] == 'M') i=6; else // 20x
        if (command[1] == 'F') i=7; else // 48x
        if (command[1] == 'S') i=8; else i=command[1]-'0'; // typically 240x to 480x can be as low as 60x
        setGuideRate(i);
        boolReply=false; 
      } else commandError=CE_CMD_UNKNOWN;
     } else

#if ROTATOR == ON
// r - Rotator/De-rotator Commands
      if (command[0] == 'r') {
#if MOUNT_TYPE == ALTAZM
// :r+#       Enable derotator
//            Returns: Nothing
      if (command[1] == '+' && parameter[0] == 0) { rot.enableDR(true); boolReply=false; } else
// :r-#       Disable derotator
//            Returns: Nothing
      if (command[1] == '-' && parameter[0] == 0) { rot.enableDR(false); boolReply=false; } else
// :rP#       Move rotator to the parallactic angle
//            Returns: Nothing
      if (command[1] == 'P' && parameter[0] == 0) { double h,d; getApproxEqu(&h,&d,true); rot.setPA(h,d); boolReply=false; } else
// :rR#       Reverse derotator direction
//            Returns: Nothing
      if (command[1] == 'R' && parameter[0] == 0) { rot.reverseDR(); boolReply=false; } else
#endif
// :rT#       Get status
//            Returns: M# (for moving) or S# (for stopped)
        if (command[1] == 'T') { if (rot.moving()) strcpy(reply,"M"); else strcpy(reply,"S"); boolReply=false; } else
// :rI#       Get mIn position (in degrees)
//            Returns: n#
        if (command[1] == 'I') { sprintf(reply,"%ld",(long)round(rot.getMin())); boolReply=false; } else
// :rM#       Get Max position (in degrees)
//            Returns: n#
        if (command[1] == 'M') { sprintf(reply,"%ld",(long)round(rot.getMax())); boolReply=false; } else
// :rD#       Get rotator degrees per step
//            Returns: n.n#
        if (command[1] == 'D') { dtostrf(1.0/rot.getStepsPerDegree(),7,5,reply); boolReply=false; } else
// :rb#       Get rotator backlash amount in steps
//            Return: n#
        if (command[1] == 'b' && parameter[0] == 0) { sprintf(reply,"%ld",(long)rot.getBacklash()); boolReply=false; } else
// :rb[n]#
//            Set rotator backlash amount in steps
//            Returns: 0 on failure
//                     1 on success
      if (command[1] == 'b') { if (atoi2((char*)&parameter[0],&i)) { if (!rot.setBacklash(i)) commandError=CE_PARAM_RANGE; } else commandError=CE_PARAM_FORM; } else
// :rF#       Reset rotator at the home position
//            Returns: Nothing
      if (command[1] == 'F' && parameter[0] == 0) { rot.reset(); boolReply=false; } else
// :rC#       Moves rotator to the home position
//            Returns: Nothing
      if (command[1] == 'C' && parameter[0] == 0) { rot.home(); boolReply=false; } else
// :rG#       Get rotator current position in degrees
//            Returns: sDDD*MM#
      if (command[1] == 'G' && parameter[0] == 0) { f1=rot.getPosition(); doubleToDms(reply,&f1,true,true,PM_LOW); boolReply=false; } else
// :rc#       Set continuous move mode (for next move command)
//            Returns: Nothing
      if (command[1] == 'c' && parameter[0] == 0) { rot.moveContinuous(true); boolReply=false; } else
// :r>#       Move clockwise as set by :rn# command, default = 1 deg (or 0.1 deg/s in continuous mode)
//            Returns: Nothing
      if (command[1] == '>' && parameter[0] == 0) { rot.startMoveCW(); boolReply=false; } else
// :r<#       Move counter clockwise as set by :rn# command
//            Returns: Nothing
      if (command[1] == '<' && parameter[0] == 0) { rot.startMoveCCW(); boolReply=false; } else
// :rQ#       Stops movement (except derotator)
//            Returns: Nothing
      if (command[1] == 'Q' && parameter[0] == 0) { rot.stopMove(); rot.moveContinuous(false); boolReply=false; } else
// :r[n]#     Move increment where n = 1 for 1 degrees, 2 for 2 degrees, 3 for 5 degrees, 4 for 10 degrees
//            Move rate where n = 1 for .01 deg/s, 2 for 0.1 deg/s, 3 for 1.0 deg/s, 4 for 5.0 deg/s
//            Returns: Nothing
      if ((command[1] >= '1' && command[1] <= '4') && parameter[0] == 0) {
        i=command[1]-'1';
        int t[]={1,2,5,10}; double t1[]={0.01,0.1,1.0,5.0};
        rot.setIncrement(t[i]); rot.setMoveRate(t1[i]); boolReply=false;
      } else
// :rS[sDDD*MM'SS]#
//            Set position in degrees
//            Returns: 0 on failure
//                     1 on success
      if (command[1] == 'S') {
        if (parameter[0] == '-') f=-1.0; else f=1.0;
        if (parameter[0] == '+' || parameter[0] == '-') i1=1; else i1=0;
        if (dmsToDouble(&f1,(char *)&parameter[i1],false)) { if (!rot.setTarget(f*f1)) commandError=CE_SLEW_ERR_IN_STANDBY; } else commandError=CE_PARAM_FORM;
      } else commandError=CE_CMD_UNKNOWN;
     } else
#endif

// S - Telescope Set Commands
      if (command[0] == 'S') {
// :Sa[sDD*MM]#
//            Set target object Altitude to sDD*MM# or sDD*MM:SS# assumes high precision but falls back to low precision
//            Returns:
//            0 if Object is within slew range, 1 otherwise
      if (command[1] == 'a')  {
         if (!dmsToDouble(&newTargetAlt,parameter,true,PM_HIGH))
           if (!dmsToDouble(&newTargetAlt,parameter,true,PM_LOW)) commandError=CE_PARAM_FORM;
      } else
// :SB[n]#    Set Baud Rate where n is an ASCII digit (1..9) with the following interpertation
//            0=115.2K, 1=56.7K, 2=38.4K, 3=28.8K, 4=19.2K, 5=14.4K, 6=9600, 7=4800, 8=2400, 9=1200
//            Returns: 1 (at the current baud rate and then changes to the new rate for further communication)
      if (command[1] == 'B') {
        i=(int)(parameter[0]-'0');
        if (i >= 0 && i < 10) {
          if (process_command == COMMAND_SERIAL_A) {
            SerialA.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialA.transmit()); 
            #endif
            delay(50); SerialA.begin(baudRate[i]);
            boolReply=false; 
#ifdef HAL_SERIAL_B_ENABLED
          } else
          if (process_command == COMMAND_SERIAL_B) {
            SerialB.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialB.transmit()); 
            #endif
            delay(50);
  #ifdef SERIAL_B_RX
            SerialB.begin(baudRate[i], SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
  #else
            SerialB.begin(baudRate[i]);
  #endif
            boolReply=false;
#endif
#if defined(HAL_SERIAL_C_ENABLED) && !defined(HAL_SERIAL_C_BLUETOOTH)
          } else
          if (process_command == COMMAND_SERIAL_C) {
            SerialC.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialC.transmit()); 
            #endif
            delay(50); SerialC.begin(baudRate[i]);
            boolReply=false; 
#endif
#if defined(HAL_SERIAL_D_ENABLED)
          } else
          if (process_command == COMMAND_SERIAL_D) {
            SerialD.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialD.transmit()); 
            #endif
            delay(50); SerialD.begin(baudRate[i]); 
            boolReply=false;
#endif
          } else commandError=CE_CMD_UNKNOWN;
        } else commandError=CE_PARAM_RANGE;
      } else
// :SC[MM/DD/YY]#
//            Change Date to MM/DD/YY
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'C')  {
        if (dateToDouble(&JD,parameter)) {
          nv.writeFloat(EE_JD,JD);
          updateLST(jd2last(JD,UT1,true));
          dateWasSet=true;
          if (generalError == ERR_SITE_INIT && dateWasSet && timeWasSet) generalError=ERR_NONE;
        } else commandError=CE_PARAM_FORM; } else 
//  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
//            Set target object declination
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'd')  {
        if (!dmsToDouble(&origTargetDec,parameter,true)) commandError=CE_PARAM_FORM;
      } else
//  :Sg[(s)DDD*MM]# or :Sg[(s)DDD*MM:SS]# or :Sg[(s)DDD*MM:SS.SSS]#
//            Set current site longitude, east longitudes can be negative or > 180 degrees
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'g')  {
        if (parameter[0] == '-' || parameter[0] == '+') i1=1; else i1=0;
        if (dmsToDouble(&longitude,(char *)&parameter[i1],false)) {
          if (parameter[0] == '-') longitude=-longitude;
          if (longitude >= -180.0 && longitude <= 360.0) {
            if (longitude >= 180.0) longitude-=360.0;
            nv.writeFloat(EE_sites+currentSite*25+4,longitude);
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
        updateLST(jd2last(JD,UT1,false));
        } else
//  :SG[sHH]# or :SG[sHH:MM]# (where MM is 30 or 45)
//            Set the number of hours added to local time to yield UTC
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'G')  { 
        if (strlen(parameter) < 7) {
          double f=0.0;
          char *temp=strchr(parameter,':');
          if (temp) {
            if (temp[0] == ':' && temp[1] == '4' && temp[2] == '5') { temp[0]=0; f=0.75; } else
            if (temp[0] == ':' && temp[1] == '3' && temp[2] == '0') { temp[0]=0; f=0.5; } else 
            if (temp[0] == ':' && temp[1] == '0' && temp[2] == '0') { temp[0]=0; f=0.0; } else { i=-999; } // force error if not :00 or :30 or :45
          }

          if (atoi2(parameter,&i)) {
            if (i >= -24 && i <= 24) {
              if (i<0) timeZone=i-f; else timeZone=i+f;
              b=encodeTimeZone(timeZone)+128;
              nv.update(EE_sites+currentSite*25+8,b);
              updateLST(jd2last(JD,UT1,true));
            } else commandError=CE_PARAM_RANGE;
          } else commandError=CE_PARAM_FORM;
        } else commandError=CE_PARAM_FORM; 
      } else
//  :Sh[sDD]#
//            Set the lowest elevation to which the telescope will goTo
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'h')  {
        if (atoi2(parameter,&i)) {
          if (i >= -30 && i <= 30) {
            minAlt=i; nv.update(EE_minAlt,minAlt+128);
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else
//  :SL[HH:MM:SS]# or :SL[HH:MM:SS.SSS]#
//            Set the local Time
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'L')  {  
        if (hmsToDouble(&LMT,parameter,PM_HIGH) || hmsToDouble(&LMT,parameter,PM_HIGHEST)) {
#ifndef ESP32
          nv.writeFloat(EE_LMT,LMT);
#endif
          UT1=LMT+timeZone;
          updateLST(jd2last(JD,UT1,true));
          timeWasSet=true;
          if (generalError == ERR_SITE_INIT && dateWasSet && timeWasSet) generalError=ERR_NONE;
        } else commandError=CE_PARAM_FORM;
      } else 
//  :SM[s]# or :SN[s]# or :SO[s]# or :SP[s]#
//            Set site name, string may be up to 15 characters
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P') {
        i=command[1]-'M';
        if (strlen(parameter) > 15) commandError=CE_PARAM_RANGE; else nv.writeString(EE_sites+i*25+9,parameter);
      } else
//  :So[DD]#
//            Set the overhead elevation limit in degrees relative to the horizon
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'o')  {
        if (atoi2(parameter,&i)) {
          if (i >= 60 && i <= 90) {
            maxAlt=i;
#if MOUNT_TYPE == ALTAZM
            if (maxAlt > 87) maxAlt=87;
#endif
            nv.update(EE_maxAlt,maxAlt); 
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
      } else
//  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
//            Set target object RA
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'r')  {
        if (hmsToDouble(&origTargetRA,parameter)) origTargetRA*=15.0; else commandError=CE_PARAM_RANGE;
      } else 
//  :SS[HH:MM:SS]#
//            Sets the local (apparent) sideral time to HH:MM:SS
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'S')  { if (!hmsToDouble(&f,parameter,PM_HIGH)) commandError=CE_PARAM_FORM; else updateLST(f); } else 
//  :St[sDD*MM]# or :St[sDD*MM:SS]# or :St[sDD*MM:SS.SSS]#
//            Set current site latitude
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 't')  {
        if (dmsToDouble(&f,parameter,true)) setLatitude(f); else commandError=CE_PARAM_FORM;
      } else 
//  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is solar rate
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'T')  { 
        if (!isSlewing()) {
          f=strtod(parameter,&conv_end);
          if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
            if (fabs(f) < 0.1) {
              trackingState=TrackingNone;
            } else {
              if (trackingState == TrackingNone) { trackingState=TrackingSidereal; enableStepperDrivers(); }
              setTrackingRate((f/60.0)/1.00273790935);
            }
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_MOUNT_IN_MOTION;
      } else
// :SX[II,n]# Set OnStep value where II is the numeric index and n is the value to set (possibly floating point)
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'X')  {
        if (parameter[2] != ',') { parameter[0]=0; commandError=CE_PARAM_FORM; }                             // make sure command format is correct
        if (parameter[0] == '0') { // 0n: Align Model
          static int star;
          switch (parameter[1]) {
            case '0': Align.ax1Cor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                      // ax1Cor
            case '1': Align.ax2Cor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                      // ax2Cor 
            case '2': Align.altCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                      // altCor
            case '3': Align.azmCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                      // azmCor
            case '4': Align.doCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                       // doCor
            case '5': Align.pdCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                       // pdCor
#if MOUNT_TYPE == FORK
            case '6': Align.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                       // ffCor
            case '7': break;                                                                                 // dfCor
#else
            case '6': break;                                                                                 // ffCor
            case '7': Align.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                       // dfCor
#endif
            case '8': Align.tfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;                       // tfCor
            case '9': { i=strtol(&parameter[3],NULL,10); if (i == 1) { alignNumStars=star; alignThisStar=star+1; Align.model(star); } else star=0; } break;  // use 0 to start upload of stars for align, use 1 to trigger align
            case 'A': { if (!hmsToDouble(&Align.actual[star].ha,&parameter[3],PM_HIGH))       commandError=CE_PARAM_FORM; else Align.actual[star].ha =(Align.actual[star].ha*15.0)/Rad; } break; // Star  #n HA
            case 'B': { if (!dmsToDouble(&Align.actual[star].dec,&parameter[3],true,PM_HIGH)) commandError=CE_PARAM_FORM; else Align.actual[star].dec=Align.actual[star].dec/Rad;       } break; // Star  #n Dec
            case 'C': { if (!hmsToDouble(&Align.mount[star].ha,&parameter[3],PM_HIGH))        commandError=CE_PARAM_FORM; else Align.mount[star].ha  =(Align.mount[star].ha*15.0)/Rad;  } break; // Mount #n HA
            case 'D': { if (!dmsToDouble(&Align.mount[star].dec,&parameter[3],true,PM_HIGH))  commandError=CE_PARAM_FORM; else Align.mount[star].dec =Align.mount[star].dec/Rad;        } break; // Star  #n Dec
            case 'E': Align.actual[star].side=Align.mount[star].side=strtol(&parameter[3],NULL,10); star++; break; // Mount PierSide (and increment n)
            default:  commandError=CE_CMD_UNKNOWN;
          }
        } else
        if (parameter[0] == '4') { // 4n: Encoder
          switch (parameter[1]) {
            static double encoderAxis1=0;
            static double encoderAxis2=0;
            case '0': // set encoder Axis1 value
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && fabs(f) <= 360.0) encoderAxis1=f; else commandError=CE_PARAM_RANGE;
              break;
            case '1': // set encoder Axis2 value
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && fabs(f) <= 360.0) encoderAxis2=f; else commandError=CE_PARAM_RANGE;
              break;
            case '2': // sync encoder to last values
              if (parameter[3] == '1' && parameter[4] == 0) syncEnc(encoderAxis1,encoderAxis2);
              break;
            case '3': // re-enable setting OnStep to Encoders after a Sync 
              syncToEncodersOnly=false;
              break;
            default: commandError=CE_CMD_UNKNOWN;
          }
        } else
        if (parameter[0] == '9') { // 9n: Misc.
          switch (parameter[1]) {
            case '2': // set new slew rate (returns 1 success or 0 failure)
              if (!isSlewing()) {
                maxRate=strtod(&parameter[3],&conv_end)*16.0;
                if (maxRate < (long)(maxRateBaseActual*8.0)) maxRate=maxRateBaseActual*8.0;
                if (maxRate > (long)(maxRateBaseActual*32.0)) maxRate=maxRateBaseActual*32.0;
                if (maxRate < maxRateLowerLimit()) maxRate=maxRateLowerLimit();
                nv.writeLong(EE_maxRateL,maxRate);
                setAccelerationRates(maxRate);
              } else commandError=CE_MOUNT_IN_MOTION;
            break;
            case '3': // slew rate preset (returns nothing)
              boolReply=false;
              if (!isSlewing()) {
                switch (parameter[3]) {
                  case '5': maxRate=maxRateBaseActual*(16.0*2.0); break; // 50%
                  case '4': maxRate=maxRateBaseActual*(16.0*1.5); break; // 75%
                  case '3': maxRate=maxRateBaseActual*(16.0*1.0); break; // 100%
                  case '2': maxRate=maxRateBaseActual*(16.0/1.5); break; // 150%
                  case '1': maxRate=maxRateBaseActual*(16.0/2.0); break; // 200%
                  default:  maxRate=maxRateBaseActual*16.0;
                }
                if (maxRate < maxRateLowerLimit()) maxRate=maxRateLowerLimit();

                nv.writeLong(EE_maxRateL,maxRate);
                setAccelerationRates(maxRate);
              } else commandError=CE_MOUNT_IN_MOTION;
            break;
#if MOUNT_TYPE == GEM
            case '5': // autoMeridianFlip
              if (parameter[3] == '0' || parameter[3] == '1') { i=parameter[3]-'0'; autoMeridianFlip=i; nv.write(EE_autoMeridianFlip,autoMeridianFlip); } else commandError=CE_PARAM_RANGE;
            break;
#endif
            case '6': // preferred pier side 
              switch (parameter[3]) {
                case 'E': preferredPierSide=EAST; break;
                case 'W': preferredPierSide=WEST; break;
                case 'B': preferredPierSide=BEST; break;
                default:  commandError=CE_PARAM_RANGE;
              }
            break;
            case '7': // buzzer
              if (parameter[3] == '0' || parameter[3] == '1') { soundEnabled=parameter[3]-'0'; } else commandError=CE_PARAM_RANGE;
            break;
            case '8': // pause at home on meridian flip
              if (parameter[3] == '0' || parameter[3] == '1') { pauseHome=parameter[3]-'0'; nv.write(EE_pauseHome,pauseHome); } else commandError=CE_PARAM_RANGE;
            break;
            case '9': // continue if paused at home
              if (parameter[3] == '1') { if (waitingHome) waitingHomeContinue=true; } commandError=CE_PARAM_RANGE;
            break;
            case 'A': // temperature in deg. C
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && f >= -100.0 && f < 100.0) {
                ambient.setTemperature(f);
              } else commandError=CE_PARAM_RANGE;
            break;
            case 'B': // pressure in mb
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && f >= 500.0 && f < 1500.0) {
                ambient.setPressure(f);
              } else commandError=CE_PARAM_RANGE;
            break;
            case 'C': // relative humidity in % 
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && f >= 0.0 && f < 100.0) {
                ambient.setHumidity(f);
              } else commandError=CE_PARAM_RANGE;
            break;
            case 'D': // altitude 
              f=strtod(&parameter[3],&conv_end);
              if (&parameter[3] != conv_end && f >= -100.0 && f < 20000.0) {
                ambient.setAltitude(f);
              } else commandError=CE_PARAM_RANGE;
            break;
            default: commandError=CE_CMD_UNKNOWN;
          }
        } else
        if (parameter[0] == 'A') {
          uint8_t state=nv.read(EE_settingsRuntime);
          if (parameter[1] == 'C' && (parameter[3] == '0' || parameter[3] == '1') && parameter[4] == 0) {
            // :SXAC,n# to switch between compile and run-time settings
            if (parameter[3]-'0' == 0) state=1; else state=0; nv.write(EE_settingsRuntime,state);
          } else {
            int p=parameter[1]-'1';
            // check for all axes set to revert
            if (state&1) {
              // check for this axis set to revert
              if (!(state&(1<<(p+1)))) {
                if (p == 0 || p == 1 || (p == 2 && ROTATOR == ON) || (p == 3 && FOCUSER1 == ON) || (p == 4 && FOCUSER2 == ON)) {
                  // bit 0 = settings at compile (0) or run time (1), bits 1 to 5 = (1) to reset axis n on next boot
                  if (parameter[3] == 'R' && parameter[4] == 0) {
                    // :SXA1,R# to revert an axis to defaults
                     state|=1<<(p+1); nv.write(EE_settingsRuntime,state);
                  } else {
                    // :SXAn: Set axis settings :SXA1,....#
                    axisSettings axis;
                    bool error=false;
                    if (!decodeAxisSettings(&parameter[3],axis)) error=true;
                    if (!validateAxisSettings(p+1,MOUNT_TYPE==ALTAZM,axis)) error=true;
                    if (p == 0 && axis.microsteps < AXIS1_DRIVER_MICROSTEPS_GOTO) axis.microsteps=AXIS1_DRIVER_MICROSTEPS_GOTO;
                    if (p == 1 && axis.microsteps < AXIS2_DRIVER_MICROSTEPS_GOTO) axis.microsteps=AXIS2_DRIVER_MICROSTEPS_GOTO;
                    if (p == 0 && AXIS1_DRIVER_MODEL != OFF && translateMicrosteps(AXIS1_DRIVER_MODEL, axis.microsteps, true) == 255) error=true;
                    if (p == 1 && AXIS2_DRIVER_MODEL != OFF && translateMicrosteps(AXIS2_DRIVER_MODEL, axis.microsteps, true) == 255) error=true;
                    if (p == 2 && AXIS3_DRIVER_MODEL != OFF && translateMicrosteps(AXIS3_DRIVER_MODEL, axis.microsteps, true) == 255) error=true;
                    if (p == 3 && AXIS4_DRIVER_MODEL != OFF && translateMicrosteps(AXIS4_DRIVER_MODEL, axis.microsteps, true) == 255) error=true;
                    if (p == 4 && AXIS5_DRIVER_MODEL != OFF && translateMicrosteps(AXIS5_DRIVER_MODEL, axis.microsteps, true) == 255) error=true;
                    if (!error) {
                      nv.writeBytes(EE_settingsAxis1+(p*17),(byte*)&axis,sizeof(axis));
                      boolReply=false;
                    } else commandError=CE_PARAM_RANGE;
                  }
                } else commandError=CE_PARAM_RANGE;
              } else commandError=CE_0;
            } else commandError=CE_0;
          }
        } else
        if (parameter[0] == 'E') { // En: Simple value
          long l;
          switch (parameter[1]) {
            case '7': // stepsPerWormRotation
              l=strtol(&parameter[3],NULL,10);
              if (AXIS1_PEC != ON) l=0;
              if (l >= 0 && l < 129600000) nv.writeLong(EE_stepsPerWormRotAxis1,l); else commandError=CE_PARAM_RANGE;
              break;
#if MOUNT_TYPE == GEM
            case '9': // minutes past meridianE
              f=(double)strtol(&parameter[3],NULL,10)/4.0;
              if (labs(f) <= 180) {
                degreesPastMeridianE=f;
                i=round(degreesPastMeridianE); if (degreesPastMeridianE > 60) i= 60+round((i-60)/2); else if (degreesPastMeridianE < -60) i=-60+round((i+60)/2);
                nv.write(EE_dpmE,round(i+128));
              } else commandError=CE_PARAM_RANGE;
              break;
            case 'A': // minutes past meridianW
              f=(double)strtol(&parameter[3],NULL,10)/4.0;
              if (labs(f) <= 180) {
                degreesPastMeridianW=f;
                i=round(degreesPastMeridianW); if (degreesPastMeridianW > 60) i= 60+round((i-60)/2); else if (degreesPastMeridianW < -60) i=-60+round((i+60)/2);
                nv.write(EE_dpmW,round(i+128));
              } else commandError=CE_PARAM_RANGE;
              break;
#endif
            default: commandError=CE_CMD_UNKNOWN;
          }
        } else
#if DEBUG != OFF
        if (parameter[0] == 'F') { // Fn: Debug
          switch (parameter[1]) {  // DebugF, EEPROM upload
            case 'F':
              if (strlen(parameter) != 24) { commandError=CE_CMD_UNKNOWN; break; }
              char s[5]; s[0]=parameter[3]; s[1]=parameter[4]; s[2]=parameter[5]; s[3]=parameter[6]; s[4]=0;
              long base=(atoi(s)-1)*8;
              int rec[8];
              for (int i=0; i<8; i++) {
                const char ss[17] = "0123456789ABCDEF";
                if (!strchr(ss,parameter[i*2+8]) || !strchr(ss,parameter[i*2+9])) { commandError=CE_PARAM_RANGE; break; }
                int h=parameter[i*2+8]-'0'; if (h > 9) h-=7;
                int l=parameter[i*2+9]-'0'; if (l > 9) l-=7;
                rec[i]=h*16+l;
              }
              for (int i=0; i<8; i++) nv.write(base+i,rec[i]);
            break;
          }
        } else
#endif
#ifdef FEATURES_PRESENT
        if (parameter[0] == 'X') { // Xn: set auXiliary feature
          featuresSetCommand(parameter);
        } else
#endif
          commandError=CE_CMD_UNKNOWN;
       } else
// :Sz[DDD*MM]#
//            Set target object Azimuth to DDD*MM or DDD*MM:SS assumes high precision but falls back to low precision
//            Return: 0 on failure
//                    1 on success
      if (command[1] == 'z')  {
        if (!dmsToDouble(&newTargetAzm,parameter,false,PM_HIGH))
          if (!dmsToDouble(&newTargetAzm,parameter,false,PM_LOW)) commandError=CE_PARAM_FORM;
        } else commandError=CE_CMD_UNKNOWN;
      } else 
// T - Tracking Commands
//
// :T+#       Master sidereal clock faster by 0.02 Hertz (stored in EEPROM)
// :T-#       Master sidereal clock slower by 0.02 Hertz (stored in EEPROM)
// :TS#       Track rate solar
// :TL#       Track rate lunar
// :TQ#       Track rate sidereal
// :TR#       Master sidereal clock reset (to calculated sidereal rate, stored in EEPROM)
// :TK#       Track rate king
//            Returns: Nothing
//
// :Te#       Tracking enable
// :Td#       Tracking disable
// :To#       OnTrack enable
// :Tr#       Track refraction enable
// :Tn#       Track refraction disable
// :T1#       Track single axis (disable Dec tracking on Eq mounts)
// :T2#       Track dual axis
//            Return: 0 on failure
//                    1 on success

      if (command[0] == 'T' && parameter[0] == 0) {
#if MOUNT_TYPE != ALTAZM
        static bool dualAxis=false;
        if (command[1] == 'o') { rateCompensation=RC_FULL_RA; setTrackingRate(DefaultTrackingRate); } else // turn full compensation on, defaults to base sidereal tracking rate
        if (command[1] == 'r') { rateCompensation=RC_REFR_RA; setTrackingRate(DefaultTrackingRate); } else // turn refraction compensation on, defaults to base sidereal tracking rate
        if (command[1] == 'n') { rateCompensation=RC_NONE; dualAxis=false; setTrackingRate(DefaultTrackingRate); } else // turn refraction off, sidereal tracking rate resumes
        if (command[1] == '1') { dualAxis=false; } else                                                      // turn off dual axis tracking
        if (command[1] == '2') { dualAxis=true;  } else                                                      // turn on dual axis tracking
#endif
        if (command[1] == '+') { siderealInterval-=HzCf*(0.02); boolReply=false; } else
        if (command[1] == '-') { siderealInterval+=HzCf*(0.02); boolReply=false; } else
        if (command[1] == 'S') { setTrackingRate(0.99726956632); rateCompensation=RC_NONE; boolReply=false; } else // solar tracking rate 60Hz
        if (command[1] == 'L') { setTrackingRate(0.96236513150); rateCompensation=RC_NONE; boolReply=false; } else // lunar tracking rate 57.9Hz
        if (command[1] == 'Q') { setTrackingRate(DefaultTrackingRate); boolReply=false; } else                     // sidereal tracking rate
        if (command[1] == 'R') { siderealInterval=masterSiderealInterval; boolReply=false; } else                  // reset master sidereal clock interval
        if (command[1] == 'K') { setTrackingRate(0.99953004401); rateCompensation=RC_NONE; boolReply=false; } else // king tracking rate 60.136Hz
        if (command[1] == 'e') {
          if (isParked()) commandError=CE_PARKED; else
          if (trackingState == TrackingMoveTo || trackingSyncInProgress() || isHoming()) commandError=CE_MOUNT_IN_MOTION; else
          {
            initGeneralError();
            trackingState=TrackingSidereal;
            enableStepperDrivers();
          }
        } else
        if (command[1] == 'd') {
          if (trackingState == TrackingMoveTo || trackingSyncInProgress() || isHoming()) commandError=CE_MOUNT_IN_MOTION; else trackingState=TrackingNone;
        } else
          commandError=CE_CMD_UNKNOWN;

#if MOUNT_TYPE != ALTAZM
       if ( dualAxis && rateCompensation == RC_REFR_RA)   rateCompensation=RC_REFR_BOTH;
       if (!dualAxis && rateCompensation == RC_REFR_BOTH) rateCompensation=RC_REFR_RA;
       if ( dualAxis && rateCompensation == RC_FULL_RA)   rateCompensation=RC_FULL_BOTH;
       if (!dualAxis && rateCompensation == RC_FULL_BOTH) rateCompensation=RC_FULL_RA;
#endif

        // Only burn the new rate if changing the sidereal interval
        if (commandError == CE_NONE && (command[1] == '+' || command[1] == '-' || command[1] == 'R')) {
          nv.writeLong(EE_siderealInterval,siderealInterval);
          SiderealClockSetInterval(siderealInterval);
          cli(); siderealRate=siderealInterval/stepsPerSecondAxis1; sei();
        }

        setDeltaTrackingRate();

      } else
     
// U - Precision Toggle
// :U#        Toggle between low/hi precision positions
//            Low -  RA/Dec/etc. displays and accepts HH:MM.M sDD*MM
//            High - RA/Dec/etc. displays and accepts HH:MM:SS sDD*MM:SS
//            Returns: Nothing
      if (command[0] == 'U' && command[1] == 0) { if (precision == PM_LOW) precision=PM_HIGH; else precision=PM_LOW; boolReply=false; } else

#if AXIS1_PEC == ON
// V - PEC Readout
// :VR[n]#    Read PEC table entry rate adjustment (in steps +/-) for worm segment n (in seconds)
//            Returns: sn#
// :VR#       Read PEC table entry rate adjustment (in steps +/-) for currently playing segment and its rate adjustment (in steps +/-)
//            Returns: sn,n#
      if (command[0] == 'V' && command[1] == 'R') {
        bool conv_result=true;
        if (parameter[0] == 0) { i=pecIndex1; } else conv_result=atoi2(parameter,&i);
        if (conv_result) {
          if (i >= 0 && i < pecBufferSize) {
            if (parameter[0] == 0) {
              i-=1; if (i < 0) i+=secondsPerWormRotationAxis1; if (i >= secondsPerWormRotationAxis1) i-=secondsPerWormRotationAxis1;
              i1=pecBuffer[i]-128; sprintf(reply,"%+04i,%03i",i1,i);
            } else {
              i1=pecBuffer[i]-128; sprintf(reply,"%+04i",i1);
            }
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
        boolReply=false;
      } else
// :Vr[n]#    Read out RA PEC ten byte frame in hex format starting at worm segment n (in seconds)
//            Returns: x0x1x2x3x4x5x6x7x8x9# (hex one byte integers)
//            Ten rate adjustment factors for 1s worm segments in steps +/- (steps = x0 - 128, etc.)
      if (command[0] == 'V' && command[1] == 'r') {
        if (atoi2(parameter,&i)) {
          if (i >= 0 && i < pecBufferSize) {
            int j=0;
            byte b;
            char x[3]="  ";
            for (j=0; j < 10; j++) {
              if (i+j < pecBufferSize) b=pecBuffer[i+j]; else b=128;
             
              sprintf(x,"%02X",b);
              strcat(reply,x);
            }
          } else commandError=CE_PARAM_RANGE;
        } else commandError=CE_PARAM_FORM;
        boolReply=false;
      } else
// :VW#       PEC number of steps per worm rotation
//            Returns: n#
      if (command[0] == 'V' && command[1] == 'W' && parameter[0] == 0) {
        sprintf(reply,"%06ld",stepsPerWormRotationAxis1);
        boolReply=false;
      } else
#endif
// :VS#       PEC number of steps per second of worm rotation
//            Returns: n.n#
      if (command[0] == 'V' && command[1] == 'S' && parameter[0] == 0) {
        char temp[12];
        dtostrf(stepsPerSecondAxis1,0,6,temp);
        strcpy(reply,temp);
        boolReply=false;
      } else
#if AXIS1_PEC == ON
//  :VH#      PEC index sense position in seconds
//            Returns: n#
      if (command[0] == 'V' && command[1] == 'H' && parameter[0] == 0) {
        long s=(long)((double)wormSensePos/(double)stepsPerSecondAxis1);
        while (s > secondsPerWormRotationAxis1) s-=secondsPerWormRotationAxis1;
        while (s < 0) s+=secondsPerWormRotationAxis1;
        sprintf(reply,"%05ld",s);
        boolReply=false;
      } else
      
// :WR+#      Move PEC Table ahead by one second
// :WR-#      Move PEC Table back by one second
//            Return: 0 on failure
//                    1 on success
// :WR[n,sn]# Write PEC table entry for worm segment [n] (in seconds) where [sn] is the correction in steps +/- for this 1 second segment
//            Returns: Nothing
      if (command[0] == 'W' && command[1] == 'R') { 
        if (parameter[1] == 0) {
          if (parameter[0] == '+') {
            i=pecBuffer[secondsPerWormRotationAxis1-1];
            memmove((byte *)&pecBuffer[1],(byte *)&pecBuffer[0],secondsPerWormRotationAxis1-1);
            pecBuffer[0]=i;
          } else
          if (parameter[0] == '-') {
            i=pecBuffer[0];
            memmove((byte *)&pecBuffer[0],(byte *)&pecBuffer[1],secondsPerWormRotationAxis1-1);
            pecBuffer[secondsPerWormRotationAxis1-1]=i;
          } commandError=CE_CMD_UNKNOWN;
        } else {
          // it should be an int, see if it converts and is in range
          char *parameter2=strchr(parameter,',');
          if (parameter2) {
            parameter2[0]=0; parameter2++;
            if (atoi2(parameter,&i)) {
              if (i >= 0 && i < pecBufferSize) {
                if (atoi2(parameter2,&i2)) {
                  if (i2 >= -128 && i2 <= 127) {
                    pecBuffer[i]=i2+128;
                    pecRecorded =true;
                  } else commandError=CE_PARAM_RANGE;
                } else commandError=CE_PARAM_FORM;
              } else commandError=CE_PARAM_RANGE;
            } else commandError=CE_PARAM_FORM;
          } else commandError=CE_PARAM_FORM;
          boolReply=false;
        }
      } else
#endif

// W - Site select/get
// :W[n]#     Sets current site to n, where n = 0..3
//            Returns: Nothing
// :W?#       Queries current site
//            Returns: n#
      if (command[0] == 'W') {
        if (command[1] >= '0' && command[1] <= '3' && parameter[0] == 0) {
          currentSite=command[1]-'0'; nv.update(EE_currentSite,currentSite); boolReply=false;
          double f=nv.readFloat(EE_sites+currentSite*25+0);
          if (f < -90 || f > 90) { f=0.0; DLF("ERR, processCommands(): bad NV latitude"); }
          setLatitude(f);
          longitude=nv.readFloat(EE_sites+currentSite*25+4);
          if (longitude < -360 || longitude > 360) { longitude=0.0; DLF("ERR, processCommands(): bad NV longitude"); }
          timeZone=nv.read(EE_sites+currentSite*25+8)-128;
          timeZone=decodeTimeZone(timeZone);
          if (timeZone < -14 || timeZone > 12) { timeZone=0.0; DLF("ERR, processCommands(): bad NV timeZone"); }
          updateLST(jd2last(JD,UT1,false));
        } else 
        if (command[1] == '?') {
          boolReply=false;
          sprintf(reply,"%i",currentSite);
        } else commandError=CE_CMD_UNKNOWN;
      } else commandError=CE_CMD_UNKNOWN;

 // Process reply
      if (boolReply) {
        if (commandError != CE_NONE) reply[0]='0'; else reply[0]='1';
        reply[1]=0;
        supress_frame=true;
      }

      if (process_command == COMMAND_SERIAL_A) {
        if (commandError != CE_NULL) { cmdA.lastError=commandError; logErrors("MSG: CMD_CH_A",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdA.checksum) {
          if (cmdA.checksum)  { checksum(reply); strcat(reply,cmdA.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialA.print(reply);
        }
      } else

#ifdef HAL_SERIAL_B_ENABLED
      if (process_command == COMMAND_SERIAL_B) {
        if (commandError != CE_NULL) { cmdB.lastError=commandError; logErrors("MSG: CMD_CH_B",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdB.checksum) {
          if (cmdB.checksum)  { checksum(reply); strcat(reply,cmdB.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          // static int se=0; se++; if (se == 22) { se=0; reply[2]='x'; } // simulate data corruption
          SerialB.print(reply);
        }
      } else
#endif

#ifdef HAL_SERIAL_C_ENABLED
      if (process_command == COMMAND_SERIAL_C) {
        if (commandError != CE_NULL) { cmdC.lastError=commandError; logErrors("MSG: CMD_CH_C",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdC.checksum) {
          if (cmdC.checksum)  { checksum(reply); strcat(reply,cmdC.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialC.print(reply);
        }
      } else
#endif

#ifdef HAL_SERIAL_D_ENABLED
      if (process_command == COMMAND_SERIAL_D) {
        if (commandError != CE_NULL) { cmdD.lastError=commandError; logErrors("MSG: CMD_CH_D",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdD.checksum) {
          if (cmdD.checksum)  { checksum(reply); strcat(reply,cmdD.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialD.print(reply);
        }
      } else
#endif

#ifdef HAL_SERIAL_E_ENABLED
      if (process_command == COMMAND_SERIAL_E) {
        if (commandError != CE_NULL) { cmdE.lastError=commandError; logErrors("MSG: CMD_CH_E",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdE.checksum) {
          if (cmdE.checksum)  { checksum(reply); strcat(reply,cmdE.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialE.print(reply);
        }
      } else
#endif

#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
      if (process_command == COMMAND_SERIAL_ST4) {
        if (commandError != CE_NULL) { cmdST4.lastError=commandError; logErrors("MSG: CMD_CH_ST4",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdST4.checksum) {
          if (cmdST4.checksum)  { checksum(reply); strcat(reply,cmdST4.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialST4.print(reply);
        }
      } else
#endif

      if (process_command == COMMAND_SERIAL_X) {
        if (commandError != CE_NULL) { cmdX.lastError=commandError; logErrors("MSG: CMD_CH_X",command,parameter,commandError); }
        if (strlen(reply) > 0 || cmdX.checksum) {
          if (cmdX.checksum)  { checksum(reply); strcat(reply,cmdX.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          strcpy(_replyX,reply);
        }
      }

      boolReply=true;
   }
}

// calculate the checksum and add to string
void checksum(char s[]) {
  char HEXS[3]="";
  byte cks=0; for (unsigned int cksCount0=0; cksCount0 < strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
  sprintf(HEXS,"%02X",cks);
  strcat(s,HEXS);
}

void forceRefreshGetEqu() {
  _coord_t=millis()-100UL;
}

// local command processing
bool _ignoreReply=false;
// true if command isn't complete
bool cmdWaiting() {
  if (cmdX.ready()) return true;
  if (_replyX[0] != 0 && !_ignoreReply) return true;
  return false;
}
// set command to be processed and if reply should be be ignored
void cmdSend(const char *s, bool ignoreReply=false) {
  _ignoreReply=ignoreReply;
  _replyX[0]=0;
  cmdX.flush();
  int l=0;
  while (s[l] != 0) { cmdX.add(s[l]); l++; }
}
// get the last command reply
bool cmdReply(char *s) {
  if (_replyX[0] == 0) return false;
  strcpy(s,_replyX); _replyX[0]=0;
  return true;
}

void logErrors(const char ch[], char cmd[], char param[], CommandErrors cmdErr) {
  if (cmdErr <= CE_0) return;
  V(ch); V(" \""); V(cmd); V(param); V("\", Error "); VL(commandErrorStr[cmdErr]);
}

void focuserRotatorSave() {
#if FOCUSER1 == ON
  foc1.setTcfEnable(false);
  foc1.stopMove();
  foc1.savePosition();
#endif
#if FOCUSER2 == ON
  foc2.setTcfEnable(false);
  foc2.stopMove();
  foc2.savePosition();
#endif
#if ROTATOR == ON
  #if MOUNT_TYPE == ALTAZM
    rot.enableDR(false);
  #endif
  rot.stopMove();
  rot.savePosition();
#endif
}
