// -----------------------------------------------------------------------------------
// Configuration for legacy OnStep "Alternate pin-map" (Mega2560)
// *** this is for backwards compatability with older OnStep systems ***

/*
 * For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep and 
 * join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 * See the Pins.Mega2560Alt.h file for detailed information on this pin map to be sure it matches your wiring *** USE AT YOUR OWN RISK ***
 *
*/

#define Mega2560Alt_OFF   //  <- Turn _ON to use this configuration

#ifdef Mega2560Alt_ON
// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// Default speed for Serial1 com port, Default=9600
#define SERIAL_B_BAUD_DEFAULT 9600

// Mount type, default is _GEM (German Equatorial) other options are _FORK, _FORK_ALT.  _FORK switches off Meridian Flips after (1, 2 or 3 star) alignment is done.  _FORK_ALT disables Meridian Flips (1 star align.)
// _ALTAZM is for Alt/Azm mounted 'scopes (1 star align only.)
#define MOUNT_TYPE_GEM

// ST4 interface on Pins 47, 43, 45, 49.  Pin 47 is RA- (West), Pin 43 is Dec- (South), Pin 45 is Dec+ (North), Pin 49 is RA+ (East.)
// ST4_ON enables the interface.  ST4_PULLUP enables the interface and any internal pullup resistors.  default=_OFF
// It is up to you to create an interface that meets the electrical specifications of any connected device, use at your own risk.
#define ST4_OFF
// If SEPARATE_PULSE_GUIDE_RATE_ON is used the ST4 port is limited to guide rates <= 1X except when ST4_HAND_CONTROL_ON is used.
// Additionally, ST4_HAND_CONTROL_ON enables special features: Press and hold [E]+[W] buttons for > 2 seconds...  In this mode [E] decreases and [W] increases guide rates (or if tracking isn't on yet adjusts illuminated recticule brightness.)
// [S] for Sync (or Accept if in align mode.) [N] for Tracking on/off. -OR- Press and hold [N]+[S] buttons for > 2 seconds...  In this mode [E] selects prior and [W] next user catalog item.
// [N] to do a Goto to the catalog item.  [S] for Sound on/off.  The keypad returns to normal operation after 4 seconds of inactivity.  ST4_HAND_CONTROL_ON also adds a 100ms de-bounce to all button presses.
// Finally, during a goto pressing any button aborts the slew.  If meridian flip paused at home, pressing any button continues.  default=_ON
#define ST4_HAND_CONTROL_ON

// use separate pulse-guide rate so centering and guiding don't disturb each other, default=_ON
#define SEPARATE_PULSE_GUIDE_RATE_ON

// Guide time limit (in seconds,) default=0 (no limit.)  A safety feature, some guides are started with one command and stopped with another.  
// If the stop command is never received the guide will continue forever unless this is enabled.
#define GUIDE_TIME_LIMIT 0

// RTC (Real Time Clock) support, default=_OFF. Other options: RTC_DS3234 for a DS3234 on the default SPI interface pins (and CS on pin 10, optionally wire the SQW output to the PPS pin below.)
#define RTC_OFF
// PPS use _ON or _PULLUP to enable the input and use the built-in pullup resistor.  Sense rising edge on pin 21 for optional precision clock source (GPS, for example), default=_OFF
#define PPS_SENSE_OFF

// PEC sense on Pin 2 use _ON or _PULLUP to enable the input/use the built-in pullup resistor (digital input) or provide a comparison value (see below) for Analog operation (on Pin A1,) default=_OFF
// Analog values range from 0 to 1023 which indicate voltages from 0-3.3VDC on the analog pin, for example "PEC_SENSE 600" would detect an index when the voltage exceeds 2.92V
// With either index detection method, once triggered 60s must expire before another detection can happen.  This gives time for the index magnet to pass by the detector before another cycle begins.
// Ignored on Alt/Azm mounts.
#define PEC_SENSE_OFF
// PEC sense, rising edge (default with PEC_SENSE_STATE HIGH, use LOW for falling edge, ex. PEC_SENSE_ON) ; for optional PEC index
#define PEC_SENSE_STATE HIGH

// switch close (to ground) on Pin 3 for optional limit sense (stops gotos and/or tracking), default=_OFF
#define LIMIT_SENSE_OFF

// light status LED by sink to ground (Pin 9) and source +5V (Pin 8), default=_ON.
// _ON and OnStep keeps this illuminated to indicate that the controller is active.  When sidereal tracking this LED will rapidly flash
#define STATUS_LED_PINS_ON
// lights 2nd status LED by sink to ground (Pin 7), default=_OFF.
// _ON sets this to blink at 1 sec intervals when PPS is synced
#define STATUS_LED2_PINS_OFF
// lights reticule LED by sink to ground (Pin 44), default=_OFF.
// RETICULE_LED_PINS n, where n=0 to 255 activates this feature and sets default brightness
#define RETICULE_LED_PINS_OFF

// Sound/buzzer on Pin 42, default=_OFF.
// Specify frequency for a piezo speaker (for example "BUZZER 2000") or use BUZZER_ON for a piezo buzzer.
#define BUZZER_OFF
// Sound state at startup, default=_ON.
#define DEFAULT_SOUND_ON

// optionally adjust tracking rate to compensate for atmospheric refraction, default=_OFF (limited testing done)
// can be turned on/off with the :Te# and :Td# commands regardless of this setting
#define TRACK_REFRACTION_RATE_DEFAULT_OFF

// Set to _OFF and OnStep will allow Syncs to change pier side for GEM mounts (on/off), default=_ON
#define SYNC_CURRENT_PIER_SIDE_ONLY_ON

// Set to _ON and OnStep will remember the last auto meridian flip setting (on/off), default=_OFF
#define REMEMBER_AUTO_MERIDIAN_FLIP_OFF

// Set to _ON and OnStep will travel directly across a meridian flip without visiting the home position (on/off), default=_OFF (only applies if pause at home is disabled)
#define MERIDIAN_FLIP_SKIP_HOME_OFF

// Set to _ON and OnStep will remember the last meridian flip pause at home setting (on/off), default=_OFF
#define REMEMBER_PAUSE_HOME_OFF

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define REMEMBER_MAX_RATE_OFF        // set to _ON and OnStep will remember rates set in the ASCOM driver, Android App, etc. default=_OFF 
#define MaxRate                   96 // microseconds per microstep default setting for gotos, can be adjusted for two times lower or higher at run-time
                                     // minimum* (fastest goto) is around 32 (Mega2560,) default=96 higher is ok
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
                                     
                                     // PEC, number of steps for a complete worm rotation (in RA), (StepsPerDegreeAxis1*360)/gear_reduction2.  Ignored on Alt/Azm mounts.
#define StepsPerWormRotationAxis1 12800L
                                     // G11              : (12800*360)/360 = 12800

#define PECBufferSize           824  // PEC, buffer size, max should be no more than 3384, your required buffer size >= StepsPerAxis1WormRotation/(StepsPerDegeeAxis1/240)
                                     // for the most part this doesn't need to be changed, but adjust when needed.  824 seconds is the default.  Ignored on Alt/Azm mounts.

#define MinutesPastMeridianE      30 // for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - a half hour of RA is the default = 30.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
#define MinutesPastMeridianW      30 // as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
                                     // The above two lines can be removed and settings in EEPROM will be used instead, be sure to set the Meridian limits in control software if you do this!  
                                     // If you don't remove these lines Meridian limits will return to these defaults on power up.
#define UnderPoleLimit            12 // maximum allowed hour angle (+/-) under the celestial pole. OnStep will flip the mount and move the Dec. >90 degrees (+/-) once past this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
                                     // If left alone, the mount will stop tracking when it hits this limit.  Valid range is 7 to 11 hours.
#define MinDec                   -91 // minimum allowed declination, default = -91 (off)  Ignored on Alt/Azm mounts.
#define MaxDec                   +91 // maximum allowed declination, default =  91 (off)  Ignored on Alt/Azm mounts.
                                     // For example, a value of +80 would stop gotos/tracking near the north celestial pole.
                                     // For a Northern Hemisphere user, this would stop tracking when the mount is in the polar home position but
                                     // that can be easily worked around by doing an alignment once and saving a park position (assuming a 
                                     // fork/yoke mount with meridian flips turned off by setting the minutesPastMeridian values to cover the whole sky)
#define MaxAzm                   180 // Alt/Az mounts only. +/- maximum allowed Azimuth, default =  180.  Allowed range is 180 to 360

// AXIS1/2 STEPPER DRIVER CONTROL ------------------------------------------------------------------------------------------
// Axis1: Pins 35,37 = Step,Dir (RA/Azm)
// Axis2: Pins 24,22 = Step,Dir (Dec/Alt)

// Reverse the direction of movement.  Adjust as needed or reverse your wiring so things move in the right direction
#define AXIS1_REVERSE_OFF            // RA/Azm axis
#define AXIS2_REVERSE_OFF            // Dec/Alt axis

// Stepper driver Enable support is always on, just wire Enable to Pins 23 (Axis1) and 36 (Axis2) and OnStep will pull these HIGH
// to disable stepper drivers on startup and when Parked or Homed.  An Align or UnPark will enable the drivers.  Adjust below if you need these pulled LOW to disable the drivers.
#define AXIS1_DISABLE HIGH
#define AXIS2_DISABLE HIGH

// For equatorial mounts, _ON powers down the Declination axis when it's not being used to help lower power use.  During low rate guiding (<=1x) the axis stays enabled
// for 10 minutes after any guide on either axis.  Otherwise, the Dec axis is disabled (powered off) 10 seconds after movement stops.
#define AXIS2_AUTO_POWER_DOWN_OFF

// Basic stepper driver mode setup . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// If used, this requires connections M0, M1, and M2 on Pins 29,27,25 for Axis1 (RA/Azm) and Pins 30,32,34 for Axis2 (Dec/Alt).
// Stepper driver models are as follows: (for example AXIS1_DRIVER_MODEL DRV8825,) A4988, LV8729, RAPS128, S109, ST820, TMC2208, TMC2130 (spreadCycle,) 
// TMC2130_QUIET (stealthChop tracking,) TMC2130_VQUIET (full stealthChop mode,) add _LOWPWR for 50% power during tracking (for example: TMC2130_QUIET_LOWPWR)
#define AXIS1_DRIVER_MODEL_OFF      // Axis1 (RA/Azm):  Default _OFF, Stepper driver model (see above)
#define AXIS1_MICROSTEPS_OFF        // Axis1 (RA/Azm):  Default _OFF, Microstep mode when the scope is doing sidereal tracking (for example: AXIS1_MICROSTEPS 32)
#define AXIS1_MICROSTEPS_GOTO_OFF   // Axis1 (RA/Azm):  Default _OFF, Optional microstep mode used during gotos (for example: AXIS1_MICROSTEPS_GOTO 2)
#define AXIS2_DRIVER_MODEL_OFF      // Axis2 (Dec/Alt): Default _OFF, Stepper driver model (see above)
#define AXIS2_MICROSTEPS_OFF        // Axis2 (Dec/Alt): Default _OFF, Microstep mode when the scope is doing sidereal tracking
#define AXIS2_MICROSTEPS_GOTO_OFF   // Axis2 (Dec/Alt): Default _OFF, Optional microstep mode used during gotos
// Note: you can replace this section with the contents of "AdvancedStepperSetup.txt" . . . . . . . . . . . . . . . . . . . 

// Stepper driver Fault detection, just wire the driver Fault signal to Pins 39 (Axis1) and 38 (Axis2), default=_OFF.  The SPI interface (on M0/M1/M2/Aux) can be used to detect errors on the TMC2130.
// other settings are LOW, HIGH, TMC2130 (if available applies internal pullup if LOW and pulldown if HIGH.)
#define AXIS1_FAULT_OFF
#define AXIS2_FAULT_OFF

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 2
#include "src/pinmaps/Pins.Mega2560Alt.h"
#endif

