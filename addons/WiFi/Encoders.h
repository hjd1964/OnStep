// -------------------------------------------------------------------------------------------------------
// Handle encoders, both CW/CCW and Quadrature A/B types are supported

#include "MountStatus.h"

#if AXIS1_ENC > 0 && AXIS2_ENC > 0
  #define ENCODERS ON
#endif

#if ENCODERS == ON

#ifdef ESP8266
  #include <Esp.h>
#endif

#if ENCODERS_AUTO_SYNC_DEFAULT == ON
  bool encAutoSync=true;
#else
  bool encAutoSync=false;
#endif
long Axis1EncDiffLimit=AXIS1_ENC_DIFF_LIMIT;
long Axis2EncDiffLimit=AXIS2_ENC_DIFF_LIMIT;

// encoder position
volatile int32_t __p1,__p2;

// encoder polling rate in seconds, default=2.0
#define POLLING_RATE 2.0

// encoder rate control
#if AXIS1_ENC_RATE_CONTROL == ON

  // user interface and settings
  bool encSweep=true;
  bool encRateControl=false;
  long Axis1EncProp=10;
  long Axis1EncMinGuide=100;

  // timing related
  volatile uint32_t T0=0;
  volatile uint32_t T1=0;
  volatile uint32_t Telapsed=0;

  #define MIN_ENC_PERIOD 0.2
  #define MAX_ENC_PERIOD 5.0
  float arcSecondsPerTick=(1.0/AXIS1_ENC_TICKS_DEG)*3600.0; // (0.0018)*3600 = 6.48
  float usPerTick=(arcSecondsPerTick/15.041)*1000000.0;     // 6.48/15.041 = 0.4308 seconds per tick

  unsigned long msPerTickMax =(arcSecondsPerTick/15.041)*1000.0*MAX_ENC_PERIOD;
#if AXIS1_ENC_BIN_AVG > 0
  volatile uint32_t usPerBinTickMin =(double)usPerTick*(double)AXIS1_ENC_BIN_AVG*MIN_ENC_PERIOD;
  volatile uint32_t usPerBinTickMax =(double)usPerTick*(double)AXIS1_ENC_BIN_AVG*MAX_ENC_PERIOD;
#endif
#if defined(ESP8266)
  volatile uint32_t clocksPerTickMin=(double)usPerTick*(double)ESP.getCpuFreqMHz()*MIN_ENC_PERIOD;
  volatile uint32_t clocksPerTickMax=(double)usPerTick*(double)ESP.getCpuFreqMHz()*MAX_ENC_PERIOD;
#elif defined(__MK20DX256__)
  volatile uint32_t clocksPerTickMin=(double)usPerTick*(double)(F_CPU/1000000L)*MIN_ENC_PERIOD;
  volatile uint32_t clocksPerTickMax=(double)usPerTick*(double)(F_CPU/1000000L)*MAX_ENC_PERIOD;
#else
  volatile uint32_t clocksPerTickMin=(double)usPerTick*MIN_ENC_PERIOD;
  volatile uint32_t clocksPerTickMax=(double)usPerTick*MAX_ENC_PERIOD;
#endif
#if defined(ESP8266)
  #define GetClockCount ESP.getCycleCount()
  #define ClockCountToMicros ((uint32_t)ESP.getCpuFreqMHz())
#elif defined(__MK20DX256__)
  #define GetClockCount ARM_DWT_CYCCNT
  #define ClockCountToMicros (F_CPU/1000000L)
#else
  #define GetClockCount micros()
  #define ClockCountToMicros (1L)
#endif
  
  // averages & rate calculation
  volatile long Axis1EncStaSamples=20;
  volatile long Axis1EncLtaSamples=200;
  volatile int32_t Tsta=0;
  volatile int32_t Tlta=0;
#if AXIS1_ENC_BIN_AVG > 0
  volatile uint32_t StaBins[AXIS1_ENC_BIN_AVG];
  volatile uint32_t LtaBins[AXIS1_ENC_BIN_AVG];
  volatile uint32_t T1Bins[AXIS1_ENC_BIN_AVG];
#endif
  float axis1EncRateSta=1.0;
  float axis1EncRateLta=1.0;
  float axis1EncRateComp=0.0;
  float axis1Rate=1.0;

#if AXIS1_ENC_INTPOL_COS == ON
  long Axis1EncIntPolPeriod=AXIS1_ENC_BIN_AVG;
  long Axis1EncIntPolPhase=1;
  long Axis1EncIntPolMag=0;
  float intpolComp=0;
  float intpolPhase=0;
#endif

#if AXIS1_ENC_RATE_AUTO > 0
  static unsigned long nextWormPeriod=0;
  static float axis1RateDelta=0;
#endif

  // guiding
  float guideCorrection=0;
  long guideCorrectionMillis=0;

#endif

// ----------------------------------------------------------------------------------------------------------------
// this is for Quadrature A/B type encoders (library based)
#if (AXIS1_ENC_RATE_CONTROL != ON && !defined(ESP32)) && (AXIS1_ENC == AB || AXIS2_ENC == AB)
  #include <Encoder.h> // from https://github.com/PaulStoffregen/Encoder
#if AXIS1_ENC == AB
  Encoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN);
#endif
#if AXIS2_ENC == AB
  Encoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN);
#endif
#endif
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
// support for local encoder ISR routines
#if AXIS1_ENC_RATE_CONTROL == ON
static unsigned long lastLogRate=0;
bool fastMotion() { return Telapsed<clocksPerTickMin; }
bool slowMotion() { return (millis()-lastLogRate)>msPerTickMax; }

// this ISR function times the arrival of pulses from the RA axis encoder
void ICACHE_RAM_ATTR __logRate() {
  lastLogRate=millis();
#if AXIS1_ENC_BIN_AVG > 0
  int i=abs(__p1)%AXIS1_ENC_BIN_AVG;
  uint32_t T0us=T0/ClockCountToMicros;
  uint32_t Te=T0us-T1Bins[i]; T1Bins[i]=T0us;
  if ((Te>usPerBinTickMin) && (Te<usPerBinTickMax))
  {
    StaBins[i]=((StaBins[i]*(Axis1EncStaSamples-1))+Te)/Axis1EncStaSamples;
    LtaBins[i]=((LtaBins[i]*(Axis1EncLtaSamples-1))+Te)/Axis1EncLtaSamples;
  }
#else
  uint32_t Te=Telapsed/ClockCountToMicros;
  Tsta=((Tsta*(Axis1EncStaSamples-1))+Te)/Axis1EncStaSamples;
  Tlta=((Tlta*(Axis1EncLtaSamples-1))+Te)/Axis1EncLtaSamples;
#endif
}
#endif
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
// ISR to read Quadrature A/B type encoders
//                 ______        ______       
//         A _____|      |______|      |______ A
// neg <--      ______        ______        __    --> pos
//         B __|      |______|      |______|   B

#if (AXIS1_ENC_RATE_CONTROL == ON || defined(ESP32)) && (AXIS1_ENC == AB || AXIS2_ENC == AB)
  volatile int16_t __aPin1,__bPin1;
  volatile bool __a_set1=false;
  volatile bool __b_set1=false;
  void ICACHE_RAM_ATTR __a1() {
#ifdef __MK20DX256__
    __a_set1 = digitalReadFast(__aPin1) == HIGH;
#else
    __a_set1 = digitalRead(__aPin1) == HIGH;
#endif
    if (__a_set1 != __b_set1) __p1--; else __p1++;
#if AXIS1_ENC_RATE_CONTROL == ON
    T0=GetClockCount; Telapsed=(T0-T1); T1=T0;
    if (Telapsed>clocksPerTickMin) __logRate();
#endif
  }
  void ICACHE_RAM_ATTR __b1() { 
#ifdef __MK20DX256__
    __b_set1 = digitalReadFast(__bPin1) == HIGH;
#else
    __b_set1 = digitalRead(__bPin1) == HIGH;
#endif
    if (__a_set1 == __b_set1) __p1--; else __p1++;
#if AXIS1_ENC_RATE_CONTROL == ON
    T0=GetClockCount; Telapsed=(T0-T1); T1=T0;
    if (Telapsed>clocksPerTickMin) __logRate();
#endif
  }

  volatile int16_t __aPin2,__bPin2;
  volatile bool __a_set2=false;
  volatile bool __b_set2=false;
  void ICACHE_RAM_ATTR __a2() {
    __a_set2 = digitalRead(__aPin2) == HIGH;
    if (__a_set2 != __b_set2) __p2--; else __p2++;
  }
  void ICACHE_RAM_ATTR __b2() {
    __b_set2 = digitalRead(__bPin2) == HIGH;
    if (__a_set2 == __b_set2) __p2--; else __p2++;
  }
  
  class ABEncoder {
    public:
      ABEncoder(int16_t aPin, int16_t bPin, int16_t axis) {
        _axis=axis;
        pinMode(aPin,INPUT_PULLUP);
        pinMode(bPin,INPUT_PULLUP);
        if (_axis==1) {
          __aPin1=aPin; __bPin1=bPin;
          attachInterrupt(digitalPinToInterrupt(aPin),__a1,CHANGE);
          attachInterrupt(digitalPinToInterrupt(bPin),__b1,CHANGE);
        }
        if (_axis==2) {
          __aPin2=aPin; __bPin2=bPin;
          attachInterrupt(digitalPinToInterrupt(aPin),__a2,CHANGE);
          attachInterrupt(digitalPinToInterrupt(bPin),__b2,CHANGE);
        }
      }
      int32_t read() {
        int32_t v=0;
        if (_axis==1) { noInterrupts(); v=__p1; interrupts(); }
        if (_axis==2) { noInterrupts(); v=__p2; interrupts(); }
        return v;
      }
      void write(int32_t v) {
        if (_axis==1) { noInterrupts(); __p1=v; interrupts(); }
        if (_axis==2) { noInterrupts(); __p2=v; interrupts(); }
      }
    private:
      int16_t _axis;
  };
#if AXIS1_ENC == AB
  ABEncoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN,1);
#endif
#if AXIS2_ENC == AB
  ABEncoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN,2);
#endif
#endif
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
// ISR to read CW/CCW type encoders
#if AXIS1_ENC == CWCCW || AXIS2_ENC == CWCCW
  void ICACHE_RAM_ATTR __cw1() {
    __p1++;
#if AXIS1_ENC_RATE_CONTROL == ON
    T0=GetClockCount; Telapsed=(T0-T1); T1=T0;
    if (Telapsed>clocksPerTickMin) __logRate();
#endif
  }
  void ICACHE_RAM_ATTR __ccw1() {
    __p1--;
#if AXIS1_ENC_RATE_CONTROL == ON
    T0=GetClockCount; Telapsed=(T0-T1); T1=T0;
    if (Telapsed>clocksPerTickMin) __logRate();
#endif
  }

  void ICACHE_RAM_ATTR __cw2() { __p2++; }
  void ICACHE_RAM_ATTR __ccw2() { __p2--; }

  class CwCcwEncoder {
    public:
      CwCcwEncoder(int16_t cwPin, int16_t ccwPin, int16_t axis) {
        _cwPin=cwPin;
        _ccwPin=ccwPin;
        _axis=axis;
        pinMode(_cwPin,INPUT_PULLUP);
        pinMode(_ccwPin,INPUT_PULLUP);
        if (_axis==1) {
          attachInterrupt(digitalPinToInterrupt(_cwPin),__cw1,CHANGE);
          attachInterrupt(digitalPinToInterrupt(_ccwPin),__ccw1,CHANGE);
        }
        if (_axis==2) {
          attachInterrupt(digitalPinToInterrupt(_cwPin),__cw2,CHANGE);
          attachInterrupt(digitalPinToInterrupt(_ccwPin),__ccw2,CHANGE);
        }
      }
      int32_t read() {
        int32_t v=0;
        if (_axis==1) { noInterrupts(); v=__p1; interrupts(); }
        if (_axis==2) { noInterrupts(); v=__p2; interrupts(); }
        return v;
      }
      void write(int32_t v) {
        if (_axis==1) { noInterrupts(); __p1=v; interrupts(); }
        if (_axis==2) { noInterrupts(); __p2=v; interrupts(); }
      }
    private:
      int16_t _cwPin;
      int16_t _ccwPin;
      int16_t _axis;
  };
#if AXIS1_ENC == CWCCW
  CwCcwEncoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN,1);
#endif
#if AXIS2_ENC == CWCCW
  CwCcwEncoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN,2);
#endif
#endif
// ----------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------
// background process position/rate control for encoders 
class Encoders {
  public:
    void init() {
    }
    void syncFromOnStep() {
      // automatically sync the encoders to OnStep's position when at home or parked
#if AXIS1_ENC_REVERSE == ON
        axis1Pos.write(-_osAxis1*(double)AXIS1_ENC_TICKS_DEG);
#else
        axis1Pos.write(_osAxis1*(double)AXIS1_ENC_TICKS_DEG);
#endif
#if AXIS2_ENC_REVERSE == ON
        axis2Pos.write(-_osAxis2*(double)AXIS2_ENC_TICKS_DEG);
#else
        axis2Pos.write(_osAxis2*(double)AXIS2_ENC_TICKS_DEG);
#endif
    }
    void syncToOnStep() {
        char s[22];
        // automatically sync OnStep to the encoders' position
        Ser.print(":SX40,"); Ser.print(_enAxis1,6); Ser.print("#"); Ser.readBytes(s,1);
        Ser.print(":SX41,"); Ser.print(_enAxis2,6); Ser.print("#"); Ser.readBytes(s,1);
        Ser.print(":SX42,1#"); Ser.readBytes(s,1);
    }
    void poll() {
      // check encoders and sync OnStep if diff is too great, checks every 2 seconds
      static unsigned long nextEncCheckMs=millis()+(unsigned long)(POLLING_RATE*1000.0);
      unsigned long temp=millis();
      char *conv_end;
      if ((long)(temp-nextEncCheckMs)>0) {
        nextEncCheckMs=temp+(unsigned long)(POLLING_RATE*1000.0);
        char s[22];
        if (command(":GX42#",s) && strlen(s) > 1) {
          double f=strtod(s,&conv_end);
          if (&s[0] != conv_end && f >= -999.9 && f <= 999.9) _osAxis1=f;
        }
        if (command(":GX43#",s) && strlen(s) > 1) {
          double f=strtod(s,&conv_end);
          if (&s[0] != conv_end && f >= -999.9 && f <= 999.9) _osAxis2=f;
        }
        _enAxis1=(double)axis1Pos.read()/(double)AXIS1_ENC_TICKS_DEG;
#if AXIS1_ENC_REVERSE == ON
        _enAxis1=-_enAxis1;
#endif
        _enAxis2=(double)axis2Pos.read()/(double)AXIS2_ENC_TICKS_DEG;
#if AXIS2_ENC_REVERSE == ON
        _enAxis2=-_enAxis2;
#endif

        mountStatus.update();
        if (encAutoSync && mountStatus.valid()) {
          if (mountStatus.atHome() || mountStatus.parked() || mountStatus.aligning() || mountStatus.syncToEncodersOnly()) {
            syncFromOnStep();
            // re-enable normal operation once we're updated here
            if (mountStatus.syncToEncodersOnly()) { Ser.print(":SX43,1#"); Ser.readBytes(s,1); }
          } else
            if (!mountStatus.slewing() && !mountStatus.guiding()) {
              if ((fabs(_osAxis1-_enAxis1)>(double)(Axis1EncDiffLimit/3600.0)) ||
                  (fabs(_osAxis2-_enAxis2)>(double)(Axis2EncDiffLimit/3600.0))) syncToOnStep();
            }
        }

        // automatic rate compensation
#if AXIS1_ENC_RATE_CONTROL == ON

        // get the averages
#if AXIS1_ENC_BIN_AVG > 0
        Tsta=0; Tlta=0;
        for (int i=0; i<AXIS1_ENC_BIN_AVG; i++) { Tsta+=StaBins[i]; Tlta+=LtaBins[i]; }
        Tsta/=AXIS1_ENC_BIN_AVG; // average
        Tlta/=AXIS1_ENC_BIN_AVG;
        Tsta/=AXIS1_ENC_BIN_AVG; // each period is AXIS1_ENC_BIN_AVG X longer than the step to step frequency
        Tlta/=AXIS1_ENC_BIN_AVG;
#endif
        axis1EncRateSta=(usPerTick/Tsta)+axis1EncRateComp;
        axis1EncRateLta=(usPerTick/Tlta)+axis1EncRateComp;

        // get the tracking rate OnStep thinks it has once every ten seconds
        static int pass=-1;
        pass++;
        if (pass%5==0) {
          Ser.print(":GX49#"); s[Ser.readBytesUntil('#',s,20)]=0;
          if (strlen(s)>1) axis1Rate=atof(s); else axis1Rate=0;
        }

        // reset averages if rate is too fast or too slow
        static unsigned long resetTimeout=0;
        if (fastMotion() || slowMotion()) resetTimeout=millis();

        // keep things reset for 15 seconds if just starting up again
        if ((long)(millis()-resetTimeout)<15000L) { clearAverages(); return; }

        // encoder rate control disabled
        if (!encRateControl) return;
          
#if AXIS1_ENC_INTPOL_COS == ON
        long a1=axis1Pos.read();
        intpolPhase=(a1+Axis1EncIntPolPhase) % Axis1EncIntPolPeriod;
        float a3=(intpolPhase/(float)Axis1EncIntPolPeriod)*3.1415*2.0;
        intpolComp=cos(a3)*(Axis1EncIntPolMag/1000000.0);
        axis1EncRateSta=axis1EncRateSta+intpolComp;
#endif

#if AXIS1_ENC_RATE_AUTO > 0
        if ((long)(millis()-nextWormPeriod)>=0) {
          nextWormPeriod=millis()+(unsigned long)(AXIS1_ENC_RATE_AUTO)*997UL;
          axis1EncRateComp+=axis1RateDelta/(double)(AXIS1_ENC_RATE_AUTO);
          if (axis1EncRateComp>+0.01) axis1EncRateComp=+0.01;
          if (axis1EncRateComp<-0.01) axis1EncRateComp=-0.01;
          axis1RateDelta=0;
        }
        axis1RateDelta+=(axis1Rate-axis1EncRateSta)*POLLING_RATE;
#endif

        // accumulate tracking rate departures for pulse-guide, rate delta * 2 seconds
        guideCorrection+=(axis1Rate-axis1EncRateSta)*((float)Axis1EncProp/100.0)*POLLING_RATE;

        if (guideCorrection>POLLING_RATE) clearAverages(); else
        if (guideCorrection<-POLLING_RATE) clearAverages(); else
        if (guideCorrection>Axis1EncMinGuide/1000.0) {
          guideCorrectionMillis=round(guideCorrection*1000.0);
          Ser.print(":Mgw"); Ser.print(guideCorrectionMillis); Ser.print("#");
          guideCorrection=0;
        } else
        if (guideCorrection<-Axis1EncMinGuide/1000.0) {
          guideCorrectionMillis=round(guideCorrection*1000.0);
          Ser.print(":Mge"); Ser.print(-guideCorrectionMillis); Ser.print("#");
          guideCorrection=0;
        } else 
          guideCorrectionMillis=0;
#endif
      }
    }

#if AXIS1_ENC_RATE_CONTROL == ON
    void clearAverages() {
      double d=usPerTick*axis1Rate;
#if AXIS1_ENC_BIN_AVG > 0
      for (int i=0; i<AXIS1_ENC_BIN_AVG; i++) { StaBins[i]=d*AXIS1_ENC_BIN_AVG; LtaBins[i]=d*AXIS1_ENC_BIN_AVG; }
#endif
      Tsta=d;
      Tlta=d;
      axis1EncRateSta=usPerTick/d;
      axis1EncRateLta=usPerTick/d;
      guideCorrection=0.0;
      guideCorrectionMillis=0;
#if AXIS1_ENC_RATE_AUTO > 0
      axis1EncRateComp=0.0;
      axis1RateDelta=0;
      nextWormPeriod=millis()+(unsigned long)(AXIS1_ENC_RATE_AUTO)*997UL;;
#endif
    }
#endif
  
    double getAxis1() { return _enAxis1; }
    double getAxis2() { return _enAxis2; }
    double getOnStepAxis1() { return _osAxis1; }
    double getOnStepAxis2() { return _osAxis2; }

  private:
    double _osAxis1=0;
    double _osAxis2=0;
    double _enAxis1=0;
    double _enAxis2=0;
};
#endif
