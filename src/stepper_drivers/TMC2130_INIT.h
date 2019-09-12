// TMC2130 stepper driver init

#if AXIS1_DRIVER_MODEL == TMC2130
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
#elif AXIS1_DRIVER_MODEL == TMC2130_LOWPWR
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #if defined(AXIS1_TMC_IGOTO) || defined(AXIS1_TMC_IRUN) || defined(AXIS1_TMC_IHOLD)
    #error "Configuration: AXIS1 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS1_TMC_IHOLD 1000 // 40% of Vref setting on trim-pot
  #define AXIS1_TMC_IRUN 1250  // 50%
  #define AXIS1_TMC_IGOTO 2500 // 100%
  #define AXIS1_TMC_VREF
#elif AXIS1_DRIVER_MODEL == TMC2130_QUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #define AXIS1_TMC_MODE STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC2130_QUIET_LOWPWR
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #define AXIS1_TMC_MODE STEALTHCHOP
  #if defined(AXIS1_TMC_IGOTO) || defined(AXIS1_TMC_IRUN) || defined(AXIS1_TMC_IHOLD)
    #error "Configuration: AXIS1 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS1_TMC_IHOLD 1000
  #define AXIS1_TMC_IRUN 1250
  #define AXIS1_TMC_IGOTO 2500
  #define AXIS1_TMC_VREF
#elif AXIS1_DRIVER_MODEL == TMC2130_VQUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #define AXIS1_TMC_MODE STEALTHCHOP
  #define AXIS1_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC2130_VQUIET_LOWPWR
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #define AXIS1_TMC_MODE STEALTHCHOP
  #define AXIS1_TMC_MODE_GOTO STEALTHCHOP
  #if defined(AXIS1_TMC_IGOTO) || defined(AXIS1_TMC_IRUN) || defined(AXIS1_TMC_IHOLD)
    #error "Configuration: AXIS1 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS1_TMC_IHOLD 1000
  #define AXIS1_TMC_IRUN 1250
  #define AXIS1_TMC_IGOTO 2500
  #define AXIS1_TMC_VREF
#endif

#if AXIS2_DRIVER_MODEL == TMC2130
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
#elif AXIS2_DRIVER_MODEL == TMC2130_LOWPWR
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #if defined(AXIS2_TMC_IGOTO) || defined(AXIS2_TMC_IRUN) || defined(AXIS2_TMC_IHOLD)
    #error "Configuration: AXIS2 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS2_TMC_IHOLD 1000
  #define AXIS2_TMC_IRUN 1250
  #define AXIS2_TMC_IGOTO 2500
  #define AXIS2_TMC_VREF
#elif AXIS2_DRIVER_MODEL == TMC2130_QUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #define AXIS2_TMC_MODE STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC2130_QUIET_LOWPWR
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #define AXIS2_TMC_MODE STEALTHCHOP
  #if defined(AXIS2_TMC_IGOTO) || defined(AXIS2_TMC_IRUN) || defined(AXIS2_TMC_IHOLD)
    #error "Configuration: AXIS2 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS2_TMC_IHOLD 1000
  #define AXIS2_TMC_IRUN 1250
  #define AXIS2_TMC_IGOTO 2500
  #define AXIS2_TMC_VREF
#elif AXIS2_DRIVER_MODEL == TMC2130_VQUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #define AXIS2_TMC_MODE STEALTHCHOP
  #define AXIS2_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC2130_VQUIET_LOWPWR
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #define AXIS2_TMC_MODE STEALTHCHOP
  #define AXIS2_TMC_MODE_GOTO STEALTHCHOP
  #if defined(AXIS2_TMC_IGOTO) || defined(AXIS2_TMC_IRUN) || defined(AXIS2_TMC_IHOLD)
    #error "Configuration: AXIS2 TMC2130, setting current directly is incompatible with _LOWPWR option."
  #endif
  #define AXIS2_TMC_IHOLD 1000
  #define AXIS2_TMC_IRUN 1250
  #define AXIS2_TMC_IGOTO 2500
  #define AXIS2_TMC_VREF
#endif

#if AXIS2_DRIVER_MODEL == TMC_SPI
  #if !defined(AXIS1_TMC_VREF) && (defined(AXIS1_TMC_IRUN) && !defined(AXIS1_TMC_IGOTO) && !defined(AXIS1_TMC_IHOLD))
    #warning "Configuration: AXIS1 TMC2130, use Vref=2.5V and be sure AXIS1_TMC_IRUN matches your stepper motor's current rating"
  #elif !defined(AXIS1_TMC_VREF) && (defined(AXIS1_TMC_IRUN) || defined(AXIS1_TMC_IGOTO) || defined(AXIS1_TMC_IHOLD))
    #warning "Configuration: AXIS1 TMC2130, use Vref=2.5V and be sure AXIS1_TMC_IRUN, AXIS1_TMC_IGOTO, and AXIS1_TMC_IHOLD are all set properly for your stepper motor."
  #else
    #warning "Configuration: AXIS1 TMC2130, set Vref to match your stepper motor's current rating."
  #endif
  
  #if !defined(AXIS2_TMC_VREF) && (defined(AXIS2_TMC_IRUN) && !defined(AXIS2_TMC_IGOTO) && !defined(AXIS2_TMC_IHOLD))
    #warning "Configuration: AXIS2 TMC2130, use Vref=2.5V and be sure AXIS2_TMC_IRUN matches your stepper motor's current rating."
  #elif !defined(AXIS2_TMC_VREF) && (defined(AXIS2_TMC_IRUN) || defined(AXIS2_TMC_IGOTO) || defined(AXIS2_TMC_IHOLD))
    #warning "Configuration: AXIS2 TMC2130, use Vref=2.5V and be sure AXIS2_TMC_IRUN, AXIS2_TMC_IGOTO, and AXIS2_TMC_IHOLD are all set properly for your stepper motor."
  #else
    #warning "Configuration: AXIS2 TMC2130, set Vref to match your stepper motor's current rating."
  #endif
#endif
