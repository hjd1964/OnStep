// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

// moves telescope to the home position, then stops tracking
int goHome() {
  int f=validateGoto(); if (f==5) f=8; if (f!=0) return f; // goto allowed?

  cli();
  if (pierSide==PierSideWest) targetAxis1.part.m=-celestialPoleAxis1*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=celestialPoleAxis1*(long)StepsPerDegreeAxis1-indexAxis1Steps; targetAxis1.part.f=0;
  targetAxis2.part.m=(long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2)-indexAxis2Steps; targetAxis2.part.f=0;
  startAxis1=posAxis1;
  startAxis2=posAxis2;
    
  abortTrackingState=trackingState;
  lastTrackingState=TrackingNone;
  trackingState=TrackingMoveTo; SiderealClockSetInterval(siderealInterval);
  sei();

  homeMount=true;
  
  StepperModeGoto();
  
  return 0;
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
boolean setHome() {
  if (trackingState==TrackingMoveTo) return false;  // fail, forcing home not allowed during a move

  Init_Startup_Values();

  // initialize and disable the stepper drivers
  StepperModeTrackingInit();
 
  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus=NotParked;
  EEPROM.write(EE_parkStatus,parkStatus);
  
  // reset PEC, unless we have an index to recover from this
  pecRecorded=EEPROM.read(EE_pecRecorded);
  #ifdef PEC_SENSE_OFF
    pecStatus=IgnorePEC;
    EEPROM.write(EE_pecStatus,pecStatus);
  #else
    pecStatus=EEPROM.read(EE_pecStatus);
  #endif
  if (!pecRecorded) pecStatus=IgnorePEC;

  // initialize guiding
  initGuide();

  // the polar home position
  startAxis1 = celestialPoleAxis1*(long)StepsPerDegreeAxis1;
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
  
  return true;
}

