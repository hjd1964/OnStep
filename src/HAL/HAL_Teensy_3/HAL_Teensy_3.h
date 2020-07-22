// Platform setup ------------------------------------------------------------------------------------

// Lower limit (fastest) step rate in uS for this platform, width of step pulse, and set HAL_FAST_PROCESSOR is needed
#if defined(__MK64FX512__) 
  // for using the DAC as a digital output on Teensy3.5 A21=66 A22=67
  #define digitalWrite(x,y) { if (x==66 || x==67) { if ((y)==LOW) analogWrite(x,0); else analogWrite(x,255); } else digitalWrite(x,y); }
  #define HAL_MAXRATE_LOWER_LIMIT 12
  #define HAL_PULSE_WIDTH 750
  #define HAL_FAST_PROCESSOR
#elif defined(__MK66FX1M0__)
  // for using the DAC as a digital output on Teensy3.6 A21=66 A22=67
  #define digitalWrite(x,y) { if (x==66 || x==67) { if ((y)==LOW) analogWrite(x,0); else analogWrite(x,255); } else digitalWrite(x,y); }
  #if (F_CPU>=240000000)
    #define HAL_MAXRATE_LOWER_LIMIT 2
    #define HAL_PULSE_WIDTH 260
  #elif (F_CPU>=180000000)
    #define HAL_MAXRATE_LOWER_LIMIT 2.6
    #define HAL_PULSE_WIDTH 400
  #else
    #define HAL_MAXRATE_LOWER_LIMIT 4.8
    #define HAL_PULSE_WIDTH 500
  #endif
  #define HAL_FAST_PROCESSOR
#else
  // Teensy3.2,3.1,etc.
  #if (F_CPU>=120000000)
    #define HAL_MAXRATE_LOWER_LIMIT 10
    #define HAL_PULSE_WIDTH 800
  #elif (F_CPU>=96000000)
    #define HAL_MAXRATE_LOWER_LIMIT 12
    #define HAL_PULSE_WIDTH 900
  #elif (F_CPU>=72000000)
    #define HAL_MAXRATE_LOWER_LIMIT 14
    #define HAL_PULSE_WIDTH 1000
  #else
    #define HAL_MAXRATE_LOWER_LIMIT 28
    #define HAL_PULSE_WIDTH 1500
  #endif
#endif

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional
#define SerialB Serial1
#define HAL_SERIAL_B_ENABLED
#if (defined(__MK64FX512__) || defined(__MK66FX1M0__)) && SERIAL_C_BAUD_DEFAULT != OFF
  #define SerialC Serial4
  #define HAL_SERIAL_C_ENABLED
#endif
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
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define HAL_Wire Wire1
#else
#define HAL_Wire Wire
#endif

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32_PLUS)
  #include "../drivers/NV_I2C_EEPROM_AT24C32_PLUS.h"
#elif defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32_C.h"
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
void HAL_Init(void) {
  // calibrate delayNanoseconds()
  uint32_t startTime,npp;
  cli(); startTime=micros(); delayNanoseconds(65535); npp=micros(); sei(); npp=((int32_t)(npp-startTime)*1000)/63335;
  if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp;

  analogReadResolution(10);
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
  float v=(analogRead(Tpin)/1024.0)*3.3;
  float t=(-(v-0.719)/0.001715)+25.0;
  return t;
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

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
// Re-program interval for the motor timers

#define TIMER_RATE_MHZ (F_BUS/1000000.0)                 // Teensy motor timers run at F_BUS Hz so use full resolution
#define TIMER_RATE_16MHZ_TICKS (16.0/TIMER_RATE_MHZ)     // 16.0/TIMER_RATE_MHZ
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
  uint32_t i = fiv/reps-1.0; // has -1 since this is dropped right into a timer register
  cli(); *nextRate=i; *nextRep=reps; sei();
}

// Must work from within the motor ISR timers, in tick units
#define QuickSetIntervalAxis1(r) (PIT_LDVAL1=r)
#define QuickSetIntervalAxis2(r) (PIT_LDVAL2=r)

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
