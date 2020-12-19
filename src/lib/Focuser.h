// -----------------------------------------------------------------------------------
// Focusers

#pragma once

// time to write position to nv after last movement of Focuser
#if NV_ENDURANCE == VHIGH
  #define FOCUSER_WRITE_DELAY 1000L*5L       // 5 seconds
#elif NV_ENDURANCE == HIGH
  #define FOCUSER_WRITE_DELAY 1000L*60L      // 1 minute
#else
  #define FOCUSER_WRITE_DELAY 1000L*60L*5L   // 5 minutes
#endif
#ifndef FOCUSER_POWER_DOWN_DELAY
  #define FOCUSER_POWER_DOWN_DELAY 10L*1000L // 10 seconds to power off, when the power down setting is active
#endif

class focuser {
  public:
    virtual void init(int stepPin, int dirPin, int enPin, int nvAddress, int nvTcfCoef, int nvTcfEn, float maxRate, double stepsPerMicro, double min, double max, double minRate) { }

    // DC motor control
    virtual bool isDcFocuser() { return false; }
    virtual void initDcPower(int nvDcPower) { }
    virtual bool setDcPower(byte power) { return false; }
    virtual byte getDcPower() { return 0; }
    virtual bool setPhase1() { return false; }
    virtual bool setPhase2() { return false; }

    // temperature compensation
    virtual bool setTcfCoef(double coef) { return false; }
    double getTcfCoef() { return tcf_coef; }
    virtual bool setTcfDeadband(int deadband) { return false; }
    virtual int getTcfDeadband() { return tcf_deadband; }
    virtual bool setTcfEnable(bool enabled) { return false; }
    bool getTcfEnable() { return tcf; }
    long getTcfSteps() {
      if (tcf) {
        float tt = ambient.getTelescopeTemperature();
        if (isnan(tt)) { tcf=false; return 0; }
        float tc = -round((tcf_coef * (tt - tcf_t0)) * spm);
        return lround(tc/(float)tcf_deadband)*(long)tcf_deadband;
      } else return 0;
    }
    virtual double getTcfT0() { return 0; }

    // get step size in microns
    virtual double getStepsPerMicro() { return spm; }

    // minimum position in steps
    void setMin(long min) { smin=min; if (smin < 0 || smin > 500L*1000L*10L) smin=0; if (smin > smax) smin=smax; backlashMax=(smax-smin)/10; if (backlashMax > 32767) backlashMax=32767; }
    long getMin() { return smin; }

    // maximum position in steps
    void setMax(long max) { smax=max; if (smax < 0 || smax > 500*1000L*10L) smax=0; if (smax < smin) smax=smin; backlashMax=(smax-smin)/10; if (backlashMax > 32767) backlashMax=32767; }
    long getMax() { return smax; }

    // backlash, in steps
    virtual bool setBacklash(int b) { return false; }
    virtual int getBacklash() { return backlash; }

    // sets logic state for reverse motion
    virtual void setReverseState(int reverseState) { }

    // sets logic state for disabling stepper driver
    virtual void setDisableState(bool disableState) { }

    // allows enabling/disabling stepper driver
    virtual void powerDownActive(bool active) { }

    // set movement rate in microns/second, from minRate to 1000
    virtual void setMoveRate(double rate) { }

    // move in
    virtual void startMoveIn() { }

    // move out
    virtual void startMoveOut() { }

    // check if moving
    virtual bool moving() { return inMotion; }

    // stop move
    void stopMove() { delta.fixed=0; }

    // get position in steps
    long getPosition() { return spos - getTcfSteps(); }

    // sets current position in steps
    void setPosition(long pos) {
      setTcfEnable(false);
      spos=pos;
      if (spos < smin) spos=smin; if (spos > smax) spos=smax;
      target.part.m=spos; target.part.f=0;
    }

    // sets target position in steps
    virtual bool setTarget(long pos) { return false; }

    // sets target relative position in steps
    virtual void relativeTarget(long pos) { }

    // do automatic movement, call this method at 100Hz
    virtual void poll() { }

    // follow( (trackingState == TrackingMoveTo) || guideDirAxis1 || guideDirAxis2) );
    virtual void follow(bool slewing) { }

    void savePosition() { writeTarget(); }
  
  protected:
    bool movementAllowed() {
      if (enPin == SHARED && !axis1Enabled) return false; else return true;
    }
    void writeTarget() {
      nv.writeLong(nvAddress+EE_focSpos,spos);
      nv.writeLong(nvAddress+EE_focTarget,(long)target.part.m);
      nv.writeInt(nvAddress+EE_focBacklashPos,backlashPos);
    }

    // parameters
    int stepPin=-1;
    int dirPin=-1;
    int enPin=-1;
    float minRate=-1;
    float maxRate=-1;
    long spsMax=-1;
    long smin=0;
    long smax=1000;
    int nvAddress=-1;
    
    bool reverse=false;
    bool phase1=true;
    long nvDcPower=-1;

    // state
    int reverseState=LOW;
    int forwardState=HIGH;
    bool pda=false;
    bool tcf=false;
    int disableState=LOW;
    int enableState=HIGH;
    bool currentlyDisabled=true;

    // conversion
    double spm=1.0;
    double tcf_coef=0.0;
    double tcf_t0=10.0;
    double powerFor1mmSec=0.0;

    // position
    fixed_t target;
    long spos=0;
    long lastPos=0;
    int backlash=0;
    int backlashPos=0;
    int backlashDir=0;
    long backlashMax=0;
    int tcf_deadband=1;

    // automatic movement
    double moveRate=0.0;
    fixed_t delta;
    bool inMotion=false;

    // timing
    unsigned long lastMoveMs=0;
    unsigned long sinceMovingMs=0;
    unsigned long lastTarget=0;
    unsigned long lastPhysicalMove=0;
    unsigned long nextPhysicalMove=0;
    unsigned long lastPollingTime=0;
};
