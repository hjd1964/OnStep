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

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#define RefractionRateRange 10

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

