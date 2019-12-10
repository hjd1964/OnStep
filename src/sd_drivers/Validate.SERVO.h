// SERVO driver validate

#if AXIS1_DRIVER_MODEL == SERVO
  #if AXIS1_DRIVER_MICROSTEPS != 1
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; SERVO invalid digital gearing mode, you must use: 1"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 1 && AXIS1_DRIVER_MICROSTEPS_GOTO != 2 && AXIS1_DRIVER_MICROSTEPS_GOTO != 4 && AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16 && AXIS1_DRIVER_MICROSTEPS_GOTO != 32 && AXIS1_DRIVER_MICROSTEPS_GOTO != 64 && AXIS1_DRIVER_MICROSTEPS_GOTO != 128
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; SERVO invalid digital gearing mode, use: 128,64,32,16,8,4,2,1,or OFF"
    #endif
    #warning "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; SERVO digital gearing multiplies step size by this amount"
  #endif
  #if STEP_WAVE_FORM == PULSE
    #warning "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse mode can degrade step signal integrity (due to signal distance traveled) and/or exceed signal timing requirements in some cases."
    #if HAL_PULSE_WIDTH < GENERIC_PULSE_WIDTH
      #warning "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below 1uS, the SERVO driver may or may not support these speeds."
    #endif
  #endif
#endif

#if AXIS2_DRIVER_MODEL == SERVO
  #if AXIS2_DRIVER_MICROSTEPS != 1
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; SERVO invalid digital gearing mode, you must use: 1"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 1 && AXIS2_DRIVER_MICROSTEPS_GOTO != 2 && AXIS2_DRIVER_MICROSTEPS_GOTO != 4 && AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16 && AXIS2_DRIVER_MICROSTEPS_GOTO != 32 && AXIS2_DRIVER_MICROSTEPS_GOTO != 64 && AXIS2_DRIVER_MICROSTEPS_GOTO != 128
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; SERVO invalid digital gearing mode, use: 128,64,32,16,8,4,2,1,or OFF"
    #endif
    #warning "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; SERVO digital gearing multiplies step size by this amount"
  #endif
  #if STEP_WAVE_FORM == PULSE
    #warning "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse mode can degrade step signal integrity (due to signal distance traveled) and/or exceed signal timing requirements in some cases."
    #if HAL_PULSE_WIDTH < GENERIC_PULSE_WIDTH
      #warning "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below 1uS, the SERVO driver may or may not support these speeds."
    #endif
  #endif
#endif
