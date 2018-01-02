// -----------------------------------------------------------------------------------
// Configuration for legacy OnStep "Alternate pin-map" (Mega2560)

/*
 * For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep and 
 * join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 * See the Pins.MiniPCB.h file for detailed information on this pin map to be sure it matches your wiring *** USE AT YOUR OWN RISK ***
 *
*/

#define Mega2560Alt_OFF   //  <- Turn _ON to use this configuration

#ifdef Mega2560Alt_ON
// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// Default speed for Serial1 com port, Default=9600
#define SERIAL1_BAUD_DEFAULT 9600

// Mount type, default is _GEM (German Equatorial) other options are _FORK, _FORK_ALT.  _FORK switches off Meridian Flips after (1, 2 or 3 star) alignment is done.  _FORK_ALT disables Meridian Flips (1 star align.)
// _ALTAZM is for Alt/Azm mounted 'scopes (1 star align only.)
#define MOUNT_TYPE_GEM

// ST4 interface on Pins 47, 43, 45, 49.  Pin 47 is RA- (West), Pin 43 is Dec- (South), Pin 45 is Dec+ (North), Pin 49 is RA+ (East.)
// ST4_ON enables the interface.  ST4_PULLUP enables the interface and any internal pullup resistors.  default=OFF
// It is up to you to create an interface that meets the electrical specifications of any connected device, use at your own risk.
#define ST4_OFF
// If SEPARATE_PULSE_GUIDE_RATE_ON is used the ST4 port is limited to guide rates <= 1X except when ST4_HAND_CONTROL_ON is used.
// Additionally, ST4_HAND_CONTROL_ON enables special features: Press and hold [E]+[W] buttons for > 2 seconds...  In this mode [E] decreases and [W] increases guide rates (or if tracking isn't on yet adjusts illuminated recticule brightness.)
// [S] for Sync (or Accept if in align mode.) [N] for Tracking on/off. -OR- Press and hold [N]+[S] buttons for > 2 seconds...  In this mode [E] selects prior and [W] next user catalog item.
// [N] to do a Goto to the catalog item.  [S] for Sound on/off.  The keypad returns to normal operation after 4 seconds of inactivity.  ST4_HAND_CONTROL_ON also adds a 100ms de-bounce to all button presses.
#define ST4_HAND_CONTROL_ON

// use separate pulse-guide rate so centering and guiding don't disturb each other, default=ON
#define SEPARATE_PULSE_GUIDE_RATE_ON

// Guide time limit (in seconds,) default=0 (no limit.)  A safety feature, some guides are started with one command and stopped with another.  
// If the stop command is never received the guide will continue forever unless this is enabled.
#define GUIDE_TIME_LIMIT 0

// RTC (Real Time Clock) support, Default=OFF. Other options: RTC_DS3234 for a DS3234 on the default SPI interface pins (and CS on pin 10, optionally wire the SQW output to the PPS pin below.)
#define RTC_OFF
// PPS use _ON or _PULLUP to enable the input and use the built-in pullup resistor.  Sense rising edge on pin 21 for optional precision clock source (GPS, for example), default=OFF
#define PPS_SENSE_OFF

// PEC sense on Pin 2 use _ON or _PULLUP to enable the input/use the built-in pullup resistor (digital input) or provide a comparison value (see below) for Analog operation (on Pin A1,) default=OFF
// Analog values range from 0 to 1023 which indicate voltages from 0-3.3VDC on the analog pin, for example "PEC_SENSE 600" would detect an index when the voltage exceeds 2.92V
// With either index detection method, once triggered 60s must expire before another detection can happen.  This gives time for the index magnet to pass by the detector before another cycle begins.
// Ignored on Alt/Azm mounts.
#define PEC_SENSE_OFF
// PEC sense, rising edge (default with PEC_SENSE_STATE HIGH, use LOW for falling edge, ex. PEC_SENSE_ON) ; for optional PEC index
#define PEC_SENSE_STATE HIGH

// switch close (to ground) on Pin 3 for optional limit sense (stops gotos and/or tracking), default=OFF
#define LIMIT_SENSE_OFF

// light status LED by sink to ground (Pin 9) and source +5V (Pin 8), default=ON.
// _ON and OnStep keeps this illuminated to indicate that the controller is active.  When sidereal tracking this LED will rapidly flash
#define STATUS_LED_PINS_ON
// lights 2nd status LED by sink to ground (Pin 7), default=OFF.
// _ON sets this to blink at 1 sec intervals when PPS is synced
#define STATUS_LED2_PINS_OFF
// lights reticule LED by sink to ground (Pin 44), default=OFF.
// RETICULE_LED_PINS n, where n=0 to 255 activates this feature and sets default brightness
#define RETICULE_LED_PINS_OFF

// Sound/buzzer on Pin 42, default=OFF.
// Specify frequency for a piezo speaker (for example "BUZZER 2000") or use BUZZER_ON for a piezo buzzer.
#define BUZZER_OFF
// Sound state at startup, default=ON.
#define DEFAULT_SOUND_ON

// optionally adjust tracking rate to compensate for atmospheric refraction, default=OFF (limited testing done)
// can be turned on/off with the :Te# and :Td# commands regardless of this setting
#define TRACK_REFRACTION_RATE_DEFAULT_OFF

// Set to ON and OnStep will remember the last auto meridian flip setting (on/off), default=OFF
#define REMEMBER_AUTO_MERIDIAN_FLIP_OFF

// Set to ON and OnStep will remember the last meridian flip pause at home setting (on/off), default=OFF
#define REMEMBER_PAUSE_HOME_OFF

// ADJUST THE FOLLOWING TO MATCH YOUR MOUNT --------------------------------------------------------------------------------
#define REMEMBER_MAX_RATE_OFF        // set to ON and OnStep will remember rates set in the ASCOM driver or Android App, default=OFF 
#define MaxRate                   96 // this is the minimum number of micro-seconds between micro-steps
                                     // minimum* (fastest goto) is around 32, default=96 higher is ok
                                     // too low and OnStep communicates slowly and/or freezes as the motor timers use up all the MCU time
                                     // * = minimum can be lower, when both AXIS1/AXIS2_MODE_GOTO are used by AXIS1/AXIS2_STEP_GOTO times
                                     
#define DegreesForAcceleration   5.0 // approximate number of degrees for full acceleration or deceleration: higher values=longer acceleration/deceleration
                                     // Default=5.0, too low (about <1) can cause gotos to never end if micro-step mode switching is enabled.
#define DegreesForRapidStop      1.0 // approximate number of degrees required to stop when requested or if limit is exceeded during a slew: higher values=longer deceleration
                                     // Default=1.0, too low (about <1) can cause gotos to never end if micro-step mode switching is enabled.

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

#define REVERSE_AXIS1_OFF            // reverse the direction of movement for the RA/Azm axis, adjust as needed or reverse your wiring so things move in the right direction
#define REVERSE_AXIS2_OFF            // reverse the direction of movement for the Dec/Alt axis

#define MinutesPastMeridianE      60 // for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
#define MinutesPastMeridianW      60 // as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
#define UnderPoleLimit            12 // maximum allowed hour angle (+/-) under the celestial pole. OnStep will flip the mount and move the Dec. >90 degrees (+/-) once past this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
                                     // If left alone, the mount will stop tracking when it hits this limit.  Valid range is 7 to 11 hours.
#define MinDec                   -91 // minimum allowed declination, default = -91 (off)  Ignored on Alt/Azm mounts.
#define MaxDec                   +91 // maximum allowed declination, default =  91 (off)  Ignored on Alt/Azm mounts.
                                     // For example, a value of +80 would stop gotos/tracking near the north celestial pole.
                                     // For a Northern Hemisphere user, this would stop tracking when the mount is in the polar home position but
                                     // that can be easily worked around by doing an alignment once and saving a park position (assuming a 
                                     // fork/yolk mount with meridian flips turned off by setting the minutesPastMeridian values to cover the whole sky)
#define MaxAzm                   180 // Alt/Az mounts only. +/- maximum allowed Azimuth, default =  180.  Allowed range is 180 to 360


// AXIS1/2 STEPPER DRIVER CONTROL ------------------------------------------------------------------------------------------
// Axis1: Pins 35,37 = Step,Dir (RA/Azm)
// Axis2: Pins 22,24 = Step,Dir (Dec/Alt)

// Stepper driver Enable support is always on, just wire Enable to Pins 23 (Axis1) and 36 (Axis2) and OnStep will pull these HIGH
// to disable stepper drivers on startup and when Parked or Homed.  An Align or UnPark will enable the drivers.  Adjust below if you need these pulled LOW to disable the drivers.
#define AXIS1_DISABLED_HIGH
#define AXIS2_DISABLED_HIGH
// For equatorial mounts, _ON powers down the Declination axis when it's not being used to help lower power use.  During low rate guiding (<=1x) the axis stays enabled
// for 10 minutes after any guide on either axis.  Otherwise, the Dec axis is disabled (powered off) 10 seconds after movement stops.
#define AUTO_POWER_DOWN_AXIS2_OFF

/* Stepper driver Mode control
// M0, M1, and M2 are on Pins 25,27, and 29 for RA and Pins 34,32,30 for Dec.
// values 0 to 7 (0b000 to 111): for example "#define AXIS1_MODE 4" is the same as "#define AXIS1_MODE 0b100" which sets M2 to HIGH, M1 to LOW, and M0 to LOW
//                                                                                                      / | \
//                                                                                                    M2  M1 M0
// DRV8825 or A4988 or RAPS128:
// use MODE_SWITCH_BEFORE_SLEW_OFF
// micro-stepping modes: 5=32x, 4=16x, 3=8x, 2=4x, 1=2x, 0=1x (RAPS128 also supports 6=64x, 7=128x)
//
// SilentStepStick TMC2100/2130 configurations (M0 is CFG1, M1 is CFG2, M2 is CFG3):
// use MODE_SWITCH_BEFORE_SLEW_ON
// I recommend keeping CFG3 and CFG2 open (not connected) CFG1 can then be pulled low for 16x micro-stepping (256x intpol) in spreadCycle mode (0b110000 = 48 = OPEN/OPEN/OUTPUT, ignore/ignore/LOW)
// or keep CFG3 and CFG1 open (not connected) and CFG2 can be pulled high for 4x micro-stepping (256x intpol) in spreadCycle mode (0b101010 = 42 = OPEN/OUTPUT/OPEN, ignore/HIGH/ignore)
//
// SilentStepStick TMC2130 SPI configurations:
// use MODE_SWITCH_BEFORE_SLEW_SPI.  Default operation in spreadCycle and with 256x interpolation on:
// AXISn_MODE and AXISn_MODE_GOTO can be set to 0-8 where 0=256x, 1=128x, 2=64x, 3=32x, 4=16x, 5=8x, 6=4x, 7=2x, 8=1x
// You can also turn stealthChop on (TMC_STEALTHCHOP) and/or disable 256x interpolation (TMC_NINTPOL.) For example 32x mode with stealthChop:
// AXIS1_MODE (3|TMC_STEALTHCHOP)
// Another option allows setting 50% power while tracking (TMC_LOWPWR.)  As above, with this option on too:
// AXIS1_MODE (3|TMC_STEALTHCHOP|TMC_LOWPWR)
*/
#define AXIS1_MODE_OFF               // programs the RA/Az uStep mode M0/M1/M2, optional and default _OFF.
#define AXIS1_MODE_GOTO_OFF          // programs the RA/Az uStep mode M0/M1/M2, used during gotos, optional and default _OFF.
#define AXIS1_STEP_GOTO 1            // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4
#define AXIS2_MODE_OFF               // programs the Dec/Alt uStep mode M0/M1/M2, optional and default _OFF.
#define AXIS2_MODE_GOTO_OFF          // programs the Dec/Alt uStep mode M0/M1/M2, used during gotos, optional and default _OFF.
#define AXIS2_STEP_GOTO 1            // 1=goto mode is same as normal mode: for example if normal tracking mode is 32x and goto is 8x this would be 4
#define MODE_SWITCH_BEFORE_SLEW_OFF  // _ON (or _SPI) for _MODE and _MODE_GOTO settings to start/stop just before/after the slew, otherwise they are active during the slew at <128uS/step speeds
                                     // _SPI as above but uses SPI (on M0/M1/M2/Aux) to do the switching (TMC2130.)

// Secondary stepper driver decay control (for both Axis, the _DECAY_MODE and _DECAY_MODE_GOTO settings always start/stop just before/after the slew)
// typically used for the DRV8825 or A9488 where decay control is seperate from the micro-step mode control.  The driver modules usually need to be modified to accomplish this.
// Axis1 decay mode is on Pin 41 and Axis2 decay mode is on Pin 40.  Options are _HIGH, _LOW, _OPEN, default is _OFF
// MODE_SWITCH_BEFORE_SLEW must be _OFF if using this.
#define DECAY_MODE_OFF
#define DECAY_MODE_GOTO_OFF

// if the decay/micro-step mode switch happens before/after a slew, inserts a 3ms delay before the motors take a step
#define MODE_SWITCH_SLEEP_OFF

// Stepper driver Fault detection, just wire the driver Fault signal to Pins 39 (Axis1) and 38 (Axis2), default=OFF.
// other settings are LOW, HIGH, SPI.  The SPI interface (on M0/M1/M2/Aux) can be used to detect errors on the TMC2130.
#define AXIS1_FAULT_OFF
#define AXIS2_FAULT_OFF

// ------------------------------------------------------------------------------------------------------------------------
// FOCUSER ROTATOR OR ALT/AZ DE-ROTATION ----------------------------------------------------------------------------------
// A9,A8 = Step,Dir
#define ROTATOR_OFF                  // enable or disable rotator feature (for any mount type,) default=OFF (de-rotator is available only for MOUNT_TYPE_ALTAZM.)
#define MaxRateAxis3               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerDegreeAxis3     64.0 // calculated as    :  stepper_steps * micro_steps * gear_reduction1 * (gear_reduction2/360)
                                     // Rotator          :  24            * 8           * 20              *  6/360                = 64
                                     // For de-rotation of Alt/Az mounts a quick estimate of the required resolution (in StepsPerDegree)
                                     // would be an estimate of the circumference of the useful imaging circle in (pixels * 2)/360
#define REVERSE_AXIS3_OFF            // reverse the direction of Axis3 rotator movement
#define MinAxis3                -180 // minimum allowed Axis3 rotator, default = -180
#define MaxAxis3                 180 // maximum allowed Axis3 rotator, default =  180

// FOCUSER1 ---------------------------------------------------------------------------------------------------------------
// A11,A10 = Step,Dir
#define FOCUSER1_OFF                 // enable or disable focuser feature, default=OFF
#define MaxRateAxis4               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerMicrometerAxis4  0.5 // figure this out by testing or other means
#define REVERSE_AXIS4_OFF            // reverse the direction of Axis4 focuser movement
#define MinAxis4               -25.0 // minimum allowed Axis4 position in millimeters, default = -25.0
#define MaxAxis4                25.0 // maximum allowed Axis4 position in millimeters, default =  25.0

// FOCUSER2 ---------------------------------------------------------------------------------------------------------------
// A13,A12 = Step,Dir
#define FOCUSER2_OFF                 // enable or disable focuser feature, default=OFF
#define MaxRateAxis5               8 // this is the minimum number of milli-seconds between micro-steps, default=8
#define StepsPerMicrometerAxis5  0.5 // figure this out by testing or other means
#define REVERSE_AXIS5_OFF            // reverse the direction of Axis4 focuser movement
#define MinAxis5               -25.0 // minimum allowed Axis5 position in millimeters, default = -25.0
#define MaxAxis5                25.0 // maximum allowed Axis5 position in millimeters, default =  25.0

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
#include "Pins.Mega2560Alt.h"
#endif

