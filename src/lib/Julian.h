// Miscellaneous Utility Function

#pragma once

// Convert Gregorian date (Y,M,D) to Julian day number
double julian(int Year, int Month, int Day) {
  if ((Month==1) || (Month==2)) { Year--; Month=Month+12; }
  double B=2.0-floor(Year/100.0)+floor(Year/400.0);
  return (B+floor(365.25*Year)+floor(30.6001*(Month+1.0))+Day+1720994.5); //+(Time/24.0);
}

// Convert Julian day number to Gregorian date (Y,M,D)
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

// Utility function for extracting date components from a string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9') {
    v = *p - '0';
  }
  return 10 * v + *++p - '0';
}

// Extract the month from the compiler's __DATE__ string
int CompilerMonth(const char* date) {
  // Example input:  "Mar 26 2018"

  int Month=1;
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 

  switch (date[0]) {
    case 'J': 
      Month = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7);
      break;
    case 'F': 
      Month = 2;
      break;
    case 'A': 
      Month = date[2] == 'r' ? 4 : 8;
      break;
    case 'M': 
      Month = date[2] == 'r' ? 3 : 5;
      break;
    case 'S': 
      Month = 9;
      break;
    case 'O': 
      Month = 10;
      break;
    case 'N': 
      Month = 11;
      break;
    case 'D': 
      Month = 12;
      break;
  }

  return Month;
}

// Extract the year from the compiler __DATE__ string
int CompilerYear(const char* date) {
  // Example input:  "Mar 26 2018"
  return conv2d(date + 9);
}

// Extract the day from the compiler __DATE__ string
int CompilerDay(const char* date) {
  // Example input:  "Mar 26 2018"
  return conv2d(date + 4);
}

// Convert the compiler's date to a Julian date
double CompilerDateToJulian() {
  return julian(CompilerYear(__DATE__), CompilerMonth(__DATE__), CompilerDay(__DATE__));
}

