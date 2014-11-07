#if defined(__arm__) && defined(TEENSYDUINO)
#define ISR(f) void f (void)
IntervalTimer itimer1;
void TIMER1_COMPA_vect(void);
volatile boolean HAclr = true;
volatile boolean TakeStepHA = false;
volatile boolean DEclr = true;
volatile boolean TakeStepDec = false;
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
#endif
}

// set timer3 to rate (in microseconds*16)
volatile uint32_t nextHArate;
void Timer3SetRate(long rate) {
#if defined(__AVR__)
  // valid for rates down to 0.25 second
  if (StepsPerSecond<31) rate=rate/64; else rate=rate/8;
  cli(); nextHArate=rate-1; sei();
#elif defined(__arm__) && defined(TEENSYDUINO)
  nextHArate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1;
  cli(); PIT_LDVAL1=nextHArate; sei();
#endif
}
// set timer4 to rate (in microseconds*16)
volatile uint32_t nextDErate;
void Timer4SetRate(long rate) {
#if defined(__AVR__)
  // valid for rates down to 0.25 second
  if (StepsPerSecond<31) rate=rate/64; else rate=rate/8;
  cli(); nextDErate=rate-1; sei();
#elif defined(__arm__) && defined(TEENSYDUINO)
  nextDErate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1;
  cli(); PIT_LDVAL2=nextDErate; sei();
#endif
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles sidereal time and programming the drive rates
volatile long isrTimerRateHA;
volatile long isrTimerRateDec;
volatile long runTimerRateHA=0;
volatile long runTimerRateDec=0;
volatile boolean wasInBacklashHA=false;
volatile boolean wasInBacklashDec=false;
volatile boolean gotoRateHA=false;
volatile boolean gotoRateDec=false;
volatile byte cnt = 0;
ISR(TIMER1_COMPA_vect,ISR_NOBLOCK)
{
  // run 1/3 of the time at 3x the rate
  cnt++; if (cnt%3!=0) return;  
  lst++;
  
  cli(); targetHA1=targetHA+PEC_HA; sei();
   
  if (trackingState==TrackingSidereal) {
    
    // automatic rate calculation HA
    long calculatedTimerRateHA;
    double timerRateHA1=1.0; if (guideDirHA && (currentGuideRate>1)) timerRateHA1=0.0;
    double timerRateHA2=fabs(guideTimerRateHA+pecTimerRateHA+timerRateHA1);
    if (timerRateHA2>0.5) calculatedTimerRateHA=(double)SiderealRate/timerRateHA2; else calculatedTimerRateHA=(double)SiderealRate/0.5;
    if (runTimerRateHA!=calculatedTimerRateHA) { timerRateHA=calculatedTimerRateHA; runTimerRateHA=calculatedTimerRateHA; }

    // dynamic rate adjust
    double x=fabs(targetHA1-posHA); x=x*x;
    if (x>0) {
      if (x>160.0) x=160.0; x=50000.00-x; x=x/50000.0;
      timerRateHA=calculatedTimerRateHA*x; // up to 0.32% faster (or as little as 0.002%)
      runTimerRateHA=timerRateHA;
    }
    
    // automatic rate calculation Dec
    long calculatedTimerRateDec;
    double timerRateDec1=guideTimerRateDec;
    // if we're stopped, just run the timer fast since we're not moving anyway
    if (timerRateDec1>0.5) calculatedTimerRateDec=(double)SiderealRate/timerRateDec1; else calculatedTimerRateDec=(double)SiderealRate/0.5;
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateDec!=calculatedTimerRateDec) { timerRateDec=calculatedTimerRateDec; runTimerRateDec=calculatedTimerRateDec; }
 
    // dynamic rate adjust
    x=fabs(targetDec-posDec); x=x*x;
    if (x>0) {
      if (x>160.0) x=160.0; x=50000.00-x; x=x/50000.0;
      timerRateDec=calculatedTimerRateDec*x; // up to 0.32% faster (or as little as 0.002%)
      runTimerRateDec=timerRateDec;
    }
  }
  
  long thisTimerRateHA=timerRateHA;
  #ifdef DEC_RATIO_ON
  long thisTimerRateDec=timerRateDec*timerRateRatio;
  #else
  long thisTimerRateDec=timerRateDec;
  #endif
  
  if (trackingState==TrackingSidereal) {
    // override rate during backlash compensation
    if (inBacklashHA) { thisTimerRateHA=timerRateBacklashHA; wasInBacklashHA=true; } 
    // override rate during backlash compensation
    if (inBacklashDec) { thisTimerRateDec=timerRateBacklashDec; wasInBacklashDec=true; }
    // travel through the backlash is done, but we weren't following the target while it was happening!
    // so now get us back to near where we need to be
    if ((!inBacklashHA) && (wasInBacklashHA) && (!guideDirHA)) {
      if (abs(posHA-(targetHA+PEC_HA))>2) thisTimerRateHA=SiderealRate/4; else wasInBacklashHA=false;
    }
    if ((!inBacklashDec) && (wasInBacklashDec) && (!guideDirDec)) {
      if (abs(posDec-targetDec)>2) thisTimerRateDec=SiderealRate/4; else wasInBacklashDec=false;
    }
  }
  if (trackingState==TrackingMoveTo) {
    // override rate during backlash compensation
    if (inBacklashHA) { thisTimerRateHA=timerRateBacklashHA; wasInBacklashHA=true; } 
    // override rate during backlash compensation
    if (inBacklashDec) { thisTimerRateDec=timerRateBacklashDec; wasInBacklashDec=true; }
      
    // trigger Goto step mode when faster than the fastest guide rate
    #if defined(DE_MODE) && defined(DE_MODE_GOTO)
    gotoRateDec=(thisTimerRateDec<SiderealRate/80);
    #endif
    #if defined(HA_MODE) && defined(HA_MODE_GOTO)
    gotoRateHA=(thisTimerRateHA<SiderealRate/80);
    #endif
  }
  
  // set the rates
  if (thisTimerRateHA!=isrTimerRateHA) {
    Timer3SetRate(thisTimerRateHA/PPSrateRatio);
    isrTimerRateHA=thisTimerRateHA;
  }
  if (thisTimerRateDec!=isrTimerRateDec) {
    Timer4SetRate(thisTimerRateDec/PPSrateRatio);
    isrTimerRateDec=thisTimerRateDec;
  }
 
}

#if defined(HA_MODE) && defined(HA_MODE_GOTO)
volatile long stepHA=1;
volatile boolean gotoModeHA=false;
#else
#define stepHA 1
#endif

ISR(TIMER3_COMPA_vect)
{
#if defined(__AVR__)
  #if defined(HA_MODE) && defined(HA_MODE_GOTO)
  OCR3A = nextHArate*stepHA;
  #else
  OCR3A = nextHArate;
  #endif

  // drivers step on the rising edge, need >=1.9uS to settle (for DRV8825 or A4988) so this is early in the routine
  CLR(HAStepPORT,  HAStepBit);

#if defined(HA_MODE) && defined(HA_MODE_GOTO)
  // switch micro-step mode
  if ((!gotoModeHA && gotoRateHA) || (gotoModeHA && !gotoRateHA)) {
    // only when at the home position
    if ((posHA+blHA)%128==0) {
      // make sure travel through backlash compensation is finished
      if ((blHA==backlashHA) || (blHA==0)) {
        // switch mode
        if (gotoModeHA) {
          digitalWrite(HA_M0,(HA_MODE & 1));
          digitalWrite(HA_M1,(HA_MODE>>1 & 1));
          digitalWrite(HA_M2,(HA_MODE>>2 & 1));
          stepHA=1;
          gotoModeHA=false;
        } else {
          digitalWrite(HA_M0,(HA_MODE_GOTO & 1));
          digitalWrite(HA_M1,(HA_MODE_GOTO>>1 & 1));
          digitalWrite(HA_M2,(HA_MODE_GOTO>>2 & 1));
          stepHA=HA_STEP_GOTO;
          gotoModeHA=true;
        }
      }
    }
  }
#endif

  // Guessing about 4+4+1+ 4+4+1+ 1+ 2+1+2+ 13=37 clocks between here and the step signal which is 2.3uS
  if (posHA!=targetHA1) { // Move the RA stepper to the target
    if (posHA<targetHA1) dirHA=1; else dirHA=0; // Direction control
    #ifdef REVERSE_HA_ON
      if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring.)  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
    #else                                                                        // Guessing about 1+2+1+4+4+1=13 clocks between here and the step signal which is 0.81uS
      if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit);
    #endif
    // telescope moves WEST with the sky, blHA is the amount of EAST backlash
    if (dirHA==1) {
     if (blHA<backlashHA) { blHA++; inBacklashHA=true; } else { inBacklashHA=false; posHA+=stepHA; }
    } else {
      if (blHA>0)         { blHA--; inBacklashHA=true; } else { inBacklashHA=false; posHA-=stepHA; }
    }
    SET(HAStepPORT, HAStepBit);
  }

#elif defined(__arm__) && defined(TEENSYDUINO)
  // On the much faster Teensy run at twice the rate and pull the step pin low every other call
  // Step signal doesn't happen until the next ISR call, so loads of time settling
  if (HAclr) {
    CLR(HAStepPORT,  HAStepBit);
    if (posHA!=targetHA1) { // Move the RA stepper to the target
      if (posHA<targetHA1) dirHA=1; else dirHA=0; // Direction control
      #ifdef REVERSE_HA_ON
        if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring.)  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
      #else
        if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit);
      #endif
      // telescope moves WEST with the sky, blHA is the amount of EAST backlash
      if (dirHA==1) {
        if (blHA<backlashHA) { blHA++; inBacklashHA=true; } else { inBacklashHA=false; posHA++; }
      } else {
        if (blHA>0)          { blHA--; inBacklashHA=true; } else { inBacklashHA=false; posHA--; }
      }
      TakeStepHA=true;
    } else TakeStepHA=false;
    HAclr=false;
  } else {
    if (TakeStepHA) SET(HAStepPORT, HAStepBit);
    HAclr=true;
  }
#endif
}

#if defined(DE_MODE) && defined(DE_MODE_GOTO)
volatile long stepDec=1;
volatile boolean gotoModeDec=false;
#else
#define stepDec 1
#endif

ISR(TIMER4_COMPA_vect)
{
#if defined(__AVR__)

  // drivers step on the rising edge
  CLR(DecStepPORT,  DecStepBit);

  #if defined(DE_MODE) && defined(DE_MODE_GOTO)
  OCR4A = nextDErate*stepDec;
  #else
  OCR4A = nextDErate;
  #endif

#if defined(DE_MODE) && defined(DE_MODE_GOTO)
  // switch micro-step mode
  if ((!gotoModeDec && gotoRateDec) || (gotoModeDec && !gotoRateDec)) {
    // only when at home position
    if ((posDec+blDec)%128==0) {
      // make sure travel through backlash compensation is finished
      if ((blDec==backlashDec) || (blDec==0)) {
        // switch mode
        if (gotoModeDec) {
          digitalWrite(DE_M0,(DE_MODE & 1));
          digitalWrite(DE_M1,(DE_MODE>>1 & 1));
          digitalWrite(DE_M2,(DE_MODE>>2 & 1));
          stepDec=1;
          gotoModeDec=false;
        } else {
          digitalWrite(DE_M0,(DE_MODE_GOTO & 1));
          digitalWrite(DE_M1,(DE_MODE_GOTO>>1 & 1));
          digitalWrite(DE_M2,(DE_MODE_GOTO>>2 & 1));
          stepDec=DE_STEP_GOTO;
          gotoModeDec=true;
        }
      }
    }
  }
#endif
  
  if (posDec!=targetDec) { // move the Dec stepper to the target
    // telescope normally starts on the EAST side of the pier looking at the WEST sky
    if (posDec<targetDec) dirDec=1; else dirDec=0;                                     // Direction control
    #ifdef REVERSE_DEC_ON
      if (DecDir==dirDec) SET(DecDirPORT, DecDirBit); else CLR(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
    #else
      if (DecDir==dirDec) CLR(DecDirPORT, DecDirBit); else SET(DecDirPORT, DecDirBit);
    #endif
    // telescope moving toward celestial pole in the sky, blDec is the amount of opposite backlash
    if (dirDec==1) {
      if (blDec<backlashDec) { blDec++; inBacklashDec=true; } else { inBacklashDec=false; posDec+=stepDec; }
    } else {
      if (blDec>0)           { blDec--; inBacklashDec=true; } else { inBacklashDec=false; posDec-=stepDec; }
    }
    SET(DecStepPORT, DecStepBit);
  }

#elif defined(__arm__) && defined(TEENSYDUINO)
  // on the much faster Teensy run this ISR at twice the normal rate and pull the step pin low every other call
  if (DEclr) {
    CLR(DecStepPORT,  DecStepBit);
    if (posDec!=targetDec) { // move the Dec stepper to the target
      // telescope normally starts on the EAST side of the pier looking at the WEST sky
      if (posDec<targetDec) dirDec=1; else dirDec=0;                                     // Direction control
      #ifdef REVERSE_DEC_ON
        if (DecDir==dirDec) SET(DecDirPORT, DecDirBit); else CLR(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
      #else
        if (DecDir==dirDec) CLR(DecDirPORT, DecDirBit); else SET(DecDirPORT, DecDirBit);
      #endif
      // telescope moving toward celestial pole in the sky, blDec is the amount of opposite backlash
      if (dirDec==1) {
        if (blDec<backlashDec) { blDec++; inBacklashDec=true; } else { inBacklashDec=false; posDec++; }
      } else {
        if (blDec>0)           { blDec--; inBacklashDec=true; } else { inBacklashDec=false; posDec--; }
      }
      TakeStepDec=true;
    } else TakeStepDec=false;
    DEclr=false;
  } else {
    if (TakeStepDec) SET(DecStepPORT, DecStepBit);
    DEclr=true;
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

