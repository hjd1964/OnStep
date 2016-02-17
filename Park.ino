// -----------------------------------------------------------------------------------
// functions related to Parking the mount

// sets the park postion as the current position
boolean setPark() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    lastTrackingState=trackingState;
    trackingState=TrackingNone;

    // don't worry about moving around: during parking pec is turned off and backlash is cleared (0) so that targetAxis1/targetAxis2=posAxis1/posAxis2
    // this should handle getting us back to the home position for micro-step modes up to 256X

    // if sync anywhere is enabled use the corrected location
  #ifdef SYNC_ANYWHERE_ON
    long ax1md=((((long)targetAxis1.part.m+IHS)-trueAxis1)%1024L-((long)targetAxis1.part.m+IHS)%1024L);
    long ax2md=((((long)targetAxis2.part.m+IDS)-trueAxis2)%1024L-((long)targetAxis2.part.m+IDS)%1024L);
    long h=(((long)targetAxis1.part.m+IHS)/1024L)*1024L-ax1md;
    long d=(((long)targetAxis2.part.m+IDS)/1024L)*1024L-ax2md;
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
  EEPROM_writeFloat(EE_doCor,doCor);
  EEPROM_writeFloat(EE_pdCor,pdCor);
  EEPROM_writeFloat(EE_altCor,altCor);
  EEPROM_writeFloat(EE_azmCor,azmCor);
  EEPROM_writeFloat(EE_IH,IH);
  EEPROM_writeFloat(EE_ID,ID);
  return true;
}

// takes up backlash and returns to the current position
boolean parkClearBacklash() {

  // backlash takeup rate
  cli();
  long LastTimerRateAxis1=timerRateAxis1;
  long LastTimerRateAxis2=timerRateAxis2;
  timerRateAxis1=timerRateBacklashAxis1;
  timerRateAxis2=timerRateBacklashAxis2;
  sei();

  // figure out how long we'll have to wait for the backlash to clear (+50%)
  long t; if (backlashAxis1>backlashAxis2) t=((long)backlashAxis1*1500)/(long)StepsPerSecondAxis1; else t=((long)backlashAxis2*1500)/(long)StepsPerSecondAxis1;
  t=(t/BacklashTakeupRate+250)/12;

  // start by moving fully into the backlash
  cli();
  targetAxis1.part.m += backlashAxis1;
  targetAxis2.part.m += backlashAxis2;
  sei();

  // wait until done or timed out
  for (int i=0; i<12; i++) if ((blAxis1!=backlashAxis1) || (posAxis1!=(long)targetAxis1.part.m) || (blAxis2!=backlashAxis2) || (posAxis2!=(long)targetAxis2.part.m)) delay(t);

  // then reverse direction and take it all up
  cli();
  targetAxis1.part.m -= backlashAxis1;
  targetAxis2.part.m -= backlashAxis2;
  sei();

  // wait until done or timed out, plus a safety margin
  for (int i=0; i<24; i++) if ((blAxis1!=0) || (posAxis1!=(long)targetAxis1.part.m) || (blAxis2!=0) || (posAxis2!=(long)targetAxis2.part.m)) delay(t);

  // we arrive back at the exact same position so ftargetAxis1/Dec don't need to be touched
  
  // move at the previous speed
  cli();
  timerRateAxis1=LastTimerRateAxis1;
  timerRateAxis2=LastTimerRateAxis2;
  sei();
  
  // return true on success
  if ((blAxis1!=0) || (blAxis2!=0)) return false; else return true;
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
        long ihs=(IHS/1024L)*1024L;
        long ids=(IDS/1024L)*1024L;
        h=h-ihs;
        d=d-ids;
        float ih=ihs/(long)StepsPerDegreeAxis1;
        float id=ids/(long)StepsPerDegreeAxis2;
        // also save the alignment index values in this mode since they can change widely
        EEPROM_writeFloat(EE_IH,ih);
        EEPROM_writeFloat(EE_ID,id);
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
  parkStatus=EEPROM.read(EE_parkStatus);
  parkSaved =EEPROM.read(EE_parkSaved);
  parkStatus=Parked;
  if (trackingState!=TrackingMoveTo) {
    if (parkStatus==Parked) {
      if (parkSaved) {
        // enable the stepper drivers
        digitalWrite(Axis1_EN,Axis1_Enabled);
        digitalWrite(Axis2_EN,Axis2_Enabled);
        delay(10);

        // get corrections
        doCor=EEPROM_readFloat(EE_doCor);
        pdCor=EEPROM_readFloat(EE_pdCor);
        altCor=EEPROM_readFloat(EE_altCor);
        azmCor=EEPROM_readFloat(EE_azmCor);
        IH=EEPROM_readFloat(EE_IH);
        IHS=(long)(IH*(double)StepsPerDegreeAxis1);
        ID=EEPROM_readFloat(EE_ID);
        IDS=(long)(ID*(double)StepsPerDegreeAxis2);

        // get our position
        cli();
        posAxis1=EEPROM_readLong(EE_posAxis1);  targetAxis1.part.m=posAxis1; targetAxis1.part.f=0;
        posAxis2=EEPROM_readLong(EE_posAxis2);  targetAxis2.part.m=posAxis2; targetAxis2.part.f=0;
        trueAxis1=EEPROM_readLong(EE_trueAxis1);
        trueAxis2=EEPROM_readLong(EE_trueAxis2);

  // if sync anywhere is enabled we have a corrected location, convert to instrument
  // just like we did when we parked
  #ifdef SYNC_ANYWHERE_ON
        posAxis1=((posAxis1-IHS)/1024L)*1024L;
        posAxis2=((posAxis2-IDS)/1024L)*1024L;
  #endif
        sei();

        // see what side of the pier we're on
        pierSide=EEPROM.read(EE_pierSide);
        if (pierSide==PierSideWest) DecDir = DecDirWInit; else DecDir = DecDirEInit;

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

