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
  if (rate>65536) rate=65536;
  nextHArate=rate-1;
#elif defined(__arm__) && defined(TEENSYDUINO)
  nextHArate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1;
  cli();
  PIT_LDVAL1=nextHArate;
  sei();
#endif
}
// set timer4 to rate (in microseconds*16)
volatile uint32_t nextDErate;
void Timer4SetRate(long rate) {
#if defined(__AVR__)
  // valid for rates down to 0.25 second
  if (StepsPerSecond<31) rate=rate/64; else rate=rate/8;
  if (rate>65536) rate=65536;
  nextDErate=rate-1;
#elif defined(__arm__) && defined(TEENSYDUINO)
  nextDErate=(F_BUS / 1000000) * (rate*0.0625) * 0.5 - 1;
  cli();
  PIT_LDVAL2=nextDErate;
  sei();
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
ISR(TIMER1_COMPA_vect)
{
  lst++;

  if (trackingState==TrackingSidereal) {
    // automatic rate calculation HA
    double timerRateHA1=((double)moveTimerRateHA*1.1)/SiderealRate;
    double timerRateHA2=((double)pecTimerRateHA*1.1) /SiderealRate;
    double timerRateHA3=1.02                         /SiderealRate; if (moveDirHA && (currentGuideRate>1)) timerRateHA3=0;
    double calculatedTimerRateHA=abs(timerRateHA1+timerRateHA2+timerRateHA3);
    // if we're stopped, just run the timer fast since we're not moving anyway
    if (calculatedTimerRateHA>0) calculatedTimerRateHA=1.0/calculatedTimerRateHA; else calculatedTimerRateHA=SiderealRate*10.0; 
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateHA!=calculatedTimerRateHA) { timerRateHA=calculatedTimerRateHA; runTimerRateHA=calculatedTimerRateHA; }
    // dynamic rate adjust
    if (abs(posHA-(targetHA+PEC_HA))>1) {
      timerRateHA=timerRateHA/rd; if (timerRateHA<SiderealRate/60) timerRateHA=SiderealRate/60;     // 60X sidereal speed as fast as we allow 
    } else {
      timerRateHA=timerRateHA*rd; if (timerRateHA>SiderealRate*10.0) timerRateHA=SiderealRate*10.0; // 0.1X sidereal speed as slow as we allow
    }

    // automatic rate calculation Dec
    double calculatedTimerRateDec=((double)moveTimerRateDec*1.1)/SiderealRate;
    // if we're stopped, just run the timer fast since we're not moving anyway
    if (calculatedTimerRateDec>0) calculatedTimerRateDec=1.00/calculatedTimerRateDec; else calculatedTimerRateDec=SiderealRate*10.0;
    // remember our "running" rate and only update the actual rate when it changes
    if (runTimerRateDec!=calculatedTimerRateDec) { timerRateDec=calculatedTimerRateDec; runTimerRateDec=calculatedTimerRateDec; }
    // dynamic rate adjust
    if (abs(posDec-targetDec)>1) { 
      timerRateDec=timerRateDec/rd; if (timerRateDec<SiderealRate/60) timerRateDec=SiderealRate/60; 
    } else { 
      timerRateDec=timerRateDec*rd; if (timerRateDec>SiderealRate*10.0) timerRateDec=SiderealRate*10.0; 
    }
  }

  long thisTimerRateHA=timerRateHA;
  #ifdef DEC_RATIO_ON
  long thisTimerRateDec=timerRateDec*timerRateRatio;
  #else
  long thisTimerRateDec=timerRateDec;
  #endif
  
  // backlash compensation timing
  if ((trackingState==TrackingSidereal) || (trackingState==TrackingMoveTo)) {
    // override rate during backlash compensation
    if (inBacklashHA) { thisTimerRateHA=timerRateBacklashHA; wasInBacklashHA=true; } 
    // override rate during backlash compensation
    if (inBacklashDec) { thisTimerRateDec=timerRateBacklashDec; wasInBacklashDec=true; }
  }
  if (trackingState==TrackingSidereal) {
    // travel through the backlash is done, but we weren't following the target while it was happening!
    // so now get us back to near where we need to be
    if ((!inBacklashHA) && (wasInBacklashHA)) {
      if (abs(posHA-(targetHA+PEC_HA))>2) thisTimerRateHA=SiderealRate/4; else wasInBacklashHA=false;
    }
    if ((!inBacklashDec) && (wasInBacklashDec)) {
      if (abs(posDec-targetDec)>2) thisTimerRateDec=SiderealRate/4; else wasInBacklashDec=false;
    }
  }
  
  // set the rates
  if (thisTimerRateHA!=isrTimerRateHA) {
    Timer3SetRate(thisTimerRateHA);
    isrTimerRateHA=thisTimerRateHA;
  }
  if (thisTimerRateDec!=isrTimerRateDec) {
    Timer4SetRate(thisTimerRateDec);
    isrTimerRateDec=thisTimerRateDec;
  }
}

ISR(TIMER3_COMPA_vect)
{
#if defined(__AVR__)
  // drivers step on the rising edge, need >=1.9uS to settle (for DRV8825 or A4988) so this is early in the routine
  CLR(HAStepPORT,  HAStepBit);
  // Guessing about 4+4+1+ 4+4+1+ 1+ 2+1+2+ 13=37 clocks between here and the step signal which is 2.3uS
  if (posHA!=(targetHA+PEC_HA)) { // Move the RA stepper to the target
    if (posHA<(targetHA+PEC_HA)) dirHA=1; else dirHA=0; // Direction control
    #ifdef REVERSE_HA_ON
      if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring.)  Needs >=0.65uS before/after rising step signal (DRV8825 or A4988).
    #else                                                                        // Guessing about 1+2+1+4+4+1=13 clocks between here and the step signal which is 0.81uS
      if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit);
    #endif
    // telescope moves WEST with the sky, blHA is the amount of EAST backlash
    if (dirHA==1) {
     if (blHA<backlashHA) { blHA++; inBacklashHA=true; } else { inBacklashHA=false; posHA++; }
    } else {
      if (blHA>0)         { blHA--; inBacklashHA=true; } else { inBacklashHA=false; posHA--; }
    }
    SET(HAStepPORT, HAStepBit);
  }
  OCR3A = nextHArate;
#elif defined(__arm__) && defined(TEENSYDUINO)
  // On the much faster Teensy run at twice the rate and pull the step pin low every other call
  // Step signal doesn't happen until the next ISR call, so loads of time settling
  if (HAclr) {
    CLR(HAStepPORT,  HAStepBit);
    if (posHA!=(targetHA+PEC_HA)) { // Move the RA stepper to the target
      if (posHA<(targetHA+PEC_HA)) dirHA=1; else dirHA=0; // Direction control
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

ISR(TIMER4_COMPA_vect)
{
#if defined(__AVR__)
  // drivers step on the rising edge
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
    SET(DecStepPORT, DecStepBit);
  }
  OCR4A = nextDErate;
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
