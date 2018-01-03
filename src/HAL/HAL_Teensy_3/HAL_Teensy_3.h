#define __ARM_Teensy3__

#include <EEPROM.h>

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// Timers
IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Initialize the timer that handles the sidereal clock
void HAL_Init_Timer_Sidereal() {
  SiderealClockSetInterval(siderealInterval);
}

// Initialize Axis1 and Axis2 motor timers and set their priorities
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

// Set timer1 to interval (in microseconds*16), this is the 1/100 second sidereal timer

#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);
IntervalTimer itimer1;

void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);
  itimer1.begin(TIMER1_COMPA_vect, (float)iv * 0.0625);
}

void QuickSetIntervalAxis1(uint32_t r) {
  PIT_LDVAL1=r;
}

void QuickSetIntervalAxis2(uint32_t r) {
  PIT_LDVAL2=r;
}

// --------------------------------------------------------------------------------------------------
// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define StepPinAxis1_HIGH digitalWriteFast(Axis1StepPin, HIGH)
#define StepPinAxis1_LOW digitalWriteFast(Axis1StepPin, LOW)
#define DirPinAxis1_HIGH digitalWriteFast(Axis1DirPin, HIGH)
#define DirPinAxis1_LOW digitalWriteFast(Axis1DirPin, LOW)

#define StepPinAxis2_HIGH digitalWriteFast(Axis2StepPin, HIGH)
#define StepPinAxis2_LOW digitalWriteFast(Axis2StepPin, LOW)
#define DirPinAxis2_HIGH digitalWriteFast(Axis2DirPin, HIGH)
#define DirPinAxis2_LOW digitalWriteFast(Axis2DirPin, LOW)
