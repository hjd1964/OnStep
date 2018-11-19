// -----------------------------------------------------------------------------------
// Timers and interrupt handling

#ifndef HAL_SLOW_PROCESSOR
// if we're running the motor timers 2x the MaxRate, these are the variables to keep track of that
volatile boolean clearAxis1 = true;
volatile boolean takeStepAxis1 = false;
volatile boolean clearAxis2 = true;
volatile boolean takeStepAxis2 = false;
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO)
volatile long modeAxis1_next=AXIS1_MODE;
volatile boolean gotoModeAxis1=false;
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO)
volatile long modeAxis2_next=AXIS2_MODE;
volatile boolean gotoModeAxis2=false;
#endif

volatile bool axis2Powered = true;

//--------------------------------------------------------------------------------------------------
// Set hardware timer rates

// set Timer1 master sidereal clock to interval (in microseconds*16)
volatile long isrTimerRateAxis1=0;
volatile long isrTimerRateAxis2=0;
volatile long runTimerRateAxis1=0;
volatile long runTimerRateAxis2=0;
void SiderealClockSetInterval(long iv) {
  if (trackingState==TrackingMoveTo) Timer1SetInterval(iv/100,PPSrateRatio); else Timer1SetInterval(iv/300,PPSrateRatio);

  isrTimerRateAxis1=0; // also force rate update for Axis1/2 timers so that PPS adjustments take hold immediately
  isrTimerRateAxis2=0;
}

// prepare to set Timer3 to interval (in microseconds*16), maximum time is about 134 seconds
volatile uint32_t nextAxis1Rate = 100000UL;
volatile uint16_t t3cnt = 0;
volatile uint16_t t3rep = 1;
volatile long timerDirAxis1 = 0;
volatile long thisTimerRateAxis1 = 10000UL;
volatile boolean fastAxis1 = false;
void timer3SetInterval(long iv) {
#ifdef HAL_FIXED_PRESCALE_16BIT_MOTOR_TIMERS
  iv=iv/8L;
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L) { t*=2; iv=i/t; if (t==4096) { iv=65535L; break; } }
  cli(); nextAxis1Rate=iv-1L; t3rep=t; fastAxis1=(t3rep==1); sei();
#elif defined(__ARM_STM32__)
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*8L) { t*=2; iv=i/t; if (t==4096) { iv=65535L*8L; break; } }
  cli(); nextAxis1Rate=((F_COMP/1000000.0) * (iv*0.0625) * 0.5); t3rep=t; fastAxis1=(t3rep==1); sei();
#else
  // 4.194 * 32 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*1024L) { t++; iv=i/t; if (t==32) { iv=65535L*1024L; break; } }
  cli(); nextAxis1Rate=((F_COMP/1000000.0) * (iv*0.0625) * 0.5 - 1.0); t3rep=t; fastAxis1=(t3rep==1); sei();
#endif
}

// prepare to set Timer4 to interval (in microseconds*16), maximum time is about 134 seconds
volatile uint32_t nextAxis2Rate = 100000UL;
volatile uint16_t t4cnt = 0;
volatile uint16_t t4rep = 1;
volatile long timerDirAxis2 = 0;
volatile long thisTimerRateAxis2 = 10000UL;
volatile boolean fastAxis2 = false;
void timer4SetInterval(long iv) {
#ifdef HAL_FIXED_PRESCALE_16BIT_MOTOR_TIMERS
  iv=iv/8L;
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L) { t*=2; iv=i/t; if (t==4096) { iv=65535L; break; } }
  cli(); nextAxis2Rate=iv-1L; t4rep=t; fastAxis2=(t4rep==1); sei();
#elif defined(__ARM_STM32__)
  // 0.0327 * 4096 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*8L) { t*=2; iv=i/t; if (t==4096) { iv=65535L*8L; break; } }
  cli(); nextAxis2Rate=((F_COMP/1000000.0) * (iv*0.0625) * 0.5); t4rep=t; fastAxis2=(t4rep==1); sei();
#else
  // 4.194 * 32 = 134.21s
  uint32_t i=iv; uint16_t t=1; while (iv>65536L*1024L) { t++; iv=i/t; if (t==32) { iv=65535L*1024L; break; } }
  cli(); nextAxis2Rate=((F_COMP/1000000.0) * (iv*0.0625) * 0.5 - 1.0); t4rep=t; fastAxis2=(t4rep==1); sei();
#endif
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and setting up the Axis1/2 intervals for later programming
volatile boolean wasInbacklashAxis1=false;
volatile boolean wasInbacklashAxis2=false;
volatile boolean gotoRateAxis1=false;
volatile boolean gotoRateAxis2=false;
volatile byte cnt=0;
volatile double guideTimerRateAxis1A=0.0;
volatile double guideTimerRateAxis2A=0.0;
volatile byte guideDirChangeTimerAxis1=0;
volatile byte lastGuideDirAxis1=0;
volatile byte guideDirChangeTimerAxis2=0;
volatile byte lastGuideDirAxis2=0;

#ifdef HAL_USE_NOBLOCK_FOR_TIMER1
ISR(TIMER1_COMPA_vect,ISR_NOBLOCK)
#else
IRAM_ATTR ISR(TIMER1_COMPA_vect)
#endif
{
#ifdef HAL_TIMER1_PREFIX
  HAL_TIMER1_PREFIX;
#endif

  // run 1/3 of the time at 3x the rate, unless a goto is happening
  if (trackingState!=TrackingMoveTo) { cnt++; if (cnt%3!=0) goto done; cnt=0; }
  lst++;

  // handle buzzer
  if (buzzerDuration>0) { buzzerDuration--; if (buzzerDuration==0) digitalWrite(TonePin,LOW); }

#ifndef ESP32
  timerSuper();
#endif

done: {}
#ifdef HAL_TIMER1_SUFFIX
  HAL_TIMER1_SUFFIX;
#endif
}

void timerSuper() {
  if (trackingState!=TrackingMoveTo) {
    // automatic rate calculation HA
    long calculatedTimerRateAxis1;

    // guide rate acceleration/deceleration and control
    if (guideDirAxis1) {
      if ((fabs(guideTimerRateAxis1)<2.0) && (fabs(guideTimerRateAxis1A)<2.0)) { 
        // slow speed guiding, no acceleration
        guideTimerRateAxis1A=guideTimerRateAxis1; 
        // break
        if (guideDirAxis1=='b') { guideDirAxis1=0; guideTimerRateAxis1=0.0; guideTimerRateAxis1A=0.0; }
      } else {
        // high speed guiding
        stepperModeGoto();

        // at higher step rates where torque is reduced make smaller rate changes
        double r=1.2-sqrt((abs(guideTimerRateAxis1A)/slewRateX));
        if (r<0.2) r=0.2; if (r>1.2) r=1.2;

        // acceleration/deceleration control
        if ((guideDirAxis1!=lastGuideDirAxis1) && (lastGuideDirAxis1!=0)) guideDirChangeTimerAxis1=25;
        lastGuideDirAxis1=guideDirAxis1;

        double gtr1=guideTimerRateAxis1; if (guideDirAxis1=='b') gtr1=0.0;
        if (guideDirChangeTimerAxis1>0) guideDirChangeTimerAxis1--; else {
          if (guideTimerRateAxis1A>gtr1) { guideTimerRateAxis1A-=(accXPerSec/100.0)*r; if (guideTimerRateAxis1A<gtr1) guideTimerRateAxis1A=gtr1; }
          if (guideTimerRateAxis1A<gtr1) { guideTimerRateAxis1A+=(accXPerSec/100.0)*r; if (guideTimerRateAxis1A>gtr1) guideTimerRateAxis1A=gtr1; }
        }

        // stop guiding
        if (guideDirAxis1=='b') {
          if (abs(guideTimerRateAxis1A)<0.001) { guideDirAxis1=0; lastGuideDirAxis1=0; guideTimerRateAxis1=0.0; guideTimerRateAxis1A=0.0; guideDirChangeTimerAxis1=0; if (!guideDirAxis2) stepperModeTracking(); }
        }
      }
    } else guideTimerRateAxis1A=0.0;

    double timerRateAxis1A=trackingTimerRateAxis1;
    double timerRateAxis1B=guideTimerRateAxis1A+pecTimerRateAxis1+timerRateAxis1A;
    if (timerRateAxis1B<-0.00001) { timerRateAxis1B=fabs(timerRateAxis1B); cli(); timerDirAxis1=-1; sei(); } else 
      if (timerRateAxis1B>0.00001) { cli(); timerDirAxis1=1; sei(); } else { cli(); timerDirAxis1=0; sei(); timerRateAxis1B=0.5; }
    calculatedTimerRateAxis1=round((double)SiderealRate/timerRateAxis1B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis1!=calculatedTimerRateAxis1) { timerRateAxis1=calculatedTimerRateAxis1; runTimerRateAxis1=calculatedTimerRateAxis1; }

    // automatic rate calculation Dec
    long calculatedTimerRateAxis2;

    // guide rate acceleration/deceleration
    if (guideDirAxis2) {
      if ((fabs(guideTimerRateAxis2)<2.0) && (fabs(guideTimerRateAxis2A)<2.0)) { 
        // slow speed guiding, no acceleration
        guideTimerRateAxis2A=guideTimerRateAxis2; 
        // break mode
        if (guideDirAxis2=='b') { guideDirAxis2=0; guideTimerRateAxis2=0.0; guideTimerRateAxis2A=0.0; }
      } else {
        // use acceleration
        stepperModeGoto();

        // at higher step rates where torque is reduced make smaller rate changes
        double r=1.2-sqrt((abs(guideTimerRateAxis2A)/slewRateX));
        if (r<0.2) r=0.2; if (r>1.2) r=1.2;

        // acceleration/deceleration control
        if ((guideDirAxis2!=lastGuideDirAxis2) && (lastGuideDirAxis2!=0)) guideDirChangeTimerAxis2=25;
        lastGuideDirAxis2=guideDirAxis2;

        double gtr2=guideTimerRateAxis2; if (guideDirAxis2=='b') gtr2=0.0;
        if (guideDirChangeTimerAxis2>0) guideDirChangeTimerAxis2--; else {
          if (guideTimerRateAxis2A>gtr2) { guideTimerRateAxis2A-=(accXPerSec/100.0)*r; if (guideTimerRateAxis2A<gtr2) guideTimerRateAxis2A=gtr2; }
          if (guideTimerRateAxis2A<gtr2) { guideTimerRateAxis2A+=(accXPerSec/100.0)*r; if (guideTimerRateAxis2A>gtr2) guideTimerRateAxis2A=gtr2; }
        }

        // stop guiding
        if (guideDirAxis2=='b') {
          if (abs(guideTimerRateAxis2A)<0.001) { guideDirAxis2=0; lastGuideDirAxis2=0; guideTimerRateAxis2=0.0; guideTimerRateAxis2A=0.0; guideDirChangeTimerAxis2=0; if (!guideDirAxis1) stepperModeTracking(); }
        }
      }
    } else guideTimerRateAxis2A=0.0;

    double timerRateAxis2A=trackingTimerRateAxis2;
    double timerRateAxis2B=guideTimerRateAxis2A+timerRateAxis2A;
    if (timerRateAxis2B<-0.0001) { timerRateAxis2B=fabs(timerRateAxis2B); cli(); timerDirAxis2=-1; sei(); } else
      if (timerRateAxis2B>0.0001) { cli(); timerDirAxis2=1; sei(); } else { cli(); timerDirAxis2=0; sei(); timerRateAxis2B=0.5; }
    calculatedTimerRateAxis2=round((double)SiderealRate/timerRateAxis2B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis2!=calculatedTimerRateAxis2) { timerRateAxis2=calculatedTimerRateAxis2; runTimerRateAxis2=calculatedTimerRateAxis2; }
  }
  
  thisTimerRateAxis1=timerRateAxis1;
  if (useTimerRateRatio) { thisTimerRateAxis2=(timerRateAxis2*timerRateRatio); } else { thisTimerRateAxis2=timerRateAxis2; }
  
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

  // set the rates
  if (thisTimerRateAxis1!=isrTimerRateAxis1) {
    timer3SetInterval(thisTimerRateAxis1/PPSrateRatio);
    isrTimerRateAxis1=thisTimerRateAxis1;
  }
  if (thisTimerRateAxis2!=isrTimerRateAxis2) {
    timer4SetInterval(thisTimerRateAxis2/PPSrateRatio);
    isrTimerRateAxis2=thisTimerRateAxis2;
  }
}

IRAM_ATTR ISR(TIMER3_COMPA_vect)
{
#ifdef HAL_TIMER3_PREFIX
  HAL_TIMER3_PREFIX;
#endif

  if (!fastAxis1) { t3cnt++; if (t3cnt%t3rep!=0) goto done; }

  StepPinAxis1_LOW;

#ifndef HAL_SLOW_PROCESSOR
  if (clearAxis1) {
    takeStepAxis1=false;
#endif

#if defined(AXIS1_MODE) && defined(AXIS1_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // switch micro-step mode
  if (gotoModeAxis1!=gotoRateAxis1) {
    // only when at an allowed position
    if ((gotoModeAxis1) || ((posAxis1+blAxis1)%AXIS1_STEP_GOTO==0)) {
      // switch mode
      if (gotoModeAxis1) { stepAxis1=1; modeAxis1_next=AXIS1_MODE; gotoModeAxis1=false; } else { stepAxis1=AXIS1_STEP_GOTO; modeAxis1_next=AXIS1_MODE_GOTO; gotoModeAxis1=true; }
      digitalWrite(Axis1_M0,(modeAxis1_next & 1));
      digitalWrite(Axis1_M1,(modeAxis1_next>>1 & 1));
      digitalWrite(Axis1_M2,(modeAxis1_next>>2 & 1));
    }
  }
#endif

#ifdef HAL_SLOW_PROCESSOR
  QuickSetIntervalAxis1(nextAxis1Rate*stepAxis1);
#endif

  if ((trackingState!=TrackingMoveTo) && (!inbacklashAxis1)) targetAxis1.part.m+=timerDirAxis1*stepAxis1;

  // move the RA/Azm stepper to the target
  if (posAxis1!=(long)targetAxis1.part.m) {

    // set direction
    if (posAxis1<(long)targetAxis1.part.m) dirAxis1=1; else dirAxis1=0;
    #ifdef AXIS1_REVERSE_ON
      if (defaultDirAxis1==dirAxis1) DirPinAxis1_LOW; else DirPinAxis1_HIGH;
    #else
      if (defaultDirAxis1==dirAxis1) DirPinAxis1_HIGH; else DirPinAxis1_LOW;
    #endif
  
    // telescope moves WEST with the sky, blAxis1 is the amount of EAST backlash
    if (dirAxis1==1) {
      if (blAxis1<backlashAxis1) { blAxis1+=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1+=stepAxis1; }
    } else {
      if (blAxis1>0)             { blAxis1-=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1-=stepAxis1; }
    }

#ifndef HAL_SLOW_PROCESSOR
      takeStepAxis1=true;
    }
    clearAxis1=false;
  } else { 
    if (takeStepAxis1) StepPinAxis1_HIGH;
    clearAxis1=true;

    QuickSetIntervalAxis1(nextAxis1Rate*stepAxis1);
  }
#else
    StepPinAxis1_HIGH;
  }
#endif

done: {}
#ifdef HAL_TIMER3_SUFFIX
  HAL_TIMER3_SUFFIX;
#endif
}

IRAM_ATTR ISR(TIMER4_COMPA_vect)
{
#ifdef HAL_TIMER4_PREFIX
  HAL_TIMER4_PREFIX;
#endif

  if (!fastAxis2) { t4cnt++; if (t4cnt%t4rep!=0) goto done; }

  StepPinAxis2_LOW;

#ifndef HAL_SLOW_PROCESSOR
  if (clearAxis2) {
    takeStepAxis2=false;
#endif

#if defined(AXIS2_MODE) && defined(AXIS2_MODE_GOTO) && !defined(MODE_SWITCH_BEFORE_SLEW_ON) && !defined(MODE_SWITCH_BEFORE_SLEW_SPI)
  // switch micro-step mode
  if (gotoModeAxis2!=gotoRateAxis2) {
    // only when at an allowed position
    if ((gotoModeAxis2) || ((posAxis2+blAxis2)%AXIS2_STEP_GOTO==0)) {
      // switch mode
      if (gotoModeAxis2) { stepAxis2=1; modeAxis2_next=AXIS2_MODE; gotoModeAxis2=false; } else { stepAxis2=AXIS2_STEP_GOTO; modeAxis2_next=AXIS2_MODE_GOTO; gotoModeAxis2=true; }
      digitalWrite(Axis2_M0,(modeAxis2_next & 1));
      digitalWrite(Axis2_M1,(modeAxis2_next>>1 & 1));
      digitalWrite(Axis2_M2,(modeAxis2_next>>2 & 1));
    }
  }
#endif

#ifdef HAL_SLOW_PROCESSOR
  QuickSetIntervalAxis2(nextAxis2Rate*stepAxis2);
#endif

  if ((trackingState!=TrackingMoveTo) && (!inbacklashAxis2)) targetAxis2.part.m+=timerDirAxis2*stepAxis2;

  // move the Dec/Alt stepper to the target
  if (axis2Powered && (posAxis2!=(long)targetAxis2.part.m)) {
    
    // set direction
    if (posAxis2<(long)targetAxis2.part.m) dirAxis2=1; else dirAxis2=0;
    #ifdef AXIS2_REVERSE_ON
      if (defaultDirAxis2==dirAxis2) DirPinAxis2_LOW; else DirPinAxis2_HIGH;
    #else
      if (defaultDirAxis2==dirAxis2) DirPinAxis2_HIGH; else DirPinAxis2_LOW;
    #endif
   
    // telescope moving toward celestial pole in the sky, blAxis2 is the amount of opposite backlash
    if (dirAxis2==1) {
      if (blAxis2<backlashAxis2) { blAxis2+=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2+=stepAxis2; }
    } else {
      if (blAxis2>0)             { blAxis2-=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2-=stepAxis2; }
    }

#ifndef HAL_SLOW_PROCESSOR
      takeStepAxis2=true;
    }
    clearAxis2=false;
  } else { 
    if (takeStepAxis2) StepPinAxis2_HIGH;
    clearAxis2=true;

    QuickSetIntervalAxis2(nextAxis2Rate*stepAxis2);
  }
#else
    StepPinAxis2_HIGH;
  }
#endif

done: {}
#ifdef HAL_TIMER4_SUFFIX
  HAL_TIMER4_SUFFIX;
#endif
}

#if defined(AXIS2_AUTO_POWER_DOWN_ON) && !defined(MOUNT_TYPE_ALTAZM)
// Auto power down the Dec motor
void autoPowerDownAxis2() {
  static long Axis2PowerOffTimer = 0;
  static long timerLastPosAxis2 = 0;
  if (axis2Enabled) {
    // timer count down
    if (Axis2PowerOffTimer>0) Axis2PowerOffTimer--; 

    // if the guide rate <= 1x and we're guiding on either axis set the timer to 10 minutes
    if ((fabs(guideTimerBaseRateAxis1)<=1.000001) && (guideDirAxis2 || guideDirAxis1)) Axis2PowerOffTimer=10L*60L*100L;

    // if Axis2 isn't stationary, or needs to move, set the timer to a minimum of 10 seconds
    cli(); 
    if (((posAxis2!=timerLastPosAxis2) || (posAxis2!=(long)targetAxis2.part.m)) && (Axis2PowerOffTimer<10*100)) { timerLastPosAxis2=posAxis2; Axis2PowerOffTimer=10*100; }

    // enable/disable Axis2
    if (Axis2PowerOffTimer==0) {
      if (axis2Powered && !takeStepAxis2) { axis2Powered=false; digitalWrite(Axis2_EN,AXIS2_DISABLE); }
    } else {
      if (!axis2Powered) { digitalWrite(Axis2_EN,AXIS2_ENABLE); axis2Powered=true; delayMicroseconds(10); }
    }
    sei();
  } else { Axis2PowerOffTimer=0; axis2Powered=true; }
}
#endif

#if defined(PPS_SENSE_ON) || defined(PPS_SENSE_PULLUP)
// PPS interrupt
void clockSync() {
  #define NUM_SECS_TO_AVERAGE 40
  unsigned long t=micros();
  unsigned long oneS=(t-PPSlastMicroS);
  if ((oneS>1000000-20000) && (oneS<1000000+20000)) {
    PPSavgMicroS=(PPSavgMicroS*(NUM_SECS_TO_AVERAGE-1)+oneS)/NUM_SECS_TO_AVERAGE;
    PPSsynced=true;
  } else PPSsynced=false;
  PPSlastMicroS=t;
}
#endif
