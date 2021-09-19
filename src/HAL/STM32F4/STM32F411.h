// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

#define HAL_MAXRATE_LOWER_LIMIT 16   // Lower limit (fastest) step rate in uS (in SQW mode) assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH         500  // Width of step pulse

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional

HardwareSerial HWSerial2(PA3, PA2); // RX2, TX2
#define SerialB HWSerial2
#define HAL_SERIAL_B_ENABLED

HardwareSerial HWSerial1(PA10, PA9); // RX1, TX1
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SerialC HWSerial1
  #define HAL_SERIAL_C_ENABLED
#endif
  
// HardwareSerial HWSerial1(PA10, PA9);  // RX1, TX1
// HardwareSerial HWSerial2(PA3, PA2);   // RX2, TX2
// HardwareSerial HWSerial6(PA12, PA11); // RX6, TX6

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#if defined(NV_M24C32)
  // The MaxPCB3I has an 8192 byte EEPROM built-in (rated for 5M write cycles)
  #define NV_ENDURANCE HIGH
  #define E2END 8191
  #define I2C_EEPROM_ADDRESS 0x50
  #define HAL_WIRE_RESET_AFTER_CONNECT
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  // defaults to 0x57 and 4KB as used on DS3231 RTC modules
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#endif

//----------------------------------------------------------------------------------------------------
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
  startTime=micros(); delayNanoseconds(65535); npp=micros(); npp=((int32_t)(npp-startTime)*1000)/63335;
  if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp;
  analogWriteResolution(8);
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return -999;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers
// frequency compensation for adjusting microseconds to timer counts
#define F_COMP 4000000

#define ISR(f) void f (void)

// was TIM1 which collided with PWM on the Reticle pin
#define TIM_SIDEREAL   TIM4
#define TIM_AXIS1      TIM9
#define TIM_AXIS2      TIM11

HardwareTimer *Timer_Sidereal = new HardwareTimer(TIM_SIDEREAL);
HardwareTimer *Timer_Axis1    = new HardwareTimer(TIM_AXIS1);
HardwareTimer *Timer_Axis2    = new HardwareTimer(TIM_AXIS2);

#define SIDEREAL_CH  1
#define AXIS1_CH     1
#define AXIS2_CH     1

// Sidereal timer
void TIMER1_COMPA_vect(void);

// Axis1 motor timer
void TIMER3_COMPA_vect(void);

// Axis2 motor timer
void TIMER4_COMPA_vect(void);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  uint32_t psf;

  Timer_Sidereal->pause();
  Timer_Sidereal->setMode(SIDEREAL_CH, TIMER_OUTPUT_COMPARE);
  Timer_Sidereal->setCaptureCompare(SIDEREAL_CH, 1); // Interrupt 1 count after each update
  Timer_Sidereal->attachInterrupt(SIDEREAL_CH, TIMER1_COMPA_vect);

  // Set up period
  // 0.166... us per count (72/12 = 6MHz) 10.922 ms max, more than enough for the 1/100 second sidereal clock +/- any PPS adjustment for xo error
  psf = Timer_Sidereal->getTimerClkFreq()/6000000; // for example, 72000000/6000000 = 12
  Timer_Sidereal->setPrescaleFactor(psf);
  Timer_Sidereal->setOverflow(round((60000.0/1.00273790935)/3.0));

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  Timer_Sidereal->setInterruptPriority(2, 2);

  // Start the timer counting
  Timer_Sidereal->resume();

  // Refresh the timer's count, prescale, and overflow
  Timer_Sidereal->refresh();
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  uint32_t psf;
  // ===== Axis 1 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis1->pause();

  // Set up an interrupt on channel 3
  Timer_Axis1->setMode(AXIS1_CH, TIMER_OUTPUT_COMPARE);
  Timer_Axis1->setCaptureCompare(AXIS1_CH, 1);  // Interrupt 1 count after each update
  Timer_Axis1->attachInterrupt(AXIS1_CH, TIMER3_COMPA_vect);

  // Set up period
  // 0.25... us per count (72/18 = 4MHz) 16.384 ms max, good resolution for accurate motor timing and still a reasonable range (for lower steps per degree)
  psf = Timer_Axis1->getTimerClkFreq()/F_COMP; // for example, 72000000/4000000 = 18
  Timer_Axis1->setPrescaleFactor(psf);
  Timer_Axis1->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // set the motor timer to run at the highest priority
  Timer_Axis1->setInterruptPriority(1, 0);

  // Start the timer counting
  Timer_Axis1->resume();
  
  // Refresh the timer's count, prescale, and overflow
  Timer_Axis1->refresh();

  // ===== Axis 2 Timer =====
  // Pause the timer while we're configuring it
  Timer_Axis2->pause();

  // Set up an interrupt on channel 2
  Timer_Axis2->setMode(AXIS2_CH, TIMER_OUTPUT_COMPARE);
  Timer_Axis2->setCaptureCompare(AXIS2_CH, 1);  // Interrupt 1 count after each update
  Timer_Axis2->attachInterrupt(AXIS2_CH, TIMER4_COMPA_vect);

  // Set up period
  psf = Timer_Axis2->getTimerClkFreq()/F_COMP; // for example, 72000000/4000000 = 18
  Timer_Axis2->setPrescaleFactor(psf);
  Timer_Axis2->setOverflow(65535); // allow enough time that the sidereal clock will tick

  // set the motor timer to run at the highest priority
  Timer_Axis2->setInterruptPriority(1, 1);

  // Start the timer counting
  Timer_Axis2->resume();

  // Refresh the timer's count, prescale, and overflow
  Timer_Axis2->refresh();
}

// Set timer1 to interval (in 0.0625 microsecond units), for the 1/100 second sidereal timer
void Timer1SetInterval(long iv, double rateRatio) {
  Timer_Sidereal->setOverflow(round((((double)iv/16.0)*6.0)/rateRatio)); // our "clock" ticks at 6MHz due to the pre-scaler setting
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

#define TIMER_RATE_MHZ         4L   // STM32 motor timers run at 4 MHz
#define TIMER_RATE_16MHZ_TICKS 4L   // 16L/TIMER_RATE_MHZ, 4x slower than the default 16MHz

// prepare to set Axis1/2 hw timers to interval (in 1/16 microsecond units)
void PresetTimerInterval(long iv, bool TPS, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // maximum time is about 134 seconds
  if (iv>2144000000) iv=2144000000;

  // minimum time is 1 micro-second
  if (iv < 16) iv=16;

  // TPS (timer pulse step) == false for SQW mode and double the timer rate
  if (!TPS) iv/=2L;

  iv/=TIMER_RATE_16MHZ_TICKS;
  uint32_t reps = (iv/65536)+1;
  uint32_t i = iv/reps-1;
  cli(); *nextRate=i; *nextRep=reps; sei();
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
#define QuickSetIntervalAxis1(r) WRITE_REG(TIM_AXIS1->ARR, r)
#define QuickSetIntervalAxis2(r) WRITE_REG(TIM_AXIS2->ARR, r)

// --------------------------------------------------------------------------------------------------
// Fast port writing help, etc.

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define a1STEP_H WRITE_REG(Axis1_StpPORT->BSRR, Axis1_StpBIT)
#define a1STEP_L WRITE_REG(Axis1_StpPORT->BSRR, Axis1_StpBIT << 16)
#define a1DIR_H  WRITE_REG(Axis1_DirPORT->BSRR, Axis1_DirBIT)
#define a1DIR_L  WRITE_REG(Axis1_DirPORT->BSRR, Axis1_DirBIT << 16)

#define a2STEP_H WRITE_REG(Axis2_StpPORT->BSRR, Axis2_StpBIT)
#define a2STEP_L WRITE_REG(Axis2_StpPORT->BSRR, Axis2_StpBIT << 16)
#define a2DIR_H  WRITE_REG(Axis2_DirPORT->BSRR, Axis2_DirBIT)
#define a2DIR_L  WRITE_REG(Axis2_DirPORT->BSRR, Axis2_DirBIT << 16)

// fast bit-banged SPI should hit an ~1 MHz bitrate for TMC drivers
#define delaySPI delayNanoseconds(250)

#define a1CS_H WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT)
#define a1CS_L WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT << 16)
#define a1CLK_H WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT)
#define a1CLK_L WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT << 16)
#define a1SDO_H WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT)
#define a1SDO_L WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT << 16)
#define a1M0(P) if (P) WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT); else WRITE_REG(Axis1_M0PORT->BSRR, Axis1_M0BIT << 16)
#define a1M1(P) if (P) WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT); else WRITE_REG(Axis1_M1PORT->BSRR, Axis1_M1BIT << 16)
#define a1M2(P) if (P) WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT); else WRITE_REG(Axis1_M2PORT->BSRR, Axis1_M2BIT << 16)

#define a2CS_H WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT)
#define a2CS_L WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT << 16)
#define a2CLK_H WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT)
#define a2CLK_L WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT << 16)
#define a2SDO_H WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT)
#define a2SDO_L WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT << 16)
#define a2M0(P) if (P) WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT); else WRITE_REG(Axis2_M0PORT->BSRR, Axis2_M0BIT << 16)
#define a2M1(P) if (P) WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT); else WRITE_REG(Axis2_M1PORT->BSRR, Axis2_M1BIT << 16)
#define a2M2(P) if (P) WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT); else WRITE_REG(Axis2_M2PORT->BSRR, Axis2_M2BIT << 16)
