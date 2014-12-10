// Guide ---------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

void Guide() {
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  cli(); long guideLst=lst; sei();
  if (guideLst!=guideSiderealTimer) {
    guideSiderealTimer=guideLst;  
    int sign=0;
    if (guideDirHA) {
      if (((guideLst%gr_st==0) && ((guideLst%gr_sk!=0) || ((guideLst%gr_st1==0) && (guideLst%gr_sk1!=0) )))) {
        // as above, and keep track of how much we've moved for PEC recording
        if (guideDirHA=='e') sign=-1; else sign=1; guideHA=sign*amountGuideHA;
        // for RA, only apply the corrections now if fast guiding; otherwise they get combined with PEC & sidereal-tracking and are applied later
        if (currentGuideRate>2) { if (!inBacklashHA) { cli(); targetHA+=(long)guideHA; sei(); } } else { accGuideHA+=guideHA; }
      }
      if (!inBacklashHA) {
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationHA>0)  {
          guideDurationHA-=(long)(micros()-guideDurationLastHA);
          guideDurationLastHA=micros();
          if (guideDurationHA<=0) { lstGuideStopHA=lst+amountGuideHA*(1.0/StepsPerSecond)*150.0; guideDirHA=0; } 
        }
      } else {
        // reset the counter if in backlash
        guideDurationLastHA=micros();
      }
    }
    
    if (guideDirDec) {
      if (((guideLst%gd_st==0) && ((guideLst%gd_sk!=0) || ((guideLst%gd_st1==0) && (guideLst%gd_sk1!=0) )))) {
        // nudge the targetDec (where we're supposed to be) by amountMoveDec
        if (guideDirDec=='s') sign=-1; else sign=1; cli(); targetDec=targetDec+(long)sign*(long)amountGuideDec; sei();
      }
      if (!inBacklashDec) {
        // for pulse guiding, count down the mS and stop when timed out
        if (guideDurationDec>0)  {
          guideDurationDec-=(long)(micros()-guideDurationLastDec);
          guideDurationLastDec=micros();
          if (guideDurationDec<=0) { lstGuideStopDec=lst+amountGuideDec*(1.0/StepsPerSecondDec)*150.0; guideDirDec=0;  } 
        }
      } else {
        // reset the counter if in backlash
        guideDurationLastDec=micros();
      }
    }

  }
  // allow the elevated rate to persist for a moment to allow the bulk added steps to play out after stopping
  if ((!guideDirHA) && (fabs(guideTimerRateHA)>0.001) && (lst>=lstGuideStopHA)) { cli(); guideTimerRateHA=0.0; sei(); }
  if ((!guideDirDec) && (fabs(guideTimerRateDec)>0.001) && (lst>=lstGuideStopDec)) { cli(); guideTimerRateDec=0.0; sei(); }
}
