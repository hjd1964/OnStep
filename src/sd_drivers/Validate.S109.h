// S109 stepper driver validate

#if AXIS1_DRIVER_MODEL == S109
  #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16 && AXIS1_DRIVER_MICROSTEPS != 32
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; S109 invalid micro-step mode, use: 32,16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 1 && AXIS1_DRIVER_MICROSTEPS_GOTO != 2 && AXIS1_DRIVER_MICROSTEPS_GOTO != 4 && AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16 && AXIS1_DRIVER_MICROSTEPS_GOTO != 32
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; S109 invalid micro-step mode, use: 32,16,8,4,2,1,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < S109_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the S109 stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == S109
  #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16 && AXIS2_DRIVER_MICROSTEPS != 32
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; S109 invalid micro-step mode, use: 32,16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 1 && AXIS2_DRIVER_MICROSTEPS_GOTO != 2 && AXIS2_DRIVER_MICROSTEPS_GOTO != 4 && AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16 && AXIS2_DRIVER_MICROSTEPS_GOTO != 32
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; S109 invalid micro-step mode, use: 32,16,8,4,2,1,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < S109_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the S109 stepper driver specifications."
  #endif
#endif
