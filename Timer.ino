//--------------------------------------------------------------------------------------------------
// set timer to rate (in microseconds*16)
void Timer1SetRate(long rate) {
  TCCR1B = 0; TCCR1A = 0;  
  TIMSK1 = 0;

  // set compare match register to desired timer count:
  rate--;
  if (rate<65536) { TCCR1B |= (1 << CS10); OCR1A = rate; } else {
  rate=rate/8;
  if (rate<65536) { TCCR1B |= (1 << CS11); OCR1A = rate; } else {
  rate=rate/8;
  if (rate<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS11); OCR1A = rate; } else {
  rate=rate/4;  
  if (rate<65536) { TCCR1B |= (1 << CS12); OCR1A = rate; } else {
  rate=rate/4;
  if (rate<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS12); OCR1A = rate;
  }}}}}

  // CTC mode
  TCCR1B |= (1 << WGM12);
  // timer compare interrupt enable
  TIMSK1 |= (1 << OCIE1A);
}
// set timer to rate (in microseconds*16)
volatile long nextHArate;
void Timer3SetRate(long rate) {
  // set compare match register to desired timer count:
  rate--;
  if (rate<65536) { TCCR3B = (1 << WGM12) | (1 << CS10); } else {
  rate=rate/8;
  if (rate<65536) {  TCCR3B = (1 << WGM12) | (1 << CS11);  } else {
  rate=rate/8;
  if (rate<65536) {  TCCR3B = (1 << WGM12) | (1 << CS10) | (1 << CS11);   } else {
  rate=rate/4;  
  if (rate<65536) { TCCR3B = (1 << WGM12) | (1 << CS12);   } else {
  rate=rate/4;
  if (rate<65536) { TCCR3B = (1 << WGM12) | (1 << CS10) | (1 << CS12);  
  }}}}}
  nextHArate=rate;
}
// set timer to rate (in microseconds*16)
volatile long nextDErate;
void Timer4SetRate(long rate) {
  // set compare match register to desired timer count:
  rate--;
  if (rate<65536) { TCCR4B = (1 << WGM12) | (1 << CS10); } else {
  rate=rate/8;
  if (rate<65536) { TCCR4B = (1 << WGM12) | (1 << CS11); } else {
  rate=rate/8;
  if (rate<65536) { TCCR4B = (1 << WGM12) | (1 << CS10) | (1 << CS11); } else {
  rate=rate/4;  
  if (rate<65536) { TCCR4B = (1 << WGM12) | (1 << CS12); } else {
  rate=rate/4;
  if (rate<65536) {  TCCR4B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
  }}}}}
  nextDErate=rate;
}

//--------------------------------------------------------------------------------------------------
// Timer1 handles just the sidereal time, about 1mS rate

ISR(TIMER1_COMPA_vect)
{
  lst++;

  if (trackingState==TrackingSidereal) {
    long thisSkipCount=skipCountHA;
    // override rate during backlash compensation
    if (inBacklashHA) thisSkipCount=skipCountBacklashHA;
    if (thisSkipCount!=isrSkipCountHA) {
      Timer3SetRate(thisSkipCount);
      isrSkipCountHA=thisSkipCount;
    }

    thisSkipCount=skipCountDec;
    // override rate during backlash compensation
    if (inBacklashDec) thisSkipCount=skipCountBacklashDec;
    if (thisSkipCount!=isrSkipCountDec) {
      Timer4SetRate(thisSkipCount);
      isrSkipCountDec=thisSkipCount;
    }
  } else {
    if (skipCountHA!=isrSkipCountHA) {
      Timer3SetRate(skipCountHA);
      isrSkipCountHA=skipCountHA;
    } else
    if (skipCountDec!=isrSkipCountDec) {
      Timer4SetRate(skipCountDec);
      isrSkipCountDec=skipCountDec;
    }
  }

}

ISR(TIMER3_COMPA_vect)
{
  // routine might run in say 5uS?
  // drivers step on the rising edge, they need 1uS to settle so this is early in the routine to allow time after they're primed for a step
  CLR(HAStepPORT,  HAStepBit);

  if (posHA<(targetHA+PEC_HA)) dirHA=1; else dirHA=0;                        // Direction control
#ifdef ReverseHA
  if (HADir==dirHA) CLR(HADirPORT, HADirBit); else SET(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
#else
  if (HADir==dirHA) SET(HADirPORT, HADirBit); else CLR(HADirPORT, HADirBit); // Set direction, HADir default LOW (=0, for my wiring)
#endif

  if (posHA!=(targetHA+PEC_HA)) {                  // Move the RA stepper to the target

    // telescope moves WEST with the sky, blHA is the amount of EAST backlash
    if (dirHA==1) {
     if (blHA<backlashHA) { blHA++; inBacklashHA=true; } else { inBacklashHA=false; posHA++; }
    } else {
      if (blHA>0)         { blHA--; inBacklashHA=true; } else { inBacklashHA=false; posHA--; }
    }

    SET(HAStepPORT, HAStepBit);
  }
  OCR3A = nextHArate;
}

ISR(TIMER4_COMPA_vect)
{
  // routine might run in say 5uS?
  // drivers step on the rising edge, they need 1uS to settle so this is early in the routine to allow time after they're primed for a step
  CLR(DecStepPORT, DecStepBit);
  
  // telescope normally starts on the EAST side of the pier looking at the WEST sky
  if (posDec<targetDec) dirDec=1; else dirDec=0;                                   // Direction control
#ifdef ReverseDec
  if (DecDir==dirDec) SET(DecDirPORT, DecDirBit); else CLR(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
#else
  if (DecDir==dirDec) CLR(DecDirPORT, DecDirBit); else SET(DecDirPORT, DecDirBit); // Set direction, decDir default HIGH (=1, for my wiring)
#endif

  if (posDec!=targetDec) {                         // move the Dec stepper to the target

    // telescope moving NORTH in the sky, blDec is the amount of SOUTH backlash
    if (dirDec==1) {
      if (blDec<backlashDec) { blDec++; inBacklashDec=true; } else { inBacklashDec=false; posDec++; }
    } else {
      if (blDec>0)           { blDec--; inBacklashDec=true; } else { inBacklashDec=false; posDec--; }
    }

    SET(DecStepPORT, DecStepBit);
  }

  OCR4A = nextDErate;
}

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
