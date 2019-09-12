// TMC5160 stepper driver init

#if AXIS1_DRIVER_MODEL == TMC5160_QUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC5160
  #define AXIS1_TMC_MODE STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC5160_VQUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC5160
  #define AXIS1_TMC_MODE STEALTHCHOP
  #define AXIS1_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC5160_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#elif AXIS1_DRIVER_MODEL == TMC5160_QUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#elif AXIS1_DRIVER_MODEL == TMC5160_VQUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#endif

#if AXIS2_DRIVER_MODEL == TMC5160_QUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC5160
  #define AXIS2_TMC_MODE STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC5160_VQUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC5160
  #define AXIS2_TMC_MODE STEALTHCHOP
  #define AXIS2_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC5160_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#elif AXIS2_DRIVER_MODEL == TMC5160_QUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#elif AXIS2_DRIVER_MODEL == TMC5160_VQUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC5160 stepper drivers, use AXISn_TMC_IRUN and _IGOTO to set the pwr levels."
#endif

#if AXIS1_DRIVER_MODEL == TMC5160
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #ifndef AXIS1_TMC_IRUN
    #warning "Configuration: AXIS1_TMC_IRUN current isn't specified, falling back to 600 (mA.)"
    #define AXIS1_TMC_IRUN 600 // peak current in mA (0.6A)
  #endif
  #ifndef AXIS1_TMC_RSENSE
    #define AXIS1_TMC_RSENSE 0.075
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC5160
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #ifndef AXIS2_TMC_IRUN
    #warning "Configuration: AXIS2_TMC_IRUN current isn't specified, falling back to 600 (mA.)"
    #define AXIS2_TMC_IRUN 600
  #endif
  #ifndef AXIS2_TMC_RSENSE
    #define AXIS2_TMC_RSENSE 0.075
  #endif
#endif
