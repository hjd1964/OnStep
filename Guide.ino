// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

char          ST4DirAxis1             = 'b';
long          guideDurationAxis1      = -1;
unsigned long guideDurationLastAxis1  = -1;
char          ST4DirAxis2             = 'b';
long          guideDurationAxis2      = -1;
unsigned long guideDurationLastAxis2  = -1;

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;  
    if (guideDirAxis1) {
      if (!inbacklashAxis1) {
        // guideAxis1 keeps track of how many steps we've moved for PEC recording
        if (guideDirAxis1=='e') guideAxis1.fixed=-amountGuideAxis1.fixed; else if (guideDirAxis1=='w') guideAxis1.fixed=amountGuideAxis1.fixed;

        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationAxis1>0)  {
          guideDurationAxis1-=(long)(micros()-guideDurationLastAxis1);
          guideDurationLastAxis1=micros();
          if (guideDurationAxis1<=0) { guideDirAxis1='b'; } // break
        }
      } else {
        // don't count time if in backlash
        guideDurationLastAxis1=micros();
      }
    }
    
    if (guideDirAxis2) {
      if (!inbacklashAxis2) {
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationAxis2>0)  {
          guideDurationAxis2-=(long)(micros()-guideDurationLastAxis2);
          guideDurationLastAxis2=micros();
          if (guideDurationAxis2<=0) { guideDirAxis2='b'; }  // break 
        }
      } else {
        // don't count time if in backlash
        guideDurationLastAxis2=micros();
      }
    }
  }
}

// initialize guiding
void initGuide() {
  guideDirAxis1          = 0;
  guideDurationAxis1     = -1;
  guideDurationLastAxis1 = -1;
  guideDirAxis2          = 0;
  guideDurationAxis2     = -1;
  guideDurationLastAxis2 = -1;
}
  
// start a guide in RA or Azm, direction must be 'e', 'w', or 'b', guide rate in x the sidereal rate (=1,) guideDuration is in ms (-1 to ignore) 
bool startGuideAxis1(char direction, double guideRate, long guideDuration) {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo) && (direction!=guideDirAxis1)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis1) && (fabs(guideTimerRateAxis1)>2)) { 
      stopGuideAxis1();
    } else {
      enableGuideRate(guideRate);
      guideDirAxis1=direction;
      guideDurationLastAxis1=micros();
      guideDurationAxis1=guideDuration;
      cli();
      guideStartTimeAxis1=millis();
      if (guideDirAxis1=='e') guideTimerRateAxis1=-guideTimerBaseRate; else guideTimerRateAxis1=guideTimerBaseRate; 
      sei();
    }
  } else return false;
  return true;
}

// stops guide in RA or Azm
void stopGuideAxis1() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis1) && (guideDirAxis1!='b')) { guideDirAxis1='b'; guideBreakTimeAxis1=millis(); } sei();
  }
}
  
// start a guide in Dec or Alt, direction must be 'n', 's', or 'b', guide rate in x the sidereal rate (=1,) guideDuration is in ms (-1 to ignore) 
bool startGuideAxis2(char c, double guideRate, long guideDuration) {
  if (((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) && (c!=guideDirAxis2)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis2) && (fabs(guideTimerRateAxis2)>2)) { 
      stopGuideAxis2();
    } else {
      enableGuideRate(guideRate);
      guideDirAxis2=c;
      guideDurationLastAxis2=micros();
      guideDurationAxis2=guideDuration;
      cli();
      guideStartTimeAxis2=millis();
      if (guideDirAxis2=='s') guideTimerRateAxis2=-guideTimerBaseRate; else guideTimerRateAxis2=guideTimerBaseRate; 
      sei();
    }
  } else return false;
  return true;
}

// stops guide in Dec or Alt
void stopGuideAxis2() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis2) && (guideDirAxis2!='b')) { guideDirAxis2='b'; guideBreakTimeAxis2=millis(); } sei();
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

