#define STM32_OFF
#ifdef STM32_ON
#define MOUNT_TYPE_GEM
#define StepsPerDegreeAxis1 9600.0
#define StepsPerWormRotationAxis1 24000.0
#define AXIS1_DRIVER_MODEL LV8729
#define AXIS1_MICROSTEPS 32
#define AXIS1_MICROSTEPS_GOTO 1

#define StepsPerDegreeAxis2 9600.0
#define StepsPerWormRotationAxis2 24000.0
#define AXIS2_DRIVER_MODEL LV8729
#define AXIS2_MICROSTEPS 32
#define AXIS2_MICROSTEPS_GOTO 1

#define MaxRate 20
#define PECBufferSize 600

#define FileVersionConfig 1
#define AUTOSTART_TRACKING_ON
// The following are useful, so defaulted to ON
#define TRACK_REFRACTION_RATE_DEFAULT_ON
#define REMEMBER_MAX_RATE_ON

// The following are less common options. Edit them by manually.
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
#define REMEMBER_AUTO_MERIDIAN_FLIP_ON
#define REMEMBER_PAUSE_HOME_ON
#define REVERSE_AXIS1_OFF
#define REVERSE_AXIS2_OFF
#define DegreesForAcceleration 5.0
#define DegreesForRapidStop 1.0
#define BacklashTakeupRate 25
#define MinutesPastMeridianE 60
#define MinutesPastMeridianW 60
#define UnderPoleLimit 12
#define MinDec -91
#define MaxDec +91
#define MaxAzm 180
#define AXIS1_DISABLED_HIGH
#define AXIS2_DISABLED_HIGH
#define AUTO_POWER_DOWN_AXIS2_OFF
#define AXIS1_FAULT_OFF
#define AXIS2_FAULT_OFF
#define ROTATOR_OFF
#define MaxRateAxis3 8
#define StepsPerDegreeAxis3 64.0
#define REVERSE_AXIS3_OFF
#define DISABLE_AXIS3_OFF
#define MinAxis3 -180
#define MaxAxis3 180
#define FOCUSER1_OFF
#define MaxRateAxis4 8
#define StepsPerMicrometerAxis4 0.5
#define REVERSE_AXIS4_OFF
#define DISABLE_AXIS4_OFF
#define MinAxis4 -25.0
#define MaxAxis4 25.0
#define FOCUSER2_OFF
#define MaxRateAxis5 8
#define StepsPerMicrometerAxis5 0.5
#define REVERSE_AXIS5_OFF
#define DISABLE_AXIS5_OFF
#define MinAxis5 -25.0
#define MaxAxis5 25.0
// Include the pins file
#include "Pins.STM32.h"
#endif
