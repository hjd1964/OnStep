// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// This is to avoid lots of warnings
#undef boolean
#define boolean bool

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 20 // FIXME

// Width of step pulse
#define HAL_PULSE_WIDTH 1200 // FIXME

#include <HardwareTimer.h>

// Get this library from https://github.com/watterott/Arduino-Libs/archive/master.zip
#include <digitalWriteFast.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

#define SerialA Serial
#define SerialB Serial1
#define SerialC Serial3

// SerialA is always enabled, SerialB and SerialC are optional
#define HAL_SERIAL_B_ENABLED
#define HAL_SERIAL_C_ENABLED

// New symbol for the default I2C port -------------------------------------------------------------
#define HAL_Wire Wire

// Non-volatile storage ------------------------------------------------------------------------------
// The FYSETC S6 has a 2047 byte EEPROM built-in
#undef E2END
#include "../drivers/NV_I2C_EEPROM_24LC16_C.h"

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
//#define F_COMP 4000000.0

#define ISR(f) void f (void)

HardwareTimer *Timer_Sidereal = new HardwareTimer(TIM1);
HardwareTimer *Timer_Axis1    = new HardwareTimer(TIM2); // 32bit timer
HardwareTimer *Timer_Axis2    = new HardwareTimer(TIM5); // 32bit timer

// Timer frequency, depends on the STM32 timer type. See the datasheet.
// Note that Timer_Axis1 and Timer_Axis2 *MUST* be the exact same type since we have
// only a single function PresetTimerInterval() for both axes.
uint32_t timerFreq = Timer_Axis1->getTimerClkFreq();

// Sidereal timer
void TIMER1_COMPA_vect(void);

// Axis1 motor timer
void TIMER3_COMPA_vect(void);

// Axis2 motor timer
void TIMER4_COMPA_vect(void);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  Timer_Sidereal->pause();
  Timer_Sidereal->setMode(1, TIMER_OUTPUT_COMPARE);
  Timer_Sidereal->setCaptureCompare(1, 1); // Interrupt 1 count after each update
  Timer_Sidereal->attachInterrupt(1, TIMER1_COMPA_vect);

  // Set up period
  // 0.166... us per count (72/12 = 6MHz) 10.922 ms max, more than enough for the 1/100 second sidereal clock +/- any PPS adjustment for xo error
  unsigned long psf = F_CPU/6000000; // for example, 72000000/6000000 = 12
  Timer_Sidereal->setPrescaleFactor(psf);
  Timer_Sidereal->setOverflow(round((60000.0/1.00273790935)/3.0));

  // Refresh the timer's count, prescale, and overflow
  Timer_Sidereal->refresh();

  // Start the timer counting
  Timer_Sidereal->resume();
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // ===== Axis 1 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis1->pause();

  // Set up an interrupt on channel 3
  Timer_Axis1->setMode(3, TIMER_OUTPUT_COMPARE);
  Timer_Axis1->setCaptureCompare(3, 1);  // Interrupt 1 count after each update
  Timer_Axis1->attachInterrupt(3, TIMER3_COMPA_vect);

  // Set up period
  // 0.25... us per count (72/18 = 4MHz) 16.384 ms max, good resolution for accurate motor timing and still a reasonable range (for lower steps per degree)
  unsigned long psf = F_CPU/4000000; // for example, 72000000/4000000 = 18
  Timer_Axis1->setPrescaleFactor(psf);
  Timer_Axis1->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis1->refresh();

  // Start the timer counting
  Timer_Axis1->resume();
  
  // ===== Axis 2 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis2->pause();

  // Set up an interrupt on channel 2
  Timer_Axis2->setMode(2, TIMER_OUTPUT_COMPARE);
  Timer_Axis2->setCaptureCompare(2, 1);  // Interrupt 1 count after each update
  Timer_Axis2->attachInterrupt(2, TIMER4_COMPA_vect);

  // Set up period
  //Timer_Axis2->setPrescaleFactor(psf);
  Timer_Axis2->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis2->refresh();

  // Start the timer counting
  Timer_Axis2->resume();

  // ===== Set timer priorities =====
  // set the 1/100 second sidereal clock timer to run at the second highest priority
  Timer_Sidereal->setInterruptPriority(2, 0);

  // set the motor timers to run at the highest priority
  Timer_Axis1->setInterruptPriority(0, 0);
  Timer_Axis2->setInterruptPriority(0, 0);
}

// Set timer1 to interval (in 0.0625 microsecond units), for the 1/100 second sidereal timer
void Timer1SetInterval(long iv, double rateRatio) {
  Timer_Sidereal->setOverflow(round((((double)iv/16.0)*6.0)/rateRatio)); // our "clock" ticks at 6MHz due to the pre-scaler setting
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

// prepare to set Axis1/2 hw timers to interval (in microseconds*16), maximum time is about 134 seconds
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*8L) { t*=2; iv=i/t; if (t==4096) { iv=65535L*8L; break; } }
  cli(); *nextRate=((timerFreq/1000000) * (iv*0.0625) * TPSM); *nextRep=t; sei();
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
void QuickSetIntervalAxis1(uint32_t r) {
  //Timer_Axis1->pause();

  Timer_Axis1->setOverflow(r);

  // Refresh the timer's count, prescale, and overflow
  //Timer_Axis1->refresh();

  // Start the timer counting
  //Timer_Axis1->resume();
}
void QuickSetIntervalAxis2(uint32_t r) {
  //Timer_Axis2->pause();
  Timer_Axis2->setOverflow(r);

  // Refresh the timer's count, prescale, and overflow
  //Timer_Axis2->refresh();

  // Start the timer counting
  //Timer_Axis2->resume();
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help, etc.

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define a1STEP_H digitalWriteFast(Axis1_STEP, HIGH)
#define a1STEP_L digitalWriteFast(Axis1_STEP, LOW)
#define a1DIR_H digitalWriteFast(Axis1_DIR, HIGH)
#define a1DIR_L digitalWriteFast(Axis1_DIR, LOW)

#define a2STEP_H digitalWriteFast(Axis2_STEP, HIGH)
#define a2STEP_L digitalWriteFast(Axis2_STEP, LOW)
#define a2DIR_H digitalWriteFast(Axis2_DIR, HIGH)
#define a2DIR_L digitalWriteFast(Axis2_DIR, LOW)

// fast bit-banged SPI should hit an ~1 MHz bitrate for TMC drivers
#define delaySPI delayNanoseconds(500)

#define a1CS_H digitalWriteFast(Axis1_M2,HIGH)
#define a1CS_L digitalWriteFast(Axis1_M2,LOW)
#define a1CLK_H digitalWriteFast(Axis1_M1,HIGH)
#define a1CLK_L digitalWriteFast(Axis1_M1,LOW)
#define a1SDO_H digitalWriteFast(Axis1_M0,HIGH)
#define a1SDO_L digitalWriteFast(Axis1_M0,LOW)
#define a1M0(P) digitalWriteFast(Axis1_M0,(P))
#define a1M1(P) digitalWriteFast(Axis1_M1,(P))
#define a1M2(P) digitalWriteFast(Axis1_M2,(P))

#define a2CS_H digitalWriteFast(Axis2_M2,HIGH)
#define a2CS_L digitalWriteFast(Axis2_M2,LOW)
#define a2CLK_H digitalWriteFast(Axis2_M1,HIGH)
#define a2CLK_L digitalWriteFast(Axis2_M1,LOW)
#define a2SDO_H digitalWriteFast(Axis2_M0,HIGH)
#define a2SDO_L digitalWriteFast(Axis2_M0,LOW)
#define a2M0(P) digitalWriteFast(Axis2_M0,(P))
#define a2M1(P) digitalWriteFast(Axis2_M1,(P))
#define a2M2(P) digitalWriteFast(Axis2_M2,(P))