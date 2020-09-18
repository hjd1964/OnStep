// -----------------------------------------------------------------------------------
// Global variables 

#pragma once

// Time keeping --------------------------------------------------------------------------------------------------------------------
long siderealTimer                      = 0;                 // counter to issue steps during tracking
long pecSiderealTimer                   = 0;                 // time since worm wheel zero index for PEC
long guideSiderealTimer                 = 0;                 // counter to issue steps during guiding
bool dateWasSet                         = false;             // keep track of date/time validity
bool timeWasSet                         = false;                          
                                                                          
double UT1                              = 0.0;               // the current universal time
double UT1_start                        = 0.0;               // the start of UT1
double JD                               = 0.0;               // and date, used for computing LST
double LMT                              = 0.0;
double timeZone                         = 0.0;
                                                                          
long lst_start                          = 0;                 // marks the start lst when UT1 is set 
volatile long lst                       = 0;                 // local (apparent) sidereal time in 0.01 second ticks,
                                                             // takes 249 days to roll over.
                                                             // 1.00273 wall clock seconds per sidereal second
                                                                          
const long masterSiderealInterval      = 15956313L;                      
long siderealInterval                  = masterSiderealInterval;               
                                                             // default = 15956313 ticks per sidereal second, where a tick
                                                             // is 1/16 uS this is stored in EEPROM which is updated/adjusted
                                                             // with the ":T+#" and ":T-#" commands a higher number here means
                                                             // a longer count which slows down the sidereal clock
                                                                          
const double HzCf                       = 16000000.0/60.0;   // conversion factor to go to/from Hz for sidereal interval
                                                                          
volatile long siderealRate;                                  // based on the siderealInterval, time between steps sidereal tracking
volatile long backlashTakeupRate;                            // backlash takeup rate
                                                                          
unsigned long last_loop_micros          = 0;                 // workload monitoring
long this_loop_time                     = 0;
long loop_time                          = 0;
long worst_loop_time                    = 0;
long average_loop_time                  = 0;

// PPS (GPS) -----------------------------------------------------------------------------------------------------------------------
volatile unsigned long ppsLastMicroS    = 1000000UL;
volatile unsigned long ppsAvgMicroS     = 1000000UL;
volatile double ppsRateRatio            = 1.0;
volatile double ppsLastRateRatio        = 1.0;
volatile bool ppsSynced              = false;

// Tracking and rate control -------------------------------------------------------------------------------------------------------
#if MOUNT_TYPE != ALTAZM
  enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
  #if TRACK_REFRACTION_RATE_DEFAULT == ON
    RateCompensation rateCompensation   = RC_REFR_RA;
  #else
    RateCompensation rateCompensation   = RC_NONE;
  #endif
#else
  enum RateCompensation {RC_NONE};
  RateCompensation rateCompensation     = RC_NONE;
#endif

double slewSpeed                        = 0;
volatile long timerRateAxis1            = 0;
volatile long timerRateBacklashAxis1    = 0;
volatile bool inbacklashAxis1        = false;
bool haltAxis1                       = false;
bool faultAxis1                      = false;
volatile long timerRateAxis2            = 0;
volatile long timerRateBacklashAxis2    = 0;
volatile bool inbacklashAxis2        = false;
bool haltAxis2                       = false;
bool faultAxis2                      = false;

#if AXIS1_DRIVER_MODEL == TMC_SPI
  #define AXIS1_DRIVER_SWITCH_RATE 150*16L
#else
  #define AXIS1_DRIVER_SWITCH_RATE 80*16L
#endif
#if AXIS2_DRIVER_MODEL == TMC_SPI
  #define AXIS2_DRIVER_SWITCH_RATE 150*16L
#else
  #define AXIS2_DRIVER_SWITCH_RATE 80*16L
#endif

#pragma pack(1)
typedef struct AxisSettings {
   double stepsPerMeasure;                                   // degrees for axis1-3 or microns for axis4-5
   int16_t microsteps;
   int16_t IRUN;
   int8_t reverse;
   int16_t min;
   int16_t max;
} axisSettings;
#pragma pack()
volatile axisSettings axis1Settings     = {AXIS1_STEPS_PER_DEGREE, AXIS1_DRIVER_MICROSTEPS, AXIS1_DRIVER_IRUN, AXIS1_DRIVER_REVERSE, AXIS1_LIMIT_MIN, AXIS1_LIMIT_MAX};
volatile axisSettings axis2Settings     = {AXIS2_STEPS_PER_DEGREE, AXIS2_DRIVER_MICROSTEPS, AXIS2_DRIVER_IRUN, AXIS2_DRIVER_REVERSE, AXIS2_LIMIT_MIN, AXIS2_LIMIT_MAX};
volatile axisSettings axis3Settings     = {AXIS3_STEPS_PER_DEGREE, AXIS3_DRIVER_MICROSTEPS, AXIS3_DRIVER_IRUN, AXIS3_DRIVER_REVERSE, AXIS3_LIMIT_MIN, AXIS3_LIMIT_MAX};
volatile axisSettings axis4Settings     = {AXIS4_STEPS_PER_MICRON, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_REVERSE, AXIS4_LIMIT_MIN, AXIS4_LIMIT_MAX};
volatile axisSettings axis5Settings     = {AXIS5_STEPS_PER_MICRON, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_REVERSE, AXIS5_LIMIT_MIN, AXIS5_LIMIT_MAX};

typedef struct AxisSettingsEx {
   int16_t IHOLD;
   int16_t IGOTO;
} axisSettingsEx;
axisSettingsEx axis1SettingsEx          = {AXIS1_DRIVER_IHOLD, AXIS1_DRIVER_IGOTO};
axisSettingsEx axis2SettingsEx          = {AXIS2_DRIVER_IHOLD, AXIS2_DRIVER_IGOTO};
axisSettingsEx axis3SettingsEx          = {AXIS3_DRIVER_IHOLD, OFF};
axisSettingsEx axis4SettingsEx          = {AXIS4_DRIVER_IHOLD, OFF};
axisSettingsEx axis5SettingsEx          = {AXIS5_DRIVER_IHOLD, OFF};

unsigned long axis1StepsGoto            = 1;
unsigned long axis2StepsGoto            = 1;

#define stepsPerSecondAxis1               (axis1Settings.stepsPerMeasure/240.0)
#define arcSecPerStepAxis1                (3600.0/axis1Settings.stepsPerMeasure)
#define stepsPerSecondAxis2               (axis2Settings.stepsPerMeasure/240.0)
#define arcSecPerStepAxis2                (3600.0/axis2Settings.stepsPerMeasure)
#define DefaultTrackingRate               1
volatile double trackingTimerRateAxis1  = DefaultTrackingRate;
volatile double trackingTimerRateAxis2  = DefaultTrackingRate;
volatile double timerRateRatio;
volatile bool useTimerRateRatio;
long stepsPerWormRotationAxis1;
long secondsPerWormRotationAxis1;
long maxRate;
#define maxRateBaseDesired                ((1000000.0/(SLEW_RATE_BASE_DESIRED))/axis1Settings.stepsPerMeasure)
double maxRateBaseActual;
volatile double stepsForRateChangeAxis1;
volatile double stepsForRateChangeAxis2;

// Basic stepper driver mode setup -------------------------------------------------------------------------------------------------
#if AXIS1_DRIVER_MODEL != OFF
  // Holds translated code for driver microstep setting
  volatile uint8_t _axis1_code;
  #define AXIS1_DRIVER_CODE _axis1_code
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    volatile uint8_t _axis1_code_goto;
    #define AXIS1_DRIVER_CODE_GOTO _axis1_code_goto
  #endif
#endif
#if AXIS2_DRIVER_MODEL != OFF
  volatile uint8_t _axis2_code;
  #define AXIS2_DRIVER_CODE _axis2_code
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    volatile uint8_t _axis2_code_goto;
    #define AXIS2_DRIVER_CODE_GOTO _axis2_code_goto
  #endif
#endif
#if AXIS3_DRIVER_MODEL != OFF
  volatile uint8_t _axis3_code;
  #define AXIS3_DRIVER_CODE _axis3_code
#endif
#if AXIS4_DRIVER_MODEL != OFF
  volatile uint8_t _axis4_code;
  #define AXIS4_DRIVER_CODE _axis4_code
#endif
#if AXIS5_DRIVER_MODEL != OFF
  volatile uint8_t _axis5_code;
  #define AXIS5_DRIVER_CODE _axis5_code
#endif

// Location ------------------------------------------------------------------------------------------------------------------------
double latitude                         = 0.0;
double latitudeAbs                      = 0.0;
double latitudeSign                     = 1.0;
double cosLat                           = 1.0;
double sinLat                           = 0.0;
double longitude                        = 0.0;

// Coordinates ---------------------------------------------------------------------------------------------------------------------
#ifndef TELESCOPE_COORDINATES
  #define TELESCOPE_COORDINATES TOPOCENTRIC
#endif
#if MOUNT_TYPE == GEM
  double homePositionAxis1              = 90.0;
#else
  double homePositionAxis1              = 0.0;
#endif
volatile long posAxis1                  = 0;                 // hour angle position in steps
volatile int blAxis1                    = 0;                 // backlash position in steps
volatile int backlashAxis1              = 0;                 // total backlash in steps
volatile long startAxis1                = 0;                 // hour angle of goto start position in steps
volatile fixed_t targetAxis1;                                // hour angle of goto end   position in steps
double origTargetRA                     = 0.0;               // holds the RA for gotos before possible conversion to observed place
double newTargetRA                      = 0.0;               // holds the RA for gotos after conversion to observed place
double newTargetAzm                     = 0.0;               // holds the altitude and azmiuth for slews
fixed_t origTargetAxis1;
double indexAxis1                       = 0;                 // index offset corrections, simple align/sync
long   indexAxis1Steps                  = 0;
volatile long stepAxis1=1;
fixed_t fstepAxis1;                                          // tracking and PEC, fractional steps

double homePositionAxis2                = 0.0;
volatile long posAxis2                  = 0;                 // declination position in steps
volatile int blAxis2                    = 0;                 // backlash position in steps
volatile int backlashAxis2              = 0;                 // total backlash in steps
volatile long startAxis2                = 0;                 // declination of goto start position in steps
volatile fixed_t targetAxis2;                                // declination of goto end   position in steps
double origTargetDec                    = 0.0;               // holds the Dec for gotos before possible conversion to observed place
double newTargetDec                     = 0.0;               // holds the Dec for gotos after conversion to observed place
double newTargetAlt                     = 0.0;               // holds the altitude and azmiuth for slews
fixed_t origTargetAxis2;
double indexAxis2                       = 0;                 // index offset corrections, simple align/sync
long   indexAxis2Steps                  = 0;
volatile long stepAxis2=1;
fixed_t fstepAxis2;                                          // tracking and PEC, fractional steps

double currentAlt                       = 45.0;              // the current altitude
double currentDec                       = 0.0;               // the current declination

// Limits --------------------------------------------------------------------------------------------------------------------------
long   degreesPastMeridianE             = 15;                // east of pier.  How far past the meridian before we do a flip.
long   degreesPastMeridianW             = 15;                // west of pier.  Mount stops tracking when it hits the this limit.
int    minAlt;                                               // the min altitude, in deg, so we don't try to point too low
int    maxAlt;                                               // the max altitude, in deg, keeps telescope away from mount/tripod

// Stepper driver enable/disable and direction -------------------------------------------------------------------------------------
#define DefaultDirAxis1NCPInit            0
#define DefaultDirAxis1SCPInit            1
volatile byte dirAxis1                  = 1;                 // stepping direction + or -
volatile byte defaultDirAxis1           = DefaultDirAxis1NCPInit;

#define DefaultDirAxis2EInit              1
#define DefaultDirAxis2WInit              0
volatile byte dirAxis2                  = 1;                 // stepping direction + or -
volatile byte defaultDirAxis2           = DefaultDirAxis2EInit;

// Status --------------------------------------------------------------------------------------------------------------------------
// Note: the following error codes are obsolete ERR_SYNC, ERR_PARK
enum GeneralErrors {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};
GeneralErrors generalError = ERR_NONE;

enum CommandErrors {
  CE_NONE, CE_0, CE_CMD_UNKNOWN, CE_REPLY_UNKNOWN, CE_PARAM_RANGE, CE_PARAM_FORM,
  CE_ALIGN_FAIL, CE_ALIGN_NOT_ACTIVE, CE_NOT_PARKED_OR_AT_HOME, CE_PARKED,
  CE_PARK_FAILED, CE_NOT_PARKED, CE_NO_PARK_POSITION_SET, CE_GOTO_FAIL, CE_LIBRARY_FULL,
  CE_GOTO_ERR_BELOW_HORIZON, CE_GOTO_ERR_ABOVE_OVERHEAD, CE_SLEW_ERR_IN_STANDBY, 
  CE_SLEW_ERR_IN_PARK, CE_GOTO_ERR_GOTO, CE_SLEW_ERR_OUTSIDE_LIMITS, CE_SLEW_ERR_HARDWARE_FAULT,
  CE_MOUNT_IN_MOTION, CE_GOTO_ERR_UNSPECIFIED, CE_NULL};
CommandErrors commandError = CE_NONE;

#if DEBUG != OFF
  // command errors
  #define L_CE_NONE "No Errors"
  #define L_CE_0 "reply 0"
  #define L_CE_CMD_UNKNOWN "command unknown"
  #define L_CE_REPLY_UNKNOWN "invalid reply"
  #define L_CE_PARAM_RANGE "parameter out of range"
  #define L_CE_PARAM_FORM "bad parameter format"
  #define L_CE_ALIGN_FAIL "align failed"
  #define L_CE_ALIGN_NOT_ACTIVE "align not active"
  #define L_CE_NOT_PARKED_OR_AT_HOME "not parked or at home"
  #define L_CE_PARKED "already parked"
  #define L_CE_PARK_FAILED "park failed"
  #define L_CE_NOT_PARKED "not parked"
  #define L_CE_NO_PARK_POSITION_SET "no park position set"
  #define L_CE_GOTO_FAIL "goto failed"
  #define L_CE_LIBRARY_FULL "library full"
  #define L_CE_GOTO_ERR_BELOW_HORIZON "goto below horizon"
  #define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto above overhead"
  #define L_CE_SLEW_ERR_IN_STANDBY "slew in standby"
  #define L_CE_SLEW_ERR_IN_PARK "slew in park"
  #define L_CE_GOTO_ERR_GOTO "already in goto"
  #define L_CE_SLEW_ERR_OUTSIDE_LIMITS "outside limits"
  #define L_CE_SLEW_ERR_HARDWARE_FAULT "hardware fault"
  #define L_CE_MOUNT_IN_MOTION "mount in motion"
  #define L_CE_GOTO_ERR_UNSPECIFIED "other"
  #define L_CE_UNK "unknown"
  
  char commandErrorStr[30][25] = {
    L_CE_NONE, L_CE_0, L_CE_CMD_UNKNOWN, L_CE_REPLY_UNKNOWN, L_CE_PARAM_RANGE,
    L_CE_PARAM_FORM, L_CE_ALIGN_FAIL, L_CE_ALIGN_NOT_ACTIVE, L_CE_NOT_PARKED_OR_AT_HOME,
    L_CE_PARKED, L_CE_PARK_FAILED, L_CE_NOT_PARKED, L_CE_NO_PARK_POSITION_SET, L_CE_GOTO_FAIL,
    L_CE_LIBRARY_FULL, L_CE_GOTO_ERR_BELOW_HORIZON, L_CE_GOTO_ERR_ABOVE_OVERHEAD,
    L_CE_SLEW_ERR_IN_STANDBY, L_CE_SLEW_ERR_IN_PARK, L_CE_GOTO_ERR_GOTO, L_CE_SLEW_ERR_OUTSIDE_LIMITS,
    L_CE_SLEW_ERR_HARDWARE_FAULT, L_CE_MOUNT_IN_MOTION, L_CE_GOTO_ERR_UNSPECIFIED, L_CE_UNK};
#else
  char commandErrorStr[0][0];
#endif
  
enum PrecisionMode {PM_LOW, PM_HIGH, PM_HIGHEST};
PrecisionMode precision = PM_HIGH;

// Tracking modes ------------------------------------------------------------------------------------------------------------------
#define TrackingNone                      0
#define TrackingSidereal                  1
#define TrackingMoveTo                    2
#define StartAbortGoto                    1
volatile byte trackingState             = TrackingNone;
byte abortTrackingState                 = TrackingNone;
volatile byte lastTrackingState         = TrackingNone;
int trackingSyncSeconds                 = 0;
byte abortGoto                          = 0;
volatile bool safetyLimitsOn         = true;
bool axis1Enabled                    = false;
bool axis2Enabled                    = false;
bool syncToEncodersOnly              = false;
enum StopSlewActions {SS_ALL_FAST, SS_LIMIT, SS_LIMIT_HARD, SS_LIMIT_AXIS1_MIN, SS_LIMIT_AXIS1_MAX, SS_LIMIT_AXIS2_MIN, SS_LIMIT_AXIS2_MAX};
                                        
// Meridian flips ------------------------------------------------------------------------------------------------------------------
#define MeridianFlipNever                 0
#define MeridianFlipAlign                 1
#define MeridianFlipAlways                2
#if MOUNT_TYPE == GEM
  byte meridianFlip = MeridianFlipAlways;
#endif
#if MOUNT_TYPE == FORK
  byte meridianFlip = MeridianFlipNever;
#endif
#if MOUNT_TYPE == ALTAZM
  byte meridianFlip = MeridianFlipNever;
#endif

byte pierSideControl = PierSideNone;
int preferredPierSide = PIER_SIDE_PREFERRED_DEFAULT;
bool autoMeridianFlip                   = false;             // auto meridian flip/continue as tracking hits AXIS1_LIMIT_MERIDIAN_W
bool pauseHome                          = false;             // allow pause at home?
bool waitingHomeContinue                = false;             // set to true to stop pause
bool waitingHome                        = false;             // true if waiting at home

// Parking -------------------------------------------------------------------------------------------------------------------------
#define PCB_BUSY                         -1
#define PCB_FAILURE                       0
#define PCB_SUCCESS                       1

#define PARK_STATUS_FIRST                 0
#define NotParked                         0
#define Parking                           1
#define Parked                            2
#define ParkFailed                        3
#define ParkUnknown                       4
#define PARK_STATUS_LAST                  4
byte    parkStatus                      = NotParked;
bool parkSaved                          = false;

// Homing --------------------------------------------------------------------------------------------------------------------------
bool atHome                             = true;
bool homeMount                          = false;

// Command processing --------------------------------------------------------------------------------------------------------------
#define BAUD 9600
// serial speed
unsigned long baudRate[10] = {115200,56700,38400,28800,19200,14400,9600,4800,2400,1200};

// Guiding and slewing -------------------------------------------------------------------------------------------------------------
#define RateToDegPerSec                   (1000000.0/axis1Settings.stepsPerMeasure)
#define RateToASPerSec                    (RateToDegPerSec*3600.0)
#define RateToXPerSec                     (RateToASPerSec/15.0)
double  slewRateX;
double  accXPerSec;
double  guideRates[10]={3.75,7.5,15,30,60,120,300,720,         720,    720};
//                      .25X .5x 1x 2x 4x  8x 20x 48x half-MaxRate MaxRate
//                         0   1  2  3  4   5   6   7            8       9

#define GuideRate1x                       2
#ifndef GuideRateDefault
  #define GuideRateDefault                6
#endif
#define GuideRateNone                     255
byte currentGuideRate                   = GuideRateDefault;
byte currentPulseGuideRate              = GuideRate1x;
volatile byte activeGuideRate           = GuideRateNone;
                                        
volatile byte guideDirAxis1             = 0;
char          ST4DirAxis1               = 'b';
volatile byte guideDirAxis2             = 0;
char          ST4DirAxis2               = 'b';
int           spiralGuide               = 0;

volatile double guideTimerRateAxis1     = 0.0;
volatile double guideTimerRateAxis2     = 0.0;
volatile double guideTimerBaseRateAxis1 = 0.0;
volatile double guideTimerBaseRateAxis2 = 0.0;
fixed_t amountGuideAxis1;
fixed_t guideAxis1;
fixed_t amountGuideAxis2;
fixed_t guideAxis2;

// PEC control ---------------------------------------------------------------------------------------------------------------------
#define PEC_STATUS_FIRST                  0
#define IgnorePEC                         0
#define ReadyPlayPEC                      1
#define PlayPEC                           2
#define ReadyRecordPEC                    3
#define RecordPEC                         4
#define PEC_STATUS_LAST                   4

#define PECStatusString                   "IpPrR"
#define PECStatusStringAlt                "/,~;^"

byte    pecStatus                       = IgnorePEC;
bool pecRecorded                        = false;
bool pecFirstRecord                     = false;
long    lastPecIndex                    = -1;
int     pecBufferSize                   = 0;
long    pecIndex                        = 0;
long    pecIndex1                       = 0;
#if PEC_SENSE == ON || PEC_SENSE == ON_PULLUP || PEC_SENSE == ON_PULLDOWN
  int   pecValue                        = PEC_SENSE_STATE;
#elif PEC_SENSE_STATE == HIGH
  int   pecValue                        = 1023;
#else
  int   pecValue                        = 0;
#endif
int     pecAutoRecord                   = 0;                 // for writing to PEC table to EEPROM
long    wormSensePos                    = 0;                 // in steps
bool wormSensedAgain                    = false;             // indicates PEC index was found
bool pecBufferStart                     = false;                                   
fixed_t accPecGuideHA;                                       // for PEC, buffers steps to be recorded
volatile double pecTimerRateAxis1 = 0.0;
#if AXIS1_PEC == ON
  static byte *pecBuffer;
#endif

// Misc ----------------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

// current site index and name
byte currentSite = 0; 
char siteName[16];

// status state
bool ledOn                              = false;
bool led2On                             = false;

// sound/buzzer
#if BUZZER_STATE_DEFAULT == ON
  bool soundEnabled                     = true;
#else                                   
  bool soundEnabled                     = false;
#endif
volatile int buzzerDuration = 0;

// reticule control
#if LED_RETICLE >= 0
  int reticuleBrightness=LED_RETICLE;
#endif

// aux pin control
#ifdef FEATURES_PRESENT
typedef struct Features {
   const char* name;
   const int purpose;
   const int64_t temp;
   const int64_t pin;
   int value;
   dewHeaterControl *dewHeater;
   intervalometerControl *intervalometer;
} features;

features feature[8] = {
  {FEATURE1_NAME,FEATURE1_PURPOSE,FEATURE1_TEMP,FEATURE1_PIN,0,NULL,NULL},
  {FEATURE2_NAME,FEATURE2_PURPOSE,FEATURE2_TEMP,FEATURE2_PIN,0,NULL,NULL},
  {FEATURE3_NAME,FEATURE3_PURPOSE,FEATURE3_TEMP,FEATURE3_PIN,0,NULL,NULL},
  {FEATURE4_NAME,FEATURE4_PURPOSE,FEATURE4_TEMP,FEATURE4_PIN,0,NULL,NULL},
  {FEATURE5_NAME,FEATURE5_PURPOSE,FEATURE5_TEMP,FEATURE5_PIN,0,NULL,NULL},
  {FEATURE6_NAME,FEATURE6_PURPOSE,FEATURE6_TEMP,FEATURE6_PIN,0,NULL,NULL},
  {FEATURE7_NAME,FEATURE7_PURPOSE,FEATURE7_TEMP,FEATURE7_PIN,0,NULL,NULL},
  {FEATURE8_NAME,FEATURE8_PURPOSE,FEATURE8_TEMP,FEATURE8_PIN,0,NULL,NULL}
};
#endif
