// Time keeping ------------------------------------------------------------------------------------------------------------
long siderealTimer    = 0;           // counter to issue steps during tracking
long PecSiderealTimer = 0;           // time since worm wheel zero index for PEC
long guideSiderealTimer=0;           // counter to issue steps during guiding
unsigned long housekeepingTimer;     // counter for timing housekeeping

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
                                     // default = 15956313 ticks per sidereal hundredth second, where a tick is 1/16 uS
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
#ifdef MOUNT_TYPE_ALTAZM
#define refraction_enable false      // refraction isn't allowed in Alt/Azm mode
#else
#define refraction_enable true       // refraction allowed
#endif

#ifdef TRACK_REFRACTION_RATE_DEFAULT_ON
boolean refraction = refraction_enable;
#else
boolean refraction = false;
#endif
boolean onTrack = false;
boolean onTrackDec = false;

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
#define StepsPerSecondAxis2              ((double)StepsPerDegreeAxis2/240.0)
#define BreakDistAxis1                   (2L)
#define BreakDistAxis2                   (2L)
#define SecondsPerWormRotationAxis1      ((long)(StepsPerWormRotationAxis1/StepsPerSecondAxis1))
volatile double StepsForRateChangeAxis1= ((double)DegreesForAcceleration/sqrt((double)StepsPerDegreeAxis1))*0.3333333*StepsPerDegreeAxis1*MaxRate;
volatile double StepsForRateChangeAxis2= ((double)DegreesForAcceleration/sqrt((double)StepsPerDegreeAxis2))*0.3333333*StepsPerDegreeAxis2*MaxRate;
#ifndef DegreesForRapidStop
#define DegreesForRapidStop 1.0
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
long celestialPoleAxis1  = 90L;
#endif
#if defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
long celestialPoleAxis1  = 0L;
#endif
double celestialPoleAxis2 = 90.0;
// either 0 or (fabs(latitude))
#define AltAzmDecStartPos (fabs(latitude))

volatile long posAxis1   = 90L*(long)StepsPerDegreeAxis1;// hour angle position in steps
long trueAxis1           = 90L*(long)StepsPerDegreeAxis1;// correction to above for motor shaft position steps
volatile long startAxis1 = 90L*(long)StepsPerDegreeAxis1;// hour angle of goto start position in steps
volatile fixed_t targetAxis1;                            // hour angle of goto end   position in steps
volatile byte dirAxis1   = 1;                            // stepping direction + or -
double newTargetRA       = 0.0;                          // holds the RA for goTos
fixed_t origTargetAxis1;
#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long stepAxis1=1;
#else
#define stepAxis1 1
#endif

volatile long posAxis2   = 90L*(long)StepsPerDegreeAxis2;// declination position in steps
long trueAxis2           = 90L*(long)StepsPerDegreeAxis2;// correction to above for motor shaft position steps
volatile long startAxis2 = 90L*(long)StepsPerDegreeAxis2;// declination of goto start position in steps
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
double currentAlt = 45;                            // the current altitude
// for goto's, how far past the meridian to allow before we do a flip (if on the East side of the pier) - one hour of RA is the default = 60.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianE = 60L;
// as above, if on the West side of the pier.  If left alone, the mount will stop tracking when it hits the this limit.  Sometimes used for Fork mounts in Align mode.  Ignored on Alt/Azm mounts.
long minutesPastMeridianW = 60L;
int    minAlt;                                     // the minimum altitude, in degrees, for goTo's (so we don't try to point too low)
int    maxAlt;                                     // the maximum altitude, in degrees, for goTo's (to keep the telescope tube away from the mount/tripod)
bool   autoMeridianFlip = false;                   // automatically do a meridian flip and continue when we hit the MinutesPastMeridianW

// Globals for rotator/de-rotator ------------------------------------------------------------------------------------------
#ifdef ROTATOR_ON
bool deRotate        = false;
bool deRotateReverse = false;
long posAxis3        = 0;                          // rotator position in steps
fixed_t targetAxis3;                               // rotator goto position in steps
fixed_t amountRotateAxis3;                         // rotator movement per 0.01/s
long axis3Increment  = 1;                          // rotator increment for manual control
unsigned long axis3Ms=0;
#ifdef REVERSE_AXIS3_ON
#define AXIS3_FORWARD LOW
#define AXIS3_REVERSE HIGH
#else
#define AXIS3_FORWARD HIGH
#define AXIS3_REVERSE LOW
#endif
#endif

// Globals for focusers ----------------------------------------------------------------------------------------------------
#ifdef FOCUSER1_ON
long posAxis4        = 0;                          // focuser position in steps
fixed_t targetAxis4;                               // focuser goto position in steps
fixed_t amountMoveAxis4;                           // focuser movement per 0.01/s
long axis4Increment  = 1;                          // focuser increment for manual control
unsigned long axis4Ms=0;
#ifdef REVERSE_AXIS4_ON
#define AXIS4_FORWARD LOW
#define AXIS4_REVERSE HIGH
#else
#define AXIS4_FORWARD HIGH
#define AXIS4_REVERSE LOW
#endif
#endif

#ifdef FOCUSER2_ON
long posAxis5        = 0;                          // focuser position in steps
fixed_t targetAxis5;                               // focuser goto position in steps
fixed_t amountMoveAxis5;                           // focuser movement per 0.01/s
long axis5Increment  = 1;                          // focuser increment for manual control
unsigned long axis5Ms=0;
#ifdef REVERSE_AXIS5_ON
#define AXIS5_FORWARD LOW
#define AXIS5_REVERSE HIGH
#else
#define AXIS5_FORWARD HIGH
#define AXIS5_REVERSE LOW
#endif
#endif

// Stepper driver enable/disable and direction -----------------------------------------------------------------------------
#if defined(AXIS1_DISABLED_HIGH)
#define Axis1_Disabled HIGH
#define Axis1_Enabled LOW
#endif
#if defined(AXIS1_DISABLED_LOW)
#define Axis1_Disabled LOW
#define Axis1_Enabled HIGH
#endif
boolean axis1Enabled = false;
#if defined(AXIS2_DISABLED_HIGH)
#define Axis2_Disabled HIGH
#define Axis2_Enabled LOW
#endif
#if defined(AXIS2_DISABLED_LOW)
#define Axis2_Disabled LOW
#define Axis2_Enabled HIGH
#endif
boolean axis2Enabled = false;

#define defaultDirAxis2EInit   1
#define defaultDirAxis2WInit   0
volatile byte defaultDirAxis2  = defaultDirAxis2EInit;
#define defaultDirAxis1NCPInit 0
#define defaultDirAxis1SCPInit 1
volatile byte defaultDirAxis1  = defaultDirAxis1NCPInit;

// Status ------------------------------------------------------------------------------------------------------------------
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
Errors lastError = ERR_NONE;

boolean highPrecision = true;

#define TrackingNone             0
#define TrackingSidereal         1
#define TrackingMoveTo           2
#define TrackingSiderealDisabled 3
volatile byte trackingState      = TrackingNone;
byte abortTrackingState          = TrackingNone;
volatile byte lastTrackingState  = TrackingNone;
boolean abortSlew                = false;

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

#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22
byte pierSide            = PierSideNone;

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
pserial PSerial;
pserial1 PSerial1;

// Guide command ------------------------------------------------------------------------------------------------------------
#define GuideRate1x        2
#define GuideRate24x       6
#define GuideRateNone      255

#define slewRate (1.0/(((double)StepsPerDegreeAxis1*(MaxRate/1000000.0)))*3600.0)
#define slewRateX (slewRate/15.0)
#define halfSlewRate (slewRate/2.0)
#define acc (slewRateX/DegreesForAcceleration)  // say 5 degrees to 240x for example = 240/5 = 48X/s
double  guideRates[10]={3.75,7.5,15,30,60,120,360,720,halfSlewRate,slewRate};
//                      .25X .5x 1x 2x 4x  8x 24x 48x half-MaxRate MaxRate

byte currentGuideRate        = GuideRate24x;
byte currentPulseGuideRate   = GuideRate1x;
volatile byte activeGuideRate= GuideRateNone;

volatile byte guideDirAxis1           = 0;
char          ST4DirAxis1             = 'b';
volatile byte guideDirAxis2           = 0;
char          ST4DirAxis2             = 'b';

volatile double   guideTimerRateAxis1 = 0.0;
volatile double   guideTimerRateAxis2 = 0.0;

double  guideTimerBaseRate = 0;
fixed_t amountGuideAxis1;
fixed_t guideAxis1;
fixed_t amountGuideAxis2;
fixed_t guideAxis2;

// PEC control --------------------------------------------------------------------------------------------------------------
#define PECStatusString  "IpPrR"
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

// EEPROM Info --------------------------------------------------------------------------------------------------------------
// 0-1023 bytes
// general purpose storage 0..99

#define EE_posAxis1    0      // 4
#define EE_posAxis2    4      // 4
#define EE_pierSide    8      // 1
#define EE_parkStatus  9      // 1
#define EE_parkSaved   10     // 1

#define EE_currentSite 11     // 1

#define EE_sgSgtAxis1  12     // 1
#define EE_sgSgtAxis2  13     // 1

#define EE_LMT         14     // 4
#define EE_JD          18     // 4

#define EE_pulseGuideRate 22  // 1
#define EE_maxRate     23     // 2

#define EE_autoMeridianFlip 25 // 1

#define EE_dfCor       26     // 4

#define EE_trueAxis1   30     // 4
#define EE_trueAxis2   34     // 4

#define EE_dpmE        38     // 1
#define EE_dpmW        39     // 1
#define EE_minAlt      40     // 1
#define EE_maxAlt      41     // 1

#define EE_doCor       42     // 4
#define EE_pdCor       46     // 4
#define EE_altCor      50     // 4
#define EE_azmCor      54     // 4
#define EE_indexAxis1  58     // 4
#define EE_indexAxis2  62     // 4
#define EE_tfCor       66     // 4

#define EE_pecStatus   70     // 1
#define EE_pecRecorded 71     // 1

#define EE_sgLimitAxis1 72    // 2
#define EE_sgLimitAxis2 74    // 2

#define EE_wormSensePos 76    // 4

#define EE_backlashAxis1 80   // 4
#define EE_backlashAxis2 84   // 4
#define EE_siderealInterval 88  // 4
#define EE_onTrackDec  92     // 1
#define EE_pauseHome   93     // 1 +2

#define EE_autoInitKey 96

// site index: 100-199
// 100..103 latitude  1  ((index 1-1)*25+100)
// 104..107 longitude 1
// 108      timeZone  1
// 109..124 site name 1

// 125..128 latitude  2  ((index 2-1)*25+100)
// 129..132 longitude 2
// 133      timeZone  2
// 134..149 site name 2

// 150..103 latitude  3  ((index 3-1)*25+100)
// 154..157 longitude 3
// 158      timeZone  3
// 159..174 site name 3

// 175..178 latitude  4  ((index 4-1)*25+100)
// 179..182 longitude 4
// 183      timeZone  4
// 184..199 site name 4

#define EE_sites    100

// PEC table: 200...PECBufferSize+200-1
// PECBufferSize table of byte sized integers -128..+127, units are steps

#define EE_pecTable 200

// Library
// Catalog storage starts at 200+PECBufferSize and fills EEPROM

