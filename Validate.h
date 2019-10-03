// -----------------------------------------------------------------------------------
// Validate current configuration

#pragma once

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// correct for configuration backwards compatability

// -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------
// setup defaults

// initial allowed degrees past the meridian on the East and West sides of the pier
#define AXIS1_LIMIT_MERIDIAN_E 7.5 
#define AXIS1_LIMIT_MERIDIAN_W 7.5

// automatically set MaxRate from SLEW_RATE_BASE_DESIRED
#define MaxRate ((1000000.0/SLEW_RATE_BASE_DESIRED)/AXIS1_STEPS_PER_DEGREE)

// automatically calculate the pecBufferSize
#define PEC_BUFFER_SIZE ceil(AXIS1_STEPS_PER_WORMROT/(AXIS1_STEPS_PER_DEGREE/240.0))

// always enable the separate pulse guide rate
#define SEPARATE_PULSE_GUIDE_RATE ON

// set serial port baud rate if not done so already
#ifndef SERIAL_A_BAUD_DEFAULT
  #define SERIAL_A_BAUD_DEFAULT 9600
#endif
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
// stepper driver mode setup validation

// check timer wave mode
#if STEP_WAVE_FORM != PULSE && STEP_WAVE_FORM != SQUARE && STEP_WAVE_FORM != DEDGE
  #error "Configuration: STEP_WAVE_FORM; Unknown value, please correct your configuration."
#endif
#if STEP_WAVE_FORM != PULSE
  #undef HAL_PULSE_WIDTH
  // set pulse width in SQW and DEDGE mode well above triggering PULSE check code
  #define HAL_PULSE_WIDTH 10000
#endif

// check model
#if (AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL == OFF) || (AXIS2_DRIVER_MODEL != OFF && AXIS1_DRIVER_MODEL == OFF)
  #error "AXISn_DRIVER_MODEL; If using the driver mode control you must use it for both AXIS1 and AXIS2."
#endif

#if AXIS1_DRIVER_MODEL != OFF

  // validate tracking and goto micro-step modes
  #if AXIS1_DRIVER_MICROSTEPS == OFF
    #error "AXIS1_DRIVER_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS1_DRIVER_MODEL!"
  #endif
  #if AXIS2_DRIVER_MICROSTEPS == OFF
    #error "AXIS2_DRIVER_MICROSTEPS must be defined in your Config.xxx.h file if using AXIS2_DRIVER_MODEL!"
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #if (AXIS1_DRIVER_MICROSTEPS <= AXIS1_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS1_DRIVER_MICROSTEPS == AXIS1_DRIVER_MICROSTEPS_GOTO))
        #error "Configuration: AXIS1_DRIVER_MICROSTEPS_GOTO should be less than AXIS1_DRIVER_MICROSTEPS or OFF"
    #endif
    #define AXIS1_DRIVER_STEP_GOTO (AXIS1_DRIVER_MICROSTEPS/AXIS1_DRIVER_MICROSTEPS_GOTO)
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #if (AXIS2_DRIVER_MICROSTEPS <= AXIS2_DRIVER_MICROSTEPS_GOTO) && (!(MODE_SWITCH_BEFORE_SLEW != OFF && AXIS2_DRIVER_MICROSTEPS == AXIS2_DRIVER_MICROSTEPS_GOTO))
        #error "Configuration: AXIS2_DRIVER_MICROSTEPS_GOTO should be less than AXIS2_DRIVER_MICROSTEPS or OFF"
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
    #error "AXIS1_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
  #endif
  #if (AXIS2_DRIVER_MODEL < DRIVER_MODEL_FIRST) || (AXIS2_DRIVER_MODEL > DRIVER_MODEL_LAST)
    #error "AXIS2_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
  #endif
  #if ROTATOR == ON && (AXIS3_DRIVER_MODEL != OFF) && (AXIS3_DRIVER_MODEL != TMC_SPI)
    #error "AXIS3_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
  #endif
  #if FOCUSER1 == ON && (AXIS4_DRIVER_MODEL != OFF) && (AXIS4_DRIVER_MODEL != TMC_SPI)
    #error "AXIS4_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
  #endif
  #if FOCUSER2 == ON && (AXIS5_DRIVER_MODEL != OFF) && (AXIS5_DRIVER_MODEL != TMC_SPI)
    #error "AXIS5_DRIVER_MODEL; Unknown stepper driver specified, check your configuration file."
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
  #warning "Configuration: Stepper drivers for Axis1 and Axis2 are not defined.  Be sure to properly configure micro-step mode, Vref/current, etc. manually with shunts, dip-switches, as required."
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

// PINMAPS -----------------------------------------------------------------------------------------------------------------
#if PINMAP < PINMAP_FIRST || PINMAP > PINMAP_LAST
  #error "Configuration: You must specify a PINMAP, etc. in the Config.h file"
#endif

// SERIAL PORTS ------------------------------------------------------------------------------------------------------------
#if SERIAL_B_ESP_FLASHING == ON
  #if PINMAP == MiniPCB || PINMAP == MiniPCB2 || PINMAP == MaxPCB || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX1
      #define USES_AUX1 false
    #else
      #error "Configuration: SERIAL_B_ESP_FLASHING enabled but Aux1 is already in use, choose one feature on Aux1"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == MaxPCB || PINMAP == MaxTMC || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration: SERIAL_B_ESP_FLASHING enabled but Aux2 is already in use, choose one feature on Aux2"
    #endif
  #endif
  #if PINMAP == Classic || PINMAP == Mega2560Alt || PINMAP == MiniPCB2 || PINMAP == MaxPCB2
    #error "Configuration: SERIAL_B_ESP_FLASHING not supported on this PINMAP"
  #endif
#endif

// USER FEEDBACK ----------------------------
#if LED_STATUS_PIN == ON
  #if PINMAP == MaxESP || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == Ramps14 || PINMAP == STM32B
    #ifndef USES_AUX0
      #define USES_AUX0 false  // false means the AUX0 doesn't have I2C sharing
    #else
      #error "Configuration: LED_STATUS_PIN enabled but Aux0 is already in use, choose one feature on Aux0"
    #endif
  #endif
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration: LED_STATUS_PIN enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
#endif
#if LED_STATUS2_PIN == ON
  #if PINMAP == MaxESP || PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration: LED_STATUS2_PIN enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration: LED_STATUS2_PIN enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif
#if LED_RETICLE_PIN == ON
  #if PINMAP == MaxESP || PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration: LED_RETICLE_PIN enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX4
      #define USES_AUX4 
    #else
      #error "Configuration: LED_RETICLE_PIN enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif
#if BUZZER == ON
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8 
    #else
      #error "Configuration: BUZZER enabled but Aux8 is already in use, choose one feature on Aux8"
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
        #error "Configuration: RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
      #endif
    #endif
    #ifndef USES_AUX1
      #define USES_AUX1 true  // true means the AUX1 has I2C sharing
    #else
      #if USES_AUX1 != true
        #error "Configuration: RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
      #endif
    #endif
  #endif
#endif
#if PPS_SENSE == ON
  #if PINMAP == MaxESP
    #ifndef USES_AUX0
      #define USES_AUX0
    #else
      #error "Configuration: PPS_SENSE enabled but Aux0 is already in use, choose one feature on Aux0"
    #endif
  #endif
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX7
      #define USES_AUX7
    #else
      #error "Configuration: PPS_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
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
        #error "Configuration: RTC DS3231 I2C enabled but Aux0 is already in use, choose one feature on Aux0"
      #endif
    #endif
    #ifndef USES_AUX1
      #define USES_AUX1 true  // true means the AUX1 has I2C sharing
    #else
      #if USES_AUX1 != true
        #error "Configuration: RTC DS3231 I2C enabled but Aux1 is already in use, choose one feature on Aux1"
      #endif
    #endif
  #endif
#endif
#if TELESCOPE_TEMPERATURE == DS1820
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3
    #ifndef USES_AUX8
      #define USES_AUX8
    #else
      #error "Configuration: RTC DS1820 OneWire interface enabled but Aux8 is already in use, choose one feature on Aux8"
    #endif
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxTMC
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration: RTC DS1820 OneWire interface enabled but Aux4 is already in use, choose one feature on Aux4"
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
      #error "Configuration: LIMIT_SENSE enabled but Aux7 is already in use, choose one feature on Aux7"
    #endif
  #endif
  #if PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #ifndef USES_AUX3
      #define USES_AUX3
    #else
      #error "Configuration: LIMIT_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
    #endif
  #endif
#endif
#if HOME_SENSE != OFF
  #if PINMAP == MaxESP2 || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2|| PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == Ramps14
    #ifndef USES_AUX3
      #define USES_AUX3
    #else
      #error "Configuration: HOME_SENSE enabled but Aux3 is already in use, choose one feature on Aux3"
    #endif
    #ifndef USES_AUX4
      #define USES_AUX4
    #else
      #error "Configuration: HOME_SENSE enabled but Aux4 is already in use, choose one feature on Aux4"
    #endif
  #endif
#endif

// MOTION CONTROL AXIS1/2 -----------------
#if AXIS1_DRIVER_STATUS != OFF
  #if PINMAP == MaxESP2 || PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2 || PINMAP == STM32Blue
    #ifndef USES_AUX1
      #define USES_AUX1
    #else
      #error "Configuration: AXIS1_DRIVER_STATUS enabled but Aux1 is already in use, choose one feature on Aux1"
    #endif
  #endif
  #if PINMAP == MaxTMC || PINMAP == MiniEMB
    #if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
      #error "Configuration: AXIS2_DRIVER_STATUS allows Aux1 use in TMC_SPI mode only"
    #endif
  #endif
#endif

#if AXIS2_DRIVER_STATUS != OFF
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MiniPCB || PINMAP == MiniPCB2 || PINMAP == STM32Blue
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration: AXIS2_DRIVER_STATUS enabled but Aux2 is already in use, choose one feature on Aux2"
    #endif
  #endif
#endif

#if AXIS2_DRIVER_POWER_DOWN == ON
  #if PINMAP == MaxESP3 || PINMAP == MaxESP2 || PINMAP == MaxTMC
      #error "Configuration: AXIS2_DRIVER_POWER_DOWN not supported on this PINMAP"
  #endif
#endif

// MOTION CONTROL FOCUSERS/ROTATOR ----------

// focusers/rotator allowed?
#if ROTATOR == ON
  #if Axis3StepPin == -1 || Axis3DirPin == -1
    #error "Configuration: AXIS3 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER1 == ON
  #if Axis4StepPin == -1 || Axis4DirPin == -1
    #error "Configuration: AXIS4 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON
  #if (Axis5StepPin == -1 || Axis5DirPin == -1) && AXIS5_DRIVER_DC_MODE == OFF
    #error "Configuration: AXIS5 step/dir interface is not supported on this PINMAP"
  #endif
#endif

// focuser/rotators any invalid combinations?
#if FOCUSER1 == ON && ROTATOR == ON
  #if PINMAP == MaxESP2 || PINMAP == STM32Blue
    #error "Configuration: AXIS3 and AXIS4 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON && ROTATOR == ON
  #if PINMAP == Classic || PINMAP == MaxESP3 || PINMAP == MaxPCB || PINMAP == MaxPCB2 || PINMAP == MaxTMC || PINMAP == MiniPCB || PINMAP == MiniEMB || PINMAP == MiniPCB2
    #error "Configuration: AXIS3 and AXIS5 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

// focuser/rotators any invalid EN pin use?
#if ROTATOR == ON && AXIS3_DRIVER_POWER_DOWN == ON
  #if Axis3_EN == -1
    #error "Configuration: AXIS3_DRIVER_POWER_DOWN enabled but not supported with this PINMAP"
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration: AXIS3_DRIVER_POWER_DOWN early v1.x MaxPCB boards had the option to use Aux3 for EN control, this is no longer supported (now uses Aux5.)"
    #endif
    #ifndef USES_AUX5
      #warning "Configuration: AXIS3_DRIVER_POWER_DOWN using Aux5."
      #define USES_AUX5
    #else
      #error "Configuration: AXIS3_DRIVER_POWER_DOWN enabled but Aux5 is already in use, choose one feature on Aux5"
    #endif
  #endif
#endif
#if FOCUSER1 == ON && (AXIS4_DRIVER_POWER_DOWN == ON || AXIS4_DRIVER_DC_MODE != OFF)
  #if AXIS4_DRIVER_POWER_DOWN == ON && AXIS4_DRIVER_DC_MODE != OFF
    #error "Configuration: AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis4_EN == -1
    #error "Configuration: AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support which this PINMAP doesn't have"
  #endif
  #if PINMAP == MaxESP3
    #ifndef USES_AUX2
      #define USES_AUX2
    #else
      #error "Configuration: AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux2 is already in use, choose one feature on Aux2"
    #endif
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration: AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support, early v1.x MaxPCB boards had the option to use Aux4 for EN control but this is no longer supported."
    #endif
    #ifndef USES_AUX6
      #define USES_AUX6
    #else
      #error "Configuration: AXIS4_DRIVER_POWER_DOWN and AXIS4_DRIVER_DC_MODE require ENable signal support but Aux6 is already in use, choose one feature on Aux6"
    #endif
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_POWER_DOWN == ON
  #if AXIS5_DRIVER_POWER_DOWN == ON && AXIS5_DRIVER_DC_MODE != OFF
    #error "Configuration: AXIS5_DRIVER_POWER_DOWN and AXIS5_DRIVER_DC_MODE can't be used at the same time"
  #endif
  #if Axis5_EN == -1
    #error "Configuration: AXIS5_DRIVER_POWER_DOWN requires ENable signal support which this PINMAP doesn't have"
  #endif
  #if PINMAP == MaxPCB || PINMAP == MaxPCB2
    #if PINMAP == MaxPCB
      #warning "Configuration: AXIS5_DRIVER_POWER_DOWN early v1.x MaxPCB boards had the option to use Aux3 for EN control, this is no longer supported (now uses Aux5.)"
    #endif
    #ifndef USES_AUX5
      #define USES_AUX5
    #else
      #error "Configuration: AXIS5_DRIVER_POWER_DOWN enabled but Aux5 is already in use, choose one feature on Aux5"
    #endif
  #endif
#endif

// focuser/rotators any invalid focuser DC modes?
#if !(FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE != OFF)
  #if AXIS4_DRIVER_DC_MODE == XDIRECT && AXIS4_DRIVER_MODEL != TMC_SPI
    #error "Configuration: AXIS4_DRIVER_DC_MODE error XDIRECT is for TMC stepper drivers only."
  #endif
#endif
#if FOCUSER2 == ON && AXIS5_DRIVER_DC_MODE != OFF
  #if !(FOCUSER1 == ON && AXIS4_DRIVER_DC_MODE != OFF)
    #error "Configuration: AXIS5_DRIVER_DC_MODE requires enabling FOCUSER1 in AXIS4_DRIVER_DC_MODE.  In this mode FOCUSER2 USES the FOCUSER1 stepper driver"
  #endif
#endif

// PINMAP SPECIFIC VALIDATION ----------------------

#if PINMAP == MaxTMC || PINMAP ==MksGenL2
  // special checks for SPI buss pin maps to ensure all enabled stepper drivers are TMC2130 or TMC5160 in SPI mode
  #if AXIS1_DRIVER_MODEL != TMC_SPI
    #error "Configuration: AXIS1_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
  #endif
  #if AXIS2_DRIVER_MODEL != TMC_SPI
    #error "Configuration: AXIS2_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
  #endif
  #if ROTATOR == ON
    #if AXIS3_DRIVER_MODEL != TMC_SPI
      #error "Configuration: AXIS3_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
  #if FOCUSER1 == ON
    #if AXIS4_DRIVER_MODEL != TMC_SPI
      #error "Configuration: AXIS4_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
  #if FOCUSER2 == ON
    #if AXIS5_DRIVER_MODEL != TMC_SPI
      #error "Configuration: AXIS5_DRIVER_MODEL must be a TMC2130 or TMC5160 for this PINMAP"
    #endif
  #endif
#else
  // and when not MaxTMC to ensure all focuser/rotator stepper drivers off (manual configuration only.)
  #if ROTATOR == ON && AXIS3_DRIVER_MODEL != OFF
    #error "Configuration: AXIS3_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
  #if FOCUSER1 == ON && AXIS4_DRIVER_MODEL != OFF
    #error "Configuration: AXIS4_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
  #if FOCUSER2 == ON && AXIS5_DRIVER_MODEL != OFF
    #error "Configuration: AXIS5_DRIVER_MODEL must be OFF for this PINMAP"
  #endif
#endif

// GENERAL PURPOSE VALIDATION ----------------------

#if SERIAL_B_ESP_FLASHING == ON  && (!defined(ESP8266Gpio0Pin) || !defined(ESP8266RstPin))
  #error "Configuration: SERIAL_B_ESP_FLASHING not supported for this PINMAP"
#endif

#if LED_STATUS_PIN != OFF && LEDnegPin == -1
  #error "Configuration: LED_STATUS_PIN not supported for this PINMAP, must be OFF"
#endif

#if LED_STATUS2_PIN != OFF && LEDneg2Pin == -1
  #error "Configuration: LED_STATUS2_PIN not supported for this PINMAP, must be OFF"
#endif

#if LED_RETICLE_PIN != OFF && ReticlePin == -1
  #error "Configuration: LED_RETICLE_PIN not supported for this PINMAP, must be OFF"
#endif

#if PEC_SENSE > 0 && AnalogPecPin == -1
  #error "Configuration: PEC_SENSE in ANALOG mode not supported for this PINMAP, use ON, etc. NOT a threshold value"
#endif
