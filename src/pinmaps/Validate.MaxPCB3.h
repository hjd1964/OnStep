// MaxPCB v3 specific pins validation

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #if AssignAux2 < 0
    #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but Aux2 is already in use, choose one feature on Aux2."
  #else
    #undef AssignAux2
    #define AssignAux2 -2
  #endif
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS == ON
  #if AssignAux0 < 0
    #error "Configuration (Config.h): LED_STATUS enabled but Aux0 is already in use, choose one feature on Aux0"
  #else
    #undef AssignAux0
    #define AssignAux0 -2
  #endif
#endif

#if LED_STATUS2 == ON
  #if AssignAux8 < 0
    #error "Configuration (Config.h): LED_STATUS enabled but Aux8 is already in use, choose one feature on Aux8"
  #else
    #undef AssignAux8
    #define AssignAux8 -2
  #endif
#endif

#if LED_RETICLE == ON
  #if AssignAux8 < 0
    #error "Configuration (Config.h): LED_RETICLE enabled but Aux8 is already in use, choose one feature on Aux8"
  #else
    #undef AssignAux8
    #define AssignAux8 -2
  #endif
#endif

#if BUZZER == ON
  // dedicated pin
#endif

// TIME -------------------------------------
#if RTC == DS3231
  // dedicated pins
#endif

#if PPS_SENSE == ON
  // dedicated pin
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  // dedicated pins
#endif

#if TELESCOPE_TEMPERATURE == DS1820
  #if AssignAux4 < 0
    #error "Configuration (Config.h): RTC DS1820 OneWire interface enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef AssignAux4
    #define AssignAux4 -2
  #endif
#endif

#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  // dedicated pin
#endif

#if LIMIT_SENSE != OFF
  #if AssignAux7 < 0
    #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
  #else
    #undef AssignAux7
    #define AssignAux7 -2
  #endif
#endif

#if HOME_SENSE != OFF
  #if AssignAux3 < 0
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
  #else
    #undef AssignAux3
    #define AssignAux3 -2
  #endif
  #if AssignAux4 < 0
    #error "Configuration (Config.h): HOME_SENSE enabled but Aux4 is already in use, choose one feature on Aux4"
  #else
    #undef AssignAux4
    #define AssignAux4 -2
  #endif
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  #if AssignAux1 < -1
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef AssignAux1
    #define AssignAux1 -1
  #endif
  #if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if AssignAux1 < -1
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
  #else
    #undef AssignAux1
    #define AssignAux1 -1
  #endif
  #if AXIS2_DRIVER_STATUS == HIGH || AXIS2_DRIVER_STATUS == LOW
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
  #endif
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  // dedicated pin
#endif

// MOTION CONTROL FOCUSERS/ROTATOR --------
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  #if AssignAux5 < 0
    #error "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN requires ENable signal support but Aux5 is already in use, choose one feature on Aux5"
  #else
    #undef AssignAux5
    #define AssignAux5 -2
  #endif
#endif
#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  #if AssignAux6 < 0
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux6 is already in use, choose one feature on Aux6"
  #else
    #undef AssignAux6
    #define AssignAux6 -2
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON && AXIS4_DRIVER_DC_MODE == OFF
  #if AssignAux5 < 0
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN requires ENable signal support but Aux5 is already in use, choose one feature on Aux5"
  #else
    #undef AssignAux5
    #define AssignAux5 -2
  #endif
#endif
