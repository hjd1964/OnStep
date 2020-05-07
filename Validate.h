// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// correct for configuration backwards compatability

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

// default allowed degrees past the meridian on the East and West sides of the pier
#define AXIS1_LIMIT_MERIDIAN_E 7.5 
#define AXIS1_LIMIT_MERIDIAN_W 7.5

// always enable the separate pulse guide rate
#define SEPARATE_PULSE_GUIDE_RATE ON

// automatically set MaxRate from SLEW_RATE_BASE_DESIRED
#define MaxRate ((1000000.0/SLEW_RATE_BASE_DESIRED)/AXIS1_STEPS_PER_DEGREE)

// automatically calculate the pecBufferSize
#if MOUNT_TYPE == ALTAZM
  #define PEC_BUFFER_SIZE 0
#else
  #define PEC_BUFFER_SIZE ceil(AXIS1_STEPS_PER_WORMROT/(AXIS1_STEPS_PER_DEGREE/240.0))
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
  #error "Configuration (Config.h): Setting FEATURE1_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE2_NAME
  #error "Configuration (Config.h): Setting FEATURE2_NAME must be present!"
#endif
#ifndef FEATURE2_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE must be present!"
#elif FEATURE2_PURPOSE != OFF && (FEATURE2_PURPOSE < AUXILLARY_FIRST || FEATURE2_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE3_NAME
  #error "Configuration (Config.h): Setting FEATURE3_NAME must be present!"
#endif
#ifndef FEATURE3_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE must be present!"
#elif FEATURE3_PURPOSE != OFF && (FEATURE3_PURPOSE < AUXILLARY_FIRST || FEATURE3_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE4_NAME
  #error "Configuration (Config.h): Setting FEATURE4_NAME must be present!"
#endif
#ifndef FEATURE4_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE must be present!"
#elif FEATURE4_PURPOSE != OFF && (FEATURE4_PURPOSE < AUXILLARY_FIRST || FEATURE4_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE5_NAME
  #error "Configuration (Config.h): Setting FEATURE5_NAME must be present!"
#endif
#ifndef FEATURE5_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE must be present!"
#elif FEATURE5_PURPOSE != OFF && (FEATURE5_PURPOSE < AUXILLARY_FIRST || FEATURE5_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE6_NAME
  #error "Configuration (Config.h): Setting FEATURE6_NAME must be present!"
#endif
#ifndef FEATURE6_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE must be present!"
#elif FEATURE6_PURPOSE != OFF && (FEATURE6_PURPOSE < AUXILLARY_FIRST || FEATURE6_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE7_NAME
  #error "Configuration (Config.h): Setting FEATURE7_NAME must be present!"
#endif
#ifndef FEATURE7_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE must be present!"
#elif FEATURE7_PURPOSE != OFF && (FEATURE7_PURPOSE < AUXILLARY_FIRST || FEATURE7_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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

#ifndef FEATURE8_NAME
  #error "Configuration (Config.h): Setting FEATURE8_NAME must be present!"
#endif
#ifndef FEATURE8_PURPOSE
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE must be present!"
#elif FEATURE8_PURPOSE != OFF && (FEATURE8_PURPOSE < AUXILLARY_FIRST || FEATURE8_PURPOSE > AUXILLARY_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE invalid, use OFF, SWITCH, ANALOG_OUTPUT, DEW_HEATER, or INTERVALOMETER."
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
#elif TRACK_BACKLASH_RATE < 5 || TRACK_BACKLASH_RATE > 50
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE invalid, use a number between 5 and 50 (x sidereal rate.)"
#elif TRACK_BACKLASH_RATE > 25 && (AXIS1_DRIVER_MICROSTEPS_GOTO != OFF || AXIS2_DRIVER_MICROSTEPS_GOTO != OFF)
  #warning "Configuration (Config.h): Setting TRACK_BACKLASH_RATE, above 25x can cause problems if AXISn_STEPS_PER_DEGREE > 30600 *and* on-the-fly micro-step mode switching is used"
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
#if defined(__AVR_ATmega2560__) && STEP_WAVE_FORM == SQUARE
  #warning "Configuration (Config.h): When compiling for Mega2560, STEP_WAVE_FORM PULSE is recommended for best performance."
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

#ifndef AXIS2_TANGENT_ARM
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM must be present!"
#elif (AXIS2_TANGENT_ARM != ON && AXIS2_TANGENT_ARM != OFF)
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM invalid, use OFF or ON only."
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

#ifndef AXIS4_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE must be present!"
#elif AXIS4_LIMIT_MIN_RATE < 1 || AXIS4_LIMIT_MIN_RATE > 1000
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
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

#ifndef AXIS5_LIMIT_MIN_RATE
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE must be present!"
#elif AXIS5_LIMIT_MIN_RATE < 1 || AXIS5_LIMIT_MIN_RATE > 1000
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN_RATE invalid, use a number between 1 and 1000 (micrometers per second.)"
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
      #define AXIS1_DRIVER_STEP_GOTO (AXIS1_DRIVER_MICROSTEPS/AXIS1_DRIVER_MICROSTEPS_GOTO)
    #else
      #define AXIS1_DRIVER_STEP_GOTO (AXIS1_DRIVER_MICROSTEPS_GOTO/AXIS1_DRIVER_MICROSTEPS)
    #endif
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if AXIS2_DRIVER_MODEL != SERVO && AXIS2_DRIVER_MODEL != SERVO1 && AXIS2_DRIVER_MODEL != SERVO2
      #if (AXIS2_DRIVER_MICROSTEPS <= AXIS2_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS2_DRIVER_MICROSTEPS == AXIS2_DRIVER_MICROSTEPS_GOTO))
          #error "Configuration (Config.h): AXIS2_DRIVER_MICROSTEPS_GOTO should be less than AXIS2_DRIVER_MICROSTEPS or OFF"
      #endif
      #define AXIS2_DRIVER_STEP_GOTO (AXIS2_DRIVER_MICROSTEPS/AXIS2_DRIVER_MICROSTEPS_GOTO)
    #else
      #define AXIS2_DRIVER_STEP_GOTO (AXIS2_DRIVER_MICROSTEPS_GOTO/AXIS2_DRIVER_MICROSTEPS)
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

// if AXISn_DRIVER_STEP_GOTO isn't defined, do so
#ifndef AXIS1_DRIVER_STEP_GOTO
  #define AXIS1_DRIVER_STEP_GOTO 1
#endif
#ifndef AXIS2_DRIVER_STEP_GOTO
  #define AXIS2_DRIVER_STEP_GOTO 1
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
#if AXIS3_DRIVER_MODEL != TMC_SPI
  #if AXIS3_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS3_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS3_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS3_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if AXIS4_DRIVER_MODEL != TMC_SPI
  #if AXIS4_DRIVER_IHOLD != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_IHOLD must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
  #if AXIS4_DRIVER_IRUN != OFF
    #error "Configuration (Config.h): AXIS4_DRIVER_IRUN must be OFF unless used with a TMC SPI mode stepper driver."
  #endif
#endif
#if AXIS5_DRIVER_MODEL != TMC_SPI
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
