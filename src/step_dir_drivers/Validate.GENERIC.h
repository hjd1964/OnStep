// GENERIC stepper driver validate

#if AXIS1_DRIVER_MODEL == GENERIC
  #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16 && AXIS1_DRIVER_MICROSTEPS != 32 && AXIS1_DRIVER_MICROSTEPS != 64 && AXIS1_DRIVER_MICROSTEPS != 128 && AXIS1_DRIVER_MICROSTEPS != 256
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; GENERIC stepper driver invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; GENERIC stepper driver invalid micro-step mode, must be OFF"
  #endif
  #if HAL_PULSE_WIDTH < GENERIC_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the GENERIC stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == GENERIC
  #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16 && AXIS2_DRIVER_MICROSTEPS != 32 && AXIS2_DRIVER_MICROSTEPS != 64 && AXIS2_DRIVER_MICROSTEPS != 128 && AXIS2_DRIVER_MICROSTEPS != 256
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; GENERIC invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; GENERIC stepper driver invalid micro-step mode, must be OFF"
  #endif
  #if HAL_PULSE_WIDTH < GENERIC_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the GENERIC stepper driver specifications."
  #endif
#endif
