// -----------------------------------------------------------------------------------
// Focusers

#pragma once

#include "StepperDC.h"

// time to write position to nv after last movement of Focuser 1/2, default = 5 minutes
#define FOCUSER_WRITE_DELAY 1000L*60L*5L

class focuserDC {
  public:
    // init(Axis5StepPin,Axis5DirPin,Axis4_EN,EE_posAxis5,MaxRateAxis5,StepsPerMicrometerAxis5);
    void init(int stepPin, int dirPin, int enPin, int nvAddress, long maxRate, double stepsPerMicro) {

      // init only happens once, on the first call and is ignored otherwise
      dcMotor.init(stepPin,dirPin,enPin,maxRate);
      
      this->nvAddress=nvAddress;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;
    
      spos=readPos();
      target.part.m=spos; target.part.f=0;
      lastPos=spos;
      delta.fixed=0;

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // microns per second default
      setMoveRate(500);

      // default min/max
      setMin(umin);
      setMax(umax);

      nextPhysicalMove=millis()+(unsigned long)maxRate;
      lastPhysicalMove=nextPhysicalMove;
    }

    void setDcPower(byte power) {
      this->powerFor1mmSec=power;
    }

    void setPhase1() {
      phase1=true;
    }
    void setPhase2() {
      phase1=false;
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
      if (reverseState==HIGH) reverse=true; else reverse=false;
    }

    // sets logic state for disabling stepper driver
    void setDisableState(boolean disableState) {
      dcMotor.setDisableState(disableState);
    }

    // allows enabling/disabling stepper driver
    void powerDownActive(boolean active) {
    }

    // set movement rate in microns/second
    void setMoveRate(double rate) {
      // a rate of 1000 gives 1mm/second (fastest)
      moveRate=rate*spm;                                  // in steps per second, for a DC motor a step is 1 micron.
      if (moveRate>spsMax) moveRate=spsMax;               // limit to maxRate
    }

    // check if moving
    bool moving() {
      if ((delta.fixed!=0) || ((long)target.part.m!=spos)) return true; else return false;
    }

    // move in
    void startMoveIn() {
      // rate is some fraction of 1 millimeter per second so this is the % power for 1 millimeter per second motion
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      delta.fixed=doubleToFixed(+moveRate/100.0); // in steps per centi-second
    }

    // move out
    void startMoveOut() {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
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
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      target.part.m=round(pos*spm); target.part.f=0;
      if ((long)target.part.m<smin) target.part.m=smin; if ((long)target.part.m>smax) target.part.m=smax;
    }

    // sets target relative position in microns
    void relativeTarget(double pos) {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
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
          
      // write position to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
      if ((spos!=lastPos)) { lastMove=millis(); lastPos=spos; }
      if (!slewing && (spos!=readPos())) {
        // needs updating and enough time has passed?
        if ((long)(millis()-lastMove)>FOCUSER_WRITE_DELAY) writePos(spos);
      }

      unsigned long tempMs=millis();
      if ((long)(tempMs-nextPhysicalMove)>0) {
        nextPhysicalMove=tempMs+(unsigned long)maxRate;
        if (moving()) {
          if ((spos<(long)target.part.m) && (spos<smax)) {
            if (reverse) dcMotor.setDirectionIn(); else dcMotor.setDirectionOut();
            spos++;
            lastPhysicalMove=millis();
          } else
          if ((spos>(long)target.part.m) && (spos>smin)) {
            if (reverse) dcMotor.setDirectionOut(); else dcMotor.setDirectionIn();
            spos--;
            lastPhysicalMove=millis();
          }
        }
      }

      if (moving()) {
        if (phase1) dcMotor.setPhase1(); else dcMotor.setPhase2();
        dcMotor.enabled(true);
        dcMotor.poll();
        lastPollingTime=millis();
        wasMoving=true;
      } else if (wasMoving) if ((long)(tempMs-lastPollingTime) > maxRate+1) { dcMotor.enabled(false); wasMoving=false; }
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

    // parameters
    int nvAddress=-1;
    long maxRate=-1;
    long spsMax=-1;
    long umin=-6L*25400L;
    long smin=-1;
    long umax=+6L*25400L;
    long smax=-1;
    bool reverse=false;
    bool phase1=true;

    // conversion
    double spm=1.0;
    double powerFor1mmSec=0.0;

    // position
    fixed_t target;
    long spos=0;
    long lastPos=0;

    // automatic movement
    double moveRate=0.0;
    fixed_t delta;
    bool wasMoving=false;

    // timing
    unsigned long lastMs=0;
    unsigned long lastMove=0;
    unsigned long lastPhysicalMove=0;
    unsigned long nextPhysicalMove=0;
    unsigned long lastPollingTime=0;
};
