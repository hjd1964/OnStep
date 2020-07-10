// MaxPCB v2 specific pins validation

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  // dedicated pin
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
  #if ASSIGNED_AUX8 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LED_STATUS enabled but Aux8 is already in use, choose one feature on Aux8"
  #else
    #undef ASSIGNED_AUX8
    #define ASSIGNED_AUX8 PIN_DEDICATED
  #endif
#endif

#if LED_RETICLE == ON
  #if ASSIGNED_AUX8 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LED_RETICLE enabled but Aux8 is already in use, choose one feature on Aux8"
  #else
    #undef ASSIGNED_AUX8
    #define ASSIGNED_AUX8 PIN_DEDICATED
  #endif
#endif

#if BUZZER == ON
  // dedicated pin
#endif

// TIME -------------------------------------
#if TIME_LOCATION_SOURCE == DS3231
  // dedicated pins
#endif

#if PPS_SENSE == ON
  // dedicated pin
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  // dedicated pins
#endif

#if OneWirePin == Aux4 && defined(ONEWIRE_DEVICES_PRESENT)
  #if ASSIGNED_AUX4 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): OneWire interface device(s) enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef ASSIGNED_AUX4
    #define ASSIGNED_AUX4 PIN_DEDICATED
  #endif
#endif

#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  // dedicated pin
#endif

#if LIMIT_SENSE != OFF
  #if ASSIGNED_AUX7 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
  #else
    #undef ASSIGNED_AUX7
    #define ASSIGNED_AUX7 PIN_DEDICATED
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
  #if ASSIGNED_AUX1 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef ASSIGNED_AUX1
    #define ASSIGNED_AUX1 PIN_DEDICATED
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if ASSIGNED_AUX2 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS enabled but Aux2 is already in use, choose one feature on Aux2"
  #else
    #undef ASSIGNED_AUX2
    #define ASSIGNED_AUX2 PIN_DEDICATED
  #endif
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  // dedicated pin
#endif

// MOTION CONTROL FOCUSERS/ROTATOR --------
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  #if ASSIGNED_AUX5 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN requires ENable signal support but Aux5 is already in use, choose one feature on Aux5"
  #else
    #undef ASSIGNED_AUX5
    #define ASSIGNED_AUX5 PIN_DEDICATED
  #endif
#endif
#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  #if ASSIGNED_AUX6 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux6 is already in use, choose one feature on Aux6"
  #else
    #undef ASSIGNED_AUX6
    #define ASSIGNED_AUX6 PIN_DEDICATED
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON && AXIS4_DRIVER_DC_MODE == OFF
  #if ASSIGNED_AUX5 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN requires ENable signal support but Aux5 is already in use, choose one feature on Aux5"
  #else
    #undef ASSIGNED_AUX5
    #define ASSIGNED_AUX5 PIN_DEDICATED
  #endif
#endif
