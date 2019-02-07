// -----------------------------------------------------------------------------------
// Rotator

#pragma once

// ROTATOR/DEROTATOR ----------------------------

class rotator {
  public:
    void init(int stepPin, int dirPin, int enPin, long maxRate, double stepsPerDeg) {
      this->stepPin=stepPin;
      this->dirPin=dirPin;
      this->enPin=enPin;
      this->maxRate=maxRate;
      this->spd=stepsPerDeg;

      if (stepPin!=-1) pinMode(stepPin,OUTPUT);
      if (dirPin!=-1) pinMode(dirPin,OUTPUT);

      // positions
      target.fixed=0;
      delta.fixed=0;
      deltaDR.fixed=0;
      amountRotate.fixed=0;

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // degrees per second default
      setMoveRate(1.0);

      // default min/max
      setMin(umin);
      setMax(umax);

      nextPhysicalMove=millis()+(unsigned long)maxRate;
      lastPhysicalMove=nextPhysicalMove;
    }

    // minimum position in degrees
    void setMin(double min) {
      smin=(min*spd);
    }
    double getMin() {
      return smin/spd;
    }

    // maximum position in degrees
    void setMax(double max) {
      smax=(max*spd);
    }
    double getMax() {
      return smax/spd;
    }

    // sets logic state for reverse motion
    void setReverseState(int reverseState) {
      this->reverseState=reverseState;
      if (reverseState==LOW) forwardState=HIGH; else forwardState=LOW;
    }

    // sets logic state for disabling stepper driver
    void setDisableState(boolean disableState) {
      this->disableState=disableState;
      if (disableState==LOW) enableState=HIGH; else enableState=LOW;
      if (enPin!=-1) { pinMode(enPin,OUTPUT); enableDriver(); currentlyDisabled=false; }
    }

    // allows enabling/disabling stepper driver
    void powerDownActive(boolean active) {
      if (enPin==-1) { pda=false; return; }
      pda=active;
      if (pda) { pinMode(enPin,OUTPUT); disableDriver(); currentlyDisabled=true; }
    }

#ifdef MOUNT_TYPE_ALTAZM
    // enable/disable the de-rotator
    void enableDR(bool state) {
      DR=state;
    }

    // sets rotator to the parallactic angle in this area of the sky
    void setPA(double h, double d) {
      target.part.m=(long)(ParallacticAngle(h,d)*(double)spd); target.part.f=0;
    }
#endif

    // reverse the direction of rotation
    void reverseDR() {
      DRreverse=!DRreverse;
    }

    // reset to home
    void reset() {
      spos=0;
      target.fixed=0;
      delta.fixed=0;
      DR=false;
      increment=1.0;
      deltaDR.fixed=0;
    }

    // return to home
    void home() {
      target.fixed=0;
      delta.fixed=0;
      DR=false;
      increment=1.0;
      deltaDR.fixed=0;
    }

    // set amount of movement
    void setIncrement(double i) {
      increment=i;
    }

    // set movement rate in deg/second (continuous mode)
    void setMoveRate(double rate) {
      moveRate=rate*spd;                          // in steps per second
      if (moveRate>spsMax) moveRate=spsMax;       // limit to maxRate
    }
    
    // check if moving
    bool moving() {
      if ((delta.fixed!=0) || ((long)target.part.m!=spos)) return true; else return false;
    }

    // enable/disable new continuous move mode
    void moveContinuous(bool en) {
      mc=en;
    }

    // move CW
    void startMoveCW() {
      if (mc) {
        delta.fixed=doubleToFixed(+moveRate/100.0); // in steps per centi-second
      } else {
        fixed_t xl;
        xl.part.m=(long)(increment*spd*1000.0); xl.fixed/=1000;
        target.fixed+=xl.fixed;
        if ((long)target.part.m>smax) { target.part.m=smax; target.part.f=0; }
      }
    }

    // move CCW
    void startMoveCCW() {
      if (mc) {
        delta.fixed=doubleToFixed(-moveRate/100.0); // in steps per centi-second
      } else {
        fixed_t xl;
        xl.part.m=(long)(increment*spd*1000.0); xl.fixed/=1000;
        target.fixed-=xl.fixed;
        if ((long)target.part.m<smin) { target.part.m=smin; target.part.f=0; }
      }
    }

    // stop move
    void stopMove() {
      delta.fixed=0;
      target.part.m=spos; target.part.f=0;
    }

    // get position
    double getPosition() {
      return ((double)spos+0.5)/spd;
    }

    // sets current position in degrees
    void setPosition(double deg) {
      spos=round(deg*spd);
      if (spos<smin) spos=smin; if (spos>smax) spos=smax;
      target.part.m=spos; target.part.f=0;
    }

    // set target
    void setTarget(double deg) {
      target.part.m=deg*spd; target.part.f=0;
      if ((long)target.part.m<smin) target.part.m=smin; if ((long)target.part.m>smax) target.part.m=smax;
    }

    // do de-rotate movement
    void move(boolean tracking) {
      if (DR && tracking) target.fixed+=deltaDR.fixed;
      target.fixed+=delta.fixed;
      if (((long)target.part.m<smin) || ((long)target.part.m>smax)) { DR=false; delta.fixed=0; deltaDR.fixed=0; }
    }

#ifdef MOUNT_TYPE_ALTAZM
    // calculate new de-rotation rate if needed
    void derotate(double h, double d) {
      if (DR) {
        double pr=ParallacticRate(h,d)*spd;     // in steps per second
        if (DRreverse) pr=-pr;
        deltaDR.fixed=doubleToFixed(pr/100.0);  // in steps per 1/100 second
      }
    }
#endif
    
    void follow() {
      if (pda && !currentlyDisabled && ((millis()-lastPhysicalMove)>10000L)) { currentlyDisabled=true; disableDriver(); }
      
      unsigned long tempMs=millis();
      if ((long)(tempMs-nextPhysicalMove)>0) {
        nextPhysicalMove=tempMs+(unsigned long)maxRate;
      
        if ((spos<(long)target.part.m) && (spos<smax)) {
          if (pda && currentlyDisabled) { currentlyDisabled=false; enableDriver(); delayMicroseconds(10); }
          digitalWrite(stepPin,LOW); delayMicroseconds(10);
          digitalWrite(dirPin,forwardState); delayMicroseconds(10);
          digitalWrite(stepPin,HIGH); spos++;
          lastPhysicalMove=millis();
        } else
        if ((spos>(long)target.part.m) && (spos>smin)) {
          if (pda && currentlyDisabled) { currentlyDisabled=false; enableDriver(); delayMicroseconds(10); }
          digitalWrite(stepPin,LOW); delayMicroseconds(10);
          digitalWrite(dirPin,reverseState); delayMicroseconds(10);
          digitalWrite(stepPin,HIGH); spos--;
          lastPhysicalMove=millis();
        }
      }
    }

  private:

    void enableDriver() {
      if (enPin==-1) return;
      // for Aux5/Aux6 (DAC) support for stepper driver EN control on MaxPCB
#if defined(A21) && defined(A22)
      if (enPin==A21) { if (enableState==HIGH) analogWrite(A21,1024); else analogWrite(A21,0); return; }
      if (enPin==A22) { if (enableState==HIGH) analogWrite(A22,1024); else analogWrite(A22,0); return; }
#endif
      digitalWrite(enPin,enableState);
    }

    void disableDriver() {
      if (enPin==-1) return;
#if defined(A21) && defined(A22)
      if (enPin==A21) { if (disableState==HIGH) analogWrite(A21,1024); else analogWrite(A21,0); return; }
      if (enPin==A22) { if (disableState==HIGH) analogWrite(A22,1024); else analogWrite(A22,0); return; }
#endif
      digitalWrite(enPin,disableState);
    }

#ifdef MOUNT_TYPE_ALTAZM
    // returns parallactic angle in degrees
    double ParallacticAngle(double HA, double Dec) {
      return atan2(sin(HA/Rad),cos(Dec/Rad)*tan(latitude/Rad)-sin(Dec/Rad)*cos(HA/Rad))*Rad;
    }
    
    // returns parallactic rate in degrees per second
    double ParallacticRate(double HA, double Dec) {
      // one minute of HA in degrees=15/60=0.25
      double a1=ParallacticAngle(HA-0.125,Dec);
      double a2=ParallacticAngle(HA+0.125,Dec);
      if ((a1>+90.0) && (a2<-90.0)) a2+=360.0;
      if ((a1<-90.0) && (a2>+90.0)) a1+=360.0;
      return (a2-a1)/60.0;
    /*
      double x=cos(Dec*Rad)*tan(latitude*Rad)-sin(Dec*Rad)*cos(HA*Rad);
      double sin2H=sin(HA*Rad)*sin(HA*Rad);
      return (0.25*(x*cos(HA*Rad)-sin2H*sin(Dec*Rad))/(x*x+sin2H))/60.0;
    */
    }
#endif

    // parameters
    int stepPin=-1;
    int dirPin=-1;
    int enPin=-1;
    int nvAddress=-1;
    long maxRate=-1;
    long spsMax=-1;
    long umin=-180;
    long smin=-1;
    long umax=+180;
    long smax=-1;

    // state
    boolean mc=false;
    int reverseState=LOW;
    int forwardState=HIGH;
    boolean pda=false;
    int disableState=LOW;
    int enableState=HIGH;
    bool currentlyDisabled=true;
    bool DRreverse=false;
    bool DR=false;
    
    // conversion
    double spd=1.0;

    // position
    fixed_t amountRotate;
    fixed_t target;
    long spos=0;
    long lastPos=0;

    // automatic movement
    double moveRate=0.0;
    fixed_t delta;
    fixed_t deltaDR;
    double increment=0.0;
    
    // timing
    unsigned long lastMs=0;
    unsigned long lastMove=0;
    unsigned long lastPhysicalMove=0;
    unsigned long nextPhysicalMove=0;
};

