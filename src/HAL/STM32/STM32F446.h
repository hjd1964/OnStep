// HAL setup for STM32F446 microcontroller

#define HAL_FAST_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 14

// Width of step pulse
#define HAL_PULSE_WIDTH         500

// SPI delay in nanoseconds
#define SPI_DELAY_NS            250

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional
#define SerialB Serial1
#define HAL_SERIAL_B_ENABLED
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SerialC Serial3
  #define HAL_SERIAL_C_ENABLED
#endif

// Handle special case of using software serial for a GPS
#if SerialGPS == SoftwareSerial2
  #include <SoftwareSerial.h>
  SoftwareSerial SWSerialGPS(PA3, PA2); // RX2, TX2
  #undef SerialGPS
  #define SerialGPS SWSerialGPS
#endif

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#if defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  // The FYSETC S6 v2 has a 4096 byte EEPROM built-in
  #if PINMAP == FYSETC_S6_2
    #define E2END 4095
    #define I2C_EEPROM_ADDRESS 0x50
  #endif
  // The FYSETC S6 has a 2048 byte EEPROM built-in
  #if PINMAP == FYSETC_S6
    #define E2END 2047
    #define I2C_EEPROM_ADDRESS 0x50
  #endif
  // Defaults to 0x57 and 4KB 
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#endif

//--------------------------------------------------------------------------------------------------
// Initialize timers
// frequency compensation for adjusting microseconds to timer counts
#define F_COMP 4000000

#define ISR(f) void f (void)

#define TIM_SIDEREAL   TIM1
#define TIM_AXIS1      TIM10
#define TIM_AXIS2      TIM11

HardwareTimer *Timer_Sidereal = new HardwareTimer(TIM_SIDEREAL);
HardwareTimer *Timer_Axis1    = new HardwareTimer(TIM_AXIS1);
HardwareTimer *Timer_Axis2    = new HardwareTimer(TIM_AXIS2);

#define SIDEREAL_CH  1
#define AXIS1_CH     1
#define AXIS2_CH     1

#include "Common.h"

#include "F4_FastWrite.h"
