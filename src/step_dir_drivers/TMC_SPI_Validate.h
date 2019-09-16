// TMC_SPI stepper driver validate

#if AXIS1_DRIVER_MODEL == TMC_SPI
  #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64 && AXIS1_MICROSTEPS!=128 && AXIS1_MICROSTEPS!=256
    #error "Configuration: AXIS1_MICROSTEPS; TMC SPI stepper driver invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
  #endif
  #if AXIS2_DRIVER_MODEL != TMC_SPI
    #error "Configuration: TMC SPI stepper drivers must be used in pairs (both Axis1 and Axis2.)"
  #endif
  #ifdef AXIS1_MICROSTEPS_GOTO
    #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64 && AXIS1_MICROSTEPS_GOTO!=128 && AXIS1_MICROSTEPS_GOTO!=256
      #error "Configuration: AXIS1_MICROSTEPS_GOTO; TMC SPI stepper driver invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
    #if AXIS1_MICROSTEPS != AXIS1_MICROSTEPS_GOTO
      #warning "Configuration: AXIS2_MICROSTEPS_GOTO; is NOT _OFF.  This can effect pointing accuracy and PEC if index sensing isn't used."
    #endif
  #endif
  #if HAL_PULSE_WIDTH < TMC_SPI_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the TMC SPI stepper driver specifications."
  #endif
#endif
#if AXIS2_DRIVER_MODEL == TMC_SPI
  #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64 && AXIS2_MICROSTEPS!=128 && AXIS2_MICROSTEPS!=256
    #error "Configuration: AXIS2_MICROSTEPS; TMC SPI stepper driver invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
  #endif
  #ifdef AXIS2_MICROSTEPS_GOTO
    #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64 && AXIS2_MICROSTEPS_GOTO!=128 && AXIS2_MICROSTEPS_GOTO!=256
      #error "Configuration: AXIS2_MICROSTEPS_GOTO; TMC SPI stepper driver invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
    #if AXIS2_MICROSTEPS != AXIS2_MICROSTEPS_GOTO
      #warning "Configuration: AXIS2_MICROSTEPS_GOTO; is NOT _OFF.  This can effect pointing accuracy."
    #endif
  #endif
  #if HAL_PULSE_WIDTH < TMC_SPI_PULSE_WIDTH
    #error "Configuration: STEP_WAVE_FORM PULSE; Pulse width is below the TMC SPI stepper driver specifications."
  #endif
#endif
