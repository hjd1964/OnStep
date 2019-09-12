// TMC2209 stepper driver init

#if AXIS1_DRIVER_MODEL == TMC2209_QUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC2209
  #define AXIS1_TMC_MODE STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC2209_VQUIET
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL TMC2209
  #define AXIS1_TMC_MODE STEALTHCHOP
  #define AXIS1_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS1_DRIVER_MODEL == TMC5160_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#elif AXIS1_DRIVER_MODEL == TMC5160_QUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#elif AXIS1_DRIVER_MODEL == TMC5160_VQUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#endif

#if AXIS2_DRIVER_MODEL == TMC2209_QUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC2209
  #define AXIS2_TMC_MODE STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC2209_VQUIET
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL TMC2209
  #define AXIS2_TMC_MODE STEALTHCHOP
  #define AXIS2_TMC_MODE_GOTO STEALTHCHOP
#elif AXIS2_DRIVER_MODEL == TMC5160_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#elif AXIS2_DRIVER_MODEL == TMC5160_QUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#elif AXIS2_DRIVER_MODEL == TMC5160_VQUIET_LOWPWR
  #error "Configuration: _LOWPWR option isn't supported for TMC2209 stepper drivers."
#endif

#if AXIS1_DRIVER_MODEL == TMC2209
  #define AXIS1_DISABLE_M2
  #ifdef AXIS1_TMC_MODE
    #if AXIS1_TMC_MODE == STEALTHCHOP
      #define AXIS1_DECAY_MODE LOW // stealthChop
    #endif
    #if AXIS1_TMC_MODE == SPREADCYCLE
      #define AXIS1_DECAY_MODE HIGH // spreadCycle
    #endif
  #else
    #define AXIS1_DECAY_MODE HIGH
  #endif
  #ifdef AXIS1_TMC_MODE_GOTO
    #if AXIS1_TMC_MODE_GOTO == STEALTHCHOP
      #define AXIS1_DECAY_MODE_GOTO LOW
    #endif
    #if AXIS1_TMC_MODE_GOTO == SPREADCYCLE
      #define AXIS1_DECAY_MODE_GOTO HIGH
    #endif
  #else
    #define AXIS1_DECAY_MODE_GOTO HIGH
  #endif
#endif

#if AXIS2_DRIVER_MODEL == TMC2209
  #define AXIS2_DISABLE_M2
  #ifdef AXIS2_TMC_MODE
    #if AXIS2_TMC_MODE == STEALTHCHOP
      #define AXIS2_DECAY_MODE LOW // stealthChop
    #endif
    #if AXIS2_TMC_MODE == SPREADCYCLE
      #define AXIS2_DECAY_MODE HIGH // spreadCycle
    #endif
  #else
    #define AXIS2_DECAY_MODE HIGH
  #endif
  #ifdef AXIS2_TMC_MODE_GOTO
    #if AXIS2_TMC_MODE_GOTO == STEALTHCHOP
      #define AXIS2_DECAY_MODE_GOTO LOW
    #endif
    #if AXIS2_TMC_MODE_GOTO == SPREADCYCLE
      #define AXIS2_DECAY_MODE_GOTO HIGH
    #endif
  #else
    #define AXIS2_DECAY_MODE_GOTO HIGH
  #endif
#endif
