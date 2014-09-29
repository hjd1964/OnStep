// -----------------------------------------------------------------------------------
// functions to move the mount to the a new position

// moves the mount
void moveTo() {
  // HA goes from +90...0..-90
  //                W   .   E
  if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipWE1)) {
    // save destination
    cli(); 
    origTargetHA =targetHA; 
    origTargetDec=targetDec;
 
    // first phase, move to 60 HA (4 hours)
    timerRateHA =SiderealRate;
    #ifdef DEC_RATIO_ON
    timerRateDec=SiderealRate*timerRateRatio;
    #else
    timerRateDec=SiderealRate;
    #endif

    // just move HA, not the declination yet
    if (pierSide==PierSideFlipWE1) targetHA=-60L*StepsPerDegreeHA; else targetHA=60L*StepsPerDegreeHA; 

    // if we're under the pole, slew both axis back at once
    if (abs((double)posDec/(double)StepsPerDegreeDec)>90.0) { 
      startDec=posDec; targetDec=celestialPoleDec*StepsPerDegreeDec;
    } else { 
      targetDec=posDec;
      if (abs(latitude)<45.0) {
        // override if we're at a low latitude and in the opposite sky, then goto 0 deg. declination
        if ((celestialPoleDec>0) && (posDec<0)) targetDec=0;
        if ((celestialPoleDec<0) && (posDec>0)) targetDec=0;
      }
    sei();
   }
    
    pierSide++;
  }
  cli();
  long distStartHA=abs(posHA-startHA);           // distance from start HA
  long distStartDec=abs(posDec-startDec);        // distance from start Dec
  long distDestHA;
  long distDestDec;
  
  Again:
  distDestHA =abs(posHA-targetHA);               // distance from dest HA
  distDestDec =abs(posDec-targetDec);            // distance from dest Dec

  // quickly slow the motors and stop in 1 degree
  if (abortSlew) {
    // aborts the meridian flip
    if ((pierSide==PierSideFlipWE1) || (pierSide==PierSideFlipWE2) || (pierSide==PierSideFlipWE3)) pierSide=PierSideWest;
    if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipEW3)) pierSide=PierSideEast;

    // set the destination near where we are now    
    if (distDestHA>StepsPerDegreeHA)   { if (posHA>targetHA)   targetHA =posHA-StepsPerDegreeHA;   else targetHA =posHA +StepsPerDegreeHA;  }
    if (distDestDec>StepsPerDegreeDec) { if (posDec>targetDec) targetDec=posDec-StepsPerDegreeDec; else targetDec=posDec+StepsPerDegreeDec; }

    abortSlew=false;
    goto Again;
  }
  
  sei();

  // First, for Right Ascension
  double temp;
  if (distStartHA>distDestHA) {
    temp=(StepsForRateChange/sqrt(distDestHA));       // 50000/40000=1.02  50000/10=5000 slow down the slew
//    if ((temp<100) && (temp>=10))  temp=101;        // exclude a range of speeds
  } else {
    temp=(StepsForRateChange/sqrt(distStartHA));      // speed up the slew
//    if ((temp<100) && (temp>=10))  temp=9;          // exclude a range of speeds
  }
  if (temp<MaxRate*16)     temp=MaxRate*16;           // fastest rate
  if (temp>SiderealRate/4) temp=SiderealRate/4;       // slowest rate (4x sidereal)

  cli();  timerRateHA=temp;  sei();

  // Now, for Declination
  if (distStartDec>distDestDec) {
      temp=(StepsForRateChange/sqrt(distDestDec));    // 50000/40000=1.02  50000/10=5000 slow down the slew
//      if ((temp<100) && (temp>=10))  temp=101;      // exclude a range of speeds
    } else {
      temp=(StepsForRateChange/sqrt(distStartDec));   // speed up the slew
//      if ((temp<100) && (temp>=10))  temp=9;        // exclude a range of speeds
    }
  #ifdef DEC_RATIO_ON
  if (temp<MaxRate*16*timerRateRatio) temp=MaxRate*16*timerRateRatio; // fastest rate
  #else
  if (temp<MaxRate*16)                temp=MaxRate*16;
  #endif
  if (temp>SiderealRate/4)            temp=SiderealRate/4; // slowest rate (4x sidereal)

  cli(); timerRateDec=temp; sei();

  if ((distDestHA<=2) && (distDestDec<=2)) { 
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      // make sure we're at the home position when flipping sides of the mount
      cli();
      if (pierSide==PierSideFlipWE2) targetHA=-90L*StepsPerDegreeHA; else targetHA=90L*StepsPerDegreeHA; 
      startDec=posDec; targetDec=celestialPoleDec*StepsPerDegreeDec; 
      sei();
      pierSide++;
    } else
    if ((pierSide==PierSideFlipEW3) || (pierSide==PierSideFlipWE3)) {
      
      // the blDec gets "reversed" when we Meridian flip, since the NORTH/SOUTH movements are reversed
      cli(); blDec=backlashDec-blDec; sei();

      if (pierSide==PierSideFlipEW3) {
        pierSide=PierSideWest;
        cli();
        // reverse the Declination movement
        DecDir  = DecDirWInit;
        // if we were on the east side of the pier the HA's were in the western sky, and were positive
        // now we're in the eastern sky and the HA's are negative
        posHA=posHA-180L*StepsPerDegreeHA;
        sei();
      } else {
        pierSide=PierSideEast;
        cli();
        // normal Declination
        DecDir  = DecDirEInit;      
        // if we were on the west side of the pier the HA's were in the eastern sky, and were negative
        // now we're in the western sky and the HA's are positive
        posHA=posHA+180L*StepsPerDegreeHA;
        sei();
      }
    
      // now complete the slew
      cli();
      startHA  =posHA;
      targetHA =origTargetHA;
      startDec =posDec;
      targetDec=origTargetDec;
      sei();
    } else {
      // restore normal sidereal tracking
      trackingState=lastTrackingState;
      cli(); 
      timerRateHA  =SiderealRate;
      #ifdef DEC_RATIO_ON
      timerRateDec =SiderealRate*timerRateRatio;
      #else
      timerRateDec =SiderealRate;
      #endif
      sei();
      
      // other special gotos: for parking the mount and homeing the mount
      if (parkStatus==Parking) {
        // give the drives a moment to settle in
        delay(3000);
        if ((posHA==targetHA) && (posDec==targetDec)) {
          
          if (parkClearBacklash()) {
            // success, we're parked
            parkStatus=Parked; EEPROM.write(EE_parkStatus,parkStatus);
            
            // and store our pointing model too, just in-case something was updated
            EEPROM_writeQuad(EE_altCor,(byte*)&altCor);
            EEPROM_writeQuad(EE_azmCor,(byte*)&azmCor);
            EEPROM_writeQuad(EE_IH,(byte*)&IH);
            EEPROM_writeQuad(EE_ID,(byte*)&ID);
 
          } else parkStatus=ParkFailed;

      } else parkStatus=ParkFailed;

      } else
        if (homeMount) { setHome(); homeMount=false; atHome=true; }
    }
  }
}

