// -----------------------------------------------------------------------------------
// Functions to move the mount to the a new position

long lastPosAxis2=0;

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
 
    timerRateAxis1=SiderealRate;
    timerRateAxis2=SiderealRate;
    sei();

    // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
    cli();
    if (pierSide==PierSideFlipWE1) { 
      if (celestialPoleAxis1==0) targetAxis1.part.m=0*(long)StepsPerDegreeAxis1-indexAxis1Steps; else {
        if ((currentAlt<10.0) && (startAxis1>-90L*(long)StepsPerDegreeAxis1-indexAxis1Steps)) targetAxis1.part.m=-60L*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=-(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps;
      }
    } else {
      if (celestialPoleAxis1==0) targetAxis1.part.m=0*(long)StepsPerDegreeAxis1-indexAxis1Steps; else {
        if ((currentAlt<10.0) && (startAxis1<90L*(long)StepsPerDegreeAxis1-indexAxis1Steps)) targetAxis1.part.m=60L*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps;
      }
    } 
    targetAxis1.part.f=0;
    targetAxis2.part.m=((long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2))-indexAxis2Steps; targetAxis2.part.f=0;
    sei();

    cli();
    if (celestialPoleAxis2>0) {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (posAxis2>((long)((90.0-latitude)*(double)StepsPerDegreeAxis2))-indexAxis2Steps) {
        if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; else targetAxis1.part.m=(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; targetAxis1.part.f=0;
      } else {
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        // in the extreme case, where the user is very near the (Earths!) equator an Horizon limit of -10 or -15 may be necessary for proper operation.
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posAxis2<0-indexAxis2Steps)) {
          if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-45L*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=45L*(long)StepsPerDegreeAxis1-indexAxis1Steps; targetAxis1.part.f=0;
        }
      }
    } else {
      // if Dec is in the general area of the pole, slew both axis back at once
      if (posAxis2<((long)((-90.0-latitude)*(double)StepsPerDegreeAxis2))-indexAxis2Steps) {
        if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; else targetAxis1.part.m=(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; targetAxis1.part.f=0;
      } else { 
        // override if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (posAxis2>0-indexAxis2Steps)) {
          if (pierSide==PierSideFlipWE1) targetAxis1.part.m=-45L*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=45L*(long)StepsPerDegreeAxis1-indexAxis1Steps; targetAxis1.part.f=0;
        }
      }
    }
    sei();

    pierSide++;
  }

  long distStartAxis1,distStartAxis2,distDestAxis1,distDestAxis2;

  cli();
  distStartAxis1=abs(posAxis1-startAxis1);  // distance from start Axis1
  distStartAxis2=abs(posAxis2-startAxis2);  // distance from start Axis2
  sei();
  if (distStartAxis1<1) distStartAxis1=1;
  if (distStartAxis2<1) distStartAxis2=1;
  
  Again:
  cli();
  long tempPosAxis2=posAxis2;
  distDestAxis1=abs(posAxis1-(long)targetAxis1.part.m);      // distance from dest Axis1
  distDestAxis2=abs(tempPosAxis2-(long)targetAxis2.part.m);  // distance from dest Axis2
  sei();

  // adjust rates near the horizon to help keep from exceeding the minAlt limit
  #ifndef MOUNT_TYPE_ALTAZM
  tempPosAxis2+=indexAxis2Steps; if (latitude<0) tempPosAxis2=-tempPosAxis2;
  if (((latitude>10) || (latitude<-10)) && (distStartAxis1>((DegreesForAcceleration*StepsPerDegreeAxis1)/16))) {
    // if Dec is decreasing, slow down Dec
    if (tempPosAxis2<lastPosAxis2) {
      cli();
      double minAlt2=minAlt+10.0;
      long a=(currentAlt-minAlt2)*StepsPerDegreeAxis2; if (a<((DegreesForAcceleration*StepsPerDegreeAxis2)/8)) a=((DegreesForAcceleration*StepsPerDegreeAxis2)/8);
      if (a<distDestAxis2) distDestAxis2=a;
      sei();
    } else
    // if Dec is increasing, slow down HA
    {
      cli();
      double minAlt2=minAlt+10.0;
      long a=(currentAlt-minAlt2)*StepsPerDegreeAxis1; if (a<((DegreesForAcceleration*StepsPerDegreeAxis1)/8)) a=((DegreesForAcceleration*StepsPerDegreeAxis1)/8);
      if (a<distDestAxis1) distDestAxis1=a;
      sei();
    }
  }
  lastPosAxis2=tempPosAxis2;
  #endif

  if (distDestAxis1<1) distDestAxis1=1;
  if (distDestAxis2<1) distDestAxis2=1;

  // quickly slow the motors and stop in DegreesForRapidStop
  if (abortSlew) {
    // aborts any meridian flip
    if ((pierSide==PierSideFlipWE1) || (pierSide==PierSideFlipWE2) || (pierSide==PierSideFlipWE3)) pierSide=PierSideWest;
    if ((pierSide==PierSideFlipEW1) || (pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipEW3)) pierSide=PierSideEast;
    if (pauseHome) { waitingHome=false; waitingHomeContinue=false; }

    // set the destination near where we are now
    cli();
    if (distDestAxis1>(long)(StepsPerDegreeAxis1*DegreesForRapidStop)) { if (posAxis1>(long)targetAxis1.part.m) targetAxis1.part.m=posAxis1-(long)(StepsPerDegreeAxis1*DegreesForRapidStop); else targetAxis1.part.m=posAxis1+(long)(StepsPerDegreeAxis1*DegreesForRapidStop); targetAxis1.part.f=0; }
    if (distDestAxis2>(long)(StepsPerDegreeAxis2*DegreesForRapidStop)) { if (posAxis2>(long)targetAxis2.part.m) targetAxis2.part.m=posAxis2-(long)(StepsPerDegreeAxis2*DegreesForRapidStop); else targetAxis2.part.m=posAxis2+(long)(StepsPerDegreeAxis2*DegreesForRapidStop); targetAxis2.part.f=0; }
    sei();

    if (parkStatus==Parking) {
      lastTrackingState=abortTrackingState;
      parkStatus=NotParked;
      nv.write(EE_parkStatus,parkStatus);
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
  if (distStartAxis1>distDestAxis1) {
    temp=(StepsForRateChangeAxis1/isqrt32(distDestAxis1));   // slow down (temp gets bigger)
  } else {
    temp=(StepsForRateChangeAxis1/isqrt32(distStartAxis1));  // speed up (temp gets smaller)
  }
  if (temp<maxRate) temp=maxRate;                            // fastest rate 
  if (temp>TakeupRate) temp=TakeupRate;                      // slowest rate
  cli(); timerRateAxis1=temp; sei();

  // Now, for Declination
  if (distStartAxis2>distDestAxis2) {
    temp=(StepsForRateChangeAxis2/isqrt32(distDestAxis2));   // slow down
  } else {
    temp=(StepsForRateChangeAxis2/isqrt32(distStartAxis2));  // speed up
  }
  if (temp<maxRate) temp=maxRate;                            // fastest rate
  if (temp>TakeupRate) temp=TakeupRate;                      // slowest rate
  cli(); timerRateAxis2=temp; sei();

#ifdef MOUNT_TYPE_ALTAZM
  // in AltAz mode if the end of slew doesn't get close enough within 3 seconds: stop tracking for a moment to allow target/actual position synchronization
  static bool forceSlewStop=false;
  static unsigned long slewStopTime=0;
  if ( (!forceSlewStop) && (distDestAxis1<=GetStepsPerSecondAxis1()) && (distDestAxis2<=GetStepsPerSecondAxis2()) ) { slewStopTime=millis()+3000L; forceSlewStop=true; }
  if ( (lastTrackingState==TrackingSidereal) && (forceSlewStop && ((long)(millis()-slewStopTime)>0)) ) lastTrackingState=TrackingSiderealDisabled;
#endif

  if ((distDestAxis1<=2) && (distDestAxis2<=2)) {
    
#ifdef MOUNT_TYPE_ALTAZM
    // if we stopped tracking turn it back on now
    if (lastTrackingState==TrackingSiderealDisabled) lastTrackingState=TrackingSidereal;
    forceSlewStop=false;
#endif
    
    if ((pierSide==PierSideFlipEW2) || (pierSide==PierSideFlipWE2)) {
      // just wait stop here until we get notification to continue
      if (pauseHome) {
        if (!waitingHomeContinue) { waitingHome=true; return; }
        soundAlert(); waitingHome=false; waitingHomeContinue=false;
      }

      // make sure we're at the home position just before flipping sides of the mount
      startAxis1=posAxis1;
      startAxis2=posAxis2;
      cli();
      if (celestialPoleAxis1==0) {
        // for fork mounts
        if (pierSide==PierSideFlipEW2) targetAxis1.part.m=180L*(long)StepsPerDegreeAxis1-indexAxis1Steps; else targetAxis1.part.m=-180L*(long)StepsPerDegreeAxis1-indexAxis1Steps; targetAxis1.part.f=0;
      } else {
        // for eq mounts
        if (pierSide==PierSideFlipEW2) targetAxis1.part.m=(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; else targetAxis1.part.m=-(long)(celestialPoleAxis1*(double)StepsPerDegreeAxis1)-indexAxis1Steps; targetAxis1.part.f=0;
      }
      targetAxis2.part.m=(long)(celestialPoleAxis2*(double)StepsPerDegreeAxis2)-indexAxis2Steps; targetAxis2.part.f=0;
      sei();

      pierSide++;
    } else
    if ((pierSide==PierSideFlipEW3) || (pierSide==PierSideFlipWE3)) {
      
      // the blAxis2 gets "reversed" when we Meridian flip, since the NORTH/SOUTH movements are reversed
      cli(); blAxis2=backlashAxis2-blAxis2; sei();

      if (pierSide==PierSideFlipEW3) {
        pierSide=PierSideWest;
        cli();
        // reverse the Declination movement
        defaultDirAxis2  = defaultDirAxis2WInit;
        // if we were on the east side of the pier the HA's were in the western sky, and were positive
        // now we're in the eastern sky and the HA's are negative
        posAxis1-=(long)(180.0*(double)StepsPerDegreeAxis1);
        trueAxis1-=(long)(180.0*(double)StepsPerDegreeAxis1);
        sei();
        forceRefreshGetEqu();
      } else {
        pierSide=PierSideEast;
        cli();
        // normal Declination
        defaultDirAxis2  = defaultDirAxis2EInit;      
        // if we were on the west side of the pier the HA's were in the eastern sky, and were negative
        // now we're in the western sky and the HA's are positive
        posAxis1+=(long)(180.0*(double)StepsPerDegreeAxis1);
        trueAxis1+=(long)(180.0*(double)StepsPerDegreeAxis1);
        sei();
        forceRefreshGetEqu();
      }
    
      // now complete the slew
      cli();
      startAxis1=posAxis1;
      targetAxis1.fixed=origTargetAxis1.fixed;
      startAxis2=posAxis2;
      targetAxis2.part.m=origTargetAxis2; targetAxis2.part.f=0;
      sei();
    } else {

      StepperModeTracking();

      // other special gotos: for parking the mount and homing the mount
      if (parkStatus==Parking) {
        // clear the backlash
        int i=parkClearBacklash(); if (i==-1) return; // working

        // stop the motor timers (except guiding)
        cli(); trackingTimerRateAxis1=0.0; trackingTimerRateAxis2=0.0; sei(); delay(11);
        
        // restore trackingState
        trackingState=lastTrackingState; lastTrackingState=TrackingNone;
        SiderealClockSetInterval(siderealInterval);

        // validate location
        byte parkPierSide=nv.read(EE_pierSide);
        if ((blAxis1!=0) || (blAxis2!=0) || (posAxis1!=(long)targetAxis1.part.m) || (posAxis2!=(long)targetAxis2.part.m) || (pierSide!=parkPierSide) || (i!=1)) { parkStatus=ParkFailed; nv.write(EE_parkStatus,parkStatus); }

        // sound park done
        soundAlert();

        // wrap it up
        parkFinish();
      } else
        // sound goto done
        soundAlert();
  
        // restore last tracking state
        cli();
        timerRateAxis1=SiderealRate;
        timerRateAxis2=SiderealRate;
        sei();
  
        if (homeMount) {
          // clear the backlash
          if (parkClearBacklash()==-1) return;  // working, no error flagging

          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);

          setHome();
          homeMount=false; 
          atHome=true;

          DisableStepperDrivers();
        } else {
          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);
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

