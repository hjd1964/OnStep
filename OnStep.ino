/*
 * Title       On-Step
 * by          Howard Dutton
 *
 * Copyright (C) 2012 to 2015 Howard Dutton
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
 * Revision History
 *
 * Date                Version           Comment
 * 08-12-2012          0.1               Basic function, implemented ISR, moves steppers around
 * 09-15-2012          0.5               First working version, no PEC or align
 * 11-12-2012          0.90              PEC and one star align implemented
 * 11-30-2012          0.92              Working and tested except for PEC and 2/3 star align
 * 02-09-2013          0.94              PEC appears to be working, still need to test 2/3 star align
 * 02-21-2013          0.96              Support for Mega-2560/two channel serial control, ISR overhaul and fixed timing related problems
 * 02-25-2013          0.98              Optional outgoing checksum for both serial channels
 * 02-27-2013          0.99a1            Support for differing RA/Dec gear ratios, Timer0 disabled and millis() replaced
 * 03-31-2013          0.99a2            Optional checksum for both serial channels is now bidirectional, delay() replaced
 *                                       Corrected a logic bug in the PEC routine.  MinAlt was limited to 0-90 range, -30 to 30 deg range is
 *                                       now used, storage format adjusted. TimeZone storage format was adjusted slightly, issue an :SG??# before use
 *                                       Park delays adjusted for quicker operation
 *                                       PEC improvements, new command to clear PEC buffer, PEC recording averages 2:1 favoring the buffer,
 *                                       PEC playback is now adjusted to be one second prior to the recorded value in the sequence
 *                                       Added new PEC readout mode for live visualization
 * 04-04-2013          0.99a3            Sidereal tracking for various StepsPerSecond is now automatic and no longer requires programming for setup
 * 04-09-2013          0.99a4            PEC correction for working in the eastern sky.  Re-enabled Timer0, tracking rates are now
 *                                       based on Timer0 with the hundreth/second sidereal clock synced to it.  Fixed minor bug in Sync command,
 *                                       now works correctly in the eastern sky
 * 05-03-2013          0.99a5            Minor command-set changes for better LX200 protocol compatibility
 * 05-10-2013          0.99a6            Command processing code clean-up from above command-set changes, support for "ACK - Alignment Query" command on Mega-2560 only
 * 06-05-2013          0.99a7            Added commands :WR+# and :WR-#, to shift the (in static memory) PEC table ahead or back a second.
 *                                       Corrected bug, ID index/offset value needs to be reversed with Meridian flip.
 *                                       Corrected bug, mount won't park properly with backlash values set.  Incorrectly calculated delay (int overflow).
 *                                       Added command :GXnn# to return various internal values from OnStep.  ID and IH are the only ones defined so far.
 *                                       Added ability to supply 5V power to the logic side of stepper drivers that lack onboard regulators.
 * 06-09-2013          0.99a8            Corrected bug, when a meridian flip will be required, Declination of the goto needs a correction applied 
 *                                       (for ID index/offset) before the goTo function is called.  
 *                                       Cleanup of direction changing code in ISR, faster and smaller code.
 * 09-01-2013          0.99a9            Declination movement was in the wrong direction for my setup, reversed CLR(); else SET(); in Timer.ino 
 * 09-21-2013          0.99a10           Fixes to initialization code, EEPROM reads should have been writes.
 * 10-14-2013          0.99a11           Fixed HA/RA and Dec ability to reverse direction in initialization.
 * 11-14-2013          0.99b1            Fixed "creep" in HA/RA and Dec during guiding and backlash compensation.  Also fixed tracking oscillation in HA/RA at
 *                                       certain guide rates (sidereal tracking and guiding interacting with backlash compensation).  Improved method of applying
 *                                       stepper motor high-speed takeup during backlash compensation.  Added option to easily adjust backlash takeup rate.
 * 11-22-2013          0.99b2            Minor DEBUG mode fixes.
 * 11-22-2013          0.99b3            Minor DEC_RATIO fixes.
 * 11-23-2013          0.99b4            MinAlt was being written to EEPROM incorrectly on set. MasterSiderealInterval should have been initialized only once.
 * 12-15-2013          0.99x1            Now uses Timer3 and Timer4 to control HA/Dec step rates.  Variables that hold/control slew rates expanded from int to long.
 *                                       Recalculation of various slew rates and sidereal clock rate.  I still would like to write my own Serial port code since
 *                                       commands being sent/recieved by the Arduino library code disturbs the stepping pulses.
 * 12-23-2013          0.99x2            Removed support for atmega328 and obsolete "debug mode" code.  Various sidereal timing fixes.  Serial communication code no-longer
 *                                       uses Arduino libraries for smaller size and higher performance.
 * 12-29-2013          0.99x3            Renamed variables to better reflect their use.
 * 01-16-2014          0.99x4            Changes to setPark, faster backlash takeup and compatibility with 32 uStep stepper motor drivers
 * 01-24-2014          0.99x5            Fixed references for reversing HA/Dec directions, added white-space striping to routines in Astro.ino, 
 *                                       serial1 no-longer ends replys with /r/n in Command.ino 
 * 01-28-2014          0.99x6            Fixed bug in serial1 transmit function *
 * 04-17-2014          0.99x7            Fixed successful syncEqu() undefined return
 * 04-21-2014          0.99x8            Set-park delay adjusted to allow for larger backlash values
 * 04-24-2014          0.99x9            Fixed minor bug in :VW# command (PEC stepsPerWormRotation)
 * 05-02-2014          0.99x10           Fixed bug when parking near the meridian and cleaned up serial protocol for better compatability
 * 05-04-2014          0.99x11           LX200 Protocol fix, commands returning 1/0 success/failure (or numeric status, for example :MS#) now omit the '#' framing character
 *                                       this requires updated ASCOM drivers and Hand controller App but brings better compatibility with LX200 protocol software
 * 05-05-2014          0.99x12           Added fast PEC readout command ":VrNNNN#"
 * 05-22-2014          0.99x13           Added guiding to status command ":GU#"
 * 05-29-2014          0.99x14           Added feature. First-time uploads of OnStep will burn defaults into EEPROM automatically now
 *                                       *** this will overwrite your parking info, goto limits, etc. (once) when upgrading to this version unless you set INIT_KEY to true  ***
 *                                       *** if upgrading from a prior version, set INIT_KEY to true and uploaded then back to false and upload again                        ***
 *                                       *** those uploading for the first time or upgrading from the main branch should just leave INIT_KEY alone (false)                   ***
 * 06-05-2014          0.99x15           Fixed RESCUE_MODE code, thanks to N_DD for pointing this out.
 * 06-20-2014          0.99b16           Merged Paul Stoffregen's Teensy3.1 support code
 * 06-20-2014          0.99b17           Removed redundant legacy clock sync code, fixed T+/T- commands writing long SiderealInterval as an int into EEPROM
 *                                       reworked interrupt initialization code, added Paul's suggested interrupt prioritization code
 *                                       added 2uS delay to Timer3/4 ISR's to keep stepper drivers, etc. in spec. also switched to fixed prescaler in Timer3/4 on AVR
 *                                       fixed abortSlew bug in MoveTo.  changed pins for RA stepper on Teensy.  added support for LX200 like commands to get/set 
 *                                       tracking rate (this very slightly breaks OnStep's ASCOM driver and requires driver v1.16 to work properly.)
 * 06-23-2014          0.99b18           Refinement of tracking commands in Command.ino, fixed Park/Unpark bug (int temporary storage should be long) in Park.ino and OnStep.ino
 * 06-23-2014          0.99b19           Fixes to Timer.ino and OnStep.ino to reduce jitter on Teensy3.1
 * 07-22-2014          1.0a1             Changed logic of tracking rate command :TQ#, now only stores the custom sidereal rate (and any changes +/-) in EEPROM
 * 08-04-2014          1.0a2             Switched to running the motor timers at the sidereal rate. Support added for goto/operation under the celestial pole
 *                                       Safety check code, should keep mount from tracking past the meridian limit, below horizon limit, or above the overhead limit
 * 09-07-2014          1.0a3             Getting blank site names causes problems so "None" is now returned for empty site names. Added commands to return backlash values.
 * 09-08-2014          1.0a4             Code to allow operation in the southern hemisphere (declination of the "celestial pole" changes between NCP +90 and SCP -90 based on latitude.)
 *                                       RA motor direction is also reversed verses north latitude setting.  Added code to stop Gotos with :Q# command.
 * 09-29-2014          1.0a5             When ready to start/resume playing PEC, the playback rate is now raised so the motors quickly arrive at the new position in the playback sequence.
 *                                       This is needed due to the changes made in 1.0a2  Goto minimum rate is now 4x sidereal, makes more sense to stay a little fast as we approach the target.
 *                                       Cleaned up code, renamed some variables, fixed backlash/tracking rate problem, and added some comments in Timer.ino  
 *                                       Fixed goto meridian flip logic problem and generally cleaned up the goto initiation code.
 *                                       Fixed altitude calculation used for limits, horizon and overhead limits now stop tracking.
 * 10-01-2014          1.0a6             Numerous performance related tweeks.  Fixed gotos that involve a meridian flip, they now arrive closer to their destination.
 *                                       underPoleLimit can now be set to values other than the default of 9 hours. Added safety code to stop the mount when tracking past this limit
 * 10-04-2014          1.0a7             Fixes to recently added code to keep 'scope from exceeding safety limits during meridian flips.  Added experimental align model code for cone error.
 * 10-06-2014          1.0a8             First successful test of two star align, compensates for Polar altitude misalignment.  Simple change keeps GetEqu (lots of floating point trig) from needing 
 *                                       to be run twice when :GR# and :GD# are run and/or more than once a second during goto's.
 * 10-10-2014          1.0b1             Added :AW# command to write align model to EEPROM without having to do a Set Park.  Added doCor and pdCor alignment coefficients to values saved in EEPROM.
 *                                       Reworked/added the PD and DO alignment model calculations. Refined code that determines coefficient values during the alignment process
 * 10-14-2014          1.0b2             More pointing model refinement.  Added code to correct destination coordinates when doing meridian flips.  
 *                                       Changed code that reads model coefficients (:GX#), added code to write (:SX#) them.
 * 10-17-2014          1.0b3             Added code to disable the stepper drivers until Align is started and while Parked:  Mega2650 uses Pin 25 for RA and 29 for Dec. (Teensy3.1 Pin 16 Dec and 20 RA)
 *                                       Added code to set stepper driver ustep mode:  Mega2560 uses Pins 22,23,24 for M0,M1,M2 (RA) and Pins 26,27,28 for Dec (Teensy3.1 13,14,15 for Dec and 17,18,19 for RA)
 *                                       Added code to support limit switch on pin 3
 * 10-29-2014          1.0b4             Updated step generation code for sidereal tracking and guiding, allows better performance and non-integral StepsPerSecond rates
 *                                       Finished code to enable dynamic tracking rate compensation for refraction.  Added code to allow use of PEC index sense feature
 *                                       New commands :Te# :Td# turn refraction tracking on/off, :VH# reads PEC HALL sensor position in seconds
 * 10-31-2014          1.0b5             Added code for GPS PPS sync. and Timer1 now runs at clock /3 instead of /8 for higher accuracy
 * 11-06-2014          1.0b6             Changes to Timer.ino improve performance, lower timing jitter.  Improvements/fixes to guiding function.  Added PPS lock status to command :GU#.
 *                                       Moved configuration to a header file, cleaned up source code a bit.
 * 11-08-2014          1.0b7             ParkClearBacklash now uses the BacklashTakeupRate, doubled takeup time to fix bug.  Guide function now only "moves" once backlash takeup is finished.
 *                                       Switching SiderealClock down to a slightly less accurate /8 rate during gotos to save MPU cycles.  Adjusted PEC to temporarily fall out of play mode when
 *                                       guiding at >1x sidereal.  Cleaned up source code a bit more.  Fixed stepper driver mode/enable/fault pin assignments.
 * 12-09-2014          1.0b8             Added guide rate 0 at 0.25x the sidereal rate.  Rearranged Timer.ino so the Teensy3.1 should work with the micro-step mode switching feature.
 * 01-03-2015          1.0b9             Added PEC analog sensor index support.  Fixes to PEC, record now waits for beginning of buffer to start and PEC buffer overflow protection added. 
 *                                       PEC now stays active if position in buffer is lost when index support is turned on since it can recover from this.
 *                                       Added command to get the current pulseguide rate.  Added (optional) seperate pulse guide rate with memory.  Minor fix to smooth out guide after travel through backlash.
 * 01-13-2015          1.0b10            MaxRate can now be set by updated ASCOM driver, OnStep can optionally remember this setting when it changes.
 * 02-06-2015          1.0b11            Corrected sidereal rate bug, improvements to guiding responsiveness, status LEDS now provide more feedback.
 * 03-08-2015          1.0b12            Abandoned step rate generation logic method in favor of fixed point math for rate calculations.  Hopefully fixed Teensy3.1 micro-step mode switch during goto problems. 
 *                                       Initial support for fork mounts: polar home position HA=0, mode w/meridian flips completely disabled, mode with meridian flips enabled until align is complete.
 * 03-26-2015          1.0b13            Minor fix so OnStep remembers the MaxRate set by the ASCOM driver (when this is enabled in config.h) 
 * 03-30-2015          1.0b14            Fixes for goto micro-step mode switch on Mega2560 and PEC soft index detect. PEC Clear command now clears hard index too.
 * 04-13-2015          1.0b15            Fixes startup initialization of polar home position for southern hemisphere users
 * 04-28-2015          1.0b16            Added support for user defined object libraries.  There are up to 15 seperate libraries of which 14 are set aside for user object lists.  The total number of objects that
 *                                       can be stored depends on EEPROM size and PEC table size.  With the default 824 byte PEC table, the Mega2560 can store 192 objects and the Teensy3.1 can store 64.  
 *                                       Commands are :Lon# select catalog n, :LB# move back, :LN# move next, :LCn# move to catalog record n, :LI# get object info, :LW write object info, :LL# clear catalog, #L!# clear library.
 *                                       Fixed bug, 3-star align was disabling meridian flips for GEM mounts.
 * 05-14-2015          1.0b17            Fixed bug, Latitude/Longitude/UT/LMT are now correctly stored in EEPROM on Teensy3.1.  
 *                                       Improved user defined object library record format: object names can now be 11 chars max (was 7.)
 * 05-18-2015          1.0b18            Added experimental support of ST4 interface.  
 * 06-05-2015          1.0b19            Fixed point math improvements: smaller more elegant code, should be faster too.  Added support for disabling stepper drivers with a high or low signal.
 * 07-14-2015          1.0b20            Minor code corrections to fix warnings during Teensy3.1 compile.  Re-arranged altitude limits calculation so it completes once a second.
 * 07-17-2015          1.0b21            Added code that makes more intelligent decisions about when to use an RA "waypoint" back to the polar home position during meridian flips to avoid exceeding 
 *                                       horizon limits (makes most meridian flips faster.) Compatibility fixes to EEPROM_writeInt and EEPROM_readInt for Teensy3.1 target.  Support for up to 256X micro-stepping.
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
 
#include "EEPROM.h"
#include "math.h"
#include "errno.h"
// Use Config.h to configure OnStep to your requirements 
#include "Config.h"
#include "Library.h"
#include "FPoint.h"

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "07 14 15"
#define FirmwareNumber "1.0b20"
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
int timeZone = 0;                    //

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

double HzCf = 16000000.0/60.0;       // conversion factor to go to/from Hz for tracking rate commands

volatile long SiderealRate;          // based on the siderealInterval, this is the time between steps for sidereal tracking
volatile long TakeupRate;            // this is the takeup rate for synchronizing the target and actual positions when needed

boolean customRateActive=true;       // automatically modify the siderealInterval to compensate for atmospheric refraction

// PPS (GPS)
volatile unsigned long PPSlastMicroS = 1000000;
volatile unsigned long PPSavgMicroS = 1000000;
volatile double PPSrateRatio = 1.0;
volatile boolean PPSsynced = false;

// Tracking and rate control
#ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
boolean refraction = true;           // track at the refraction rate for the area of the sky
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

volatile double  pecTimerRateHA = 0;
volatile double  guideTimerRateHA = 0;
volatile double  guideTimerRateDec = 0;
volatile double  timerRateRatio = ((double)StepsPerDegreeHA/(double)StepsPerDegreeDec);
#define SecondsPerWormRotation  ((long)(StepsPerWormRotation/StepsPerSecond))
#define StepsPerSecondDec       ((double)(StepsPerDegreeDec/3600.0)*15.0)

#if defined(__arm__) && defined(TEENSYDUINO)
IntervalTimer itimer3;
void TIMER3_COMPA_vect(void);

IntervalTimer itimer4;
void TIMER4_COMPA_vect(void);
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
long celestialPoleDec = 90L;

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

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

// I set the pin usage to facilitate easy connection of jumper cables
// for now, the #defines below are used to program the port modes using the standard Arduino library

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   328  | 2560
#define LEDposPin  8       // Pin 8 (LED)   PB0    PH5
#define LEDnegPin  9       // Pin 9 (GND)   PB1    PH6
#define LEDneg2Pin 10      // Pin 10 (GND)  P??    PB4

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  4       // Pin 4 (Dir)   PD4    PG5
#define Dec5vPin   5       // Pin 5 (5V?)   PD5    PE3
#define DecStepPin 6       // Pin 6 (Step)  PD6    PH3
#define DecGNDPin  7       // Pin 7 (GND)   PD7    PH4

// defines for direct port control
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
// The Arduino attachInterrupt function works in two modes, on the '2560 it takes an Interrupt# on the Teensy and others it takes a Pin#
#define PpsInt     2       // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

#define HADirPin   11      // Pin 11 (Dir)  PB3    PB5
#define HA5vPin    12      // Pin 12 (5V?)  PB4    PB6
#define HAStepPin  13      // Pin 13 (Step) PB5    PB7
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
#define ST4RAw     47      // Pin 47 ST4 RA- West
#define ST4DEs     49      // Pin 49 ST4 DE- South
#define ST4DEn     51      // Pin 51 ST4 DE+ North
#define ST4RAe     53      // Pin 53 ST4 RA+ East

#elif defined(__arm__) && defined(TEENSYDUINO)

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
#define LEDneg2Bit  3      // Pin 3
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
volatile byte lastTrackingState = TrackingNone;
boolean abortSlew        = false;

#define MeridianFlipNever  0
#define MeridianFlipAlign  1
#define MeridianFlipAlways 2
byte meridianFlip = MeridianFlipAlways;

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

// Misc ---------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

int baudRate[10] = {0,56700,38400,28800,19200,14400,9600,4800,2400,1200};

// current site index and name
byte currentSite = 0; 
char siteName[16];

// align command
double altCor            = 0;       // for geometric coordinate correction/align, - is below the pole, + above
double azmCor            = 0;       // - is right of the pole, + is left
double doCor             = 0;       // declination/optics orthogonal correction
double pdCor             = 0;       // declination/polar orthogonal correction
double IH                = 0;       // offset corrections/align
double ID                = 0;

// tracking and PEC, fractional steps
fixed_t fstep;
fixed_t pstep;

// guide command
#define GuideRate1x        2
#define GuideRate16x       6
#define GuideRateNone      255
double  guideRates[10]={3.75,7.5,15,30,60,120,240,360,600,900}; 
//                      .25X .5x 1x 2x 4x  8x 16x 24x 40x 60x
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
//
long lstGuideStopHA=0;
long lstGuideStopDec=0;

// PEC control
#define PECStatusString  "IpPrR"
#define IgnorePEC        0
#define ReadyPlayPEC     1
#define PlayPEC          2
#define ReadyRecordPEC   3
#define RecordPEC        4
long    accPecGuideHA    = 0;        // for PEC, buffers steps to be recorded
boolean PECfirstRecord   = false;
boolean PECstatus        = IgnorePEC;
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
// initialize some fixed-point values
  amountGuideHA.fixed=0;
  amountGuideDec.fixed=0;
  fstep.fixed=0;
  pstep.fixed=0;
  origTargetHA.fixed = 0;
  targetHA.part.m = 90L*StepsPerDegreeHA;
  targetHA.part.f = 0;
  targetDec.part.m = 90L*StepsPerDegreeDec;
  targetDec.part.f = 0;

  fstep.fixed=doubleToFixed(StepsPerSecond/100.0);
  
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
  timerRateHA     =SiderealRate;
  timerRateDec    =SiderealRate;

  // backlash takeup rates
  timerRateBacklashHA =timerRateHA /BacklashTakeupRate;
  timerRateBacklashDec=timerRateDec/BacklashTakeupRate;

  // initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);
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
#endif

  // get ready for serial communications
  Serial1_Init(9600);
  Serial_Init(9600);
  
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite);  if (currentSite>3) currentSite=0; // site index is valid?
  float f; EEPROM_readQuad(EE_sites+(currentSite)*25+0,(byte*)&f); latitude=f;
  if (latitude<0) celestialPoleDec=-90L; else celestialPoleDec=90L;
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (celestialPoleDec>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
  EEPROM_readQuad(EE_sites+(currentSite)*25+4,(byte*)&f); longitude=f;
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);

  // update starting coordinates to reflect NCP or SCP polar home position
  startHA = celestialPoleHA*StepsPerDegreeHA;
  startDec = celestialPoleDec*StepsPerDegreeDec;
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
  if (trackingState==TrackingSidereal) { 

    // ST4 INTERFACE -------------------------------------------------------------------------------------
    #if defined(ST4_ON) || defined(ST4_PULLUP)
    if (parkStatus==NotParked) {
      if (digitalRead(ST4RAw)==LOW) ST4RA_state='w'; else if (digitalRead(ST4RAe)==LOW) ST4RA_state='e'; else ST4RA_state=0;
      if (digitalRead(ST4DEn)==LOW) ST4DE_state='n'; else if (digitalRead(ST4DEs)==LOW) ST4DE_state='s'; else ST4DE_state=0;
      // RA changed?
      if (ST4RA_last!=ST4RA_state) {
        ST4RA_last=ST4RA_state;
        if (ST4RA_state) { 
          #ifdef SEPERATE_PULSE_GUIDE_RATE_ON
          enableGuideRate(currentPulseGuideRate);
          #else
          enableGuideRate(currentGuideRate);
          #endif
          guideDirHA=ST4RA_state;
          guideDurationHA=-1;
          cli(); if (guideDirHA=='e') guideTimerRateHA=-guideTimerRate; else guideTimerRateHA=guideTimerRate; sei();
        } else {
          if (guideDirHA) { lstGuideStopHA=lst+3; guideDirHA=0; }
        }
      }
      // Dec changed?
      if (ST4DE_last!=ST4DE_state) {
        ST4DE_last=ST4DE_state;
        if (ST4DE_state) { 
          #ifdef SEPERATE_PULSE_GUIDE_RATE_ON
          enableGuideRate(currentPulseGuideRate);
          #else
          enableGuideRate(currentGuideRate);
          #endif
          guideDirDec=ST4DE_state;
          guideDurationDec=-1;
          cli(); guideTimerRateDec=guideTimerRate; sei();
        } else {
          if (guideDirDec) { lstGuideStopDec=lst+3; guideDirDec=0; }
        }
      }
    }
    #endif

    guideHA=0;
    Guide();
  }
  
  // PERIODIC ERROR CORRECTION -------------------------------------------------------------------------
  if ((trackingState==TrackingSidereal) && (!((guideDirHA || guideDirDec) && (activeGuideRate>GuideRate1x)))) { 
    // only active while sidereal tracking with a guide rate that makes sense
    Pec();
  } else disablePec();

  // SIDEREAL TRACKING ---------------------------------------------------------------------------------
  cli(); long tempLst=lst; sei();
  if (tempLst!=siderealTimer) {
    siderealTimer=tempLst;
    
    // only active while sidereal tracking with a guide rate that makes sense
    if ((trackingState==TrackingSidereal) && !(guideDirHA && (activeGuideRate>GuideRate1x))) {
      // apply the Tracking, Guiding, and PEC
      cli();
      targetHA.fixed+=fstep.fixed;
      if (pecTimerRateHA!=0) targetHA.fixed+=pstep.fixed;
      sei();

      #ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { SET(LEDnegPORT,LEDnegBit); LED_ON=false; } else { CLR(LEDnegPORT,LEDnegBit); LED_ON=true; } }
      #endif
    }

    if (trackingState==TrackingMoveTo) {
      // keeps the target where it's supposed to be while doing gotos
      if (lastTrackingState==TrackingSidereal) { origTargetHA.fixed+=fstep.fixed; cli(); targetHA.fixed+=fstep.fixed; sei();  }
      moveTo();
    }
  }

  // figure out the current Alititude
  if (lst%5==0) do_alt_calc(); // call 20x a second to spread out the overhead

  // HOUSEKEEPING --------------------------------------------------------------------------------------
  // timer... falls in once a second, keeps the universal time clock ticking,
  // handles PPS GPS signal processing, watches safety limits, adjusts tracking rate for refraction
  unsigned long m=millis();
  if ((long)(m-(clockTimer+999UL))>0) {
    clockTimer=m;
    
    // for testing, average steps per second
    // debugv1=(debugv1*19+(targetHA*1000-lastTargetHA))/20;
    // lastTargetHA=targetHA*1000;
    
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

    #ifdef PPS_SENSE_ON
    // update clock
    if (trackingState==TrackingSidereal) {
      cli();
      PPSrateRatio=((double)1000000/(double)(PPSavgMicroS));
      SetSiderealClockRate(siderealInterval);
      if ((long)(micros()-(PPSlastMicroS+2000000UL))>0) PPSsynced=false; // if more than two seconds has ellapsed without a pulse we've lost sync
      sei();
      #ifdef STATUS_LED2_PINS_ON
      if (PPSsynced) { if (LED2_ON) { SET(LEDneg2PORT, LEDneg2Bit); LED2_ON=false; } else { CLR(LEDneg2PORT, LEDneg2Bit); LED2_ON=true; } } else { SET(LEDneg2PORT, LEDneg2Bit); LED2_ON=false; } // indicate PPS
      #endif
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
      if (pierSide==PierSideWest) { cli(); if (posHA>(minutesPastMeridianW*StepsPerDegreeHA/4L)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  sei(); }}
      if (pierSide==PierSideEast) { cli(); if (posHA>(underPoleLimit*StepsPerDegreeHA*15L))      { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  sei(); }}
    } else {
      // when Fork mounted, ignore pierSide and just stop the mount if it passes the underPoleLimit
      cli(); if (posHA>(underPoleLimit*StepsPerDegreeHA*15L)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  sei(); }
    }
    if ((getApproxDec()<minDec) || (getApproxDec()>maxDec)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

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
    if ((lst_now-lst_start)>24*3600*100) update_lst();

    // finally, adjust the tracking rate on-the-fly to compensate for refraction
    if (refraction) CEquToTracRateCor();

  } else {
  // COMMAND PROCESSING --------------------------------------------------------------------------------
  // acts on commands recieved across Serial0 and Serial1 interfaces
    processCommands();
  }
}

