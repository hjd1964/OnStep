// -----------------------------------------------------------------------------------
// Focusers

#pragma once

#include "Focuser.h"

class focuserStepper : public focuser {
  public:
    void init(int stepPin, int dirPin, int enPin, int nvAddress, float maxRate, double stepsPerMicro, double min, double max, double minRate) {
      this->stepPin=stepPin;
      this->dirPin=dirPin;
      this->enPin=enPin;
      this->nvAddress=nvAddress;
      this->minRate=minRate;
      this->maxRate=maxRate;
      this->spm=stepsPerMicro;

      // default min/max (in microns)
      setMin(min*spm);
      setMax(max*spm);
      
      pinMode(stepPin,OUTPUT);
      pinMode(dirPin,OUTPUT);

      // get the temperature compensated focusing settings
      float coef=nv.readFloat(nvAddress+EE_tcfCoef);
      if (fabs(coef) > 999.0) { coef=0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfCoef > 999.0 um/deg. C (set to 0.0)"); }
      setTcfCoef(coef);

      int deadband=nv.readInt(nvAddress+EE_tcfDeadband);
      if (deadband < 1)     { deadband=1; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfDeadband < 1 steps (set to 1)"); }
      if (deadband > 32767) { deadband=1; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfDeadband > 32767 steps (set to 1)"); }
      setTcfDeadband(deadband);

      tcf_t0=nv.readFloat(nvAddress+EE_tcfT0);
      if (tcf_t0 < -100.0) { tcf_t0=10.0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfT0 < -100.0 deg. C (set to 10.0)"); }
      if (tcf_t0 >  150.0) { tcf_t0=10.0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV TcfT0 >  150.0 deg. C (set to 10.0)"); }

      int tcfEn=nv.read(nvAddress+EE_tcfEn);
      if (tcfEn != true && tcfEn != false) { tcfEn=false; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV tcfEn (set to false)"); }

      // get backlash amount
      int b=nv.readInt(nvAddress+EE_focBacklash);
      if (b < 0) { b=0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV focBacklash < 0 steps (set to 0)"); }
      setBacklash(b);

      // get backlash position
      backlashPos=nv.readInt(nvAddress+EE_focBacklashPos);
      if (backlashPos < 0)           { backlashPos=0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV focBacklash < 0 steps (set to 0)"); }
      if (backlashPos > backlashMax) { backlashPos=0; generalError=ERR_NV_INIT; DLF("ERR, foc.init(): bad NV focBacklash > 1/10 full range (set to 0)"); }

      // get step position
      spos=nv.readLong(nvAddress+EE_focSpos);
      if (spos < smin) { spos=smin; DLF("WRN, foc.init(): bad NV position < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      if (spos > smax) { spos=smax; DLF("WRN, foc.init(): bad NV position > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      lastPos=spos+backlashPos;

      // get target position
      target.part.m=nv.readLong(nvAddress+EE_focTarget); target.part.f=0;
      if ((long)target.part.m < smin) { target.part.m=smin; DLF("WRN, foc.init(): bad NV target < _LIMIT_MIN (set to _LIMIT_MIN)"); }
      if ((long)target.part.m > smax) { target.part.m=smax; DLF("WRN, foc.init(): bad NV target > _LIMIT_MAX (set to _LIMIT_MAX)"); }
      lastTarget=target.part.m;

      // prepare for movement
      delta.fixed=0;
      spsMax=(1.0/maxRate)*1000.0; // steps per second, maximum
      setMoveRate(100);            // microns per second default
      nextPhysicalMove=micros()+(unsigned long)(maxRate*1000.0);
      lastPhysicalMove=nextPhysicalMove;
    }

    // temperature compensation coefficient in um/deg.C
    bool setTcfCoef(double coef) { if (fabs(coef) >= 1000.0) return false; tcf_coef = coef; nv.writeFloat(nvAddress+EE_tcfCoef,tcf_coef); return true; }

    // temperature compensation deadband in um
    bool setTcfDeadband(int deadband) { if (deadband < 1 || deadband > 32767) return false; tcf_deadband = deadband; nv.writeInt(nvAddress+EE_tcfDeadband,tcf_deadband); return true; }

    // temperature compensation deadband enable/disable
    bool setTcfEnable(bool enabled) {
      if (tcf != enabled) {
        if (enabled) {
          tcf_t0=ambient.getTelescopeTemperature();
          nv.writeFloat(nvAddress+EE_tcfT0,tcf_t0);
        } else {
          target.part.m=(long)target.part.m+getTcfSteps();
        }
        tcf = enabled; nv.write(nvAddress+EE_tcfEn,tcf);
      }
      return true;
    }

    double getTcfT0() {
      return tcf_t0;
    }

    // set backlash, in steps
    bool setBacklash(int b) { if (b < 0 || b > backlashMax) return false; backlash = b; nv.writeInt(nvAddress+EE_focBacklash,backlash); return true; }

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
    void powerDownActive(bool active, bool startupOnly) {
      if (enPin == OFF || enPin == SHARED || startupOnly) active=false;
      pda=active;
      this->startupOnly=startupOnly;
      if (pda || startupOnly) { pinMode(enPin,OUTPUT); disableDriver(); currentlyDisabled=true; }
    }

    // set movement rate in microns/second, from minRate to 1000
    void setMoveRate(double rate) {
      rate = constrain(rate,minRate,1000);
      moveRate=rate*spm;                            // in steps per second
      if (moveRate > spsMax) moveRate=spsMax;       // limit to maxRate
    }

    // move in
    void startMoveIn() {
      if (!movementAllowed()) return;
      if (startupOnly && currentlyDisabled) { enableDriver(); currentlyDisabled=false; }
      delta.fixed=doubleToFixed(+moveRate/100.0);   // in steps per centi-second
    }

    // move out
    void startMoveOut() {
      if (!movementAllowed()) return;
      if (startupOnly && currentlyDisabled) { enableDriver(); currentlyDisabled=false; }
      delta.fixed=doubleToFixed(-moveRate/100.0);   // in steps per centi-second
    }

    // check if moving
    bool moving() { if (delta.fixed != 0 || spos != (long)target.part.m) return true; else return false; }

    // sets target position in steps
    bool setTarget(long pos) {
      if (!movementAllowed()) return false;
      if (startupOnly && currentlyDisabled) { enableDriver(); currentlyDisabled=false; }
      target.part.m=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
      return true;
    }

    // sets target relative position in steps
    void relativeTarget(long pos) {
      if (!movementAllowed()) return;
      if (startupOnly && currentlyDisabled) { enableDriver(); currentlyDisabled=false; }
      target.part.m+=pos; target.part.f=0;
      if ((long)target.part.m < smin) target.part.m=smin; if ((long)target.part.m > smax) target.part.m=smax;
    }
    
    // do automatic movement
    void poll() {
      if (!movementAllowed()) return;
      target.fixed+=delta.fixed;
      // stop at limits
      if (((long)target.part.m < smin) || ((long)target.part.m > smax)) delta.fixed=0;
    }

    void follow(bool mountSlewing) {
      if (!movementAllowed()) return;

      // if enabled and the timeout has elapsed, disable the stepper driver
      if (pda && !currentlyDisabled && ((long)(micros()-lastPhysicalMove) > FOCUSER_POWER_DOWN_DELAY*1000L)) { disableDriver(); currentlyDisabled=true; }

      unsigned long microsNow=micros();
      if ((long)(microsNow-nextPhysicalMove) > 0) {
        nextPhysicalMove=microsNow+(unsigned long)(maxRate*1000.0);

        // keep track of when motion starts and stops
        if (target.part.m != lastTarget) { inMotion=true; lastTarget=target.part.m; }
        if (spos+backlashPos != lastPos) { lastMoveMs=millis(); lastPos=spos+backlashPos; }
        if ((long)(millis()-lastMoveMs) > 1000) inMotion=false;
  
        // write position as needed to non-volatile storage if not moving for FOCUSER_WRITE_DELAY milliseconds
        if (moving()) sinceMovingMs=millis();
        if (!mountSlewing && (long)(millis()-sinceMovingMs) > FOCUSER_WRITE_DELAY) writeTarget();

        if ((spos < (long)target.part.m+getTcfSteps() && spos < smax) || backlashDir == BD_OUT) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,forwardState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH);
          if (backlashPos < backlash) { backlashPos++; backlashDir=BD_OUT; } else { spos++; backlashDir=BD_NONE; }
          lastPhysicalMove=micros();
        } else
        if ((spos > (long)target.part.m+getTcfSteps() && spos > smin) || backlashDir == BD_IN) {
          if (pda && currentlyDisabled) { enableDriver(); currentlyDisabled=false; delayMicroseconds(5); }
          digitalWrite(stepPin,LOW); delayMicroseconds(5);
          digitalWrite(dirPin,reverseState); delayMicroseconds(5);
          digitalWrite(stepPin,HIGH);
          if (backlashPos > 0) { backlashPos--; backlashDir=BD_IN; } else { spos--; backlashDir=BD_NONE; }
          lastPhysicalMove=micros();
        }
      }
    }

  private:

    void enableDriver() {
      if (enPin == OFF || enPin == SHARED) return;
      digitalWrite(enPin,enableState); delayMicroseconds(20);
    }

    void disableDriver() {
      if (enPin == OFF || enPin == SHARED) return;
      digitalWrite(enPin,disableState); delayMicroseconds(20);
    }

    bool startupOnly = false;
};
