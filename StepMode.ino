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
#if !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // automatic mode switching during slews, initialize micro-step mode
  #ifdef AXIS1_MICROSTEP_CODE
    if ((AXIS1_MICROSTEP_CODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MICROSTEP_CODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MICROSTEP_CODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MICROSTEP_CODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MICROSTEP_CODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MICROSTEP_CODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  
  #ifdef AXIS2_MICROSTEP_CODE
    if ((AXIS2_MICROSTEP_CODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MICROSTEP_CODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MICROSTEP_CODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MICROSTEP_CODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MICROSTEP_CODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MICROSTEP_CODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#endif
}

// if stepper drive can switch decay mode, set it here
void stepperModeTracking(boolean init_tmc) {
  if (_stepperModeTrack) return;
  _stepperModeTrack=true;
  cli();

#ifdef AXIS1_DECAY_MODE
  #if AXIS1_DECAY_MODE==OPEN
    pinModeOpen(Axis1_Mode);
  #elif AXIS1_DECAY_MODE==LOW
    pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  #elif AXIS1_DECAY_MODE==HIGH
    pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  #endif
#endif

#ifdef AXIS2_DECAY_MODE
  #if AXIS2_DECAY_MODE==OPEN
    pinModeOpen(Axis1_Mode);
  #elif AXIS2_DECAY_MODE==LOW
    pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  #elif AXIS2_DECAY_MODE==HIGH
    pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  #endif
#endif

#if defined(MODE_SWITCH_BEFORE_SLEW_ON)
  #ifdef AXIS1_MICROSTEP_CODE
    #ifdef AXIS1_MICROSTEP_CODE_GOTO
    stepAxis1=1;
    #endif
    if ((AXIS1_MICROSTEP_CODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MICROSTEP_CODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MICROSTEP_CODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MICROSTEP_CODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MICROSTEP_CODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MICROSTEP_CODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MICROSTEP_CODE
    #ifdef AXIS2_MICROSTEP_CODE_GOTO
    stepAxis2=1;
    #endif
    if ((AXIS2_MICROSTEP_CODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MICROSTEP_CODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MICROSTEP_CODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MICROSTEP_CODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MICROSTEP_CODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MICROSTEP_CODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  stepAxis1=1;
  stepAxis2=1;
  #if (AXIS1_TMC_MODE==STEALTHCHOP) || (AXIS2_TMC_MODE==STEALTHCHOP)
    if (init_tmc) {
      tmcAxis1.setup(AXIS1_TMC_INTPOL,AXIS1_TMC_MODE,AXIS1_MICROSTEP_CODE&0b001111,AXIS1_TMC_IRUN,AXIS2_TMC_IRUN,AXIS1_TMC_RSENSE);
      tmcAxis2.setup(AXIS2_TMC_INTPOL,AXIS2_TMC_MODE,AXIS2_MICROSTEP_CODE&0b001111,AXIS2_TMC_IRUN,AXIS2_TMC_IRUN,AXIS2_TMC_RSENSE);
      delay(150);
    }
  #endif
  tmcAxis1.setup(AXIS1_TMC_INTPOL,AXIS1_TMC_MODE,AXIS1_MICROSTEP_CODE&0b001111,AXIS1_TMC_IRUN,AXIS1_TMC_IHOLD,AXIS1_TMC_RSENSE);
  tmcAxis2.setup(AXIS2_TMC_INTPOL,AXIS2_TMC_MODE,AXIS2_MICROSTEP_CODE&0b001111,AXIS2_TMC_IRUN,AXIS2_TMC_IHOLD,AXIS2_TMC_RSENSE);
#endif

#ifdef MODE_SWITCH_SLEEP_ON
  if (atHome) delay(20);
  delay(3);
#endif
  sei();
}

void stepperModeGoto() {
  if (!_stepperModeTrack) return;
  _stepperModeTrack=false;
  cli();
#if defined(DECAY_MODE_GOTO) && (DECAY_MODE_GOTO==OPEN)
  pinModeOpen(Axis1_Mode);
  pinModeOpen(Axis2_Mode);
#elif defined(DECAY_MODE_GOTO) && (DECAY_MODE_GOTO==LOW)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
#elif defined(DECAY_MODE_GOTO) && (DECAY_MODE_GOTO==HIGH)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis2_Mode,HIGH);
#elif defined(MODE_SWITCH_BEFORE_SLEW_ON)
  #ifdef AXIS1_MICROSTEP_CODE_GOTO
    stepAxis1=AXIS1_STEP_GOTO;
    if ((AXIS1_MICROSTEP_CODE_GOTO & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MICROSTEP_CODE_GOTO    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MICROSTEP_CODE_GOTO & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MICROSTEP_CODE_GOTO>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MICROSTEP_CODE_GOTO & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MICROSTEP_CODE_GOTO>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MICROSTEP_CODE_GOTO
    stepAxis2=AXIS2_STEP_GOTO;
    if ((AXIS2_MICROSTEP_CODE_GOTO & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MICROSTEP_CODE_GOTO    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MICROSTEP_CODE_GOTO & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MICROSTEP_CODE_GOTO>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MICROSTEP_CODE_GOTO & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MICROSTEP_CODE_GOTO>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  stepAxis1=AXIS1_STEP_GOTO;
  tmcAxis1.setup(AXIS1_TMC_INTPOL,AXIS1_TMC_MODE_GOTO,AXIS1_MICROSTEP_CODE_GOTO&0b001111,AXIS1_TMC_IGOTO,AXIS1_TMC_IHOLD,AXIS1_TMC_RSENSE);
  stepAxis2=AXIS2_STEP_GOTO;
  tmcAxis2.setup(AXIS2_TMC_INTPOL,AXIS2_TMC_MODE_GOTO,AXIS2_MICROSTEP_CODE_GOTO&0b001111,AXIS2_TMC_IGOTO,AXIS2_TMC_IHOLD,AXIS2_TMC_RSENSE);
#endif

#ifdef MODE_SWITCH_SLEEP_ON
  delay(3);
#endif
  sei();
}

void enableStepperDrivers() {
  // enable the stepper drivers
  if (axis1Enabled==false) {
    digitalWrite(Axis1_EN,AXIS1_ENABLE); axis1Enabled=true;
    digitalWrite(Axis2_EN,AXIS2_ENABLE); axis2Enabled=true;
    delay(5); // enable or coming out of sleep on DRV8825 or A4988 is done in <2ms
  }
}

void disableStepperDrivers() {
  // disable the stepper drivers
  if (axis1Enabled==true) {
    digitalWrite(Axis1_EN,AXIS1_DISABLE); axis1Enabled=false;
    digitalWrite(Axis2_EN,AXIS2_DISABLE); axis2Enabled=false;
    delay(5); // enable or coming out of sleep on DRV8825 or A4988 is done in <2ms
  }
}
