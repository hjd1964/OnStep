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
