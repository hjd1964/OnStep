// -----------------------------------------------------------------------------------
// Validate current configuration

#if !defined(Classic_ON) && !defined(MiniPCB_ON) && !defined(MaxPCB_ON) && !defined(TM4C_ON) && !defined(Ramps14_ON) && !defined(Mega2560Alt_ON) && !defined(STM32_ON)
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

#if defined(ALTERNATE_PINMAP_ON)
  #error "ALTERNATE_PINMAP_ON is an obsolete option, you can't use this configuration."
#endif

#if defined(MODE_SWITCH_BEFORE_SLEW_ON) || defined(MODE_SWITCH_BEFORE_SLEW_SPI)
#if !defined(AXIS1_MODE) || !defined(AXIS1_MODE_GOTO)
  #error "Config.xxx.h error: AXIS1_MODE and AXIS1_MODE_GOTO must be set to a valid value."
#endif
#if !defined(AXIS2_MODE) || !defined(AXIS2_MODE_GOTO)
  #error "Config.xxx.h error: AXIS2_MODE and AXIS2_MODE_GOTO must be set to a valid value."
#endif
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


