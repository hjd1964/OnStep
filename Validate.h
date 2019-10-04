// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// correct for configuration backwards compatability

// N/A

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// setup defaults

// default allowed degrees past the meridian on the East and West sides of the pier
#define AXIS1_LIMIT_MERIDIAN_E 7.5 
#define AXIS1_LIMIT_MERIDIAN_W 7.5

// always enable the separate pulse guide rate
#define SEPARATE_PULSE_GUIDE_RATE ON

// automatically set MaxRate from SLEW_RATE_BASE_DESIRED
#define MaxRate ((1000000.0/SLEW_RATE_BASE_DESIRED)/AXIS1_STEPS_PER_DEGREE)

// automatically calculate the pecBufferSize
#define PEC_BUFFER_SIZE ceil(AXIS1_STEPS_PER_WORMROT/(AXIS1_STEPS_PER_DEGREE/240.0))

// figure out how many align star are allowed for the configuration
#if defined(MAX_NUM_ALIGN_STARS)
  #if MAX_NUM_ALIGN_STARS > '9' || MAX_NUM_ALIGN_STARS < '6'
    #error "MAX_NUM_ALIGN_STARS must be 6 to 9"
  #else
    #warning "MAX_NUM_ALIGN_STARS explicitly defined in Config file. Controller may be slow for a few minutes after last star align."
  #endif
#else
  #if defined(HAL_FAST_PROCESSOR)
    #define MAX_NUM_ALIGN_STARS '9'
  #else
    #define MAX_NUM_ALIGN_STARS '6'
  #endif
#endif

// Set disable state from EN state for stepper drivers
#if AXIS1_DRIVER_ENABLE == LOW
  #define AXIS1_DRIVER_DISABLE HIGH
#endif
#if AXIS1_DRIVER_ENABLE == HIGH
  #define AXIS1_DRIVER_DISABLE LOW
#endif

#if AXIS2_DRIVER_ENABLE == LOW
  #define AXIS2_DRIVER_DISABLE HIGH
#endif
#if AXIS2_DRIVER_ENABLE == HIGH
  #define AXIS2_DRIVER_DISABLE LOW
#endif

#if AXIS3_DRIVER_ENABLE == LOW
  #define AXIS3_DRIVER_DISABLE HIGH
#endif
#if AXIS3_DRIVER_ENABLE == HIGH
  #define AXIS3_DRIVER_DISABLE LOW
#endif

#if AXIS4_DRIVER_ENABLE == LOW
  #define AXIS4_DRIVER_DISABLE HIGH
#endif
#if AXIS4_DRIVER_ENABLE == HIGH
  #define AXIS4_DRIVER_DISABLE LOW
#endif

#if AXIS5_DRIVER_ENABLE == LOW
  #define AXIS5_DRIVER_DISABLE HIGH
#endif
#if AXIS5_DRIVER_ENABLE == HIGH
  #define AXIS5_DRIVER_DISABLE LOW
#endif

// make sure all stepper driver advanced options are present even if not defined
#ifndef AXIS1_DRIVER_DECAY_MODE
  #define AXIS1_DRIVER_DECAY_MODE OFF
#endif
#ifndef AXIS1_DRIVER_DECAY_MODE_GOTO
  #define AXIS1_DRIVER_DECAY_MODE_GOTO OFF
#endif
#ifndef AXIS1_DRIVER_INTPOL
  #define AXIS1_DRIVER_INTPOL OFF
#endif
#ifndef AXIS1_DRIVER_IRUN
  #define AXIS1_DRIVER_IRUN OFF
#endif
#ifndef AXIS1_DRIVER_IGOTO
  #define AXIS1_DRIVER_IGOTO OFF
#endif
#ifndef AXIS2_DRIVER_DECAY_MODE
  #define AXIS2_DRIVER_DECAY_MODE OFF
#endif

#ifndef AXIS2_DRIVER_DECAY_MODE
  #define AXIS2_DRIVER_DECAY_MODE OFF
#endif
#ifndef AXIS2_DRIVER_DECAY_MODE_GOTO
  #define AXIS2_DRIVER_DECAY_MODE_GOTO OFF
#endif
#ifndef AXIS2_DRIVER_INTPOL
  #define AXIS2_DRIVER_INTPOL OFF
#endif
#ifndef AXIS2_DRIVER_IRUN
  #define AXIS2_DRIVER_IRUN OFF
#endif
#ifndef AXIS2_DRIVER_IGOTO
  #define AXIS2_DRIVER_IGOTO OFF
#endif

#ifndef AXIS3_DRIVER_DECAY_MODE
  #define AXIS3_DRIVER_DECAY_MODE OFF
#endif
#ifndef AXIS3_DRIVER_INTPOL
  #define AXIS3_DRIVER_INTPOL OFF
#endif
#ifndef AXIS3_DRIVER_IRUN
  #define AXIS3_DRIVER_IRUN OFF
#endif

#ifndef AXIS4_DRIVER_DECAY_MODE
  #define AXIS4_DRIVER_DECAY_MODE OFF
#endif
#ifndef AXIS4_DRIVER_INTPOL
  #define AXIS4_DRIVER_INTPOL OFF
#endif
#ifndef AXIS4_DRIVER_IRUN
  #define AXIS4_DRIVER_IRUN OFF
#endif

#ifndef AXIS5_DRIVER_DECAY_MODE
  #define AXIS5_DRIVER_DECAY_MODE OFF
#endif
#ifndef AXIS5_DRIVER_INTPOL
  #define AXIS5_DRIVER_INTPOL OFF
#endif
#ifndef AXIS5_DRIVER_IRUN
  #define AXIS5_DRIVER_IRUN OFF
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// Config.h FILE VALIDATION

#ifndef PINMAP
  #error "Configuration (Config.h): Setting PINMAP must be present!"
#elif PINMAP < PINMAP_FIRST || PINMAP > PINMAP_LAST
  #error "Configuration (Config.h): You must specify a PINMAP!"
#endif

#ifndef SERIAL_A_BAUD_DEFAULT
  #error "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT must be present!"
#elif SERIAL_A_BAUD_DEFAULT != 9600 && SERIAL_A_BAUD_DEFAULT != 19200 && SERIAL_A_BAUD_DEFAULT != 38400 && SERIAL_A_BAUD_DEFAULT != 57600 && SERIAL_A_BAUD_DEFAULT != 115200
  #error "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT invalid, use 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif
#ifndef SERIAL_B_BAUD_DEFAULT
  #error "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT must be present!"
#elif SERIAL_B_BAUD_DEFAULT != OFF && SERIAL_B_BAUD_DEFAULT != 9600 && SERIAL_B_BAUD_DEFAULT != 19200 && SERIAL_B_BAUD_DEFAULT != 38400 && SERIAL_B_BAUD_DEFAULT != 57600 && SERIAL_B_BAUD_DEFAULT != 115200
  #error "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT invalid, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
  #error "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT must be present!"
#elif !defined(HAL_SERIAL_C_BLUETOOTH)
  #if SERIAL_C_BAUD_DEFAULT != ON && SERIAL_C_BAUD_DEFAULT != OFF && SERIAL_C_BAUD_DEFAULT != 9600 && SERIAL_C_BAUD_DEFAULT != 19200 && SERIAL_C_BAUD_DEFAULT != 38400 && SERIAL_C_BAUD_DEFAULT != 57600 && SERIAL_C_BAUD_DEFAULT != 115200
    #error "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT invalid, use ON/OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
  #endif
#endif

#ifndef MOUNT_TYPE
  #error "Configuration (Config.h): Setting MOUNT_TYPE must be present!"
#elif MOUNT_TYPE != OFF && (MOUNT_TYPE < MOUNT_TYPE_FIRST || MOUNT_TYPE > MOUNT_TYPE_LAST)
  #error "Configuration (Config.h): Setting MOUNT_TYPE use OFF, GEM, FORK, or ALTAZM."
#endif

#ifndef LED_STATUS
  #error "Configuration (Config.h): Setting LED_STATUS must be present!"
#elif LED_STATUS != OFF && LED_STATUS != ON
  #error "Configuration (Config.h): Setting LED_STATUS invalid, use OFF or ON only."
#endif

#ifndef LED_STATUS2
  #error "Configuration (Config.h): Setting LED_STATUS2 must be present!"
#elif LED_STATUS2 != OFF && LED_STATUS2 != ON
  #error "Configuration (Config.h): Setting LED_STATUS2 invalid, use OFF or ON only."
#endif

#ifndef LED_RETICLE
  #error "Configuration (Config.h): Setting LED_RETICLE must be present!"
#elif LED_RETICLE != OFF && LED_RETICLE != ON && (LED_RETICLE < 0 || LED_RETICLE > 255)
  #error "Configuration (Config.h): Setting LED_RETICLE invalid, use OFF, ON, or a number between 0 and 255 (0 to 100% power.)"
#endif

#ifndef BUZZER
  #error "Configuration (Config.h): Setting BUZZER must be present!"
#elif BUZZER != OFF && BUZZER != ON && (BUZZER < 100 || BUZZER > 6000)
  #error "Configuration (Config.h): Setting BUZZER invalid, use OFF or ON only or a number between 100 and 6000 (Hz.)"
#endif

#ifndef BUZZER_STATE_DEFAULT
  #error "Configuration (Config.h): Setting BUZZER_STATE_DEFAULT must be present!"
#elif BUZZER_STATE_DEFAULT != OFF && BUZZER_STATE_DEFAULT != ON
  #error "Configuration (Config.h): Setting BUZZER_STATE_DEFAULT invalid, use OFF or ON only."
#endif

#ifndef RTC
  #error "Configuration (Config.h): Setting RTC must be present!"
#elif RTC != OFF && (RTC < RTC_FIRST || RTC > RTC_LAST)
  #error "Configuration (Config.h): Setting RTC invalid, use OFF or DS3231, DS3234, TEENSY only."
#endif

#ifndef WEATHER
  #error "Configuration (Config.h): Setting WEATHER must be present!"
#elif WEATHER != OFF && (WEATHER < WEATHER_FIRST || WEATHER > WEATHER_LAST)
  #error "Configuration (Config.h): Setting WEATHER sensor invalid, use OFF or BME280, BME280_0x76, BME280SPI only."
#endif

#ifndef TELESCOPE_TEMPERATURE
  #error "Configuration (Config.h): Setting TELESCOPE_TEMPERATURE must be present!"
#elif TELESCOPE_TEMPERATURE != OFF && (TELESCOPE_TEMPERATURE < TELESCOPE_TEMPERATURE_FIRST || TELESCOPE_TEMPERATURE > TELESCOPE_TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting TELESCOPE_TEMPERATURE sensor invalid, use OFF or DS1820 only."
#endif

#ifndef HOME_SENSE
  #error "Configuration (Config.h): Setting HOME_SENSE must be present!"
#elif HOME_SENSE != OFF && HOME_SENSE != ON && HOME_SENSE != ON_PULLUP && HOME_SENSE != ON_PULLDOWN
  #error "Configuration (Config.h): Setting HOME_SENSE invalid, use OFF, ON, ON_PULLUP, or ON_PULLDOWN only."
#endif

#ifndef HOME_SENSE_STATE_AXIS1
  #error "Configuration (Config.h): Setting HOME_SENSE_STATE_AXIS1 must be present!"
#elif HOME_SENSE_STATE_AXIS1 != HIGH && HOME_SENSE_STATE_AXIS1 != LOW
  #error "Configuration (Config.h): Setting HOME_SENSE_STATE_AXIS1 invalid, use HIGH or LOW."
#endif

#ifndef HOME_SENSE_STATE_AXIS2
  #error "Configuration (Config.h): Setting HOME_SENSE_STATE_AXIS2 must be present!"
#elif HOME_SENSE_STATE_AXIS2 != HIGH && HOME_SENSE_STATE_AXIS2 != LOW
  #error "Configuration (Config.h): Setting HOME_SENSE_STATE_AXIS2 invalid, use HIGH or LOW."
#endif

#ifndef LIMIT_SENSE
  #error "Configuration (Config.h): Setting LIMIT_SENSE must be present!"
#elif LIMIT_SENSE != OFF && LIMIT_SENSE != ON && LIMIT_SENSE != ON_PULLUP && LIMIT_SENSE != ON_PULLDOWN
  #error "Configuration (Config.h): Setting LIMIT_SENSE invalid, use OFF, ON, ON_PULLUP, or ON_PULLDOWN only."
#endif

#ifndef LIMIT_SENSE_STATE
  #error "Configuration (Config.h): Setting LIMIT_SENSE_STATE must be present!"
#elif LIMIT_SENSE_STATE != HIGH && LIMIT_SENSE_STATE != LOW
  #error "Configuration (Config.h): Setting LIMIT_SENSE_STATE invalid, use HIGH or LOW."
#endif

#ifndef PEC_SENSE
  #error "Configuration (Config.h): Setting PEC_SENSE must be present!"
#elif PEC_SENSE != OFF && PEC_SENSE != ON && PEC_SENSE != ON_PULLUP && PEC_SENSE != ON_PULLDOWN
  #error "Configuration (Config.h): Setting PEC_SENSE invalid, use OFF, ON, ON_PULLUP, or ON_PULLDOWN only."
#endif

#ifndef PEC_SENSE_STATE
  #error "Configuration (Config.h): Setting PEC_SENSE_STATE must be present!"
#elif PEC_SENSE_STATE != HIGH && PEC_SENSE_STATE != LOW
  #error "Configuration (Config.h): Setting PEC_SENSE_STATE invalid, use HIGH or LOW."
#endif

#ifndef PPS_SENSE
  #error "Configuration (Config.h): Setting PPS_SENSE must be present!"
#elif PPS_SENSE != OFF && PPS_SENSE != ON && PPS_SENSE != ON_PULLUP && PPS_SENSE != ON_PULLDOWN
  #error "Configuration (Config.h): Setting PPS_SENSE invalid, use OFF, ON, ON_PULLUP, or ON_PULLDOWN only."
#endif

#ifndef ST4_INTERFACE
  #error "Configuration (Config.h): Setting ST4_INTERFACE must be present!"
#elif ST4_INTERFACE != OFF && ST4_INTERFACE != ON && ST4_INTERFACE != ON_PULLUP
  #error "Configuration (Config.h): Setting ST4_INTERFACE invalid, use OFF, ON, ON_PULLUP only."
#endif

#ifndef ST4_HAND_CONTROL
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL must be present!"
#elif ST4_HAND_CONTROL != OFF && ST4_HAND_CONTROL != ON
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL invalid, use OFF or ON only."
#endif

#ifndef ST4_HAND_CONTROL_FOCUSER
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL_FOCUSER must be present!"
#elif ST4_HAND_CONTROL_FOCUSER != OFF && ST4_HAND_CONTROL_FOCUSER != ON
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL_FOCUSER invalid, use OFF or ON only."
#endif

#ifndef GUIDE_TIME_LIMIT
  #error "Configuration (Config.h): Setting GUIDE_TIME_LIMIT must be present!"
#elif GUIDE_TIME_LIMIT < 0 || GUIDE_TIME_LIMIT > 120
  #error "Configuration (Config.h): Setting GUIDE_TIME_LIMIT invalid, use a number between 0 (no time limit) and 120 (seconds.)"
#endif

#ifndef GUIDE_DISABLE_BACKLASH
  #error "Configuration (Config.h): Setting GUIDE_DISABLE_BACKLASH must be present!"
#elif GUIDE_DISABLE_BACKLASH != OFF && GUIDE_DISABLE_BACKLASH != ON
  #error "Configuration (Config.h): Setting GUIDE_DISABLE_BACKLASH invalid, use OFF or ON only."
#endif

#ifndef TRACK_AUTOSTART
  #error "Configuration (Config.h): Setting TRACK_AUTOSTART must be present!"
#elif TRACK_AUTOSTART != OFF && TRACK_AUTOSTART != ON
  #error "Configuration (Config.h): Setting TRACK_AUTOSTART invalid, use OFF or ON only."
#endif

#ifndef TRACK_REFRACTION_RATE_DEFAULT
  #error "Configuration (Config.h): Setting TRACK_REFRACTION_RATE_DEFAULT must be present!"
#elif TRACK_REFRACTION_RATE_DEFAULT != OFF && TRACK_REFRACTION_RATE_DEFAULT != ON
  #error "Configuration (Config.h): Setting TRACK_REFRACTION_RATE_DEFAULT invalid, use OFF or ON only."
#endif

#ifndef TRACK_BACKLASH_RATE
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE must be present!"
#elif TRACK_BACKLASH_RATE < 5 || TRACK_BACKLASH_RATE > 100
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE invalid, use a number between 5 and 100 (x sidereal rate.)"
#endif

#ifndef SYNC_CURRENT_PIER_SIDE_ONLY
  #error "Configuration (Config.h): Setting SYNC_CURRENT_PIER_SIDE_ONLY must be present!"
#elif SYNC_CURRENT_PIER_SIDE_ONLY != OFF && SYNC_CURRENT_PIER_SIDE_ONLY != ON
  #error "Configuration (Config.h): Setting SYNC_CURRENT_PIER_SIDE_ONLY invalid, use OFF or ON only."
#endif

#ifndef SLEW_RATE_MEMORY
  #error "Configuration (Config.h): Setting SLEW_RATE_MEMORY must be present!"
#elif SLEW_RATE_MEMORY != OFF && SLEW_RATE_MEMORY != ON
  #error "Configuration (Config.h): Setting SLEW_RATE_MEMORY invalid, use OFF or ON only."
#endif

#ifndef MFLIP_SKIP_HOME
  #error "Configuration (Config.h): Setting MFLIP_SKIP_HOME must be present!"
#elif MFLIP_SKIP_HOME != OFF && MFLIP_SKIP_HOME != ON
  #error "Configuration (Config.h): Setting MFLIP_SKIP_HOME invalid, use OFF or ON only."
#endif

#ifndef MFLIP_PAUSE_HOME_MEMORY
  #error "Configuration (Config.h): Setting MFLIP_PAUSE_HOME_MEMORY must be present!"
#elif MFLIP_PAUSE_HOME_MEMORY != OFF && MFLIP_PAUSE_HOME_MEMORY != ON
  #error "Configuration (Config.h): Setting MFLIP_PAUSE_HOME_MEMORY invalid, use OFF or ON only."
#endif

#ifndef MFLIP_AUTOMATIC_MEMORY
  #error "Configuration (Config.h): Setting MFLIP_AUTOMATIC_MEMORY must be present!"
#elif MFLIP_AUTOMATIC_MEMORY != OFF && MFLIP_AUTOMATIC_MEMORY != ON
  #error "Configuration (Config.h): Setting MFLIP_AUTOMATIC_MEMORY invalid, use OFF or ON only."
#endif

#ifndef STRICT_PARKING
  #error "Configuration (Config.h): Setting STRICT_PARKING must be present!"
#elif STRICT_PARKING != OFF && STRICT_PARKING != ON
  #error "Configuration (Config.h): Setting STRICT_PARKING invalid, use OFF or ON only."
#endif

#ifndef STEP_WAVE_FORM
  #error "Configuration (Config.h): Setting STEP_WAVE_FORM must be present!"
#elif STEP_WAVE_FORM != PULSE && STEP_WAVE_FORM != SQUARE && STEP_WAVE_FORM != DEDGE
  #error "Configuration (Config.h): Setting STEP_WAVE_FORM invalid, use PULSE, SQUARE, or DEDGE only."
#endif

#ifndef AXIS1_LIMIT_UNDER_POLE
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_UNDER_POLE must be present!"
#elif AXIS1_LIMIT_UNDER_POLE < 150 || AXIS1_LIMIT_UNDER_POLE > 180
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_UNDER_POLE invalid, use a number between 150 and 180 (degrees.)"
#endif

#ifndef AXIS1_LIMIT_MAXAZM
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAXAZM must be present!"
#elif AXIS1_LIMIT_MAXAZM < 180 || AXIS1_LIMIT_MAXAZM > 360
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAXAZM invalid, use a number between 180 and 360 (degrees.)"
#endif

#ifndef AXIS2_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN must be present!"
#elif (AXIS2_LIMIT_MIN < -91 || AXIS2_LIMIT_MIN > 0)
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN invalid, use a number between -91 and 0 (degrees.)  -91 = disabled."
#endif

#ifndef AXIS2_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX must be present!"
#elif (AXIS2_LIMIT_MAX < 0 || AXIS2_LIMIT_MAX > 91)
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX invalid, use a number between 0 and 91 (degrees.)  91 = disabled."
#endif

#ifndef AXIS3_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MIN must be present!"
#elif (AXIS3_LIMIT_MIN < -360 || AXIS3_LIMIT_MIN > 0)
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MIN invalid, use a number between -360 and 0 (degrees.)"
#endif

#ifndef AXIS3_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MAX must be present!"
#elif (AXIS3_LIMIT_MAX < 0 || AXIS3_LIMIT_MAX > 360)
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MAX invalid, use a number between 0 and 360 (degrees.)"
#endif

#ifndef AXIS4_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE must be present!"
#elif AXIS4_LIMIT_MIN_RATE < 1 || AXIS4_LIMIT_MIN_RATE > 1000
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
#endif

#ifndef AXIS4_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN must be present!"
#elif (AXIS4_LIMIT_MIN < -300 || AXIS4_LIMIT_MIN > 300) && AXIS4_LIMIT_MIN < AXIS4_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN invalid, use a number between -300 and 300 (mm) but < AXIS4_LIMIT_MAX."
#endif

#ifndef AXIS4_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MAX must be present!"
#elif (AXIS4_LIMIT_MAX < -300 || AXIS4_LIMIT_MAX > 300) && AXIS4_LIMIT_MAX > AXIS4_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MAX invalid, use a number between -300 and 300 (mm) but > AXIS4_LIMIT_MIN."
#endif

#ifndef AXIS5_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE must be present!"
#elif AXIS5_LIMIT_MIN_RATE < 1 || AXIS5_LIMIT_MIN_RATE > 1000
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
#endif

#ifndef AXIS5_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN must be present!"
#elif (AXIS5_LIMIT_MIN < -300 || AXIS5_LIMIT_MIN > 300) && AXIS5_LIMIT_MIN < AXIS5_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN invalid, use a number between -300 and 300 (mm) but < AXIS5_LIMIT_MAX."
#endif

#ifndef AXIS5_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MAX must be present!"
#elif (AXIS5_LIMIT_MAX < -300 || AXIS5_LIMIT_MAX > 300) && AXIS5_LIMIT_MAX > AXIS5_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MAX invalid, use a number between -300 and 300 (mm) but > AXIS5_LIMIT_MIN."
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// stepper driver mode setup validation

// step wave form
#if STEP_WAVE_FORM != PULSE
  #undef HAL_PULSE_WIDTH
  // set pulse width in SQW and DEDGE mode well above triggering PULSE check code
  #define HAL_PULSE_WIDTH 10000
#endif

// check model
#if (AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL == OFF) || (AXIS2_DRIVER_MODEL != OFF && AXIS1_DRIVER_MODEL == OFF)
  #error "Configuration (Config.h): AXISn_DRIVER_MODEL; If using the driver mode control you must use it for both AXIS1 and AXIS2."
#endif

#if AXIS1_DRIVER_MODEL != OFF

  // validate tracking and goto micro-step modes
  #if AXIS1_DRIVER_MICROSTEPS == OFF
    #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS must be set if using AXIS1_DRIVER_MODEL!"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS == OFF
    #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS must be set if using AXIS2_DRIVER_MODEL!"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if (AXIS1_DRIVER_MICROSTEPS <= AXIS1_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS1_DRIVER_MICROSTEPS == AXIS1_DRIVER_MICROSTEPS_GOTO))
        #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO should be less than AXIS1_DRIVER_MICROSTEPS or OFF"
    #endif
    #define AXIS1_DRIVER_STEP_GOTO (AXIS1_DRIVER_MICROSTEPS/AXIS1_DRIVER_MICROSTEPS_GOTO)
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if (AXIS2_DRIVER_MICROSTEPS <= AXIS2_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS2_DRIVER_MICROSTEPS == AXIS2_DRIVER_MICROSTEPS_GOTO))
        #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO should be less than AXIS2_DRIVER_MICROSTEPS or OFF"
    #endif
    #define AXIS2_DRIVER_STEP_GOTO (AXIS2_DRIVER_MICROSTEPS/AXIS2_DRIVER_MICROSTEPS_GOTO)
  #endif

  // initialize custom stepper driver values
  #include "src/step_dir_drivers/TMC2100_Init.h"
  #include "src/step_dir_drivers/TMC2130_Init.h"
  #include "src/step_dir_drivers/TMC2209_Init.h"
  #include "src/step_dir_drivers/TMC5160_Init.h"
  #include "src/step_dir_drivers/TMC_SPI_Init.h"

  // check to see if a valid stepper driver code exists
  #if (AXIS1_DRIVER_MODEL < DRIVER_MODEL_FIRST) || (AXIS1_DRIVER_MODEL > DRIVER_MODEL_LAST)
    #error "Configuration (Config.h): AXIS1_DRIVER_MODEL unknown stepper driver specified."
  #endif
  #if (AXIS2_DRIVER_MODEL < DRIVER_MODEL_FIRST) || (AXIS2_DRIVER_MODEL > DRIVER_MODEL_LAST)
    #error "Configuration (Config.h): AXIS2_DRIVER_MODEL unknown stepper driver specified."
  #endif
  #if ROTATOR == ON && (AXIS3_DRIVER_MODEL != OFF) && (AXIS3_DRIVER_MODEL != TMC_SPI)
    #error "Configuration (Config.h): AXIS3_DRIVER_MODEL unknown stepper driver specified."
  #endif
  #if FOCUSER1 == ON && (AXIS4_DRIVER_MODEL != OFF) && (AXIS4_DRIVER_MODEL != TMC_SPI)
    #error "Configuration (Config.h): AXIS4_DRIVER_MODEL unknown stepper driver specified."
  #endif
  #if FOCUSER2 == ON && (AXIS5_DRIVER_MODEL != OFF) && (AXIS5_DRIVER_MODEL != TMC_SPI)
    #error "Configuration (Config.h): AXIS5_DRIVER_MODEL unknown stepper driver specified."
  #endif

  // validate stepper driver setup
  #include "src/step_dir_drivers/A4988_Validate.h"
  #include "src/step_dir_drivers/DRV8825_Validate.h"
  #include "src/step_dir_drivers/RAPS128_Validate.h"
  #include "src/step_dir_drivers/LV8729_Validate.h"
  #include "src/step_dir_drivers/S109_Validate.h"
  #include "src/step_dir_drivers/ST820_Validate.h"
  #include "src/step_dir_drivers/TMC2100_Validate.h"
  #include "src/step_dir_drivers/TMC2208_Validate.h"
  #include "src/step_dir_drivers/TMC2209_Validate.h"
  #include "src/step_dir_drivers/TMC_SPI_Validate.h"

  // for stepper drivers where AXISn_MICROSTEPS_GOTO must be defined
  #if MODE_SWITCH_BEFORE_SLEW != OFF
    #if AXIS1_DRIVER_MICROSTEPS != OFF && AXIS1_DRIVER_MICROSTEPS_GOTO == OFF
      #undef AXIS1_DRIVER_MICROSTEPS_GOTO 
      #define AXIS1_DRIVER_MICROSTEPS_GOTO AXIS1_DRIVER_MICROSTEPS
    #endif
    #if AXIS2_DRIVER_MICROSTEPS != OFF && AXIS2_DRIVER_MICROSTEPS_GOTO == OFF
      #undef AXIS2_DRIVER_MICROSTEPS_GOTO 
      #define AXIS2_DRIVER_MICROSTEPS_GOTO AXIS2_DRIVER_MICROSTEPS
    #endif
  #endif

#else
  #warning "Configuration (Config.h): Stepper drivers for Axis1 and Axis2 are not defined.  Be sure to properly configure micro-step mode, Vref/current, etc. manually with shunts, dip-switches, as required."
#endif

// if AXISn_DRIVER_STEP_GOTO isn't defined, do so
#ifndef AXIS1_DRIVER_STEP_GOTO
  #define AXIS1_DRIVER_STEP_GOTO 1
#endif
#ifndef AXIS2_DRIVER_STEP_GOTO
  #define AXIS2_DRIVER_STEP_GOTO 1
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// misc. validation

// check for conflicted pin use, dedicated pins are ignored

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #if PINMAP == MiniPCB || PINMAP == MiniPCB2 || PINMAP == MaxPCB || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX1
      #define USES_AUX1 false
    #else
      #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but Aux1 is already in use, choose one feature on Aux1."
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == MaxPCB || PINMAP == MaxTMC || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but Aux2 is already in use, choose one feature on Aux2."
    #endif
  #endif
  #if PINMAP == Classic || PINMAP == Mega2560Alt || PINMAP == MiniPCB2 || PINMAP == MaxPCB2
    #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported on this PINMAP"
  #endif
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS == ON
  #if PINMAP == MaxESP || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX0
      #define USES_AUX0 false  // false means the AUX0 doesn't have I2C sharing
    #else
      #error "Configuration (Config.h): LED_STATUS enabled but Aux0 is already in use, choose one feature on Aux0"
    #endif
  #endif
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration (Config.h): LED_STATUS enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
#endif
#if LED_STATUS2 == ON
  #if PINMAP == MaxESP || PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration (Config.h): LED_STATUS2 enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration (Config.h): LED_STATUS2 enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif
#if LED_RETICLE == ON
  #if PINMAP == MaxESP || PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration (Config.h): LED_RETICLE enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX4
      #define USES_AUX4 
    #else
      #error "Configuration (Config.h): LED_RETICLE enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif
#if BUZZER == ON
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8 
    #else
      #error "Configuration (Config.h): BUZZER enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
#endif

// TIME -------------------------------------
#if RTC == DS3231
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX0
      #define USES_AUX0 true  // true means the AUX0 has I2C sharing
    #else
      #if USES_AUX0 != true
        #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
      #endif
    #endif
    #ifndef USES_AUX1
      #define USES_AUX1 true  // true means the AUX1 has I2C sharing
    #else
      #if USES_AUX1 != true
        #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
      #endif
    #endif
  #endif
#endif
#if PPS_SENSE == ON
  #if PINMAP == MaxESP
    #ifndef USES_AUX0
      #define USES_AUX0
    #else
      #error "Configuration (Config.h): PPS_SENSE enabled but Aux0 is already in use, choose one feature on Aux0"
    #endif
  #endif
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX7
      #define USES_AUX7
    #else
      #error "Configuration (Config.h): PPS_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
    #endif
  #endif
#endif

// SENSORS ----------------------------------
#if WEATHER == BME280
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX0
      #define USES_AUX0 true  // true means the AUX0 has I2C sharing
    #else
      #if USES_AUX0 != true
        #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
      #endif
    #endif
    #ifndef USES_AUX1
      #define USES_AUX1 true  // true means the AUX1 has I2C sharing
    #else
      #if USES_AUX1 != true
        #error "Configuration (Config.h): RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
      #endif
    #endif
  #endif
#endif
#if TELESCOPE_TEMPERATURE == DS1820
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration (Config.h): RTC DS1820 OneWire interface enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxTMC
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration (Config.h): RTC DS1820 OneWire interface enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif
#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
#endif
#if LIMIT_SENSE != OFF
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC
    #ifndef USES_AUX7
      #define USES_AUX7
    #else
      #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX3
      #define USES_AUX3
    #else
      #error "Configuration (Config.h): LIMIT_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
    #endif
  #endif
#endif
#if HOME_SENSE != OFF
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == Ramps14
    #ifndef USES_AUX3
      #define USES_AUX3
    #else
      #error "Configuration (Config.h): HOME_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
    #endif
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration (Config.h): HOME_SENSE enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  #if PINMAP == MaxESP2 || PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == STM32Blue
    #ifndef USES_AUX1
      #define USES_AUX1
    #else
      #error "Configuration (Config.h): AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
    #endif
  #endif
  #if PINMAP == MaxTMC || PINMAP == MiniEMB
    #if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
      #error "Configuration (Config.h): AXIS2_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
    #endif
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MiniPCB || PINMAP == MiniPCB2 || PINMAP == STM32Blue
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration (Config.h): AXIS2_DRIVER_STATUS enabled but Aux2 is already in use, choose one feature on Aux2"
    #endif
  #endif
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  #if PINMAP == MaxESP3 || PINMAP == MaxESP2 || PINMAP == MaxTMC
      #error "Configuration (Config.h): AXIS2_DRIVER_POWER_DOWN not supported on this PINMAP"
  #endif
#endif

// MOTION CONTROL FOCUSERS/ROTATOR ----------

// focusers/rotator allowed?
#if ROTATOR == ON
  #if Axis3StepPin == -1 || Axis3DirPin == -1
    #error "Configuration (Config.h): AXIS3 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER1 == ON
  #if Axis4StepPin == -1 || Axis4DirPin == -1
    #error "Configuration (Config.h): AXIS4 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON
  #if (Axis5StepPin == -1 || Axis5DirPin == -1) && AXIS5_DRIVER_DC_MODE == OFF
    #error "Configuration (Config.h): AXIS5 step/dir interface is not supported on this PINMAP"
  #endif
#endif

// focuser/rotators any invalid combinations?
#if FOCUSER1 == ON && ROTATOR == ON
  #if PINMAP == MaxESP2 || PINMAP == STM32Blue
    #error "Configuration (Config.h): AXIS3 and AXIS4 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON && ROTATOR == ON
  #if PINMAP == Classic || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #error "Configuration (Config.h): AXIS3 and AXIS5 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

// focuser/rotators any invalid EN pin use?
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  #if Axis3_EN == -1
    #error "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN enabled but not supported with this PINMAP"
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN early v1.x MaxPCB boards had the option to use Aux3 for EN control, this is no longer supported (now uses Aux5.)"
    #endif
    #ifndef USES_AUX5
      #warning "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN using Aux5."
      #define USES_AUX5
    #else
      #error "Configuration (Config.h): AXIS3_DRIVER_POWER_DOWN enabled but Aux5 is already in use, choose one feature on Aux5"
    #endif
  #endif
#endif
#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  #if AXIS4_DRIVER_POWER_DOWN == ON && AXIS4_DRIVER_DC_MODE != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis4_EN == -1
    #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support which this PINMAP doesn't have"
  #endif
  #if PINMAP == MaxESP3
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux2 is already in use, choose one feature on Aux2"
    #endif
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support, early v1.x MaxPCB boards had the option to use Aux4 for EN control but this is no longer supported."
    #endif
    #ifndef USES_AUX6
      #define USES_AUX6
    #else
      #error "Configuration (Config.h): AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux6 is already in use, choose one feature on Aux6"
    #endif
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON
  #if AXIS5_DRIVER_POWER_DOWN == ON && AXIS5_DRIVER_DC_MODE != OFF
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN and AXIS5_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis5_EN == -1
    #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN requires ENable signal support which this PINMAP doesn't have"
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN early v1.x MaxPCB boards had the option to use Aux3 for EN control, this is no longer supported (now uses Aux5.)"
    #endif
    #ifndef USES_AUX5
      #define USES_AUX5
    #else
      #error "Configuration (Config.h): AXIS5_DRIVER_POWER_DOWN enabled but Aux5 is already in use, choose one feature on Aux5"
    #endif
  #endif
#endif

// focuser/rotators any invalid focuser DC modes?
#if !(FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE != OFF)
  #if AXIS4_DRIVER_DC_MODE == XDIRECT && AXIS4_DRIVER_MODEL != TMC_SPI
    #error "Configuration (Config.h): AXIS4_DRIVER_DC_MODE error XDIRECT is for TMC stepper drivers only."
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_DC_MODE != OFF
  #if !(FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE != OFF)
    #error "Configuration (Config.h): AXIS5_DRIVER_DC_MODE requires enabling FOCUSER1 in AXIS4_DRIVER_DC_MODE.  In this mode FOCUSER2 USES the FOCUSER1 stepper driver"
  #endif
#endif

// PINMAP SPECIFIC VALIDATION ----------------------

#if PINMAP == MaxTMC || PINMAP ==MksGenL2
  // special checks for SPI buss pin maps to ensure all enabled stepper drivers are TMC2130 or TMC5160 in SPI mode
  #if AXIS1_DRIVER_MODEL != TMC_SPI
    #error "Configuration (Config.h): AXIS1_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
  #endif
  #if AXIS2_DRIVER_MODEL != TMC_SPI
    #error "Configuration (Config.h): AXIS2_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
  #endif
  #if ROTATOR == ON
    #if AXIS3_DRIVER_MODEL != TMC_SPI
      #error "Configuration (Config.h): AXIS3_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
  #if FOCUSER1 == ON
    #if AXIS4_DRIVER_MODEL != TMC_SPI
      #error "Configuration (Config.h): AXIS4_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
  #if FOCUSER2 == ON
    #if AXIS5_DRIVER_MODEL != TMC_SPI
      #error "Configuration (Config.h): AXIS5_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
#else
  // and when not MaxTMC to ensure all focuser/rotator stepper drivers off (manual configuration only.)
  #if ROTATOR == ON && AXIS3_DRIVER_MODEL != OFF
    #error "Configuration (Config.h): AXIS3_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
  #if FOCUSER1 == ON && AXIS4_DRIVER_MODEL != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
  #if FOCUSER2 == ON && AXIS5_DRIVER_MODEL != OFF
    #error "Configuration (Config.h): AXIS5_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
#endif

// GENERAL PURPOSE VALIDATION ----------------------

#if SERIAL_B_ESP_FLASHING == ON  && (!defined(ESP8266Gpio0Pin) || !defined(ESP8266RstPin))
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported for this PINMAP"
#endif

#if LED_STATUS != OFF && LEDnegPin == -1
  #error "Configuration (Config.h): LED_STATUS not supported for this PINMAP, must be OFF"
#endif

#if LED_STATUS2 != OFF && LEDneg2Pin == -1
  #error "Configuration (Config.h): LED_STATUS2 not supported for this PINMAP, must be OFF"
#endif

#if LED_RETICLE != OFF && ReticlePin == -1
  #error "Configuration (Config.h): LED_RETICLE not supported for this PINMAP, must be OFF"
#endif

#if PEC_SENSE > 0 && AnalogPecPin == -1
  #error "Configuration (Config.h): PEC_SENSE in ANALOG mode not supported for this PINMAP, use ON, etc. NOT a threshold value"
#endif
