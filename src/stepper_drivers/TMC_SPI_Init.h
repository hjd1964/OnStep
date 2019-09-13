// TMC SPI stepper driver init

#if (AXIS1_DRIVER_MODEL == TMC_SPI) || (AXIS2_DRIVER_MODEL == TMC_SPI)
  #define MODE_SWITCH_BEFORE_SLEW_SPI
#endif

#if AXIS1_DRIVER_MODEL == TMC_SPI
  #ifndef AXIS1_TMC_MODE
    #define AXIS1_TMC_MODE SPREADCYCLE
  #endif
  #ifndef AXIS1_TMC_MODE_GOTO
    #define AXIS1_TMC_MODE_GOTO SPREADCYCLE
  #endif
  #ifndef AXIS1_TMC_INTPOL
    #define AXIS1_TMC_INTPOL true
  #endif
  #ifndef AXIS1_TMC_IRUN
    #define AXIS1_TMC_IRUN 2500
  #endif
  #ifndef AXIS1_TMC_IHOLD
    #define AXIS1_TMC_IHOLD (AXIS1_TMC_IRUN/2)
  #endif
  #ifndef AXIS1_TMC_IGOTO
    #define AXIS1_TMC_IGOTO AXIS1_TMC_IRUN
  #endif
  #ifndef AXIS1_TMC_RSENSE
    #define AXIS1_TMC_RSENSE 0.11+0.02
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC_SPI
  #ifndef AXIS2_TMC_MODE
    #define AXIS2_TMC_MODE SPREADCYCLE
  #endif
  #ifndef AXIS2_TMC_MODE_GOTO
    #define AXIS2_TMC_MODE_GOTO SPREADCYCLE
  #endif
  #ifndef AXIS2_TMC_INTPOL
    #define AXIS2_TMC_INTPOL true
  #endif
  #ifndef AXIS2_TMC_IRUN
    #define AXIS2_TMC_IRUN 2500
  #endif
  #ifndef AXIS2_TMC_IHOLD
    #define AXIS2_TMC_IHOLD (AXIS2_TMC_IRUN/2)
  #endif
  #ifndef AXIS2_TMC_IGOTO
    #define AXIS2_TMC_IGOTO AXIS2_TMC_IRUN
  #endif
  #ifndef AXIS2_TMC_RSENSE
    #define AXIS2_TMC_RSENSE 0.11+0.02
  #endif
#endif
