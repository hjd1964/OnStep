// -----------------------------------------------------------------------------------
// Functions for initializing pins, variables, and timers on startup

void Init_Startup_Values() {
// simplified stepper driver mode setup
// if we made through validation and AXIS1_DRIVER_MODEL exists; AXIS2_DRIVER_MODEL, AXIS1_MICROSTEPS, and AXIS2_MICROSTEPS also exist and passed validation in the pre-processor
#ifdef AXIS1_DRIVER_MODEL
  // translate microsteps to mode bits
  Axis1_Microsteps = TranslateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_MICROSTEPS);
  Axis2_Microsteps = TranslateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_MICROSTEPS);
  #ifdef AXIS1_MICROSTEPS_GOTO
    Axis1_MicrostepsGoto = TranslateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_MICROSTEPS_GOTO)|TMC_AXIS1_MODE;
  #endif  
  #ifdef AXIS2_MICROSTEPS_GOTO
    Axis2_MicrostepsGoto = TranslateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_MICROSTEPS_GOTO)|TMC_AXIS2_MODE;
  #endif  
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
  targetAxis1.part.m = 90L*(long)StepsPerDegreeAxis1; targetAxis1.part.f = 0;
  posAxis1           = 90L*(long)StepsPerDegreeAxis1;
  trueAxis1          = 90L*(long)StepsPerDegreeAxis1;
  targetAxis2.part.m = 90L*(long)StepsPerDegreeAxis2; targetAxis2.part.f = 0;
  posAxis2           = 90L*(long)StepsPerDegreeAxis2;
  trueAxis2          = 90L*(long)StepsPerDegreeAxis2;
  sei();

#ifdef ROTATOR_ON
  targetAxis3.fixed = 0;
  amountRotateAxis3.fixed=0;
#endif
#ifdef FOCUSER1_ON
  targetAxis4.fixed = 0;
  amountMoveAxis4.fixed=0;
#endif
#ifdef FOCUSER2_ON
  targetAxis5.fixed = 0;
  amountMoveAxis5.fixed=0;
#endif

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

  // initialize alignment
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
  cli();
  trackingState       = TrackingNone;
  lastTrackingState   = TrackingNone;
  timerRateAxis1      = SiderealRate;
  timerRateAxis2      = SiderealRate;
  sei();
}

void Init_Pins() {
// ------------------------------------------------------------------
// ESP-01 (ESP8266) firmware flashing control

#ifdef ESP8266_CONTROL_ON
  pinMode(Axis1_Aux,OUTPUT);                // ESP8266 GPIO0
  digitalWrite(Axis1_Aux,HIGH); delay(20);  // Run mode
  pinMode(Axis2_Aux,OUTPUT);                // ESP8266 RST
  digitalWrite(Axis2_Aux,LOW);  delay(200); // Reset, if LOW
  digitalWrite(Axis2_Aux,HIGH);             // Reset, inactive HIGH
#endif

// ------------------------------------------------------------------
// LED and audible feedback

#ifdef STATUS_LED_PINS_ON
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);  // light status LED (provides GND)
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
#endif
  LED_ON=true;
#endif

#ifdef STATUS_LED_PINS
  pinMode(LEDnegPin,OUTPUT); digitalWrite(LEDnegPin,LOW);  // light status LED (provides pwm'd GND for polar reticule)
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT); digitalWrite(LEDposPin,HIGH); // sometimes +5v is provided on a pin
#endif
  analogWrite(LEDnegPin,STATUS_LED_PINS);
  LED_ON=true;
#endif

#ifdef RETICULE_LED_PINS
  pinMode(ReticulePin,OUTPUT); analogWrite(ReticulePin,reticuleBrightness); // light reticule LED
#endif

#ifdef STATUS_LED2_PINS_ON
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,LOW); // light second status LED (provides just GND)
  LED2_ON=false;
#elif defined(STATUS_LED2_PINS)
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,LOW); // light second status LED (provides pwm'd GND for polar reticule)
  analogWrite(LEDneg2Pin,STATUS_LED2_PINS);
#endif

// ready the sound/buzzer pin
#if defined(BUZZER) || defined(BUZZER_ON)
  pinMode(TonePin,OUTPUT);
  digitalWrite(TonePin,LOW);
#endif

// ------------------------------------------------------------------
// Misc. devices and sensors

// PEC index sense
#ifdef PEC_SENSE_ON
  pinMode(PecPin,INPUT);
#elif defined(PEC_SENSE_PULLUP)
  pinMode(PecPin,INPUT_PULLUP);
#elif defined(PEC_SENSE_PULLDOWN)
  pinMode(PecPin,INPUT_PULLDOWN);
#endif

// limit switch sense
#ifdef LIMIT_SENSE_ON  
  pinMode(LimitPin,INPUT_PULLUP);
#endif

// ST4 interface
#ifdef ST4_ON
  pinMode(ST4RAw,INPUT);
  pinMode(ST4RAe,INPUT);
  pinMode(ST4DEn,INPUT);
  pinMode(ST4DEs,INPUT);
#elif defined(ST4_PULLUP)
  pinMode(ST4RAw,INPUT_PULLUP);
  pinMode(ST4RAe,INPUT_PULLUP);
  pinMode(ST4DEn,INPUT_PULLUP);
  pinMode(ST4DEs,INPUT_PULLUP);
#endif

// Pulse per second
#ifdef PPS_SENSE_ON
  pinMode(PpsPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(PpsPin),ClockSync,RISING);
#elif defined(PPS_SENSE_PULLUP)
  pinMode(PpsPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PpsPin),ClockSync,RISING);
#endif

// ------------------------------------------------------------------
// Stepper driver control

  pinMode(Axis1StepPin,OUTPUT);   // Axis1
  pinMode(Axis1DirPin,OUTPUT); 
// provide 5V power to stepper drivers if requested (classic Pin-map)
#ifdef POWER_SUPPLY_PINS_ON
  pinMode(Axis15vPin,OUTPUT);
  digitalWrite(Axis15vPin,HIGH);
  pinMode(Axis25vPin,OUTPUT);     // Axis2
  digitalWrite(Axis25vPin,HIGH);
#endif
// provide Gnd on next to the Dec stepper pins if requested (classic Pin-map)
#ifdef Axis2GndPin
  pinMode(Axis2GndPin,OUTPUT);
  digitalWrite(Axis2GndPin,LOW);
#endif
  pinMode(Axis2StepPin,OUTPUT); 
  pinMode(Axis2DirPin,OUTPUT); 
#ifdef ROTATOR_ON
  pinMode(Axis3StepPin,OUTPUT);   // Axis3
  pinMode(Axis3DirPin,OUTPUT);
#endif
#ifdef FOCUSER1_ON
  pinMode(Axis4StepPin,OUTPUT);   // Axis4
  pinMode(Axis4DirPin,OUTPUT); 
#endif
#ifdef FOCUSER2_ON
  pinMode(Axis5StepPin,OUTPUT);   // Axis5
  pinMode(Axis5DirPin,OUTPUT); 
#endif

// inputs for stepper drivers fault signal
#ifdef AXIS1_FAULT_LOW
  pinMode(Axis1_FAULT,INPUT_PULLUP);
#elif defined(AXIS1_FAULT_HIGH)
  #ifdef INPUT_PULLDOWN
  pinMode(Axis1_FAULT,INPUT_PULLDOWN);
  #else
  pinMode(Axis1_FAULT,INPUT);
  #endif
#endif
#ifdef AXIS2_FAULT_LOW
  pinMode(Axis2_FAULT,INPUT_PULLUP);
#elif defined(AXIS1_FAULT_HIGH)
  #ifdef INPUT_PULLDOWN
  pinMode(Axis2_FAULT,INPUT_PULLDOWN);
  #else
  pinMode(Axis2_FAULT,INPUT);
  #endif
#endif

// initialize and disable the stepper drivers
  pinMode(Axis1_EN,OUTPUT); 
  pinMode(Axis2_EN,OUTPUT);
  StepperModeTrackingInit();

// turn on the Rotator/Focuser stepper drivers (LOW)
#ifdef Axis3_EN
    pinMode(Axis3_EN,OUTPUT); 
#endif
#ifdef Axis4_EN
    pinMode(Axis4_EN,OUTPUT); 
#endif
#ifdef Axis5_EN
    pinMode(Axis5_EN,OUTPUT); 
#endif

}

void Init_ReadEEPROM_Values() {
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite); if (currentSite>3) currentSite=0; // site index is valid?
  latitude=EEPROM_readFloat(EE_sites+(currentSite)*25+0);

#ifdef MOUNT_TYPE_ALTAZM
  celestialPoleAxis2=AltAzmDecStartPos;
  if (latitude<0) celestialPoleAxis1=180L; else celestialPoleAxis1=0L;
#else
  if (latitude<0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (latitude>0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
  longitude=EEPROM_readFloat(EE_sites+(currentSite)*25+4);
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);

  // update starting coordinates to reflect NCP or SCP polar home position
  startAxis1 = celestialPoleAxis1*(long)StepsPerDegreeAxis1;
  startAxis2 = celestialPoleAxis2*(double)StepsPerDegreeAxis2;
  cli();
  targetAxis1.part.m = startAxis1;
  targetAxis1.part.f = 0;
  posAxis1           = startAxis1;
  trueAxis1          = startAxis1;
  targetAxis2.part.m = startAxis2;
  targetAxis2.part.f = 0;
  posAxis2           = startAxis2;
  trueAxis2          = startAxis2;
  sei();
  
  // get date and time from EEPROM, start keeping time
  JD=EEPROM_readFloat(EE_JD);
  LMT=EEPROM_readFloat(EE_LMT);
#ifdef RTC_DS3234
  rtc.begin(DS3234_CS_PIN); rtc.update();
  if ((rtc.year()>=0) && (rtc.month()<=99) && (rtc.month()>=1) && (rtc.month()<=12) && (rtc.date()>=1) && (rtc.date()<=31) &&
      (rtc.hour()>=0) && (rtc.hour()<=23) && (rtc.minute()>=0) && (rtc.minute()<=59) && (rtc.second()>=0) && (rtc.second()<=59)) {
    int y1=rtc.year(); if (y1>11) y1=y1+2000; else y1=y1+2100;
    JD=julian(y1,rtc.month(),rtc.date());
    LMT=(rtc.hour()+(rtc.minute()/60.0)+(rtc.second()/3600.0));
    rtc.writeSQW(SQW_SQUARE_1);
  }
#endif
  UT1=LMT+timeZone;
  update_lst(jd2last(JD,UT1,false));

  // get the minutes past meridian east/west
#ifdef MOUNT_TYPE_GEM
  minutesPastMeridianE=round(((EEPROM.read(EE_dpmE)-128)*60.0)/15.0); if (abs(minutesPastMeridianE)>180) minutesPastMeridianE=60;
  minutesPastMeridianW=round(((EEPROM.read(EE_dpmW)-128)*60.0)/15.0); if (abs(minutesPastMeridianW)>180) minutesPastMeridianW=60;
#endif
  // override if specified in Config.h
#ifdef MinutesPastMeridianE
  minutesPastMeridianE=MinutesPastMeridianE;
#endif
#ifdef MinutesPastMeridianW
  minutesPastMeridianW=MinutesPastMeridianW;
#endif
  
  // get the min. and max altitude
  minAlt=EEPROM.read(EE_minAlt)-128;
  maxAlt=EEPROM.read(EE_maxAlt);
#ifdef MOUNT_TYPE_ALTAZM
  if (maxAlt>87) maxAlt=87;
#endif

  // get the backlash amounts
  backlashAxis2=EEPROM_readInt(EE_backlashAxis2);
  backlashAxis1=EEPROM_readInt(EE_backlashAxis1);
  
  // get the PEC status
  pecStatus  =EEPROM.read(EE_pecStatus);
  pecRecorded=EEPROM.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;
  for (int i=0; i<PECBufferSize; i++) pecBuffer[i]=EEPROM.read(EE_pecTable+i);
  wormSensePos=EEPROM_readLong(EE_wormSensePos);
#ifdef PEC_SENSE_OFF
  wormSensePos=0;
  pecStatus=IgnorePEC;
#endif
  
  // get the Park status
  parkSaved=EEPROM.read(EE_parkSaved);
  parkStatus=EEPROM.read(EE_parkStatus);

  // get the pulse-guide rate
  currentPulseGuideRate=EEPROM.read(EE_pulseGuideRate); if (currentPulseGuideRate>GuideRate1x) currentPulseGuideRate=GuideRate1x;

  // get the Goto rate and constrain values to the limits (1/2 to 2X the MaxRate,) maxRate is in 16MHz clocks but stored in micro-seconds
  maxRate=EEPROM_readInt(EE_maxRate)*16;
  if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
  if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
#if !defined(RememberMaxRate_ON) && !defined(REMEMBER_MAX_RATE_ON)
  if (maxRate!=MaxRate*16L) { maxRate=MaxRate*16L; EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L)); }
#endif
  SetAccelerationRates(maxRate); // set the new acceleration rate

  // get autoMeridianFlip
#if defined(MOUNT_TYPE_GEM) && defined(REMEMBER_AUTO_MERIDIAN_FLIP_ON)
  autoMeridianFlip=EEPROM.read(EE_autoMeridianFlip);
#endif

  // get meridian flip pause at home
#if defined(MOUNT_TYPE_GEM) && defined(REMEMBER_PAUSE_HOME_ON)
  pauseHome=EEPROM.read(EE_pauseHome);
#endif

  // set the default guide rate, 24x sidereal
  setGuideRate(GuideRate24x);
  enableGuideRate(GuideRate24x);
}

void Init_EEPROM_Values() {
  // EEPROM automatic initialization
  long autoInitKey = initKey;
  long thisAutoInitKey;
  if (INIT_KEY) EEPROM_writeLong(EE_autoInitKey,autoInitKey);
  thisAutoInitKey=EEPROM_readLong(EE_autoInitKey);
  if (autoInitKey!=thisAutoInitKey) {
    // init the site information, lat/long/tz/name
    EEPROM.write(EE_currentSite,0);
    latitude=0; longitude=0;
    for (int l=0; l<4; l++) {
      EEPROM_writeFloat(EE_sites+(l)*25+0,latitude);
      EEPROM_writeFloat(EE_sites+(l)*25+4,longitude);
      EEPROM.write(EE_sites+(l)*25+8,128);
      EEPROM.write(EE_sites+(l)*25+9,0);
    }
  
    // init the date and time January 1, 2013. 0 hours LMT
    JD=2456293.5;
    LMT=0.0;
    EEPROM_writeFloat(EE_JD,JD);
    EEPROM_writeFloat(EE_LMT,LMT);
  
    // init the minutes past meridian east/west
    EEPROM.write(EE_dpmE,round((minutesPastMeridianE*15.0)/60.0)+128);
    EEPROM.write(EE_dpmW,round((minutesPastMeridianW*15.0)/60.0)+128);

    // init the min and max altitude
    minAlt=-10;
    maxAlt=85;
    EEPROM.write(EE_minAlt,minAlt+128);
    EEPROM.write(EE_maxAlt,maxAlt);
  
    // init (clear) the backlash amounts
    EEPROM_writeInt(EE_backlashAxis2,0);
    EEPROM_writeInt(EE_backlashAxis1,0);
  
    // init the PEC status, clear the index and buffer
    EEPROM.write(EE_pecStatus,IgnorePEC);
    EEPROM.write(EE_pecRecorded,false);
    for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_pecTable+l,128);
    wormSensePos=0;
    EEPROM_writeLong(EE_wormSensePos,wormSensePos);
    
    // init the Park status
    EEPROM.write(EE_parkSaved,false);
    EEPROM.write(EE_parkStatus,NotParked);
  
    // init the pulse-guide rate
    EEPROM.write(EE_pulseGuideRate,GuideRate1x);
    
    // init the default maxRate
    if (maxRate<2L*16L) maxRate=2L*16L; if (maxRate>10000L*16L) maxRate=10000L*16L;
    EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L));

    // init autoMeridianFlip
    EEPROM.write(EE_autoMeridianFlip,autoMeridianFlip);

    // init the sidereal tracking rate, use this once - then issue the T+ and T- commands to fine tune
    // 1/16uS resolution timer, ticks per sidereal second
    EEPROM_writeLong(EE_siderealInterval,siderealInterval);
    
    // finally, stop the init from happening again
    EEPROM_writeLong(EE_autoInitKey,autoInitKey);
    
    // clear the pointing model
    saveAlignModel();

    // disable OnTrack/Refraction dual axis mode
    EEPROM.write(EE_onTrackDec,0);
  }
}

void Init_Start_Timers() {
  // Initialize the timers that handle the sidereal clock, RA, and Dec
  HAL_Init_Timer_Sidereal();

  // wait for the sidereal clock to tick
  delay(15);

  // Initialize Axis1 and Axis2 motor timers and set their priorities
  HAL_Init_Timers_Motor();
}

void EnableStepperDrivers() {
  // enable the stepper drivers
  if (axis1Enabled==false) {
    digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
    digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
    delay(10);
  }
}

void DisableStepperDrivers() {
  // disable the stepper drivers
  if (axis1Enabled==true) {
    digitalWrite(Axis1_EN,Axis1_Disabled); axis1Enabled=false;
    digitalWrite(Axis2_EN,Axis2_Disabled); axis2Enabled=false;
    delay(10);
  }
}

// simplified stepper driver mode setup
#ifdef AXIS1_DRIVER_MODEL

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to modebit settings 
unsigned int TranslateMicrosteps(int axis, int DriverModel, unsigned int Microsteps) {
  unsigned int Mode;
    
  // we search for each model, since they are different
  switch(DriverModel) {
    case A4988:
      Mode = searchTable(StepsA4988, LEN_A4988, Microsteps);
      break;
    case DRV8825:
      Mode = searchTable(StepsDRV8825, LEN_DRV8825, Microsteps);
      break;
    case LV8729:
      Mode = searchTable(StepsLV8729, LEN_LV8729, Microsteps);
      break;
    case TMC2208:
      Mode = searchTable(StepsTMC2208, LEN_TMC2208, Microsteps);
      break;
    case TMC2130:
      Mode = searchTable(StepsTMC2130, LEN_TMC2130, Microsteps);
      break;
    default:
      Mode=1;
  }

  return Mode;
}

// search function
unsigned int searchTable(unsigned int Table[][2], int TableLen, unsigned int Microsteps) {
  int i;
  
  for(i = 0; i < TableLen; i++) {
    if (Table[i][0] == Microsteps) {
      return Table[i][1];
    }
  }
  
  return 0;
}
#endif

