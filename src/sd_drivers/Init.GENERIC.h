// SERVO driver init

#if AXIS1_DRIVER_MODEL == GENERIC4
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL GENERIC2
  #define Axis1StepPinStep Axis1StepPinLOW
  #define Axis1StepPinReset Axis1StepPinHIGH
#endif
#if AXIS2_DRIVER_MODEL == GENERIC4
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL GENERIC2
  #define Axis2StepPinStep Axis2StepPinLOW
  #define Axis2StepPinReset Axis2StepPinHIGH
#endif
#if AXIS1_DRIVER_MODEL == GENERIC3
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL GENERIC
  #define Axis1StepPinStep Axis1StepPinLOW
  #define Axis1StepPinReset Axis1StepPinHIGH
#endif
#if AXIS2_DRIVER_MODEL == GENERIC3
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL GENERIC
  #define Axis2StepPinStep Axis2StepPinLOW
  #define Axis2StepPinReset Axis2StepPinHIGH
#endif
#if AXIS1_DRIVER_MODEL == GENERIC2
  #undef AXIS1_DRIVER_MODEL
  #define AXIS1_DRIVER_MODEL GENERIC
  #undef AXIS1_DRIVER_ENABLE
  #define AXIS1_DRIVER_ENABLE HIGH
#endif
#if AXIS2_DRIVER_MODEL == GENERIC2
  #undef AXIS2_DRIVER_MODEL
  #define AXIS2_DRIVER_MODEL GENERIC
  #undef AXIS2_DRIVER_ENABLE
  #define AXIS2_DRIVER_ENABLE HIGH
#endif
