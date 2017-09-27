// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

long          guideTimeRemainingAxis1    = -1;
unsigned long guideTimeThisIntervalAxis1 = -1;
long          guideTimeRemainingAxis2    = -1;
unsigned long guideTimeThisIntervalAxis2 = -1;

// initialize guiding
void initGuide() {
  guideDirAxis1              =  0;
  guideTimeRemainingAxis1    = -1;
  guideTimeThisIntervalAxis1 = -1;
  guideDirAxis2              =  0;
  guideTimeRemainingAxis2    = -1;
  guideTimeThisIntervalAxis2 = -1;
}

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  guideAxis1.fixed=0;
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;  
    if (guideDirAxis1) {
      if (!inbacklashAxis1) {
        // guideAxis1 keeps track of how many steps we've moved for PEC recording
        if (guideDirAxis1=='e') guideAxis1.fixed=-amountGuideAxis1.fixed; else if (guideDirAxis1=='w') guideAxis1.fixed=amountGuideAxis1.fixed;

        // for pulse guiding, count down the mS and stop when timed out
        if (guideTimeRemainingAxis1>0) {
          guideTimeRemainingAxis1-=(long)(micros()-guideTimeThisIntervalAxis1);
          guideTimeThisIntervalAxis1=micros();
          if (guideTimeRemainingAxis1<=0) { guideDirAxis1='b'; } // break
        }
      } else {
        // don't count time if in backlash
        guideTimeThisIntervalAxis1=micros();
      }
    }
    
    if (guideDirAxis2) {
      if (!inbacklashAxis2) {
        // for pulse guiding, count down the mS and stop when timed out
        if (guideTimeRemainingAxis2>0) {
          guideTimeRemainingAxis2-=(long)(micros()-guideTimeThisIntervalAxis2);
          guideTimeThisIntervalAxis2=micros();
          if (guideTimeRemainingAxis2<=0) { guideDirAxis2='b'; }  // break 
        }
      } else {
        // don't count time if in backlash
        guideTimeThisIntervalAxis2=micros();
      }
    }
  }
}

// start a guide in RA or Azm, direction must be 'e', 'w', or 'b', guideRate is the rate selection (0 to 9), guideDuration is in ms (0 to ignore) 
bool startGuideAxis1(char direction, int guideRate, long guideDuration) {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo) && (direction!=guideDirAxis1) && (axis1Enabled)) {
    enableGuideRate(guideRate);
    guideDirAxis1=direction;
    guideTimeThisIntervalAxis1=micros();
    guideTimeRemainingAxis1=guideDuration*1000L;
    cli();
    if (guideDirAxis1=='e') guideTimerRateAxis1=-guideTimerBaseRate; else guideTimerRateAxis1=guideTimerBaseRate; 
    sei();
  } else return false;
  return true;
}

// stops guide in RA or Azm
void stopGuideAxis1() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis1) && (guideDirAxis1!='b')) { guideDirAxis1='b'; } sei();
  }
}
  
// start a guide in Dec or Alt, direction must be 'n', 's', or 'b', guideRate is the rate selection (0 to 9), guideDuration is in ms (0 to ignore) 
bool startGuideAxis2(char c, int guideRate, long guideDuration) {
  if (((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) && (c!=guideDirAxis2) && (axis1Enabled)) {
    enableGuideRate(guideRate);
    guideDirAxis2=c;
    guideTimeThisIntervalAxis2=micros();
    guideTimeRemainingAxis2=guideDuration*1000L;
    cli();
    if (guideDirAxis2=='s') guideTimerRateAxis2=-guideTimerBaseRate; else guideTimerRateAxis2=guideTimerBaseRate; 
    sei();
  } else return false;
  return true;
}

// stops guide in Dec or Alt
void stopGuideAxis2() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis2) && (guideDirAxis2!='b')) { guideDirAxis2='b'; } sei();
  }
}

// sets the rates for guide commands
void setGuideRate(int g) {
  currentGuideRate=g;
  if ((g<=GuideRate1x) && (currentPulseGuideRate!=g)) { currentPulseGuideRate=g; EEPROM.update(EE_pulseGuideRate,g); }
}

// enables the guide rate
void enableGuideRate(int g) {
  // don't do these calculations unless we have to
  if (activeGuideRate==g) return;
  
  activeGuideRate=g;

  // this enables the guide rate
  guideTimerBaseRate=(double)(guideRates[g]/15.0);

  cli();
  amountGuideAxis1.fixed=doubleToFixed((guideTimerBaseRate*StepsPerSecondAxis1)/100.0);
  amountGuideAxis2.fixed=doubleToFixed((guideTimerBaseRate*StepsPerSecondAxis2)/100.0);
  sei();
}

// handle the ST4 interface and hand controller features

#ifdef ST4_HAND_CONTROL_ON
const long Debounce_ms=100;
const long AltMode_ms=2000;
const long Shed_ms=4000;
#endif

void ST4() {
#if defined(ST4_ON) || defined(ST4_PULLUP)
    // ST4 INTERFACE
    byte w1=digitalRead(ST4RAw); byte e1=digitalRead(ST4RAe); byte n1=digitalRead(ST4DEn); byte s1=digitalRead(ST4DEs);
    delayMicroseconds(50);
    byte w2=digitalRead(ST4RAw); byte e2=digitalRead(ST4RAe); byte n2=digitalRead(ST4DEn); byte s2=digitalRead(ST4DEs);

    // if signals aren't stable ignore them
    if ((w1==w2) && (e1==e2) && (n1==n2) && (s1==s2)) {
      static char c1=0;
      static unsigned long c1Time=0;
      if ((w1==HIGH) && (e1==HIGH)) if (c1!='b') { c1='b'; c1Time=millis(); }
      if ((w1==LOW)  && (e1==HIGH)) if (c1!='w') { c1='w'; c1Time=millis(); }
      if ((w1==HIGH) && (e1==LOW))  if (c1!='e') { c1='e'; c1Time=millis(); }
      if ((w1==LOW)  && (e1==LOW))  if (c1!='+') { c1='+'; c1Time=millis(); }
      static char c2=0;
      static unsigned long c2Time=0;
      if ((n1==HIGH) && (s1==HIGH)) if (c2!='b') { c2='b'; c2Time=millis(); }
      if ((n1==LOW)  && (s1==HIGH)) if (c2!='n') { c2='n'; c2Time=millis(); }
      if ((n1==HIGH) && (s1==LOW))  if (c2!='s') { c2='s'; c2Time=millis(); }
      if ((n1==LOW)  && (s1==LOW))  if (c2!='+') { c2='+'; c2Time=millis(); }

#ifdef ST4_HAND_CONTROL_ON
      static char keys_up=true;
      if ((c1=='b') && (c2=='b') && ((long)(millis()-c1Time)>Debounce_ms) && ((long)(millis()-c2Time)>Debounce_ms)) keys_up=true;
      static bool altModeA=false;
      static bool altModeB=false;
      static unsigned long altModeShed=0;
      if (c1=='+') stopGuideAxis1(); if (c2=='+') stopGuideAxis2(); // stop any guide that might have been triggered
      if ((c1=='+') && ((long)(millis()-c1Time)>AltMode_ms) && (!altModeB)) { if (!altModeA) { altModeA=true; soundBeep(); } altModeShed=millis(); }
      if ((c2=='+') && ((long)(millis()-c2Time)>AltMode_ms) && (!altModeA)) { if (!altModeB) { altModeB=true; soundBeep(); } altModeShed=millis(); }
      if ((altModeA || altModeB) && ((long)(millis()-altModeShed)<Shed_ms)) {
        if (keys_up) {
          if (altModeA) {
            int c=currentGuideRate;
            if ((c1=='w') && (c1Time>Debounce_ms)) {
              if (trackingState==TrackingNone) localCommand(":B+#"); else {
                if (c>=7) c=8; else if (c>=5) c=7; else if (c>=2) c=5; else if (c<2) c=2; keys_up=false; altModeShed=millis();
              }
            }
            if ((c1=='e') && (c1Time>Debounce_ms)) { 
              if (trackingState==TrackingNone) localCommand(":B-#"); else {
                if (c<=5) c=2; else if (c<=7) c=5; else if (c<=8) c=7; else if (c>8) c=8; keys_up=false; altModeShed=millis();
              }
            }
            if ((c2=='s') && (c2Time>Debounce_ms)) { if (alignThisStar>alignNumStars) localCommand(":CS#"); else localCommand(":A+#"); keys_up=false; altModeShed=millis(); }
            if ((c2=='n') && (c2Time>Debounce_ms)) {
              if ((pauseHome) && (waitingHome)) waitingHomeContinue=true; else
                if (trackingState==TrackingSidereal) { trackingState=TrackingNone; DisableStepperDrivers(); } else
                  if (trackingState==TrackingNone) { trackingState=TrackingSidereal; EnableStepperDrivers(); }
              keys_up=false; altModeShed=millis();
            }
            if (c!=currentGuideRate) { setGuideRate(c); enableGuideRate(c); keys_up=false; altModeShed=millis(); }
          }
          if (altModeB) {
            keys_up=false; altModeShed=millis();
            if ((c1=='w') && (c1Time>Debounce_ms)) { localCommand(":LN#"); keys_up=false; altModeShed=millis(); }
            if ((c1=='e') && (c1Time>Debounce_ms)) { localCommand(":LB#"); keys_up=false; altModeShed=millis(); }
            if ((c2=='s') && (c2Time>Debounce_ms)) { soundEnabled=!soundEnabled; keys_up=false; altModeShed=millis(); }
            if ((c2=='n') && (c2Time>Debounce_ms)) { localCommand(":LI#"); localCommand(":MS#"); keys_up=false; altModeShed=millis(); }
          }
          soundClick();
        }
      } else {
        if ((altModeA || altModeB)) { altModeA=false; altModeB=false; soundBeep(); }
#endif
        if ((axis1Enabled) && (!waitingHome)) {
          // guide E/W
          if (c1!=ST4DirAxis1) {
            ST4DirAxis1=c1;
#ifdef ST4_HAND_CONTROL_ON
            if (((c1=='e') || (c1=='w')) && (c1Time>Debounce_ms)) {
#else
            if ((c1=='e') || (c1=='w')) {
#endif
    #if defined(SEPARATE_PULSE_GUIDE_RATE_ON) && !defined(ST4_HAND_CONTROL_ON)
              if (trackingState==TrackingSidereal) startGuideAxis1(c1,currentPulseGuideRate,GUIDE_TIME_LIMIT*1000);
    #else
              startGuideAxis1(c1,currentGuideRate,GUIDE_TIME_LIMIT*1000);
    #endif
            }
            if (c1=='b') stopGuideAxis1();
          }

          // guide N/S
          if (c2!=ST4DirAxis2) {
            PSerial.putch(c2); PSerial.puts("\r\n");
            ST4DirAxis2=c2;
#ifdef ST4_HAND_CONTROL_ON
            if (((c2=='n') || (c2=='s')) && (c2Time>Debounce_ms)) {
#else
            if ((c2=='n') || (c2=='s')) {
#endif
    #if defined(SEPARATE_PULSE_GUIDE_RATE_ON) && !defined(ST4_HAND_CONTROL_ON)
              if (trackingState==TrackingSidereal) startGuideAxis2(c2,currentPulseGuideRate,GUIDE_TIME_LIMIT*1000);
    #else
              startGuideAxis2(c2,currentGuideRate,GUIDE_TIME_LIMIT*1000);
    #endif
            }
            if (c2=='b') stopGuideAxis2();
          }
        }
#ifdef ST4_HAND_CONTROL_ON
      }
#endif
    } else {
      // continue if paused at home
      if ((w1==LOW) || (e1==LOW) || (n1==LOW) || (s1==LOW)) waitingHomeContinue=true;
    }
#endif
}

