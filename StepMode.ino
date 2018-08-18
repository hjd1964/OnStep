// -----------------------------------------------------------------------------------
// Stepper driver mode control

bool _stepperModeTrack=false;

// initialize stepper drivers
void StepperModeTrackingInit() {
  _stepperModeTrack=false; 

  // enable stepper drivers
  enableStepperDrivers();
  // program the mode
  stepperModeTracking();
  // then wait for 100ms so TMC2100 or TMC2130 in stealthChop have time to calibrate motor current if needed (a one-time event)
  delay(100);
  // then disable again
  disableStepperDrivers();

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
}

// if stepper drive can switch decay mode, set it here
void stepperModeTracking() {
  if (_stepperModeTrack) return;
  _stepperModeTrack=true;
  cli();
#if defined(DECAY_MODE) && (DECAY_MODE==OPEN)
  pinModeOpen(Axis1_Mode);
  pinModeOpen(Axis2_Mode);
#elif defined(DECAY_MODE) && (DECAY_MODE==LOW)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis1_Mode,LOW);
#elif defined(DECAY_MODE) && (DECAY_MODE==HIGH)
  pinMode(Axis1_Mode,OUTPUT); digitalWrite(Axis1_Mode,HIGH);
  pinMode(Axis2_Mode,OUTPUT); digitalWrite(Axis2_Mode,HIGH);
#elif defined(MODE_SWITCH_BEFORE_SLEW_ON)
  #ifdef AXIS1_MODE
    #ifdef AXIS1_MODE_GOTO 
    stepAxis1=1;
    #endif
    if ((AXIS1_MODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MODE
    #ifdef AXIS2_MODE_GOTO 
    stepAxis2=1;
    #endif
    if ((AXIS2_MODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  stepAxis1=1;
  bool nintpol=((AXIS1_MODE & 0b0010000)!=0);
  bool stealth=((AXIS1_MODE & 0b0100000)!=0);
  bool lowpwr =((AXIS1_MODE & 0b1000000)!=0);
  tmcAxis1.setup(!nintpol,stealth,AXIS1_MODE&0b001111,lowpwr);

  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  stepAxis2=1;
  nintpol=((AXIS2_MODE & 0b0010000)!=0);
  stealth=((AXIS2_MODE & 0b0100000)!=0);
  lowpwr =((AXIS2_MODE & 0b1000000)!=0);
  tmcAxis2.setup(!nintpol,stealth,AXIS2_MODE&0b001111,lowpwr);

  // allow stealth chop current regulation to ramp up to the initial motor current before moving
  if ((((AXIS1_MODE & 0b0100000)!=0) || ((AXIS2_MODE & 0b0100000)!=0)) && (atHome)) delay(100);
#endif

#ifdef MODE_SWITCH_SLEEP_ON
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
  #ifdef AXIS1_MODE_GOTO
    stepAxis1=AXIS1_STEP_GOTO;
    if ((AXIS1_MODE_GOTO & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE_GOTO    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE_GOTO & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE_GOTO>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE_GOTO & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE_GOTO>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  #ifdef AXIS2_MODE_GOTO
    stepAxis2=AXIS2_STEP_GOTO;
    if ((AXIS2_MODE_GOTO & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE_GOTO    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE_GOTO & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE_GOTO>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE_GOTO & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE_GOTO>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#elif defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  stepAxis1=AXIS1_STEP_GOTO;
  bool nintpol=((AXIS1_MODE_GOTO & 0b0010000)!=0);
  bool stealth=((AXIS1_MODE_GOTO & 0b0100000)!=0);
  bool lowpwr =((AXIS1_MODE_GOTO & 0b1000000)!=0);
  tmcAxis1.setup(!nintpol,stealth,AXIS1_MODE_GOTO&0b001111,lowpwr);

  // default 256x interpolation ON, stealthChop OFF (spreadCycle), micro-steps
  stepAxis2=AXIS2_STEP_GOTO;
  nintpol=((AXIS2_MODE_GOTO & 0b0010000)!=0);
  stealth=((AXIS2_MODE_GOTO & 0b0100000)!=0);
  lowpwr =((AXIS2_MODE_GOTO & 0b1000000)!=0);
  tmcAxis2.setup(!nintpol,stealth,AXIS2_MODE_GOTO&0b001111,lowpwr);
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

