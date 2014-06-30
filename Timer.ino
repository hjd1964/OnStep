#if defined(__arm__) && defined(TEENSYDUINO)
#define ISR(f) void f (void)
IntervalTimer itimer1;
void TIMER1_COMPA_vect(void);
volatile boolean HAclr = true;
volatile boolean DEclr = true;
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
ISR(TIMER1_COMPA_vect)
{
  lst++;

  if ((trackingState==TrackingSidereal) || (trackingState==TrackingMoveTo)) {
    long thisTimerRate=timerRateHA;
    // override rate during backlash compensation
    if (inBacklashHA) thisTimerRate=timerRateBacklashHA;
    if (thisTimerRate!=isrTimerRateHA) {
      Timer3SetRate(thisTimerRate);
      isrTimerRateHA=thisTimerRate;
    }
    thisTimerRate=timerRateDec;
    // override rate during backlash compensation
    if (inBacklashDec) thisTimerRate=timerRateBacklashDec;
    if (thisTimerRate!=isrTimerRateDec) {
      Timer4SetRate(thisTimerRate);
      isrTimerRateDec=thisTimerRate;
    }
  } else {
    if (timerRateHA!=isrTimerRateHA) {
      Timer3SetRate(timerRateHA);
      isrTimerRateHA=timerRateHA;
    }
    if (timerRateDec!=isrTimerRateDec) {
      Timer4SetRate(timerRateDec);
      isrTimerRateDec=timerRateDec;
    }
  }
}

ISR(TIMER3_COMPA_vect)
{
#if defined(__AVR__)
  // drivers step on the rising edge, they need 1uS to settle so this is early in the routine
  CLR(HAStepPORT,  HAStepBit);
  if (posHA<(targetHA+PEC_HA)) dirHA=1; else dirHA=0; // Direction control
  #ifdef REVERSE_HA_ON
    if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
  #else
    if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
  #endif
#elif defined(__arm__) && defined(TEENSYDUINO)
  // on the much faster Teensy run at twice the rate and pull the step pin low every other call
  if (HAclr) {
    CLR(HAStepPORT,  HAStepBit);
    if (posHA<(targetHA+PEC_HA)) dirHA=1; else dirHA=0; // Direction control
    #ifdef REVERSE_HA_ON
      if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
    #else
      if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
    #endif
    HAclr=false;
    return;
  } else HAclr=true;
#endif
  
  if (posHA!=(targetHA+PEC_HA)) { // Move the RA stepper to the target
    // telescope moves WEST with the sky, blHA is the amount of EAST backlash
    if (dirHA==1) {
     if (blHA<backlashHA) { blHA++; inBacklashHA=true; } else { inBacklashHA=false; posHA++; }
    } else {
      if (blHA>0)         { blHA--; inBacklashHA=true; } else { inBacklashHA=false; posHA--; }
    }

    SET(HAStepPORT, HAStepBit);
  }
#if defined(__AVR__)
  OCR3A = nextHArate;
#endif
}

ISR(TIMER4_COMPA_vect)
{
#if defined(__AVR__)
  // drivers step on the rising edge, they need 1uS to settle so this is early in the routine
  CLR(DecStepPORT,  DecStepBit);
  // telescope normally starts on the EAST side of the pier looking at the WEST sky
  if (posDec<targetDec) dirDec=1; else dirDec=0;                                     // Direction control
  #ifdef REVERSE_DEC_ON
    if (DecDir==dirDec) SET(DecDirPORT, DecDirBit); else CLR(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
  #else
    if (DecDir==dirDec) CLR(DecDirPORT, DecDirBit); else SET(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
  #endif
#elif defined(__arm__) && defined(TEENSYDUINO)
  // on the much faster Teensy run this ISR at twice the normal rate and pull the step pin low every other call
  if (DEclr) {
    CLR(DecStepPORT,  DecStepBit);
    // telescope normally starts on the EAST side of the pier looking at the WEST sky
    if (posDec<targetDec) dirDec=1; else dirDec=0;                                     // Direction control
    #ifdef REVERSE_DEC_ON
      if (DecDir==dirDec) SET(DecDirPORT, DecDirBit); else CLR(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
    #else
      if (DecDir==dirDec) CLR(DecDirPORT, DecDirBit); else SET(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
    #endif
    DEclr=false;
    return;
  } else DEclr=true;
#endif

  if (posDec!=targetDec) {                                                         // move the Dec stepper to the target
    // telescope moving NORTH in the sky, blDec is the amount of SOUTH backlash
    if (dirDec==1) {
      if (blDec<backlashDec) { blDec++; inBacklashDec=true; } else { inBacklashDec=false; posDec++; }
    } else {
      if (blDec>0)           { blDec--; inBacklashDec=true; } else { inBacklashDec=false; posDec--; }
    }

    SET(DecStepPORT, DecStepBit);
  }

#if defined(__AVR__)
  OCR4A = nextDErate;
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
