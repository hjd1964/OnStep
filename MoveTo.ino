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
    origTargetAxis1.fixed = targetAxis1.fixed;
    origTargetAxis2 = (long)targetAxis2.part.m;
 
    timerRateAxis1 =SiderealRate;
    timerRateAxis2=SiderealRate;
    sei();

    // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
    cli();
    if (pierSide==PierSideFlipWE1) { 
      if (celestialPoleHA==0) targetAxis1.part.m=0*StepsPerDegreeAxis1-IHS; else {
        if ((currentAlt<10.0) && (startAxis1>-90L*StepsPerDegreeAxis1-IHS)) targetAxis1.part.m=-60L*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=-celestialPoleHA*StepsPerDegreeAxis1-IHS;
      }
    } else {
      if (celestialPoleHA==0) targetAxis1.part.m=0*StepsPerDegreeAxis1; else {
        if ((currentAlt<10.0) && (startAxis1<90L*StepsPerDegreeAxis1-IHS)) targetAxis1.part.m=60L*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=celestialPoleHA*StepsPerDegreeAxis1-IHS; 
      }
    } 
    targetAxis1.part.f=0;
    targetAxis2.part.m=((long)(celestialPoleDec*StepsPerDegreeAxis2))-IDS; targetAxis2.part.f=0;
    sei();

    cli();
    if (celestialPoleDec>0) {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (posAxis2>((long)((90.0-latitude)*StepsPerDegreeAxis2))-IDS) {
        if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-celestialPoleHA*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=celestialPoleHA*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
      } else {
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        // in the extreme case, where the user is very near the (Earths!) equator an Horizon limit of -10 or -15 may be necessary for proper operation.
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posAxis2<0-IDS)) {
          if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-45L*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=45L*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
        }
      }
    } else {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (posAxis2<((long)((-90.0-latitude)*StepsPerDegreeAxis2))-IDS) {
        if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-celestialPoleHA*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=celestialPoleHA*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
      } else { 
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posAxis2>0-IDS)) {
          if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-45L*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=45L*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
        }
      }
    }
    sei();

    pierSide++;
  }

  long diststartAxis1,distStartDec,distDestHA,distDestDec;

  cli();
  diststartAxis1=abs(posAxis1-startAxis1);    // distance from start HA
  distStartDec=abs(posAxis2-startAxis2); // distance from start Dec
  sei();
  if (diststartAxis1<1)  diststartAxis1=1;
  if (distStartDec<1) distStartDec=1;
  
  Again:
  cli();
  distDestHA=abs(posAxis1-(long)targetAxis1.part.m);  // distance from dest HA
  distDestDec=abs(posAxis2-(long)targetAxis2.part.m); // distance from dest Dec
  sei();
  if (distDestHA<1)  distDestHA=1;
  if (distDestDec<1) distDestDec=1;

  // quickly slow the motors and stop in 1 degree
  if (abortSlew) {
    // aborts any meridian flip
    if ((pierSide==PierSideFlipWE1) || (pierSide==PierSideFlipWE2) || (pierSide==PierSideFlipWE3)) pierSide=PierSideWest;
    if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipEW3)) pierSide=PierSideEast;

    // set the destination near where we are now
    // todo: the new targetAxis1/Dec should be checked against the old targetAxis1/Dec, if the old targetAxis1/Dec is closer than use it
    cli();
    if (distDestHA>StepsPerDegreeAxis1)   { if (posAxis1>(long)targetAxis1.part.m)   targetAxis1.part.m =posAxis1-StepsPerDegreeAxis1;   else targetAxis1.part.m =posAxis1 +StepsPerDegreeAxis1; targetAxis1.part.f=0; }
    if (distDestDec>StepsPerDegreeAxis2) { if (posAxis2>(long)targetAxis2.part.m) targetAxis2.part.m=posAxis2-StepsPerDegreeAxis2; else targetAxis2.part.m=posAxis2+StepsPerDegreeAxis2; targetAxis2.part.f=0; }
    sei();

    if (parkStatus==Parking) {
      lastTrackingState=abortTrackingState;
      parkStatus=NotParked;
      EEPROM.write(EE_parkStatus,parkStatus);
    } else
    if (homeMount) {
      lastTrackingState=abortTrackingState;
      homeMount=false;
    }
    
    abortSlew=false;
    goto Again;
  }

  // First, for Right Ascension
  long temp;
  if (diststartAxis1>distDestHA) {
    temp=(StepsForRateChangeAxis1/isqrt32(distDestHA));    // slow down (temp gets bigger)
//  if ((temp<100) && (temp>=10))  temp=101;          // exclude a range of speeds
  } else {
    temp=(StepsForRateChangeAxis1/isqrt32(diststartAxis1));   // speed up (temp gets smaller)
//  if ((temp<100) && (temp>=10))  temp=9;            // exclude a range of speeds
  }
  if (temp<maxRate) temp=maxRate;                     // fastest rate
  if (temp>TakeupRate) temp=TakeupRate;               // slowest rate (4x sidereal)
  cli(); timerRateAxis1=temp; sei();

  // Now, for Declination
  if (distStartDec>distDestDec) {
      temp=(StepsForRateChangeAxis2/isqrt32(distDestDec)); // slow down
//    if ((temp<100) && (temp>=10))  temp=101;        // exclude a range of speeds
    } else {
      temp=(StepsForRateChangeAxis2/isqrt32(distStartDec)); // speed up
//    if ((temp<100) && (temp>=10))  temp=9;          // exclude a range of speeds
    }
#ifdef MaxRateDecRatio
  if (temp<maxRate*MaxRateDecRatio) temp=maxRate*MaxRateDecRatio; // fastest rate
#else
  if (temp<maxRate) temp=maxRate;                     // fastest rate
#endif
  if (temp>TakeupRate) temp=TakeupRate;               // slowest rate (4x sidereal)
  cli(); timerRateAxis2=temp; sei();

  if ((distDestHA<=2) && (distDestDec<=2)) { 
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      // make sure we're at the home position when flipping sides of the mount
      if (celestialPoleHA==0) {
        cli();
        startAxis1=posAxis1;
        if (pierSide==PierSideFlipWE2) targetAxis1.part.m=-180L*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=180L*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
        startAxis2=posAxis2; targetAxis2.part.m=celestialPoleDec*StepsPerDegreeAxis2-IDS; targetAxis2.part.f=0;
        sei();
      } else {
        cli();
        startAxis1=posAxis1;
        if (pierSide==PierSideFlipWE2) targetAxis1.part.m=-celestialPoleHA*StepsPerDegreeAxis1-IHS; else targetAxis1.part.m=celestialPoleHA*StepsPerDegreeAxis1-IHS; targetAxis1.part.f=0;
        startAxis2=posAxis2; targetAxis2.part.m=celestialPoleDec*StepsPerDegreeAxis2-IDS; targetAxis2.part.f=0;
        sei();
      }
      
      pierSide++;
    } else
    if ((pierSide==PierSideFlipEW3) || (pierSide==PierSideFlipWE3)) {
      
      // the blAxis2 gets "reversed" when we Meridian flip, since the NORTH/SOUTH movements are reversed
      cli(); blAxis2=backlashAxis2-blAxis2; sei();
      // also the Index is reversed since we just flipped 180 degrees
   //   ID=-ID; IDS=-IDS;

      if (pierSide==PierSideFlipEW3) {
        pierSide=PierSideWest;
        cli();
        // reverse the Declination movement
        DecDir  = DecDirWInit;
        // if we were on the east side of the pier the HA's were in the western sky, and were positive
        // now we're in the eastern sky and the HA's are negative
        posAxis1=posAxis1-180L*StepsPerDegreeAxis1;
        sei();
      } else {
        pierSide=PierSideEast;
        cli();
        // normal Declination
        DecDir  = DecDirEInit;      
        // if we were on the west side of the pier the HA's were in the eastern sky, and were negative
        // now we're in the western sky and the HA's are positive
        posAxis1=posAxis1+180L*StepsPerDegreeAxis1;
        sei();
      }
    
      // now complete the slew
      cli();
      startAxis1  =posAxis1;
      targetAxis1.fixed=origTargetAxis1.fixed;
      startAxis2 =posAxis2;
      targetAxis2.part.m=origTargetAxis2; targetAxis2.part.f=0;
      sei();
    } else {
      // restore last tracking state
      trackingState=lastTrackingState; SetSiderealClockRate(siderealInterval);
      cli();
      timerRateAxis1  =SiderealRate;
      timerRateAxis2 =SiderealRate;
      sei();
      
      // other special gotos: for parking the mount and homeing the mount
      if (parkStatus==Parking) {

        // give the drives a moment to settle in
        for (int i=0; i<12; i++) if ((posAxis1!=(long)targetAxis1.part.m) || (posAxis2!=(long)targetAxis2.part.m)) delay(250);

        if ((posAxis1==(long)targetAxis1.part.m) && (posAxis2==(long)targetAxis2.part.m)) {
          if (parkClearBacklash()) {
            // success, we're parked
            parkStatus=Parked; EEPROM.write(EE_parkStatus,parkStatus);

            // just store the indexes of our pointing model
            EEPROM_writeQuad(EE_IH,(byte*)&IH);
            EEPROM_writeQuad(EE_ID,(byte*)&ID);
            
            // disable the stepper drivers
            digitalWrite(Axis1_EN,Axis1_Disabled);
            digitalWrite(Axis2_EN,Axis2_Disabled);

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
