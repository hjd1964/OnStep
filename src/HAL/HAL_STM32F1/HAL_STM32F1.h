// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// The STM32 platform has no built in EEPROM. Although there is EEPROM emulation in flash, it does
// work. Therefore a real EEPROM is needed. 
// The best way is to use the low cost DS3231 RTC module, which has a 4K I2C EEPROM built in.
// The EEPROM's address is different from the default, so we define it before including HAL_24LC.h
#define I2C_EEPROM_ADDRESS 0x57

// End of EEPROM
#define E2END 4095

//#include <EEPROM.h>
#include "../drivers/HAL_24LC.h"

// Lower limit (fastest) step rate in uS for this platform
// the exact model should be detected and these tailored to each, but this is a good starting point
#define MaxRateLowerLimit 16

#include <HardwareTimer.h>

// Get this library from https://github.com/watterott/Arduino-Libs/archive/master.zip
#include <digitalWriteFast.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define PSerial Serial
#define PSerial1 Serial1
// SERIAL is always enabled SERIAL1 and SERIAL4 are optional
#define HAL_SERIAL1_ENABLED

// New symbol for the default I2C port -------------------------------------------------------------
#define HAL_Wire Wire

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 1000000.0

// initialised here and not in timer.ino
void TIMER1_COMPA_vect(void);

HardwareTimer itimer3(3);
void TIMER3_COMPA_vect(void);

HardwareTimer itimer4(4);
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
  itimer3.pause();

  //itimer3.setPrescaleFactor(SMT32_PRESCALER);
  //itimer3.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer3.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt on channel 3
  itimer3.setChannel3Mode(TIMER_OUTPUT_COMPARE);
  itimer3.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  itimer3.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer3.refresh();

  // Start the timer counting
  itimer3.resume();
  
  // Pause the timer while we're configuring it
  itimer4.pause();

  //itimer4.setPrescaleFactor(SMT32_PRESCALER);
  //itimer4.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer4.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt on channel 4
  itimer4.setChannel4Mode(TIMER_OUTPUT_COMPARE);
  itimer4.setCompare(TIMER_CH4, 1);  // Interrupt 1 count after each update
  itimer4.attachInterrupt(TIMER_CH4, TIMER4_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer4.refresh();

  // Start the timer counting
  itimer4.resume();

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  nvic_irq_set_priority(NVIC_TIMER1_CC, 2);
  // set the motor timers to run at the highest priority
  nvic_irq_set_priority(NVIC_TIMER3, 0);
  nvic_irq_set_priority(NVIC_TIMER4, 0);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);
HardwareTimer itimer1(1);

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);

  // This code is based on the following document
  //
  // http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.10/lang/api/hardwaretimer.html#lang-hardwaretimer
  // And this document:
  //
  // http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/timers.html
  // Pause the timer while we're configuring it
  itimer1.pause();

  //itimer1.setPrescaleFactor(STM32_PRESCALER);
  //itimer1.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer1.setPeriod((float)iv * 0.0625); // in microseconds

  // Set up an interrupt on channel 1
  itimer1.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  itimer1.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  itimer1.attachInterrupt(TIMER_CH1, TIMER1_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer1.refresh();

  // Start the timer counting
  itimer1.resume();
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

void QuickSetIntervalAxis1(uint32_t r) {
  // Pause the timer while we're configuring it
  itimer3.pause();

  //itimer3.setPrescaleFactor(STM32_PRESCALER);
  //itimer3.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer3.setPeriod(r); // in microseconds

  // Set up an interrupt on channel 1
  itimer3.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  itimer3.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  itimer3.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer3.refresh();

  // Start the timer counting
  itimer3.resume();
}

void QuickSetIntervalAxis2(uint32_t r) {
  // Pause the timer while we're configuring it
  itimer4.pause();

  //itimer4.setPrescaleFactor(STM32_PRESCALER);
  //itimer4.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer4.setPeriod(r); // in microseconds

  // Set up an interrupt on channel 1
  itimer4.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  itimer4.setCompare(TIMER_CH4, 1);  // Interrupt 1 count after each update
  itimer4.attachInterrupt(TIMER_CH4, TIMER4_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer4.refresh();

  // Start the timer counting
  itimer4.resume();
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

