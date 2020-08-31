// -----------------------------------------------------------------------------------------------------------------------------
// Astronomy related functions

// convert string in format MM/DD/YY to julian date
bool dateToDouble(double *JulianDay, char *date) {
  char m[3],d[3],y[3];
  int  m1,d1,y1;
  
  if (strlen(date) !=  8) return false;

  m[0]=*date++; m[1]=*date++; m[2]=0; if (!atoi2(m,&m1,false)) return false;
  if (*date++ != '/') return false; d[0]=*date++; d[1]=*date++; d[2]=0; if (!atoi2(d,&d1,false)) return false;
  if (*date++ != '/') return false; y[0]=*date++; y[1]=*date++; y[2]=0; if (!atoi2(y,&y1,false)) return false;
  if ((m1 < 1) || (m1 > 12) || (d1 < 1) || (d1 > 31) || (y1 < 0) || (y1 > 99)) return false;
  if (y1 > 11) y1=y1+2000; else y1=y1+2100;
  
  *JulianDay=julian(y1,m1,d1);
  return true;
}

// convert string in format HH:MM:SS to double
// (also handles)           HH:MM.M
// (also handles)           HH:MM:SS
// (also handles)           HH:MM:SS.SSSS
bool hmsToDouble(double *f, char *hms, PrecisionMode p) {
  char h[3],m[5];
  int  h1,m1,m2=0;
  double s1=0;

  while (*hms == ' ') hms++; // strip prefix white-space

  if (strlen(hms) > 13) hms[13]=0; // limit maximum length
  int len=strlen(hms);
  
  if (p == PM_HIGHEST || p == PM_HIGH) { // validate length
    if (len != 8 && len < 10) return false;
  } else
  if (p == PM_LOW) {
    if (len != 7) return false;
  }

  // convert the hours part
  h[0]=*hms++; h[1]=*hms++; h[2]=0; if (!atoi2(h,&h1,false)) return false;

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*hms++ != ':') return false;
  m[0]=*hms++; m[1]=*hms++; m[2]=0; if (!atoi2(m,&m1,false)) return false;

  if (p == PM_HIGHEST || p == PM_HIGH) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*hms++ != ':') return false;
    if (!atof2(hms,&s1,false)) return false;
  } else
  if (p == PM_LOW) {
    // make sure the seperator is an allowed character, then convert the decimal minutes part
    if (*hms++ != '.') return false;
    m2=(*hms++)-'0';
  }
  
  if (h1 < 0 || h1 > 23 || m1 < 0 || m1 > 59 || m2 < 0 || m2 > 9 || s1 < 0 || s1 > 59.9999) return false;

  *f=(double)h1+(double)m1/60.0+(double)m2/600.0+s1/3600.0;
  return true;
}
bool hmsToDouble(double *f, char *hms) {
  if (!hmsToDouble(f,hms,PM_HIGHEST))
    if (!hmsToDouble(f,hms,PM_HIGH))
      if (!hmsToDouble(f,hms,PM_LOW)) return false;
  return true;
}

// convert double to string in a variety of formats (as above) 
void doubleToHms(char *reply, double *f, PrecisionMode p) {
  double h1,m1,f1,s1,sd=0;

  // round to 0.00005 second or 0.5 second, depending on precision mode
  if (p == PM_HIGHEST) f1=fabs(*f)+0.0000000139; else f1=fabs(*f)+0.000139;

  h1=floor(f1);
  m1=(f1-h1)*60.0;
  s1=(m1-floor(m1))*60.0;

  // finish off calculations for hms and form string template
  char s[]="%s%02d:%02d:%02d.%04d";
  if (p == PM_HIGHEST) {
    sd=(s1-floor(s1))*10000.0;
  } else
  if (p == PM_HIGH) {
    s[16]=0;
  } else
  if (p == PM_LOW) {
    s1=s1/6.0;
    s[11]='.'; s[14]='1'; s[16]=0;
  }

  // set sign and return result string
  char sign[2]="";
  if ((sd != 0 || s1 != 0 || m1 != 0 || h1 != 0) && *f < 0.0) strcpy(sign,"-");
  if (p == PM_HIGHEST) sprintf(reply,s,sign,(int)h1,(int)m1,(int)s1,(int)sd); else sprintf(reply,s,sign,(int)h1,(int)m1,(int)s1);
}

// convert string in format sDD:MM:SS to double
// (also handles)           sDD:MM:SS.SSS
//                          DDD:MM:SS
//                          sDD:MM
//                          DDD:MM
//                          sDD*MM
//                          DDD*MM
bool dmsToDouble(double *f, char *dms, bool sign_present, PrecisionMode p) {
  char d[4],m[5];
  int d1,m1,lowLimit=0,highLimit=360,len;
  double s1=0,sign=1;
  bool secondsOff=false;

  while (*dms == ' ') dms++; // strip prefix white-space
  if (strlen(dms) > 13) dms[13]=0; // maximum length
  len=strlen(dms);

  if (p == PM_HIGHEST || p == PM_HIGH) { // validate length
    if (len != 9 && len < 11) return false;
  } else
  if (p == PM_LOW) {
    if (len != 6) {
      if (len != 9) return false;
      secondsOff=false;
    } else secondsOff = true;
  }

  // determine if the sign was used and accept it if so, then convert the degrees part
  if (sign_present) {
    if (*dms == '-') sign=-1.0; else if (*dms == '+') sign=1.0; else return false; 
    dms++; d[0]=*dms++; d[1]=*dms++; d[2]=0; if (!atoi2(d,&d1,false)) return false;
  } else {
    d[0]=*dms++; d[1]=*dms++; d[2]=*dms++; d[3]=0; if (!atoi2(d,&d1,false)) return false;
  }

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*dms != ':' && *dms != '*' && *dms != char(223)) return false; else dms++;
  m[0]=*dms++; m[1]=*dms++; m[2]=0; if (!atoi2(m,&m1,false)) return false;

  if ((p == PM_HIGHEST || p == PM_HIGH) && !secondsOff) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*dms++ != ':' && *dms++ != '\'') return false;
    if (!atof2(dms,&s1,false)) return false;
  }

  if (sign_present) { lowLimit=-90; highLimit=90; }
  if ((d1 < lowLimit) || (d1 > highLimit) || (m1 < 0) || (m1 > 59) || (s1 < 0) || (s1 > 59.999)) return false;

  *f=sign*((double)d1+(double)m1/60.0+s1/3600.0);
  return true;
}
bool dmsToDouble(double *f, char *dms, bool sign_present) {
  if (!dmsToDouble(f,dms,sign_present,PM_HIGHEST))
    if (!dmsToDouble(f,dms,sign_present,PM_HIGH))
      if (!dmsToDouble(f,dms,sign_present,PM_LOW)) return false;
  return true;
}

// convert double to string in a variety of formats (as above) 
void doubleToDms(char *reply, double *f, bool fullRange, bool signPresent, PrecisionMode p) {
  char sign[]="+";
  int  o=0;
  double d1,m1,s1=0,s2,f1;

  // setup formatting, handle adding the sign
  f1=*f;
  if (f1 < 0) { f1=-f1; sign[0]='-'; }

  // round to 0.0005 arc-second or 0.5 arc-second, depending on precision mode
  if (p == PM_HIGHEST) f1=f1+0.000000139; else f1=f1+0.000139; 

  d1=floor(f1);
  m1=(f1-d1)*60.0;
  s1=(m1-floor(m1))*60.0;

  // finish off calculations for dms and form string template
  char s[]="+%02d*%02d:%02d.%03d";
  if (p == PM_HIGHEST) {
    s2=(s1-floor(s1))*1000.0;
  } else s[15]=0;

  if (signPresent) {
    if (sign[0] == '-') s[0]='-';
    o=1;
  } else memmove(&s[0],&s[1],strlen(s));

  if (fullRange) s[2+o]='3';
 
  // return result string
  if (p == PM_HIGHEST) {
    sprintf(reply,s,(int)d1,(int)m1,(int)s1,(int)s2);
  } else
  if (p == PM_HIGH) {
    sprintf(reply,s,(int)d1,(int)m1,(int)s1);
  } else
  if (p == PM_LOW) {
    s[9+o]=0;
    sprintf(reply,s,(int)d1,(int)m1);
  }
}

// convert timezone to string in format sHHH:MM[:SS]
void timeZoneToHM(char *reply, double tz) {
  double f=fabs(frac(tz));
  sprintf(reply,"%+03d",(int)tz);
  // append for :30
  if (fabs(f-0.5) < 0.00000001) {
    strcat(reply,":30");
  }
  // append for :45
  if (fabs(f-0.75) < 0.00000001) {
    strcat(reply,":45");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
// Date/time conversion

// convert date/time to Greenwich Apparent Sidereal time
double jd2gast(double JulianDay, double ut1) {
  int y,m,d;
  greg(JulianDay,&y,&m,&d);
  double JulianDay0=julian(y,m,d);
  double D= (JulianDay - 2451545.0)+(ut1/24.0);
  double D0=(JulianDay0- 2451545.0);
  double H = ut1;
  double T = D/36525.0;
  double gmst=6.697374558 + 0.06570982441908*D0;
  gmst=timeRange(gmst);
  gmst=gmst + 1.00273790935*H + 0.000026*T*T;
  gmst=timeRange(gmst);

  // equation of the equinoxes
  double O = 125.04 - 0.052954*D;
  double L = 280.47 + 0.98565*D;
  double E = 23.4393 - 0.0000004*D;
  double W = -0.000319*sin(O/Rad) - 0.000024*sin((2*L)/Rad);
  double eqeq = W*cos(E/Rad);
  double gast=gmst+eqeq;
  return timeRange(gast);
}

// convert date/time to Local Apparent Sidereal Time
// optionally updates the RTC, uses longitude
double jd2last(double JulianDay, double ut1, bool updateRTC) {
  if (updateRTC) {
    // UT to local time
    double lmt=ut1-timeZone;

    // correct for day moving forward/backward... this works for multipule days of up-time
    double J=JulianDay;
    while (lmt >= 24.0) { lmt=lmt-24.0; J=J-1.0; } 
    if    (lmt < 0.0)   { lmt=lmt+24.0; J=J+1.0; }

    // set the RTC
    tls.set(J,lmt);
  }
  // JulianDay is the Local date, jd2gast requires a universal time
  // this is a hack that leaves the date alone and lets the UT1 cover
  // the difference in time to the next (or previous) day
  double gast=jd2gast(JulianDay,ut1);
  return timeRange(gast-(longitude/15.0));
}

// passes Local Apparent Sidereal Time to stepper timer
void updateLST(double t) {
  long lst1=(t/24.0)*8640000.0;

  // set the local sidereal time
  cli(); 
  lst=lst1;
  sei();
  UT1_start=UT1;
  lst_start=lst1;
}

// convert the lst (in 1/100 second units) into floating point hours
double LST() {
  cli(); long tempLst=lst; sei();
  while (tempLst > 8640000) tempLst-=8640000;
  return (tempLst/8640000.0)*24.0;
}

double decodeTimeZone(double tz) {
  // -100 codes for :30
  if (tz < -24.0) {
    tz=tz+100.0;
    if (tz < 0) tz=tz-0.5; else tz=tz+0.5;
  }
  // +100 codes for :45
  if (tz > 24.0) {
    tz=tz-100.0;
    if (tz < 0) tz=tz-0.75; else tz=tz+0.75;
  }
  return tz;
}

double encodeTimeZone(double tz) {
  double f=fabs(frac(tz));
  // -100 codes for :30
  if (fabs(f-0.5) < 0.00000001) {
    tz=(long)tz-100.0;
  }
  // +100 codes for :45
  if (fabs(f-0.75) < 0.00000001) {
    tz=(long)tz+100.0;
  }
  tz=(long)tz;
  return tz;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Coordinate conversion

// sets latitude and associated values
void setLatitude(double Lat) {
  latitude=Lat;
  nv.writeFloat(EE_sites+currentSite*25+0,latitude);
  cosLat=cos(latitude/Rad);
  sinLat=sin(latitude/Rad);
  latitudeAbs=fabs(latitude);
  if (latitude >= 0) latitudeSign=1; else latitudeSign=-1;
  if (latitude >= 0) {
    if (axis1Settings.reverse == ON) defaultDirAxis1 = DefaultDirAxis1SCPInit; else defaultDirAxis1 = DefaultDirAxis1NCPInit;
  } else {
    if (axis1Settings.reverse == ON) defaultDirAxis1 = DefaultDirAxis1NCPInit; else defaultDirAxis1 = DefaultDirAxis1SCPInit;
  }

  // the polar home position
#if MOUNT_TYPE == ALTAZM
  homePositionAxis1=0.0;
  homePositionAxis2=0.0;
#else
  if (latitude < 0) homePositionAxis2=-90.0; else homePositionAxis2=90.0;
#endif
}

// convert equatorial coordinates to horizon
// this takes approx. 1.4mS on a 16MHz Mega2560
void equToHor(double HA, double Dec, double *Alt, double *Azm) {
  HA =HA/Rad;
  Dec=Dec/Rad;
  double cosHA=cos(HA);
  double SinAlt = (sin(Dec) * sinLat) + (cos(Dec) * cosLat * cosHA);  
  *Alt   = asin(SinAlt);
  double t1=sin(HA);
  double t2=cosHA*sinLat-tan(Dec)*cosLat;
  *Azm=atan2(t1,t2)*Rad;
  *Azm=*Azm+180.0;
  *Alt=*Alt*Rad;
}
      
// convert horizon coordinates to equatorial
// this takes approx. 1.4mS
void horToEqu(double Alt, double Azm, double *HA, double *Dec) { 
  Alt  = Alt/Rad;
  Azm  = Azm/Rad;
  double cosAzm=cos(Azm);
  double SinDec = (sin(Alt) * sinLat) + (cos(Alt) * cosLat * cosAzm);  
  *Dec = asin(SinDec); 
  double t1=sin(Azm);
  double t2=cosAzm*sinLat-tan(Alt)*cosLat;
  *HA =atan2(t1,t2)*Rad;
  *HA =*HA+180.0;
  *Dec=*Dec*Rad;
}

// returns the amount of refraction (in arcminutes) at the given true altitude (degrees), pressure (millibars), and temperature (celsius)
double trueRefrac(double Alt, double Pressure=1010.0, double Temperature=10.0) {
  double TPC=(Pressure/1010.0) * (283.0/(273.0+Temperature));
  double r=( ( 1.02*cot( (Alt+(10.3/(Alt+5.11)))/Rad ) ) ) * TPC;  if (r < 0.0) r=0.0;
  return r;
}

// returns the amount of refraction (in arcminutes) at the given apparent altitude (degrees), pressure (millibars), and temperature (celsius)
double apparentRefrac(double Alt, double Pressure=1010.0, double Temperature=10.0) {
  double r=trueRefrac(Alt,Pressure,Temperature);
  r=trueRefrac(Alt-(r/60.0),Pressure,Temperature);
  return r;
}

// converts from the "Topocentric" to "Observed"
void topocentricToObservedPlace(double *RA, double *Dec) {
  double Alt,Azm;
  double h=LST()*15.0-*RA;
  double d=*Dec;

#if TOPOCENTRIC_STRICT == ON
  // within about 1/20 arc-second of NCP
  if (fabs(d-90.0) < 0.00001) { Azm=0.0; Alt=latitude; } else 
  // within about 1/20 arc-second of SCP
  if (fabs(d+90.0) < 0.00001) { Azm=180.0; Alt=-latitude; } else equToHor(h,d,&Alt,&Azm);
#else
  // within about 1/20 arc-second of NCP or SCP, just exit
  if (fabs(d-90.0) < 0.00001 || fabs(d+90.0) < 0.00001) return; else equToHor(h,d,&Alt,&Azm);
#endif

  Alt = Alt+trueRefrac(Alt)/60.0;
  horToEqu(Alt,Azm,&h,&d);
  *RA=degRange(LST()*15.0-h); *Dec=d;
}

// converts from the "Observed" to "Topocentric"
void observedPlaceToTopocentric(double *RA, double *Dec) {
  double Alt,Azm;
  double h=LST()*15.0-*RA;
  double d=*Dec;
  
#if TOPOCENTRIC_STRICT == ON
  // within about 1/20 arc-second of the "refracted" NCP
  if (fabs(d-90.0) < 0.00001) { Azm=0.0; Alt=latitude; } else
  // within about 1/20 arc-second of the "refracted" SCP
  if (fabs(d+90.0) < 0.00001) { Azm=180.0; Alt=-latitude; } else equToHor(h,d,&Alt,&Azm);
#else  
  // within about 1/20 arc-second of NCP or SCP, just exit
  if (fabs(d-90.0) < 0.00001 || fabs(d+90.0) < 0.00001) return; else equToHor(h,d,&Alt,&Azm);
#endif

  Alt = Alt-apparentRefrac(Alt)/60.0;
  horToEqu(Alt,Azm,&h,&d);
  *RA=degRange(LST()*15.0-h); *Dec=d;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Tracking rate control

// _deltaAxis1/2 are in arc-seconds/second
double _deltaAxis1=15.0,_deltaAxis2=0.0;

bool trackingSyncInProgress() {
  static int lastTrackingSyncSeconds=0;

  // sound goto done
  if (trackingSyncSeconds == 0 && lastTrackingSyncSeconds != trackingSyncSeconds) {
    soundAlert();
    VLF("MSG: Tracking sync done");
  }

  lastTrackingSyncSeconds=trackingSyncSeconds;
  if (trackingState != TrackingSidereal) trackingSyncSeconds=0;

  return trackingSyncSeconds > 0;
}

void setDeltaTrackingRate() {
  double f1=0.0, f2=0.0;

  if (trackingSyncInProgress()) {
    trackingSyncSeconds--;
    
  #if MOUNT_TYPE == ALTAZM
    double a,z,d1,d2,newTargetAlt,newTargetAzm;
    getHor(&a,&z);
    double newTargetHA=haRange(LST()*15.0-newTargetRA);
    equToHor(newTargetHA,newTargetDec,&newTargetAlt,&newTargetAzm);
    d1=-(z-newTargetAzm);
    d2=-(a-newTargetAlt);
  #else
    double r,d,d1,d2;
    getEqu(&r,&d,false);
    d1=r-newTargetRA;
    d2=d-newTargetDec;
    if (getInstrPierSide() == PierSideEast) d2=-d2;
  #endif
    if ((fabs(d1) < arcSecPerStepAxis1/3600.0) && (fabs(d2) < arcSecPerStepAxis2/3600.0)) {
      trackingSyncSeconds=0;
    } else {
      f1=(d1*3600.0)/120.0; if (f1 < -5.0) f1=-5.0; if (f1 > 5.0) f1=5.0;
      if (fabs(d1) < arcSecPerStepAxis1/3600.0) f1=0.0;
      f2=(d2*3600.0)/120.0; if (f2 < -5.0) f2=-5.0; if (f2 > 5.0) f2=5.0;
      if (fabs(d2) < arcSecPerStepAxis2/3600.0) f2=0.0;
    }
  }

#if MOUNT_TYPE != ALTAZM
  if ((rateCompensation != RC_REFR_BOTH) && (rateCompensation != RC_FULL_BOTH)) _deltaAxis2=0.0;
#endif
  cli();
  // trackingTimerRateAxis1/2 are x the sidereal rate
  if (trackingState == TrackingSidereal) trackingTimerRateAxis1=(_deltaAxis1/15.0)+f1; else trackingTimerRateAxis1=0.0;
  if (trackingState == TrackingSidereal) trackingTimerRateAxis2=(_deltaAxis2/15.0)+f2; else trackingTimerRateAxis2=0.0;
  sei();
  fstepAxis1.fixed=doubleToFixed( ((axis1Settings.stepsPerMeasure/240.0)*(_deltaAxis1/15.0))/100.0 );
  fstepAxis2.fixed=doubleToFixed( ((axis2Settings.stepsPerMeasure/240.0)*(_deltaAxis2/15.0))/100.0 );
}

double _currentRate=1.0;
void setTrackingRate(double r) {
  _currentRate=r;
#if MOUNT_TYPE != ALTAZM
  _deltaAxis1=r*15.0;
  _deltaAxis2=0.0;
#endif
}

double getTrackingRate60Hz() {
  double f=0;
  // during slews, if tracking is enabled it's at the default sidereal rate
  if (trackingState == TrackingMoveTo && lastTrackingState == TrackingSidereal) f=1.00273790935*60.0;
#if MOUNT_TYPE == ALTAZM
    if (trackingState == TrackingSidereal) f=_currentRate*1.00273790935*60.0;
#else
    if (trackingState == TrackingSidereal) { cli(); f=(trackingTimerRateAxis1*1.00273790935)*60.0; sei(); }
#endif
  return f;
}

double getstepsPerSecondAxis1() {
 double s=((axis1Settings.stepsPerMeasure/240.0)*(_deltaAxis1/15.0));
 if (s < 8.0) s=8.0;
 return s;
}

double getstepsPerSecondAxis2() {
 double s=((axis2Settings.stepsPerMeasure/240.0)*(_deltaAxis2/15.0));
 if (s < 8.0) s=8.0;
 return s;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Low overhead altitude calculation, 16 calls to complete

bool doFastAltCalc(bool recalc) {
  bool done=false;
  
  static byte ac_step = 0;
  static double ac_HA=0,ac_Dec=0;
  static double ac_sindec,ac_cosdec,ac_cosha;
  static double ac_sinalt;

  if (recalc == true) { ac_step=0; return false; }
  
  ac_step++;
  // load HA/Dec
  if (ac_step == 1) {
    getApproxEqu(&ac_HA,&ac_Dec,true);
    currentDec=ac_Dec;
  } else
  // convert units
  if (ac_step == 2) {
    ac_HA =ac_HA/Rad;
    ac_Dec=ac_Dec/Rad;
  } else
  // prep Dec
  if (ac_step == 3) {
    ac_sindec=sin(ac_Dec);
  } else
  // prep Dec
  if (ac_step == 4) {
    ac_cosdec=cos(ac_Dec);
  } else
  // prep HA
  if (ac_step == 5) {
    ac_cosha=cos(ac_HA);
  } else
  // calc Alt, phase 1
  if (ac_step == 6) {
    ac_sinalt = (ac_sindec * sinLat) + (ac_cosdec * cosLat * ac_cosha); 
  } else
  // calc Alt, phase 2
  if (ac_step == 7) {
    currentAlt=asin(ac_sinalt)*Rad;
  } else
  // finish
  if (ac_step == 8) {
    ac_step=0;
    done=true;
  }
  return done;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Refraction adjusted tracking

// Alternate tracking rate calculation method
double ztr(double a) {
  if (a > 89.8) return 14.9998;
  if (a > 89.5) return 14.9995;

  double Alt1=a+0.25; if (Alt1 < 0.0) Alt1=0.0;
  double Alt2=a-0.25; if (Alt2 < 0.0) Alt2=0.0;

  double Alt1_ = Alt1 - ( trueRefrac(Alt1) / 60.0 );
  double Alt2_ = Alt2 - ( trueRefrac(Alt2) / 60.0 );

  double x=15.0 * ((double)(( Alt1 - Alt2 ) / ( Alt1_ - Alt2_ ))); if (x > 15.0) x=15.0;
  return x;
}

#if MOUNT_TYPE != ALTAZM

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#ifdef HAL_NO_DOUBLE_PRECISION
#define RefractionRateRange 30.0
#else
#define RefractionRateRange 1.0
#endif

bool doRefractionRateCalc() {
  bool done=false;

  static int rr_step = 0;
  static double rr_Axis1=0,rr_Axis2=0;
  static double rr_Dec=0,rr_HA=0;
  static double rr_Dec1=0,rr_HA1=0,rr_Dec2=-91,rr_HA2=0;
  static double rr_Alt,rr_Azm;

  // turn off if not tracking at sidereal rate
  if (trackingState != TrackingSidereal) { _deltaAxis1=_currentRate*15.0; _deltaAxis2=0.0; return true; }
  
  rr_step++;
  // load HA/Dec
  if (rr_step == 1) {
    if ((rateCompensation == RC_FULL_RA) || (rateCompensation == RC_FULL_BOTH)) getEqu(&rr_Axis1,&rr_Axis2,true); else getApproxEqu(&rr_Axis1,&rr_Axis2,true);
  } else

  // convert units,  get ahead of and behind current position
  if ((rr_step == 5) || (rr_step == 105)) {
    rr_HA =rr_Axis1;
    rr_Dec=rr_Axis2;
    if (rr_step == 5)   rr_HA =rr_HA-(RefractionRateRange/60.0);
    if (rr_step == 105) rr_HA =rr_HA+(RefractionRateRange/60.0);
  } else

  // get the instrument coordinates
  if ((rr_step == 10) || (rr_step == 110)) {
    if ((rateCompensation == RC_FULL_RA) || (rateCompensation == RC_FULL_BOTH)) {
      Align.equToInstr(rr_HA,rr_Dec,&rr_HA,&rr_Dec,getInstrPierSide());
    }
  }

  // get the Horizon coords
  if ((rr_step == 15) || (rr_step == 115)) {
    equToHor(rr_HA,rr_Dec,&rr_Alt,&rr_Azm);
  } else

  // apply refraction
  if ((rr_step == 20) || (rr_step == 120)) {
    rr_Alt+=apparentRefrac(rr_Alt,ambient.getPressure(),ambient.getTemperature())/60.0;
  } else

  // convert back to the Equtorial coords
  if ((rr_step == 25) || (rr_step == 125)) {
    horToEqu(rr_Alt,rr_Azm,&rr_HA1,&rr_Dec1);
    if (rr_HA1 > 180.0) rr_HA1-=360.0; // HA range +/-180
  } else

  // calculate refraction rate deltas'
  if ((rr_step == 30) || (rr_step == 130)) {
    // store first calc
    if (rr_step == 30) { rr_HA2=rr_HA1; rr_Dec2=rr_Dec1; }
    // we have both -0.5hr and +0.5hr values 
    if (rr_step == 130) {
      // set rates
      // handle coordinate wrap
      if ((rr_HA1 < -90.0) && (rr_HA2 > 90.0)) rr_HA1+=360.0;
      if ((rr_HA2 < -90.0) && (rr_HA1 > 90.0)) rr_HA2+=360.0;

      // set rates
      double dax1=(rr_HA1-rr_HA2)*(15.0/(RefractionRateRange/60.0))/2.0;
      if (fabs(_deltaAxis1-dax1) > 0.005) _deltaAxis1=dax1; else _deltaAxis1=(_deltaAxis1*9.0+dax1)/10.0;
      double dax2;
      if (getInstrPierSide() == PierSideWest) {
        dax2=(rr_Dec2-rr_Dec1)*(15.0/(RefractionRateRange/60.0))/2.0;
      } else {
        dax2=(rr_Dec1-rr_Dec2)*(15.0/(RefractionRateRange/60.0))/2.0;
      }
      if (fabs(_deltaAxis2-dax2) > 0.005) _deltaAxis2=dax2; else _deltaAxis2=(_deltaAxis2*9.0+dax2)/10.0;
      
      // override for special case of near a celestial pole
      if (90.0-fabs(rr_Dec) < (1.0/3600.0)) { _deltaAxis1=_currentRate*15.0; _deltaAxis2=0.0; }

      // override for special case of near the zenith
      if (currentAlt > 85.0) { _deltaAxis1=ztr(currentAlt); _deltaAxis2=0.0; }
    }
  } else

  // finish once every 200 calls
  if (rr_step == 200) {
    rr_step=0;
    done=true;
  }
  return done;
}

#endif

// -----------------------------------------------------------------------------------------------------------------------------
// AltAz tracking

#if MOUNT_TYPE == ALTAZM

#define AltAzTrackingRange 5  // distance in arc-min (10) ahead of and behind the current Equ position, used for rate calculation

bool doHorRateCalc() {
  bool done=false;

  static int az_step=0;
  static double az_Axis1=0,az_Axis2=0;
  static double az_Dec=0,az_HA=0;
  static double az_Dec1=0,az_HA1=0;
  static double az_Alt,az_Alt1,az_Alt2;
  static double az_Azm,az_Azm1,az_Azm2;

  // turn off if not tracking at sidereal rate
  if (((trackingState != TrackingSidereal) && (trackingState != TrackingMoveTo))) { _deltaAxis1=0.0; _deltaAxis2=0.0; return true; }

  az_step++;
  // convert units, get ahead of and behind current position
  if (az_step == 1) {
    if (trackingState == TrackingMoveTo) {
      cli();
      az_Axis1=targetAxis1.part.m+indexAxis1Steps;
      az_Axis2=targetAxis2.part.m+indexAxis2Steps;
      sei();
    } else {
      cli();
      az_Axis1=posAxis1+indexAxis1Steps;
      az_Axis2=posAxis2+indexAxis2Steps;
      sei();
    }
    // get the Azm
    az_Azm=(double)az_Axis1/axis1Settings.stepsPerMeasure;
    // get the Alt
    az_Alt=(double)az_Axis2/axis2Settings.stepsPerMeasure; 
  } else

  // convert to Equatorial coords
  if ((az_step == 5)) {
    horToEqu(az_Alt,az_Azm,&az_HA1,&az_Dec1);
  } else

  // look ahead of and behind the current position
  if ((az_step == 10) || (az_step == 110)) {
    if (az_step == 10 ) az_HA =(az_HA1-(AltAzTrackingRange/60.0));
    if (az_step == 110) az_HA =(az_HA1+(AltAzTrackingRange/60.0));
    az_Dec=az_Dec1;
  } else

  // each back to the Horizon coords
  if ((az_step == 15) || (az_step == 115)) {
    equToHor(az_HA,az_Dec,&az_Alt,&az_Azm);
    if (az_Azm > 180.0)  az_Azm-=360.0;
    if (az_Azm < -180.0) az_Azm+=360.0;

    if (az_step == 15)  { 
      az_Alt2=az_Alt;
      az_Azm2=az_Azm;
    }
    if (az_step == 115) { 
      az_Alt1=az_Alt;
      az_Azm1=az_Azm;
    }
  } else
  
  // calculate tracking rate deltas'
  if ((az_step == 20) || (az_step == 120)) {
    // we have both -0.5hr and +0.5hr values 
    if (az_step == 120) {
      // handle coordinate wrap
      if ((az_Azm1 < -90.0) && (az_Azm2 > 90.0)) az_Azm1+=360.0;
      if ((az_Azm2 < -90.0) && (az_Azm1 > 90.0)) az_Azm2+=360.0;
      
      // set rates
      _deltaAxis1=((az_Azm1-az_Azm2)*(15.0/(AltAzTrackingRange/60.0))/2.0)*_currentRate;
      _deltaAxis2=((az_Alt1-az_Alt2)*(15.0/(AltAzTrackingRange/60.0))/2.0)*_currentRate; 
      
      // override for special case of near a celestial pole
      if (90.0-fabs(az_Dec) <= 0.5) { _deltaAxis1=0.0; _deltaAxis2=0.0; }
    }
  } else

  // finish once every 200 calls
  if (az_step == 200) {
    az_step=0;
    done=true;
  }
  return done;
}
#endif

// -----------------------------------------------------------------------------------------------------------------------------
// Acceleration rate calculation
void setAccelerationRates(double maxRate) {
  
  // set the new guide acceleration rate
  slewRateX  = (RateToXPerSec/(maxRate/16.0))*7.0;                // 7x for exponential factor average rate
  slewRateX = slewRateX*((maxRateBaseActual/2.0)/(maxRate/16.0)); // scale with maxRate so SLEW_ACCELERATION_DIST and SLEW_RAPID_STOP_DIST are approximately correct
  accXPerSec = slewRateX/SLEW_ACCELERATION_DIST;
  guideRates[9]=RateToASPerSec/(maxRate/16.0); guideRates[8]=guideRates[9]/2.0;
  activeGuideRate=GuideRateNone;
  
  // set the new goto acceleration rate
  cli();
  stepsForRateChangeAxis1= (sqrt((double)SLEW_ACCELERATION_DIST*axis1Settings.stepsPerMeasure))*maxRate;
  stepsForRateChangeAxis2= (sqrt((double)SLEW_ACCELERATION_DIST*axis2Settings.stepsPerMeasure))*maxRate;
  sei();

  // slewSpeed is in degrees per second
  slewSpeed=(1000000.0/(maxRate/16.0))/axis1Settings.stepsPerMeasure;
}
