//--------------------------------------------------------------------------------------------------
// Goto, commands to move the telescope to an location or to report the current location

// check if goto/sync is valid
GotoErrors validateGoto() {
  // Check state
  if (faultAxis1 || faultAxis2)                return GOTO_ERR_HARDWARE_FAULT;
  if (!axis1Enabled)                           return GOTO_ERR_STANDBY;
  if (parkStatus!=NotParked)                   return GOTO_ERR_PARK;
  if (guideDirAxis1 || guideDirAxis2)          return GOTO_ERR_IN_MOTION;
  if (trackingState==TrackingMoveTo)           return GOTO_ERR_GOTO;
  return GOTO_ERR_NONE;
}

GotoErrors validateGotoCoords(double HA, double Dec, double Alt) {
  // Check coordinates
  if (Alt<minAlt)                              return GOTO_ERR_BELOW_HORIZON;
  if (Alt>maxAlt)                              return GOTO_ERR_ABOVE_OVERHEAD;
  if (Dec>MaxDec)                              return GOTO_ERR_OUTSIDE_LIMITS;
  if (Dec<MinDec)                              return GOTO_ERR_OUTSIDE_LIMITS;
  if ((fabs(HA)>(double)UnderPoleLimit*15.0) ) return GOTO_ERR_OUTSIDE_LIMITS;
  return GOTO_ERR_NONE;
}

GotoErrors validateGoToEqu(double RA, double Dec) {
  double a,z;
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);
  GotoErrors r=validateGoto(); if (r!=GOTO_ERR_NONE) return r;
  r=validateGotoCoords(HA,Dec,a);
  return r;
}

// syncs the telescope/mount to the sky
GotoErrors syncEqu(double RA, double Dec) {
  double a,z;

  // Convert RA into hour angle, get altitude
  // hour angleTrackingMoveTo
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  GotoErrors f=validateGoto(); if ((f!=GOTO_ERR_NONE) && (f!=GOTO_ERR_STANDBY)) return f;
  f=validateGotoCoords(HA,Dec,a); if (f!=GOTO_ERR_NONE) return f;

  double Axis1,Axis2;
#ifdef MOUNT_TYPE_ALTAZM
  equToHor(HA,Dec,&Axis2,&Axis1);
  Align.horToInstr(Axis2,Axis1,&Axis2,&Axis1,getInstrPierSide());
  Axis1=haRange(Axis1);
#else
  Align.equToInstr(HA,Dec,&Axis1,&Axis2,getInstrPierSide());
#endif

  // just turn on tracking
  if (atHome) { trackingState=TrackingSidereal; enableStepperDrivers(); }

  int newPierSide=getInstrPierSide();
  if (meridianFlip!=MeridianFlipNever) {
    if (atHome) { if (Axis1<0) newPierSide=PierSideWest; else newPierSide=PierSideEast; } else // best side of pier decided based on meridian
    if (preferredPierSide==PPS_WEST) newPierSide=PierSideWest; else // west side of pier - we're in the eastern sky and the HA's are negative
    if (preferredPierSide==PPS_EAST) newPierSide=PierSideEast; else // east side of pier - we're in the western sky and the HA's are positive
    {
#if defined(SYNC_CURRENT_PIER_SIDE_ONLY_ON) && defined(MOUNT_TYPE_GEM)
      if ((getInstrPierSide()==PierSideWest) && (haRange(Axis1)> minutesPastMeridianW/4.0)) newPierSide=PierSideEast;
      if ((getInstrPierSide()==PierSideEast) && (haRange(Axis1)<-minutesPastMeridianE/4.0)) newPierSide=PierSideWest;
#else
      if (Axis1<0) newPierSide=PierSideWest; else newPierSide=PierSideEast; // best side of pier decided based on meridian
#endif
    }
  
#if defined(SYNC_CURRENT_PIER_SIDE_ONLY_ON) && defined(MOUNT_TYPE_GEM)
    if (!atHome && (newPierSide!=getInstrPierSide())) return GOTO_ERR_OUTSIDE_LIMITS;
#endif
  } else {
    // always on the "east" side of pier - we're in the western sky and the HA's are positive
    // this is the default in the polar-home position and also for MOUNT_TYPE_FORK and MOUNT_TYPE_ALTAZM.  MOUNT_TYPE_FORK_ALT ends up pierSideEast, but flips are allowed until aligned.
    newPierSide=PierSideEast;
  }

  setIndexAxis1(Axis1,newPierSide);
  setIndexAxis2(Axis2,newPierSide);

  return GOTO_ERR_NONE;
}

// syncs internal counts to shaft encoder position (in degrees)
GotoErrors syncEnc(double EncAxis1, double EncAxis2) {
  // validate
  GotoErrors f=validateGoto(); if (f!=GOTO_ERR_NONE) return f;

  // no sync from encoders during an alignment!
  if (alignActive()) return GOTO_ERR_NONE;

  long e1=EncAxis1*(double)StepsPerDegreeAxis1;
  long e2=EncAxis2*(double)StepsPerDegreeAxis2;

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
  indexAxis1=(double)indexAxis1Steps/(double)StepsPerDegreeAxis1;
  indexAxis2Steps-=delta2;
  indexAxis2=(double)indexAxis2Steps/(double)StepsPerDegreeAxis2;

  return GOTO_ERR_NONE;
}

// get internal counts as shaft encoder position (in degrees)
int getEnc(double *EncAxis1, double *EncAxis2) {
  long a1,a2;
  cli();
  a1=posAxis1;
  a2=posAxis2;
  sei();
  a1+=indexAxis1Steps;
  a2+=indexAxis2Steps;
  
  *EncAxis1=(double)a1/(double)StepsPerDegreeAxis1;
  *EncAxis2=(double)a2/(double)StepsPerDegreeAxis2;

  return 0;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
 
#ifndef MOUNT_TYPE_ALTAZM
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
    while (*RA>=360.0) *RA-=360.0;
    while (*RA<0.0) *RA+=360.0;
  } else *RA=HA;
  
  return true;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getApproxEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
#ifndef MOUNT_TYPE_ALTAZM
  HA=getInstrAxis1();
  *Dec=getInstrAxis2();
#else
  double Z=getInstrAxis1();
  double A=getInstrAxis2();
  horToEqu(A,Z,&HA,Dec);
#endif

  HA=haRange(HA);
  if (*Dec>90.0) *Dec=+90.0;
  if (*Dec<-90.0) *Dec=-90.0;
  
  // return either the RA or the HA depending on returnHA
  if (!returnHA) {
    *RA=degRange(LST()*15.0-HA);
  } else *RA=HA;
  return true;
}

// gets the telescopes current Alt and Azm
boolean getHor(double *Alt, double *Azm) {
  double h,d;
  getEqu(&h,&d,true);
  equToHor(h,d,Alt,Azm);
  return true;
}

// causes a goto to the same RA/Dec on the opposite pier side if possible
GotoErrors goToHere(bool toEastOnly) {
  bool verified=false;
  PreferredPierSide p=preferredPierSide;
  if (meridianFlip==MeridianFlipNever) return GOTO_ERR_OUTSIDE_LIMITS;
  cli(); long h=posAxis1+indexAxis1Steps; sei();
  if ((!toEastOnly) && (getInstrPierSide()==PierSideEast) && (h<(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L))) { verified=true; preferredPierSide=PPS_WEST; }
  if ((getInstrPierSide()==PierSideWest) && (h>(-minutesPastMeridianE*(long)StepsPerDegreeAxis1/4L))) { verified=true; preferredPierSide=PPS_EAST; }
  if (verified) {
    double newRA,newDec;
    getEqu(&newRA,&newDec,false);
    GotoErrors r=goToEqu(newRA,newDec);
    preferredPierSide=p;
    return r;
  } else return GOTO_ERR_OUTSIDE_LIMITS;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
GotoErrors goToEqu(double RA, double Dec) {
  double a,z;
  double Axis1,Axis2;
  double Axis1Alt,Axis2Alt;

  // Convert RA into hour angle, get altitude
  double HA=haRange(LST()*15.0-RA);
  equToHor(HA,Dec,&a,&z);

  // validate
  GotoErrors r=validateGoto(); if (r==GOTO_ERR_GOTO) { if (!abortSlew) abortSlew=StartAbortSlew; } if (r!=GOTO_ERR_NONE) return r;
  r=validateGotoCoords(HA,Dec,a); if (r!=GOTO_ERR_NONE) return r;

#ifdef MOUNT_TYPE_ALTAZM
  equToHor(HA,Dec,&a,&z);
  Align.horToInstr(a,z,&a,&z,getInstrPierSide());
  z=haRange(z);

  if ((MaxAzm>180) && (MaxAzm<=360)) {
    // adjust coordinate range to allow going past 180 deg.
    // position a1 is 0..180
    double a1=getInstrAxis1();
    if (a1>=0) {
      // and goto z is in -0..-180
      if (z<0) {
        // the alternate z1 is in 180..360
        double z1=z+360.0;
        if ((z1<MaxAzm) && (dist(a1,z)>dist(a1,z1))) z=z1;
      }
    }
    // position a1 -0..-180
    if (a1<0) { 
      // and goto z is in 0..180
      if (z>0) {
        // the alternate z1 is in -360..-180
        double z1=z-360.0;
        if ((z1>-MaxAzm) && (dist(a1,z)>dist(a1,z1))) z=z1;
      }
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
  int p=PierSideNone; if (getInstrPierSide()==PierSideEast) p=PierSideWest; else if (getInstrPierSide()==PierSideWest) p=PierSideEast;
  Align.equToInstr(HA,Dec,&Axis1Alt,&Axis2Alt,p);
#endif

  // goto function takes HA and Dec in steps
  // when in align mode, force pier side
  byte thisPierSide = PierSideBest;
  if (meridianFlip!=MeridianFlipNever) {
    if (preferredPierSide==PPS_WEST) thisPierSide=PierSideWest;
    if (preferredPierSide==PPS_EAST) thisPierSide=PierSideEast;
  }

  return goTo(Axis1,Axis2,Axis1Alt,Axis2Alt,thisPierSide);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
GotoErrors goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  horToEqu(*Alt,*Azm,&HA,&Dec);
  double RA=degRange(LST()*15.0-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination - both are in steps.  Alternate targets are used when a meridian flip occurs
GotoErrors goTo(double thisTargetAxis1, double thisTargetAxis2, double altTargetAxis1, double altTargetAxis2, int gotoPierSide) {
  atHome=false;
  int thisPierSide=getInstrPierSide();

  if (meridianFlip!=MeridianFlipNever) {
    // where the allowable hour angles are
    double eastOfPierMaxHA= 180.0;
    double eastOfPierMinHA=-minutesPastMeridianE/4.0;
    double westOfPierMaxHA= minutesPastMeridianW/4.0;
    double westOfPierMinHA=-180.0;

    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((gotoPierSide!=PierSideBest) && (thisPierSide!=gotoPierSide)) {
      if (thisPierSide==PierSideEast) eastOfPierMinHA= minutesPastMeridianW/4.0;
      if (thisPierSide==PierSideWest) westOfPierMaxHA=-minutesPastMeridianE/4.0;
    }
    
    // if doing a meridian flip, use the opposite pier side coordinates
    if (thisPierSide==PierSideEast) {
      if ((thisTargetAxis1>eastOfPierMaxHA) || (thisTargetAxis1<eastOfPierMinHA)) {
        thisPierSide=PierSideFlipEW1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1>westOfPierMaxHA) return GOTO_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide==PierSideWest) {
      if ((thisTargetAxis1>westOfPierMaxHA) || (thisTargetAxis1<westOfPierMinHA)) {
        thisPierSide=PierSideFlipWE1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1<eastOfPierMinHA) return GOTO_ERR_OUTSIDE_LIMITS;
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (thisPierSide==PierSideNone) {
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetAxis1<0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        thisPierSide=PierSideWest;
      } else {
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        thisPierSide=PierSideEast;
      }
    }
  } else {
    if (getInstrPierSide()==PierSideNone) {
        // always on the "east" side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position and also for MOUNT_TYPE_FORK and MOUNT_TYPE_ALTAZM.  MOUNT_TYPE_FORK_ALT ends up pierSideEast, but flips are allowed until aligned.
        thisPierSide=PierSideEast;
    }
  }
  
  // final validation
#ifdef MOUNT_TYPE_ALTAZM
  // allow +/- 360 in Az
  if (((thisTargetAxis1>MaxAzm) || (thisTargetAxis1<-MaxAzm)) || ((thisTargetAxis2>180.0) || (thisTargetAxis2<-180.0))) return GOTO_ERR_UNSPECIFIED;
#else
  if (((thisTargetAxis1>180.0) || (thisTargetAxis1<-180.0)) || ((thisTargetAxis2>180.0) || (thisTargetAxis2<-180.0))) return GOTO_ERR_UNSPECIFIED;
#endif
  lastTrackingState=trackingState;

  cli();
  trackingState=TrackingMoveTo;
  SiderealClockSetInterval(siderealInterval);

  startAxis1=posAxis1;
  startAxis2=posAxis2;

  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;
  sei();

  int p=PierSideEast; switch (thisPierSide) { case PierSideWest: case PierSideFlipEW1: p=PierSideWest; break; }
  setTargetAxis1(thisTargetAxis1,p);
  setTargetAxis2(thisTargetAxis2,p);

  #ifdef MERIDIAN_FLIP_SKIP_HOME_ON
  boolean gotoDirect=true;
  #else
  boolean gotoDirect=false;
  #endif
  if (!pauseHome && gotoDirect) {
    if (thisPierSide==PierSideFlipWE1) pierSideControl=PierSideEast; else
    if (thisPierSide==PierSideFlipEW1) pierSideControl=PierSideWest; else pierSideControl=thisPierSide;
  } else pierSideControl=thisPierSide;

  D("Goto Axis1, Current "); D(((double)(long)posAxis1)/(double)StepsPerDegreeAxis1); D(" -to-> "); DL(((double)(long)targetAxis1.part.m)/(double)StepsPerDegreeAxis1);
  D("Goto Axis2, Current "); D(((double)(long)posAxis2)/(double)StepsPerDegreeAxis2); D(" -to-> "); DL(((double)(long)targetAxis2.part.m)/(double)StepsPerDegreeAxis2); DL("");

  reactivateBacklashComp();
  disablePec();
  soundAlert();
  stepperModeGoto();

  return GOTO_ERR_NONE;
}
