// -----------------------------------------------------------------------------------
// Validate current configuration

#if !defined(Classic_ON) && !defined(MiniPCB_ON) && !defined(MaxPCB_ON) && !defined(TM4C_ON) && !defined(Ramps14_ON) && !defined(Mega2560Alt_ON) && !defined(STM32_ON)
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif
/*
#if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
#if !defined(AXIS1_MICROSTEPS)
  #error "Config.xxx.h error: AXIS1_MICROSTEPS must be set to a valid value."
#endif
#if !defined(AXIS2_MICROSTEPS)
  #error "Config.xxx.h error: AXIS2_MICROSTEPS must be set to a valid value."
#endif
#endif
*/
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

#if defined(AXIS1_MODE) || defined(AXIS2_MODE) || defined(AXIS1_MODE_GOTO) || defined(AXIS2_MODE_GOTO)
  // We will not use AXISn_MICROSTEPS, this preserves backward compatibility
#elif defined(Ramps14_ON)
  // Does not use AXISn_MICROSTEPS 
#else
  #ifndef AXIS1_MICROSTEPS
    #error "AXIS1_MICROSTEPS must be defined in your Config.xxx.h file!"
  #endif

  #ifndef AXIS2_MICROSTEPS
    #error "AXIS2_MICROSTEPS must be defined in your Config.xxx.h file!"
  #endif

  #if AXIS1_DRIVER_MODEL == A4988
    #if AXIS1_MICROSTEPS > 16
      #error "The configured stepper driver model for axis 1 does not support stepping over 16"
    #endif
  #elif AXIS1_DRIVER_MODEL == DRV8825
    #if AXIS1_MICROSTEPS > 32
      #error "The configured stepper driver model for axis 1  does not support stepping over 32"
    #endif
  #elif AXIS1_DRIVER_MODEL == LV8729
    #if AXIS1_MICROSTEPS > 128
      #error "The configured stepper driver model for axis 1  does not support stepping over 128"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2130
    #if AXIS1_MICROSTEPS > 256
      #error "The configured stepper driver model for axis 1  does not support stepping over 256"
    #endif
  #elif AXIS1_DRIVER_MODEL == TMC2208
    #if AXIS1_MICROSTEPS > 16
      #error "The configured stepper driver model for axis 1  does not support stepping over 16"
    #endif
  #else
    #error "No stepper driver model configured for axis 1 !"
  #endif

  #if AXIS2_DRIVER_MODEL == A4988
    #if AXIS2_MICROSTEPS > 16
      #error "The configured stepper driver model for axis 1 does not support stepping over 16"
    #endif
  #elif AXIS2_DRIVER_MODEL == DRV8825
    #if AXIS2_MICROSTEPS > 32
      #error "The configured stepper driver model for axis 1  does not support stepping over 32"
    #endif
  #elif AXIS2_DRIVER_MODEL == LV8729
    #if AXIS2_MICROSTEPS > 128
      #error "The configured stepper driver model for axis 1  does not support stepping over 128"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2130
    #if AXIS2_MICROSTEPS > 256
      #error "The configured stepper driver model for axis 1  does not support stepping over 256"
    #endif
  #elif AXIS2_DRIVER_MODEL == TMC2208
    #if AXIS2_MICROSTEPS > 16
      #error "The configured stepper driver model for axis 1  does not support stepping over 16"
    #endif
  #else
    #error "No stepper driver model configured for axis 1 !"
  #endif

#endif
