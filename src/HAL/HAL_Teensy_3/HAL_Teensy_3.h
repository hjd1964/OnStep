#define __ARM_Teensy3__

#include <EEPROM.h>

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#define RefractionRateRange 10

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// Timers
IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

