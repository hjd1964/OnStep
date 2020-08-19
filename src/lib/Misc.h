// -----------------------------------------------------------------------------------------------------------------------------
// Misc. general purpose functions

// string to int with error checking
bool atoi2(char *a, int *i, bool sign=true) {
  int len=strlen(a);
  if (len == 0 || len > 6) return false;
  for (int l=0; l < len; l++) {
    if ((l == 0) && ((a[l] == '+') || (a[l] == '-')) && sign) continue;
    if ((a[l] < '0') || (a[l] > '9')) return false;
  }
  long l=atol(a);
  if ((l < -32767) || (l > 32768)) return false;
  *i=l;
  return true;
}

// string to float with error checking
bool atof2(char *a, double *d, bool sign=true) {
  int dc=0;
  int len=strlen(a);
  for (int l=0; l < len; l++) {
    if ((l == 0) && ((a[l] == '+') || (a[l] == '-')) && sign) continue;
    if (a[l] == '.') { if (dc == 0) { dc++; continue; } else return false; }
    if ((a[l] < '0') || (a[l] > '9')) return false;
  }
  *d=atof(a);
  return true;
}

// convert axis settings string into numeric form
bool decodeAxisSettings(char s[], axisSettings &a) {
  if (strcmp(s,"0") != 0) {
    char *ws=s;
    char *conv_end;
    double f=strtod(ws,&conv_end); if (&s[0] != conv_end) a.stepsPerMeasure=f; else return false;
    ws=strchr(ws,','); if (ws != NULL) {
      ws++; a.microsteps=strtol(ws,NULL,10);
      ws=strchr(ws,','); if (ws != NULL) {
        ws++; a.IRUN=strtol(ws,NULL,10);
        ws=strchr(ws,','); if (ws != NULL) {
          ws++; a.reverse=strtol(ws,NULL,10);
          ws=strchr(ws,','); if (ws != NULL) {
            ws++; a.min=strtol(ws,NULL,10);
            ws=strchr(ws,','); if (ws != NULL) {
              ws++; a.max=strtol(ws,NULL,10);
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

// convert axis settings string into numeric form
bool validateAxisSettings(int axisNum, bool altAz, volatile axisSettings &a) {
  int   MinLimitL[5]   = {-180,-90,-360,  0,  0};
  int   MinLimitH[5]   = { -90,  0,   0,500,500};
  int   MaxLimitL[5]   = {  90,  0,   0,  0,  0};
  int   MaxLimitH[5]   = { 180, 90, 360,500,500};
  float StepsLimitL[5] = {   150.0,   150.0,    5.0, 0.005, 0.005};
  float StepsLimitH[5] = {122400.0,122400.0, 7200.0,  20.0,  20.0};
  int   IrunLimitH[5]  = { 3000, 3000, 1000, 1000, 1000};
  if (altAz) { MinLimitL[0]=-360; MinLimitH[0]=-180; MaxLimitL[0]=180; MaxLimitH[0]=360; }
  axisNum--;
  if (a.stepsPerMeasure < StepsLimitL[axisNum] || a.stepsPerMeasure > StepsLimitH[axisNum]) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad stepsPerMeasure="); DL(a.stepsPerMeasure); return false; }
  if (a.microsteps != OFF && (a.microsteps < 1 || a.microsteps > 256)) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad microsteps="); DL(a.microsteps); return false; }
  if (a.IRUN != OFF && (a.IRUN < 0 || a.IRUN > IrunLimitH[axisNum])) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad IRUN="); DL(a.IRUN); return false; }
  if (a.reverse != OFF && a.reverse != ON) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad reverse="); DL(a.reverse); return false; }
  if (a.min < MinLimitL[axisNum] || a.min > MinLimitH[axisNum]) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad min="); DL(a.min); return false; }
  if (a.max < MaxLimitL[axisNum] || a.max > MaxLimitH[axisNum]) { DF("ERR, validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad max="); DL(a.max); return false; }
  return true;
}

// numeric help
double timeRange(double t) {
  while (t >= 24.0) t-=24.0;
  while (t < 0.0) t+=24.0;
  return t;
}

double haRange(double d) {
  while (d >= 180.0) d-=360.0;
  while (d < -180.0) d+=360.0;
  return d;
}

double degRange(double d) {
  while (d >= 360.0) d-=360.0;
  while (d <   0.0)  d+=360.0;
  return d;
}

double dist(double a, double b) {
  if (a > b) return a-b; else return b-a;
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
