// Classic (original) specific pins validation

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported on this PINMAP"
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS == ON
  // dedicated pin
#endif

#if LED_STATUS2 == ON
  // dedicated pin
#endif

#if LED_RETICLE == ON
  // dedicated pin
#endif

#if BUZZER == ON
  // dedicated pin
#endif

// TIME -------------------------------------
#if TIME_LOCATION_SOURCE == DS3231
  #error "Configuration (Config.h): I2C is not supported on this PINMAP"
#endif

#if PPS_SENSE == ON
  // dedicated pin
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  #error "Configuration (Config.h): I2C is not supported on this PINMAP"
#endif

#if TELESCOPE_TEMPERATURE == DS1820
  // dedicated pin
#endif

#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  // dedicated pin
#endif

#if LIMIT_SENSE != OFF
  // dedicated pin
#endif

#if HOME_SENSE != OFF
  // dedicated pin
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  // dedicated pin
#endif

#if AXIS2_DRIVER_STATUS != OFF
  // dedicated pin
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  // dedicated pin
#endif

// MOTION CONTROL FOCUSERS/ROTATOR --------
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  // there is no EN pin
#endif

#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  // there is no EN pin
#endif

#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON
  // there is no EN pin
#endif
