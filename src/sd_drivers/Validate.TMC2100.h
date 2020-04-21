// TMC2100 stepper driver validate

#if AXIS1_DRIVER_MODEL == TMC2100
  #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && AXIS1_DRIVER_MICROSTEPS != 16
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
  #endif
  #warning "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: OFF here"
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS1_DRIVER_MICROSTEPS != AXIS1_DRIVER_MICROSTEPS_GOTO
      #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO; must be set to the same value as AXIS1_DRIVER_MICROSTEPS or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < TMC2100_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width is below the TMC2100 stepper driver specifications."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC2100
  #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && AXIS2_DRIVER_MICROSTEPS != 16
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
  #endif
  #warning "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: OFF here"
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MICROSTEPS != AXIS2_DRIVER_MICROSTEPS_GOTO
      #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO; must be set to the same value as AXIS2_DRIVER_MICROSTEPS or OFF"
    #endif
  #endif
  #if STEP_WAVE_FORM == PULSE && HAL_PULSE_WIDTH < TMC2100_PULSE_WIDTH
    #error "Configuration (Config.h): STEP_WAVE_FORM PULSE; Pulse width exceeds the TMC2100 stepper driver specifications."
  #endif
#endif
