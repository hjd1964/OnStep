/*
 * Title       OnStep
 * by          Howard Dutton
 *
 * Copyright (C) 2012 to 2017 Howard Dutton
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

// Use Config.h to configure OnStep to your requirements 
// help stepper driver configuration
#define TMC_LOWPWR      64
#define TMC_STEALTHCHOP 32
#define TMC_NINTPOL     16
#define SYNC_ANYWHERE_ON
#if defined(__arm__) && defined(TEENSYDUINO)
#define __ARM_Teensy3__
#endif
#include "TM4C.h"
#include "Config.h"
#include "Pins.h"
#include "errno.h"
#include "math.h"
#include "FPoint.h"
#include "Serial.h"
#include "Library.h"
#include "Align.h"
#include "Command.h"

// There is a bug in Arduino/Energia which ignores #ifdef preprocessor directives when generating a list of files
// Until this is fixed YOU MUST MANUALLY UN-COMMENT the #include line below if using the Launchpad Connected device.
#if defined(W5100_ON)
#define ETHERNET_ON
#include "SPI.h"
// OnStep uses the EthernetPlus.h library for the W5100 on the Mega2560 and Launchpad TM4C:
// this is available at: https://github.com/hjd1964/EthernetPlus and should be installed in your "~\Documents\Arduino\libraries" folder
//#include "EthernetPlus.h"
// OnStep uses the Ethernet.h library for the W5100 on the Teensy3.2:
//#include "Ethernet.h"
#endif
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
#define ETHERNET_ON
//#include "Ethernet.h"
#endif

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "02 28 17"
#define FirmwareNumber "1.0a"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__)
#define F_BUS SysCtlClockGet() // no pre-scaling of timers on Tiva Launchpads
#elif defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// There is a bug with SysCtlClockGet and TM4C1294 MCUs. At the moment we just hardcode the value and set the cpu frequency manually at the start of setup()
#define F_BUS 120000000 
#endif

// forces initialialization of a host of settings in EEPROM. OnStep does this automatically, most likely, you will want to leave this alone
#define INIT_KEY false    // set to true to keep automatic initilization from happening.  This is a one-time operation... upload to the Arduino, then set to false and upload again
#define initKey 915307548 // unique identifier for the current initialization format, do not change

// Time keeping ------------------------------------------------------------------------------------------------------------
long siderealTimer    = 0;           // counter to issue steps during tracking
long PecSiderealTimer = 0;           // time since worm wheel zero index for PEC
long guideSiderealTimer=0;           // counter to issue steps during guiding
unsigned long housekeepingTimer;     // counter for timing housekeeping

double UT1       = 0.0;              // the current universal time
double UT1_start = 0.0;              // the start of UT1
double JD  = 0.0;                    // and date, used for computing LST
double LMT = 0.0;                    //
double timeZone = 0.0;               //

long   lst_start = 0;                // this marks the start lst when UT1 is set 
volatile long lst = 0;               // this is the local (apparent) sidereal time in 1/100 seconds (23h 56m 4.1s per day = 86400 clock seconds/
                                     // 86164.09 sidereal seconds = 1.00273 clock seconds per sidereal second.)  Takes 249 days to roll over.

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

// PPS (GPS) ---------------------------------------------------------------------------------------------------------------
volatile unsigned long PPSlastMicroS = 1000000UL;
volatile unsigned long PPSavgMicroS = 1000000UL;
volatile double PPSrateRatio = 1.0;
volatile double LastPPSrateRatio = 1.0;
volatile boolean PPSsynced = false;

// Tracking and rate control -----------------------------------------------------------------------------------------------
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
boolean onTrack = false;

long    maxRate = MaxRate*16L;
volatile long    timerRateAxis1 = 0;
volatile long    timerRateBacklashAxis1 = 0;
volatile boolean inbacklashAxis1 = false;
boolean faultAxis1 = false;
volatile long    timerRateAxis2 = 0;
volatile long    timerRateBacklashAxis2 = 0;
volatile boolean inbacklashAxis2 = false;
boolean faultAxis2 = false;

#define default_tracking_rate 1
volatile double  trackingTimerRateAxis1= default_tracking_rate;
volatile double  trackingTimerRateAxis2= default_tracking_rate;
volatile double  timerRateRatio        = ((double)StepsPerDegreeAxis1/(double)StepsPerDegreeAxis2);
volatile boolean useTimerRateRatio     = (StepsPerDegreeAxis1!=StepsPerDegreeAxis2);
#define StepsPerSecondAxis1              ((double)StepsPerDegreeAxis1/240.0)
#define StepsPerSecondAxis2              ((double)StepsPerDegreeAxis2/240.0)
#define BreakDistAxis1                   (2L)
#define BreakDistAxis2                   (2L)
#define SecondsPerWormRotationAxis1      ((long)(StepsPerWormRotationAxis1/StepsPerSecondAxis1))
volatile double StepsForRateChangeAxis1= ((double)DegreesForAcceleration/sqrt((double)StepsPerDegreeAxis1))*0.3333333*StepsPerDegreeAxis1*MaxRate;
volatile double StepsForRateChangeAxis2= ((double)DegreesForAcceleration/sqrt((double)StepsPerDegreeAxis2))*0.3333333*StepsPerDegreeAxis2*MaxRate;
#ifndef DegreesForRapidStop
#define DegreesForRapidStop 1.0
#endif

// Interrupts and timers ---------------------------------------------------------------------------------------------------
#if defined(__ARM_TI_TM4C__)
#define cli() noInterrupts()
#define sei() interrupts()
#endif

#if defined(__ARM_Teensy3__)
IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);

#elif defined(__ARM_TI_TM4C__)
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

// Coordinates -------------------------------------------------------------------------------------------------------------
#ifdef MOUNT_TYPE_GEM
long celestialPoleAxis1  = 90L;
#endif
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
long celestialPoleAxis1  = 0L;
#endif
double celestialPoleAxis2 = 90.0;
// either 0 or (fabs(latitude))
#define AltAzmDecStartPos (fabs(latitude))

volatile long posAxis1   = 90L*(long)StepsPerDegreeAxis1;// hour angle position in steps
long trueAxis1           = 90L*(long)StepsPerDegreeAxis1;// correction to above for motor shaft position steps
volatile long startAxis1 = 90L*(long)StepsPerDegreeAxis1;// hour angle of goto start position in steps
volatile fixed_t targetAxis1;                            // hour angle of goto end   position in steps
volatile byte dirAxis1   = 1;                            // stepping direction + or -
double newTargetRA       = 0.0;                          // holds the RA for goTos
fixed_t origTargetAxis1;
#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long stepAxis1=1;
#else
#define stepAxis1 1
#endif

volatile long posAxis2   = 90L*(long)StepsPerDegreeAxis2;// declination position in steps
long trueAxis2           = 90L*(long)StepsPerDegreeAxis2;// correction to above for motor shaft position steps
volatile long startAxis2 = 90L*(long)StepsPerDegreeAxis2;// declination of goto start position in steps
volatile fixed_t targetAxis2;                            // declination of goto end   position in steps
volatile byte dirAxis2   = 1;                            // stepping direction + or -
double newTargetDec      = 0.0;                          // holds the Dec for goTos
long origTargetAxis2     = 0;
#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
volatile long stepAxis2=1;
#else
#define stepAxis2 1
#endif

double newTargetAlt=0.0, newTargetAzm=0.0;         // holds the altitude and azmiuth for slews
double currentAlt = 45;                            // the current altitude
int    minAlt;                                     // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                     // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)
bool   autoMeridianFlip = false;                   // automatically do a meridian flip and continue when we hit the MinutesPastMeridianW

// Fast port writting help -------------------------------------------------------------------------------------------------
#if defined(__ARM_TI_TM4C__)
#define CLR(x,y) (GPIOPinWrite(x,y,0))
#define SET(x,y) (GPIOPinWrite(x,y,y))
#define TGL(x,y) (GPIOPinRead(x,y)==0?GPIOPinWrite(x,y,y):GPIOPinWrite(x,y,0)) // untested, not used in current version
#else
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))
#endif

// Stepper driver enable/disable and direction -----------------------------------------------------------------------------
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

#define DecDirEInit  1
#define DecDirWInit  0
volatile byte DecDir = DecDirEInit;
#define HADirNCPInit 0
#define HADirSCPInit 1
volatile byte HADir  = HADirNCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

boolean highPrecision = true;

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
byte meridianFlip = MeridianFlipAlign;
#endif
#ifdef MOUNT_TYPE_FORK_ALT
byte meridianFlip = MeridianFlipNever;
#endif
#ifdef MOUNT_TYPE_ALTAZM
byte meridianFlip = MeridianFlipNever;
#endif

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

enum PreferredPierSide {PPS_BEST,PPS_EAST,PPS_WEST};
PreferredPierSide preferredPierSide = PPS_BEST;

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
// serial speed
unsigned long baudRate[10] = {115200,56700,38400,28800,19200,14400,9600,4800,2400,1200};
pserial PSerial;
pserial1 PSerial1;
bbspi BBSpi;

// Guide command ------------------------------------------------------------------------------------------------------------
#define GuideRate1x        2
#define GuideRate16x       6
#define GuideRateNone      255

#define slewRate (1.0/(((double)StepsPerDegreeAxis1*(MaxRate/1000000.0)))*3600.0)
#define slewRateX (slewRate/15.0)
#define halfSlewRate (slewRate/2.0)
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,halfSlewRate,slewRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x half-MaxRate MaxRate

byte currentGuideRate        = GuideRate16x;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirAxis1        = 0;
long          guideDurationHA      = -1;
unsigned long guideDurationLastHA  = 0;
volatile byte guideDirAxis2        = 0;
long          guideDurationDec     = -1;
unsigned long guideDurationLastDec = 0;

volatile double   guideTimerRateAxis1 = 0.0;
volatile double   guideTimerRateAxis2 = 0.0;
volatile uint32_t guideStartTimeAxis1 = 0;
volatile uint32_t guideStartTimeAxis2 = 0;
volatile uint32_t guideBreakTimeAxis1 = 0;
volatile uint32_t guideBreakTimeAxis2 = 0;

long lasttargetAxis1=0;
long debugv1 = 0;
boolean axis1Enabled = false;
boolean axis2Enabled = false;

double  guideTimerBaseRate = 0;
fixed_t amountGuideHA;
fixed_t guideHA;
fixed_t amountGuideDec;
fixed_t guideDec;

// PEC control --------------------------------------------------------------------------------------------------------------
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
volatile double pecTimerRateAxis1 = 0.0;
// it takes 3.3ms to record a value to EEPROM, this can effect tracking performance since interrupts are disabled during the operation.
// so we store PEC data in RAM while recording.  When done, sidereal tracking is turned off and the data is written to EEPROM.
// writing the data can take up to 3 seconds.
byte pecBuffer[PECBufferSize];

// Misc ---------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

// align
#ifndef ALIGN_GOTOASSIST_ON
  #if defined(MOUNT_TYPE_GEM)
  #define MAX_NUM_ALIGN_STARS '3'
  #elif defined(MOUNT_TYPE_FORK)
  #define MAX_NUM_ALIGN_STARS '3'
  #elif defined(MOUNT_TYPE_FORK_ALT)
  #define MAX_NUM_ALIGN_STARS '1'
  #elif defined(MOUNT_TYPE_ALTAZM)
  #define MAX_NUM_ALIGN_STARS '3'
  #endif
#else
  #if defined(MOUNT_TYPE_GEM)
  #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK)
  #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK_ALT)
  #define MAX_NUM_ALIGN_STARS '1'
  #elif defined(MOUNT_TYPE_ALTAZM)
  #define MAX_NUM_ALIGN_STARS '3'
  #endif
#endif

// current site index and name
byte currentSite = 0; 
char siteName[16];

// offset corrections simple align
double indexAxis1       = 0;
long   indexAxis1Steps  = 0;
double indexAxis2       = 0;
long   indexAxis2Steps  = 0;

// tracking and PEC, fractional steps
fixed_t fstepAxis1;
fixed_t fstepAxis2;

// status state
boolean LED_ON = false;
boolean LED2_ON = false;

// reticule control
#ifdef RETICULE_LED_PINS
int reticuleBrightness=RETICULE_LED_PINS;
#endif

// backlash control
volatile int backlashAxis1  = 0;
volatile int backlashAxis2  = 0;
volatile int blAxis1        = 0;
volatile int blAxis2        = 0;

// ST4 interface
char ST4RA_state = 0;
char ST4RA_last = 0;
char ST4DE_state = 0;
char ST4DE_last = 0;

// EEPROM Info --------------------------------------------------------------------------------------------------------------
// 0-1023 bytes
// general purpose storage 0..99

#define EE_posAxis1    0      // 4
#define EE_posAxis2    4      // 4
#define EE_pierSide    8      // 1
#define EE_parkStatus  9      // 1
#define EE_parkSaved   10     // 1

#define EE_currentSite 11     // 1 +2
#define EE_LMT         14     // 4
#define EE_JD          18     // 4

#define EE_pulseGuideRate 22  // 1
#define EE_maxRate     23     // 2

#define EE_autoMeridianFlip 25 // 1

#define EE_dfCor       26     // 4

#define EE_trueAxis1   30     // 4
#define EE_trueAxis2   34     // 4 +3

#define EE_minAlt      40     // 1
#define EE_maxAlt      41     // 1

#define EE_doCor       42     // 4
#define EE_pdCor       46     // 4
#define EE_altCor      50     // 4
#define EE_azmCor      54     // 4
#define EE_indexAxis1  58     // 4
#define EE_indexAxis2  62     // 4
#define EE_tfCor       66     // 4

#define EE_pecStatus   70     // 1
#define EE_pecRecorded 71     // 1 +4
#define EE_wormSensePos 76    // 4

#define EE_backlashAxis1 80   // 4
#define EE_backlashAxis2 84   // 4
#define EE_siderealInterval 88  // 4 +4

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

void setup() {
#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  // due to a bug we set the frequency manually here
  uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), F_BUS);
#endif

#ifdef ESP8266_CONTROL_ON
  pinMode(Axis1_Aux,OUTPUT);               // ESP8266 GPIO0
  digitalWrite(Axis1_Aux,HIGH); delay(20); // Run mode
  pinMode(Axis2_Aux,OUTPUT);               // ESP8266 RST
  digitalWrite(Axis2_Aux,LOW);  delay(20); // Reset, if LOW
  digitalWrite(Axis2_Aux,HIGH);            // Reset, inactive HIGH
#endif

// initialize some fixed-point values
  amountGuideHA.fixed=0;
  amountGuideDec.fixed=0;
  guideHA.fixed=0;
  guideDec.fixed=0;
  accPecGuideHA.fixed=0;
  fstepAxis1.fixed=0;
  fstepAxis2.fixed=0;
  origTargetAxis1.fixed = 0;
  targetAxis1.part.m = 90L*(long)StepsPerDegreeAxis1;
  targetAxis1.part.f = 0;
  targetAxis2.part.m = 90L*(long)StepsPerDegreeAxis2;
  targetAxis2.part.f = 0;
  fstepAxis1.fixed=doubleToFixed(StepsPerSecondAxis1/100.0);

// initialize alignment
  #ifdef MOUNT_TYPE_ALTAZM
  Align.init();
  #endif
  GeoAlign.init();

// initialize the stepper control pins Axis1 and Axis2
  pinMode(Axis1StepPin,OUTPUT);
  pinMode(Axis1DirPin,OUTPUT); 
#ifdef Axis2GndPin
  pinMode(Axis2GndPin,OUTPUT);
  digitalWrite(Axis2GndPin,LOW);
#endif
  pinMode(Axis2StepPin,OUTPUT); 
  pinMode(Axis2DirPin,OUTPUT); 

// override any status LED and set the reset pin HIGH
#if defined(W5100_ON) && defined(__ARM_Teensy3__)
#ifdef STATUS_LED_PINS_ON
#undef STATUS_LED_PINS_ON
#endif
#ifdef STATUS_LED_PINS
#undef STATUS_LED_PINS
#endif
  pinMode(RstPin,OUTPUT);
  digitalWrite(RstPin,LOW);
  delay(500);
  digitalWrite(RstPin,HIGH);
#endif

// light status LED (provides GND)
#ifdef STATUS_LED_PINS_ON
  pinMode(LEDnegPin,OUTPUT);
  digitalWrite(LEDnegPin,LOW);
// sometimes +5v is provided on a pin
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT);
  digitalWrite(LEDposPin,HIGH);
#endif
  LED_ON=true;
#endif

// light status LED (provides pwm'd GND for polar reticule)
#ifdef STATUS_LED_PINS
  pinMode(LEDnegPin,OUTPUT);
  digitalWrite(LEDnegPin,LOW);
// sometimes +5v is provided on a pin
#ifdef LEDposPin
  pinMode(LEDposPin,OUTPUT);
  digitalWrite(LEDposPin,HIGH);
#endif
  analogWrite(LEDnegPin,STATUS_LED_PINS);
  LED_ON=true;
#endif

// light reticule LED
#ifdef RETICULE_LED_PINS
#if defined(__ARM_Teensy3__) && !defined(ALTERNATE_PINMAP_ON)
  #ifdef STATUS_LED_PINS_ON
    #undef STATUS_LED_PINS_ON
  #endif
  #ifdef STATUS_LED_PINS
    #undef STATUS_LED_PINS
  #endif
#endif
  pinMode(ReticulePin,OUTPUT);
  analogWrite(ReticulePin,reticuleBrightness);
#endif

// light second status LED (provides just GND)
#ifdef STATUS_LED2_PINS_ON
  pinMode(LEDneg2Pin,OUTPUT);
  digitalWrite(LEDneg2Pin,LOW);
  LED2_ON=false;
#endif
// light second status LED (provides pwm'd GND for polar reticule)
#ifdef STATUS_LED2_PINS
  pinMode(LEDneg2Pin,OUTPUT);
  digitalWrite(LEDneg2Pin,LOW);
  analogWrite(LEDneg2Pin,STATUS_LED2_PINS);
#endif

// provide 5V power to stepper drivers if requested
#ifdef POWER_SUPPLY_PINS_ON  
  pinMode(Axis15vPin,OUTPUT);
  digitalWrite(Axis15vPin,HIGH);
  pinMode(Axis25vPin,OUTPUT);
  digitalWrite(Axis25vPin,HIGH);
#endif

// PEC index sense
#ifdef PEC_SENSE_ON
  pinMode(PecPin,INPUT);
#endif
#ifdef PEC_SENSE_PULLUP
  pinMode(PecPin,INPUT_PULLUP);
#endif

// limit switch sense
#ifdef LIMIT_SENSE_ON  
  pinMode(LimitPin,INPUT_PULLUP);
#endif

// ST4 interface
#ifdef ST4_ON
  pinMode(ST4RAw,INPUT);
  pinMode(ST4RAe,INPUT);
  pinMode(ST4DEn,INPUT);
  pinMode(ST4DEs,INPUT);
#endif
#ifdef ST4_PULLUP
  pinMode(ST4RAw,INPUT_PULLUP);
  pinMode(ST4RAe,INPUT_PULLUP);
  pinMode(ST4DEn,INPUT_PULLUP);
  pinMode(ST4DEs,INPUT_PULLUP);
#endif

// inputs for stepper drivers fault signal
#ifndef AXIS1_FAULT_OFF
  #if defined(__ARM_Teensy3__) && defined(ALTERNATE_PINMAP_ON)
    #ifdef AXIS1_FAULT_LOW
      pinMode(Axis1_FAULT,INPUT_PULLUP);
    #endif
    #ifdef AXIS1_FAULT_HIGH
      pinMode(Axis1_FAULT,INPUT_PULLDOWN);
    #endif
  #else
    pinMode(Axis1_FAULT,INPUT);
  #endif
#endif
#ifndef AXIS2_FAULT_OFF
  #if defined(__ARM_Teensy3__) && defined(ALTERNATE_PINMAP_ON)
    #ifdef AXIS2_FAULT_LOW
      pinMode(Axis2_FAULT,INPUT_PULLUP);
    #endif
    #ifdef AXIS1_FAULT_HIGH
      pinMode(Axis2_FAULT,INPUT_PULLDOWN);
    #endif
  #else
    pinMode(Axis2_FAULT,INPUT);
  #endif
#endif
  
  // initialize/disable the stepper drivers
  pinMode(Axis1_EN,OUTPUT); digitalWrite(Axis1_EN,Axis1_Disabled); axis1Enabled=false;
  pinMode(Axis2_EN,OUTPUT); digitalWrite(Axis2_EN,Axis2_Disabled); axis2Enabled=false;
  DecayModeTracking();

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#if !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // automatic mode switching during slews, initialize micro-step mode
  #ifdef AXIS1_MODE
    if ((AXIS1_MODE & 0b001000)==0) { pinMode(Axis1_M0,OUTPUT); digitalWrite(Axis1_M0,(AXIS1_MODE    & 1)); } else { pinMode(Axis1_M0,INPUT); }
    if ((AXIS1_MODE & 0b010000)==0) { pinMode(Axis1_M1,OUTPUT); digitalWrite(Axis1_M1,(AXIS1_MODE>>1 & 1)); } else { pinMode(Axis1_M1,INPUT); }
    if ((AXIS1_MODE & 0b100000)==0) { pinMode(Axis1_M2,OUTPUT); digitalWrite(Axis1_M2,(AXIS1_MODE>>2 & 1)); } else { pinMode(Axis1_M2,INPUT); }
  #endif
  
  #ifdef AXIS2_MODE
    if ((AXIS2_MODE & 0b001000)==0) { pinMode(Axis2_M0,OUTPUT); digitalWrite(Axis2_M0,(AXIS2_MODE    & 1)); } else { pinMode(Axis2_M0,INPUT); }
    if ((AXIS2_MODE & 0b010000)==0) { pinMode(Axis2_M1,OUTPUT); digitalWrite(Axis2_M1,(AXIS2_MODE>>1 & 1)); } else { pinMode(Axis2_M1,INPUT); }
    if ((AXIS2_MODE & 0b100000)==0) { pinMode(Axis2_M2,OUTPUT); digitalWrite(Axis2_M2,(AXIS2_MODE>>2 & 1)); } else { pinMode(Axis2_M2,INPUT); }
  #endif
#endif

#ifdef PPS_SENSE_ON
#if defined(__AVR_ATmega2560__)
  attachInterrupt(PpsInt,ClockSync,RISING);
#elif defined(__ARM_Teensy3__)
  attachInterrupt(PpsPin,ClockSync,RISING);
#elif defined(__ARM_TI_TM4C__)
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

    // init autoMeridianFlip
    EEPROM.write(EE_autoMeridianFlip,autoMeridianFlip);

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
  cli(); SiderealRate=siderealInterval/StepsPerSecondAxis1; TakeupRate=round(SiderealRate/1.1); sei();
  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;
  SetTrackingRate(default_tracking_rate);

  // backlash takeup rates
  timerRateBacklashAxis1=timerRateAxis1/BacklashTakeupRate;
  timerRateBacklashAxis2=timerRateAxis2/BacklashTakeupRate;

#if defined(__ARM_TI_TM4C__)
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

#if defined(__AVR_ATmega2560__)
  TCCR3B = (1 << WGM12) | (1 << CS11); // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  TCCR4B = (1 << WGM12) | (1 << CS11); // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
#elif defined(__ARM_Teensy3__)
  // set the system timer for millis() to the second highest priority
  SCB_SHPR3 = (32 << 24) | (SCB_SHPR3 & 0x00FFFFFF);

  itimer3.begin(TIMER3_COMPA_vect, (float)128 * 0.0625);
  itimer4.begin(TIMER4_COMPA_vect, (float)128 * 0.0625);

  // set the 1/100 second sidereal clock timer to run at the second highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH0, 32);
  // set the motor timers to run at the highest priority
  NVIC_SET_PRIORITY(IRQ_PIT_CH1, 0);
  NVIC_SET_PRIORITY(IRQ_PIT_CH2, 0);

#elif defined(__ARM_TI_TM4C__)
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
  PSerial.begin(9600); // for Tiva TM4C the serial is redirected to serial5 in serial.ino file
  PSerial1.begin(9600);

#ifdef ENTERNET_ON
  // get ready for Ethernet communications
  Ethernet_Init();
#endif
  
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite);  if (currentSite>3) currentSite=0; // site index is valid?
  latitude=EEPROM_readFloat(EE_sites+(currentSite)*25+0);

#ifdef MOUNT_TYPE_ALTAZM
  celestialPoleAxis2=AltAzmDecStartPos;
  if (latitude<0) celestialPoleAxis1=180L; else celestialPoleAxis1=0L;
#else
  if (latitude<0) celestialPoleAxis2=-90.0; else celestialPoleAxis2=90.0;
#endif
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (latitude>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
  longitude=EEPROM_readFloat(EE_sites+(currentSite)*25+4);
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  timeZone=decodeTimeZone(timeZone);
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);

  // update starting coordinates to reflect NCP or SCP polar home position
  startAxis1 = celestialPoleAxis1*(long)StepsPerDegreeAxis1;
  startAxis2 = celestialPoleAxis2*(double)StepsPerDegreeAxis2;
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
  #if !defined(RememberMaxRate_ON) && !defined(REMEMBER_MAX_RATE_ON)
  if (maxRate!=MaxRate*16L) { maxRate=MaxRate*16L; EEPROM_writeInt(EE_maxRate,(int)(maxRate/16L)); }
  #endif
  SetAccelerationRates(maxRate); // set the new acceleration rate

  // get autoMeridianFlip
  #ifdef REMEMBER_AUTO_MERIDIAN_FLIP_ON
  autoMeridianFlip=EEPROM.read(EE_autoMeridianFlip);
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
  housekeepingTimer=lst; 
  sei();
  last_loop_micros=micros();

  // autostart tracking
#if defined(AUTOSTART_TRACKING_ON) && (defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORKALT))
  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
  setHome();
  
  // enable the stepper drivers
  digitalWrite(Axis1_EN,Axis1_Enabled); axis1Enabled=true;
  digitalWrite(Axis2_EN,Axis2_Enabled); axis2Enabled=true;
  delay(10);

  // start tracking
  trackingState=TrackingSidereal;
#endif
}

void loop() {

  // GUIDING -------------------------------------------------------------------------------------------
  if (trackingState!=TrackingMoveTo) { 

#if defined(ST4_ON) || defined(ST4_PULLUP)
    // ST4 INTERFACE
    if (parkStatus==NotParked) {
      byte w1=digitalRead(ST4RAw); byte e1=digitalRead(ST4RAe); byte n1=digitalRead(ST4DEn); byte s1=digitalRead(ST4DEs);
      delayMicroseconds(50);
      byte w2=digitalRead(ST4RAw); byte e2=digitalRead(ST4RAe); byte n2=digitalRead(ST4DEn); byte s2=digitalRead(ST4DEs);
      // if signals aren't stable ignore them
      if ((w1==w2) && (e1==e2) && (n1==n2) && (s1==s2)) {
        ST4RA_state=0; if (w1==LOW) { if (e1!=LOW) ST4RA_state='w'; } else if (e1==LOW) ST4RA_state='e';
        ST4DE_state=0; if (n1==LOW) { if (s1!=LOW) ST4DE_state='n'; } else if (s1==LOW) ST4DE_state='s';
      }

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
          cli(); if (guideDirAxis1=='e') guideTimerRateAxis1=-guideTimerBaseRate; else guideTimerRateAxis1=guideTimerBaseRate; sei();
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
          cli(); if (guideDirAxis2=='s') guideTimerRateAxis2=-guideTimerBaseRate; else guideTimerRateAxis2=guideTimerBaseRate; sei();
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

  // 1/100 SECOND TIMED --------------------------------------------------------------------------------
  cli(); long tempLst=lst; sei();
  if (tempLst!=siderealTimer) {
    siderealTimer=tempLst;

#ifndef MOUNT_TYPE_ALTAZM
    // WRITE PERIODIC ERROR CORRECTION TO EEPROM
    if (pecAutoRecord>0) {
      // write PEC table to EEPROM, should do several hundred bytes/second
      pecAutoRecord--;
      EEPROM.update(EE_indexWorm+pecAutoRecord,pecBuffer[pecAutoRecord]);
    }
#endif

    // SIDEREAL TRACKING
    // only active while sidereal tracking with a guide rate that makes sense
    if (trackingState==TrackingSidereal) {
#ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { digitalWrite(LEDnegPin,HIGH); LED_ON=false; } else { digitalWrite(LEDnegPin,LOW); LED_ON=true; } }
#endif
    }

    // SIDEREAL TRACKING DURING GOTOS
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

    // SAFETY CHECKS
    // support for limit switch(es)
#ifdef LIMIT_SENSE_ON
    byte ls1=digitalRead(LimitPin); delayMicroseconds(50); byte ls2=digitalRead(LimitPin);
    if ((ls1==LOW) && (ls2==LOW)) { lastError=ERR_LIMIT_SENSE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif
    // check for fault signal, stop any slew or guide and turn tracking off
#ifdef AXIS1_FAULT_LOW
    faultAxis1=(digitalRead(Axis1_FAULT)==LOW);
#endif
#ifdef AXIS1_FAULT_HIGH
    faultAxis1=(digitalRead(Axis1_FAULT)==HIGH);
#endif
#ifdef AXIS1_FAULT_SPI
  if (lst%2==0) {
    spiStart(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
    faultAxis1=TMC2130_error();
    spiEnd();
  }
#endif
#ifdef AXIS2_FAULT_LOW
    faultAxis2=(digitalRead(Axis2_FAULT)==LOW);
#endif
#ifdef AXIS2_FAULT_HIGH
    faultAxis2=(digitalRead(Axis2_FAULT)==HIGH);
#endif
#ifdef AXIS2_FAULT_SPI
  if (lst%2==1) {
    spiStart(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
    faultAxis2=TMC2130_error();
    spiEnd();
  }
#endif
    if (faultAxis1 || faultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }
    // check altitude overhead limit and horizon limit
    if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { lastError=ERR_ALT; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

    // UPDATE THE UT1 CLOCK
    cli(); long cs=lst; sei();
    double t2=(double)((cs-lst_start)/100.0)/1.00273790935;
    // This just needs to be accurate to the nearest second, it's about 10x better
    UT1=UT1_start+(t2/3600.0);
  }

  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  cli(); long cs=lst; sei();
  if ((long)(cs-(housekeepingTimer+99L))>0) {
    housekeepingTimer=cs;

    // for testing, loop time
//    PSerial1.puts("stepAxis1=");
//    PSerial1.putf((double)loop_time/1000000.0);
//    PSerial1.puts("\r\n");
//    
    // for testing, average steps per second
//    if (debugv1>100000) debugv1=100000; if (debugv1<0) debugv1=0;
//    debugv1=(debugv1*19+(targetAxis1.part.m*1000-lasttargetAxis1))/20;
//    lasttargetAxis1=targetAxis1.part.m*1000;
    
    // adjust tracking rate for Alt/Azm mounts
    // adjust tracking rate for refraction
    SetDeltaTrackingRate();

    // basic check to see if we're not at home
    if (trackingState!=TrackingNone) atHome=false;

#ifdef PEC_SENSE
    // see if we're on the PEC index
    if (trackingState==TrackingSidereal) 
    pecAnalogValue = analogRead(AnalogPecPin);
#endif
    
#ifdef PPS_SENSE_ON
    // update clock via PPS
    if (trackingState==TrackingSidereal) {
      cli();
      PPSrateRatio=((double)1000000.0/(double)(PPSavgMicroS));
      if ((long)(micros()-(PPSlastMicroS+2000000UL))>0) PPSsynced=false; // if more than two seconds has ellapsed without a pulse we've lost sync
      sei();
  #ifdef STATUS_LED2_PINS_ON
      if (PPSsynced) { if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; } else { digitalWrite(LEDneg2Pin,LOW); LED2_ON=true; } } else { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; } // indicate PPS
  #endif
      if (LastPPSrateRatio!=PPSrateRatio) { SetSiderealClockRate(siderealInterval); LastPPSrateRatio=PPSrateRatio; }
    }
#endif

#ifdef STATUS_LED_PINS_ON
    // LED indicate PWR on 
    if (trackingState!=TrackingSidereal) if (!LED_ON) { digitalWrite(LEDnegPin,LOW); LED_ON=true; }
#endif
#ifdef STATUS_LED2_PINS_ON
    // LED indicate STOP and GOTO
    if (trackingState==TrackingNone) if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; }
    if (trackingState==TrackingMoveTo) if (!LED2_ON) { digitalWrite(LEDneg2Pin,LOW); LED2_ON=true; }
#endif

    // safety checks, keeps mount from tracking past the meridian limit, past the UnderPoleLimit, or past the Dec limits
    if (meridianFlip!=MeridianFlipNever) {
      if (pierSide==PierSideWest) {
        cli(); long p1=posAxis1+indexAxis1Steps; sei();
        if (p1>(MinutesPastMeridianW*(long)StepsPerDegreeAxis1/4L)) {
          // do an automatic meridian flip and continue if just tracking
          // checks: enabled && not too far past the meridian (doesn't make sense) && not in inaccessible area between east and west limits && finally that a slew isn't happening
          if (autoMeridianFlip && (p1<(MinutesPastMeridianW*(long)StepsPerDegreeAxis1/4L+(1.0/60.0)*(long)StepsPerDegreeAxis1)) && (p1>(-MinutesPastMeridianE*(long)StepsPerDegreeAxis1/4L)) && (trackingState!=TrackingMoveTo)) {
            double newRA,newDec;
            getEqu(&newRA,&newDec,false);
            if (goToEqu(newRA,newDec)) { // returns 0 on success
              lastError=ERR_MERIDIAN; 
              trackingState=TrackingNone;
            }
          } else {
            lastError=ERR_MERIDIAN; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;
          }
        }
      }
      if (pierSide==PierSideEast) { cli(); if (posAxis1+indexAxis1Steps>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  } sei(); }
    } else {
#ifndef MOUNT_TYPE_ALTAZM
      // when Fork mounted, ignore pierSide and just stop the mount if it passes the UnderPoleLimit
      cli(); if (posAxis1+indexAxis1Steps>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#else
      // when Alt/Azm mounted, just stop the mount if it passes MaxAzm
      cli(); if (posAxis1+indexAxis1Steps>((long)MaxAzm*(long)StepsPerDegreeAxis1)) { lastError=ERR_AZM; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#endif
    }
    // check for exceeding MinDec or MaxDec
#ifndef MOUNT_TYPE_ALTAZM
    if ((getApproxDec()<MinDec) || (getApproxDec()>MaxDec)) { lastError=ERR_DEC; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif      

  } else {
  // COMMAND PROCESSING --------------------------------------------------------------------------------
  // acts on commands recieved across Serial0 and Serial1 interfaces
    processCommands();
  }
}

