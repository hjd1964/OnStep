// -----------------------------------------------------------------------------------
// Functions related to Parking the mount

// sets the park postion as the current position
CommandErrors setPark() {
  if (parkStatus == ParkFailed)         return CE_PARK_FAILED;
  if (parkStatus == Parked)             return CE_PARKED;
  if (isSlewing())                      return CE_MOUNT_IN_MOTION;
  if (faultAxis1 || faultAxis2)         return CE_SLEW_ERR_HARDWARE_FAULT;

  lastTrackingState=trackingState;
  trackingState=TrackingNone;

  // store our position
  nv.writeFloat(EE_posAxis1,getInstrAxis1());
  nv.writeFloat(EE_posAxis2,getInstrAxis2());
  int p=getInstrPierSide(); if (p == PierSideNone) nv.write(EE_pierSide,PierSideEast); else nv.write(EE_pierSide,p);

  // record our park status
  parkSaved=true; nv.write(EE_parkSaved,parkSaved);

  // and remember what the index corrections are too (etc.)
  saveAlignModel();

  trackingState=lastTrackingState;
  
  return CE_NONE;
}

// moves the telescope to the park position
CommandErrors park() {
  if (!parkSaved)                       return CE_NO_PARK_POSITION_SET;
  if (parkStatus == Parked)             return CE_PARKED;
  if (!axis1Enabled)                    return CE_SLEW_ERR_IN_STANDBY;
  if (isSlewing())                      return CE_MOUNT_IN_MOTION;
  if (faultAxis1 || faultAxis2)         return CE_SLEW_ERR_HARDWARE_FAULT;

  CommandErrors e=validateGoto();
  if (e != CE_NONE)                     return e;
  
  // stop tracking
  abortTrackingState=trackingState;
  lastTrackingState=TrackingNone;
  trackingState=TrackingNone;

#if MOUNT_TYPE != ALTAZM
  // turn off the PEC while we park
  disablePec();
  pecStatus=IgnorePEC;
#endif

  // save the worm sense position
  nv.writeLong(EE_wormSensePos,wormSensePos);

  // record our park status
  int lastParkStatus=parkStatus; 
  parkStatus=Parking; nv.write(EE_parkStatus,parkStatus);
  
  // get suggested park position
  double parkTargetAxis1=nv.readFloat(EE_posAxis1);
  double parkTargetAxis2=nv.readFloat(EE_posAxis2);
  int parkPierSide=nv.read(EE_pierSide);
  if (parkPierSide != PierSideNone && parkPierSide != PierSideEast && parkPierSide != PierSideWest) { parkPierSide=PierSideNone; DL("NV: bad parkPierSide"); } // valid parkPierSide?

  // now, goto this target coordinate
  e=goTo(parkTargetAxis1,parkTargetAxis2,parkTargetAxis1,parkTargetAxis2,parkPierSide);
  if (e != CE_NONE) {
    trackingState=abortTrackingState; // resume tracking state
    parkStatus=lastParkStatus;        // revert the park status
    nv.write(EE_parkStatus,parkStatus);
    return e;
  }
  return CE_NONE;
}

// records the park position, updates status, shuts down the stepper motors
void parkFinish() {
  if (parkStatus != ParkFailed) {
    // success, we're parked
    parkStatus=Parked; nv.write(EE_parkStatus,parkStatus);
  
    // store the pointing model
    saveAlignModel();
  }
  
  disableStepperDrivers();
}

// adjusts targetAxis1/2 to the nearest park position for micro-step modes up to PARK_MAX_MICROSTEP
#define PARK_MAX_MICROSTEP 256
void targetNearestParkPosition() {
  // once set, parkClearBacklash() will synchronize Pos with Target again along with clearing the backlash
  cli(); long parkPosAxis1=targetAxis1.part.m; long parkPosAxis2=targetAxis2.part.m; sei();
  parkPosAxis1-=((long)PARK_MAX_MICROSTEP*2L); 
  for (int l=0; l < (PARK_MAX_MICROSTEP*4); l++) {
    if ((parkPosAxis1)%((long)PARK_MAX_MICROSTEP*4L) == 0) break;
    parkPosAxis1++;
  }
  parkPosAxis2-=((long)PARK_MAX_MICROSTEP*2L);
  for (int l=0; l < (PARK_MAX_MICROSTEP*4); l++) {
    if ((parkPosAxis2)%((long)PARK_MAX_MICROSTEP*4L) == 0) break;
    parkPosAxis2++;
  }
  cli(); targetAxis1.part.m=parkPosAxis1; targetAxis1.part.f=0; targetAxis2.part.m=parkPosAxis2; targetAxis2.part.f=0; sei();
}

// takes up backlash and returns to the current position
bool doParkClearBacklash(int phase) {
  static unsigned long timeout=0;
  static bool failed=false;

  if (phase == 1) {
    failed=false;
    targetNearestParkPosition();
    timeout=(unsigned long)millis()+10000UL; // set timeout in 10s
    return true;
  }
  if (phase == 2) {
    // wait until done or timed out
    cli(); if ((posAxis1 == (long)targetAxis1.part.m) && (posAxis2 == (long)targetAxis2.part.m) && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; }  sei();
    if ((long)(millis()-timeout) > 0) { failed=true; DL("PCB: timeout finding nearest position"); return true; } else return false;
  }
  if (phase == 3) {
    // start by moving fully into the backlash
    cli(); targetAxis1.part.m += 1; targetAxis2.part.m += 1; sei();
    timeout=(unsigned long)millis()+10000UL;
    return true;
  }
  if (phase == 4) {
    // wait until done or timed out
    cli(); if ((posAxis1 == (long)targetAxis1.part.m) && (posAxis2 == (long)targetAxis2.part.m) && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; }  sei();
    if ((long)(millis()-timeout) > 0) { failed=true; DL("PCB: timeout moving into BL"); return true; } else return false;
  }
  if (phase == 5) {
    // then reverse direction and take it all up
    cli(); targetAxis1.part.m -= 1; targetAxis2.part.m -= 1; sei();
    timeout=(unsigned long)millis()+10000UL;
    return true;
  }
  if (phase == 6) {
    // wait until done or timed out
    cli(); if (posAxis1 == (long)targetAxis1.part.m && posAxis2 == (long)targetAxis2.part.m && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; } sei();
    if ((long)(millis()-timeout) > 0) { failed=true; DL("PCB: timeout moving out of BL"); return true; } else return false;
  }
  if (phase == 7) {
    // double check to be sure backlash is zero
    cli(); if (blAxis1 != 0 || blAxis2 != 0) { sei(); failed=true; DL("PCB: non-zero BL"); } else sei();
    return true;
  }
  if (phase == 8) {
    // return true on success
    return !failed;
  }
  return false;
}

int parkClearBacklash() {
  static int phase=1;
  if (phase == 1) { if (doParkClearBacklash(1)) phase++; MLL("PCB: start clearing BL"); } else
  if (phase == 2) { if (doParkClearBacklash(2)) phase++; } else
  if (phase == 3) { if (doParkClearBacklash(3)) phase++; } else
  if (phase == 4) { if (doParkClearBacklash(4)) phase++; } else
  if (phase == 5) { if (doParkClearBacklash(5)) phase++; } else
  if (phase == 6) { if (doParkClearBacklash(6)) phase++; } else
  if (phase == 7) { if (doParkClearBacklash(7)) phase++; } else
  if (phase == 8) { phase=1; if (doParkClearBacklash(8)) { MLL("PCB: done clearing BL"); return PCB_SUCCESS; } else { DL("PCB: failure"); return PCB_FAILURE; } }
  return PCB_BUSY;
}

// returns a parked telescope to operation, you must set date and time before calling this.  it also
// depends on the latitude, longitude, and timeZone; but those are stored and recalled automatically
CommandErrors unPark(bool withTrackingOn) {
  if (!parkSaved)                       return CE_NO_PARK_POSITION_SET;
  if (parkStatus != Parked && !atHome)  return CE_NOT_PARKED;
#if STRICT_PARKING == ON
  if (parkStatus != Parked)             return CE_NOT_PARKED;
#endif
  if (isSlewing())                      return CE_MOUNT_IN_MOTION;
  if (faultAxis1 || faultAxis2)         return CE_SLEW_ERR_HARDWARE_FAULT;

  initStartupValues();

  // make sure limits are on
  safetyLimitsOn=true;

  // initialize and disable the stepper drivers
  StepperModeTrackingInit();
   
  // the polar home position
  currentAlt=45.0;
  doFastAltCalc(true);
  InitStartPosition();

  // stop the motor timers (except guiding)
  cli(); trackingTimerRateAxis1=0.0; trackingTimerRateAxis2=0.0; sei(); delay(11);

  // load the pointing model
  loadAlignModel();

  // get suggested park position
  int parkPierSide=nv.read(EE_pierSide);
  if (parkPierSide != PierSideNone && parkPierSide != PierSideEast && parkPierSide != PierSideWest) { parkPierSide=PierSideNone; DL("NV: bad parkPierSide"); } // valid parkPierSide?

  setTargetAxis1(nv.readFloat(EE_posAxis1),parkPierSide);
  setTargetAxis2(nv.readFloat(EE_posAxis2),parkPierSide);

  // adjust target to the actual park position (just like we did when we parked)
  targetNearestParkPosition();

  // and set the instrument position to agree
  cli();
  posAxis1=targetAxis1.part.m;
  posAxis2=targetAxis2.part.m;
  sei();
  
  // set Meridian Flip behaviour to match mount type
  #if MOUNT_TYPE == GEM
    meridianFlip=MeridianFlipAlways;
  #else
    meridianFlip=MeridianFlipNever;
  #endif

  if (withTrackingOn) {
    // update our status, we're not parked anymore
    parkStatus=NotParked;
    nv.write(EE_parkStatus,parkStatus);

    // start tracking
    trackingState=TrackingSidereal;
    enableStepperDrivers();

    // get PEC status
    pecStatus  =nv.read(EE_pecStatus);
    if (pecStatus < PEC_STATUS_FIRST || pecStatus > PEC_STATUS_LAST) { pecStatus=IgnorePEC; DL("NV: bad pecStatus"); } // valid PEC status?
    
    pecRecorded=nv.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;
    if (pecRecorded != true && pecRecorded != false) { pecRecorded=false; DL("NV: bad pecRecorded"); } // valid PEC recorded?
  }
  return CE_NONE;
}

boolean isParked() {
  return (parkStatus == Parked);
}

boolean saveAlignModel() {
  // and store our corrections
  Align.writeCoe();
  nv.writeFloat(EE_indexAxis1,indexAxis1);
  nv.writeFloat(EE_indexAxis2,indexAxis2);
  return true;
}

boolean loadAlignModel() {
  // get align/corrections
  indexAxis1=nv.readFloat(EE_indexAxis1);
  if (indexAxis1 < -720 || indexAxis1 > 720) { indexAxis1=0; DL("NV: bad indexAxis1"); } // valid indexAxis1 recorded?
  indexAxis1Steps=(long)(indexAxis1*(double)AXIS1_STEPS_PER_DEGREE);
  
  indexAxis2=nv.readFloat(EE_indexAxis2);
  if (indexAxis2 < -720 || indexAxis2 > 720) { indexAxis2=0; DL("NV: bad indexAxis2"); } // valid indexAxis2 recorded?
  indexAxis2Steps=(long)(indexAxis2*(double)AXIS2_STEPS_PER_DEGREE);
  
  Align.readCoe();
  return true;
}
