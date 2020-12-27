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
  if (AXIS2_TANGENT_ARM == OFF && mountType != ALTAZM) {
    if (Dec < axis2Settings.min)               return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (Dec > axis2Settings.max)               return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
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
  if (mountType == ALTAZM) {
    equToHor(HA,Dec,&Axis2,&Axis1);
    AlignH.horToInstr(Axis2,Axis1,&Axis2,&Axis1,getInstrPierSide());
    Axis1=haRange(Axis1);
  } else {
    AlignE.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());
  }

  // west side of pier - we're in the eastern sky and the HA's are negative
  // east side of pier - we're in the western sky and the HA's are positive
  int newPierSide=getInstrPierSide();
  if (meridianFlip != MeridianFlipNever) {
    if (atHome) { if (Axis1 < 0) newPierSide=PIER_SIDE_WEST; else newPierSide=PIER_SIDE_EAST; } else // best side of pier decided based on meridian
#if PIER_SIDE_SYNC_CHANGE_SIDES == ON
    if (preferredPierSideDefault == WEST) { newPierSide=PIER_SIDE_WEST; if (haRange(Axis1) > degreesPastMeridianW) newPierSide=PIER_SIDE_EAST; } else
    if (preferredPierSideDefault == EAST) { newPierSide=PIER_SIDE_EAST; if (haRange(Axis1) < -degreesPastMeridianE) newPierSide=PIER_SIDE_WEST; } else
#endif
    {
      if ((getInstrPierSide() == PIER_SIDE_WEST) && (haRange(Axis1) > degreesPastMeridianW)) newPierSide=PIER_SIDE_EAST;
      if ((getInstrPierSide() == PIER_SIDE_EAST) && (haRange(Axis1) < -degreesPastMeridianE)) newPierSide=PIER_SIDE_WEST;
    }

#if PIER_SIDE_SYNC_CHANGE_SIDES == OFF
    if ((!atHome) && (newPierSide != getInstrPierSide())) return CE_SLEW_ERR_OUTSIDE_LIMITS;
#endif

  } else {
    // always on the "east" side of pier - we're in the western sky and the HA's are positive
    // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
    newPierSide=PIER_SIDE_EAST;
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
 
  if (mountType != ALTAZM) {
    HA=getInstrAxis1();
    *Dec=getInstrAxis2();
    // apply pointing model
    AlignE.instrToEqu(HA,*Dec,&HA,Dec,getInstrPierSide());
  } else {
    double Z=getInstrAxis1();
    double A=getInstrAxis2();
    // apply pointing model
    AlignH.instrToHor(A,Z,&A,&Z,getInstrPierSide());
    horToEqu(A,Z,&HA,Dec);
  }

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
  
  if (mountType != ALTAZM) {
    HA=getInstrAxis1();
    *Dec=getInstrAxis2();
  } else {
    double Z=getInstrAxis1();
    double A=getInstrAxis2();
    horToEqu(A,Z,&HA,Dec);
  }

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
CommandErrors goToHere(int destinationPierSide) {
  bool validated=false;
  if (meridianFlip == MeridianFlipNever) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  cli(); long h=posAxis1+indexAxis1Steps; sei();
  if (getInstrPierSide() == PIER_SIDE_EAST && destinationPierSide == PIER_SIDE_WEST && h < degreesPastMeridianW*(long)axis1Settings.stepsPerMeasure) validated=true;
  if (getInstrPierSide() == PIER_SIDE_WEST && destinationPierSide == PIER_SIDE_EAST && h > -degreesPastMeridianE*(long)axis1Settings.stepsPerMeasure) validated=true;
  if (validated) {
    double newRA,newDec;
    getEqu(&newRA,&newDec,false);
    CommandErrors e=goToEqu(newRA,newDec,destinationPierSide,false);
    return e;
  }
  if (getInstrPierSide() == PIER_SIDE_EAST && destinationPierSide == PIER_SIDE_EAST) return CE_NONE;
  if (getInstrPierSide() == PIER_SIDE_WEST && destinationPierSide == PIER_SIDE_WEST) return CE_NONE;
  return CE_SLEW_ERR_OUTSIDE_LIMITS;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
CommandErrors goToEqu(double RA, double Dec, int preferredPierSide, bool validateOnly) {
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

  if (mountType == ALTAZM) {
    equToHor(HA,Dec,&a,&z);
    AlignH.horToInstr(a,z,&a,&z,getInstrPierSide());
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
  } else {
    // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
    AlignE.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());
  
    // as above... for the opposite pier side just incase we need to do a meridian flip
    int p=PIER_SIDE_NONE; if (getInstrPierSide() == PIER_SIDE_EAST) p=PIER_SIDE_WEST; else if (getInstrPierSide() == PIER_SIDE_WEST) p=PIER_SIDE_EAST;
    AlignE.equToInstr(HA,Dec,&Axis1Alt,&Axis2Alt,p);
  }

  // goto function takes HA and Dec in steps
  byte thisPierSide = PIER_SIDE_BEST;
  if (meridianFlip != MeridianFlipNever) {
    if (preferredPierSide == WEST) thisPierSide=PIER_SIDE_WEST;
    if (preferredPierSide == EAST) thisPierSide=PIER_SIDE_EAST;
  }

  return goTo(Axis1,Axis2,Axis1Alt,Axis2Alt,thisPierSide,validateOnly);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
CommandErrors goToHor(double *Alt, double *Azm, int preferredPierSide) {
  double HA,Dec;
  
  horToEqu(*Alt,*Azm,&HA,&Dec);
  double RA=degRange(LST()*15.0-HA);
  return goToEqu(RA,Dec,preferredPierSide,false);
}

// moves the mount to a new Hour Angle and Declination, both in degrees.  Alternate targets are used when a meridian flip occurs
CommandErrors goTo(double thisTargetAxis1, double thisTargetAxis2, double altTargetAxis1, double altTargetAxis2, byte thisPierSide, bool validateOnly) {
  if (!validateOnly) atHome=false;
  byte currentPierSide=getInstrPierSide();
  byte thisPierSideControl=currentPierSide;
  if (meridianFlip != MeridianFlipNever) {
    // where the allowable hour angles are
    double eastOfPierMaxHA= axis1Settings.max;
    double eastOfPierMinHA=-degreesPastMeridianE;
    double westOfPierMaxHA= degreesPastMeridianW;
    double westOfPierMinHA= axis1Settings.min;

    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((thisPierSide != PIER_SIDE_BEST) && (thisPierSide != currentPierSide)) {
      if (currentPierSide == PIER_SIDE_EAST) eastOfPierMinHA= degreesPastMeridianW;
      if (currentPierSide == PIER_SIDE_WEST) westOfPierMaxHA=-degreesPastMeridianE;
    }
    
    // if doing a meridian flip, use the opposite pier side coordinates
    if (currentPierSide == PIER_SIDE_EAST) {
      if ((thisTargetAxis1 > eastOfPierMaxHA) || (thisTargetAxis1 < eastOfPierMinHA)) {
        thisPierSideControl=PIER_SIDE_EW1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 > westOfPierMaxHA) return CE_SLEW_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (currentPierSide == PIER_SIDE_WEST) {
      if ((thisTargetAxis1 > westOfPierMaxHA) || (thisTargetAxis1 < westOfPierMinHA)) {
        thisPierSideControl=PIER_SIDE_WE1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1 < eastOfPierMinHA) return CE_SLEW_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (currentPierSide == PIER_SIDE_NONE) {
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetAxis1 < 0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        thisPierSideControl=PIER_SIDE_WEST;
      } else {
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        thisPierSideControl=PIER_SIDE_EAST;
      }
    }
  } else {
    if (currentPierSide == PIER_SIDE_NONE) {
        // always on the "east" side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
        thisPierSideControl=PIER_SIDE_EAST;
    }
  }
  
  // final validation
  int destinationPierSide=PIER_SIDE_EAST;
  if (thisPierSideControl == PIER_SIDE_WEST || thisPierSideControl == PIER_SIDE_EW1) destinationPierSide=PIER_SIDE_WEST;

  if (mountType == ALTAZM) {
    // allow +/- 360 in Az
    if (((thisTargetAxis1 > axis1Settings.max) || (thisTargetAxis1 < axis1Settings.min)) || ((thisTargetAxis2 > 180.0) || (thisTargetAxis2 < -180.0))) return CE_GOTO_ERR_UNSPECIFIED;
  } else {
    if (((thisTargetAxis1 > 270.0) || (thisTargetAxis1 < -270.0)) || ((thisTargetAxis2 > 270.0) || (thisTargetAxis2 < -270.0))) return CE_GOTO_ERR_UNSPECIFIED;
    if (AXIS2_TANGENT_ARM == ON) {
      if (toInstrAxis2(thisTargetAxis2,destinationPierSide) < axis2Settings.min) return CE_SLEW_ERR_OUTSIDE_LIMITS;
      if (toInstrAxis2(thisTargetAxis2,destinationPierSide) > axis2Settings.max) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
  }

  if (!pauseHome && MFLIP_SKIP_HOME == ON) {
    if (thisPierSideControl == PIER_SIDE_WE1) thisPierSideControl=PIER_SIDE_EAST; else
    if (thisPierSideControl == PIER_SIDE_EW1) thisPierSideControl=PIER_SIDE_WEST;
  }

  if (validateOnly) {
    if (thisPierSideControl == PIER_SIDE_EW1 || thisPierSideControl == PIER_SIDE_WEST) return CE_0; else return CE_NONE;
  }

  lastTrackingState=trackingState;

  cli();
  trackingState=TrackingMoveTo;
  SiderealClockSetInterval(siderealInterval);

  startAxis1=posAxis1;
  startAxis2=posAxis2;

  timerRateAxis1=siderealRate;
  timerRateAxis2=siderealRate;
  sei();

  setTargetAxis1(thisTargetAxis1,destinationPierSide);
  setTargetAxis2(thisTargetAxis2,destinationPierSide);
  pierSideControl=thisPierSideControl;

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
