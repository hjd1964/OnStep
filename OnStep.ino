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

// Use Config.xxxxx.h to configure OnStep to your requirements

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "12 08 17"
#define FirmwareNumber "1.0a"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

// some defines to help with configuration
#define TMC_LOWPWR      64
#define TMC_STEALTHCHOP 32
#define TMC_NINTPOL     16
#define SYNC_ANYWHERE_ON

#include "src/HAL/HAL.h"

#include "Config.Classic.h"
#include "Config.MiniPCB.h"
#include "Config.MaxPCB.h"
#include "Config.Ramps14.h"
#include "Config.Mega2560Alt.h"
#include "Config.STM32.h"

#if !defined(Classic_ON) && !defined(MiniPCB_ON) && !defined(MaxPCB_ON) && !defined(TM4C_ON) && !defined(Ramps14_ON) && !defined(Mega2560Alt_ON) && !defined(STM32_ON)
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif

#if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
#if !defined(AXIS1_MODE) || !defined(AXIS1_MODE_GOTO)
  #error "Config.xxx.h error: AXIS1_MODE and AXIS1_MODE_GOTO must be set to a valid value."
#endif
#if !defined(AXIS2_MODE) || !defined(AXIS2_MODE_GOTO)
  #error "Config.xxx.h error: AXIS2_MODE and AXIS2_MODE_GOTO must be set to a valid value."
#endif
#endif

#if !defined(FOCUSER1_ON) && defined(FOCUSER2_ON)
#error "Focuser2 can't be enabled without first enabling Focuser1"
#endif

#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
#define SEPARATE_PULSE_GUIDE_RATE_ON
#endif
#ifndef GUIDE_TIME_LIMIT
#define GUIDE_TIME_LIMIT 0
#endif
#ifndef MaxRot
#define MaxRot MaxAxis3
#endif
#ifndef MinRot
#define MinRot MinAxis3
#endif
#include "errno.h"
#include "math.h"
#include "FPoint.h"
#include "Serial.h"
#include "Library.h"
#include "Align.h"
#include "Command.h"

#ifdef MODE_SWITCH_BEFORE_SLEW_SPI
#include "TMC2130.h"
//               SS      ,SCK     ,MISO     ,MOSI
tmc2130 tmcAxis1(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
tmc2130 tmcAxis2(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
#endif

#ifdef RTC_DS3234
#include <SparkFunDS3234RTC.h>  //https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library/archive/master.zip
#define DS3234_CS_PIN 10
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
boolean onTrackDec = false;

long    maxRate = MaxRate*16L;
double  slewSpeed = 0;
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
// for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianE = 60L;
// as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianW = 60L;
int    minAlt;                                     // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                     // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)
bool   autoMeridianFlip = false;                   // automatically do a meridian flip and continue when we hit the MinutesPastMeridianW

// Globals for rotator/de-rotator ------------------------------------------------------------------------------------------
#ifdef ROTATOR_ON
bool deRotate        = false;
bool deRotateReverse = false;
long posAxis3        = 0;                          // rotator position in steps
fixed_t targetAxis3;                               // rotator goto position in steps
fixed_t amountRotateAxis3;                         // rotator movement per 0.01/s
long axis3Increment  = 1;                          // rotator increment for manual control
unsigned long axis3Ms=0;
#ifdef REVERSE_AXIS3_ON
#define AXIS3_FORWARD LOW
#define AXIS3_REVERSE HIGH
#else
#define AXIS3_FORWARD HIGH
#define AXIS3_REVERSE LOW
#endif
#endif

// Globals for focusers ----------------------------------------------------------------------------------------------------
#ifdef FOCUSER1_ON
long posAxis4        = 0;                          // focuser position in steps
fixed_t targetAxis4;                               // focuser goto position in steps
fixed_t amountMoveAxis4;                           // focuser movement per 0.01/s
long axis4Increment  = 1;                          // focuser increment for manual control
unsigned long axis4Ms=0;
#ifdef REVERSE_AXIS4_ON
#define AXIS4_FORWARD LOW
#define AXIS4_REVERSE HIGH
#else
#define AXIS4_FORWARD HIGH
#define AXIS4_REVERSE LOW
#endif
#endif

#ifdef FOCUSER2_ON
long posAxis5        = 0;                          // focuser position in steps
fixed_t targetAxis5;                               // focuser goto position in steps
fixed_t amountMoveAxis5;                           // focuser movement per 0.01/s
long axis5Increment  = 1;                          // focuser increment for manual control
unsigned long axis5Ms=0;
#ifdef REVERSE_AXIS5_ON
#define AXIS5_FORWARD LOW
#define AXIS5_REVERSE HIGH
#else
#define AXIS5_FORWARD HIGH
#define AXIS5_REVERSE LOW
#endif
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
boolean axis1Enabled = false;
#if defined(AXIS2_DISABLED_HIGH)
#define Axis2_Disabled HIGH
#define Axis2_Enabled LOW
#endif
#if defined(AXIS2_DISABLED_LOW)
#define Axis2_Disabled LOW
#define Axis2_Enabled HIGH
#endif
boolean axis2Enabled = false;

#define defaultDirAxis2EInit   1
#define defaultDirAxis2WInit   0
volatile byte defaultDirAxis2  = defaultDirAxis2EInit;
#define defaultDirAxis1NCPInit 0
#define defaultDirAxis1SCPInit 1
volatile byte defaultDirAxis1  = defaultDirAxis1NCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

boolean highPrecision = true;

#define TrackingNone             0
#define TrackingSidereal         1
#define TrackingMoveTo           2
#define TrackingSiderealDisabled 3
volatile byte trackingState      = TrackingNone;
byte abortTrackingState          = TrackingNone;
volatile byte lastTrackingState  = TrackingNone;
boolean abortSlew                = false;

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

// Guide command ------------------------------------------------------------------------------------------------------------
#define GuideRate1x        2
#define GuideRate24x       6
#define GuideRateNone      255

#define slewRate (1.0/(((double)StepsPerDegreeAxis1*(MaxRate/1000000.0)))*3600.0)
#define slewRateX (slewRate/15.0)
#define halfSlewRate (slewRate/2.0)
#define acc (slewRateX/DegreesForAcceleration)  // say 5 degrees to 240x for example = 240/5 = 48X/s
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,halfSlewRate,slewRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x half-MaxRate MaxRate

byte currentGuideRate        = GuideRate24x;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirAxis1           = 0;
char          ST4DirAxis1             = 'b';
volatile byte guideDirAxis2           = 0;
char          ST4DirAxis2             = 'b';

volatile double   guideTimerRateAxis1 = 0.0;
volatile double   guideTimerRateAxis2 = 0.0;

double  guideTimerBaseRate = 0;
fixed_t amountGuideAxis1;
fixed_t guideAxis1;
fixed_t amountGuideAxis2;
fixed_t guideAxis2;

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

// sound/buzzer
#ifdef DEFAULT_SOUND_ON
boolean soundEnabled = true;
#else
boolean soundEnabled = false;
#endif
volatile int buzzerDuration = 0;

// pause at home on meridian flip
boolean pauseHome = false;            // allow pause at home?
boolean waitingHomeContinue = false;  // set to true to stop pause
boolean waitingHome = false;          // true if waiting at home

// reticule control
#ifdef RETICULE_LED_PINS
int reticuleBrightness=RETICULE_LED_PINS;
#endif

// backlash control
volatile int backlashAxis1  = 0;
volatile int backlashAxis2  = 0;
volatile int blAxis1        = 0;
volatile int blAxis2        = 0;

// EEPROM Info --------------------------------------------------------------------------------------------------------------
// 0-1023 bytes
// general purpose storage 0..99

#define EE_posAxis1    0      // 4
#define EE_posAxis2    4      // 4
#define EE_pierSide    8      // 1
#define EE_parkStatus  9      // 1
#define EE_parkSaved   10     // 1

#define EE_currentSite 11     // 1

#define EE_sgSgtAxis1  12     // 1
#define EE_sgSgtAxis2  13     // 1

#define EE_LMT         14     // 4
#define EE_JD          18     // 4

#define EE_pulseGuideRate 22  // 1
#define EE_maxRate     23     // 2

#define EE_autoMeridianFlip 25 // 1

#define EE_dfCor       26     // 4

#define EE_trueAxis1   30     // 4
#define EE_trueAxis2   34     // 4

#define EE_dpmE        38     // 1
#define EE_dpmW        39     // 1
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
#define EE_pecRecorded 71     // 1

#define EE_sgLimitAxis1 72    // 2
#define EE_sgLimitAxis2 74    // 2

#define EE_wormSensePos 76    // 4

#define EE_backlashAxis1 80   // 4
#define EE_backlashAxis2 84   // 4
#define EE_siderealInterval 88  // 4
#define EE_onTrackDec  92     // 1
#define EE_pauseHome   93     // 1 +2

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

// PEC table: 200...PECBufferSize+200-1
// PECBufferSize table of byte sized integers -128..+127, units are steps

#define EE_pecTable 200

// Library
// Catalog storage starts at 200+PECBufferSize and fills EEPROM

void setup() {
  // set initial values for some variables
  Init_Startup_Values();

  // set pins for input/output as specified in Config.h and PinMap.h
  Init_Pins();

  // if this is the first startup set EEPROM to defaults
  Init_EEPROM_Values();
  
  // this sets up the sidereal timer and tracking rates
  siderealInterval=EEPROM_readLong(EE_siderealInterval); // the number of 16MHz clocks in one sidereal second (this is scaled to actual processor speed)
  SiderealRate=siderealInterval/StepsPerSecondAxis1;
  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;

  // backlash takeup rates
  TakeupRate=round(SiderealRate/1.1);
  timerRateBacklashAxis1=SiderealRate/BacklashTakeupRate;
  timerRateBacklashAxis2=(SiderealRate/BacklashTakeupRate)*timerRateRatio;

  // now read any saved values from EEPROM into varaibles to restore our last state
  Init_ReadEEPROM_Values();

  SetTrackingRate(default_tracking_rate);
  SetDeltaTrackingRate();

  // starts the hardware timers that keep sidereal time, move the motors, etc.
  Init_Start_Timers();

  // get ready for serial communications
  PSerial.begin(9600); // for Tiva TM4C the serial is redirected to serial5 in serial.ino file
#ifdef SERIAL1_BAUD_DEFAULT
  PSerial1.begin(SERIAL1_BAUD_DEFAULT);
#else
  PSerial1.begin(9600);
#endif
#ifdef SER4_AVAILABLE
#ifdef SERIAL4_BAUD_DEFAULT
  Serial4.begin(SERIAL4_BAUD_DEFAULT);
#else
  Serial4.begin(9600);
#endif
#endif
 
  // autostart tracking
#if defined(AUTOSTART_TRACKING_ON) && (defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORKALT))
  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
  setHome();

  // start tracking
  trackingState=TrackingSidereal;
  EnableStepperDrivers();
#endif

  // prep counters (for keeping time in main loop)
  cli(); siderealTimer=lst; guideSiderealTimer=lst; PecSiderealTimer=lst; sei();
  housekeepingTimer=millis()+1000UL; 
  last_loop_micros=micros();
#ifdef ROTATOR_ON
  axis3Ms=millis()+(unsigned long)MaxRateAxis3;
#endif
#ifdef FOCUSER1_ON
  axis4Ms=millis()+(unsigned long)MaxRateAxis4;
#endif
#ifdef FOCUSER2_ON
  axis5Ms=millis()+(unsigned long)MaxRateAxis5;
#endif
}

void loop() {

  // GUIDING -------------------------------------------------------------------------------------------
  ST4();
  if ((trackingState!=TrackingMoveTo) && (parkStatus==NotParked)) Guide();

#ifndef MOUNT_TYPE_ALTAZM
  // PERIODIC ERROR CORRECTION -------------------------------------------------------------------------
  if ((trackingState==TrackingSidereal) && (parkStatus==NotParked) && (!((guideDirAxis1 || guideDirAxis2) && (activeGuideRate>GuideRate1x)))) { 
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
      EEPROM.update(EE_pecTable+pecAutoRecord,pecBuffer[pecAutoRecord]);
    }
#endif

    // FLASH LED DURING SIDEREAL TRACKING
    if (trackingState==TrackingSidereal) {
#ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { digitalWrite(LEDnegPin,HIGH); LED_ON=false; } else { digitalWrite(LEDnegPin,LOW); LED_ON=true; } }
#endif
    }

    // SIDEREAL TRACKING DURING GOTOS
    // keeps the target where it's supposed to be while doing gotos
    if (trackingState==TrackingMoveTo) {
      moveTo();
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
    }

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    // do de-rotate movement
    if (deRotate && (trackingState==TrackingSidereal)) {
      targetAxis3.fixed+=amountRotateAxis3.fixed;
      double f=(long)targetAxis3.part.m; f/=(double)StepsPerDegreeAxis3;
      if ((f<(double)MinAxis3) || (f>(double)MaxAxis3)) { deRotate=false; amountRotateAxis3.fixed=0; }
    }
#endif
#if defined(FOCUSER1_ON)
    // do automatic movement
    targetAxis4.fixed+=amountMoveAxis4.fixed;
    { double f=(long)targetAxis4.part.m; f/=(double)StepsPerMicrometerAxis4;
    if ((f<(double)MinAxis4*1000.0) || (f>(double)MaxAxis4*1000.0)) amountMoveAxis4.fixed=0; }
#endif
#if defined(FOCUSER2_ON)
    // do automatic movement
    targetAxis5.fixed+=amountMoveAxis5.fixed;
    { double f=(long)targetAxis5.part.m; f/=(double)StepsPerMicrometerAxis5;
    if ((f<(double)MinAxis5*1000.0) || (f>(double)MaxAxis5*1000.0)) amountMoveAxis5.fixed=0; }
#endif

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
    BBSpi.begin(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
    faultAxis1=TMC2130_error();
    BBSpi.end();
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
    BBSpi.begin(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
    faultAxis2=TMC2130_error();
    BBSpi.end();
  }
#endif
#if defined(MODE_SWITCH_BEFORE_SLEW_SPI) && defined(STALL_GUARD_ON)
  tmcAxis1.sgUpdateResult();
  tmcAxis2.sgUpdateResult();
  static int sgCountDown=200;
  if (trackingState==TrackingSidereal) {
    if (sgCountDown>0) sgCountDown--; else {
      // stallGuard, update SG_RESULTs, stop the slew if we hit the slew limit (0)
      bool sgFaultAxis1=false; if ((EEPROM_readInt(EE_sgLimitAxis1)>0) && (tmcAxis1.sgResult<EEPROM_readInt(EE_sgLimitAxis1))) sgFaultAxis1=true;
      bool sgFaultAxis2=false; if ((EEPROM_readInt(EE_sgLimitAxis2)>0) && (tmcAxis2.sgResult<EEPROM_readInt(EE_sgLimitAxis2))) sgFaultAxis2=true;
      if (sgFaultAxis1 || sgFaultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }
    }
  } else {
    sgCountDown=200;
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

unsigned long tempMs;

  // ROTATOR/DEROTATOR ---------------------------------------------------------------------------------
#ifdef ROTATOR_ON
  tempMs=millis();
  if ((long)(tempMs-axis3Ms)>0) {
    axis3Ms=tempMs+(unsigned long)MaxRateAxis3;

    if ((posAxis3<(long)targetAxis3.part.m) && (posAxis3<((double)MaxRot*(double)StepsPerDegreeAxis3))) {
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3++;
    }
    if ((posAxis3>(long)targetAxis3.part.m) && (posAxis3>((double)MinRot*(double)StepsPerDegreeAxis3))) {
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3--;
    }
  }
#endif

  // FOCUSER1 -------------------------------------------------------------------------------------------
#ifdef FOCUSER1_ON
  tempMs=millis();
  if ((long)(tempMs-axis4Ms)>0) {
    axis4Ms=tempMs+(unsigned long)MaxRateAxis4;

    if ((posAxis4<(long)targetAxis4.part.m) && (posAxis4<((double)MaxAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4++;
    }
    if ((posAxis4>(long)targetAxis4.part.m) && (posAxis4>((double)MinAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4--;
    }
  }
#endif

  // FOCUSER2 -------------------------------------------------------------------------------------------
#ifdef FOCUSER2_ON
  tempMs=millis();
  if ((long)(tempMs-axis5Ms)>0) {
    axis5Ms=tempMs+(unsigned long)MaxRateAxis5;

    if ((posAxis5<(long)targetAxis5.part.m) && (posAxis5<((double)MaxAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5++;
    }
    if ((posAxis5>(long)targetAxis5.part.m) && (posAxis5>((double)MinAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5--;
    }
  }
#endif

  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  tempMs=millis();
  if ((long)(tempMs-housekeepingTimer)>0) {
    housekeepingTimer=tempMs+1000UL;

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    // calculate new de-rotation rate if needed
    if (deRotate && (trackingState==TrackingSidereal)) {
      double h,d;
      getApproxEqu(&h,&d,true);
      double pr=ParallacticRate(h,d)*(double)StepsPerDegreeAxis3; // in steps per second
      if (deRotateReverse) pr=-pr;
      amountRotateAxis3.fixed=doubleToFixed(pr/100.0);            // in steps per 1/100 second
    }
#endif

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
    
#if defined(PPS_SENSE_ON) || defined(PPS_SENSE_PULLUP)
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

    // SAFETY CHECKS, keeps mount from tracking past the meridian limit, past the UnderPoleLimit, or past the Dec limits
    if (meridianFlip!=MeridianFlipNever) {
      if (pierSide==PierSideWest) {
        cli(); long p1=posAxis1+indexAxis1Steps; sei();
        if (p1>(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L)) {
          // do an automatic meridian flip and continue if just tracking
          // checks: enabled && not too far past the meridian (doesn't make sense) && not in inaccessible area between east and west limits && finally that a slew isn't happening
          if (autoMeridianFlip && (p1<(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L+(1.0/60.0)*(long)StepsPerDegreeAxis1)) && (p1>(-minutesPastMeridianE*(long)StepsPerDegreeAxis1/4L)) && (trackingState!=TrackingMoveTo)) {
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
      if (pierSide==PierSideEast) { cli(); if (posAxis1+indexAxis1Steps>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei(); }
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
    processCommands();
  }
}

