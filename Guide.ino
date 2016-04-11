// Guide ---------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;
    if (guideDirAxis1) {
      if (!inbacklashAxis1) {
        // guideHA keeps track of how many steps we've moved for PEC recording
        if (guideDirAxis1=='e') guideHA.fixed=-amountGuideHA.fixed; else if (guideDirAxis1=='w') guideHA.fixed=amountGuideHA.fixed;
        cli();
        targetAxis1.fixed+=guideHA.fixed;
        sei();

        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationHA>0)  {
          guideDurationHA-=(long)(micros()-guideDurationLastHA);
          guideDurationLastHA=micros();
          if (guideDurationHA<=0) { guideDirAxis1='b'; } // break
        }
      } else {
        // don't count time if in backlash
        guideDurationLastHA=micros();
      }
    }
    
    if (guideDirAxis2) {
      if (!inbacklashAxis2) {
        cli();
        // nudge the targetAxis2 (where we're supposed to be) by amountMoveDec
        if (guideDirAxis2=='s') targetAxis2.fixed-=amountGuideDec.fixed; else if (guideDirAxis2=='n') targetAxis2.fixed+=amountGuideDec.fixed; 
        sei();
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationDec>0)  {
          guideDurationDec-=(long)(micros()-guideDurationLastDec);
          guideDurationLastDec=micros();
          if (guideDurationDec<=0) { guideDirAxis2='b'; }  // break 
        }
      } else {
        // don't count time if in backlash
        guideDurationLastDec=micros();
      }
    }

  }
  // allow the elevated rate to persist for a moment to allow the bulk added steps to play out after stopping
  // if the guide rate is negative and slow it's always faster to go back to the sidereal rate
  if ((currentGuideRate<=GuideRate1x) && (fabs(guidetimerRateAxis1)<0.001)) { cli(); guidetimerRateAxis1=0.0; sei(); }
}
