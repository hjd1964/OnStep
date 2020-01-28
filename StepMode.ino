// -----------------------------------------------------------------------------------
// Stepper driver mode control

bool _stepperModeTrack=false;

// initialize stepper drivers
void StepperModeTrackingInit() {
  _stepperModeTrack=false; 

  // enable stepper drivers
  enableStepperDrivers();
  // program the mode and enable initialization, should it be required
  stepperModeTracking(true);
  // then disable again
  disableStepperDrivers();

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#if MODE_SWITCH_BEFORE_SLEW == OFF
  // automatic mode switching during slews, initialize micro-step mode
  #ifdef AXIS1_DRIVER_CODE
    if ((AXIS1_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_DRIVER_CODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_DRIVER_CODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_DRIVER_CODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  
  #ifdef AXIS2_DRIVER_CODE
    if ((AXIS2_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_DRIVER_CODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_DRIVER_CODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_DRIVER_CODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#endif
}

// if stepper drive can switch decay mode, set it here
void stepperModeTracking(boolean init_tmc) {
  if (_stepperModeTrack) return;
  _stepperModeTrack=true;
  cli();

#ifdef AXIS1_DRIVER_DECAY_MODE
  #if AXIS1_DRIVER_DECAY_MODE == OPEN
    pinModeOpen(Axis1ModePin);
  #elif AXIS1_DRIVER_DECAY_MODE == LOW
    pinMode(Axis1ModePin,OUTPUT); digitalWrite(Axis1ModePin,LOW);
  #elif AXIS1_DRIVER_DECAY_MODE == HIGH
    pinMode(Axis1ModePin,OUTPUT); digitalWrite(Axis1ModePin,HIGH);
  #endif
#endif

#ifdef AXIS2_DRIVER_DECAY_MODE
  #if AXIS2_DRIVER_DECAY_MODE == OPEN
    pinModeOpen(Axis1ModePin);
  #elif AXIS2_DRIVER_DECAY_MODE == LOW
    pinMode(Axis2ModePin,OUTPUT); digitalWrite(Axis2ModePin,LOW);
  #elif AXIS2_DRIVER_DECAY_MODE == HIGH
    pinMode(Axis2ModePin,OUTPUT); digitalWrite(Axis2ModePin,HIGH);
  #endif
#endif

#if MODE_SWITCH_BEFORE_SLEW == ON
  #ifdef AXIS1_DRIVER_CODE
    #ifdef AXIS1_DRIVER_CODE_GOTO
    stepAxis1=1;
    #endif
    if ((AXIS1_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_DRIVER_CODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_DRIVER_CODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_DRIVER_CODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_DRIVER_CODE
    #ifdef AXIS2_DRIVER_CODE_GOTO
    stepAxis2=1;
    #endif
    if ((AXIS2_DRIVER_CODE & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_DRIVER_CODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_DRIVER_CODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_DRIVER_CODE & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_DRIVER_CODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif MODE_SWITCH_BEFORE_SLEW == TMC_SPI
  stepAxis1=1;
  stepAxis2=1;
  if (init_tmc) {
    #if AXIS1_DRIVER_PWM_GRAD != OFF
      tmcAxis1.set_PWMCONF_PWM_GRAD(AXIS1_DRIVER_PWM_GRAD);
    #endif
    #if AXIS1_DRIVER_FS_VHIGH != OFF
      tmcAxis1.set_THIGH_value( round(12000000.0/(AXIS1_DRIVER_FS_VHIGH*(256.0/AXIS1_DRIVER_MICROSTEPS))) );
      tmcAxis1.set_CHOPCONF_vhighfs(1);
      tmcAxis1.set_CHOPCONF_vhighchm(1);
    #endif
    #if AXIS1_DRIVER_SC_VHIGH != OFF
      tmcAxis1.set_TPWMTHRS_value( round(12000000.0/(AXIS1_DRIVER_SC_VHIGH*(256.0/AXIS1_DRIVER_MICROSTEPS))) );
    #endif

    #if AXIS2_DRIVER_PWM_GRAD != OFF
      tmcAxis2.set_PWMCONF_PWM_GRAD(AXIS2_DRIVER_PWM_GRAD);
    #endif
    #if AXIS2_DRIVER_FS_VHIGH != OFF
      tmcAxis2.set_THIGH_value( round(12000000.0/(AXIS2_DRIVER_FS_VHIGH*(256.0/AXIS2_DRIVER_MICROSTEPS))) );
      tmcAxis2.set_CHOPCONF_vhighfs(1);
      tmcAxis2.set_CHOPCONF_vhighchm(1);
    #endif
    #if AXIS2_DRIVER_SC_VHIGH != OFF
      tmcAxis2.set_TPWMTHRS_value( round(12000000.0/(AXIS2_DRIVER_SC_VHIGH*(256.0/AXIS2_DRIVER_MICROSTEPS))) );
    #endif
    
    #if AXIS1_DRIVER_DECAY_MODE == STEALTHCHOP || AXIS2_DRIVER_DECAY_MODE == STEALTHCHOP
      tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE&0b001111,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_RSENSE,AXIS1_DRIVER_SUBMODEL);
      tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE&0b001111,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_RSENSE,AXIS2_DRIVER_SUBMODEL);
    #endif
    delay(150);
  }
  tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE,AXIS1_DRIVER_CODE&0b001111,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IHOLD,AXIS1_DRIVER_RSENSE,AXIS1_DRIVER_SUBMODEL);
  tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE,AXIS2_DRIVER_CODE&0b001111,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IHOLD,AXIS2_DRIVER_RSENSE,AXIS2_DRIVER_SUBMODEL);
#endif

#if MODE_SWITCH_SLEEP == ON
  if (atHome) delay(20);
  delay(3);
#endif
  sei();
}

void stepperModeGoto() {
  if (!_stepperModeTrack) return;
  _stepperModeTrack=false;
  cli();

#ifdef AXIS1_DRIVER_DECAY_MODE_GOTO
  #if AXIS1_DECAY_MODE_GOTO == OPEN
    pinModeOpen(Axis1ModePin);
  #elif AXIS1_DRIVER_DECAY_MODE_GOTO == LOW
    pinMode(Axis1ModePin,OUTPUT); digitalWrite(Axis1ModePin,LOW);
  #elif AXIS1_DRIVER_DECAY_MODE_GOTO == HIGH
    pinMode(Axis1ModePin,OUTPUT); digitalWrite(Axis1ModePin,HIGH);
  #endif
#endif

#ifdef AXIS2_DRIVER_DECAY_MODE_GOTO
  #if AXIS2_DRIVER_DECAY_MODE_GOTO == OPEN
    pinModeOpen(Axis1ModePin);
  #elif AXIS2_DRIVER_DECAY_MODE_GOTO == LOW
    pinMode(Axis2ModePin,OUTPUT); digitalWrite(Axis2ModePin,LOW);
  #elif AXIS2_DRIVER_DECAY_MODE_GOTO == HIGH
    pinMode(Axis2ModePin,OUTPUT); digitalWrite(Axis2ModePin,HIGH);
  #endif
#endif

#if MODE_SWITCH_BEFORE_SLEW == ON
  #ifdef AXIS1_DRIVER_CODE_GOTO
    stepAxis1=AXIS1_DRIVER_STEP_GOTO;
    if ((AXIS1_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_DRIVER_CODE_GOTO    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_DRIVER_CODE_GOTO>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_DRIVER_CODE_GOTO>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_DRIVER_CODE_GOTO
    stepAxis2=AXIS2_DRIVER_STEP_GOTO;
    if ((AXIS2_DRIVER_CODE_GOTO & 0b001000) == 0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_DRIVER_CODE_GOTO    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_DRIVER_CODE_GOTO & 0b010000) == 0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_DRIVER_CODE_GOTO>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_DRIVER_CODE_GOTO & 0b100000) == 0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_DRIVER_CODE_GOTO>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif MODE_SWITCH_BEFORE_SLEW == TMC_SPI
  stepAxis1=AXIS1_DRIVER_STEP_GOTO;
  tmcAxis1.setup(AXIS1_DRIVER_INTPOL,AXIS1_DRIVER_DECAY_MODE_GOTO,AXIS1_DRIVER_CODE_GOTO&0b001111,AXIS1_DRIVER_IGOTO,AXIS1_DRIVER_IHOLD,AXIS1_DRIVER_RSENSE,AXIS1_DRIVER_SUBMODEL);
  stepAxis2=AXIS2_DRIVER_STEP_GOTO;
  tmcAxis2.setup(AXIS2_DRIVER_INTPOL,AXIS2_DRIVER_DECAY_MODE_GOTO,AXIS2_DRIVER_CODE_GOTO&0b001111,AXIS2_DRIVER_IGOTO,AXIS2_DRIVER_IHOLD,AXIS2_DRIVER_RSENSE,AXIS2_DRIVER_SUBMODEL);
#endif

#if MODE_SWITCH_SLEEP == ON
  delay(3);
#endif
  sei();
}

void enableStepperDrivers() {
  // enable the stepper drivers
  if (axis1Enabled == false) {
    digitalWrite(Axis1_EN,AXIS1_DRIVER_ENABLE); axis1Enabled=true;
    digitalWrite(Axis2_EN,AXIS2_DRIVER_ENABLE); axis2Enabled=true;
    delay(5); // enable or coming out of sleep on DRV8825 or A4988 is done in < 2ms
  }
}

void disableStepperDrivers() {
  // disable the stepper drivers
  if (axis1Enabled == true) {
    digitalWrite(Axis1_EN,AXIS1_DRIVER_DISABLE); axis1Enabled=false;
    digitalWrite(Axis2_EN,AXIS2_DRIVER_DISABLE); axis2Enabled=false;
    delay(5); // enable or coming out of sleep on DRV8825 or A4988 is done in < 2ms
  }
}
