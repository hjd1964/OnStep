// -----------------------------------------------------------------------------------
// Functions related to Homing the mount

// moves telescope to the home position, then stops tracking
boolean goHome() {
  if ((parkStatus!=NotParked) && (parkStatus!=Parking)) return false;  // fail, moving to home not allowed if Parked
  if (trackingState==TrackingMoveTo)                    return false;  // fail, moving to home not allowed during a move
  if (guideDirAxis1 || guideDirAxis2)                   return false;  // fail, moving to home not allowed while guiding

  cli();
  #ifdef SYNC_ANYWHERE_ON
  if (pierSide==PierSideWest) targetAxis1.part.m=-celestialPoleAxis1*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=celestialPoleAxis1*(long)StepsPerDegreeAxis1-indexAxis1Steps; targetAxis1.part.f=0;
  targetAxis2.part.m=(long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2)-indexAxis2Steps; targetAxis2.part.f=0;
  #else
  if (pierSide==PierSideWest) targetAxis1.part.m=-celestialPoleAxis1*(long)StepsPerDegreeAxis1; else targetAxis1.part.m=celestialPoleAxis1*(long)StepsPerDegreeAxis1; targetAxis1.part.f=0;
  targetAxis2.part.m=(long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2); targetAxis2.part.f=0;
  #endif
  startAxis1=posAxis1;
  startAxis2=posAxis2;
    
  abortTrackingState=trackingState;
  lastTrackingState=TrackingNone;
  trackingState=TrackingMoveTo; SetSiderealClockRate(siderealInterval);
  sei();

  homeMount=true;
  
  DecayModeGoto();
  
  return true;
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
boolean setHome() {
  if (trackingState==TrackingMoveTo) return false;  // fail, forcing home not allowed during a move

  // default values for state variables
  pierSide            = PierSideNone;
  dirAxis1            = 1;
  dirAxis2            = 1;
  defaultDirAxis2     = defaultDirAxis2EInit;
  if (latitude>0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
  newTargetRA         = 0;        
  newTargetDec        = 0;
  newTargetAlt        = 0;
  newTargetAzm        = 0;
  origTargetAxis1.fixed = 0;
  origTargetAxis2       = 0;
  
  // reset pointing model
  alignNumStars       = 0;
  alignThisStar       = 0;
  indexAxis1          = 0;
  indexAxis1Steps     = 0;
  indexAxis2          = 0;
  indexAxis2Steps     = 0;
  #ifdef MOUNT_TYPE_ALTAZM
  Align.init();
  #endif
  GeoAlign.init();
  
  // reset meridian flip control
  #ifdef MOUNT_TYPE_GEM
  meridianFlip = MeridianFlipAlways;
  #endif
  #ifdef MOUNT_TYPE_FORK
  meridianFlip = MeridianFlipAlign;
  #endif
  #ifdef MOUNT_TYPE_FORK_ALT
  meridianFlip = MeridianFlipNever;
  #endif
  #ifdef MOUNT_TYPE_ALTAZM
  meridianFlip = MeridianFlipNever;
  #endif

  // where we are
  homeMount           = false;
  atHome              = true;
  lastError           = ERR_NONE;

  // reset tracking and rates
  trackingState       = TrackingNone;
  lastTrackingState   = TrackingNone;
  timerRateAxis1      = SiderealRate;
  timerRateAxis2      = SiderealRate;

  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus          = NotParked;
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

  // the polar home position
  startAxis1 = celestialPoleAxis1*(long)StepsPerDegreeAxis1;
  startAxis2 = celestialPoleAxis2*(double)StepsPerDegreeAxis2;

  // clear pulse-guiding state
  guideDirAxis1          = 0;
  guideDurationAxis1     = 0;
  guideDurationLastAxis1 = 0;
  guideDirAxis2          = 0;
  guideDurationAxis2     = 0;
  guideDurationLastAxis2 = 0;

  // initialize/disable the stepper drivers
  digitalWrite(Axis1_EN,Axis1_Disabled); axis1Enabled=false;
  digitalWrite(Axis2_EN,Axis2_Disabled); axis2Enabled=false;
  DecayModeTracking();

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

