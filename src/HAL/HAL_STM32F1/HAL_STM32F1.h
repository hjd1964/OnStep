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

/*
   New symbols for the Serial ports so they can be remapped if necessary -----------------------------

   Which ports to use for WiFi, and USB?

   The hardware serial ports and pins for STM32F103 are:
     USART1: TX PA9,  RX PA10
     USART2: TX PA2,  RX PA3
     USART3: TX PB10, RX PB11

   Different flashing methods will remap the port numbers differently, as follows.

   For "DFU" (STM32duino), or ST-Link V2, the ports are:
     Serial1 -> USART 1
     Serial2 -> USART 2
     Serial3 -> USART 3

   If "Serial" method, using a UART to TTL converter on pins A9 and A10, the ports are:
     Serial  -> USART 1
     Serial1 -> USART 2
     Serial2 -> USART 3
 */
#if defined(SERIAL_USB)
  #define SerialA Serial1
  #define SerialB Serial3
#else
  #define SerialA Serial
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
// General purpose initialize for HAL

void HAL_Init(void) {
  // Make sure that debug pins are not reserved, and therefore usable as GPIO
  disableDebugPorts();
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return -999;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

#if (F_CPU!=48000000) && (F_CPU!=72000000)
  #error "OnStep STM32 HAL timer design might not be appropraite for this clock rate, choose 72MHz or 48MHz."
#endif

// This code is based on the following document
//
// http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.10/lang/api/hardwaretimer.html#lang-hardwaretimer
// And this document:
//
// http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/timers.html
// Pause the timer while we're configuring it

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 4000000.0
#define ISR(f) void f (void)

// Sidereal timer is on STM32 Hardware Timer 1
HardwareTimer Timer_Sidereal(1);
void TIMER1_COMPA_vect(void);

// Axis1 motor timer is on STM32 Hardware Timer 3
HardwareTimer Timer_Axis1(3);
void TIMER3_COMPA_vect(void);

// Axis2 motor timer is on STM32 Hardware Timer 2
// Even though the called function is TIMER4_COMPA_vect(). The reason is that Timer 4
// on the STM32 is used for I2C, which is required for the EEPROM on the RTC module
HardwareTimer Timer_Axis2(2);
void TIMER4_COMPA_vect(void);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  Timer_Sidereal.pause();

  Timer_Sidereal.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
  Timer_Sidereal.setCompare(TIMER_CH1, 1); // Interrupt 1 count after each update
  Timer_Sidereal.attachInterrupt(TIMER_CH1, TIMER1_COMPA_vect);

  // Set up period
  // 0.166... us per count (72/12 = 6MHz) 10.922 ms max, more than enough for the 1/100 second sidereal clock +/- any PPS adjustment for xo error
  unsigned long psf = F_CPU/6000000; // for example, 72000000/6000000 = 12
  Timer_Sidereal.setPrescaleFactor(psf);
  Timer_Sidereal.setOverflow(round((60000.0/1.00273790935)/3.0));

  // Refresh the timer's count, prescale, and overflow
  Timer_Sidereal.refresh();

  // Start the timer counting
  Timer_Sidereal.resume();
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // ===== Axis 1 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis1.pause();

  // Set up an interrupt on channel 3
  Timer_Axis1.setMode(TIMER_CH3, TIMER_OUTPUT_COMPARE);
  Timer_Axis1.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  Timer_Axis1.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Set up period
  // 0.25... us per count (72/18 = 4MHz) 16.384 ms max, good resolution for accurate motor timing and still a reasonable range (for lower steps per degree)
  unsigned long psf = F_CPU/4000000; // for example, 72000000/4000000 = 18
  Timer_Axis1.setPrescaleFactor(psf);
  Timer_Axis1.setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis1.refresh();

  // Start the timer counting
  Timer_Axis1.resume();
  
  // ===== Axis 2 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis2.pause();

  // Set up an interrupt on channel 2
  Timer_Axis2.setMode(TIMER_CH2, TIMER_OUTPUT_COMPARE);
  Timer_Axis2.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  Timer_Axis2.attachInterrupt(TIMER_CH2, TIMER4_COMPA_vect);

  // Set up period
  Timer_Axis2.setPrescaleFactor(psf);
  Timer_Axis2.setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis2.refresh();

  // Start the timer counting
  Timer_Axis2.resume();

  // ===== Set timer priorities =====
  // set the 1/100 second sidereal clock timer to run at the second highest priority
  nvic_irq_set_priority(NVIC_TIMER1_CC, 2);

  // set the motor timers to run at the highest priority
  nvic_irq_set_priority(NVIC_TIMER2, 0);
  nvic_irq_set_priority(NVIC_TIMER3, 0);
}

// Set timer1 to interval (in 0.0625 microsecond units), for the 1/100 second sidereal timer
void Timer1SetInterval(long iv, double rateRatio) {
  Timer_Sidereal.setOverflow(round((((double)iv/16.0)*6.0)/rateRatio)); // our "clock" ticks at 6MHz due to the pre-scaler setting
}

// Set the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers
void QuickSetIntervalAxis1(uint32_t r) {
  // Set up period
  Timer_Axis1.setOverflow(r);
}

void QuickSetIntervalAxis2(uint32_t r) {
  // Set up period
  Timer_Axis2.setOverflow(r);
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
