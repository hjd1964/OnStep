// TMC2209 stepper driver validate

#if AXIS1_DRIVER_MODEL == TMC2209
  #if AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64
    #error "Configuration: AXIS1_MICROSTEPS; TMC2208 invalid micro-step mode, use: 64,32,16,or 8"
  #endif
  #if HAL_PULSE_WIDTH < TMC2209_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the TMC2209 stepper driver specifications."
  #endif
#endif
#if AXIS2_DRIVER_MODEL == TMC2209
  #if AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64
    #error "Configuration: AXIS2_MICROSTEPS; TMC2209 invalid micro-step mode, use: 64,32,16,or 8"
  #endif
  #if HAL_PULSE_WIDTH < TMC2209_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the TMC2209 stepper driver specifications."
  #endif
#endif
#ifdef AXIS1_MICROSTEPS_GOTO
  #if AXIS1_DRIVER_MODEL == TMC2209
    #if AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64
      #error "Configuration: AXIS1_MICROSTEPS_GOTO; TMC2209 invalid micro-step mode, use: 64,32,16,8,or _OFF"
    #endif
  #endif
#endif
#ifdef AXIS2_MICROSTEPS_GOTO
  #if AXIS2_DRIVER_MODEL == TMC2209
    #if AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64
      #error "Configuration: AXIS2_MICROSTEPS_GOTO; TMC2209 invalid micro-step mode, use: 64,32,16,8,or _OFF"
    #endif
  #endif
#endif
