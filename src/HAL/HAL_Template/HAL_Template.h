// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more Platform_Name boards based on different lines are supported
#define __Platform_Name__

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 16

// width of step pulse
#define HAL_PULSE_WIDTH 500

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional
//#define SerialB Serial1

// New symbol for the default I2C port -------------------------------------------------------------
#define HAL_Wire Wire

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
void HAL_Init(void) {
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return -999;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 1000000

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);  // Sidereal timer
void TIMER3_COMPA_vect(void);  // Axis1 RA/Azm timer
void TIMER4_COMPA_vect(void);  // Axis2 DEC/Alt timer

// if possible run TIMER3 and TIMER4 at the highest priority level with everything else below

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

void Timer1SetInterval(long iv, double rateRatio) {
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

// iv: the interval in microseconds*16
// TPSM: the pulse vs. sqwave mode speed up (1.0 or 0.5 respectively)
// nextRate: the rate to be passed to QuickSetIntervalAxisn() in microseconds*(F_COMP/1000000.0) units
// nextRep:  the rate slow-down multiplier.  ISR is called this many times before allowed to run, set to 0 to run every time.
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
void QuickSetIntervalAxis1(uint32_t r) {
}
void QuickSetIntervalAxis2(uint32_t r) {
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define Axis1StepPinHIGH digitalWrite(Axis1StepPin, HIGH)
#define Axis1StepPinLOW digitalWrite(Axis1StepPin, LOW)
#define Axis1DirPinHIGH digitalWrite(Axis1DirPin, HIGH)
#define Axis1DirPinLOW digitalWrite(Axis1DirPin, LOW)

#define Axis2StepPinHIGH digitalWrite(Axis2StepPin, HIGH)
#define Axis2StepPinLOW digitalWrite(Axis2StepPin, LOW)
#define Axis2DirPinHIGH digitalWrite(Axis2DirPin, HIGH)
#define Axis2DirPinLOW digitalWrite(Axis2DirPin, LOW)
