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
 
    // first phase, move to 90 HA
    timerRateHA =SiderealRate*2;
    #ifdef DEC_RATIO_ON
    timerRateDec=SiderealRate*timerRateRatio;
    #else
    timerRateDec=SiderealRate;
    #endif

    // just move HA, not the declination yet
    if (pierSide==PierSideFlipWE1) targetHA=-90L*StepsPerDegreeHA; else targetHA=90L*StepsPerDegreeHA; 
    targetDec=posDec;
    sei();
    
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
  long timerRate=(timerRateHA/10L); if (timerRate<1) timerRate=1; // lower limit always allows change

  double temp;
  if (distStartHA>distDestHA) {
    temp=(StepsForRateChange/sqrt(distDestHA));       // 50000/40000=1.02  50000/10=5000 slow down the slew
//    if ((temp<100) && (temp>=10))  temp=101;          // exclude a range of speeds
  } else {
    temp=(StepsForRateChange/sqrt(distStartHA));      // speed up the slew
//    if ((temp<100) && (temp>=10))  temp=9;            // exclude a range of speeds
  }
  if (temp<MaxRate)        temp=MaxRate;              // fastest rate
  if (temp>SiderealRate/2L) temp=SiderealRate/2L;       // slowest rate (4x sidereal), remember SiderealRate is actually twice the sidereal rate

  cli();  timerRateHA=round(temp);  sei();

  // Now, for Declination
  timerRate=(timerRateDec/10L); if (timerRate<1) timerRate=1; // lower limit always allows change
  
  if (distStartDec>distDestDec) {
      temp=(StepsForRateChange/sqrt(distDestDec));    // 50000/40000=1.02  50000/10=5000 slow down the slew
//      if ((temp<100) && (temp>=10))  temp=101;        // exclude a range of speeds
    } else {
      temp=(StepsForRateChange/sqrt(distStartDec));   // speed up the slew
//      if ((temp<100) && (temp>=10))  temp=9;          // exclude a range of speeds
    }
  if (temp<MaxRate)        temp=MaxRate;              // fastest rate
  if (temp>SiderealRate/2L) temp=SiderealRate/2L;       // slowest rate (4x sidereal)

  cli(); timerRateDec=round(temp); sei();

  if ((distDestHA<=2) && (distDestDec<=2)) { 
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      pierSide++;
      // move to the home position first when flipping sides of the mount
      cli(); startDec=posDec; targetDec=90L*StepsPerDegreeDec; sei();
    } else
    if ((pierSide==PierSideFlipEW3) || (pierSide==PierSideFlipWE3)) {
      
      cli();
      // the blDec gets "reversed" when we Meridian flip, since the NORTH/SOUTH movements are reversed
    // 80  =   120     - 40
      blDec=backlashDec-blDec;
      sei();

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
      // restore normal sidereal tracking 2x in RA, 1x in Dec
      trackingState=lastTrackingState;
      cli(); 
      timerRateHA  =SiderealRate;
      #ifdef DEC_RATIO_ON
      timerRateDec =SiderealRate*2*timerRateRatio;
      #else
      timerRateDec =SiderealRate*2;
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


