// -----------------------------------------------------------------------------------------------------------------------------
// Astronomy related functions

// convert string in format MM/DD/YY to julian date
boolean dateToDouble(double *JulianDay, char *date) {
  char m[3],d[3],y[3];
  int  m1,d1,y1;
  
  if (strlen(date)!= 8) return false;

  m[0]=*date++; m[1]=*date++; m[2]=0; atoi2(m,&m1);
  if (*date++!='/') return false; d[0]=*date++; d[1]=*date++; d[2]=0; atoi2(d,&d1);
  if (*date++!='/') return false; y[0]=*date++; y[1]=*date++; y[2]=0; atoi2(y,&y1);
  if ((m1<1) || (m1>12) || (d1<1) || (d1>31) || (y1<0) || (y1>99)) return false;
  if (y1>11) y1=y1+2000; else y1=y1+2100;
  
  *JulianDay=julian(y1,m1,d1);
  return true;
}

// convert string in format HH:MM:SS to floating point
// (also handles)           HH:MM.M
boolean hmsToDouble(double *f, char *hms) {
  char h[3],m[5],s[3];
  int  h1,m1,m2=0,s1=0;
  
  while (*hms==' ') hms++; // strip prefix white-space

  if (highPrecision) { if (strlen(hms)!= 8) return false; } else if (strlen(hms)!= 7) return false;

  h[0]=*hms++; h[1]=*hms++; h[2]=0; atoi2(h,&h1);
  if (highPrecision) {
    if (*hms++!=':') return false; m[0]=*hms++; m[1]=*hms++; m[2]=0; atoi2(m,&m1);
    if (*hms++!=':') return false; s[0]=*hms++; s[1]=*hms++; s[2]=0; atoi2(s,&s1);
  } else {
    if (*hms++!=':') return false; m[0]=*hms++; m[1]=*hms++; m[2]=0; atoi2(m,&m1);
    if (*hms++!='.') return false; m2=(*hms++)-'0';
  }
  if ((h1<0) || (h1>23) || (m1<0) || (m1>59) || (m2<0) || (m2>9) || (s1<0) || (s1>59)) return false;
  
  *f=h1+m1/60.0+m2/600.0+s1/3600.0;
  return true;
}

boolean doubleToHms(char *reply, double *f) {
  double h1,m1,f1,s1;

  f1=fabs(*f)+0.00001; // keep from rounding down in seconds

  h1=floor(f1);
  m1=(f1-h1)*60;
  s1=(m1-floor(m1));

  char s[]="%s%02d:%02d:%02d";
  if (highPrecision) {
    s1=s1*60;
  } else {
    s1=s1*10;
    s[11]='.'; s[14]='1';
  }
  char sign[2]="";
  if (((s1!=0) || (m1!=0) || (h1!=0)) && (*f<0.0)) strcpy(sign,"-");
  sprintf(reply,s,sign,(int)h1,(int)m1,(int)s1);

  return true;
}

// convert string in format sDD:MM:SS to floating point
// (also handles)           DDD:MM:SS
//                          sDD:MM
//                          DDD:MM
//                          sDD*MM
//                          DDD*MM
boolean dmsToDouble(double *f, char *dms, boolean sign_present) {
  char d[4],m[5],s[3];
  int d1, m1, s1=0;
  int lowLimit=0, highLimit=360;
  int checkLen,checkLen1;
  double sign = 1.0;
  boolean secondsOff = false;

  while (*dms==' ') dms++; // strip prefix white-space

  checkLen1=strlen(dms);

  // determine if the seconds field was used and accept it if so
  if (highPrecision) { 
    checkLen=9;
    if (checkLen1 != checkLen) return false;
  } else {
    checkLen=6;
    if (checkLen1 != checkLen) {
      if (checkLen1==9) { secondsOff=false; checkLen=9; } else return false;
    } else secondsOff = true;
  }

  // determine if the sign was used and accept it if so
  if (sign_present) {
    if (*dms=='-') sign=-1.0; else if (*dms=='+') sign=1.0; else return false; 
    dms++; d[0]=*dms++; d[1]=*dms++; d[2]=0; if (!atoi2(d,&d1)) return false;
  } else {
    d[0]=*dms++; d[1]=*dms++; d[2]=*dms++; d[3]=0; if (!atoi2(d,&d1)) return false;
  }

  // make sure the seperator is an allowed character
  if ((*dms!=':') && (*dms!='*') && (*dms!=char(223))) return false; else dms++;

  m[0]=*dms++; m[1]=*dms++; m[2]=0; if (!atoi2(m,&m1)) return false;

  if ((highPrecision) && (!secondsOff)) {
    // make sure the seperator is an allowed character
    if (*dms++!=':') return false; 
    s[0]=*dms++; s[1]=*dms++; s[2]=0; atoi2(s,&s1);
  }

  if (sign_present) { lowLimit=-90; highLimit=90; }
  if ((d1<lowLimit) || (d1>highLimit) || (m1<0) || (m1>59) || (s1<0) || (s1>59)) return false;
  
  *f=sign*(d1+m1/60.0+s1/3600.0);
  return true;
}

boolean doubleToDms(char *reply, double *f, boolean fullRange, boolean signPresent) {
  char sign[]="+";
  int  o=0,d1,s1=0;
  double m1,f1;
  f1=*f;

  // setup formatting, handle adding the sign
  if (f1<0) { f1=-f1; sign[0]='-'; }

  f1=f1+0.00001; // add a little tiny bit to keep from rounding down in seconds
  d1=floor(f1);
  m1=(f1-d1)*60.0;
  s1=(m1-floor(m1))*60.0;
  
  char s[]="+%02d*%02d:%02d";
  if (signPresent) { 
    if (sign[0]=='-') { s[0]='-'; } o=1;
  } else {
    strcpy(s,"%02d*%02d:%02d");
  }
  if (fullRange) s[2+o]='3';
 

  if (highPrecision) {
    sprintf(reply,s,d1,(int)m1,s1);
  } else {
    s[9+o]=0;
    sprintf(reply,s,d1,(int)m1);
  }
  return true;
}

double frac(double v) {
  return v - ((long)v);
}

double decodeTimeZone(double tz) {
  // -100 codes for :30
  if (tz<-24.0) {
    tz=tz+100.0;
    if (tz<0) tz=tz-0.5; else tz=tz+0.5;
  }
  // +100 codes for :45
  if (tz>24.0) {
    tz=tz-100.0;
    if (tz<0) tz=tz-0.75; else tz=tz+0.75;
  }
  return tz;
}

double encodeTimeZone(double tz) {
  double f=fabs(frac(tz));
  // -100 codes for :30
  if (fabs(f-0.5)<0.00000001) {
    tz=(long)tz-100.0;
  }
  // +100 codes for :45
  if (fabs(f-0.75)<0.00000001) {
    tz=(long)tz+100.0;
  }
  tz=(long)tz;
  return tz;
}

void timeZoneToHM(char *reply, double tz) {
  double f=fabs(frac(tz));
  sprintf(reply,"%+03d",(int)tz);
  // append for :30
  if (fabs(f-0.5)<0.00000001) {
    strcat(reply,":30");
  }
  // append for :45
  if (fabs(f-0.75)<0.00000001) {
    strcat(reply,":45");
  }
}

// integer numeric conversion with error checking
boolean atoi2(char *a, int *i) {
  char *conv_end;
  long l=strtol(a,&conv_end,10);
  
  if ((l<-32767) || (l>32768) || (&a[0]==conv_end)) return false;
  *i=l;
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Coordinate conversion

// takes the topocentric refracted coordinates and applies corrections to arrive at instrument equatorial coordinates 
boolean EquToIEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) {
  if (Dec>90.0) Dec=90.0;
  if (Dec<-90.0) Dec=-90.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double dSin=sin(d);
    double dCos=cos(d);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh = doCor*(1.0/dCos)*p;

    // ------------------------------------------------------------
    // misalignment due to Dec axis being perp. to RA axis
    double PDh =-pdCor*(dSin/dCos)*p;

    // polar misalignment
    double h1=-azmCor*cos(h)*(dSin/dCos) + altCor*sin(h)*(dSin/dCos);
    double d1=+azmCor*sin(h)             + altCor*cos(h);
    *HA1 =HA +(h1+PDh+DOh);
    *Dec1=Dec+ d1;
  } else {
    // just ignore the the correction if right on the pole
    *HA1 =HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  
#ifndef MOUNT_TYPE_ALTAZM
  // switch to under the pole coordinates
  if ((Lat>=0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0))) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(90.0-*Dec1)+90.0;
  }
  if ((Lat<0) && ((abs(*HA1)>(double)UnderPoleLimit*15.0) )) {
    *HA1 =*HA1-180.0; while (*HA1<-180.0) *HA1=*HA1+360.0;
    *Dec1=(-90.0-*Dec1)-90.0;
  }
#endif

  // finally, apply the index offsets
  *HA1-=IH; *Dec1-=ID;

  return true;
}

// takes the instrument equatorial coordinates and applies corrections to arrive at topocentric refracted coordinates
boolean IEquToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  // remove the index offsets
  HA+=IH; Dec+=ID;

#ifndef MOUNT_TYPE_ALTAZM
  // switch from under the pole coordinates
  if (Dec>90.0) { Dec=(90.0-Dec)+90; HA=HA-180.0; }
  if (Dec<-90.0) { Dec=(-90.0-Dec)-90.0; HA=HA-180.0; }
#endif
  while (HA>180.0) HA-=360.0;
  while (HA<-180.0) HA+=360.0;

  // breaks-down near the pole (limited to >1' from pole)
  if (abs(Dec)<89.98333333) {
    double h =HA/Rad;
    double d =Dec/Rad;
    double dSin=sin(d);
    double dCos=cos(d);
    double p=1.0; if (pierSide==PierSideWest) p=-1.0;

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Dec axis
    // negative numbers are further (S) from the NCP, swing to the
    // equator and the effect on declination is 0. At the SCP it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Polar axis misalignment
    double DOh = doCor*(1.0/dCos)*p;

    // as the above offset becomes zero near the equator, the affect
    // works on HA instead.  meridian flips effect this in HA
    double PDh =-pdCor*(dSin/dCos)*p;

    // ------------------------------------------------------------
    // polar misalignment
    double h1=-azmCor*cos(h)*(dSin/dCos) + altCor*sin(h)*(dSin/dCos);
    double d1=+azmCor*sin(h)             + altCor*cos(h);
    *HA1 =HA -(h1+PDh+DOh);
    *Dec1=Dec- d1;
  } else {
    // just ignore the the correction if right on the pole
    *HA1=HA;
    *Dec1=Dec;
  }

  while (*HA1>180.0) *HA1-=360.0;
  while (*HA1<-180.0) *HA1+=360.0;
  if (*Dec1>90.0) *Dec1=90.0;
  if (*Dec1<-90.0) *Dec1=-90.0;
  return true;
}

// convert equatorial coordinates to horizon
// this takes approx. 1.4mS on a 16MHz Mega2560
void EquToHor(double HA, double Dec, double *Alt, double *Azm) {
  while (HA<0.0)    HA=HA+360.0;
  while (HA>=360.0) HA=HA-360.0;
  HA =HA/Rad;
  Dec=Dec/Rad;
  double SinAlt = (sin(Dec) * sinLat) + (cos(Dec) * cosLat * cos(HA));  
  *Alt   = asin(SinAlt);
  double t1=sin(HA);
  double t2=cos(HA)*sinLat-tan(Dec)*cosLat;
  *Azm=atan2(t1,t2)*Rad;
  *Azm=*Azm+180.0;
  *Alt = *Alt*Rad;
}
      
// convert horizon coordinates to equatorial
// this takes approx. 1.4mS
void HorToEqu(double Alt, double Azm, double *HA, double *Dec) { 
  while (Azm<0)      Azm=Azm+360.0;
  while (Azm>=360.0) Azm=Azm-360.0;
  Alt  = Alt/Rad;
  Azm  = Azm/Rad;
  double SinDec = (sin(Alt) * sinLat) + (cos(Alt) * cosLat * cos(Azm));  
  *Dec = asin(SinDec); 
  double t1=sin(Azm);
  double t2=cos(Azm)*sinLat-tan(Alt)*cosLat;
  *HA=atan2(t1,t2)*Rad;
  *HA=*HA+180.0;
  *Dec = *Dec*Rad;
}

double cot(double n) {
  return 1.0/tan(n);
}

// -----------------------------------------------------------------------------------------------------------------------------
// Refraction rate tracking

int az_step = 0;
double az_Axis1=0,az_Axis2=0;
double az_Dec=0,az_HA=0;
double az_Dec1=0,az_HA1=0,az_Dec2=-91,az_HA2=0;
double az_Alt,az_Azm,_az_Alt;
double az_deltaAxis1=15.0,az_deltaAxis2=0.0;

// az_deltaH/D are in arc-seconds/second
// trackingTimerRateAxis1/2 are x the sidereal rate
void SetDeltaTrackingRate() {
  trackingTimerRateAxis1 = az_deltaAxis1/15.0;
  trackingTimerRateAxis2 = az_deltaAxis2/15.0;
  fstepAxis1.fixed=doubleToFixed( (((double)StepsPerDegreeAxis1/240.0)*trackingTimerRateAxis1)/100.0 );
  fstepAxis2.fixed=doubleToFixed( (((double)StepsPerDegreeAxis2/240.0)*trackingTimerRateAxis2)/100.0 );
}

void SetTrackingRate(double r) {
  az_deltaAxis1=r*15.0;
  az_deltaAxis2=0.0;
}

// returns the amount of refraction (in arcminutes) at given altitude (degrees), pressure (millibars), and temperature (celsius)
double Refrac(double Alt, double Pressure=1010.0, double Temperature=15.0) {
  double TPC=(Pressure/1010.0) * (283.0/(273.0+Temperature));
  double r=( ( 1.02*cot( (Alt+(10.3/(Alt+5.11)))/Rad ) ) ) * TPC;  if (r<0.0) r=0.0;
  return r;
}

// Alternate tracking rate calculation method
double ZenithTrackingRate() {
  double Alt1=currentAlt+0.5; if (Alt1<0.0) Alt1=0.0;
  double Alt2=currentAlt-0.5; if (Alt2<0.0) Alt2=0.0;
  if (currentAlt>89.8) return 15.0;
  if (currentAlt>89.5) return 14.998;

  double Alt1_ = Alt1 - ( Refrac(Alt1) / 60.0 );
  double Alt2_ = Alt2 - ( Refrac(Alt2) / 60.0 );
  
  return 15.0 * ((double)(( Alt1 - Alt2 ) / ( Alt1_ - Alt2_ )));
}

// low overhead altitude calculation, 16 calls to complete
byte ac_step = 0;
double ac_HA=0,ac_De=0,ac_Dec=0;
double ac_sindec,ac_cosdec,ac_cosha;
double ac_sinalt;

double getApproxDec() {
  return ac_De;
}

boolean do_fastalt_calc() {
  boolean done=false;
  ac_step++;
  // load HA/Dec
  if (ac_step==1) {
    getApproxEqu(&ac_HA,&ac_De,true);
    ac_Dec=ac_De;
  } else
  // convert units
  if (ac_step==2) {
    ac_HA =ac_HA/Rad;
    ac_Dec=ac_Dec/Rad;
  } else
  // prep Dec
  if (ac_step==3) {
    ac_sindec=sin(ac_Dec);
  } else
  // prep Dec
  if (ac_step==4) {
    ac_cosdec=cos(ac_Dec);
  } else
  // prep HA
  if (ac_step==5) {
    ac_cosha=cos(ac_HA);
  } else
  // calc Alt, phase 1
  if (ac_step==6) {
    ac_sinalt = (ac_sindec * sinLat) + (ac_cosdec * cosLat * ac_cosha); 
  } else
  // calc Alt, phase 2
  if (ac_step==7) {
    currentAlt=asin(ac_sinalt)*Rad;
  } else
  // finish
  if (ac_step==8) {
    ac_step=0;
    done=true;
  }
  return done;
}

// low overhead refraction rate calculation, 200 calls to complete

#define RefractionRateRange 10  // distance in arc-min (20) ahead of and behind the current Equ position, used for rate calculation

boolean do_refractionRate_calc() {
  boolean done=false;

  // turn off if not tracking at sidereal rate  
  if (trackingState!=TrackingSidereal) { az_deltaAxis1=15.0; az_deltaAxis2=0.0; return true; }
  
  az_step++;
  // load HA/Dec
  if (az_step==1) {
    if (onTrack)
      getEqu(&az_Axis1,&az_Axis2,true);
    else
      getApproxEqu(&az_Axis1,&az_Axis2,true);
  } else

  // convert units,  get ahead of and behind current position
  if ((az_step==5) || (az_step==105)) {
    az_Dec=az_Axis2;
    az_HA =az_Axis1;
    if (az_step==5)   az_HA =az_HA-(RefractionRateRange/60.0);
    if (az_step==105) az_HA =az_HA+(RefractionRateRange/60.0);
  } else
  
  // get the Horizon coords
  if ((az_step==10) || (az_step==110)) {
    if (onTrack) EquToIEqu(latitude,az_HA,az_Dec,&az_HA,&az_Dec);
  }

  // get the Horizon coords
  if ((az_step==15) || (az_step==115)) {
    EquToHor(az_HA,az_Dec,&az_Alt,&az_Azm);
  } else

  // apply refraction
  if ((az_step==20) || (az_step==120)) {
    az_Alt+=Refrac(az_Alt)/60.0;
  } else

  // convert back to the Equtorial coords
  if ((az_step==25) || (az_step==125)) {
    HorToEqu(az_Alt,az_Azm,&az_HA1,&az_Dec1);
    if (az_HA1>180.0) az_HA1-=360.0; // HA range +/-180
  } else

  // calculate refraction rate deltas'
  if ((az_step==30) || (az_step==130)) {
    // store first calc
    if (az_step==30) { az_HA2=az_HA1; az_Dec2=az_Dec1; }
    // we have both -0.5hr and +0.5hr values 
    if (az_step==130) {
      // set rates
      // handle coordinate wrap
      if ((az_HA1<-90.0) && (az_HA2>90.0)) az_HA1+=360.0;

      // set rates
      az_deltaAxis1=(az_HA1-az_HA2)  *(15.0/(RefractionRateRange/60.0))/2.0;
      az_deltaAxis2=(az_Dec1-az_Dec2)*(15.0/(RefractionRateRange/60.0))/2.0;
      
      // override for special case of near a celestial pole
      if (90.0-fabs(az_Dec)<(1.0/3600.0)) { az_deltaAxis1=15.0; az_deltaAxis2=0.0; }
      // override for special case of near the zenith
      if (currentAlt>(90.0-7.5)) {
        az_deltaAxis1=ZenithTrackingRate();
        az_deltaAxis2=0.0;
      }
    }
  } else

  // finish once every 200 calls
  if (az_step==200) {
    az_step=0;
    done=true;
  }
  return done;
}

// -----------------------------------------------------------------------------------------------------------------------------
// AltAz tracking

#ifdef MOUNT_TYPE_ALTAZM

#define AltAzTrackingRange 10  // distance in arc-min (20) ahead of and behind the current Equ position, used for rate calculation
double az_Alt1,az_Alt2,az_Azm1,az_Azm2;

boolean do_altAzmRate_calc() {
  boolean done=false;

  // turn off if not tracking at sidereal rate
  if (((trackingState!=TrackingSidereal) && (trackingState!=TrackingMoveTo))) { az_deltaAxis1=0.0; az_deltaAxis2=0.0; return true; }

  az_step++;
  // convert units, get ahead of and behind current position
  if (az_step==1) {
    if (trackingState==TrackingMoveTo) {
      cli();
      az_Axis1=targetAxis1.part.m+IHS;
      az_Axis2=targetAxis2.part.m+IDS;
      sei();
    } else {
      cli();
      az_Axis1=posAxis1+IHS;
      az_Axis2=posAxis2+IDS;
      sei();
    }
    // get the Azm
    az_Azm=(double)az_Axis1/(double)StepsPerDegreeAxis1;
    // get the Alt
    az_Alt=(double)az_Axis2/(double)StepsPerDegreeAxis2; 
  } else

  // convert to Equatorial coords
  if ((az_step==5)) {
    HorToEqu(az_Alt,az_Azm,&az_HA1,&az_Dec1);
  } else

  // look ahead of and behind the current position
  if ((az_step==10) || (az_step==110)) {
    if (az_step==10 ) az_HA =(az_HA1-(AltAzTrackingRange/60.0));
    if (az_step==110) az_HA =(az_HA1+(AltAzTrackingRange/60.0));
    az_Dec=az_Dec1;
  } else

  // each back to the Horizon coords
  if ((az_step==15) || (az_step==115)) {
    EquToHor(az_HA,az_Dec,&az_Alt,&az_Azm);
    if (az_Azm>180.0)  az_Azm-=360.0;
    if (az_Azm<-180.0) az_Azm+=360.0;

    if (az_step==15)  { 
      az_Alt2=az_Alt;
      az_Azm2=az_Azm;
    }
    if (az_step==115) { 
      az_Alt1=az_Alt;
      az_Azm1=az_Azm;
    }
  } else
  
  // calculate tracking rate deltas'
  if ((az_step==20) || (az_step==120)) {
    // we have both -0.5hr and +0.5hr values 
    if (az_step==120) {
      // handle coordinate wrap
      if ((az_Azm1<-90.0) && (az_Azm2>90.0)) az_Azm1+=360.0;
      if ((az_Azm2<-90.0) && (az_Azm1>90.0)) az_Azm2+=360.0;
      
      // set rates
      az_deltaAxis1=(az_Azm1-az_Azm2)*(15.0/(AltAzTrackingRange/60.0))/2.0;
      az_deltaAxis2=(az_Alt1-az_Alt2)*(15.0/(AltAzTrackingRange/60.0))/2.0; 
      
      // override for special case of near a celestial pole
      if (90.0-fabs(az_Dec)<=0.5) { az_deltaAxis1=0.0; az_deltaAxis2=0.0; }
    }
  } else

  // finish once every 200 calls
  if (az_step==200) {
    az_step=0;
    done=true;
  }
  return done;
}
#endif

// -----------------------------------------------------------------------------------------------------------------------------
// Date Time conversion

// converts Gregorian date (Y,M,D) to Julian day number
double julian(int Year, int Month, int Day) {
  if ((Month==1) || (Month==2)) { Year--; Month=Month+12; }
  double B=2.0-floor(Year/100.0)+floor(Year/400.0);
  return (B+floor(365.25*Year)+floor(30.6001*(Month+1.0))+Day+1720994.5); //+(Time/24.0);
}

// converts Julian day number to Gregorian date (Y,M,D)
void greg(double JulianDay, int *Year, int *Month, int *Day) {
  double A,B,C,D,D1,E,F,G,I;

  JulianDay=JulianDay+0.5;
  I=floor(JulianDay);
 
  F=0.0; //  JD-I;
  if (I>2299160.0) {
    A=int((I-1867216.25)/36524.25);
    B=I+1.0+A-floor(A/4.0);
  } else B=I;

  C=B+1524.0;
  D=floor((C-122.1)/365.25);
  E=floor(365.25*D);
  G=floor((C-E)/30.6001);

  D1=C-E+F-floor(30.6001*G);
  *Day=floor(D1);
  if (G<13.5)     *Month=floor(G-1.0);   else *Month=floor(G-13.0);
  if (*Month>2.5) *Year=floor(D-4716.0); else *Year=floor(D-4715.0);
}

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
// uses longitude
double jd2last(double JulianDay, double ut1) {
  // JulianDay is the Local date, jd2gast requires a universal time
  // this is a hack that leaves the date alone and lets the UT1 cover
  // the difference in time to the next (or previous) day
  double gast=jd2gast(JulianDay,ut1);
  return timeRange(gast-(longitude/15.0));
}

// passes Local Apparent Sidereal Time to stepper timer
void update_lst(double t) {
  long lst1=(t/24.0)*8640000.0;

  // set the local sidereal time
  cli(); 
  lst=lst1;
  sei();
}

// convert the lst (in 1/100 second units) into floating point hours
double LST() {
  cli(); long tempLst=lst; sei();
  while (tempLst>8640000) tempLst-=8640000;
  return (tempLst/8640000.0)*24.0;
}

// -----------------------------------------------------------------------------------------------------------------------------
// Misc. numeric conversion

double timeRange(double t) {
  while (t>=24.0) t-=24.0;
  while (t<  0.0) t+=24.0;
  return t;
}

double haRange(double d) {
  while (d>=180.0) d-=360.0;
  while (d<-180.0) d+=360.0;
  return d;
}

double degRange(double d) {
  while (d>=360.0) d-=360.0;
  while (d<  0.0)  d+=360.0;
  return d;
}

double dist(double a, double b) {
  if (a>b) return a-b; else return b-a;
}

// floating point range of +/-255.999999x
uint64_t doubleToFixed(double d) {
  fixed_t x;
  x.fixed = (long)(d*8388608.0);  // shift 23 bits
  x.fixed = x.fixed<<9;
  return x.fixed;
}

// floating point range of +/-255.999999x
double fixedToDouble(fixed_t a) {
  long l = a.fixed>>9;          // shift 9 bits
  return ((double)l/8388608.0); // and 23 more, for 32 bits total
}

// -----------------------------------------------------------------------------------------------------------------------------
// Align

bool startAlign(char c) {
  // :A2 and :A3 arent supported with Fork mounts in alternate mode
  #if defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
  if (c=='1') {
  #endif

  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets IH, ID, azmCor=0; altCor=0;
  setHome();
  
  // enable the stepper drivers
  digitalWrite(Axis1_EN,Axis1_Enabled);
  digitalWrite(Axis2_EN,Axis2_Enabled);
  delay(10);

  // start tracking
  trackingState=TrackingSidereal;

  // start align... AlignOneStar1=01, AlignTwoStar1=11, AlignThreeStar1=21
  alignMode=AlignOneStar1+(c-'1')*10;

  return false;

  #if defined(MOUNT_TYPE_FORK_ALT) || defined(MOUNT_TYPE_ALTAZM)
  } else return true;
  #endif
}

/*
Alignment Logic:
Near the celestial equator (Dec=0, HA=0)...
the azmCor term is 0 in Dec
the altCor term is 1 in Dec
the doCor  term is 1 in HA
the pdCor  term is 0 in HA

Near HA=6 and Dec=45...
the azmCor term is 1 in Dec
the altCor term is 0 in Dec
the doCor  term is 0 in HA
the pdCor  term is 1 in HA
*/

// for align
double avgDec = 0.0;
double avgHA  = 0.0;
bool nextAlign() {
  // First star:
  // Near the celestial equator (Dec=0, HA=0), telescope West of the pier if multi-star align
  if ((alignMode==AlignOneStar1) || (alignMode==AlignTwoStar1) || (alignMode==AlignThreeStar1)) {
    if ((alignMode==AlignOneStar1) && (meridianFlip==MeridianFlipAlign)) meridianFlip=MeridianFlipNever;
    alignMode++;
    // set the IH offset
    // set the ID offset
    if (!syncEqu(newTargetRA,newTargetDec)) { return true; }
    avgDec=newTargetDec;
    avgHA =haRange(LST()*15.0-newTargetRA);
  } else 
  // Second star:
  // Near the celestial equator (Dec=0, HA=0), telescope East of the pier
  if ((alignMode==AlignTwoStar2) || (alignMode==AlignThreeStar2)) {
    if ((alignMode==AlignTwoStar2) && (meridianFlip==MeridianFlipAlign)) meridianFlip=MeridianFlipNever;
    alignMode++;
    double IH1=IH;
    double ID1=-ID;

    avgDec=(avgDec+newTargetDec)/2.0;
    avgHA =(-avgHA+haRange(LST()*15.0-newTargetRA))/2.0; // last HA is negative because we were on the other side of the meridian
    if (syncEqu(newTargetRA,newTargetDec)) {
      double IH2=IH;
      double ID2=ID;

      IH  = (IH2+IH1)/2.0;                      // average offset in HA
      IHS = (long)(IH*(double)StepsPerDegreeAxis1);
      ID  = (ID2-ID1)/2.0;                      // new offset in Dec
      IDS = (long)(ID*(double)StepsPerDegreeAxis2);

      double IH3=IH;
      double ID3=ID;
    
      altCor=-(ID2+ID1)/2.0;                    // Negative when pointed below the pole
      altCor= altCor/cos(avgHA/Rad);            // correct for measurements being away from the Meridian

      // allow the altCor to be applied
      if (syncEqu(newTargetRA,newTargetDec)) {
        IH2=IH;
        ID2=ID;

        doCor =-(IH2-IH1)/2.0;                  // the difference of these two values should be a decent approximation of the optical axis to Dec axis error (aka cone error)
        doCor = doCor*cos(avgDec/Rad);          // correct for measurement being away from the Celestial Equator

        IH  = IH3;
        IHS = (long)(IH*(double)StepsPerDegreeAxis1);
        ID  = ID3;
        IDS = (long)(ID*(double)StepsPerDegreeAxis2);
      } else return true;
    } else return true;
  } else 
  // Third star:
  // Near (Dec=45, HA=6), telescope East of the pier
  if (alignMode==AlignThreeStar3) {
    #ifndef MOUNT_TYPE_GEM
    meridianFlip=MeridianFlipNever;
    #endif
    alignMode++;
    
    double IH1=IH;
    double ID1=ID;
    if (syncEqu(newTargetRA,newTargetDec)) {
      double IH2=IH;
      double ID2=ID;

      azmCor = -(ID2-ID1);                                        // offset in declination is largely due to polar align Azm error
      azmCor = azmCor/sin(haRange(LST()*15.0-newTargetRA)/Rad);   // correct for HA of measurement location

      // allow the azmCor to be applied
      if (syncEqu(newTargetRA,newTargetDec)) {
        IH2=IH;
        ID2=ID;
        // only apply Dec axis flexture term on GEMs
        #ifdef MOUNT_TYPE_GEM
        pdCor = (IH2-IH1);                      // the Dec axis to RA axis perp. error should be the only major source of error left affecting the HA
        pdCor = pdCor/tan(newTargetDec/Rad);    // correct for Dec of measurement location
        #else
        pdCor = 0.0;
        #endif

        IH=IH1;
        IHS = (long)(IH*(double)StepsPerDegreeAxis1);
        ID=ID1;
        IDS = (long)(ID*(double)StepsPerDegreeAxis2);
      } else return true;
    } else return true;
  } else return true;

  return false;
}

