// -----------------------------------------------------------------------------------
// Julian date/time conversion

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

