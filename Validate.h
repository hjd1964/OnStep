// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// correct for configuration backwards compatability

// support for old decay mode settings
#ifdef DECAY_MODE
  #define AXIS1_DECAY_MODE DECAY_MODE
  #define AXIS2_DECAY_MODE DECAY_MODE
#endif
#ifdef DECAY_MODE_GOTO
  #define AXIS1_DECAY_MODE_GOTO DECAY_MODE_GOTO
  #define AXIS2_DECAY_MODE_GOTO DECAY_MODE_GOTO
#endif

// automatically set minimum move rates for focusers if they aren't defined
#ifndef AXIS4_MIN_MOVE_RATE
  #define AXIS4_MIN_MOVE_RATE 10
#endif
#ifndef AXIS5_MIN_MOVE_RATE
  #define AXIS5_MIN_MOVE_RATE 10
#endif

// the mount type FORK with Meridian flips enabled for align (only) is not-longer required use _FORK instead
#ifdef MOUNT_TYPE_FORK_ALT
  #define MOUNT_TYPE_FORK
#endif

// automatically set MaxRate if DefaultSlewRate is present
#if !defined(MaxRate) && defined(DesiredBaseSlewRate)
  #define MaxRate ((1000000.0/DesiredBaseSlewRate)/StepsPerDegreeAxis1)
#endif

// replace REMEMBER_MAX_RATE_ON or RememberMaxRate_ON with REMEMBER_SLEW_RATE_ON
#if defined(RememberMaxRate_ON) || defined(REMEMBER_MAX_RATE_ON)
  #ifndef REMEMBER_SLEW_RATE_ON
    #define REMEMBER_SLEW_RATE_ON
  #endif
#endif

// set serial port baud rate the old way
#ifdef SERIAL1_BAUD_DEFAULT
  #define SERIAL_B_BAUD_DEFAULT SERIAL1_BAUD_DEFAULT
#endif
#ifdef SERIAL4_BAUD_DEFAULT
  #define SERIAL_C_BAUD_DEFAULT SERIAL4_BAUD_DEFAULT
#endif

// config pre-version 1
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
  #define SEPARATE_PULSE_GUIDE_RATE_ON
#endif
#ifndef MaxRot
  #define MaxRot MaxAxis3
#endif
#ifndef MinRot
  #define MinRot MinAxis3
#endif

// config version 1
#ifdef REVERSE_AXIS1_ON
  #define AXIS1_REVERSE_ON
#endif
#ifdef REVERSE_AXIS2_ON
  #define AXIS2_REVERSE_ON
#endif
#ifdef REVERSE_AXIS3_ON
  #define AXIS3_REVERSE_ON
#endif
#ifdef REVERSE_AXIS4_ON
  #define AXIS4_REVERSE_ON
#endif
#ifdef REVERSE_AXIS5_ON
  #define AXIS5_REVERSE_ON
#endif
#ifndef AXIS1_DISABLE
  #if defined(AXIS1_DISABLED_HIGH)
    #define AXIS1_DISABLE HIGH
  #endif
  #if defined(AXIS1_DISABLED_LOW)
    #define AXIS1_DISABLE LOW
  #endif
#endif
#ifndef AXIS2_DISABLE
  #if defined(AXIS2_DISABLED_HIGH)
    #define AXIS2_DISABLE HIGH
  #endif
  #if defined(AXIS2_DISABLED_LOW)
    #define AXIS2_DISABLE LOW
  #endif
#endif
#ifdef DISABLE_AXIS3
  #define AXIS3_DISABLE DISABLE_AXIS3
#endif
#ifdef DISABLE_AXIS4
  #define AXIS3_DISABLE DISABLE_AXIS4
#endif
#ifdef DISABLE_AXIS5
  #define AXIS3_DISABLE DISABLE_AXIS5
#endif
#ifdef AXIS1_FAULT_LOW
  #define AXIS1_FAULT LOW
#endif
#ifdef AXIS1_FAULT_HIGH
  #define AXIS1_FAULT HIGH
#endif
#ifdef AXIS1_FAULT_SPI
  #define AXIS1_FAULT TMC_SPI
#endif
#if AXIS1_FAULT==TMC2130
  #undef AXIS1_FAULT
  #define AXIS1_FAULT TMC_SPI
#endif
#ifdef AXIS2_FAULT_LOW
  #define AXIS2_FAULT LOW
#endif
#ifdef AXIS2_FAULT_HIGH
  #define AXIS2_FAULT HIGH
#endif
#ifdef AXIS2_FAULT_SPI
  #define AXIS2_FAULT TMC_SPI
#endif
#if AXIS2_FAULT==TMC2130
  #undef AXIS2_FAULT
  #define AXIS2_FAULT TMC_SPI
#endif
#ifdef AUTO_POWER_DOWN_AXIS2_ON
  #define AXIS2_AUTO_POWER_DOWN_ON
#endif
#ifdef DECAY_MODE_LOW
  #define DECAY_MODE LOW
#endif
#ifdef DECAY_MODE_HIGH
  #define DECAY_MODE HIGH
#endif
#ifdef DECAY_MODE_OPEN
  #define DECAY_MODE OPEN
#endif
#ifdef DECAY_MODE_GOTO_LOW
  #define DECAY_MODE_GOTO LOW
#endif
#ifdef DECAY_MODE_GOTO_HIGH
  #define DECAY_MODE_GOTO HIGH
#endif
#ifdef DECAY_MODE_GOTO_OPEN
  #define DECAY_MODE_GOTO OPEN
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// setup defaults

// Set timer wave mode
#ifndef STEP_WAVE_FORM
  #define STEP_WAVE_FORM SQUARE
#endif
#if STEP_WAVE_FORM!=PULSE && STEP_WAVE_FORM!=SQUARE && STEP_WAVE_FORM!=DEDGE
  #error "Configuration: STEP_WAVE_FORM; Unknown value, please correct your configuration."
#endif
#if STEP_WAVE_FORM != PULSE
  #undef HAL_PULSE_WIDTH
  // Set pulse width in SQW and DEDGE mode well above triggering PULSE check code
  #define HAL_PULSE_WIDTH 10000
#endif

#ifndef GUIDE_TIME_LIMIT
  #define GUIDE_TIME_LIMIT 0
#endif

// set serial port baud rate if not done so already
#ifndef SERIAL_B_BAUD_DEFAULT
  #define SERIAL_B_BAUD_DEFAULT 9600
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
  #define SERIAL_C_BAUD_DEFAULT 9600
#endif

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

// make both enable and disable values
#if AXIS1_DISABLE==LOW
  #define AXIS1_ENABLE HIGH
#endif
#if AXIS1_DISABLE==HIGH
  #define AXIS1_ENABLE LOW
#endif
#if AXIS2_DISABLE==LOW
  #define AXIS2_ENABLE HIGH
#endif
#if AXIS2_DISABLE==HIGH
  #define AXIS2_ENABLE LOW
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// stepper driver mode setup validation
#if (defined(AXIS1_DRIVER_MODEL) && !defined(AXIS2_DRIVER_MODEL)) || (defined(AXIS2_DRIVER_MODEL) && !defined(AXIS1_DRIVER_MODEL))
  #error "AXISn_DRIVER_MODEL; If using the basic driver mode setup you must use it for both AXIS1 and AXIS2."
#endif

#if defined(AXIS1_DRIVER_MODEL)
  // attempting to use the basic (simplified) stepper driver mode setup

  // validate tracking and goto micro-step modes
  #ifndef AXIS1_MICROSTEPS
    #error "AXIS1_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS1_DRIVER_MODEL!"
  #endif
  #ifndef AXIS2_MICROSTEPS
    #error "AXIS2_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS2_DRIVER_MODEL!"
  #endif
  #ifdef AXIS1_MICROSTEPS_GOTO
    #if (AXIS1_MICROSTEPS <= AXIS1_MICROSTEPS_GOTO) && (!((defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)) && (AXIS1_MICROSTEPS == AXIS1_MICROSTEPS_GOTO)))
        #error "Configuration: AXIS1_MICROSTEPS_GOTO should be less than AXIS1_MICROSTEPS or _OFF"
    #endif
    #define AXIS1_STEP_GOTO (AXIS1_MICROSTEPS/AXIS1_MICROSTEPS_GOTO)
  #endif
  #ifdef AXIS2_MICROSTEPS_GOTO
    #if (AXIS2_MICROSTEPS <= AXIS2_MICROSTEPS_GOTO) && (!((defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)) && (AXIS2_MICROSTEPS == AXIS2_MICROSTEPS_GOTO)))
        #error "Configuration: AXIS2_MICROSTEPS_GOTO should be less than AXIS2_MICROSTEPS or _OFF"
    #endif
    #define AXIS2_STEP_GOTO (AXIS2_MICROSTEPS/AXIS2_MICROSTEPS_GOTO)
  #endif

  // initialize custom stepper driver values
  #include "src/step_dir_drivers/TMC2100_Init.h"
  #include "src/step_dir_drivers/TMC2130_Init.h"
  #include "src/step_dir_drivers/TMC2209_Init.h"
  #include "src/step_dir_drivers/TMC5160_Init.h"
  #include "src/step_dir_drivers/TMC_SPI_Init.h"

  // check to see if a valid stepper driver code exists
  #if (AXIS1_DRIVER_MODEL < DRIVER_MODEL_FIRST) || (AXIS1_DRIVER_MODEL > DRIVER_MODEL_LAST)
    #error "AXIS1_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
  #endif
  #if (AXIS2_DRIVER_MODEL < DRIVER_MODEL_FIRST) || (AXIS2_DRIVER_MODEL > DRIVER_MODEL_LAST)
    #error "AXIS2_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
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
  #if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
    #if defined(AXIS1_MICROSTEPS) && !defined(AXIS1_MICROSTEPS_GOTO)
      #define AXIS1_MICROSTEPS_GOTO AXIS1_MICROSTEPS
    #endif
    #if defined(AXIS2_MICROSTEPS) && !defined(AXIS2_MICROSTEPS_GOTO)
      #define AXIS2_MICROSTEPS_GOTO AXIS2_MICROSTEPS
    #endif
  #endif

#else
  // attempting to use the advanced stepper driver mode setup

  #if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
    #if !defined(AXIS1_MODE) || !defined(AXIS1_MODE_GOTO)
      #error "Configuration: AXIS1_MODE and AXIS1_MODE_GOTO must be set to a valid value."
    #endif
    #if !defined(AXIS2_MODE) || !defined(AXIS2_MODE_GOTO)
      #error "Configuration: AXIS2_MODE and AXIS2_MODE_GOTO must be set to a valid value."
    #endif
  #endif

#endif

// if AXISn_STEP_GOTO isn't defined, do so
#ifndef AXIS1_STEP_GOTO
  #define AXIS1_STEP_GOTO 1
#endif
#ifndef AXIS2_STEP_GOTO
  #define AXIS2_STEP_GOTO 1
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// misc. validation

// validate: warn the user not to have MISO wired up and try to use ESP8266 control too
#if defined(ESP8266_CONTROL_ON) && defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  #warning "Configuration: be sure the Aux1 and Aux2 pins are wired into the ESP8266 GPIO0 and RST pins and **NOT** the TMC SPI stepper driver SDO pins"
  #if (AXIS1_FAULT==TMC_SPI) || (AXIS2_FAULT==TMC_SPI)
    #error "Configuration: Fault detection across SPI conflicts with ESP8266 control, choose one feature and wire for it correctly"
  #endif
#endif

// validate: to make sure the PEC buffer size isn't too large
#if PECBufferSize > 3384
  #error "PECBufferSize cannot be greater than 3384. Please use the spreadsheet to calculate your correct value"
#endif

// validate: block this old pin-map configuration option since it's been replaced with other functionality
#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif

// validate: focuser configuration
#if !defined(FOCUSER1_ON) && defined(FOCUSER2_ON)
  #error "Focuser2 can't be enabled without first enabling Focuser1"
#endif

// validate: reticule and status LED configuration
#if defined(MaxPCB_ON) || defined(MaxESP2_ON) || defined(MiniPCB_ON)
  #if defined(RETICULE_LED_PINS) && defined(STATUS_LED_PINS2_ON)
    #error "You can't have the Illuminated Reticule and Status2 LEDs both enabled in this configuration."
  #endif
#endif

// validate: limit sense state
#ifdef LIMIT_SENSE_STATE
  #if LIMIT_SENSE_STATE != LOW && LIMIT_SENSE_STATE != HIGH
    #error "LIMIT_SENSE_STATE must be either HIGH or LOW"
  #endif
#else
  #define LIMIT_SENSE_STATE LOW // Default to low
#endif
