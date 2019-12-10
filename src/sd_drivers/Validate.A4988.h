// A4988 stepper driver validate

#if AXIS1_DRIVER_MODEL == A4988
  #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 1 && AXIS1_DRIVER_MICROSTEPS_GOTO != 2 && AXIS1_DRIVER_MICROSTEPS_GOTO != 4 && AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < A4988_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the A4988 stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == A4988
  #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 1 && AXIS2_DRIVER_MICROSTEPS_GOTO != 2 && AXIS2_DRIVER_MICROSTEPS_GOTO != 4 && AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < A4988_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the A4988 stepper driver specifications."
  #endif
#endif
