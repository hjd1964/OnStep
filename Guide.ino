// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

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

bool startGuideAxis1(char c, double guideRate, long guideDuration) {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo) && (c!=guideDirAxis1)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis1) && (fabs(guideTimerRateAxis1)>2)) { 
      if (guideDirAxis1!='b') { guideDirAxis1='b'; guideBreakTimeAxis1=millis(); } // break
    } else {
      enableGuideRate(guideRate);
      guideDirAxis1=c;
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

void stopGuideAxis1() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis1) && (guideDirAxis1!='b')) { guideDirAxis1='b'; guideBreakTimeAxis1=millis(); } sei(); // break
  }
}
  
bool startGuideAxis2(char c, double guideRate, long guideDuration) {
  if (((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) && (c!=guideDirAxis2)) {
    // block user from changing direction at high rates, just stop the guide instead
    if ((guideDirAxis2) && (fabs(guideTimerRateAxis2)>2)) { 
      if (guideDirAxis2!='b') { guideDirAxis2='b'; guideBreakTimeAxis2=millis(); } // break
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

void stopGuideAxis2() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    cli(); if ((guideDirAxis2) && (guideDirAxis2!='b')) { guideDirAxis2='b'; guideBreakTimeAxis2=millis(); } sei(); // break
  }
}

