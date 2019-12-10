// Platform setup ------------------------------------------------------------------------------------

// Lower limit (fastest) step rate in uS for this platform, width of step pulse, and set HAL_FAST_PROCESSOR is needed
#if defined(__IMXRT1052__) || defined(__IMXRT1062__) 
  #define HAL_MAXRATE_LOWER_LIMIT 1.5
  #define HAL_PULSE_WIDTH 0 // effectively disable pulse mode since the pulse width is unknown at this time
  #define HAL_FAST_PROCESSOR
#endif

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional
#define SerialB Serial1
#define HAL_SERIAL_B_ENABLED

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32_PLUS)
  #include "../drivers/NV_I2C_EEPROM_AT24C32_PLUS.h"
#elif defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#include "imxrt.h"

void HAL_Init(void) {
  analogReadResolution(10);

  // clear/make available all PIT timers
  CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);
  PIT_MCR = 1;
  PIT_TCTRL0=0;
  PIT_TCTRL1=0;
  PIT_TCTRL2=0;
  PIT_TCTRL3=0;
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return 25.0;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_BUS 24000000
#define F_COMP F_BUS

static IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

static IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // set the system timer for millis() to the second highest priority
//  SCB_SHPR3 = (32 << 24) | (SCB_SHPR3 & 0x00FFFFFF);

  if (!itimer3.begin(TIMER3_COMPA_vect, (float)128 * 0.0625)) Serial.println("Error assigning timer3");
  itimer3.priority(0);
  if (!itimer4.begin(TIMER4_COMPA_vect, (float)128 * 0.0625)) Serial.println("Error assigning timer4");
  itimer4.priority(0);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);

static IntervalTimer itimer1;

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  analogWriteResolution(8);
  SiderealClockSetInterval(siderealInterval);
}

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  if (!itimer1.begin(TIMER1_COMPA_vect, (float)iv * 0.0625)) Serial.println("Error assigning timer1");
  itimer1.priority(32);
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

// prepare to set Axis1/2 hw timers to interval (in microseconds*16), maximum time is about 134 seconds
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // 0.262 * 512 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*64L) { t++; iv=i/t; if (t==512) { iv=65535L*64L; break; } }
  cli(); *nextRate=iv*TPSM; *nextRep=t; sei();
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
void QuickSetIntervalAxis1(uint32_t r) {
  itimer3.update((float)r * 0.0625);
}
void QuickSetIntervalAxis2(uint32_t r) {
  itimer4.update((float)r * 0.0625);
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define Axis1StepPinHIGH digitalWriteFast(Axis1StepPin, HIGH)
#define Axis1StepPinLOW digitalWriteFast(Axis1StepPin, LOW)
#define Axis1DirPinHIGH digitalWriteFast(Axis1DirPin, HIGH)
#define Axis1DirPinLOW digitalWriteFast(Axis1DirPin, LOW)

#define Axis2StepPinHIGH digitalWriteFast(Axis2StepPin, HIGH)
#define Axis2StepPinLOW digitalWriteFast(Axis2StepPin, LOW)
#define Axis2DirPinHIGH digitalWriteFast(Axis2DirPin, HIGH)
#define Axis2DirPinLOW digitalWriteFast(Axis2DirPin, LOW)
