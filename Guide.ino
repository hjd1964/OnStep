// Guide ---------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;  
    if (guideDirHA) {
      if (!inBacklashHA) {
        long v=fixedToLong(fTargetHA);
        if (guideDirHA=='e') fTargetHA-=amountGuideHA; else fTargetHA+=amountGuideHA;
        // guideHA keeps track of how many steps we've moved for PEC recording
        guideHA=fixedToLong(fTargetHA)-v;
        cli(); targetHA=fixedToLong(fTargetHA); sei();

        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationHA>0)  {
          guideDurationHA-=(long)(micros()-guideDurationLastHA);
          guideDurationLastHA=micros();
          if (guideDurationHA<=0) { lstGuideStopHA=lst+3; guideDirHA=0; } 
        }
      } else {
        // don't count time if in backlash
        guideDurationLastHA=micros();
      }
    }
    
    if (guideDirDec) {
      if (!inBacklashDec) {
        // nudge the targetDec (where we're supposed to be) by amountMoveDec
        if (guideDirDec=='s') fTargetDec-=amountGuideDec; else fTargetDec+=amountGuideDec; 
        cli(); targetDec=fixedToLong(fTargetDec); sei();
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationDec>0)  {
          guideDurationDec-=(long)(micros()-guideDurationLastDec);
          guideDurationLastDec=micros();
          if (guideDurationDec<=0) { lstGuideStopDec=lst+3; guideDirDec=0;  } 
        }
      } else {
        // don't count time if in backlash
        guideDurationLastDec=micros();
      }
    }

  }
  // allow the elevated rate to persist for a moment to allow the bulk added steps to play out after stopping
  // if the guide rate is negative and slow it's always faster to go back to the sidereal rate
  if ((currentGuideRate<=GuideRate1x) && (guideTimerRateHA<0.001)) { cli(); guideTimerRateHA=0.0; sei(); }
  if ((!guideDirHA) && (fabs(guideTimerRateHA)>0.001) && (lst>=lstGuideStopHA)) { cli(); guideTimerRateHA=0.0; sei(); }
  if ((!guideDirDec) && (fabs(guideTimerRateDec)>0.001) && (lst>=lstGuideStopDec)) { cli(); guideTimerRateDec=0.0; sei(); }
}
