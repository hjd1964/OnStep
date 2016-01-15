/*
 * Title       On-Step
 * by          Howard Dutton
 *
 * Copyright (C) 2012 to 2016 Howard Dutton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 *
 * Revision History, see GitHub
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * Arduino Stepper motor controller for Losmandy G11 mounts (and others)
 * with LX200 derived command set.
 *
 */

#include "math.h"
#include "errno.h"
// Use Config.h to configure OnStep to your requirements 
#include "Config.h"
#include "Library.h"
#include "FPoint.h"
#include "TM4C.h"
// There is a bug in Arduino/Energia which ignores #ifdef preprocessor directives when generating a list of files
// Until this is fixed YOU MUST MANUALLY UN-COMMENT the #include line below if using the Launchpad Connected device
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__) || defined(W5100_ON)
//#include "Ethernet.h"
#endif

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define F_BUS SysCtlClockGet() // no pre-scaling of timers on Tiva Launchpads
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// There is a bug with SysCtlClockGet and TM4C1294 MCUs. At the moment we just hardcode the value and set the cpu frequency manually at the start of setup()
#define F_BUS 120000000 
#endif

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "01 14 16"
#define FirmwareNumber "1.0b27"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

// forces initialialization of a host of settings in EEPROM. OnStep does this automatically, most likely, you will want to leave this alone
#define INIT_KEY false    // set to true to keep automatic initilization from happening.  This is a one-time operation... upload to the Arduino, then set to false and upload again
#define initKey 915307548 // unique identifier for the current initialization format, do not change

// Time keeping ------------------------------------------------------------------------------------------------------------
long siderealTimer    = 0;           // counter to issue steps during tracking
long PecSiderealTimer = 0;           // time since worm wheel zero index for PEC
long guideSiderealTimer=0;           // counter to issue steps during guiding
unsigned long clockTimer;            // wall time base, one second counter

double UT1       = 0.0;              // the current universal time
double UT1_start = 0.0;              // the start of UT1
unsigned long UT1mS_start = 0;       // mS at the start of UT1
double JD  = 0.0;                    // and date, used for computing LST
double LMT = 0.0;                    // internally, the date and time is kept in JD/LMT and LMT is updated to keep current time
double timeZone = 0.0;               //

double LST = 0.0;                    // this is the local (apparent) sidereal time in 24 hour format (0 to <24) must be updated when accessed
long lst_start = 0;                  // the start of lst
unsigned long lst_mS_start = 0;      // mS at the start of lst
volatile long lst = 0;               // this is the local (apparent) sidereal time in 1/100 seconds (23h 56m 4.1s per day = 86400 clock seconds/
                                     // 86164.09 sidereal seconds = 1.00273 clock seconds per sidereal second)

long siderealInterval       = 15956313L;
long masterSiderealInterval = siderealInterval;
                                     // default = 15956313 ticks per sidereal hundredth second, where a tick is 1/16 uS
                                     // this is stored in EEPROM which is updated/adjusted with the ":T+#" and ":T-#" commands
                                     // a higher number here means a longer count which slows down the sidereal clock

double HzCf = 16000000.0/60.0;       // conversion factor to go to/from Hz for sidereal interval

volatile long SiderealRate;          // based on the siderealInterval, this is the time between steps for sidereal tracking
volatile long TakeupRate;            // this is the takeup rate for synchronizing the target and actual positions when needed

// PPS (GPS)
volatile unsigned long PPSlastMicroS = 1000000UL;
volatile unsigned long PPSavgMicroS = 1000000UL;
volatile double PPSrateRatio = 1.0;
volatile double LastPPSrateRatio = 1.0;
volatile boolean PPSsynced = false;

// Tracking and rate control
#ifdef MOUNT_TYPE_ALTAZM
#define refraction_enable false      // refraction isn't allowed in Alt/Azm mode
#else
#define refraction_enable true       // refraction allowed
#endif

#ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
boolean refraction = refraction_enable;
#else
boolean refraction = false;
#endif

long    maxRate = MaxRate*16L;
volatile long    timerRateHA = 0;
volatile long    timerRateBacklashHA = 0;
volatile boolean inBacklashHA=false;
volatile long    timerRateDec = 0;
volatile long    timerRateBacklashDec = 0;
volatile boolean inBacklashDec=false;

#ifdef MOUNT_TYPE_ALTAZM
#define default_tracking_rate 0
#else
#define default_tracking_rate 1
#endif
volatile double  trackingTimerRateHA = default_tracking_rate;
volatile double  trackingTimerRateDec = default_tracking_rate;
volatile double  pecTimerRateHA = 0.0;
volatile double  guideTimerRateHA = 0.0;
volatile double  guideTimerRateDec = 0.0;
volatile double  timerRateRatio = ((double)StepsPerDegreeHA/(double)StepsPerDegreeDec);
volatile boolean useTimerRateRatio = (StepsPerDegreeHA!=StepsPerDegreeDec);
#define SecondsPerWormRotation  ((long)(StepsPerWormRotation/StepsPerSecond))
#define StepsPerSecondDec       ((double)(StepsPerDegreeDec/3600.0)*15.0)

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define cli() noInterrupts()
#define sei() interrupts()
#endif

#if defined(__arm__) && defined(TEENSYDUINO)
IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// Energia does not have IntervalTimer so we have to initialise timers manually

void TIMER1_COMPA_vect(void); // it gets initialised here and not in timer.ino

//Timer itimer3;
void TIMER3_COMPA_vect(void);

//Timer itimer4;
void TIMER4_COMPA_vect(void);
#endif

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define Sysctl_Periph_Timer1 SYSCTL_PERIPH_TIMER1
#define Timer1_base TIMER1_BASE
#define Int_timer1 INT_TIMER1A

#define Sysctl_Periph_Timer3 SYSCTL_PERIPH_TIMER2
#define Timer3_base TIMER2_BASE
#define Int_timer3 INT_TIMER2A

#define Sysctl_Periph_Timer4 SYSCTL_PERIPH_TIMER3
#define Timer4_base TIMER3_BASE
#define Int_timer4 INT_TIMER3A

#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define Sysctl_Periph_Timer1 SYSCTL_PERIPH_TIMER1
#define Timer1_base TIMER1_BASE
#define Int_timer1 INT_TIMER1A

#define Sysctl_Periph_Timer3 SYSCTL_PERIPH_TIMER2
#define Timer3_base TIMER2_BASE
#define Int_timer3 INT_TIMER2A

#define Sysctl_Periph_Timer4 SYSCTL_PERIPH_TIMER3
#define Timer4_base TIMER3_BASE
#define Int_timer4 INT_TIMER3A
#endif

// Location ----------------------------------------------------------------------------------------------------------------
double latitude  = 0.0;
double cosLat = 1.0;
double sinLat = 0.0;
double longitude = 0.0;

// fix underPoleLimit for fork mounts
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
#undef underPoleLimit
#define underPoleLimit 12
#endif

#ifdef MOUNT_TYPE_GEM
long celestialPoleHA  = 90L;
#endif
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
long celestialPoleHA  = 0L;
#endif
double celestialPoleDec = 90.0;

volatile long posHA      = 90L*StepsPerDegreeHA;   // hour angle position in steps
volatile long startHA    = 90L*StepsPerDegreeHA;   // hour angle of goto start position in steps
volatile fixed_t targetHA;                         // hour angle of goto end   position in steps
volatile byte dirHA      = 1;                      // stepping direction + or -
volatile long PEC_HA     = 0;                      // for PEC, adds or subtracts steps
double newTargetRA       = 0.0;                    // holds the RA for goTos
fixed_t origTargetHA;

volatile long posDec     = 90L*StepsPerDegreeDec; // declination position in steps
volatile long startDec   = 90L*StepsPerDegreeDec; // declination of goto start position in steps
volatile fixed_t targetDec;                       // declination of goto end   position in steps
volatile byte dirDec     = 1;                     // stepping direction + or -
double newTargetDec      = 0.0;                   // holds the Dec for goTos
long origTargetDec       = 0;

double newTargetAlt=0.0, newTargetAzm=0.0;        // holds the altitude and azmiuth for slews
double currentAlt = 45;                           // the current altitude
int    minAlt;                                    // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                    // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)

// Stepper/position/rate ----------------------------------------------------------------------------------------------------

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define CLR(x,y) (GPIOPinWrite(x,y,0))
#define SET(x,y) (GPIOPinWrite(x,y,y))
#define TGL(x,y) (GPIOPinRead(x,y)==0?GPIOPinWrite(x,y,y):GPIOPinWrite(x,y,0)) // untested, not used in current version
#else
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))
#endif

// I set the pin usage to facilitate easy connection of jumper cables
// for now, the #defines below are used to program the port modes using the standard Arduino library

// defines for direct port control
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   2560
#define LEDposPin  8       // Pin 8 (LED)   PH5
#define LEDnegPin  9       // Pin 9 (GND)   PH6

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  4       // Pin 4 (Dir)   PG5
#define Dec5vPin   5       // Pin 5 (5V?)   PE3
#define DecStepPin 6       // Pin 6 (Step)  PH3
#define DecGNDPin  7       // Pin 7 (GND)   PH4

#define LEDneg2Pin 10      // Pin 10 (GND)  PB4

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
// The Arduino attachInterrupt function works in two modes, on the '2560 it takes an Interrupt# on the Teensy and others it takes a Pin#
#define PpsInt     2       // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

#define HADirPin   11      // Pin 11 (Dir)  PB5
#define HA5vPin    12      // Pin 12 (5V?)  PB6
#define HAStepPin  13      // Pin 13 (Step) PB7
                           // Pin GND (GND)
                         
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      22      // Pin 22 (Microstep Mode 0)
#define HA_M1      23      // Pin 23 (Microstep Mode 1)
#define HA_M2      24      // Pin 24 (Microstep Mode 2)
#define HA_EN      25      // Pin 25 (Enabled when LOW)
#define HA_FAULT   26      // Pin 26 (Fault if LOW)

#define DE_M0      27      // Pin 27 (Microstep Mode 0)
#define DE_M1      28      // Pin 28 (Microstep Mode 1)
#define DE_M2      29      // Pin 29 (Microstep Mode 2)
#define DE_EN      30      // Pin 30 (Enabled when LOW)
#define DE_FAULT   31      // Pin 31 (Fault if LOW)

#define LEDposBit  5       // Pin 8
#define LEDposPORT PORTH   //
#define LEDnegBit  6       // Pin 9
#define LEDnegPORT PORTH   //
#define LEDneg2Bit  4      // Pin 10
#define LEDneg2PORT PORTB  //

#define DecDirBit  5       // Pin 4
#define DecDirPORT PORTG   //
#define Dec5vBit   3       // Pin 5
#define Dec5vPORT  PORTE   //
#define DecStepBit 3       // Pin 6
#define DecStepPORT PORTH  //
#define DecGNDBit  4       // Pin 7
#define DecGNDPORT PORTH   //

#define HADirBit   5       // Pin 11
#define HADirPORT  PORTB   //
#define HA5vBit    6       // Pin 12
#define HA5vPORT   PORTB   //
#define HAStepBit  7       // Pin 13
#define HAStepPORT PORTB   //

// ST4 interface
#ifdef ST4_ALTERNATE_PINS_ON
#define ST4RAw     47      // Pin 47 ST4 RA- West
#define ST4DEs     43      // Pin 43 ST4 DE- South
#define ST4DEn     45      // Pin 45 ST4 DE+ North
#define ST4RAe     49      // Pin 49 ST4 RA+ East
#else
#define ST4RAw     47      // Pin 47 ST4 RA- West
#define ST4DEs     49      // Pin 49 ST4 DE- South
#define ST4DEn     51      // Pin 51 ST4 DE+ North
#define ST4RAe     53      // Pin 53 ST4 RA+ East
#endif

#elif defined(__arm__) && defined(TEENSYDUINO)
// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   2560
#define LEDposPin  8       // Pin 8 (LED)   PH5
#define LEDnegPin  9       // Pin 9 (GND)   PH6

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  4       // Pin 4 (Dir)   PG5
#define Dec5vPin   5       // Pin 5 (5V?)   PE3
#define DecStepPin 6       // Pin 6 (Step)  PH3
#define DecGNDPin  7       // Pin 7 (GND)   PH4

#define LEDneg2Pin 7       // Pin 7 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin     23      // Pin 23 (PPS time source, GPS for example)

#define HADirPin   10      // Pin 10 (Dir)
#define HA5vPin    11      // Pin 11 (5V?)
#define HAStepPin  12      // Pin 12 (Step)
                           // Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      13      // Pin 13 (Microstep Mode 0)
#define HA_M1      14      // Pin 14 (Microstep Mode 1)
#define HA_M2      15      // Pin 15 (Microstep Mode 2)
#define HA_EN      16      // Pin 16 (Enabled when LOW)
#define HA_FAULT   17      // Pin 17 (Fault if LOW)

#define DE_M0      18      // Pin 18 (Microstep Mode 0)
#define DE_M1      19      // Pin 19 (Microstep Mode 1)
#define DE_M2      20      // Pin 20 (Microstep Mode 2)
#define DE_EN      21      // Pin 21 (Enabled when LOW)
#define DE_FAULT   22      // Pin 22 (Fault if LOW)

#define LEDposBit  0       // Pin 8
#define LEDposPORT PORTB   //
#define LEDnegBit  1       // Pin 9
#define LEDnegPORT PORTB   //
#define LEDneg2Bit 7       // Pin 7
#define LEDneg2PORT PORTD  //

#define DecDirBit  4       // Pin 4
#define DecDirPORT PORTD   //
#define Dec5vBit   5       // Pin 5
#define Dec5vPORT  PORTD   //
#define DecStepBit 6       // Pin 6
#define DecStepPORT PORTD  //
#define DecGNDBit  7       // Pin 7
#define DecGNDPORT PORTD   //

#define HADirBit   2       // Pin 10
#define HADirPORT  PORTB   //
#define HA5vBit    3       // Pin 11
#define HA5vPORT   PORTB   //
#define HAStepBit  4       // Pin 12
#define HAStepPORT PORTB   //

// ST4 interface
#define ST4RAw     24      // Pin 24 ST4 RA- West
#define ST4DEs     25      // Pin 25 ST4 DE- South
#define ST4DEn     26      // Pin 26 ST4 DE+ North
#define ST4RAe     27      // Pin 27 ST4 RA+ East

#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
// Note that TM4C123 has resistors R9 anr R10 between pins 14 (B6) and 23 (D0) and between pins 15 (B7) and 24 (D1)
// Make sure you look at the list of the pins and options to avoid clashes or desolder the two bridges
// These pins are used for DE_M2 (14) and DE_M3 (15) and for DecDirPin (23) and Dec5VPin (24)
// If you have defined DE_MODE_OFF in Config.h you should be safe to leave things as they are.

// Also note that we are using UART1 and UART 5 which use pins 3-6

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     11      // Pin A2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   12      // Pin A3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin  2       // Pin B5 (LED)
#define LEDnegPin  33      // Pin D6 (GND)


// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  23       // Pin D0 (Dir)
#define Dec5vPin   24       // Pin D1 (5V?)
#define DecStepPin 25       // Pin D2 (Step)
#define DecGNDPin  26       // Pin D3 (GND)

#define LEDneg2Pin 26       // Pin D3 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin     19      // Pin B2 (PPS time source, GPS for example)

#define HADirPin   27      // Pin E1 (Dir)
#define HA5vPin    28      // Pin E2 (5V?)
#define HAStepPin  29      // Pin E3 (Step)
// Pin GND (GND)
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      34      // Pin C7 (Microstep Mode 0)
#define HA_M1      35      // Pin C6 (Microstep Mode 1)
#define HA_M2      36      // Pin C5 (Microstep Mode 2)
#define HA_EN      37      // Pin C4 (Enabled when LOW)
#define HA_FAULT   38      // Pin B3 (Fault if LOW)

#define DE_M0      13      // Pin A4 (Microstep Mode 0)
#define DE_M1      14      // Pin B6 (Microstep Mode 1) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 14 is connected to pin 23
#define DE_M2      15      // Pin B7 (Microstep Mode 2) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 15 is connected to pin 24
#define DE_EN      18      // Pin E0 (Enabled when LOW)
#define DE_FAULT   17      // Pin F0 (Fault if LOW) NOTE, this is connected to pushbutton switch 2

#define LEDposBit  GPIO_PIN_5       // Pin 2
#define LEDposPORT GPIO_PORTB_BASE   //
#define LEDnegBit  GPIO_PIN_6       // Pin 33
#define LEDnegPORT GPIO_PORTD_BASE   //
#define LEDneg2Bit GPIO_PIN_3       // Pin 26
#define LEDneg2PORT GPIO_PORTD_BASE  //

#define DecDirBit  GPIO_PIN_0       // Pin 23=D0=B6 = pin 14
#define DecDirPORT GPIO_PORTD_BASE   //
#define Dec5vBit   GPIO_PIN_1       // Pin 24=D1=B7 = pin 15
#define Dec5vPORT  GPIO_PORTD_BASE   //
#define DecStepBit GPIO_PIN_2       // Pin 25
#define DecStepPORT GPIO_PORTD_BASE  //
#define DecGNDBit  GPIO_PIN_3       // Pin 26
#define DecGNDPORT GPIO_PORTD_BASE   //

#define HADirBit   GPIO_PIN_1       // Pin 27
#define HADirPORT  GPIO_PORTE_BASE   //
#define HA5vBit    GPIO_PIN_2       // Pin 28
#define HA5vPORT   GPIO_PORTE_BASE   //
#define HAStepBit  GPIO_PIN_3       // Pin 29
#define HAStepPORT GPIO_PORTE_BASE   //

// ST4 interface
#define ST4RAw     7      // Pin B4 ST4 RA- West
#define ST4DEs     8      // Pin A5 ST4 DE- South
#define ST4DEn     9      // Pin A6 ST4 DE+ North
#define ST4RAe     10     // Pin A7 ST4 RA+ East

#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// No need to desolder anything on this launchpad as pins we are using are not bridged

// Note that we are using UART7 and UART 5 which use pins 3-5 and pin 8 (C4,C5,C6,C7)

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     11      // Pin P2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   12      // Pin N3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin  2       // Pin E4 (LED)
#define LEDnegPin  33      // Pin L1 (GND)


// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  23       // Pin E0 (Dir)
#define Dec5vPin   24       // Pin E1 (5V?)
#define DecStepPin 25       // Pin E2 (Step)
#define DecGNDPin  26       // Pin E3 (GND)

#define LEDneg2Pin 26       // Pin E3 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin     19      // Pin M3 (PPS time source, GPS for example)

#define HADirPin   27      // Pin D7 (Dir)
#define HA5vPin    28      // Pin A6 (5V?)
#define HAStepPin  29      // Pin M4 (Step)
// Pin GND (GND)
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      34      // Pin L0 (Microstep Mode 0)
#define HA_M1      35      // Pin L5 (Microstep Mode 1)
#define HA_M2      36      // Pin L4 (Microstep Mode 2)
#define HA_EN      37      // Pin G0 (Enabled when LOW)
#define HA_FAULT   38      // Pin F3 (Fault if LOW)

#define DE_M0      13      // Pin N2 (Microstep Mode 0)
#define DE_M1      14      // Pin D0 (Microstep Mode 1)
#define DE_M2      15      // Pin D1 (Microstep Mode 2)
#define DE_EN      18      // Pin H2 (Enabled when LOW)
#define DE_FAULT   17      // Pin H3 (Fault if LOW) NOTE, this is connected to pushbutton switch 2

#define LEDposBit  GPIO_PIN_4       // Pin 2
#define LEDposPORT GPIO_PORTE_BASE   //
#define LEDnegBit  GPIO_PIN_1       // Pin 33
#define LEDnegPORT GPIO_PORTL_BASE   //
#define LEDneg2Bit GPIO_PIN_3       // Pin 26
#define LEDneg2PORT GPIO_PORTE_BASE  //

#define DecDirBit  GPIO_PIN_0       // Pin 23
#define DecDirPORT GPIO_PORTE_BASE   //
#define Dec5vBit   GPIO_PIN_1       // Pin 24
#define Dec5vPORT  GPIO_PORTE_BASE   //
#define DecStepBit GPIO_PIN_2       // Pin 25
#define DecStepPORT GPIO_PORTE_BASE  //
#define DecGNDBit  GPIO_PIN_3       // Pin 26
#define DecGNDPORT GPIO_PORTE_BASE   //

#define HADirBit   GPIO_PIN_7       // Pin 27
#define HADirPORT  GPIO_PORTD_BASE   //
#define HA5vBit    GPIO_PIN_6       // Pin 28
#define HA5vPORT   GPIO_PORTA_BASE   //
#define HAStepBit  GPIO_PIN_4       // Pin 29
#define HAStepPORT GPIO_PORTM_BASE   //

// ST4 interface
#define ST4RAw     7      // Pin D3 ST4 RA- West
#define ST4DEs     6      // Pin C7 ST4 DE- South
#define ST4DEn     9      // Pin B2 ST4 DE+ North
#define ST4RAe     10     // Pin B3 ST4 RA+ East

#endif


#if defined(HA_DISABLED_HIGH)
#define HA_Disabled HIGH
#define HA_Enabled LOW
#endif
#if defined(HA_DISABLED_LOW)
#define HA_Disabled LOW
#define HA_Enabled HIGH
#endif
#if defined(DE_DISABLED_HIGH)
#define DE_Disabled HIGH
#define DE_Enabled LOW
#endif
#if defined(DE_DISABLED_LOW)
#define DE_Disabled LOW
#define DE_Enabled HIGH
#endif

#define DecDirEInit      1
#define DecDirWInit      0
volatile byte DecDir     = DecDirEInit;
#define HADirNCPInit     0
#define HADirSCPInit     1
volatile byte HADir      = HADirNCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
boolean highPrecision    = true;

#define TrackingNone     0
#define TrackingSidereal 1
#define TrackingMoveTo   2
volatile byte trackingState     = TrackingNone;
byte abortTrackingState = TrackingNone;
volatile byte lastTrackingState = TrackingNone;
boolean abortSlew        = false;

#define MeridianFlipNever  0
#define MeridianFlipAlign  1
#define MeridianFlipAlways 2
#ifdef MOUNT_TYPE_GEM
byte meridianFlip = MeridianFlipAlways;
#endif
#ifdef MOUNT_TYPE_FORK
byte meridianFlip = MeridianFlipNever;
#endif
#ifdef MOUNT_TYPE_FORK_ALT
byte meridianFlip = MeridianFlipAlign;
#endif
#ifdef MOUNT_TYPE_ALTAZM
byte meridianFlip = MeridianFlipNever;
#endif

#define AlignNone        0
#define AlignOneStar1    1
#define AlignTwoStar1    11
#define AlignTwoStar2    12
#define AlignThreeStar1  21
#define AlignThreeStar2  22
#define AlignThreeStar3  23
byte alignMode           = AlignNone;

#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22
byte pierSide            = PierSideNone;

#define NotParked        0
#define Parking          1
#define Parked           2
#define ParkFailed       3
byte    parkStatus       = NotParked;
boolean parkSaved        = false;
boolean atHome           = false;
boolean homeMount        = false;

// Command processing -------------------------------------------------------------------------------------------------------
#define BAUD 9600

boolean commandError     = false;
boolean quietReply       = false;

char reply[50];

char command[3];
char parameter[25];
byte bufferPtr= 0;

char command_serial_zero[25];
char parameter_serial_zero[25];
byte bufferPtr_serial_zero= 0;

char Serial_recv_buffer[256] = "";
volatile byte Serial_recv_tail = 0;
volatile byte Serial_recv_head = 0;
char Serial_xmit_buffer[50] = "";
byte Serial_xmit_index = 0;

char Serial1_recv_buffer[256] = "";
volatile byte Serial1_recv_tail = 0;
volatile byte Serial1_recv_head = 0;
char Serial1_xmit_buffer[50] = "";
byte Serial1_xmit_index = 0;

// for bluetooth/serial 1
char command_serial_one[25];
char parameter_serial_one[25];
byte bufferPtr_serial_one= 0;

// for ethernet
char command_ethernet[25];
char parameter_ethernet[25];
byte bufferPtr_ethernet= 0;
int  www_xmit_buffer_send_pos=0;
int  www_xmit_buffer_pos=0;
char www_xmit_buffer[1024] = "";

// Misc ---------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

unsigned int baudRate[10] = {0,56700,38400,28800,19200,14400,9600,4800,2400,1200};

// current site index and name
byte currentSite = 0; 
char siteName[16];

// align command
double altCor            = 0;       // for geometric coordinate correction/align, - is below the pole, + above
double azmCor            = 0;       // - is right of the pole, + is left
double doCor             = 0;       // declination/optics orthogonal correction
double pdCor             = 0;       // declination/polar orthogonal correction
double IH                = 0;       // offset corrections/align
long   IHS               = 0;
double ID                = 0;

// tracking and PEC, fractional steps
fixed_t fstepHA;
fixed_t fstepDec;
fixed_t pstep;

// guide command
#define GuideRate1x        2
#define GuideRate16x       6
#define GuideRateNone      255

#define slewRate (1.0/((StepsPerDegreeHA*(MaxRate/1000000.0)))*3600.0)
#define halfSlewRate (slewRate/2.0)
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,halfSlewRate,slewRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x half-MaxRate MaxRate

byte currentGuideRate        = GuideRate16x;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirHA           = 0;
long          guideDurationHA      = -1;
unsigned long guideDurationLastHA  = 0;
byte          guideDirDec          = 0;
long          guideDurationDec     = -1;
unsigned long guideDurationLastDec = 0;

long lastTargetHA=0;
long debugv1 = 0;

double  guideTimerRate    = 0;
fixed_t amountGuideHA;
long    guideHA           = 0;
fixed_t amountGuideDec;
long    guideDec          = 0;

// Reticule control
#ifdef RETICULE_LED_PINS
int reticuleBrightness=RETICULE_LED_PINS;
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define reticulePin 44
#endif
#if defined(__arm__) && defined(TEENSYDUINO)
#define reticulePin 9
#endif
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define reticulePin 33
#endif
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define reticulePin 33
#endif
#endif

// PEC control
#define PECStatusString  "IpPrR"
#define IgnorePEC        0
#define ReadyPlayPEC     1
#define PlayPEC          2
#define ReadyRecordPEC   3
#define RecordPEC        4
long    accPecGuideHA    = 0;        // for PEC, buffers steps to be recorded
boolean PECfirstRecord   = false;
int     PECstatus        = IgnorePEC;
boolean PECrecorded      = false;
long    PECindex_record  = 0;
double  PECstartDelta    = 0;
long    lastWormRotationStepPos = -1;
long    wormRotationStepPos = 0;
long    PECindex         = 0;
long    PECindex1        = 0;
int     PECav            = 0;
long    lastPECindex     = -1;
long    PECtime_lastSense= 0;      // time since last PEC index was sensed
long    PECindex_sense   = 0;      // position of active PEC index sensed
long    next_PECindex_sense=-1;    // position of next PEC index sensed
int     PECautoRecord    = 0;      // for writing to PEC table to EEPROM
boolean PECindexDetected = false;  // indicates PEC index was found

// backlash control
volatile int backlashHA   = 0;
volatile int backlashDec  = 0;
volatile int blHA         = 0;
volatile int blDec        = 0;

// status state
boolean LED_ON = false;
boolean LED2_ON = false;

// ST4 interface
char ST4RA_state = 0;
char ST4RA_last = 0;
char ST4DE_state = 0;
char ST4DE_last = 0;

// EEPROM Info --------------------------------------------------------------------------------------------------------------
// 0-1023 bytes
// general purpose storage 0..99

#define EE_posHA       0
#define EE_posDec      4
#define EE_pierSide    8
#define EE_parkStatus  9
#define EE_parkSaved   10

#define EE_currentSite 11
#define EE_LMT         14
#define EE_JD          18

#define EE_pulseGuideRate 22
#define EE_maxRate     23

#define EE_minAlt      40
#define EE_maxAlt      41

#define EE_doCor       42
#define EE_pdCor       46
#define EE_altCor      50
#define EE_azmCor      54
#define EE_IH          58
#define EE_ID          62

#define EE_PECstatus   70
#define EE_PECrecorded 71
#define EE_PECrecord_index 72
#define EE_PECsense_index 76

#define EE_backlashHA  80
#define EE_backlashDec 84

#define EE_siderealInterval 88

#define EE_autoInitKey 96

// site index: 100-199
// 100..103 latitude  1  ((index 1-1)*25+100)
// 104..107 longitude 1
// 108      timeZone  1
// 109..124 site name 1

// 125..128 latitude  2  ((index 2-1)*25+100)
// 129..132 longitude 2
// 133      timeZone  2
// 134..149 site name 2

// 150..103 latitude  3  ((index 3-1)*25+100)
// 154..157 longitude 3
// 158      timeZone  3
// 159..174 site name 3

// 175..178 latitude  4  ((index 4-1)*25+100)
// 179..182 longitude 4
// 183      timeZone  4
// 184..199 site name 4

#define EE_sites    100

// PEC index: 200...1023
// PECBufferSize byte sized integers -128..+127, units are steps

#define EE_PECindex 200

// it takes 3.3ms to record a value to EEPROM, this can effect tracking performance since interrupts are disabled during the operation.
// so we store PEC data in RAM while recording.  When done, sidereal tracking is turned off and the data is written to EEPROM.
// writing the data can take up to 3 seconds.

byte PEC_buffer[PECBufferSize];

void setup() {
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  // due to a bug we set the frequency manually here
  uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), F_BUS);
#endif

// initialize some fixed-point values
  amountGuideHA.fixed=0;
  amountGuideDec.fixed=0;
  fstepHA.fixed=0;
  fstepDec.fixed=0;
  pstep.fixed=0;
  origTargetHA.fixed = 0;
  targetHA.part.m = 90L*StepsPerDegreeHA;
  targetHA.part.f = 0;
  targetDec.part.m = 90L*StepsPerDegreeDec;
  targetDec.part.f = 0;

  fstepHA.fixed=doubleToFixed(StepsPerSecond/100.0);
  
// initialize the stepper control pins RA and Dec
  pinMode(HAStepPin, OUTPUT);
  pinMode(HADirPin, OUTPUT); 
  pinMode(DecGNDPin, OUTPUT);
  CLR(DecGNDPORT, DecGNDBit);
  pinMode(DecStepPin, OUTPUT); 
  pinMode(DecDirPin, OUTPUT); 

// light status LED (provides both +5 and GND)
#ifdef STATUS_LED_PINS_ON
  pinMode(LEDnegPin, OUTPUT);
  CLR(LEDnegPORT, LEDnegBit);
  pinMode(LEDposPin, OUTPUT);
  SET(LEDposPORT, LEDposBit);
  LED_ON=true;
#endif
// light status LED (provides both +5 and pwm'd GND for polar reticule)
#ifdef STATUS_LED_PINS
  pinMode(LEDnegPin, OUTPUT);
  CLR(LEDnegPORT, LEDnegBit);
  pinMode(LEDposPin, OUTPUT);
  SET(LEDposPORT, LEDposBit);
  analogWrite(LEDnegPin,STATUS_LED_PINS);
  LED_ON=true;
#endif

// light reticule LED
#ifdef RETICULE_LED_PINS
// make sure the status LED isn't defined
#if defined(__arm__) && defined(TEENSYDUINO)
  #ifdef STATUS_LED_PINS_ON
    #undef STATUS_LED_PINS_ON
  #endif
  #ifdef STATUS_LED_PINS
    #undef STATUS_LED_PINS
  #endif
#endif
  pinMode(reticulePin, OUTPUT);
  analogWrite(reticulePin,reticuleBrightness);
#endif

// light second status LED (provides just GND)
#ifdef STATUS_LED2_PINS_ON
  pinMode(LEDneg2Pin, OUTPUT);
  CLR(LEDneg2PORT, LEDneg2Bit); LED2_ON=false;
#endif
// light second status LED (provides pwm'd GND for polar reticule)
#ifdef STATUS_LED2_PINS
  pinMode(LEDneg2Pin, OUTPUT);
  CLR(LEDneg2PORT, LEDneg2Bit);
  analogWrite(LEDneg2Pin,STATUS_LED2_PINS);
#endif

// provide 5V power to stepper drivers if requested
#ifdef POWER_SUPPLY_PINS_ON  
  pinMode(HA5vPin, OUTPUT);
  SET(HA5vPORT, HA5vBit);
  pinMode(Dec5vPin, OUTPUT);
  SET(Dec5vPORT, Dec5vBit);
#endif

// PEC index sense
#ifdef PEC_SENSE_ON
  pinMode(PecPin, INPUT);
#endif

// limit switch sense
#ifdef LIMIT_SENSE_ON  
  pinMode(LimitPin, INPUT_PULLUP);
#endif

// ST4 interface
#ifdef ST4_ON
  pinMode(ST4RAw, INPUT);
  pinMode(ST4RAe, INPUT);
  pinMode(ST4DEn, INPUT);
  pinMode(ST4DEs, INPUT);
#endif
#ifdef ST4_PULLUP
  pinMode(ST4RAw, INPUT_PULLUP);
  pinMode(ST4RAe, INPUT_PULLUP);
  pinMode(ST4DEn, INPUT_PULLUP);
  pinMode(ST4DEs, INPUT_PULLUP);
#endif

// disable the stepper drivers for now, if the enable lines are connected
  pinMode(HA_EN,OUTPUT); digitalWrite(HA_EN,HA_Disabled);
  pinMode(DE_EN,OUTPUT); digitalWrite(DE_EN,DE_Disabled);

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#ifdef HA_MODE
  pinMode(HA_M0, OUTPUT); digitalWrite(HA_M0,(HA_MODE & 1));
  pinMode(HA_M1, OUTPUT); digitalWrite(HA_M1,(HA_MODE>>1 & 1));
  pinMode(HA_M2, OUTPUT); digitalWrite(HA_M2,(HA_MODE>>2 & 1));
#endif
#ifdef DE_MODE
  pinMode(DE_M0, OUTPUT); digitalWrite(DE_M0,(DE_MODE & 1));
  pinMode(DE_M1, OUTPUT); digitalWrite(DE_M1,(DE_MODE>>1 & 1));
  pinMode(DE_M2, OUTPUT); digitalWrite(DE_M2,(DE_MODE>>2 & 1));
#endif

#ifdef PPS_SENSE_ON
#if defined(__AVR__)
  attachInterrupt(PpsInt,ClockSync,RISING);
#elif defined(__arm__) && defined(TEENSYDUINO)
  attachInterrupt(PpsPin,ClockSync,RISING);
#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  attachInterrupt(PpsPin,ClockSync,RISING);
#endif
#endif

  // EEPROM automatic initialization
  long int autoInitKey = initKey;
  long int thisAutoInitKey;
  if (INIT_KEY) EEPROM_writeQuad(EE_autoInitKey,(byte*)&autoInitKey);
  EEPROM_readQuad(EE_autoInitKey,(byte*)&thisAutoInitKey);
  if (autoInitKey!=thisAutoInitKey) {
    // init the site information, lat/long/tz/name
    EEPROM.write(EE_currentSite,0);
    latitude=0; longitude=0;
    for (int l=0; l<4; l++) {
      float f=latitude; EEPROM_writeQuad(EE_sites+(l)*25+0,(byte*)&f);
      f=longitude; EEPROM_writeQuad(EE_sites+(l)*25+4,(byte*)&f);
      EEPROM.write(EE_sites+(l)*25+8,128);
      EEPROM.write(EE_sites+(l)*25+9,0);
    }
  
    // init the date and time January 1, 2013. 0 hours LMT
    JD=2456293.5;
    LMT=0.0;
    float f=JD; EEPROM_writeQuad(EE_JD,(byte*)&f);
    f=LMT; EEPROM_writeQuad(EE_LMT,(byte*)&f);
  
    // init the min and max altitude
    minAlt=-10;
    maxAlt=85;
    EEPROM.write(EE_minAlt,minAlt+128);
    EEPROM.write(EE_maxAlt,maxAlt);
  
    // init (clear) the backlash amounts
    EEPROM_writeInt(EE_backlashDec,0);
    EEPROM_writeInt(EE_backlashHA,0);
  
    // init the PEC status, clear the index and buffer
    PECindex_record=0;
    EEPROM.write(EE_PECstatus,IgnorePEC);
    EEPROM.write(EE_PECrecorded,false);
    for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_PECindex+l,128);
    EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
    EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
    
    // init the Park status
    EEPROM.write(EE_parkSaved,false);
    EEPROM.write(EE_parkStatus,NotParked);
  
    // init the pulse-guide rate
    EEPROM.write(EE_pulseGuideRate,GuideRate1x);
    
    // init the default maxRate
    if (maxRate<2L*16L) maxRate=2L*16L; if (maxRate>10000L*16L) maxRate=10000L*16L;
    EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L));

    // init the sidereal tracking rate, use this once - then issue the T+ and T- commands to fine tune
    // 1/16uS resolution timer, ticks per sidereal second
    EEPROM_writeQuad(EE_siderealInterval,(byte*)&siderealInterval);
    
    // finally, stop the init from happening again
    EEPROM_writeQuad(EE_autoInitKey,(byte*)&autoInitKey);
    
    // clear the pointing model
    saveAlignModel();
  }

  // this sets the sidereal timer, controls the tracking speed so that the mount moves precisely with the stars
  EEPROM_readQuad(EE_siderealInterval,(byte*)&siderealInterval);

  // 16MHZ clocks for steps per second of sidereal tracking
  cli(); SiderealRate=siderealInterval/StepsPerSecond; TakeupRate=SiderealRate/4; sei();
  timerRateHA =SiderealRate;
  timerRateDec=SiderealRate;

  // backlash takeup rates
  timerRateBacklashHA =timerRateHA /BacklashTakeupRate;
  timerRateBacklashDec=timerRateDec/BacklashTakeupRate;

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
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
  SetSiderealClockRate(siderealInterval);

  // Start Timer 1A
  TimerEnable(Timer1_base, TIMER_A);



  // we also initialise timer 2A and 3A here as they may get used uninitialised from
  // the interrupt for timer 1 if it gets triggered in the meantime
  // we will not start them yet though

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

  //IntMasterEnable(); // not sure if needed, it works without
#else
  // initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);
#endif

#if defined(__AVR__)
  if (StepsPerSecond<31)
    TCCR3B = (1 << WGM12) | (1 << CS10) | (1 << CS11);  // ~0 to 0.25 seconds   (4 steps per second minimum, granularity of timer is 4uS)   /64 pre-scaler
  else
    TCCR3B = (1 << WGM12) | (1 << CS11);                // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  if (StepsPerSecond<31)
    TCCR4B = (1 << WGM12) | (1 << CS10) | (1 << CS11);  // ~0 to 0.25 seconds   (4 steps per second minimum, granularity of timer is 4uS)   /64 pre-scaler
  else
    TCCR4B = (1 << WGM12) | (1 << CS11);                // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
#elif defined(__arm__) && defined(TEENSYDUINO)
  // set the system timer for millis() to the second highest priority
  SCB_SHPR3 = (32 << 24) | (SCB_SHPR3 & 0x00FFFFFF);

  itimer3.begin(TIMER3_COMPA_vect, (float)128 * 0.0625);
  itimer4.begin(TIMER4_COMPA_vect, (float)128 * 0.0625);

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH0, 32);
  // set the motor timers to run at the highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH1, 0);
  NVIC_SET_PRIORITY(IRQ_PIT_CH2, 0);

#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  TimerLoadSet(Timer3_base, TIMER_A, (int) (F_BUS / 1000000 * 128 * 0.0625));
  TimerLoadSet(Timer4_base, TIMER_A, (int) (F_BUS / 1000000 * 128 * 0.0625));

  // Start Timer 2A and 3A
  TimerEnable(Timer3_base, TIMER_A);
  TimerEnable(Timer4_base, TIMER_A);

  IntPrioritySet(Int_timer1, 1);
  IntPrioritySet(Int_timer3, 0);
  IntPrioritySet(Int_timer4, 0);
#endif

  // get ready for serial communications
  Serial1_Init(9600);
  Serial_Init(9600); // for Tiva TM4C the serial is redirected to serial5 in serial.ino file

#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__) || defined(W5100_ON)
  // get ready for Ethernet communications
  Ethernet_Init();
#endif
  
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite);  if (currentSite>3) currentSite=0; // site index is valid?
  float f; EEPROM_readQuad(EE_sites+(currentSite)*25+0,(byte*)&f); latitude=f;

#ifdef MOUNT_TYPE_ALTAZM
  celestialPoleDec=fabs(latitude);
  if (latitude<0) celestialPoleHA=180L; else celestialPoleHA=0L;
#else
  if (latitude<0) celestialPoleDec=-90.0; else celestialPoleDec=90.0;
#endif
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (latitude>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
  EEPROM_readQuad(EE_sites+(currentSite)*25+4,(byte*)&f); longitude=f;
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);

  // update starting coordinates to reflect NCP or SCP polar home position
  startHA = celestialPoleHA*StepsPerDegreeHA;
  startDec = celestialPoleDec*(double)StepsPerDegreeDec;
  cli();
  targetHA.part.m  = startHA; 
  targetHA.part.f  = 0;
  posHA            = startHA;
  targetDec.part.m = startDec; 
  targetDec.part.f = 0;
  posDec           = startDec;
  sei();
  
  // get date and time from EEPROM, start keeping time
  EEPROM_readQuad(EE_JD,(byte*)&f); JD=f;
  EEPROM_readQuad(EE_LMT,(byte*)&f); LMT=f;
  UT1=LMT+timeZone;
  UT1_start  =UT1;
  UT1mS_start=millis(); 

  LST=jd2last(JD,UT1);
  update_lst();
  
  // get the min. and max altitude
  minAlt=EEPROM.read(EE_minAlt)-128;
  maxAlt=EEPROM.read(EE_maxAlt);

  // get the backlash amounts
  backlashDec=EEPROM_readInt(EE_backlashDec);
  backlashHA=EEPROM_readInt(EE_backlashHA);
  
  // get the PEC status
  PECstatus  =EEPROM.read(EE_PECstatus); 
  PECrecorded=EEPROM.read(EE_PECrecorded);
  if (!PECrecorded) PECstatus=IgnorePEC;
  for (int i=0; i<PECBufferSize; i++) PEC_buffer[i]=EEPROM.read(EE_PECindex+i);
  EEPROM_readQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
  EEPROM_readQuad(EE_PECsense_index,(byte*)&PECindex_sense);
  #ifdef PEC_SENSE_OFF
  PECindex_sense=0;
  #endif
  
  // get the Park status
  parkSaved=EEPROM.read(EE_parkSaved);
  parkStatus=EEPROM.read(EE_parkStatus);

  // get the pulse-guide rate
  currentPulseGuideRate=EEPROM.read(EE_pulseGuideRate); if (currentPulseGuideRate>GuideRate1x) currentPulseGuideRate=GuideRate1x;

  // get the Goto rate and constrain values to the limits (1/2 to 2X the MaxRate,) maxRate is in 16MHz clocks but stored in micro-seconds
  maxRate=EEPROM_readInt(EE_maxRate)*16;
  if (maxRate<(MaxRate/2L)*16L) maxRate=(MaxRate/2L)*16L;
  if (maxRate>(MaxRate*2L)*16L) maxRate=(MaxRate*2L)*16L;
  #ifndef RememberMaxRate_ON
  if (maxRate!=MaxRate*16L) { maxRate=MaxRate*16L; EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L)); }
  #endif
  
  // makes onstep think that you parked the 'scope
  // combined with a hack in the goto syncEqu() function and you can quickly recover from
  // a reset without loosing much accuracy in the sky.  PEC is toast though.
  #ifdef RESCUE_MODE_ON
  parkSaved=true;    
  parkStatus=Parked;
  #endif

  // set the default guide rate, 16x sidereal
  setGuideRate(GuideRate16x);
  enableGuideRate(GuideRate16x);
  delay(110);
  
  // prep timers
  cli(); 
  guideSiderealTimer = lst;
  PecSiderealTimer = lst;
  siderealTimer = lst;
  PECtime_lastSense = lst;
  clockTimer=millis(); 
  sei();

}

void loop() {

  // GUIDING -------------------------------------------------------------------------------------------
  if (trackingState!=TrackingMoveTo) { 

    // ST4 INTERFACE -------------------------------------------------------------------------------------
    #if defined(ST4_ON) || defined(ST4_PULLUP)
    if (parkStatus==NotParked) {
      ST4RA_state=0; if (digitalRead(ST4RAw)==LOW) { if (digitalRead(ST4RAe)!=LOW) ST4RA_state='w'; } else if (digitalRead(ST4RAe)==LOW) ST4RA_state='e';
      ST4DE_state=0; if (digitalRead(ST4DEn)==LOW) { if (digitalRead(ST4DEs)!=LOW) ST4DE_state='n'; } else if (digitalRead(ST4DEs)==LOW) ST4DE_state='s';
      // RA changed?
      if (ST4RA_last!=ST4RA_state) {
        ST4RA_last=ST4RA_state;
        if (ST4RA_state) { 
          #ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            #ifdef ST4_HAND_CONTROL_ON
              enableGuideRate(currentGuideRate);
            #else
              enableGuideRate(currentPulseGuideRate);
            #endif
          #else
            enableGuideRate(currentGuideRate);
          #endif
          guideDirHA=ST4RA_state;
          guideDurationHA=-1;
          cli(); if (guideDirHA=='e') guideTimerRateHA=-guideTimerRate; else guideTimerRateHA=guideTimerRate; sei();
        } else {
          if (guideDirHA) { guideDirHA='b'; }
        }
      }
      // Dec changed?
      if (ST4DE_last!=ST4DE_state) {
        ST4DE_last=ST4DE_state;
        if (ST4DE_state) { 
          #ifdef SEPERATE_PULSE_GUIDE_RATE_ON
            #ifdef ST4_HAND_CONTROL_ON
              enableGuideRate(currentGuideRate);
            #else
              enableGuideRate(currentPulseGuideRate);
            #endif
          #else
            enableGuideRate(currentGuideRate);
          #endif
          guideDirDec=ST4DE_state;
          guideDurationDec=-1;
          cli(); guideTimerRateDec=guideTimerRate; sei();
        } else {
          if (guideDirDec) { guideDirDec='b'; }
        }
      }
    }
    #endif

    guideHA=0;
    Guide();
  }

#ifndef MOUNT_TYPE_ALTAZM
  // PERIODIC ERROR CORRECTION -------------------------------------------------------------------------
  if ((trackingState==TrackingSidereal) && (!((guideDirHA || guideDirDec) && (activeGuideRate>GuideRate1x)))) { 
    // only active while sidereal tracking with a guide rate that makes sense
    Pec();
  } else disablePec();
  if (PECautoRecord>0) {
    // write PEC table to EEPROM, should do about 100 bytes/second
    PECautoRecord--;
    EEPROM.update(EE_PECindex+PECautoRecord,PEC_buffer[PECautoRecord]);
  }
#endif

  // SIDEREAL TRACKING ---------------------------------------------------------------------------------
  cli(); long tempLst=lst; sei();
  if (tempLst!=siderealTimer) {
    siderealTimer=tempLst;
    
    // only active while sidereal tracking with a guide rate that makes sense
    if ((trackingState==TrackingSidereal) && (!((guideDirHA || guideDirDec) && (activeGuideRate>GuideRate1x)))) {
      // apply the Tracking, Guiding, and PEC
      cli();
      targetHA.fixed+=fstepHA.fixed;
      targetDec.fixed+=fstepDec.fixed;
      if (pecTimerRateHA!=0) targetHA.fixed+=pstep.fixed;
      sei();

      #ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { SET(LEDnegPORT,LEDnegBit); LED_ON=false; } else { CLR(LEDnegPORT,LEDnegBit); LED_ON=true; } }
      #endif
    }

    // keeps the target where it's supposed to be while doing gotos
    if (trackingState==TrackingMoveTo) {
      if (lastTrackingState==TrackingSidereal) { 
        origTargetHA.fixed+=fstepHA.fixed;
        // origTargetDec isn't used in Alt/Azm mode
        cli();
        targetHA.fixed+=fstepHA.fixed;
        targetDec.fixed+=fstepDec.fixed;
        sei();  
      }
      moveTo();
    }

    // figure out the current Alititude
    if (lst%3==0) do_fastalt_calc();

#ifdef MOUNT_TYPE_ALTAZM
    // figure out the current Alt/Azm tracking rates
    if (lst%3!=0) do_altAzmRate_calc();
#else
    // figure out the current refraction compensated tracking rate
    if (refraction && (lst%3!=0)) do_refractionRate_calc();
#endif
}

  // HOUSEKEEPING --------------------------------------------------------------------------------------
  // timer... falls in once a second, keeps the universal time clock ticking,
  // handles PPS GPS signal processing, watches safety limits, adjusts tracking rate for refraction
  unsigned long m=millis();
  if ((long)(m-(clockTimer+999UL))>0) {
    clockTimer=m;

    // for testing, average steps per second
    if (debugv1>100000) debugv1=100000; if (debugv1<0) debugv1=0;
    
    debugv1=(debugv1*19+(targetHA.part.m*1000-lastTargetHA))/20;
    lastTargetHA=targetHA.part.m*1000;
    
    // update the UT1 CLOCK
    // this clock doesn't rollover (at 24 hours) since that would cause date confusion
    double t2=(double)(m-UT1mS_start)/1000.0;
    UT1=UT1_start+t2/3600.0;   // This just needs to be accurate to the nearest second, it's about 10x better

    // update the local sidereal time floating point representation
    update_LST();
    
    #ifdef PEC_SENSE
    // see if we're on the PEC index
    if (trackingState==TrackingSidereal) PECav = analogRead(1);
    #endif
    
    // adjust tracking rate for Alt/Azm mounts
    // adjust tracking rate for refraction
    SetDeltaTrackingRate();

    #ifdef PPS_SENSE_ON
    // update clock
    if (trackingState==TrackingSidereal) {
      cli();
      PPSrateRatio=((double)1000000.0/(double)(PPSavgMicroS));
      if ((long)(micros()-(PPSlastMicroS+2000000UL))>0) PPSsynced=false; // if more than two seconds has ellapsed without a pulse we've lost sync
      sei();
      #ifdef STATUS_LED2_PINS_ON
      if (PPSsynced) { if (LED2_ON) { SET(LEDneg2PORT, LEDneg2Bit); LED2_ON=false; } else { CLR(LEDneg2PORT, LEDneg2Bit); LED2_ON=true; } } else { SET(LEDneg2PORT, LEDneg2Bit); LED2_ON=false; } // indicate PPS
      #endif
      if (LastPPSrateRatio!=PPSrateRatio) { SetSiderealClockRate(siderealInterval); LastPPSrateRatio=PPSrateRatio; }
    }
    #endif

    #ifdef STATUS_LED_PINS_ON
    if (trackingState!=TrackingSidereal) if (!LED_ON) { CLR(LEDnegPORT, LEDnegBit); LED_ON=true; }    // indicate PWR on 
    #endif
    #ifdef STATUS_LED2_PINS_ON
    if (trackingState==TrackingNone) if (LED2_ON) { SET(LEDneg2PORT, LEDneg2Bit); LED2_ON=false; }    // indicate STOP
    if (trackingState==TrackingMoveTo) if (!LED2_ON) { CLR(LEDneg2PORT, LEDneg2Bit); LED2_ON=true; }  // indicate GOTO
    #endif

    // safety checks, keeps mount from tracking past the meridian limit, past the underPoleLimit, below horizon limit, above the overhead limit, or past the Dec limits
    if (meridianFlip!=MeridianFlipNever) {
      if (pierSide==PierSideWest) { cli(); if (posHA+IHS>(minutesPastMeridianW*StepsPerDegreeHA/4L)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  } sei(); }
      if (pierSide==PierSideEast) { cli(); if (posHA+IHS>(underPoleLimit*StepsPerDegreeHA*15L))      { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  } sei(); }
    } else {
#ifndef MOUNT_TYPE_ALTAZM
      // when Fork mounted, ignore pierSide and just stop the mount if it passes the underPoleLimit
      cli(); if (posHA+IHS>(underPoleLimit*StepsPerDegreeHA*15L)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#else
      // when Alt/Azm mounted, just stop the mount if it passes +180 degrees Azm
      cli(); if (posHA+IHS>(180L*StepsPerDegreeHA*15L)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#endif
    }
#ifndef MOUNT_TYPE_ALTAZM
    if ((getApproxDec()<minDec) || (getApproxDec()>maxDec)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif      

    // support for limit switch(es)
    #ifdef LIMIT_SENSE_ON  
    if (digitalRead(LimitPin)==LOW) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
    #endif

    // check altitude every second
    if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

    // reset the sidereal clock once a day, to keep the significant digits from being consumed
    // I'm shooting for keeping OnStep reliable for about 50 days of continuous uptime (until millis() rolls over)
    // really working with a single and should have, just barely, enough significant digits to get us through a day
    unsigned long lst_now=lst_start+round( (double)((m-lst_mS_start)/10.0) * 1.00273790935);
    if ((lst_now-lst_start)>24L*3600L*100L) update_lst();

  } else {
  // COMMAND PROCESSING --------------------------------------------------------------------------------
  // acts on commands recieved across Serial0 and Serial1 interfaces
    processCommands();
  }
}
