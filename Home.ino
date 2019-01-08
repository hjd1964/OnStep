// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

#ifdef HOME_SENSE_ON
enum findHomeModes { FH_OFF,FH_FAST,FH_IDLE,FH_SLOW,FH_DONE };
findHomeModes findHomeMode=FH_OFF;
int PierSideStateAxis1=LOW;
int PierSideStateAxis2=LOW;
unsigned long findHomeTimeout=0L;

void checkHome() {
  // check if find home timed out or stopped
  if ((findHomeMode==FH_FAST) || (findHomeMode==FH_SLOW)) {
    if (((long)(millis()-findHomeTimeout)>0L) || ((guideDirAxis1==0) && (guideDirAxis2==0))) {
      if ((guideDirAxis1=='e') || (guideDirAxis1=='w')) guideDirAxis1='b';
      if ((guideDirAxis2=='n') || (guideDirAxis2=='s')) guideDirAxis2='b';
      safetyLimitsOn=true;
      lastError=ERR_LIMIT_SENSE;
      findHomeMode=FH_OFF;
    } else {
      if ((digitalRead(Axis1_HOME)!=PierSideStateAxis1) && ((guideDirAxis1=='e') || (guideDirAxis1=='w'))) StopAxis1();
      if ((digitalRead(Axis2_HOME)!=PierSideStateAxis2) && ((guideDirAxis2=='n') || (guideDirAxis2=='s'))) StopAxis2();
    }
  }
  // we are idle and waiting for a fast guide to stop before the final slow guide to refine the home position
  if ((findHomeMode==FH_IDLE) && (guideDirAxis1==0) && (guideDirAxis2==0)) {
    findHomeMode=FH_OFF;
    goHome(false);
  }
  // we are finishing off the find home
  if ((findHomeMode==FH_DONE) && (guideDirAxis1==0) && (guideDirAxis2==0)) {
    findHomeMode=FH_OFF;
    setHome();
  }
}

void StopAxis1() {
  guideDirAxis1='b';
  if ((guideDirAxis2!='n') && (guideDirAxis2!='s')) { if (findHomeMode==FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode==FH_FAST) findHomeMode=FH_IDLE; }
}

void StopAxis2() {
  guideDirAxis2='b';
  if ((guideDirAxis1!='e') && (guideDirAxis1!='w')) { if (findHomeMode==FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode==FH_FAST) findHomeMode=FH_IDLE; }
}
#endif

// moves telescope to the home position, then stops tracking
GotoErrors goHome(boolean fast) {

  GotoErrors f=validateGoto();
  
#ifdef HOME_SENSE_ON

  // goto allowed?
  if ((f!=GOTO_ERR_NONE) && (f!=GOTO_ERR_STANDBY)) return f; 

  if (findHomeMode!=FH_OFF) return GOTO_ERR_IN_MOTION; // guide allowed?

  // stop tracking
  trackingState=TrackingNone;

  // decide direction to guide
#ifdef HOME_AXIS1_REVERSE_ON
  char a1; if (digitalRead(Axis1_HOME)==HIGH) a1='w'; else a1='e';
#else
  char a1; if (digitalRead(Axis1_HOME)==HIGH) a1='e'; else a1='w';
#endif
#ifdef HOME_AXIS2_REVERSE_ON
  char a2; if (digitalRead(Axis2_HOME)==HIGH) a2='n'; else a2='s';
#else
  char a2; if (digitalRead(Axis2_HOME)==HIGH) a2='s'; else a2='n';
#endif

 // if (getInstrPierSide()==PierSideWest) { if (a2=='n') a2='s'; else a2='n'; }
  
  // attach interrupts to stop guide
  PierSideStateAxis1=digitalRead(Axis1_HOME);
  PierSideStateAxis2=digitalRead(Axis2_HOME);
  
  // disable limits
  safetyLimitsOn=false;
  
  // start guides
  if (fast) {
    setHome();

    // make sure tracking is enabled
    trackingState=TrackingSidereal;
    enableStepperDrivers();

    findHomeMode=FH_FAST;
    // 8=HalfMaxRate
    double secPerDeg=3600.0/(double)guideRates[8];
    findHomeTimeout=millis()+(unsigned long)(secPerDeg*180.0*1000.0);
    startGuideAxis1(a1,8,0);
    startGuideAxis2(a2,8,0,true);
  } else {
    findHomeMode=FH_SLOW;
    findHomeTimeout=millis()+30000UL;
    startGuideAxis1(a1,7,0);       // 7=48x sidereal
    startGuideAxis2(a2,7,0,true);
  }

#else
  // goto allowed?
  if (f!=GOTO_ERR_NONE) return f; 

  goTo(celestialPoleAxis1,celestialPoleAxis2,celestialPoleAxis1,celestialPoleAxis2,PierSideEast);
  homeMount=true;
#endif
  
  return GOTO_ERR_NONE;
}

boolean isHoming() {
#ifdef HOME_SENSE_ON
  return (homeMount || (findHomeMode!=FH_OFF));
#else
  return homeMount;
#endif
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
GotoErrors setHome() {
  if (guideDirAxis1 || guideDirAxis2) return GOTO_ERR_IN_MOTION;
  reactivateBacklashComp();

  if (trackingState==TrackingMoveTo)  return GOTO_ERR_GOTO;

  initStartupValues();

  // make sure limits are on
  safetyLimitsOn=true;

  // initialize and disable the stepper drivers
  StepperModeTrackingInit();
 
  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus=NotParked;
  nv.write(EE_parkStatus,parkStatus);
  
  // reset PEC, unless we have an index to recover from this
  pecRecorded=nv.read(EE_pecRecorded);
  #ifdef PEC_SENSE_OFF
    pecStatus=IgnorePEC;
    nv.write(EE_pecStatus,pecStatus);
  #else
    pecStatus=nv.read(EE_pecStatus);
  #endif
  if (!pecRecorded) pecStatus=IgnorePEC;

  // the polar home position
  InitStartPosition();
  
  return GOTO_ERR_NONE;
}

