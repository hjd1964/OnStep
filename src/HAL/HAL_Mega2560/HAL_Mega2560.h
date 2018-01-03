#include <EEPROM.h>

// This platform doesn't support true double precision math
#define HAL_NO_DOUBLE_PRECISION

// Slower MCUs can't do a wide range of degrees for misalignment
#define HAL_ALIGN_MODEL_SLOW

// Enable a pseudo low priority level for Timer1 (sidereal clock) so the
// critical motor ISR's don't get delayed by the big slow sidereal clock ISR
#define HAL_USE_NOBLOCK_FOR_TIMER1

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

extern long int siderealInterval;
extern void SetSiderealClockRate (long int);

// Initialize the timer that handles the sidereal clock
void HAL_Init_Timer_Sidereal() {
  SetSiderealClockRate(siderealInterval);
}

// Initialize Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor() {
  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3B = (1 << WGM12) | (1 << CS11);
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4B = (1 << WGM12) | (1 << CS11);
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
}
