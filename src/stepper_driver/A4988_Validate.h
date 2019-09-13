// A4988 stepper driver validate

#if AXIS1_DRIVER_MODEL == A4988
  #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16
    #error "Configuration: AXIS1_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
  #endif
  #if HAL_PULSE_WIDTH < A4988_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the A4988 stepper driver specifications."
  #endif
#endif
#if AXIS2_DRIVER_MODEL == A4988
  #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16
    #error "Configuration: AXIS2_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
  #endif
  #if HAL_PULSE_WIDTH < A4988_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the A4988 stepper driver specifications."
  #endif
#endif
#ifdef AXIS1_MICROSTEPS_GOTO
  #if AXIS1_DRIVER_MODEL == A4988
    #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16
      #error "Configuration: AXIS1_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
    #endif
  #endif
#endif
#ifdef AXIS2_MICROSTEPS_GOTO
  #if AXIS2_DRIVER_MODEL == A4988
    #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16
      #error "Configuration: AXIS2_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
    #endif
  #endif
#endif
