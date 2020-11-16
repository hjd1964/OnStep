// -----------------------------------------------------------------------------------
// Stepper driver mode control

// initialize stepper drivers
void StepperModeTrackingInit() {
  // enable stepper drivers
  enableStepperDrivers();

  // setup Axis1 and Axis2
#if AXIS1_DRIVER_MODEL == TMC_SPI
  VLF("MSG: Setting up Axis1/2 TMC stepper drivers");
  driversInitTmcMode();
  driversInitTmcStealthChop();
#endif

  axis1DriverTrackingMode(true);
  axis2DriverTrackingMode(true);

#if AXIS1_DRIVER_MODEL == TMC2209 || AXIS2_DRIVER_MODEL == TMC2209
  delay(150);
#else
  delay(1);
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
  haltAxis1=true; haltAxis2=true;
  
  #if AXIS1_DRIVER_DECAY_MODE == STEALTHCHOP || AXIS2_DRIVER_DECAY_MODE == STEALTHCHOP
    tmcAxis1.setup(AXIS1_DRIVER_INTPOL,STEALTHCHOP,AXIS1_DRIVER_CODE,axis1Settings.IRUN,axis1Settings.IRUN);
    tmcAxis2.setup(AXIS2_DRIVER_INTPOL,STEALTHCHOP,AXIS2_DRIVER_CODE,axis2Settings.IRUN,axis2Settings.IRUN);
    delay(150);
  #endif
  tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE,axis1Settings.IRUN,axis1SettingsEx.IHOLD);
  tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE,axis2Settings.IRUN,axis2SettingsEx.IHOLD);
  
  // resume motion
  haltAxis1=false; haltAxis2=false;
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
    tmcAxis1.set_THIGH_value( round(12000000.0/(AXIS1_DRIVER_FS_VHIGH*(256.0/axis1Settings.microsteps))) );
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
    tmcAxis2.set_THIGH_value( round(12000000.0/(AXIS2_DRIVER_FS_VHIGH*(256.0/axis2Settings.microsteps))) );
    tmcAxis2.set_CHOPCONF_vhighfs(1);
    tmcAxis2.set_CHOPCONF_vhighchm(1);
  #endif

  // stealthChop
  #if AXIS1_DRIVER_PWM_GRAD != OFF
    tmcAxis1.set_PWMCONF_PWM_GRAD(AXIS1_DRIVER_PWM_GRAD);
  #endif
  #if AXIS1_DRIVER_PWM_VHIGH_SC != OFF
    tmcAxis1.set_TPWMTHRS_value( round(12000000.0/(AXIS1_DRIVER_PWM_VHIGH_SC*(256.0/axis1Settings.microsteps))) );
  #endif

  #if AXIS2_DRIVER_PWM_GRAD != OFF
    tmcAxis2.set_PWMCONF_PWM_GRAD(AXIS2_DRIVER_PWM_GRAD);
  #endif
  #if AXIS2_DRIVER_PWM_VHIGH_SC != OFF
    tmcAxis2.set_TPWMTHRS_value( round(12000000.0/(AXIS2_DRIVER_PWM_VHIGH_SC*(256.0/axis2Settings.microsteps))) );
  #endif
}

volatile bool _a1trk=false;
void axis1DriverTrackingMode(bool init) {
  if (_a1trk) return;
  if (!tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,255,axis1Settings.IRUN,axis1SettingsEx.IHOLD)) return;
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) { haltAxis1=true; if (!tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE)) { haltAxis1=false; return; } stepAxis1=1; haltAxis1=false; }
  _a1trk=true;
}

volatile bool _a2trk=false;
void axis2DriverTrackingMode(bool init) {
  if (_a2trk) return;
  if (!tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,255,axis2Settings.IRUN,axis2SettingsEx.IHOLD)) return;
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) { haltAxis2=true; if (!tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE)) { haltAxis2=false; return; } stepAxis2=1; haltAxis2=false; }
  _a2trk=true;
}

void axis1DriverGotoMode() {
  if (!_a1trk) return;
  if (!tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE_GOTO,255,axis1SettingsEx.IGOTO,axis1SettingsEx.IHOLD)) return;
  #if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS1_DRIVER_CODE_GOTO)
    haltAxis1=true; if (!tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE_GOTO)) { haltAxis1=false; return; } stepAxis1=axis1StepsGoto; haltAxis1=false;
  #endif
  _a1trk=false;
}

void axis2DriverGotoMode() {
  if (!_a2trk) return;
  if (!tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE_GOTO,255,axis2SettingsEx.IGOTO,axis2SettingsEx.IHOLD)) return;
  #if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS2_DRIVER_CODE_GOTO)
    haltAxis2=true; if (!tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE_GOTO)) { haltAxis2=false; return; } stepAxis2=axis2StepsGoto; haltAxis2=false;
  #endif
  _a2trk=false;
}

#if MODE_SWITCH_BEFORE_SLEW == OFF
  enum ModeSwitchState {MSS_IDLE, MSS_READY, MSS_GOTO, MSS_DONE};

  #ifdef AXIS1_DRIVER_CODE_GOTO
    volatile ModeSwitchState axis1ModeSwitchState = MSS_IDLE;
    IRAM_ATTR void axis1DriverTrackingFast() { axis1ModeSwitchState = MSS_DONE; }
    IRAM_ATTR void axis1DriverGotoFast()     { axis1ModeSwitchState = MSS_READY; }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    volatile ModeSwitchState axis2ModeSwitchState = MSS_IDLE;
    IRAM_ATTR void axis2DriverTrackingFast() { axis2ModeSwitchState = MSS_DONE; }
    IRAM_ATTR void axis2DriverGotoFast()     { axis2ModeSwitchState = MSS_READY; }
  #endif

  void autoModeSwitch() {
  #ifdef AXIS1_DRIVER_CODE_GOTO
    if (axis1ModeSwitchState == MSS_READY) {
      haltAxis1=true;
      tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE_GOTO);
      stepAxis1=axis1StepsGoto;
      axis1ModeSwitchState = MSS_GOTO;
      haltAxis1=false;
    }
    if (axis1ModeSwitchState == MSS_DONE) {
      haltAxis1=true;
      tmcAxis1.refresh_CHOPCONF(AXIS1_DRIVER_CODE);
      stepAxis1=1;
      while (blAxis1 > backlashAxis1) { blAxis1--; posAxis1++; }
      while (blAxis1 < 0) { blAxis1++; posAxis1--; }
      axis1ModeSwitchState = MSS_IDLE;
      haltAxis1=false;
    }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    if (axis2ModeSwitchState == MSS_READY) {
      haltAxis2=true;
      tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE_GOTO);
      stepAxis2=axis2StepsGoto;
      axis2ModeSwitchState = MSS_GOTO;
      haltAxis2=false;
    }
    if (axis2ModeSwitchState == MSS_DONE) {
      haltAxis2=true;
      tmcAxis2.refresh_CHOPCONF(AXIS2_DRIVER_CODE);
      stepAxis2=1;
      while (blAxis2 > backlashAxis2) { blAxis2--; posAxis2++; }
      while (blAxis2 < 0) { blAxis2++; posAxis2--; }
      axis2ModeSwitchState = MSS_IDLE;
      haltAxis2=false;
    }
  #endif
  }
#endif

#else
// ---------------------------------------------------------------------------------------------------
// traditional s/d stepper drivers

bool _a1trk=false;
void axis1DriverTrackingMode(bool init) {
  if (_a1trk) return; else _a1trk=true;

#if AXIS1_DRIVER_DECAY_MODE != OFF
  if (AXIS1_DRIVER_DECAY_MODE == OPEN) pinMode(Axis1_DECAY,INPUT); else { pinMode(Axis1_DECAY,OUTPUT); digitalWrite(Axis1_DECAY,AXIS1_DRIVER_DECAY_MODE); }
#endif
#ifdef AXIS1_DRIVER_CODE
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) {
    haltAxis1=true;
    stepAxis1=1;
    if ((AXIS1_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,bitRead(AXIS1_DRIVER_CODE,0)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,bitRead(AXIS1_DRIVER_CODE,1)); } else { pinMode(Axis1_M1,INPUT); }
    #ifndef AXIS1_DRIVER_DISABLE_M2
      if ((AXIS1_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,bitRead(AXIS1_DRIVER_CODE,2)); } else { pinMode(Axis1_M2,INPUT); }
    #endif
    if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
    haltAxis1=false;
  }
#endif
}

bool _a2trk=false;
void axis2DriverTrackingMode(bool init) {
  if (_a2trk) return; else _a2trk=true;

#if AXIS2_DRIVER_DECAY_MODE != OFF
  if (AXIS2_DRIVER_DECAY_MODE == OPEN) pinMode(Axis2_DECAY,INPUT); else { pinMode(Axis2_DECAY,OUTPUT); digitalWrite(Axis2_DECAY,AXIS2_DRIVER_DECAY_MODE); }
#endif
#if defined(AXIS2_DRIVER_CODE)
  if (MODE_SWITCH_BEFORE_SLEW == ON || init) {
    haltAxis2=true;
    stepAxis2=1;
    if ((AXIS2_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,bitRead(AXIS2_DRIVER_CODE,0)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,bitRead(AXIS2_DRIVER_CODE,1)); } else { pinMode(Axis2_M1,INPUT); }
    #ifndef AXIS2_DRIVER_DISABLE_M2
      if ((AXIS2_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,bitRead(AXIS2_DRIVER_CODE,2)); } else { pinMode(Axis2_M2,INPUT); }
    #endif
    if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
    haltAxis2=false;
  }
#endif
}

void axis1DriverGotoMode() {
  if (!_a1trk) return; else _a1trk=false;

#if AXIS1_DRIVER_DECAY_MODE_GOTO != OFF
  if (AXIS1_DRIVER_DECAY_MODE_GOTO == OPEN) pinMode(Axis1_DECAY,INPUT); else { pinMode(Axis1_DECAY,OUTPUT); digitalWrite(Axis1_DECAY,AXIS1_DRIVER_DECAY_MODE_GOTO); }
#endif
#if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS1_DRIVER_CODE_GOTO)
  haltAxis1=true;
  stepAxis1=axis1StepsGoto;
  if ((AXIS1_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,bitRead(AXIS1_DRIVER_CODE_GOTO,0)); } else { pinMode(Axis1_M0,INPUT); }
  if ((AXIS1_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,bitRead(AXIS1_DRIVER_CODE_GOTO,1)); } else { pinMode(Axis1_M1,INPUT); }
  #ifndef AXIS1_DRIVER_DISABLE_M2
    if ((AXIS1_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,bitRead(AXIS1_DRIVER_CODE_GOTO,2)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
  haltAxis1=false;
#endif
}

void axis2DriverGotoMode() {
  if (!_a2trk) return; else _a2trk=false;
  
#if AXIS2_DRIVER_DECAY_MODE_GOTO != OFF
  if (AXIS2_DRIVER_DECAY_MODE_GOTO == OPEN) pinMode(Axis2_DECAY,INPUT); else { pinMode(Axis2_DECAY,OUTPUT); digitalWrite(Axis2_DECAY,AXIS2_DRIVER_DECAY_MODE_GOTO); }
#endif
#if MODE_SWITCH_BEFORE_SLEW == ON && defined(AXIS2_DRIVER_CODE_GOTO)
  haltAxis2=true;
  stepAxis2=axis2StepsGoto;
  if ((AXIS2_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,bitRead(AXIS2_DRIVER_CODE_GOTO,0)); } else { pinMode(Axis2_M0,INPUT); }
  if ((AXIS2_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,bitRead(AXIS2_DRIVER_CODE_GOTO,1)); } else { pinMode(Axis2_M1,INPUT); }
  #ifndef AXIS2_DRIVER_DISABLE_M2
    if ((AXIS2_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,bitRead(AXIS2_DRIVER_CODE_GOTO,2)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
  if (MODE_SWITCH_SLEEP == ON) delay(WAIT_MODE_SWITCH);
  haltAxis2=false;
#endif
}

#if MODE_SWITCH_BEFORE_SLEW == OFF
  #ifdef AXIS1_DRIVER_CODE_GOTO
    IRAM_ATTR void axis1DriverTrackingFast() {
      a1M0(bitRead(AXIS1_DRIVER_CODE,0));
      a1M1(bitRead(AXIS1_DRIVER_CODE,1));
    #ifndef AXIS1_DRIVER_DISABLE_M2
      a1M2(bitRead(AXIS1_DRIVER_CODE,2));
    #endif
      stepAxis1=1;
    }
    IRAM_ATTR void axis1DriverGotoFast() {
      a1M0(bitRead(AXIS1_DRIVER_CODE_GOTO,0));
      a1M1(bitRead(AXIS1_DRIVER_CODE_GOTO,1));
    #ifndef AXIS1_DRIVER_DISABLE_M2
      a1M2(bitRead(AXIS1_DRIVER_CODE_GOTO,2));
    #endif
      stepAxis1=axis1StepsGoto;
    }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    IRAM_ATTR void axis2DriverTrackingFast() {
      a2M0(bitRead(AXIS2_DRIVER_CODE,0));
      a2M1(bitRead(AXIS2_DRIVER_CODE,1));
    #ifndef AXIS2_DRIVER_DISABLE_M2
      a2M2(bitRead(AXIS2_DRIVER_CODE,2));
    #endif
      stepAxis2=1;
    }
    IRAM_ATTR void axis2DriverGotoFast() {
      a2M0(bitRead(AXIS2_DRIVER_CODE_GOTO,0));
      a2M1(bitRead(AXIS2_DRIVER_CODE_GOTO,1));
    #ifndef AXIS2_DRIVER_DISABLE_M2
      a2M2(bitRead(AXIS2_DRIVER_CODE_GOTO,2));
    #endif
      stepAxis2=axis2StepsGoto;
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
    #if AXIS1_DRIVER_MODEL == TMC_SPI && Axis1_EN == OFF
      tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE,axis1Settings.IRUN,axis1SettingsEx.IHOLD);
    #else
      digitalWrite(Axis1_EN,AXIS1_DRIVER_ENABLE);
    #endif
    axis1Enabled=true;

    #if AXIS2_DRIVER_MODEL == TMC_SPI && Axis2_EN == OFF
      tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE,axis2Settings.IRUN,axis2SettingsEx.IHOLD);
    #else
      if (Axis2_EN != OFF && Axis2_EN != SHARED) digitalWrite(Axis2_EN,AXIS2_DRIVER_ENABLE);
    #endif
    axis2Enabled=true;

    delay(WAIT_DRIVER_ENABLE);
    VLF("MSG: Axis1/2 stepper drivers enabled");
  }
}

void disableStepperDrivers() {
  // disable the stepper drivers
  if (axis1Enabled == true) {
    #if AXIS1_DRIVER_MODEL == TMC_SPI && Axis1_EN == OFF
      tmcAxis1.setup(AXIS1_DRIVER_INTPOL,STEALTHCHOP,AXIS1_DRIVER_CODE,0,0);
    #else
      digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE);
    #endif
    axis1Enabled=false;

    #if AXIS2_DRIVER_MODEL == TMC_SPI && Axis2_EN == OFF
      tmcAxis2.setup(AXIS2_DRIVER_INTPOL,STEALTHCHOP,AXIS2_DRIVER_CODE,0,0);
    #else
      if (Axis2_EN != OFF && Axis2_EN != SHARED) digitalWrite(Axis2_EN,AXIS2_DRIVER_DISABLE);
    #endif
    axis2Enabled=false;

    delay(WAIT_DRIVER_ENABLE);
    VLF("MSG: Axis1/2 stepper drivers disabled");
  }
}
