// -----------------------------------------------------------------------------------
// Focusers

#pragma once

// time to write position to nv after last movement of Focuser 1/2, default = 5 minutes
#define FOCUSER_WRITE_DELAY 1000L*60L*5L

#include "Focuser.h"

class focuserStepper : public focuser {
  public:
    void init(int stepPin, int dirPin, int enPin, int nvAddress, int nvTcfCoef, int nvTcfEn, float maxRate, double stepsPerMicro, double min, double max, double minRate) {
      this->stepPin=stepPin;
      this->dirPin=dirPin;
      this->enPin=enPin;
      this->nvAddress=nvAddress;
      this->nvTcfCoef=nvTcfCoef;
      this->nvTcfEn=nvTcfEn;
      this->minRate=minRate;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;

      if (stepPin != -1) pinMode(stepPin,OUTPUT);
      if (dirPin != -1) pinMode(dirPin,OUTPUT);

      // get the temperature compensated focusing settings
      float coef=nv.readFloat(nvTcfCoef);
      if (fabs(coef) > 1000.0) { coef=0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfCoef > 1000 um/deg. C (set to 0.0)"); }
      setTcfCoef(coef);

      int tcfEn=nv.read(nvTcfEn);
      if (tcfEn != true && tcfEn != false) { tcfEn=false; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV tcfEn (set to false)"); }
      setTcfEnable(tcfEn);
      
      // get step position
      spos=readPos();
      long lmin=(long)(min*spm); if (spos < lmin) { spos=lmin; DLF("WRN, foc.init(): bad NV position < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      long lmax=(long)(max*spm); if (spos > lmax) { spos=lmax; DLF("WRN, foc.init(): bad NV position > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      target.part.m=spos; target.part.f=0;
      lastPos=spos;
      delta.fixed=0;

      // set min/max
      setMin(lmin);
      setMax(lmax);

      // steps per second, maximum
      spsMax=(1.0/maxRate)*1000.0;
      // microns per second default
      setMoveRate(100);

      nextPhysicalMove=micros()+(unsigned long)(maxRate*1000.0);
      lastPhysicalMove=nextPhysicalMove;
    }

    // temperature compensation
    void setTcfCoef(double coef) { if (fabs(coef) >= 1000.0) coef = 0.0; tcf_coef = coef; nv.writeFloat(nvTcfCoef,tcf_coef); }
    double getTcfCoef() { return tcf_coef; }
    void setTcfEnable(bool enabled) { tcf = enabled; nv.write(nvTcfEn,tcf); }
    bool getTcfEnable() { return tcf; }

    // sets logic state for reverse motion
    void setReverseState(int reverseState) {
      this->reverseState=reverseState;
      if (reverseState == LOW) forwardState=HIGH; else forwardState=LOW;
    }

    // sets logic state for disabling stepper driver
    void setDisableState(bool disableState) {
      this->disableState=disableState;
      if (disableState == LOW) enableState=HIGH; else enableState=LOW;
      if (enPin != -1) { pinMode(enPin,OUTPUT); enableDriver(); currentlyDisabled=false; }
    }

    // allows enabling/disabling stepper driver
    void powerDownActive(bool active) {
      if (enPin == -1) { pda=false; return; }
      pda=active;
      if (pda) { pinMode(enPin,OUTPUT); disableDriver(); currentlyDisabled=true; }
    }

    // set movement rate in microns/second, from minRate to 1000
    void setMoveRate(double rate) {
      rate = constrain(rate,minRate,1000);
      moveRate=rate*spm;                            // in steps per second
      if (moveRate > spsMax) moveRate=spsMax;       // limit to maxRate
    }

    // move in
    void startMoveIn() {
      delta.fixed=doubleToFixed(+moveRate/100.0);   // in steps per centi-second
    }

    // move out
    void startMoveOut() {
      delta.fixed=doubleToFixed(-moveRate/100.0);   // in steps per centi-second
    }

    // sets target position in steps
    void setTarget(long pos) {
      target.part.m=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
    }

    // sets target relative position in steps
    void relativeTarget(long pos) {
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

      // if enabled and the timeout has elapsed, disable the stepper driver
      if (pda && !currentlyDisabled && ((long)(micros()-lastPhysicalMove) > 10000000L)) { disableDriver(); currentlyDisabled=true; }
    
      // write position to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
      if ((spos != lastPos)) { lastMove=millis(); lastPos=spos; }
      if (!slewing && (spos != readPos())) {
        // needs updating and enough time has passed?
        if ((long)(millis()-lastMove) > FOCUSER_WRITE_DELAY) writePos(spos);
      }

      // temperature compensation
      long tcfSteps;
      if (tcf) {
        tcfSteps = -round((tcf_coef * (ambient.getTelescopeTemperature() - 10.0)) * spm);
      } else {
        tcfSteps = 0;
      }

      unsigned long microsNow=micros();
      if ((long)(microsNow-nextPhysicalMove) > 0) {
        nextPhysicalMove=microsNow+(unsigned long)(maxRate*1000.0);
    
        if ((spos < (long)target.part.m + tcfSteps) && (spos < smax)) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,forwardState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH); spos++;
          lastPhysicalMove=micros();
        } else
        if ((spos > (long)target.part.m + tcfSteps) && (spos > smin)) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,reverseState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH); spos--;
          lastPhysicalMove=micros();
        }
      }
    }

  private:

    void enableDriver() {
      if (enPin == -1) return;
      digitalWrite(enPin,enableState); delayMicroseconds(20);
    }

    void disableDriver() {
      if (enPin == -1) return;
      digitalWrite(enPin,disableState); delayMicroseconds(20);
    }
};
