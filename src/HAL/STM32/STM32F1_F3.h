// HAL setup for STM32 F103 and F303 microcontrollers

#if defined(STM32F303xC)

  // Lower limit (fastest) step rate in uS for this platform (in SQW mode)
  // This assumes optimization in the Arduino IDE is set to "Fastest (-O3)"
  #define HAL_MAXRATE_LOWER_LIMIT 16

  #define HAL_FAST_PROCESSOR

  #define TIM_SIDEREAL TIM15
  #define TIM_AXIS1    TIM16
  #define TIM_AXIS2    TIM17

#elif defined(STM32F103xB)

  // Lower limit (fastest) step rate in uS for this platform (in SQW mode)
  #define HAL_MAXRATE_LOWER_LIMIT 34

  #define TIM_SIDEREAL TIM4
  #define TIM_AXIS1    TIM1
  #define TIM_AXIS2    TIM2

#else
  #error "Unknown Board for this HAL file"
#endif

// Width of step pulse
#define HAL_PULSE_WIDTH 1900

// fast bit-banged SPI should hit an ~1 MHz bitrate for TMC drivers
#define SPI_DELAY_NS    500

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial1
// SerialA is always enabled, SerialB and SerialC are optional
HardwareSerial Serial2(PB11, PB10);
#define SerialB Serial2
#define HAL_SERIAL_B_ENABLED
#if SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SerialC isn't supported, disable this option."
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
  // Defaults to 0x57 and 4KB
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#endif

//--------------------------------------------------------------------------------------------------
// Initialize timers
// frequency compensation for adjusting microseconds to timer counts
#define F_COMP 4000000

#define ISR(f) void f (void)

HardwareTimer *Timer_Sidereal = new HardwareTimer(TIM_SIDEREAL);
HardwareTimer *Timer_Axis1    = new HardwareTimer(TIM_AXIS1);
HardwareTimer *Timer_Axis2    = new HardwareTimer(TIM_AXIS2);
// TIMER_TONE uses TIM3
// TIMER_SERVO uses TIM2
#undef TIMER_SERVO

#define SIDEREAL_CH  1
#define AXIS1_CH     1
#define AXIS2_CH     1

#include "Common.h"

#include "F1_FastWrite.h"

