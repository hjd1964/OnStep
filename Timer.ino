// -----------------------------------------------------------------------------------
// Timers and interrupt handling

#if STEP_WAVE_FORM == PULSE
  // motor timers at 1x rate
  #define TIMER_PULSE_STEP true
#elif STEP_WAVE_FORM == DEDGE
  // motor timers at 1x rate
  #define TIMER_PULSE_STEP true
  volatile byte toggleStateAxis1 = 0;
  volatile byte toggleStateAxis2 = 0;
#else
  // motor timers at 2x rate
  #define TIMER_PULSE_STEP false
  volatile bool clearAxis1 = true;
  volatile bool takeStepAxis1 = false;
  volatile bool clearAxis2 = true;
#endif
  volatile bool takeStepAxis2 = false;

#if defined(AXIS1_DRIVER_CODE) && defined(AXIS1_DRIVER_CODE_GOTO)
  volatile long AXIS1_DRIVER_CODE_NEXT=AXIS1_DRIVER_CODE;
  volatile bool gotoModeAxis1=false;
#endif

#if defined(AXIS2_DRIVER_CODE) && defined(AXIS2_DRIVER_CODE_GOTO)
  volatile long AXIS2_DRIVER_CODE_NEXT=AXIS2_DRIVER_CODE;
  volatile bool gotoModeAxis2=false;
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
  if (trackingState == TrackingMoveTo) Timer1SetInterval(iv/100,ppsRateRatio); else Timer1SetInterval(iv/300,ppsRateRatio);

  isrTimerRateAxis1=0; // also force rate update for Axis1/2 timers so that PPS adjustments take hold immediately
  isrTimerRateAxis2=0;
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and setting up the Axis1/2 intervals for later programming
volatile bool gotoRateAxis1=false;
volatile bool gotoRateAxis2=false;
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
          axis1DriverGotoMode();

          // at higher step rates where torque is reduced make smaller rate changes
          double r=1.2-sqrt((fabs(guideTimerRateAxis1A)/slewRateX));
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
            if (fabs(guideTimerRateAxis1A) < 0.001) { guideDirAxis1=0; lastGuideDirAxis1=0; guideTimerRateAxis1=0.0; guideTimerRateAxis1A=0.0; guideDirChangeTimerAxis1=0; axis1DriverTrackingMode(false); }
          }
        }
      }
    } else guideTimerRateAxis1A=0.0;

    double timerRateAxis1B=guideTimerRateAxis1A+pecTimerRateAxis1+trackingTimerRateAxis1;
    if (timerRateAxis1B < -0.00001) { timerRateAxis1B=fabs(timerRateAxis1B); cli(); timerDirAxis1=-1; sei(); } else 
      if (timerRateAxis1B > 0.00001) { cli(); timerDirAxis1=1; sei(); } else { cli(); timerDirAxis1=0; sei(); timerRateAxis1B=1.0; }
    double f = round(siderealRate/timerRateAxis1B);
    if (fabs(f) > 2144000000) { cli(); timerDirAxis1=0; sei(); f = round(siderealRate); }
    long calculatedTimerRateAxis1=f;
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis1 != calculatedTimerRateAxis1) { timerRateAxis1=calculatedTimerRateAxis1; runTimerRateAxis1=calculatedTimerRateAxis1; }
 
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
          axis2DriverGotoMode();
  
          // at higher step rates where torque is reduced make smaller rate changes
          double r=1.2-sqrt((fabs(guideTimerRateAxis2A)/slewRateX));
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
            if (fabs(guideTimerRateAxis2A) < 0.001) { guideDirAxis2=0; lastGuideDirAxis2=0; guideTimerRateAxis2=0.0; guideTimerRateAxis2A=0.0; guideDirChangeTimerAxis2=0; axis2DriverTrackingMode(false); }
          }
        }
      }
    } else guideTimerRateAxis2A=0.0;

    double timerRateAxis2B=guideTimerRateAxis2A+trackingTimerRateAxis2;
    if (timerRateAxis2B < -0.0001) { timerRateAxis2B=fabs(timerRateAxis2B); cli(); timerDirAxis2=-1; sei(); } else
      if (timerRateAxis2B > 0.0001) { cli(); timerDirAxis2=1; sei(); } else { cli(); timerDirAxis2=0; sei(); timerRateAxis2B=1.0; }
    f = round(siderealRate/timerRateAxis2B);
    if (fabs(f) > 2144000000/timerRateRatio) { cli(); timerDirAxis2=0; sei(); f = round(siderealRate); }
    long calculatedTimerRateAxis2=f;
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateAxis2 != calculatedTimerRateAxis2) { timerRateAxis2=calculatedTimerRateAxis2; runTimerRateAxis2=calculatedTimerRateAxis2; }
  }
  
  thisTimerRateAxis1=timerRateAxis1;
  if (useTimerRateRatio) { thisTimerRateAxis2=(timerRateAxis2*timerRateRatio); } else { thisTimerRateAxis2=timerRateAxis2; }
  
  // override rate during backlash compensation
  if (inbacklashAxis1) thisTimerRateAxis1=timerRateBacklashAxis1;
  // override rate during backlash compensation
  if (inbacklashAxis2) thisTimerRateAxis2=timerRateBacklashAxis2;

  // trigger goto step mode
#if defined(AXIS1_DRIVER_CODE) && defined(AXIS1_DRIVER_CODE_GOTO) && MODE_SWITCH_BEFORE_SLEW == OFF
  gotoRateAxis1=(thisTimerRateAxis1 < AXIS1_DRIVER_SWITCH_RATE);
#endif
#if defined(AXIS2_DRIVER_CODE) && defined(AXIS2_DRIVER_CODE_GOTO) && MODE_SWITCH_BEFORE_SLEW == OFF
  gotoRateAxis2=(thisTimerRateAxis2 < AXIS2_DRIVER_SWITCH_RATE);
#endif

  // set the rates
  if (thisTimerRateAxis1 != isrTimerRateAxis1) {
#if defined(AXIS1_DRIVER_CODE_GOTO)
    PresetTimerInterval((thisTimerRateAxis1/ppsRateRatio)*axis1StepsGoto, TIMER_PULSE_STEP, &nextAxis1GotoRate, &nextAxis1GotoRep);
#endif
    PresetTimerInterval(thisTimerRateAxis1/ppsRateRatio, TIMER_PULSE_STEP, &nextAxis1Rate, &nextAxis1Rep);
    isrTimerRateAxis1=thisTimerRateAxis1;
  }
  if (thisTimerRateAxis2 != isrTimerRateAxis2) {
#if defined(AXIS2_DRIVER_CODE_GOTO)
    PresetTimerInterval((thisTimerRateAxis2/ppsRateRatio)*axis2StepsGoto, TIMER_PULSE_STEP, &nextAxis2GotoRate, &nextAxis2GotoRep);
#endif
    PresetTimerInterval(thisTimerRateAxis2/ppsRateRatio, TIMER_PULSE_STEP, &nextAxis2Rate, &nextAxis2Rep);
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

#if MODE_SWITCH_BEFORE_SLEW == OFF && AXIS1_DRIVER_MODEL == TMC_SPI && defined(AXIS1_DRIVER_CODE_GOTO)
  if (haltAxis1 || axis1ModeSwitchState == MSS_READY) goto done;
#else
  if (haltAxis1) goto done;
#endif

#if STEP_WAVE_FORM != DEDGE
  a1CLEAR;
#endif

#if STEP_WAVE_FORM == SQUARE
  if (clearAxis1) {
    takeStepAxis1=false;
#endif

#if MODE_SWITCH_BEFORE_SLEW == OFF && defined(AXIS1_DRIVER_CODE_GOTO)
  // switch micro-step mode
  if (gotoModeAxis1 != gotoRateAxis1) {
    // only when at an allowed position
    if (gotoModeAxis1 || (posAxis1+blAxis1)%axis1StepsGoto == 0) {
      if (gotoModeAxis1) { gotoModeAxis1=false; axis1DriverTrackingFast(); } else { gotoModeAxis1=true; axis1DriverGotoFast(); }
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
#if MODE_SWITCH_BEFORE_SLEW == ON || (AXIS1_DRIVER_MODEL == TMC_SPI && defined(AXIS1_DRIVER_CODE_GOTO))
  if ((labs(posAxis1 - (long)targetAxis1.part.m) >= stepAxis1) || inbacklashAxis1) {
#else
  if ((posAxis1 != (long)targetAxis1.part.m) || inbacklashAxis1) {
#endif

    // set direction
    if (posAxis1 < (long)targetAxis1.part.m) dirAxis1=1; else dirAxis1=0;
    if (defaultDirAxis1 == dirAxis1) a1DIR_H; else a1DIR_L;

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
    if (takeStepAxis1) a1STEP;
    clearAxis1=true;

#if defined(AXIS1_DRIVER_CODE_GOTO)
    if (stepAxis1 != 1) QuickSetIntervalAxis1(nextAxis1GotoRate); else
#endif
    QuickSetIntervalAxis1(nextAxis1Rate);
  }
#else
#if STEP_WAVE_FORM == DEDGE
    toggleStateAxis1++;
    if (toggleStateAxis1%2 == 0) a1CLEAR; else a1STEP;
#else
    a1STEP;
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

#if MODE_SWITCH_BEFORE_SLEW == OFF && AXIS2_DRIVER_MODEL == TMC_SPI && defined(AXIS2_DRIVER_CODE_GOTO)
  if (haltAxis2 || axis2ModeSwitchState == MSS_READY) goto done;
#else
  if (haltAxis2) goto done;
#endif

#if STEP_WAVE_FORM != DEDGE
  a2CLEAR;
#endif

#if STEP_WAVE_FORM == SQUARE
  if (clearAxis2) {
    takeStepAxis2=false;
#endif

#if MODE_SWITCH_BEFORE_SLEW == OFF && defined(AXIS2_DRIVER_CODE_GOTO)
  // switch micro-step mode
  if (gotoModeAxis2 != gotoRateAxis2) {
    // only when at an allowed position
    if (gotoModeAxis2 || (posAxis2+blAxis2)%axis2StepsGoto == 0) {
      if (gotoModeAxis2) { gotoModeAxis2=false; axis2DriverTrackingFast(); } else { gotoModeAxis2=true; axis2DriverGotoFast(); }
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
#if MODE_SWITCH_BEFORE_SLEW == ON || (AXIS2_DRIVER_MODEL == TMC_SPI && defined(AXIS2_DRIVER_CODE_GOTO))
  if (axis2Powered && ((labs(posAxis2 - (long)targetAxis2.part.m) >= stepAxis2) || inbacklashAxis2)) {
#else
  if (axis2Powered && ((posAxis2 != (long)targetAxis2.part.m) || inbacklashAxis2)) {
#endif

    // set direction
    if (posAxis2 < (long)targetAxis2.part.m) dirAxis2=1; else dirAxis2=0;
    if (defaultDirAxis2 == dirAxis2) a2DIR_H; else a2DIR_L;

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
    if (takeStepAxis2) a2STEP;
    clearAxis2=true;

#if defined(AXIS2_DRIVER_CODE_GOTO)
    if (stepAxis2 != 1) QuickSetIntervalAxis2(nextAxis2GotoRate); else
#endif
    QuickSetIntervalAxis2(nextAxis2Rate);
  }
#else
#if STEP_WAVE_FORM == DEDGE
    toggleStateAxis2++;
    if (toggleStateAxis2%2 == 0) a2CLEAR; else a2STEP;
#else
    a2STEP;
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
      return getstepsPerSecondAxis1()*1.00273790935;
    } else
      return 16000000.0/(double)isrTimerRateAxis1;
  } else
    return (16000000.0/(double)isrTimerRateAxis1)*(double)timerDirAxis1;
}

double getFrequencyHzAxis2() {
  if (trackingState == TrackingMoveTo) {
    if (posAxis2 == (long)targetAxis2.part.m)
      return getstepsPerSecondAxis2()*1.00273790935;
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
  unsigned long oneS=(t-ppsLastMicroS);
  if ((oneS > 1000000-20000) && (oneS < 1000000+20000)) {
    ppsAvgMicroS=(ppsAvgMicroS*(NUM_SECS_TO_AVERAGE-1)+oneS)/NUM_SECS_TO_AVERAGE;
    ppsSynced=true;
  } else ppsSynced=false;
  ppsLastMicroS=t;
}
#endif
