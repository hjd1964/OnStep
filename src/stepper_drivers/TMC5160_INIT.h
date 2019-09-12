// TMC5160 stepper driver init

#if AXIS1_DRIVER_MODEL == TMC5160
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #ifndef AXIS1_TMC_IHOLD
    #define AXIS1_TMC_IHOLD 300 // peak current in mA (0.3A)
  #endif
  #ifndef AXIS1_TMC_IRUN
    #define AXIS1_TMC_IRUN 600 // peak current in mA (0.6A)
  #endif
  #ifndef AXIS1_TMC_RSENSE
    #define AXIS1_TMC_RSENSE 0.075
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC5160
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #ifndef AXIS2_TMC_IHOLD
    #define AXIS2_TMC_IHOLD 300
  #endif
  #ifndef AXIS2_TMC_IRUN
    #define AXIS2_TMC_IRUN 600
  #endif
  #ifndef AXIS2_TMC_RSENSE
    #define AXIS2_TMC_RSENSE 0.075
  #endif
#endif
