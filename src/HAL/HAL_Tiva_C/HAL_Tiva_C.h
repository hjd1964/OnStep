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

// The Energia IDE only has EEPROM.read and EEPROM.write, it does not include EEPROM.update.
// My patch has been accepted but it will take a while until the next version is released.
// Until then you can use the included EEPROM_LP.ino and EEPROM_LP.h.
#include "EEPROM_LP.h"

// Different values for EEPROM end
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define E2END 2047
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define E2END 6143
#endif

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#define RefractionRateRange 10

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// Fast port writing help
#define CLR(x,y) (GPIOPinWrite(x,y,0))
#define SET(x,y) (GPIOPinWrite(x,y,y))
#define TGL(x,y) (GPIOPinRead(x,y)==0?GPIOPinWrite(x,y,y):GPIOPinWrite(x,y,0)) // untested, not used in current version

// Timers
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

#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  // Due to a bug we set the frequency manually on initialization
  uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), F_BUS);
#endif
