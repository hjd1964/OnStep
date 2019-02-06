// -----------------------------------------------------------------------------------
// Command processing

// last RA/Dec time
unsigned long _coord_t=0;

// help with commands
enum Command {COMMAND_NONE, COMMAND_SERIAL_A, COMMAND_SERIAL_B, COMMAND_SERIAL_C, COMMAND_SERIAL_ST4, COMMAND_SERIAL_X};
cb cmdA;  // the first Serial is always enabled
#ifdef HAL_SERIAL_B_ENABLED
cb cmdB;
#endif
#ifdef HAL_SERIAL_C_ENABLED
cb cmdC;
#endif
#ifdef ST4_HAND_CONTROL_ON
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
    static char parameter[25];
    static boolean commandError = false;
    static boolean quietReply   = false;

    boolean supress_frame = false;
    char *conv_end;
#ifdef FOCUSER1_ON
    static char primaryFocuser = 'F';
#endif
#ifdef FOCUSER2_ON
    static char secondaryFocuser = 'f';
#endif

    // accumulate the command
    if ((SerialA.available()>0) && (!cmdA.ready())) cmdA.add(SerialA.read());
#ifdef HAL_SERIAL_B_ENABLED
    if ((SerialB.available()>0) && (!cmdB.ready())) cmdB.add(SerialB.read());
#endif
#ifdef HAL_SERIAL_C_ENABLED
    if ((SerialC.available()>0) && (!cmdC.ready())) cmdC.add(SerialC.read());
#endif
#ifdef ST4_HAND_CONTROL_ON
    if ((SerialST4.available()>0) && (!cmdST4.ready())) cmdST4.add(SerialST4.read());
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
#endif

    // if a command is ready, process it
    Command process_command = COMMAND_NONE;
    if (cmdA.ready()) { strcpy(command,cmdA.getCmd()); strcpy(parameter,cmdA.getParameter()); cmdA.flush(); SerialA.flush(); process_command=COMMAND_SERIAL_A; }
#ifdef HAL_SERIAL_B_ENABLED
    else if (cmdB.ready()) { strcpy(command,cmdB.getCmd()); strcpy(parameter,cmdB.getParameter()); cmdB.flush(); SerialB.flush();  process_command=COMMAND_SERIAL_B; }
#endif
#ifdef HAL_SERIAL_C_ENABLED
    else if (cmdC.ready()) { strcpy(command,cmdC.getCmd()); strcpy(parameter,cmdC.getParameter()); cmdC.flush(); SerialC.flush(); process_command=COMMAND_SERIAL_C; }
#endif
#ifdef ST4_HAND_CONTROL_ON
    else if (cmdST4.ready()) { strcpy(command,cmdST4.getCmd()); strcpy(parameter,cmdST4.getParameter()); cmdST4.flush(); process_command=COMMAND_SERIAL_ST4; }
#endif
    else if (cmdX.ready()) { strcpy(command,cmdX.getCmd()); strcpy(parameter,cmdX.getParameter()); cmdX.flush(); process_command=COMMAND_SERIAL_X; }
    else return;

    if (process_command) {
// Command is two chars followed by an optional parameter...
      commandError=false;
// Handles empty and one char replies
      reply[0]=0; reply[1]=0;

//   (char)6 - Special
      if (command[0]==(char)6) {
        if (command[1]=='0') {
          reply[0]=command[1]; strcpy(reply,"CK_FAIL");  // last cmd checksum failed
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
//          Return: 0 on failure
//                  1 on success
        if ((command[1]>='1') && (command[1]<=MAX_NUM_ALIGN_STARS) && (parameter[0]==0)) {
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
       
          if (commandError) { alignNumStars=0; alignThisStar=1; }
        } else
//  :A+#  Manual Alignment, set target location
//         Returns:
//         1: If correction is accepted
//         0: Failure, Manual align mode not set or distance too far 
        if (command[1]=='+') {
          if (alignActive()) {
            commandError = !alignStar();
            if (commandError) { alignNumStars=0; alignThisStar=0; }
          } else commandError=true;
        }
        else
          commandError=true;
      }
      else

//   $ - Set parameter
//  :$BDddd# Set Dec/Alt Antibacklash
//          Return: 0 on failure
//                  1 on success
//  :$BRddd# Set RA/Azm Antibacklash
//          Return: 0 on failure
//                  1 on success
//         Set the Backlash values.  Units are arc-seconds
      if ((command[0]=='$') && (command[1]=='B')) {
        if ((strlen(parameter)>1) && (strlen(parameter)<5)) {
          if ( (atoi2((char*)&parameter[1],&i)) && ((i>=0) && (i<=999))) { 
            if (parameter[0]=='D') {
              reactivateBacklashComp();
              cli(); backlashAxis2=(int)round(((double)i*(double)StepsPerDegreeAxis2)/3600.0); sei();
              nv.writeInt(EE_backlashAxis2,backlashAxis2);
            } else
            if (parameter[0]=='R') {
              reactivateBacklashComp();
              cli(); backlashAxis1 =(int)round(((double)i*(double)StepsPerDegreeAxis1)/3600.0); sei();
              nv.writeInt(EE_backlashAxis1,backlashAxis1);
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
            reactivateBacklashComp();
            i=(int)round(((double)backlashAxis2*3600.0)/(double)StepsPerDegreeAxis2);
            if (i<0) i=0; if (i>999) i=999;
            sprintf(reply,"%d",i);
            quietReply=true;
        } else
        if (parameter[0]=='R') {
            reactivateBacklashComp();
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
//         Returns: Nothing (Sync's fail silently)
//  :CM#   Synchonize the telescope with the current database object (as above)
//         Returns: "N/A#" on success, "En#" on failure where n is the error code per the :MS# command
      if ((command[0]=='C') && ((command[1]=='S') || command[1]=='M'))  {
        if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {

          if (alignActive()) {
            if (alignStar()) i=GOTO_ERR_NONE; else { i=GOTO_ERR_UNSPECIFIED; alignNumStars=0; alignThisStar=0; }
          } else { 
            i=syncEqu(newTargetRA,newTargetDec);
          }

          if (command[1]=='M') {
            if (i==0) strcpy(reply,"N/A");
            if (i>0) { reply[0]='E'; reply[1]='0'+i; reply[2]=0; }
          }

          quietReply=true;
        }
      } else 

//   D - Distance Bars
//  :D#    returns an "\0x7f#" if the mount is moving, otherwise returns "#".
      if ((command[0]=='D') && (command[1]==0))  { if (trackingState==TrackingMoveTo) { reply[0]=(char)127; reply[1]=0; } else { reply[0]='#'; reply[1]=0; supress_frame=true; } quietReply=true; } else 

#ifdef ESP8266_CONTROL_ON
//   E - Enter special mode
//  :ESPFLASH# ESP8266 device flash mode
//         Returns: Never (infinite loop)
//       OnStep must be at home and tracking turned off for this command to work.  A power cycle is required to resume normal operation.
      if (command[0]=='E') {
        if ((command[1]=='S') && (parameter[0]=='P') && (parameter[1]=='F') && (parameter[2]=='L') && (parameter[3]=='A') && (parameter[4]=='S') && (parameter[5]=='H')) {
          if ((atHome) && (trackingState==TrackingNone)) {
            // initialize both serial ports
            SerialA.println("The ESP-01 should now be in flash upload mode (at 115200 Baud.)");
            SerialA.println("Waiting for data.");
            delay(500);
            SerialA.begin(115200);
            SerialB.begin(115200);
            digitalWrite(Axis1_Aux,LOW); delay(20);  // Pgm mode LOW
            
            digitalWrite(Axis2_Aux,LOW);  delay(20); // Reset, if LOW
            digitalWrite(Axis2_Aux,HIGH);            // Reset, inactive HIGH

            while (true) {
              // read from port 1, send to port 0:
              if (SerialB.available()) {
                int inByte = SerialB.read();
                SerialA.write(inByte);
              }
              
              // read from port 0, send to port 1:
              if (SerialA.available()) {
                int inByte = SerialA.read();
                SerialB.write(inByte);
              }
            }
          } else commandError=true;
        } else commandError=true;
      } else
#endif

#ifdef FOCUSER1_ON
//   F - Focuser1 Commands
      if (command[0]==primaryFocuser) {
//  :FA#  Active?
//          Return: 0 on failure
//                  1 on success
//  :FAn# Select focuser 1 or 2
//          Return: 0 on failure
//                  1 on success
        if (command[1]=='A') {
#ifdef FOCUSER2_ON
          if ((parameter[0]=='1') && (parameter[1]==0)) { primaryFocuser='F'; secondaryFocuser='f'; } else
          if ((parameter[0]=='2') && (parameter[1]==0)) { primaryFocuser='f'; secondaryFocuser='F'; } else
#endif
          if (parameter[0]!=0) commandError=true;
        } else
//  :F+#   Move focuser in (toward objective)
//         Returns: Nothing
      if (command[1]=='+') { foc1.startMoveIn(); quietReply=true; } else
//  :F-#   Move focuser out (away from objective)
//         Returns: Nothing
      if (command[1]=='-') { foc1.startMoveOut(); quietReply=true; } else
//  :FQ#   Stop the focuser
//         Returns: Nothing
      if (command[1]=='Q') { foc1.stopMove(); quietReply=true; } else
//  :FG#   Get focuser current position (in microns)
//         Returns: snnn#
      if (command[1]=='G') { sprintf(reply,"%ld",(long)round(foc1.getPosition())); quietReply=true; } else
//  :FI#  Get full in position (in microns)
//         Returns: n#
        if (command[1]=='I') { sprintf(reply,"%ld",(long)round(foc1.getMin())); quietReply=true; } else
//  :FM#  Get max position (in microns)
//         Returns: n#
        if (command[1]=='M') { sprintf(reply,"%ld",(long)round(foc1.getMax())); quietReply=true; } else
//  :FT#  get status
//         Returns: M# (for moving) or S# (for stopped)
        if (command[1]=='T') { if (foc1.moving()) strcpy(reply,"M"); else strcpy(reply,"S"); quietReply=true; } else
//  :FZ#   Set focuser position as zero
//         Returns: Nothing
      if (command[1]=='Z') { foc1.setPosition(0); quietReply=true; } else
//  :FH#   Set focuser position as half-travel
//         Returns: Nothing
      if (command[1]=='H') { foc1.setPosition(((MaxAxis4+MinAxis4)/2.0)*1000.0); quietReply=true; } else
//  :FF#   Set focuser for fast motion (1mm/s)
//         Returns: Nothing
      if (command[1]=='F') { foc1.setMoveRate(1000); quietReply=true; } else
#ifdef AXIS4_DC_MODE_ON
//  :FP#      Get focuser DC Motor Power Level (in %)
//            Returns: nnn#
//  :FPnnn#   Set focuser DC Motor Power Level (in %)
//            Return: 0 on failure
//                    1 on success
      if (command[1]=='P') {
        if (parameter[0]==0) {
          sprintf(reply,"%d",(int)dcPwrAxis4); quietReply=true; 
        } else {
          i=atol(parameter);
          if ((i>=0) && (i<=100)) { dcPwrAxis4=i; foc1.setDcPower(dcPwrAxis4); nv.write(EE_dcPwrAxis4,i); } else commandError=true; 
        }
      } else
#endif
//  :Fp#      Check for focuser pseudo absolute mode
//            Return: 0 on failure
//                    1 on success
      if (command[1]>='p') {
#ifndef AXIS4_DC_MODE_ON
      commandError=true;
#endif
      } else 
//  :FRsnnn#  Set focuser target position relative (in microns)
//            Returns: Nothing
      if (command[1]=='R') { foc1.relativeTarget(atol(parameter)); quietReply=true; } else
//  :FS#      Set focuser for slow motion (0.01mm/s)
//            Returns: Nothing
//  :FSsnnn#  Set focuser target position (in microns)
//            Returns: Nothing
      if (command[1]=='S') { if (parameter[0]==0) { foc1.setMoveRate(10); quietReply=true; } else foc1.setTarget(atol(parameter)); } else
//  :Fn#   Movement rate, 1=finest, 2=0.01mm/second, 3=0.1mm/second, 4=1mm/second
//         Returns: Nothing
      if ((command[1]>='1') && (command[1]<='4')) { i=command[1]-'1'; int p[] = {1,10,100,1000}; foc1.setMoveRate(p[i]); quietReply=true; } else commandError=true;
      } else
#endif

#ifdef FOCUSER2_ON
//   f - Focuser2 Commands
      if (command[0]==secondaryFocuser) {
//  :fA#  Active?
//          Return: 0 on failure
//                  1 on success
//  :fAn# Select focuser 1 or 2
//          Return: 0 on failure
//                  1 on success
        if (command[1]=='A') {
          if ((parameter[0]=='1') && (parameter[1]==0)) { primaryFocuser='F'; secondaryFocuser='f'; } else
          if ((parameter[0]=='2') && (parameter[1]==0)) { primaryFocuser='f'; secondaryFocuser='F'; } else
          if (parameter[0]!=0) commandError=true;
        } else
//  :f+#   Move focuser in (toward objective,) default rate = 0.1mm/second
//         Returns: Nothing
      if (command[1]=='+') { foc2.startMoveIn(); quietReply=true; } else
//  :f-#   Move focuser out (away from objective)
//         Returns: Nothing
      if (command[1]=='-') { foc2.startMoveOut(); quietReply=true; } else
//  :fQ#   Stop the focuser
//         Returns: Nothing
      if (command[1]=='Q') { foc2.stopMove(); quietReply=true; } else
//  :fG#   Get focuser current position (in microns)
//         Returns: snnn#
      if (command[1]=='G') { sprintf(reply,"%ld",(long)round(foc2.getPosition())); quietReply=true; } else
//  :fI#  Get full in position (in microns)
//         Returns: n#
        if (command[1]=='I') { sprintf(reply,"%ld",(long)round(foc2.getMin())); quietReply=true; } else
//  :fM#  Get max position (in microns)
//         Returns: n#
        if (command[1]=='M') { sprintf(reply,"%ld",(long)round(foc2.getMax())); quietReply=true; } else
//  :fT#  get status
//         Returns: M# (for moving) or S# (for stopped)
        if (command[1]=='T') { if (foc2.moving()) strcpy(reply,"M"); else strcpy(reply,"S"); quietReply=true; } else
//  :fZ#   Set focuser position as zero
//         Returns: Nothing
      if (command[1]=='Z') { foc2.setPosition(0); quietReply=true; } else
//  :fH#   Set focuser position as half-travel
//         Returns: Nothing
      if (command[1]=='H') { foc2.setPosition(((MaxAxis5+MinAxis5)/2.0)*1000.0); quietReply=true; } else
//  :fF#   Set focuser for fast motion (1mm/s)
//         Returns: Nothing
      if (command[1]=='F') { foc2.setMoveRate(1000); quietReply=true; } else
#ifdef AXIS5_DC_MODE_ON
//  :fP#      Get focuser DC Motor Power Level (in %)
//            Returns: nnn#
//  :fPnnn#   Set focuser DC Motor Power Level (in %)
//            Return: 0 on failure
//                    1 on success
      if (command[1]=='p') {
        if (parameter[0]==0) {
          sprintf(reply,"%d",(int)dcPwrAxis5); quietReply=true; 
        } else {
          i=atol(parameter);
          if ((i>=0) && (i<=100)) { dcPwrAxis5=i; foc2.setDcPower(dcPwrAxis5); nv.write(EE_dcPwrAxis5,i); } else commandError=true; 
        }
      } else
#endif
//  :fp#      Check for focuser pseudo absolute mode
//            Return: 0 on failure
//                    1 on success
      if (command[1]>='p') {
#ifndef AXIS5_DC_MODE_ON
      commandError=true;
#endif
      } else 
//  :fRsnnn#  Set focuser target position relative (in microns)
//            Returns: Nothing
      if (command[1]=='R') { foc2.relativeTarget(atol(parameter)); quietReply=true; } else
//  :fS#      Set focuser for slow motion (0.01mm/s)
//            Returns: Nothing
//  :fSsnnn#  Set focuser target position (in microns)
//            Returns: Nothing
      if (command[1]=='S') { if (parameter[0]==0) { foc2.setMoveRate(10); quietReply=true; } else foc2.setTarget(atol(parameter)); } else
//  :fn#   Movement rate, 1=finest, 2=0.01mm/second, 3=0.1mm/second, 4=1mm/second
//         Returns: Nothing
      if ((command[1]>='1') && (command[1]<='4')) { i=command[1]-'1'; int p[] = {1,10,100,1000}; foc2.setMoveRate(p[i]); quietReply=true; } else commandError=true;
     } else
#endif

//   G - Get Telescope Information
      if (command[0]=='G') {

//  :GA#   Get Telescope Altitude
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
//         The current scope altitude
      if (command[1]=='A')  { getHor(&f,&f1); if (!doubleToDms(reply,&f,false,true)) commandError=true; else quietReply=true; } else
//  :Ga#   Get Local Time in 12 hour format
//         Returns: HH:MM:SS#
      if (command[1]=='a')  { LMT=timeRange(UT1-timeZone); if (LMT>12.0) LMT-=12.0; if (!doubleToHms(reply,&LMT,true)) commandError=true; else quietReply=true; } else 
//  :GC#   Get the current date
//         Returns: MM/DD/YY#
//         The current local calendar date
      if (command[1]=='C') { 
        LMT=UT1-timeZone;
        // correct for day moving forward/backward... this works for multipule days of up-time
        double J=JD;
        int y,m,d;
        while (LMT>=24.0) { LMT=LMT-24.0; J=J-1.0; } 
        if    (LMT<0.0)   { LMT=LMT+24.0; J=J+1.0; }
        greg(J,&y,&m,&d); y-=2000; if (y>=100) y-=100;
        sprintf(reply,"%02d/%02d/%02d",m,d,y); 
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
//  :GDe#   Get Telescope Declination
//         Returns: sDD*MM'SS.s#
      if (command[1]=='D')  { 
#ifdef HAL_SLOW_PROCESSOR
        if (millis()-_coord_t<100)
#else
        if (millis()-_coord_t<5)
#endif
        { f=_ra; f1=_dec; } else {
          getEqu(&f,&f1,false); f/=15.0;
          _ra=f; _dec=f1; _coord_t=millis(); 
        }
        if (parameter[0]==0) {
          if (!doubleToDms(reply,&f1,false,true)) commandError=true; else quietReply=true; 
        } else
        if ((parameter[0]=='e') && (parameter[1]==0)) {
          if (!doubleToDmsd(reply,&f1)) commandError=true; else quietReply=true; 
        } else commandError=true;
      } else 
//  :Gd#   Get Currently Selected Target Declination
//         Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
//  :Gde#  Get Currently Selected Target Declination
//         Returns: sDD*MM'SS.s#
      if (command[1]=='d')  { 
        if (parameter[0]==0) {
          if (!doubleToDms(reply,&newTargetDec,false,true)) commandError=true; else quietReply=true; 
        } else
        if ((parameter[0]=='e') && (parameter[1]==0)) {
          if (!doubleToDmsd(reply,&newTargetDec)) commandError=true; else quietReply=true; 
        } else commandError=true;
      } else 
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
      if (command[1]=='L')  { LMT=timeRange(UT1-timeZone); if (!doubleToHms(reply,&LMT,true)) commandError=true; else quietReply=true; } else 
//  :GM#   Get Site 1 Name
//  :GN#   Get Site 2 Name
//  :GO#   Get Site 3 Name
//  :GP#   Get Site 4 Name
//         Returns: <string>#
//         A # terminated string with the name of the requested site.
      if ((command[1]=='M') || (command[1]=='N') || (command[1]=='O') || (command[1]=='P'))  { 
        i=command[1]-'M';
        nv.readString(EE_sites+i*25+9,reply); 
        if (reply[0]==0) { strcat(reply,"None"); }
        quietReply=true; 
      } else
//  :Gm#   Gets the meridian pier-side
//         Returns: E#, W#, N# (none/parked), ?# (Meridian flip in progress) 
//         A # terminated string with the pier side.
      if (command[1]=='m')  {
        reply[0]='?'; reply[1]=0;
        if (getInstrPierSide()==PierSideNone) reply[0]='N';
        if (getInstrPierSide()==PierSideEast) reply[0]='E';
        if (getInstrPierSide()==PierSideWest) reply[0]='W';
        quietReply=true; } else
//  :Go#   Get Overhead Limit
//         Returns: DD*#
//         The highest elevation above the horizon that the telescope will goto
      if (command[1]=='o')  { sprintf(reply,"%02d*",maxAlt); quietReply=true; } else
//  :GR#   Get Telescope RA
//         Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
//  :GRa#  Get Telescope RA
//         Returns: HH:MM:SS.s#
      if (command[1]=='R')  {
#ifdef HAL_SLOW_PROCESSOR
        if (millis()-_coord_t<100)
#else
        if (millis()-_coord_t<5)
#endif
        { f=_ra; f1=_dec; } else {
          getEqu(&f,&f1,false); f/=15.0;
          _ra=f; _dec=f1; _coord_t=millis(); 
        }
        if (parameter[0]==0) {
           if (!doubleToHms(reply,&f,highPrecision)) commandError=true; else quietReply=true;  
        } else
        if ((parameter[0]=='a') && (parameter[1]==0)) {
          if (!doubleToHmsd(reply,&f)) commandError=true; else quietReply=true;
        } else commandError=true;
      } else 
//  :Gr#   Get current/target object RA
//         Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
//  :Gra#  Get Telescope RA
//         Returns: HH:MM:SS.ss#
      if (command[1]=='r')  {
        f=newTargetRA; f/=15.0;
        if (parameter[0]==0) {
           if (!doubleToHms(reply,&f,highPrecision)) commandError=true; else quietReply=true;
        } else
        if ((parameter[0]=='a') && (parameter[1]==0)) {
          if (!doubleToHmsd(reply,&f)) commandError=true; else quietReply=true;
        } else commandError=true;
      } else 
//  :GS#   Get the Sidereal Time
//         Returns: HH:MM:SS#
//         The Sidereal Time as an ASCII Sexidecimal value in 24 hour format
      if (command[1]=='S')  { f=LST(); if (!doubleToHms(reply,&f,true)) commandError=true; else quietReply=true; } else 
//  :GT#   Get tracking rate
//         Returns: dd.ddddd# (OnStep returns more decimal places than LX200 standard)
//         Returns the tracking rate if siderealTracking, 0.0 otherwise
      if (command[1]=='T')  {
        if (trackingState==TrackingSidereal) {
#ifdef MOUNT_TYPE_ALTAZM
          f=getTrackingRate()*1.00273790935*60.0; 
#else
          cli();
          f=(trackingTimerRateAxis1*1.00273790935)*60.0;
          sei();
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
        if ((trackingState!=TrackingSidereal) || trackingSyncInProgress()) reply[i++]='n'; // [n]ot tracking
        if ((trackingState!=TrackingMoveTo) && !trackingSyncInProgress())  reply[i++]='N'; // [N]o goto
        const char *parkStatusCh = "pIPF";       reply[i++]=parkStatusCh[parkStatus]; // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
        if (pecRecorded)                         reply[i++]='R';                      // PEC data has been [R]ecorded
        if (atHome)                              reply[i++]='H';                      // at [H]ome
        if (PPSsynced)                           reply[i++]='S';                      // PPS [S]ync
        if ((guideDirAxis1) || (guideDirAxis2))  reply[i++]='G';                      // [G]uide active
        if (faultAxis1 || faultAxis2)            reply[i++]='f';                      // axis [f]ault
#ifndef MOUNT_TYPE_ALTAZM
        if (rateCompensation==RC_REFR_RA)      { reply[i++]='r'; reply[i++]='s'; }    // [r]efr enabled [s]ingle axis
        if (rateCompensation==RC_REFR_BOTH)    { reply[i++]='r'; }                    // [r]efr enabled
        if (rateCompensation==RC_FULL_RA)      { reply[i++]='t'; reply[i++]='s'; }    // on[t]rack enabled [s]ingle axis
        if (rateCompensation==RC_FULL_BOTH)    { reply[i++]='t'; }                    // on[t]rack enabled
#endif
        if (waitingHome)                         reply[i++]='w';                      // [w]aiting at home
        if (pauseHome)                           reply[i++]='u';                      // pa[u]se at home enabled?
        if (soundEnabled)                        reply[i++]='z';                      // bu[z]zer enabled?
#ifdef MOUNT_TYPE_GEM
        if (autoMeridianFlip)                    reply[i++]='a';                      // [a]uto meridian flip
#endif
#ifndef MOUNT_TYPE_ALTAZM        
        const char *pch = PECStatusStringAlt; reply[i++]=pch[pecStatus];              // PEC Status is one of "/,~;^" (/)gnore, get ready to (,)lay, (~)laying, get ready to (;)ecord, (^)ecording
        // if (wormSensedAgain) { reply[i++]='.'; wormSensedAgain=false; }            // PEC optional (.) to indicate an index detect since last call
#endif
        // provide mount type
        #if defined(MOUNT_TYPE_GEM)
        reply[i++]='E';
        #elif defined(MOUNT_TYPE_FORK)
        reply[i++]='K';
        #elif defined(MOUNT_TYPE_FORK_ALT)
        reply[i++]='k';
        #elif defined(MOUNT_TYPE_ALTAZM)
        reply[i++]='A';
        #endif

        // provide pier side info.
        if (getInstrPierSide()==PierSideNone) reply[i++]='o'; else                    // pier side n[o]ne
        if (getInstrPierSide()==PierSideEast) reply[i++]='T'; else                    // pier side eas[T]
        if (getInstrPierSide()==PierSideWest) reply[i++]='W';                         // pier side [W]est

        // provide last error
        reply[i++]='0'+lastError;
        reply[i++]=0;
        quietReply=true;

      } else
//  :GVD# Get Telescope Firmware Date
//         Returns: mmm dd yyyy#
//  :GVM# General Message
//         returns: s# (where s is a string up to 16 chars)
//  :GVN# Get Telescope Firmware Number
//         Returns: d.dc#
//  :GVP# Get Telescope Product Name
//         Returns: <string>#
//  :GVT# Get Telescope Firmware Time
//         returns: HH:MM:SS#
      if (command[1]=='V') {
        if (parameter[1]==(char)0) {
          if (parameter[0]=='D') strcpy(reply,FirmwareDate); else
          if (parameter[0]=='M') sprintf(reply,"OnStep %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
          if (parameter[0]=='N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
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
            static int star=0;
            switch (parameter[1]) {
              case '0': sprintf(reply,"%ld",(long)(Align.ax1Cor*3600.0)); quietReply=true; break; // ax1Cor
              case '1': sprintf(reply,"%ld",(long)(Align.ax2Cor*3600.0)); quietReply=true; break; // ax2Cor
              case '2': sprintf(reply,"%ld",(long)(Align.altCor*3600.0)); quietReply=true; break; // altCor
              case '3': sprintf(reply,"%ld",(long)(Align.azmCor*3600.0)); quietReply=true; break; // azmCor
              case '4': sprintf(reply,"%ld",(long)(Align.doCor*3600.0)); quietReply=true; break;  // doCor
              case '5': sprintf(reply,"%ld",(long)(Align.pdCor*3600.0)); quietReply=true; break;  // pdCor
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
              case '6': sprintf(reply,"%ld",(long)(Align.dfCor*3600.0)); quietReply=true; break;  // ffCor
              case '7': sprintf(reply,"%ld",(long)(0)); quietReply=true; break;                   // dfCor
#else
              case '6': sprintf(reply,"%ld",(long)(0)); quietReply=true; break;                   // ffCor
              case '7': sprintf(reply,"%ld",(long)(Align.dfCor*3600.0)); quietReply=true; break;  // dfCor
#endif
              case '8': sprintf(reply,"%ld",(long)(Align.tfCor*3600.0)); quietReply=true; break;  // tfCor

              case '9': { int n=0; if (alignThisStar>alignNumStars) n=alignNumStars; sprintf(reply,"%ld",(long)(n)); star=0; quietReply=true; } break; // Number of stars, reset to first star
              case 'A': { double f=(Align.actual[star].ha*Rad)/15.0; doubleToHms(reply,&f,true);       quietReply=true; } break; // Star  #n HA
              case 'B': { double f=(Align.actual[star].dec*Rad);     doubleToDms(reply,&f,false,true); quietReply=true; } break; // Star  #n Dec
              case 'C': { double f=(Align.mount[star].ha*Rad)/15.0;  doubleToHms(reply,&f,true);       quietReply=true; } break; // Mount #n HA
              case 'D': { double f=(Align.mount[star].dec*Rad);      doubleToDms(reply,&f,false,true); quietReply=true; } break; // Mount #n Dec
              case 'E': sprintf(reply,"%ld",(long)(Align.mount[star].side)); star++; quietReply=true; break;                     // Mount PierSide (and increment n)
              default:  commandError=true;
            }
          } else
          if (parameter[0]=='4') { // 4n: Encoder
            switch (parameter[1]) {
              case '0': if (getEnc(&f,&f1)==0) { if (!doubleToDms(reply,&f,true,true)) commandError=true; else quietReply=true; } else commandError=true; break;  // Get formatted absolute Axis1 angle
              case '1': if (getEnc(&f,&f1)==0) { if (!doubleToDms(reply,&f1,true,true)) commandError=true; else quietReply=true; } else commandError=true; break; // Get formatted absolute Axis2 angle 
              case '2': if (getEnc(&f,&f1)==0) { dtostrf(f,0,6,reply); quietReply=true; } else commandError=true; break;                                          // Get absolute Axis1 angle in degrees
              case '3': if (getEnc(&f,&f1)==0) { dtostrf(f1,0,6,reply); quietReply=true; } else commandError=true; break;                                         // Get absolute Axis2 angle in degrees
              case '9': cli(); dtostrf(trackingTimerRateAxis1,1,8,reply); sei(); quietReply=true; break;                                                          // Get current tracking rate
              default:  commandError=true;
            }
          } else
          if (parameter[0]=='8') { // 8n: Date/Time
            switch (parameter[1]) {
              case '0': f=timeRange(UT1); doubleToHms(reply,&f,true); quietReply=true; break;  // UTC time
              case '1': f1=JD; f=UT1; while (f>=24.0) { f-=24.0; f1+=1; } while (f<0.0) { f+=24.0; f1-=1; } greg(f1,&i2,&i,&i1); i2=(i2/99.99999-floor(i2/99.99999))*100; sprintf(reply,"%02d/%02d/%02d",i,i1,i2); quietReply=true; break; // UTC date
              default:  commandError=true;
            }
          } else
          if (parameter[0]=='9') { // 9n: Misc.
            switch (parameter[1]) {
              case '0': dtostrf(guideRates[currentPulseGuideRate]/15.0,2,2,reply); quietReply=true; break;  // pulse-guide rate
              case '1': sprintf(reply,"%i",pecAnalogValue); quietReply=true; break;                         // pec analog value
              case '2': sprintf(reply,"%ld",(long)(maxRate/16L)); quietReply=true; break;                   // MaxRate
              case '3': sprintf(reply,"%ld",(long)(MaxRate)); quietReply=true; break;                       // MaxRate (default)
              case '4': if (meridianFlip==MeridianFlipNever) { sprintf(reply,"%d N",getInstrPierSide()); } else { sprintf(reply,"%d",getInstrPierSide()); } quietReply=true; break; // pierSide (N if never)
              case '5': sprintf(reply,"%i",(int)autoMeridianFlip); quietReply=true; break;                  // autoMeridianFlip
              case '6':                                                                                     // preferred pier side
                if (preferredPierSide==PPS_EAST) strcpy(reply,"E"); else
                if (preferredPierSide==PPS_WEST) strcpy(reply,"W"); else strcpy(reply,"B");
                quietReply=true; break;
              case '7': dtostrf(slewSpeed,3,1,reply); quietReply=true; break;                               // slew speed
              case '8': 
#ifdef ROTATOR_ON
#ifdef MOUNT_TYPE_ALTAZM
                strcpy(reply,"D");
#else
                strcpy(reply,"R");
#endif
#else
                strcpy(reply,"N");
#endif
                quietReply=true; break; // rotator availablity 2=rotate/derotate, 1=rotate, 0=off
              case 'A': dtostrf(ambient.getTemperature(),3,1,reply); quietReply=true; break;       // temperature in deg. C
              case 'B': dtostrf(ambient.getPressure(),3,1,reply); quietReply=true; break;          // pressure in mb
              case 'C': dtostrf(ambient.getHumidity(),3,1,reply); quietReply=true; break;          // relative humidity in %
              case 'D': dtostrf(ambient.getAltitude(),3,1,reply); quietReply=true; break;          // altitude in meters
              case 'E': dtostrf(ambient.getDewPoint(),3,1,reply); quietReply=true; break;          // dew point in deg. C
              case 'F': { float t=HAL_MCU_Temperature(); if (t>-999) { dtostrf(t,1,0,reply); quietReply=true; } else commandError=true; } break; // internal MCU temperature in deg. C
              default:  commandError=true;
            }
          } else
#if (AXIS1_FAULT==TMC2130) && (AXIS2_FAULT==TMC2130)
          if (parameter[0]=='U') { // Un: Get stepper driver statUs

            switch (parameter[1]) {
              case '1':
                tmcAxis1.getStatus();
                strcat(reply,tmcAxis1.statusSTST() ? "ST," : ","); // Standstill
                strcat(reply,tmcAxis1.statusOLa() ? "OA," : ",");  // Open Load A
                strcat(reply,tmcAxis1.statusOLb() ? "OB," : ",");  // Open Load B
                strcat(reply,tmcAxis1.statusS2Ga() ? "GA," : ","); // Short to Ground A
                strcat(reply,tmcAxis1.statusS2Gb() ? "GB," : ","); // Short to Ground B
                strcat(reply,tmcAxis1.statusOT() ? "OT," : ",");   // Overtemperature Shutdown 150C
                strcat(reply,tmcAxis1.statusOTPW() ? "PW" : "");   // Overtemperature Pre-warning 120C
                quietReply=true;
              break;
              case '2':
                tmcAxis2.getStatus();
                strcat(reply,tmcAxis2.statusSTST() ? "ST," : ","); // Standstill
                strcat(reply,tmcAxis2.statusOLa() ? "OA," : ",");  // Open Load A
                strcat(reply,tmcAxis2.statusOLb() ? "OB," : ",");  // Open Load B
                strcat(reply,tmcAxis2.statusS2Ga() ? "GA," : ","); // Short to Ground A
                strcat(reply,tmcAxis2.statusS2Gb() ? "GB," : ","); // Short to Ground B
                strcat(reply,tmcAxis2.statusOT() ? "OT," : ",");   // Overtemperature Shutdown 150C
                strcat(reply,tmcAxis2.statusOTPW() ? "PW" : "");   // Overtemperature Pre-warning 120C
                quietReply=true;
              break;
              default:  commandError=true;
            }
          } else
#endif
          if (parameter[0]=='E') { // En: Get settings
            switch (parameter[1]) {
              case '1': sprintf(reply,"%ld",(long)MaxRate); quietReply=true; break;
              case '2': dtostrf(DegreesForAcceleration,2,1,reply); quietReply=true; break;
              case '3': sprintf(reply,"%ld",(long)round(BacklashTakeupRate)); quietReply=true; break;
              case '4': sprintf(reply,"%ld",(long)round(StepsPerDegreeAxis1)); quietReply=true; break;
              case '5': sprintf(reply,"%ld",(long)round(StepsPerDegreeAxis2)); quietReply=true; break;
              case '6': dtostrf(StepsPerSecondAxis1,3,6,reply); quietReply=true; break;
              case '7': sprintf(reply,"%ld",(long)round(StepsPerWormRotationAxis1)); quietReply=true; break;
              case '8': sprintf(reply,"%ld",(long)round(PECBufferSize)); quietReply=true; break;
#ifdef MOUNT_TYPE_GEM
              case '9': sprintf(reply,"%ld",(long)round(minutesPastMeridianE)); quietReply=true; break;
              case 'A': sprintf(reply,"%ld",(long)round(minutesPastMeridianW)); quietReply=true; break;
#endif
              case 'B': sprintf(reply,"%ld",(long)round(UnderPoleLimit)); quietReply=true; break;
              case 'C': sprintf(reply,"%ld",(long)round(MinDec)); quietReply=true; break;
              case 'D': sprintf(reply,"%ld",(long)round(MaxDec)); quietReply=true; break;
              default:  commandError=true;
            }
          } else
          if (parameter[0]=='F') { // Fn: Debug
            long temp;
            switch (parameter[1]) {
              case '0': cli(); temp=(long)(posAxis1-((long)targetAxis1.part.m)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break; // Debug0, true vs. target RA position
              case '1': cli(); temp=(long)(posAxis2-((long)targetAxis2.part.m)); sei(); sprintf(reply,"%ld",temp); quietReply=true; break; // Debug1, true vs. target Dec position
              case '2': cli(); temp=(long)trackingState; sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                         // Debug2, trackingState
              case '3': sprintf(reply,"%ld",(long)(_deltaAxis1*1000.0*1.00273790935)); quietReply=true; break;                             // Debug3, RA refraction tracking rate
              case '4': sprintf(reply,"%ld",(long)(_deltaAxis2*1000.0*1.00273790935)); quietReply=true; break;                             // Debug4, Dec refraction tracking rate
              case '6': cli(); temp=(long)(targetAxis1.part.m); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                  // Debug6, HA target position
              case '7': cli(); temp=(long)(targetAxis2.part.m); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                  // Debug7, Dec target position
              case '8': cli(); temp=(long)(posAxis1); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                            // Debug8, HA motor position
              case '9': cli(); temp=(long)(posAxis2); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;                            // Debug9, Dec motor position
              case 'A': sprintf(reply,"%ld%%",(worst_loop_time*100L)/9970L); worst_loop_time=0; quietReply=true; break;                    // DebugA, Workload
              case 'B': cli(); temp=(long)(trackingTimerRateAxis1*1000.0); sei(); sprintf(reply,"%ld",temp); quietReply=true; break;       // DebugB, trackingTimerRateAxis1
              case 'E': double ra, de; cli(); getEqu(&ra,&de,false); sei(); sprintf(reply,"%f,%f",ra,de); quietReply=true; break;          // DebugE, equatorial coordinates degrees (no division by 15)
              default:  commandError=true;
            }
          } else
#ifdef Aux0
          if ((parameter[0]=='G') && (parameter[1]=='0')) { sprintf(reply,"%d",(int)round((float)valueAux0/2.55)); quietReply=true; } else
#endif
#ifdef Aux1
          if ((parameter[0]=='G') && (parameter[1]=='1')) { sprintf(reply,"%d",(int)round((float)valueAux1/2.55)); quietReply=true; } else
#endif
#ifdef Aux2
          if ((parameter[0]=='G') && (parameter[1]=='2')) { sprintf(reply,"%d",(int)round((float)valueAux2/2.55)); quietReply=true; } else
#endif
#ifdef Aux3
          if ((parameter[0]=='G') && (parameter[1]=='3')) { sprintf(reply,"%d",(int)round((float)valueAux3/2.55)); quietReply=true; } else
#endif
#ifdef Aux4
          if ((parameter[0]=='G') && (parameter[1]=='4')) { sprintf(reply,"%d",(int)round((float)valueAux4/2.55)); quietReply=true; } else
#endif
#ifdef Aux5
          if ((parameter[0]=='G') && (parameter[1]=='5')) { sprintf(reply,"%d",(int)round((float)valueAux5/2.55)); quietReply=true; } else
#endif
#ifdef Aux6
          if ((parameter[0]=='G') && (parameter[1]=='6')) { sprintf(reply,"%d",(int)round((float)valueAux6/2.55)); quietReply=true; } else
#endif
#ifdef Aux7
          if ((parameter[0]=='G') && (parameter[1]=='7')) { sprintf(reply,"%d",(int)round((float)valueAux7/2.55)); quietReply=true; } else
#endif
#ifdef Aux8
          if ((parameter[0]=='G') && (parameter[1]=='8')) { sprintf(reply,"%d",(int)round((float)valueAux8/2.55)); quietReply=true; } else
#endif
#ifdef Aux9
          if ((parameter[0]=='G') && (parameter[1]=='9')) { sprintf(reply,"%d",(int)round((float)valueAux9/2.55)); quietReply=true; } else
#endif
#ifdef Aux10
          if ((parameter[0]=='G') && (parameter[1]=='A')) { sprintf(reply,"%d",(int)round((float)valueAux10/2.55)); quietReply=true; } else
#endif
#ifdef Aux11
          if ((parameter[0]=='G') && (parameter[1]=='B')) { sprintf(reply,"%d",(int)round((float)valueAux11/2.55)); quietReply=true; } else
#endif
#ifdef Aux12
          if ((parameter[0]=='G') && (parameter[1]=='C')) { sprintf(reply,"%d",(int)round((float)valueAux12/2.55)); quietReply=true; } else
#endif
#ifdef Aux13
          if ((parameter[0]=='G') && (parameter[1]=='D')) { sprintf(reply,"%d",(int)round((float)valueAux13/2.55)); quietReply=true; } else
#endif
#ifdef Aux14
          if ((parameter[0]=='G') && (parameter[1]=='E')) { sprintf(reply,"%d",(int)round((float)valueAux14/2.55)); quietReply=true; } else
#endif
#ifdef Aux15
          if ((parameter[0]=='G') && (parameter[1]=='F')) { sprintf(reply,"%d",(int)round((float)valueAux15/2.55)); quietReply=true; } else
#endif
            commandError=true;
        } else commandError=true;
      } else
//  :GZ#   Get telescope azimuth
//         Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
      if (command[1]=='Z')  { getHor(&f,&f1); f1=degRange(f1); if (!doubleToDms(reply,&f1,true,false)) commandError=true; else quietReply=true; } else commandError=true;
      } else

//  h - Home Position Commands
      if (command[0]=='h') {
//  :hF#   Reset telescope at the home position.  This position is required for a Cold Start.
//         Point to the celestial pole with the counterweight pointing downwards (CWD position).
//         Returns: Nothing
      if (command[1]=='F')  { 
#ifdef FOCUSER1_ON
        foc1.savePosition();
#endif
#ifdef FOCUSER2_ON
        foc2.savePosition();
#endif
        setHome(); quietReply=true; 
      } else 
//  :hC#   Moves telescope to the home position
//         Returns: Nothing
      if (command[1]=='C')  { 
#ifdef FOCUSER1_ON
        foc1.savePosition();
#endif
#ifdef FOCUSER2_ON
        foc2.savePosition();
#endif
        goHome(true); quietReply=true;
      } else 
//  :hP#   Goto the Park Position
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='P')  { 
#ifdef FOCUSER1_ON
        foc1.savePosition();
#endif
#ifdef FOCUSER2_ON
        foc2.savePosition();
#endif
        if (park()) commandError=true; 
        } else 
//  :hQ#   Set the park position
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='Q')  { if (!setPark()) commandError=true; } else 
//  :hR#   Restore parked telescope to operation
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='R')  { if (!unPark()) commandError=true; }
      else commandError=true;

      } else

//   L - Object Library Commands
      if (command[0]=='L') {

// :LB#    Find previous object and set it as the current target object.
//          Returns: Nothing
      if ((command[1]=='B') && (parameter[0]==0)) { 
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
      if ((command[1]=='I') && (parameter[0]==0)) {
        Lib.readVars(reply,&i,&newTargetRA,&newTargetDec);

        char const * objType=objectStr[i];
        strcat(reply,",");
        strcat(reply,objType);
        quietReply=true;
      } else 

// :LIG#   Get Object Information and goto
//         Returns: Nothing
      if ((command[1]=='I') && (parameter[0]=='G') && (parameter[1]==0)) {
        Lib.readVars(reply,&i,&newTargetRA,&newTargetDec);

        goToEqu(newTargetRA,newTargetDec);
        quietReply=true;
      } else 

// :LR#    Get Object Information including RA and Dec, with advance to next Record
//          Returns: <string>#
//          Returns a string containing the current target object’s name, type, RA, and Dec.
      if ((command[1]=='R') && (parameter[0]==0)) {
        Lib.readVars(reply,&i,&newTargetRA,&newTargetDec);

        char const * objType=objectStr[i];
        char ws[20];

        strcat(reply,",");
        strcat(reply,objType);
        if (strcmp(reply,",UNK")!=0) {
          f=newTargetRA; f/=15.0; doubleToHms(ws,&f,highPrecision); strcat(reply,","); strcat(reply,ws);
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
      if ((command[1]=='N') && (parameter[0]==0)) { 
          Lib.nextRec();
          quietReply=true;
      } else 

// :L$#    Move to catalog name record
//          Returns: 1
      if ((command[1]=='$') && (parameter[0]==0)) { 
          Lib.nameRec();
          quietReply=false;
      } else 

// :LD#    Clear current record
//          Returns: Nothing
      if ((command[1]=='D') && (parameter[0]==0)) { 
          Lib.clearCurrentRec();
          quietReply=true;
      } else 

// :LL#    Clear current catalog
//          Returns: Nothing
      if ((command[1]=='L') && (parameter[0]==0)) { 
          Lib.clearLib();
          quietReply=true;
      } else 

// :L!#    Clear library (all catalogs)
//          Returns: Nothing
      if ((command[1]=='!') && (parameter[0]==0)) { 
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
//         Returns: 0..9, see :MS#
      if (command[1]=='A') {
        if (axis1Enabled) {
          i=goToHor(&newTargetAlt, &newTargetAzm);
          reply[0]=i+'0'; reply[1]=0; 
          quietReply=true; 
          supress_frame=true;
        } else { reply[0]='4'; reply[1]=0; }
      } else
//  :Mgdnnnn# Pulse guide command
//          Returns: Nothing
      if (command[1]=='g') {
        if ( (atoi2((char *)&parameter[1],&i)) && ((i>=0) && (i<=16399)) ) {
          if (((parameter[0]=='e') || (parameter[0]=='w')) && (guideDirAxis1==0)) {
#ifdef SEPARATE_PULSE_GUIDE_RATE_ON
            startGuideAxis1(parameter[0],currentPulseGuideRate,i);
#else
            startGuideAxis1(parameter[0],currentGuideRate,i);
#endif
            quietReply=true;
          } else
          if (((parameter[0]=='n') || (parameter[0]=='s')) && (guideDirAxis2==0)) { 
#ifdef SEPARATE_PULSE_GUIDE_RATE_ON
            startGuideAxis2(parameter[0],currentPulseGuideRate,i);
#else
            startGuideAxis2(parameter[0],currentGuideRate,i);
#endif
            quietReply=true;
          } else commandError=true;
        } else commandError=true;
      } else
//  :Me# & :Mw#      Move Telescope East or West at current slew rate
//         Returns: Nothing
      if ( ( (command[1]=='e') || (command[1]=='w') ) && (parameter[0]==0) ) {
        startGuideAxis1(command[1],currentGuideRate,GUIDE_TIME_LIMIT*1000);
        quietReply=true;
      } else
//  :Mn# & :Ms#      Move Telescope North or South at current slew rate
//         Returns: Nothing
      if ( ( (command[1]=='n') || (command[1]=='s') ) && (parameter[0]==0) ) {
        startGuideAxis2(command[1],currentGuideRate,GUIDE_TIME_LIMIT*1000);
        quietReply=true;
      } else

//  :MP#   Goto the Current Position for Polar Align
//         Returns: 0..9, see :MS#
      if (command[1]=='P')  {
        double r,d;
        getEqu(&r,&d,false);
        i=validateGoToEqu(r,d);
        if (i==0) {
          Align.altCor=0.0;
          Align.azmCor=0.0;
          i=goToEqu(r,d);
        }
        reply[0]=i+'0'; reply[1]=0;
        quietReply=true;
        supress_frame=true;
      } else

//  :MS#   Goto the Target Object
//         Returns:
//         0=Goto is possible
//         1=below the horizon limit
//         2=above overhead limit
//         3=controller in standby
//         4=mount is parked
//         5=Goto in progress
//         6=outside limits (MaxDec, MinDec, UnderPoleLimit, MeridianLimit)
//         7=hardware fault
//         8=already in motion
//         9=unspecified error
      if (command[1]=='S')  {
        i=goToEqu(newTargetRA,newTargetDec);
        reply[0]=i+'0'; reply[1]=0;
        quietReply=true;
        supress_frame=true; 
      } else

//  :MN#   Goto current RA/Dec but East of the Pier (within meridian limit overlap for GEM mounts)
//         Returns: 0..9, see :MS#
      if (command[1]=='N')  {
        i=goToHere(true);
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
          if (parameter[1]=='+') { if (pecRecorded) pecStatus=ReadyPlayPEC; nv.update(EE_pecStatus,pecStatus); } else
          if (parameter[1]=='-') { pecStatus=IgnorePEC; nv.update(EE_pecStatus,pecStatus); } else
          if ((parameter[1]=='/') && (trackingState==TrackingSidereal)) { pecStatus=ReadyRecordPEC; nv.update(EE_pecStatus,IgnorePEC); } else
          if (parameter[1]=='Z') { 
            for (i=0; i<PECBufferSize; i++) pecBuffer[i]=128;
            pecFirstRecord = true;
            pecStatus      = IgnorePEC;
            pecRecorded    = false;
            nv.update(EE_pecStatus,pecStatus);
            nv.update(EE_pecRecorded,pecRecorded);
          } else
          if (parameter[1]=='!') {
            pecRecorded=true;
            nv.update(EE_pecRecorded,pecRecorded);
            nv.writeLong(EE_wormSensePos,wormSensePos);
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
          stopMount();
          quietReply=true; 
        } else
//  :Qe# & Qw#   Halt east/westward Slews
//         Returns: Nothing
        if ( ( (command[1]=='e') || (command[1]=='w') ) && (parameter[0]==0) ) {
          stopGuideAxis1();
          quietReply=true;
        } else
//  :Qn# & Qs#   Halt north/southward Slews
//         Returns: Nothing
        if ( ( (command[1]=='n') || (command[1]=='s') ) && (parameter[0]==0) ) {
          stopGuideAxis2();
          quietReply=true;
        } else commandError=true;
      } else

//   R - Slew Rate Commands
      if (command[0]=='R') {

//   :RAdd.d#   Set Axis1 Guide rate to dd.d degrees per second
//              Returns: Nothing
      if (command[1]=='A') {
        f=strtod(parameter,&conv_end);
        double maxStepsPerSecond=1000000.0/(maxRate/16.0);
        if (&parameter[0]!=conv_end) {
          if (f<1.0/60.0/60.0) f=1.0/60.0/60.0;
          if (f>maxStepsPerSecond/StepsPerDegreeAxis1) f=maxStepsPerSecond/StepsPerDegreeAxis1;
          customGuideRateAxis1(f*240.0,GUIDE_TIME_LIMIT*1000);
        }
        quietReply=true; 
      } else
//   :REdd.d#   Set Axis2 Guide rate to dd.d degrees per second
//              Returns: Nothing
      if (command[1]=='E') {
        f=strtod(parameter,&conv_end);
        double maxStepsPerSecond=1000000.0/(maxRate/16.0);
        if (&parameter[0]!=conv_end) {
          if (f<1.0/60.0/60.0) f=1.0/60.0/60.0;
          if (f>maxStepsPerSecond/StepsPerDegreeAxis2) f=maxStepsPerSecond/StepsPerDegreeAxis2;
          customGuideRateAxis2(f*240.0,GUIDE_TIME_LIMIT*1000);
        }
        quietReply=true; 
      } else

//  :RG#   Set Slew rate to Guiding Rate (slowest) 1X
//  :RC#   Set Slew rate to Centering rate (2nd slowest) 8X
//  :RM#   Set Slew rate to Find Rate (2nd Fastest) 24X
//  :RS#   Set Slew rate to max (fastest) ?X (1/2 of MaxRate)
//  :Rn#   Set Slew rate to n, where n=0..9
//         Returns: Nothing
      if ((command[1]=='G') || (command[1]=='C') || (command[1]=='M') || (command[1]=='S') || ((command[1]>='0') && (command[1]<='9'))) {
        if (command[1]=='G') i=2; else // 1x
        if (command[1]=='C') i=5; else // 8x
        if (command[1]=='M') i=6; else // 24x
        if (command[1]=='S') i=8; else i=command[1]-'0';
        setGuideRate(i);
        quietReply=true; 
      } else commandError=true;
     } else

#ifdef ROTATOR_ON
//   r - Rotator/De-rotator Commands
      if (command[0]=='r') {
#ifdef MOUNT_TYPE_ALTAZM
//  :r+#   Enable derotator
//         Returns: Nothing
      if (command[1]=='+') { rot.enableDR(true); quietReply=true; } else
//  :r-#   Disable derotator
//         Returns: Nothing
      if (command[1]=='-') { rot.enableDR(false); quietReply=true; } else
//  :rP#   Move rotator to the parallactic angle
//         Returns: Nothing
      if (command[1]=='P') { double h,d; getApproxEqu(&h,&d,true); rot.setPA(h,d); quietReply=true; } else
//  :rR#   Reverse derotator direction
//         Returns: Nothing
      if (command[1]=='R') { rot.reverseDR(); quietReply=true; } else
#endif
//  :rF#   Reset rotator at the home position
//         Returns: Nothing
      if (command[1]=='F') { rot.reset(); quietReply=true; } else
//  :rC#   Moves rotator to the home position
//         Returns: Nothing
      if (command[1]=='C') { rot.home(); quietReply=true; } else
//  :rG#   Get rotator current position in degrees
//         Returns: sDDD*MM#
      if (command[1]=='G') {
        f1=rot.getPosition();
        i=highPrecision; highPrecision=false;
        if (!doubleToDms(reply,&f1,true,true)) commandError=true; else quietReply=true;
        highPrecision=i;
      } else
//  :r~#   Set continuous move mode
//         Return: 0 on failure
//                 1 on success
      if (command[1]=='~') { rot.moveContinuous(true); } else
//  :r>#   Move clockwise as set by :rn# command, default = 1 deg (or 0.1 deg/s in continuous mode)
//         Returns: Nothing
      if (command[1]=='>') { rot.startMoveCW(); quietReply=true; } else
//  :r<#   Move counter clockwise as set by :rn# command
//         Returns: Nothing
      if (command[1]=='<') { rot.startMoveCCW(); quietReply=true; } else
//  :rQ#   Stops movement (except derotator)
//         Returns: Nothing
      if (command[1]=='Q') { rot.stopMove(); quietReply=true; } else
//  :rn#   Move increment, 1=1 degrees, 2=2 degrees, 3=5 degrees, 4=10 degrees
//         Move rate     , 1=.01 deg/s, 2=0.1 deg/s, 3=1.0 deg/s, 4=5.0 deg/s
//         Returns: Nothing
      if ((command[1]>='1') && (command[1]<='4')) { i=command[1]-'1'; int t[]={1,2,5,10}; double t1[]={0.01,0.1,1.0,5.0}; rot.setIncrement(t[i]); rot.setMoveRate(t1[i]); quietReply=true; } else
//  :rSsDDD*MM'SS#  Set position
//         Return: 0 on failure
//                 1 on success
      if (command[1]=='S') {
        i=highPrecision; highPrecision=true;
        if (parameter[0]=='-') f=-1.0; else f=1.0;
        if ((parameter[0]=='+') || (parameter[0]=='-')) i1=1; else i1=0;
        if (!dmsToDouble(&f1,&parameter[i1],true)) commandError=true; else rot.setTarget(f*f1);
        highPrecision=i;
      } else commandError=true;
     } else
#endif

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
          if (process_command==COMMAND_SERIAL_A) {
            SerialA.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialA.transmit()); 
            #endif
            delay(50); SerialA.begin(baudRate[i]);
            quietReply=true; 
#ifdef HAL_SERIAL_B_ENABLED
          } else
          if (process_command==COMMAND_SERIAL_B) {
            SerialB.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialB.transmit()); 
            #endif
            delay(50); SerialB.begin(baudRate[i]); 
            quietReply=true;
#endif
#if defined(HAL_SERIAL_C_ENABLED) && !defined(HAL_SERIAL_C_BLUETOOTH)
          } else
          if (process_command==COMMAND_SERIAL_C) {
            SerialC.print("1"); 
            #ifdef HAL_SERIAL_TRANSMIT
            while (SerialC.transmit()); 
            #endif
            delay(50); SerialC.begin(baudRate[i]);
            quietReply=true; 
#endif
          } else commandError=true;
        } else commandError=true;
      } else
//  :SCMM/DD/YY#
//          Change Date to MM/DD/YY
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='C')  { if (dateToDouble(&JD,parameter)) { nv.writeFloat(EE_JD,JD); updateLST(jd2last(JD,UT1,true)); } else commandError=true; } else 
//  :SdsDD*MM#
//          Set target object declination to sDD*MM or sDD*MM:SS depending on the current precision setting, automatically detects low/high precision
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='d')  { 
        if (!dmsToDouble(&newTargetDec,parameter,true)) {
          i=highPrecision; highPrecision=!highPrecision;
          if (!dmsToDouble(&newTargetDec,parameter,true)) commandError=true; 
          highPrecision=i;
        }
      } else 
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
          if ((longitude>=-180.0) && (longitude<=360.0)) {
            if (longitude>=180.0) longitude-=360.0;
            nv.writeFloat(EE_sites+(currentSite)*25+4,longitude);
          } else commandError=true;
        }
        updateLST(jd2last(JD,UT1,false));
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
            nv.update(EE_sites+(currentSite)*25+8,b);
            updateLST(jd2last(JD,UT1,true));
          } else commandError=true;
        } else commandError=true; 
      } else
//  :Sh+DD#
//          Set the lowest elevation to which the telescope will goTo
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='h')  { if ((parameter[0]!=0) && (strlen(parameter)<4)) {
        if ( (atoi2(parameter,&i)) && ((i>=-30) && (i<=30))) { minAlt=i; nv.update(EE_minAlt,minAlt+128); } else commandError=true; 
      } else commandError=true; } else
//  :SLHH:MM:SS#
//          Set the local Time
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='L')  {  
        i=highPrecision; highPrecision=true; 
        if (!hmsToDouble(&LMT,parameter)) commandError=true; else {
#ifndef ESP32
          nv.writeFloat(EE_LMT,LMT);
#endif
          UT1=LMT+timeZone;
          updateLST(jd2last(JD,UT1,true));
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
        if (strlen(parameter)>15) commandError=true; else nv.writeString(EE_sites+i*25+9,parameter);
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
          nv.update(EE_maxAlt,maxAlt); 
        } else commandError=true; 
      } else commandError=true; } else
//  :SrHH:MM.T#
//  :SrHH:MM:SS#
//          Set target object RA to HH:MM.T or HH:MM:SS based on precision setting, automatically detects low/high precision
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='r')  {
        if (!hmsToDouble(&newTargetRA,parameter)) {
          i=highPrecision; highPrecision=!highPrecision;
          if (!hmsToDouble(&newTargetRA,parameter)) commandError=true; else newTargetRA*=15.0;
          highPrecision=i;
        } else newTargetRA*=15.0;
      } else 
//  :SSHH:MM:SS#
//          Sets the local (apparent) sideral time to HH:MM:SS
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='S')  { i=highPrecision; highPrecision=true; if (!hmsToDouble(&f,parameter)) commandError=true; else updateLST(f); highPrecision=i; } else 
//  :StsDD*MM#
//          Sets the current site latitude to sDD*MM#
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='t')  { 
        i=highPrecision; highPrecision=false; 
        if (!dmsToDouble(&latitude,parameter,true)) { commandError=true; } else {
          nv.writeFloat(100+(currentSite)*25+0,latitude);
#ifdef MOUNT_TYPE_ALTAZM
          celestialPoleAxis2=AltAzmDecStartPos;
          if (latitude<0) celestialPoleAxis1=180.0; else celestialPoleAxis1=0.0;
#else
          if (latitude<0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (latitude>0.0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
        }
        highPrecision=i;
      } else 
//  :STdd.ddddd#    Set Tracking Rate
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='T')  { 
        if ((trackingState==TrackingSidereal) || (trackingState==TrackingNone)) {
          f=strtod(parameter,&conv_end);
          if ( (&parameter[0]!=conv_end) && (((f>=30.0) && (f<90.0)) || (fabs(f)<0.1))) {
            if (fabs(f)<0.1) {
              trackingState=TrackingNone;
            } else {
              if (trackingState==TrackingNone) { trackingState=TrackingSidereal; enableStepperDrivers(); }
              setTrackingRate((f/60.0)/1.00273790935);
            }
          } else commandError=true;
        } else commandError=true;
      } else
//  :SXnn,VVVVVV...#   Set OnStep value
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='X')  {
        if (parameter[2]!=',') { parameter[0]=0; commandError=true; }                      // make sure command format is correct
        if (parameter[0]=='0') { // 0n: Align Model
          static int star;
          switch (parameter[1]) {
            case '0': Align.ax1Cor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // ax1Cor
            case '1': Align.ax2Cor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // ax2Cor 
            case '2': Align.altCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // altCor
            case '3': Align.azmCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;    // azmCor
            case '4': Align.doCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;     // doCor
            case '5': Align.pdCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;     // pdCor
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
            case '6': Align.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;     // ffCor
            case '7': break;                                                               // dfCor
#else
            case '6': break;                                                               // ffCor
            case '7': Align.dfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;     // dfCor
#endif
            case '8': Align.tfCor=(double)strtol(&parameter[3],NULL,10)/3600.0; break;     // tfCor
            case '9': { i=strtol(&parameter[3],NULL,10); if (i==1) { alignNumStars=star; alignThisStar=star+1; Align.model(star); } else star=0; } break; // use 0 to start upload of stars for align, use 1 to trigger align
            case 'A': { i=highPrecision; highPrecision=true; if (!hmsToDouble(&Align.actual[star].ha,&parameter[3])) commandError=true; else Align.actual[star].ha=(Align.actual[star].ha*15.0)/Rad; highPrecision=i; } break; // Star  #n HA
            case 'B': { i=highPrecision; highPrecision=true; if (!dmsToDouble(&Align.actual[star].dec,&parameter[3],true)) commandError=true; else Align.actual[star].dec=Align.actual[star].dec/Rad; highPrecision=i; } break; // Star  #n Dec
            case 'C': { i=highPrecision; highPrecision=true; if (!hmsToDouble(&Align.mount[star].ha,&parameter[3])) commandError=true; else Align.mount[star].ha=(Align.mount[star].ha*15.0)/Rad; highPrecision=i; } break; // Mount  #n HA
            case 'D': { i=highPrecision; highPrecision=true; if (!dmsToDouble(&Align.mount[star].dec,&parameter[3],true)) commandError=true; else Align.mount[star].dec=Align.mount[star].dec/Rad; highPrecision=i; } break; // Star  #n Dec
            case 'E': Align.actual[star].side=Align.mount[star].side=strtol(&parameter[3],NULL,10); star++; break; // Mount PierSide (and increment n)
            default:  commandError=true;
          }
        } else
        if (parameter[0]=='4') { // 4n: Encoder
          switch (parameter[1]) {
            static double encoderAxis1=0;
            static double encoderAxis2=0;
            case '0': // set encoder Axis1 value
              f=strtod(&parameter[3],&conv_end);
              if ( (&parameter[3]!=conv_end) && (f>=-999.9) && (f<=999.9)) encoderAxis1=f; else commandError=true;
              break;
            case '1': // set encoder Axis2 value
              f=strtod(&parameter[3],&conv_end);
              if ( (&parameter[3]!=conv_end) && (f>=-999.9) && (f<=999.9)) encoderAxis2=f; else commandError=true;
              break;
            case '2': // sync encoder to last values
              if ( (parameter[3]=='1') && (parameter[4]==0)) if (syncEnc(encoderAxis1,encoderAxis2)) commandError=true;
              break;
            default: commandError=true;
          }
        } else
        if (parameter[0]=='9') { // 9n: Misc.
          switch (parameter[1]) {
            case '2': // set new acceleration rate (returns 1 success or 0 failure)
              if (!isSlewing()) {
                maxRate=strtol(&parameter[3],NULL,10)*16L;
                if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
                if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
                nv.writeInt(EE_maxRate,(int)(maxRate/16L));
                setAccelerationRates(maxRate);
              } else commandError=true;
            break;
            case '3': // acceleration rate preset (returns nothing)
              quietReply=true;
              if (!isSlewing()) {
                switch (parameter[3]) {
                  case '5': maxRate=MaxRate*32L; break; // 50%
                  case '4': maxRate=MaxRate*24L; break; // 75%
                  case '3': maxRate=MaxRate*16L; break; // 100%
                  case '2': maxRate=MaxRate*12L; break; // 150%
                  case '1': maxRate=MaxRate*8L;  break; // 200%
                  default:  maxRate=MaxRate*16L;
                  nv.writeInt(EE_maxRate,(int)(maxRate/16L));
                  setAccelerationRates(maxRate);
                }
              }
            break;
#ifdef MOUNT_TYPE_GEM
            case '5': // autoMeridianFlip
              if ((parameter[3]=='0') || (parameter[3]=='1')) { i=parameter[3]-'0'; autoMeridianFlip=i; nv.write(EE_autoMeridianFlip,autoMeridianFlip); } else commandError=true;
            break;
#endif
            case '6': // preferred pier side 
              switch (parameter[3]) {
                case 'E': preferredPierSide=PPS_EAST; break;
                case 'W': preferredPierSide=PPS_WEST; break;
                case 'B': preferredPierSide=PPS_BEST; break;
                default:  commandError=true;
              }
            break;
            case '7': // buzzer
              if ((parameter[3]=='0') || (parameter[3]=='1')) { soundEnabled=parameter[3]-'0'; } else commandError=true;
            break;
            case '8': // pause at home on meridian flip
              if ((parameter[3]=='0') || (parameter[3]=='1')) { pauseHome=parameter[3]-'0'; nv.write(EE_pauseHome,pauseHome); } else commandError=true;
            break;
            case '9': // continue if paused at home
              if ((parameter[3]=='1')) { if (waitingHome) waitingHomeContinue=true; } commandError=true;
            break;
            case 'A': // temperature in deg. C
              f=strtod(parameter,&conv_end);
              if ( (&parameter[0]!=conv_end) && (f>=-100.0) && (f<100.0)) {
                ambient.setTemperature(f);
              } else commandError=true;
            break;
            case 'B': // pressure in mb
              f=strtod(parameter,&conv_end);
              if ( (&parameter[0]!=conv_end) && (f>=500.0) && (f<1500.0)) {
                ambient.setPressure(f);
              } else commandError=true;
            break;
            case 'C': // relative humidity in % 
              if ( (&parameter[0]!=conv_end) && (f>=0.0) && (f<100.0)) {
                ambient.setHumidity(f);
              } else commandError=true;
            break;
            case 'D': // altitude 
              if ( (&parameter[0]!=conv_end) && (f>=0.0) && (f<100.0)) {
                ambient.setAltitude(f);
              } else commandError=true;
            break;
            default: commandError=true;
          }
        } else
#ifdef MOUNT_TYPE_GEM
        if (parameter[0]=='E') { // En: Simple value
          switch (parameter[1]) {
            case '9': // minutesPastMeridianE 
              minutesPastMeridianE=(double)strtol(&parameter[3],NULL,10); 
              if (minutesPastMeridianE>180) minutesPastMeridianE=180; 
              if (minutesPastMeridianE<-180) minutesPastMeridianE=-180; 
              nv.write(EE_dpmE,round((minutesPastMeridianE*15.0)/60.0)+128); 
              break;
            case 'A': // minutesPastMeridianW
              minutesPastMeridianW=(double)strtol(&parameter[3],NULL,10); 
              if (minutesPastMeridianW>180) minutesPastMeridianW=180; 
              if (minutesPastMeridianW<-180) minutesPastMeridianW=-180; 
              nv.write(EE_dpmW,round((minutesPastMeridianW*15.0)/60.0)+128); 
              break;
            default: commandError=true;
          }
        } else
#endif
        if (parameter[0]=='G') { // Gn: General purpose output
          long v=(double)strtol(&parameter[3],NULL,10);
          if ((v>=0) && (v<=255)) {
#ifdef Aux0
            if (parameter[1]=='0') { valueAux0=v; byte p=Aux0; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifndef MODE_SWITCH_BEFORE_SLEW_SPI
#ifdef Aux1
            if (parameter[1]=='1') { valueAux1=v; byte p=Aux1; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux2
            if (parameter[1]=='2') { valueAux2=v; byte p=Aux2; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#endif
#ifdef Aux3
            if (parameter[1]=='3') { valueAux3=v; byte p=Aux3; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux3_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux4
            if (parameter[1]=='4') { valueAux4=v; byte p=Aux4; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux4_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux5
            if (parameter[1]=='5') { valueAux5=v; byte p=Aux5; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux5_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux6
            if (parameter[1]=='6') { valueAux6=v; byte p=Aux6; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux6_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux7
            if (parameter[1]=='7') { valueAux7=v; byte p=Aux7; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux7_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux8
            if (parameter[1]=='8') { valueAux8=v; byte p=Aux8; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; }
  #ifdef Aux8_Analog
              analogWrite(p,v); } else
  #else
              if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
  #endif
#endif
#ifdef Aux9
            if (parameter[1]=='9') { valueAux9=v; byte p=Aux9; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux10
            if (parameter[1]=='A') { valueAux10=v; byte p=Aux10; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux11
            if (parameter[1]=='B') { valueAux11=v; byte p=Aux11; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux12
            if (parameter[1]=='C') { valueAux12=v; byte p=Aux12; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux13
            if (parameter[1]=='D') { valueAux13=v; byte p=Aux13; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux14
            if (parameter[1]=='E') { valueAux14=v; byte p=Aux14; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
#ifdef Aux15
            if (parameter[1]=='F') { valueAux15=v; byte p=Aux15; static bool init=false; if (!init) { pinMode(p,OUTPUT); init=true; } if (v==0) digitalWrite(p,LOW); else digitalWrite(p,HIGH); } else
#endif
            commandError=true;
          } else commandError=true;
        } else commandError=true;
      } else
//  :SzDDD*MM#
//          Sets the target Object Azimuth
//          Return: 0 on failure
//                  1 on success
      if (command[1]=='z')  { if (!dmsToDouble(&newTargetAzm,parameter,false)) commandError=true; } else commandError=true;
      } else 
//   T - Tracking Commands
//
//  :T+#   Master sidereal clock faster by 0.02 Hertz (stored in EEPROM)
//  :T-#   Master sidereal clock slower by 0.02 Hertz (stored in EEPROM)
//  :TS#   Track rate solar
//  :TL#   Track rate lunar
//  :TQ#   Track rate sidereal
//  :TR#   Master sidereal clock reset (to calculated sidereal rate, stored in EEPROM)
//  :TK#   Track rate king
//         Returns: Nothing
//
//  :Te#   Tracking enable
//  :Td#   Tracking disable
//  :To#   OnTrack enable
//  :Tr#   Track refraction enable
//  :Tn#   Track refraction disable
//  :T1#   Track single axis (disable Dec tracking on Eq mounts)
//  :T2#   Track dual axis
//         Return: 0 on failure
//                 1 on success

      if ((command[0]=='T') && (parameter[0]==0)) {
#ifndef MOUNT_TYPE_ALTAZM
        static bool dualAxis=false;
        if (command[1]=='o') { rateCompensation=RC_FULL_RA; setTrackingRate(default_tracking_rate); } else                  // turn full compensation on, defaults to base sidereal tracking rate
        if (command[1]=='r') { rateCompensation=RC_REFR_RA; setTrackingRate(default_tracking_rate); } else                  // turn refraction compensation on, defaults to base sidereal tracking rate
        if (command[1]=='n') { rateCompensation=RC_NONE; setTrackingRate(default_tracking_rate); } else                     // turn refraction off, sidereal tracking rate resumes
        if (command[1]=='1') { dualAxis=false; } else                                                                       // turn off dual axis tracking
        if (command[1]=='2') { dualAxis=true;  } else                                                                       // turn on dual axis tracking
#endif
        if (command[1]=='+') { siderealInterval-=HzCf*(0.02); quietReply=true; } else
        if (command[1]=='-') { siderealInterval+=HzCf*(0.02); quietReply=true; } else
        if (command[1]=='S') { setTrackingRate(0.99726956632); rateCompensation=RC_NONE; quietReply=true; } else            // solar tracking rate 60Hz
        if (command[1]=='L') { setTrackingRate(0.96236513150); rateCompensation=RC_NONE; quietReply=true; } else            // lunar tracking rate 57.9Hz
        if (command[1]=='Q') { setTrackingRate(default_tracking_rate); quietReply=true; } else                              // sidereal tracking rate
        if (command[1]=='R') { siderealInterval=15956313L; quietReply=true; } else                                          // reset master sidereal clock interval
        if (command[1]=='K') { setTrackingRate(0.99953004401); rateCompensation=RC_NONE; quietReply=true; } else            // king tracking rate 60.136Hz
        if ((command[1]=='e') && !isSlewing() && !isHoming() && !isParked() ) { trackingState=TrackingSidereal; enableStepperDrivers(); } else
        if ((command[1]=='d') && !isSlewing() && !isHoming() ) trackingState=TrackingNone; else
          commandError=true;

#ifndef MOUNT_TYPE_ALTAZM
       if (dualAxis && (rateCompensation==RC_REFR_RA)) rateCompensation=RC_REFR_BOTH;
       if (!dualAxis && (rateCompensation==RC_REFR_BOTH)) rateCompensation=RC_REFR_RA;
       if (dualAxis && (rateCompensation==RC_FULL_RA)) rateCompensation=RC_FULL_BOTH;
       if (!dualAxis && (rateCompensation==RC_FULL_BOTH)) rateCompensation=RC_FULL_RA;
#endif

        // Only burn the new rate if changing the sidereal interval
        if ((!commandError) && ((command[1]=='+') || (command[1]=='-') || (command[1]=='R'))) {
          nv.writeLong(EE_siderealInterval,siderealInterval);
          SiderealClockSetInterval(siderealInterval);
          cli(); SiderealRate=siderealInterval/StepsPerSecondAxis1; sei();
        }

        setDeltaTrackingRate();

      } else
      if (command[0]=='T') {
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
#endif
//   V - PEC Readout StepsPerSecondAxis1
//  :VS#
//         Returns: DDD.DDDDDD#
      if ((command[0]=='V') && (command[1]=='S')) {
        if (parameter[0]==0) {
          char temp[12];
          dtostrf(StepsPerSecondAxis1,0,6,temp);
          strcpy(reply,temp);
          quietReply=true;
        } else commandError=true;
      } else
#ifndef MOUNT_TYPE_ALTAZM
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
          currentSite=command[1]-'0'; nv.update(EE_currentSite,currentSite); quietReply=true;
          latitude=nv.readFloat(EE_sites+(currentSite*25+0));
#ifdef MOUNT_TYPE_ALTAZM
          celestialPoleAxis2=AltAzmDecStartPos;
#else
          if (latitude<0.0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
          cosLat=cos(latitude/Rad);
          sinLat=sin(latitude/Rad);
          if (latitude>0.0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
          longitude=nv.readFloat(EE_sites+(currentSite*25+4));
          timeZone=nv.read(EE_sites+(currentSite)*25+8)-128;
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

      if ((strlen(reply)>0) || (cmdA.checksum)) {
        if (process_command==COMMAND_SERIAL_A) {
          if (cmdA.checksum)  { checksum(reply); strcat(reply,cmdA.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialA.print(reply);
        }
      }

#ifdef HAL_SERIAL_B_ENABLED
      if ((strlen(reply)>0) || (cmdB.checksum)) {
        if (process_command==COMMAND_SERIAL_B) {
          if (cmdB.checksum)  { checksum(reply); strcat(reply,cmdB.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");

          /*
          // simulate data corruption
          static int se=0;
          se++;
          if (se==22) { se=0; reply[2]='x'; }
          */

          SerialB.print(reply);
        }
      }
#endif

#ifdef HAL_SERIAL_C_ENABLED
      if ((strlen(reply)>0) || (cmdC.checksum)) {
        if (process_command==COMMAND_SERIAL_C) {
          if (cmdC.checksum)  { checksum(reply); strcat(reply,cmdC.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialC.print(reply);
        }
      }
#endif

#ifdef ST4_HAND_CONTROL_ON
      if ((strlen(reply)>0) || (cmdST4.checksum)) {
        if (process_command==COMMAND_SERIAL_ST4) {
          if (cmdST4.checksum)  { checksum(reply); strcat(reply,cmdST4.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          SerialST4.print(reply);
        }
      }
#endif

      if ((strlen(reply)>0) || (cmdX.checksum)) {
        if (process_command==COMMAND_SERIAL_X) {
          if (cmdX.checksum)  { checksum(reply); strcat(reply,cmdX.getSeq()); supress_frame=false; }
          if (!supress_frame) strcat(reply,"#");
          strcpy(_replyX,reply);
        }
      }
      quietReply=false;
   }
}

void stopMount() {
  if ((parkStatus==NotParked) || (parkStatus==Parking)) {
    stopGuideAxis1();
    stopGuideAxis2();
    if (trackingState==TrackingMoveTo) if (!abortSlew) abortSlew=StartAbortSlew;
  }
}

// calculate the checksum and add to string
void checksum(char s[]) {
  char HEXS[3]="";
  byte cks=0; for (unsigned int cksCount0=0; cksCount0<strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
  sprintf(HEXS,"%02X",cks);
  strcat(s,HEXS);
}

void forceRefreshGetEqu() {
  _coord_t=millis()-100;
}

// local command processing
bool _ignoreReply=false;
// true if command isn't complete
bool cmdWaiting() {
  if (cmdX.ready()) return true;
  if ((_replyX[0]!=0) && !_ignoreReply) return true;
  return false;
}
// set command to be processed and if reply should be be ignored
void cmdSend(const char *s, bool ignoreReply=false) {
  _ignoreReply=ignoreReply;
  _replyX[0]=0;
  cmdX.flush();
  int l=0;
  while (s[l]!=0) { cmdX.add(s[l]); l++; }
}
// get the last command reply
bool cmdReply(char *s) {
  if (_replyX[0]==0) return false;
  strcpy(s,_replyX); _replyX[0]=0;
  return true;
}
