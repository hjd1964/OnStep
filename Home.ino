// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

#ifdef HOME_SENSE_ON
enum findHomeModes { FH_OFF,FH_FAST,FH_IDLE,FH_SLOW,FH_DONE };
volatile findHomeModes findHomeMode=FH_OFF;
unsigned long findHomeTimeout=0L;

void checkHome() {
  // check if find home timed out or stopped
  if ((findHomeMode==FH_FAST) || (findHomeMode==FH_SLOW)) {
    if (((long)(millis()-findHomeTimeout)>0L) || ((guideDirAxis1==0) && (guideDirAxis2==0))) {
      if ((guideDirAxis1=='e') || (guideDirAxis1=='w')) guideDirAxis1='b';
      if ((guideDirAxis2=='n') || (guideDirAxis2=='s')) guideDirAxis2='b';
      detachInterrupt(Axis1_HOME);
      detachInterrupt(Axis2_HOME);
      safetyLimitsOn=true;
      lastError=ERR_LIMIT_SENSE;
      findHomeMode=FH_OFF;
    }
  }
  // we are idle and waiting for a fast guide to stop before the final slow guide to refine the home position
  if ((findHomeMode==FH_IDLE) && (guideDirAxis1==0) && (guideDirAxis2==0)) {
    findHomeMode=FH_OFF;
    goHome(false);
  }
  // we are finishing off the find home
  if ((findHomeMode==FH_DONE) && (guideDirAxis1==0) && (guideDirAxis2==0)) {
    detachInterrupt(Axis1_HOME);
    detachInterrupt(Axis2_HOME);
    findHomeMode=FH_OFF;
    setHome();
  }
}

void StopAxis1() {
  guideDirAxis1='b';
  detachInterrupt(Axis1_HOME);
  if ((guideDirAxis2!='n') && (guideDirAxis2!='s')) { if (findHomeMode==FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode==FH_FAST) findHomeMode=FH_IDLE; }
}

void StopAxis2() {
  guideDirAxis2='b';
  detachInterrupt(Axis2_HOME);
  if ((guideDirAxis1!='e') && (guideDirAxis1!='w')) { if (findHomeMode==FH_SLOW) findHomeMode=FH_DONE; if (findHomeMode==FH_FAST) findHomeMode=FH_IDLE; }
}
#endif

// moves telescope to the home position, then stops tracking
int goHome(boolean fast) {
  int f=validateGoto(); if (f==5) f=8; if (f!=0) return f; // goto allowed?

#ifdef HOME_SENSE_ON
  if (findHomeMode!=FH_OFF) return 8; // guide allowed?

//  pinMode(Axis1_HOME,INPUT_PULLUP);
//  pinMode(Axis2_HOME,INPUT_PULLUP);

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
  if (pierSide==PierSideNone) {
    pierSide=PierSideEast;
    defaultDirAxis2=defaultDirAxis2EInit;
  }
  if (pierSide==PierSideWest) { if (a2=='n') a2='s'; else a2='n'; }
  
  // attach interrupts to stop guide
  attachInterrupt(digitalPinToInterrupt(Axis1_HOME), StopAxis1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Axis2_HOME), StopAxis2, CHANGE);
  
  // disable limits
  safetyLimitsOn=false;
  
  // start guides
  if (fast) {
    findHomeMode=FH_FAST;
    // 8=HalfMaxRate
    double secPerDeg=3600.0/(double)guideRates[8];
    findHomeTimeout=millis()+(unsigned long)(secPerDeg*180.0*1000.0);
    startGuideAxis1(a1,8,0);
    startGuideAxis2(a2,8,0);
  } else {
    findHomeMode=FH_SLOW;
    // 6=24x sidereal
    findHomeTimeout=millis()+60000UL;
    startGuideAxis1(a1,6,0);
    startGuideAxis2(a2,6,0);
  }

#else
  cli();
  if (pierSide==PierSideWest) targetAxis1.part.m=-(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; else targetAxis1.part.m=(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; targetAxis1.part.f=0;
  targetAxis2.part.m=(long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2)-indexAxis2Steps; targetAxis2.part.f=0;
  startAxis1=posAxis1;
  startAxis2=posAxis2;
    
  abortTrackingState=trackingState;
  lastTrackingState=TrackingNone;
  trackingState=TrackingMoveTo; SiderealClockSetInterval(siderealInterval);
  sei();

  homeMount=true;
  
  StepperModeGoto();
#endif
  
  return 0;
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
int setHome() {
  if (guideDirAxis1 || guideDirAxis2) return 8;   // fail, already in motion
  if (trackingState==TrackingMoveTo)  return 5;   // fail, goto in progress

  Init_Startup_Values();

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
  startAxis1 = celestialPoleAxis1*(double)StepsPerDegreeAxis1;
  startAxis2 = celestialPoleAxis2*(double)StepsPerDegreeAxis2;
  cli();
  targetAxis1.part.m = startAxis1; targetAxis1.part.f = 0;
  posAxis1           = startAxis1;
  trueAxis1          = startAxis1;
  targetAxis2.part.m = startAxis2; targetAxis2.part.f = 0;
  posAxis2           = startAxis2;
  trueAxis2          = startAxis2;
  blAxis1            = 0;
  blAxis2            = 0;
  sei();
  
  return 0;
}

