// -----------------------------------------------------------------------------------
// Functions related to Parking the mount

// sets the park postion as the current position
boolean setPark() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo) && ((pierSide==PierSideNone) || (pierSide==PierSideEast) || (pierSide==PierSideWest))) {
    lastTrackingState=trackingState;
    trackingState=TrackingNone;

    // store our position as index corrected raw step counts
    cli();
    long parkPosAxis1=targetAxis1.part.m+indexAxis1Steps;
    long parkPosAxis2=targetAxis2.part.m+indexAxis2Steps;
    sei();
    nv.writeLong(EE_posAxis1,parkPosAxis1);
    nv.writeLong(EE_posAxis2,parkPosAxis2);

    // and remember what side of the pier we're on
    // if no pier side is set use the default
    if (pierSide==PierSideNone) nv.write(EE_pierSide,PierSideEast); else nv.write(EE_pierSide,pierSide);
    parkSaved=true;
    nv.write(EE_parkSaved,parkSaved);

    // and remember what the index corrections are too (etc.)
    saveAlignModel();

    trackingState=lastTrackingState;
    return true;
  }
  return false;
}

// moves the telescope to the park position
byte park() {
  if (parkStatus==Parked) return 0;                        // already parked
  int f=validateGoto(); if (f==5) f=8; if (f!=0) return f; // goto allowed?
  
  parkSaved=nv.read(EE_parkSaved);
  if (parkSaved) {
    // stop tracking
    abortTrackingState=trackingState;
    lastTrackingState=TrackingNone;
    trackingState=TrackingNone;

    // turn off the PEC while we park
    DisablePec();
    pecStatus=IgnorePEC;
    // save the worm sense position
    nv.writeLong(EE_wormSensePos,wormSensePos);

    // record our park status
    int lastParkStatus=parkStatus;
    parkStatus=Parking;
    nv.write(EE_parkStatus,parkStatus);
    
    // get the position we're supposed to park at
    long a1=nv.readLong(EE_posAxis1)-indexAxis1Steps;
    long a2=nv.readLong(EE_posAxis2)-indexAxis2Steps;
    byte gotoPierSide=nv.read(EE_pierSide);

    // now, goto this target coordinate
    int gotoStatus=goTo(a1,a2,a1,a2,gotoPierSide);

    // if failure
    if (gotoStatus!=0) {
      trackingState=abortTrackingState; // resume tracking state
      parkStatus=lastParkStatus;        // revert the park status
      nv.write(EE_parkStatus,parkStatus);
    }

    return gotoStatus;
  } else return 10; // no park position saved
}

// records the park position, updates status, shuts down the stepper motors
void parkFinish() {
  if (parkStatus!=ParkFailed) {
    // success, we're parked
    parkStatus=Parked; nv.write(EE_parkStatus,parkStatus);
  
    // trueAxisn is used to translate steps to instrument coords and changes with the pier side
    nv.writeLong(EE_trueAxis1,trueAxis1);
    nv.writeLong(EE_trueAxis2,trueAxis2);
  
    // store the pointing model
    saveAlignModel();
  }
  
  DisableStepperDrivers();
}

// adjusts targetAxis1/2 to the nearest park position for micro-step modes up to PARK_MAX_MICROSTEP
#define PARK_MAX_MICROSTEP 256
void targetNearestParkPosition() {
  // once set, parkClearBacklash() will synchronize Pos with Target again along with clearing the backlash
  cli(); long parkPosAxis1=targetAxis1.part.m; long parkPosAxis2=targetAxis2.part.m; sei();
  parkPosAxis1-=((long)PARK_MAX_MICROSTEP*2L); 
  for (int l=0; l<(PARK_MAX_MICROSTEP*4); l++) {
    if ((parkPosAxis1-trueAxis1)%((long)PARK_MAX_MICROSTEP*4L)==0) break;
    parkPosAxis1++;
  }
  parkPosAxis2-=((long)PARK_MAX_MICROSTEP*2L);
  for (int l=0; l<(PARK_MAX_MICROSTEP*4); l++) {
    if ((parkPosAxis2-trueAxis2)%((long)PARK_MAX_MICROSTEP*4L)==0) break;
    parkPosAxis2++;
  }
  cli(); targetAxis1.part.m=parkPosAxis1; targetAxis1.part.f=0; targetAxis2.part.m=parkPosAxis2; targetAxis2.part.f=0; sei();
}

// takes up backlash and returns to the current position
bool doParkClearBacklash(int phase) {
  static unsigned long timeout=0;
  static bool failed=false;

  if (phase==1) {
    failed=false;
    targetNearestParkPosition();
    timeout=(unsigned long)millis()+10000UL; // set timeout in 10s
    return true;
  }
  // wait until done or timed out
  if (phase==2) {
    cli(); if ((posAxis1==(long)targetAxis1.part.m) && (posAxis2==(long)targetAxis2.part.m) && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; }  sei();
    if ((long)(millis()-timeout)>0) { failed=true; return true; } else return false;
  }
  if (phase==3) {
    // start by moving fully into the backlash
    cli(); targetAxis1.part.m += 1; targetAxis2.part.m += 1; sei();
    timeout=(unsigned long)millis()+10000UL; // set timeout in 10s
    return true;
  }
  // wait until done or timed out
  if (phase==4) {
    cli(); if ((posAxis1==(long)targetAxis1.part.m) && (posAxis2==(long)targetAxis2.part.m) && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; }  sei();
    if ((long)(millis()-timeout)>0) { failed=true; return true; } else return false;
  }
  if (phase==5) {
    // then reverse direction and take it all up
    cli(); targetAxis1.part.m -= 1; targetAxis2.part.m -= 1; sei();
    timeout=(unsigned long)millis()+10000UL; // set timeout in 10s
    return true;
  }
  // wait until done or timed out
  if (phase==6) {
    cli(); if ((posAxis1==(long)targetAxis1.part.m) && (posAxis2==(long)targetAxis2.part.m) && !inbacklashAxis1 && !inbacklashAxis2) { sei(); return true; } sei();
    if ((long)(millis()-timeout)>0) { failed=true; return true; } else return false;
  }
  // we arrive back at the exact same position so targetAxis1/targetAxis2 don't need to be touched
  if (phase==7) {
    // return true on success
    cli(); if ((blAxis1!=0) || (blAxis2!=0) || (posAxis1!=(long)targetAxis1.part.m) || (posAxis2!=(long)targetAxis2.part.m) || failed) { sei(); return false; } else { sei(); return true; }
  }
  return false;
}

int parkClearBacklash() {
  static int phase=1;

  if (phase==1) { if (doParkClearBacklash(1)) phase++; } else
  if (phase==2) { if (doParkClearBacklash(2)) phase++; } else
  if (phase==3) { if (doParkClearBacklash(3)) phase++; } else
  if (phase==4) { if (doParkClearBacklash(4)) phase++; } else
  if (phase==5) { if (doParkClearBacklash(5)) phase++; } else
  if (phase==6) { if (doParkClearBacklash(6)) phase++; } else
  if (phase==7) { phase=1; if (doParkClearBacklash(7)) return 1; else return 0; }
  return -1;
}

// returns a parked telescope to operation, you must set date and time before calling this.  it also
// depends on the latitude, longitude, and timeZone; but those are stored and recalled automatically
boolean unpark() {
#ifdef STRICT_PARKING_ON
  if (parkStatus==Parked) {
#else
  if ((parkStatus==Parked) || ((atHome) && (parkStatus==NotParked))) {
#endif
    parkStatus=nv.read(EE_parkStatus);
    parkSaved =nv.read(EE_parkSaved);
    parkStatus=Parked;
    if (parkStatus==Parked) {
      if (parkSaved) {
        // stop the motor timers (except guiding)
        cli(); trackingTimerRateAxis1=0.0; trackingTimerRateAxis2=0.0; sei(); delay(11);

        // load the pointing model
        loadAlignModel();

        // where we were supposed to park
        cli();
        targetAxis1.part.m=nv.readLong(EE_posAxis1)-indexAxis1Steps; targetAxis1.part.f=0;
        targetAxis2.part.m=nv.readLong(EE_posAxis2)-indexAxis2Steps; targetAxis2.part.f=0;
        trueAxis1=nv.readLong(EE_trueAxis1);
        trueAxis2=nv.readLong(EE_trueAxis2);
        sei();

        // adjust to the actual park position
        targetNearestParkPosition();
        cli();
        posAxis1=targetAxis1.part.m;
        posAxis2=targetAxis2.part.m;
        blAxis1=0;
        blAxis2=0;
        sei();

        // see what side of the pier we're on
        pierSide=nv.read(EE_pierSide);
        if (pierSide==PierSideWest) defaultDirAxis2 = defaultDirAxis2WInit; else defaultDirAxis2 = defaultDirAxis2EInit;

        // set Meridian Flip behaviour to match mount type
        #ifdef MOUNT_TYPE_GEM
        meridianFlip=MeridianFlipAlways;
        #else
        meridianFlip=MeridianFlipNever;
        #endif
        
        // update our status, we're not parked anymore
        parkStatus=NotParked;
        nv.write(EE_parkStatus,parkStatus);

        // start tracking
        trackingState=TrackingSidereal;
        EnableStepperDrivers();

        // get PEC status
        pecStatus  =nv.read(EE_pecStatus);
        pecRecorded=nv.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;

        return true;
      };
    };
  };
  return false;
}

boolean isParked() {
  return (parkStatus==Parked);
}

boolean saveAlignModel() {
  // and store our corrections
  GeoAlign.writeCoe();
  cli();
  nv.writeFloat(EE_indexAxis1,indexAxis1);
  nv.writeFloat(EE_indexAxis2,indexAxis2);
  sei();
  return true;
}

boolean loadAlignModel() {
  // get align/corrections
  cli();
  indexAxis1=nv.readFloat(EE_indexAxis1);
  indexAxis1Steps=(long)(indexAxis1*(double)StepsPerDegreeAxis1);
  indexAxis2=nv.readFloat(EE_indexAxis2);
  indexAxis2Steps=(long)(indexAxis2*(double)StepsPerDegreeAxis2);
  sei();
  GeoAlign.readCoe();
  return true;
}

