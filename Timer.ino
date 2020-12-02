// -----------------------------------------------------------------------------------
// Timers and interrupt handling

#if STEP_WAVE_FORM == PULSE
  // motor timers at 1x rate
  #define TIMER_PULSE_STEP_MULTIPLIER 1
#elif STEP_WAVE_FORM == DEDGE
  // motor timers at 1x rate
  #define TIMER_PULSE_STEP_MULTIPLIER 1
  volatile byte toggleStateAxis1 = 0;
  volatile byte toggleStateAxis2 = 0;
#else
  // motor timers at 2x rate
  #define TIMER_PULSE_STEP_MULTIPLIER 0.5
  volatile boolean clearAxis1 = true;
  volatile boolean takeStepAxis1 = false;
  volatile boolean clearAxis2 = true;
#endif
  volatile boolean takeStepAxis2 = false;

#if defined(AXIS1_DRIVER_CODE) && defined(AXIS1_DRIVER_CODE_GOTO)
  volatile long AXIS1_DRIVER_CODE_NEXT=AXIS1_DRIVER_CODE;
  volatile boolean gotoModeAxis1=false;
#endif

#if defined(AXIS2_DRIVER_CODE) && defined(AXIS2_DRIVER_CODE_GOTO)
  volatile long AXIS2_DRIVER_CODE_NEXT=AXIS2_DRIVER_CODE;
  volatile boolean gotoModeAxis2=false;
#endif

volatile bool axis2Powered = true;

//--------------------------------------------------------------------------------------------------
// Hardware timer rates

volatile long isrTimerRateAxis1=0;
volatile long isrTimerRateAxis2=0;
volatile long runTimerRateAxis1=0;
volatile long runTimerRateAxis2=0;

volatile uint32_t nextAxis1Rate = 100000UL;
volatile uint16_t nextAxis1Rep = 1;
#if defined(AXIS1_DRIVER_CODE_GOTO)
volatile uint32_t nextAxis1GotoRate = 100000UL;
volatile uint16_t nextAxis1GotoRep = 1;
#endif
volatile long timerDirAxis1 = 0;
volatile long thisTimerRateAxis1 = 10000UL;

volatile uint32_t nextAxis2Rate = 100000UL;
volatile uint16_t nextAxis2Rep = 1;
#if defined(AXIS2_DRIVER_CODE_GOTO)
volatile uint32_t nextAxis2GotoRate = 100000UL;
volatile uint16_t nextAxis2GotoRep = 1;
#endif
volatile long timerDirAxis2 = 0;
volatile long thisTimerRateAxis2 = 10000UL;

// set Timer1 master sidereal clock to interval (in microseconds*16)
void SiderealClockSetInterval(long iv) {
  if (trackingState == TrackingMoveTo) Timer1SetInterval(iv/100,PPSrateRatio); else Timer1SetInterval(iv/300,PPSrateRatio);

  isrTimerRateAxis1=0; // also force rate update for Axis1/2 timers so that PPS adjustments take hold immediately
  isrTimerRateAxis2=0;
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and setting up the Axis1/2 intervals for later programming
volatile boolean gotoRateAxis1=false;
volatile boolean gotoRateAxis2=false;
volatile byte siderealClockCycleCount=0;
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

  // run at 3x the rate, unless a goto is happening
  bool centiSecond=true;
  if (trackingState != TrackingMoveTo) {
    siderealClockCycleCount++; 
    if (siderealClockCycleCount%3 != 0) {
      centiSecond=false;
#ifndef HAL_FAST_PROCESSOR
      goto done;
#endif
    } else siderealClockCycleCount=0;
  }
  
  if (centiSecond) {
    lst++;
    // handle buzzer
    if (buzzerDuration > 0) { buzzerDuration--; if (buzzerDuration == 0) digitalWrite(TonePin,LOW); }
  }

#ifndef ESP32
  timerSupervisor(centiSecond);
#endif

#ifndef HAL_FAST_PROCESSOR
done: {}
#endif

#ifdef HAL_TIMER1_SUFFIX
  HAL_TIMER1_SUFFIX;
#endif
}

void timerSupervisor(bool isCentiSecond) {
  if (trackingState != TrackingMoveTo) {
    // automatic rate calculation HA
    long calculatedTimerRateAxis1;

    // guide rate acceleration/deceleration and control
    if (guideDirAxis1) {
      if ((fabs(guideTimerRateAxis1) < 10.0) && (fabs(guideTimerRateAxis1A) < 10.0)) {
        // slow speed guiding, no acceleration
        guideTimerRateAxis1A=guideTimerRateAxis1; 
        // break
        if (guideDirAxis1 == 'b') { guideDirAxis1=0; guideTimerRateAxis1=0.0; guideTimerRateAxis1A=0.0; }
      } else {
        if ((isCentiSecond) && (!inbacklashAxis1)) {
          // high speed guiding
          stepperModeGoto();

          // at higher step rates where torque is reduced make smaller rate changes
          double r=1.2-sqrt((abs(guideTimerRateAxis1A)/slewRateX));
          if (r < 0.2) r=0.2; if (r > 1.2) r=1.2;
  
          // acceleration/deceleration control
          if ((guideDirAxis1 != lastGuideDirAxis1) && (lastGuideDirAxis1 != 0)) guideDirChangeTimerAxis1=25;
          lastGuideDirAxis1=guideDirAxis1;
  
          double gtr1=guideTimerRateAxis1; if (guideDirAxis1 == 'b') gtr1=0.0;
          if (guideDirChangeTimerAxis1 > 0) guideDirChangeTimerAxis1--; else {
            if (guideTimerRateAxis1A > gtr1) { guideTimerRateAxis1A-=(accXPerSec/100.0)*r; if (guideTimerRateAxis1A < gtr1) guideTimerRateAxis1A=gtr1; }
            if (guideTimerRateAxis1A < gtr1) { guideTimerRateAxis1A+=(accXPerSec/100.0)*r; if (guideTimerRateAxis1A > gtr1) guideTimerRateAxis1A=gtr1; }
          }
  
          // stop guiding
          if (guideDirAxis1 == 'b') {
            if (abs(guideTimerRateAxis1A) < 0.001) { guideDirAxis1=0; lastGuideDirAxis1=0; guideTimerRateAxis1=0.0; guideTimerRateAxis1A=0.0; guideDirChangeTimerAxis1=0; if (!guideDirAxis2) stepperModeTracking(false); }
          }
        }
      }
    } else guideTimerRateAxis1A=0.0;

    double timerRateAxis1A=trackingTimerRateAxis1;
    double timerRateAxis1B=guideTimerRateAxis1A+pecTimerRateAxis1+timerRateAxis1A;
    if (timerRateAxis1B < -0.00001) { timerRateAxis1B=fabs(timerRateAxis1B); cli(); timerDirAxis1=-1; sei(); } else 
      if (timerRateAxis1B > 0.00001) { cli(); timerDirAxis1=1; sei(); } else { cli(); timerDirAxis1=0; sei(); timerRateAxis1B=1.0; }
    calculatedTimerRateAxis1=round((double)SiderealRate/timerRateAxis1B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis1 != calculatedTimerRateAxis1) { timerRateAxis1=calculatedTimerRateAxis1; runTimerRateAxis1=calculatedTimerRateAxis1; }

    // automatic rate calculation Dec
    long calculatedTimerRateAxis2;

    // guide rate acceleration/deceleration
    if (guideDirAxis2) {
      if ((fabs(guideTimerRateAxis2) < 10.0) && (fabs(guideTimerRateAxis2A) < 10.0)) {
        // slow speed guiding, no acceleration
        guideTimerRateAxis2A=guideTimerRateAxis2; 
        // break mode
        if (guideDirAxis2 == 'b') { guideDirAxis2=0; guideTimerRateAxis2=0.0; guideTimerRateAxis2A=0.0; }
      } else {
        if ((isCentiSecond) && (!inbacklashAxis2)) {
          // use acceleration
          stepperModeGoto();
  
          // at higher step rates where torque is reduced make smaller rate changes
          double r=1.2-sqrt((abs(guideTimerRateAxis2A)/slewRateX));
          if (r < 0.2) r=0.2; if (r > 1.2) r=1.2;
  
          // acceleration/deceleration control
          if ((guideDirAxis2 != lastGuideDirAxis2) && (lastGuideDirAxis2 != 0)) guideDirChangeTimerAxis2=25;
          lastGuideDirAxis2=guideDirAxis2;
  
          double gtr2=guideTimerRateAxis2; if (guideDirAxis2 == 'b') gtr2=0.0;
          if (guideDirChangeTimerAxis2 > 0) guideDirChangeTimerAxis2--; else {
            if (guideTimerRateAxis2A > gtr2) { guideTimerRateAxis2A-=(accXPerSec/100.0)*r; if (guideTimerRateAxis2A < gtr2) guideTimerRateAxis2A=gtr2; }
            if (guideTimerRateAxis2A < gtr2) { guideTimerRateAxis2A+=(accXPerSec/100.0)*r; if (guideTimerRateAxis2A > gtr2) guideTimerRateAxis2A=gtr2; }
          }
  
          // stop guiding
          if (guideDirAxis2 == 'b') {
            if (abs(guideTimerRateAxis2A) < 0.001) { guideDirAxis2=0; lastGuideDirAxis2=0; guideTimerRateAxis2=0.0; guideTimerRateAxis2A=0.0; guideDirChangeTimerAxis2=0; if (!guideDirAxis1) stepperModeTracking(false); }
          }
        }
      }
    } else guideTimerRateAxis2A=0.0;

    double timerRateAxis2A=trackingTimerRateAxis2;
    double timerRateAxis2B=guideTimerRateAxis2A+timerRateAxis2A;
    if (timerRateAxis2B < -0.0001) { timerRateAxis2B=fabs(timerRateAxis2B); cli(); timerDirAxis2=-1; sei(); } else
      if (timerRateAxis2B > 0.0001) { cli(); timerDirAxis2=1; sei(); } else { cli(); timerDirAxis2=0; sei(); timerRateAxis2B=1.0; }
    calculatedTimerRateAxis2=round((double)SiderealRate/timerRateAxis2B);
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis2 != calculatedTimerRateAxis2) { timerRateAxis2=calculatedTimerRateAxis2; runTimerRateAxis2=calculatedTimerRateAxis2; }
  }
  
  thisTimerRateAxis1=timerRateAxis1;
  if (useTimerRateRatio) { thisTimerRateAxis2=(timerRateAxis2*timerRateRatio); } else { thisTimerRateAxis2=timerRateAxis2; }
  
  // override rate during backlash compensation
  if (inbacklashAxis1) thisTimerRateAxis1=timerRateBacklashAxis1;
  // override rate during backlash compensation
  if (inbacklashAxis2) thisTimerRateAxis2=timerRateBacklashAxis2;

  // trigger Goto step mode, rapid acceleration (low SLEW_ACCELERATION_DIST) can leave too little time
  // until the home position arrives to actually switch to tracking micro-step mode. the larger step size
  // then causes backlash compensation to activate which in-turn keeps goto micro-step mode from turning off
  #if defined(AXIS1_DRIVER_CODE) && defined(AXIS1_DRIVER_CODE_GOTO) && MODE_SWITCH_BEFORE_SLEW == OFF
  gotoRateAxis1=(thisTimerRateAxis1 < 128*16L);   // activate < 128us rate
  #endif
  #if defined(AXIS2_DRIVER_CODE) && defined(AXIS2_DRIVER_CODE_GOTO) && MODE_SWITCH_BEFORE_SLEW == OFF
  gotoRateAxis2=(thisTimerRateAxis2 < 128*16L);   // activate < 128us rate
  #endif

  // set the rates
  if (thisTimerRateAxis1 != isrTimerRateAxis1) {
#if defined(AXIS1_DRIVER_CODE_GOTO)
    PresetTimerInterval((thisTimerRateAxis1/PPSrateRatio)*AXIS1_DRIVER_STEP_GOTO, TIMER_PULSE_STEP_MULTIPLIER, &nextAxis1GotoRate, &nextAxis1GotoRep);
#endif
    PresetTimerInterval(thisTimerRateAxis1/PPSrateRatio, TIMER_PULSE_STEP_MULTIPLIER, &nextAxis1Rate, &nextAxis1Rep);
    isrTimerRateAxis1=thisTimerRateAxis1;
  }
  if (thisTimerRateAxis2 != isrTimerRateAxis2) {
#if defined(AXIS2_DRIVER_CODE_GOTO)
    PresetTimerInterval((thisTimerRateAxis2/PPSrateRatio)*AXIS2_DRIVER_STEP_GOTO, TIMER_PULSE_STEP_MULTIPLIER, &nextAxis2GotoRate, &nextAxis2GotoRep);
#endif
    PresetTimerInterval(thisTimerRateAxis2/PPSrateRatio, TIMER_PULSE_STEP_MULTIPLIER, &nextAxis2Rate, &nextAxis2Rep);
    isrTimerRateAxis2=thisTimerRateAxis2;
  }
}

IRAM_ATTR ISR(TIMER3_COMPA_vect)
{
#ifdef HAL_TIMER3_PREFIX
  HAL_TIMER3_PREFIX;
#endif

  static uint16_t count = 0;
#if defined(AXIS1_DRIVER_CODE_GOTO)
  if (stepAxis1 != 1) { if (nextAxis1GotoRep > 1) { count++; if (count%nextAxis1GotoRep != 0) goto done; } } else
#endif
  if (nextAxis1Rep > 1) { count++; if (count%nextAxis1Rep != 0) goto done; }

#if STEP_WAVE_FORM != DEDGE
  Axis1StepPinReset;
#endif

#if STEP_WAVE_FORM == SQUARE
  if (clearAxis1) {
    takeStepAxis1=false;
#endif

#if MODE_SWITCH_BEFORE_SLEW == OFF && defined(AXIS1_DRIVER_CODE_GOTO)
  // switch micro-step mode
  if (gotoModeAxis1 != gotoRateAxis1) {
    // only when at an allowed position
    if ((gotoModeAxis1) || ((posAxis1+blAxis1)%AXIS1_DRIVER_STEP_GOTO == 0)) {
      // switch mode
      if (gotoModeAxis1) { stepAxis1=1; AXIS1_DRIVER_CODE_NEXT=AXIS1_DRIVER_CODE; gotoModeAxis1=false; } else { stepAxis1=AXIS1_DRIVER_STEP_GOTO; AXIS1_DRIVER_CODE_NEXT=AXIS1_DRIVER_CODE_GOTO; gotoModeAxis1=true; }
      digitalWrite(Axis1_M0,(AXIS1_DRIVER_CODE_NEXT & 1));
      digitalWrite(Axis1_M1,(AXIS1_DRIVER_CODE_NEXT>>1 & 1));
      #ifndef AXIS1_DRIVER_DISABLE_M2
        digitalWrite(Axis1_M2,(AXIS1_DRIVER_CODE_NEXT>>2 & 1));
      #endif
    }
  }
#endif

#if STEP_WAVE_FORM != SQUARE
#if defined(AXIS1_DRIVER_CODE_GOTO)
  if (stepAxis1 != 1) QuickSetIntervalAxis1(nextAxis1GotoRate); else
#endif
  QuickSetIntervalAxis1(nextAxis1Rate);
#endif

  if ((trackingState != TrackingMoveTo) && (!inbacklashAxis1)) targetAxis1.part.m+=timerDirAxis1*stepAxis1;

  // move the RA/Azm stepper to the target
#if MODE_SWITCH_BEFORE_SLEW == OFF
  if ((posAxis1 != (long)targetAxis1.part.m) || inbacklashAxis1) {
#else
  if ((labs(posAxis1 - (long)targetAxis1.part.m) >= stepAxis1) || inbacklashAxis1) {
#endif

    // set direction
    if (posAxis1 < (long)targetAxis1.part.m) dirAxis1=1; else dirAxis1=0;
    #if AXIS1_DRIVER_REVERSE == ON
      if (defaultDirAxis1 == dirAxis1) Axis1DirPinLOW; else Axis1DirPinHIGH;
    #else
      if (defaultDirAxis1 == dirAxis1) Axis1DirPinHIGH; else Axis1DirPinLOW;
    #endif
  
    // telescope moves WEST with the sky, blAxis1 is the amount of EAST backlash
    if (dirAxis1 == 1) {
      if (blAxis1 < backlashAxis1) { blAxis1+=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1+=stepAxis1; }
    } else {
      if (blAxis1 > 0)             { blAxis1-=stepAxis1; inbacklashAxis1=true; } else { inbacklashAxis1=false; posAxis1-=stepAxis1; }
    }

#if STEP_WAVE_FORM == SQUARE
      takeStepAxis1=true;
    }
    clearAxis1=false;
  } else { 
    if (takeStepAxis1) Axis1StepPinStep;
    clearAxis1=true;

#if defined(AXIS1_DRIVER_CODE_GOTO)
    if (stepAxis1 != 1) QuickSetIntervalAxis1(nextAxis1GotoRate); else
#endif
    QuickSetIntervalAxis1(nextAxis1Rate);
  }
#else
#if STEP_WAVE_FORM == DEDGE
    toggleStateAxis1++;
    if (toggleStateAxis1%2 == 0) Axis1StepPinReset; else Axis1StepPinStep;
#else
    Axis1StepPinStep;
#endif
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

  static uint16_t count = 0;
#if defined(AXIS2_DRIVER_CODE_GOTO)
  if (stepAxis2 != 1) { if (nextAxis2GotoRep > 1) { count++; if (count%nextAxis2GotoRep != 0) goto done; } } else
#endif
  if (nextAxis2Rep > 1) { count++; if (count%nextAxis2Rep != 0) goto done; }

#if STEP_WAVE_FORM != DEDGE
  Axis2StepPinReset;
#endif

#if STEP_WAVE_FORM == SQUARE
  if (clearAxis2) {
    takeStepAxis2=false;
#endif

#if MODE_SWITCH_BEFORE_SLEW == OFF && defined(AXIS2_DRIVER_CODE_GOTO) 
  // switch micro-step mode
  if (gotoModeAxis2 != gotoRateAxis2) {
    // only when at an allowed position
    if ((gotoModeAxis2) || ((posAxis2+blAxis2)%AXIS2_DRIVER_STEP_GOTO == 0)) {
      // switch mode
      if (gotoModeAxis2) { stepAxis2=1; AXIS2_DRIVER_CODE_NEXT=AXIS2_DRIVER_CODE; gotoModeAxis2=false; } else { stepAxis2=AXIS2_DRIVER_STEP_GOTO; AXIS2_DRIVER_CODE_NEXT=AXIS2_DRIVER_CODE_GOTO; gotoModeAxis2=true; }
      digitalWrite(Axis2_M0,(AXIS2_DRIVER_CODE_NEXT & 1));
      digitalWrite(Axis2_M1,(AXIS2_DRIVER_CODE_NEXT>>1 & 1));
      #ifndef AXIS2_DRIVER_DISABLE_M2
        digitalWrite(Axis2_M2,(AXIS2_DRIVER_CODE_NEXT>>2 & 1));
      #endif
    }
  }
#endif

#if STEP_WAVE_FORM != SQUARE
#if defined(AXIS2_DRIVER_CODE_GOTO)
  if (stepAxis2 != 1) QuickSetIntervalAxis2(nextAxis2GotoRate); else
#endif
  QuickSetIntervalAxis2(nextAxis2Rate);
#endif

  if ((trackingState != TrackingMoveTo) && (!inbacklashAxis2)) targetAxis2.part.m+=timerDirAxis2*stepAxis2;

  // move the Dec/Alt stepper to the target
#if MODE_SWITCH_BEFORE_SLEW == OFF
  if (axis2Powered && ((posAxis2 != (long)targetAxis2.part.m) || inbacklashAxis2)) {
#else
  if (axis2Powered && ((labs(posAxis2 - (long)targetAxis2.part.m) >= stepAxis2) || inbacklashAxis2)) {
#endif
    
    // set direction
    if (posAxis2 < (long)targetAxis2.part.m) dirAxis2=1; else dirAxis2=0;
    #if AXIS2_DRIVER_REVERSE == ON
      if (defaultDirAxis2 == dirAxis2) Axis2DirPinLOW; else Axis2DirPinHIGH;
    #else
      if (defaultDirAxis2 == dirAxis2) Axis2DirPinHIGH; else Axis2DirPinLOW;
    #endif
   
    // telescope moving toward celestial pole in the sky, blAxis2 is the amount of opposite backlash
    if (dirAxis2 == 1) {
      if (blAxis2 < backlashAxis2) { blAxis2+=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2+=stepAxis2; }
    } else {
      if (blAxis2 > 0)             { blAxis2-=stepAxis2; inbacklashAxis2=true; } else { inbacklashAxis2=false; posAxis2-=stepAxis2; }
    }

#if STEP_WAVE_FORM == SQUARE
      takeStepAxis2=true;
    }
    clearAxis2=false;
  } else { 
    if (takeStepAxis2) Axis2StepPinStep;
    clearAxis2=true;

#if defined(AXIS2_DRIVER_CODE_GOTO)
    if (stepAxis2 != 1) QuickSetIntervalAxis2(nextAxis2GotoRate); else
#endif
    QuickSetIntervalAxis2(nextAxis2Rate);
  }
#else
#if STEP_WAVE_FORM == DEDGE
    toggleStateAxis2++;
    if (toggleStateAxis2%2 == 0) Axis2StepPinReset; else Axis2StepPinStep;
#else
    Axis2StepPinStep;
#endif
  }
#endif

done: {}
#ifdef HAL_TIMER4_SUFFIX
  HAL_TIMER4_SUFFIX;
#endif
}

double getFrequencyHzAxis1() {
  if (trackingState == TrackingMoveTo) {
    if (posAxis1 == (long)targetAxis1.part.m) {
      return getStepsPerSecondAxis1()*1.00273790935;
    } else
      return 16000000.0/(double)isrTimerRateAxis1;
  } else
    return (16000000.0/(double)isrTimerRateAxis1)*(double)timerDirAxis1;
}

double getFrequencyHzAxis2() {
  if (trackingState == TrackingMoveTo) {
    if (posAxis2 == (long)targetAxis2.part.m)
      return getStepsPerSecondAxis2()*1.00273790935;
    else
      return 16000000.0/(double)isrTimerRateAxis2;
  } else
    return (16000000.0/(double)isrTimerRateAxis2)*(double)timerDirAxis2;
}

#if AXIS2_DRIVER_POWER_DOWN == ON && MOUNT_TYPE != ALTAZM
  // Auto power down the Dec motor
  void autoPowerDownAxis2() {
    static long Axis2PowerOffTimer = 0;
    static long timerLastPosAxis2 = 0;
    if (axis2Enabled) {
      // timer count down
      if (Axis2PowerOffTimer > 0) Axis2PowerOffTimer--; 
  
      // if the guide rate <= 1x and we're guiding on either axis set the timer to 10 minutes
      if ((fabs(guideTimerBaseRateAxis1) <= 1.000001) && (guideDirAxis2 || guideDirAxis1)) Axis2PowerOffTimer=10L*60L*100L;
  
      // if Axis2 isn't stationary, or needs to move, set the timer to a minimum of 10 seconds
      cli(); 
      if (((posAxis2 != timerLastPosAxis2) || (posAxis2 != (long)targetAxis2.part.m)) && (Axis2PowerOffTimer < 10*100)) { timerLastPosAxis2=posAxis2; Axis2PowerOffTimer=10*100; }
  
      // enable/disable Axis2
      if (Axis2PowerOffTimer == 0) {
        if (axis2Powered && !takeStepAxis2) { axis2Powered=false; digitalWrite(Axis2_EN,AXIS2_DRIVER_DISABLE); }
      } else {
        if (!axis2Powered) { digitalWrite(Axis2_EN,AXIS2_DRIVER_ENABLE); axis2Powered=true; delayMicroseconds(10); }
      }
      sei();
    } else { Axis2PowerOffTimer=0; axis2Powered=true; }
  }
#endif

#if PPS_SENSE != OFF
// PPS interrupt
void clockSync() {
  #define NUM_SECS_TO_AVERAGE 40
  unsigned long t=micros();
  unsigned long oneS=(t-PPSlastMicroS);
  if ((oneS > 1000000-20000) && (oneS < 1000000+20000)) {
    PPSavgMicroS=(PPSavgMicroS*(NUM_SECS_TO_AVERAGE-1)+oneS)/NUM_SECS_TO_AVERAGE;
    PPSsynced=true;
  } else PPSsynced=false;
  PPSlastMicroS=t;
}
#endif
