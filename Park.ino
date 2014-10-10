// -----------------------------------------------------------------------------------
// functions related to Parking the mount 

// sets the park postion as the current position
boolean setPark() {
  if ((parkStatus==NotParked) && (trackingState!=TrackingMoveTo)) {
    lastTrackingState=trackingState;
    trackingState=TrackingNone;
    int lastPECstatus=PECstatus;

    // turn off PEC for a moment
    cli(); long l = PEC_HA; PEC_HA=0; sei();
    
    // SiderealRate (x20 speed), ignore rate ratios - we're working in steps
    cli();
    long LasttimerRateHA =timerRateHA;
    long LasttimerRateDec=timerRateDec;
    timerRateHA =SiderealRate/20L;
    timerRateDec=SiderealRate/20L;
    sei();
    
    // find a park position and store it, the main loop isn't running while we're here
    // so sidereal tracking isn't an issue right now

    // ok, now figure out where to move the HA and Dec to arrive at stepper home positions
    // per Allegro data-sheet for A4983, 1/2 step takes 8 steps to cycle home, 1/4 = 16,
    // ... 1/8 = 32 and 1/16 = 64 (*2 to handle up to 32 uStep drivers)
    cli();
    targetHA =(posHA / 128L) * 128L;
    targetDec=(posDec / 128L) * 128L;
    sei();

    // let it settle into a fixed position, 20 times the sidereal rate at 12 steps per second minimum... 
    // 128/240=0.53S, worst case
    delay(2000);

    // reality check, we should be at the park position
    if ((targetHA!=posHA) || (targetDec!=posDec)) { trackingState=lastTrackingState; return false;  }
    // reality check, we shouldn't have any backlash remaining
    if ((!parkClearBacklash())) { trackingState=lastTrackingState; return false; } 

    // store our position
    EEPROM_writeQuad(EE_posHA ,(byte*)&posHA);
    EEPROM_writeQuad(EE_posDec,(byte*)&posDec);

    // and the align
    saveAlignModel();

    // and remember what side of the pier we're on
    EEPROM.write(EE_pierSide,pierSide);
    parkSaved=true;
    EEPROM.write(EE_parkSaved,parkSaved);

    // move at the previous speed
    cli();
    timerRateHA =LasttimerRateHA;
    timerRateDec=LasttimerRateDec;
    sei();

    trackingState=lastTrackingState;

    // turn PEC back on
    cli(); PEC_HA=l; sei();
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

  // SiderealRate (x10 speed), ignore rate ratios for the moment - we're working in steps
  cli();
  long LasttimerRateHA =timerRateHA;
  long LasttimerRateDec=timerRateDec;
  timerRateHA =SiderealRate/10L;
  timerRateDec=SiderealRate/10L;
  sei();

  // figure out how long we'll have to wait at 10x speed for the backlash to clear (+10%)
  long t; if (backlashHA>backlashDec) t=(((long)backlashHA*110L)/(long)StepsPerSecond); else t=(((long)backlashDec*110L)/(long)StepsPerSecond); 

  // start by moving fully into the backlash
  cli();
  targetHA =targetHA  + backlashHA;
  targetDec=targetDec + backlashDec;
  sei();
  delay(t);

  // then reverse direction and take it all up
  cli();
  targetHA =targetHA  - backlashHA;
  targetDec=targetDec - backlashDec;
  sei();
  delay(t);
  
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
        PECstatus=IgnorePEC;
        cli(); PEC_HA=0; sei(); 
        PEC_SKIP = 0;
  
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
        goTo(tempHA,tempDec,gotoPierSide);

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
        // get our position
        cli();
        EEPROM_readQuad(EE_posHA,(byte*)&posHA);   targetHA=posHA;
        EEPROM_readQuad(EE_posDec,(byte*)&posDec); targetDec=posDec;
        sei();
  
        // get corrections
        EEPROM_readQuad(EE_doCor,(byte*)&doCor);
        EEPROM_readQuad(EE_pdCor,(byte*)&pdCor);
        EEPROM_readQuad(EE_altCor,(byte*)&altCor);
        EEPROM_readQuad(EE_azmCor,(byte*)&azmCor);
        EEPROM_readQuad(EE_IH,(byte*)&IH);
        EEPROM_readQuad(EE_ID,(byte*)&ID);

        azmCor=-azmCor;
        
        // see what side of the pier we're on
        pierSide=EEPROM.read(EE_pierSide);
        if (pierSide==PierSideWest) DecDir = DecDirWInit; else DecDir = DecDirEInit;
        
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


