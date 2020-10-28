// HAL setup for STM32F411

#define HAL_FAST_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
// This assumes optimization set to Fastest (-O3)
#define HAL_MAXRATE_LOWER_LIMIT 16

// Width of step pulse
#define HAL_PULSE_WIDTH         500

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional

HardwareSerial HWSerial2(PA3, PA2); // RX2, TX2
#define SerialB HWSerial2
#define HAL_SERIAL_B_ENABLED

#if SERIAL_C_BAUD_DEFAULT != OFF
  HardwareSerial HWSerial1(PA10, PA9); // RX1, TX1
  #define SerialC HWSerial1
  #define HAL_SERIAL_C_ENABLED
#endif
  
// HardwareSerial HWSerial1(PA10, PA9);  // RX1, TX1
// HardwareSerial HWSerial2(PA3, PA2);   // RX2, TX2
// HardwareSerial HWSerial6(PA12, PA11); // RX6, TX6

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#if defined(NV_AT24C32)
  // defaults to 0x57 and 4KB
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  // The STM32F411 MaxPCB3 has an 8192 byte EEPROM built-in (rated for 5M write cycles)
  #define NV_ENDURANCE HIGH
  #define E2END 8191
  #define I2C_EEPROM_ADDRESS 0x50
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#endif

//----------------------------------------------------------------------------------------------------
// Nanoseconds delay function
unsigned int _nanosPerPass=1;
void delayNanoseconds(unsigned int n) {
  unsigned int np=(n/_nanosPerPass);
  for (unsigned int i=0; i<np; i++) { __asm__ volatile ("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"); }
}

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
void HAL_Initialize(void) {
  // calibrate delayNanoseconds()
  uint32_t startTime,npp;
  startTime=micros(); delayNanoseconds(65535); npp=micros(); npp=((int32_t)(npp-startTime)*1000)/63335;
  if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp;
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return -999;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers
// frequency compensation for adjusting microseconds to timer counts
//#define F_COMP 19.7721042
#define F_COMP 4000000

#define ISR(f) void f (void)

#define TIM_SIDEREAL   TIM1
#define TIM_AXIS1      TIM9
#define TIM_AXIS2      TIM11

HardwareTimer *Timer_Sidereal = new HardwareTimer(TIM_SIDEREAL);
HardwareTimer *Timer_Axis1    = new HardwareTimer(TIM_AXIS1);
HardwareTimer *Timer_Axis2    = new HardwareTimer(TIM_AXIS2);

#define SIDEREAL_CH  1
#define AXIS1_CH     1
#define AXIS2_CH     1

#include "STM32Common.h"

#include "STM32F4Common.h"
