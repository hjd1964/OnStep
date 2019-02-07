// -----------------------------------------------------------------------------------
// Functions to move the mount to the a new position

// moves the mount
void moveTo() {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, first phase.  only happens for GEM mounts
  if ((pierSideControl==PierSideFlipEW1) || (pierSideControl==PierSideFlipWE1)) {

    // save destination
    cli(); 
    origTargetAxis1.fixed = targetAxis1.fixed;
    origTargetAxis2 = (long)targetAxis2.part.m;
 
    timerRateAxis1=SiderealRate;
    timerRateAxis2=SiderealRate;
    sei();

    // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
    if (pierSideControl==PierSideFlipWE1) {
      if (celestialPoleAxis1==0.0) setTargetAxis1(0.0,PierSideWest); else {
        if ((currentAlt<10.0) && (getStartAxis1()>-90.0)) setTargetAxis1(-60.0,PierSideWest); else setTargetAxis1(-celestialPoleAxis1,PierSideWest);
      }
      setTargetAxis2(celestialPoleAxis2,PierSideWest);
    } else {
      if (celestialPoleAxis1==0.0) setTargetAxis1(0.0,PierSideEast); else {
        if ((currentAlt<10.0) && (getStartAxis1()<90.0)) setTargetAxis1(60.0,PierSideEast); else setTargetAxis1(celestialPoleAxis1,PierSideEast);
      }
      setTargetAxis2(celestialPoleAxis2,PierSideEast);
    }

    // first phase, override above for additional waypoints
    if (celestialPoleAxis2>0.0) {
      if (getInstrAxis2()>90.0-latitude) {
        // if Dec is in the general area of the pole, slew both axis back at once
        if (pierSideControl==PierSideFlipWE1) setTargetAxis1(-celestialPoleAxis1,PierSideWest); else setTargetAxis1(celestialPoleAxis1,PierSideEast);
      } else {
        // if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        // in the extreme case, where the user is very near the (Earths!) equator an Horizon limit of -10 or -15 may be necessary for proper operation.
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (getInstrAxis2()<0.0)) {
          if (pierSideControl==PierSideFlipWE1) setTargetAxis1(-45.0,PierSideWest); else setTargetAxis1(45.0,PierSideEast);
        }
      }
    } else {
      if (getInstrAxis2()<-90.0-latitude) {
        // if Dec is in the general area of the pole, slew both axis back at once
        if (pierSideControl==PierSideFlipWE1) setTargetAxis1(-celestialPoleAxis1,PierSideWest); else setTargetAxis1(celestialPoleAxis1,PierSideEast);
      } else { 
        // if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        if ((currentAlt<20.0) && (abs(latitude)<45.0) && (getInstrAxis2()>0.0)) {
          if (pierSideControl==PierSideFlipWE1) setTargetAxis1(-45.0,PierSideWest); else setTargetAxis1(45.0,PierSideEast);
        }
      }
    }

//    D("Flp1 Axis1, Current "); D(((double)(long)posAxis1)/(double)StepsPerDegreeAxis1); D(" -to-> "); DL(((double)(long)targetAxis1.part.m)/(double)StepsPerDegreeAxis1);
//    D("Flp1 Axis2, Current "); D(((double)(long)posAxis2)/(double)StepsPerDegreeAxis2); D(" -to-> "); DL(((double)(long)targetAxis2.part.m)/(double)StepsPerDegreeAxis2); DL("");
    pierSideControl++;
    forceRefreshGetEqu();
  }

  long distStartAxis1,distStartAxis2,distDestAxis1,distDestAxis2;

  cli();
  distStartAxis1=abs(posAxis1-startAxis1);  // distance from start Axis1
  distStartAxis2=abs(posAxis2-startAxis2);  // distance from start Axis2
  sei();
  if (distStartAxis1<1) distStartAxis1=1;
  if (distStartAxis2<1) distStartAxis2=1;

  cli();
  distDestAxis1=abs(posAxis1-(long)targetAxis1.part.m);  // distance from dest Axis1
  distDestAxis2=abs(posAxis2-(long)targetAxis2.part.m);  // distance from dest Axis2
  sei();
  
  // adjust rates near the horizon to help keep from exceeding the minAlt limit
  #ifndef MOUNT_TYPE_ALTAZM
  long a2=abs(getInstrAxis2()*StepsPerDegreeAxis2);
  static long lastPosAxis2=0;
  if (((latitude>10) || (latitude<-10)) && (distStartAxis1>((DegreesForAcceleration*StepsPerDegreeAxis1)/16))) {
    // if Dec is decreasing, slow down Dec
    if (a2<lastPosAxis2) {
      double minAlt2=minAlt+10.0;
      long a=(currentAlt-minAlt2)*StepsPerDegreeAxis2; if (a<((DegreesForAcceleration*StepsPerDegreeAxis2)/8)) a=((DegreesForAcceleration*StepsPerDegreeAxis2)/8);
      if (a<distDestAxis2) distDestAxis2=a;
    } else
    // if Dec is increasing, slow down HA
    {
      double minAlt2=minAlt+10.0;
      long a=(currentAlt-minAlt2)*StepsPerDegreeAxis1; if (a<((DegreesForAcceleration*StepsPerDegreeAxis1)/8)) a=((DegreesForAcceleration*StepsPerDegreeAxis1)/8);
      if (a<distDestAxis1) distDestAxis1=a;
    }
  }
  lastPosAxis2=a2;
  #endif

  if (distDestAxis1<1) distDestAxis1=1;
  if (distDestAxis2<1) distDestAxis2=1;

  // quickly slow the motors and stop in DegreesForRapidStop
  static double deaccXPerSec=0;
  static double a1r=0;
  static double a2r=0;
  if (abortSlew==1) {
    // aborts any meridian flip
    if ((pierSideControl==PierSideFlipWE1) || (pierSideControl==PierSideFlipWE2) || (pierSideControl==PierSideFlipWE3)) pierSideControl=PierSideWest;
    if ((pierSideControl==PierSideFlipEW1) || (pierSideControl==PierSideFlipEW2) || (pierSideControl==PierSideFlipEW3)) pierSideControl=PierSideEast;
    if (pauseHome) { waitingHome=false; waitingHomeContinue=false; }

    // stop parking/homing
    if (parkStatus==Parking) {
      lastTrackingState=abortTrackingState;
      parkStatus=NotParked;
      nv.write(EE_parkStatus,parkStatus);
    } else
    if (homeMount) {
      lastTrackingState=abortTrackingState;
      homeMount=false;
    }

    // set rate (in X sidereal) at which we slow down from
    double rateXPerSec = RateToXPerSec/(maxRate/16.0);
    D("rateXPerSec="); DL(rateXPerSec);
    double numSecToStop = (DegreesForRapidStop / (rateXPerSec/240.0));
    D("numSecToStop="); DL(numSecToStop);
    deaccXPerSec = (DegreesForRapidStop/numSecToStop)*240.0;
    D("deaccXPerSec="); DL(deaccXPerSec);

    abortSlew++;
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
  if (abortSlew!=0) {
    //                50x=                  5000/100
    if (abortSlew==2) { a1r=(double)SiderealRate/(double)temp; } else
    if (abortSlew==3) {
      double r=1.2-sqrt((abs(a1r)/slewRateX));
      if (r<0.2) r=0.2; if (r>1.2) r=1.2;
      a1r-=(deaccXPerSec/100.0)*r; if (a1r<2.0) a1r=2.0;
    }
    // 100=5000/50
    temp=round((double)SiderealRate/a1r);
  }
  cli(); timerRateAxis1=temp; sei();

  // Now, for Declination
  if (distStartAxis2>distDestAxis2) {
    temp=(StepsForRateChangeAxis2/isqrt32(distDestAxis2));   // slow down
  } else {
    temp=(StepsForRateChangeAxis2/isqrt32(distStartAxis2));  // speed up
  }
  if (temp<maxRate) temp=maxRate;                            // fastest rate
  if (temp>TakeupRate) temp=TakeupRate;                      // slowest rate
  if (abortSlew!=0) {
    if (abortSlew==2) { a2r=(double)SiderealRate/(double)temp; abortSlew++; } else
    if (abortSlew==3) { 
      double r=1.2-sqrt((abs(a2r)/slewRateX));
      if (r<0.2) r=0.2; if (r>1.2) r=1.2;
      a2r-=(deaccXPerSec/100.0)*r;
      if (a2r<2.00) a2r=2.0;
      if ((a1r<2.00001) && (a2r<2.00001)) abortSlew++;
    } else
    if (abortSlew==4) { abortSlew=0; cli(); targetAxis1.part.m=posAxis1; targetAxis2.part.m=posAxis2; sei(); distDestAxis1=0; distDestAxis2=0; }
    temp=round((double)SiderealRate/a2r);
  }
  cli(); timerRateAxis2=temp; sei();

  // make sure we're using the tracking mode microstep setting near the end of slew
  if ((distDestAxis1<=getStepsPerSecondAxis1()) && (distDestAxis2<=getStepsPerSecondAxis2()) ) stepperModeTracking();

  // the end of slew doesn't get close enough within 6 seconds: stop tracking for a moment to allow target/actual position synchronization
  static unsigned long slewStopTime=0;
  static bool slewEnding=false;
  static bool slewAbort=false;
  if ( !slewEnding && (distDestAxis1<=getStepsPerSecondAxis1()*2L) && (distDestAxis2<=getStepsPerSecondAxis2()*2L) ) { slewStopTime=millis()+6000L; slewEnding=true; }
  if ( slewEnding && (long)(millis()-slewStopTime)>0) { lastError=ERR_GOTO_SYNC; slewAbort=true; }

  if ( ((distDestAxis1<=ceil(abs(fixedToDouble(fstepAxis1)))+1) && (distDestAxis2<=ceil(abs(fixedToDouble(fstepAxis2)))+1) ) || slewAbort) {
    slewEnding=false;
    slewAbort=false;

    // assurance that we're really in tracking mode
    stepperModeTracking();

    if ((pierSideControl==PierSideFlipEW2) || (pierSideControl==PierSideFlipWE2)) {
      // just wait stop here until we get notification to continue
      if (pauseHome) {
        if (!waitingHomeContinue) { waitingHome=true; return; }
        soundAlert(); waitingHome=false; waitingHomeContinue=false;
      }

      // make sure we're at the home position just before flipping sides of the mount
      cli();
      startAxis1=posAxis1;
      startAxis2=posAxis2;
      sei();
      if (celestialPoleAxis1==0.0) {
        // for fork mounts
        if (pierSideControl==PierSideFlipEW2) setTargetAxis1(180.0,PierSideEast); else setTargetAxis1(-180.0,PierSideWest);
      } else {
        // for eq mounts
        if (pierSideControl==PierSideFlipEW2) setTargetAxis1(celestialPoleAxis1,PierSideEast); else setTargetAxis1(-celestialPoleAxis1,PierSideWest);
      }
      pierSideControl++;

      stepperModeGoto();
      forceRefreshGetEqu();
    } else
    if ((pierSideControl==PierSideFlipEW3) || (pierSideControl==PierSideFlipWE3)) {

      if (pierSideControl==PierSideFlipEW3) pierSideControl=PierSideWest; else pierSideControl=PierSideEast;
    
      // now complete the slew
      cli();
      startAxis1=posAxis1;
      targetAxis1.fixed=origTargetAxis1.fixed;
      startAxis2=posAxis2;
      targetAxis2.part.m=origTargetAxis2; targetAxis2.part.f=0;
      sei();

      stepperModeGoto();
      forceRefreshGetEqu();
    } else {

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
        if ((blAxis1!=0) || (blAxis2!=0) || (posAxis1!=(long)targetAxis1.part.m) || (posAxis2!=(long)targetAxis2.part.m) || (pierSideControl!=parkPierSide) || (i!=1)) { parkStatus=ParkFailed; nv.write(EE_parkStatus,parkStatus); }

        // sound park done
        soundAlert();

        // wrap it up
        parkFinish();
      } else {
        // restore last tracking state
        cli();
        timerRateAxis1=SiderealRate;
        timerRateAxis2=SiderealRate;
        sei();
  
        if (homeMount) {
          // sound goto done
          soundAlert();
          
          // clear the backlash
          if (parkClearBacklash()==-1) return;  // working, no error flagging

          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);

          setHome();
          homeMount=false; 
          atHome=true;

          disableStepperDrivers();
        } else {
          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);
          setDeltaTrackingRate();
          
          // allow 10 seconds for synchronization of coordinates after goto ends
          if (trackingState==TrackingSidereal) trackingSyncSeconds=15;
        }
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

void stopLimit() {
  if (trackingState==TrackingMoveTo) { if (!abortSlew) abortSlew=StartAbortSlew; } else trackingState=TrackingNone;
}
