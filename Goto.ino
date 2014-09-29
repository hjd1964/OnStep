//--------------------------------------------------------------------------------------------------
// GoTo, commands to move the telescope to an location or to report the current location

// syncs the telescope/mount to the sky
boolean syncEqu(double RA, double Dec) {
  // hour angle
  double HA=LST-RA;
  while (HA>+12.0) HA-=24.0;
  while (HA<-12.0) HA+=24.0;

  // correct for polar offset only by clearing the equatorial offsets
  IH=0; ID=0;
  EquToCEqu(latitude,HA,Dec,&HA,&Dec);

  while (HA>+12.0) HA-=24.0;
  while (HA<-12.0) HA+=24.0;
  
#ifdef RESCUE_MODE_ON
  // allows one to quickly set where OnStep thinks the 'scope is to anywhere in the western sky
  cli();
  pierSide=PierSideEast;
  DecDir = DecDirEInit;
  targetHA =round(HA*15.0*(double)StepsPerDegreeHA);
  posHA    =targetHA;
  targetDec=round(Dec*(double)StepsPerDegreeDec);
  posDec=targetDec;
  sei();
  return true;
#else
  // compute index offsets IH/ID, if they're within reason 
  // actual PosHA/PosDec are the coords of where this really is
  // IH/ID are the amount to add to the actual RA/Dec to arrive at the correct position
  // double's are really single's on the ATMega's, and we're a digit or two shy of what's required to
  // hold the steps (for my 11520 steps per degree) but it's still getting down to the arc-sec level
  // in the western sky hour angles run from   0 to 12
  // in the eastern sky hour angles run from -12 to 0
  // -12 - (-11) = -12 + 11 = -1
  double HA1=((double)targetHA/(double)StepsPerDegreeHA)/15.0;
  cli();
  IH=HA-HA1;
  ID=Dec-((double)targetDec/(double)StepsPerDegreeDec);
  sei();

  if ((abs(ID)>30.0) || (abs(IH)>2.0)) { IH=0; ID=0; return false; } else return true;
#endif
}

// this returns the current, uncorrected HA and Dec
void getHADec(double *HA, double *Dec) {
  cli(); 
  // get the hour angle
  *HA=((double)posHA/(double)StepsPerDegreeHA)/15.0;
  // get the declination
  *Dec=((double)posDec/(double)StepsPerDegreeDec); 
  sei();
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
  // get the uncorrected HA and Dec
  getHADec(&HA,Dec);
  
  // correct for over slew, polar offset, refraction, coordinate systems, etc. as required
  CEquToEqu(latitude,HA,*Dec,&HA,Dec);
  
  // return either the RA or the HA depending on returnHA
  if (!returnHA) *RA=timeRange(LST-HA); else *RA=HA;
  return true;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getApproxEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
  // get the uncorrected HA and Dec
  getHADec(&HA,Dec);
  
  // remove the index offsets
  HA=HA+IH;
  *Dec=*Dec+ID;
  
  // un-do, under the pole
  if (*Dec>90.0) {
    *Dec=(90.0-*Dec)+90.0;
    HA =HA-12;
  }
  while (HA>+12.0) HA=HA-24.0;
  while (HA<-12.0) HA=HA+24.0;
  if (*Dec>+90.0) *Dec=+90.0;
  if (*Dec<-90.0) *Dec=-90.0;
  
  // return either the RA or the HA depending on returnHA
  if (!returnHA) *RA=timeRange(LST-HA); else *RA=HA;
  return true;
}

// gets the telescopes current Alt and Azm
boolean getHor(double *Alt, double *Azm) {
double HA,Dec;
  getEqu(&HA,&Dec,true);
  EquToHor(latitude,HA,Dec,Alt,Azm);
  return true;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
byte goToEqu(double RA, double Dec) {
  double Alt,Azm,f;

  // fail with position unreachable if we're not unparked
  if (parkStatus!=NotParked) return 4;
  // abort goto and fail with position unreachable if we're already moving
  if (trackingState==TrackingMoveTo) { abortSlew=true; return 5; }
  
  // Convert RA into hour angle
  double HA=LST-RA;
  while (HA>+12.0) HA=HA-24.0;
  while (HA<-12.0) HA=HA+24.0;

  // Check to see if this goto is valid
  EquToHor(latitude,HA,Dec,&Alt,&Azm);
  if (Alt<minAlt) return 1; // below horizon
  if (Alt>maxAlt) return 6; // outside limits

  // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
  if (!EquToCEqu(latitude,HA,Dec,&HA,&Dec)) return 2;

  long HA1=HA*15.0*(double)StepsPerDegreeHA;

  // check to see if we're doing a meridian flip, if so pre-correct the destination.  ID will get updated during the goTo()
  // same EXACT math as goTo() uses so we're sure to detect the flip every time
//  if (((pierSide==PierSideEast) && (HA1<-(minutesPastMeridianE*StepsPerDegreeHA/4L))) ||
//      ((pierSide==PierSideWest) && (HA1> (minutesPastMeridianW*StepsPerDegreeHA/4L)))) {
//    Dec=Dec+ID+ID;
//  }

  // goto function takes HA and Dec in steps
  // when in align mode, force pier side
  if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) {
    return goTo(HA1,Dec*(double)StepsPerDegreeDec,PierSideWest);
  } else
  if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2) || (alignMode==AlignThreeStar3)) {
    return goTo(HA1,Dec*(double)StepsPerDegreeDec,PierSideEast);
  } else {
    return goTo(HA1,Dec*(double)StepsPerDegreeDec,PierSideBest);
  }
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
byte goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  HorToEqu(latitude,*Alt,*Azm,&HA,&Dec);
  
  double RA=timeRange(LST-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination (HA,Dec) - both are in steps, all other HA's and Dec's are in Hours and Degrees
byte goTo(long HASteps, long DecSteps, byte gotoPierSide) {
  // HA goes from +90...0..-90
  //                W   .   E

  // make sure we're not already moving somewhere
  if ((trackingState==TrackingMoveTo) || moveDirHA || moveDirDec) return 4;
  
  atHome=false;

  cli();
  timerRateHA=SiderealRate;
  startHA    =posHA;
  targetHA   =HASteps;
  sei();

  // where the allowable hour angles are
  long eastOfPierMaxHA= 9*15*StepsPerDegreeHA;
  long eastOfPierMinHA=-(minutesPastMeridianE*StepsPerDegreeHA/4L);
  long westOfPierMaxHA= (minutesPastMeridianW*StepsPerDegreeHA/4L);
  long westOfPierMinHA=-9*15*StepsPerDegreeHA;

  // override the defaults and force a flip if near the meridian and possible
  if ((gotoPierSide!=PierSideBest) && (pierSide!=gotoPierSide)) {
    eastOfPierMinHA= (minutesPastMeridianW*StepsPerDegreeHA/4L);
    westOfPierMaxHA=-(minutesPastMeridianE*StepsPerDegreeHA/4L);
  }
  
  // if only do a meridian flip, if we have to
  if (pierSide==PierSideEast) {
    if ((targetHA>eastOfPierMaxHA) || (targetHA<eastOfPierMinHA)) {
      pierSide=PierSideFlipEW1; 
      DecSteps=DecSteps+ID*2.0*(double)StepsPerDegreeDec;
      ID=-ID;  // correct the index if we're crossing the meridian
    }
  } else
  if (pierSide==PierSideWest) {
    if ((targetHA>westOfPierMaxHA) || (targetHA<westOfPierMinHA)) {
      pierSide=PierSideFlipWE1; 
      DecSteps=DecSteps+ID*2.0*(double)StepsPerDegreeDec;
      ID=-ID;  // correct the index if we're crossing the meridian
    }
  } else
  if (pierSide==PierSideNone) {
    // ID flips back and forth +/-, but that doesn't matter, if we're homed the ID is 0
    // we're in the polar home position, so pick a side (of the pier)
    if (targetHA<0) {
      // west side of pier - we're in the eastern sky and the HA's are negative
      // default, in the polar-home position is +90 deg. HA
      pierSide=PierSideWest;
      DecDir  = DecDirWInit;
    
      cli();
      posHA=posHA-180L*StepsPerDegreeHA;
      startHA=posHA;
      sei();
    } else { 
      // east side of pier - we're in the western sky and the HA's are positive
      // this is the default in the polar-home position
      pierSide=PierSideEast;
      DecDir = DecDirEInit;
    }
  }

  cli(); 
  targetDec   =DecSteps;  
  #ifdef DEC_RATIO_ON
  timerRateDec=SiderealRate*timerRateRatio;
  #else
  timerRateDec=SiderealRate;
  #endif
  startDec    =posDec;
  sei();

  lastTrackingState=trackingState;
  trackingState    =TrackingMoveTo;

  return 0;
}
