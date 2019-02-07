// -----------------------------------------------------------------------------------
// Functions for initializing pins, variables, and timers on startup

#define DEBUG_AXIS_MODES_OFF

void initStartupValues() {
  
// Basic stepper driver mode setup
// if we made through validation and AXIS1_DRIVER_MODEL exists; AXIS2_DRIVER_MODEL, AXIS1_MICROSTEPS, and AXIS2_MICROSTEPS also exist and passed validation in the pre-processor
#ifdef AXIS1_DRIVER_MODEL
  // translate microsteps to mode bits
  Axis1_Microsteps = translateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_MICROSTEPS)|TMC_AXIS1_MODE; // if this isn't a TMC2130 stepper driver TMC_AXISn_MODE, etc. = 0
  Axis2_Microsteps = translateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_MICROSTEPS)|TMC_AXIS2_MODE;
  #ifdef AXIS1_MICROSTEPS_GOTO
    Axis1_MicrostepsGoto = translateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_MICROSTEPS_GOTO)|TMC_AXIS1_MODE_GOTO;
  #endif
  #ifdef AXIS2_MICROSTEPS_GOTO
    Axis2_MicrostepsGoto = translateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_MICROSTEPS_GOTO)|TMC_AXIS2_MODE_GOTO;
  #endif
#endif

#ifdef DEBUG_AXIS_MODES_ON
  DL("A reminder: TMC_LOWPWR=64, TMC_STEALTHCHOP=32");
  D("AXIS1_MODE="); D(AXIS1_MODE);
  D(", AXIS1_MODE_GOTO="); D(AXIS1_MODE_GOTO);
  D(", AXIS1_STEP_GOTO="); DL(AXIS1_STEP_GOTO);
  D("AXIS2_MODE="); D(AXIS2_MODE);
  D(", AXIS2_MODE_GOTO="); D(AXIS2_MODE_GOTO);
  D(", AXIS2_STEP_GOTO="); DL(AXIS2_STEP_GOTO);
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
  Align.init();

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
  waitingHome         = false;
  waitingHomeContinue = false;

  // PEC sanity check
  if (SecondsPerWormRotationAxis1>PECBufferSize) SecondsPerWormRotationAxis1=PECBufferSize;

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

#ifdef MaxESP2_ON
pinMode(Axis1_Aux,INPUT_PULLUP);
#endif

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
  pinMode(LEDneg2Pin,OUTPUT); digitalWrite(LEDneg2Pin,HIGH); // light second status LED (provides just GND)
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

// Pulse per second
#ifdef PPS_SENSE_ON
  pinMode(PpsPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#elif defined(PPS_SENSE_PULLUP)
  pinMode(PpsPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PpsPin),clockSync,RISING);
#endif

// Home position sensing
#ifdef HOME_SENSE_ON
  pinMode(Axis1_HOME,INPUT_PULLUP);
  pinMode(Axis2_HOME,INPUT_PULLUP);
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

  // inputs for stepper drivers fault signal
#ifdef AXIS1_FAULT
  #if AXIS1_FAULT==LOW
    pinMode(Axis1_FAULT,INPUT_PULLUP);
  #elif AXIS1_FAULT==HIGH
    #ifdef INPUT_PULLDOWN
      pinMode(Axis1_FAULT,INPUT_PULLDOWN);
    #else
      pinMode(Axis1_FAULT,INPUT);
    #endif
  #endif
#endif

#ifdef AXIS2_FAULT
  #if AXIS2_FAULT==LOW
    pinMode(Axis2_FAULT,INPUT_PULLUP);
  #elif AXIS1_FAULT==HIGH
    #ifdef INPUT_PULLDOWN
    pinMode(Axis2_FAULT,INPUT_PULLDOWN);
    #else
    pinMode(Axis2_FAULT,INPUT);
    #endif
  #endif
#endif

  // initialize and disable the main axes stepper drivers
  pinMode(Axis1_EN,OUTPUT); 
  pinMode(Axis2_EN,OUTPUT);
  StepperModeTrackingInit();
}

void initReadNvValues() {
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=nv.read(EE_currentSite); if (currentSite>3) currentSite=0; // site index is valid?
  latitude=nv.readFloat(EE_sites+(currentSite)*25+0);
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (latitude>0) defaultDirAxis1 = defaultDirAxis1NCPInit; else defaultDirAxis1 = defaultDirAxis1SCPInit;
  longitude=nv.readFloat(EE_sites+(currentSite)*25+4);

  // the polar home position
#ifdef MOUNT_TYPE_ALTAZM
  celestialPoleAxis2=AltAzmDecStartPos;
  if (latitude<0) celestialPoleAxis1=180.0; else celestialPoleAxis1=0.0;
#else
  if (latitude<0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
  InitStartPosition();

  timeZone=nv.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  nv.readString(EE_sites+(currentSite)*25+9,siteName);

  // get date and time from EEPROM, start keeping time
  JD=nv.readFloat(EE_JD);
  LMT=nv.readFloat(EE_LMT);

  // get the RTC (if present) ready
  urtc.init();
  // read the date/time from RTC (if present)
  urtc.get(JD,LMT);

  UT1=LMT+timeZone;
  updateLST(jd2last(JD,UT1,false));

  // get the minutes past meridian east/west
#ifdef MOUNT_TYPE_GEM
  minutesPastMeridianE=round(((nv.read(EE_dpmE)-128)*60.0)/15.0); if (abs(minutesPastMeridianE)>180) minutesPastMeridianE=60;
  minutesPastMeridianW=round(((nv.read(EE_dpmW)-128)*60.0)/15.0); if (abs(minutesPastMeridianW)>180) minutesPastMeridianW=60;
#endif
  // override if specified in Config.h
#ifdef MinutesPastMeridianE
  minutesPastMeridianE=MinutesPastMeridianE;
#endif
#ifdef MinutesPastMeridianW
  minutesPastMeridianW=MinutesPastMeridianW;
#endif
  
  // get the min. and max altitude
  minAlt=nv.read(EE_minAlt)-128;
  maxAlt=nv.read(EE_maxAlt);
#ifdef MOUNT_TYPE_ALTAZM
  if (maxAlt>87) maxAlt=87;
#endif

  // get the backlash amounts
  backlashAxis2=nv.readInt(EE_backlashAxis2);
  backlashAxis1=nv.readInt(EE_backlashAxis1);
  
  // get the PEC status
  pecStatus  =nv.read(EE_pecStatus);
  pecRecorded=nv.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;
  for (int i=0; i<PECBufferSize; i++) pecBuffer[i]=nv.read(EE_pecTable+i);
  wormSensePos=nv.readLong(EE_wormSensePos);
#ifdef PEC_SENSE_OFF
  wormSensePos=0;
  pecStatus=IgnorePEC;
#endif
  
  // get the Park status
  parkSaved=nv.read(EE_parkSaved);
  parkStatus=nv.read(EE_parkStatus);

  // get the pulse-guide rate
  currentPulseGuideRate=nv.read(EE_pulseGuideRate); if (currentPulseGuideRate>GuideRate1x) currentPulseGuideRate=GuideRate1x;

  // get the Goto rate and constrain values to the limits (1/2 to 2X the MaxRate,) maxRate is in 16MHz clocks but stored in micro-seconds
  maxRate=nv.readInt(EE_maxRate)*16;
  if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
  if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
#if !defined(RememberMaxRate_ON) && !defined(REMEMBER_MAX_RATE_ON)
  if (maxRate!=MaxRate*16L) { maxRate=MaxRate*16L; nv.writeInt(EE_maxRate,(int)(maxRate/16L)); }
#endif
  setAccelerationRates(maxRate); // set the new acceleration rate

  // get autoMeridianFlip
#if defined(MOUNT_TYPE_GEM) && defined(REMEMBER_AUTO_MERIDIAN_FLIP_ON)
  autoMeridianFlip=nv.read(EE_autoMeridianFlip);
#endif

  // get meridian flip pause at home
#if defined(MOUNT_TYPE_GEM) && defined(REMEMBER_PAUSE_HOME_ON)
  pauseHome=nv.read(EE_pauseHome);
#endif

  // set the default guide rate
  setGuideRate(GuideRateDefault);
  enableGuideRate(GuideRateDefault);

  // for DC focusers read in the % power
#ifdef AXIS4_DC_MODE_ON
  dcPwrAxis4=nv.read(EE_dcPwrAxis4);
#endif
#ifdef AXIS5_DC_MODE_ON
  dcPwrAxis5=nv.read(EE_dcPwrAxis5);
#endif

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
  setIndexAxis1(celestialPoleAxis1,PierSideEast);
  setIndexAxis2(celestialPoleAxis2,PierSideEast);
}

void initWriteNvValues() {
  // EEPROM automatic initialization
  long autoInitKey = initKey;
  long thisAutoInitKey;
  if (INIT_KEY) nv.writeLong(EE_autoInitKey,autoInitKey);
  thisAutoInitKey=nv.readLong(EE_autoInitKey);
  if (autoInitKey!=thisAutoInitKey) {
    // init the site information, lat/long/tz/name
    nv.write(EE_currentSite,0);
    latitude=0; longitude=0;
    for (int l=0; l<4; l++) {
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
  
    // init the minutes past meridian east/west
    nv.write(EE_dpmE,round((minutesPastMeridianE*15.0)/60.0)+128);
    nv.write(EE_dpmW,round((minutesPastMeridianW*15.0)/60.0)+128);

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
    for (int l=0; l<PECBufferSize; l++) nv.write(EE_pecTable+l,128);
    wormSensePos=0;
    nv.writeLong(EE_wormSensePos,wormSensePos);
    
    // init the Park status
    nv.write(EE_parkSaved,false);
    nv.write(EE_parkStatus,NotParked);
  
    // init the pulse-guide rate
    nv.write(EE_pulseGuideRate,GuideRate1x);
    
    // init the default maxRate
    if (maxRate<2L*16L) maxRate=2L*16L; if (maxRate>10000L*16L) maxRate=10000L*16L;
    nv.writeInt(EE_maxRate,(int)(maxRate/16L));

    // init autoMeridianFlip
    nv.write(EE_autoMeridianFlip,autoMeridianFlip);

    // init the sidereal tracking rate, use this once - then issue the T+ and T- commands to fine tune
    // 1/16uS resolution timer, ticks per sidereal second
    nv.writeLong(EE_siderealInterval,siderealInterval);

    // set default focuser positions at zero
    nv.writeLong(EE_posAxis4,0L);
    nv.writeLong(EE_posAxis5,0L);
    // for DC focusers read in the % power
#ifdef AXIS4_DC_MODE_ON
    nv.write(EE_dcPwrAxis4,50);
#endif
#ifdef AXIS5_DC_MODE_ON
    nv.write(EE_dcPwrAxis5,50);
#endif

    // finally, stop the init from happening again
    nv.writeLong(EE_autoInitKey,autoInitKey);
    
    // clear the pointing model
    saveAlignModel();
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

// Basic stepper driver mode setup
#ifdef AXIS1_DRIVER_MODEL

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to modebit settings 
unsigned int translateMicrosteps(int axis, int DriverModel, unsigned int Microsteps) {
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
    case ST820:
      Mode = searchTable(StepsST820, LEN_ST820, Microsteps);
      break;
    case TMC2100:
      Mode = searchTable(StepsTMC2100, LEN_TMC2100, Microsteps);
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
