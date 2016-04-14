//--------------------------------------------------------------------------------------------------
// GoTo, commands to move the telescope to an location or to report the current location

// syncs the telescope/mount to the sky
boolean syncEqu(double RA, double Dec) {
  // hour angleTrackingMoveTo
  double HA=haRange(LST()*15.0-RA);

  // correct for polar misalignment only by clearing the index offsets
  IH=0; ID=0; IHS=0; IDS=0;

  double Axis1,Axis2;
#ifdef MOUNT_TYPE_ALTAZM
  EquToHor(HA,Dec,&Axis2,&Axis1); // convert from HA/Dec to Alt/Azm
  if (Axis1>180.0) Axis1-=360.0;
#else
  EquToIEqu(latitude,HA,Dec,&Axis1,&Axis2);
#endif

#if defined(SYNC_ANYWHERE_ON) && !defined(MOUNT_TYPE_ALTAZM)
  if ((pierSide==PierSideNone) || ((pierSide==PierSideWest) && (Axis1>0)) || ((pierSide==PierSideEast) && (Axis1<0))) {
    trackingState=TrackingSidereal;
    atHome=false;
    if (meridianFlip!=MeridianFlipNever) {
      // we're in the polar home position, so pick a side (of the pier)
      if (Axis1<0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        pierSide=PierSideWest;
        DecDir  =DecDirWInit;
      } else { 
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        pierSide=PierSideEast;
        DecDir = DecDirEInit;
      }
    } else {
      // always on the "east" side of pier - we're in the western sky and the HA's are positive
      // this is the default in the polar-home position and also for MOUNT_TYPE_FORK and MOUNT_TYPE_ALTAZM.  MOUNT_TYPE_FORK_ALT ends up pierSideEast, but flips are allowed until aligned.
      pierSide=PierSideEast;
      DecDir = DecDirEInit;
    }
  }
#endif

  // compute index offsets IH/ID, if they're within reason 
  // actual posAxis1/posAxis2 are the coords of where this really is
  // IH/ID are the amount to add to the actual RA/Dec to arrive at the correct position
  // double's are really single's on the ATMega's, and we're a digit or two shy of what's required to
  // hold the steps in some cases but it's still getting down to the arc-sec level
  // HA goes from +180...0..-180
  //                 W   .   E
  // IH and ID values get subtracted to arrive at the correct location
  IH=Axis1-((double)(long)targetAxis1.part.m)/(double)StepsPerDegreeAxis1;
#ifdef MOUNT_TYPE_ALTAZM
  if (abs(IH-360.0)<abs(IH)) IH-=360.0;
  if (abs(IH+360.0)<abs(IH)) IH+=360.0;
#endif
  IHS=(long)(IH*(double)StepsPerDegreeAxis1);
  ID=Axis2-((double)(long)targetAxis2.part.m)/(double)StepsPerDegreeAxis2;
  IDS=(long)(ID*(double)StepsPerDegreeAxis2);

#ifndef SYNC_ANYWHERE_ON
  if ((abs(ID)>30.0) || (abs(IH)>30.0)) { IH=0; ID=0; IHS=0; IDS=0; lastError=ERR_SYNC; return false; }
#endif
  return true;
}

// this returns the telescopes HA and Dec (index corrected for Alt/Azm)
void getHADec(double *HA, double *Dec) {
  cli();
  double Axis1=posAxis1;
  double Axis2=posAxis2;
  sei();
  
#ifdef MOUNT_TYPE_ALTAZM
  // get the hour angle (or Azm)
  double z=Axis1/(double)StepsPerDegreeAxis1;
  // get the declination (or Alt)
  double a=Axis2/(double)StepsPerDegreeAxis2;

  // instrument to corrected horizon
  z+=IH;
  a+=ID;

  HorToEqu(a,z,HA,Dec); // convert from Alt/Azm to HA/Dec
#else
  // get the hour angle (or Azm)
  *HA=Axis1/(double)StepsPerDegreeAxis1;
  // get the declination (or Alt)
  *Dec=Axis2/(double)StepsPerDegreeAxis2;
#endif
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
  // get the HA and Dec (already index corrected on AltAzm)
  getHADec(&HA,Dec);
  
#ifndef MOUNT_TYPE_ALTAZM
  // correct for under the pole, polar misalignment, and index offsets
  IEquToEqu(latitude,HA,*Dec,&HA,Dec);
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
  
  // get the HA and Dec (already index corrected on AltAzm)
  getHADec(&HA,Dec);
  
#ifndef MOUNT_TYPE_ALTAZM
  // instrument to corrected equatorial
  HA+=IH;
  *Dec+=ID;
#endif
  
  // un-do, under the pole
  if (*Dec>90.0) { *Dec=(90.0-*Dec)+90; HA=HA-180.0; }
  if (*Dec<-90.0) { *Dec=(-90.0-*Dec)-90.0; HA=HA-180.0; }

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
  EquToHor(h,d,Alt,Azm);
  return true;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
byte goToEqu(double RA, double Dec) {
  double a,z;

  // Convert RA into hour angle, get altitude
  double HA=haRange(LST()*15.0-RA);
  EquToHor(HA,Dec,&a,&z);

  // Check to see if this goto is valid
  if ((parkStatus!=NotParked) && (parkStatus!=Parking)) return 4;   // fail, Parked
  if (a<minAlt)                                         return 1;   // fail, below horizon
  if (a>maxAlt)                                         return 6;   // fail, outside limits
  if (Dec>MaxDec)                                       return 6;   // fail, outside limits
  if (Dec<MinDec)                                       return 6;   // fail, outside limits
  if (trackingState==TrackingMoveTo) { abortSlew=true;  return 5; } // fail, prior goto cancelled
  if (guideDirAxis1 || guideDirAxis2)                   return 7;   // fail, unspecified error

#ifdef MOUNT_TYPE_ALTAZM
  EquToHor(HA,Dec,&a,&z);
  z=haRange(z);  

  cli(); double a1=(posAxis1+IHS)/StepsPerDegreeAxis1; sei();

#ifdef MOUNT_TYPE_ALTAZM
  if ((MaxAzm>180) && (MaxAzm<=360)) {
    // adjust coordinate range to allow going past 180 deg.
    // position a1 is 0..180
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
#endif
  
  // corrected to instrument horizon
  z-=IH;
  a-=ID;
  long Axis1=z*(double)StepsPerDegreeAxis1;
  long Axis2=a*(double)StepsPerDegreeAxis2;
  long Axis1Alt=Axis1;
  long Axis2Alt=Axis2;
#else
  // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
  double h,d;
  if (!EquToIEqu(latitude,HA,Dec,&h,&d)) return 2; // fail, coordinates invalid
  long Axis1=h*(double)StepsPerDegreeAxis1;
  long Axis2=d*(double)StepsPerDegreeAxis2;

  // as above... for the opposite pier side just incase we need to do a meridian flip
  byte p=pierSide; if (pierSide==PierSideEast) pierSide=PierSideWest; else if (pierSide==PierSideWest) pierSide=PierSideEast;
  if (!EquToIEqu(latitude,HA,Dec,&h,&d)) return 2; // fail, coordinates invalid

  long Axis1Alt=h*(double)StepsPerDegreeAxis1;
  long Axis2Alt=d*(double)StepsPerDegreeAxis2;
  pierSide=p;
#endif

  // goto function takes HA and Dec in steps
  // when in align mode, force pier side
  byte thisPierSide=PierSideBest;
  if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) thisPierSide=PierSideWest;
  if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2) || (alignMode==AlignThreeStar3)) thisPierSide=PierSideEast;
  return goTo(Axis1,Axis2,Axis1Alt,Axis2Alt,thisPierSide);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
byte goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  HorToEqu(*Alt,*Azm,&HA,&Dec);
  double RA=degRange(LST()*15.0-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination - both are in steps.  Alternate targets are used when a meridian flip occurs
byte goTo(long thisTargetAxis1, long thisTargetAxis2, long altTargetAxis1, long altTargetAxis2, byte gotoPierSide) {
  // HA goes from +90...0..-90
  //                W   .   E

  if (faultAxis1 || faultAxis2) return 7; // fail, unspecified error

  atHome=false;

  if (meridianFlip!=MeridianFlipNever) {
    // where the allowable hour angles are
    long eastOfPierMaxHA= UnderPoleLimit*15L*(long)StepsPerDegreeAxis1;
    long eastOfPierMinHA=-(MinutesPastMeridianE*StepsPerDegreeAxis1/4L);
    long westOfPierMaxHA= (MinutesPastMeridianW*StepsPerDegreeAxis1/4L);
    long westOfPierMinHA=-UnderPoleLimit*15L*(long)StepsPerDegreeAxis1;
  
    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((gotoPierSide!=PierSideBest) && (pierSide!=gotoPierSide)) {
      eastOfPierMinHA= (MinutesPastMeridianW*(long)StepsPerDegreeAxis1/4L);
      westOfPierMaxHA=-(MinutesPastMeridianE*(long)StepsPerDegreeAxis1/4L);
    }
    // if doing a meridian flip, use the opposite pier side coordinates
    if (pierSide==PierSideEast) {
      if ((thisTargetAxis1+IHS>eastOfPierMaxHA) || (thisTargetAxis1+IHS<eastOfPierMinHA)) {
        pierSide=PierSideFlipEW1;
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1+IHS>westOfPierMaxHA) return 6; // fail, outside limits
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (pierSide==PierSideWest) {
      if ((thisTargetAxis1+IHS>westOfPierMaxHA) || (thisTargetAxis1+IHS<westOfPierMinHA)) {
        pierSide=PierSideFlipWE1; 
        thisTargetAxis1 =altTargetAxis1;
        if (thisTargetAxis1+IHS<eastOfPierMinHA) return 6; // fail, outside limits
        thisTargetAxis2=altTargetAxis2;
      }
    } else
    if (pierSide==PierSideNone) {
      // ID flips back and forth +/-, but that doesn't matter, if we're homed the ID is 0
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetAxis1<0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        pierSide=PierSideWest;
        DecDir  =DecDirWInit;
        // default, if the polar-home position is +90 deg. HA, we want -90HA
        // for a fork mount the polar-home position is 0 deg. HA, so leave it alone
#if !defined(MOUNT_TYPE_FORK) && !defined(MOUNT_TYPE_FORK_ALT)
        cli(); posAxis1-=(long)(180.0*(double)StepsPerDegreeAxis1); sei();
        trueAxis1-=(long)(180.0*(double)StepsPerDegreeAxis1);
#endif
      } else {
        // east side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position
        pierSide=PierSideEast;
        DecDir = DecDirEInit;
      }
    }
  } else {
    if (pierSide==PierSideNone) {
        // always on the "east" side of pier - we're in the western sky and the HA's are positive
        // this is the default in the polar-home position and also for MOUNT_TYPE_FORK and MOUNT_TYPE_ALTAZM.  MOUNT_TYPE_FORK_ALT ends up pierSideEast, but flips are allowed until aligned.
        pierSide=PierSideEast;
        DecDir = DecDirEInit;
    }
  }
  
  // final validation
#ifdef MOUNT_TYPE_ALTAZM
  // allow +/- 360 in Az
  if (((thisTargetAxis1+IHS>(long)StepsPerDegreeAxis1*MaxAzm) || (thisTargetAxis1+IHS<-(long)StepsPerDegreeAxis1*MaxAzm)) ||
     ((thisTargetAxis2+IDS>(long)StepsPerDegreeAxis2*180L) || (thisTargetAxis2+IDS<-(long)StepsPerDegreeAxis2*180L))) return 7; // fail, unspecified error
#else
  if (((thisTargetAxis1+IHS>(long)StepsPerDegreeAxis1*180L) || (thisTargetAxis1+IHS<-(long)StepsPerDegreeAxis1*180L)) ||
     ((thisTargetAxis2+IDS>(long)StepsPerDegreeAxis2*180L) || (thisTargetAxis2+IDS<-(long)StepsPerDegreeAxis2*180L))) return 7; // fail, unspecified error
#endif
  lastTrackingState=trackingState;

  cli();
  trackingState=TrackingMoveTo; 
  SetSiderealClockRate(siderealInterval);

  startAxis1=posAxis1;
  startAxis2=posAxis2;

  targetAxis1.part.m=thisTargetAxis1; targetAxis1.part.f=0;
  targetAxis2.part.m=thisTargetAxis2; targetAxis2.part.f=0;

  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;
  sei();

  DisablePec();

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  DecayModeGoto();
#endif
  
  return 0;
}
