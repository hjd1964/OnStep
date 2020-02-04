// -----------------------------------------------------------------------------------
// Functions for initializing pins, variables, and timers on startup

#define DEBUG_AXIS_MODES_OFF

void initGeneralError() {
  switch (generalError) {
    case ERR_ALT_MIN:
    case ERR_LIMIT_SENSE:
    case ERR_DEC:
    case ERR_AZM:
    case ERR_UNDER_POLE:
    case ERR_MERIDIAN:
    case ERR_SYNC:
    case ERR_ALT_MAX:
    case ERR_PARK: generalError=ERR_NONE; break;
    default: break;
  }
}

void initPre() {
  // initialize and disable the main axes stepper drivers
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE);
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE);
}

void initStartupValues() {
  // Basic stepper driver mode setup
  // if we made through validation and AXIS1_DRIVER_MODEL exists; AXIS2_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS,
  // and AXIS2_DRIVER_MICROSTEPS also exist and passed validation in the pre-processor
#if AXIS1_DRIVER_MODEL != OFF
  // translate microsteps to microstep bit code
  AXIS1_DRIVER_CODE = translateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS);
  AXIS2_DRIVER_CODE = translateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS);
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    AXIS1_DRIVER_CODE_GOTO = translateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS_GOTO);
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    AXIS2_DRIVER_CODE_GOTO = translateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS_GOTO);
  #endif
#endif
#if AXIS3_DRIVER_MODEL != OFF
  AXIS3_DRIVER_CODE = translateMicrosteps(3, AXIS3_DRIVER_MODEL, AXIS3_DRIVER_MICROSTEPS);
#endif
#if AXIS4_DRIVER_MODEL != OFF
  AXIS4_DRIVER_CODE = translateMicrosteps(4, AXIS4_DRIVER_MODEL, AXIS4_DRIVER_MICROSTEPS);
#endif
#if AXIS5_DRIVER_MODEL != OFF
  AXIS5_DRIVER_CODE = translateMicrosteps(5, AXIS5_DRIVER_MODEL, AXIS5_DRIVER_MICROSTEPS);
#endif

  // initialize some fixed-point values
  amountGuideAxis1.fixed=0;
  amountGuideAxis2.fixed=0;
  guideAxis1.fixed=0;
  guideAxis2.fixed=0;
  accPecGuideHA.fixed=0;
  fstepAxis1.fixed=0;
  fstepAxis2.fixed=0;
  origTargetAxis1.fixed = 0;
  cli();
  targetAxis1.part.m = 0; targetAxis1.part.f = 0;
  posAxis1           = 0;
  targetAxis2.part.m = 0; targetAxis2.part.f = 0;
  posAxis2           = 0;
  sei();

  // default values for state variables
  pierSideControl     = PierSideNone;
  dirAxis1            = 1;
  dirAxis2            = 1;
  defaultDirAxis2     = defaultDirAxis2EInit;
  if (latitude >= 0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
  newTargetRA         = 0;        
  newTargetDec        = 0;
  newTargetAlt        = 0;
  newTargetAzm        = 0;
  origTargetAxis1.fixed = 0;
  origTargetAxis2       = 0;

  // initialize alignment
  alignNumStars       = 0;
  alignThisStar       = 0;
  indexAxis1          = 0;
  indexAxis1Steps     = 0;
  indexAxis2          = 0;
  indexAxis2Steps     = 0;
  Align.init();

   // reset meridian flip control
  #if MOUNT_TYPE == GEM
    meridianFlip = MeridianFlipAlways;
  #endif
  #if MOUNT_TYPE == FORK
    meridianFlip = MeridianFlipNever;
  #endif
  #if MOUNT_TYPE == ALTAZM
    meridianFlip = MeridianFlipNever;
  #endif

  // clear errors that are no-longer relevant after init
  initGeneralError();

  // where we are
  homeMount           = false;
  atHome              = true;
  waitingHome         = false;
  waitingHomeContinue = false;

  // PEC sanity check
  if (pecBufferSize < 0 || pecBufferSize > 3384) pecBufferSize=0;
  if (SecondsPerWormRotationAxis1>pecBufferSize) SecondsPerWormRotationAxis1=pecBufferSize;

  // reset tracking and rates
  cli();
  trackingState       = TrackingNone;
  lastTrackingState   = TrackingNone;
  timerRateAxis1      = SiderealRate;
  timerRateAxis2      = SiderealRate;
  sei();
}

void initPins() {
// ------------------------------------------------------------------
// Pull the Axis1/2 RST Pin HIGH on the MaxESP2

#if PINMAP == MaxESP2
  pinMode(Axis1_M3,INPUT_PULLUP);
#endif

// ------------------------------------------------------------------
// ESP-01 (ESP8266) firmware flashing control

#if SERIAL_B_ESP_FLASHING == ON
  pinMode(ESP8266Gpio0Pin,OUTPUT);                // ESP8266 GPIO0
  digitalWrite(ESP8266Gpio0Pin,HIGH); delay(20);  // Run mode
  pinMode(ESP8266RstPin,OUTPUT);                  // ESP8266 RST
  digitalWrite(ESP8266RstPin,LOW);  delay(200);   // Reset, if LOW
  digitalWrite(ESP8266RstPin,HIGH);               // Reset, inactive HIGH
#endif

// ------------------------------------------------------------------
// User feedback

#if LED_STATUS == ON
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);  // light status LED (provides GND)
  #ifdef LEDposPin
    pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
  #endif
  ledOn=true;
#endif

#if LED_STATUS >= 0
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);  // light status LED (provides pwm'd GND for polar reticule)
  #ifdef LEDposPin
    pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
  #endif
  analogWrite(LEDnegPin,LED_STATUS);
  ledOn=true;
#endif

#if LED_RETICLE >= 0
  pinMode(ReticlePin,OUTPUT); analogWrite(ReticlePin,reticuleBrightness); // light reticule LED
#endif

#if LED_STATUS2 == ON
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,HIGH); // light second status LED (provides just GND)
  led2On=false;
#elif LED_STATUS2 >= 0
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,LOW); // light second status LED (provides pwm'd GND for polar reticule)
  analogWrite(LEDneg2Pin,LED_STATUS2);
#endif

// ready the sound/buzzer pin
#if BUZZER == ON || BUZZER >= 0
  pinMode(TonePin,OUTPUT);
  digitalWrite(TonePin,LOW);
#endif

// ------------------------------------------------------------------
// Sensors

// Home position sensing
#if HOME_SENSE == ON
  pinMode(Axis1HomePin,INPUT);
  pinMode(Axis2HomePin,INPUT);
#elif HOME_SENSE == ON_PULLUP
  pinMode(Axis1HomePin,INPUT_PULLUP);
  pinMode(Axis2HomePin,INPUT_PULLUP);
#elif HOME_SENSE == ON_PULLDOWN
  pinMode(Axis1HomePin,INPUT_PULLDOWN);
  pinMode(Axis2HomePin,INPUT_PULLDOWN);
#endif

// limit switch sense
#if LIMIT_SENSE == ON
  pinMode(LimitPin,INPUT);
#elif LIMIT_SENSE == ON_PULLUP
  pinMode(LimitPin,INPUT_PULLUP);
#elif LIMIT_SENSE == ON_PULLDOWN
  pinMode(LimitPin,INPUT_PULLDOWN);
#endif

// PEC index sense
#if PEC_SENSE == ON
  pinMode(PecPin,INPUT);
#elif PEC_SENSE == ON_PULLUP
  pinMode(PecPin,INPUT_PULLUP);
#elif PEC_SENSE == ON_PULLDOWN
  pinMode(PecPin,INPUT_PULLDOWN);
#endif

// Pulse per second
#if PPS_SENSE == ON
  pinMode(PpsPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#elif PPS_SENSE == ON_PULLUP
  pinMode(PpsPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#elif PPS_SENSE == ON_PULLDOWN
  pinMode(PpsPin,INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#endif

// ------------------------------------------------------------------
// Stepper driver control

  pinMode(Axis1StepPin,OUTPUT);   // Axis1
  pinMode(Axis1DirPin,OUTPUT); 
  pinMode(Axis2StepPin,OUTPUT);   // Axis2 
  pinMode(Axis2DirPin,OUTPUT); 

  // provide 5V (or 3.3V) power to stepper drivers if requested (classic Pin-map)
#ifdef POWER_SUPPLY_PINS_ON
  pinMode(Axis15vPin,OUTPUT);
  digitalWrite(Axis15vPin,HIGH);
  pinMode(Axis25vPin,OUTPUT);
  digitalWrite(Axis25vPin,HIGH);
#endif
  // provide Gnd on next to the Dec stepper pins if requested (classic Pin-map)
#ifdef Axis2GndPin
  pinMode(Axis2GndPin,OUTPUT);
  digitalWrite(Axis2GndPin,LOW);
#endif

// inputs for stepper drivers fault signal
#if AXIS1_DRIVER_STATUS == LOW
  pinMode(Axis1FaultPin,INPUT_PULLUP);
#elif AXIS1_DRIVER_STATUS == HIGH
  #ifdef INPUT_PULLDOWN
    pinMode(Axis1FaultPin,INPUT_PULLDOWN);
  #else
    pinMode(Axis1FaultPin,INPUT);
  #endif
#endif

#if AXIS2_DRIVER_STATUS == LOW
  pinMode(Axis2FaultPin,INPUT_PULLUP);
#elif AXIS1_DRIVER_STATUS == HIGH
  #ifdef INPUT_PULLDOWN
    pinMode(Axis2FaultPin,INPUT_PULLDOWN);
  #else
    pinMode(Axis2FaultPin,INPUT);
  #endif
#endif

  StepperModeTrackingInit();
}

void initReadNvValues() {
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=nv.read(EE_currentSite); if (currentSite > 3) currentSite=0; // site index is valid?
  setLatitude(nv.readFloat(EE_sites+(currentSite)*25+0));
  longitude=nv.readFloat(EE_sites+(currentSite)*25+4);
  InitStartPosition();

  timeZone=nv.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  nv.readString(EE_sites+(currentSite)*25+9,siteName);

  // get date and time from EEPROM, start keeping time
  JD=nv.readFloat(EE_JD);
  LMT=nv.readFloat(EE_LMT);

  // read the date/time from TLS (if present)
  if (tls.active) {
    tls.get(JD,LMT);
    dateWasSet=true; timeWasSet=true;
  }

  UT1=LMT+timeZone;
  updateLST(jd2last(JD,UT1,false));

  // get the degrees past meridian east/west
#if MOUNT_TYPE == GEM
  int i=round(nv.read(EE_dpmE)-128);
  if (i > 60) i=((i-90)*2)+60; else if (i < -60) i=((i+90)*2)-60;
  degreesPastMeridianE=i;

  i=round(nv.read(EE_dpmW)-128);
  if (i > 60) i=((i-60)*2)+60; else if (i < -60) i=((i+60)*2)-60;
  degreesPastMeridianW=i;
#endif
  
  // get the min. and max altitude
  minAlt=nv.read(EE_minAlt)-128;
  maxAlt=nv.read(EE_maxAlt);
#if MOUNT_TYPE == ALTAZM
  if (maxAlt > 87) maxAlt=87;
#endif

  // get the backlash amounts
  backlashAxis2=nv.readInt(EE_backlashAxis2);
  backlashAxis1=nv.readInt(EE_backlashAxis1);
  
#if MOUNT_TYPE != ALTAZM
  // get the PEC status
  pecStatus  =nv.read(EE_pecStatus);
  pecRecorded=nv.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;
  for (int i=0; i < pecBufferSize; i++) pecBuffer[i]=nv.read(EE_pecTable+i);
  wormSensePos=nv.readLong(EE_wormSensePos);
  #if PEC_SENSE == OFF
    wormSensePos=0;
    pecStatus=IgnorePEC;
  #endif
#endif

  // get the Park status
  parkSaved=nv.read(EE_parkSaved);
  parkStatus=nv.read(EE_parkStatus);
  // tried to park but crashed?
  if (parkStatus == Parking) { parkStatus=ParkFailed; nv.write(EE_parkStatus,parkStatus); }

  // get the pulse-guide rate
  currentPulseGuideRate=nv.read(EE_pulseGuideRate); if (currentPulseGuideRate > GuideRate1x) currentPulseGuideRate=GuideRate1x;

  // set the default MaxRate based on the desired goto speed
  MaxRateDef=MaxRate;
  if (MaxRateDef < maxRateLowerLimit()/16.0) MaxRateDef=maxRateLowerLimit()/16.0;

  // get the max goto rate
  maxRate=(int16_t)nv.readInt(EE_maxRate)*16; // maxRate is in 16MHz clocks but stored in micro-seconds
  // check for flag that maxRate is stored in EE_maxRateL, if not move it there
  if (maxRate == -16) maxRate=nv.readLong(EE_maxRateL); else { nv.writeInt(EE_maxRate,-1); nv.writeLong(EE_maxRateL,maxRate); }
  // constrain values to the limits (1/2 to 2X the MaxRateDef) and platform limits
  if (maxRate < (double)MaxRateDef*8.0) maxRate=(double)MaxRateDef*8.0;
  if (maxRate > (double)MaxRateDef*32.0) maxRate=(double)MaxRateDef*32.0;
  if (maxRate < maxRateLowerLimit()) maxRate=maxRateLowerLimit();
  
#if SLEW_RATE_MEMORY == OFF
  if (maxRate != (long)((double)MaxRateDef*16.0)) {
    maxRate=(double)MaxRateDef*16.0; 
    nv.writeLong(EE_maxRateL,maxRate);
  }
#endif
  setAccelerationRates(maxRate); // set the new acceleration rate

  // get autoMeridianFlip
#if MOUNT_TYPE == GEM && MFLIP_AUTOMATIC_MEMORY == ON
  autoMeridianFlip=nv.read(EE_autoMeridianFlip);
#endif

  // get meridian flip pause at home
#if MOUNT_TYPE == GEM && MFLIP_PAUSE_HOME_MEMORY == ON
  pauseHome=nv.read(EE_pauseHome);
#endif

  // set the default guide rate
  setGuideRate(GuideRateDefault);
  enableGuideRate(GuideRateDefault);

}

// the polar home position
void InitStartPosition() {
  startAxis1 = 0;
  startAxis2 = 0;
  cli();
  targetAxis1.part.m = 0; targetAxis1.part.f = 0;
  posAxis1           = 0;
  targetAxis2.part.m = 0; targetAxis2.part.f = 0;
  posAxis2           = 0;
  blAxis1            = 0;
  blAxis2            = 0;
  sei();
  setIndexAxis1(homePositionAxis1,PierSideEast);
  setIndexAxis2(homePositionAxis2,PierSideEast);
}

void initWriteNvValues() {
  // EEPROM automatic initialization
  if (NV_INIT_KEY_RESET) nv.writeLong(EE_autoInitKey,0);
  if (nv.readLong(EE_autoInitKey) != NV_INIT_KEY) {
    for (int i=0; i<200; i++) { 
      #ifdef HAL_SERIAL_TRANSMIT
        SerialA.transmit();
      #endif
      delay(10);
    }
    
    // init the whole nv memory
    for (int i=0; i<E2END; i++) nv.write(i,0);
    
    // init the site information, lat/long/tz/name
    nv.write(EE_currentSite,0);
    latitude=0; longitude=0;
    for (int l=0; l < 4; l++) {
      nv.writeFloat(EE_sites+(l)*25+0,latitude);
      nv.writeFloat(EE_sites+(l)*25+4,longitude);
      nv.write(EE_sites+(l)*25+8,128);
      nv.write(EE_sites+(l)*25+9,0);
    }
  
    // init the date and time January 1, 2013. 0 hours LMT
    JD=CompilerDateToJulian();
    LMT=0.0;
    nv.writeFloat(EE_JD,JD);
    nv.writeFloat(EE_LMT,LMT);
  
    // init the degrees past meridian east/west
    nv.write(EE_dpmE,round(AXIS1_LIMIT_MERIDIAN_E+128));
    nv.write(EE_dpmW,round(AXIS1_LIMIT_MERIDIAN_W+128));

    // init the min and max altitude
    minAlt=-10;
    maxAlt=80;
    nv.write(EE_minAlt,minAlt+128);
    nv.write(EE_maxAlt,maxAlt);
  
    // init (clear) the backlash amounts
    nv.writeInt(EE_backlashAxis2,0);
    nv.writeInt(EE_backlashAxis1,0);
  
    // init the PEC status, clear the index and buffer
    nv.write(EE_pecStatus,IgnorePEC);
    nv.write(EE_pecRecorded,false);
    for (int l=0; l < pecBufferSize; l++) nv.write(EE_pecTable+l,128);
    wormSensePos=0;
    nv.writeLong(EE_wormSensePos,wormSensePos);
    
    // init the Park status
    nv.write(EE_parkSaved,false);
    nv.write(EE_parkStatus,NotParked);
  
    // init the pulse-guide rate
    nv.write(EE_pulseGuideRate,GuideRate1x);
    
    // init the default maxRate
    if (maxRate < 2L*16L) maxRate=2L*16L; 
    if (maxRate > 10000L*16L) maxRate=10000L*16L;
    if (maxRate<maxRateLowerLimit()) maxRate=maxRateLowerLimit();
    nv.writeInt(EE_maxRate,-1); nv.writeLong(EE_maxRateL,maxRate);

    // init autoMeridianFlip
    nv.write(EE_autoMeridianFlip,autoMeridianFlip);

    // init the sidereal tracking rate, use this once - then issue the T+ and T- commands to fine tune
    // 1/16uS resolution timer, ticks per sidereal second
    nv.writeLong(EE_siderealInterval,siderealInterval);

    // set default focuser positions at zero
    nv.writeLong(EE_posAxis4,0L);
    nv.writeLong(EE_posAxis5,0L);
    // for DC focusers read in the % power
    nv.write(EE_dcPwrAxis4,50);
    nv.write(EE_dcPwrAxis5,50);
    // clear focuser TCF values
    nv.writeFloat(EE_tcfCoefAxis4,0.0);
    nv.writeFloat(EE_tcfCoefAxis5,0.0);
    nv.write(EE_tcfEnAxis4,0);
    nv.write(EE_tcfEnAxis5,0);

    // clear the library/catalogs
    Lib.clearAll();
    
    // clear the pointing model
    saveAlignModel();

    // finally, stop the init from happening again
    nv.writeLong(EE_autoInitKey,NV_INIT_KEY);
  }
}

void initStartTimers() {
  // Initialize the timers that handle the sidereal clock, RA, and Dec
  HAL_Init_Timer_Sidereal();

  // wait for the sidereal clock to tick
  delay(15);

  // Initialize Axis1 and Axis2 motor timers and set their priorities
  HAL_Init_Timers_Motor();
}
