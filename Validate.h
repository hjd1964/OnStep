// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

#ifdef Classic_ON
  #define Configuration_Found
#endif
#ifdef MiniPCB_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxPCB_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxESP2_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef Ramps14_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef Mega2560Alt_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32CZ_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32Black_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32Blue_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef Configuration_Duplicate
  #error "You have more than one Config.xxx.h file enabled, ONLY ONE can be enabled with _ON."
#endif

#ifndef Configuration_Found
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

// configuration file version

#ifdef FileVersionConfig
  #if FileVersionConfig < FirmwareVersionConfig
    // firmware version 2 is compatible with file version 1
    #if (FileVersionConfig==1) && (FirmwareVersionConfig==2)
      #warning "Configuration: There have been changes to the configuration file format, but OnStep is still backwards compatible for now."
    #else
      #error "Configuration: There have been changes to the configuration file format.  You'll have to make a new Config.xxx.h file."
    #endif
  #elif FileVersionConfig > FirmwareVersionConfig
    #error "Configuration: Configuration file version mismatch."
  #endif
#else
  #if (FirmwareVersionConfig == 1) || (FirmwareVersionConfig == 2)
    #warning "Configuration: Config.xxx.h file version isn't specified (pre-version 1?)"
  #else
    #error "Configuration: There have been changes to the configuration file format.  You'll have to make a new Config.xxx.h file."
  #endif
#endif

// misc.

#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif

#if !defined(FOCUSER1_ON) && defined(FOCUSER2_ON)
  #error "Focuser2 can't be enabled without first enabling Focuser1"
#endif

#if defined(MaxPCB_ON) || defined(MaxESP2_ON) || defined(MiniPCB_ON)
  #if defined(RETICULE_LED_PINS) && defined(STATUS_LED_PINS2_ON)
    #error "You can't have the Illuminated Reticule and Status2 LEDs both enabled in this configuration."
  #endif
#endif

// -----------------------------------------------------------------------------------
// misc. configuration #defines to correct for backwards compatability etc.

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
#ifdef AXIS1_FAULT_SPI
#define AXIS1_FAULT TMC2130
#endif
#ifdef AXIS1_FAULT_LOW
#define AXIS1_FAULT LOW
#endif
#ifdef AXIS1_FAULT_HIGH
#define AXIS1_FAULT HIGH
#endif
#ifdef AXIS2_FAULT_SPI
#define AXIS2_FAULT TMC2130
#endif
#ifdef AXIS2_FAULT_LOW
#define AXIS2_FAULT LOW
#endif
#ifdef AXIS2_FAULT_HIGH
#define AXIS2_FAULT HIGH
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
// setup defaults

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
#if defined(HAL_FAST_PROCESSOR)
  #if defined(MOUNT_TYPE_GEM)
    #define MAX_NUM_ALIGN_STARS '9'
  #elif defined(MOUNT_TYPE_FORK)
    #define MAX_NUM_ALIGN_STARS '9'
  #elif defined(MOUNT_TYPE_FORK_ALT)
    #define MAX_NUM_ALIGN_STARS '9'
  #elif defined(MOUNT_TYPE_ALTAZM)
    #define MAX_NUM_ALIGN_STARS '9'
  #endif
#else
  #if defined(MOUNT_TYPE_GEM)
    #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK)
    #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK_ALT)
    #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_ALTAZM)
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
// stepper driver mode setup

#if (defined(AXIS1_DRIVER_MODEL) && !defined(AXIS2_DRIVER_MODEL)) || (defined(AXIS2_DRIVER_MODEL) && !defined(AXIS1_DRIVER_MODEL))
  #error "AXISn_DRIVER_MODEL; If using the simplified driver mode setup you must use it for both AXIS1 and AXIS2."
#endif

#if defined(AXIS1_DRIVER_MODEL)
  // attempting to use the simplified stepper driver setup

  // special SPI decay modes etc. for TMC2130 AXIS1
  #if AXIS1_DRIVER_MODEL == TMC2130_LOWPWR
    #undef AXIS1_DRIVER_MODEL
    #define AXIS1_DRIVER_MODEL TMC2130
    #define TMC_AXIS1_MODE TMC_LOWPWR
    #define TMC_AXIS1_MODE_GOTO 0
  #elif AXIS1_DRIVER_MODEL == TMC2130_QUIET
    #undef AXIS1_DRIVER_MODEL
    #define AXIS1_DRIVER_MODEL TMC2130
    #define TMC_AXIS1_MODE TMC_STEALTHCHOP
    #define TMC_AXIS1_MODE_GOTO 0
  #elif AXIS1_DRIVER_MODEL == TMC2130_QUIET_LOWPWR
    #undef AXIS1_DRIVER_MODEL
    #define AXIS1_DRIVER_MODEL TMC2130
    #define TMC_AXIS1_MODE TMC_STEALTHCHOP|TMC_LOWPWR
    #define TMC_AXIS1_MODE_GOTO 0
  #elif AXIS1_DRIVER_MODEL == TMC2130_VQUIET
    #undef AXIS1_DRIVER_MODEL
    #define AXIS1_DRIVER_MODEL TMC2130
    #define TMC_AXIS1_MODE TMC_STEALTHCHOP
    #define TMC_AXIS1_MODE_GOTO TMC_STEALTHCHOP
  #elif AXIS1_DRIVER_MODEL == TMC2130_VQUIET_LOWPWR
    #undef AXIS1_DRIVER_MODEL
    #define AXIS1_DRIVER_MODEL TMC2130
    #define TMC_AXIS1_MODE TMC_STEALTHCHOP|TMC_LOWPWR
    #define TMC_AXIS1_MODE_GOTO TMC_STEALTHCHOP
  #else
    #define TMC_AXIS1_MODE 0
    #define TMC_AXIS1_MODE_GOTO 0
  #endif

  // special SPI decay modes etc. for TMC2130 AXIS2
  #if AXIS2_DRIVER_MODEL == TMC2130_LOWPWR
    #undef AXIS2_DRIVER_MODEL
    #define AXIS2_DRIVER_MODEL TMC2130
    #define TMC_AXIS2_MODE TMC_LOWPWR
    #define TMC_AXIS2_MODE_GOTO 0
  #elif AXIS2_DRIVER_MODEL == TMC2130_QUIET
    #undef AXIS2_DRIVER_MODEL
    #define AXIS2_DRIVER_MODEL TMC2130
    #define TMC_AXIS2_MODE TMC_STEALTHCHOP
    #define TMC_AXIS2_MODE_GOTO 0
  #elif AXIS2_DRIVER_MODEL == TMC2130_QUIET_LOWPWR
    #undef AXIS2_DRIVER_MODEL
    #define AXIS2_DRIVER_MODEL TMC2130
    #define TMC_AXIS2_MODE TMC_STEALTHCHOP|TMC_LOWPWR
    #define TMC_AXIS2_MODE_GOTO 0
  #elif AXIS2_DRIVER_MODEL == TMC2130_VQUIET
    #undef AXIS2_DRIVER_MODEL
    #define AXIS2_DRIVER_MODEL TMC2130
    #define TMC_AXIS2_MODE TMC_STEALTHCHOP
    #define TMC_AXIS2_MODE_GOTO TMC_STEALTHCHOP
  #elif AXIS2_DRIVER_MODEL == TMC2130_VQUIET_LOWPWR
    #undef AXIS2_DRIVER_MODEL
    #define AXIS2_DRIVER_MODEL TMC2130
    #define TMC_AXIS2_MODE TMC_STEALTHCHOP|TMC_LOWPWR
    #define TMC_AXIS2_MODE_GOTO TMC_STEALTHCHOP
  #else
    #define TMC_AXIS2_MODE 0
    #define TMC_AXIS2_MODE_GOTO 0
  #endif

  // tracking micro-step mode AXIS1
  #ifndef AXIS1_MICROSTEPS
    #error "Configuration: AXIS1_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS1_DRIVER_MODEL!"
  #endif

  #if AXIS1_DRIVER_MODEL == A4988
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16
      #error "Configuration: AXIS1_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == DRV8825
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32
      #error "Configuration: AXIS1_MICROSTEPS; DRV8825/S109 invalid micro-step mode, use: 32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == LV8729 || AXIS1_DRIVER_MODEL == RAPS128
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64 && AXIS1_MICROSTEPS!=128
      #error "Configuration: AXIS1_MICROSTEPS; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == ST820
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=128 && AXIS1_MICROSTEPS!=256
      #error "Configuration: AXIS1_MICROSTEPS; ST820 invalid micro-step mode, use: 256,128,32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2100
    #define MODE_SWITCH_BEFORE_SLEW_ON
    #define MODE_SWITCH_SLEEP_ON
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=16
      #error "Configuration: AXIS1_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
    #endif
    #warning "Configuration: AXIS1_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: _OFF here"
  #elif AXIS1_DRIVER_MODEL == TMC2130
    #define MODE_SWITCH_BEFORE_SLEW_SPI
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64 && AXIS1_MICROSTEPS!=128 && AXIS1_MICROSTEPS!=256
      #error "Configuration: AXIS1_MICROSTEPS; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
    #if AXIS2_DRIVER_MODEL != TMC2130
      #error "Configuration: TMC2130 stepper drivers must be used in pairs (both Axis1 and Axis2.)"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2208
    #if AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16
      #error "Configuration: AXIS1_MICROSTEPS; TMC2208 invalid micro-step mode, use: 16,8,4,or 2"
    #endif
  #else
    #error "Configuration: Unrecognized stepper driver model for Axis1 !"
  #endif

  // tracking micro-step mode AXIS2
  #ifndef AXIS2_MICROSTEPS
    #error "AXIS2_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS2_DRIVER_MODEL!"
  #endif
  
  #if AXIS2_DRIVER_MODEL == A4988
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16
      #error "Configuration: AXIS2_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == DRV8825
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32
      #error "Configuration: AXIS2_MICROSTEPS; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == LV8729 || AXIS2_DRIVER_MODEL == RAPS128
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64 && AXIS2_MICROSTEPS!=128
      #error "Configuration: AXIS2_MICROSTEPS; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == ST820
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=128 && AXIS2_MICROSTEPS!=256
      #error "Configuration: AXIS2_MICROSTEPS; ST820 invalid micro-step mode, use: 256,128,32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2100
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=16
      #error "Configuration: AXIS2_MICROSTEPS; TMC2100 invalid micro-step mode, use: 16,4,2,or 1"
    #endif
    #warning "Configuration: AXIS2_MICROSTEPS; This mode is supported in spreadCycle but without 256x interpolation: it's usually best to hard-wire the TMC2100 and use: _OFF here"
  #elif AXIS2_DRIVER_MODEL == TMC2130
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64 && AXIS2_MICROSTEPS!=128 && AXIS2_MICROSTEPS!=256
      #error "Configuration: AXIS2_MICROSTEPS; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2208
    #if AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16
      #error "Configuration: AXIS2_MICROSTEPS; TMC2208 invalid micro-step mode, use: 16,8,4,or 2"
    #endif
  #else
    #error "Configuration error: Unrecognized stepper driver model for Axis2 !"
  #endif

  // goto micro-step mode AXIS1
  #ifdef AXIS1_MICROSTEPS_GOTO

    #if AXIS1_DRIVER_MODEL == A4988
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == DRV8825
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; DRV8825/S109 invalid micro-step mode, use: 32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == LV8729 || AXIS1_DRIVER_MODEL == RAPS128
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64 && AXIS1_MICROSTEPS_GOTO!=128
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2100
      #if AXIS1_MICROSTEPS != AXIS1_MICROSTEPS_GOTO
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; must be set to the same value as AXIS1_MICROSTEPS,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2130
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64 && AXIS1_MICROSTEPS_GOTO!=128 && AXIS1_MICROSTEPS_GOTO!=256
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
      #endif
      #if AXIS1_MICROSTEPS != AXIS1_MICROSTEPS_GOTO
        #warning "Configuration: AXIS2_MICROSTEPS_GOTO; is NOT _OFF.  This can effect pointing accuracy slightly (and PEC if index sensing isn't used.)"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2208
      #if AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16
        #error "Configuration: AXIS1_MICROSTEPS_GOTO; TMC2208 invalid micro-step mode, use: 16,8,4,2,or _OFF"
      #endif
    #endif

    #if (AXIS1_MICROSTEPS <= AXIS1_MICROSTEPS_GOTO) && (!((defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)) && (AXIS1_MICROSTEPS == AXIS1_MICROSTEPS_GOTO)))
        #error "Configuration: AXIS1_MICROSTEPS_GOTO should be less than AXIS1_MICROSTEPS or _OFF"
    #endif

    // if we get here we have valid AXIS1_MICROSTEPS and AXIS1_MICROSTEPS_GOTO
    #define AXIS1_STEP_GOTO (AXIS1_MICROSTEPS/AXIS1_MICROSTEPS_GOTO)
  
  #endif

  // goto micro-step mode AXIS2
  #ifdef AXIS2_MICROSTEPS_GOTO
  
    #if AXIS2_DRIVER_MODEL == A4988
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == DRV8825
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == LV8729 || AXIS2_DRIVER_MODEL == RAPS128
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64 && AXIS2_MICROSTEPS_GOTO!=128
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2100
      #if AXIS2_MICROSTEPS != AXIS2_MICROSTEPS_GOTO
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; must be set to the same value as AXIS2_MICROSTEPS,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2130
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64 && AXIS2_MICROSTEPS_GOTO!=128 && AXIS2_MICROSTEPS_GOTO!=256
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
      #endif
      #if AXIS2_MICROSTEPS != AXIS2_MICROSTEPS_GOTO
        #warning "Configuration: AXIS2_MICROSTEPS_GOTO; is NOT _OFF.  This can effect pointing accuracy slightly (and PEC if index sensing isn't used.)"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2208
      #if AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16
        #error "Configuration: AXIS2_MICROSTEPS_GOTO; TMC2208 invalid micro-step mode, use: 16,8,4,2,or _OFF"
      #endif
    #endif

    #if (AXIS2_MICROSTEPS <= AXIS2_MICROSTEPS_GOTO) && (!((defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)) && (AXIS2_MICROSTEPS == AXIS2_MICROSTEPS_GOTO)))
        #error "Configuration: AXIS2_MICROSTEPS_GOTO should be less than AXIS2_MICROSTEPS or _OFF"
    #endif

    // if we get here we have valid AXIS2_MICROSTEPS and AXIS2_MICROSTEPS_GOTO
    #define AXIS2_STEP_GOTO (AXIS2_MICROSTEPS/AXIS2_MICROSTEPS_GOTO)

  #endif

  #if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
    // help for TMC2100 and TMC2130 where AXISn_MICROSTEPS_GOTO must be defined
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
// warn the user not to have MISO wired up and try to use ESP8266 control too
#if defined(ESP8266_CONTROL_ON) && defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  #warning "Configuration: be sure the Aux1 and Aux2 pins are wired into the ESP8266 GPIO0 and RST pins and **NOT** the SSS TMC2130 SDO pins"
  #if (AXIS1_FAULT==TMC2130) || (AXIS2_FAULT==TMC2130)
    #error "Configuration: Fault detection across SPI conflicts with ESP8266 control, choose one feature and wire for it correctly"
  #endif
#endif

// -----------------------------------------------------------------------------------
// check to see if we're trying to operate at too fast a rate
#if AXIS2_STEP_GOTO==1 && AXIS1_STEP_GOTO==1
  #define __MaxRate_LowerLimit MaxRate_LowerLimit
#else
  // micro-step mode switching run-time code is compiled in so ISR's are a bit slower
  #define __MaxRate_LowerLimit MaxRate_LowerLimit*2
#endif

#if (MaxRate*AXIS1_STEP_GOTO + MaxRate*AXIS2_STEP_GOTO)/2 < __MaxRate_LowerLimit
  #error "Configuration: the MaxRate setting exceeds the platform performance, increase MaxRate or use/adjust micro-step mode switching"
#endif
#if (MaxRate*AXIS1_STEP_GOTO + MaxRate*AXIS2_STEP_GOTO)/2 < __MaxRate_LowerLimit*2
  #warning "Configuration: the MaxRate run-time adjustability (0.5x to 2x MaxRate) can be set to exceed the platform performance, you might want to increase MaxRate or use/adjust micro-step mode switching"
#endif

#if PECBufferSize > 3384
  #error "PECBufferSize cannot be greater than 3384. Please use the spreadsheet to calculate your correct value"
#endif
