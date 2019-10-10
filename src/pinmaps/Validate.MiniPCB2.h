// MiniPCB v2 specific pins validation

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported on this PINMAP"
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS == ON
  #if Aux0 < 0
    #error "Configuration (Config.h): LED_STATUS enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef Aux0
    #define Aux0 -2
  #endif
#endif

#if LED_STATUS2 == ON
  #if Aux4 < 0
    #error "Configuration (Config.h): LED_STATUS enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef Aux4
    #define Aux4 -2
  #endif
#endif

#if LED_RETICLE == ON
  #if Aux4 < 0
    #error "Configuration (Config.h): LED_RETICLE enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef Aux4
    #define Aux4 -2
  #endif
#endif

#if BUZZER == ON
  // dedicated pin
#endif

// TIME -------------------------------------
#if RTC == DS3231
  #if Aux0 < -1 // in use or other than I2C?
    #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef Aux0
    #define Aux0 -1
  #endif
  #if Aux1 < -1 // in use or other than I2C?
    #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef Aux1
    #define Aux1 -1
  #endif
#endif

#if PPS_SENSE == ON
  // dedicated pin
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  #if Aux0 < -1 // in use or other than I2C?
    #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef Aux0
    #define Aux0 -1
  #endif
  #if Aux1 < -1 // in use or other than I2C?
    #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef Aux1
    #define Aux1 -1
  #endif
#endif

#if TELESCOPE_TEMPERATURE == DS1820
  // dedicated pin
#endif

#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  // dedicated pin
#endif

#if LIMIT_SENSE != OFF
  #if Aux3 < 0
    #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
  #else
    #undef Aux3
    #define Aux3 -2
  #endif
#endif

#if HOME_SENSE != OFF
  #if Aux3 < 0
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
  #else
    #undef Aux3
    #define Aux3 -2
  #endif
  #if Aux4 < 0
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef Aux4
    #define Aux4 -2
  #endif
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  #if Aux1 < 0
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef Aux1
    #define Aux1 -2
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if Aux2 < 0
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS enabled but Aux2 is already in use, choose one feature on Aux2"
  #else
    #undef Aux2
    #define Aux2 -2
  #endif
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  // dedicated pin
#endif

// MOTION CONTROL FOCUSERS/ROTATOR ----------
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  // there is no EN pin
#endif

#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  // there is no EN pin
#endif

#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON
  // there is no EN pin
#endif
