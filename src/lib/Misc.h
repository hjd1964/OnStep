// -----------------------------------------------------------------------------------------------------------------------------
// Misc. general purpose functions

// string to int with error checking
bool atoi2(char *a, int *i, bool sign=true) {
  int len=strlen(a);
  if (len>6) return false;
  for (int l=0; l<len; l++) {
    if ((l==0) && ((a[l]=='+') || (a[l]=='-')) && sign) continue;
    if ((a[l]<'0') || (a[l]>'9')) return false;
  }
  long l=atol(a);
  if ((l<-32767) || (l>32768)) return false;
  *i=l;
  return true;
}

// string to float with error checking
bool atof2(char *a, double *d, bool sign=true) {
  int dc=0;
  int len=strlen(a);
  for (int l=0; l<len; l++) {
    if ((l==0) && ((a[l]=='+') || (a[l]=='-')) && sign) continue;
    if (a[l]=='.') { if (dc==0) { dc++; continue; } else return false; }
    if ((a[l]<'0') || (a[l]>'9')) return false;
  }
  *d=atof(a);
  return true;
}

// numeric help
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

double angDist(double h, double d, double h1, double d1) {
  return acos(sin(d/Rad)*sin(d1/Rad)+cos(d/Rad)*cos(d1/Rad)*cos((h1-h)/Rad))*Rad;
}

double frac(double v) {
  return v - ((long)v);
}

double cot(double n) {
  return 1.0/tan(n);
}
