// -----------------------------------------------------------------------------------
// Timers and interrupt handling

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
#define ISR(f) void f (void)
void TIMER1_COMPA_vect(void);
volatile boolean clearAxis1 = true;
volatile boolean takeStepAxis1 = false;
volatile boolean clearAxis2 = true;
volatile boolean takeStepAxis2 = false;

#if defined(__ARM_Teensy3__)
IntervalTimer itimer1;
#endif
// Energia does not have IntervalTimer so the timers were already initialised in OnStep.ino
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long modeAxis1_next=AXIS1_MODE;
volatile boolean gotoModeAxis1=false;
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
volatile long modeAxis2_next=AXIS2_MODE;
volatile boolean gotoModeAxis2=false;
#endif

#ifdef AUTO_POWER_DOWN_AXIS2_ON
volatile long Axis2PowerOffTimer = 0;
volatile bool axis2Powered = false;
#endif

//--------------------------------------------------------------------------------------------------
// set timer1 to interval (in microseconds*16)
void Timer1SetInterval(long interval) {
#if defined(__AVR_ATmega2560__)
  TCCR1B = 0; TCCR1A = 0;
  TIMSK1 = 0;
  interval=interval/PPSrateRatio;

  // set compare match register to desired timer count:
  if (interval<65536) { TCCR1B |= (1 << CS10); } else {
  interval=interval/8;
  if (interval<65536) { TCCR1B |= (1 << CS11); } else {
  interval=interval/8;
  if (interval<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS11); } else {
  interval=interval/4;  
  if (interval<65536) { TCCR1B |= (1 << CS12); } else {
  interval=interval/4;
  if (interval<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS12); 
  }}}}}
  
  OCR1A = interval-1;
  // CTC mode
  TCCR1B |= (1 << WGM12);
  // timer compare interrupt enable
  TIMSK1 |= (1 << OCIE1A);
#elif defined(__ARM_Teensy3__)
  itimer1.begin(TIMER1_COMPA_vect, (float)interval * 0.0625);
#elif defined(__ARM_TI_TM4C__)
  TimerLoadSet(Timer1_base, TIMER_A, (int)(F_BUS/1000000 * interval * 0.0625));
#endif
}

// set the master sidereal clock rate, also forces rate update for RA/Dec timer rates so that PPS adjustments take hold immediately
volatile long isrTimerRateAxis1=0;
volatile long isrTimerRateAxis2=0;
volatile long runTimerRateAxis1=0;
volatile long runTimerRateAxis2=0;
void SetSiderealClockRate(long Interval) {
  if (trackingState==TrackingMoveTo) Timer1SetInterval(Interval/100); else Timer1SetInterval(Interval/300);
  isrTimerRateAxis1=0;
  isrTimerRateAxis2=0;
}

// set timer3 to interval (in microseconds*16), maximum time is about 134 seconds
volatile uint32_t nextAxis1Rate = 100000UL;
volatile uint16_t t3cnt = 0;
volatile uint16_t t3rep = 1;
volatile long timerDirAxis1 = 0;
volatile long thisTimerRateAxis1 = 10000UL;
volatile boolean fastAxis1 = false;
void Timer3SetInterval(long interval) {
#if defined(__AVR_ATmega2560__)
  interval=interval/8L;
  // 0.0327 * 4096 = 134.21s
  long i=interval; uint16_t t=1; while (interval>65536L) { t*=2; interval=i/t; if (t==4096) { interval=65535L; break; } }
  cli(); nextAxis1Rate=interval-1L; t3rep=t; fastAxis1=(t3rep==1); sei();
#elif (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
  // 4.194 * 32 = 134.21s
  long i=interval; uint16_t t=1; while (interval>65536L*1024L) { t++; interval=i/t; if (t==32) { interval=65535L*1024L; break; } }
  cli(); nextAxis1Rate=(F_BUS/1000000) * (interval*0.0625) * 0.5 - 1; t3rep=t; fastAxis1=(t3rep==1); sei();
#endif
}

// set timer4 to interval (in microseconds*16), maximum time is about 134 seconds
volatile uint32_t nextAxis2Rate = 100000UL;
volatile uint16_t t4cnt = 0;
volatile uint16_t t4rep = 1;
volatile long timerDirAxis2 = 0;
volatile long thisTimerRateAxis2 = 10000UL;
volatile boolean fastAxis2 = false;
void Timer4SetInterval(long interval) {
#if defined(__AVR_ATmega2560__)
  interval=interval/8L;
  // 0.0327 * 4096 = 134.21s
  long i=interval; uint16_t t=1; while (interval>65536L) { t*=2; interval=i/t; if (t==4096) { interval=65535L; break; } }
  cli(); nextAxis2Rate=interval-1L; t4rep=t; fastAxis2=(t4rep==1); sei();
#elif (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
  // 4.194 * 32 = 134.21s
  long i=interval; uint16_t t=1; while (interval>65536L*1024L) { t++; interval=i/t; if (t==32) { interval=65535L*1024L; break; } }
  cli(); nextAxis2Rate=(F_BUS/1000000) * (interval*0.0625) * 0.5 - 1; t4rep=t; fastAxis2=(t4rep==1); sei();
#endif
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and programming the drive rates
volatile boolean wasInbacklashAxis1=false;
volatile boolean wasInbacklashAxis2=false;
volatile boolean gotoRateAxis1=false;
volatile boolean gotoRateAxis2=false;
volatile byte cnt = 0;
volatile double guideTimerRateAxis1A      = 0.0;
volatile double guideTimerRateAtBreakAxis1= 0.0;
volatile double guideTimerRateAxis2A      = 0.0;
volatile double guideTimerRateAtBreakAxis2= 0.0;
volatile long timerLastPosAxis2=0;

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
ISR(TIMER1_COMPA_vect)
#else
ISR(TIMER1_COMPA_vect,ISR_NOBLOCK)
#endif
{
#if defined(__ARM_TI_TM4C__)
  TimerIntClear( Timer1_base, TIMER_TIMA_TIMEOUT );
#endif

  // run 1/3 of the time at 3x the rate, unless a goto is happening
  if (trackingState!=TrackingMoveTo) { cnt++; if (cnt%3!=0) return; cnt=0; }
  lst++;

  if (trackingState!=TrackingMoveTo) {
    // automatic rate calculation HA
    long calculatedTimerRateAxis1;
    
    // guide rate acceleration/deceleration and control
    if (guideDirAxis1) {
      if ((fabs(guideTimerRateAxis1)<10.0) && (fabs(guideTimerRateAxis1A)<10.0)) { 
        // slow speed guiding, no acceleration
        guideTimerRateAxis1A=guideTimerRateAxis1; 
        // break
        if (guideDirAxis1=='b') { guideTimerRateAtBreakAxis1=0; guideDirAxis1=0; guideTimerRateAxis1=0; guideTimerRateAxis1A=0; }
      } else {
        // use acceleration
        DecayModeGoto();
        long t=(millis()-guideStartTimeAxis1);
        guideTimerRateAxis1A=sqrt(t/(((480.0*DegreesForAcceleration)/slewRateX)*1000.0))*fabs(slewRateX);
        if (guideTimerRateAxis1A>fabs(guideTimerRateAxis1)) guideTimerRateAxis1A=fabs(guideTimerRateAxis1);
        if (guideTimerRateAxis1<0) guideTimerRateAxis1A=-guideTimerRateAxis1A;
        // stop guiding
        if (guideDirAxis1=='b') {
          if (guideTimerRateAtBreakAxis1==0) guideTimerRateAtBreakAxis1=fabs(guideTimerRateAxis1A);
          long t=(((480.0*DegreesForAcceleration)/slewRateX)*1000.0)*sq(guideTimerRateAtBreakAxis1/fabs(slewRateX))-(millis()-guideBreakTimeAxis1); if (t<0) t=0;
          guideTimerRateAxis1A=sqrt(t/(((480.0*DegreesForAcceleration)/slewRateX)*1000.0))*fabs(slewRateX);
          if (guideTimerRateAxis1<0) guideTimerRateAxis1A=-guideTimerRateAxis1A;
          if (abs(guideTimerRateAxis1A)<1.0) { guideTimerRateAtBreakAxis1=0; guideDirAxis1=0; guideTimerRateAxis1=0; guideTimerRateAxis1A=0; if (!guideDirAxis2) DecayModeTracking(); }
        }
      }
    } else { guideTimerRateAxis1A=0; }

    double timerRateAxis1A=trackingTimerRateAxis1;
    double timerRateAxis1B=guideTimerRateAxis1A+pecTimerRateAxis1+timerRateAxis1A;
    if (timerRateAxis1B<0) { timerRateAxis1B=fabs(timerRateAxis1B); cli(); timerDirAxis1=-1; sei(); } else { cli(); timerDirAxis1=1; sei(); }
    if (timerRateAxis1B<0.00001) { timerDirAxis1=0; timerRateAxis1B=0.5; }
    calculatedTimerRateAxis1=round((double)SiderealRate/timerRateAxis1B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis1!=calculatedTimerRateAxis1) { timerRateAxis1=calculatedTimerRateAxis1; runTimerRateAxis1=calculatedTimerRateAxis1; }

    // automatic rate calculation Dec
    long calculatedTimerRateAxis2;

    // guide rate acceleration/deceleration
    if (guideDirAxis2) {
      if ((fabs(guideTimerRateAxis2)<10.0) && (fabs(guideTimerRateAxis2A)<10.0)) { 
        // slow speed guiding, no acceleration
        guideTimerRateAxis2A=guideTimerRateAxis2; 
        // break mode
        if (guideDirAxis2=='b') { guideTimerRateAtBreakAxis2=0; guideDirAxis2=0; guideTimerRateAxis2=0; guideTimerRateAxis2A=0; }
      } else {
        // use acceleration
        DecayModeGoto();
        long t=(millis()-guideStartTimeAxis2);
        guideTimerRateAxis2A=sqrt(t/(((480.0*DegreesForAcceleration)/slewRateX)*1000.0))*fabs(slewRateX);
        if (guideTimerRateAxis2A>fabs(guideTimerRateAxis2)) guideTimerRateAxis2A=fabs(guideTimerRateAxis2);
        if (guideTimerRateAxis2<0) guideTimerRateAxis2A=-guideTimerRateAxis2A;
        // stop guiding
        if (guideDirAxis2=='b') {
          if (guideTimerRateAtBreakAxis2==0) guideTimerRateAtBreakAxis2=fabs(guideTimerRateAxis2);
          long t=(((480.0*DegreesForAcceleration)/slewRateX)*1000.0)*sq(guideTimerRateAtBreakAxis2/fabs(slewRateX))-(millis()-guideBreakTimeAxis2); if (t<0) t=0;
          guideTimerRateAxis2A=sqrt(t/(((480.0*DegreesForAcceleration)/slewRateX)*1000.0))*fabs(slewRateX);
          if (guideTimerRateAxis2<0) guideTimerRateAxis2A=-guideTimerRateAxis2A;
          if (abs(guideTimerRateAxis2A)<1.0) { guideTimerRateAtBreakAxis2=0; guideDirAxis2=0; guideTimerRateAxis2=0; guideTimerRateAxis2A=0; if (!guideDirAxis1) DecayModeTracking(); }
        }
      }
    } else guideTimerRateAxis2A=0;

    double timerRateAxis2A=trackingTimerRateAxis2;
    double timerRateAxis2B=guideTimerRateAxis2A+timerRateAxis2A;
    if (timerRateAxis2B<0) { timerRateAxis2B=fabs(timerRateAxis2B); cli(); timerDirAxis2=-1; sei(); } else { cli(); timerDirAxis2=1; sei(); }
    if (timerRateAxis2B<0.0001) { timerDirAxis2=0; timerRateAxis2B=0.5; }
    calculatedTimerRateAxis2=round((double)SiderealRate/timerRateAxis2B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis2!=calculatedTimerRateAxis2) { timerRateAxis2=calculatedTimerRateAxis2; runTimerRateAxis2=calculatedTimerRateAxis2; }
  }
  
  thisTimerRateAxis1=timerRateAxis1;
  thisTimerRateAxis2; if (useTimerRateRatio) { thisTimerRateAxis2=(timerRateAxis2*timerRateRatio); } else { thisTimerRateAxis2=timerRateAxis2; }
  
  // override rate during backlash compensation
  if (inbacklashAxis1) { thisTimerRateAxis1=timerRateBacklashAxis1; wasInbacklashAxis1=true; }
  // override rate during backlash compensation
  if (inbacklashAxis2) { thisTimerRateAxis2=timerRateBacklashAxis2; wasInbacklashAxis2=true; }

  // trigger Goto step mode, rapid acceleration (low DegreesForAcceleration) can leave too little time
  // until the home position arrives to actually switch to tracking micro-step mode. the larger step size
  // then causes backlash compensation to activate which in-turn keeps goto micro-step mode from turning off
  #if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  gotoRateAxis1=(thisTimerRateAxis1<128*16L);   // activate <128us rate
  #endif
  #if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  gotoRateAxis2=(thisTimerRateAxis2<128*16L);   // activate <128us rate
  #endif

#if defined(AUTO_POWER_DOWN_AXIS2_ON) && !defined(MOUNT_TYPE_ALTAZM)
    // ------------------------------------------------------------------------------------------------------------------------------------
    // Power down the Dec motor
    
  if (axis2Enabled) {
    // timer count down
    if (Axis2PowerOffTimer>0) Axis2PowerOffTimer--; 

    // if the guide rate <= 1x and we're guiding on either axis set the timer to 10 minutes
    if ((fabs(guideTimerBaseRate)<=1.000001) && (guideDirAxis2 || guideDirAxis1)) Axis2PowerOffTimer=10*60*100;

    // if Axis2 isn't stationary set the timer to a minimum of 10 seconds
    cli(); if ((posAxis2!=timerLastPosAxis2) && (Axis2PowerOffTimer<10*100)) { timerLastPosAxis2=posAxis2; Axis2PowerOffTimer=10*100; } sei();

    // enable/disable Axis2
    if (Axis2PowerOffTimer==0) {
      if (axis2Powered) { digitalWrite(Axis2_EN,Axis2_Disabled); axis2Powered=false; }
    } else {
      if (!axis2Powered) { cli(); digitalWrite(Axis2_EN,Axis2_Enabled); axis2Powered=true; delayMicroseconds(10); sei(); }
    }
  } else { Axis2PowerOffTimer=0; axis2Powered=true; }
    // ------------------------------------------------------------------------------------------------------------------------------------
#endif

  // set the rates
  if (thisTimerRateAxis1!=isrTimerRateAxis1) {
    Timer3SetInterval(thisTimerRateAxis1/PPSrateRatio);
    isrTimerRateAxis1=thisTimerRateAxis1;
  }
  if (thisTimerRateAxis2!=isrTimerRateAxis2) {
    Timer4SetInterval(thisTimerRateAxis2/PPSrateRatio);
    isrTimerRateAxis2=thisTimerRateAxis2;
  }
}

ISR(TIMER3_COMPA_vect)
{
#if defined(__ARM_TI_TM4C__)
  TimerIntClear( Timer3_base, TIMER_TIMA_TIMEOUT );
#endif

  if (!fastAxis1) { t3cnt++; if (t3cnt%t3rep!=0) return; }

  // drivers step on the rising edge, need >=1.9uS to settle (for DRV8825 or A4988) so this is early in the routine
#if defined(__ARM_Teensy3__)
  digitalWriteFast(Axis1StepPin,LOW);
#else
  CLR(Axis1StepPORT,Axis1StepBit);
#endif

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
  // on the much faster Teensy and Tiva TM4C run this ISR at twice the normal rate and pull the step pin low every other call
  if (clearAxis1) {
    takeStepAxis1=false;
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // switch micro-step mode
  if (gotoModeAxis1!=gotoRateAxis1) {
    // only when at the home position
    if (((posAxis1+blAxis1)-trueAxis1)%128==0) { // good for up to 128x micro-stepping
      // switch mode
      if (gotoModeAxis1) { stepAxis1=1; modeAxis1_next=AXIS1_MODE; gotoModeAxis1=false; } else { stepAxis1=AXIS1_STEP_GOTO; modeAxis1_next=AXIS1_MODE_GOTO; gotoModeAxis1=true; }
      digitalWrite(Axis1_M0,(modeAxis1_next & 1));
      digitalWrite(Axis1_M1,(modeAxis1_next>>1 & 1));
      digitalWrite(Axis1_M2,(modeAxis1_next>>2 & 1));
    }
  }
#endif

#if defined(__AVR_ATmega2560__)
  OCR3A=nextAxis1Rate*stepAxis1;
#endif

  if ((trackingState!=TrackingMoveTo) && (!inbacklashAxis1)) targetAxis1.part.m+=timerDirAxis1*stepAxis1;

  // Guessing about 4+4+1+ 4+4+1+ 1+ 2+1+2+ 13=37 clocks between here and the step signal which is 2.3uS
  if (posAxis1!=(long)targetAxis1.part.m) { // Move the RA stepper to the target
    if (posAxis1<(long)targetAxis1.part.m) dirAxis1=1; else dirAxis1=0; // Direction control

    // Guessing about 1+2+1+4+4+1=13 clocks between here and the step signal which is 0.81uS
    // Set direction.  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
#if defined(__ARM_Teensy3__)
    #ifdef REVERSE_AXIS1_ON
      if (defaultDirAxis1==dirAxis1) digitalWriteFast(Axis1DirPin, LOW); else digitalWriteFast(Axis1DirPin, HIGH);
    #else                                                                                                 
      if (defaultDirAxis1==dirAxis1) digitalWriteFast(Axis1DirPin, HIGH); else digitalWriteFast(Axis1DirPin, LOW);
    #endif
#else
    #ifdef REVERSE_AXIS1_ON
      if (defaultDirAxis1==dirAxis1) CLR(Axis1DirPORT, Axis1DirBit); else SET(Axis1DirPORT, Axis1DirBit);
    #else
      if (defaultDirAxis1==dirAxis1) SET(Axis1DirPORT, Axis1DirBit); else CLR(Axis1DirPORT, Axis1DirBit);
    #endif
#endif
  
    // telescope moves WEST with the sky, blAxis1 is the amount of EAST backlash
    if (dirAxis1==1) {
      if (blAxis1<backlashAxis1) { blAxis1+=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1+=stepAxis1; }
    } else {
      if (blAxis1>0)             { blAxis1-=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1-=stepAxis1; }
    }

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
      takeStepAxis1=true;
    }
    clearAxis1=false;
  } else { 
    if (takeStepAxis1) {
#if defined(__ARM_Teensy3__)
      digitalWriteFast(Axis1StepPin,HIGH);
#else
      SET(Axis1StepPORT, Axis1StepBit); 
#endif
    }
    clearAxis1=true;

#if defined(__ARM_Teensy3__)
    PIT_LDVAL1=nextAxis1Rate*stepAxis1;
#elif defined(__ARM_TI_TM4C__)
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
#if defined(__ARM_TI_TM4C__)
  TimerIntClear( Timer4_base, TIMER_TIMA_TIMEOUT );
#endif

  if (!fastAxis2) { t4cnt++; if (t4cnt%t4rep!=0) return; }

#if defined(__ARM_Teensy3__)
  digitalWriteFast(Axis2StepPin,LOW);
#else
  // drivers step on the rising edge
  CLR(Axis2StepPORT,Axis2StepBit);
#endif

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
  // on the much faster Teensy and Tiva TM4C run this ISR at twice the normal rate and pull the step pin low every other call
  if (clearAxis2) {
    takeStepAxis2=false;
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // switch micro-step mode
  if (gotoModeAxis2!=gotoRateAxis2) {
    // only when at home position
    if (((posAxis2+blAxis2)-trueAxis2)%128==0) { // good for up to 128x micro-stepping
      // switch mode
      if (gotoModeAxis2) { stepAxis2=1; modeAxis2_next=AXIS2_MODE; gotoModeAxis2=false; } else { stepAxis2=AXIS2_STEP_GOTO; modeAxis2_next=AXIS2_MODE_GOTO; gotoModeAxis2=true; }
      digitalWrite(Axis2_M0,(modeAxis2_next & 1));
      digitalWrite(Axis2_M1,(modeAxis2_next>>1 & 1));
      digitalWrite(Axis2_M2,(modeAxis2_next>>2 & 1));
    }
  }
#endif

#if defined(__AVR_ATmega2560__)
  OCR4A=nextAxis2Rate*stepAxis2;
#endif

  if ((trackingState!=TrackingMoveTo) && (!inbacklashAxis2)) targetAxis2.part.m+=timerDirAxis2*stepAxis2;

  if (posAxis2!=(long)targetAxis2.part.m) { // move the Dec stepper to the target
    // telescope normally starts on the EAST side of the pier looking at the WEST sky
    if (posAxis2<(long)targetAxis2.part.m) dirAxis2=1; else dirAxis2=0; // Direction control
    // Set direction.  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
#if defined(__ARM_Teensy3__)
    #ifdef REVERSE_AXIS2_ON
      if (defaultDirAxis2==dirAxis2) digitalWriteFast(Axis2DirPin, LOW); else digitalWriteFast(Axis2DirPin, HIGH);
    #else
      if (defaultDirAxis2==dirAxis2) digitalWriteFast(Axis2DirPin, HIGH); else digitalWriteFast(Axis2DirPin, LOW);
    #endif
#else
    #ifdef REVERSE_AXIS2_ON
      if (defaultDirAxis2==dirAxis2) SET(Axis2DirPORT, Axis2DirBit); else CLR(Axis2DirPORT, Axis2DirBit);
    #else
      if (defaultDirAxis2==dirAxis2) CLR(Axis2DirPORT, Axis2DirBit); else SET(Axis2DirPORT, Axis2DirBit);
    #endif
#endif
   
    // telescope moving toward celestial pole in the sky, blAxis2 is the amount of opposite backlash
    if (dirAxis2==1) {
      if (blAxis2<backlashAxis2) { blAxis2+=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2+=stepAxis2; }
    } else {
      if (blAxis2>0)             { blAxis2-=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2-=stepAxis2; }
    }

#if (defined(__ARM_Teensy3__) || defined(__ARM_TI_TM4C__))
      takeStepAxis2=true;
    }
    clearAxis2=false; 
  } else { 
    if (takeStepAxis2) { 
#if defined(__ARM_Teensy3__)
      digitalWriteFast(Axis2StepPin,HIGH);
#else
      SET(Axis2StepPORT, Axis2StepBit);
#endif
    }
    clearAxis2=true; 

#if defined(__ARM_Teensy3__)
    PIT_LDVAL2=nextAxis2Rate*stepAxis2;
#elif defined(__ARM_TI_TM4C__)
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

#if defined(__AVR_ATmega2560__)
// UART Receive Complete Interrupt Handler for Serial0
ISR(USART0_RX_vect)  {
  PSerial._recv_buffer[PSerial._recv_tail]=UDR0; 
  PSerial._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}

// UART Receive Complete Interrupt Handler for Serial1
ISR(USART1_RX_vect)  {
  PSerial1._recv_buffer[PSerial1._recv_tail]=UDR1; 
  PSerial1._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}
#endif


