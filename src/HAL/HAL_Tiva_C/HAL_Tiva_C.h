// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more TM4C boards based on different lines are supported
#define __ARM_TI_TM4C__

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
// no pre-scaling of timers on Tiva Launchpads
#define F_BUS SysCtlClockGet()
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// There is a bug with SysCtlClockGet and TM4C1294 MCUs. At the moment we just hardcode the
// value and set the cpu frequency manually at the start of setup()
#define F_BUS 120000000
#endif

// Launchpad (TIVA) libraries
#include "Energia.h"
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include "inc/hw_ints.h"

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_AT24C32)
  #include "../drivers/NV_I2C_EEPROM_AT24C32.h"
#elif defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  #include "../drivers/NV_EEPROM_TIVA.h"
  #if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
  #define E2END 2047
  #elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  #define E2END 6143
  #endif
#endif

// Use an RTC (Real Time Clock) if present -----------------------------------------------------------
#include "../drivers/RTCw.h"

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()
#define HAL_TIMER1_PREFIX TimerIntClear( Timer1_base, TIMER_TIMA_TIMEOUT )
#define HAL_TIMER3_PREFIX TimerIntClear( Timer3_base, TIMER_TIMA_TIMEOUT )
#define HAL_TIMER4_PREFIX TimerIntClear( Timer4_base, TIMER_TIMA_TIMEOUT )

// Lower limit (fastest) step rate in uS for this platform
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  #define MaxRate_LowerLimit 8
#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
  #define MaxRate_LowerLimit 16
#else
  #define MaxRate_LowerLimit 16
#endif

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define SerialA Serial
#define SerialB Serial1
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define SerialA Serial1
#define SerialB Serial7
#endif
// SerialA is always enabled, SerialB and SerialC are optional
#define HAL_SERIAL_B_ENABLED

// New symbol for the default I2C port -------------------------------------------------------------
#define HAL_Wire Wire

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
#define F_COMP F_BUS

#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  // Due to a bug we set the frequency manually on initialization
  uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), F_BUS);
#endif

// The Energia IDE does not have IntervalTimer so we have to initialise timers manually

// We initialize these here and not in timer.ino
void TIMER1_COMPA_vect(void);

void TIMER3_COMPA_vect(void);

void TIMER4_COMPA_vect(void);

#define Sysctl_Periph_Timer1 SYSCTL_PERIPH_TIMER1
#define Timer1_base TIMER1_BASE
#define Int_timer1 INT_TIMER1A

#define Sysctl_Periph_Timer3 SYSCTL_PERIPH_TIMER2
#define Timer3_base TIMER2_BASE
#define Int_timer3 INT_TIMER2A

#define Sysctl_Periph_Timer4 SYSCTL_PERIPH_TIMER3
#define Timer4_base TIMER3_BASE
#define Int_timer4 INT_TIMER3A

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal() {
  // need to initialise timers before using SetSiderealClockRate
  // all timers are 32 bits
  // timer 1A is used instead of itimer1
  // timer 2A is used instead of itimer3
  // timer 3A is used instead of itimer4

  // Enable Timer 1 Clock
  SysCtlPeripheralEnable(Sysctl_Periph_Timer1);

  // Configure Timer Operation as Periodic
  TimerConfigure(Timer1_base, TIMER_CFG_PERIODIC);

  // register interrupt without editing the startup Energia file
  //IntRegister( INT_TIMER1A, TIMER1_COMPA_vect );
  TimerIntRegister(Timer1_base, TIMER_A, TIMER1_COMPA_vect );

  // Enable Timer 1A interrupt
  IntEnable(Int_timer1);

  // Timer 1A generate interrupt when Timeout occurs
  TimerIntEnable(Timer1_base, TIMER_TIMA_TIMEOUT);

  // Configure Timer Frequency - initialize the timers that handle the sidereal clock, RA, and Dec
  SiderealClockSetInterval(siderealInterval);

  // Start Timer 1A
  TimerEnable(Timer1_base, TIMER_A);
}

// Initialize Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // Enable Timer 2 and 3 Clocks
  SysCtlPeripheralEnable(Sysctl_Periph_Timer3);
  SysCtlPeripheralEnable(Sysctl_Periph_Timer4);

  // Configure Timer Operation as Periodic
  TimerConfigure(Timer3_base, TIMER_CFG_PERIODIC);
  TimerConfigure(Timer4_base, TIMER_CFG_PERIODIC);

  // register interrupts without editing the startup Energia file
  //IntRegister( INT_TIMER2A, TIMER3_COMPA_vect );
  //IntRegister( INT_TIMER3A, TIMER4_COMPA_vect );
  TimerIntRegister(Timer3_base, TIMER_A, TIMER3_COMPA_vect );
  TimerIntRegister(Timer4_base, TIMER_A, TIMER4_COMPA_vect );

  // Enable Timer 2A and 3A interrupts
  IntEnable(Int_timer3);
  IntEnable(Int_timer4);

  // Timer 2A and 3A generate interrupt when Timeout occurs
  TimerIntEnable(Timer3_base, TIMER_TIMA_TIMEOUT);
  TimerIntEnable(Timer4_base, TIMER_TIMA_TIMEOUT);

  // *

  TimerLoadSet(Timer3_base, TIMER_A, (int) (F_COMP / 1000000 * 128 * 0.0625));
  TimerLoadSet(Timer4_base, TIMER_A, (int) (F_COMP / 1000000 * 128 * 0.0625));

  // Start Timer 2A and 3A
  TimerEnable(Timer3_base, TIMER_A);
  TimerEnable(Timer4_base, TIMER_A);

  IntPrioritySet(Int_timer1, 1);
  IntPrioritySet(Int_timer3, 0);
  IntPrioritySet(Int_timer4, 0);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  TimerLoadSet(Timer1_base, TIMER_A, (int)(F_COMP/1000000 * iv * 0.0625));
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

void QuickSetIntervalAxis1(uint32_t r) {
  TimerLoadSet(Timer3_base, TIMER_A, r);
}

void QuickSetIntervalAxis2(uint32_t r) {
  TimerLoadSet(Timer4_base, TIMER_A, r);
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

#define CLR(x,y) (GPIOPinWrite(x,y,0))
#define SET(x,y) (GPIOPinWrite(x,y,y))
#define TGL(x,y) (GPIOPinRead(x,y)==0?GPIOPinWrite(x,y,y):GPIOPinWrite(x,y,0)) // untested, not used in current version

// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define StepPinAxis1_HIGH SET(Axis1StepPORT, Axis1StepBit)
#define StepPinAxis1_LOW CLR(Axis1StepPORT, Axis1StepBit)
#define DirPinAxis1_HIGH SET(Axis1DirPORT, Axis1DirBit)
#define DirPinAxis1_LOW CLR(Axis1DirPORT, Axis1DirBit)

#define StepPinAxis2_HIGH SET(Axis2StepPORT, Axis2StepBit)
#define StepPinAxis2_LOW CLR(Axis2StepPORT, Axis2StepBit)
#define DirPinAxis2_HIGH SET(Axis2DirPORT, Axis2DirBit)
#define DirPinAxis2_LOW CLR(Axis2DirPORT, Axis2DirBit)
