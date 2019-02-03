// -----------------------------------------------------------------------------------
// Focusers

#pragma once

// time to write position to nv after last movement of Focuser 1/2, default = 5 minutes
#define FOCUSER_WRITE_DELAY 1000L*60L*5L

class focuser {
  public:
    // init(Axis5StepPin,Axis5DirPin,Axis4_EN,EE_posAxis5,MaxRateAxis5,StepsPerMicrometerAxis5);
    void init(int stepPin, int dirPin, int enPin, int nvAddress, long maxRate, double stepsPerMicro) {
      this->stepPin=stepPin;
      this->dirPin=dirPin;
      this->enPin=enPin;
      this->nvAddress=nvAddress;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;

      if (stepPin!=-1) pinMode(stepPin,OUTPUT);
      if (dirPin!=-1) pinMode(dirPin,OUTPUT);
    
      spos=readPos();
      target.part.m=spos; target.part.f=0;
      lastPos=spos;
      delta.fixed=0;

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // microns per second default
      setMoveRate(100);

      // default min/max
      setMin(umin);
      setMax(umax);

      nextPhysicalMove=millis()+(unsigned long)maxRate;
      lastPhysicalMove=nextPhysicalMove;
    }

    // setMin(MinAxis5)
    // minimum position in microns
    void setMin(double min) {
      smin=(min*spm);
    }
    double getMin() {
      return smin/spm;
    }

    // setMax(MaxAxis5)
    // maximum position in microns
    void setMax(double max) {
      smax=(max*spm);
    }
    double getMax() {
      return smax/spm;
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

    // set movement rate in microns/second
    void setMoveRate(double rate) {
      moveRate=rate*spm;                          // in steps per second
      if (moveRate>spsMax) moveRate=spsMax;       // limit to maxRate
    }

    // check if moving
    bool moving() {
      if ((delta.fixed!=0) || ((long)target.part.m!=spos)) return true; else return false;
    }

    // move in
    void startMoveIn() {
      delta.fixed=doubleToFixed(+moveRate/100.0); // in steps per centi-second
    }

    // move out
    void startMoveOut() {
      delta.fixed=doubleToFixed(-moveRate/100.0); // in steps per centi-second
    }

    // stop move
    void stopMove() {
      delta.fixed=0;
      target.part.m=spos; target.part.f=0;
    }

    // get position
    double getPosition() {
      return ((double)spos)/spm;
    }

    // sets current position in microns
    void setPosition(double pos) {
      spos=round(pos*spm);
      if (spos<smin) spos=smin; if (spos>smax) spos=smax;
      target.part.m=spos; target.part.f=0;
    }

    // sets target position in microns
    void setTarget(double pos) {
      target.part.m=round(pos*spm); target.part.f=0;
      if ((long)target.part.m<smin) target.part.m=smin; if ((long)target.part.m>smax) target.part.m=smax;
    }

    // sets target relative position in microns
    void relativeTarget(double pos) {
      target.part.m+=round(pos*spm); target.part.f=0;
      if ((long)target.part.m<smin) target.part.m=smin; if ((long)target.part.m>smax) target.part.m=smax;
    }

    // do automatic movement
    void move() {
      target.fixed+=delta.fixed;
      // stop at limits
      if (((long)target.part.m<smin) || ((long)target.part.m>smax)) delta.fixed=0;
    }

    // follow( (trackingState==TrackingMoveTo) || guideDirAxis1 || guideDirAxis2) );
    void follow(boolean slewing) {

      // if enabled and the timeout has elapsed, disable the stepper driver
      if (pda && !currentlyDisabled && ((millis()-lastPhysicalMove)>10000L)) { disableDriver(); currentlyDisabled=true; }
    
      // write position to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
      if ((spos!=lastPos)) { lastMove=millis(); lastPos=spos; }
      if (!slewing && (spos!=readPos())) {
        // needs updating and enough time has passed?
        if ((long)(millis()-lastMove)>FOCUSER_WRITE_DELAY) writePos(spos);
      }

      unsigned long tempMs=millis();
      if ((long)(tempMs-nextPhysicalMove)>0) {
        nextPhysicalMove=tempMs+(unsigned long)maxRate;
    
        if ((spos<(long)target.part.m) && (spos<smax)) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,forwardState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH); spos++;
          lastPhysicalMove=millis();
        } else
        if ((spos>(long)target.part.m) && (spos>smin)) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,reverseState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH); spos--;
          lastPhysicalMove=millis();
        }
      }
    }

    void savePosition() {
      writePos(spos);
    }
  
  private:
    long readPos() {
      return nv.readLong(nvAddress);
    }

    void writePos(long p) {
      nv.writeLong(nvAddress,(long)p);
    }

    void enableDriver() {
      if (enPin==-1) return;
      // for Aux5/Aux6 (DAC) support for stepper driver EN control on MaxPCB Aux5=A21=66 Aux6=A22=67
      if ((enPin==66) || (enPin==67)) { if (enableState==HIGH) analogWrite(enPin,255); else analogWrite(enPin,0); delayMicroseconds(30); } else 
      { digitalWrite(enPin,enableState); delayMicroseconds(5); }
    }

    void disableDriver() {
      if (enPin==-1) return;
      if ((enPin==66) || (enPin==67)) { if (disableState==HIGH) analogWrite(enPin,255); else analogWrite(enPin,0); delayMicroseconds(30); } else 
      { digitalWrite(enPin,disableState); delayMicroseconds(5); }
    }

    // parameters
    int stepPin=-1;
    int dirPin=-1;
    int enPin=-1;
    int nvAddress=-1;
    long maxRate=-1;
    long spsMax=-1;
    long umin=-6L*25400L;
    long smin=-1;
    long umax=+6L*25400L;
    long smax=-1;

    // state
    int reverseState=LOW;
    int forwardState=HIGH;
    boolean pda=false;
    int disableState=LOW;
    int enableState=HIGH;
    bool currentlyDisabled=true;

    // conversion
    double spm=1.0;

    // position
    fixed_t target;
    long spos=0;
    long lastPos=0;

    // automatic movement
    double moveRate=0.0;
    fixed_t delta;

    // timing
    unsigned long lastMs=0;
    unsigned long lastMove=0;
    unsigned long lastPhysicalMove=0;
    unsigned long nextPhysicalMove=0;

};
