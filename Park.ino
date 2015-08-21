// -----------------------------------------------------------------------------------
// functions related to Parking the mount

// sets the park postion as the current position
boolean setPark() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    lastTrackingState=trackingState;
    trackingState=TrackingNone;

    // don't worry about moving around: during parking pec is turned off and backlash is cleared (0) so that targetHA/Dec=posHA/Dec
    // this should handle getting us back to the home position for micro-step modes up to 256X
    long int h=((long int)targetHA.part.m/1024L)*1024L;
    long int d=((long int)targetDec.part.m/1024L)*1024L;

    // store our position
    EEPROM_writeQuad(EE_posHA ,(byte*)&h);
    EEPROM_writeQuad(EE_posDec,(byte*)&d);

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
  EEPROM_writeQuad(EE_doCor,(byte*)&doCor);
  EEPROM_writeQuad(EE_pdCor,(byte*)&pdCor);
  EEPROM_writeQuad(EE_altCor,(byte*)&altCor);
  EEPROM_writeQuad(EE_azmCor,(byte*)&azmCor);
  EEPROM_writeQuad(EE_IH,(byte*)&IH);
  EEPROM_writeQuad(EE_ID,(byte*)&ID);
  return true;
}

// takes up backlash and returns to the current position
boolean parkClearBacklash() {

  // backlash takeup rate
  cli();
  long LasttimerRateHA =timerRateHA;
  long LasttimerRateDec=timerRateDec;
  timerRateHA =timerRateBacklashHA;
  timerRateDec=timerRateBacklashDec;
  sei();

  // figure out how long we'll have to wait for the backlash to clear (+50%)
  long t; if (backlashHA>backlashDec) t=((long)backlashHA*1500)/(long)StepsPerSecond; else t=((long)backlashDec*1500)/(long)StepsPerSecond;
  t=t/BacklashTakeupRate+250;

  // start by moving fully into the backlash
  cli();
  targetHA.part.m += backlashHA;
  targetDec.part.m += backlashDec;
  sei();
  delay(t);

  // then reverse direction and take it all up
  cli();
  targetHA.part.m  -= backlashHA;
  targetDec.part.m -= backlashDec;
  sei();
  delay(t*2); // if sitting on the opposite side of the backlash, it might take twice as long to clear it
  // we arrive back at the exact same position so fTargetHA/Dec don't need to be touched
  
  // move at the previous speed
  cli();
  timerRateHA =LasttimerRateHA;
  timerRateDec=LasttimerRateDec;
  sei();
  
  // return true on success
  if ((blHA!=0) || (blDec!=0)) return false; else return true;
}

// moves the telescope to the park position, stops tracking
byte park() {
  // Gets park position and moves the mount there
  if (trackingState!=TrackingMoveTo) {
    parkSaved=EEPROM.read(EE_parkSaved);
    if (parkStatus==NotParked) {
      if (parkSaved) {
        // stop tracking, we're shutting down
        trackingState=TrackingNone; lastTrackingState=TrackingNone;

        // turn off the PEC while we park
        disablePec();
        PECstatus=IgnorePEC;
  
        // record our status
        parkStatus=Parking;
        EEPROM.write(EE_parkStatus,parkStatus);
        
        // get the position we're supposed to park at
        cli();
        long tempHA;  EEPROM_readQuad(EE_posHA,(byte*)&tempHA);
        long tempDec; EEPROM_readQuad(EE_posDec,(byte*)&tempDec);
        sei();
        
        // now, slew to this target HA,Dec
        byte gotoPierSide=EEPROM.read(EE_pierSide);
        goTo(tempHA,tempDec,tempHA,tempDec,gotoPierSide);

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
        digitalWrite(HA_EN,HA_Enabled);
        digitalWrite(DE_EN,DE_Enabled);
        delay(10);

        // get our position
        cli();
        EEPROM_readQuad(EE_posHA,(byte*)&posHA);   targetHA.part.m=posHA; targetHA.part.f=0;
        EEPROM_readQuad(EE_posDec,(byte*)&posDec); targetDec.part.m=posDec; targetDec.part.f=0;
        sei();
  
        // get corrections
        EEPROM_readQuad(EE_doCor,(byte*)&doCor);
        EEPROM_readQuad(EE_pdCor,(byte*)&pdCor);
        EEPROM_readQuad(EE_altCor,(byte*)&altCor);
        EEPROM_readQuad(EE_azmCor,(byte*)&azmCor);
        EEPROM_readQuad(EE_IH,(byte*)&IH);
        IHS=IH*15.0*StepsPerDegreeHA;
        EEPROM_readQuad(EE_ID,(byte*)&ID);

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
        
        return true;
      };
    };
  };
  return false;
}


