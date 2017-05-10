// -----------------------------------------------------------------------------------
// Functions related to Parking the mount

// sets the park postion as the current position
boolean setPark() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    lastTrackingState=trackingState;
    trackingState=TrackingNone;

    // don't worry about moving around: during parking pec is turned off and backlash is cleared (0) so that targetAxis1/targetAxis2=posAxis1/posAxis2
    // this should handle getting us back to the home position for micro-step modes up to 256X

    // if sync anywhere is enabled use the corrected location
  #ifdef SYNC_ANYWHERE_ON
    long ax1md=((((long)targetAxis1.part.m+indexAxis1Steps)-trueAxis1)%1024L-((long)targetAxis1.part.m+indexAxis1Steps)%1024L);
    long ax2md=((((long)targetAxis2.part.m+indexAxis2Steps)-trueAxis2)%1024L-((long)targetAxis2.part.m+indexAxis2Steps)%1024L);
    long h=(((long)targetAxis1.part.m+indexAxis1Steps)/1024L)*1024L-ax1md;
    long d=(((long)targetAxis2.part.m+indexAxis2Steps)/1024L)*1024L-ax2md;
  #else
    long ax1md=(((long)targetAxis1.part.m-trueAxis1)%1024L-(long)targetAxis1.part.m%1024L);
    long ax2md=(((long)targetAxis2.part.m-trueAxis2)%1024L-(long)targetAxis2.part.m%1024L);
    long h=(((long)targetAxis1.part.m)/1024L)*1024L-ax1md;
    long d=(((long)targetAxis2.part.m)/1024L)*1024L-ax2md;
  #endif

    // store our position
    EEPROM_writeLong(EE_posAxis1,h);
    EEPROM_writeLong(EE_posAxis2,d);
    EEPROM_writeLong(EE_trueAxis1,trueAxis1);
    EEPROM_writeLong(EE_trueAxis2,trueAxis2);

    // and the align
    saveAlignModel();

    // and remember what side of the pier we're on
    EEPROM.write(EE_pierSide,pierSide);
    parkSaved=true;
    EEPROM.write(EE_parkSaved,parkSaved);

    trackingState=lastTrackingState;
    return true;
  }
  return false;
}

boolean saveAlignModel() {
  // and store our corrections
  GeoAlign.writeCoe();
  EEPROM_writeFloat(EE_indexAxis1,indexAxis1);
  EEPROM_writeFloat(EE_indexAxis2,indexAxis2);
  return true;
}

// takes up backlash and returns to the current position
long _bt=0;
bool doParkClearBacklash(int phase) {
  if (phase==1) {
    // start by moving fully into the backlash
    cli(); targetAxis1.part.m += backlashAxis1; targetAxis2.part.m += backlashAxis2; sei();

    // figure out how long we'll have to wait for the backlash to clear (+25%)
    long t; if (backlashAxis1>backlashAxis2) t=((long)backlashAxis1*1250)/(long)StepsPerSecondAxis1; else t=((long)backlashAxis2*1250)/(long)StepsPerSecondAxis1;
    t/=BacklashTakeupRate; // divide by the takeup rate
    t+=100;                // and add on minimum amount of time should the distance be very short
    _bt=t+millis();
    return true;
  }
  // wait until done or timed out
  if (phase==2) {
    return (millis()-_bt>0);
  }
  if (phase==3) {
    // then reverse direction and take it all up
    cli(); targetAxis1.part.m -= backlashAxis1; targetAxis2.part.m -= backlashAxis2; sei();

    // figure out how long we'll have to wait for the backlash to clear (+25%)
    long t; if (backlashAxis1>backlashAxis2) t=((long)backlashAxis1*1250)/(long)StepsPerSecondAxis1; else t=((long)backlashAxis2*1250)/(long)StepsPerSecondAxis1;
    t/=BacklashTakeupRate; // divide by the takeup rate
    t+=100;                // and add on minimum amount of time should the distance be very short
    _bt=t+millis();
    return true;
  }
  // wait until done or timed out
  if (phase==4) {
    return (millis()-_bt>0);
  }
  // we arrive back at the exact same position so targetAxis1/targetAxis2 don't need to be touched
  if (phase==5) {
    // return true on success
    if ((blAxis1!=0) || (blAxis2!=0) || (posAxis1!=(long)targetAxis1.part.m) || (posAxis2!=(long)targetAxis2.part.m)) return false; else return true;
  }
  return false;
}

int _phase=1;
int parkClearBacklash() {
  if (_phase==1) { if (doParkClearBacklash(1)) _phase++; } else
  if (_phase==2) { if (doParkClearBacklash(2)) _phase++; } else
  if (_phase==3) { if (doParkClearBacklash(3)) _phase++; } else
  if (_phase==4) { if (doParkClearBacklash(4)) _phase++; } else
  if (_phase==5) { _phase=1; if (doParkClearBacklash(5)) return 1; else return 0; }
  return -1;
}

// moves the telescope to the park position, stops tracking
byte park() {
  // Gets park position and moves the mount there
  if (trackingState!=TrackingMoveTo) {
    parkSaved=EEPROM.read(EE_parkSaved);
    if (parkStatus==NotParked) {
      if (parkSaved) {
        // stop tracking
        abortTrackingState=trackingState;
        lastTrackingState=TrackingNone;
        trackingState=TrackingNone; 

        // turn off the PEC while we park
        DisablePec();
        pecStatus=IgnorePEC;
  
        // record our status
        parkStatus=Parking;
        EEPROM.write(EE_parkStatus,parkStatus);
        
        // save the worm sense position
        EEPROM_writeLong(EE_wormSensePos,wormSensePos);
        
        // get the position we're supposed to park at
        long h=EEPROM_readLong(EE_posAxis1);
        long d=EEPROM_readLong(EE_posAxis2);
        
        // now, slew to this target HA,Dec
        byte gotoPierSide=EEPROM.read(EE_pierSide);

        // if sync anywhere is enabled we have a corrected location, convert to instrument
        // and make sure we land on full-step, and store this new location so we remember PEC
  #ifdef SYNC_ANYWHERE_ON
        long ihs=(indexAxis1Steps/1024L)*1024L;
        long ids=(indexAxis2Steps/1024L)*1024L;
        h=h-ihs;
        d=d-ids;
        float ih=ihs/(long)StepsPerDegreeAxis1;
        float id=ids/(long)StepsPerDegreeAxis2;
        // also save the alignment index values in this mode since they can change widely
        EEPROM_writeFloat(EE_indexAxis1,ih);
        EEPROM_writeFloat(EE_indexAxis2,id);
  #endif

        goTo(h,d,h,d,gotoPierSide);

        return 0;
      } else return 1; // no park position saved
    } else return 2; // not parked
  } else return 3; // already moving
}

// returns a parked telescope to operation, you must set date and time before calling this.  it also
// depends on the latitude, longitude, and timeZone; but those are stored and recalled automatically
boolean unpark() {
  if ((parkStatus==Parked) || (atHome)) {
    parkStatus=EEPROM.read(EE_parkStatus);
    parkSaved =EEPROM.read(EE_parkSaved);
    parkStatus=Parked;
    if (parkStatus==Parked) {
      if (parkSaved) {
        // enable the stepper drivers
        digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
        digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
        delay(10);

        // get corrections
        GeoAlign.readCoe();
        indexAxis1=EEPROM_readFloat(EE_indexAxis1);
        indexAxis1Steps=(long)(indexAxis1*(double)StepsPerDegreeAxis1);
        indexAxis2=EEPROM_readFloat(EE_indexAxis2);
        indexAxis2Steps=(long)(indexAxis2*(double)StepsPerDegreeAxis2);

        // get our position
        cli();
        posAxis1=EEPROM_readLong(EE_posAxis1); targetAxis1.part.m=posAxis1; targetAxis1.part.f=0;
        posAxis2=EEPROM_readLong(EE_posAxis2); targetAxis2.part.m=posAxis2; targetAxis2.part.f=0;
        trueAxis1=EEPROM_readLong(EE_trueAxis1);
        trueAxis2=EEPROM_readLong(EE_trueAxis2);

  // if sync anywhere is enabled we have a corrected location, convert to instrument
  // just like we did when we parked
  #ifdef SYNC_ANYWHERE_ON
        posAxis1=((posAxis1-indexAxis1Steps)/1024L)*1024L;
        posAxis2=((posAxis2-indexAxis2Steps)/1024L)*1024L;
  #endif
        sei();

        // see what side of the pier we're on
        pierSide=EEPROM.read(EE_pierSide);
        if (pierSide==PierSideWest) defaultDirAxis2 = defaultDirAxis2WInit; else defaultDirAxis2 = defaultDirAxis2EInit;

        // set Meridian Flip behaviour to match mount type
        #ifdef MOUNT_TYPE_GEM
        meridianFlip=MeridianFlipAlways;
        #else
        meridianFlip=MeridianFlipNever;
        #endif
        
        // update our status, we're not parked anymore
        parkStatus=NotParked;
        EEPROM.write(EE_parkStatus,parkStatus);
          
        // start tracking the sky
        trackingState=TrackingSidereal;

        // get PEC status
        pecStatus  =EEPROM.read(EE_pecStatus);
        pecRecorded=EEPROM.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;

        return true;
      };
    };
  };
  return false;
}

