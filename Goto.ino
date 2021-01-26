//--------------------------------------------------------------------------------------------------
// Goto, commands to move the telescope to an location or to report the current location

// check if goto/sync is valid
CommandErrors validateGoto() {
  // Check state
  if (parkStatus != NotParked)                 return CE_SLEW_ERR_IN_PARK;
  if (!axis1Enabled)                           return CE_SLEW_ERR_IN_STANDBY;
  if (trackingSyncInProgress())                return CE_MOUNT_IN_MOTION;
  if (trackingState == TrackingMoveTo)         return CE_GOTO_ERR_GOTO;
  if (guideDirAxis1 || guideDirAxis2)          return CE_MOUNT_IN_MOTION;
  if (faultAxis1 || faultAxis2)                return CE_SLEW_ERR_HARDWARE_FAULT;
  return CE_NONE;
}

CommandErrors validateGotoCoords(double HA, double Dec, double Alt) {
  // Check coordinates
  if (Alt < minAlt)                            return CE_GOTO_ERR_BELOW_HORIZON;
  if (Alt > maxAlt)                            return CE_GOTO_ERR_ABOVE_OVERHEAD;
#if AXIS2_TANGENT_ARM == OFF && MOUNT_TYPE != ALTAZM
    if (Dec < axis2Settings.min)               return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (Dec > axis2Settings.max)               return CE_SLEW_ERR_OUTSIDE_LIMITS;
#endif
  if (HA < axis1Settings.min)                  return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (HA > axis1Settings.max)                  return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

CommandErrors validateGoToEqu(double RA, double Dec) {
  double a,z;
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);
  CommandErrors r=validateGoto(); if (r != CE_NONE) return r;
  r=validateGotoCoords(HA,Dec,a);
  return r;
}

// syncs the telescope/mount to the sky
CommandErrors syncEqu(double RA, double Dec) {
  double a,z;

  // Convert RA into hour angle, get altitude
  // hour angleTrackingMoveTo
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  CommandErrors e=validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { trackingState=TrackingSidereal; enableStepperDrivers(); e=validateGoto(); }
  if (e != CE_NONE) return e;
  e=validateGotoCoords(HA,Dec,a);
  if (e != CE_NONE) return e;

  double Axis1,Axis2;
#if MOUNT_TYPE == ALTAZM
  equToHor(HA,Dec,&Axis2,&Axis1);
  Align.horToInstr(Axis2,Axis1,&Axis2,&Axis1,getInstrPierSide());
  Axis1=haRange(Axis1);
#else
  Align.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());
#endif

  // west side of pier - we're in the eastern sky and the HA's are negative
  // east side of pier - we're in the western sky and the HA's are positive
  int newPierSide=getInstrPierSide();
  if (meridianFlip != MeridianFlipNever) {
    if (atHome) { if (Axis1 < 0) newPierSide=PierSideWest; else newPierSide=PierSideEast; } else // best side of pier decided based on meridian
#if PIER_SIDE_SYNC_CHANGE_SIDES == ON
    if (preferredPierSide == WEST) { newPierSide=PierSideWest; if (haRange(Axis1) > degreesPastMeridianW) newPierSide=PierSideEast; } else
    if (preferredPierSide == EAST) { newPierSide=PierSideEast; if (haRange(Axis1) < -degreesPastMeridianE) newPierSide=PierSideWest; } else
#endif
    {
      if ((getInstrPierSide() == PierSideWest) && (haRange(Axis1) > degreesPastMeridianW)) newPierSide=PierSideEast;
      if ((getInstrPierSide() == PierSideEast) && (haRange(Axis1) < -degreesPastMeridianE)) newPierSide=PierSideWest;
    }

#if PIER_SIDE_SYNC_CHANGE_SIDES == OFF
    if ((!atHome) && (newPierSide != getInstrPierSide())) return CE_SLEW_ERR_OUTSIDE_LIMITS;
#endif

  } else {
    // always on the "east" side of pier - we're in the western sky and the HA's are positive
    // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
    newPierSide=PierSideEast;
  }

  setIndexAxis1(Axis1,newPierSide);
  setIndexAxis2(Axis2,newPierSide);
  safetyLimitsOn=true;
  syncToEncodersOnly=true;

  VLF("MSG: Sync, indices set");

  return CE_NONE;
}

// syncs internal counts to shaft encoder position (in degrees)
CommandErrors syncEnc(double EncAxis1, double EncAxis2) {
  // validate
  CommandErrors f=validateGoto(); if (f != CE_NONE) return f;

  // no sync from encoders during an alignment!
  if (alignActive()) return CE_NONE;

  // force syncing to encoders only
  if (syncToEncodersOnly) return CE_NONE;

  // don't allow syncing outside of our normal operating range
  if (EncAxis1 < -360.0 || EncAxis2 < -360.0 || EncAxis1 > 360.0 || EncAxis2 > 360.0) {
    DLF("MSG: Sync from Encoders ignored, Axis");
    if (EncAxis1 < -360.0) DLF("1 < -360.0"); else
    if (EncAxis2 < -360.0) DLF("2 < -360.0"); else
    if (EncAxis1 > 360.0) DLF("1 > 360.0"); else
    if (EncAxis2 > 360.0) DLF("2 > 360.0");
    return CE_PARAM_RANGE;
  }

  long e1=EncAxis1*axis1Settings.stepsPerMeasure;
  long e2=EncAxis2*axis2Settings.stepsPerMeasure;

  long a1,a2;
  cli();
  a1=posAxis1;
  a2=posAxis2;
  sei();
  a1+=indexAxis1Steps;
  a2+=indexAxis2Steps;

  long delta1=a1-e1;
  long delta2=a2-e2;

  indexAxis1Steps-=delta1;
  indexAxis1=(double)indexAxis1Steps/axis1Settings.stepsPerMeasure;
  indexAxis2Steps-=delta2;
  indexAxis2=(double)indexAxis2Steps/axis2Settings.stepsPerMeasure;

  VLF("MSG: Sync from Encoders, indices set");

  return CE_NONE;
}

// get internal counts as shaft encoder position (in degrees)
void getEnc(double *EncAxis1, double *EncAxis2) {
  long a1,a2;
  cli();
  a1=posAxis1;
  a2=posAxis2;
  sei();
  a1+=indexAxis1Steps;
  a2+=indexAxis2Steps;
  
  *EncAxis1=(double)a1/axis1Settings.stepsPerMeasure;
  *EncAxis2=(double)a2/axis2Settings.stepsPerMeasure;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
bool getEqu(double *RA, double *Dec, bool returnHA) {
  double HA;
 
#if MOUNT_TYPE != ALTAZM
  HA=getInstrAxis1();
  *Dec=getInstrAxis2();
  // apply pointing model
  Align.instrToEqu(HA,*Dec,&HA,Dec,getInstrPierSide());
#else
  double Z=getInstrAxis1();
  double A=getInstrAxis2();
  // apply pointing model
  Align.instrToHor(A,Z,&A,&Z,getInstrPierSide());
  horToEqu(A,Z,&HA,Dec);
#endif

  // return either the RA or the HA depending on returnHA
  if (!returnHA) {
    *RA=(LST()*15.0-HA);
    while (*RA >= 360.0) *RA-=360.0;
    while (*RA < 0.0) *RA+=360.0;
  } else *RA=HA;
  
  return true;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
bool getApproxEqu(double *RA, double *Dec, bool returnHA) {
  double HA;
  
#if MOUNT_TYPE != ALTAZM
  HA=getInstrAxis1();
  *Dec=getInstrAxis2();
#else
  double Z=getInstrAxis1();
  double A=getInstrAxis2();
  horToEqu(A,Z,&HA,Dec);
#endif

  HA=haRange(HA);
  if (*Dec > 90.0) *Dec=+90.0;
  if (*Dec < -90.0) *Dec=-90.0;
  
  // return either the RA or the HA depending on returnHA
  if (!returnHA) {
    *RA=degRange(LST()*15.0-HA);
  } else *RA=HA;
  return true;
}

// gets the telescopes current Alt and Azm
bool getHor(double *Alt, double *Azm) {
  double h,d;
  getEqu(&h,&d,true);
  equToHor(h,d,Alt,Azm);
  return true;
}

// causes a goto to the same RA/Dec on the opposite pier side if possible
CommandErrors goToHere(bool toEastOnly) {
  bool verified=false;
  int p=preferredPierSide;
  if (meridianFlip == MeridianFlipNever) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  cli(); long h=posAxis1+indexAxis1Steps; sei();
  if ((!toEastOnly) && (getInstrPierSide() == PierSideEast) && (h < (degreesPastMeridianW*(long)axis1Settings.stepsPerMeasure))) { verified=true; preferredPierSide=WEST; }
  if ((getInstrPierSide() == PierSideWest) && (h > (-degreesPastMeridianE*(long)axis1Settings.stepsPerMeasure))) { verified=true; preferredPierSide=EAST; }
  if (verified) {
    double newRA,newDec;
    getEqu(&newRA,&newDec,false);
    CommandErrors e=goToEqu(newRA,newDec);
    preferredPierSide=p;
    return e;
  } else return CE_SLEW_ERR_OUTSIDE_LIMITS;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
CommandErrors goToEqu(double RA, double Dec) {
  double a,z;
  double Axis1,Axis2;
  double Axis1Alt,Axis2Alt;

  // Convert RA into hour angle, get altitude
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  CommandErrors e=validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome && timeWasSet && dateWasSet) { trackingState=TrackingSidereal; enableStepperDrivers(); e=validateGoto(); }
#ifndef CE_GOTO_ERR_GOTO_OFF
  if (e == CE_GOTO_ERR_GOTO) { if (!abortGoto) abortGoto=StartAbortGoto; } 
#endif
  if (e != CE_NONE) return e;
  e=validateGotoCoords(HA,Dec,a);
  if (e != CE_NONE) return e;

#if MOUNT_TYPE == ALTAZM
  // allow infinite coordinate wrap for Axis1 Azm by forcing instrument coordinates back within the +/- 180 degree range
  if (axis1Settings.min == -360 && axis1Settings.max == 360) nullIndexAxis1();

  equToHor(HA,Dec,&a,&z);
  Align.horToInstr(a,z,&a,&z,getInstrPierSide());
  z=haRange(z);

  // adjust coordinate range to allow going past 180 deg.
  // position a1 is 0..180
  double a1=getInstrAxis1();
  if (a1 >= 0) {
    // and goto z is in -0..-180
    if (z < 0) {
      // the alternate z1 is in 180..360
      double z1=z+360.0;
      if ((z1 < axis1Settings.max) && (dist(a1,z) > dist(a1,z1))) z=z1;
    }
  }
  // position a1 -0..-180
  if (a1 < 0) { 
    // and goto z is in 0..180
    if (z > 0) {
      // the alternate z1 is in -360..-180
      double z1=z-360.0;
      if ((z1 > axis1Settings.min) && (dist(a1,z) > dist(a1,z1))) z=z1;
    }
  }
  
  Axis1=z;
  Axis2=a;
  Axis1Alt=z;
  Axis2Alt=a;
#else
  // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
  Align.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());

  // as above... for the opposite pier side just incase we need to do a meridian flip
  int p=PierSideNone; if (getInstrPierSide() == PierSideEast) p=PierSideWest; else if (getInstrPierSide() == PierSideWest) p=PierSideEast;
  Align.equToInstr(HA,Dec,&Axis1Alt,&Axis2Alt,p);
#endif

  // goto function takes HA and Dec in steps
  byte thisPierSide = PierSideBest;
  if (meridianFlip != MeridianFlipNever) {
    if (preferredPierSide == WEST) thisPierSide=PierSideWest;
    if (preferredPierSide == EAST) thisPierSide=PierSideEast;
  }

  return goTo(Axis1,Axis2,Axis1Alt,Axis2Alt,thisPierSide);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
CommandErrors goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  horToEqu(*Alt,*Azm,&HA,&Dec);
  double RA=degRange(LST()*15.0-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination, both in degrees.  Alternate targets are used when a meridian flip occurs
CommandErrors goTo(double thisTargetAxis1, double thisTargetAxis2, double altTargetAxis1, double altTargetAxis2, int gotoPierSide) {
  atHome=false;
  int thisPierSide=getInstrPierSide();
  if (meridianFlip != MeridianFlipNever) {
    // where the allowable hour angles are
    double eastOfPierMaxHA= axis1Settings.max;
    double eastOfPierMinHA=-degreesPastMeridianE;
    double westOfPierMaxHA= degreesPastMeridianW;
    double westOfPierMinHA= axis1Settings.min;

    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((gotoPierSide != PierSideBest) && (thisPierSide != gotoPierSide)) {
      if (thisPierSide == PierSideEast) eastOfPierMinHA= degreesPastMeridianW;
      if (thisPierSide == PierSideWest) westOfPierMaxHA=-degreesPastMeridianE;
    }
    
    // if doing a meridian flip, use the opposite pier side coordinates
    if (thisPierSide == PierSideEast) {
      if ((thisTargetAxis1 > eastOfPierMaxHA) || (thisTargetAxis1 < eastOfPierMinHA)) {
        thisPierSide=PierSideFlipEW1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 > westOfPierMaxHA) return CE_SLEW_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide == PierSideWest) {
      if ((thisTargetAxis1 > westOfPierMaxHA) || (thisTargetAxis1 < westOfPierMinHA)) {
        thisPierSide=PierSideFlipWE1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 < eastOfPierMinHA) return CE_SLEW_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide == PierSideNone) {
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetAxis1 < 0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        thisPierSide=PierSideWest;
      } else {
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        thisPierSide=PierSideEast;
      }
    }
  } else {
    if (getInstrPierSide() == PierSideNone) {
        // always on the "east" side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
        thisPierSide=PierSideEast;
    }
  }
  
  // final validation
  int p=PierSideEast; switch (thisPierSide) { case PierSideWest: case PierSideFlipEW1: p=PierSideWest; break; }
#if MOUNT_TYPE == ALTAZM
  // allow +/- 360 in Az
  if (((thisTargetAxis1 > axis1Settings.max) || (thisTargetAxis1 < axis1Settings.min)) || ((thisTargetAxis2 > 180.0) || (thisTargetAxis2 < -180.0))) return CE_GOTO_ERR_UNSPECIFIED;
#else
  if (((thisTargetAxis1 > 270.0) || (thisTargetAxis1 < -270.0)) || ((thisTargetAxis2 > 270.0) || (thisTargetAxis2 < -270.0))) return CE_GOTO_ERR_UNSPECIFIED;
  #if AXIS2_TANGENT_ARM == ON
    if (toInstrAxis2(thisTargetAxis2,p) < axis2Settings.min) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (toInstrAxis2(thisTargetAxis2,p) > axis2Settings.max) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  #endif
#endif
  lastTrackingState=trackingState;

  cli();
  trackingState=TrackingMoveTo;
  SiderealClockSetInterval(siderealInterval);

  startAxis1=posAxis1;
  startAxis2=posAxis2;

  timerRateAxis1=siderealRate;
  timerRateAxis2=siderealRate;
  sei();

  setTargetAxis1(thisTargetAxis1,p);
  setTargetAxis2(thisTargetAxis2,p);

  if (!pauseHome && MFLIP_SKIP_HOME == ON) {
    if (thisPierSide == PierSideFlipWE1) pierSideControl=PierSideEast; else
    if (thisPierSide == PierSideFlipEW1) pierSideControl=PierSideWest; else pierSideControl=thisPierSide;
  } else pierSideControl=thisPierSide;

  reactivateBacklashComp();
#if AXIS1_PEC == ON
  disablePec();
#endif
  soundAlert();

  axis1DriverGotoMode();
  axis2DriverGotoMode();

  VLF("MSG: Goto started");
  return CE_NONE;
}
