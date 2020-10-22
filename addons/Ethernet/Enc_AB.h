// ----------------------------------------------------------------------------------------------------------------
// Read Quadrature A/B type encoders

// this is for Quadrature A/B type encoders (library based)
#if AXIS1_ENC_RATE_CONTROL != ON && (AXIS1_ENC == AB || AXIS2_ENC == AB)
  #include <Encoder.h> // from https://github.com/PaulStoffregen/Encoder 
#if AXIS1_ENC == AB
  Encoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN);
#endif
#if AXIS2_ENC == AB
  Encoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN);
#endif
#endif

// this is for Quadrature A/B type encoders (local)
//                 ______        ______       
//         A _____|      |______|      |______ A
// neg <--      ______        ______        __    --> pos
//         B __|      |______|      |______|   B

#if AXIS1_ENC_RATE_CONTROL == ON && (AXIS1_ENC == AB || AXIS2_ENC == AB)
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

// ----------------------------------------------------------------------------------------------------------------
// this ISR function times the arrival of pulses from the RA axis encoder

#if AXIS1_ENC == AB && AXIS1_ENC_RATE_CONTROL == ON
  static unsigned long lastLogRate=0;
  bool fastMotion() { return Telapsed<clocksPerTickMin; }
  bool slowMotion() { return (millis()-lastLogRate)>msPerTickMax; }
  
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

#endif
