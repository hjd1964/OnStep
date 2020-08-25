// -----------------------------------------------------------------------------------
// Functions for initializing pins, variables, and timers on startup

void initPre() {
  // initialize and disable the main axes stepper drivers
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE);
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE);
}

void initPins() {
  // Allow a given pin to supply power to devices
#ifdef GeneralPurposePowerPin1
  pinMode(GeneralPurposePowerPin1,OUTPUT);
  digitalWrite(GeneralPurposePowerPin1,HIGH);
#endif

#ifdef GeneralPurposePowerPin2
  pinMode(GeneralPurposePowerPin2,OUTPUT);
  digitalWrite(GeneralPurposePowerPin2,HIGH);
#endif

  // Pull the Axis1/2 RST Pin HIGH on the MaxESP2
#if PINMAP == MaxESP2
  pinMode(Axis1_M3,INPUT_PULLUP);
#endif

  // ESP-01 (ESP8266) firmware flashing control
#if SERIAL_B_ESP_FLASHING == ON
  pinMode(ESP8266Gpio0Pin,OUTPUT);                // ESP8266 GPIO0
  digitalWrite(ESP8266Gpio0Pin,HIGH); delay(20);  // Run mode
  pinMode(ESP8266RstPin,OUTPUT);                  // ESP8266 RST
  digitalWrite(ESP8266RstPin,LOW);  delay(200);   // Reset, if LOW
  digitalWrite(ESP8266RstPin,HIGH);               // Reset, inactive HIGH
#endif

  // light status LED (provides GND)
#if LED_STATUS == ON
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);
  #ifdef LEDposPin
    pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
  #endif
  ledOn=true;
#endif

  // light status LED (provides pwm'd GND for polar reticule)
#if LED_STATUS >= 0
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);
  #ifdef LEDposPin
    pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
  #endif
  analogWrite(LEDnegPin,LED_STATUS);
  ledOn=true;
#endif

  // light reticule LED
#if LED_RETICLE >= 0
  pinMode(ReticlePin,OUTPUT); analogWrite(ReticlePin,reticuleBrightness);
#endif

  // light second status LED
#if LED_STATUS2 == ON
  // provides just GND
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,HIGH);
  led2On=false;
#elif LED_STATUS2 >= 0
  // provides pwm'd GND for polar reticule
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,LOW);
  analogWrite(LEDneg2Pin,LED_STATUS2);
#endif

  // ready the sound/buzzer pin
#if BUZZER == ON || BUZZER >= 0
  pinMode(TonePin,OUTPUT);
  digitalWrite(TonePin,LOW);
#endif

  // Home position sensing
#if HOME_SENSE == ON
  pinMode(Axis1_HOME,INPUT);
  pinMode(Axis2_HOME,INPUT);
#elif HOME_SENSE == ON_PULLUP
  pinMode(Axis1_HOME,INPUT_PULLUP);
  pinMode(Axis2_HOME,INPUT_PULLUP);
#elif HOME_SENSE == ON_PULLDOWN
  pinMode(Axis1_HOME,INPUT_PULLDOWN);
  pinMode(Axis2_HOME,INPUT_PULLDOWN);
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
#elif PPS_SENSE == ON_BOTH
  pinMode(PpsPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,CHANGE);
#elif PPS_SENSE == ON_PULLUP
  pinMode(PpsPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#elif PPS_SENSE == ON_PULLDOWN
  pinMode(PpsPin,INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#endif

  // Stepper driver control
  pinMode(Axis1_STEP,OUTPUT);
  pinMode(Axis1_DIR,OUTPUT); 
  pinMode(Axis2_STEP,OUTPUT);
  pinMode(Axis2_DIR,OUTPUT); 

#ifdef POWER_SUPPLY_PINS_ON
  // provide 5V (or 3.3V) power to stepper drivers if requested (classic Pin-map)
  pinMode(Axis15vPin,OUTPUT);
  digitalWrite(Axis15vPin,HIGH);
  pinMode(Axis25vPin,OUTPUT);
  digitalWrite(Axis25vPin,HIGH);
#endif
#ifdef Axis2GndPin
  // provide Gnd on next to the Dec stepper pins if requested (classic Pin-map)
  pinMode(Axis2GndPin,OUTPUT);
  digitalWrite(Axis2GndPin,LOW);
#endif

  // inputs for stepper drivers fault signal
#if AXIS1_DRIVER_STATUS == LOW
  pinMode(Axis1_FAULT,INPUT_PULLUP);
#elif AXIS1_DRIVER_STATUS == HIGH
  #ifdef INPUT_PULLDOWN
    pinMode(Axis1_FAULT,INPUT_PULLDOWN);
  #else
    pinMode(Axis1_FAULT,INPUT);
  #endif
#endif

#if AXIS2_DRIVER_STATUS == LOW
  pinMode(Axis2_FAULT,INPUT_PULLUP);
#elif AXIS1_DRIVER_STATUS == HIGH
  #ifdef INPUT_PULLDOWN
    pinMode(Axis2_FAULT,INPUT_PULLDOWN);
  #else
    pinMode(Axis2_FAULT,INPUT);
  #endif
#endif
}

void initWriteNvValues() {
  // EEPROM automatic initialization
  if (NV_FACTORY_RESET == ON) nv.writeLong(EE_autoInitKey,0);

  if (nv.readLong(EE_autoInitKey) != NV_INIT_KEY) {
    // wipe the whole nv memory
    VF("MSG: Wipe NV "); V(E2END+1); VLF(" Bytes (please wait)");
    for (int i=0; i<E2END; i++) nv.write(i,0);

    VLF("MSG: Init NV to defaults");

    // default stepper driver setup is from Config.h
    nv.write(EE_settingsRuntime,0);

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
    wormSensePos=0;
    nv.writeLong(EE_wormSensePos,wormSensePos);

    // init the Park status
    nv.write(EE_parkSaved,false);
    nv.write(EE_parkStatus,NotParked);

    // init the pulse-guide rate
    nv.write(EE_pulseGuideRate,GuideRate1x);

    // init the default maxRate
    maxRate=((1000000.0/(SLEW_RATE_BASE_DESIRED))/AXIS1_STEPS_PER_DEGREE)*16L;
    if (maxRate < 2L*16L) maxRate=2L*16L;
    if (maxRate > 10000L*16L) maxRate=10000L*16L;
    if (maxRate < maxRateLowerLimit()) maxRate=maxRateLowerLimit();
    nv.writeInt(EE_maxRate,-1); nv.writeLong(EE_maxRateL,maxRate);

    // init autoMeridianFlip
    nv.write(EE_autoMeridianFlip,autoMeridianFlip);

    // init the sidereal tracking rate
    // 1/16uS resolution timer, ticks per sidereal second
    nv.writeLong(EE_siderealInterval,masterSiderealInterval);

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
  
  // bit 0 = settings at compile (0) or run time (1), bits 1 to 5 = (1) to reset axis n on next boot
  int axisReset=nv.read(EE_settingsRuntime);
  if (!(axisReset&0b0000001)) axisReset|=0b0111110; // force reset of all axis settings
  if   (axisReset&0b0000010) { nv.writeBytes(EE_settingsAxis1,(byte*)&axis1Settings,sizeof(axis1Settings)); VLF("MSG: Init NV Axis1 defaults"); }
  if   (axisReset&0b0000100) { nv.writeBytes(EE_settingsAxis2,(byte*)&axis2Settings,sizeof(axis2Settings)); VLF("MSG: Init NV Axis2 defaults"); }
  if   (axisReset&0b0001000) { nv.writeBytes(EE_settingsAxis3,(byte*)&axis3Settings,sizeof(axis3Settings)); VLF("MSG: Init NV Axis3 defaults"); }
  if   (axisReset&0b0010000) { nv.writeBytes(EE_settingsAxis4,(byte*)&axis4Settings,sizeof(axis4Settings)); VLF("MSG: Init NV Axis4 defaults"); }
  if   (axisReset&0b0100000) { nv.writeBytes(EE_settingsAxis5,(byte*)&axis5Settings,sizeof(axis5Settings)); VLF("MSG: Init NV Axis5 defaults"); }
  axisReset&=0b0000001; // clear reset bits of all individual axis settings
  nv.write(EE_settingsRuntime,axisReset);
}

void initReadNvValues() {
  if (E2END < 1023) { generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV size < 1024 bytes"); }

  // get axis settings
  nv.readBytes(EE_settingsAxis1,(byte*)&axis1Settings,sizeof(axis1Settings));
  nv.readBytes(EE_settingsAxis2,(byte*)&axis2Settings,sizeof(axis2Settings));
  nv.readBytes(EE_settingsAxis3,(byte*)&axis3Settings,sizeof(axis3Settings));
  nv.readBytes(EE_settingsAxis4,(byte*)&axis4Settings,sizeof(axis4Settings));
  nv.readBytes(EE_settingsAxis5,(byte*)&axis5Settings,sizeof(axis5Settings));
  if (axis1Settings.IRUN != AXIS1_DRIVER_IRUN) { axis1SettingsEx.IGOTO=axis1Settings.IRUN; axis1SettingsEx.IHOLD=axis1Settings.IRUN/2; }
  if (!validateAxisSettings(1,MOUNT_TYPE==ALTAZM,axis1Settings)) generalError=ERR_NV_INIT;
  if (axis2Settings.IRUN != AXIS2_DRIVER_IRUN) { axis2SettingsEx.IGOTO=axis2Settings.IRUN; axis2SettingsEx.IHOLD=axis2Settings.IRUN/2; }
  if (!validateAxisSettings(2,MOUNT_TYPE==ALTAZM,axis2Settings)) generalError=ERR_NV_INIT;
  if (axis3Settings.IRUN != AXIS3_DRIVER_IRUN) axis3SettingsEx.IHOLD=axis3Settings.IRUN/2;
  if (!validateAxisSettings(3,MOUNT_TYPE==ALTAZM,axis3Settings)) generalError=ERR_NV_INIT;
  if (axis4Settings.IRUN != AXIS4_DRIVER_IRUN) axis4SettingsEx.IHOLD=axis4Settings.IRUN/2;
  if (!validateAxisSettings(4,MOUNT_TYPE==ALTAZM,axis4Settings)) generalError=ERR_NV_INIT;
  if (axis5Settings.IRUN != AXIS5_DRIVER_IRUN) axis5SettingsEx.IHOLD=axis5Settings.IRUN/2;
  if (!validateAxisSettings(5,MOUNT_TYPE==ALTAZM,axis5Settings)) generalError=ERR_NV_INIT;

  timerRateRatio    = axis1Settings.stepsPerMeasure/axis2Settings.stepsPerMeasure;
  useTimerRateRatio = axis1Settings.stepsPerMeasure != axis2Settings.stepsPerMeasure;

  #if AXIS1_DRIVER_MODEL != SERVO && AXIS1_DRIVER_MODEL != SERVO1 && AXIS1_DRIVER_MODEL != SERVO2
    if (AXIS1_DRIVER_MICROSTEPS_GOTO != OFF) axis1StepsGoto = axis1Settings.microsteps/AXIS1_DRIVER_MICROSTEPS_GOTO;
  #else
    if (AXIS1_DRIVER_MICROSTEPS_GOTO != OFF) axis1StepsGoto = AXIS1_DRIVER_MICROSTEPS_GOTO/axis1Settings.microsteps;
  #endif
  #if AXIS2_DRIVER_MODEL != SERVO && AXIS2_DRIVER_MODEL != SERVO1 && AXIS2_DRIVER_MODEL != SERVO2
    if (AXIS2_DRIVER_MICROSTEPS_GOTO != OFF) axis2StepsGoto = axis2Settings.microsteps/AXIS2_DRIVER_MICROSTEPS_GOTO;
  #else
    if (AXIS2_DRIVER_MICROSTEPS_GOTO != OFF) axis2StepsGoto = AXIS2_DRIVER_MICROSTEPS_GOTO/axis2Settings.microsteps;
  #endif
  
  // Basic stepper driver mode setup
  // if we made through validation and AXIS1_DRIVER_MODEL exists; AXIS2_DRIVER_MODEL, axis1Settings.microsteps,
  // and axis2Settings.microsteps also exist and passed validation in the pre-processor
#if AXIS1_DRIVER_MODEL != OFF
  // translate microsteps to microstep bit code
  AXIS1_DRIVER_CODE = translateMicrosteps(AXIS1_DRIVER_MODEL, axis1Settings.microsteps);
  AXIS2_DRIVER_CODE = translateMicrosteps(AXIS2_DRIVER_MODEL, axis2Settings.microsteps);
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    AXIS1_DRIVER_CODE_GOTO = translateMicrosteps(AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS_GOTO);
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    AXIS2_DRIVER_CODE_GOTO = translateMicrosteps(AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS_GOTO);
  #endif
#endif
#if AXIS3_DRIVER_MODEL != OFF
  AXIS3_DRIVER_CODE = translateMicrosteps(AXIS3_DRIVER_MODEL, axis3Settings.microsteps);
#endif
#if AXIS4_DRIVER_MODEL != OFF
  AXIS4_DRIVER_CODE = translateMicrosteps(AXIS4_DRIVER_MODEL, axis4Settings.microsteps);
#endif
#if AXIS5_DRIVER_MODEL != OFF
  AXIS5_DRIVER_CODE = translateMicrosteps(AXIS5_DRIVER_MODEL, axis5Settings.microsteps);
#endif

  // get the site information, if a GPS were attached we would use that here instead
  currentSite=nv.read(EE_currentSite);
  if (currentSite > 3) { currentSite=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV currentSite"); }

  double f=nv.readFloat(EE_sites+currentSite*25+0);
  if (f < -90 || f > 90) { f=0.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV latitude"); }
  setLatitude(f);
  longitude=nv.readFloat(EE_sites+currentSite*25+4);
  if (longitude < -360 || longitude > 360) { longitude=0.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV longitude"); }

  // get date and time from EEPROM, start keeping time
  timeZone=nv.read(EE_sites+currentSite*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  if (timeZone < -12 || timeZone > 14) { timeZone=0.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV timeZone"); }
  nv.readString(EE_sites+currentSite*25+9,siteName);

  JD=nv.readFloat(EE_JD);
  LMT=nv.readFloat(EE_LMT);
  if (tls.active) { 
    tls.get(JD,LMT); // read the date/time from TLS (if present)
    dateWasSet=true; timeWasSet=true;
  }
  if (JD < 2451544.5 || JD > 2816787.5) JD=2451544.5; // valid date?
  if (LMT < 0 || LMT > 24) { LMT=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV LMT"); }

  UT1=LMT+timeZone;
  updateLST(jd2last(JD,UT1,false));

  // get the degrees past meridian east/west
#if MOUNT_TYPE == GEM
  int i=round(nv.read(EE_dpmE)-128);
  if (i > 60) i=((i-90)*2)+60; else if (i < -60) i=((i+90)*2)-60;
  degreesPastMeridianE=i;
  if (degreesPastMeridianE < -180 || degreesPastMeridianE > 180) { degreesPastMeridianE=0.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV degreesPastMeridianE"); }

  i=round(nv.read(EE_dpmW)-128);
  if (i > 60) i=((i-60)*2)+60; else if (i < -60) i=((i+60)*2)-60;
  degreesPastMeridianW=i;
  if (degreesPastMeridianW < -180 || degreesPastMeridianW > 180) { degreesPastMeridianW=0.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV degreesPastMeridianW"); }
#endif
  
  // get the min. and max altitude
  minAlt=nv.read(EE_minAlt)-128;
  if (minAlt < -30 || minAlt > 30) { minAlt=-10.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV minAlt"); }
  maxAlt=nv.read(EE_maxAlt);
#if MOUNT_TYPE == ALTAZM
  if (maxAlt > 87) maxAlt=87;
#endif
  if (maxAlt < 60 || maxAlt > 90) { maxAlt=80.0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV maxAlt"); }

  // get the backlash amounts
  backlashAxis1=nv.readInt(EE_backlashAxis1);
  if (backlashAxis1 < 0 ) { backlashAxis1=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV backlashAxis1"); }
  backlashAxis2=nv.readInt(EE_backlashAxis2);
  if (backlashAxis2 < 0 ) { backlashAxis2=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV backlashAxis2"); }
  
  // setup PEC and get data
  stepsPerWormRotationAxis1=AXIS1_STEPS_PER_WORMROT*(axis1Settings.stepsPerMeasure/AXIS1_STEPS_PER_DEGREE);
  secondsPerWormRotationAxis1=stepsPerWormRotationAxis1/stepsPerSecondAxis1;
  if (MOUNT_TYPE == ALTAZM) pecBufferSize=0; else pecBufferSize=ceil(stepsPerWormRotationAxis1/(axis1Settings.stepsPerMeasure/240.0));
  if (pecBufferSize != 0) {
    if (pecBufferSize < 61) { pecBufferSize=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): invalid pecBufferSize, PEC disabled"); }
    if (200+pecBufferSize >= E2END-200) { pecBufferSize=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): pecBufferSize exceeds available NV, PEC disabled"); }
  }
  if (secondsPerWormRotationAxis1 > pecBufferSize) secondsPerWormRotationAxis1=pecBufferSize;

#if MOUNT_TYPE != ALTAZM
  createPecBuffer();
  bool pecBufferNeedsInit=true;
  for (int i=0; i < pecBufferSize; i++) { pecBuffer[i]=nv.read(EE_pecTable+i); if (pecBuffer[i] != 0) pecBufferNeedsInit=false; }
  if (pecBufferNeedsInit) for (int l=0; l < pecBufferSize; l++) nv.write(EE_pecTable+l,128);
  wormSensePos=nv.readLong(EE_wormSensePos); // validation of this value is not useful
  #if PEC_SENSE == OFF
    wormSensePos=0;
    pecStatus=IgnorePEC;
  #endif

  pecStatus  =nv.read(EE_pecStatus);
  if (pecStatus < PEC_STATUS_FIRST || pecStatus > PEC_STATUS_LAST) { pecStatus=IgnorePEC; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV pecStatus"); }
  pecRecorded=nv.read(EE_pecRecorded);
  if (pecRecorded != true && pecRecorded != false) { pecRecorded=false; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV pecRecorded"); }
  if (!pecRecorded) pecStatus=IgnorePEC;
#endif
  
  // get the Park status
  parkSaved=nv.read(EE_parkSaved);
  if (parkSaved != true && parkSaved != false) { parkSaved=false; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV parkSaved"); }
  parkStatus=nv.read(EE_parkStatus);
  if (parkStatus < PARK_STATUS_FIRST || parkStatus > PARK_STATUS_LAST) { parkStatus=NotParked; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV parkStatus"); }
  // tried to park but crashed?
  if (parkStatus == Parking) { parkStatus=ParkFailed; nv.write(EE_parkStatus,parkStatus); }

  // get the pulse-guide rate
  currentPulseGuideRate=nv.read(EE_pulseGuideRate);
  if (currentPulseGuideRate < 0) { currentPulseGuideRate=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV currentPulseGuideRate"); }
  if (currentPulseGuideRate > GuideRate1x) { currentPulseGuideRate=GuideRate1x; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV currentPulseGuideRate"); }

  // set the default MaxRate based on the desired goto speed
  maxRateBaseActual=maxRateBaseDesired;
  if (maxRateBaseActual < maxRateLowerLimit()/8.0) maxRateBaseActual=maxRateLowerLimit()/8.0;
  if (maxRateBaseActual > 1000000.0) maxRateBaseActual=1000000.0;

  // get the max goto rate
  maxRate=(int16_t)nv.readInt(EE_maxRate)*16; // maxRate is in 16MHz clocks but stored in micro-seconds
  // check for flag that maxRate is stored in EE_maxRateL, if not move it there
  if (maxRate == -16) maxRate=nv.readLong(EE_maxRateL); else { nv.writeInt(EE_maxRate,-1); nv.writeLong(EE_maxRateL,maxRate); }
  // constrain values to the limits (1/2 to 2X the maxRateBaseActual) and platform limits
  if (maxRate < (long)(maxRateBaseActual*8.0))  { maxRate=maxRateBaseActual*8.0; DLF("WRN, initReadNvValues(): NV maxRate (too low)"); }
  if (maxRate > (long)(maxRateBaseActual*32.0)) { maxRate=maxRateBaseActual*32.0; DLF("WRN, initReadNvValues(): NV maxRate (too high)"); }
  if (maxRate < maxRateLowerLimit()) maxRate=maxRateLowerLimit();
  
#if SLEW_RATE_MEMORY == OFF
  if (maxRate != (long)(maxRateBaseActual*16.0)) { maxRate=maxRateBaseActual*16.0; nv.writeLong(EE_maxRateL,maxRate); }
#endif

  // set the new acceleration rate
  setAccelerationRates(maxRate);

  // get autoMeridianFlip
#if MOUNT_TYPE == GEM && MFLIP_AUTOMATIC_MEMORY == ON
  autoMeridianFlip=nv.read(EE_autoMeridianFlip);
  if (autoMeridianFlip != 1 && autoMeridianFlip != 0) { autoMeridianFlip=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV autoMeridianFlip"); }
#endif

  // get meridian flip pause at home
#if MOUNT_TYPE == GEM && MFLIP_PAUSE_HOME_MEMORY == ON
  pauseHome=nv.read(EE_pauseHome);
  if (pauseHome != 1 && pauseHome != 0) { pauseHome=0; generalError=ERR_NV_INIT; DLF("ERR, initReadNvValues(): bad NV pauseHome"); }
#endif

  // set the default guide rate
  setGuideRate(GuideRateDefault);
  enableGuideRate(GuideRateDefault);
}

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

void initStartupValues() {
  // initialize some fixed-point values
  amountGuideAxis1.fixed= 0;
  amountGuideAxis2.fixed= 0;
  guideAxis1.fixed      = 0;
  guideAxis2.fixed      = 0;
  accPecGuideHA.fixed   = 0;
  fstepAxis1.fixed      = 0;
  fstepAxis2.fixed      = 0;
  origTargetAxis1.fixed = 0;

  // default values for state variables
  pierSideControl       = PierSideNone;
  dirAxis1              = 1;
  if (latitude >= 0) {
    if (axis1Settings.reverse == ON) defaultDirAxis1 = DefaultDirAxis1SCPInit; else defaultDirAxis1 = DefaultDirAxis1NCPInit;
  } else {
    if (axis1Settings.reverse == ON) defaultDirAxis1 = DefaultDirAxis1NCPInit; else defaultDirAxis1 = DefaultDirAxis1SCPInit;
  }
  dirAxis2              = 1;
  if (axis2Settings.reverse == ON) defaultDirAxis2 = DefaultDirAxis2WInit; else defaultDirAxis2 = DefaultDirAxis2EInit;
  newTargetRA           = 0;        
  newTargetDec          = 0;
  newTargetAlt          = 0;
  newTargetAzm          = 0;
  origTargetAxis1.fixed = 0;
  origTargetAxis2.fixed = 0;

  // initialize alignment
  alignNumStars         = 0;
  alignThisStar         = 0;
  indexAxis1            = 0;
  indexAxis1Steps       = 0;
  indexAxis2            = 0;
  indexAxis2Steps       = 0;
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
  homeMount             = false;
  atHome                = true;
  waitingHome           = false;
  waitingHomeContinue   = false;

  // reset tracking and rates
  cli();
  trackingState         = TrackingNone;
  lastTrackingState     = TrackingNone;
  timerRateAxis1        = siderealRate;
  timerRateAxis2        = siderealRate;
  sei();
}

// the start position
void initStartPosition() {
  startAxis1         = 0;
  startAxis2         = 0;
  cli();
  targetAxis1.part.m = 0; targetAxis1.part.f = 0;
  posAxis1           = 0;
  blAxis1            = 0;
  targetAxis2.part.m = 0; targetAxis2.part.f = 0;
  posAxis2           = 0;
  blAxis2            = 0;
  sei();
  setIndexAxis1(homePositionAxis1,PierSideEast);
  setIndexAxis2(homePositionAxis2,PierSideEast);
}

void initStartTimers() {
  // Initialize the timers that handle the sidereal clock, RA, and Dec
  HAL_Init_Timer_Sidereal();

  // wait for the sidereal clock to tick
  delay(15);

  // Initialize Axis1 and Axis2 motor timers and set their priorities
  HAL_Init_Timers_Motor();
}
