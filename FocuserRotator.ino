// -----------------------------------------------------------------------------------
// Rotator/DeRotator/Focusers

// ROTATOR/DEROTATOR ----------------------------
#ifdef ROTATOR_ON

// time to write position to EEPROM after last movement of Focuser 1/2, default = 5 minutes
#define DelayToWrite 1000L*60L*5L

#ifdef MOUNT_TYPE_ALTAZM
void RotatorMove() {
  // do de-rotate movement
  if (deRotate && (trackingState==TrackingSidereal)) {
    targetAxis3.fixed+=amountRotateAxis3.fixed;
    double f=(long)targetAxis3.part.m; f/=(double)StepsPerDegreeAxis3;
    if ((f<(double)MinAxis3) || (f>(double)MaxAxis3)) { deRotate=false; amountRotateAxis3.fixed=0; }
  }
}

void DeRotate() {
  // calculate new de-rotation rate if needed
  if (deRotate && (trackingState==TrackingSidereal)) {
    double h,d;
    getApproxEqu(&h,&d,true);
    double pr=ParallacticRate(h,d)*(double)StepsPerDegreeAxis3; // in steps per second
    if (deRotateReverse) pr=-pr;
    amountRotateAxis3.fixed=doubleToFixed(pr/100.0);            // in steps per 1/100 second
  }
}
#endif

void RotatorFollow() {
  unsigned long tempMs=millis();

#ifdef AXIS3_DISABLE
  static unsigned long lastMoveAxis3=0;
  static bool dd = true;
  if (!dd && ((millis()-lastMoveAxis3)>10000)) { dd=true; digitalWrite(Axis3_EN,AXIS3_DISABLE); }
#endif
  
  if ((long)(tempMs-axis3Ms)>0) {
    axis3Ms=tempMs+(unsigned long)MaxRateAxis3;
  
    if ((posAxis3<(long)targetAxis3.part.m) && (posAxis3<((double)MaxRot*(double)StepsPerDegreeAxis3))) {
#ifdef AXIS3_DISABLE
      if (dd) { dd=false; digitalWrite(Axis3_EN,AXIS3_ENABLE); delayMicroseconds(10); }
      lastMoveAxis3=millis();
#endif
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3++;
    } else
    if ((posAxis3>(long)targetAxis3.part.m) && (posAxis3>((double)MinRot*(double)StepsPerDegreeAxis3))) {
#ifdef AXIS3_DISABLE
      if (dd) { dd=false; digitalWrite(Axis3_EN,AXIS3_ENABLE); delayMicroseconds(10); }
      lastMoveAxis3=millis();
#endif
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3--;
    }
  }
}

#endif

// FOCUSER1 -------------------------------------
#ifdef FOCUSER1_ON

void Focuser1Move() {
  // do automatic movement
  targetAxis4.fixed+=amountMoveAxis4.fixed;
  { double f=(long)targetAxis4.part.m; f/=(double)StepsPerMicrometerAxis4;
  if ((f<(double)MinAxis4*1000.0) || (f>(double)MaxAxis4*1000.0)) amountMoveAxis4.fixed=0; }
}

void Focuser1SavePos() {
  EEPROM_writeLong(EE_posAxis4,(long)posAxis4);
}

void Focuser1RecallPos() {
  posAxis4=EEPROM_readLong(EE_posAxis4);
  targetAxis4.part.m=posAxis4; targetAxis4.part.f=0;
}

void Focuser1Follow() {
  unsigned long tempMs=millis();

#ifdef AXIS4_DISABLE
  static unsigned long lastMoveAxis4=0;
  static bool dd = true;
  if (!dd && ((millis()-lastMoveAxis4)>10000)) { dd=true; digitalWrite(Axis4_EN,AXIS4_DISABLE); }
#endif

  // write position to EEPROM
  // not moving for x minutes?
  static unsigned long lastMove=millis();
  static long lastPos=0;
  if ((posAxis4!=lastPos)) { lastMove=millis(); lastPos=posAxis4; }
  // needs updating?
  if (posAxis4!=EEPROM_readLong(EE_posAxis4)) {
    // not slewing
    if ((trackingState!=TrackingMoveTo) && !guideDirAxis1 && !guideDirAxis2) {
      // enough time has passed
      if ((long)(millis()-lastMove)>DelayToWrite) Focuser1SavePos();
    }
  }

  if ((long)(tempMs-axis4Ms)>0) {
    axis4Ms=tempMs+(unsigned long)MaxRateAxis4;

    if ((posAxis4<(long)targetAxis4.part.m) && (posAxis4<((double)MaxAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
#ifdef AXIS4_DISABLE
      if (dd) { dd=false; digitalWrite(Axis4_EN,AXIS4_ENABLE); delayMicroseconds(10); }
      lastMoveAxis4=millis();
#endif
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4++;
    } else
    if ((posAxis4>(long)targetAxis4.part.m) && (posAxis4>((double)MinAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
#ifdef AXIS4_DISABLE
      if (dd) { dd=false; digitalWrite(Axis4_EN,AXIS4_ENABLE); delayMicroseconds(10); }
      lastMoveAxis4=millis();
#endif
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4--;
    }
  }
}

#endif

// FOCUSER2 -------------------------------------
#ifdef FOCUSER2_ON

void Focuser2Move() {
  // do automatic movement
  targetAxis5.fixed+=amountMoveAxis5.fixed;
  { double f=(long)targetAxis5.part.m; f/=(double)StepsPerMicrometerAxis5;
  if ((f<(double)MinAxis5*1000.0) || (f>(double)MaxAxis5*1000.0)) amountMoveAxis5.fixed=0; }
}

void Focuser2SavePos() {
  EEPROM_writeLong(EE_posAxis5,(long)posAxis5);
}

void Focuser2RecallPos() {
  posAxis5=EEPROM_readLong(EE_posAxis5);
  targetAxis5.part.m=posAxis5; targetAxis5.part.f=0;
}

void Focuser2Follow() {
  unsigned long tempMs=millis();

#ifdef AXIS5_DISABLE
  static unsigned long lastMoveAxis5=0;
  static bool dd = true;
  if (!dd && ((millis()-lastMoveAxis5)>10000)) { dd=true; digitalWrite(Axis4_EN,AXIS4_DISABLE); }
#endif

  // write position to EEPROM
  // not moving for x minutes?
  static unsigned long lastMove=millis();
  static long lastPos=0;
  if ((posAxis5!=lastPos)) { lastMove=millis(); lastPos=posAxis5; }
  // needs updating?
  if (posAxis5!=EEPROM_readLong(EE_posAxis5)) {
    // not slewing
    if ((trackingState!=TrackingMoveTo) && !guideDirAxis1 && !guideDirAxis2) {
      // enough time has passed
      if ((long)(millis()-lastMove)>DelayToWrite) Focuser2SavePos();
    }
  }

  if ((long)(tempMs-axis5Ms)>0) {
    axis5Ms=tempMs+(unsigned long)MaxRateAxis5;

    if ((posAxis5<(long)targetAxis5.part.m) && (posAxis5<((double)MaxAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
#ifdef AXIS5_DISABLE
      if (dd) { dd=false; digitalWrite(Axis5_EN,AXIS5_ENABLE); delayMicroseconds(10); }
      lastMoveAxis5=millis();
#endif
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5++;
    } else
    if ((posAxis5>(long)targetAxis5.part.m) && (posAxis5>((double)MinAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
#ifdef AXIS5_DISABLE
      if (dd) { dd=false; digitalWrite(Axis5_EN,AXIS5_ENABLE);  delayMicroseconds(10); }
      lastMoveAxis5=millis();
#endif
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5--;
    }
  }
}

#endif


