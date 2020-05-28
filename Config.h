// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStep

/*
 *          For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep 
 *                      and join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 *           *** Read the compiler warnings and errors, they are there to help guard against invalid configurations ***
*/

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// PINMAP ---------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PINMAP
//   *** See the matching Pins.xxx.h file for your setup (found by looking in src/pinmaps/Models.h) with detailed information ***
//   *** to be sure it matches your wiring.            *** USE AT YOUR OWN RISK ***                                           ***

//      Parameter Name              Value   Default  Notes                                                                      Hint
#define PINMAP                        OFF //    OFF, Choose from: MiniPCB, MiniPCB2, MaxPCB, MaxPCB2, STM32Blue,             <-Req'd
                                          //         Ramps14, MaxESP2, MaxESP3.  Check Constants.h for more info.

// SERIAL PORTS ---------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SERIAL
#define SERIAL_A_BAUD_DEFAULT        9600 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Infreq
#define SERIAL_B_BAUD_DEFAULT        9600 //   9600, n. See (src/HAL/) for your MCU Serial port # etc.                        Option
#define SERIAL_B_ESP_FLASHING         OFF //    OFF, ON Upload ESP8266 WiFi firmware through SERIAL_B with :ESPFLASH# cmd.    Option
#define SERIAL_C_BAUD_DEFAULT         OFF //    OFF, n, ON for ESP32 Bluetooth.                                               Option
#define SERIAL_C_BLUETOOTH_NAME  "OnStep" // "On..", Bluetooth device name for ESP32.                                         Option

// MOUNT TYPE -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_TYPE                    GEM //    GEM, GEM for German Equatorial, FORK for Equatorial Fork, or ALTAZM          <-Req'd
                                          //         Dobsonian etc. mounts. GEM Eq mounts perform meridian flips.

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define LED_STATUS                     ON //     ON, Blinks w/sidereal tracking otherwise steady on indicates activity.       Option
#define LED_STATUS2                   OFF //    OFF, ON Blinks 1s interval w/PPS sync, steady for gotos, off if in standby.   Option
#define LED_RETICLE                   OFF //    OFF, n. Where n=0..255 (0..100%) activates feature sets default brightness.   Option
#define BUZZER                        OFF //    OFF, ON, n. Where n=100..6000 (Hz freq.) for piezo speaker. ON for buzzer.    Option
#define BUZZER_STATE_DEFAULT          OFF //    OFF, ON Start with piezo buzzer/speaker enabled.                              Option

// TIME AND LOCATION -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TLS
#define TIME_LOCATION_SOURCE          OFF //    OFF, DS3231 (I2c,) DS3234 (Spi,) TEENSY (T3.2 internal,) or GPS source.       Option
                                          //         Provides Date/Time, and if available, PPS & Lat/Long also.

// SENSORS -------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
// * = also supports ON_PULLUP or ON_PULLDOWN to activate MCU internal resistors if present.
#define WEATHER                       OFF //    OFF, BME280 (I2C 0x77,) BME280_0x76, BME280_SPI (see pinmap for CS.)          Option
                                          //         BMP280 (I2C 0x77,) BMP280_0x76, BMP280_SPI (see pinmap for CS.)
                                          //         BME280 or BMP280 for temperature, pressure.  BME280 for humidity also.
                                          
#define TELESCOPE_TEMPERATURE         OFF //    OFF, DS1820, n. Where n is the DS1820 s/n for focuser temperature.            Adjust

#define HOME_SENSE                    OFF //    OFF, ON*. Automatically detect and use home switches. For GEM mode only.      Option
#define HOME_SENSE_STATE_AXIS1       HIGH //   HIGH, State when clockwise of home position, as seen from front. Rev. w/LOW.   Adjust
#define HOME_SENSE_STATE_AXIS2       HIGH //   HIGH, State when clockwise of home position, as seen from above. Rev. w/LOW.   Adjust
                                          //         Signal state reverses when travel moves ccw past the home position.

#define LIMIT_SENSE                   OFF //    OFF, ON* limit sense switch close to Gnd stops gotos and/or tracking.         Option
#define LIMIT_SENSE_STATE             LOW //    LOW, For NO (normally open) switches, HIGH for NC (normally closed.)          Adjust
#define PEC_SENSE                     OFF //    OFF, ON*, n, sense digital OR n=0 to 1023 (0 to 3.3V or 5V) analog threshold. Option
#define PEC_SENSE_STATE              HIGH //   HIGH, Senses the PEC signal rising edge or use LOW for falling edge.           Adjust
                                          //         Ignored in ALTAZM mode.

#define PPS_SENSE                     OFF //    OFF, ON* enables PPS (pulse per second,) senses signal rising edge.           Option
                                          //         Better tracking accuracy especially for Mega2560's w/ceramic resonator.

// ST4 INTERFACE ------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#ST4
// *** It is up to you to verify the interface meets the electrical specifications of any connected device, use at your own risk ***
#define ST4_INTERFACE                 OFF //    OFF, ON, ON_PULLUP enables interface. <= 1X guides unless hand control mode.  Option
                                          //         During goto btn press: aborts slew or continue meridian flip pause home
#define ST4_HAND_CONTROL              OFF //    OFF, ON for hand controller special features and SHC support.                 Option
                                          //         Hold [E]+[W] btns >2s: Guide rate   [E]-  [W]+  [N] trk on/off [S] sync
                                          //         Hold [N]+[S] btns >2s: Usr cat item [E]-  [W]+  [N] goto [S] snd on/off
#define ST4_HAND_CONTROL_FOCUSER      OFF //    OFF, ON alternate to above: Focuser move [E]f1 [W]f2 [N]-     [S]+            Option

// GUIDING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#GUIDING
#define GUIDE_TIME_LIMIT                0 //      0, No guide time limit. Or n. Where n=1..120 second time limit guard.       Adjust
#define GUIDE_DISABLE_BACKLASH        OFF //    OFF, Disable backlash takeup during guiding at <= 1X                          Option

// TRACKING BEHAVIOUR -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TRACKING
#define TRACK_AUTOSTART               OFF //    OFF, ON Start with tracking enabled.                                          Option
#define TRACK_REFRACTION_RATE_DEFAULT OFF //    OFF, ON Start w/atmospheric refract. compensation (RA axis/Eq mounts only.)   Option
#define TRACK_BACKLASH_RATE            25 //     25, n. Where n=2..50 (x sidereal rate) during backlash takeup.               Option
                                          //         Too fast motors stall/gears slam or too slow and sluggish in backlash.

// SYNCING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SYNCING
#define SYNC_CURRENT_PIER_SIDE_ONLY    ON //     ON, Disables ability of sync to change pier side, for GEM mounts.            Option

// SLEWING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SLEWING
#define SLEW_RATE_BASE_DESIRED        1.0 //    1.0, n. Desired slew rate in deg/sec. Adjustable at run-time from            <-Req'd
                                          //         1/2 to 2x this rate, and as MCU performace considerations require.
#define SLEW_RATE_MEMORY              OFF //    OFF, ON Remembers rates set across power cycles.                              Option
#define SLEW_ACCELERATION_DIST        5.0 //    5.0, n, (degrees.) Approx. distance for acceleration (and deceleration.)      Adjust
#define SLEW_RAPID_STOP_DIST          2.5 //    2.0, n, (degrees.) Approx. distance required to stop when a slew              Adjust
                                          //         is aborted or a limit is exceeded.

#define MFLIP_SKIP_HOME               OFF //    OFF, ON Goto directly to the destination without visiting home position.      Option
#define MFLIP_PAUSE_HOME_MEMORY       OFF //    OFF, ON Remember meridian flip pause at home setting across power cycles.     Option
#define MFLIP_AUTOMATIC_MEMORY        OFF //    OFF, ON Remember automatic meridian flip setting across power cycles.         Option

// PARKING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PARKING
#define STRICT_PARKING                OFF //    OFF, ON Un-parking is only allowed if successfully parked.                    Option

// MOTION CONTROL -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOTION
#define STEP_WAVE_FORM             SQUARE // SQUARE, PULSE Step signal wave form faster rates. SQUARE best signal integrity.  Adjust

// Stepper driver models (also see ~/OnStep/src/sd_drivers/Models.h for additional infrequently used models and more info.): 
// A4988, DRV8825, LV8729, S109, SSS TMC2209*, TMC2130* **, and TMC5160* ***
// * = add _QUIET (stealthChop tracking) or _VQUIET (stealthChop tracking & slew,) for example "TMC2130_VQUIET"
// ** = SSS TMC2130 if you choose to set stepper driver current (in mA) set Vref pot. 2.5V instead of by motor current as usual.
// *** = SSS TMC5160 you must set stepper driver current (in mA) w/ #define AXISn_TMC_IRUN (IHOLD, etc.)

// AXIS1 RA/AZM
// see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS1
#define AXIS1_STEPS_PER_DEGREE    12800.0 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0
#define AXIS1_STEPS_PER_WORMROT     12800 //  12800, n. Number of steps per worm rotation (PEC Eq mode only:)                <-Req'd
                                          //         n = (AXIS1_STEPS_PER_DEGREE*360)/reduction_final_stage

#define AXIS1_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                      <-Often
#define AXIS1_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF //    OFF, n. Microstep mode used during gotos.                                     Option
#define AXIS1_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS1_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS1_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses same as IRUN.                    Option
#define AXIS1_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often
#define AXIS1_DRIVER_STATUS           OFF //    OFF, TMC_SPI, HIGH, or LOW.  Polling for driver status info/fault detection.  Option

#define AXIS1_LIMIT_UNDER_POLE        180 //    180, n. Where n=150..180 (degrees.) Max HA hour angle + or - for Eq modes.    Infreq
#define AXIS1_LIMIT_MAXAZM            360 //    360, n. Where n=180..360 (degrees.) Max Azimuth + or - for AltAzm mode only.  Infreq

// AXIS2 DEC/ALT
// see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS2
#define AXIS2_STEPS_PER_DEGREE    12800.0 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0

#define AXIS2_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                      <-Often
#define AXIS2_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF //    OFF, n. Microstep mode used during gotos.                                     Option
#define AXIS2_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS2_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS2_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses same as IRUN.                    Option
#define AXIS2_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off 10sec after movement stops or 10min after last<=1x guide.  Option
#define AXIS2_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often
#define AXIS2_DRIVER_STATUS           OFF //    OFF, TMC_SPI, HIGH, or LOW.  Polling for driver status info/fault detection.  Option
#define AXIS2_TANGENT_ARM             OFF //    OFF, ON +limit range below. Set cntr w/[Reset Home] Return cntr w/[Find Home] Infreq

#define AXIS2_LIMIT_MIN               -91 //    -91, n. Where n=-91..0 (degrees.) Minimum allowed declination.                Infreq
#define AXIS2_LIMIT_MAX                91 //     91, n. Where n=0..91 (degrees.) Maximum allowed declination.                 Infreq

// AXIS3 ROTATOR
// see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS3
#define ROTATOR                       OFF //    OFF, ON to enable the rotator (or de-rotator for ALTAZM mounts.)              Option
#define AXIS3_STEPS_PER_DEGREE       64.0 //   64.0, n. Number of steps per degree for rotator/de-rotator.                    Adjust
                                          //         Alt/Az de-rotation: n = (num_circumference_pixels * 2)/360, minimum
#define AXIS3_SLEW_RATE_DESIRED       1.0 //    1.0, n, (degrees/second) Maximum speed depends on processor.                  Adjust

#define AXIS3_DRIVER_MODEL            OFF //    OFF, TMC2130, TMC5160. Leave OFF for all drivers models except these.         Option
#define AXIS3_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                   For TMC2130, TMC5160. Option
#define AXIS3_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current standstill. OFF uses IRUN/2.0.                  "       Option
#define AXIS3_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.   "       Option
#define AXIS3_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS3_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option

#define AXIS3_LIMIT_MIN              -180 //   -180, n. Where n=-360..0 (degrees.) Minimum allowed rotator angle.             Infreq
#define AXIS3_LIMIT_MAX               180 //    180, n. Where n=0..360 (degrees.) Maximum allowed rotator angle.              Infreq

// AXIS4 FOCUSER 1
// see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4
#define FOCUSER1                      OFF //    OFF, ON to enable this focuser.                                               Option
#define AXIS4_STEPS_PER_MICRON        0.5 //    0.5, n. Steps per micrometer. Figure this out by testing or other means.      Adjust
#define AXIS4_SLEW_RATE_DESIRED       500 //    500, n, Where n=200..5000 (um/s.) Max microns/second. In DC mode, max pwr %   Adjust

#define AXIS4_DRIVER_MODEL            OFF //    OFF, TMC2130, TMC5160. Leave OFF for all drivers models except these.         Option
#define AXIS4_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                   For TMC2130, TMC5160. Option
#define AXIS4_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current standstill. OFF uses IRUN/2.0.                  "       Option
#define AXIS4_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.   "       Option
#define AXIS4_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS4_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS4_DRIVER_DC_MODE          OFF //    OFF, DRV8825 for pwm dc motor control on stepper driver outputs.              Option

#define AXIS4_LIMIT_MIN_RATE           10 //     10, n. Where n=1..1000 (um/s.) Minimum microns/second. In DC mode, min pwr % Adjust
#define AXIS4_LIMIT_MIN                 0 //      0, n. Where n=0..500 (millimeters.) Minimum allowed position.               Adjust
#define AXIS4_LIMIT_MAX                50 //     50, n. Where n=0..500 (millimeters.) Maximum allowed position.               Adjust

// AXIS5 FOCUSER 2
// see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS5
#define FOCUSER2                      OFF //    OFF, ON to enable this focuser.                                               Option
#define AXIS5_STEPS_PER_MICRON        0.5 //    0.5, n. Steps per micrometer. Figure this out by testing or other means.      Adjust
#define AXIS5_SLEW_RATE_DESIRED       500 //    500, n, Where n=200..5000 (um/s.) Max microns/second. In DC mode, max pwr %   Adjust

#define AXIS5_DRIVER_MODEL            OFF //    OFF, TMC2130, TMC5160. Leave OFF for all drivers models except these.         Option
#define AXIS5_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                   For TMC2130, TMC5160. Option
#define AXIS5_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current standstill. OFF uses IRUN/2.0.                  "       Option
#define AXIS5_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.   "       Option
#define AXIS5_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS5_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS5_DRIVER_DC_MODE          OFF //    OFF, DRV8825 for pwm dc motor control on stepper driver outputs.              Option

#define AXIS5_LIMIT_MIN_RATE           10 //     10, n. Where n=1..1000 (um/s.) Minimum microns/second. In DC mode, min pwr % Adjust
#define AXIS5_LIMIT_MIN                 0 //      0, n. Where n=0..500 (millimeters.) Minimum allowed position.               Adjust
#define AXIS5_LIMIT_MAX                50 //     50, n. Where n=0..500 (millimeters.) Maximum allowed position.               Adjust

// AUXILIARY FEATURE CONTROL ------------------------------ see https://onstep.groups.io/g/main/wiki/6-ConfigurationMaster#AUXILIARY
#define FEATURE_LIST_DS               OFF //    OFF, temporarily set ON to list ds18b20 and ds2413 device serial numbers.     Adjust
#define FEATURE1_NAME          "FEATURE1" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE1_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE1_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE1_PIN                  OFF //    OFF, AUX, DS2413, n. Where n is ds2413 s/n (gpio0) or n=0 to 255 (pin#.)      Adjust
#define FEATURE2_NAME          "FEATURE2" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE2_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE2_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE2_PIN                  OFF //    OFF, AUX, CHAIN (ds2413 gpio1,) n. Where n=0 to 255 (pin#.)                   Adjust
#define FEATURE3_NAME          "FEATURE3" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE3_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE3_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE3_PIN                  OFF //    OFF, AUX, DS2413, n. Where n is ds2413 s/n (gpio0) or n=0 to 255 (pin#.)      Adjust
#define FEATURE4_NAME          "FEATURE4" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE4_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE4_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE4_PIN                  OFF //    OFF, AUX, CHAIN (ds2413 gpio1,) n. Where n=0 to 255 (pin#.)                   Adjust
#define FEATURE5_NAME          "FEATURE5" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE5_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE5_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE5_PIN                  OFF //    OFF, AUX, DS2413, n. Where n is ds2413 s/n (gpio0) or n=0 to 255 (pin#.)      Adjust
#define FEATURE6_NAME          "FEATURE6" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE6_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE6_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE6_PIN                  OFF //    OFF, AUX, CHAIN (ds2413 gpio1,) n. Where n=0 to 255 (pin#.)                   Adjust
#define FEATURE7_NAME          "FEATURE7" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE7_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE7_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE7_PIN                  OFF //    OFF, AUX, DS2413, n. Where n is ds2413 s/n (gpio0) or n=0 to 255 (pin#.)      Adjust
#define FEATURE8_NAME          "FEATURE8" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE8_PURPOSE              OFF //    OFF, SWITCH, ANALOG, DEW_HEATER, INTERVALOMETER.                              Adjust
#define FEATURE8_TEMP                 OFF //    OFF, DS1820, n. Where n is the ds18b20 s/n for DEW_HEATER temperature.        Adjust
#define FEATURE8_PIN                  OFF //    OFF, AUX, CHAIN (ds2413 gpio1,) n. Where n=0 to 255 (pin#.)                   Adjust

// THAT'S IT FOR USER CONFIGURATION!

// -------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 3
