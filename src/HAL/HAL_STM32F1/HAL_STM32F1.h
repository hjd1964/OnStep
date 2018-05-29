// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// Lower limit (fastest) step rate in uS for this platform -------------------------------------------
// the exact model should be detected and these tailored to each, but this is a good starting point
#define MaxRateLowerLimit 16

#include <HardwareTimer.h>

// Get this library from https://github.com/watterott/Arduino-Libs/archive/master.zip
#include <digitalWriteFast.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial

// Which port to use for WiFi?
//
// The hardware serial ports and pins for STM32F103 are:
//   USART1: TX PA9,  RX PA10
//   USART2: TX PA2,  RX PA3
//   USART3: TX PB10, RX PB11

// Different flashing methods will remap the port numbers differently, as follows.
//
// For DFU (STM32duino), or ST-Link V2, the ports are:
//   Serial1 -> USART 1
//   Serial2 -> USART 2
//   Serial3 -> USART 3
// If "Serial" method, using a USB to TTL dongle on pins A9 and A10, the ports are:
//   Serial  -> USART 1
//   Serial1 -> USART 2
//   Serial2 -> USART 3
#if defined(SERIAL_USB)
  #define SerialB Serial3
#else
  #define SerialB Serial2
#endif

// SerialA is always enabled, SerialB and SerialC are optional
#define HAL_SERIAL_B_ENABLED

// New symbol for the default I2C port -------------------------------------------------------------
#define HAL_Wire Wire

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#endif

// Use an RTC (Real Time Clock) if present -----------------------------------------------------------
#include "../drivers/RTCw.h"

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 1000000.0

// initialised here and not in timer.ino
void TIMER1_COMPA_vect(void);

HardwareTimer Timer_Axis1(3);
void TIMER3_COMPA_vect(void);

HardwareTimer Timer_Axis2(2);
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  SiderealClockSetInterval(siderealInterval);
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // Pause the timer while we're configuring it
  Timer_Axis1.pause();

  //Timer_Axis1.setPrescaleFactor(SMT32_PRESCALER);
  //Timer_Axis1.setOverflow(STM32_OVERFLOW);
  // Set up period
  Timer_Axis1.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt on channel 3
  Timer_Axis1.setChannel3Mode(TIMER_OUTPUT_COMPARE);
  Timer_Axis1.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  Timer_Axis1.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis1.refresh();

  // Start the timer counting
  Timer_Axis1.resume();
  
  // Pause the timer while we're configuring it
  Timer_Axis2.pause();

  // Set up period
  Timer_Axis2.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt for the channel
  Timer_Axis2.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  Timer_Axis2.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  Timer_Axis2.attachInterrupt(TIMER_CH2, TIMER4_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis2.refresh();

  // Start the timer counting
  Timer_Axis2.resume();

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  nvic_irq_set_priority(NVIC_TIMER1_CC, 2);
  // set the motor timers to run at the highest priority
  nvic_irq_set_priority(NVIC_TIMER2, 0);
  nvic_irq_set_priority(NVIC_TIMER3, 0);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);
HardwareTimer Timer_Sidereal(1);

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);

  // This code is based on the following document
  //
  // http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.10/lang/api/hardwaretimer.html#lang-hardwaretimer
  // And this document:
  //
  // http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/timers.html
  // Pause the timer while we're configuring it
  Timer_Sidereal.pause();

  // Set up period
  Timer_Sidereal.setPeriod((float)iv * 0.0625); // in microseconds

  // Set up an interrupt on channel 1
  Timer_Sidereal.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  Timer_Sidereal.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  Timer_Sidereal.attachInterrupt(TIMER_CH1, TIMER1_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  Timer_Sidereal.refresh();

  // Start the timer counting
  Timer_Sidereal.resume();
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

void QuickSetIntervalAxis1(uint32_t r) {
  // Pause the timer while we're configuring it
  Timer_Axis1.pause();

  // Set up period
  Timer_Axis1.setPeriod(r); // in microseconds

  // Set up an interrupt on channel 1
  Timer_Axis1.setChannel3Mode(TIMER_OUTPUT_COMPARE);
  Timer_Axis1.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  Timer_Axis1.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis1.refresh();

  // Start the timer counting
  Timer_Axis1.resume();
}

void QuickSetIntervalAxis2(uint32_t r) {
  // Pause the timer while we're configuring it
  Timer_Axis2.pause();

  // Set up period
  Timer_Axis2.setPeriod(r); // in microseconds

  // Set up an interrupt on channel 1
  Timer_Axis2.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  Timer_Axis2.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  Timer_Axis2.attachInterrupt(TIMER_CH2, TIMER4_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis2.refresh();

  // Start the timer counting
  Timer_Axis2.resume();
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define StepPinAxis1_HIGH digitalWriteFast(Axis1StepPin, HIGH)
#define StepPinAxis1_LOW digitalWriteFast(Axis1StepPin, LOW)
#define DirPinAxis1_HIGH digitalWriteFast(Axis1DirPin, HIGH)
#define DirPinAxis1_LOW digitalWriteFast(Axis1DirPin, LOW)

#define StepPinAxis2_HIGH digitalWriteFast(Axis2StepPin, HIGH)
#define StepPinAxis2_LOW digitalWriteFast(Axis2StepPin, LOW)
#define DirPinAxis2_HIGH digitalWriteFast(Axis2DirPin, HIGH)
#define DirPinAxis2_LOW digitalWriteFast(Axis2DirPin, LOW)

