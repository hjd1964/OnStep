// -----------------------------------------------------------------------------------
// Focusers

#pragma once

// time to write position to nv after last movement of Focuser 1/2, default = 5 minutes
#define FOCUSER_WRITE_DELAY 1000L*60L*5L

#include "focuser.h"
#include "StepperDC.h"

class focuserDC : public focuser  {
  public:
    void init(int stepPin, int dirPin, int enPin, int nvAddress, int nvTcfCoef, int nvTcfEn, float maxRate, double stepsPerMicro, double min, double max, double minRate) {

      // init only happens once, on the first call and is ignored otherwise
      // 10 = 100ms pwm frequency
      dcMotor.init(stepPin,dirPin,enPin,10);
      
      this->nvAddress=nvAddress;
      this->minRate=minRate;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;
    
      spos=readPos();
      // constrain step position
      long lmin=(long)(min*spm); if (spos < lmin) { spos=lmin; DLF("WRN, foc.init(): bad NV position < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      long lmax=(long)(max*spm); if (spos > lmax) { spos=lmax; DLF("WRN, foc.init(): bad NV position > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      target.part.m=spos; target.part.f=0;
      lastPos=spos;
      delta.fixed=0;

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // microns per second default
      setMoveRate(500);

      // default min/max
      setMin(lmin);
      setMax(lmax);

      nextPhysicalMove=millis()+(unsigned long)maxRate;
      lastPhysicalMove=nextPhysicalMove;
    }

    // DC motor control
    bool isDcFocuser() { return true; }
    void initDcPower(int nvDcPower) { this->nvDcPower=nvDcPower; powerFor1mmSec=nv.read(nvDcPower); }
    void setDcPower(byte power) { this->powerFor1mmSec=power; nv.write(nvDcPower,powerFor1mmSec); }
    byte getDcPower() { return powerFor1mmSec; }
    void setPhase1() { phase1=true; }
    void setPhase2() { phase1=false; }
    
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
    void setTarget(long pos) {
      dcMotor.setPower((moveRate/1000.0)*powerFor1mmSec);
      target.part.m=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
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

    void follow(bool slewing) {
          
      // write position to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
      if ((spos != lastPos)) { lastMove=millis(); lastPos=spos; }
      if (!slewing && (spos != readPos())) {
        // needs updating and enough time has passed?
        if ((long)(millis()-lastMove) > FOCUSER_WRITE_DELAY) writePos(spos);
      }

      unsigned long tempMs=millis();
      if ((long)(tempMs-nextPhysicalMove) > 0) {
        nextPhysicalMove=tempMs+(unsigned long)maxRate;
        if (moving()) {
          if ((spos < (long)target.part.m) && (spos < smax)) {
            if (reverse) dcMotor.setDirectionIn(); else dcMotor.setDirectionOut();
            spos++;
            lastPhysicalMove=millis();
          } else
          if ((spos > (long)target.part.m) && (spos > smin)) {
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
};
