// -----------------------------------------------------------------------------------
// Global variables 

#pragma once

// Time keeping ------------------------------------------------------------------------------------------------------------
long siderealTimer    = 0;           // counter to issue steps during tracking
long PecSiderealTimer = 0;           // time since worm wheel zero index for PEC
long guideSiderealTimer=0;           // counter to issue steps during guiding

double UT1       = 0.0;              // the current universal time
double UT1_start = 0.0;              // the start of UT1
double JD  = 0.0;                    // and date, used for computing LST
double LMT = 0.0;                    //
double timeZone = 0.0;               //

long   lst_start = 0;                // this marks the start lst when UT1 is set 
volatile long lst = 0;               // this is the local (apparent) sidereal time in 1/100 seconds (23h 56m 4.1s per day = 86400 clock seconds/
                                     // 86164.09 sidereal seconds = 1.00273 clock seconds per sidereal second.)  Takes 249 days to roll over.

long siderealInterval       = 15956313L;
long masterSiderealInterval = siderealInterval;
                                     // default = 15956313 ticks per sidereal second, where a tick is 1/16 uS
                                     // this is stored in EEPROM which is updated/adjusted with the ":T+#" and ":T-#" commands
                                     // a higher number here means a longer count which slows down the sidereal clock

double HzCf = 16000000.0/60.0;       // conversion factor to go to/from Hz for sidereal interval

volatile long SiderealRate;          // based on the siderealInterval, this is the time between steps for sidereal tracking
volatile long TakeupRate;            // this is the takeup rate for synchronizing the target and actual positions when needed

long last_loop_micros=0;             // workload monitoring
long this_loop_time=0;
long loop_time=0;
long worst_loop_time=0;

// PPS (GPS) ---------------------------------------------------------------------------------------------------------------
volatile unsigned long PPSlastMicroS = 1000000UL;
volatile unsigned long PPSavgMicroS = 1000000UL;
volatile double PPSrateRatio = 1.0;
volatile double LastPPSrateRatio = 1.0;
volatile boolean PPSsynced = false;

// Tracking and rate control -----------------------------------------------------------------------------------------------
#ifndef MOUNT_TYPE_ALTAZM
  enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
  #ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
    RateCompensation rateCompensation=RC_REFR_RA;
  #else
    RateCompensation rateCompensation=RC_NONE;
  #endif
#else
  enum RateCompensation {RC_NONE};
  RateCompensation rateCompensation=RC_NONE;
#endif

long    maxRate = MaxRate*16L;

double  slewSpeed = 0;
volatile long    timerRateAxis1 = 0;
volatile long    timerRateBacklashAxis1 = 0;
volatile boolean inbacklashAxis1 = false;
boolean faultAxis1 = false;
volatile long    timerRateAxis2 = 0;
volatile long    timerRateBacklashAxis2 = 0;
volatile boolean inbacklashAxis2 = false;
boolean faultAxis2 = false;

#define default_tracking_rate 1
volatile double  trackingTimerRateAxis1= default_tracking_rate;
volatile double  trackingTimerRateAxis2= default_tracking_rate;
volatile double  timerRateRatio        = ((double)StepsPerDegreeAxis1/(double)StepsPerDegreeAxis2);
volatile boolean useTimerRateRatio     = (StepsPerDegreeAxis1!=StepsPerDegreeAxis2);
#define StepsPerSecondAxis1              ((double)StepsPerDegreeAxis1/240.0)
#define ArcSecPerStepAxis1               (3600.0/StepsPerDegreeAxis1)
#define StepsPerSecondAxis2              ((double)StepsPerDegreeAxis2/240.0)
#define ArcSecPerStepAxis2               (3600.0/StepsPerDegreeAxis2)
#define BreakDistAxis1                   (2L)
#define BreakDistAxis2                   (2L)
long SecondsPerWormRotationAxis1       = ((long)(StepsPerWormRotationAxis1/StepsPerSecondAxis1));
volatile double StepsForRateChangeAxis1= (sqrt((double)DegreesForAcceleration*(double)StepsPerDegreeAxis1))*(double)MaxRate*16.0;
volatile double StepsForRateChangeAxis2= (sqrt((double)DegreesForAcceleration*(double)StepsPerDegreeAxis2))*(double)MaxRate*16.0;
#ifndef DegreesForRapidStop
#define DegreesForRapidStop 1.0
#endif

// Basic stepper driver mode setup ------------------------------------------------------------------------------------
#ifdef AXIS1_DRIVER_MODEL

  // Microsteps for each axis
  volatile uint8_t Axis1_Microsteps;
  #define AXIS1_MODE Axis1_Microsteps

  volatile uint8_t Axis2_Microsteps;
  #define AXIS2_MODE Axis2_Microsteps
  
  #ifdef AXIS1_MICROSTEPS_GOTO
    volatile uint8_t Axis1_MicrostepsGoto;
    #define AXIS1_MODE_GOTO Axis1_MicrostepsGoto
  #endif
  
  #ifdef AXIS2_MICROSTEPS_GOTO
    volatile uint8_t Axis2_MicrostepsGoto;
    #define AXIS2_MODE_GOTO Axis2_MicrostepsGoto
  #endif

#endif

// Location ----------------------------------------------------------------------------------------------------------------
double latitude  = 0.0;
double cosLat = 1.0;
double sinLat = 0.0;
double longitude = 0.0;

// fix UnderPoleLimit for fork mounts
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT)
#undef UnderPoleLimit
#define UnderPoleLimit 12
#endif

// Coordinates -------------------------------------------------------------------------------------------------------------
#ifdef MOUNT_TYPE_GEM
double celestialPoleAxis1  = 90.0;
#endif
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
double celestialPoleAxis1  = 0.0;
#endif
double celestialPoleAxis2 = 90.0;
// either 0 or (fabs(latitude))
#define AltAzmDecStartPos (fabs(latitude))

volatile long posAxis1   = 0;                            // hour angle position in steps
volatile long startAxis1 = 0;                            // hour angle of goto start position in steps
volatile fixed_t targetAxis1;                            // hour angle of goto end   position in steps
volatile byte dirAxis1   = 1;                            // stepping direction + or -
double newTargetRA       = 0.0;                          // holds the RA for goTos
fixed_t origTargetAxis1;
#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long stepAxis1=1;
#else
#define stepAxis1 1
#endif

volatile long posAxis2   = 0;                            // declination position in steps
volatile long startAxis2 = 0;                            // declination of goto start position in steps
volatile fixed_t targetAxis2;                            // declination of goto end   position in steps
volatile byte dirAxis2   = 1;                            // stepping direction + or -
double newTargetDec      = 0.0;                          // holds the Dec for goTos
long origTargetAxis2     = 0;
#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
volatile long stepAxis2=1;
#else
#define stepAxis2 1
#endif

double newTargetAlt=0.0, newTargetAzm=0.0;         // holds the altitude and azmiuth for slews
// for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianE = 60L;
// as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianW = 60L;
int    minAlt;                                     // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                     // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)
bool   autoMeridianFlip = false;                   // automatically do a meridian flip and continue when we hit the MinutesPastMeridianW

double currentAlt = 45.0;                          // the current altitude
double currentDec = 0.0;                           // the current declination

// Stepper driver enable/disable and direction -----------------------------------------------------------------------------

#define defaultDirAxis2EInit   1
#define defaultDirAxis2WInit   0
volatile byte defaultDirAxis2  = defaultDirAxis2EInit;
#define defaultDirAxis1NCPInit 0
#define defaultDirAxis1SCPInit 1
volatile byte defaultDirAxis1  = defaultDirAxis1NCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC};
Errors lastError = ERR_NONE;
enum GotoErrors {GOTO_ERR_NONE, GOTO_ERR_BELOW_HORIZON, GOTO_ERR_ABOVE_OVERHEAD, GOTO_ERR_STANDBY, GOTO_ERR_PARK, GOTO_ERR_GOTO, GOTO_ERR_OUTSIDE_LIMITS, GOTO_ERR_HARDWARE_FAULT, GOTO_ERR_IN_MOTION, GOTO_ERR_UNSPECIFIED};

boolean highPrecision = true;

#define TrackingNone             0
#define TrackingSidereal         1
#define TrackingMoveTo           2
volatile byte trackingState      = TrackingNone;
byte abortTrackingState          = TrackingNone;
volatile byte lastTrackingState  = TrackingNone;
int trackingSyncSeconds          = 0;
#define StartAbortSlew 1
byte abortSlew                   = 0;
volatile boolean safetyLimitsOn  = true;
boolean axis1Enabled             = false;
boolean axis2Enabled             = false;

#define MeridianFlipNever  0
#define MeridianFlipAlign  1
#define MeridianFlipAlways 2
#ifdef MOUNT_TYPE_GEM
byte meridianFlip = MeridianFlipAlways;
#endif
#ifdef MOUNT_TYPE_FORK
byte meridianFlip = MeridianFlipAlign;
#endif
#ifdef MOUNT_TYPE_FORK_ALT
byte meridianFlip = MeridianFlipNever;
#endif
#ifdef MOUNT_TYPE_ALTAZM
byte meridianFlip = MeridianFlipNever;
#endif

byte pierSideControl = PierSideNone;
enum PreferredPierSide {PPS_BEST,PPS_EAST,PPS_WEST};
PreferredPierSide preferredPierSide = PPS_BEST;

#define NotParked        0
#define Parking          1
#define Parked           2
#define ParkFailed       3
byte    parkStatus       = NotParked;
boolean parkSaved        = false;
boolean atHome           = true;
boolean homeMount        = false;

// Command processing -------------------------------------------------------------------------------------------------------
#define BAUD 9600
// serial speed
unsigned long baudRate[10] = {115200,56700,38400,28800,19200,14400,9600,4800,2400,1200};

// Guide command ------------------------------------------------------------------------------------------------------------
#define GuideRate1x        2
#ifndef GuideRateDefault
#define GuideRateDefault   6  // 24x
#endif
#define GuideRateNone      255
#define RateToDegPerSec  (1000000.0/(double)StepsPerDegreeAxis1)
#define RateToASPerSec   (RateToDegPerSec*3600.0)
#define RateToXPerSec    (RateToASPerSec/15.0)
double  slewRateX     =  (RateToXPerSec/MaxRate)*2.5; // 5x for exponential factor average rate and / 2x for default MaxRate
double  accXPerSec    =  (slewRateX/DegreesForAcceleration);
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,(RateToASPerSec/MaxRate)/2.0,RateToASPerSec/MaxRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x       half-MaxRate                   MaxRate

byte currentGuideRate        = GuideRateDefault;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirAxis1           = 0;
char          ST4DirAxis1             = 'b';
volatile byte guideDirAxis2           = 0;
char          ST4DirAxis2             = 'b';

volatile double  guideTimerRateAxis1 = 0.0;
volatile double  guideTimerRateAxis2 = 0.0;
volatile double  guideTimerBaseRateAxis1 = 0.0;
volatile double  guideTimerBaseRateAxis2 = 0.0;
fixed_t amountGuideAxis1;
fixed_t guideAxis1;
fixed_t amountGuideAxis2;
fixed_t guideAxis2;

// PEC control --------------------------------------------------------------------------------------------------------------
#define PECStatusString    "IpPrR"
#define PECStatusStringAlt "/,~;^"
#define IgnorePEC        0
#define ReadyPlayPEC     1
#define PlayPEC          2
#define ReadyRecordPEC   3
#define RecordPEC        4
byte    pecStatus         = IgnorePEC;
boolean pecRecorded       = false;
boolean pecFirstRecord    = false;
long    lastPecIndex      = -1;
long    pecIndex          = 0;
long    pecIndex1         = 0;
int     pecAnalogValue    = 0;
int     pecAutoRecord     = 0;      // for writing to PEC table to EEPROM
long    wormSensePos      = 0;      // in steps
boolean wormSensedAgain   = false;  // indicates PEC index was found
int     LastPecPinState   = PEC_SENSE_STATE;
boolean pecBufferStart    = false;
fixed_t accPecGuideHA;              // for PEC, buffers steps to be recorded
volatile double pecTimerRateAxis1 = 0.0;
// it takes 3.3ms to record a value to EEPROM, this can effect tracking performance since interrupts are disabled during the operation.
// so we store PEC data in RAM while recording.  When done, sidereal tracking is turned off and the data is written to EEPROM.
// writing the data can take up to 3 seconds.
byte pecBuffer[PECBufferSize];

// Misc ---------------------------------------------------------------------------------------------------------------------
#define Rad 57.29577951

// current site index and name
byte currentSite = 0; 
char siteName[16];

// offset corrections simple align
double indexAxis1       = 0;
long   indexAxis1Steps  = 0;
double indexAxis2       = 0;
long   indexAxis2Steps  = 0;

// tracking and PEC, fractional steps
fixed_t fstepAxis1;
fixed_t fstepAxis2;

// status state
boolean LED_ON = false;
boolean LED2_ON = false;

// sound/buzzer
#ifdef DEFAULT_SOUND_ON
boolean soundEnabled = true;
#else
boolean soundEnabled = false;
#endif
volatile int buzzerDuration = 0;

// pause at home on meridian flip
boolean pauseHome = false;            // allow pause at home?
boolean waitingHomeContinue = false;  // set to true to stop pause
boolean waitingHome = false;          // true if waiting at home

// reticule control
#ifdef RETICULE_LED_PINS
int reticuleBrightness=RETICULE_LED_PINS;
#endif

// backlash control
volatile int backlashAxis1  = 0;
volatile int backlashAxis2  = 0;
volatile int blAxis1        = 0;
volatile int blAxis2        = 0;

// focuser control
#ifdef AXIS4_DC_MODE_ON
byte dcPwrAxis4 = 50;
#endif
#ifdef AXIS5_DC_MODE_ON
byte dcPwrAxis5 = 50;
#endif

// aux pin control
#ifdef Aux0
byte valueAux0 = 0;
#endif
#ifdef Aux1
byte valueAux1 = 0;
#endif
#ifdef Aux2
byte valueAux2 = 0;
#endif
#ifdef Aux3
byte valueAux3 = 0;
#endif
#ifdef Aux4
byte valueAux4 = 0;
#endif
#ifdef Aux5
byte valueAux5 = 0;
#endif
#ifdef Aux6
byte valueAux6 = 0;
#endif
#ifdef Aux7
byte valueAux7 = 0;
#endif
#ifdef Aux8
byte valueAux8 = 0;
#endif
#ifdef Aux9
byte valueAux9 = 0;
#endif
#ifdef Aux10
byte valueAux10 = 0;
#endif
#ifdef Aux11
byte valueAux11 = 0;
#endif
#ifdef Aux12
byte valueAux12 = 0;
#endif
#ifdef Aux13
byte valueAux13 = 0;
#endif
#ifdef Aux14
byte valueAux14 = 0;
#endif
#ifdef Aux15
byte valueAux15 = 0;
#endif
