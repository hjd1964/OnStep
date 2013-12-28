// -----------------------------------------------------------------------------------
// functions related to Homing the mount

// moves telescope to the home position, then stops tracking... I could do a motor sleep here
// if the sleep pin of the BED's were wired up.  I guess the BED's will remember their micro-step position from sleep?
// anyway, the scope will still know where it's pointed and a call to ":MS#" etc. will start it up
boolean goHome() {
  if (trackingState!=TrackingMoveTo) {
    cli();
    startHA =posHA;
    startDec=posDec;
    if (pierSide==PierSideWest) targetHA=-90L*StepsPerDegreeHA; else targetHA=90L*StepsPerDegreeHA;
    targetDec=90L*StepsPerDegreeDec;
    sei();
    
    pierSide         = PierSideNone;
    lastTrackingState= TrackingNone;
    
    trackingState    = TrackingMoveTo;
    homeMount        = true;
  
    return true;
  } else return false;
}

// sets telescope home position; user manually moves to Hour Angle 90 and Declination 90 (CWD position),
// then the first gotoEqu will set the pier side and turn on tracking
boolean setHome() {
  // default values for state variables
  pierSide            = PierSideNone;
  dirDec              = 1;
  DecDir              = DecDirEInit;
  HADir               = HADirEInit;
  dirHA               = 1;
  newTargetRA         = 0;        
  newTargetDec        = 0;
  newTargetAlt        = 0;
  newTargetAzm        = 0;
  origTargetHA        = 0;
  origTargetDec       = 0;
  
  // reset pointing model
  alignMode           = AlignNone;
  altCor              = 0; 
  azmCor              = 0;
  IH                  = 0;
  ID                  = 0;
  
  // where we are
  homeMount           = false;
  atHome              = true;

  // reset tracking and rates
  trackingState       = TrackingNone;
  lastTrackingState   = TrackingNone;
  skipCountHA         = SiderealRate;
  #ifdef DEC_RATIO_ON
  skipCountDec        =SiderealRate*SkipCountRateRatio;
  #else
  skipCountDec        =SiderealRate;
  #endif

  // not parked, but don't wipe the park position if it's saved - we can still use it
  parkStatus          = NotParked;
  EEPROM.write(EE_parkStatus,parkStatus);
  
  // reset PEC  
  PECstatus           = IgnorePEC;
  PECrecorded         = false;
  EEPROM.write(EE_PECstatus,PECstatus);
  EEPROM.write(EE_PECrecorded,PECrecorded);

  // the polar home position
  startHA             = 90L*StepsPerDegreeHA;
  startDec            = 90L*StepsPerDegreeDec;

  cli();
  targetHA            = startHA;
  posHA               = startHA;
  PEC_HA              = 0;
  targetDec           = startDec;
  posDec              = startDec;
  sei();

  return true;
}
