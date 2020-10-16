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
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  // defaults to 0x57 and 4KB
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
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
  cli(); startTime=micros(); delayNanoseconds(65535); npp=micros(); sei(); npp=((int32_t)(npp-startTime)*1000)/63335;
  if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp;
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
// Fast port writing help, etc.

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define a1STEP_H digitalWrite(Axis1_STEP, HIGH)
#define a1STEP_L digitalWrite(Axis1_STEP, LOW)
#define a1DIR_H digitalWrite(Axis1_DIR, HIGH)
#define a1DIR_L digitalWrite(Axis1_DIR, LOW)

#define a2STEP_H digitalWrite(Axis2_STEP, HIGH)
#define a2STEP_L digitalWrite(Axis2_STEP, LOW)
#define a2DIR_H digitalWrite(Axis2_DIR, HIGH)
#define a2DIR_L digitalWrite(Axis2_DIR, LOW)

// fast bit-banged SPI should hit an ~1 MHz bitrate for TMC drivers
#define delaySPI delayNanoseconds(500)

#define a1CS_H digitalWrite(Axis1_M2,HIGH)
#define a1CS_L digitalWrite(Axis1_M2,LOW)
#define a1CLK_H digitalWrite(Axis1_M1,HIGH)
#define a1CLK_L digitalWrite(Axis1_M1,LOW)
#define a1SDO_H digitalWrite(Axis1_M0,HIGH)
#define a1SDO_L digitalWrite(Axis1_M0,LOW)
#define a1M0(P) digitalWrite(Axis1_M0,(P))
#define a1M1(P) digitalWrite(Axis1_M1,(P))
#define a1M2(P) digitalWrite(Axis1_M2,(P))

#define a2CS_L digitalWrite(Axis2_M2,LOW)
#define a2CS_H digitalWrite(Axis2_M2,HIGH)
#define a2CLK_L digitalWrite(Axis2_M1,LOW)
#define a2CLK_H digitalWrite(Axis2_M1,HIGH)
#define a2SDO_H digitalWrite(Axis2_M0,HIGH)
#define a2SDO_L digitalWrite(Axis2_M0,LOW)
#define a2M0(P) digitalWrite(Axis2_M0,(P))
#define a2M1(P) digitalWrite(Axis2_M1,(P))
#define a2M2(P) digitalWrite(Axis2_M2,(P))
