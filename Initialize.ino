// -----------------------------------------------------------------------------------
// Functions for initializing pins, variables, and timers on startup

void Init_Startup_Values() {
// initialize some fixed-point values
  amountGuideAxis1.fixed=0;
  amountGuideAxis2.fixed=0;
  guideAxis1.fixed=0;
  guideAxis2.fixed=0;
  accPecGuideHA.fixed=0;
  fstepAxis1.fixed=0;
  fstepAxis2.fixed=0;
  origTargetAxis1.fixed = 0;
  targetAxis1.part.m = 90L*(long)StepsPerDegreeAxis1;
  targetAxis1.part.f = 0;
  targetAxis2.part.m = 90L*(long)StepsPerDegreeAxis2;
  targetAxis2.part.f = 0;
  fstepAxis1.fixed=doubleToFixed(StepsPerSecondAxis1/100.0);

// initialize alignment
  #ifdef MOUNT_TYPE_ALTAZM
  Align.init();
  #endif
  GeoAlign.init();
}

void Init_Pins() {
#ifdef ESP8266_CONTROL_ON
  pinMode(Axis1_Aux,OUTPUT);                // ESP8266 GPIO0
  digitalWrite(Axis1_Aux,HIGH); delay(20);  // Run mode
  pinMode(Axis2_Aux,OUTPUT);                // ESP8266 RST
  digitalWrite(Axis2_Aux,LOW);  delay(200); // Reset, if LOW
  digitalWrite(Axis2_Aux,HIGH);             // Reset, inactive HIGH
#endif

// initialize the stepper control pins Axis1 and Axis2
  pinMode(Axis1StepPin,OUTPUT);
  pinMode(Axis1DirPin,OUTPUT); 
#ifdef Axis2GndPin
  pinMode(Axis2GndPin,OUTPUT);
  digitalWrite(Axis2GndPin,LOW);
#endif
  pinMode(Axis2StepPin,OUTPUT); 
  pinMode(Axis2DirPin,OUTPUT); 

// override any status LED and set the reset pin HIGH
#if defined(W5100_ON) && defined(__ARM_Teensy3__)
#ifdef STATUS_LED_PINS_ON
#undef STATUS_LED_PINS_ON
#endif
#ifdef STATUS_LED_PINS
#undef STATUS_LED_PINS
#endif
  pinMode(RstPin,OUTPUT);
  digitalWrite(RstPin,LOW);
  delay(500);
  digitalWrite(RstPin,HIGH);
#endif

// light status LED (provides GND)
#ifdef STATUS_LED_PINS_ON
  pinMode(LEDnegPin,OUTPUT);
  digitalWrite(LEDnegPin,LOW);
// sometimes +5v is provided on a pin
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT);
  digitalWrite(LEDposPin,HIGH);
#endif
  LED_ON=true;
#endif

// light status LED (provides pwm'd GND for polar reticule)
#ifdef STATUS_LED_PINS
  pinMode(LEDnegPin,OUTPUT);
  digitalWrite(LEDnegPin,LOW);
// sometimes +5v is provided on a pin
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT);
  digitalWrite(LEDposPin,HIGH);
#endif
  analogWrite(LEDnegPin,STATUS_LED_PINS);
  LED_ON=true;
#endif

// light reticule LED
#ifdef RETICULE_LED_PINS
#if defined(__ARM_Teensy3__) && !defined(ALTERNATE_PINMAP_ON)
  #ifdef STATUS_LED_PINS_ON
    #undef STATUS_LED_PINS_ON
  #endif
  #ifdef STATUS_LED_PINS
    #undef STATUS_LED_PINS
  #endif
#endif
  pinMode(ReticulePin,OUTPUT);
  analogWrite(ReticulePin,reticuleBrightness);
#endif

// light second status LED (provides just GND)
#ifdef STATUS_LED2_PINS_ON
  pinMode(LEDneg2Pin,OUTPUT);
  digitalWrite(LEDneg2Pin,LOW);
  LED2_ON=false;
#endif
// light second status LED (provides pwm'd GND for polar reticule)
#ifdef STATUS_LED2_PINS
  pinMode(LEDneg2Pin,OUTPUT);
  digitalWrite(LEDneg2Pin,LOW);
  analogWrite(LEDneg2Pin,STATUS_LED2_PINS);
#endif

// provide 5V power to stepper drivers if requested
#ifdef POWER_SUPPLY_PINS_ON  
  pinMode(Axis15vPin,OUTPUT);
  digitalWrite(Axis15vPin,HIGH);
  pinMode(Axis25vPin,OUTPUT);
  digitalWrite(Axis25vPin,HIGH);
#endif

// PEC index sense
#ifdef PEC_SENSE_ON
  pinMode(PecPin,INPUT);
#endif
#ifdef PEC_SENSE_PULLUP
  pinMode(PecPin,INPUT_PULLUP);
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
#endif
#ifdef ST4_PULLUP
  pinMode(ST4RAw,INPUT_PULLUP);
  pinMode(ST4RAe,INPUT_PULLUP);
  pinMode(ST4DEn,INPUT_PULLUP);
  pinMode(ST4DEs,INPUT_PULLUP);
#endif

// inputs for stepper drivers fault signal
#ifndef AXIS1_FAULT_OFF
  #if defined(__ARM_Teensy3__) && defined(ALTERNATE_PINMAP_ON)
    #ifdef AXIS1_FAULT_LOW
      pinMode(Axis1_FAULT,INPUT_PULLUP);
    #endif
    #ifdef AXIS1_FAULT_HIGH
      pinMode(Axis1_FAULT,INPUT_PULLDOWN);
    #endif
  #else
    pinMode(Axis1_FAULT,INPUT);
  #endif
#endif
#ifndef AXIS2_FAULT_OFF
  #if defined(__ARM_Teensy3__) && defined(ALTERNATE_PINMAP_ON)
    #ifdef AXIS2_FAULT_LOW
      pinMode(Axis2_FAULT,INPUT_PULLUP);
    #endif
    #ifdef AXIS1_FAULT_HIGH
      pinMode(Axis2_FAULT,INPUT_PULLDOWN);
    #endif
  #else
    pinMode(Axis2_FAULT,INPUT);
  #endif
#endif
  
  // initialize/enable the stepper drivers
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
  delay(100);
  DecayModeTracking();
  // initialize/disable the stepper drivers
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,Axis1_Disabled); axis1Enabled=false;
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis2_EN,Axis2_Disabled); axis2Enabled=false;

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#if !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // automatic mode switching during slews, initialize micro-step mode
  #ifdef AXIS1_MODE
    if ((AXIS1_MODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  
  #ifdef AXIS2_MODE
    if ((AXIS2_MODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#endif

#ifdef PPS_SENSE_ON
#if defined(__AVR_ATmega2560__)
  attachInterrupt(PpsInt,ClockSync,RISING);
#elif defined(__ARM_Teensy3__)
  attachInterrupt(PpsPin,ClockSync,RISING);
#elif defined(__ARM_TI_TM4C__)
  attachInterrupt(PpsPin,ClockSync,RISING);
#endif
#endif
}

void Init_ReadEEPROM_Values() {
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite);  if (currentSite>3) currentSite=0; // site index is valid?
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
  UT1=LMT+timeZone;
  UT1_start=UT1;
  update_lst(jd2last(JD,UT1));

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
  for (int i=0; i<PECBufferSize; i++) pecBuffer[i]=EEPROM.read(EE_indexWorm+i);
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

  // makes onstep think that you parked the 'scope
  // combined with a hack in the goto syncEqu() function and you can quickly recover from
  // a reset without loosing much accuracy in the sky.  PEC is toast though.
#ifdef RESCUE_MODE_ON
  parkSaved=true;    
  parkStatus=Parked;
#endif

  // set the default guide rate, 16x sidereal
  setGuideRate(GuideRate16x);
  enableGuideRate(GuideRate16x);
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
    for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_indexWorm+l,128);
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
  }
}

void Init_Start_Timers() {
#if defined(__ARM_TI_TM4C__)
  // need to initialise timers before using SetSiderealClockRate
  // all timers are 32 bits
  // timer 1A is used instead of itimer1
  // timer 2A is used instead of itimer3
  // timer 3A is used instead of itimer4

  // Enable Timer 1 Clock
  SysCtlPeripheralEnable(Sysctl_Periph_Timer1);

  // Configure Timer Operation as Periodic
  TimerConfigure(Timer1_base, TIMER_CFG_PERIODIC);

  // register interrupt without editing the startup Energia file
  //IntRegister( INT_TIMER1A, TIMER1_COMPA_vect );
  TimerIntRegister(Timer1_base, TIMER_A, TIMER1_COMPA_vect );

  // Enable Timer 1A interrupt
  IntEnable(Int_timer1);

  // Timer 1A generate interrupt when Timeout occurs
  TimerIntEnable(Timer1_base, TIMER_TIMA_TIMEOUT);

  // Configure Timer Frequency - initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);

  // Start Timer 1A
  TimerEnable(Timer1_base, TIMER_A);

  // we also initialise timer 2A and 3A here as they may get used uninitialised from
  // the interrupt for timer 1 if it gets triggered in the meantime
  // we will not start them yet though

  // Enable Timer 2 and 3 Clocks
  SysCtlPeripheralEnable(Sysctl_Periph_Timer3);
  SysCtlPeripheralEnable(Sysctl_Periph_Timer4);

  // Configure Timer Operation as Periodic
  TimerConfigure(Timer3_base, TIMER_CFG_PERIODIC);
  TimerConfigure(Timer4_base, TIMER_CFG_PERIODIC);

  // register interrupts without editing the startup Energia file
  //IntRegister( INT_TIMER2A, TIMER3_COMPA_vect );
  //IntRegister( INT_TIMER3A, TIMER4_COMPA_vect );
  TimerIntRegister(Timer3_base, TIMER_A, TIMER3_COMPA_vect );
  TimerIntRegister(Timer4_base, TIMER_A, TIMER4_COMPA_vect );

  // Enable Timer 2A and 3A interrupts
  IntEnable(Int_timer3);
  IntEnable(Int_timer4);

  // Timer 2A and 3A generate interrupt when Timeout occurs
  TimerIntEnable(Timer3_base, TIMER_TIMA_TIMEOUT);
  TimerIntEnable(Timer4_base, TIMER_TIMA_TIMEOUT);

  //IntMasterEnable(); // not sure if needed, it works without
#else
  // initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);
#endif

#if defined(__AVR_ATmega2560__)
  TCCR3B = (1 << WGM12) | (1 << CS11); // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  TCCR4B = (1 << WGM12) | (1 << CS11); // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
#elif defined(__ARM_Teensy3__)
  // set the system timer for millis() to the second highest priority
  SCB_SHPR3 = (32 << 24) | (SCB_SHPR3 & 0x00FFFFFF);

  itimer3.begin(TIMER3_COMPA_vect, (float)128 * 0.0625);
  itimer4.begin(TIMER4_COMPA_vect, (float)128 * 0.0625);

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH0, 32);
  // set the motor timers to run at the highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH1, 0);
  NVIC_SET_PRIORITY(IRQ_PIT_CH2, 0);

#elif defined(__ARM_TI_TM4C__)
  TimerLoadSet(Timer3_base, TIMER_A, (int) (F_BUS / 1000000 * 128 * 0.0625));
  TimerLoadSet(Timer4_base, TIMER_A, (int) (F_BUS / 1000000 * 128 * 0.0625));

  // Start Timer 2A and 3A
  TimerEnable(Timer3_base, TIMER_A);
  TimerEnable(Timer4_base, TIMER_A);

  IntPrioritySet(Int_timer1, 1);
  IntPrioritySet(Int_timer3, 0);
  IntPrioritySet(Int_timer4, 0);
#endif
}

