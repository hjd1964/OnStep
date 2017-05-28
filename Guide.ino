// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

long          guideTimeRemainingAxis1    = -1;
unsigned long guideTimeThisIntervalAxis1 = -1;
long          guideTimeRemainingAxis2    = -1;
unsigned long guideTimeThisIntervalAxis2 = -1;

// initialize guiding
void initGuide() {
  guideDirAxis1          = 0;
  guideTimeRemainingAxis1     = -1;
  guideTimeThisIntervalAxis1 = -1;
  guideDirAxis2          = 0;
  guideTimeRemainingAxis2     = -1;
  guideTimeThisIntervalAxis2 = -1;
}

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

// start a guide in RA or Azm, direction must be 'e', 'w', or 'b', guideRate is the rate selection (0 to 9), guideDuration is in ms (-1 to ignore) 
bool startGuideAxis1(char direction, int guideRate, long guideDuration) {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo) && (direction!=guideDirAxis1)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis1) && (fabs(guideTimerRateAxis1)>2)) { 
      stopGuideAxis1();
    } else {
      enableGuideRate(guideRate);
      guideDirAxis1=direction;
      guideTimeThisIntervalAxis1=micros();
      guideTimeRemainingAxis1=guideDuration*1000L;
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
  
// start a guide in Dec or Alt, direction must be 'n', 's', or 'b', guideRate is the rate selection (0 to 9), guideDuration is in ms (-1 to ignore) 
bool startGuideAxis2(char c, int guideRate, long guideDuration) {
  if (((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) && (c!=guideDirAxis2)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis2) && (fabs(guideTimerRateAxis2)>2)) { 
      stopGuideAxis2();
    } else {
      enableGuideRate(guideRate);
      guideDirAxis2=c;
      guideTimeThisIntervalAxis2=micros();
      guideTimeRemainingAxis2=guideDuration*1000L;
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

