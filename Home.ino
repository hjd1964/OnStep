// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

#if (HOME_SENSE == ON)
enum findHomeModes { FH_OFF,FH_FAST,FH_IDLE,FH_SLOW,FH_DONE };
findHomeModes findHomeMode=FH_OFF;
int PierSideStateAxis1=LOW;
int PierSideStateAxis2=LOW;
unsigned long findHomeTimeout=0L;

void checkHome() {
  // check if find home timed out or stopped
  if ((findHomeMode == FH_FAST) || (findHomeMode == FH_SLOW)) {
    if (((long)(millis()-findHomeTimeout) > 0L) || ((guideDirAxis1 == 0) && (guideDirAxis2 == 0))) {
      if ((guideDirAxis1 == 'e') || (guideDirAxis1 == 'w')) guideDirAxis1='b';
      if ((guideDirAxis2 == 'n') || (guideDirAxis2 == 's')) guideDirAxis2='b';
      safetyLimitsOn=true;
      generalError=ERR_LIMIT_SENSE;
      findHomeMode=FH_OFF;
    } else {
      if ((digitalRead(Axis1HomePin) != PierSideStateAxis1) && ((guideDirAxis1 == 'e') || (guideDirAxis1 == 'w'))) StopAxis1();
      if ((digitalRead(Axis2HomePin) != PierSideStateAxis2) && ((guideDirAxis2 == 'n') || (guideDirAxis2 == 's'))) StopAxis2();
    }
  }
  // we are idle and waiting for a fast guide to stop before the final slow guide to refine the home position
  if ((findHomeMode == FH_IDLE) && (guideDirAxis1 == 0) && (guideDirAxis2 == 0)) {
    findHomeMode=FH_OFF;
    goHome(false);
  }
  // we are finishing off the find home
  if ((findHomeMode == FH_DONE) && (guideDirAxis1 == 0) && (guideDirAxis2 == 0)) {
    findHomeMode=FH_OFF;
    setHome();
  }
}

void StopAxis1() {
  guideDirAxis1='b';
  if ((guideDirAxis2 != 'n') && (guideDirAxis2 != 's')) { if (findHomeMode == FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode == FH_FAST) findHomeMode=FH_IDLE; }
}

void StopAxis2() {
  guideDirAxis2='b';
  if ((guideDirAxis1 != 'e') && (guideDirAxis1 != 'w')) { if (findHomeMode == FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode == FH_FAST) findHomeMode=FH_IDLE; }
}
#endif

// moves telescope to the home position, then stops tracking
CommandErrors goHome(boolean fast) {
  CommandErrors e=validateGoto();
  
#if HOME_SENSE == ON
  if ((e != CE_NONE) && (e != CE_SLEW_ERR_IN_STANDBY)) return e;

  if (findHomeMode != FH_OFF) return CE_MOUNT_IN_MOTION;

  // stop tracking
  trackingState=TrackingNone;

  // decide direction to guide
  char a1; if (digitalRead(Axis1HomePin) == HOME_SENSE_STATE_AXIS2) a1='w'; else a1='e';
  char a2; if (digitalRead(Axis2HomePin) == HOME_SENSE_STATE_AXIS2) a2='n'; else a2='s';

  // attach interrupts to stop guide
  PierSideStateAxis1=digitalRead(Axis1HomePin);
  PierSideStateAxis2=digitalRead(Axis2HomePin);
  
  // disable limits
  safetyLimitsOn=false;
  
  // start guides
  if (fast) {
    setHome();

    // make sure tracking is enabled
    trackingState=TrackingSidereal;
    enableStepperDrivers();

    findHomeMode=FH_FAST;
    double secPerDeg=3600.0/(double)guideRates[8];
    findHomeTimeout=millis()+(unsigned long)(secPerDeg*180.0*1000.0);
    
    // 8=HalfMaxRate
    e=startGuideAxis1(a1,8,0);
    if (e == CE_NONE) e=startGuideAxis2(a2,8,0,true);
  } else {
    findHomeMode=FH_SLOW;
    findHomeTimeout=millis()+30000UL;
    
    // 7=48x sidereal
    e=startGuideAxis1(a1,7,0);
    if (e == CE_NONE) e=startGuideAxis2(a2,7,0,true);
  }
  if (e != CE_NONE) stopSlewing();
  return e;
#else
  if (e != CE_NONE) return e; 

  e=goTo(homePositionAxis1,homePositionAxis2,homePositionAxis1,homePositionAxis2,PierSideEast);
  homeMount=true;
  return e;
#endif
}

boolean isHoming() {
#if HOME_SENSE == ON
  return (homeMount || (findHomeMode != FH_OFF));
#else
  return homeMount;
#endif
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
CommandErrors setHome() {
  if (isSlewing()) return CE_MOUNT_IN_MOTION;

  // back to startup state
  reactivateBacklashComp();
  initStartupValues();
  currentAlt=45.0;
  doFastAltCalc(true);
  safetyLimitsOn=true;

  // no errors
  generalError=ERR_NONE;

  // initialize and disable the stepper drivers
  StepperModeTrackingInit();
 
  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus=NotParked;
  nv.write(EE_parkStatus,parkStatus);
  
  // reset PEC, unless we have an index to recover from this
  pecRecorded=nv.read(EE_pecRecorded);
  #if PEC_SENSE == OFF
    pecStatus=IgnorePEC;
    nv.write(EE_pecStatus,pecStatus);
  #else
    pecStatus=nv.read(EE_pecStatus);
  #endif
  if (!pecRecorded) pecStatus=IgnorePEC;

  // the polar home position
  InitStartPosition();
  
  return CE_NONE;
}
