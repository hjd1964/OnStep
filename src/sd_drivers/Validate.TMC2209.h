// TMC2209 stepper driver validate

#if AXIS1_DRIVER_MODEL == TMC2209
  #if AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16 && AXIS1_DRIVER_MICROSTEPS != 32 && AXIS1_DRIVER_MICROSTEPS != 64
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; TMC2209 invalid micro-step mode, use: 64,32,16,or 8"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16 && AXIS1_DRIVER_MICROSTEPS_GOTO != 32 && AXIS1_DRIVER_MICROSTEPS_GOTO != 64
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; TMC2209 invalid micro-step mode, use: 64,32,16,8,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < TMC2209_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the TMC2209 stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC2209
  #if AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16 && AXIS2_DRIVER_MICROSTEPS != 32 && AXIS2_DRIVER_MICROSTEPS != 64
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; TMC2209 invalid micro-step mode, use: 64,32,16,or 8"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16 && AXIS2_DRIVER_MICROSTEPS_GOTO != 32 && AXIS2_DRIVER_MICROSTEPS_GOTO != 64
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; TMC2209 invalid micro-step mode, use: 64,32,16,8,or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < TMC2209_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the TMC2209 stepper driver specifications."
  #endif
#endif
