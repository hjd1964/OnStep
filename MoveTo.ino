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
    origTargetHA.fixed = targetHA.fixed;
    origTargetDec = (long int)targetDec.part.m;
 
    timerRateHA =SiderealRate;
    timerRateDec=SiderealRate;
    sei();

    // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
    cli();
    if (pierSide==PierSideFlipWE1) { 
      if ((currentAlt<10.0) && (startHA>-90L*StepsPerDegreeHA)) targetHA.part.m=-60L*StepsPerDegreeHA; else targetHA.part.m=-celestialPoleHA*StepsPerDegreeHA; 
    } else {
      if ((currentAlt<10.0) && (startHA<90L*StepsPerDegreeHA)) targetHA.part.m=60L*StepsPerDegreeHA; else targetHA.part.m=celestialPoleHA*StepsPerDegreeHA; 
    } 
    targetHA.part.f=0;
    targetDec.part.m=celestialPoleDec*StepsPerDegreeDec;
    sei();

    cli();
    if (celestialPoleDec>0) {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (((double)posDec/(double)StepsPerDegreeDec)>90.0-latitude) {
        if (pierSide==PierSideFlipWE1) targetHA.part.m=-celestialPoleHA*StepsPerDegreeHA; else targetHA.part.m=celestialPoleHA*StepsPerDegreeHA; targetHA.part.f=0;
      } else {
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        // in the extreme case, where the user is very near the (Earths!) equator an Horizon limit of -10 or -15 may be necessary for proper operation.
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posDec<0)) {
          if (pierSide==PierSideFlipWE1) targetHA.part.m=-45L*StepsPerDegreeHA; else targetHA.part.m=45L*StepsPerDegreeHA; targetHA.part.f=0;
        }
      }
    } else {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (((double)posDec/(double)StepsPerDegreeDec)<-90.0-latitude) {
        if (pierSide==PierSideFlipWE1) targetHA.part.m=-celestialPoleHA*StepsPerDegreeHA; else targetHA.part.m=celestialPoleHA*StepsPerDegreeHA; targetHA.part.f=0;
      } else { 
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posDec>0)) {
          if (pierSide==PierSideFlipWE1) targetHA.part.m=-45L*StepsPerDegreeHA; else targetHA.part.m=45L*StepsPerDegreeHA; targetHA.part.f=0;
        }
      }
    }
    sei();

    pierSide++;
  }

  long distStartHA,distStartDec,distDestHA,distDestDec;

  cli();
  distStartHA=abs(posHA-startHA);    // distance from start HA
  distStartDec=abs(posDec-startDec); // distance from start Dec
  sei();
  if (distStartHA<1)  distStartHA=1;
  if (distStartDec<1) distStartDec=1;
  
  Again:
  cli();
  distDestHA=abs(posHA-(long int)targetHA.part.m);    // distance from dest HA
  distDestDec=abs(posDec-(long int)targetDec.part.m); // distance from dest Dec
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
    if (distDestHA>StepsPerDegreeHA)   { if (posHA>(long int)targetHA.part.m)   targetHA.part.m =posHA-StepsPerDegreeHA;   else targetHA.part.m =posHA +StepsPerDegreeHA; targetHA.part.f=0; }
    if (distDestDec>StepsPerDegreeDec) { if (posDec>(long int)targetDec.part.m) targetDec.part.m=posDec-StepsPerDegreeDec; else targetDec.part.m=posDec+StepsPerDegreeDec; targetDec.part.f=0; }
    sei();
    
    abortSlew=false;
    goto Again;
  }

  // First, for Right Ascension
  long temp;
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
#ifdef MaxRateDecRatio
  if (temp<maxRate*MaxRateDecRatio) temp=maxRate*MaxRateDecRatio; // fastest rate
#else
  if (temp<maxRate) temp=maxRate;                     // fastest rate
#endif
  if (temp>TakeupRate) temp=TakeupRate;               // slowest rate (4x sidereal)
  cli(); timerRateDec=temp; sei();

  if ((distDestHA<=2) && (distDestDec<=2)) { 
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      // make sure we're at the home position when flipping sides of the mount
      cli();
      startHA=posHA; if (pierSide==PierSideFlipWE2) targetHA.part.m=-celestialPoleHA*StepsPerDegreeHA; else targetHA.part.m=celestialPoleHA*StepsPerDegreeHA; targetHA.part.f=0;
      startDec=posDec; targetDec.part.m=celestialPoleDec*StepsPerDegreeDec; targetDec.part.f=0;
      sei();
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
      targetHA.fixed=origTargetHA.fixed;
      startDec =posDec;
      targetDec.part.m=origTargetDec; targetDec.part.f=0;
      sei();
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

        if ((posHA==(long int)targetHA.part.m) && (posDec==(long int)targetDec.part.m)) {
          
          if (parkClearBacklash()) {
            // success, we're parked
            parkStatus=Parked; EEPROM.write(EE_parkStatus,parkStatus);

            // just store the indexes of our pointing model
            EEPROM_writeQuad(EE_IH,(byte*)&IH);
            EEPROM_writeQuad(EE_ID,(byte*)&ID);
            
            // disable the stepper drivers
            digitalWrite(HA_EN,HA_Disabled);
            digitalWrite(DE_EN,DE_Disabled);

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
