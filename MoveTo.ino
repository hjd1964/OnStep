// -----------------------------------------------------------------------------------
// functions to move the mount to the a new position

// moves the mount
void moveTo() {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, first phase.  only happens for GEM mounts
  if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipWE1)) {
    
    // save destination
    cli(); 
    origTargetHA =longToFixed(targetHA);
    origTargetDec=targetDec;
 
    // first phase, move to 60 HA (4 hours)
    timerRateHA =SiderealRate;
    timerRateDec=SiderealRate;
    sei();

    // default
    cli(); 
    if (pierSide==PierSideFlipWE1) targetHA=-60L*StepsPerDegreeHA; else targetHA=60L*StepsPerDegreeHA;
    targetDec=celestialPoleDec*StepsPerDegreeDec;
    sei();

    cli();
    if (celestialPoleDec>0) {
      // if Dec is in the general area of the pole, slew both axis back at once
      if ((posDec/(double)StepsPerDegreeDec)>90-latitude) {
        if (pierSide==PierSideFlipWE1) targetHA=-celestialPoleHA*StepsPerDegreeHA; else targetHA=celestialPoleHA*StepsPerDegreeHA;
      } else {
        // override if we're at a low latitude and in the opposite sky, leave the HA alone
        if ((abs(latitude)<45.0) && (posDec<0)) {
          if (pierSide==PierSideFlipWE1) targetHA=-45L*StepsPerDegreeHA; else targetHA=45L*StepsPerDegreeHA;
        }
      }
    } else {
      // if Dec is in the general area of the pole, slew both axis back at once
      if ((posDec/(double)StepsPerDegreeDec)<-90-latitude) {
        if (pierSide==PierSideFlipWE1) targetHA=-celestialPoleHA*StepsPerDegreeHA; else targetHA=celestialPoleHA*StepsPerDegreeHA; 
      } else { 
        // override if we're at a low latitude and in the opposite sky, leave the HA alone
        if ((abs(latitude)<45.0) && (posDec>0)) {
          if (pierSide==PierSideFlipWE1) targetHA=-45L*StepsPerDegreeHA; else targetHA=45L*StepsPerDegreeHA; 
        }
      }
    }
    sei();

    fTargetHA =longToFixed(targetHA);
    fTargetDec=longToFixed(targetDec);

    pierSide++;
  }

  long distStartHA,distStartDec,distDestHA,distDestDec;

  cli();
  distStartHA=abs(posHA-startHA);           // distance from start HA
  distStartDec=abs(posDec-startDec);        // distance from start Dec
  sei();
  if (distStartHA<1)  distStartHA=1;
  if (distStartDec<1) distStartDec=1;
  
  Again:
  cli();
  distDestHA=abs(posHA-targetHA);           // distance from dest HA
  distDestDec=abs(posDec-targetDec);        // distance from dest Dec
  sei();
  if (distDestHA<1)  distDestHA=1;
  if (distDestDec<1) distDestDec=1;

  // quickly slow the motors and stop in 1 degree
  if (abortSlew) {
    // aborts any meridian flip
    if ((pierSide==PierSideFlipWE1) || (pierSide==PierSideFlipWE2) || (pierSide==PierSideFlipWE3)) pierSide=PierSideWest;
    if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipEW3)) pierSide=PierSideEast;

    // set the destination near where we are now
    // todo: the new targetHA/Dec should be checked against the old targetHA/Dec, if the old targetHA/Dec is closer than use it
    cli();
    if (distDestHA>StepsPerDegreeHA)   { if (posHA>targetHA)   targetHA =posHA-StepsPerDegreeHA;   else targetHA =posHA +StepsPerDegreeHA; fTargetHA=longToFixed(targetHA); }
    if (distDestDec>StepsPerDegreeDec) { if (posDec>targetDec) targetDec=posDec-StepsPerDegreeDec; else targetDec=posDec+StepsPerDegreeDec; fTargetDec=longToFixed(targetDec); }
    sei();
    
    abortSlew=false;
    goto Again;
  }

  // First, for Right Ascension
  unsigned long temp;
  if (distStartHA>distDestHA) {
    temp=(StepsForRateChange/isqrt32(distDestHA));    // slow down (temp gets bigger)
//  if ((temp<100) && (temp>=10))  temp=101;          // exclude a range of speeds
  } else {
    temp=(StepsForRateChange/isqrt32(distStartHA));   // speed up (temp gets smaller)
//  if ((temp<100) && (temp>=10))  temp=9;            // exclude a range of speeds
  }
  if (temp<maxRate) temp=maxRate;                     // fastest rate
  if (temp>TakeupRate) temp=TakeupRate;               // slowest rate (4x sidereal)
  cli(); timerRateHA=temp; sei();

  // Now, for Declination
  if (distStartDec>distDestDec) {
      temp=(StepsForRateChange/isqrt32(distDestDec)); // slow down
//    if ((temp<100) && (temp>=10))  temp=101;        // exclude a range of speeds
    } else {
      temp=(StepsForRateChange/isqrt32(distStartDec));// speed up
//    if ((temp<100) && (temp>=10))  temp=9;          // exclude a range of speeds
    }
  if (temp<maxRate) temp=maxRate;                     // fastest rate
  if (temp>TakeupRate) temp=TakeupRate;               // slowest rate (4x sidereal)
  cli(); timerRateDec=temp; sei();

  if ((distDestHA<=2) && (distDestDec<=2)) { 
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      // make sure we're at the home position when flipping sides of the mount
      cli();
      startHA=posHA; if (pierSide==PierSideFlipWE2) targetHA=-celestialPoleHA*StepsPerDegreeHA; else targetHA=celestialPoleHA*StepsPerDegreeHA; 
      startDec=posDec; targetDec=celestialPoleDec*StepsPerDegreeDec; 
      sei();
      fTargetHA=longToFixed(targetHA);
      fTargetDec=longToFixed(targetDec);
      pierSide++;
    } else
    if ((pierSide==PierSideFlipEW3) || (pierSide==PierSideFlipWE3)) {
      
      // the blDec gets "reversed" when we Meridian flip, since the NORTH/SOUTH movements are reversed
      cli(); blDec=backlashDec-blDec; sei();
      // also the Index is reversed since we just flipped 180 degrees
      ID=-ID;

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
      targetHA =fixedToLong(origTargetHA);
      startDec =posDec;
      targetDec=origTargetDec;
      sei();
      fTargetHA=origTargetHA;
      fTargetDec=longToFixed(targetDec);
    } else {
      // restore last tracking state
      trackingState=lastTrackingState;
      SetSiderealClockRate(siderealInterval);

      cli();
      timerRateHA  =SiderealRate;
      timerRateDec =SiderealRate;
      sei();
      
      // other special gotos: for parking the mount and homeing the mount
      if (parkStatus==Parking) {

        // give the drives a moment to settle in
        delay(3000);

        if ((posHA==targetHA) && (posDec==targetDec)) {
          
          if (parkClearBacklash()) {
            // success, we're parked
            parkStatus=Parked; EEPROM.write(EE_parkStatus,parkStatus);

            // just store the indexes of our pointing model
            EEPROM_writeQuad(EE_IH,(byte*)&IH);
            EEPROM_writeQuad(EE_ID,(byte*)&ID);
            
            // disable the stepper drivers
            digitalWrite(HA_EN,HIGH);
            digitalWrite(DE_EN,HIGH);


          } else parkStatus=ParkFailed;
      } else parkStatus=ParkFailed;

      } else
        if (homeMount) { 
          setHome(); 
          homeMount=false; 
          atHome=true;
        }
    }
  }
}

// fast integer square root routine, Integer Square Roots by Jack W. Crenshaw
uint32_t isqrt32 (uint32_t n) {
    register uint32_t root=0, remainder, place= 0x40000000;
    remainder = n;

    while (place > remainder) place = place >> 2;
    while (place) {
        if (remainder >= root + place) {
            remainder = remainder - root - place;
            root = root + (place << 1);
        }
        root = root >> 1;
        place = place >> 2;
    }
    return root;
}
