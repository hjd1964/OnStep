// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

#if FileVersionConfig != 3 && FileVersionConfig != 4
  #error "Configuration (Config.h): FileVersionConfig (Config.h version) must be 3 or 4 for this OnStep."
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// correct for configuration backwards compatability

#ifndef FEATURE1_DEFAULT_VALUE
  #define FEATURE1_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE2_DEFAULT_VALUE
  #define FEATURE2_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE3_DEFAULT_VALUE
  #define FEATURE3_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE4_DEFAULT_VALUE
  #define FEATURE4_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE5_DEFAULT_VALUE
  #define FEATURE5_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE6_DEFAULT_VALUE
  #define FEATURE6_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE7_DEFAULT_VALUE
  #define FEATURE7_DEFAULT_VALUE OFF
#endif
#ifndef FEATURE8_DEFAULT_VALUE
  #define FEATURE8_DEFAULT_VALUE OFF
#endif

#ifndef PIER_SIDE_PREFERRED_DEFAULT
  #define PIER_SIDE_PREFERRED_DEFAULT BEST
#endif

#ifndef PIER_SIDE_SYNC_CHANGE_SIDES
  #if SYNC_CURRENT_PIER_SIDE_ONLY == OFF
    #define PIER_SIDE_SYNC_CHANGE_SIDES ON
  #elif SYNC_CURRENT_PIER_SIDE_ONLY == ON
    #define PIER_SIDE_SYNC_CHANGE_SIDES OFF
  #endif
#endif

#if AXIS2_LIMIT_MIN == -91
  #undef AXIS2_LIMIT_MIN
  #define AXIS2_LIMIT_MIN -90
#endif

#if AXIS2_LIMIT_MAX == 91
  #undef AXIS2_LIMIT_MAX
  #define AXIS2_LIMIT_MAX 90
#endif

#if MOUNT_TYPE == ALTAZM
  #if !defined(AXIS1_LIMIT_MIN) && defined(AXIS1_LIMIT_MAXAZM)
    #define AXIS1_LIMIT_MIN -AXIS1_LIMIT_MAXAZM
    #define AXIS1_LIMIT_MAX AXIS1_LIMIT_MAXAZM
  #endif
#else
  #if !defined(AXIS1_LIMIT_MIN) && defined(AXIS1_LIMIT_UNDER_POLE)
    #define AXIS1_LIMIT_MIN -AXIS1_LIMIT_UNDER_POLE
    #define AXIS1_LIMIT_MAX AXIS1_LIMIT_UNDER_POLE
  #endif
#endif

#ifndef AXIS2_TANGENT_ARM
  #define AXIS2_TANGENT_ARM OFF
#endif

#ifndef FEATURE1_PURPOSE
  #define FEATURE1_PURPOSE OFF
#endif
#ifndef FEATURE1_TEMP
  #define FEATURE1_TEMP OFF
#endif
#ifndef FEATURE2_PURPOSE
  #define FEATURE2_PURPOSE OFF
#endif
#ifndef FEATURE2_TEMP
  #define FEATURE2_TEMP OFF
#endif
#ifndef FEATURE3_PURPOSE
  #define FEATURE3_PURPOSE OFF
#endif
#ifndef FEATURE3_TEMP
  #define FEATURE3_TEMP OFF
#endif
#ifndef FEATURE4_PURPOSE
  #define FEATURE4_PURPOSE OFF
#endif
#ifndef FEATURE4_TEMP
  #define FEATURE4_TEMP OFF
#endif
#ifndef FEATURE5_PURPOSE
  #define FEATURE5_PURPOSE OFF
#endif
#ifndef FEATURE5_TEMP
  #define FEATURE5_TEMP OFF
#endif
#ifndef FEATURE6_PURPOSE
  #define FEATURE6_PURPOSE OFF
#endif
#ifndef FEATURE6_TEMP
  #define FEATURE6_TEMP OFF
#endif
#ifndef FEATURE7_PURPOSE
  #define FEATURE7_PURPOSE OFF
#endif
#ifndef FEATURE7_TEMP
  #define FEATURE7_TEMP OFF
#endif
#ifndef FEATURE8_PURPOSE
  #define FEATURE8_PURPOSE OFF
#endif
#ifndef FEATURE8_TEMP
  #define FEATURE8_TEMP OFF
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// setup defaults

// enable PEC code only if we need it
#if !(AXIS1_STEPS_PER_WORMROT == 0 || MOUNT_TYPE == ALTAZM)
  #define AXIS1_PEC ON
#else
  #define AXIS1_PEC OFF
#endif

// default allowed degrees past the meridian on the East and West sides of the pier
#define AXIS1_LIMIT_MERIDIAN_E 7.5 
#define AXIS1_LIMIT_MERIDIAN_W 7.5

// always enable the separate pulse guide rate
#define SEPARATE_PULSE_GUIDE_RATE ON

// default time for spiral guides is 103.4 seconds
#ifndef GUIDE_SPIRAL_TIME_LIMIT
  #define GUIDE_SPIRAL_TIME_LIMIT 103.4
#endif

// automatically set focuser/rotator step rate (or focuser DC pwm freq.) from AXISn_SLEW_RATE_DESIRED
#ifndef AXIS3_STEP_RATE_MAX
  #define AXIS3_STEP_RATE_MAX (1000.0/(AXIS3_SLEW_RATE_DESIRED*AXIS3_STEPS_PER_DEGREE))
#endif
#ifndef AXIS4_STEP_RATE_MAX
  #if AXIS5_DRIVER_DC_MODE == ON
    #define AXIS4_STEP_RATE_MAX (1000.0/(AXIS4_SLEW_RATE_DESIRED*10.0))
  #else
    #define AXIS4_STEP_RATE_MAX (1000.0/(AXIS4_SLEW_RATE_DESIRED*AXIS4_STEPS_PER_MICRON))
  #endif
#endif
#ifndef AXIS5_STEP_RATE_MAX
  #if AXIS5_DRIVER_DC_MODE == ON
    #define AXIS5_STEP_RATE_MAX (1000.0/(AXIS5_SLEW_RATE_DESIRED*10.0))
  #else
    #define AXIS5_STEP_RATE_MAX (1000.0/(AXIS5_SLEW_RATE_DESIRED*AXIS5_STEPS_PER_MICRON))
  #endif
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

// make sure all stepper driver options are present even if not defined
#ifndef MODE_SWITCH_BEFORE_SLEW
  #define MODE_SWITCH_BEFORE_SLEW OFF
#endif
#ifndef MODE_SWITCH_SLEEP
  #define MODE_SWITCH_SLEEP OFF
#endif

#ifndef AXIS1_DRIVER_ENABLE
  #define AXIS1_DRIVER_ENABLE LOW
#endif
#if AXIS1_DRIVER_ENABLE == LOW
  #define AXIS1_DRIVER_DISABLE HIGH
#elif AXIS1_DRIVER_ENABLE == HIGH
  #define AXIS1_DRIVER_DISABLE LOW
#endif
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
#ifndef AXIS1_DRIVER_CHOP_TOFF
  #define AXIS1_DRIVER_CHOP_TOFF OFF
#endif
#ifndef AXIS1_DRIVER_CHOP_HSTART
  #define AXIS1_DRIVER_CHOP_HSTART OFF
#endif
#ifndef AXIS1_DRIVER_CHOP_HEND
  #define AXIS1_DRIVER_CHOP_HEND OFF
#endif
#ifndef AXIS1_DRIVER_CHOP_RNDTF
  #define AXIS1_DRIVER_CHOP_RNDTF OFF
#endif
#ifndef AXIS1_DRIVER_CHOP_TBL
  #define AXIS1_DRIVER_CHOP_TBL OFF
#endif
#ifndef AXIS1_DRIVER_FS_VHIGH
  #define AXIS1_DRIVER_FS_VHIGH OFF
#endif
#ifndef AXIS1_DRIVER_PWM_GRAD
  #define AXIS1_DRIVER_PWM_GRAD OFF
#endif
#ifndef AXIS1_DRIVER_PWM_VHIGH_SC
  #define AXIS1_DRIVER_PWM_VHIGH_SC OFF
#endif

#ifndef AXIS2_DRIVER_ENABLE
  #define AXIS2_DRIVER_ENABLE LOW
#endif
#if AXIS2_DRIVER_ENABLE == LOW
  #define AXIS2_DRIVER_DISABLE HIGH
#elif AXIS2_DRIVER_ENABLE == HIGH
  #define AXIS2_DRIVER_DISABLE LOW
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
#ifndef AXIS2_DRIVER_CHOP_TOFF
  #define AXIS2_DRIVER_CHOP_TOFF OFF
#endif
#ifndef AXIS2_DRIVER_CHOP_HSTART
  #define AXIS2_DRIVER_CHOP_HSTART OFF
#endif
#ifndef AXIS2_DRIVER_CHOP_HEND
  #define AXIS2_DRIVER_CHOP_HEND OFF
#endif
#ifndef AXIS2_DRIVER_CHOP_RNDTF
  #define AXIS2_DRIVER_CHOP_RNDTF OFF
#endif
#ifndef AXIS2_DRIVER_CHOP_TBL
  #define AXIS2_DRIVER_CHOP_TBL OFF
#endif
#ifndef AXIS2_DRIVER_FS_VHIGH
  #define AXIS2_DRIVER_FS_VHIGH OFF
#endif
#ifndef AXIS2_DRIVER_PWM_GRAD
  #define AXIS2_DRIVER_PWM_GRAD OFF
#endif
#ifndef AXIS2_DRIVER_PWM_VHIGH_SC
  #define AXIS2_DRIVER_PWM_VHIGH_SC OFF
#endif

#ifndef AXIS3_DRIVER_ENABLE
  #define AXIS3_DRIVER_ENABLE LOW
#endif
#if AXIS3_DRIVER_ENABLE == LOW
  #define AXIS3_DRIVER_DISABLE HIGH
#elif AXIS3_DRIVER_ENABLE == HIGH
  #define AXIS3_DRIVER_DISABLE LOW
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

#ifndef AXIS4_DRIVER_ENABLE
  #define AXIS4_DRIVER_ENABLE LOW
#endif
#if AXIS4_DRIVER_ENABLE == LOW
  #define AXIS4_DRIVER_DISABLE HIGH
#elif AXIS4_DRIVER_ENABLE == HIGH
  #define AXIS4_DRIVER_DISABLE LOW
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

#ifndef AXIS5_DRIVER_ENABLE
  #define AXIS5_DRIVER_ENABLE LOW
#endif
#if AXIS5_DRIVER_ENABLE == LOW
  #define AXIS5_DRIVER_DISABLE HIGH
#elif AXIS5_DRIVER_ENABLE == HIGH
  #define AXIS5_DRIVER_DISABLE LOW
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
#elif SERIAL_A_BAUD_DEFAULT != 9600 && SERIAL_A_BAUD_DEFAULT != 19200 && SERIAL_A_BAUD_DEFAULT != 38400 && SERIAL_A_BAUD_DEFAULT != 57600 && SERIAL_A_BAUD_DEFAULT != 115200 && SERIAL_B_BAUD_DEFAULT != 256000
  #error "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT invalid, use 9600, 19200, 38400, 57600 or 115200 (baud.)"
#elif defined(HAL_SLOW_PROCESSOR) && SERIAL_A_BAUD_DEFAULT != OFF && SERIAL_A_BAUD_DEFAULT != 9600 && SERIAL_A_BAUD_DEFAULT != 19200
  #error "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT invalid for HAL_SLOW_PROCESSOR, use OFF or 9600, 19200 (baud.)"
#endif
#ifndef SERIAL_B_BAUD_DEFAULT
  #error "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT must be present!"
#elif SERIAL_B_BAUD_DEFAULT != OFF && SERIAL_B_BAUD_DEFAULT != 9600 && SERIAL_B_BAUD_DEFAULT != 19200 && SERIAL_B_BAUD_DEFAULT != 38400 && SERIAL_B_BAUD_DEFAULT != 57600 && SERIAL_B_BAUD_DEFAULT != 115200 && SERIAL_B_BAUD_DEFAULT != 256000
  #error "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT invalid, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
#elif defined(HAL_SLOW_PROCESSOR) && SERIAL_B_BAUD_DEFAULT != OFF && SERIAL_B_BAUD_DEFAULT != 9600 && SERIAL_B_BAUD_DEFAULT != 19200
  #error "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT invalid for HAL_SLOW_PROCESSOR, use OFF or 9600, 19200 (baud.)"
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
  #error "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT must be present!"
#elif !defined(HAL_SERIAL_C_BLUETOOTH)
  #if SERIAL_C_BAUD_DEFAULT != OFF && SERIAL_C_BAUD_DEFAULT != 9600 && SERIAL_C_BAUD_DEFAULT != 19200 && SERIAL_C_BAUD_DEFAULT != 38400 && SERIAL_C_BAUD_DEFAULT != 57600 && SERIAL_C_BAUD_DEFAULT != 115200 && SERIAL_B_BAUD_DEFAULT != 256000
    #error "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT invalid, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
  #elif defined(HAL_SLOW_PROCESSOR) && SERIAL_C_BAUD_DEFAULT != OFF && SERIAL_C_BAUD_DEFAULT != 9600 && SERIAL_C_BAUD_DEFAULT != 19200
    #error "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT invalid for HAL_SLOW_PROCESSOR, use OFF or 9600, 19200 (baud.)"
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
#elif LED_RETICLE != OFF && (LED_RETICLE < 0 || LED_RETICLE > 255)
  #error "Configuration (Config.h): Setting LED_RETICLE invalid, use OFF or a number between 0 and 255 (0 to 100% power.)"
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

#ifndef TIME_LOCATION_SOURCE
  #error "Configuration (Config.h): Setting TIME_LOCATION_SOURCE must be present!"
#elif TIME_LOCATION_SOURCE != OFF && (TIME_LOCATION_SOURCE < TLS_FIRST || TIME_LOCATION_SOURCE > TLS_LAST)
  #error "Configuration (Config.h): Setting TIME_LOCATION_SOURCE invalid, use OFF or DS3231, DS3234, TEENSY, GPS only."
#endif

#ifndef WEATHER
  #error "Configuration (Config.h): Setting WEATHER must be present!"
#elif WEATHER != OFF && (WEATHER < WEATHER_FIRST || WEATHER > WEATHER_LAST)
  #error "Configuration (Config.h): Setting WEATHER sensor invalid, use OFF or BME280, BME280_0x76, BME280SPI only."
#endif

#ifndef WEATHER_SUPRESS_ERRORS
  #define WEATHER_SUPRESS_ERRORS OFF
#endif

#if FOCUSER1 == OFF && FOCUSER2 != OFF
  #error "Configuration (Config.h): FOCUSER2 can't be enabled if FOCUSER1 isn't enabled; if using only one focuser it must be FOCUSER1."
#endif

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #define FEATURES_PRESENT
#endif

#ifdef FEATURES_PRESENT
#ifndef FEATURE1_NAME
  #error "Configuration (Config.h): Setting FEATURE1_NAME must be present!"
#endif
#ifndef FEATURE1_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE1_PURPOSE must be present!"
#elif FEATURE1_PURPOSE != OFF && (FEATURE1_PURPOSE < AUXILLARY_FIRST || FEATURE1_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE1_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE1_TEMP
  #error "Configuration (Config.h): Setting FEATURE1_TEMP must be present!"
#elif FEATURE1_TEMP != OFF && (FEATURE1_TEMP & DS_MASK) != DS1820 && (FEATURE1_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE1_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE1_PIN
  #error "Configuration (Config.h): Setting FEATURE1_PIN must be present!"
#elif FEATURE1_PIN != OFF && (FEATURE1_PIN & DS_MASK) != DS2413 && FEATURE1_PIN < 0 && FEATURE1_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE1_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE1_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE1_DEFAULT_VALUE must be present!"
#elif FEATURE1_PURPOSE == SWITCH || FEATURE1_PURPOSE == DEW_HEATER
  #if FEATURE1_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE1_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE1_PURPOSE == ANALOG_OUT
  #if FEATURE1_DEFAULT_VALUE != OFF && (FEATURE1_DEFAULT_VALUE < 0 || FEATURE1_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE1_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE1_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE1_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE2_NAME
  #error "Configuration (Config.h): Setting FEATURE2_NAME must be present!"
#endif
#ifndef FEATURE2_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE must be present!"
#elif FEATURE2_PURPOSE != OFF && (FEATURE2_PURPOSE < AUXILLARY_FIRST || FEATURE2_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE2_TEMP
  #error "Configuration (Config.h): Setting FEATURE2_TEMP must be present!"
#elif FEATURE2_TEMP != OFF && (FEATURE2_TEMP & DS_MASK) != DS1820 && (FEATURE2_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE2_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE2_PIN
  #error "Configuration (Config.h): Setting FEATURE2_PIN must be present!"
#elif FEATURE2_PIN != OFF && (FEATURE2_PIN & DS_MASK) != DS2413 && FEATURE2_PIN < 0 && FEATURE2_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE2_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE2_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE2_DEFAULT_VALUE must be present!"
#elif FEATURE2_PURPOSE == SWITCH || FEATURE2_PURPOSE == DEW_HEATER
  #if FEATURE2_DEFAULT_VALUE != OFF && FEATURE2_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE2_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE2_PURPOSE == ANALOG_OUT
  #if FEATURE2_DEFAULT_VALUE != OFF && (FEATURE2_DEFAULT_VALUE < 0 || FEATURE2_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE2_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE2_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE2_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE3_NAME
  #error "Configuration (Config.h): Setting FEATURE3_NAME must be present!"
#endif
#ifndef FEATURE3_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE must be present!"
#elif FEATURE3_PURPOSE != OFF && (FEATURE3_PURPOSE < AUXILLARY_FIRST || FEATURE3_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE3_TEMP
  #error "Configuration (Config.h): Setting FEATURE3_TEMP must be present!"
#elif FEATURE3_TEMP != OFF && (FEATURE3_TEMP & DS_MASK) != DS1820 && (FEATURE3_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE3_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE3_PIN
  #error "Configuration (Config.h): Setting FEATURE3_PIN must be present!"
#elif FEATURE3_PIN != OFF && (FEATURE3_PIN & DS_MASK) != DS2413 && FEATURE3_PIN < 0 && FEATURE3_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE3_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE3_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE3_DEFAULT_VALUE must be present!"
#elif FEATURE3_PURPOSE == SWITCH || FEATURE3_PURPOSE == DEW_HEATER
  #if FEATURE3_DEFAULT_VALUE != OFF && FEATURE3_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE3_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE3_PURPOSE == ANALOG_OUT
  #if FEATURE3_DEFAULT_VALUE != OFF && (FEATURE3_DEFAULT_VALUE < 0 || FEATURE3_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE3_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE3_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE3_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE4_NAME
  #error "Configuration (Config.h): Setting FEATURE4_NAME must be present!"
#endif
#ifndef FEATURE4_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE must be present!"
#elif FEATURE4_PURPOSE != OFF && (FEATURE4_PURPOSE < AUXILLARY_FIRST || FEATURE4_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE4_TEMP
  #error "Configuration (Config.h): Setting FEATURE4_TEMP must be present!"
#elif FEATURE4_TEMP != OFF && (FEATURE4_TEMP & DS_MASK) != DS1820 && (FEATURE4_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE4_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE4_PIN
  #error "Configuration (Config.h): Setting FEATURE4_PIN must be present!"
#elif FEATURE4_PIN != OFF && (FEATURE4_PIN & DS_MASK) != DS2413 && FEATURE4_PIN < 0 && FEATURE4_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE4_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE4_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE4_DEFAULT_VALUE must be present!"
#elif FEATURE4_PURPOSE == SWITCH || FEATURE4_PURPOSE == DEW_HEATER
  #if FEATURE4_DEFAULT_VALUE != OFF && FEATURE4_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE4_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE4_PURPOSE == ANALOG_OUT
  #if FEATURE4_DEFAULT_VALUE != OFF && (FEATURE4_DEFAULT_VALUE < 0 || FEATURE4_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE4_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE4_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE4_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE5_NAME
  #error "Configuration (Config.h): Setting FEATURE5_NAME must be present!"
#endif
#ifndef FEATURE5_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE must be present!"
#elif FEATURE5_PURPOSE != OFF && (FEATURE5_PURPOSE < AUXILLARY_FIRST || FEATURE5_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE5_TEMP
  #error "Configuration (Config.h): Setting FEATURE5_TEMP must be present!"
#elif FEATURE5_TEMP != OFF && (FEATURE5_TEMP & DS_MASK) != DS1820 && (FEATURE5_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE5_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE5_PIN
  #error "Configuration (Config.h): Setting FEATURE5_PIN must be present!"
#elif FEATURE5_PIN != OFF && (FEATURE5_PIN & DS_MASK) != DS2413 && FEATURE5_PIN < 0 && FEATURE5_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE5_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE5_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE5_DEFAULT_VALUE must be present!"
#elif FEATURE5_PURPOSE == SWITCH || FEATURE5_PURPOSE == DEW_HEATER
  #if FEATURE5_DEFAULT_VALUE != OFF && FEATURE5_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE5_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE5_PURPOSE == ANALOG_OUT
  #if FEATURE5_DEFAULT_VALUE != OFF && (FEATURE5_DEFAULT_VALUE < 0 || FEATURE5_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE5_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE5_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE5_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE6_NAME
  #error "Configuration (Config.h): Setting FEATURE6_NAME must be present!"
#endif
#ifndef FEATURE6_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE must be present!"
#elif FEATURE6_PURPOSE != OFF && (FEATURE6_PURPOSE < AUXILLARY_FIRST || FEATURE6_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE6_TEMP
  #error "Configuration (Config.h): Setting FEATURE6_TEMP must be present!"
#elif FEATURE6_TEMP != OFF && (FEATURE6_TEMP & DS_MASK) != DS1820 && (FEATURE6_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE6_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE6_PIN
  #error "Configuration (Config.h): Setting FEATURE6_PIN must be present!"
#elif FEATURE6_PIN != OFF && (FEATURE6_PIN & DS_MASK) != DS2413 && FEATURE6_PIN < 0 && FEATURE6_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE6_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE6_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE6_DEFAULT_VALUE must be present!"
#elif FEATURE6_PURPOSE == SWITCH || FEATURE6_PURPOSE == DEW_HEATER
  #if FEATURE6_DEFAULT_VALUE != OFF && FEATURE6_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE6_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE6_PURPOSE == ANALOG_OUT
  #if FEATURE6_DEFAULT_VALUE != OFF && (FEATURE6_DEFAULT_VALUE < 0 || FEATURE6_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE6_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE6_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE6_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE7_NAME
  #error "Configuration (Config.h): Setting FEATURE7_NAME must be present!"
#endif
#ifndef FEATURE7_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE must be present!"
#elif FEATURE7_PURPOSE != OFF && (FEATURE7_PURPOSE < AUXILLARY_FIRST || FEATURE7_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE7_TEMP
  #error "Configuration (Config.h): Setting FEATURE7_TEMP must be present!"
#elif FEATURE7_TEMP != OFF && (FEATURE7_TEMP & DS_MASK) != DS1820 && (FEATURE7_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE7_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE7_PIN
  #error "Configuration (Config.h): Setting FEATURE7_PIN must be present!"
#elif FEATURE7_PIN != OFF && (FEATURE7_PIN & DS_MASK) != DS2413 && FEATURE7_PIN < 0 && FEATURE7_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE7_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE7_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE7_DEFAULT_VALUE must be present!"
#elif FEATURE7_PURPOSE == SWITCH || FEATURE7_PURPOSE == DEW_HEATER
  #if FEATURE7_DEFAULT_VALUE != OFF && FEATURE7_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE7_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE7_PURPOSE == ANALOG_OUT
  #if FEATURE7_DEFAULT_VALUE != OFF && (FEATURE7_DEFAULT_VALUE < 0 || FEATURE7_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE7_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE7_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE7_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#ifndef FEATURE8_NAME
  #error "Configuration (Config.h): Setting FEATURE8_NAME must be present!"
#endif
#ifndef FEATURE8_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE must be present!"
#elif FEATURE8_PURPOSE != OFF && (FEATURE8_PURPOSE < AUXILLARY_FIRST || FEATURE8_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, etc."
#endif
#ifndef FEATURE8_TEMP
  #error "Configuration (Config.h): Setting FEATURE8_TEMP must be present!"
#elif FEATURE8_TEMP != OFF && (FEATURE8_TEMP & DS_MASK) != DS1820 && (FEATURE8_TEMP & DS_MASK) != DS18S20
  #error "Configuration (Config.h): Setting FEATURE8_TEMP invalid, use OFF, DS1820, or DS1820 s/n only."
#endif
#ifndef FEATURE8_PIN
  #error "Configuration (Config.h): Setting FEATURE8_PIN must be present!"
#elif FEATURE8_PIN != OFF && (FEATURE8_PIN & DS_MASK) != DS2413 && FEATURE8_PIN < 0 && FEATURE8_PIN > 255
  #error "Configuration (Config.h): Setting FEATURE8_PIN invalid, use OFF, DS2413, DS2413 s/n, or a valid pin# (0 to 255 only.)"
#endif
#ifndef FEATURE8_DEFAULT_VALUE
  #error "Configuration (Config.h): Setting FEATURE8_DEFAULT_VALUE must be present!"
#elif FEATURE8_PURPOSE == SWITCH || FEATURE8_PURPOSE == DEW_HEATER
  #if FEATURE8_DEFAULT_VALUE != OFF && FEATURE8_DEFAULT_VALUE != ON
    #error "Configuration (Config.h): Setting FEATURE8_DEFAULT_VALUE (SWITCH or DEW_HEATER) invalid, use OFF or ON only."
  #endif
#elif FEATURE8_PURPOSE == ANALOG_OUT
  #if FEATURE8_DEFAULT_VALUE != OFF && (FEATURE8_DEFAULT_VALUE < 0 || FEATURE8_DEFAULT_VALUE > 255)
    #error "Configuration (Config.h): Setting FEATURE8_DEFAULT_VALUE (ANALOG OUT) invalid, use OFF or 0 to 255 only."
  #endif
#else // everything else
  #if FEATURE8_DEFAULT_VALUE != OFF
    #error "Configuration (Config.h): Setting FEATURE8_DEFAULT_VALUE (etc.) invalid, use OFF only."
  #endif
#endif

#if (FEATURE1_TEMP & DS_MASK) == DS1820 || (FEATURE2_TEMP & DS_MASK) == DS1820 || (FEATURE3_TEMP & DS_MASK) == DS1820 || (FEATURE4_TEMP & DS_MASK) == DS1820 || (FEATURE5_TEMP & DS_MASK) == DS1820 || (FEATURE6_TEMP & DS_MASK) == DS1820 || (FEATURE7_TEMP & DS_MASK) == DS1820 || (FEATURE8_TEMP & DS_MASK) == DS1820 
  #define DS1820_DEVICES_PRESENT
#endif

#if (FEATURE1_TEMP & DS_MASK) == DS18S20 || (FEATURE2_TEMP & DS_MASK) == DS18S20 || (FEATURE3_TEMP & DS_MASK) == DS18S20 || (FEATURE4_TEMP & DS_MASK) == DS18S20 || (FEATURE5_TEMP & DS_MASK) == DS18S20 || (FEATURE6_TEMP & DS_MASK) == DS18S20 || (FEATURE7_TEMP & DS_MASK) == DS18S20 || (FEATURE8_TEMP & DS_MASK) == DS18S20 
  #define DS1820_DEVICES_PRESENT
#endif

#if (FEATURE1_PIN & DS_MASK) == DS2413 || (FEATURE2_PIN & DS_MASK) == DS2413 || (FEATURE3_PIN & DS_MASK) == DS2413 || (FEATURE4_PIN & DS_MASK) == DS2413 || (FEATURE5_PIN & DS_MASK) == DS2413 || (FEATURE6_PIN & DS_MASK) == DS2413 || (FEATURE7_PIN & DS_MASK) == DS2413 || (FEATURE8_PIN & DS_MASK) == DS2413
  #define DS2413_DEVICES_PRESENT
#endif
#endif

#if !defined(DS1820_DEVICES_PRESENT) && (TELESCOPE_TEMPERATURE & DS_MASK) == DS1820
  #define DS1820_DEVICES_PRESENT
#endif

#if !defined(DS1820_DEVICES_PRESENT) && (TELESCOPE_TEMPERATURE & DS_MASK) == DS18S20
  #define DS1820_DEVICES_PRESENT
#endif

#if defined(DS2413_DEVICES_PRESENT) || defined(DS1820_DEVICES_PRESENT)
  #define ONEWIRE_DEVICES_PRESENT
#endif

#ifndef TELESCOPE_TEMPERATURE
  #error "Configuration (Config.h): Setting TELESCOPE_TEMPERATURE must be present!"
#elif TELESCOPE_TEMPERATURE != OFF && TELESCOPE_TEMPERATURE != DS1820 && (TELESCOPE_TEMPERATURE & 0x3f00000000000000) != 0x2800000000000000
  #error "Configuration (Config.h): Setting TELESCOPE_TEMPERATURE sensor invalid, use OFF, DS1820, or a DS1820 serial# only."
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
#elif (PEC_SENSE != OFF && PEC_SENSE != ON && PEC_SENSE != ON_PULLUP && PEC_SENSE != ON_PULLDOWN) && PEC_SENSE < 0 && PEC_SENSE > 1023
  #error "Configuration (Config.h): Setting PEC_SENSE invalid, use OFF, ON, ON_PULLUP, ON_PULLDOWN or a number between 0 and 1023 (0 to 3.3V or 0 to 5V) only."
#endif

#ifndef PEC_SENSE_STATE
  #error "Configuration (Config.h): Setting PEC_SENSE_STATE must be present!"
#elif PEC_SENSE_STATE != HIGH && PEC_SENSE_STATE != LOW
  #error "Configuration (Config.h): Setting PEC_SENSE_STATE invalid, use HIGH or LOW."
#endif

#ifndef PPS_SENSE
  #error "Configuration (Config.h): Setting PPS_SENSE must be present!"
#elif PPS_SENSE != OFF && PPS_SENSE != ON && PPS_SENSE != ON_BOTH && PPS_SENSE != ON_PULLUP && PPS_SENSE != ON_PULLDOWN
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
#elif TRACK_BACKLASH_RATE < 5 || TRACK_BACKLASH_RATE > 50
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE invalid, use a number between 5 and 50 (x sidereal rate.)"
#elif TRACK_BACKLASH_RATE > 25 && (AXIS1_DRIVER_MICROSTEPS_GOTO != OFF || AXIS2_DRIVER_MICROSTEPS_GOTO != OFF)
  #warning "Configuration (Config.h): Setting TRACK_BACKLASH_RATE, above 25x can cause problems if AXISn_STEPS_PER_DEGREE > 30600 *and* on-the-fly micro-step mode switching is used"
#endif

#ifndef PIER_SIDE_SYNC_CHANGE_SIDES
  #error "Configuration (Config.h): Setting PIER_SIDE_SYNC_CHANGE_SIDES must be present!"
#elif PIER_SIDE_SYNC_CHANGE_SIDES != OFF && PIER_SIDE_SYNC_CHANGE_SIDES != ON
  #error "Configuration (Config.h): Setting PIER_SIDE_SYNC_CHANGE_SIDES invalid, use OFF or ON only."
#endif

#ifndef PIER_SIDE_PREFERRED_DEFAULT
  #error "Configuration (Config.h): Setting PIER_SIDE_PREFERRED_DEFAULT must be present!"
#elif PIER_SIDE_PREFERRED_DEFAULT != BEST && PIER_SIDE_PREFERRED_DEFAULT != EAST && PIER_SIDE_PREFERRED_DEFAULT != WEST
  #error "Configuration (Config.h): Setting PIER_SIDE_PREFERRED_DEFAULT invalid, use BEST, EAST, or WEST only."
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
#if defined(__AVR_ATmega2560__) && STEP_WAVE_FORM == SQUARE
  #warning "Configuration (Config.h): When compiling for Mega2560, STEP_WAVE_FORM PULSE is recommended for best performance."
#endif

#if MOUNT_TYPE == ALTAZM
  #ifndef AXIS1_LIMIT_MIN
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN must be present!"
  #elif AXIS1_LIMIT_MIN > -180 || AXIS1_LIMIT_MIN < -360
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN invalid, use a number between -180 and -360 (degrees.)"
  #endif
  #ifndef AXIS1_LIMIT_MAX
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX must be present!"
  #elif AXIS1_LIMIT_MAX < 180 || AXIS1_LIMIT_MAX > 360
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX invalid, use a number between 180 and 360 (degrees.)"
  #endif
#else
  #ifndef AXIS1_LIMIT_MIN
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN must be present!"
  #elif AXIS1_LIMIT_MIN < -270 || AXIS1_LIMIT_MIN > -90
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN invalid, use a number between -90 and -270 (degrees.)"
  #endif
  #ifndef AXIS1_LIMIT_MAX
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX must be present!"
  #elif AXIS1_LIMIT_MAX < 90 || AXIS1_LIMIT_MAX > 270
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX invalid, use a number between 90 and 270 (degrees.)"
  #endif
#endif

#ifndef AXIS2_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN must be present!"
#elif (AXIS2_LIMIT_MIN < -90 || AXIS2_LIMIT_MIN > 0)
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN invalid, use a number between -90 and 0 (degrees.)  -90 = disabled."
#endif

#ifndef AXIS2_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX must be present!"
#elif (AXIS2_LIMIT_MAX < 0 || AXIS2_LIMIT_MAX > 90)
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX invalid, use a number between 0 and 90 (degrees.)  90 = disabled."
#endif

#ifndef AXIS2_TANGENT_ARM
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM must be present!"
#elif AXIS2_TANGENT_ARM != ON && AXIS2_TANGENT_ARM != OFF
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM invalid, use OFF or ON only."
#elif AXIS2_TANGENT_ARM == ON && MOUNT_TYPE == ALTAZM
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM is not compatible with ALTAZM mode."
#endif
#ifndef AXIS2_TANGENT_ARM_CORRECTION
  #if AXIS2_TANGENT_ARM == ON
    #define AXIS2_TANGENT_ARM_CORRECTION ON
  #else
    #define AXIS2_TANGENT_ARM_CORRECTION OFF
  #endif
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

#if !defined(AXIS4_SLEW_RATE_DESIRED) && !defined(AXIS4_STEP_RATE_MAX)
  #error "Configuration (Config.h): Setting AXIS4_SLEW_RATE_DESIRED must be present!"
#elif defined(AXIS4_SLEW_RATE_DESIRED) && AXIS4_DRIVER_DC_MODE == OFF && (AXIS4_SLEW_RATE_DESIRED < 200 || AXIS4_SLEW_RATE_DESIRED > 5000)
  #error "Configuration (Config.h): Setting AXIS4_SLEW_RATE_DESIRED invalid, use a number between 200 and 5000 (micrometers per second.)"
#elif defined(AXIS4_SLEW_RATE_DESIRED) &&AXIS4_DRIVER_DC_MODE == ON && (AXIS4_SLEW_RATE_DESIRED < 10 || AXIS4_SLEW_RATE_DESIRED > 100)
  #error "Configuration (Config.h): Setting AXIS4_SLEW_RATE_DESIRED invalid, use a number between 10 and 100%."
#endif

#ifndef AXIS4_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE must be present!"
#elif AXIS4_DRIVER_DC_MODE == OFF && (AXIS4_LIMIT_MIN_RATE < 1 || AXIS4_LIMIT_MIN_RATE > 1000)
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
#elif AXIS4_DRIVER_DC_MODE == ON && (AXIS4_LIMIT_MIN_RATE < 10 || AXIS4_LIMIT_MIN_RATE > 100) 
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE invalid, use a number between 10 and 100%."
#endif

#ifndef AXIS4_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN must be present!"
#elif (AXIS4_LIMIT_MIN < 0 || AXIS4_LIMIT_MIN > 500) && AXIS4_LIMIT_MIN < AXIS4_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN invalid, use a number between 0 and 500 (mm) but < AXIS4_LIMIT_MAX."
#endif

#ifndef AXIS4_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MAX must be present!"
#elif (AXIS4_LIMIT_MAX < 0 || AXIS4_LIMIT_MAX > 500) && AXIS4_LIMIT_MAX > AXIS4_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MAX invalid, use a number between 0 and 500 (mm) but > AXIS4_LIMIT_MIN."
#endif

#if !defined(AXIS5_SLEW_RATE_DESIRED) && !defined(AXIS5_STEP_RATE_MAX)
  #error "Configuration (Config.h): Setting AXIS5_SLEW_RATE_DESIRED must be present!"
#elif defined(AXIS5_SLEW_RATE_DESIRED) && AXIS5_DRIVER_DC_MODE == OFF && (AXIS5_SLEW_RATE_DESIRED < 200 || AXIS5_SLEW_RATE_DESIRED > 5000) 
  #error "Configuration (Config.h): Setting AXIS5_SLEW_RATE_DESIRED invalid, use a number between 200 and 5000 (micrometers per second.)"
#elif defined(AXIS5_SLEW_RATE_DESIRED) && AXIS5_DRIVER_DC_MODE == ON && (AXIS5_SLEW_RATE_DESIRED < 10 || AXIS5_SLEW_RATE_DESIRED > 100) 
  #error "Configuration (Config.h): Setting AXIS5_SLEW_RATE_DESIRED invalid, use a number between 10 and 100%."
#endif

#ifndef AXIS5_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE must be present!"
#elif AXIS5_DRIVER_DC_MODE == OFF && (AXIS5_LIMIT_MIN_RATE < 1 || AXIS5_LIMIT_MIN_RATE > 1000)
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
#elif AXIS5_DRIVER_DC_MODE == ON && (AXIS5_LIMIT_MIN_RATE < 10 || AXIS5_LIMIT_MIN_RATE > 100) 
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE invalid, use a number between 10 and 100%."
#endif

#ifndef AXIS5_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN must be present!"
#elif (AXIS5_LIMIT_MIN < 0 || AXIS5_LIMIT_MIN > 500) && AXIS5_LIMIT_MIN < AXIS5_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN invalid, use a number between 0 and 500 (mm) but < AXIS5_LIMIT_MAX."
#endif

#ifndef AXIS5_LIMIT_MAX
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MAX must be present!"
#elif (AXIS5_LIMIT_MAX < 0 || AXIS5_LIMIT_MAX > 500) && AXIS5_LIMIT_MAX > AXIS5_LIMIT_MIN
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MAX invalid, use a number between 0 and 500 (mm) but > AXIS5_LIMIT_MIN."
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// stepper driver mode setup validation

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
    #if AXIS1_DRIVER_MODEL != SERVO && AXIS1_DRIVER_MODEL != SERVO1 && AXIS1_DRIVER_MODEL != SERVO2
      #if (AXIS1_DRIVER_MICROSTEPS <= AXIS1_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS1_DRIVER_MICROSTEPS == AXIS1_DRIVER_MICROSTEPS_GOTO))
          #error "Configuration (Config.h): AXIS1_DRIVER_MICROSTEPS_GOTO should be less than AXIS1_DRIVER_MICROSTEPS or OFF"
      #endif
    #endif
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MODEL != SERVO && AXIS2_DRIVER_MODEL != SERVO1 && AXIS2_DRIVER_MODEL != SERVO2
      #if (AXIS2_DRIVER_MICROSTEPS <= AXIS2_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS2_DRIVER_MICROSTEPS == AXIS2_DRIVER_MICROSTEPS_GOTO))
          #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO should be less than AXIS2_DRIVER_MICROSTEPS or OFF"
      #endif
    #endif
  #endif

  // initialize custom step dir driver values
  #include "src/sd_drivers/Init.TMC2100.h"
  #include "src/sd_drivers/Init.TMC2130.h"
  #include "src/sd_drivers/Init.TMC2209.h"
  #include "src/sd_drivers/Init.TMC5160.h"
  #include "src/sd_drivers/Init.TMC_SPI.h"
  #include "src/sd_drivers/Init.GENERIC.h"
  #include "src/sd_drivers/Init.SERVO.h"

  // check to see if a valid step dir driver code exists
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

  // validate step dir driver setup
  #include "src/sd_drivers/Validate.A4988.h"
  #include "src/sd_drivers/Validate.DRV8825.h"
  #include "src/sd_drivers/Validate.RAPS128.h"
  #include "src/sd_drivers/Validate.LV8729.h"
  #include "src/sd_drivers/Validate.S109.h"
  #include "src/sd_drivers/Validate.ST820.h"
  #include "src/sd_drivers/Validate.TMC2100.h"
  #include "src/sd_drivers/Validate.TMC2208.h"
  #include "src/sd_drivers/Validate.TMC2209.h"
  #include "src/sd_drivers/Validate.TMC_SPI.h"
  #include "src/sd_drivers/Validate.GENERIC.h"
  #include "src/sd_drivers/Validate.SERVO.h"

  #if AXIS1_DRIVER_DECAY_MODE_GOTO == STEALTHCHOP || AXIS2_DRIVER_DECAY_MODE_GOTO == STEALTHCHOP
    #warning "Configuration (Config.h): TMC stepper driver _VQUIET mode is generally not recommended except for situations where motor RPM is low."
  #endif

  // for stepper drivers where AXISn_MICROSTEPS_GOTO must be defined
  #if MODE_SWITCH_BEFORE_SLEW == ON && AXIS1_DRIVER_MICROSTEPS != OFF && AXIS1_DRIVER_MICROSTEPS_GOTO == OFF
    #undef AXIS1_DRIVER_MICROSTEPS_GOTO
    #define AXIS1_DRIVER_MICROSTEPS_GOTO AXIS1_DRIVER_MICROSTEPS
    #undef AXIS2_DRIVER_MICROSTEPS_GOTO
    #define AXIS2_DRIVER_MICROSTEPS_GOTO AXIS2_DRIVER_MICROSTEPS
  #endif

#else
  #warning "Configuration (Config.h): Stepper drivers for Axis1 and Axis2 are not defined.  Be sure to properly configure micro-step mode, Vref/current, etc. manually with shunts, dip-switches, as required."
#endif

// if fast Axis1/2 step signal routines aren't defined, do so
#ifndef a1STEP
  #define a1STEP a1STEP_H
#endif
#ifndef a1CLEAR
  #define a1CLEAR a1STEP_L
#endif
#ifndef a2STEP
  #define a2STEP a2STEP_H
#endif
#ifndef a2CLEAR
  #define a2CLEAR a2STEP_L
#endif

// make sure current settings aren't present unless an TMC_SPI driver is present
#if AXIS1_DRIVER_MODEL != TMC_SPI
  #if AXIS1_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS1_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS1_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS1_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS1_DRIVER_IGOTO != OFF
    #error "Configuration (Config.h): AXIS1_DRIVER_IGOTO must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if AXIS2_DRIVER_MODEL != TMC_SPI
  #if AXIS2_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS2_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS2_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS2_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS2_DRIVER_IGOTO != OFF
    #error "Configuration (Config.h): AXIS2_DRIVER_IGOTO must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if ROTATOR == ON && AXIS3_DRIVER_MODEL != TMC_SPI
  #if AXIS3_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS3_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS3_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS3_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if FOCUSER1 == ON && AXIS4_DRIVER_MODEL != TMC_SPI
  #if AXIS4_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS4_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_MODEL != TMC_SPI
  #if AXIS5_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS5_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS5_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS5_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// Validate pinmaps

#include "src/pinmaps/ValidatePins.h"
