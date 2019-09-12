// TMC2100 stepper driver validate

#if AXIS1_DRIVER_MODEL == TMC2100
  #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=16
    #error "Configuration: AXIS1_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
  #endif
  #warning "Configuration: AXIS1_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: _OFF here"
  #if HAL_PULSE_WIDTH < TMC2100_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the TMC2100 stepper driver specifications."
  #endif
#endif
#if AXIS2_DRIVER_MODEL == TMC2100
  #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=16
    #error "Configuration: AXIS2_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
  #endif
  #warning "Configuration: AXIS2_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: _OFF here"
  #if HAL_PULSE_WIDTH < TMC2100_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width exceeds the TMC2100 stepper driver specifications."
  #endif
#endif
#ifdef AXIS1_MICROSTEPS_GOTO
  #if AXIS1_DRIVER_MODEL == TMC2100
    #if AXIS1_MICROSTEPS != AXIS1_MICROSTEPS_GOTO
      #error "Configuration: AXIS1_MICROSTEPS_GOTO; must be set to the same value as AXIS1_MICROSTEPS,or _OFF"
    #endif
  #endif
#endif
#ifdef AXIS2_MICROSTEPS_GOTO
  #if AXIS2_DRIVER_MODEL == TMC2100
    #if AXIS2_MICROSTEPS != AXIS2_MICROSTEPS_GOTO
      #error "Configuration: AXIS2_MICROSTEPS_GOTO; must be set to the same value as AXIS2_MICROSTEPS,or _OFF"
    #endif
  #endif
#endif
