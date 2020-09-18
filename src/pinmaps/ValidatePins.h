// -------------------------------------------------------------------------------------------------
// Validates pinmap model for current configuration

// helps keep track of Aux pin assignments
#define PIN_NOT_ASSIGNED 0
#define PIN_SHARED_I2C 1
#define PIN_DEDICATED 2
#define ASSIGNED_AUX0 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX1 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX2 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX3 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX4 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX5 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX6 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX7 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX8 PIN_NOT_ASSIGNED

#if PINMAP == Classic
  #include "Validate.Classic.h"
#endif
#if PINMAP == ClassicShield
  #include "Validate.Classic.h"
#endif
#if PINMAP == Mega2560Alt
#endif
#if PINMAP == MiniPCB
  #include "Validate.MiniPCB.h"
#endif
#if PINMAP == MiniPCB13
  #include "Validate.MiniPCB13.h"
#endif
#if PINMAP == MiniPCB2
  #include "Validate.MiniPCB2.h"
#endif
#if PINMAP == MaxPCB
  #include "Validate.MaxPCB.h"
#endif
#if PINMAP == MaxPCB2
  #include "Validate.MaxPCB2.h"
#endif
#if PINMAP == MaxPCB3
  #include "Validate.MaxPCB3.h"
#endif
#if PINMAP == MaxESP
  #include "Validate.MaxESP.h"
#endif
#if PINMAP == MaxESP2
  #include "Validate.MaxESP2.h"
#endif
#if PINMAP == MaxESP3
  #include "Validate.MaxESP3.h"
#endif
#if PINMAP == CNC3
  #include "Validate.CNC3.h"
#endif
#if PINMAP == Ramps14 || PINMAP == MksGenL2 || PINMAP == MksGenL21
  #include "Validate.Ramps14.h"
#endif
#if PINMAP == STM32Blue
  #include "Validate.STM32Blue.h"
#endif
#if PINMAP == FYSETC_S6
  #include "Validate.FYSETC_S6.h"
#endif

// GENERAL PURPOSE PINMAP VALIDATION ---------------

#if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
  #if AXIS1_DRIVER_MODEL == TMC_SPI
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(Axis1_FAULT)
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH feature isn't supported on this PINMAP"
  #elif AXIS1_DRIVER_MODEL != GENERIC && AXIS1_DRIVER_MODEL != SERVO
    #warning "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH use often requires modification of stepper drivers"
  #endif
#endif
#if AXIS2_DRIVER_STATUS == HIGH || AXIS2_DRIVER_STATUS == LOW
  #if AXIS2_DRIVER_MODEL == TMC_SPI
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(Axis2_FAULT)
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH feature isn't supported on this PINMAP"
  #elif AXIS2_DRIVER_MODEL != GENERIC && AXIS2_DRIVER_MODEL != SERVO
    #warning "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH use often requires modification of stepper drivers"
  #endif
#endif

#if SERIAL_B_ESP_FLASHING == ON  && (!defined(ESP8266Gpio0Pin) || !defined(ESP8266RstPin))
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported for this PINMAP"
#endif

// powering down Dec supported only if EN is available and not shared with Axis1
#if AXIS2_DRIVER_POWER_DOWN == ON && (Axis2_EN == -1 || Axis1_EN == Axis2_EN)
  #error "Configuration (Config.h): AXIS2_DRIVER_POWER_DOWN not supported on this PINMAP"
#endif

// focusers/rotator allowed?
#if ROTATOR == ON
  #if Axis3_STEP == -1 || Axis3_DIR == -1
    #error "Configuration (Config.h): AXIS3 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER1 == ON
  #if Axis4_STEP == -1 || Axis4_DIR == -1
    #error "Configuration (Config.h): AXIS4 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON
  #if (Axis5_STEP == -1 || Axis5_DIR == -1) && AXIS5_DRIVER_DC_MODE == OFF
    #error "Configuration (Config.h): AXIS5 step/dir interface is not supported on this PINMAP"
  #endif
#endif

// leds allowed?
#if LED_STATUS != OFF && LEDnegPin == -1
  #error "Configuration (Config.h): LED_STATUS not supported for this PINMAP, must be OFF"
#endif

#if LED_STATUS2 != OFF && LEDneg2Pin == -1
  #error "Configuration (Config.h): LED_STATUS2 not supported for this PINMAP, must be OFF"
#endif

#if LED_RETICLE != OFF && ReticlePin == -1
  #error "Configuration (Config.h): LED_RETICLE not supported for this PINMAP, must be OFF"
#endif

// analog PEC allowed?
#if PEC_SENSE > 0 && AnalogPecPin == -1
  #error "Configuration (Config.h): PEC_SENSE in ANALOG mode not supported for this PINMAP, use ON, etc. NOT a threshold value"
#endif

// TIME_LOCATION_SOURCE DS3234 SPI allowed?
#if (TIME_LOCATION_SOURCE == DS3234S || TIME_LOCATION_SOURCE == DS3234M) && !defined(DS3234_CS_PIN)
  #if PINMAP == Classic
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported, use PINMAP ClassicShield or add '#define ST4_ALTERNATE_PINS_ON' to move the ST4 port pins"
    #else
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported for this PINMAP"
  #endif
#endif

// WEATHER BME280 SPI allowed?
#if WEATHER == BME280_SPI && !defined(BME280_CS_PIN)
  #error "Configuration (Config.h): BME280 weather using SPI is not supported for this PINMAP"
#endif

// focuser/rotators any invalid combinations?
#if ROTATOR == ON && FOCUSER1 == ON
  #if Axis3_STEP == Axis4_STEP
    #error "Configuration (Config.h): AXIS3 and AXIS4 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

#if ROTATOR == ON && FOCUSER2 == ON
  #if Axis3_STEP == Axis5_STEP
    #error "Configuration (Config.h): AXIS3 and AXIS5 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

// focuser/rotators any invalid focuser DC modes?
#if FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE == XDIRECT && AXIS4_DRIVER_MODEL != TMC_SPI
  #error "Configuration (Config.h): AXIS4_DRIVER_DC_MODE error XDIRECT is for TMC stepper drivers only."
#endif

#if FOCUSER2 == ON && AXIS4_DRIVER_DC_MODE == XDIRECT && AXIS5_DRIVER_MODEL != TMC_SPI
  #error "Configuration (Config.h): AXIS4_DRIVER_DC_MODE error XDIRECT is for TMC stepper drivers only."
#endif

#if FOCUSER2 == ON && AXIS5_DRIVER_DC_MODE != OFF
  #if !(FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE != OFF)
    #error "Configuration (Config.h): AXIS5_DRIVER_DC_MODE requires enabling FOCUSER1 in AXIS4_DRIVER_DC_MODE.  In this mode FOCUSER2 USES the FOCUSER1 stepper driver"
  #endif
#endif

#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  #if Axis3_EN == -1
    #error "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN enabled but not supported on this PINMAP"
  #endif
#endif

#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  #if AXIS4_DRIVER_POWER_DOWN == ON && AXIS4_DRIVER_DC_MODE != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis4_EN == -1
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support which this PINMAP doesn't have or is in use for other purposes"
  #endif
#endif

#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON
  #if AXIS5_DRIVER_POWER_DOWN == ON && AXIS5_DRIVER_DC_MODE != OFF
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN and AXIS5_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis5_EN == -1
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN requires ENable signal support which this PINMAP doesn't have"
  #endif
#endif

// ACCESSORIES ------------------------------
#if FEATURE1_PURPOSE != OFF && FEATURE1_PIN == AUX && defined(Aux1)
  #if ASSIGNED_AUX1 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE1_PIN AUX enabled but Aux1 is already in use, choose one feature on Aux1"
  #endif
  #undef FEATURE1_PIN
  #define FEATURE1_PIN Aux1
#endif
#if FEATURE2_PURPOSE != OFF && FEATURE2_PIN == AUX && defined(Aux2)
  #if ASSIGNED_AUX2 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE2_PIN AUX enabled but Aux2 is already in use, choose one feature on Aux2"
  #endif
  #undef FEATURE2_PIN
  #define FEATURE2_PIN Aux2
#endif
#if FEATURE3_PURPOSE != OFF && FEATURE3_PIN == AUX && defined(Aux3)
  #if ASSIGNED_AUX3 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE3_PIN AUX enabled but Aux3 is already in use, choose one feature on Aux3"
  #endif
  #undef FEATURE3_PIN
  #define FEATURE3_PIN Aux3
#endif
#if FEATURE4_PURPOSE != OFF && FEATURE4_PIN == AUX && defined(Aux3)
  #if ASSIGNED_AUX4 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE4_PIN AUX enabled but Aux4 is already in use, choose one feature on Aux4"
  #endif
  #undef FEATURE4_PIN
  #define FEATURE4_PIN Aux4
#endif
#if FEATURE5_PURPOSE != OFF && FEATURE5_PIN == AUX && defined(Aux5)
  #if ASSIGNED_AUX5 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE5_PIN AUX enabled but Aux5 is already in use, choose one feature on Aux5"
  #endif
  #undef FEATURE5_PIN
  #define FEATURE5_PIN Aux5
#endif
#if FEATURE6_PURPOSE != OFF && FEATURE6_PIN == AUX && defined(Aux6)
  #if ASSIGNED_AUX6 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE6_PIN AUX enabled but Aux6 is already in use, choose one feature on Aux6"
  #endif
  #undef FEATURE6_PIN
  #define FEATURE6_PIN Aux6
#endif
#if FEATURE7_PURPOSE != OFF && FEATURE7_PIN == AUX && defined(Aux7)
  #if ASSIGNED_AUX7 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE7_PIN AUX enabled but Aux7 is already in use, choose one feature on Aux7"
  #endif
  #undef FEATURE7_PIN
  #define FEATURE7_PIN Aux7
#endif
#if FEATURE8_PURPOSE != OFF && FEATURE8_PIN == AUX && defined(Aux8)
  #if ASSIGNED_AUX8 != PIN_NOT_ASSIGNED
    #error "Configuration (Config.h): FEATURE8_PIN AUX enabled but Aux8 is already in use, choose one feature on Aux8"
  #endif
  #undef FEATURE8_PIN
  #define FEATURE8_PIN Aux8
#endif
