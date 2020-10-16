// Platform setup ------------------------------------------------------------------------------------
// 2020 - Gildo Bosi
// ----------------- 

// This is for fast processors with hardware FP
#define HAL_FAST_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 14

// Width of step pulse
#define HAL_PULSE_WIDTH 1000

//    Arduino DUE serial def
//Serial  :  0 (RX) and  1 (TX)
//Serial 1: 19 (RX) and 18 (TX)
//Serial 2: 17 (RX) and 16 (TX)
//Serial 3: 15 (RX) and 14 (TX)

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
#define SerialA Serial
#define SerialB Serial1
#define SerialC Serial3

// SerialA is always enabled, SerialB and SerialC are optional
#define HAL_SERIAL_B_ENABLED
#define HAL_SERIAL_C_ENABLED

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#if defined(NV_MB85RC256V)
  #include "../drivers/NV_I2C_FRAM_MB85RC256V.h"
#else
  // defaults to 0x57 and 4KB
  #include "../drivers/NV_I2C_EEPROM_24XX_C.h"
#endif

#include <avr/dtostrf.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
void HAL_Initialize(void)
{
}

float HAL_MCU_Temperature(void)
{
  return -999.0; // disable value
}

//--------------------------------------------------------------------------------------------------
// Initialize timers

// frequency compensation (F_COMP/1000000.0) for adjusting microseconds to timer counts
#define F_COMP 1000000.0

void TIMER1_COMPA_vect(void);
void TIMER3_COMPA_vect(void);
void TIMER4_COMPA_vect(void);

extern long int siderealInterval;
extern void SiderealClockSetInterval(long int);

// Paramters table:                       (Due Pins)   onStep Timer
// TC0, 0, TC0_IRQn  =>  TC0_Handler()    2, 13
// TC0, 1, TC1_IRQn  =>  TC1_Handler()    60, 61
// TC0, 2, TC2_IRQn  =>  TC2_Handler()    58
// TC1, 0, TC3_IRQn  =>  TC3_Handler()    none         TIMER3
// TC1, 1, TC4_IRQn  =>  TC4_Handler()    none         TIMER4
// TC1, 2, TC5_IRQn  =>  TC5_Handler()    none         TIMER1
// TC2, 0, TC6_IRQn  =>  TC6_Handler()    4, 5         Pin 5 dimmer led polar finder! don't use timer
// TC2, 1, TC7_IRQn  =>  TC7_Handler()    3, 10
// TC2, 2, TC8_IRQn  =>  TC8_Handler()    11, 12       tone beep

//TC1 ch 0
void TC3_Handler()
{
  TC_GetStatus(TC1, 0);
  TIMER3_COMPA_vect();
}

//TC1 ch 1
void TC4_Handler()
{
  TC_GetStatus(TC1, 1);
  TIMER4_COMPA_vect();
}

//TC1 ch 2
void TC5_Handler()
{
  TC_GetStatus(TC1, 2);
  TIMER1_COMPA_vect();
}

// These are the clock frequencies available to the timers /2,/8,/32,/128
// TIMER_CLOCK1: 84Mhz/2 = 42.000 MHz
// TIMER_CLOCK2: 84Mhz/8 = 10.500 MHz
// TIMER_CLOCK3: 84Mhz/32 = 2.625 MHz
// TIMER_CLOCK4: 84Mhz/128 = 656.250 KHz
// TIMER_CLOCK5: SLCK ( slow clock )

#define TIMER_CLOCK1_DIV 2
#define TIMER_CLOCK2_DIV 8
#define TIMER_CLOCK3_DIV 32
#define TIMER_CLOCK4_DIV 128

// Start timer. Parameters are:
// tc       : timer counter. Can be TC0, TC1 or TC2
// channel  : Can be 0, 1 or 2
// irq      : irq number. See table.
// frequency:frequency (in Hz)
//
// example: startTimer(TC1, 0, TC3_IRQn, 40);

static void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency)
{
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
  uint32_t rc = VARIANT_MCK / TIMER_CLOCK2_DIV / frequency;
  TC_SetRA(tc, channel, 0);        
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

static void setPeriod(Tc *tc, uint32_t channel, unsigned long microseconds)
{
  uint32_t rc = (VARIANT_MCK / TIMER_CLOCK2_DIV) / (1000000.0 / microseconds);
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
}

// Init sidereal clock timer
void HAL_Init_Timer_Sidereal()
{
  startTimer(TC1, 2, TC5_IRQn, 1000);
  SiderealClockSetInterval(siderealInterval);
}

// Init Axis1 and Axis2 motor timers and set their priorities
void HAL_Init_Timers_Motor()
{
  startTimer(TC1, 0, TC3_IRQn, 1000);
  startTimer(TC1, 1, TC4_IRQn, 1000);
  setPeriod(TC1, 0, 150); // give 0.15ms before the timers fire (and update to the correct rate)
  setPeriod(TC1, 1, 150);
}

//--------------------------------------------------------------------------------------------------
// Set timer1 to interval (in microseconds*16), for the 1/100 second sidereal timer

static void Timer1SetInterval(long iv, double PPSrateRatio)
{
  iv = round(((double)iv) / PPSrateRatio);
  //setPeriod(TC1, 2, (float)iv * 0.0625);
  setPeriod(TC1, 2, iv >> 4); // div 16
}

//--------------------------------------------------------------------------------------------------
// Quickly reprogram the interval (in microseconds*(F_COMP/1000000.0)) for the motor timers, must work from within the motor ISR timers

// prepare to set Axis1/2 hw timers to interval (in microseconds*16), maximum time is about 134 seconds
void PresetTimerInterval(long iv, float TPSM, volatile uint32_t *nextRate, volatile uint16_t *nextRep)
{
  // 0.262 * 512 = 134.21s
  uint32_t i = iv;
  uint16_t t = 1;
  while (iv > 65536L * 64L)
  {
    t++;
    iv = i / t;
    if (t == 512)
    {
      iv = 65535L * 64L;
      break;
    }
  }
  cli();
  *nextRate = ((F_COMP / 1000000.0) * (iv * 0.0625) * TPSM - 1.0);
  *nextRep = t;
  sei();
}

void QuickSetIntervalAxis1(uint32_t r)
{
  setPeriod(TC1, 0, r);
}

void QuickSetIntervalAxis2(uint32_t r)
{
  setPeriod(TC1, 1, r);
}

// --------------------------------------------------------------------------------------------------
// Fast port writing help

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

#define a2CS_L digitalWrite(Axis2_M2,LOW)
#define a2CS_H digitalWrite(Axis2_M2,HIGH)
#define a2CLK_L digitalWrite(Axis2_M1,LOW)
#define a2CLK_H digitalWrite(Axis2_M1,HIGH)
#define a2SDO_H digitalWrite(Axis2_M0,HIGH)
#define a2SDO_L digitalWrite(Axis2_M0,LOW)
#define a2M0(P) digitalWrite(Axis2_M0,(P))
#define a2M1(P) digitalWrite(Axis2_M1,(P))
#define a2M2(P) digitalWrite(Axis2_M2,(P))


#ifdef BUZZER

volatile static int32_t toggles;     // number of ups/downs in a tone burst
volatile static int outTonePin = 4;  

// duration in ms
void tone(int Pin, int toneFreq, int duration);
void noTone(void); // non rientrante!

void tone(int Pin, int toneFreq, int duration)
{
  toneFreq *= 2; // toggle require double freq
  outTonePin = Pin;
  toggles = toneFreq * duration / 1000;   // calculate no of waveform edges (rises/falls) for the tone burst
  startTimer(TC2, 2, TC8_IRQn, toneFreq); // Start Timer/Counter 2, channel 2, interrupt, frequency
}

void TC8_Handler(void) // timer ISR  TC2 ch 0
{
  TC_GetStatus(TC2, 2);
  if (toggles > 0)
  {
    digitalWrite(outTonePin, !digitalRead(outTonePin)); // invert the pin state (toggle)
    toggles--;
  }
  else
    noTone();
}

void noTone()
{
  TC_Stop(TC2, 2);               // stop timer
  digitalWrite(outTonePin, LOW); // no signal on pin
}
#endif
