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
  decDir1=DecDir1Setting; decDir2=DecDir2Setting;
  targetHA =round(HA*15.0*(double)stepsPerDegreeHA);
  posHA    =targetHA;
  targetDec=round(Dec*(double)stepsPerDegreeDec);
  posDec=targetDec;
  sei();
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

  if ((abs(ID)>30.0) || (abs(IH)>2.0)) { IH=0; ID=0; return false; };
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
  // adjust HA to +/- 12 hours
  while (*HA>+12.0) *HA=*HA-24.0;
  while (*HA<-12.0) *HA=*HA+24.0;
  // limit declination to +/-90 degrees
  if (*Dec>+90.0) *Dec=+90.0;
  if (*Dec<-90.0) *Dec=-90.0;
}

// gets the telescopes current RA and Dec, set returnHA to true for Horizon Angle instead of RA
boolean getEqu(double *RA, double *Dec, boolean returnHA) {
  double HA;
  
  // get the uncorrected HA and Dec
  getHADec(&HA,Dec);
  
  // correct for polar offset, refraction, coordinate systems, etc. as required
  CEquToEqu(latitude,HA,*Dec,&HA,Dec);
  
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

  // Check to see if this goto is valid
  EquToHor(latitude,HA,Dec,&Alt,&Azm);

#ifdef DEBUG_ON
  Serial1.print("Alt="); Serial1.println(Alt);
  Serial1.print(", Azm="); Serial1.println(Azm);
#endif

  if (Alt<minAlt) return 1; // below horizon
  if (Alt>maxAlt) return 6; // outside limits

  // correct for polar offset, refraction, coordinate systems, etc. as required
  if (!EquToCEqu(latitude,HA,Dec,&HA,&Dec)) return 2;

  // important to make sure the HA/Dec are in range at this point
  while (HA>+12.0) HA=HA-24.0;
  while (HA<-12.0) HA=HA+24.0;

  // outside limits, too far under the NCP
  if (abs(HA)>9.0) return 6;

  long HA1=HA*15.0*(double)StepsPerDegreeHA;

  // check to see if we're doing a meridian flip, if so pre-correct the destination.  ID will get updated during the goTo()
  // same EXACT math as goTo() uses so we're sure to detect the flip every time
  #ifdef MEGA2560_ON
  if (((pierSide==PierSideEast) && (HA1<-(minutesPastMeridian*StepsPerDegreeHA/4L))) ||
      ((pierSide==PierSideWest) && (HA1> (minutesPastMeridian*StepsPerDegreeHA/4L)))) {
    Dec=Dec+ID+ID;
  }
  #endif

  if (Dec>+90.0) Dec=+90.0;
  if (Dec<-90.0) Dec=-90.0;
  
  // goto function takes HA and Dec in steps
  return goTo(HA1,Dec*(double)StepsPerDegreeDec);
}

// moves the mount to a new Altitude and Azmiuth (Alt,Azm) in degrees
byte goToHor(double *Alt, double *Azm) {
  double HA,Dec;
  
  HorToEqu(latitude,*Alt,*Azm,&HA,&Dec);
  
  double RA=timeRange(LST-HA);
  
  return goToEqu(RA,Dec);
}

// moves the mount to a new Hour Angle and Declination (HA,Dec) - both are in steps, all other HA's and Dec's are in Hours and Degrees
byte goTo(long HASteps, long DecSteps) {
  // make sure we're not already moving somewhere
  if ((trackingState==TrackingMoveTo) || moveDirHA || moveDirDec) return 4;
  
  atHome=false;

  cli();
  skipCountHA=SiderealRate*2;
  startHA    =posHA;
  targetHA   =HASteps;
  sei();

  // handle Meridian flip
  if (pierSide==PierSideEast) {
    if (targetHA<-(minutesPastMeridian*StepsPerDegreeHA/4L)) {
      pierSide=PierSideFlipEW1; 
      ID=-ID;  // correct the index if we're crossing the meridian
      #ifdef DEBUG_ON
      Serial1.println("Meridian flip EW");
      #endif
    }
  } else
  if (pierSide==PierSideWest) {
    if (targetHA> (minutesPastMeridian*StepsPerDegreeHA/4L)) {
      pierSide=PierSideFlipWE1; 
      ID=-ID;  // correct the index if we're crossing the meridian
      #ifdef DEBUG_ON
      Serial1.println("Meridian flip WE");
      #endif
    }
  } else { 
    // pierSide=PierSideNone
    // ID flips back and forth +/-, but that doesn't matter, if we're homed the ID is 0
    // we're in the polar home position, so pick a side (of the pier)
    if (targetHA<0) {
      pierSide=PierSideWest;
      DecDir  = DecDirWInit;

      // west side of pier - we're in the eastern sky and the HA's are negative
      // default, in the polar-home position is +90 deg. HA
      cli();
      posHA=posHA-180L*StepsPerDegreeHA;
      startHA=posHA;
      sei();
      
      #ifdef DEBUG_ON
      Serial1.println("West side of pier");
      #endif
    } else { 
      pierSide=PierSideEast;
      DecDir = DecDirEInit;

      // east side of pier - we're in the western sky and the HA's are positive
      // this is the default in the polar-home position

      #ifdef DEBUG_ON 
      Serial1.println("East side of pier");
      #endif
    }
  }

  cli(); 
  targetDec   =DecSteps;  
  #ifdef DEC_RATIO_ON
  skipCountDec=SiderealRate*2*SkipCountRateRatio;
  #else
  skipCountDec=SiderealRate*2;
  #endif
  startDec    =posDec;
  sei();

#ifdef DEBUG_ON
  Serial1.print("Current position = ");
  Serial1.print((posHA/StepsPerDegreeHA)/15.0);
  Serial1.print("(HA) ");
  Serial1.print(posDec/StepsPerDegreeDec);
  Serial1.println("(Dec)");
  
  Serial1.print("Target  position =");
  Serial1.print((targetHA/StepsPerDegreeHA)/15.0);
  Serial1.print("(HA) ");
  Serial1.print(targetDec/StepsPerDegreeDec);
  Serial1.println("(Dec)");
#endif

  lastTrackingState=trackingState;
  trackingState    =TrackingMoveTo;

  return 0;
}




