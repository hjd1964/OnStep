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
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SerialD SerialUSB1
  #define SERIAL_D_BAUD_DEFAULT 9600
  #define HAL_SERIAL_D_ENABLED
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SerialE SerialUSB2
  #define SERIAL_E_BAUD_DEFAULT 9600
  #define HAL_SERIAL_E_ENABLED
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32_PLUS)
  #include "../drivers/NV_I2C_EEPROM_AT24C32_PLUS.h"
#elif defined(NV_AT24C32)
  // defaults to 0x57 and 4KB
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#include "imxrt.h"

void HAL_Initialize(void) {
  analogReadResolution(10);

  // clear/make available all PIT timers
//  CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);
//  PIT_MCR = 1;
//  PIT_TCTRL0=0;
//  PIT_TCTRL1=0;
//  PIT_TCTRL2=0;
//  PIT_TCTRL3=0;
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  return 25.0;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

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

#define F_BUS 16000000L                                  // we force the timer to run at 16MHz
#define TIMER_RATE_MHZ (F_BUS/1000000.0)                 // Teensy motor timers run at F_BUS Hz so use full resolution
#define TIMER_RATE_16MHZ_TICKS (16.0/TIMER_RATE_MHZ)     // 16.0/24.0 = 0.6666
const double timerRate16MHzTicks TIMER_RATE_16MHZ_TICKS; // make sure this is pre-calculated

// prepare to set Axis1/2 hw timers to interval (in 1/16 microsecond units)
void PresetTimerInterval(long iv, bool TPS, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // maximum time is about 134 seconds
  if (iv>2144000000) iv=2144000000;

  // minimum time is 1 micro-second
  if (iv<16) iv=16;

  // TPS (timer pulse step) == false for SQW mode and double the timer rate
  if (!TPS) iv/=2L;

  double fiv = iv/timerRate16MHzTicks;
  uint32_t reps = (fiv/4194304.0)+1.0;
  uint32_t i = fiv/reps;
  cli(); *nextRate=i; *nextRep=reps; sei();
}

// Must work from within the motor ISR timers, in microseconds*(F_COMP/1000000.0) units
void QuickSetIntervalAxis1(uint32_t r) {
  itimer3.update((float)r * 0.0625);
}
void QuickSetIntervalAxis2(uint32_t r) {
  itimer4.update((float)r * 0.0625);
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
