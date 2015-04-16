//--------------------------------------------------------------------------------------------------
// GoTo, commands to move the telescope to an location or to report the current location

// syncs the telescope/mount to the sky
boolean syncEqu(double RA, double Dec) {
  // hour angleTrackingMoveTo
  double HA=LST-RA;
  while (HA>+12.0) HA-=24.0;
  while (HA<-12.0) HA+=24.0;

  // correct for polar misalignment only by clearing the index offsets
  IH=0; ID=0;
  EquToCEqu(latitude,HA,Dec,&HA,&Dec);
  
#ifdef RESCUE_MODE_ON
  // allows one to quickly set where OnStep thinks the 'scope is to anywhere in the western sky
  cli();
  pierSide=PierSideEast;
  DecDir = DecDirEInit;
  targetHA =round(HA*(double)(StepsPerDegreeHA*15L)); ftargetHA=longToFixed(targetHA);
  posHA    =targetHA;
  targetDec=round(Dec*(double)StepsPerDegreeDec); ftargetDec=longToFixed(targetDec);
  posDec=targetDec;
  sei();
  return true;
#else
  // compute index offsets IH/ID, if they're within reason 
  // actual PosHA/PosDec are the coords of where this really is
  // IH/ID are the amount to add to the actual RA/Dec to arrive at the correct position
  // double's are really single's on the ATMega's, and we're a digit or two shy of what's required to
  // hold the steps (for my 11520 steps per degree) but it's still getting down to the arc-sec level
  // HA goes from +12...0..-12
  //                W   .   E
  // IH and ID values get subtracted to arrive at the correct location
  double HA1=(double)targetHA/(double)(StepsPerDegreeHA*15L);
  IH=HA-HA1;
  ID=Dec-(double)targetDec/(double)StepsPerDegreeDec;

  if ((abs(ID)>30.0) || (abs(IH)>2.0)) { IH=0; ID=0; return false; } else return true;
#endif
}

// this returns the telescopes HA and Dec
void getHADec(double *HA, double *Dec) {
  cli();
  long h=posHA;
  long d=posDec;
  sei();
  // get the hour angle
  *HA=(double)h/(double)(StepsPerDegreeHA*15L);
  // get the declination
  *Dec=(double)d/(double)StepsPerDegreeDec; 
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
long lastGetEqu=0;
double lastGetEquLST=0;
double lastGetEquHA=0;
double lastGetEquDec=0;
boolean getEqu(double *RA, double *Dec, boolean returnHA, boolean fast) {
  unsigned long Tmin=100;
  if (trackingState==TrackingMoveTo) Tmin=1000;

  // return last values if recent results apply
  if ((fast) && (millis()-lastGetEqu<Tmin)) {
    *Dec=lastGetEquDec;
    // return either the RA or the HA depending on returnHA
    if (!returnHA) *RA=timeRange(lastGetEquLST-lastGetEquHA); else *RA=lastGetEquHA;
    return true;
  }
  
  // get the uncorrected HA and Dec
  getHADec(&lastGetEquHA,Dec);
  
  // correct for under the pole, polar misalignment, and index offsets
  CEquToEqu(latitude,lastGetEquHA,*Dec,&lastGetEquHA,Dec);

  lastGetEquLST=LST;
  lastGetEquDec=*Dec;
  lastGetEqu=millis();

  // return either the RA or the HA depending on returnHA
  if (!returnHA) *RA=timeRange(LST-lastGetEquHA); else *RA=lastGetEquHA;
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
  if (*Dec>90.0) { *Dec=(90.0-*Dec)+90; HA=HA-12; }
  if (*Dec<-90.0) { *Dec=(-90.0-*Dec)-90.0; HA=HA-12; }

  while (HA>=12.0) HA=HA-24.0;
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
  getEqu(&HA,&Dec,true,true);
  EquToHor(latitude,HA,Dec,Alt,Azm);
  return true;
}

// moves the mount to a new Right Ascension and Declination (RA,Dec) in degrees
byte goToEqu(double RA, double Dec) {
  double Alt,Azm,f;

  // Convert RA into hour angle, get altitude
  double HA=LST-RA;
  while (HA>=12.0) HA=HA-24.0;
  while (HA<-12.0) HA=HA+24.0;
  EquToHor(latitude,HA,Dec,&Alt,&Azm);

  // Check to see if this goto is valid
  if (parkStatus!=NotParked) return 4; // fail, not unparked 
  if (Alt<minAlt) return 1;            // fail, below horizon
  if (Alt>maxAlt) return 6;            // fail, outside limits
  if (Dec>maxDec) return 6;            // fail, outside limits
  if (Dec<minDec) return 6;            // fail, outside limits

  // correct for polar offset, refraction, coordinate systems, operation past pole, etc. as required
  double ha,dec;
  if (!EquToCEqu(latitude,HA,Dec,&ha,&dec)) return 2; // fail, coordinates invalid
  long HA1=ha*(double)(StepsPerDegreeHA*15L);
  long DEC1=dec*(double)StepsPerDegreeDec;

  // as above... for the opposite pier side just incase we need to do a meridian flip
  byte p=pierSide; if (pierSide==PierSideEast) pierSide=PierSideWest; else if (pierSide==PierSideWest) pierSide=PierSideEast;
  ID=-ID; if (!EquToCEqu(latitude,HA,Dec,&ha,&dec)) { ID=-ID; return 2; } ID=-ID; // fail, coordinates invalid
  long HA2=ha*(double)(StepsPerDegreeHA*15L);
  long DEC2=dec*(double)StepsPerDegreeDec;
  pierSide=p;

  // goto function takes HA and Dec in steps
  // when in align mode, force pier side
  byte thisPierSide=PierSideBest;
  if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) thisPierSide=PierSideWest;
  if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2) || (alignMode==AlignThreeStar3)) thisPierSide=PierSideEast;
  return goTo(HA1,DEC1,HA2,DEC2,thisPierSide);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
byte goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  HorToEqu(latitude,*Alt,*Azm,&HA,&Dec);
  
  double RA=timeRange(LST-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination - both are in steps.  Alternate targets are used when a meridian flip occurs
byte goTo(long thisTargetHA, long thisTargetDec, long altTargetHA, long altTargetDec, byte gotoPierSide) {
  // HA goes from +90...0..-90
  //                W   .   E

  // Check to see if this goto is valid
  if (trackingState==TrackingMoveTo) { abortSlew=true; return 5; }    // fail, prior goto cancelled
  if (guideDirHA || guideDirDec) return 7;                           // fail, unspecified error
  
  atHome=false;

  if (meridianFlip!=MeridianFlipNever) {
    // where the allowable hour angles are
    long eastOfPierMaxHA= underPoleLimit*15*StepsPerDegreeHA;
    long eastOfPierMinHA=-(minutesPastMeridianE*StepsPerDegreeHA/4L);
    long westOfPierMaxHA= (minutesPastMeridianW*StepsPerDegreeHA/4L);
    long westOfPierMinHA=-underPoleLimit*15*StepsPerDegreeHA;
  
    // override the defaults and force a flip if near the meridian and possible (for parking and align)
    if ((gotoPierSide!=PierSideBest) && (pierSide!=gotoPierSide)) {
      eastOfPierMinHA= (minutesPastMeridianW*StepsPerDegreeHA/4L);
      westOfPierMaxHA=-(minutesPastMeridianE*StepsPerDegreeHA/4L);
    }
    
    // if doing a meridian flip, use the opposite pier side coordinates
    if (pierSide==PierSideEast) {
      if ((thisTargetHA>eastOfPierMaxHA) || (thisTargetHA<eastOfPierMinHA)) {
        pierSide=PierSideFlipEW1;
        thisTargetHA =altTargetHA;
        thisTargetDec=altTargetDec;
      }
    } else
    if (pierSide==PierSideWest) {
      if ((thisTargetHA>westOfPierMaxHA) || (thisTargetHA<westOfPierMinHA)) {
        pierSide=PierSideFlipWE1; 
        thisTargetHA =altTargetHA;
        thisTargetDec=altTargetDec;
      }
    } else
    if (pierSide==PierSideNone) {
      // ID flips back and forth +/-, but that doesn't matter, if we're homed the ID is 0
      // we're in the polar home position, so pick a side (of the pier)
      if (thisTargetHA<0) {
        // west side of pier - we're in the eastern sky and the HA's are negative
        pierSide=PierSideWest;
        DecDir  =DecDirWInit;
        // default, in the polar-home position is +90 deg. HA, we want -90HA
//        cli(); posHA=posHA-180L*StepsPerDegreeHA; sei();
        cli(); posHA=-posHA; sei();
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
        // this is the default in the polar-home position
        pierSide=PierSideEast;
        DecDir = DecDirEInit;
    }
  }
  
  lastTrackingState=trackingState;
  trackingState=TrackingMoveTo;
  SetSiderealClockRate(siderealInterval);

  cli();
  startHA =posHA;
  startDec=posDec;

  targetHA =thisTargetHA; fTargetHA=longToFixed(targetHA);
  targetDec=thisTargetDec; fTargetDec=longToFixed(targetDec);

  timerRateHA=SiderealRate;
  timerRateDec=SiderealRate;
  sei();
  
  disablePec();
  
  return 0;
}
