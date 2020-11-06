// -----------------------------------------------------------------------------------
// Rotator

#pragma once

// time to write position to nv after last movement of Rotator
#if NV_ENDURANCE == VHIGH
  #define ROTATOR_WRITE_DELAY 1000L*5L       // 5 seconds
#elif NV_ENDURANCE == HIGH
  #define ROTATOR_WRITE_DELAY 1000L*60L      // 1 minute
#else
  #define ROTATOR_WRITE_DELAY 1000L*60L*5L   // 5 minutes
#endif

class rotator {
  public:
    void init(int stepPin, int dirPin, int enPin, int nvAddress, float maxRate, double stepsPerDeg, double min, double max) {
      this->stepPin=stepPin;
      this->dirPin=dirPin;
      this->enPin=enPin;
      this->nvAddress=nvAddress;
      this->maxRate=maxRate;
      this->spd=stepsPerDeg;

      // default min/max (in degrees)
      setMin(min);
      setMax(max);

      pinMode(stepPin,OUTPUT);
      pinMode(dirPin,OUTPUT);

      // get backlash amount
      int b=nv.readInt(nvAddress+EE_rotBacklash);
      if (b < 0) { b=0; generalError=ERR_NV_INIT; DLF("ERR, rot.init(): bad NV rotBacklash < 0 steps (set to 0)"); }
      setBacklash(b);

      // get backlash position
      backlashPos=nv.readInt(nvAddress+EE_rotBacklashPos);
      if (backlashPos < 0)           { backlashPos=0; generalError=ERR_NV_INIT; DLF("ERR, rot.init(): bad NV rotBacklash < 0 steps (set to 0)"); }
      if (backlashPos > backlashMax) { backlashPos=0; generalError=ERR_NV_INIT; DLF("ERR, rot.init(): bad NV rotBacklash > 1/10 full range (set to 0)"); }

      // get step position
      spos=nv.readLong(nvAddress+EE_rotSpos);
      if (spos < smin) { spos=smin; DLF("WRN, rot.init(): bad NV position < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      if (spos > smax) { spos=smax; DLF("WRN, rot.init(): bad NV position > _LIMIT_MAX (set to _LIMIT_MAX)"); }

      // positions
      target.part.m=spos; target.part.f=0;
      delta.fixed=0;
      deltaDR.fixed=0;
      amountRotate.fixed=0;

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // degrees per second default
      setMoveRate(1.0);

      nextPhysicalMove=micros()+(unsigned long)(maxRate*1000.0);
      lastPhysicalMove=nextPhysicalMove;
    }

    // get step size in degrees
    double getStepsPerDegree() { return spd; }

    // minimum position in degrees
    void setMin(double min) { smin=min*spd; if (smin < -180L*3600L || smin > 180*3600L) smin=0; if (smin > smax) smin=smax; backlashMax=(smax-smin)/10; if (backlashMax > 32767) backlashMax=32767; }
    double getMin() { return smin/spd; }

    // maximum position in degrees
    void setMax(double max) { smax=max*spd; if (smax < -180L*3600L || smax > 180L*3600L) smax=0; if (smax < smin) smax=smin; backlashMax=(smax-smin)/10; if (backlashMax > 32767) backlashMax=32767; }
    double getMax() { return smax/spd; }

    // backlash, in steps
    bool setBacklash(int b) { if (b < 0 || b > backlashMax) return false; backlash = b; nv.writeInt(nvAddress+EE_rotBacklash,backlash); return true; }
    int getBacklash() { return backlash; }

    // sets logic state for reverse motion
    void setReverseState(int reverseState) {
      this->reverseState=reverseState;
      if (reverseState == LOW) forwardState=HIGH; else forwardState=LOW;
    }

    // sets logic state for disabling stepper driver
    void setDisableState(bool disableState) {
      this->disableState=disableState;
      if (disableState == LOW) enableState=HIGH; else enableState=LOW;
      if (enPin != OFF && enPin != SHARED) { pinMode(enPin,OUTPUT); enableDriver(); currentlyDisabled=false; }
    }

    // allows enabling/disabling stepper driver
    void powerDownActive(bool active) {
      if (enPin == OFF || enPin == SHARED) active=false;
      pda=active;
      if (pda) { pinMode(enPin,OUTPUT); disableDriver(); currentlyDisabled=true; }
    }

#if MOUNT_TYPE == ALTAZM
    // enable/disable the derotator
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
      if (moveRate > spsMax) moveRate=spsMax;     // limit to maxRate
    }
    
    // check if moving
    bool moving() {
      if (delta.fixed != 0 || (long)target.part.m != spos || backlashDir != BD_NONE) return true; else return false;
    }

    // enable/disable new continuous move mode
    void moveContinuous(bool en) {
      mc=en;
    }

    // move CW
    void startMoveCW() {
      if (!movementAllowed()) return;
      if (mc) {
        delta.fixed=doubleToFixed(+moveRate/100.0); // in steps per centi-second
      } else {
        fixed_t xl;
        xl.part.m=(long)(increment*spd*1000.0); xl.fixed/=1000;
        target.fixed+=xl.fixed;
        if ((long)target.part.m > smax) { target.part.m=smax; target.part.f=0; }
      }
    }

    // move CCW
    void startMoveCCW() {
      if (!movementAllowed()) return;
      if (mc) {
        delta.fixed=doubleToFixed(-moveRate/100.0); // in steps per centi-second
      } else {
        fixed_t xl;
        xl.part.m=(long)(increment*spd*1000.0); xl.fixed/=1000;
        target.fixed-=xl.fixed;
        if ((long)target.part.m < smin) { target.part.m=smin; target.part.f=0; }
      }
    }

    // stop move
    void stopMove() {
      delta.fixed=0;
      target.part.m=spos; target.part.f=0;
    }

    // get position
    double getPosition() {
      return ((double)spos)/spd;
    }

    // sets current position in degrees
    void setPosition(double deg) {
      spos=lround(deg*spd);
      if (spos < smin) spos=smin; if (spos > smax) spos=smax;
      target.part.m=spos; target.part.f=0;
    }

    // set target in degrees
    bool setTarget(double deg) {
      if (!movementAllowed()) return false;
      target.part.m=lround(deg*spd); target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
      return true;
    }

    // do derotate movement
    void poll(bool tracking) {
      if (!movementAllowed()) return;
      if (DR && tracking) target.fixed+=deltaDR.fixed;
      target.fixed+=delta.fixed;
      if (((long)target.part.m < smin) || ((long)target.part.m > smax)) { DR=false; delta.fixed=0; deltaDR.fixed=0; }
    }

#if MOUNT_TYPE == ALTAZM
    // calculate new derotation rate if needed
    void derotate(double h, double d) {
      if (DR) {
        double pr=ParallacticRate(h,d)*spd;     // in steps per second
        if (DRreverse) pr=-pr;
        deltaDR.fixed=doubleToFixed(pr/100.0);  // in steps per 1/100 second
      }
    }
#endif
    
    void follow(bool mountSlewing) {
      if (!movementAllowed()) return;

      // if enabled and the timeout has elapsed, disable the stepper driver
      if (pda && !currentlyDisabled && ((long)(micros()-lastPhysicalMove) > 10000000L)) { disableDriver(); currentlyDisabled=true; }

      unsigned long microsNow=micros();
      if ((long)(microsNow-nextPhysicalMove) > 0) {
        nextPhysicalMove=microsNow+(unsigned long)(maxRate*1000.0);

        // write position as needed to non-volatile storage if not moving for ROTATOR_WRITE_DELAY milliseconds
        if (moving()) sinceMovingMs=millis();
        if (!mountSlewing && !DR && (long)(millis()-sinceMovingMs) > ROTATOR_WRITE_DELAY) writeTarget();

        if ((spos < (long)target.part.m && spos < smax) || backlashDir == BD_OUT) {
          if (pda && currentlyDisabled) { currentlyDisabled=false; enableDriver(); delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,forwardState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH);
          if (backlashPos < backlash) { backlashPos++; backlashDir=BD_OUT; } else { spos++; backlashDir=BD_NONE; }
          lastPhysicalMove=micros();
        } else
        if ((spos > (long)target.part.m && spos > smin) || backlashDir == BD_IN) {
          if (pda && currentlyDisabled) { currentlyDisabled=false; enableDriver(); delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,reverseState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH);
          if (backlashPos > 0) { backlashPos--; backlashDir=BD_IN; } else { spos--; backlashDir=BD_NONE; }
          lastPhysicalMove=micros();
        }
      }
    }

    void savePosition() { writeTarget(); }
  
  private:
    bool movementAllowed() {
      if (enPin == SHARED && !axis1Enabled) return false; else return true;
    }
    void writeTarget() {
      nv.writeLong(nvAddress+EE_rotSpos,spos);
      nv.writeInt(nvAddress+EE_rotBacklashPos,backlashPos);
    }

    void enableDriver() {
      if (enPin == OFF || enPin == SHARED) return;
      // for Aux5/Aux6 (DAC) support for stepper driver EN control on MaxPCB
#if defined(A21) && defined(A22)
      if (enPin == A21) { if (enableState == HIGH) analogWrite(A21,1024); else analogWrite(A21,0); return; }
      if (enPin == A22) { if (enableState == HIGH) analogWrite(A22,1024); else analogWrite(A22,0); return; }
#endif
      digitalWrite(enPin,enableState);
    }

    void disableDriver() {
      if (enPin == OFF || enPin == SHARED) return;
#if defined(A21) && defined(A22)
      if (enPin == A21) { if (disableState == HIGH) analogWrite(A21,1024); else analogWrite(A21,0); return; }
      if (enPin == A22) { if (disableState == HIGH) analogWrite(A22,1024); else analogWrite(A22,0); return; }
#endif
      digitalWrite(enPin,disableState);
    }

#if MOUNT_TYPE == ALTAZM
    // returns parallactic angle in degrees
    double ParallacticAngle(double HA, double Dec) {
      return atan2(sin(HA/Rad),cos(Dec/Rad)*tan(latitude/Rad)-sin(Dec/Rad)*cos(HA/Rad))*Rad;
    }
    
    // returns parallactic rate in degrees per second
    double ParallacticRate(double HA, double Dec) {
      // one minute of HA in degrees=15/60=0.25
      double a1=ParallacticAngle(HA-0.125,Dec);
      double a2=ParallacticAngle(HA+0.125,Dec);
      if ((a1 > +90.0) && (a2 < -90.0)) a2+=360.0;
      if ((a1 < -90.0) && (a2 > +90.0)) a1+=360.0;
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
    float maxRate=-1;
    long spsMax=-1;
    long smin=-1;
    long smax=-1;

    // state
    bool mc=false;
    int reverseState=LOW;
    int forwardState=HIGH;
    bool pda=false;
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
    int backlash=0;
    int backlashPos=0;
    int backlashDir=0;
    long backlashMax=0;

    // automatic movement
    double moveRate=0.1;
    fixed_t delta;
    fixed_t deltaDR;
    double increment=1.0;
    
    // timing
    unsigned long sinceMovingMs=0;
    unsigned long lastPhysicalMove=0;
    unsigned long nextPhysicalMove=0;
};
