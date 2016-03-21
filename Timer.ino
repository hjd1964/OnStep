// -----------------------------------------------------------------------------------
// Timers and interrupt handling

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);
volatile boolean clearAxis1 = true;
volatile boolean takeStepAxis1 = false;
volatile boolean clearAxis2 = true;
volatile boolean takeStepAxis2 = false;

#if defined(__arm__) && defined(TEENSYDUINO)
IntervalTimer itimer1;
#endif
// Energia does not have IntervalTimer so the timers were already initialised in OnStep.ino
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long stepAxis1=1;
volatile long modeAxis1_next=AXIS1_MODE;
volatile boolean gotoModeAxis1=false;
#else
#define stepAxis1 1
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
volatile long stepAxis2=1;
volatile long modeAxis2_next=AXIS2_MODE;
volatile boolean gotoModeAxis2=false;
#else
#define stepAxis2 1
#endif

//--------------------------------------------------------------------------------------------------
// set timer1 to rate (in microseconds*16)
void Timer1SetRate(long rate) {
#if defined(__AVR__)
  TCCR1B = 0; TCCR1A = 0;
  TIMSK1 = 0;
  rate=rate/PPSrateRatio;

  // set compare match register to desired timer count:
  if (rate<65536) { TCCR1B |= (1 << CS10); } else {
  rate=rate/8;
  if (rate<65536) { TCCR1B |= (1 << CS11); } else {
  rate=rate/8;
  if (rate<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS11); } else {
  rate=rate/4;  
  if (rate<65536) { TCCR1B |= (1 << CS12); } else {
  rate=rate/4;
  if (rate<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS12); 
  }}}}}
  
  OCR1A = rate-1;
  // CTC mode
  TCCR1B |= (1 << WGM12);
  // timer compare interrupt enable
  TIMSK1 |= (1 << OCIE1A);
#elif defined(__arm__) && defined(TEENSYDUINO)
  itimer1.begin(TIMER1_COMPA_vect, (float)rate * 0.0625);
#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  TimerLoadSet(Timer1_base, TIMER_A, (int) (F_BUS / 1000000 * rate * 0.0625));
#endif
}

// set the master sidereal clock rate, also forces rate update for RA/Dec timer rates so that PPS adjustments take hold immediately
volatile long isrTimerRateAxis1=0;
volatile long isrTimerRateAxis2=0;
volatile long runtimerRateAxis1=0;
volatile long runTimerRateAxis2=0;
void SetSiderealClockRate(long Interval) {
  if (trackingState==TrackingMoveTo) Timer1SetRate(Interval/100); else Timer1SetRate(Interval/300);
  isrTimerRateAxis1=0;
  isrTimerRateAxis2=0;
}

// set timer3 to rate (in microseconds*16)
volatile uint32_t nextAxis1Rate = 100000UL;
void Timer3SetRate(long rate) {
#if defined(__AVR__)
  // valid for rates down to 0.25 second, and cap the rate
  if (StepsPerSecondAxis1<31) rate=rate/64L; else rate=rate/8L;
  if (rate>65536L) rate=65536L;
  cli(); nextAxis1Rate=rate-1L; sei();
#elif (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
  cli(); nextAxis1Rate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1; sei();
#endif
}
// set timer4 to rate (in microseconds*16)
volatile uint32_t nextAxis2Rate = 100000UL;
void Timer4SetRate(long rate) {
#if defined(__AVR__)
  // valid for rates down to 0.25 second, and cap the rate
  if (StepsPerSecondAxis1<31) rate=rate/64L; else rate=rate/8L;
  if (rate>65536L) rate=65536L;
  cli(); nextAxis2Rate=rate-1L; sei();
#elif (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
  cli(); nextAxis2Rate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1; sei();
#endif
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and programming the drive rates
volatile boolean wasInbacklashAxis1=false;
volatile boolean wasInbacklashAxis2=false;
volatile boolean gotoRateAxis1=false;
volatile boolean gotoRateAxis2=false;
volatile byte cnt = 0;

volatile double guideTimerRateAxis1A=0;
volatile double guideTimerRateAxis2A=0;

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
ISR(TIMER1_COMPA_vect)
#else
ISR(TIMER1_COMPA_vect,ISR_NOBLOCK)
#endif
{
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  TimerIntClear( Timer1_base, TIMER_TIMA_TIMEOUT );
#endif

  // run 1/3 of the time at 3x the rate, unless a goto is happening
  if (trackingState!=TrackingMoveTo) { cnt++; if (cnt%3!=0) return; cnt=0; }
  lst++;

  if (trackingState!=TrackingMoveTo) {
    // automatic rate calculation HA
    long calculatedTimerRateAxis1;
    
    // guide rate acceleration/deceleration and control
    cli();  double x=((long)targetAxis1.part.m)-posAxis1; sei();
    if ((!inbacklashAxis1) && (guideDirAxis1)) {
      if ((fabs(guidetimerRateAxis1)<10.0) && (fabs(guideTimerRateAxis1A)<10.0)) { 
        // break mode
        if (guideDirAxis1=='b') { guidetimerRateAxis1=1.0; }
        // slow speed guiding, no acceleration
        guideTimerRateAxis1A=guidetimerRateAxis1; 
      } else {
        // use acceleration
        DecayModeGoto();
        double z=(StepsForRateChangeAxis1/isqrt32(fabs(x)));
        guideTimerRateAxis1A=(1.0/(((double)StepsPerDegreeAxis1*(z/1000000.0)))*3600.0);
        if (guideTimerRateAxis1A>fabs(guidetimerRateAxis1)) guideTimerRateAxis1A=fabs(guidetimerRateAxis1);
      }
      // stop guiding
      if ((guideDirAxis1=='b') && (fabs(x)<2)) { guideDirAxis1=0; guidetimerRateAxis1=0; guideTimerRateAxis1A=0; DecayModeTracking(); }
    }

    double timerRateAxis1A=trackingTimerRateAxis1; if (((guideDirAxis1) || (guideDirAxis2)) && (activeGuideRate>GuideRate1x)) timerRateAxis1A=0.0;
    double timerRateAxis1B=fabs(guideTimerRateAxis1A+pecTimerRateAxis1+timerRateAxis1A);
    // round up to run the motor timers just a tiny bit slow, then adjust below if we start to fall behind during sidereal tracking
    if (timerRateAxis1B>0.5) calculatedTimerRateAxis1=ceil((double)SiderealRate/timerRateAxis1B)+5; else calculatedTimerRateAxis1=ceil((double)SiderealRate*2.0);
    // remember our "running" rate and only update the actual rate when it changes
    if (runtimerRateAxis1!=calculatedTimerRateAxis1) { timerRateAxis1=calculatedTimerRateAxis1; runtimerRateAxis1=calculatedTimerRateAxis1; }

    // dynamic rate adjust
    // in pre-scaler /64 mode the motor timers might be slow (relative to the sidereal timer) by as much as 0.000004 seconds/second (16000000/64)
    // so a 0.01% (0.0001x) increase is always enough to correct for this, it happens very slowly - about a single step worth of movement over an hours time
    if (x>10.0) x=10.0;
    if (x<-10.0) x=-10.0;
    x=10000.00-x; x=x/10000.0;
    timerRateAxis1=calculatedTimerRateAxis1*x; // up to 0.01% faster or slower (or as little as 0.001%)
    runtimerRateAxis1=timerRateAxis1;

    // automatic rate calculation Dec
    long calculatedTimerRateAxis2;

    // guide rate acceleration/deceleration
    cli(); x=fabs((long)targetAxis2.part.m-posAxis2); sei();
    if (!inbacklashAxis2 && guideDirAxis2) {
      if ((fabs(guideTimerRateAxis2)<10.0) && (fabs(guideTimerRateAxis2A)<10.0)) { 
        // break mode
        if (guideDirAxis2=='b') guideTimerRateAxis2=1.0;
        // slow speed guiding, no acceleration
        guideTimerRateAxis2A=guideTimerRateAxis2; 
      } else {
        // use acceleration
        DecayModeGoto();
        double z=(StepsForRateChangeAxis2/isqrt32(x));
        guideTimerRateAxis2A=(1.0/(((double)StepsPerDegreeAxis2*(z/1000000.0)))*3600.0);
        if (guideTimerRateAxis2A>fabs(guideTimerRateAxis2)) guideTimerRateAxis2A=fabs(guideTimerRateAxis2);
      }
      // stop guiding
      if ((guideDirAxis2=='b') && (x<2)) { guideDirAxis2=0; guideTimerRateAxis2=0; guideTimerRateAxis2A=0; DecayModeTracking(); }
    }
       
    double timerRateAxis2A=trackingTimerRateAxis2; if (guideDirAxis2 && (activeGuideRate>GuideRate1x)) timerRateAxis2A=0.0;
    double timerRateAxis2B=fabs(guideTimerRateAxis2A+timerRateAxis2A);
    // round up to run the motor timers just a tiny bit slow, then adjust below if we start to fall behind during sidereal tracking
    if (timerRateAxis2B>0.5) calculatedTimerRateAxis2=ceil((double)SiderealRate/timerRateAxis2B)+5; else calculatedTimerRateAxis2=ceil((double)SiderealRate*2.0);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis2!=calculatedTimerRateAxis2) { timerRateAxis2=calculatedTimerRateAxis2; runTimerRateAxis2=calculatedTimerRateAxis2; }

    // dynamic rate adjust
    if (x>1.0) {
      x=x-1.0; if (x>10.0) x=10.0; x=10000.00-x; x=x/10000.0;
      timerRateAxis2=calculatedTimerRateAxis2*x; // up to 0.01% faster (or as little as 0.001%)
      runTimerRateAxis2=timerRateAxis2;
    }
  }
  
  long thisTimerRateAxis1=timerRateAxis1;
  long thisTimerRateAxis2;
  if (useTimerRateRatio) {
    thisTimerRateAxis2=(timerRateAxis2*timerRateRatio);
  } else {
    thisTimerRateAxis2=timerRateAxis2;
  }
  
  // override rate during backlash compensation
  if (inbacklashAxis1) { thisTimerRateAxis1=timerRateBacklashAxis1; wasInbacklashAxis1=true; } 
  // override rate during backlash compensation
  if (inbacklashAxis2) { thisTimerRateAxis2=timerRateBacklashAxis2; wasInbacklashAxis2=true; }
  if ((trackingState==TrackingSidereal)) {
    // travel through the backlash is done, but we weren't following the target while it was happening!
    // so now get us back to near where we need to be
    if ((!inbacklashAxis1) && (wasInbacklashAxis1) && (!guideDirAxis1)) {
      cli(); if (abs(posAxis1-((long)targetAxis1.part.m))>2) thisTimerRateAxis1=TakeupRate; else wasInbacklashAxis1=false; sei();
    }
    if ((!inbacklashAxis2) && (wasInbacklashAxis2) && (!guideDirAxis2)) {
      cli(); if (abs(posAxis2-(long)targetAxis2.part.m)>2) thisTimerRateAxis2=TakeupRate; else wasInbacklashAxis2=false; sei();
    }
  }

  if (trackingState==TrackingMoveTo) {
    // trigger Goto step mode, rapid acceleration (low StepsForRateChange) can leave too little time
    // until the home position arrives to actually switch to tracking micro-step mode. the larger step size
    // then causes backlash compensation to activate which in-turn keeps goto micro-step mode from turning off
    #if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
    gotoRateAxis2=(thisTimerRateAxis2<128*16L);   // activate <128us rate
    #endif
    #if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
    gotoRateAxis1=(thisTimerRateAxis1<128*16L);   // activate <128us rate
    #endif
  }
  
  // set the rates
  if (thisTimerRateAxis1!=isrTimerRateAxis1) {
    Timer3SetRate(thisTimerRateAxis1/PPSrateRatio);
    isrTimerRateAxis1=thisTimerRateAxis1;
  }
  if (thisTimerRateAxis2!=isrTimerRateAxis2) {
    Timer4SetRate(thisTimerRateAxis2/PPSrateRatio);
    isrTimerRateAxis2=thisTimerRateAxis2;
  }
}

ISR(TIMER3_COMPA_vect)
{
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  TimerIntClear( Timer3_base, TIMER_TIMA_TIMEOUT );
#endif

  // drivers step on the rising edge, need >=1.9uS to settle (for DRV8825 or A4988) so this is early in the routine
  CLR(Axis1StepPORT,  Axis1StepBit);

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
  // on the much faster Teensy and Tiva TM4C run this ISR at twice the normal rate and pull the step pin low every other call
  if (clearAxis1) {
    takeStepAxis1=false;
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
  // switch micro-step mode
  if (gotoModeAxis1!=gotoRateAxis1) {
    // only when at the home position
    if (((posAxis1+blAxis1)-trueAxis1)%256==0) { // was 1024 in support of 256x drivers... if they work like the DRV8825, A4988 this should still be ok
      // switch mode
      if (gotoModeAxis1) { stepAxis1=1; modeAxis1_next=AXIS1_MODE; gotoModeAxis1=false; } else { stepAxis1=AXIS1_STEP_GOTO; modeAxis1_next=AXIS1_MODE_GOTO; gotoModeAxis1=true; }
      digitalWrite(Axis1_M0,(modeAxis1_next & 1));
      digitalWrite(Axis1_M1,(modeAxis1_next>>1 & 1));
      digitalWrite(Axis1_M2,(modeAxis1_next>>2 & 1));
    }
  }
#endif

#if defined(__AVR__)
  OCR3A=nextAxis1Rate*stepAxis1;
#endif

  // Guessing about 4+4+1+ 4+4+1+ 1+ 2+1+2+ 13=37 clocks between here and the step signal which is 2.3uS
  if (posAxis1!=(long)targetAxis1.part.m) { // Move the RA stepper to the target
    if (posAxis1<(long)targetAxis1.part.m) dirAxis1=1; else dirAxis1=0; // Direction control
    #ifdef REVERSE_AXIS1_ON
      if (HADir==dirAxis1) CLR(Axis1DirPORT, Axis1DirBit); else SET(Axis1DirPORT, Axis1DirBit); // Set direction, HADir default LOW (=0, for my wiring.)  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
    #else                                                                                       // Guessing about 1+2+1+4+4+1=13 clocks between here and the step signal which is 0.81uS
      if (HADir==dirAxis1) SET(Axis1DirPORT, Axis1DirBit); else CLR(Axis1DirPORT, Axis1DirBit);
    #endif
    // telescope moves WEST with the sky, blAxis1 is the amount of EAST backlash
    if (dirAxis1==1) {
      if (blAxis1<backlashAxis1) { blAxis1+=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1+=stepAxis1; }
    } else {
      if (blAxis1>0)             { blAxis1-=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1-=stepAxis1; }
    }

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
      takeStepAxis1=true;
    }
    clearAxis1=false;
  } else { 
    if (takeStepAxis1) SET(Axis1StepPORT, Axis1StepBit); clearAxis1=true;

#if defined(__arm__) && defined(TEENSYDUINO)
    PIT_LDVAL1=nextAxis1Rate*stepAxis1;
#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
    TimerLoadSet(Timer3_base, TIMER_A, nextAxis1Rate*stepAxis1);
#endif
  }
#else
    SET(Axis1StepPORT, Axis1StepBit);
  }
#endif
}

ISR(TIMER4_COMPA_vect)
{
#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
  TimerIntClear( Timer4_base, TIMER_TIMA_TIMEOUT );
#endif

  // drivers step on the rising edge
  CLR(Axis2StepPORT,  Axis2StepBit);

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
  // on the much faster Teensy and Tiva TM4C run this ISR at twice the normal rate and pull the step pin low every other call
  if (clearAxis2) {
    takeStepAxis2=false;
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
  // switch micro-step mode
  if (gotoModeAxis2!=gotoRateAxis2) {
    // only when at home position
    if (((posAxis2+blAxis2)-trueAxis2)%256==0) { // was 1024 in support of 256x drivers... if they work like the DRV8825, A4988 this should still be ok
      // switch mode
      if (gotoModeAxis2) { stepAxis2=1; modeAxis2_next=AXIS2_MODE; gotoModeAxis2=false; } else { stepAxis2=AXIS2_STEP_GOTO; modeAxis2_next=AXIS2_MODE_GOTO; gotoModeAxis2=true; }
      digitalWrite(Axis2_M0,(modeAxis2_next & 1));
      digitalWrite(Axis2_M1,(modeAxis2_next>>1 & 1));
      digitalWrite(Axis2_M2,(modeAxis2_next>>2 & 1));
    }
  }
#endif

#if defined(__AVR__)
  OCR4A=nextAxis2Rate*stepAxis2;
#endif
  
  if (posAxis2!=(long)targetAxis2.part.m) { // move the Dec stepper to the target
    // telescope normally starts on the EAST side of the pier looking at the WEST sky
    if (posAxis2<(long)targetAxis2.part.m) dirAxis2=1; else dirAxis2=0; // Direction control
    #ifdef REVERSE_AXIS2_ON
      if (DecDir==dirAxis2) SET(Axis2DirPORT, Axis2DirBit); else CLR(Axis2DirPORT, Axis2DirBit); // Set direction, decDir default HIGH (=1, for my wiring)
    #else
      if (DecDir==dirAxis2) CLR(Axis2DirPORT, Axis2DirBit); else SET(Axis2DirPORT, Axis2DirBit);
    #endif
    // telescope moving toward celestial pole in the sky, blAxis2 is the amount of opposite backlash
    if (dirAxis2==1) {
      if (blAxis2<backlashAxis2) { blAxis2+=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2+=stepAxis2; }
    } else {
      if (blAxis2>0)             { blAxis2-=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2-=stepAxis2; }
    }

#if (defined(__arm__) && defined(TEENSYDUINO)) || (defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__))
      takeStepAxis2=true;
    }
    clearAxis2=false; 
  } else { 
    if (takeStepAxis2) SET(Axis2StepPORT, Axis2StepBit); clearAxis2=true;

#if defined(__arm__) && defined(TEENSYDUINO)
    PIT_LDVAL2=nextAxis2Rate*stepAxis2;
#elif defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
    TimerLoadSet(Timer4_base, TIMER_A, nextAxis2Rate*stepAxis2);
#endif
  }
#else
    SET(Axis2StepPORT, Axis2StepBit);
  }
#endif
}

#ifdef PPS_SENSE_ON
// PPS interrupt
void ClockSync() {
  unsigned long t=micros();
  unsigned long oneS=(t-PPSlastMicroS);
  if ((oneS>1000000-1000) && (oneS<1000000+1000)) {
    PPSavgMicroS=(PPSavgMicroS*19+oneS)/20;
    PPSsynced=true;
  }
  PPSlastMicroS=t;
}
#endif

#if defined(__AVR__)
// UART Receive Complete Interrupt Handler for Serial0
ISR(USART0_RX_vect)  {
  Serial_recv_buffer[Serial_recv_tail]=UDR0; 
  Serial_recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}

// UART Receive Complete Interrupt Handler for Serial1
ISR(USART1_RX_vect)  {
  Serial1_recv_buffer[Serial1_recv_tail]=UDR1; 
  Serial1_recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}
#endif

