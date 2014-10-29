/*
 * Title       On-Step
 * by          Howard Dutton
 *
 * Copyright (C) 2013, 2014 Howard Dutton
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
 * 10-29-2014          1.0b4             Updated step generation code for sidereal tracking and guiding, allows better performance and non-integral StepsPerSecond rates.
 *                                       Finished code to enable dynamic tracking rate compensation for refraction.  Added code to allow use of PEC index sense feature.
 *                                       New commands :Te# :Td# turn refraction tracking on/off, :VH# reads PEC HALL sensor position in seconds.
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
#define FirmwareDate   "10 29 14"
#define FirmwareNumber "1.0b4"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// turns debugging on, used during testing, default=OFF
#define DEBUG_OFF
// for getting control of the 'scope when things go horribly wrong, default=OFF
#define RESCUE_MODE_OFF

// PEC sense rising edge on pin 2 (adjusts PEC index position), default=OFF (not tested)
#define PEC_PIN_OFF
// limit sense switch close (to ground) on pin 3 (stops gotos and/or tracking), default=OFF (not tested)
#define LIMIT_PIN_OFF
// light status LED by sink to ground (pin 9) and source +5V (pin 8), default=ON
#define STATUS_LED_PINS_ON
// lights status LED by sink to ground (pin 10), default=OFF
#define STATUS_LED2_PINS_ON
// supply power on pins 5 and 11 to Pololu or other stepper drivers without on-board 5V voltage regulators, default=OFF
#define POWER_SUPPLY_PINS_ON

// enables goTo speed equalization for differing right ascension and declination StepsPerDegreeHA/Dec, default=OFF (limited testing done)
#define DEC_RATIO_OFF

// enables alignment on two or three stars, default=ON
#define ALIGN_TWO_AND_THREE_STAR_ON

// enables Horizon coordinate goto functions, default=ON
#define ALT_AZM_GOTO_ON

// enables code to clean-up PEC readings after record (use PECprep or a spreadsheet to fix readings otherwise)
// this cleans up any tracking rate variations that would be introduced by recording more guiding corrections to either the east or west, default=ON
#define PEC_CLEANUP_ON

// adjusts tracking rate to compensate for atmospheric refraction, default=OFF (not tested)
// can be turned on/off with the :Te# and :Td# commands regardless of this setting
#define TRACK_REFRACTION_RATE_DEFAULT_OFF

// these turn on and off checksum error correction on the serial ports, default=OFF
#define CHKSUM0_OFF     // default _OFF: required for OnStep ASCOM driver
#define CHKSUM1_OFF     // default _OFF: required for OnStep Controller2 Android App (and others)

// forces initialialization of a host of settings in EEPROM. OnStep does this automatically, most likely, you will want to leave this alone
#define INIT_KEY false    // set to true to keep automatic initilization from happening.  This is a one-time operation... upload to the Arduino, then set to false and upload again
#define initKey 915307547 // unique identifier for the current initialization format, do not change

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define MaxRate                   32 // this is the minimum number of micro-seconds between micro-steps
                                     // minimum (fastest goto) is around 16 (Teensy3.1) or 32 (Mega2560), default is 96, higher is ok
                                     // too low and OnStep communicates slowly and/or freezes as the motor timers use up all the MCU time
                                     
#define StepsForRateChange  200000.0 // number of steps during acceleration and de-acceleration: higher values=longer acceleration/de-acceleration
                                     // for the most part this doesn't need to be changed, but adjust when needed

#define BacklashTakeupRate        20 // backlash takeup rate (in multipules of the sidereal rate): too fast and your motors will stall,
                                     // too slow and the mount will be sluggish while it moves through the backlash
                                     // for the most part this doesn't need to be changed, but adjust when needed
                                     // this should be 60 or lower

                                     // for my mounts both RA and Dec axis have the same gear train
#define StepsPerDegreeHA      19200L // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  48            * 16          * 50 * (40/32)    *  144/360              = 19200
                                     // Losmandy G11     :  48            * 16          * 15              *  360/360              = 11520
#define StepsPerDegreeDec     19200L // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Takahashi EM10b  :  48            * 16          * 50 * (40/32)    *  144/360              = 19200
                                     // Losmandy G11     :  48            * 16          * 15              *  360/360              = 11520
                                     // the EM10b has two spur gears that drive the RA/Dec worms, they are 60 tooth and 48 tooth gears
                                     // for an 1.25x reduction in addition to the 20:1 gear heads on the steppers for a 25:1 final ratio
                                     // before the worm/wheels 144:1
                                     
#define StepsPerSecond          80.0 // the steps per second sidereal rate = (19200/3600)*15 = 80 - OnStep can handle between 12 and 100 steps/second
                                     // StepsPerSecond doesn't need to be an integer
                                     // StepsPerWormRotation (for PEC) needs to be evenly divisible by StepsPerSecond

#define StepsPerWormRotation  38400L // PEC, number of steps for a complete worm rotation (in RA), (StepsPerDegreeHA*360)/gear_reduction2 
                                     // the EM10b has a worm-wheel with 144 teeth (19200*360)/144 = 48000
                                     // According to this the EM10b needs 600 (seconds) of PEC buffer.  Since the transfer gears have a rather large effect on the periodic error
                                     // I have two options 1. use 4x the 48000 = 192000 (2400 seconds.)  This is 4 complete rotations of the worm and 5 complete rotations of the transfer gear.
                                     //                       I expect the results would be very good, but since PECPrep is limited to 1000S I don't bother.
                                     //                    2. use 480 (seconds) of PEC buffer.  This is easier to program and keeps the PE at low levels
                                     //                       so pretending we have 180 teeth (19200*360)/180 = 38400

#define PECBufferSize           2400 // PEC, buffer size, max should be no more than 3384, your required buffer size >= StepsPerWormRotation/StepsPerSecond
                                     // for the most part this doesn't need to be changed, but adjust when needed.  824 seconds is the default

#define REVERSE_HA_ON                // reverse the direction of movement for the HA/RA axis, adjust as needed or reverse your wiring so things move in the right direction
#define REVERSE_DEC_ON               // reverse the direction of movement for the Dec axis (both reversed for my EM10b, both normal for G11)

#define minutesPastMeridianE      60 // for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60
#define minutesPastMeridianW      60 // as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit
#define underPoleLimit             9 // maximum allowed hour angle (+/-) under the celestial pole. OnStep will flip the mount and move the Dec. past 90 degrees (+/-) once past this limit
                                     // to arrive at the location.  If left alone, the mount will stop tracking when it hits this limit.  Valid range is 7 to 11 hours

// this group is for advanced configuration and is not well tested yet, leave it alone or just use the HA_MODE and DEC_MODE values if you didn't hard wire the micro-stepping mode
// DRV8825: 5=32x, 4=16x, 3=8x, 2=4x, 1=2x, 0=1x
#define HA_MODE 5                    // programs the HA uStep mode M0/M1/M2, optional and default _OFF. Other values 0 to 7 (0xb000 to 111): for example "#define HA_MODE 4"
#define HA_MODE_GOTO 3               // programs the HA uStep mode M0/M1/M2, used during gotos, optional and default _OFF. Other values 0 to 7 (0xb000 to 111): for example "#define HA_MODE 4"
#define HA_STEP_GOTO 4               // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4
#define DE_MODE_OFF                  // programs the Dec uStep mode M0/M1/M2, optional and default _OFF. Other values 0 to 7 (0xb000 to 111)
#define DE_MODE_GOTO_OFF             // programs the Dec uStep mode M0/M1/M2, used during gotos, optional and default _OFF. Other values 0 to 7 (0xb000 to 111)
#define DE_STEP_GOTO 1               // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------


// Time keeping ------------------------------------------------------------------------------------------------------------
unsigned long siderealTimer = 0;
unsigned long siderealTimer2;
unsigned long tenthSecondTimer;
//unsigned long ut1Timer;
unsigned long msTimer1 = 0;
unsigned long msTimer2 = 0;
unsigned long now = 0;

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
long PECindexTimer = 0;              // time since worm wheel zero index for PEC
long PECtime_lastSense=0;            // time since last PEC index was sensed
unsigned long PECindex_sense=0;      // position of active PEC index sensed
unsigned long next_PECindex_sense=-1;// position of next PEC index sensed

boolean customRateActive=true;       // automatically modify the siderealInterval to compensate for atmospheric refraction

long siderealInterval       = 15956313;
long masterSiderealInterval = siderealInterval;
                                     // default = 15956313 ticks per sidereal hundredth second, where a tick is 1/16 uS
                                     // this is stored in EEPROM which is updated/adjusted with the ":T+#" and ":T-#" commands
                                     // stars moving left means too fast, right means too slow (for my 'scope optics/camera)
                                     // a higher number here means a longer count which slows down the sidereal clock...
                                     // movement is based on the sidereal clock, but SiderealRate must be low enough that
                                     // the stepping keeps pace with the targetHA for accurate tracking
double HzCf = 16000000.0/60.0;
                                     // keeps sidereal time, might need to be adjusted due to variations in Arduino crystal accuracy, etc.
                                     // someday I'd like to add a GPS module and grab the pps (pulse per second) signal as a high-accuracy
                                     // time reference to correct drift in my sidereal clock due to physical effects and software limitations

long SiderealRate;                   // based on the siderealInterval, this is the time between steps for sidereal tracking
#ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
boolean refraction = true;          // track at the refraction rate for the area of the sky
#else
boolean refraction = false;
#endif
unsigned int gr_st=0,gr_sk=0,gr_st1=0,gr_sk1=0;  // used for guiding step generation, RA
unsigned int gd_st=0,gd_sk=0,gd_st1=0,gd_sk1=0;  // used for guiding step generation, Dec
volatile unsigned int st=0,sk=0,st1=0,sk1=0;     // used for sidereal tracking step generation

volatile double  pecTimerRateHA = 0;
volatile double  guideTimerRateHA = 0;
volatile long    timerRateHA = 0;
volatile long    timerRateBacklashHA = 0;
volatile boolean inBacklashHA=false;
volatile double  guideTimerRateDec = 0;
volatile long    timerRateDec = 0;
volatile long    timerRateBacklashDec = 0;
volatile boolean inBacklashDec=false;
#define timerRateRatio          ((double)StepsPerDegreeHA/(double)StepsPerDegreeDec)
#define SecondsPerWormRotation  ((long)(StepsPerWormRotation/StepsPerSecond))

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
long celestialPoleDec = 90L;
double longitude = 0.0;

volatile long posHA      = 90L*StepsPerDegreeHA;   // hour angle position in steps
volatile long startHA    = 90L*StepsPerDegreeHA;   // hour angle of goto start position in steps
volatile long targetHA   = 90L*StepsPerDegreeHA;   // hour angle of goto end   position in steps
volatile long targetHA1  = 90L*StepsPerDegreeHA;   // hour angle of goto end   position in steps
volatile byte dirHA      = 1;                      // stepping direction + or -
volatile long PEC_HA     = 0;                      // for PEC, adds or subtracts steps
double newTargetRA       = 0.0;                    // holds the RA for goTos
long origTargetHA        = 0;

volatile long posDec     = 90L*StepsPerDegreeDec; // declination position in steps
volatile long startDec   = 90L*StepsPerDegreeDec; // declination of goto start position in steps
volatile long targetDec  = 90L*StepsPerDegreeDec; // declination of goto end   position in steps
volatile byte dirDec     = 1;                     // stepping direction + or -
double newTargetDec      = 0.0;                   // holds the Dec for goTos
double avgDec            = 0.0;                   // for align
double avgHA             = 0.0;                   // for align
long origTargetDec       = 0;

double newTargetAlt=0.0, newTargetAzm=0.0;        // holds the altitude and azmiuth for slews
double currentAlt = 45;                           // the current altitude
int    minAlt;                                    // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                    // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)

// Stepper/position/rate ----------------------------------------------------------------------------------------------------

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

// I set the pin usage to facilitate easy connection of jumper cables
// for now, the #defines below are used to program the port modes using the standard Arduino library

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PecPin     2

// The limit switch sense is a 5V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LimitPin   3

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
// reserved and not yet implemented
#define PPS        18      // Pin 18 (PPS time source, GPS for example)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
//                                  Atmel   328  | 2560
#define LEDposPin  8       // Pin 8 (LED)   PB0    PH5
#define LEDnegPin  9       // Pin 9 (GND)   PB1    PH6
#define LEDneg2Pin 10      // Pin 10 (GND)  PB2?   PH7?

// The HA(RA) and Dec jumpers (going to the big easy drivers) are simply four wire jumper cables, each has identical wiring - simple modular construction
#define DecDirPin  4       // Pin 4 (Dir)   PD4    PG5
#define Dec5vPin   5       // Pin 5 (5V?)   PD5    PE3
#define DecStepPin 6       // Pin 6 (Step)  PD6    PH3
#define DecGNDPin  7       // Pin 7 (GND)   PD7    PH4

// defines for direct port control
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define HADirPin   11      // Pin 11 (Dir)  PB3    PB5
#define HA5vPin    12      // Pin 12 (5V?)  PB4    PB6
#define HAStepPin  13      // Pin 13 (Step) PB5    PB7
                           // Pin GND (GND)
                         
// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      22      // Pin 22 (Microstep Mode 0)
#define HA_M1      23      // Pin 23 (Microstep Mode 1)
#define HA_M2      24      // Pin 24 (Microstep Mode 2)
#define HA_EN      25      // Pin 25 (Enabled when HIGH)

#define DE_M0      26      // Pin 26 (Microstep Mode 0)
#define DE_M1      27      // Pin 27 (Microstep Mode 1)
#define DE_M2      28      // Pin 28 (Microstep Mode 2)
#define DE_EN      29      // Pin 29 (Enabled when HIGH)

#define LEDposBit  5       // Pin 8
#define LEDposPORT PORTH   //
#define LEDnegBit  6       // Pin 9
#define LEDnegPORT PORTH   //
#define LEDneg2Bit  7      // Pin 10
#define LEDneg2PORT PORTH  //

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

#elif defined(__arm__) && defined(TEENSYDUINO)

#define HADirPin   10      // Pin 10 (Dir)
#define HA5vPin    11      // Pin 11 (5V?)
#define HAStepPin  12      // Pin 12 (Step)
                           // Pin GND (GND)

// Pins to enable/disable the stepper drivers and set microstep mode, optional and normally just hard-wired (DRV8825)/ignored (BED-A4988)
#define HA_M0      13      // Pin 13 (Microstep Mode 0)
#define HA_M1      14      // Pin 14 (Microstep Mode 1)
#define HA_M2      15      // Pin 15 (Microstep Mode 2)
#define HA_EN      16      // Pin 16 (Enabled when HIGH)

#define DE_M0      17      // Pin 17 (Microstep Mode 0)
#define DE_M1      18      // Pin 18 (Microstep Mode 1)
#define DE_M2      19      // Pin 19 (Microstep Mode 2)
#define DE_EN      20      // Pin 20 (Enabled when HIGH)

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

#endif

#define DecDirEInit      1
#define DecDirWInit      0
#define HADirNCPInit     0
#define HADirSCPInit     1
volatile byte DecDir     = DecDirEInit;
volatile byte HADir      = HADirNCPInit;

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

char   s[20];
double f,f1,f2,f3;
int    i,i1,i2;
byte   b;

// current site index and name
byte currentSite = 0; 
char siteName[16];

// align command
double altCor            = 0;       // for geometric coordinate correction/align, - is below the pole, + above
double azmCor            = 0;       // - is right of the pole, + is left
double doCor             = 0;       // declination orthogonal correction
double pdCor             = 0;       // declination/polar orthogonal correction
double IH                = 0;       // offset corrections/align
double ID                = 0;

// guide command
byte    guideDirHA        = 0;
int     guideDurationHA   = 0;
byte    guideDirDec       = 0;
int     guideDurationDec  = 0;

// Slew control
double  guideRates[9]={7.5,15,30,60,120,240,360,600,900}; 
//                      1x 2x 4x 8x  16x 24x 40x 60x
double  guideTimerRate    = 0;
//
long    msGuideHA         = 0;
long    amountGuideHA     = 0;
long    guideHA           = 0;
long    accGuideHA        = 0;
long    msGuideDec        = 0;
long    amountGuideDec    = 0;
long    guideDec          = 0;
//
unsigned long guideSiderealTimer=0;
unsigned long lstGuideStopHA=0;
unsigned long lstGuideStopDec=0;

// PEC control
#define PECStatusString  "IpPrR"
#define IgnorePEC        0
#define ReadyPlayPEC     1
#define PlayPEC          2
#define ReadyRecordPEC   3
#define RecordPEC        4
unsigned long PEC_Timer  = 0;        // for PEC, keeps time for applying steps
long PEC_Skip            = 0;        // for PEC, number of sidereal hundredths of a second between applying steps
long accPecPlayHA        = 0;        // for PEC, buffers steps to be played back
long accPecGuideHA       = 0;        // for PEC, buffers steps to be recorded
boolean PECfirstRecord   = false;
boolean PECstatus        = IgnorePEC;
boolean PECrecorded      = false;
long    PECindex_record  = 0;
double  PECstartDelta    = 0;
long    lastWormRotationStepPos = -1;
long    wormRotationStepPos = 0;
long    PECindex         = 0;
long    PECindex1        = 0;
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
  pinMode(LEDposPin, OUTPUT);
  CLR(LEDnegPORT, LEDnegBit);
  SET(LEDposPORT, LEDposBit);
#endif

// light second status LED (provides just GND)
#ifdef STATUS_LED2_PINS_ON
  pinMode(LEDneg2Pin, OUTPUT);       
  CLR(LEDneg2PORT, LEDneg2Bit);
  analogWrite(LEDneg2Pin,250);
#endif  

// provide 5V power to stepper drivers if requested
#ifdef POWER_SUPPLY_PINS_ON  
  pinMode(HA5vPin, OUTPUT);
  SET(HA5vPORT, HA5vBit);
  pinMode(Dec5vPin, OUTPUT);
  SET(Dec5vPORT, Dec5vBit);
#endif

// PEC index sense
#ifdef PEC_PIN_ON  
  pinMode(PecPin, INPUT);
#endif

// limit switch sense
#ifdef LIMIT_PIN_ON  
  pinMode(LimitPin, INPUT);
  digitalWrite(LimitPin, HIGH); // pull pin high
#endif

// disable the stepper drivers for now, if the enable lines are connected
  pinMode(HA_EN,OUTPUT); digitalWrite(HA_EN,HIGH);
  pinMode(DE_EN,OUTPUT); digitalWrite(DE_EN,HIGH);

// if the stepper driver mode select pins are wired in, program any requested micro-step mode
#ifdef HA_MODE
  pinMode(HA_M0, OUTPUT); digitalWrite(HA_M0,(HA_MODE & 1));
  pinMode(HA_M1, OUTPUT); digitalWrite(HA_M1,(HA_MODE>>1 & 1));
  pinMode(HA_M2, OUTPUT); digitalWrite(HA_M2,(HA_MODE>>2 & 1));
#endif
#ifdef DEC_MODE
  pinMode(DE_M0, OUTPUT); digitalWrite(DE_M0,(DE_MODE & 1));
  pinMode(DE_M1, OUTPUT); digitalWrite(DE_M1,(DE_MODE>>1 & 1));
  pinMode(DE_M2, OUTPUT); digitalWrite(DE_M2,(DE_MODE>>2 & 1));
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
    PECindex_record=0;
    EEPROM.write(EE_PECstatus,IgnorePEC);
    EEPROM.write(EE_PECrecorded,false);
    for (int l=0; l<PECBufferSize; l++) EEPROM.write(EE_PECindex+l,128);
    EEPROM_writeQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
    EEPROM_writeQuad(EE_PECsense_index,(byte*)&PECindex_sense);
    
    // init the Park status
    EEPROM.write(EE_parkSaved,false);
    EEPROM.write(EE_parkStatus,NotParked);
  
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
  SiderealRate    =siderealInterval/StepsPerSecond;
  timerRateHA     =SiderealRate;
  timerRateDec    =SiderealRate;

  // figure out how to issue steps at the StepsPerSecond rate
  unsigned int _st,_sk,_st1,_sk1; stepRateParameterGenerator(StepsPerSecond,&_st,&_sk,&_st1,&_sk1); st=_st; sk=_sk; st1=_st1; sk1=_sk1;

  // backlash takeup rates
  timerRateBacklashHA =timerRateHA /BacklashTakeupRate;
  timerRateBacklashDec=timerRateDec/BacklashTakeupRate;

  // initialize the timers that handle the sidereal clock, RA, and Dec
  Timer1SetRate(siderealInterval/100);
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
  EEPROM_readQuad(EE_sites+(currentSite)*25+0,(byte*)&latitude);
  if (latitude<0) celestialPoleDec=-90L; else celestialPoleDec=90L;
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  if (celestialPoleDec>0) HADir = HADirNCPInit; else HADir = HADirSCPInit;
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
  EEPROM_readQuad(EE_PECrecord_index,(byte*)&PECindex_record); 
  EEPROM_readQuad(EE_PECsense_index,(byte*)&PECindex_sense);
  
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
  //ut1Timer=millis();
  timeOutIndexDetect=millis();
  PECindexTimer = lst;
  siderealTimer = lst;
  sei();
}

void loop() {
  processCommands();
  
  // GUIDING -------------------------------------------------------------------------------------------
  // 1/100 second sidereal timer, controls issue of steps at the selected RA and/or Dec rate(s) 
  guideHA=0;

  cli(); long guideLst=lst; sei();
  if (guideLst-guideSiderealTimer>=1) {
    guideSiderealTimer=guideLst;
    if (trackingState==TrackingSidereal) { // only active while sidereal tracking with a guide rate that makes sense
      int sign=0;
      if (((guideLst%gr_st==0) && ((guideLst%gr_sk!=0) || ((guideLst%gr_st1==0) && (guideLst%gr_sk1!=0) )))) {
        if ((guideDirHA) && (!inBacklashHA)) { 
          // as above, and keep track of how much we've moved for PEC recording
          if (guideDirHA=='e') sign=-1; else sign=1; guideHA=sign*amountGuideHA;
          // for RA, only apply the corrections now if fast guiding; otherwise they get combined with PEC & sidereal-tracking and are applied later
          if (currentGuideRate>1) { cli(); targetHA+=guideHA;  sei(); } else { accGuideHA+=guideHA; }
        }
      }
      // for pulse guiding, count down the mS and stop when timed out
      if ((guideDurationHA>0) && (!inBacklashHA))  { guideDurationHA-=msGuideHA; if (guideDurationHA<=0) { lstGuideStopHA=lst+amountGuideHA; guideDirHA=0;  } }
      
      if (((guideLst%gd_st==0) && ((guideLst%gd_sk!=0) || ((guideLst%gd_st1==0) && (guideLst%gd_sk1!=0) )))) {
        if ((guideDirDec) && (!inBacklashDec)) { 
          // nudge the targetDec (where we're supposed to be) by amountMoveDec
          if (guideDirDec=='s') sign=-1; else sign=1; cli(); targetDec=targetDec+sign*amountGuideDec; sei();
        }
      }
      // for pulse guiding, count down the mS and stop when timed out
      if ((guideDurationDec>0) && (!inBacklashDec)) { guideDurationDec-=msGuideDec; if (guideDurationDec<=0) { lstGuideStopDec=lst+amountGuideDec; guideDirDec=0; } }
    }
    // allow the elevated rate to persist for a moment to allow the bulk added steps to play out after stopping
    if ((!guideDirHA) && (guideTimerRateHA!=0) && (lst>=lstGuideStopHA)) { cli(); guideTimerRateHA=0; sei(); }
    if ((!guideDirDec) && (guideTimerRateDec!=0) && (lst>=lstGuideStopDec)) { cli(); guideTimerRateDec=0; sei(); }
  }

  // PEC ---------------------------------------------------------------------------------------------
  // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense
  if ((trackingState==TrackingSidereal) && !((guideDirHA || guideDirDec) && (currentGuideRate>1))) {

    // keep track of our current step position, and when the step position on the worm wraps during playback
    lastWormRotationStepPos = wormRotationStepPos;
    cli(); long posPEC=targetHA; sei();
    
    // where we're at (HA), must always be positive, so add 360 degrees (in steps)
    posPEC=posPEC+StepsPerDegreeHA*360;
    wormRotationStepPos =(posPEC-PECindex_record) % StepsPerWormRotation;
    
    // handle playing back and recording PEC
    if (PECstatus!=IgnorePEC) {
      // start playing PEC
      if (PECstatus==ReadyPlayPEC) {
        // approximate, might be off by nearly a second, this makes sure that the index is at the start of a second before resuming play
        if ((long)fmod(wormRotationStepPos,StepsPerSecond)==0) {
          PECstatus=PlayPEC;
          PECindex=wormRotationStepPos/StepsPerSecond; 
          // sum corrections to this point
          long m=0; for (int l=0; l<PECindex; l++) {
            long l1=l-PECindex_sense; if (l1<0) l1+=SecondsPerWormRotation;
            m+=PEC_buffer[l1]-128; 
          }
          // move to the corrected location, this might take a few seconds and can be abrupt (after goto's or when unparking)
          cli(); PEC_HA = m; sei(); 
          PECstartDelta=abs(m)/StepsPerSecond; // in units of seconds, ahead or behind, all that matters is how long the rate has to be increased for

          // playback starts now
          cli(); PECindexTimer = lst; sei();
        }
      }

      // get ready to start recording
      if (PECstatus==ReadyRecordPEC) {
        // zero index counter
        PECindex_record = posPEC % StepsPerWormRotation;
        wormRotationStepPos = (posPEC-PECindex_record) % StepsPerWormRotation;
        lastWormRotationStepPos=1; // force recording to start now
      }

      // at the start of a worm cycle (if wormRotationStepPos rolled over)
      // this only works because targetHA can't move backward while tracking and guiding at +/- 1x sidereal
      // it also works while <=1X guiding and playing PEC because the PEC corrections don't get counted in the targetHA location
      if (lastWormRotationStepPos>wormRotationStepPos) {

        // start recording PEC
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
          long sum_pec=0; for (int scc=0; scc<SecondsPerWormRotation; scc++) { sum_pec+=(int)PEC_buffer[scc]-128; }

          // this is the correction coefficient for a given location in the sequence
          double Ccf = (double)sum_pec/(double)SecondsPerWormRotation;

          // now, apply the correction to the sequence to make the PEC adjustments null out
          // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
          long lp2=0; sum_pec=0; 
          for (int scc=0; scc<SecondsPerWormRotation; scc++) {
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
        
        // zero the accululators, the index timer, also the index
        accPecGuideHA = 0;
        accPecPlayHA = 0;
        accPecPlayHA = 0; cli(); PECindexTimer = lst-1; sei();  // keeps PECindex from advancing immediately
        PECindex = 0; lastPECindex = -1;                        // starts record/playback now
      #ifdef PEC_PIN_ON
        if (next_PECindex_sense>=0) { PECindex=next_PECindex_sense; next_PECindex_sense=-1; }
      #endif
      }

      // Increment the PEC index once a second and make it go back to zero when the worm finishes a rotation
      cli(); if ((lst-PECindexTimer)%100==0) PECindex=(PECindex+1)%SecondsPerWormRotation; sei();
      PECindex1=(PECindex-PECindex_sense); if (PECindex1<0) PECindex1+=SecondsPerWormRotation;

    #ifdef PEC_PIN_ON
      // if the HALL sensor (etc.) has just arrived at the index and it's been more than 60 seconds since
      // it was there before, set this as the next start of PEC playback/recording
      cli();
      if ((digitalRead(PecPin)==HIGH) && (lst-PECtime_lastSense>6000)) {
        PECtime_lastSense=lst;
        next_PECindex_sense=PECindex;
      }
      sei();
    #endif

      accPecGuideHA+=guideHA;
    } else {
      // if we're ignoring PEC, zero the offset and keep it that way
      cli(); PEC_HA=0; sei(); 
      PEC_Skip = 0;
      // no change to tracking rate
      pecTimerRateHA=0;
    }

    // falls in whenever the PECindex changes, which is once a sidereal second
    if (PECindex1!=lastPECindex) {
      lastPECindex=PECindex1;

      // assume no change to tracking rate
      pecTimerRateHA=0;
  
      if (PECstatus==RecordPEC) {
        // save the correction 1:2 weighted average
        int l=accPecGuideHA;
        if (l<-StepsPerSecond) l=-StepsPerSecond; if (l>StepsPerSecond) l=StepsPerSecond;   // +/-1 sidereal rate range for corrections
        if (!PECfirstRecord) l=(l+((int)PEC_buffer[PECindex1]-128)*2)/3; 
        PEC_buffer[PECindex1]=l+128;  // save the correction
        accPecGuideHA=0;              // and clear the accumulator
      }
      
      if (PECstatus==PlayPEC) {
        // PECindex2 adjusts one second before the value was recorded, an estimate of the latency between image acquisition and response
        // if sending values directly to OnStep from PECprep, etc. be sure to account for this
        int PECindex2=PECindex1-1; if (PECindex2<0) PECindex2+=SecondsPerWormRotation;
        // accPecPlayHA play back speed can be +/-1 of sidereal
        // PEC_Skip is the number of ticks between the added (or skipped) steps
        accPecPlayHA=PEC_buffer[PECindex2]-128;
        if (accPecPlayHA>StepsPerSecond) accPecPlayHA=StepsPerSecond; if (accPecPlayHA<-StepsPerSecond) accPecPlayHA=-StepsPerSecond;

        pecTimerRateHA=accPecPlayHA/StepsPerSecond;
        // And run the PEC rate at another +/-5x to cover any corrections that should have been applied up to this point in the worm cycle
        // when just (re)starting PEC not worried about how smooth the play back is here, this will be over in a moment just run fast to get there 
        if (PECstartDelta>0) { PECstartDelta-=30; pecTimerRateHA+=5; }
        
        if (accPecPlayHA==0) 
          PEC_Skip=SecondsPerWormRotation+1;
        else
          PEC_Skip=(long)StepsPerSecond/abs(accPecPlayHA);
        PEC_Timer=0;
      }
    }
  } else {
    // give up recording if we stop tracking at the sidereal rate
    if (PECstatus==RecordPEC)  { PECstatus=IgnorePEC; PEC_Skip=0; } // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
    // get ready to re-index when tracking comes back
    if (PECstatus==PlayPEC)  { PECstatus=ReadyPlayPEC; PEC_Skip=0; cli(); PEC_HA=0; sei(); } 
  }
  
  // SIDEREAL TRACKING ---------------------------------------------------------------------------------
  // moves the targetHA to match the earth's rotation/revolution, can handle up to 100 steps/sidereal second
  // siderealTimer, falls in once every 1/100 sidereal second (1 tick, about 10mS) if the main loop is so busy that this timer
  // is >1 tick you have a problem and the sidereal tracking may be affected.  Again, the slowest commands seem to take 6-7mS and it's
  // just fast enough to keep up - fortunately the new design only seriously robs MPU cycles during fast GoTo operations when this 
  // code isn't running

  // keeps the target where it's supposed to be while doing gotos
  // this depends on the lst%0 being a value other than 0, technically it's undefined
  cli(); long tempLst=lst; sei();
  if (tempLst-siderealTimer>=1) {
    siderealTimer=tempLst;
    if ((trackingState==TrackingMoveTo) && (tempLst%10==0)) {
      moveTo();
    } else {
      if ((trackingState==TrackingSidereal) && !(guideDirHA && (currentGuideRate>1))) { // only active while sidereal tracking with a guide rate that makes sense
        if (((tempLst%st==0) && ((tempLst%sk!=0) || ((tempLst%st1==0) && (tempLst%sk1!=0) )))) {
            // PEC_Timer starts at zero again every second, PEC_Skip will control the rate and will trigger a +/- step every PEC_Skip steps while tracking 
            PEC_Timer++;
    
            // lookup Guiding
            long ig=0; if (accGuideHA<0) { ig=-1; accGuideHA++; } else if (accGuideHA>0) { ig=1; accGuideHA--; }
    
            // lookup PEC
            long ip=0;
            if (!PEC_Skip==0) {
              if (PEC_Timer%PEC_Skip==0) {
                if (accPecPlayHA<0) { ip=-1; accPecPlayHA++; } else if (accPecPlayHA>0) { ip=1; accPecPlayHA--; } } }
    
            // apply the Tracking, Guiding, and PEC
            cli(); targetHA=targetHA+1+ig; PEC_HA+=ip; targetHA1=targetHA+PEC_HA; sei();
        }
      }
    }
  }
  
  // UT1 CLOCK -----------------------------------------------------------------------------------------
  // timer... falls in once a second, keeps the universal time clock ticking
  // this clock doesn't rollover (at 24 hours) since that would cause date confusion
//  unsigned long tempMilli=millis(); 
//  if (tempMilli-ut1Timer>999) {
//    ut1Timer=tempMilli;
  unsigned long m=millis();
  if (m%1000==0) {
    double t2=(double)(m-UT1mS_start)/1000.0;
    UT1=UT1_start+t2/3600.0;   // This just needs to be accurate to the nearest second, it's about 10x better
    
    // update the local sidereal time floating point representation
    update_LST();
    
    // safety checks, keeps mount from tracking past the meridian limit, past the underPoleLimit, below horizon limit, or above the overhead limit
    if (pierSide==PierSideWest) { cli(); if (posHA>(minutesPastMeridianW*StepsPerDegreeHA/4L)) if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  sei(); }
    if (pierSide==PierSideEast) { cli(); if (posHA>(underPoleLimit*StepsPerDegreeHA*15L)) if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;  sei(); }
    #ifdef LIMIT_PIN_ON  
    if (digitalRead(LimitPin)==LOW) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
    #endif
    if (do_alt_calc()) {  // low overhead altitude calculation, finishes updating once every 14 seconds
      if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

      // reset the sidereal clock once a day, to keep the significant digits from being consumed
      // I'm shooting for keeping OnStep reliable for about 50 days of continuous uptime (until millis() rolls over)
      // really working with a single and should have, just barely, enough significant digits to get us through a day
      unsigned long lst_now=lst_start+round( (double)((m-lst_mS_start)/10.0) * 1.00273790935);
      if ((lst_now-lst_start)>24*3600*100) update_lst();

     // finally, adjust the tracking rate on-the-fly to compensate for refraction
     if (refraction) CEquToTracRateCor();
    }
  } else {
    processCommands();
  }
}
