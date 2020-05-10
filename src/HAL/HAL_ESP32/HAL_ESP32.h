// Platform setup ------------------------------------------------------------------------------------

// This is for fast processors with hardware FP
#define HAL_FAST_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 16

// Width of step pulse
#define HAL_PULSE_WIDTH 2500

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
// SerialA is always enabled, SerialB and SerialC are optional

// SerialB
#define SerialB Serial2
#define HAL_SERIAL_B_ENABLED

// SerialC
#if SERIAL_C_BAUD_DEFAULT != OFF
  #if SERIAL_C_BAUD_DEFAULT > 0
    #error "Bluetooth on ESP32 SERIAL_C_BAUD_DEFAULT gets set to ON or OFF only."
  #endif
  #include <BluetoothSerial.h>
  BluetoothSerial SerialC;
  #define HAL_SERIAL_C_ENABLED
  #define HAL_SERIAL_C_BLUETOOTH
  #undef SERIAL_C_BAUD_DEFAULT
  #define SERIAL_C_BAUD_DEFAULT SERIAL_C_BLUETOOTH_NAME
#endif

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire

//--------------------------------------------------------------------------------------------------
// Nanoseconds delay function
unsigned int _nanosPerPass=1;
IRAM_ATTR void delayNanoseconds(unsigned int n) {
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
}

#include "Analog.h"

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)

// Correction for ESP32's internal temperture sensor
#define INTERNAL_TEMP_CORRECTION 0

float HAL_MCU_Temperature(void) {
  return temperatureRead() + INTERNAL_TEMP_CORRECTION;
}

//--------------------------------------------------------------------------------------------------
// Interrupts

#define ISR(f) void f (void)
hw_timer_t * itimer1 = NULL;
void TIMER1_COMPA_vect(void);

hw_timer_t * itimer3 = NULL;
void TIMER3_COMPA_vect(void);

hw_timer_t * itimer4 = NULL;
void TIMER4_COMPA_vect(void);

portMUX_TYPE siderealTimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE motorTimerMux = portMUX_INITIALIZER_UNLOCKED;

#define HAL_TIMER1_PREFIX portENTER_CRITICAL_ISR( &siderealTimerMux )
#define HAL_TIMER3_PREFIX portENTER_CRITICAL_ISR( &motorTimerMux )
#define HAL_TIMER4_PREFIX portENTER_CRITICAL_ISR( &motorTimerMux )
#define HAL_TIMER1_SUFFIX portEXIT_CRITICAL_ISR( &siderealTimerMux )
#define HAL_TIMER3_SUFFIX portEXIT_CRITICAL_ISR( &motorTimerMux )
#define HAL_TIMER4_SUFFIX portEXIT_CRITICAL_ISR( &motorTimerMux )

// Override cli/sei and use for muxes
#undef cli
IRAM_ATTR void cli() { portENTER_CRITICAL(&motorTimerMux); portENTER_CRITICAL(&siderealTimerMux); }
#undef sei
IRAM_ATTR void sei() { portEXIT_CRITICAL(&siderealTimerMux); portEXIT_CRITICAL(&motorTimerMux); }
void timerAlarmsEnable() { timerAlarmEnable(itimer1); timerAlarmEnable(itimer3); timerAlarmEnable(itimer4); }
void timerAlarmsDisable() { timerAlarmDisable(itimer1); timerAlarmDisable(itimer3); timerAlarmDisable(itimer4); }

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32_C.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM_ESP.h"
#endif

//--------------------------------------------------------------------------------------------------
// Initialize timers

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // set the system timer for millis() to the second highest priority
  itimer3 = timerBegin(2, 5, true);  // the timer frequency of an ESP32 is 80MHz.  80/5 = 16 MHz (which is a timer count in OnStep due to it's Mega2560 heritage.)
  timerAttachInterrupt(itimer3, &TIMER3_COMPA_vect, true);
  timerAlarmWrite(itimer3, 1000*16, true);
  timerAlarmEnable(itimer3);
  
  itimer4 = timerBegin(3, 5, true);
  timerAttachInterrupt(itimer4, &TIMER4_COMPA_vect, true);
  timerAlarmWrite(itimer4, 1000*16, true);
  timerAlarmEnable(itimer4);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  itimer1 = timerBegin(1, 5, true);
  timerAttachInterrupt(itimer1, &TIMER1_COMPA_vect, true);
  timerAlarmWrite(itimer1, 1000*16, true);
  timerAlarmEnable(itimer1);

  SiderealClockSetInterval(siderealInterval);
}

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  timerAlarmWrite(itimer1, iv, true);
}

//--------------------------------------------------------------------------------------------------
// Re-program interval for the motor timers

#define TIMER_RATE_MHZ 16L          // ESP32 motor timers run at 16MHz so use full resolution
#define TIMER_RATE_16MHZ_TICKS 1L   // 16L/TIMER_RATE_MHZ, for the default 16MHz

// prepare to set Axis1/2 hw timers to interval (in 1/16 microsecond units)
void PresetTimerInterval(long iv, bool TPS, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // maximum time is about 134 seconds
  if (iv>2144000000) iv=2144000000;

  // minimum time is 1 micro-second
  if (iv<16) iv=16;

  // TPS (timer pulse step) == false for SQW mode and double the timer rate
  if (!TPS) iv/=2L;

  iv/=TIMER_RATE_16MHZ_TICKS;
  uint32_t reps = (iv/4194304L)+1;
  uint32_t i = iv/reps;
  cli(); *nextRate=i; *nextRep=reps; sei();
}

// Must work from within the motor ISR timers, in tick units
IRAM_ATTR void QuickSetIntervalAxis1(uint32_t r) {
  timerAlarmWrite(itimer3, r, true);
}
IRAM_ATTR void QuickSetIntervalAxis2(uint32_t r) {
  timerAlarmWrite(itimer4, r, true);
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

#define a2CS_H digitalWrite(Axis2_M2,HIGH)
#define a2CS_L digitalWrite(Axis2_M2,LOW)
#define a2CLK_H digitalWrite(Axis2_M1,HIGH)
#define a2CLK_L digitalWrite(Axis2_M1,LOW)
#define a2SDO_H digitalWrite(Axis2_M0,HIGH)
#define a2SDO_L digitalWrite(Axis2_M0,LOW)
#define a2M0(P) digitalWrite(Axis2_M0,(P))
#define a2M1(P) digitalWrite(Axis2_M1,(P))
#define a2M2(P) digitalWrite(Axis2_M2,(P))
