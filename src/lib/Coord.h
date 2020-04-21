double getInstrAxis1() {
  cli(); long p1=posAxis1; sei();
  double p=(double)((long)p1+indexAxis1Steps)/(double)AXIS1_STEPS_PER_DEGREE;
  cli(); long p2=posAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif
  
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) p=p+180.0-360.0;
  return p;
}

double getInstrAxis2() {
  cli(); long p2=posAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif
  
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) q=180.0-q; if (q > 180.0) q-=360.0; if (q < -180.0) q+=360.0;
  return q;
}

int getInstrPierSide() {
  if (atHome) return PierSideNone;
  cli(); long p2=posAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif
  
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) return PierSideWest; else return PierSideEast;
}

void setIndexAxis1(double axis1, int newPierSide) {
  // sky=pos+index, index=sky-pos
  if (newPierSide == PierSideWest) axis1=axis1+180.0;
  cli(); long p1=posAxis1; sei();
  indexAxis1=axis1-(double)p1/(double)AXIS1_STEPS_PER_DEGREE;
  indexAxis1Steps=(long)(indexAxis1*(double)AXIS1_STEPS_PER_DEGREE);
}

void setIndexAxis2(double axis2, int newPierSide) {
  if (latitude >= 0) { if (newPierSide == PierSideWest) axis2=180.0-axis2; } else { if (newPierSide == PierSideWest) axis2=-180.0-axis2; }
  if (axis2 > 360.0) axis2-=360.0; if (axis2 < -360.0) axis2+=360.0;
  cli(); long p2=posAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif

  indexAxis2=axis2-(double)p2/(double)AXIS2_STEPS_PER_DEGREE;
  indexAxis2Steps=(long)(indexAxis2*(double)AXIS2_STEPS_PER_DEGREE);
}

void setTargetAxis1(double axis1, int newPierSide) {
  if (newPierSide == PierSideWest) axis1=axis1+180.0;
  long p1=(double)(axis1-indexAxis1)*(double)AXIS1_STEPS_PER_DEGREE;
  cli(); targetAxis1.part.m=p1; targetAxis2.part.f=0; sei();
}

void setTargetAxis2(double axis2, int newPierSide) {
  if (latitude >= 0) { if (newPierSide == PierSideWest) axis2=180.0-axis2; } else { if (newPierSide == PierSideWest) axis2=-180.0-axis2; }
  if (axis2 > 360.0) axis2-=360.0; if (axis2 < -360.0) axis2+=360.0;
  axis2=(double)(axis2-indexAxis2);

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  axis2=tan(axis2/Rad)*Rad;
#endif
  
  long p2=axis2*(double)AXIS2_STEPS_PER_DEGREE;
  cli(); targetAxis2.part.m=p2; targetAxis2.part.f=0; sei();
}

double toInstrAxis2(double axis2, int newPierSide) {
  if (latitude >= 0) { if (newPierSide == PierSideWest) axis2=180.0-axis2; } else { if (newPierSide == PierSideWest) axis2=-180.0-axis2; }
  if (axis2 > 360.0) axis2-=360.0; if (axis2 < -360.0) axis2+=360.0;

  axis2=(double)(axis2-indexAxis2);
#if AXIS2_TANGENT_ARM_CORRECTION == ON
  axis2=tan(axis2/Rad)*Rad;
#endif
  
  return axis2;
}

double getTargetAxis1() {
  cli(); long p1=targetAxis1.part.m; sei();
  double p=(double)((long)p1+indexAxis1Steps)/(double)AXIS1_STEPS_PER_DEGREE;
  cli(); long p2=targetAxis2.part.m; sei();
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) p=p+180.0-360.0;
  return p;
}

double getTargetAxis2() {
  cli(); long p2=targetAxis2.part.m; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif

  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) q=180.0-q; if (q > 180.0) q-=360.0; if (q < -180.0) q+=360.0;
  return q;
}

double getStartAxis1() {
  cli(); long p1=startAxis1; sei();
  double p=(double)((long)p1+indexAxis1Steps)/(double)AXIS1_STEPS_PER_DEGREE;
  cli(); long p2=startAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif
  
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) p=p+180.0-360.0;
  return p;
}

double getStartAxis2() {
  cli(); long p2=startAxis2; sei();

#if AXIS2_TANGENT_ARM_CORRECTION == ON
  p2=atan(((double)p2/AXIS2_STEPS_PER_DEGREE)/Rad)*Rad*AXIS2_STEPS_PER_DEGREE;
#endif
  
  double q=(double)((long)p2+indexAxis2Steps)/(double)AXIS2_STEPS_PER_DEGREE;
  if ((q < -90.0) || (q > 90.0)) q=180.0-q; if (q > 180.0) q-=360.0; if (q < -180.0) q+=360.0;
  return q;
}
