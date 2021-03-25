// -----------------------------------------------------------------------------------
// Focusers

#pragma once

#include "Focuser.h"
#include "StepperDC.h"

class focuserDC : public focuser  {
  public:
    void init(int stepPin, int dirPin, int enPin, int nvAddress, float maxRate, double stepsPerMicro, double min, double max, double minRate) {

      // init only happens once, on the first call and is ignored otherwise
      // 10 = 100ms pwm frequency
      dcMotor.init(stepPin,dirPin,enPin,10);
      
      this->nvAddress=nvAddress;
      this->minRate=minRate;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;

      // set smin/smax
      setMin(min*spm);
      setMax(max*spm);

      // get DC power level
      powerFor1mmSec=nv.read(nvAddress+EE_focDcPwr);
    
      // get step position
      spos=nv.readLong(nvAddress+EE_focSpos);
      if (spos < smin) { spos=smin; DLF("WRN, foc.init(): bad NV position < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      if (spos > smax) { spos=smax; DLF("WRN, foc.init(): bad NV position > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      lastPos=spos;

      // get target position
      target.part.m=nv.readLong(nvAddress+EE_focTarget); target.part.f=0;
      if ((long)target.part.m < smin) { target.part.m=smin; DLF("WRN, foc.init(): bad NV target < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      if ((long)target.part.m > smax) { target.part.m=smax; DLF("WRN, foc.init(): bad NV target > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      lastTarget=target.part.m;

      delta.fixed=0;
      spsMax=(1.0/maxRate)*1000.0; // steps per second, maximum
      setMoveRate(500);            // microns per second default
      nextPhysicalMove=millis()+(unsigned long)maxRate;
      lastPhysicalMove=nextPhysicalMove;
    }

    // DC motor control
    bool isDcFocuser() { return true; }
    bool setDcPower(byte power) { powerFor1mmSec=power; nv.write(nvAddress+EE_focDcPwr,powerFor1mmSec); return true; }
    byte getDcPower() { return powerFor1mmSec; }
    bool setPhase1() { phase1=true; return true; }
    bool setPhase2() { phase1=false; return true; }
    void powerDownActive(bool active, bool startupOnly) { }
    
    // sets logic state for reverse motion
    void setReverseState(int reverseState) {
      if (reverseState == HIGH) reverse=true; else reverse=false;
    }

    // sets logic state for disabling stepper driver
    void setDisableState(bool disableState) {
      dcMotor.setDisableState(disableState);
    }

    // set movement rate in microns/second, from minRate to 1000
    void setMoveRate(double rate) {
      rate = constrain(rate,minRate,1000);
      moveRate=rate*spm;                            // in steps per second, for a DC motor a step is 1 micron.
      if (moveRate > spsMax) moveRate=spsMax;       // limit to maxRate
    }

    // move in
    void startMoveIn() {
      // rate is some fraction of 1 millimeter per second so this is the % power for 1 millimeter per second motion
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      delta.fixed=doubleToFixed(+moveRate/100.0);   // in steps per centi-second
    }
    
    // move out
    void startMoveOut() {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      delta.fixed=doubleToFixed(-moveRate/100.0);   // in steps per centi-second
    }

    // sets target position in steps
    bool setTarget(long pos) {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      target.part.m=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
      return true;
    }

    // sets target relative position in steps
    void relativeTarget(long pos) {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      target.part.m+=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
    }

    // do automatic movement
    void poll() {
      target.fixed+=delta.fixed;
      // stop at limits
      if (((long)target.part.m < smin) || ((long)target.part.m > smax)) delta.fixed=0;
    }

    void follow(bool mountSlewing) {
          
      unsigned long millisNow=millis();
      if ((long)(millisNow-nextPhysicalMove) > 0) {
        nextPhysicalMove=millisNow+(unsigned long)maxRate;

        // keep track of when motion starts and stops
        if (target.part.m != lastTarget) lastTarget=target.part.m;
        if (delta.fixed != 0 || ((long)target.part.m != spos)) inMotion=true; else inMotion=false;
   
        // write position as needed to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
        if (moving()) sinceMovingMs=millis();
        if (!mountSlewing && (long)(millis()-sinceMovingMs) > FOCUSER_WRITE_DELAY) writeTarget();

        if ((spos < (long)target.part.m) && spos < smax) {
          if (reverse) dcMotor.setDirectionIn(); else dcMotor.setDirectionOut();
          spos++;
          lastPhysicalMove=millis();
        } else
        if ((spos > (long)target.part.m) && spos > smin) {
          if (reverse) dcMotor.setDirectionOut(); else dcMotor.setDirectionIn();
          spos--;
          lastPhysicalMove=millis();
        }
      }

      if (moving()) {
        if (phase1) dcMotor.setPhase1(); else dcMotor.setPhase2();
        dcMotor.enabled(true);
        dcMotor.poll();
        lastPollingTime=millis();
        wasMoving=true;
      } else if (wasMoving) if ((long)(millisNow-lastPollingTime) > maxRate+1) { dcMotor.enabled(false); wasMoving=false; }
    }

  private:
    bool wasMoving=false;

};
