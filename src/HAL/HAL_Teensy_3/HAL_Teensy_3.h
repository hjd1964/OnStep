// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __ARM_Teensy3__

// Lower limit (fastest) step rate in uS for this platform
#if defined(__MK64FX512__) 
  #define MaxRate_LowerLimit 12
  #define HAL_FAST_PROCESSOR
#elif defined(__MK66FX1M0__)
  #define MaxRate_LowerLimit 4
  #define HAL_FAST_PROCESSOR
#else
  #define MaxRate_LowerLimit 16
#endif

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional
#define SerialB Serial1
#define HAL_SERIAL_B_ENABLED
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  #define SerialC Serial4
  #define HAL_SERIAL_C_ENABLED
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define HAL_Wire Wire1
#else
#define HAL_Wire Wire
#endif

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM.h"
#endif

// Use an RTC (Real Time Clock) if present -----------------------------------------------------------
#include "../drivers/RTCw.h"

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
void HAL_Init(void) {
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
#if defined(__MK64FX512__)
  int Tpin=70;
#elif defined(__MK66FX1M0__)
  int Tpin=70;
#else // Teensy3.0,3.1,3.2
  int Tpin=38;
#endif
  // delta of -1.715 mV/C where 25C measures 719 mV
  analogReadResolution(12);
  float v=(analogRead(Tpin)/4096.0)*3.3;
  float t=(-(v-0.719)/0.001715)+25.0;
  analogReadResolution(10);
  return t;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP F_BUS

IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // set the system timer for millis() to the second highest priority
  SCB_SHPR3 = (32 << 24) | (SCB_SHPR3 & 0x00FFFFFF);

  itimer3.begin(TIMER3_COMPA_vect, (float)128 * 0.0625);
  itimer4.begin(TIMER4_COMPA_vect, (float)128 * 0.0625);

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH0, 32);
  // set the motor timers to run at the highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH1, 0);
  NVIC_SET_PRIORITY(IRQ_PIT_CH2, 0);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);

IntervalTimer itimer1;

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  analogWriteResolution(8);
  SiderealClockSetInterval(siderealInterval);
}

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  itimer1.begin(TIMER1_COMPA_vect, (float)iv * 0.0625);
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

void QuickSetIntervalAxis1(uint32_t r) {
  PIT_LDVAL1=r;
}

void QuickSetIntervalAxis2(uint32_t r) {
  PIT_LDVAL2=r;
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
