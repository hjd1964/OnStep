/*
 * Title       On-Step
 * by          Howard Dutton
 *
 * Copyright (C) 2013 Howard Dutton
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
 * 01-28-2014          0.99x6            Fixed bug in serial1 transmit function
 * 04-17-2014          0.99x7            Fixed successful syncEqu() undefined return
 * 04-21-2014          0.99x8            Set-park delay adjusted to allow for larger backlash values
 * 04-24-2014          0.99x9            Fixed minor bug in :VW# command (PEC stepsPerWormRotation)
 * 05-02-2014          0.99x10           Fixed bug when parking near the meridian and
 *                                       cleaned up serial protocol for better compatability
 * 05-04-2014          0.99x11           LX200 Protocol fix, commands returning 1/0 success/failure (or numeric status, for example :MS#) now omit the '#' framing character
 *                                       this requires updated ASCOM drivers and Hand controller App but brings better compatibility with LX200 protocol software
 * 05-05-2014          0.99x12           Added fast PEC readout command ":VrNNNN#"
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * Arduino Stepper motor controller for Losmandy G11 mounts (and others)
 * with basic LX200 derived command set.
 *
 * Review the comments below when setting this up for use with for your equipment.
 * a series of #define statements tailor the configuration to your hardware/mount.
 * they are clearly commented below.
 *
 * the Arduino Mega2560 uses USB for power and communication with the host computer
 *
 * the RA  BED plugs into pins Gnd,13,12, and 11 of the Arduino
 * the Dec BED plugs into pins 7, 6, 5, and 4 (7 is Gnd and the wiring is identical.)
 * RA : Gnd,13,12,11 = Gnd,Step,N/C,Direction
 * Dec: 7,6,5,4      = Gnd,Step,N/C,Direction
 *
 * I used two SparkFun Big Easy Drivers (BED) to control the bipolar stepper motors; 
 * with proper heat-sinks they can handle up to 2A per phase at 35V.
 * Each BED is setup by soldering a 4-position male .1" header into the input side.  
 * I modified the header by removing the 2nd to last pin, and bending the last pin so
 * the header fits into the Gnd,Step,Dir holes of the BED. A four wire ribbon cable from
 * Sparkfun, etc. can then be used to plug them into the Arduino.  The outputs go to the
 * two coils of each stepper motor.  If you don't know which wires are which, the 'net is
 * full of advise on determining what you have, google it and get out your multimeter.
 * Refer to my site (Equipment->OnStep) and/or read up on the 'net to get an idea of how
 * to set the potientiometer on the BEDs to match your stepper motors; too much current
 * will BURN OUT YOUR STEPPER MOTORS.
 *
 * Optionally, the status LED plugs into pins 9,8. I use an 2.2k resistor in series with the LED.
 * LED: 9,8 = Gnd,+5V
 *
 * Optionally, the HC05 bluetooth adapter plugs into pins Tx1/Rx1 and pins +5V/Gnd. I use
 * two 2-wire cables for 0.1" headers.  Remember Rx goes to Tx and Tx goes to Rx. And be
 * sure to get a HC05 (or other) module designed for 5 volt operation otherwise you will
 * have to do the level conversion hardware yourself.
 * HC05: Tx,Rx/+5V,Gnd = Rx1,Tx1/+5V,Gnd
 * 
 */
 
#include "EEPROM.h"
#include "math.h"
#include "errno.h"

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "05 05 14"
#define FirmwareNumber "0.99x12"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR ARDUINO --------------------------------------------------------------------------

// turns debugging on, used during testing
#define DEBUG_OFF
// for getting control of the 'scope when things go horribly wrong
#define RESCUE_MODE_OFF

// supply power on pins 5 and 11 to Pololu or other stepper drivers without on-board 5V voltage regulators
#define POWER_SUPPLY_PINS_OFF

// enables goTo speed equalization for differing right ascension and declination StepsPerDegreeHA/Dec
#define DEC_RATIO_OFF

// enables alignment on two or three stars
#define ALIGN_TWO_AND_THREE_STAR_ON

// enables Horizon coordinate goto functions
#define ALT_AZM_GOTO_ON

// enables code to clean-up PEC readings after record (use PECprep or a spreadsheet to fix readings otherwise)
// this cleans up any tracking rate variations that would be introduced by recording more guiding corrections to either the east or west
#define PEC_CLEANUP_OFF

// these turn on and off checksum error correction on the serial ports
#define CHKSUM0_OFF     // default _OFF: required for OnStep ASCOM driver
#define CHKSUM1_ON      // default _ON:  required for OnStep Android Handcontroller

// this initializes a host of settings in EEPROM, OnStep won't work correctly if it isn't run once and then turned off
#define INIT_OFF        // default _OFF: set to _ON the first time you upload the onstep firmware to initialize EEPROM (then turn it off again)
                        // even with this you still need to set your latitude, longitude, timezone, local time, and date (as a minimum)
                        // see the command reference on my site (stellarjourney.com)

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define MaxRate             (96)*16  // this is the minimum number of (16MHz) clocks between micro-steps (minimum is around 16, 
                                     // default is 64, higher is ok) this is where you adjust how fast your motors run during GoTos
                                     
#define StepsForRateChange  192000.0 // number of steps during acceleration and de-acceleration: higher values=longer acceleration/de-acceleration
                                     // for the most part this doesn't need to be changed, but adjust when needed

#define BacklashTakeupRate    50     // backlash takeup rate (in multipules of the sidereal rate): too fast and your motors will stall,
                                     // too slow and the mount will be sluggish while it moves through the backlash
                                     // for the most part this doesn't need to be changed, but adjust when needed

                                     // for my EM10b both RA and Dec axis have the same gear train and this
#define StepsPerDegreeHA    7680L    // is calculated as :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  24            * 32          * 25              *  144/360              = 7680
                                     // Losmandy G11     :  48            * 16          * 15              *  360/360              = 11520
#define StepsPerDegreeDec   7680L    // is calculated as :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  24            * 32          * 25              *  144/360              = 7680
                                     // Losmandy G11     :  48            * 16          * 15              *  360/360              = 11520
                                     // the EM10b has two spur gears that drive the RA/Dec worms, they are 60 tooth and 48 tooth gears
                                     // for an 1.25x reduction in addition to the 20:1 gear heads on the steppers for a 25:1 final ratio
                                     // before the worm/wheels 144:1
                                     
#define StepsPerSecond        32     // the steps per second sidereal rate = 32 = (7680/3600)*15 - OnStep can handle between 12 and 100 steps/second
                                     // when sidereal tracking (StepsPerWormRotation must be evenly divisible by StepsPerSecond)

#define StepsPerWormRotation 19200L  // PEC, number of steps for a complete worm rotation (in RA), (StepsPerDegreeHA*360)/gear_reduction2 
                                     // the EM10b has a worm-wheel with 144 teeth (7680*360)/144 = 19200

#define PECBufferSize        824     // PEC, buffer size, max should be no more than 1336, your required buffer size = StepsPerWormRotation/StepsPerSecond
                                     // for the most part this doesn't need to be changed, but adjust when needed
                                     // the EM10b needs a minimum of 600 (seconds)

#define REVERSE_HA_OFF               // Reverse the direction of movement for the HA/RA axis
#define REVERSE_DEC_OFF              // Reverse the direction of movement for the Dec axis

long minutesPastMeridian =   60;     // for goto's, how far past the meridian to allow before we do a flip - one hour of RA is the default = 60

// THAT'S IT FOR USER CONFIGURATION!
// -------------------------------------------------------------------------------------------------------------------------


// Time keeping ------------------------------------------------------------------------------------------------------------
unsigned long siderealTimer = 0;
unsigned long siderealTimer2;
unsigned long tenthSecondTimer;
unsigned long ut1Timer;
unsigned long msTimer1 = 0;
unsigned long msTimer2 = 0;
unsigned long now = 0;
long ResultA1 = 0;
long ResultA2 = 0;

double UT1       = 0.0;              // the current universal time
double UT1_start = 0.0;              // the start of UT1
unsigned long UT1mS_start = 0;       // mS at the start of UT1
double JD  = 0.0;                    // and date, used for computing LST
double LMT = 0.0;                    // internally, the date and time is kept in JD/LMT
                                     // LMT is updated to keep current time
int timeZone = 0;                    //
double LST = 0.0;                    // this is the local (apparent) sidereal time in 24 hour format (0 to <24) must be updated when accessed

volatile long lst = 0;               // this is the local (apparent) sidereal time in 1/100 seconds (23h 56m 4.1s per day = 86400 clock seconds/
                                     // 86164.09 sidereal seconds = 1.00273 clock seconds per sidereal second)
unsigned long lst_mS_start = 0;      // mS at the start of lst
long lst_start = 0;                  // the start of lst
long PECsiderealTimer = 0;           // time since worm wheel zero index for PEC

long siderealInterval  =  15956313;
long masterSiderealInterval = siderealInterval;
                                     // default = 15956313 ticks per sidereal hundredth second, this is stored in
                                     // EEPROM which is updated/adjusted with the ":T+#" and ":T-#" commands
                                     // stars moving left means too fast, right means too slow (for my 'scope optics/camera)
                                     // a higher number here means a longer count which slows down the sidereal clock...
                                     // movement is based on the sidereal clock, but SiderealRate must be low enough that
                                     // the stepping keeps pace with the targetHA for accurate tracking
                                     
                                     // keeps sidereal time, might need to be adjusted due to variations in Arduino crystal accuracy, etc.
                                     // someday I'd like to add a GPS module and grab the pps (pulse per second) signal as a high-accuracy
                                     // time reference to correct drift in my sidereal clock due to physical effects and software limitations

long SiderealRate;                   // based on the siderealInterval, this is the time between steps for sidereal tracking

volatile long timerRateHA;
volatile long timerRateBacklashHA;
volatile boolean inBacklashHA=false;
volatile long timerRateDec;
volatile long timerRateBacklashDec;
volatile boolean inBacklashDec=false;
#define timerRateRatio    ((double)StepsPerDegreeHA/(double)StepsPerDegreeDec)

// Location ----------------------------------------------------------------------------------------------------------------
double latitude  = 0.0;
double longitude = 0.0;

volatile long posHA      = 90L*StepsPerDegreeHA;   // hour angle position in steps
volatile long startHA    = 90L*StepsPerDegreeHA;   // hour angle of goto start position in steps
volatile long targetHA   = 90L*StepsPerDegreeHA;   // hour angle of goto end   position in steps
volatile byte dirHA      = 1;                      // stepping direction + or -
volatile long PEC_HA     = 0;                      // for PEC, adds or subtracts steps
double newTargetRA       = 0.0;                    // holds the RA for goTos
long origTargetHA        = 0;

volatile long posDec     = 90L*StepsPerDegreeDec; // declination position in steps
volatile long startDec   = 90L*StepsPerDegreeDec; // declination of goto start position in steps
volatile long targetDec  = 90L*StepsPerDegreeDec; // declination of goto end   position in steps
volatile byte dirDec     = 1;                     // stepping direction + or -
double newTargetDec      = 0.0;                   // holds the Dec for goTos
long origTargetDec       = 0;

double newTargetAlt=0.0, newTargetAzm=0.0;        // holds the altitude and azmiuth for slews
int    minAlt;                                    // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                    // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)

// Stepper/position/rate ----------------------------------------------------------------------------------------------------

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

// I set the pin usage to facilitate easy connection of jumper cables
// for now, the #defines below are used to program the port modes using the standard Arduino library

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   328  | 2560
#define LEDposPin  8       // Pin 8 (LED)   PB0    PH5
#define LEDnegPin  9       // Pin 9 (GND)   PB1    PH6

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  4       // Pin 4 (Dir)   PD4    PG5
#define Dec5vPin   5       // Pin 5 (5V?)   PD5    PE3
#define DecStepPin 6       // Pin 6 (Step)  PD6    PH3
#define DecGNDPin  7       // Pin 7 (GND)   PD7    PH4

#define HADirPin   11      // Pin 11 (Dir)  PB3    PB5
#define HA5vPin    12      // Pin 12 (5V?)  PB4    PB6
#define HAStepPin  13      // Pin 13 (Step) PB5    PB7
                           // Pin GND (GND)

// defines for direct port control
#define LEDposBit  5       // Pin 8
#define LEDposPORT PORTH   //
#define LEDnegBit  6       // Pin 9
#define LEDnegPORT PORTH   //

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

#define DecDirEInit      HIGH
#define DecDirWInit      LOW
#define HADirEInit       LOW
#define HADirWInit       HIGH
volatile byte DecDir     = DecDirEInit;
volatile byte HADir      = HADirEInit;

// Status ------------------------------------------------------------------------------------------------------------------
boolean highPrecision    = true;

#define TrackingNone     0
#define TrackingSidereal 1
#define TrackingMoveTo   2
byte trackingState       = TrackingNone;
byte lastTrackingState   = TrackingNone;
byte currentGuideRate    = 1;
boolean abortSlew        = false;

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
#define F_CPU 16000000UL
#define BAUD 9600

boolean commandError     = false;
boolean quietReply       = false;

char reply[25];

char command[3];
char parameter[25];
byte bufferPtr= 0;

char command_serial_zero[25];
char parameter_serial_zero[25];
byte bufferPtr_serial_zero= 0;

char Serial_recv_buffer[256] = "";
volatile byte Serial_recv_tail = 0;
volatile byte Serial_recv_head = 0;
char Serial_xmit_buffer[25] = "";
byte Serial_xmit_index = 0;

char Serial1_recv_buffer[256] = "";
volatile byte Serial1_recv_tail = 0;
volatile byte Serial1_recv_head = 0;
char Serial1_xmit_buffer[25] = "";
byte Serial1_xmit_index = 0;

// for bluetooth/serial 1
char command_serial_one[25];
char parameter_serial_one[25];
byte bufferPtr_serial_one= 0;

// Misc ---------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

int baudRate[10] = {0,56700,38400,28800,19200,14400,9600,4800,2400,1200};

char   s[20];
double f,f1,f2,f3;
int    i,i1,i2;
byte   b;

// current site index and name
byte currentSite = 0; 
char siteName[16];

// align command
double altCor            = 0;       // for geometric coordinate correction/align
double azmCor            = 0;
double IH                = 0;       // offset corrections/align
double ID                = 0;

// move command
byte    moveDirHA        = 0;
int     moveDurationHA   = 0;
byte    moveDirDec       = 0;
int     moveDurationDec  = 0;

// Slew control
int     moveRates[9]={7,15,30,60,120,240,360,600,900}; 
//                      1x 2x 4x 8x  16x 24x 40x 60x
int     moveTimerRateHA  = 0;
int     moveTimerRateDec;
int     lastTimerRateHA;
int     lastTimerRateDec;
//
unsigned long msMoveHA   = 0;
int     amountMoveHA     = 0;
int     moveHA           = 0;
int     accGuideHA       = 0;
int     accMoveHA        = 0;
unsigned long msMoveDec  = 0;
int     amountMoveDec    = 0;
int     moveDec          = 0;

// PEC control
#define PECStatusString  "IpPrR"
#define IgnorePEC        0
#define ReadyPlayPEC     1
#define PlayPEC          2
#define ReadyRecordPEC   3
#define RecordPEC        4
unsigned long PEC_Timer  = 0;        // for PEC, keeps time for applying steps
long PEC_SKIP            = 0;        // for PEC, number of sidereal hundredths of a second between applying steps
long PEC_SKIP_HA         = 0;        // for PEC, buffers steps
boolean PECfirstRecord   = false;
boolean PECstatus        = IgnorePEC;
boolean PECrecorded      = false;
long    PECrecord_index  = 0;
long    lastWormRotationStepPos = -1;
long    wormRotationStepPos = 0;
long    PECindex         = 0;
long    lastPECindex     = -1;
unsigned long timeOutIndexDetect = 0;

// backlash control
volatile int     backlashHA   = 0;
volatile int     backlashDec  = 0;
volatile int     blHA         = 0;
volatile int     blDec        = 0;

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

#define EE_minAlt      40
#define EE_maxAlt      41

#define EE_altCor      50
#define EE_azmCor      54
#define EE_IH          58
#define EE_ID          62

#define EE_PECstatus   70
#define EE_PECrecorded 71
#define EE_PECrecord_index 72

#define EE_backlashHA  80
#define EE_backlashDec 84

#define EE_siderealInterval 88

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
  // init. the timers that handle RA and Dec
  TCCR3B = 0; TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);
  OCR3A=32767;

  TCCR4B = 0; TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
  OCR4A=32767;

  // the following could be done with register writes to save flash memory
  pinMode(HAStepPin, OUTPUT);        // initialize the stepper control pins RA
  pinMode(HADirPin, OUTPUT); 
  pinMode(DecGNDPin, OUTPUT);        // initialize the stepper control pins Dec
  CLR(DecGNDPORT, DecGNDBit);
  pinMode(DecStepPin, OUTPUT); 
  pinMode(DecDirPin, OUTPUT); 
  
  pinMode(LEDnegPin, OUTPUT);        // light status LED
  pinMode(LEDposPin, OUTPUT);     
  CLR(LEDnegPORT, LEDnegBit);
  SET(LEDposPORT, LEDposBit);

#ifdef POWER_SUPPLY_PINS_ON  
  pinMode(HA5vPin, OUTPUT);         // provide 5V power to stepper drivers if requested
  SET(HA5vPORT, HA5vBit);
  pinMode(Dec5vPin, OUTPUT);
  SET(Dec5vPORT, Dec5vBit);
#endif  

  // EEPROM init code here to help get up and running quickly
  #ifdef INIT_ON
  // init the site information, lat/long/tz/name
  EEPROM.write(EE_currentSite,0);
  latitude=0; longitude=0;
  for (int l=0; l<4; l++) {
    EEPROM_writeQuad(EE_sites+(l)*25+0,(byte*)&latitude);
    EEPROM_writeQuad(EE_sites+(l)*25+4,(byte*)&longitude);
    EEPROM.write(EE_sites+(l)*25+8,128);
    EEPROM.write(EE_sites+(l)*25+9,0);
  }

  // init the date and time January 1, 2013. 0 hours LMT
  JD=2456293.5;
  LMT=0.0;
  EEPROM_writeQuad(EE_JD,(byte*)&JD);
  EEPROM_writeQuad(EE_LMT,(byte*)&LMT);

  // init the min and max altitude
  minAlt=-10;
  maxAlt=85;
  EEPROM.write(EE_minAlt,minAlt+128);
  EEPROM.write(EE_maxAlt,maxAlt);

  // init (clear) the backlash amounts
  EEPROM_writeInt(EE_backlashDec,0);
  EEPROM_writeInt(EE_backlashHA,0);

  // init the PEC status, clear the index and buffer
  PECrecord_index=0;
  EEPROM.write(EE_PECstatus,IgnorePEC);
  EEPROM.write(EE_PECrecorded,false);
  for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_PECindex+l,128);
  EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECrecord_index); 
  
  // init the Park status
  EEPROM.write(EE_parkSaved,false);
  EEPROM.write(EE_parkStatus,NotParked);

  // init the sidereal tracking rate, use this once - then issue the T+ and T- commands to fine tune
  // 1/16uS resolution timer, ticks per sidereal second
  EEPROM_writeQuad(EE_siderealInterval,(byte*)&masterSiderealInterval);
  #endif

  // this sets the sidereal timer, controls the tracking speed so that the mount moves precisely with the stars
  EEPROM_readQuad(EE_siderealInterval,(byte*)&masterSiderealInterval);

  // this sets the maximum speed that the motors can step while sidereal tracking, one half the siderealInterval for twice the speed
  // this is so guiding corrections can be played back at up to 2X the sidereal rate in RA, 1X in Dec
  // 
  // this is the number of ISR ticks per step, used for putting a cap on the maximum speed the ISR will allow the steppers to move 

  // 16MHZ clocks for 48 steps per second + enough speed for guiding
  SiderealRate    =(siderealInterval/StepsPerSecond)/2;
  timerRateHA     =SiderealRate;
  
  #ifdef DEC_RATIO_ON
  timerRateDec    =(SiderealRate*2)*timerRateRatio;
  #else
  timerRateDec    =(SiderealRate*2);
  #endif
  
  // backlash takeup rates
  timerRateBacklashHA =timerRateHA /(BacklashTakeupRate/2);
  timerRateBacklashDec=timerRateDec/(BacklashTakeupRate);

  // disable timer0 overflow interrupt, it causes timer1 to miss too many interrupts
  // this method was abandoned, but I leave it here since it might have future application
  // to help lessen the processing load during very high speed GoTo's
 // TIMSK0 &= ~_BV(TOIE0);

  // initialize timer1
  Timer1SetRate(siderealInterval/100);

  // get ready for serial communications
  Serial1_Init(9600);
  Serial_Init(9600);
  
  // get the site information, if a GPS were attached we would use that here instead
  currentSite=EEPROM.read(EE_currentSite);
  if (currentSite>3) currentSite=0; // site index is valid?
  EEPROM_readQuad(EE_sites+(currentSite)*25+0,(byte*)&latitude);
  EEPROM_readQuad(EE_sites+(currentSite)*25+4,(byte*)&longitude);
  timeZone=EEPROM.read(EE_sites+(currentSite)*25+8)-128;
  EEPROM_readString(EE_sites+(currentSite)*25+9,siteName);
  
  // get date and time from EEPROM, start keeping time
  EEPROM_readQuad(EE_JD,(byte*)&JD);
  EEPROM_readQuad(EE_LMT,(byte*)&LMT);
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
  for (i=0; i<PECBufferSize; i++) PEC_buffer[i]=EEPROM.read(EE_PECindex+i);
  EEPROM_readQuad(EE_PECrecord_index,(byte*)&PECrecord_index); 
  
  // get the Park status
  parkSaved=EEPROM.read(EE_parkSaved);
  parkStatus=EEPROM.read(EE_parkStatus);

  // makes onstep think that you parked the 'scope
  // combined with a hack in the goto syncEqu() function and you can quickly recover from
  // a reset without loosing much accuracy in the sky.  PEC is toast though.
  #ifdef RESCUE_MODE_ON
  parkSaved=true;    
  parkStatus=Parked;
  #endif

  // set the default guide rate, 1x sidereal
  setGuideRate(1);

  // prep timers
  cli(); 
  msTimer1=millis(); 
  ut1Timer=millis();
  timeOutIndexDetect=millis();
  PECsiderealTimer = lst;
  siderealTimer = lst;
  sei();
}

void loop() {
  
  processCommands();
  
  // GUIDING -------------------------------------------------------------------------------------------
    
  // two software timers, one ea. for RA and Dec are set to trigger at the frequency necessary to guide at a given rate, 
  // only active if not slewing.
  // these routines must run every 10mS to move the mount properly during guiding (worst case) on my G11 running at 0.3125 arc-sec/step.
  // If your steppers move in smaller micro-steps than that, things happen even quicker.  Not making the 10mS timing doesn't disturb the tracking
  // accuracy of the mount, just means that your guiding speed isn't maintained.  I have a nice safety margin with 21mS timing when guiding
  // at 1x sidereal rate.  I poll the :GR# command frequently to get the RA/Dec that the mount is pointed at, hopefully that takes no more
  // than 6-7mS (according to my estimates.)  That's about the slowest running command that matters during guiding.
  moveHA=0;
  if (trackingState==TrackingSidereal) {
    int sign=0;
    unsigned long tempMilli=millis();
    if ((tempMilli-msTimer1)>=msMoveHA) {
      msTimer1=tempMilli;

      if ((moveDirHA) && (!inBacklashHA)) { 
        // as above, and keep track of how much we've moved for PEC recording
        if (moveDirHA=='e') sign=-1; else sign=1; moveHA=sign*amountMoveHA;
        // for RA, only apply the corrections now if fast guiding; otherwise they get combined with PEC & sidereal-tracking and are applied later
        if (currentGuideRate>1) { cli(); targetHA+=moveHA;  sei(); } else { accGuideHA+=moveHA; }
      }
      // for pulse guiding, count down the mS and stop when timed out
      if (moveDurationHA>0)  { moveDurationHA-=msMoveHA; if (moveDurationHA<=0) { cli(); timerRateHA=lastTimerRateHA; moveDirHA=0; sei(); } }
    }
    if ((tempMilli-msTimer2)>=msMoveDec) {
      msTimer2=tempMilli;
    
      if ((moveDirDec) && (!inBacklashDec)) { 
        // nudge the targetDec (where we're supposed to be) by amountMoveDec
        if (moveDirDec=='s') sign=-1; else sign=1; cli(); targetDec=targetDec+sign*amountMoveDec; sei();
      }
      // for pulse guiding, count down the mS and stop when timed out
      if (moveDurationDec>0) { moveDurationDec-=msMoveDec; if (moveDurationDec<=0) { cli(); timerRateDec=lastTimerRateDec; moveDirDec=0; sei(); } }
    }
  }
  
  // PEC ---------------------------------------------------------------------------------------------
    
  // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense
  if ((trackingState==TrackingSidereal) && !(moveDirHA && (currentGuideRate>1))) {

    // keep track of our current step position, and when the step position on the worm wraps during playback
    lastWormRotationStepPos = wormRotationStepPos;
    cli(); long t=targetHA; sei();

    // where we're at (HA), must always be positive, so add 360 degrees (in steps) and compute from there
    t=t+StepsPerDegreeHA*360;

    wormRotationStepPos = (t-PECrecord_index) % StepsPerWormRotation;
    
    // handle playing back and recording PEC
    if (PECstatus!=IgnorePEC) {
      // start playing PEC
      if (PECstatus==ReadyPlayPEC) {
        // approximate, might be off by nearly a second, this makes sure that the index is at the start of a second before resuming play
        if (wormRotationStepPos%StepsPerSecond==0) {
          PECstatus=PlayPEC;
          PECindex=wormRotationStepPos/StepsPerSecond;
          // sum corrections to this point
          long m=0; for (int l=0; l<PECindex; l++) { m+=PEC_buffer[l]-128; }
          // move to the corrected location, this might take a few seconds and can be abrupt (after goto's or when unparking)
          cli(); PEC_HA = m; sei();
          // playback starts now
          cli(); PECsiderealTimer = lst; sei();
        }
      }

      // start recording
      if (PECstatus==ReadyRecordPEC) {
        // zero index counter
        PECrecord_index = t % StepsPerWormRotation;
        wormRotationStepPos = (t-PECrecord_index) % StepsPerWormRotation;
        lastWormRotationStepPos++; // if this just happens to be 0, recording wouldn't start right now, so make sure its always > 0
      }

      // at the start of a worm cycle if wormRotationStepPos rolled over...
      // this only works because targetHA can't move backward while tracking at +/- 1x sidereal
      // it also works while guiding and playing PEC because the PEC corrections don't get counted in the targetHA location
      if (lastWormRotationStepPos>wormRotationStepPos) {

        // start recording PEC now
        if (PECstatus==ReadyRecordPEC) { 
          PECstatus=RecordPEC;
          PECrecorded=false;    
        } else
        // and once the PEC data is all stored, indicate that it's valid and start using it
        if (PECstatus==RecordPEC) {
          PECstatus=PlayPEC;
          PECrecorded=true;
          PECfirstRecord=false;
#ifdef PEC_CLEANUP_ON
          // the number of steps added should equal the number of steps subtracted (from the cycle)
          // first, determine how far we've moved ahead or backward in steps
          long sum_pec=0; for (int scc=0; scc<StepsPerWormRotation/StepsPerSecond; scc++) { sum_pec+=(int)PEC_buffer[scc]-128; }

          // this is the correction coefficient for a given location in the sequence
          double Ccf = (double)sum_pec/(double)(StepsPerWormRotation/StepsPerSecond);

          // now, apply the correction to the sequence to make the PEC adjustments null out
          // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
          long lp2=0; sum_pec=0; 
          for (int scc=0; scc<StepsPerWormRotation/StepsPerSecond; scc++) {
            // the correction, "now"
            long lp1=lround(-scc*Ccf);
            
            // if the correction increases or decreases then add or subtract that many steps
            PEC_buffer[scc]=(int)PEC_buffer[scc]+(lp1-lp2);

            // sum the values for a final adjustment, if necessary
            sum_pec+=(int)PEC_buffer[scc]-128;     
            lp2=lp1;
          }
          PEC_buffer[0]-=sum_pec;

          // a reality check, make sure the buffer data looks good, if not forget it
          if ((sum_pec>2) || (sum_pec<-2)) { PECrecorded=false; PECstatus=IgnorePEC; }
#endif
        }
        
        lastPECindex = PECindex-1; // immediately starts recording
        accMoveHA    = 0;          // this is how much the guide command has moved the 'scope

        // clear the PEC offset and zero the index timer so recording/playback starts now
        PEC_SKIP_HA = 0; cli(); PECsiderealTimer = lst; sei();
      }

      // sidereal timer for indexing PEC
      cli(); long tempLst=lst; sei(); if (tempLst-PECsiderealTimer>99) { PECsiderealTimer=tempLst; PECindex++; if (PECindex>=StepsPerWormRotation/StepsPerSecond) PECindex=0; }
    
      accMoveHA+=moveHA;
    } else {
      // if we're ignoring PEC, zero the offset and keep it that way
      cli(); PEC_HA=0; sei(); 
      PEC_SKIP = 0;
    }

    // falls in whenever the PECindex changes, which is once a sidereal second
    if (PECindex!=lastPECindex) {
      lastPECindex=PECindex;
  
      if (PECstatus==RecordPEC) {
        // save the correction 1:2 weighted average
        int l=accMoveHA;
        if (l<-StepsPerSecond) l=-StepsPerSecond; if (l>StepsPerSecond) l=StepsPerSecond;   // +/-1 sidereal rate range for corrections
        if (!PECfirstRecord) l=(l+((int)PEC_buffer[PECindex]-128)*2)/3; 
        PEC_buffer[PECindex]=l+128;  // save the correction
        accMoveHA=0;                 // and clear the accumulator
      }
      
      if (PECstatus==PlayPEC) {
        // PECindex2 adjusts one second before the value was recorded, an estimate of the latency between image acquisition and response
        // if sending values directly to OnStep from PECprep, etc. be sure to account for this
        int PECindex2=PECindex-1; if (PECindex2<0) PECindex2=PECindex2+(StepsPerWormRotation/StepsPerSecond);
        // PEC_HA (PEC_SKIP_HA) play back speed can be +/-1 of sidereal
        // PEC_SKIP is the number of ticks between the added (or skipped) steps
        PEC_SKIP_HA=PEC_buffer[PECindex2]-128;
        if (PEC_SKIP_HA>StepsPerSecond) PEC_SKIP_HA=StepsPerSecond; if (PEC_SKIP_HA<-StepsPerSecond) PEC_SKIP_HA=-StepsPerSecond;
        if (PEC_SKIP_HA==0) PEC_SKIP=(StepsPerWormRotation/StepsPerSecond)+1; 
        else
          PEC_SKIP=StepsPerSecond/abs(PEC_SKIP_HA);
        PEC_Timer=0;
      }
    }
  } else {
    // give up recording if we stop tracking at the sidereal rate
    if (PECstatus==RecordPEC)  { PECstatus=IgnorePEC; PEC_SKIP=0; } // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
    // get ready to re-index when tracking comes back
    if (PECstatus==PlayPEC)  { PECstatus=ReadyPlayPEC; PEC_SKIP=0; cli(); PEC_HA=0; sei(); } 
  }
  
  // tracking rate check code, detects missed ticks
  //static boolean oops;
  
  // SIDEREAL TRACKING ---------------------------------------------------------------------------------
  // moves the targetHA to match the earth's rotation/revolution, can handle up to 100 steps/sidereal second
  // siderealTimer, falls in once every 1/100 sidereal second (1 tick, about 10mS) if the main loop is so busy that this timer
  // is >1 tick you have a problem and the sidereal tracking may be affected.  Again, the slowest commands seem to take 6-7mS and it's
  // just fast enough to keep up - fortunately the new design only seriously robs MPU cycles during fast GoTo operations when this 
  // code isn't running
  cli(); long tempLst=lst; sei();
  long t=tempLst-siderealTimer;
  if (t>=1) {
    siderealTimer=tempLst;

    // tracking rate check code, detects missed ticks
//    if (t>1) oops=true; else oops=false;

    // handles moving to an new target RA and Dec
    if (trackingState==TrackingMoveTo) moveTo();

    // see if we need to take a step
    ResultA1=ResultA2;
    ResultA2=floor((double)((StepsPerSecond)/100.0)*(double)(siderealTimer%100));

    if ((((trackingState==TrackingMoveTo) && (lastTrackingState==TrackingSidereal)) || (trackingState==TrackingSidereal)) &&
        !(moveDirHA && (currentGuideRate>1))) // only active with a guide rate that makes sense
      {
      if (ResultA1!=ResultA2) {
          // PEC_Timer starts at zero again every second, PEC_SKIP will control the rate and will trigger a +/- step every PEC_SKIP steps while tracking 
          PEC_Timer++;
  
          // lookup Guiding
          long ig=0; 
          if (accGuideHA<0) { ig=-1; accGuideHA++; } else if (accGuideHA>0) { ig=1; accGuideHA--; }
  
          // lookup PEC
          long ip=0;
          if (PEC_SKIP==0) {
            // disable playback
          } else {
            // get ready to apply the correction
            if (PEC_Timer%PEC_SKIP==0) {
              if (PEC_SKIP_HA>0) { ip=1; PEC_SKIP_HA--; } else
                if (PEC_SKIP_HA<0) { ip=-1; PEC_SKIP_HA++; }
            }
          }
  
          // apply the Tracking, Guiding, and PEC
          cli(); targetHA=targetHA+1+ig; PEC_HA+=ip; sei();
      }
    }
  }

  // UT1 CLOCK -----------------------------------------------------------------------------------------
  // timer... falls in once a second, keeps the universal time clock ticking
  // this clock doesn't rollover (at 24 hours) since that would cause date confusion
  unsigned long tempMilli=millis(); 
  if (tempMilli-ut1Timer>999) {
    double t2=(double)(tempMilli-UT1mS_start)/1000.0;
    UT1=UT1_start+t2/3600.0;   // This just needs to be accurate to the nearest second, it's about 10x better
    ut1Timer=tempMilli;
    
    // naturally, were really working with a single.  we should have, just barely, enough significant digits to get us through a day
    unsigned long lst_now=lst_start+round( (double)((tempMilli-lst_mS_start)/10.0) * 1.00273790935);

    // the "master" clock is millis(), due to it's low rate and fast execution time it doesn't miss interrupts due to serial communications, etc.
    // the sidereal time is calculated and compared to the high-resolution timer, the high resolution timer is adjusted to keep the two in sync.
    // the value 160 represents 10uS at a 16MHz clock rate (0.001%)
    cli();
    if ((lst>lst_now) && (siderealInterval!=masterSiderealInterval+160)) { siderealInterval=masterSiderealInterval+160; Timer1SetRate(siderealInterval/100); } // running too fast, slow it down
    if ((lst==lst_now)&& (siderealInterval!=masterSiderealInterval))     { siderealInterval=masterSiderealInterval;     Timer1SetRate(siderealInterval/100); }
    if ((lst<lst_now) && (siderealInterval!=masterSiderealInterval-160)) { siderealInterval=masterSiderealInterval-160; Timer1SetRate(siderealInterval/100); } // running too slow, speed it up
    sei();

    // update the local sidereal time floating point representation
    update_LST();
    
    // reset the sidereal clock once a day, to keep the significant digits from being consumed, debated about including this, but
    // I'm shooting for keeping OnStep reliable for about 50 days of continuous uptime (until millis() rolls over)
    if ((lst_now-lst_start)>24*3600*100) update_lst();


/*
    // tracking rate check code, detects missed ticks
    if (oops) Serial1_print("!"); else Serial1_print(".");
*/

/*
    // this is useful checking the timing of milli
    static unsigned int sc=0;
    char temp[80];
    sc++; sc=sc%60;
    sprintf(temp,"milli=%i, lst=%i\r\n",sc,lst/100);
    Serial1_print(temp);
*/

/*
    // this is useful for seeing the guiding commands work
    static unsigned long lastPosHA,lastPosDec;
    char temp[80];
    cli(); 
    sprintf(temp,"~h=%l, ~d=%l, msch=%i, ms_h=%i\r\n",posHA-lastPosHA,posDec-lastPosDec,moveTimerRateHA,msMoveHA);
    sei();
    lastPosHA=posHA;
    lastPosDec=posDec;
    Serial1_print(temp);
 */

/*
    // this is useful for seeing what PEC and sidereal tracking is doing
    static unsigned long last_posHA=0;
    char temp[80];
    sprintf(temp,"wrsp=%l, PECindex=%l, PEC_HA=%i, ~posHA=%l\r\n",wormRotationStepPos,PECindex,PEC_buffer[PECindex]-128,posHA-last_posHA);
    Serial1_print(temp);
    last_posHA=posHA;
*/

/*
    // this is useful for seeing the Backlash compensation work
    char temp[80];
    sprintf(temp,"blHA=%l, blDec=%l, backlashHA=%l, backlashDec=%l\r\n",blHA,blDec,backlashHA,backlashDec);
    Serial1_print(temp);
 */
  }
}
