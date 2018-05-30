// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
// __SAM3X8E__

// Lower limit (fastest) step rate in uS for this platform -------------------------------------------
#define MaxRate_LowerLimit 16

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
#define SerialB Serial1
// SerialA is always enabled, SerialB and SerialC are optional
#define HAL_SERIAL_B_ENABLED

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
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

// Get this library from https://github.com/ivanseidel/DueTimer
#include <DueTimer.h>

#include <avr/dtostrf.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 1000000.0

void TIMER1_COMPA_vect(void);
void TIMER3_COMPA_vect(void);
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  Timer5.attachInterrupt(TIMER1_COMPA_vect);
  SiderealClockSetInterval(siderealInterval);
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  Timer3.attachInterrupt(TIMER3_COMPA_vect).start(150); // give 0.15ms before the timers fire (and update to the correct rate)
  Timer4.attachInterrupt(TIMER4_COMPA_vect).start(150);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  Timer5.start((float)iv * 0.0625);
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

void QuickSetIntervalAxis1(uint32_t r) {
  Timer3.setPeriod(r);
}

void QuickSetIntervalAxis2(uint32_t r) {
  Timer4.setPeriod(r);
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define StepPinAxis1_HIGH digitalWrite(Axis1StepPin, HIGH)
#define StepPinAxis1_LOW digitalWrite(Axis1StepPin, LOW)
#define DirPinAxis1_HIGH digitalWrite(Axis1DirPin, HIGH)
#define DirPinAxis1_LOW digitalWrite(Axis1DirPin, LOW)

#define StepPinAxis2_HIGH digitalWrite(Axis2StepPin, HIGH)
#define StepPinAxis2_LOW digitalWrite(Axis2StepPin, LOW)
#define DirPinAxis2_HIGH digitalWrite(Axis2DirPin, HIGH)
#define DirPinAxis2_LOW digitalWrite(Axis2DirPin, LOW)

