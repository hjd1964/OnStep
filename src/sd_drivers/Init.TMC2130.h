// TMC2130 stepper driver init

#if AXIS1_DRIVER_MODEL == TMC2130_QUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC2130
  #undef AXIS1_DRIVER_DECAY_MODE
  #define AXIS1_DRIVER_DECAY_MODE STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC2130_VQUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC2130
  #undef AXIS1_DRIVER_DECAY_MODE
  #define AXIS1_DRIVER_DECAY_MODE STEALTHCHOP
  #undef AXIS1_DRIVER_DECAY_MODE_GOTO
  #define AXIS1_DRIVER_DECAY_MODE_GOTO STEALTHCHOP
#endif
#if AXIS1_DRIVER_MODEL == TMC2130
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC_SPI
  #define AXIS1_DRIVER_SUBMODEL TMC2130
  #if AXIS1_DRIVER_IRUN != OFF && AXIS1_DRIVER_IGOTO == OFF && AXIS1_DRIVER_IHOLD == OFF
    #warning "Configuration (Config.h): AXIS1 TMC2130, use Vref=2.5V and be sure AXIS1_DRIVER_IRUN matches your stepper motor's current rating"
  #elif AXIS1_DRIVER_IRUN != OFF || AXIS1_DRIVER_IGOTO != OFF || AXIS1_DRIVER_IHOLD != OFF
    #warning "Configuration (Config.h): AXIS1 TMC2130, use Vref=2.5V and be sure AXIS1_DRIVER_IRUN, AXIS1_DRIVER_IGOTO, and AXIS1_DRIVER_IHOLD are all set properly for your stepper motor."
  #else
    #warning "Configuration (Config.h): AXIS1 TMC2130, set Vref to match your stepper motor's current rating."
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC2130_QUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC2130
  #undef AXIS2_DRIVER_DECAY_MODE
  #define AXIS2_DRIVER_DECAY_MODE STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC2130_VQUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC2130
  #undef AXIS2_DRIVER_DECAY_MODE
  #define AXIS2_DRIVER_DECAY_MODE STEALTHCHOP
  #undef AXIS2_DRIVER_DECAY_MODE_GOTO
  #define AXIS2_DRIVER_DECAY_MODE_GOTO STEALTHCHOP
#endif
#if AXIS2_DRIVER_MODEL == TMC2130
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC_SPI
  #define AXIS2_DRIVER_SUBMODEL TMC2130
  #if AXIS2_DRIVER_IRUN != OFF && AXIS2_DRIVER_IGOTO == OFF && AXIS2_DRIVER_IHOLD == OFF
    #warning "Configuration (Config.h): AXIS2 TMC2130, use Vref=2.5V and be sure AXIS2_DRIVER_IRUN matches your stepper motor's current rating."
  #elif AXIS2_DRIVER_IRUN != OFF || AXIS2_DRIVER_IGOTO != OFF || AXIS2_DRIVER_IHOLD != OFF
    #warning "Configuration (Config.h): AXIS2 TMC2130, use Vref=2.5V and be sure AXIS2_DRIVER_IRUN, AXIS2_DRIVER_IGOTO, and AXIS2_DRIVER_IHOLD are all set properly for your stepper motor."
  #else
    #warning "Configuration (Config.h): AXIS2 TMC2130, set Vref to match your stepper motor's current rating."
  #endif
#endif

#if ROTATOR == ON
  #if AXIS3_DRIVER_MODEL == TMC2130_QUIET
    #undef AXIS3_DRIVER_MODEL
    #define AXIS3_DRIVER_MODEL TMC2130
    #undef AXIS3_DRIVER_DECAY_MODE
    #define AXIS3_DRIVER_DECAY_MODE STEALTHCHOP
  #elif AXIS3_DRIVER_MODEL == TMC2130_VQUIET
    #undef AXIS3_DRIVER_MODEL
    #define AXIS3_DRIVER_MODEL TMC2130
    #undef AXIS3_DRIVER_DECAY_MODE
    #define AXIS3_DRIVER_DECAY_MODE STEALTHCHOP
  #endif
  #if AXIS3_DRIVER_MODEL == TMC2130
    #undef AXIS3_DRIVER_MODEL
    #define AXIS3_DRIVER_MODEL TMC_SPI
    #define AXIS3_DRIVER_SUBMODEL TMC2130
    #if AXIS3_DRIVER_IRUN != OFF && AXIS3_DRIVER_IHOLD == OFF
      #warning "Configuration (Config.h): AXIS3 TMC2130, use Vref=2.5V and be sure AXIS3_DRIVER_IRUN is set properly for your power supply, voltage regulator, and stepper motor limits."
    #elif AXIS3_DRIVER_IRUN != OFF || AXIS3_DRIVER_IHOLD != OFF
      #warning "Configuration (Config.h): AXIS3 TMC2130, use Vref=2.5V and be sure AXIS3_DRIVER_IRUN and AXIS3_DRIVER_IHOLD are set properly for your power supply, voltage regulator, and stepper motor limits."
    #else
      #warning "Configuration (Config.h): AXIS3 TMC2130, set Vref to match your stepper motor's current rating."
    #endif
  #endif
#endif

#if FOCUSER1 == ON
  #if AXIS4_DRIVER_MODEL == TMC2130_QUIET
    #undef AXIS4_DRIVER_MODEL
    #define AXIS4_DRIVER_MODEL TMC2130
    #undef AXIS4_DRIVER_DECAY_MODE
    #define AXIS4_DRIVER_DECAY_MODE STEALTHCHOP
  #elif AXIS4_DRIVER_MODEL == TMC2130_VQUIET
    #undef AXIS4_DRIVER_MODEL
    #define AXIS4_DRIVER_MODEL TMC2130
    #undef AXIS4_DRIVER_DECAY_MODE
    #define AXIS4_DRIVER_DECAY_MODE STEALTHCHOP
  #endif
  #if AXIS4_DRIVER_MODEL == TMC2130
    #undef AXIS4_DRIVER_MODEL
    #define AXIS4_DRIVER_MODEL TMC_SPI
    #define AXIS4_DRIVER_SUBMODEL TMC2130
    #if AXIS4_DRIVER_IRUN != OFF && AXIS4_DRIVER_IHOLD == OFF
      #warning "Configuration (Config.h): AXIS4 TMC2130, use Vref=2.5V and be sure AXIS4_DRIVER_IRUN is set properly for your power supply, voltage regulator, and stepper motor limits."
    #elif AXIS4_DRIVER_IRUN != OFF || AXIS4_DRIVER_IHOLD != OFF
      #warning "Configuration (Config.h): AXIS4 TMC2130, use Vref=2.5V and be sure AXIS4_DRIVER_IRUN and AXIS4_DRIVER_IHOLD are set properly for your power supply, voltage regulator, and stepper motor limits."
    #else
      #warning "Configuration (Config.h): AXIS4 TMC2130, set Vref to match your stepper motor's current rating."
    #endif
  #endif
#endif

#if FOCUSER2 == ON
  #if AXIS5_DRIVER_MODEL == TMC2130_QUIET
    #undef AXIS5_DRIVER_MODEL
    #define AXIS5_DRIVER_MODEL TMC2130
    #undef AXIS5_DRIVER_DECAY_MODE
    #define AXIS5_DRIVER_DECAY_MODE STEALTHCHOP
  #elif AXIS5_DRIVER_MODEL == TMC2130_VQUIET
    #undef AXIS5_DRIVER_MODEL
    #define AXIS5_DRIVER_MODEL TMC2130
    #undef AXIS5_DRIVER_DECAY_MODE
    #define AXIS5_DRIVER_DECAY_MODE STEALTHCHOP
  #endif
  #if AXIS5_DRIVER_MODEL == TMC2130
    #undef AXIS5_DRIVER_MODEL
    #define AXIS5_DRIVER_MODEL TMC_SPI
    #define AXIS5_DRIVER_SUBMODEL TMC2130
    #if AXIS5_DRIVER_IRUN != OFF && AXIS5_DRIVER_IHOLD == OFF
      #warning "Configuration (Config.h): AXIS5 TMC2130, use Vref=2.5V and be sure AXIS5_DRIVER_IRUN is set properly for your power supply, voltage regulator, and stepper motor limits."
    #elif AXIS5_DRIVER_IRUN != OFF || AXIS5_DRIVER_IHOLD != OFF
      #warning "Configuration (Config.h): AXIS5 TMC2130, use Vref=2.5V and be sure AXIS5_DRIVER_IRUN and AXIS5_DRIVER_IHOLD are set properly for your power supply, voltage regulator, and stepper motor limits."
    #else
      #warning "Configuration (Config.h): AXIS5 TMC2130, set Vref to match your stepper motor's current rating."
    #endif
  #endif
#endif
