// === CONFIGURATION START
// This configuration was generated using OnStepConfigGen.py
// On: 2018-01-06 15:43
#define STM32_ON
#ifdef  STM32_ON
#define MOUNT_TYPE_GEM
#define StepsPerDegreeAxis1        20480.0
#define StepsPerWormRotationAxis1  51200.0
#define AXIS1_DRIVER_MODEL         LV8729
#define AXIS1_MICROSTEPS           64
#define AXIS1_MICROSTEPS_GOTO      2
#define AXIS1_STEP_GOTO            2

#define StepsPerDegreeAxis2        20480.0
#define StepsPerWormRotationAxis2  51200.0
#define AXIS2_DRIVER_MODEL         LV8729
#define AXIS2_MICROSTEPS           64
#define AXIS2_MICROSTEPS_GOTO      2
#define AXIS2_STEP_GOTO            2

#define MaxRate                    16
#define PECBufferSize              600

#define MODE_SWITCH_BEFORE_SLEW_ON
#define ALIGN_GOTOASSIST_ON
#define TRACK_REFRACTION_RATE_DEFAULT_ON
#define REMEMBER_MAX_RATE_ON

// The following are less common options. Edit them by hand for now.
#define AXIS1_FAULT_OFF
#define AXIS2_FAULT_OFF
#define SERIAL1_BAUD_DEFAULT 9600
#define SERIAL4_BAUD_DEFAULT 9600
#define ESP8266_CONTROL_OFF
#define STRICT_PARKING_OFF
#define ST4_OFF
#define ST4_HAND_CONTROL_ON
#define SEPARATE_PULSE_GUIDE_RATE_ON
#define GUIDE_TIME_LIMIT 0
#define RTC_OFF
#define PPS_SENSE_OFF
#define PEC_SENSE_OFF
#define PEC_SENSE_STATE HIGH
#define LIMIT_SENSE_OFF
#define STATUS_LED_PINS_ON
#define STATUS_LED2_PINS_OFF
#define RETICULE_LED_PINS_OFF
#define BUZZER_OFF
#define DEFAULT_SOUND_ON
#define REMEMBER_AUTO_MERIDIAN_FLIP_OFF
#define REMEMBER_PAUSE_HOME_OFF
#define REVERSE_AXIS1_OFF
#define REVERSE_AXIS2_OFF
#define DegreesForAcceleration   5.0
#define DegreesForRapidStop      1.0
#define BacklashTakeupRate        25
#define MinutesPastMeridianE      60
#define MinutesPastMeridianW      60
#define UnderPoleLimit            12
#define MinDec                   -91
#define MaxDec                   +91
#define MaxAzm                   180
#define AXIS1_DISABLED_HIGH
#define AXIS2_DISABLED_HIGH
#define AUTO_POWER_DOWN_AXIS2_OFF
#define MODE_SWITCH_SLEEP_OFF
// Include the pins file
#include "Pins.STM32.h"
#endif
// === CONFIGURATION END
