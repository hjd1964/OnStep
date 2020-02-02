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
#if PINMAP == Ramps14
  #include "Validate.Ramps14.h"
#endif
#if PINMAP == MksGenL2
  #include "Validate.Ramps14.h"
#endif
#if PINMAP == STM32Blue
  #include "Validate.STM32Blue.h"
#endif
#if PINMAP == STM32Black
#endif

// GENERAL PURPOSE PINMAP VALIDATION ---------------

#if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
  #if AXIS1_DRIVER_MODEL == TMC_SPI
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(Axis1FaultPin)
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH feature isn't supported on this PINMAP"
  #elif AXIS1_DRIVER_MODEL != GENERIC && AXIS1_DRIVER_MODEL != SERVO
    #warning "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH use often requires modification of stepper drivers"
  #endif
#endif
#if AXIS2_DRIVER_STATUS == HIGH || AXIS2_DRIVER_STATUS == LOW
  #if AXIS2_DRIVER_MODEL == TMC_SPI
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(Axis2FaultPin)
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
  #if Axis3StepPin == -1 || Axis3DirPin == -1
    #error "Configuration (Config.h): AXIS3 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER1 == ON
  #if Axis4StepPin == -1 || Axis4DirPin == -1
    #error "Configuration (Config.h): AXIS4 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON
  #if (Axis5StepPin == -1 || Axis5DirPin == -1) && AXIS5_DRIVER_DC_MODE == OFF
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

// RTC DS3234 SPI allowed?
#if (RTC == DS3234S || RTC == DS3234M) && !defined(DS3234_CS_PIN)
  #if PINMAP == Classic
      #error "Configuration (Config.h): DS3234 RTC using SPI is not supported, use PINMAP ClassicShield or add '#define ST4_ALTERNATE_PINS_ON' to move the ST4 port pins"
    #else
      #error "Configuration (Config.h): DS3234 RTC using SPI is not supported for this PINMAP"
  #endif
#endif

// WEATHER BME280 SPI allowed?
#if WEATHER == BME280_SPI && !defined(BME280_CS_PIN)
  #error "Configuration (Config.h): BME280 weather using SPI is not supported for this PINMAP"
#endif

// focuser/rotators any invalid combinations?
#if ROTATOR == ON && FOCUSER1 == ON
  #if Axis3StepPin == Axis4StepPin
    #error "Configuration (Config.h): AXIS3 and AXIS4 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

#if ROTATOR == ON && FOCUSER2 == ON
  #if Axis3StepPin == Axis5StepPin
    #error "Configuration (Config.h): AXIS3 and AXIS5 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

// if we have a >= 3 driver SPI bus it's a requirement that all drivers be TMC SPI (or OFF)
// except for MksGenL2 which can set modes on shunts instead
#if PINMAP != MksGenL2
  #ifndef Axis3_M0
    #define Axis3_M0 -1
  #endif
  #ifndef Axis3_M1
    #define Axis3_M1 -1
  #endif
  #if Axis1_M0 == Axis2_M0 && Axis1_M1 == Axis2_M1 && Axis1_M0 == Axis3_M0 && Axis1_M1 == Axis3_M1
    // special checks for SPI bus pin maps to ensure all enabled stepper drivers are TMC2130 or TMC5160 in SPI mode
    #if AXIS1_DRIVER_MODEL != TMC_SPI
      #error "Configuration (Config.h): AXIS1_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
    #if AXIS2_DRIVER_MODEL != TMC_SPI
      #error "Configuration (Config.h): AXIS2_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
    #if Axis1_M0 == Axis3_M0 && Axis1_M1 == Axis3_M1 && ROTATOR == ON
      #if AXIS3_DRIVER_MODEL != TMC_SPI
        #error "Configuration (Config.h): AXIS3_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP, or AXIS3 must be OFF"
      #endif
    #endif
    #if Axis1_M0 == Axis4_M0 && Axis1_M1 == Axis4_M1 && FOCUSER1 == ON
      #if AXIS4_DRIVER_MODEL != TMC_SPI
        #error "Configuration (Config.h): AXIS4_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP, or AXIS4 must be OFF"
      #endif
    #endif
    #if Axis1_M0 == Axis5_M0 && Axis1_M1 == Axis5_M1 && FOCUSER2 == ON
      #if AXIS5_DRIVER_MODEL != TMC_SPI
        #error "Configuration (Config.h): AXIS5_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP, or AXIS5 must be OFF"
      #endif
    #endif
  #else
    #if ROTATOR == ON && AXIS3_DRIVER_MODEL != OFF
      #error "Configuration (Config.h): AXIS3_DRIVER_MODEL must be OFF for this PINMAP"
    #endif
    #if FOCUSER1 == ON && AXIS4_DRIVER_MODEL != OFF
      #error "Configuration (Config.h): AXIS4_DRIVER_MODEL must be OFF for this PINMAP"
    #endif
    #if FOCUSER2 == ON && AXIS5_DRIVER_MODEL != OFF
      #error "Configuration (Config.h): AXIS5_DRIVER_MODEL must be OFF for this PINMAP"
    #endif
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
