// -----------------------------------------------------------------------------------
// Functions to move the mount to the a new position

// moves the mount
void moveTo() {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, first phase.  only happens for GEM mounts
  if ((pierSideControl == PierSideFlipEW1) || (pierSideControl == PierSideFlipWE1)) {

    // save destination
    cli(); 
    origTargetAxis1.fixed = targetAxis1.fixed;
    origTargetAxis2.fixed = targetAxis2.fixed;
 
    timerRateAxis1=SiderealRate;
    timerRateAxis2=SiderealRate;
    sei();

    // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
    if (pierSideControl == PierSideFlipWE1) {
      if (homePositionAxis1 == 0.0) setTargetAxis1(0.0,PierSideWest); else {
        if ((currentAlt < 10.0) && (getStartAxis1() > -90.0)) setTargetAxis1(-60.0,PierSideWest); else setTargetAxis1(-homePositionAxis1,PierSideWest);
      }
      setTargetAxis2(homePositionAxis2,PierSideWest);
    } else {
      if (homePositionAxis1 == 0.0) setTargetAxis1(0.0,PierSideEast); else {
        if ((currentAlt < 10.0) && (getStartAxis1() < 90.0)) setTargetAxis1(60.0,PierSideEast); else setTargetAxis1(homePositionAxis1,PierSideEast);
      }
      setTargetAxis2(homePositionAxis2,PierSideEast);
    }

    // first phase, override above for additional waypoints
    if (homePositionAxis2 > 0.0) {
      if (getInstrAxis2() > 90.0-latitude) {
        // if Dec is in the general area of the pole, slew both axis back at once
        if (pierSideControl == PierSideFlipWE1) setTargetAxis1(-homePositionAxis1,PierSideWest); else setTargetAxis1(homePositionAxis1,PierSideEast);
      } else {
        // if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        // in the extreme case, where the user is very near the (Earths!) equator an Horizon limit of -10 or -15 may be necessary for proper operation.
        if (currentAlt < 20.0 && latitudeAbs < 45.0 && getInstrAxis2() < 0.0) {
          if (pierSideControl == PierSideFlipWE1) setTargetAxis1(-45.0,PierSideWest); else setTargetAxis1(45.0,PierSideEast);
        }
      }
    } else {
      if (getInstrAxis2() < -90.0-latitude) {
        // if Dec is in the general area of the pole, slew both axis back at once
        if (pierSideControl == PierSideFlipWE1) setTargetAxis1(-homePositionAxis1,PierSideWest); else setTargetAxis1(homePositionAxis1,PierSideEast);
      } else { 
        // if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
        if (currentAlt < 20.0 && latitudeAbs < 45.0 && getInstrAxis2() > 0.0) {
          if (pierSideControl == PierSideFlipWE1) setTargetAxis1(-45.0,PierSideWest); else setTargetAxis1(45.0,PierSideEast);
        }
      }
    }

    pierSideControl++;
    forceRefreshGetEqu();
  }

  long distStartAxis1,distStartAxis2,distDestAxis1,distDestAxis2;

  cli();
  distStartAxis1=labs(posAxis1-startAxis1);  // distance from start Axis1
  distStartAxis2=labs(posAxis2-startAxis2);  // distance from start Axis2
  sei();
  if (distStartAxis1 < 1) distStartAxis1=1;
  if (distStartAxis2 < 1) distStartAxis2=1;

  cli();
  distDestAxis1=labs(posAxis1-(long)targetAxis1.part.m);  // distance from dest Axis1
  distDestAxis2=labs(posAxis2-(long)targetAxis2.part.m);  // distance from dest Axis2
  sei();
  
  // adjust rates near the horizon to help keep from exceeding the minAlt limit
  #if MOUNT_TYPE != ALTAZM
    if (latitudeAbs > 10) {
      long posAxis2=latitudeSign*getInstrAxis2()*axis2Settings.stepsPerDegree;
      static long lastPosAxis2=0;
      double distDestLimit=currentAlt-(minAlt+10.0); if (distDestLimit < SLEW_ACCELERATION_DIST/8.0) distDestLimit=SLEW_ACCELERATION_DIST/8.0;  
      // if Dec is decreasing slow down the Dec axis, if Dec is increasing slow down the RA axis
      if (posAxis2 < lastPosAxis2) {
        if (distDestLimit*axis2Settings.stepsPerDegree < distDestAxis2) distDestAxis2=distDestLimit*axis2Settings.stepsPerDegree;
      } else {
        if (distDestLimit*axis1Settings.stepsPerDegree < distDestAxis1) distDestAxis1=distDestLimit*axis1Settings.stepsPerDegree;
      }
      lastPosAxis2=posAxis2;
    }
  #endif

  if (distDestAxis1 < 1) distDestAxis1=1;
  if (distDestAxis2 < 1) distDestAxis2=1;

  // quickly slow the motors and stop in SLEW_RAPID_STOP_DIST
  static double deaccXPerSec=0;
  static double a1r=0;
  static double a2r=0;
  if (abortSlew == 1) {
    // aborts any meridian flip
    if ((pierSideControl == PierSideFlipWE1) || (pierSideControl == PierSideFlipWE2) || (pierSideControl == PierSideFlipWE3)) pierSideControl=PierSideWest;
    if ((pierSideControl == PierSideFlipEW1) || (pierSideControl == PierSideFlipEW2) || (pierSideControl == PierSideFlipEW3)) pierSideControl=PierSideEast;
    if (pauseHome) { waitingHome=false; waitingHomeContinue=false; }

    // stop parking/homing
    if (parkStatus == Parking) {
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
    double numSecToStop = (SLEW_RAPID_STOP_DIST/(rateXPerSec/240.0));
    deaccXPerSec = (SLEW_RAPID_STOP_DIST/numSecToStop)*240.0;

    abortSlew++;
  }

  // First, for Right Ascension
  long temp;
  if (distStartAxis1 > distDestAxis1) {
    temp=(StepsForRateChangeAxis1/isqrt32(distDestAxis1));   // slow down (temp gets bigger)
  } else {
    temp=(StepsForRateChangeAxis1/isqrt32(distStartAxis1));  // speed up (temp gets smaller)
  }
  if (temp < maxRate) temp=maxRate;                          // fastest rate 
  if (temp > TakeupRate) temp=TakeupRate;                    // slowest rate
  if (abortSlew != 0) {
    if (abortSlew == 2) { a1r=(double)SiderealRate/(double)temp; } else
    if (abortSlew == 3) {
      double r=1.2-sqrt((fabs(a1r)/slewRateX));
      if (r < 0.2) r=0.2; if (r > 1.2) r=1.2;
      a1r-=(deaccXPerSec/100.0)*r; if (a1r < 2.0) a1r=2.0;
    }
    temp=round((double)SiderealRate/a1r);
  }
  cli(); timerRateAxis1=temp; sei();

  // Now, for Declination
  if (distStartAxis2 > distDestAxis2) {
    temp=(StepsForRateChangeAxis2/isqrt32(distDestAxis2));   // slow down
  } else {
    temp=(StepsForRateChangeAxis2/isqrt32(distStartAxis2));  // speed up
  }
  if (temp < maxRate) temp=maxRate;                          // fastest rate
  if (temp > TakeupRate) temp=TakeupRate;                    // slowest rate
  if (abortSlew != 0) {
    if (abortSlew == 2) { a2r=(double)SiderealRate/(double)temp; abortSlew++; } else
    if (abortSlew == 3) { 
      double r=1.2-sqrt((fabs(a2r)/slewRateX));
      if (r < 0.2) r=0.2; if (r > 1.2) r=1.2;
      a2r-=(deaccXPerSec/100.0)*r;
      if (a2r < 2.00) a2r=2.0;
      if ((a1r < 2.00001) && (a2r < 2.00001)) abortSlew++;
    } else
    if (abortSlew == 4) { abortSlew=0; cli(); targetAxis1.part.m=posAxis1; targetAxis2.part.m=posAxis2; sei(); distDestAxis1=0; distDestAxis2=0; }
    temp=round((double)SiderealRate/a2r);
  }
  cli(); timerRateAxis2=temp; sei();

  // make sure we're using the tracking mode microstep setting near the end of slew
  if (distDestAxis1 <= getStepsPerSecondAxis1()) axis1DriverTrackingMode(false);
  if (distDestAxis2 <= getStepsPerSecondAxis2()) axis2DriverTrackingMode(false);

  // the end of slew doesn't get close enough within 4 seconds force the slew to end
  static unsigned long slewStopTime=0;
  static bool slewEnding=false;
  static bool slewForceEnd=false;
  if (!slewEnding && (distDestAxis1 <= getStepsPerSecondAxis1()*4.0) && (distDestAxis2 <= getStepsPerSecondAxis2()*4.0) ) { slewStopTime=millis()+4000L; slewEnding=true; }
  if (slewEnding && ((long)(millis()-slewStopTime) > 0)) {
    if (abortSlew != 0) { cli(); targetAxis1.part.m=posAxis1; targetAxis2.part.m=posAxis2; sei(); }
    generalError=ERR_GOTO_SYNC;
    slewForceEnd=true;
  }

  if ( ((distDestAxis1 <= ceil(fabs(fixedToDouble(fstepAxis1)))+4) && (distDestAxis2 <= ceil(fabs(fixedToDouble(fstepAxis2)))+4) ) || slewForceEnd ) {
    slewEnding=false;
    slewForceEnd=false;
    abortSlew=0;

    // assurance that we're really in tracking mode
    axis1DriverTrackingMode(false);
    axis2DriverTrackingMode(false);

    if ((pierSideControl == PierSideFlipEW2) || (pierSideControl == PierSideFlipWE2)) {
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
      if (homePositionAxis1 == 0.0) {
        // for fork mounts
        if (pierSideControl == PierSideFlipEW2) setTargetAxis1(180.0,PierSideEast); else setTargetAxis1(-180.0,PierSideWest);
      } else {
        // for eq mounts
        if (pierSideControl == PierSideFlipEW2) setTargetAxis1(homePositionAxis1,PierSideEast); else setTargetAxis1(-homePositionAxis1,PierSideWest);
      }
      pierSideControl++;

      axis1DriverGotoMode();
      axis2DriverGotoMode();
      
      forceRefreshGetEqu();
    } else
    if ((pierSideControl == PierSideFlipEW3) || (pierSideControl == PierSideFlipWE3)) {

      if (pierSideControl == PierSideFlipEW3) pierSideControl=PierSideWest; else pierSideControl=PierSideEast;
    
      // now complete the slew
      cli();
      startAxis1=posAxis1;
      targetAxis1.fixed=origTargetAxis1.fixed;
      startAxis2=posAxis2;
      targetAxis2.fixed=origTargetAxis2.fixed;
      sei();

      axis1DriverGotoMode();
      axis2DriverGotoMode();

      forceRefreshGetEqu();
    } else {

      // other special gotos: for parking the mount and homing the mount
      if (parkStatus == Parking) {
        // clear the backlash
        int pcbStatus=parkClearBacklash();
        if (pcbStatus == PCB_BUSY) return;

        // stop the motor timers (except guiding)
        cli(); trackingTimerRateAxis1=0.0; trackingTimerRateAxis2=0.0; sei(); delay(11);
        
        // restore trackingState
        trackingState=lastTrackingState; lastTrackingState=TrackingNone;
        SiderealClockSetInterval(siderealInterval);

        // validate location
        byte parkPierSide=nv.read(EE_pierSide);
        if (pierSideControl != parkPierSide || pcbStatus != PCB_SUCCESS) { parkStatus=ParkFailed; nv.write(EE_parkStatus,parkStatus); }

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
          // clear the backlash
          if (parkClearBacklash() == -1) return;  // working, no error flagging

          // sound goto done
          soundAlert();

          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);

          // at the polar home position
          homeMount=false;
          if (AXIS2_TANGENT_ARM == OFF) atHome=true;
          VLF("MSG: Homing done");
        } else {
          // restore trackingState
          trackingState=lastTrackingState; lastTrackingState=TrackingNone;
          SiderealClockSetInterval(siderealInterval);
          setDeltaTrackingRate();
          VLF("MSG: Goto done");
          
          // allow 5 seconds for synchronization of coordinates after goto ends
          if (trackingState == TrackingSidereal) {
            trackingSyncSeconds=5;
            VLF("MSG: Tracking sync started");
          }
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

// check for platform rate limit (lowest maxRate) in 1/16us units
long maxRateLowerLimit() {
  double r_us=HAL_MAXRATE_LOWER_LIMIT;  // for example 16us, this basis rate has platform (STM32/Teensy3.2/3.5/3.6/Mega2560), clock rate, and ISR operating mode (Sqw/Pulse/Dedge) factored in (from HAL.)
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us=r_us/1.6; // about 1.6x faster than SQW mode in my testing
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    if (AXIS1_DRIVER_STEP_GOTO != 1 || AXIS2_DRIVER_STEP_GOTO != 1) r_us=r_us*1.7;  // if this code is enabled, 27us
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied, if tracking in 32X mode using 4X for gotos (32/4 = 8,) that's an 8x lower true rate so 27/8 = 3.4 is allowed
  double r_us_axis1=r_us/AXIS1_DRIVER_STEP_GOTO;
  double r_us_axis2=r_us/AXIS2_DRIVER_STEP_GOTO;
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  r_us=(r_us_axis1+r_us_axis2/timerRateRatio)/2.0;  // if Axis1 is 10000 step/deg & Axis2 is 20000 steps/deg, Axis2 needs to run 2x speed so we must slow down.  3.4 on one axis and 6.8 on the other for an average of 5.1

  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25) { r_us=0.25; DLF("WRN, maxRateLowerLimit(): r_us exceeds design limit"); }

  // return rate in 1/16us units
  return round(r_us*16.0);
}
