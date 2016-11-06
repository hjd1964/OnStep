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
// Until this is fixed YOU MUST MANUALLY UN-COMMENT the #include line below if using the Launchpad Connected device.
// Notice OnStep uses the EthernetPlus.h library for the W5100:
// this is available at: https://github.com/hjd1964/EthernetPlus and should be installed in your "~\Documents\Arduino\libraries" folder
#if defined(W5100_ON)
//#include "EthernetPlus.h"
#endif
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
//#include "Ethernet.h"
#endif

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define F_BUS SysCtlClockGet() // no pre-scaling of timers on Tiva Launchpads
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// There is a bug with SysCtlClockGet and TM4C1294 MCUs. At the moment we just hardcode the value and set the cpu frequency manually at the start of setup()
#define F_BUS 120000000 
#endif

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "03 18 16"
#define FirmwareNumber "1.0a32"
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

long last_loop_micros=0;             // workload monitoring
long this_loop_time=0;
long loop_time=0;
long worst_loop_time=0;

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
boolean onTrack=false;

long    maxRate = MaxRate*16L;
volatile long    timerRateAxis1 = 0;
volatile long    timerRateBacklashAxis1 = 0;
volatile boolean inbacklashAxis1 = false;
boolean faultAxis1 = false;
volatile long    timerRateAxis2 = 0;
volatile long    timerRateBacklashAxis2 = 0;
volatile boolean inbacklashAxis2 = false;
boolean faultAxis2 = false;

#ifdef MOUNT_TYPE_ALTAZM
#define default_tracking_rate 0
#else
#define default_tracking_rate 1
#endif
volatile double  trackingTimerRateAxis1= default_tracking_rate;
volatile double  trackingTimerRateAxis2= default_tracking_rate;
volatile double  pecTimerRateAxis1     = 0.0;
volatile double  guidetimerRateAxis1   = 0.0;
volatile double  guideTimerRateAxis2   = 0.0;
volatile double  timerRateRatio        = ((double)StepsPerDegreeAxis1/(double)StepsPerDegreeAxis2);
volatile boolean useTimerRateRatio     = (StepsPerDegreeAxis1!=StepsPerDegreeAxis2);
#define StepsPerSecondAxis1              ((double)StepsPerDegreeAxis1/240.0)
#define StepsPerSecondAxis2              ((double)StepsPerDegreeAxis2/240.0)
#define SecondsPerWormRotationAxis1      ((long)(StepsPerWormRotationAxis1/StepsPerSecondAxis1))
volatile double StepsForRateChangeAxis1= (double)DegreesForAcceleration*(double)StepsPerDegreeAxis1*4.0;
volatile double StepsForRateChangeAxis2= (double)DegreesForAcceleration*(double)StepsPerDegreeAxis2*4.0;

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

// fix UnderPoleLimit for fork mounts
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
#undef UnderPoleLimit
#define UnderPoleLimit 12
#endif

#ifdef MOUNT_TYPE_GEM
long celestialPoleHA  = 90L;
#endif
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
long celestialPoleHA  = 0L;
#endif
double celestialPoleDec = 90.0;

volatile long posAxis1   = 90L*(long)StepsPerDegreeAxis1;// hour angle position in steps
long trueAxis1           = 90L*(long)StepsPerDegreeAxis1;// correction to above for motor shaft position steps
volatile long startAxis1 = 90L*(long)StepsPerDegreeAxis1;// hour angle of goto start position in steps
volatile fixed_t targetAxis1;                            // hour angle of goto end   position in steps
volatile byte dirAxis1   = 1;                            // stepping direction + or -
double newTargetRA       = 0.0;                          // holds the RA for goTos
fixed_t origTargetAxis1;

volatile long posAxis2   = 90L*(long)StepsPerDegreeAxis2;// declination position in steps
long trueAxis2           = 90L*(long)StepsPerDegreeAxis2;// correction to above for motor shaft position steps
volatile long startAxis2 = 90L*(long)StepsPerDegreeAxis2;// declination of goto start position in steps
volatile fixed_t targetAxis2;                            // declination of goto end   position in steps
volatile byte dirAxis2   = 1;                            // stepping direction + or -
double newTargetDec      = 0.0;                          // holds the Dec for goTos
long origTargetAxis2     = 0;

double newTargetAlt=0.0, newTargetAzm=0.0;         // holds the altitude and azmiuth for slews
double currentAlt = 45;                            // the current altitude
int    minAlt;                                     // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                     // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)

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
#define PecPin         2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin       3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   2560
#define LEDposPin      8    // Pin 8 (LED)   PH5
#define LEDnegPin      9    // Pin 9 (GND)   PH6

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin    4    // Pin 4 (Dir)   PG5
#define Axis25vPin     5    // Pin 5 (5V?)   PE3
#define Axis2StepPin   6    // Pin 6 (Step)  PH3
#define Axis2GndPin    7    // Pin 7 (GND)   PH4

#define LEDneg2Pin    10    // Pin 10 (GND)  PB4

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
// The Arduino attachInterrupt function works in two modes, on the '2560 it takes an Interrupt# on the Teensy and others it takes a Pin#
#define PpsInt         2    // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

#define Axis1DirPin   11    // Pin 11 (Dir)  PB5
#define Axis15vPin    12    // Pin 12 (5V?)  PB6
#define Axis1StepPin  13    // Pin 13 (Step) PB7
                            // Pin GND (GND)
                         
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      22    // Pin 22 (Microstep Mode 0)
#define Axis1_M1      23    // Pin 23 (Microstep Mode 1)
#define Axis1_M2      24    // Pin 24 (Microstep Mode 2)
#define Axis1_EN      25    // Pin 25 (Enabled when LOW)
#define Axis1_FAULT   26    // Pin 26 (Fault if LOW)

#define Axis2_M0      27    // Pin 27 (Microstep Mode 0)
#define Axis2_M1      28    // Pin 28 (Microstep Mode 1)
#define Axis2_M2      29    // Pin 29 (Microstep Mode 2)
#define Axis2_EN      30    // Pin 30 (Enabled when LOW)
#define Axis2_FAULT   31    // Pin 31 (Fault if LOW)

#define Axis1_Mode    32    // Pin 32 (Mode switch for Axis1)
#define Axis2_Mode    33    // Pin 33 (Mode switch for Axis2)

#define LEDposBit      5    // Pin 8
#define LEDposPORT PORTH    //
#define LEDnegBit      6    // Pin 9
#define LEDnegPORT PORTH    //
#define LEDneg2Bit     4    // Pin 10
#define LEDneg2PORT PORTB   //

#define Axis2DirBit    5    // Pin 4
#define Axis2DirPORT PORTG  //
#define Axis25vBit     3    // Pin 5
#define Axis25vPORT  PORTE  //
#define Axis2StepBit   3    // Pin 6
#define Axis2StepPORT PORTH //
#define Axis2GNDBit    4    // Pin 7
#define Axis2GNDPORT PORTH  //

#define Axis1DirBit    5    // Pin 11
#define Axis1DirPORT  PORTB //
#define Axis15vBit     6    // Pin 12
#define Axis15vPORT   PORTB //
#define Axis1StepBit   7    // Pin 13
#define Axis1StepPORT PORTB //

// ST4 interface
#ifdef ST4_ALTERNATE_PINS_ON
#define ST4RAw        47    // Pin 47 ST4 RA- West
#define ST4DEs        43    // Pin 43 ST4 DE- South
#define ST4DEn        45    // Pin 45 ST4 DE+ North
#define ST4RAe        49    // Pin 49 ST4 RA+ East
#else
#define ST4RAw        47    // Pin 47 ST4 RA- West
#define ST4DEs        49    // Pin 49 ST4 DE- South
#define ST4DEn        51    // Pin 51 ST4 DE+ North
#define ST4RAe        53    // Pin 53 ST4 RA+ East
#endif

#elif defined(__arm__) && defined(TEENSYDUINO)
// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin         2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin       3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   2560
#define LEDposPin      8    // Pin 8 (LED)   PH5
#define LEDnegPin      9    // Pin 9 (GND)   PH6

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin    4    // Pin 4 (Dir)   PG5
#define Axis25vPin     5    // Pin 5 (5V?)   PE3
#define Axis2StepPin   6    // Pin 6 (Step)  PH3
#define Axis2GndPin    7    // Pin 7 (GND)   PH4

#define LEDneg2Pin     7    // Pin 7 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        23    // Pin 23 (PPS time source, GPS for example)

#define Axis1DirPin   10    // Pin 10 (Dir)
#define Axis15vPin    11    // Pin 11 (5V?)
#define Axis1StepPin  12    // Pin 12 (Step)
                            // Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      13    // Pin 13 (Microstep Mode 0)
#define Axis1_M1      14    // Pin 14 (Microstep Mode 1)
#define Axis1_M2      15    // Pin 15 (Microstep Mode 2)
#define Axis1_EN      16    // Pin 16 (Enabled when LOW)
#define Axis1_FAULT   17    // Pin 17 (Fault if LOW)

#define Axis2_M0      18    // Pin 18 (Microstep Mode 0)
#define Axis2_M1      19    // Pin 19 (Microstep Mode 1)
#define Axis2_M2      20    // Pin 20 (Microstep Mode 2)
#define Axis2_EN      21    // Pin 21 (Enabled when LOW)
#define Axis2_FAULT   22    // Pin 22 (Fault if LOW)

#define LEDposBit      0    // Pin 8
#define LEDposPORT PORTB    //
#define LEDnegBit      1    // Pin 9
#define LEDnegPORT PORTB    //
#define LEDneg2Bit     7    // Pin 7
#define LEDneg2PORT PORTD   //

#define Axis2DirBit    4    // Pin 4
#define Axis2DirPORT PORTD  //
#define Axis25vBit     5    // Pin 5
#define Axis25vPORT  PORTD  //
#define Axis2StepBit   6    // Pin 6
#define Axis2StepPORT PORTD //
#define Axis2GNDBit    7    // Pin 7
#define Axis2GNDPORT PORTD  //

#define Axis1DirBit    2    // Pin 10
#define Axis1DirPORT  PORTB //
#define Axis15vBit     3    // Pin 11
#define Axis15vPORT   PORTB //
#define Axis1StepBit   4    // Pin 12
#define Axis1StepPORT PORTB //

// ST4 interface
#define ST4RAw        24    // Pin 24 ST4 RA- West
#define ST4DEs        25    // Pin 25 ST4 DE- South
#define ST4DEn        26    // Pin 26 ST4 DE+ North
#define ST4RAe        27    // Pin 27 ST4 RA+ East

#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
// Note that TM4C123 has resistors R9 anr R10 between pins 14 (B6) and 23 (D0) and between pins 15 (B7) and 24 (D1)
// Make sure you look at the list of the pins and options to avoid clashes or desolder the two bridges
// These pins are used for Axis2_M2 (14) and DE_M3 (15) and for Axis2DirPin (23) and Axis25vPin (24)
// If you have defined AXIS2_MODE_OFF in Config.h you should be safe to leave things as they are.

// Also note that we are using UART1 and UART 5 which use pins 3-6

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        11    // Pin A2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin      12    // Pin A3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      2    // Pin B5 (LED)
#define LEDnegPin     33    // Pin D6 (GND)


// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin   23    // Pin D0 (Dir)
#define Axis25vPin    24    // Pin D1 (5V?)
#define Axis2StepPin  25    // Pin D2 (Step)
#define Axis2GndPin   26    // Pin D3 (GND)

#define LEDneg2Pin    26    // Pin D3 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        19    // Pin B2 (PPS time source, GPS for example)

#define Axis1DirPin   27    // Pin E1 (Dir)
#define Axis15vPin    28    // Pin E2 (5V?)
#define Axis1StepPin  29    // Pin E3 (Step)
// Pin GND (GND)
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      34    // Pin C7 (Microstep Mode 0)
#define Axis1_M1      35    // Pin C6 (Microstep Mode 1)
#define Axis1_M2      36    // Pin C5 (Microstep Mode 2)
#define Axis1_EN      37    // Pin C4 (Enabled when LOW)
#define Axis1_FAULT   38    // Pin B3 (Fault if LOW)

#define Axis2_M0      13    // Pin A4 (Microstep Mode 0)
#define Axis2_M1      14    // Pin B6 (Microstep Mode 1) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 14 is connected to pin 23
#define Axis2_M2      15    // Pin B7 (Microstep Mode 2) IF USED MAKE SURE YOU DESOLDER A BRIDGE or change pins around, otherwise pin 15 is connected to pin 24
#define Axis2_EN      18    // Pin E0 (Enabled when LOW)
#define Axis2_FAULT   17    // Pin F0 (Fault if LOW) NOTE, this is connected to pushbutton switch 2

#define LEDposBit  GPIO_PIN_5          // Pin 2
#define LEDposPORT GPIO_PORTB_BASE     //
#define LEDnegBit  GPIO_PIN_6          // Pin 33
#define LEDnegPORT GPIO_PORTD_BASE     //
#define LEDneg2Bit GPIO_PIN_3          // Pin 26
#define LEDneg2PORT GPIO_PORTD_BASE    //

#define Axis2DirBit  GPIO_PIN_0        // Pin 23=D0=B6 = pin 14
#define Axis2DirPORT GPIO_PORTD_BASE   //
#define Axis25vBit   GPIO_PIN_1        // Pin 24=D1=B7 = pin 15
#define Axis25vPORT  GPIO_PORTD_BASE   //
#define Axis2StepBit GPIO_PIN_2        // Pin 25
#define Axis2StepPORT GPIO_PORTD_BASE  //
#define Axis2GNDBit  GPIO_PIN_3        // Pin 26
#define Axis2GNDPORT GPIO_PORTD_BASE   //

#define Axis1DirBit   GPIO_PIN_1       // Pin 27
#define Axis1DirPORT  GPIO_PORTE_BASE  //
#define Axis15vBit    GPIO_PIN_2       // Pin 28
#define Axis15vPORT   GPIO_PORTE_BASE  //
#define Axis1StepBit  GPIO_PIN_3       // Pin 29
#define Axis1StepPORT GPIO_PORTE_BASE  //

// ST4 interface
#define ST4RAw         7    // Pin B4 ST4 RA- West
#define ST4DEs         8    // Pin A5 ST4 DE- South
#define ST4DEn         9    // Pin A6 ST4 DE+ North
#define ST4RAe        10    // Pin A7 ST4 RA+ East

#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// No need to desolder anything on this launchpad as pins we are using are not bridged

// Note that we are using UART7 and UART 5 which use pins 3-5 and pin 8 (C4,C5,C6,C7)

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin        11    // Pin P2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin      12    // Pin N3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LEDposPin      2    // Pin E4 (LED)
#define LEDnegPin     33    // Pin L1 (GND)


// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define Axis2DirPin   23    // Pin E0 (Dir)
#define Axis25vPin    24    // Pin E1 (5V?)
#define Axis2StepPin  25    // Pin E2 (Step)
#define Axis2GndPin   26    // Pin E3 (GND)

#define LEDneg2Pin    26    // Pin E3 (GND)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PpsPin        19    // Pin M3 (PPS time source, GPS for example)

#define Axis1DirPin   27    // Pin D7 (Dir)
#define Axis15vPin    28    // Pin A6 (5V?)
#define Axis1StepPin  29    // Pin M4 (Step)
// Pin GND (GND)
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define Axis1_M0      34    // Pin L0 (Microstep Mode 0)
#define Axis1_M1      35    // Pin L5 (Microstep Mode 1)
#define Axis1_M2      36    // Pin L4 (Microstep Mode 2)
#define Axis1_EN      37    // Pin G0 (Enabled when LOW)
#define Axis1_FAULT   38    // Pin F3 (Fault if LOW)

#define Axis2_M0      13    // Pin N2 (Microstep Mode 0)
#define Axis2_M1      14    // Pin D0 (Microstep Mode 1)
#define Axis2_M2      15    // Pin D1 (Microstep Mode 2)
#define Axis2_EN      18    // Pin H2 (Enabled when LOW)
#define Axis2_FAULT   17    // Pin H3 (Fault if LOW) NOTE, this is connected to pushbutton switch 2

#define LEDposBit  GPIO_PIN_4          // Pin 2
#define LEDposPORT GPIO_PORTE_BASE     //
#define LEDnegBit  GPIO_PIN_1          // Pin 33
#define LEDnegPORT GPIO_PORTL_BASE     //
#define LEDneg2Bit GPIO_PIN_3          // Pin 26
#define LEDneg2PORT GPIO_PORTE_BASE    //

#define Axis2DirBit  GPIO_PIN_0        // Pin 23
#define Axis2DirPORT GPIO_PORTE_BASE   //
#define Axis25vBit   GPIO_PIN_1        // Pin 24
#define Axis25vPORT  GPIO_PORTE_BASE   //
#define Axis2StepBit GPIO_PIN_2        // Pin 25
#define Axis2StepPORT GPIO_PORTE_BASE  //
#define Axis2GNDBit  GPIO_PIN_3        // Pin 26
#define Axis2GNDPORT GPIO_PORTE_BASE   //

#define Axis1DirBit   GPIO_PIN_7       // Pin 27
#define Axis1DirPORT  GPIO_PORTD_BASE  //
#define Axis15vBit    GPIO_PIN_6       // Pin 28
#define Axis15vPORT   GPIO_PORTA_BASE  //
#define Axis1StepBit  GPIO_PIN_4       // Pin 29
#define Axis1StepPORT GPIO_PORTM_BASE  //

// ST4 interface
#define ST4RAw         7    // Pin D3 ST4 RA- West
#define ST4DEs         6    // Pin C7 ST4 DE- South
#define ST4DEn         9    // Pin B2 ST4 DE+ North
#define ST4RAe        10    // Pin B3 ST4 RA+ East

#endif


#if defined(AXIS1_DISABLED_HIGH)
#define Axis1_Disabled HIGH
#define Axis1_Enabled LOW
#endif
#if defined(AXIS1_DISABLED_LOW)
#define Axis1_Disabled LOW
#define Axis1_Enabled HIGH
#endif
#if defined(AXIS2_DISABLED_HIGH)
#define Axis2_Disabled HIGH
#define Axis2_Enabled LOW
#endif
#if defined(AXIS2_DISABLED_LOW)
#define Axis2_Disabled LOW
#define Axis2_Enabled HIGH
#endif

#define DecDirEInit      1
#define DecDirWInit      0
volatile byte DecDir     = DecDirEInit;
#define HADirNCPInit     0
#define HADirSCPInit     1
volatile byte HADir      = HADirNCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

boolean highPrecision    = true;

#define TrackingNone             0
#define TrackingSidereal         1
#define TrackingMoveTo           2
#define TrackingSiderealDisabled 3
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
boolean atHome           = true;
boolean homeMount        = false;

// Command processing -------------------------------------------------------------------------------------------------------
#define BAUD 9600

boolean commandError     = false;
boolean quietReply       = false;

char reply[50];

char command[3];
char parameter[25];
byte bufferPtr= 0;

// for bluetooth/serial 0
char command_serial_zero[25];
char parameter_serial_zero[25];
byte bufferPtr_serial_zero= 0;

char Serial_recv_buffer[256] = "";
volatile byte Serial_recv_tail = 0;
volatile byte Serial_recv_head = 0;
char Serial_xmit_buffer[50] = "";
byte Serial_xmit_index = 0;

// for bluetooth/serial 1
char command_serial_one[25];
char parameter_serial_one[25];
byte bufferPtr_serial_one= 0;

char Serial1_recv_buffer[256] = "";
volatile byte Serial1_recv_tail = 0;
volatile byte Serial1_recv_head = 0;
char Serial1_xmit_buffer[50] = "";
byte Serial1_xmit_index = 0;

// for ethernet
char command_ethernet[25];
char parameter_ethernet[25];
byte bufferPtr_ethernet= 0;

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
long   IDS               = 0;

// tracking and PEC, fractional steps
fixed_t fstepAxis1;
fixed_t fstepAxis2;
fixed_t pstep;

// guide command
#define GuideRate1x        2
#define GuideRate16x       6
#define GuideRateNone      255

#define slewRate (1.0/(((double)StepsPerDegreeAxis1*(MaxRate/1000000.0)))*3600.0)
#define halfSlewRate (slewRate/2.0)
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,halfSlewRate,slewRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x half-MaxRate MaxRate

byte currentGuideRate        = GuideRate16x;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirAxis1        = 0;
long          guideDurationHA      = -1;
unsigned long guideDurationLastHA  = 0;
byte          guideDirAxis2        = 0;
long          guideDurationDec     = -1;
unsigned long guideDurationLastDec = 0;

long lasttargetAxis1=0;
long debugv1 = 0;

double  guideTimerRate    = 0;
fixed_t amountGuideHA;
fixed_t guideHA;
fixed_t amountGuideDec;
fixed_t guideDec;

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
byte    pecStatus         = IgnorePEC;
boolean pecRecorded       = false;
int     pecAutoRecord     = 0;      // for writing to PEC table to EEPROM
boolean pecFirstRecord    = false;
long    lastPecIndex      = -1;
long    pecIndex          = 0;
long    pecIndex1         = 0;
int     pecAnalogValue    = 0;
long    wormSensePos      = 0;      // in steps
boolean wormSensedAgain   = false;  // indicates PEC index was found
int     LastPecPinState   = PEC_SENSE_STATE;

boolean pecBufferStart    = false;

fixed_t accPecGuideHA;              // for PEC, buffers steps to be recorded

// backlash control
volatile int backlashAxis1  = 0;
volatile int backlashAxis2  = 0;
volatile int blAxis1        = 0;
volatile int blAxis2        = 0;

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

#define EE_posAxis1    0
#define EE_posAxis2    4
#define EE_pierSide    8
#define EE_parkStatus  9
#define EE_parkSaved   10

#define EE_currentSite 11
#define EE_LMT         14
#define EE_JD          18

#define EE_pulseGuideRate 22
#define EE_maxRate     23

#define EE_trueAxis1   30
#define EE_trueAxis2   34

#define EE_minAlt      40
#define EE_maxAlt      41

#define EE_doCor       42
#define EE_pdCor       46
#define EE_altCor      50
#define EE_azmCor      54
#define EE_IH          58
#define EE_ID          62

#define EE_pecStatus   70
#define EE_pecRecorded 71
#define EE_wormSensePos 76

#define EE_backlashAxis1 80
#define EE_backlashAxis2 84

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

#define EE_indexWorm 200

// it takes 3.3ms to record a value to EEPROM, this can effect tracking performance since interrupts are disabled during the operation.
// so we store PEC data in RAM while recording.  When done, sidereal tracking is turned off and the data is written to EEPROM.
// writing the data can take up to 3 seconds.

byte pecBuffer[PECBufferSize];

void setup() {
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  // due to a bug we set the frequency manually here
  uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), F_BUS);
#endif

// initialize some fixed-point values
  amountGuideHA.fixed=0;
  amountGuideDec.fixed=0;
  guideHA.fixed=0;
  guideDec.fixed=0;
  accPecGuideHA.fixed=0;
  fstepAxis1.fixed=0;
  fstepAxis2.fixed=0;
  pstep.fixed=0;
  origTargetAxis1.fixed = 0;
  targetAxis1.part.m = 90L*(long)StepsPerDegreeAxis1;
  targetAxis1.part.f = 0;
  targetAxis2.part.m = 90L*(long)StepsPerDegreeAxis2;
  targetAxis2.part.f = 0;

  fstepAxis1.fixed=doubleToFixed(StepsPerSecondAxis1/100.0);
  
// initialize the stepper control pins RA and Dec
  pinMode(Axis1StepPin, OUTPUT);
  pinMode(Axis1DirPin, OUTPUT); 
  pinMode(Axis2GndPin, OUTPUT);
  CLR(Axis2GNDPORT, Axis2GNDBit);
  pinMode(Axis2StepPin, OUTPUT); 
  pinMode(Axis2DirPin, OUTPUT); 

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
  pinMode(Axis15vPin, OUTPUT);
  SET(Axis15vPORT, Axis15vBit);
  pinMode(Axis25vPin, OUTPUT);
  SET(Axis25vPORT, Axis25vBit);
#endif

// PEC index sense
#ifdef PEC_SENSE_ON
  pinMode(PecPin, INPUT);
#endif
#ifdef PEC_SENSE_PULLUP
  pinMode(PecPin, INPUT_PULLUP);
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

// inputs for stepper drivers fault signal
  pinMode(Axis1_FAULT,INPUT);
  pinMode(Axis2_FAULT,INPUT);

// disable the stepper drivers for now, if the enable lines are connected
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,Axis1_Disabled);
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis2_EN,Axis2_Disabled);

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#ifdef AXIS1_MODE
  pinMode(Axis1_M0, OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE & 1));
  pinMode(Axis1_M1, OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE>>1 & 1));
  pinMode(Axis1_M2, OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE>>2 & 1));
#endif
#ifdef AXIS2_MODE
  pinMode(Axis2_M0, OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE & 1));
  pinMode(Axis2_M1, OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE>>1 & 1));
  pinMode(Axis2_M2, OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE>>2 & 1));
#endif

// if the stepper driver decay control is wired in request the decay mode
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  DecayModeTracking();
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
  long autoInitKey = initKey;
  long thisAutoInitKey;
  if (INIT_KEY) EEPROM_writeLong(EE_autoInitKey,autoInitKey);
  thisAutoInitKey=EEPROM_readLong(EE_autoInitKey);
  if (autoInitKey!=thisAutoInitKey) {
    // init the site information, lat/long/tz/name
    EEPROM.write(EE_currentSite,0);
    latitude=0; longitude=0;
    for (int l=0; l<4; l++) {
      EEPROM_writeFloat(EE_sites+(l)*25+0,latitude);
      EEPROM_writeFloat(EE_sites+(l)*25+4,longitude);
      EEPROM.write(EE_sites+(l)*25+8,128);
      EEPROM.write(EE_sites+(l)*25+9,0);
    }
  
    // init the date and time January 1, 2013. 0 hours LMT
    JD=2456293.5;
    LMT=0.0;
    EEPROM_writeFloat(EE_JD,JD);
    EEPROM_writeFloat(EE_LMT,LMT);
  
    // init the min and max altitude
    minAlt=-10;
    maxAlt=85;
    EEPROM.write(EE_minAlt,minAlt+128);
    EEPROM.write(EE_maxAlt,maxAlt);
  
    // init (clear) the backlash amounts
    EEPROM_writeInt(EE_backlashAxis2,0);
    EEPROM_writeInt(EE_backlashAxis1,0);
  
    // init the PEC status, clear the index and buffer
    EEPROM.write(EE_pecStatus,IgnorePEC);
    EEPROM.write(EE_pecRecorded,false);
    for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_indexWorm+l,128);
    wormSensePos=0;
    EEPROM_writeLong(EE_wormSensePos,wormSensePos);
    
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
    EEPROM_writeLong(EE_siderealInterval,siderealInterval);
    
    // finally, stop the init from happening again
    EEPROM_writeLong(EE_autoInitKey,autoInitKey);
    
    // clear the pointing model
    saveAlignModel();
  }

  // this sets the sidereal timer, controls the tracking speed so that the mount moves precisely with the stars
  siderealInterval=EEPROM_readLong(EE_siderealInterval);

  // 16MHZ clocks for steps per second of sidereal tracking
  cli(); SiderealRate=siderealInterval/StepsPerSecondAxis1; TakeupRate=SiderealRate/4L; sei();
  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;
  SetTrackingRate(default_tracking_rate);

  // backlash takeup rates
  timerRateBacklashAxis1=timerRateAxis1/BacklashTakeupRate;
  timerRateBacklashAxis2=timerRateAxis2/BacklashTakeupRate;

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
  if (StepsPerSecondAxis1<31)
    TCCR3B = (1 << WGM12) | (1 << CS10) | (1 << CS11);  // ~0 to 0.25 seconds   (4 steps per second minimum, granularity of timer is 4uS)   /64 pre-scaler
  else
    TCCR3B = (1 << WGM12) | (1 << CS11);                // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  if (StepsPerSecondAxis1<31)
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
  latitude=EEPROM_readFloat(EE_sites+(currentSite)*25+0);

#ifdef MOUNT_TYPE_ALTAZM
  celestialPoleDec=fabs(latitude);
  if (latitude<0) celestialPoleHA=180L; else celestialPoleHA=0L;
#else
  if (latitude<0) celestialPoleDec=-90.0; else celestialPoleDec=90.0;
#endif
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (latitude>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
  longitude=EEPROM_readFloat(EE_sites+(currentSite)*25+4);
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);

  // update starting coordinates to reflect NCP or SCP polar home position
  startAxis1 = celestialPoleHA*(long)StepsPerDegreeAxis1;
  startAxis2 = celestialPoleDec*(double)StepsPerDegreeAxis2;
  cli();
  targetAxis1.part.m = startAxis1;
  targetAxis1.part.f = 0;
  posAxis1           = startAxis1;
  trueAxis1          = startAxis1;
  targetAxis2.part.m = startAxis2;
  targetAxis2.part.f = 0;
  posAxis2           = startAxis2;
  trueAxis2          = startAxis2;
  sei();
  
  // get date and time from EEPROM, start keeping time
  JD=EEPROM_readFloat(EE_JD);
  LMT=EEPROM_readFloat(EE_LMT);
  UT1=LMT+timeZone;
  UT1_start=UT1;
  UT1mS_start=millis(); 

  update_lst(jd2last(JD,UT1));
  
  // get the min. and max altitude
  minAlt=EEPROM.read(EE_minAlt)-128;
  maxAlt=EEPROM.read(EE_maxAlt);
  #ifdef MOUNT_TYPE_ALTAZM
  if (maxAlt>87) maxAlt=87;
  #endif

  // get the backlash amounts
  backlashAxis2=EEPROM_readInt(EE_backlashAxis2);
  backlashAxis1=EEPROM_readInt(EE_backlashAxis1);
  
  // get the PEC status
  pecStatus  =EEPROM.read(EE_pecStatus);
  pecRecorded=EEPROM.read(EE_pecRecorded); if (!pecRecorded) pecStatus=IgnorePEC;
  for (int i=0; i<PECBufferSize; i++) pecBuffer[i]=EEPROM.read(EE_indexWorm+i);
  wormSensePos=EEPROM_readLong(EE_wormSensePos);
  #ifdef PEC_SENSE_OFF
  wormSensePos=0;
  pecStatus=IgnorePEC;
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
  sei();
  clockTimer=millis(); 
  last_loop_micros=micros();
  
  // autostart tracking
#if defined(AUTOSTART_TRACKING_ON) && (defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORKALT))
  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
  setHome();
  
  // enable the stepper drivers
  digitalWrite(Axis1_EN,Axis1_Enabled);
  digitalWrite(Axis2_EN,Axis2_Enabled);
  delay(10);

  // start tracking
  trackingState=TrackingSidereal;
#endif
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
          guideDirAxis1=ST4RA_state;
          guideDurationHA=-1;
          cli(); if (guideDirAxis1=='e') guidetimerRateAxis1=-guideTimerRate; else guidetimerRateAxis1=guideTimerRate; sei();
        } else {
          if (guideDirAxis1) { guideDirAxis1='b'; }
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
          guideDirAxis2=ST4DE_state;
          guideDurationDec=-1;
          cli(); guideTimerRateAxis2=guideTimerRate; sei();
        } else {
          if (guideDirAxis2) { guideDirAxis2='b'; }
        }
      }
    }
    #endif

    guideHA.fixed=0;
    Guide();
  }

#ifndef MOUNT_TYPE_ALTAZM
  // PERIODIC ERROR CORRECTION -------------------------------------------------------------------------
  if ((trackingState==TrackingSidereal) && (!((guideDirAxis1 || guideDirAxis2) && (activeGuideRate>GuideRate1x)))) { 
    // only active while sidereal tracking with a guide rate that makes sense
    Pec();
  } else DisablePec();
#endif

  // 0.01 SECOND TIMED ---------------------------------------------------------------------------------
  cli(); long tempLst=lst; sei();
  if (tempLst!=siderealTimer) {
    siderealTimer=tempLst;

#ifndef MOUNT_TYPE_ALTAZM
    // WRITE PERIODIC ERROR CORRECTION TO EEPROM -------------------------------------------------------
    if (pecAutoRecord>0) {
      // write PEC table to EEPROM, should do several hundred bytes/second
      pecAutoRecord--;
      EEPROM.update(EE_indexWorm+pecAutoRecord,pecBuffer[pecAutoRecord]);
    }
#endif

    // SIDEREAL TRACKING -------------------------------------------------------------------------------
    // only active while sidereal tracking with a guide rate that makes sense
    if ((trackingState==TrackingSidereal) && (!((guideDirAxis1 || guideDirAxis2) && (activeGuideRate>GuideRate1x)))) {
      // apply the Tracking, Guiding, and PEC
      cli();
      targetAxis1.fixed+=fstepAxis1.fixed;
      targetAxis2.fixed+=fstepAxis2.fixed;
      if (pecTimerRateAxis1!=0) targetAxis1.fixed+=pstep.fixed;
      sei();

      #ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { SET(LEDnegPORT,LEDnegBit); LED_ON=false; } else { CLR(LEDnegPORT,LEDnegBit); LED_ON=true; } }
      #endif
    }

    // SIDEREAL TRACKING DURING GOTOS ------------------------------------------------------------------
    // keeps the target where it's supposed to be while doing gotos
    if (trackingState==TrackingMoveTo) {
      if (lastTrackingState==TrackingSidereal) {
        // origTargetAxisn isn't used in Alt/Azm mode since meridian flips never happen
        origTargetAxis1.fixed+=fstepAxis1.fixed;
        // don't advance the target during meridian flips
        if ((pierSide==PierSideEast) || (pierSide==PierSideWest)) {
          cli();
          targetAxis1.fixed+=fstepAxis1.fixed;
          targetAxis2.fixed+=fstepAxis2.fixed;
          sei();  
        }
      }
      moveTo();
    }

    // figure out the current Altitude
    if (lst%3==0) do_fastalt_calc();

#ifdef MOUNT_TYPE_ALTAZM
    // figure out the current Alt/Azm tracking rates
    if (lst%3!=0) do_altAzmRate_calc();
#else
    // figure out the current refraction compensated tracking rate
    if (refraction && (lst%3!=0)) do_refractionRate_calc();
#endif
  }

  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  // HOUSEKEEPING --------------------------------------------------------------------------------------
  // timer... falls in once a second, keeps the universal time clock ticking,
  // handles PPS GPS signal processing, watches safety limits, adjusts tracking rate for refraction
  unsigned long m=millis();
  if ((long)(m-(clockTimer+999UL))>0) {
    clockTimer=m;

    // for testing, average steps per second
    if (debugv1>100000) debugv1=100000; if (debugv1<0) debugv1=0;
    
    debugv1=(debugv1*19+(targetAxis1.part.m*1000-lasttargetAxis1))/20;
    lasttargetAxis1=targetAxis1.part.m*1000;
    
    // update the UT1 CLOCK
    // this clock doesn't rollover (at 24 hours) since that would cause date confusion
    double t2=(double)(m-UT1mS_start)/1000.0;
    UT1=UT1_start+t2/3600.0;   // This just needs to be accurate to the nearest second, it's about 10x better
    
    #ifdef PEC_SENSE
    // see if we're on the PEC index
    if (trackingState==TrackingSidereal) 
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    pecAnalogValue = analogRead(1);
#elif defined(__arm__) && defined(TEENSYDUINO)
    pecAnalogValue = analogRead(14);
#endif
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

    // safety checks, keeps mount from tracking past the meridian limit, past the UnderPoleLimit, below horizon limit, above the overhead limit, or past the Dec limits
    if (meridianFlip!=MeridianFlipNever) {
      if (pierSide==PierSideWest) { cli(); if (posAxis1+IHS>(MinutesPastMeridianW*(long)StepsPerDegreeAxis1/4L)) { lastError=ERR_MERIDIAN; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  } sei(); }
      if (pierSide==PierSideEast) { cli(); if (posAxis1+IHS>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1))      { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  } sei(); }
    } else {
#ifndef MOUNT_TYPE_ALTAZM
      // when Fork mounted, ignore pierSide and just stop the mount if it passes the UnderPoleLimit
      cli(); if (posAxis1+IHS>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#else
      // when Alt/Azm mounted, just stop the mount if it passes MaxAzm
      cli(); if (posAxis1+IHS>(MaxAzm*(long)StepsPerDegreeAxis1)) { lastError=ERR_AZM; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#endif
    }
#ifndef MOUNT_TYPE_ALTAZM
    if ((getApproxDec()<MinDec) || (getApproxDec()>MaxDec)) { lastError=ERR_DEC; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif      

    // support for limit switch(es)
    #ifdef LIMIT_SENSE_ON  
    if (digitalRead(LimitPin)==LOW) { lastError=ERR_LIMIT_SENSE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
    #endif

    // check altitude every second
    if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { lastError=ERR_ALT; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

    // check for fault signal, stop any slew or guide and turn tracking off
#ifdef AXIS1_FAULT_LOW
    faultAxis1=(digitalRead(Axis1_FAULT)==LOW);
#endif
#ifdef AXIS1_FAULT_HIGH
    faultAxis1=(digitalRead(Axis1_FAULT)==HIGH);
#endif
#ifdef AXIS2_FAULT_LOW
    faultAxis2=(digitalRead(Axis2_FAULT)==LOW);
#endif
#ifdef AXIS2_FAULT_HIGH
    faultAxis2=(digitalRead(Axis2_FAULT)==HIGH);
#endif
    if (faultAxis1 || faultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }

    // basic check to see if we're not at home
    if (trackingState!=TrackingNone) atHome=false;

  } else {
  // COMMAND PROCESSING --------------------------------------------------------------------------------
  // acts on commands recieved across Serial0 and Serial1 interfaces
    processCommands();
  }
}

