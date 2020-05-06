// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#undef boolean
#define boolean bool

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 12

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
//*/

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
#define F_COMP 4000000.0

#define ISR(f) void f (void)

static HardwareTimer *HwTimer_Sidereal = new HardwareTimer(TIM1);
static HardwareTimer *HwTimer_Axis1    = new HardwareTimer(TIM2); // 32bit timer
static HardwareTimer *HwTimer_Axis2    = new HardwareTimer(TIM5); // 32bit timer

// Sidereal timer is on STM32 Hardware Timer 1
void Timer_Sidereal();
void TIMER1_COMPA_vect(void);

// Axis1 motor timer is on STM32 Hardware Timer 3
void Timer_Axis1();
void TIMER3_COMPA_vect(void);

// Axis2 motor timer is on STM32 Hardware Timer 2
// Even though the called function is TIMER4_COMPA_vect(). The reason is that Timer 4
// on the STM32F1 is used for I2C, which is required for the EEPROM on the RTC module
void Timer_Axis2();
void TIMER4_COMPA_vect(void);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  HwTimer_Sidereal->pause();
  //HwTimer_Sidereal->setMode(1, TIMER_OUTPUT_COMPARE);
  HwTimer_Sidereal->setCaptureCompare(1, 1); // Interrupt 1 count after each update
  HwTimer_Sidereal->attachInterrupt(TIMER1_COMPA_vect);

  // Set up period
  // 0.166... us per count (72/12 = 6MHz) 10.922 ms max, more than enough for the 1/100 second sidereal clock +/- any PPS adjustment for xo error
  unsigned long psf = (F_CPU/2.0)/6000000; // for example, 72000000/6000000 = 12
  HwTimer_Sidereal->setPrescaleFactor(psf);
  HwTimer_Sidereal->setOverflow(round((60000.0/1.00273790935)/3.0));

  // Start the timer counting
  HwTimer_Sidereal->resume();
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // ===== Axis 1 Timer =====
  // Pause the timer while we're configuring it
  HwTimer_Axis1->pause();

  // Set up an interrupt on channel 3
  //HwTimer_Axis1.setMode(3, TIMER_OUTPUT_COMPARE);
  HwTimer_Axis1->setCaptureCompare(3, 1);  // Interrupt 1 count after each update
  HwTimer_Axis1->attachInterrupt(TIMER3_COMPA_vect);

  // Set up period
  // 0.25... us per count (72/18 = 4MHz) 16.384 ms max, good resolution for accurate motor timing and still a reasonable range (for lower steps per degree)
  unsigned long psf = (F_CPU/2.0)/4000000; // for example, 72000000/4000000 = 18
  HwTimer_Axis1->setPrescaleFactor(psf);
  HwTimer_Axis1->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  //HwTimer_Axis1.refresh();

  // Start the timer counting
  HwTimer_Axis1->resume();
  
  // ===== Axis 2 Timer =====
  // Pause the timer while we're configuring it
  HwTimer_Axis2->pause();

  // Set up an interrupt on channel 2
  //HwTimer_Axis2.setMode(2, TIMER_OUTPUT_COMPARE);
  HwTimer_Axis2->setCaptureCompare(2, 1);  // Interrupt 1 count after each update
  HwTimer_Axis2->attachInterrupt(TIMER4_COMPA_vect);

  // Set up period
  HwTimer_Axis2->setPrescaleFactor(psf);
  HwTimer_Axis2->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // Refresh the timer's count, prescale, and overflow
  //HwTimer_Axis2.refresh();

  // Start the timer counting
  HwTimer_Axis2->resume();

  // ===== Set timer priorities =====
  // set the 1/100 second sidereal clock timer to run at the second highest priority
  //nvic_irq_set_priority(NVIC_TIMER1_CC, 2);

  // set the motor timers to run at the highest priority
  //nvic_irq_set_priority(NVIC_TIMER2, 0);
  //nvic_irq_set_priority(NVIC_TIMER3, 0);
}

// Set timer1 to interval (in 0.0625 microsecond units), for the 1/100 second sidereal timer
void Timer1SetInterval(long iv, double rateRatio) {
  HwTimer_Sidereal->setOverflow(round((((double)iv/16.0)*6.0)/rateRatio)); // our "clock" ticks at 6MHz due to the pre-scaler setting
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

// prepare to set Axis1/2 hw timers to interval (in microseconds*16), maximum time is about 134 seconds
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*8L) { t*=2; iv=i/t; if (t==4096) { iv=65535L*8L; break; } }
  cli(); *nextRate=((F_COMP/1000000.0) * (iv*0.0625) * TPSM); *nextRep=t; sei();
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
void QuickSetIntervalAxis1(uint32_t r) {
  HwTimer_Axis1->setOverflow(r);
}
void QuickSetIntervalAxis2(uint32_t r) {
  HwTimer_Axis2->setOverflow(r);
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
