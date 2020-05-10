// MiniPCB v1.3 specific pins validation

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #if ASSIGNED_AUX2 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but Aux2 is already in use, choose one feature on Aux2."
  #else
    #undef ASSIGNED_AUX2
    #define ASSIGNED_AUX2 PIN_DEDICATED
  #endif
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS == ON
  #if ASSIGNED_AUX0 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LED_STATUS enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef ASSIGNED_AUX0
    #define ASSIGNED_AUX0 PIN_DEDICATED
  #endif
#endif

#if LED_STATUS2 == ON
  #if ASSIGNED_AUX4 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LED_STATUS enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef ASSIGNED_AUX4
    #define ASSIGNED_AUX4 PIN_DEDICATED
  #endif
#endif

#if LED_RETICLE == ON
  #if ASSIGNED_AUX4 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LED_RETICLE enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef ASSIGNED_AUX4
    #define ASSIGNED_AUX4 PIN_DEDICATED
  #endif
#endif

#if BUZZER == ON
  // dedicated pin
#endif

// TIME -------------------------------------
#if TIME_LOCATION_SOURCE == DS3231
  #if ASSIGNED_AUX0 == PIN_DEDICATED
    #error "Configuration (Config.h): TIME_LOCATION_SOURCE DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef ASSIGNED_AUX0
    #define ASSIGNED_AUX0 PIN_SHARED_I2C
  #endif
  #if ASSIGNED_AUX1 == PIN_DEDICATED
    #error "Configuration (Config.h): TIME_LOCATION_SOURCE DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef ASSIGNED_AUX1
    #define ASSIGNED_AUX1 PIN_SHARED_I2C
  #endif
#endif

#if PPS_SENSE == ON
  // dedicated pin
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  #if ASSIGNED_AUX0 == PIN_DEDICATED
    #error "Configuration (Config.h): WEATHER BME280 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef ASSIGNED_AUX0
    #define ASSIGNED_AUX0 PIN_SHARED_I2C
  #endif
  #if ASSIGNED_AUX1 == PIN_DEDICATED
    #error "Configuration (Config.h): WEATHER BME280 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef ASSIGNED_AUX1
    #define ASSIGNED_AUX1 PIN_SHARED_I2C
  #endif
#endif

// dedicated pin for one-wire

#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  // dedicated pin
#endif

#if LIMIT_SENSE != OFF
  #if ASSIGNED_AUX3 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
  #else
    #undef ASSIGNED_AUX3
    #define ASSIGNED_AUX3 PIN_DEDICATED
  #endif
#endif

#if HOME_SENSE != OFF
  #if ASSIGNED_AUX3 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
  #else
    #undef ASSIGNED_AUX3
    #define ASSIGNED_AUX3 PIN_DEDICATED
  #endif
  #if ASSIGNED_AUX4 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef ASSIGNED_AUX4
    #define ASSIGNED_AUX4 PIN_DEDICATED
  #endif
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  #if ASSIGNED_AUX1 == PIN_DEDICATED
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef ASSIGNED_AUX1
    #define ASSIGNED_AUX1 PIN_SHARED_I2C
  #endif
  #if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if ASSIGNED_AUX1 == PIN_DEDICATED
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef ASSIGNED_AUX1
    #define ASSIGNED_AUX1 PIN_SHARED_I2C
  #endif
  #if AXIS2_DRIVER_STATUS == HIGH || AXIS2_DRIVER_STATUS == LOW
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
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
