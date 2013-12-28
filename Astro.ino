// -----------------------------------------------------------------------------------
// Astronomy related functions

// convert string in format MM/DD/YY to julian date
boolean dateToDouble(double *JulianDay, char *date) {
  char m[3],d[3],y[3];
  int  m1,d1,y1;
  
  if (strlen(date)!= 8) return false;

  m[0]=*date++; m[1]=*date++; m[2]=0; m1=atoi(m);
  if (*date++!='/') return false; d[0]=*date++; d[1]=*date++; d[2]=0; d1=atoi(d);
  if (*date++!='/') return false; y[0]=*date++; y[1]=*date++; y[2]=0; y1=atoi(y);
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
  
  if (highPrecision) { if (strlen(hms)!= 8) return false; } else if (strlen(hms)!= 7) return false;

  h[0]=*hms++; h[1]=*hms++; h[2]=0; h1=atoi(h);
  if (highPrecision) {
    if (*hms++!=':') return false; m[0]=*hms++; m[1]=*hms++; m[2]=0; m1=atoi(m);
    if (*hms++!=':') return false; s[0]=*hms++; s[1]=*hms++; s[2]=0; s1=atoi(s);
  } else {
    if (*hms++!=':') return false; m[0]=*hms++; m[1]=*hms++; m[2]=0; m1=atoi(m);
    if (*hms++!='.') return false; m2=(*hms++)-'0';
  }
  if ((h1<0) || (h1>23) || (m1<0) || (m1>59) || (m2<0) || (m2>9) || (s1<0) || (s1>59)) return false;
  
  *f=h1+m1/60.0+m2/600.0+s1/3600.0;
  return true;
}

boolean doubleToHms(char *reply, double *f) {
  char h[3],m[5],str[20];
  int  h1;
  double m1,f1,s1;

  f1=*f+0.00001; // keep from rounding down in seconds

  h1=floor(f1);
  m1=(f1-h1)*60;
  s1=(m1-floor(m1));

  char s[]="%02d:%02d:%02d";
  if (highPrecision) {
    s1=s1*60;
  } else {
    s1=s1*10;
    s[9]=':'; s[12]='1';
  }
  sprintf(reply,s,h1,(int)m1,(int)s1);

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
    if (*dms=='-') sign=-1.0; else if (*dms=='+') sign=1.0; else return false; *dms++;
    d[0]=*dms++; d[1]=*dms++; d[2]=0; d1=atoi(d); if (errno) return false;
  } else {
    d[0]=*dms++; d[1]=*dms++; d[2]=*dms++; d[3]=0; d1=atoi(d); if (errno) return false;
  }

  // make sure the seperator is an allowed character
  if ((*dms!=':') && (*dms!='*') && (*dms!=char(223))) return false; else *dms++; 

  m[0]=*dms++; m[1]=*dms++; m[2]=0; m1=atoi(m); if (errno) return false;

  if ((highPrecision) && (!secondsOff)) {
    // make sure the seperator is an allowed character
    if (*dms++!=':') return false; 
    s[0]=*dms++; s[1]=*dms++; s[2]=0; s1=atoi(s);
  }

  if (sign_present) { lowLimit=-90; highLimit=90; }
  if ((d1<lowLimit) || (d1>highLimit) || (m1<0) || (m1>59) || (s1<0) || (s1>59)) return false;
  
  *f=sign*(d1+m1/60.0+s1/3600.0);
  return true;
}

boolean doubleToDms(char *reply, double *f, boolean fullRange, boolean signPresent) {
  char d[4],m[5],sign[]="+";
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
//  Serial.print(str);
  return true;
}

// takes the actual equatorial coordinates and applies the offset correction
// this takes approx. 5mS, which should be ok for not delaying things too much in the main loop
boolean EquToCEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  double Alt;
  double Azm;

  if (abs(Dec)<89.98333333) {
    // breaks-down near the pole (limited to >1' from pole),
    // when time allows, I'll look into fixing this - but for now I'll just do a crude work-around
    EquToHor(Lat,HA,Dec,&Alt,&Azm);
    HorToEqu(Lat+altCor,Alt,Azm+azmCor,HA1,Dec1);
  } else {
    // just ignore the pointing model if right on the pole
    *HA1 =HA;
    *Dec1=Dec;
    return false;
  }

  // finally, apply index offsets... range limits are disabled here, we're working with offset coords
  *HA1=*HA1-IH;
  *Dec1=*Dec1-ID;
  return true;
}

// takes the offset corrected coordinates and returns the actual equatorial coordinates 
// this takes approx. 5mS, which should be ok for not delaying things too much in the main loop
boolean CEquToEqu(double Lat, double HA, double Dec, double *HA1, double *Dec1) { 
  double Alt;
  double Azm;

  // remove the index offsets
  HA=HA+IH;
  Dec=Dec+ID;
  if (Dec>+90.0) Dec=+90.0;
  if (Dec<-90.0) Dec=-90.0;

  if (abs(Dec)<89.98333333) {
    // breaks-down near the pole (limited to >1' from pole),
    // when time allows, I'll look into fixing this - but for now I'll just do a crude work-around
    EquToHor(Lat,HA,Dec,&Alt,&Azm);
    HorToEqu(Lat-altCor,Alt,Azm-azmCor,HA1,Dec1);
    return true;
  } else {
    // just ignore the pointing model if right on the pole
    *HA1=HA;
    *Dec1=Dec;
    return false;
  }
}

// convert equatorial coordinates to horizon
// this takes approx. 1-2mS
void EquToHor(double Lat, double HA, double Dec, double *Alt, double *Azm) {
  double SinAlt;  
  double CosAzm;  

  HA =(HA*15.0)/Rad;
  Dec=Dec/Rad;
  Lat=Lat/Rad;
  
  SinAlt = (sin(Dec) * sin(Lat)) + (cos(Dec) * cos(Lat) * cos(HA));  
  *Alt   = asin(SinAlt);
  CosAzm = ((sin(Dec) - (sin(Lat) * sin(*Alt))) / (cos(Lat) * cos(*Alt)));  
  *Azm   = acos(CosAzm)*Rad;
  if (sin(HA) > 0) { *Azm = 360.0 - *Azm; }
  *Alt = *Alt*Rad;
}
      
// convert horizon coordinates to equatorial
// this takes approx. 1-2mS
void HorToEqu(double Lat, double Alt, double Azm, double *HA, double *Dec) { 
  double SinDec;  
  double CosHA;
      
  Alt  = Alt/Rad;
  Azm  = Azm/Rad;
  Lat  = Lat/Rad;
  SinDec = (sin(Alt) * sin(Lat)) + (cos(Alt) * cos(Lat) * cos(Azm));  
  *Dec = asin(SinDec); 
  CosHA = ((sin(Alt) - (sin(Lat) * sin(*Dec))) / (cos(Lat) * cos(*Dec)));  
  *HA = acos(CosHA)*Rad;
  if (sin(Azm) > 0) { *HA = 360 - *HA; }
  *Dec = *Dec*Rad;
  *HA  = *HA/15.0;
}

// converts Gregorian date (Y,M,D) to Julian day number
double julian(int Year, int Month, int Day) {
  if ((Month==1) || (Month==2)) { Year--; Month=Month+12; }
  double B=2.0-floor(Year/100.0)+floor(Year/400.0);
  return (B+floor(365.25*Year)+floor(30.6001*(Month+1.0))+Day+1720994.5); //+(Time/24.0);
}

// converts Julian day number to Gregorian date (Y,M,D)
double greg(double JulianDay, int *Year, int *Month, int *Day) {
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
// uses LST, updates lst
void update_lst() {
  long lst1=(LST/24.0)*8640000.0;
  siderealTimer=lst1;

  // set the local sidereal time, mark where we start at
  cli(); 
  lst=lst1;
  lst_start=lst;
  sei();
  lst_mS_start=millis();
}

// convert the lst (in 1/100 second units) into floating point hours
// uses lst, updates LST
void update_LST() {
  cli(); long tempLst=lst; sei();
  LST=timeRange((tempLst/8640000.0)*360.0/15.0); 
}

double timeRange(double time) {
  while (time>=24.0) time-=24.0;
  while (time<  0.0) time+=24.0;
  return time;
}

double degreeRange(double d) {
  while (d>=360.0) d-=360.0;
  while (d<  0.0)  d+=360.0;
  return d;
}
