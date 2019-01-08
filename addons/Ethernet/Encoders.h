// -------------------------------------------------------------------------------------------------------
// Handle encoders, both CW/CCW and Quadrature A/B types are supported

#include "MountStatus.h"

#if (!defined(AXIS1_ENC_OFF) && !defined(AXIS2_ENC_OFF))
  #define ENCODERS_ON
#endif

#ifdef ENCODERS_ON

#ifdef ESP8266
#include <Esp.h>
#endif

bool encAutoSync=true;
long Axis1EncDiffLimit=AXIS1_ENC_DIFF_LIMIT;
long Axis2EncDiffLimit=AXIS2_ENC_DIFF_LIMIT;

// encoder rate control
#ifdef AXIS1_ENC_RATE_CONTROL_ON
  bool encRateControl=false;
  bool encSweep=true;
  volatile long Axis1EncStaSamples=20;
  volatile long Axis1EncLtaSamples=200;
  long Axis1EncProp=10;
  float Axis1EncRateComp=1.0;
  long Axis1EncIntPolPeriod=AXIS1_ENC_INTPOL_PERIOD;
  long Axis1EncIntPolPhase=1;
  long Axis1EncIntPolMag=0;
  float intpolComp=0;
  float intpolPhase=0;
  float arcSecondsPerTick=(1.0/AXIS1_ENC_TICKS_DEG)*3600.0;        // (0.0018)*3600 = 6.48
  float usPerTick=(arcSecondsPerTick/15.041)*1000000.0;            // 6.48/15.041 = 0.4308 seconds per tick
  volatile uint32_t usPerTickMin=(double)usPerTick*0.2;
  volatile uint32_t usPerTickMax=(double)usPerTick*5.0;
  float axis1EncRateSta=0;
  float axis1EncRateLta=0;
  float axis1Rate=0;
  long finalCorrection=0;

  volatile int32_t Tsta=0;
  volatile int32_t Tlta=0;
#ifdef AXIS1_ENC_BIN_AVG_ON
  volatile uint32_t StaBins[AXIS1_ENC_INTPOL_PERIOD];
  volatile uint32_t LtaBins[AXIS1_ENC_INTPOL_PERIOD];
  volatile uint32_t T1Bins[AXIS1_ENC_INTPOL_PERIOD];
#endif

#endif

  volatile int32_t __p1;
  volatile uint32_t T0=0;
  volatile uint32_t T1=0;
  volatile uint32_t Telapsed=0;

// this is for Quadrature type encoders -------------------
#if defined(AXIS1_ENC_AB) || defined(AXIS2_ENC_AB)
  #include <Encoder.h> // from https://github.com/PaulStoffregen/Encoder 
#endif
#ifdef AXIS1_ENC_AB
  Encoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN);
#endif
#ifdef AXIS2_ENC_AB
  Encoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN);
#endif
// --------------------------------------------------------

// this is for CW/CCW type encoders -----------------------
#if defined(AXIS1_ENC_CWCCW) || defined(AXIS2_ENC_CWCCW)
  void ICACHE_RAM_ATTR __cw1() {
    __p1++;
#ifdef AXIS1_ENC_RATE_CONTROL_ON
#ifdef ESP8266
    T0=ESP.getCycleCount();
#else
    T0=micros();
#endif
    Telapsed=(T0-T1); T1=T0;
    if (Telapsed>usPerTickMin) {
#ifdef AXIS1_ENC_BIN_AVG_ON
#ifdef ESP8266
      uint32_t T0us=T0/((uint32_t)ESP.getCpuFreqMHz());
#else
      uint32_t T0us=T0;
#endif
      uint32_t Te=T0us-T1Bins[__p1%AXIS1_ENC_INTPOL_PERIOD]; T1Bins[__p1%AXIS1_ENC_INTPOL_PERIOD]=T0us;
      if ((Te/AXIS1_ENC_INTPOL_PERIOD>usPerTickMin) && (Te/AXIS1_ENC_INTPOL_PERIOD<usPerTickMax)) {
        StaBins[__p1%AXIS1_ENC_INTPOL_PERIOD]=((StaBins[__p1%AXIS1_ENC_INTPOL_PERIOD]*(Axis1EncStaSamples-1))+Te)/Axis1EncStaSamples;
        LtaBins[__p1%AXIS1_ENC_INTPOL_PERIOD]=((LtaBins[__p1%AXIS1_ENC_INTPOL_PERIOD]*(Axis1EncLtaSamples-1))+Te)/Axis1EncLtaSamples;
      }
#else
#ifdef ESP8266
      uint32_t Te=Telapsed/((uint32_t)ESP.getCpuFreqMHz());
#else
      uint32_t Te=Telapsed;
#endif
      Tsta=((Tsta*(Axis1EncStaSamples-1))+Te)/Axis1EncStaSamples;
      Tlta=((Tlta*(Axis1EncLtaSamples-1))+Te)/Axis1EncLtaSamples;
#endif
    }
#endif
  }
  void ICACHE_RAM_ATTR __ccw1() { 
    __p1--;
#ifdef AXIS1_ENC_RATE_CONTROL_ON
    T0=micros(); Telapsed=T1-T0;
    if (Telapsed>2000) {
      T1=T0;
      Tsta=((Tsta*(Axis1EncStaSamples-1))+Telapsed)/Axis1EncStaSamples;
      Tlta=((Tlta*(Axis1EncLtaSamples-1))+Telapsed)/Axis1EncLtaSamples;
    }
#endif
  }

  volatile int32_t __p2;
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
#endif
#ifdef AXIS1_ENC_CWCCW
  CwCcwEncoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN,1);
#endif
#ifdef AXIS2_ENC_CWCCW
  CwCcwEncoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN,2);
#endif
// --------------------------------------------------------

class Encoders {
  public:
    void init() {
    }
    void syncFromOnStep() {
      // automatically sync the encoders to OnStep's position when at home or parked
      #ifdef AXIS1_ENC_REVERSE_ON
        axis1Pos.write(-_osAxis1*(double)AXIS1_ENC_TICKS_DEG);
      #else
        axis1Pos.write(_osAxis1*(double)AXIS1_ENC_TICKS_DEG);
      #endif
      #ifdef AXIS2_ENC_REVERSE_ON
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
      static unsigned long nextEncCheckMs=millis()+2000UL;
      unsigned long temp=millis();
      char *conv_end;
      if ((long)(temp-nextEncCheckMs)>0) {
        nextEncCheckMs=temp+2000UL;
        char s[22];
        Ser.print(":GX42#"); s[Ser.readBytesUntil('#',s,20)]=0;
        if (strlen(s)>1) { // can return error code "0" also, which is ignored
          double f=strtod(s,&conv_end);
          if ( (&s[0]!=conv_end) && (f>=-999.9) && (f<=999.9)) _osAxis1=f;
        }
        Ser.print(":GX43#"); s[Ser.readBytesUntil('#',s,20)]=0;
        if (strlen(s)>1) { // can return error code "0" also, which is ignored
          double f=strtod(s,&conv_end);
          if ( (&s[0]!=conv_end) && (f>=-999.9) && (f<=999.9)) _osAxis2=f;
        }
        _enAxis1=(double)axis1Pos.read()/(double)AXIS1_ENC_TICKS_DEG;
        #ifdef AXIS1_ENC_REVERSE_ON
        _enAxis1=-_enAxis1;
        #endif
        _enAxis2=(double)axis2Pos.read()/(double)AXIS2_ENC_TICKS_DEG;
        #ifdef AXIS2_ENC_REVERSE_ON
        _enAxis2=-_enAxis2;
        #endif

#ifdef ENCODERS_AUTO_SYNC_ON
          mountStatus.update();
          if (mountStatus.valid()) {
            if (mountStatus.atHome() || mountStatus.parked()) syncFromOnStep();
          if (!mountStatus.slewing() && !mountStatus.guiding() && encAutoSync) {
            if ((fabs(_osAxis1-_enAxis1)>(double)(Axis1EncDiffLimit/3600.0)) ||
                (fabs(_osAxis2-_enAxis2)>(double)(Axis2EncDiffLimit/3600.0))) syncToOnStep();
          }
        }
#endif

#ifdef AXIS1_ENC_RATE_CONTROL_ON

        // get the averages
#ifdef AXIS1_ENC_BIN_AVG_ON
        Tsta=0; Tlta=0;
        for (int i=0; i<AXIS1_ENC_INTPOL_PERIOD; i++) { Tsta+=StaBins[i]; Tlta+=LtaBins[i]; }
        Tsta/=AXIS1_ENC_INTPOL_PERIOD; // average
        Tlta/=AXIS1_ENC_INTPOL_PERIOD;
        Tsta/=AXIS1_ENC_INTPOL_PERIOD; // each period is AXIS1_ENC_INTPOL_PERIOD X longer than the step to step frequency
        Tlta/=AXIS1_ENC_INTPOL_PERIOD;
#endif
        axis1EncRateSta=(usPerTick/Tsta)*Axis1EncRateComp;
        axis1EncRateLta=(usPerTick/Tlta)*Axis1EncRateComp;

#ifdef AXIS1_ENC_INTPOL_COS_ON
        long a1=axis1Pos.read();
        intpolPhase=(a1+Axis1EncIntPolPhase) % Axis1EncIntPolPeriod;
        float a3=(intpolPhase/(float)Axis1EncIntPolPeriod)*3.1415*2.0;
        intpolComp=cos(a3)*(Axis1EncIntPolMag/1000000.0);
        axis1EncRateSta=axis1EncRateSta+intpolComp;
#endif

        // automatic rate compensation
#ifdef AXIS1_ENC_AUTO_RATE_COMP_ON
        #define rcount 100
        static float rateComp[rcount];
        static int samples=0; samples+=2; if (samples>AXIS1_SEC_PER_WORM_ROT) samples=AXIS1_SEC_PER_WORM_ROT;
        static int sec=-1; sec+=2; if (sec>AXIS1_SEC_PER_WORM_ROT) sec=0;
        // store the sample
        int SecondsPerSample=(AXIS1_SEC_PER_WORM_ROT/rcount);
        int i=sec/SecondsPerSample; if (i<0) i=0; if (i>rcount-1) i=rcount-1;
        rateComp[i]=usPerTick/Tsta;
        // compute the average rate over the last worm period
        float sum=0;
        float avg=1.0;
        for (int i=0; i<(samples/SecondsPerSample); i++) sum+=rateComp[i];
        if ((samples/SecondsPerSample) > 0) avg=sum/(samples/SecondsPerSample);
        Axis1EncRateComp=1.0/avg;
#endif

        // get the tracking rate OnStep thinks it has once every ten seconds
        static int pass=-1;
        pass++;
        if (pass%5==0) {
          Ser.print(":GX49#"); s[Ser.readBytesUntil('#',s,20)]=0;
          if (strlen(s)>1) {
            axis1Rate=atof(s);
          } else axis1Rate=0;
        }

        // accumulate tracking rate departures
        static float correction=0;
        float r=(axis1Rate-axis1EncRateSta)*((float)Axis1EncProp/100.0)*2.0;  // rate delta * 2 seconds
        correction+=r;

        if (!encRateControl) correction=0.0;
        if (correction>0.1) {
          finalCorrection=round(correction*1000.0);
          if (finalCorrection>2000) finalCorrection=2000;
          Ser.print(":Mgw"); Ser.print(finalCorrection); Ser.print("#");
          if (correction>2.0) correction-=2.0; else correction=0;
        } else
        if (correction<-0.1) {
          finalCorrection=round(-correction*1000.0);
          if (finalCorrection>2000) finalCorrection=2000;
          Ser.print(":Mge"); Ser.print(finalCorrection); Ser.print("#");
          finalCorrection=-finalCorrection;
          if (correction<-2.0) correction+=2.0; else correction=0;
        } else 
          finalCorrection=0;
#endif
      }
    }

#ifdef AXIS1_ENC_RATE_CONTROL_ON
    void clearAverages() {
      double d=(Axis1EncRateComp*usPerTick)/axis1Rate;
      for (int i=0; i<AXIS1_ENC_INTPOL_PERIOD; i++) { StaBins[i]=d*AXIS1_ENC_INTPOL_PERIOD; LtaBins[i]=d*AXIS1_ENC_INTPOL_PERIOD; }
      Tsta=d;
      Tlta=d;
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
