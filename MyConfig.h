
// Undefine the variables that we will use
#undef  PINMAP
#undef  ST4_INTERFACE
#undef  ST4_HAND_CONTROL
#undef  SLEW_RATE_BASE_DESIRED
#undef  TRACK_REFRACTION_RATE_DEFAULT
#undef  AXIS1_STEPS_PER_DEGREE
#undef  AXIS1_STEPS_PER_WORMROT
#undef  AXIS1_DRIVER_MODEL
#undef  AXIS1_DRIVER_MICROSTEPS
#undef  AXIS1_DRIVER_MICROSTEPS_GOTO
#undef  AXIS1_DRIVER_REVERSE
#undef  AXIS2_STEPS_PER_DEGREE
#undef  AXIS2_DRIVER_MODEL
#undef  AXIS2_DRIVER_MICROSTEPS
#undef  AXIS2_DRIVER_MICROSTEPS_GOTO
#undef  AXIS2_DRIVER_REVERSE
#undef  GUIDE_TIME_LIMIT
#undef  PEC_SENSE
#undef  PEC_SENSE_STATE
#undef  BUZZER
#undef  BUZZER_STATE_DEFAULT

// Now define our new ones
#define PINMAP                  FYSETC_S6

#define ST4_INTERFACE                  OFF
#define ST4_HAND_CONTROL               OFF

#define SLEW_RATE_BASE_DESIRED        3.5

#define TRACK_REFRACTION_RATE_DEFAULT  ON

#define AXIS1_STEPS_PER_DEGREE    28800.0
#define AXIS1_STEPS_PER_WORMROT     57600
#define AXIS1_DRIVER_MODEL         TMC5160
#define AXIS1_DRIVER_MICROSTEPS        16
#define AXIS1_DRIVER_MICROSTEPS_GOTO    4
#define AXIS1_DRIVER_REVERSE           ON

#define AXIS2_STEPS_PER_DEGREE    28800.0
#define AXIS2_DRIVER_MODEL         TMC5160
#define AXIS2_DRIVER_MICROSTEPS        16
#define AXIS2_DRIVER_MICROSTEPS_GOTO    4
#define AXIS2_DRIVER_REVERSE           ON

#define GUIDE_TIME_LIMIT               10

#define PEC_SENSE                      ON
#define PEC_SENSE_STATE               LOW

#define BUZZER                         ON
#define BUZZER_STATE_DEFAULT           ON

//#define WEATHER                        BME280_0x76
