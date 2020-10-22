// ----------------------------------------------------------------------------------------------------------------
// Read CW/CCW type encoders

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

// ----------------------------------------------------------------------------------------------------------------
// this ISR function times the arrival of pulses from the RA axis encoder

#if AXIS1_ENC == CWCCW && AXIS1_ENC_RATE_CONTROL == ON
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
