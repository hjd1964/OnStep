// Guide ---------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;  
    if (guideDirHA) {
      if (!inBacklashHA) {
        cli();
        long v=(long)targetHA.part.m;
        if (guideDirHA=='e') targetHA.fixed-=amountGuideHA.fixed; else if (guideDirHA=='w') targetHA.fixed+=amountGuideHA.fixed;
        // guideHA keeps track of how many steps we've moved for PEC recording
        guideHA=(long)targetHA.part.m-v;
        sei();

        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationHA>0)  {
          guideDurationHA-=(long)(micros()-guideDurationLastHA);
          guideDurationLastHA=micros();
          if (guideDurationHA<=0) { guideDirHA='b'; } // break
        }
      } else {
        // don't count time if in backlash
        guideDurationLastHA=micros();
      }
    }
    
    if (guideDirDec) {
      if (!inBacklashDec) {
        cli();
        // nudge the targetDec (where we're supposed to be) by amountMoveDec
        if (guideDirDec=='s') targetDec.fixed-=amountGuideDec.fixed; else if (guideDirDec=='n') targetDec.fixed+=amountGuideDec.fixed; 
        sei();
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationDec>0)  {
          guideDurationDec-=(long)(micros()-guideDurationLastDec);
          guideDurationLastDec=micros();
          if (guideDurationDec<=0) { guideDirDec='b'; }  // break 
        }
      } else {
        // don't count time if in backlash
        guideDurationLastDec=micros();
      }
    }

  }
  // allow the elevated rate to persist for a moment to allow the bulk added steps to play out after stopping
  // if the guide rate is negative and slow it's always faster to go back to the sidereal rate
  if ((currentGuideRate<=GuideRate1x) && (fabs(guideTimerRateHA)<0.001)) { cli(); guideTimerRateHA=0.0; sei(); }
}
