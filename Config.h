// -----------------------------------------------------------------------------------
// Configuration for OnStep

/*
 * For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep and join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 *                             *** Read the compiler warnings and errors, they are there to help guard against invalid configurations ***
 * 
*/

// -------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES --------------------------------------------------------------

// PINMAP ------------------------------------------------------------------------------------------------------------------
// *** See the matching Pins.xxx.h file for your setup (found by looking in src/pinmaps/Models.h) with detailed information to be sure it matches your wiring *** USE AT YOUR OWN RISK *** ***

#define PINMAP                        OFF // Default    OFF,     Choose from: MiniPCB, MiniPCB2, MaxPCB, MaxPCB2, STM32Blue,                                      <- Required.
                                          //                                  Ramps14, MaxESP2, MaxESP3, Classic, Mega2560Alt

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#define SERIAL_A_BAUD_DEFAULT        9600 // Default   9600,    Common rates are 9600,19200,57600,115200.  See (src/HAL/) for your MCU Serial port #...           Usually not changed.
#define SERIAL_B_BAUD_DEFAULT        9600 // Default   9600,    Common rates are 9600,19200,57600,115200.                                                         Optional.
#define SERIAL_B_ESP_FLASHING         OFF // Default    OFF,    Enable uploading firmware to ESP8266 WiFi through SERIAL_B port using the :ESPFLASH# command.     Optional.
#define SERIAL_C_BAUD_DEFAULT         OFF // Default    OFF,    Common rates are 9600,19200,57600,115200.  Bluetooth on ESP32 use "name" instead of rate.         Optional.

// MOUNT TYPE --------------------------------------------------------------------------------------------------------------
#define MOUNT_TYPE                    GEM // Default    GEM,    GEM for German Equatorial, FORK for Equatorial Fork, or ALTAZM for Dobsonian etc. mounts.         <- Required.  GEM supports meridian flips.

// USER FEEDBACK -----------------------------------------------------------------------------------------------------------
#define LED_STATUS_PIN                 ON // Default     ON,    Blinks when sidereal tracking otherwise steady on to indicate that the controller is active.      Optional.
#define LED_STATUS2_PIN               OFF // Default    OFF,    Blinks at 1 sec intervals when PPS is synced, steady on during gotos, off when tracking stopped.  Optional.
#define LED_RETICLE_PIN               OFF // Default    OFF,    n.  Where n=0 to 255 (0 to 100%) activates this feature and sets default brightness.              Optional.
#define BUZZER                        OFF // Default    OFF,    ON for piezo buzzer or specify frequency for a speaker (for example "BUZZER 2000".)               Optional.
#define BUZZER_STATE_DEFAULT          OFF // Default    OFF,    Start with piezo buzzer/speaker enabled.                                                          Optional.

// TIME --------------------------------------------------------------------------------------------------------------------
#define RTC                           OFF // Default    OFF,    Select RTC DS3231, DS3234, TEENSY (Teensy3.2 internal.)  Provides Date/Time/PPS (if avail.)       Optional.

// SENSORS -----------------------------------------------------------------------------------------------------------------
// * = also supports ON_PULLUP or ON_PULLDOWN to activate MCU internal resistors if present.
#define WEATHER                       OFF // Default    OFF,    BME280 temperature, pressure, humidity on I2C interface (0x77.)                                   Optional.

#define TELESCOPE_TEMPERATURE         OFF // Default    OFF,    DS1820 for telescope temperature on ONE WIRE interface.                                           Optional.

#define HOME_SENSE                    OFF // Default    OFF,    ON*.  Automatically detect and use home switches.                                                 Optional, for GEM mounts only.
#define HOME_SENSE_STATE_AXIS1       HIGH // Default   HIGH,    HIGH signal when clockwise of home position LOW otherwise (as seen from front.)                   Adjust as needed.
#define HOME_SENSE_STATE_AXIS2       HIGH // Default   HIGH,    HIGH signal when clockwise of home position LOW otherwise (as seen from front.)                   Adjust as needed.

#define LIMIT_SENSE                   OFF // Default    OFF,    ON*.  Switch close (to ground) for optional limit sense (stops gotos and/or tracking.)            Optional.
#define LIMIT_SENSE_STATE             LOW // Default    LOW,    LOW for NO (normally open) switches close w/trigger, use HIGH for NC (normally closed.)           Adjust as needed.

#define PEC_SENSE                     OFF // Default    OFF,    ON*, n.  Sense digital OR n=0 to 1023 (0 to 3.3 VDC) for analog threshold.                        Optional, ignored on Alt/Azm mounts.
#define PEC_SENSE_STATE              HIGH // Default   HIGH,    HIGH senses the PEC signal rising edge or use LOW for falling edge.                               Adjust as needed.

#define PPS_SENSE                     OFF // Default    OFF,    ON*.  Enables PPS (pulse per second.)                                                             Optional.

// ST4 INTERFACE -----------------------------------------------------------------------------------------------------------
// *** It is up to you to verify the interface meets the electrical specifications of any connected device, use at your own risk ***
#define ST4_INTERFACE                 OFF // Default    OFF,    ON or ON_PULLUP enables the ST4 interface.                                                        Optional.
#define ST4_HAND_CONTROL              OFF // Default    OFF,    Enable hand controller special features, SHC support, combo button presses on simple button pad.  Optional, OFF allows only <= 1X rates on the ST4 port.
#define ST4_HAND_CONTROL_FOCUSER      OFF // Default    OFF,    Use button pad combo presses for focuser control instead of catalog select/goto, buzzer control.  Optional.

// GUIDING BEHAVIOUR -------------------------------------------------------------------------------------------------------
#define GUIDE_TIME_LIMIT                0 // Default      0,    For no time limit, or specify guide time limit in seconds.  Safety feature.                       Adjust as needed.
#define GUIDE_DISABLE_BACKLASH        OFF // Default    OFF,    Disable backlash takeup during guiding at <= 1X                                                   Optional.

// TRACKING BEHAVIOUR ------------------------------------------------------------------------------------------------------
#define TRACK_AUTOSTART               OFF // Default    OFF,    Start with tracking enabled.                                                                      Optional.
#define TRACK_REFRACTION_RATE_DEFAULT OFF // Default    OFF,    Start with atmospheric refraction compensation enabled (RA axis only.)                            Optional, ignored on Alt/Azm mounts.
#define TRACK_BACKLASH_RATE            25 // Default     25,    Speed in x sidereal rate, too fast and motors stall, too slow and sluggish moving in backlash.    Usually not changed.

// SYNCING BEHAVIOUR -------------------------------------------------------------------------------------------------------
#define SYNC_CURRENT_PIER_SIDE_ONLY    ON // Default     ON,    Disables ability of Sync to change pier side, for GEM mounts.                                     Optional.

// SLEWING BEHAVIOUR -------------------------------------------------------------------------------------------------------
#define SLEW_RATE_BASE_DESIRED        1.0 // Default    1.0,    Desired slew (goto) rate in degrees/second, adjustable run-time from 1/2 to 2x this rate.         <- Required, adjust as needed.  Tries to use this rate, if possible.
#define SLEW_RATE_MEMORY              OFF // Default    OFF,    Remember rates set across power cycles.                                                           Optional.
#define SLEW_ACCELERATION_DIST        5.0 // Default    5.0,    Approx. distance in degrees during which acceleration and deceleration takes place.               Adjust as needed.
#define SLEW_RAPID_STOP_DIST          2.0 // Default    2.0,    Approx. distance in degrees required to stop when a slew is aborted or limit is exceeded.         Adjust as needed.
#define MFLIP_SKIP_HOME               OFF // Default    OFF,    Goto directly to the destination without visiting the home position.                              Optional.
#define MFLIP_PAUSE_HOME_MEMORY       OFF // Default    OFF,    Remember the last meridian flip pause at home setting across power cycles.                        Optional.
#define MFLIP_AUTOMATIC_MEMORY        OFF // Default    OFF,    Remember the last automatic meridian flip setting.                                                Optional.

// PARKING BEHAVIOUR -------------------------------------------------------------------------------------------------------
#define STRICT_PARKING                OFF // Default    OFF,    Enable and un-parking is only allowed if successfully parked.                                     Optional.

// MOTION CONTROL ----------------------------------------------------------------------------------------------------------
#define STEP_WAVE_FORM             SQUARE // Default SQUARE,    Step signal wave form, SQUARE for best compatibility, PULSE to allow faster step rates.           Adjust as needed.

// Stepper driver models are as follows: (for example AXIS1_DRIVER_MODEL DRV8825,) A4988, LV8729, RAPS128, S109, ST820, TMC2100, TMC2208, TMC2209*, TMC2130**, TMC5160***
// * = add _QUIET (stealthChop tracking) or _VQUIET (stealthChop tracking & slew.)
// ** = for the SSS TMC2130 you can set the stepper driver current (in milliamps) with #define AXISn_TMC_IRUN (IHOLD, etc.)  If you choose to do so set the Vref pot. to 2.5V instead of by motor current as usual.
// *** = for the SSS TMC5160 you must set the stepper driver current (in milliamps) with #define AXISn_TMC_IRUN (IHOLD, etc.)

// AXIS1 RA/AZM --------------------------
#define AXIS1_STEPS_PER_DEGREE    12800.0 // Default  12800,    Calculated as: (stepper_steps * micro_steps * gear_reduction1 * gear_reduction2)/360.0            <- Required, adjust as needed.
#define AXIS1_STEPS_PER_WORMROT     12800 // Default  12800,    Number of steps per worm rotation (PEC,) (AXIS1_STEPS_PER_DEGREE*360)/gear_reduction_final_stage. <- Required, adjust as needed, ignored in Alt/Azm mode.

#define AXIS1_DRIVER_MODEL            OFF // Default    OFF,    Stepper driver model (see above.)                                                                 <- Often configured.
#define AXIS1_DRIVER_MICROSTEPS       OFF // Default    OFF,    Microstep mode when tracking (for example: AXIS1_DRIVER_MICROSTEPS 32)                            <- Often configured.
#define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF // Default    OFF,    Microstep mode used during gotos (for example: AXIS1_DRIVER_MICROSTEPS_GOTO 2)                    Optional.
#define AXIS1_DRIVER_IHOLD            OFF // Default    OFF,    OFF for IRUN/2.0, or set value for current in milli-amps used during standstill.                  Optional, TMC2130 & TMC5160 only.
#define AXIS1_DRIVER_IRUN             OFF // Default    OFF,    Current in milli-amps when tracking, set to value appropriate for your stepper/driver rating.     Optional, TMC2130 & TMC5160 only.
#define AXIS1_DRIVER_IGOTO            OFF // Default    OFF,    OFF for same as IRUN, or set value for current in milli-amps used during gotos.                   Optional, TMC2130 & TMC5160 only.
#define AXIS1_DRIVER_REVERSE          OFF // Default    OFF,    Reverses the direction of movement.  Reversing wiring instead can also correct this.              Optional.
#define AXIS1_DRIVER_STATUS           OFF // Default    OFF,    Polling for stepper driver status info/fault detection. Options are TMC_SPI, HIGH, or LOW.        Optional.

#define AXIS1_LIMIT_UNDER_POLE      180.0 // Default  180.0,    Degrees +/-, maximum allowed Hour Angle.  Allowed range is 150 to 180.                            Adjust as needed, ignored in Alt/Azm mode.
#define AXIS1_LIMIT_MAXAZM          360.0 // Default  360.0,    Degrees +/-, maximum allowed Azimuth.  Allowed range is 180 to 360. Ignored in Eq modes.          Adjust as needed, ignored on Eq mounts.

// AXIS2 DEC/ALT -------------------------
#define AXIS2_STEPS_PER_DEGREE    12800.0 // Default  12800,    Calculated as: (stepper_steps * micro_steps * gear_reduction1 *  gear_reduction2)/360.0           <- Required, adjust as needed.

#define AXIS2_DRIVER_MODEL            OFF // Default    OFF,    Stepper driver model (see above.)                                                                 <- Often configured.
#define AXIS2_DRIVER_MICROSTEPS       OFF // Default    OFF,    Microstep mode when tracking (for example: AXIS2_DRIVER_MICROSTEPS 32)                            <- Often configured.
#define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF // Default    OFF,    Microstep mode used during gotos (for example: AXIS2_DRIVER_MICROSTEPS_GOTO 2)                    Optional.
#define AXIS2_DRIVER_IHOLD            OFF // Default    OFF,    OFF for IRUN/2.0, current in milli-amps used during standstill.                                   Optional, TMC2130 & TMC5160 only.
#define AXIS2_DRIVER_IRUN             OFF // Default    OFF,    Current in milli-amps when tracking, set to value appropriate for your stepper/driver rating.     Optional, TMC2130 & TMC5160 only.
#define AXIS2_DRIVER_IGOTO            OFF // Default    OFF,    OFF for same as IRUN, current in milli-amps used during gotos.                                    Optional, TMC2130 & TMC5160 only.
#define AXIS2_DRIVER_POWER_DOWN       OFF // Default    OFF,    Powers off axis 10 sec. after movement stops or 10 min. after last <= 1x guide.                   Optional, ignored on Alt/Azm mounts.
#define AXIS2_DRIVER_REVERSE          OFF // Default    OFF,    Reverses the direction of movement.  Reversing wiring instead can also correct this.              Optional.
#define AXIS2_DRIVER_STATUS           OFF // Default    OFF,    Polling for stepper driver status info/fault detection. Options are TMC_SPI, HIGH, or LOW.        Optional.

#define AXIS2_LIMIT_MIN             -91.0 // Default  -91.0,    Minimum allowed declination in degrees.                                                           Usually not changed.
#define AXIS2_LIMIT_MAX             +91.0 // Default   91.0,    Maximum allowed declination in degrees.                                                           Usually not changed.

// AXIS3 ROTATOR -------------------------
#define ROTATOR                       OFF // Default    OFF,    Use ON to enable the rotator (or de-rotator for ALTAZM mounts.)                                   Optional.
#define AXIS3_STEPS_PER_DEGREE       64.0 // Default   64.0,    De-rotation Alt/Az resolution: calc. circumference of useful imaging circle (pixels * 2)/360.     Adjust as needed.
#define AXIS3_STEP_RATE_MAX           8.0 // Default    8.0,    This is the minimum milli-seconds per step.  Fastest speed also limited by MCU performace.        Adjust as needed.

#define AXIS3_DRIVER_MODEL            OFF // Default    OFF,    Stepper driver model  (TMC2130 or TMC5160 only.)                                                  Optional.
#define AXIS3_DRIVER_MICROSTEPS       OFF // Default    OFF,    Microstep mode(for example: AXIS3_DRIVER_MICROSTEPS 1)                                            Optional, for TMC2130 & TMC5160 only.
#define AXIS3_DRIVER_IHOLD            OFF // Default    OFF,    OFF for IRUN/2.0, current in milli-amps used during standstill.                                   Optional, for TMC2130 & TMC5160 only.
#define AXIS3_DRIVER_IRUN             OFF // Default    OFF,    Current in milli-amps, set to value appropriate for your stepper/driver rating.                   Optional, for TMC2130 & TMC5160 only.
#define AXIS3_DRIVER_POWER_DOWN       OFF // Default    OFF,    Powers off the motor at stand-still.                                                              Optional.
#define AXIS3_DRIVER_REVERSE          OFF // Default    OFF,    Reverses the direction of movement.  Reversing wiring instead can also correct this.              Optional.

#define AXIS3_LIMIT_MIN            -180.0 // Default -180.0,    Minimum allowed rotator angle in degrees.                                                         Usually not changed.
#define AXIS3_LIMIT_MAX             180.0 // Default  180.0,    Maximum allowed rotator angle in degrees.                                                         Usually not changed.

// AXIS4 FOCUSER 1 ----------------------
#define FOCUSER1                      OFF // Default    OFF,    Use ON to enable this focuser.                                                                    Optional.
#define AXIS4_STEPS_PER_MICRON        0.5 // Default    0.5,    Steps per micro-meter.  Figure this out by testing or other means.                                Adjust as needed.
#define AXIS4_STEP_RATE_MAX           8.0 // Default    8.0,    This is the milli-seconds per step (lower is faster.)  In DC mode PWM frequency.                  Adjust as needed.

#define AXIS4_DRIVER_MODEL            OFF // Default    OFF,    Stepper driver model (TMC2130 or TMC5160 only.)                                                   Optional.
#define AXIS4_DRIVER_MICROSTEPS       OFF // Default    OFF,    Microstep mode (for example: AXIS4_DRIVER_MICROSTEPS 1)                                           Optional, for TMC2130 & TMC5160 only.
#define AXIS4_DRIVER_IHOLD            OFF // Default    OFF,    OFF for IRUN/2.0, current in milli-amps used during standstill.                                   Optional, for TMC2130 & TMC5160 only.
#define AXIS4_DRIVER_IRUN             OFF // Default    OFF,    Current in milli-amps, set to value appropriate for your stepper/driver rating.                   Optional, for TMC2130 & TMC5160 only.
#define AXIS4_DRIVER_POWER_DOWN       OFF // Default    OFF     Powers off the motor at stand-still.                                                              Optional.
#define AXIS4_DRIVER_REVERSE          OFF // Default    OFF,    Reverses the direction of movement.  Reversing wiring instead can also correct this.              Optional.
#define AXIS4_DRIVER_DC_MODE          OFF // Default    OFF,    Use DRV8825 for pwm dc motor control on stepper driver outputs.                                   Optional.

#define AXIS4_LIMIT_MIN_RATE           10 // Default     10,    Minimum micrometers per second, default=10.  In DC mode power for slowest move (1-1000 range.)    Adjust as needed.
#define AXIS4_LIMIT_MIN             -25.0 // Default  -25.0,    Minimum allowed position in millimeters.                                                          Adjust as needed.
#define AXIS4_LIMIT_MAX              25.0 // Default   25.0,    Maximum allowed position in millimeters.                                                          Adjust as needed.

// AXIS5 FOCUSER 2 ----------------------
#define FOCUSER2                      OFF // Default    OFF,    Use ON to enable this focuser.                                                                    Optional.
#define AXIS5_STEPS_PER_MICRON        0.5 // Default    0.5,    Steps per micrometer.  Figure this out by testing or other means.                                 Adjust as needed.
#define AXIS5_STEP_RATE_MAX           8.0 // Default    8.0,    This is the milli-seconds per step (lower is faster.)  In DC mode PWM frequency.                  Adjust as needed.

#define AXIS5_DRIVER_MODEL            OFF // Default    OFF,    Stepper driver model  (TMC2130 or TMC5160 only.)                                                  Optional.
#define AXIS5_DRIVER_MICROSTEPS       OFF // Default    OFF,    Microstep mode (for example: AXIS5_DRIVER_MICROSTEPS 1)                                           Optional, for TMC2130 & TMC5160 only.
#define AXIS5_DRIVER_IHOLD            OFF // Default    OFF,    OFF for IRUN/2.0, current in milli-amps used during standstill.                                   Optional, for TMC2130 & TMC5160 only.
#define AXIS5_DRIVER_IRUN             OFF // Default    OFF,    Current in milli-amps, set to value appropriate for your stepper/driver rating.                   Optional, for TMC2130 & TMC5160 only.
#define AXIS5_DRIVER_POWER_DOWN       OFF // Default    OFF     Powers off the motor at stand-still.                                                              Optional.
#define AXIS5_DRIVER_REVERSE          OFF // Default    OFF,    Reverses the direction of movement.  Reversing wiring instead can also correct this.              Optional.
#define AXIS5_DRIVER_DC_MODE          OFF // Default    OFF,    Use DRV8825 for PWM DC motor control on stepper driver outputs.                                   Optional.

#define AXIS5_LIMIT_MIN_RATE           10 // Default     10,    Minimum micrometers per second, default=10.  In DC mode power for slowest move (1-1000 range.)    Adjust as needed.
#define AXIS5_LIMIT_MIN             -25.0 // Default  -25.0,    Minimum allowed position in millimeters.                                                          Adjust as needed.
#define AXIS5_LIMIT_MAX              25.0 // Default   25.0,    Maximum allowed position in millimeters.                                                          Adjust as needed.

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 3
