// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// We derive the F_BUS variable from the actual CPU frequency of the selected board.
#define F_BUS F_CPU

#include <HardwareTimer.h>

// Get this library from https://github.com/watterott/Arduino-Libs/archive/master.zip
#include <digitalWriteFast.h>

#include <EEPROM.h>

// This is defined for Arduino, but not for other platforms. We use a conservative value.
#define E2END 2047

// For align model, MCUs with lower resources use _LOW
#define HAL_ALIGN_MODEL_HIGH

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// Timers

// it gets initialised here and not in timer.ino
void TIMER1_COMPA_vect(void);

HardwareTimer itimer3(3);
void TIMER3_COMPA_vect(void);

HardwareTimer itimer4(4);
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SetSiderealClockRate (long int);

void HAL_Init_Timer_Sidereal() {
  // initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);
}

void HAL_Init_Timers_Extra() {
  // Pause the timer while we're configuring it
  itimer3.pause();

  //itimer3.setPrescaleFactor(SMT32_PRESCALER);
  //itimer3.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer3.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt on channel 3
  itimer3.setChannel3Mode(TIMER_OUTPUT_COMPARE);
  itimer3.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  itimer3.attachInterrupt(TIMER_CH3, TIMER3_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer3.refresh();

  // Start the timer counting
  itimer3.resume();

  
  // Pause the timer while we're configuring it
  itimer4.pause();

  //itimer4.setPrescaleFactor(SMT32_PRESCALER);
  //itimer4.setOverflow(STM32_OVERFLOW);
  // Set up period
  itimer4.setPeriod((float)128 * 0.0625); // in microseconds

  // Set up an interrupt on channel 4
  itimer4.setChannel4Mode(TIMER_OUTPUT_COMPARE);
  itimer4.setCompare(TIMER_CH4, 1);  // Interrupt 1 count after each update
  itimer4.attachInterrupt(TIMER_CH4, TIMER4_COMPA_vect);

  // Refresh the timer's count, prescale, and overflow
  itimer4.refresh();

  // Start the timer counting
  itimer4.resume();

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  nvic_irq_set_priority(NVIC_TIMER1_CC, 2);
  // set the motor timers to run at the highest priority
  nvic_irq_set_priority(NVIC_TIMER3, 0);
  nvic_irq_set_priority(NVIC_TIMER4, 0);
}
