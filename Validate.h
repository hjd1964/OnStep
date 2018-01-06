// -----------------------------------------------------------------------------------
// Validate current configuration

#if !defined(Classic_ON) && !defined(MiniPCB_ON) && !defined(MaxPCB_ON) && !defined(TM4C_ON) && !defined(Ramps14_ON) && !defined(Mega2560Alt_ON) && !defined(STM32_ON)
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif

#if !defined(FOCUSER1_ON) && defined(FOCUSER2_ON)
  #error "Focuser2 can't be enabled without first enabling Focuser1"
#endif

#if defined(MaxPCB) || defined(MiniPCB)
#if defined(RETICULE_LED_PINS) && (defined(STATUS_LED_PINS_ON) || defined(STATUS_LED_PINS))
  #error "You can't have the Illuminated Reticule and Status2 LEDs both enabled in this configuration."
#endif
#endif

// -----------------------------------------------------------------------------------
// Setup defaults

// set serial port baud rate if not done so already
#ifndef SERIAL1_BAUD_DEFAULT
  #define SERIAL1_BAUD_DEFAULT 9600
#endif
#ifndef SERIAL4_BAUD_DEFAULT
  #define SERIAL4_BAUD_DEFAULT 9600
#endif

// figure out how many align star are allowed for the configuration
#ifndef ALIGN_GOTOASSIST_ON
  #if defined(MOUNT_TYPE_GEM)
  #define MAX_NUM_ALIGN_STARS '3'
  #elif defined(MOUNT_TYPE_FORK)
  #define MAX_NUM_ALIGN_STARS '3'
  #elif defined(MOUNT_TYPE_FORK_ALT)
  #define MAX_NUM_ALIGN_STARS '1'
  #elif defined(MOUNT_TYPE_ALTAZM)
  #define MAX_NUM_ALIGN_STARS '3'
  #endif
#else
  #if defined(MOUNT_TYPE_GEM)
  #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK)
  #define MAX_NUM_ALIGN_STARS '6'
  #elif defined(MOUNT_TYPE_FORK_ALT)
  #define MAX_NUM_ALIGN_STARS '1'
  #elif defined(MOUNT_TYPE_ALTAZM)
  #define MAX_NUM_ALIGN_STARS '3'
  #endif
#endif

// misc. configuration #defines to correct for backwards compatability etc.
#ifdef SEPERATE_PULSE_GUIDE_RATE_ON
#define SEPARATE_PULSE_GUIDE_RATE_ON
#endif
#ifndef GUIDE_TIME_LIMIT
#define GUIDE_TIME_LIMIT 0
#endif
#ifndef MaxRot
#define MaxRot MaxAxis3
#endif
#ifndef MinRot
#define MinRot MinAxis3
#endif
#ifndef AXIS1_STEP_GOTO
#define AXIS1_STEP_GOTO 1
#endif
#ifndef AXIS2_STEP_GOTO
#define AXIS2_STEP_GOTO 1
#endif

// simplified stepper driver mode setup
#if (defined(AXIS1_DRIVER_MODEL) && (!defined(AXIS2_DRIVER_MODEL)) || (defined(AXIS2_DRIVER_MODEL) && (!defined(AXIS1_DRIVER_MODEL))
  #error "AXISn_DRIVER_MODEL; If using the simplified driver mode setup you must use it for both AXIS1 and AXIS2."
#endif

#if defined(AXIS1_DRIVER_MODEL)
  // attempting to use the simplified stepper driver setup

  // tracking mode
  #ifndef AXIS1_MICROSTEPS
    #error "AXIS1_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS1_DRIVER_MODEL!"
  #endif

  #if AXIS1_DRIVER_MODEL == A4988
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16
      #error "AXIS1_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == DRV8825
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32
      #error "AXIS1_MICROSTEPS; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == LV8729 || AXIS1_DRIVER_MODEL == RAPS128
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64 && AXIS1_MICROSTEPS!=128
      #error "AXIS1_MICROSTEPS; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2100
    #if AXIS1_MICROSTEPS!=_OFF
      #error "AXIS1_MICROSTEPS; TMC2100 invalid micro-step mode, use: _OFF"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2130
    #if AXIS1_MICROSTEPS!=1 && AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16 && AXIS1_MICROSTEPS!=32 && AXIS1_MICROSTEPS!=64 && AXIS1_MICROSTEPS!=128 && AXIS1_MICROSTEPS!=256
      #error "AXIS1_MICROSTEPS; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2208
    #if AXIS1_MICROSTEPS!=2 && AXIS1_MICROSTEPS!=4 && AXIS1_MICROSTEPS!=8 && AXIS1_MICROSTEPS!=16
      #error "AXIS1_MICROSTEPS; TMC2208 invalid micro-step mode, use: 16,8,4,or 2"
    #endif
  #else
    #error "No stepper driver model configured for Axis1!"
  #endif

  #ifndef AXIS2_MICROSTEPS
    #error "AXIS2_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS2_DRIVER_MODEL!"
  #endif
  
  #if AXIS2_DRIVER_MODEL == A4988
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16
      #error "AXIS2_MICROSTEPS; A4988 invalid micro-step mode, use: 16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == DRV8825
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32
      #error "AXIS2_MICROSTEPS; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == LV8729 || AXIS2_DRIVER_MODEL == RAPS128
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64 && AXIS2_MICROSTEPS!=128
      #error "AXIS2_MICROSTEPS; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2100
    #if AXIS2_MICROSTEPS!=_OFF
      #error "AXIS2_MICROSTEPS; TMC2100 invalid micro-step mode, use: _OFF"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2130
    #if AXIS2_MICROSTEPS!=1 && AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16 && AXIS2_MICROSTEPS!=32 && AXIS2_MICROSTEPS!=64 && AXIS2_MICROSTEPS!=128 && AXIS2_MICROSTEPS!=256
      #error "AXIS2_MICROSTEPS; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,or 1"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2208
    #if AXIS2_MICROSTEPS!=2 && AXIS2_MICROSTEPS!=4 && AXIS2_MICROSTEPS!=8 && AXIS2_MICROSTEPS!=16
      #error "AXIS2_MICROSTEPS; TMC2208 invalid micro-step mode, use: 16,8,4,or 2"
    #endif
  #else
    #error "No stepper driver model configured for axis 1 !"
  #endif

  // goto mode
  #ifdef AXIS1_MICROSTEPS_GOTO

    #if AXIS1_DRIVER_MODEL == A4988
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16
        #error "AXIS1_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == DRV8825
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32
        #error "AXIS1_MICROSTEPS_GOTO; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == LV8729 || AXIS1_DRIVER_MODEL == RAPS128
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64 && AXIS1_MICROSTEPS_GOTO!=128
        #error "AXIS1_MICROSTEPS_GOTO; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2100
      #if AXIS1_MICROSTEPS_GOTO!=_OFF
        #error "AXIS1_MICROSTEPS_GOTO; TMC2100 invalid micro-step mode, use: _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2130
      #if AXIS1_MICROSTEPS_GOTO!=1 && AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16 && AXIS1_MICROSTEPS_GOTO!=32 && AXIS1_MICROSTEPS_GOTO!=64 && AXIS1_MICROSTEPS_GOTO!=128 && AXIS1_MICROSTEPS_GOTO!=256
        #error "AXIS1_MICROSTEPS_GOTO; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS1_DRIVER_MODEL == TMC2208
      #if AXIS1_MICROSTEPS_GOTO!=2 && AXIS1_MICROSTEPS_GOTO!=4 && AXIS1_MICROSTEPS_GOTO!=8 && AXIS1_MICROSTEPS_GOTO!=16
        #error "AXIS1_MICROSTEPS_GOTO; TMC2208 invalid micro-step mode, use: 16,8,4,2,or _OFF"
      #endif
    #endif

    // if we get here we have valid AXIS1_MICROSTEPS and AXIS1_MICROSTEPS_GOTO
    #define AXIS1_STEP_GOTO (AXIS1_MICROSTEPS/AXIS1_MICROSTEPS_GOTO)
  
  #endif

  #ifdef AXIS2_MICROSTEPS_GOTO
  
    #if AXIS2_DRIVER_MODEL == A4988
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16
        #error "AXIS2_MICROSTEPS_GOTO; A4988 invalid micro-step mode, use: 16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == DRV8825
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32
        #error "AXIS2_MICROSTEPS_GOTO; DRV8825 invalid micro-step mode, use: 32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == LV8729 || AXIS2_DRIVER_MODEL == RAPS128
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64 && AXIS2_MICROSTEPS_GOTO!=128
        #error "AXIS2_MICROSTEPS_GOTO; LV8729/RAPS128 invalid micro-step mode, use: 128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2100
      #if AXIS2_MICROSTEPS_GOTO!=_OFF
        #error "AXIS2_MICROSTEPS_GOTO; TMC2100 invalid micro-step mode, use: _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2130
      #if AXIS2_MICROSTEPS_GOTO!=1 && AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16 && AXIS2_MICROSTEPS_GOTO!=32 && AXIS2_MICROSTEPS_GOTO!=64 && AXIS2_MICROSTEPS_GOTO!=128 && AXIS2_MICROSTEPS_GOTO!=256
        #error "AXIS2_MICROSTEPS_GOTO; TMC2130 invalid micro-step mode, use: 256,128,64,32,16,8,4,2,1,or _OFF"
      #endif
    #elif AXIS2_DRIVER_MODEL == TMC2208
      #if AXIS2_MICROSTEPS_GOTO!=2 && AXIS2_MICROSTEPS_GOTO!=4 && AXIS2_MICROSTEPS_GOTO!=8 && AXIS2_MICROSTEPS_GOTO!=16
        #error "AXIS2_MICROSTEPS_GOTO; TMC2208 invalid micro-step mode, use: 16,8,4,2,or _OFF"
      #endif
    #endif

    // if we get here we have valid AXIS2_MICROSTEPS and AXIS2_MICROSTEPS_GOTO
    #define AXIS2_STEP_GOTO (AXIS2_MICROSTEPS/AXIS2_MICROSTEPS_GOTO)

  #endif

#else
  // attempting to use the advanced stepper driver setup

  #if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
    #if !defined(AXIS1_MODE) || !defined(AXIS1_MODE_GOTO)
      #error "Config.xxx.h error: AXIS1_MODE and AXIS1_MODE_GOTO must be set to a valid value."
    #endif
    #if !defined(AXIS2_MODE) || !defined(AXIS2_MODE_GOTO)
      #error "Config.xxx.h error: AXIS2_MODE and AXIS2_MODE_GOTO must be set to a valid value."
    #endif
  #endif

#endif


