// -----------------------------------------------------------------------------------
// Configuration for legacy OnStep Classic (Mega2560 or Teensy3.x)
// *** this is for backwards compatability with older OnStep systems ***

/*
 * For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep and 
 * join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 * See the Pins.Classic.h file for detailed information on this pin map to be sure it matches your wiring *** USE AT YOUR OWN RISK ***
 *
*/

#define Classic_OFF   //  <- Turn _ON to use this configuration

#ifdef Classic_ON
// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// Default speed for Serial1 com port, Default=9600
#define SERIAL_B_BAUD_DEFAULT 9600

// Mount type, default is _GEM (German Equatorial) other options are _FORK, _FORK_ALT.  _FORK switches off Meridian Flips after (1, 2 or 3 star) alignment is done.  _FORK_ALT disables Meridian Flips (1 star align.)
// _ALTAZM is for Alt/Azm mounted 'scopes (1 star align only.)
#define MOUNT_TYPE_GEM

// Strict parking, default=_OFF.  Set to _ON and unparking is only allowed if successfully parked.  Otherwise unparking is allowed if at home and not parked (the Home/Reset command ":hF#" sets this state.) 
#define STRICT_PARKING_OFF

// ST4 interface on pins 47, 49, 51, 53.  Pin 47 is RA- (West), Pin 49 is Dec- (South), Pin 51 is Dec+ (North), Pin 53 is RA+ (East.)  Teensy3.x pins 24, 25, 26, 27.
// ST4_ON enables the interface.  ST4_PULLUP enables the interface and any internal pullup resistors.
// It is up to you to create an interface that meets the electrical specifications of any connected device, use at your own risk.  default=_OFF
#define ST4_OFF
// If SEPARATE_PULSE_GUIDE_RATE_ON is used the ST4 port is limited to guide rates <= 1X except when ST4_HAND_CONTROL_ON is used.
// Additionally, ST4_HAND_CONTROL_ON enables special features: Press and hold [E]+[W] buttons for > 2 seconds...  In this mode [E] decreases and [W] increases guide rates (or if tracking isn't on yet adjusts illuminated recticule brightness.)
// [S] for Sync (or Accept if in align mode.) [N] for Tracking on/off. -OR- Press and hold [N]+[S] buttons for > 2 seconds...  In this mode [E] selects prior and [W] next user catalog item.
// [N] to do a Goto to the catalog item.  [S] for Sound on/off.  The keypad returns to normal operation after 4 seconds of inactivity.  ST4_HAND_CONTROL_ON also adds a 100ms de-bounce to all button presses.
// Finally, during a goto pressing any button aborts the slew.  If meridian flip paused at home, pressing any button continues.  default=_ON
#define ST4_HAND_CONTROL_OFF
// ST4_ALTERNATE_PINS_ON moves the interface (Mega2560 only) to pins 43, 45, 47, 49.  Pin 43 is Dec- (South), Pin 45 is Dec+ (North), Pin 47 is RA- (West), Pin 49 is RA+ (East.)
// ST4_ALTERNATE_PINS_ON is required for Steve's ST4 board and is also required if the ST4 interface is to be used alongside the SPI interface.
#define ST4_ALTERNATE_PINS_OFF

// Separate pulse-guide rate so centering and guiding don't disturb each other, default=_ON
#define SEPARATE_PULSE_GUIDE_RATE_ON

// Guide time limit (in seconds,) default=0 (no limit.)  A safety feature, some guides are started with one command and stopped with another.  
// If the stop command is never received the guide will continue forever unless this is enabled.
#define GUIDE_TIME_LIMIT 0

// RTC (Real Time Clock) support, default=_OFF. Other options: RTC_DS3234 for a DS3234 on the default SPI interface (and CS on pin 10, optionally wire the SQW output to the PPS pin below.)
#define RTC_OFF
// PPS use _ON or _PULLUP to enable the input and use the built-in pullup resistor.  Sense rising edge on pin 21 for optional precision clock source (GPS, for example), default=_OFF (Teensy3.x Pin 23)
#define PPS_SENSE_OFF

// PEC sense on pin 2 or threshold value on Analog 1 (Analog 14 on Teensy3.x) use _ON or _PULLUP to enable the input and use the built-in pullup resistor, default=_OFF
// analog values range from 0 to 1023 which indicate voltages from 0-5VDC (or 0-3.3VDC) on the analog pin, for example "PEC_SENSE 600" would detect an index when the voltage exceeds 2.92V (or 1.93V)
// with either index detection method, once triggered 60s must expire before another detection can happen.  This gives time for the index magnet to pass by the detector before another cycle begins.
// Ignored on Alt/Azm mounts.
#define PEC_SENSE_OFF
// PEC sense, rising edge (default with PEC_SENSE_STATE HIGH, use LOW for falling edge) on pin 2 (ex. PEC_SENSE_ON) ; for optional PEC index
#define PEC_SENSE_STATE HIGH

// switch close (to ground) on pin 3 for optional limit sense (stops gotos and/or tracking), default=_OFF
#define LIMIT_SENSE_OFF

// Light status LED by sink to ground (pin 9) and source +5V (pin 8), default=_ON
// _ON and OnStep keeps this illuminated to indicate that the controller is active.  When sidereal tracking this LED will rapidly flash
#define STATUS_LED_PINS_OFF
// Light 2nd status LED by sink to ground (pin 10), default=_OFF, must be off for Teensy3.x (pin 7)
// _ON sets this to blink at 1 sec intervals when PPS is synced
// n sets this to dimly light a polar finder reticle, for example I use STATUS_LED2_PINS 250
// The default SPI device CS is at pin 10 also, so if the SPI interface is used this must be _OFF
#define STATUS_LED2_PINS_OFF
// Light reticule LED by sink to ground (pin 44), default=_OFF.  Defaults to pin 9 on the Teensy3.x (STATUS_LED_PINS must be _ON)
// RETICULE_LED_PINS n, where n=0 to 255 activates this feature and sets default brightness
#define RETICULE_LED_PINS_OFF

// Sound/buzzer on pin 42, default=_OFF.  (Teensy3.x Pin 29)
// Specify frequency for a piezo speaker (for example "BUZZER 2000") or use BUZZER_ON for a piezo buzzer.
#define BUZZER_OFF
// Sound state at startup, default=_ON.
#define DEFAULT_SOUND_ON

// Optionally adjust tracking rate to compensate for atmospheric refraction, default=_OFF
// can be turned on/off with the :Te# and :Td# commands regardless of this setting
#define TRACK_REFRACTION_RATE_DEFAULT_OFF

// Set to _OFF and OnStep will allow Syncs to change pier side for GEM mounts (on/off), default=_ON
#define SYNC_CURRENT_PIER_SIDE_ONLY_ON

// Set to _ON and OnStep will remember the last auto meridian flip setting (on/off), default=_OFF
#define REMEMBER_AUTO_MERIDIAN_FLIP_OFF

// Set to _ON and OnStep will remember the last meridian flip pause at home setting (on/off), default=_OFF
#define REMEMBER_PAUSE_HOME_OFF

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define REMEMBER_MAX_RATE_OFF        // set to _ON and OnStep will remember rates set in the ASCOM driver, Android App, etc. default=_OFF
#define MaxRate                   96 // microseconds per microstep default setting for gotos, can be adjusted for two times lower or higher at run-time
                                     // minimum* (fastest goto) is around 32 (Mega2560,) 16 (Teensy3.2,) 12 (Teensy3.5,) 4 (Teensy3.6,) default=96 higher is ok
                                     // * = minimum can be lower, when both AXIS1/AXIS2_MICROSTEPS are used the compiler will warn you if it's too low

#define DegreesForAcceleration   5.0 // approximate number of degrees for full acceleration or deceleration: higher values=longer acceleration/deceleration
                                     // Default=5.0, too low (about <1) can cause gotos to never end if micro-step mode switching is enabled.
#define DegreesForRapidStop      1.0 // approximate number of degrees required to stop when requested or if limit is exceeded during a slew: higher values=longer deceleration
                                     // Default=1.0, too low (about <1) can cause gotos to never end if micro-step mode switching is enabled.

#define GUIDES_DISABLE_BACKLASH_OFF  // Set to _ON to disable backlash takeup during guiding at <= 1X, default=_OFF
#define BacklashTakeupRate        25 // backlash takeup rate (in multipules of the sidereal rate): too fast and your motors will stall,
                                     // too slow and the mount will be sluggish while it moves through the backlash
                                     // for the most part this doesn't need to be changed, but adjust when needed.  Default=25

                                     // Axis1 is for RA/Az
#define StepsPerDegreeAxis1  12800.0 // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // G11              :  400           * 32          * 1               *  360/360              = 12800
                                     // Axis2 is for Dec/Alt
#define StepsPerDegreeAxis2  12800.0 // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // G11              :  400           * 32          * 1               *  360/360              = 12800
                                     // the EM10b has two spur gears that drive the RA/Dec worms, they give an additional 1.25:1 reduction
                                     // in addition to the 18:1 gear heads on the steppers for a 22.5:1 final ratio before the worm/wheels at 144:1
                                     
                                     // PEC, number of steps for a complete worm rotation (in RA), (StepsPerDegreeAxis1*360)/gear_reduction2.  Ignored on Alt/Azm mounts.
#define StepsPerWormRotationAxis1 12800L
                                     // G11              : (12800*360)/360 = 12800

#define PECBufferSize           824  // PEC, buffer size, max should be no more than 3384, your required buffer size >= StepsPerAxis1WormRotation/(StepsPerDegeeAxis1/240)
                                     // for the most part this doesn't need to be changed, but adjust when needed.  824 seconds is the default.  Ignored on Alt/Azm mounts.

#define MinutesPastMeridianE      30 // for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
#define MinutesPastMeridianW      30 // as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
                                     // The above two lines can be removed and settings in EEPROM will be used instead, be sure to set the Meridian limits in control software if you do this!  
                                     // If you don't remove these lines Meridian limits will return to these defaults on power up.
#define UnderPoleLimit            12 // maximum allowed hour angle (+/-) under the celestial pole.  Default=12.  Ignored on Alt/Azm mounts.
                                     // If left alone, the mount will stop tracking when it hits this limit.  Valid range is 10 to 12 hours.
#define MinDec                   -91 // minimum allowed declination, default = -91 (off)  Ignored on Alt/Azm mounts.
#define MaxDec                   +91 // maximum allowed declination, default =  91 (off)  Ignored on Alt/Azm mounts.
                                     // For example, a value of +80 would stop gotos/tracking near the north celestial pole.
                                     // For a Northern Hemisphere user, this would stop tracking when the mount is in the polar home position but
                                     // that can be easily worked around by doing an alignment once and saving a park position (assuming a 
                                     // fork/yoke mount with meridian flips turned off by setting the minutesPastMeridian values to cover the whole sky)
#define MaxAzm                   180 // Alt/Az mounts only. +/- maximum allowed Azimuth, default =  180.  Allowed range is 180 to 360
                                     // Axis3 is optional and for a rotator/de-rotator

// AXIS1/2 STEPPER DRIVER CONTROL ------------------------------------------------------------------------------------------
// Axis1: Pins 13,11 = Step,Dir (RA/Azm) (Teensy3.x Pins 12,10)
// Axis2: Pins 6,4 = Step,Dir (Dec/Alt)  (Teensy3.x Pins 6,4)

// Reverse the direction of movement.  Adjust as needed or reverse your wiring so things move in the right direction
#define AXIS1_REVERSE_OFF            // RA/Azm axis
#define AXIS2_REVERSE_OFF            // Dec/Alt axis

// Stepper driver Enable support, just wire Enable to Pins 25 (Axis1) and 30 (Axis2) and OnStep will pull these HIGH to disable the stepper drivers on startup and when Parked or Homed. (Teensy3.x Pins 16,21)
// An Align, Sync, or Un-Park will enable the drivers.  Adjust below if you need these pulled LOW to disable the drivers.
#define AXIS1_DISABLE HIGH
#define AXIS2_DISABLE HIGH

// For equatorial mounts, _ON powers down the Declination axis when it's not being used to help lower power use.  During low rate guiding (<=1x) the axis stays enabled
// for 10 minutes after any guide on either axis.  Otherwise, the Dec axis is disabled (powered off) 10 seconds after movement stops.
#define AXIS2_AUTO_POWER_DOWN_OFF

// Basic stepper driver mode setup . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// If used, this requires connections M0, M1, and M2 on Pins 22,23,24 for Axis1 (RA/Azm) (Teensy3.x Pins 13,14,15.)  M0, M1, M2 are on Pins 27,28,29 for Axis2 (Dec/Alt) (Teensy3.x Pins 18,19,20.)
// Stepper driver models are as follows: (for example AXIS1_DRIVER_MODEL DRV8825,) A4988, LV8729, RAPS128, S109, ST820, TMC2208, TMC2130 (spreadCycle,) 
// TMC2130_QUIET (stealthChop tracking,) TMC2130_VQUIET (full stealthChop mode,) add _LOWPWR for 50% power during tracking (for example: TMC2130_QUIET_LOWPWR)
#define AXIS1_DRIVER_MODEL_OFF      // Axis1 (RA/Azm):  Default _OFF, Stepper driver model (see above)
#define AXIS1_MICROSTEPS_OFF        // Axis1 (RA/Azm):  Default _OFF, Microstep mode when the scope is doing sidereal tracking (for example: AXIS1_MICROSTEPS 32)
#define AXIS1_MICROSTEPS_GOTO_OFF   // Axis1 (RA/Azm):  Default _OFF, Optional microstep mode used during gotos (for example: AXIS1_MICROSTEPS_GOTO 2)
#define AXIS2_DRIVER_MODEL_OFF      // Axis2 (Dec/Alt): Default _OFF, Stepper driver model (see above)
#define AXIS2_MICROSTEPS_OFF        // Axis2 (Dec/Alt): Default _OFF, Microstep mode when the scope is doing sidereal tracking
#define AXIS2_MICROSTEPS_GOTO_OFF   // Axis2 (Dec/Alt): Default _OFF, Optional microstep mode used during gotos
// Note: you can replace this section with the contents of "AdvancedStepperSetup.txt" . . . . . . . . . . . . . . . . . . . 

// Stepper driver Fault detection
// just wire driver Fault signal to Pins 26 (Axis1) and 31 (Axis2), default=_OFF (Teensy3.x Pins 17,22) other settings are LOW and HIGH (if available applies internal pullup if LOW and pulldown if HIGH.)
#define AXIS1_FAULT_OFF
#define AXIS2_FAULT_OFF

// ------------------------------------------------------------------------------------------------------------------------
// CAMERA ROTATOR OR ALT/AZ DE-ROTATION -----------------------------------------------------------------------------------
// A9,A8 = Step,Dir (Teensy3.x 30,33 choose either this option or the second focuser, not both)
#define ROTATOR_OFF                  // use _ON to enable the rotator (for any mount type,) default=_OFF (this is also a de-rotator for MOUNT_TYPE_ALTAZM mounts.)
#define MaxRateAxis3               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerDegreeAxis3     64.0 // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Rotator          :  24            * 8           * 20              *  6/360                = 64
                                     // For de-rotation of Alt/Az mounts a quick estimate of the required resolution (in StepsPerDegree)
                                     // would be an estimate of the circumference of the useful imaging circle in (pixels * 2)/360
#define MinAxis3                -180 // minimum allowed Axis3 rotator, default = -180
#define MaxAxis3                 180 // maximum allowed Axis3 rotator, default =  180
#define AXIS3_REVERSE_OFF            // reverse the direction of Axis3 rotator movement

// FOCUSER1 ---------------------------------------------------------------------------------------------------------------
// A11,A10 = Step,Dir (Teensy3.2 pins 31,32) (Teensy3.5/3.6 pins 34,35)
#define FOCUSER1_OFF                 // use _ON to enable this focuser, default=_OFF
#define MaxRateAxis4               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerMicrometerAxis4  0.5 // figure this out by testing or other means
#define MinAxis4               -25.0 // minimum allowed Axis4 position in millimeters, default = -25.0
#define MaxAxis4                25.0 // maximum allowed Axis4 position in millimeters, default =  25.0
#define AXIS4_REVERSE_OFF            // reverse the direction of Axis4 focuser movement

// FOCUSER2 ---------------------------------------------------------------------------------------------------------------
// A13,A12 = Step,Dir (Teensy3.x pins 30,33 choose either this option or the rotator, not both) 
#define FOCUSER2_OFF                 // use _ON to enable this focuser, default=_OFF
#define MaxRateAxis5               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerMicrometerAxis5  0.5 // figure this out by testing or other means
#define MinAxis5               -25.0 // minimum allowed Axis5 position in millimeters, default = -25.0
#define MaxAxis5                25.0 // maximum allowed Axis5 position in millimeters, default =  25.0
#define AXIS5_REVERSE_OFF            // reverse the direction of Axis4 focuser movement

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 2
#include "src/pinmaps/Pins.Classic.h"
#endif

