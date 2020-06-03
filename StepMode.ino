// -----------------------------------------------------------------------------------
// Stepper driver mode control

// initialize stepper drivers
void StepperModeTrackingInit() {

  // enable stepper drivers
  enableStepperDrivers();
  
  // setup Axis1 and Axis2
#if AXIS1_DRIVER_MODEL == TMC_SPI
  driversInitTmcMode();
  driversInitTmcStealthChop();
#endif
  axis1DriverTrackingMode(true);
  axis2DriverTrackingMode(true);
#if AXIS1_DRIVER_MODEL == TMC_SPI && MODE_SWITCH_BEFORE_SLEW == OFF
  #ifdef AXIS1_DRIVER_CODE_GOTO
    axis1DriverInitFast();
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    axis2DriverInitFast();
  #endif
#endif

  // then disable again
  disableStepperDrivers();
}

#if AXIS1_DRIVER_MODEL == TMC_SPI
// -----------------------------------------------------------------------------------
// TMC SPI s/d stepper drivers

void driversInitTmcStealthChop() {
  cli();
  stepAxis1=1;
  stepAxis2=1;
  sei();
  
  // stop motion
  haltStepperDrivers();
  
  #if AXIS1_DRIVER_DECAY_MODE == STEALTHCHOP || AXIS2_DRIVER_DECAY_MODE == STEALTHCHOP
    tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IRUN);
    tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IRUN);
    delay(150);
  #endif
  tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IHOLD);
  tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IHOLD);
  
  // resume motion
  resumeStepperDrivers();
}

void driversInitTmcMode() {
  // spreadCycle
  #if AXIS1_DRIVER_CHOP_TOFF != OFF
    tmcAxis1.set_CHOPCONF_toff(AXIS1_DRIVER_CHOP_TOFF);
  #endif
  #if AXIS1_DRIVER_CHOP_HSTART != OFF
    tmcAxis1.set_CHOPCONF_hstart(AXIS1_DRIVER_CHOP_HSTART);
  #endif
  #if AXIS1_DRIVER_CHOP_HEND != OFF
    tmcAxis1.set_CHOPCONF_hend(AXIS1_DRIVER_CHOP_HEND);
  #endif
  #if AXIS1_DRIVER_CHOP_RNDTF != OFF
    tmcAxis1.set_CHOPCONF_rndtf(AXIS1_DRIVER_CHOP_RNDTF);
  #endif
  #if AXIS1_DRIVER_CHOP_TBL != OFF
    tmcAxis1.set_CHOPCONF_tbl(AXIS1_DRIVER_CHOP_TBL);
  #endif
  #if AXIS1_DRIVER_FS_VHIGH != OFF
    tmcAxis1.set_THIGH_value( round(12000000.0/(AXIS1_DRIVER_FS_VHIGH*(256.0/AXIS1_DRIVER_MICROSTEPS))) );
    tmcAxis1.set_CHOPCONF_vhighfs(1);
    tmcAxis1.set_CHOPCONF_vhighchm(1);
  #endif

  #if AXIS2_DRIVER_CHOP_TOFF != OFF
    tmcAxis2.set_CHOPCONF_toff(AXIS2_DRIVER_CHOP_TOFF);
  #endif
  #if AXIS2_DRIVER_CHOP_HSTART != OFF
    tmcAxis2.set_CHOPCONF_hstart(AXIS2_DRIVER_CHOP_HSTART);
  #endif
  #if AXIS2_DRIVER_CHOP_HEND != OFF
    tmcAxis2.set_CHOPCONF_hend(AXIS2_DRIVER_CHOP_HEND);
  #endif
  #if AXIS2_DRIVER_CHOP_RNDTF != OFF
    tmcAxis2.set_CHOPCONF_rndtf(AXIS2_DRIVER_CHOP_RNDTF);
  #endif
  #if AXIS2_DRIVER_CHOP_TBL != OFF
    tmcAxis2.set_CHOPCONF_tbl(AXIS2_DRIVER_CHOP_TBL);
  #endif
  #if AXIS2_DRIVER_FS_VHIGH != OFF
    tmcAxis2.set_THIGH_value( round(12000000.0/(AXIS2_DRIVER_FS_VHIGH*(256.0/AXIS2_DRIVER_MICROSTEPS))) );
    tmcAxis2.set_CHOPCONF_vhighfs(1);
    tmcAxis2.set_CHOPCONF_vhighchm(1);
  #endif

  // stealthChop
  #if AXIS1_DRIVER_PWM_GRAD != OFF
    tmcAxis1.set_PWMCONF_PWM_GRAD(AXIS1_DRIVER_PWM_GRAD);
  #endif
  #if AXIS1_DRIVER_PWM_VHIGH_SC != OFF
    tmcAxis1.set_TPWMTHRS_value( round(12000000.0/(AXIS1_DRIVER_PWM_VHIGH_SC*(256.0/AXIS1_DRIVER_MICROSTEPS))) );
  #endif

  #if AXIS2_DRIVER_PWM_GRAD != OFF
    tmcAxis2.set_PWMCONF_PWM_GRAD(AXIS2_DRIVER_PWM_GRAD);
  #endif
  #if AXIS2_DRIVER_PWM_VHIGH_SC != OFF
    tmcAxis2.set_TPWMTHRS_value( round(12000000.0/(AXIS2_DRIVER_PWM_VHIGH_SC*(256.0/AXIS2_DRIVER_MICROSTEPS))) );
  #endif
}

volatile boolean _a1trk=false;
void axis1DriverTrackingMode(boolean init) {
  if (_a1trk) return;
  if (!tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,255,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IHOLD)) return;
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) {  haltStepperDrivers(); if (!tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE)) { resumeStepperDrivers(); return; } stepAxis1=1; resumeStepperDrivers(); }
  _a1trk=true;
}

volatile boolean _a2trk=false;
void axis2DriverTrackingMode(boolean init) {
  if (_a2trk) return;
  if (!tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,255,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IHOLD)) return;
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) { haltStepperDrivers(); if (!tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE)) { resumeStepperDrivers(); return; } stepAxis2=1; resumeStepperDrivers(); }
  _a2trk=true;
}

void axis1DriverGotoMode() {
  if (!_a1trk) return;
  if (!tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE_GOTO,255,AXIS1_DRIVER_IGOTO,AXIS1_DRIVER_IHOLD)) return;
  #if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS1_DRIVER_CODE_GOTO)
    haltStepperDrivers(); if (!tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE_GOTO)) { resumeStepperDrivers(); return; } stepAxis1=AXIS1_DRIVER_STEP_GOTO; resumeStepperDrivers();
  #endif
  _a1trk=false;
}

void axis2DriverGotoMode() {
  if (!_a2trk) return;
  if (!tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE_GOTO,255,AXIS2_DRIVER_IGOTO,AXIS2_DRIVER_IHOLD)) return;
  #if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS2_DRIVER_CODE_GOTO)
    haltStepperDrivers(); if (!tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE_GOTO)) { resumeStepperDrivers(); return; } stepAxis2=AXIS2_DRIVER_STEP_GOTO; resumeStepperDrivers();
  #endif
  _a2trk=false;
}

#if MODE_SWITCH_BEFORE_SLEW == OFF
  #ifdef AXIS1_DRIVER_CODE_GOTO
    uint8_t _a1tg1, _a1tg2, _a1tg3, _a1t4, _a1g4;
    void axis1DriverInitFast() {
      uint32_t t=tmcAxis1._last_chop_config+(((uint32_t)AXIS1_DRIVER_CODE)<<24);
      uint32_t g=tmcAxis1._last_chop_config+(((uint32_t)AXIS1_DRIVER_CODE_GOTO)<<24);
      _a1t4 =  (t >> 24) & 0xff;
      _a1g4 =  (g >> 24) & 0xff;
      _a1tg3 = (t >> 16) & 0xff;
      _a1tg2 = (t >>  8) & 0xff;
      _a1tg1 = (t      ) & 0xff;
    }
    IRAM_ATTR void axis1DriverTrackingFast() {
      a1CS_L; delaySPI;
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((0x6C|0x80)&i) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1t4)&i    ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg3)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg2)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg1)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      a1CS_H; delaySPI;
    }
    IRAM_ATTR void axis1DriverGotoFast() {
      a1CS_L; delaySPI;
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((0x6C|0x80)&i) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1g4)&i    ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg3)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg2)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a1CLK_L; if ((_a1tg1)&i   ) a1SDO_H; else a1SDO_L; delaySPI; a1CLK_H; delaySPI; }
      a1CS_H; delaySPI;
    }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    uint8_t _a2tg1, _a2tg2, _a2tg3, _a2t4, _a2g4;
    void axis2DriverInitFast() {
      uint32_t t=tmcAxis2._last_chop_config+(((uint32_t)AXIS2_DRIVER_CODE)<<24);
      uint32_t g=tmcAxis2._last_chop_config+(((uint32_t)AXIS2_DRIVER_CODE_GOTO)<<24);
      _a2t4 =  (t >> 24) & 0xff;
      _a2g4 =  (g >> 24) & 0xff;
      _a2tg3 = (t >> 16) & 0xff;
      _a2tg2 = (t >>  8) & 0xff;
      _a2tg1 = (t      ) & 0xff;
    }
    IRAM_ATTR void axis2DriverTrackingFast() {
      a2CS_L; delaySPI;
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((0x6C|0x80)&i) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2t4)&i    ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg3)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg2)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg1)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      a2CS_H; delaySPI;
    }
    IRAM_ATTR void axis2DriverGotoFast() {
      a2CS_L; delaySPI;
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((0x6C|0x80)&i) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2g4)&i    ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg3)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg2)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      for(uint8_t i=128; i >= 1; i/=2) { a2CLK_L; if ((_a2tg1)&i   ) a2SDO_H; else a2SDO_L; delaySPI; a2CLK_H; delaySPI; }
      a2CS_H; delaySPI;
    }
  #endif
#endif

#else
// ---------------------------------------------------------------------------------------------------
// traditional s/d stepper drivers

boolean _a1trk=false;
void axis1DriverTrackingMode(boolean init) {
  if (_a1trk) return; else _a1trk=true;

#if AXIS1_DRIVER_DECAY_MODE != OFF
  if (AXIS1_DRIVER_DECAY_MODE == OPEN) pinMode(Axis1_DECAY,INPUT); else { pinMode(Axis1_DECAY,OUTPUT); digitalWrite(Axis1_DECAY,AXIS1_DRIVER_DECAY_MODE); }
#endif
#ifdef AXIS1_DRIVER_CODE
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) {
    haltStepperDrivers();
    stepAxis1=1;
    if ((AXIS1_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,bitRead(AXIS1_DRIVER_CODE,0)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,bitRead(AXIS1_DRIVER_CODE,1)); } else { pinMode(Axis1_M1,INPUT); }
    #ifndef AXIS1_DRIVER_DISABLE_M2
      if ((AXIS1_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,bitRead(AXIS1_DRIVER_CODE,2)); } else { pinMode(Axis1_M2,INPUT); }
    #endif
    if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
    resumeStepperDrivers();
  }
#endif
}

boolean _a2trk=false;
void axis2DriverTrackingMode(boolean init) {
  if (_a2trk) return; else _a2trk=true;

#if AXIS2_DRIVER_DECAY_MODE != OFF
  if (AXIS2_DRIVER_DECAY_MODE == OPEN) pinMode(Axis2_DECAY,INPUT); else { pinMode(Axis2_DECAY,OUTPUT); digitalWrite(Axis2_DECAY,AXIS2_DRIVER_DECAY_MODE); }
#endif
#if defined(AXIS2_DRIVER_CODE)
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) {
    haltStepperDrivers();
    stepAxis2=1;
    if ((AXIS2_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,bitRead(AXIS2_DRIVER_CODE,0)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,bitRead(AXIS2_DRIVER_CODE,1)); } else { pinMode(Axis2_M1,INPUT); }
    #ifndef AXIS2_DRIVER_DISABLE_M2
      if ((AXIS2_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,bitRead(AXIS2_DRIVER_CODE,2)); } else { pinMode(Axis2_M2,INPUT); }
    #endif
    if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
    resumeStepperDrivers();
  }
#endif
}

void axis1DriverGotoMode() {
  if (!_a1trk) return; else _a1trk=false;

#if AXIS1_DRIVER_DECAY_MODE_GOTO != OFF
  if (AXIS1_DRIVER_DECAY_MODE_GOTO == OPEN) pinMode(Axis1_DECAY,INPUT); else { pinMode(Axis1_DECAY,OUTPUT); digitalWrite(Axis1_DECAY,AXIS1_DRIVER_DECAY_MODE_GOTO); }
#endif
#if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS1_DRIVER_CODE_GOTO)
  haltStepperDrivers();
  stepAxis1=AXIS1_DRIVER_STEP_GOTO;
  if ((AXIS1_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,bitRead(AXIS1_DRIVER_CODE_GOTO,0)); } else { pinMode(Axis1_M0,INPUT); }
  if ((AXIS1_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,bitRead(AXIS1_DRIVER_CODE_GOTO,1)); } else { pinMode(Axis1_M1,INPUT); }
  #ifndef AXIS1_DRIVER_DISABLE_M2
    if ((AXIS1_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,bitRead(AXIS1_DRIVER_CODE_GOTO,2)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
  resumeStepperDrivers();
#endif
}

void axis2DriverGotoMode() {
  if (!_a2trk) return; else _a2trk=false;
  
#if AXIS2_DRIVER_DECAY_MODE_GOTO != OFF
  if (AXIS2_DRIVER_DECAY_MODE_GOTO == OPEN) pinMode(Axis2_DECAY,INPUT); else { pinMode(Axis2_DECAY,OUTPUT); digitalWrite(Axis2_DECAY,AXIS2_DRIVER_DECAY_MODE_GOTO); }
#endif
#if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS2_DRIVER_CODE_GOTO)
  haltStepperDrivers();
  stepAxis2=AXIS2_DRIVER_STEP_GOTO;
  if ((AXIS2_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,bitRead(AXIS2_DRIVER_CODE_GOTO,0)); } else { pinMode(Axis2_M0,INPUT); }
  if ((AXIS2_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,bitRead(AXIS2_DRIVER_CODE_GOTO,1)); } else { pinMode(Axis2_M1,INPUT); }
  #ifndef AXIS2_DRIVER_DISABLE_M2
    if ((AXIS2_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,bitRead(AXIS2_DRIVER_CODE_GOTO,2)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
  if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
  resumeStepperDrivers();
#endif
}

#if MODE_SWITCH_BEFORE_SLEW == OFF
  #ifdef AXIS1_DRIVER_CODE_GOTO
    void axis1DriverInitFast() {
    }
    IRAM_ATTR void axis1DriverTrackingFast() {
      a1M0(bitRead(AXIS1_DRIVER_CODE,0));
      a1M1(bitRead(AXIS1_DRIVER_CODE,1));
    #ifndef AXIS1_DRIVER_DISABLE_M2
      a1M2(bitRead(AXIS1_DRIVER_CODE,2));
    #endif
    }
    IRAM_ATTR void axis1DriverGotoFast() {
      a1M0(bitRead(AXIS1_DRIVER_CODE_GOTO,0));
      a1M1(bitRead(AXIS1_DRIVER_CODE_GOTO,1));
    #ifndef AXIS1_DRIVER_DISABLE_M2
      a1M2(bitRead(AXIS1_DRIVER_CODE_GOTO,2));
    #endif
    }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    void axis2DriverInitFast() {
    }
    IRAM_ATTR void axis2DriverTrackingFast() {
      a2M0(bitRead(AXIS2_DRIVER_CODE,0));
      a2M1(bitRead(AXIS2_DRIVER_CODE,1));
    #ifndef AXIS2_DRIVER_DISABLE_M2
      a2M2(bitRead(AXIS2_DRIVER_CODE,2));
    #endif
    }
    IRAM_ATTR void axis2DriverGotoFast() {
      a2M0(bitRead(AXIS2_DRIVER_CODE_GOTO,0));
      a2M1(bitRead(AXIS2_DRIVER_CODE_GOTO,1));
    #ifndef AXIS2_DRIVER_DISABLE_M2
      a2M2(bitRead(AXIS2_DRIVER_CODE_GOTO,2));
    #endif
    }
  #endif
#endif

#endif

// ---------------------------------------------------------------------------------------------------
// basic driver control functions

#define WAIT_DRIVER_ENABLE 5
void enableStepperDrivers() {
  // enable the stepper drivers
  if (axis1Enabled == false) {
    digitalWrite(Axis1_EN,AXIS1_DRIVER_ENABLE); axis1Enabled=true;
    digitalWrite(Axis2_EN,AXIS2_DRIVER_ENABLE); axis2Enabled=true;
    delay(WAIT_DRIVER_ENABLE);
  }
}

void disableStepperDrivers() {
  // disable the stepper drivers
  if (axis1Enabled == true) {
    digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE); axis1Enabled=false;
    digitalWrite(Axis2_EN,AXIS2_DRIVER_DISABLE); axis2Enabled=false;
    delay(WAIT_DRIVER_ENABLE);
  }
}

void haltStepperDrivers() {
  haltAxis1=true;
  haltAxis2=true;
}

void resumeStepperDrivers() {
  haltAxis1=false;
  haltAxis2=false;
}
