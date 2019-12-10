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
// General purpose initialize for HAL
void HAL_Init(void) {
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
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM_ESP.h"
#endif

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 16000000.0

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
// Re-program the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers

// Prepare to set Axis1/2 hw timers to interval (in microseconds*16), maximum time is about 134 seconds
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep) {
  // 0.262 * 512 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*64L) { t++; iv=i/t; if (t==512) { iv=65535L*64L; break; } }
  cli(); *nextRate=((F_COMP/1000000.0) * (iv*0.0625) * TPSM - 1.0); *nextRep=t; sei();
}

// Must work from within the motor ISR timers
IRAM_ATTR void QuickSetIntervalAxis1(uint32_t r) {
  timerAlarmWrite(itimer3, r, true);
}
IRAM_ATTR void QuickSetIntervalAxis2(uint32_t r) {
  timerAlarmWrite(itimer4, r, true);
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
