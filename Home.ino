// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

// trim time in ms at guide rate 5 (8x or 2 arc-minutes/second,) negative times reverse direction moving away from switch.
#ifndef HOME_AXIS1_TRIM_TIME_E
  #define HOME_AXIS1_TRIM_TIME_E 0
#endif
#ifndef HOME_AXIS1_TRIM_TIME_W
  #define HOME_AXIS1_TRIM_TIME_W 0
#endif
#ifndef HOME_AXIS2_TRIM_TIME_N
  #define HOME_AXIS2_TRIM_TIME_N 0
#endif
#ifndef HOME_AXIS2_TRIM_TIME_S
  #define HOME_AXIS2_TRIM_TIME_S 0
#endif

#if (HOME_SENSE != OFF)
int PierSideStateAxis1=LOW;
int PierSideStateAxis2=LOW;
unsigned long findHomeTimeout=0L;

void checkHome() {
  // check if find home timed out or stopped
  if (findHomeMode == FH_FAST || findHomeMode == FH_SLOW) {
    if ((long)(millis()-findHomeTimeout) > 0L || (!guideDirAxis1 && !guideDirAxis2)) {
      if ((long)(millis()-findHomeTimeout) > 0L) { generalError=ERR_LIMIT_SENSE; VLF("MSG: Homing timed out, stopping guides"); }
      stopGuideAxis1();
      stopGuideAxis2();
      safetyLimitsOn=true;
      findHomeMode=FH_OFF;
    } else {
      if (axis1HomeSense.read() != PierSideStateAxis1 && (guideDirAxis1 == 'e' || guideDirAxis1 == 'w')) stopHomeAxis1();
      if (axis2HomeSense.read() != PierSideStateAxis2 && (guideDirAxis2 == 'n' || guideDirAxis2 == 's')) stopHomeAxis2();
    }
  }
  // we are idle and waiting for a fast guide to stop before the final slow guide to refine the home position
  if (findHomeMode == FH_READY_SLOW && !guideDirAxis1 && !guideDirAxis2) {
    findHomeMode=FH_OFF;
    goHome(FH_SLOW);
  }
  // we are ready to trim the position
  if (findHomeMode == FH_READY_TRIM && !guideDirAxis1 && !guideDirAxis2) {
    findHomeMode=FH_OFF;
    goHome(FH_TRIM);
  }
  // we are finishing off the find home
  if (findHomeMode == FH_DONE && !guideDirAxis1 && !guideDirAxis2) {
    findHomeMode=FH_OFF;

    if (AXIS2_TANGENT_ARM == ON) {
      cli();
      targetAxis2.part.m = 0; targetAxis2.part.f = 0;
      posAxis2           = 0;
      sei();
      VLF("MSG: Homing done, tangent arm centered");
    } else {
      // at the home position
      initStartPosition();
      atHome=true;
    }
    VLF("MSG: Homing done");
  }
}

void stopHomeAxis1() {
  stopGuideAxis1();
  VLF("MSG: Homing switch detected, stopping guide on Axis1");
  if (guideDirAxis2 != 'n' && guideDirAxis2 != 's') { if (findHomeMode == FH_SLOW) findHomeMode=FH_READY_TRIM; if (findHomeMode == FH_FAST) findHomeMode=FH_READY_SLOW; }
}

void stopHomeAxis2() {
  stopGuideAxis2();
  VLF("MSG: Homing switch detected, stopping guide on Axis2");
  if (guideDirAxis1 != 'e' && guideDirAxis1 != 'w') { if (findHomeMode == FH_SLOW) findHomeMode=FH_READY_TRIM; if (findHomeMode == FH_FAST) findHomeMode=FH_READY_SLOW; }
}
#endif

// moves telescope to the home position, then stops tracking
CommandErrors goHome(findHomeModes nextMode) {
  CommandErrors e=validateGoto();
  
#if HOME_SENSE != OFF
  if (e != CE_NONE && e != CE_SLEW_ERR_IN_STANDBY) return e;

  if (findHomeMode != FH_OFF) return CE_MOUNT_IN_MOTION;

  // reset/read initial state
  axis1HomeSense.reset(); PierSideStateAxis1=axis1HomeSense.read();
  axis2HomeSense.reset(); PierSideStateAxis2=axis2HomeSense.read();

  // decide direction to guide
  char a1; if (PierSideStateAxis1 == HOME_SENSE_STATE_AXIS1) a1='w'; else a1='e';
  char a2; if (PierSideStateAxis2 == HOME_SENSE_STATE_AXIS2) a2='n'; else a2='s';

  // disable limits
  safetyLimitsOn=false;

  // start guides
  if (nextMode == FH_START) {
    abortTrackingState=trackingState;
    if (AXIS2_TANGENT_ARM == OFF) trackingState=TrackingNone;

    // make sure motors are powered on
    enableStepperDrivers();

    findHomeMode=FH_FAST;
    double secPerDeg=3600.0/(double)guideRates[8];
    findHomeTimeout=millis()+(unsigned long)(secPerDeg*180.0*1000.0);

    // 8=HalfMaxRate
    if (AXIS2_TANGENT_ARM == OFF) e=startGuideAxis1(a1,8,0,false);
    if (e == CE_NONE) e=startGuideAxis2(a2,8,0,false,true);
    if (e == CE_NONE) VLF("MSG: Homing started phase 1"); else VF("MSG: Homing start phase 1 failed");
  } else if (nextMode == FH_SLOW) {
    findHomeMode=FH_SLOW;
    findHomeTimeout=millis()+30000UL;

    // 7=48x sidereal
    if (AXIS2_TANGENT_ARM == OFF) e=startGuideAxis1(a1,7,0,false);
    if (e == CE_NONE) e=startGuideAxis2(a2,7,0,false,true);
    if (e == CE_NONE) VLF("MSG: Homing started phase 2"); else { VF("MSG: Homing start phase 2 failed, "); VL(commandErrorStr[e]); }
  } else if (nextMode == FH_TRIM) {
    findHomeMode=FH_DONE;
    findHomeTimeout=millis()+0UL;

    // 7=48x sidereal
    if (AXIS2_TANGENT_ARM == OFF) {
      if (a1 == 'e') {
        if (HOME_AXIS1_TRIM_TIME_E > 0) e=startGuideAxis1('e',5,HOME_AXIS1_TRIM_TIME_E,false); else
          if (HOME_AXIS1_TRIM_TIME_E < 0) e=startGuideAxis1('w',5,labs(HOME_AXIS1_TRIM_TIME_E),false); 
      } else
      if (a1 == 'w') {
        if (HOME_AXIS1_TRIM_TIME_W > 0) e=startGuideAxis1('w',5,HOME_AXIS1_TRIM_TIME_W,false); else
          if (HOME_AXIS1_TRIM_TIME_W < 0) e=startGuideAxis1('e',5,labs(HOME_AXIS1_TRIM_TIME_W),false); 
      }
    }
    if (e == CE_NONE) {
      if (a2 == 'n') {
        if (HOME_AXIS2_TRIM_TIME_N > 0) e=startGuideAxis2('n',5,HOME_AXIS2_TRIM_TIME_N,false,true); else
          if (HOME_AXIS2_TRIM_TIME_N < 0) e=startGuideAxis2('s',5,labs(HOME_AXIS2_TRIM_TIME_N),false,true); 
      }
      if (a2 == 's') {
        if (HOME_AXIS2_TRIM_TIME_S > 0) e=startGuideAxis2('s',5,HOME_AXIS2_TRIM_TIME_S,false,true); else
          if (HOME_AXIS2_TRIM_TIME_S < 0) e=startGuideAxis2('n',5,labs(HOME_AXIS2_TRIM_TIME_S),false,true);
      }
    }
    if (e == CE_NONE) VLF("MSG: Homing started phase 3"); else { VF("MSG: Homing start phase 3 failed, "); VL(commandErrorStr[e]); }
  }
  if (e != CE_NONE) stopSlewingAndTracking(SS_ALL_FAST);
  return e;
#else
  if (e != CE_NONE) return e; 

  abortTrackingState=trackingState;

  if (AXIS2_TANGENT_ARM == ON) {
    double h=getInstrAxis1();
    double i2=indexAxis2;
    int p=getInstrPierSide();
    if (latitude >= 0) { if (p == PIER_SIDE_WEST) i2=180.0-i2; } else { if (p == PIER_SIDE_WEST) i2=-180.0-i2; }
    e=goTo(h,i2,h,i2,p,false);
  } else {
    trackingState=TrackingNone;
    e=goTo(homePositionAxis1,homePositionAxis2,homePositionAxis1,homePositionAxis2,PIER_SIDE_EAST,false);
  }

  if (e == CE_NONE) { VLF("MSG: Homing started"); homeMount=true; } else { VLF("MSG: Homing failed"); trackingState=abortTrackingState; }
  return e;
#endif
}

bool isHoming() {
#if HOME_SENSE != OFF
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
  initStartPosition();

  currentAlt=45.0;
  doFastAltCalc(true);

  safetyLimitsOn=true;

  // initialize and disable the stepper drivers
  StepperModeTrackingInit();
 
  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus=NotParked;
  nv.write(EE_parkStatus,parkStatus);
  
  // reset PEC, unless we have an index to recover from this
  pecRecorded=nv.read(EE_pecRecorded);
  if (pecRecorded != true && pecRecorded != false) { pecRecorded=false; DLF("ERR, setHome(): bad NV pecRecorded"); }
  #if PEC_SENSE == OFF
    pecStatus=IgnorePEC;
    nv.write(EE_pecStatus,pecStatus);
  #else
    pecStatus=nv.read(EE_pecStatus);
    if (pecStatus < PEC_STATUS_FIRST || pecStatus > PEC_STATUS_LAST) { pecStatus=IgnorePEC; DLF("ERR, setHome(): bad NV pecStatus"); }
  #endif
  if (!pecRecorded) pecStatus=IgnorePEC;

  return CE_NONE;
}
