// -----------------------------------------------------------------------------------
// Focusers

#pragma once

class focuser {
  public:
    virtual void init(int stepPin, int dirPin, int enPin, int nvAddress, int nvTcfCoef, int nvTcfEn, float maxRate, double stepsPerMicro, double min, double max, double minRate) { }

    // DC motor control
    virtual bool isDcFocuser() { return false; }
    virtual void initDcPower(int nvDcPower) { }
    virtual void setDcPower(byte power) { }
    virtual byte getDcPower() { return 0; }
    virtual void setPhase1() { }
    virtual void setPhase2() { }

    // temperature compensation
    virtual void setTcfCoef(double coef) { }
    virtual double getTcfCoef() { return 0; }
    virtual void setTcfEnable(bool enabled) { }
    virtual bool getTcfEnable() { return false; }

    // get step size in microns
    virtual double getStepsPerMicro() { return spm; }

    // minimum position in steps
    void setMin(long min) { smin=min; }
    long getMin() { return smin; }

    // maximum position in steps
    void setMax(long max) { smax=max; }
    long getMax() { return smax; }

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
    bool moving() { if ((delta.fixed != 0) || ((long)target.part.m != spos)) return true; else return false; }

    // stop move
    void stopMove() { delta.fixed=0; target.part.m=spos; target.part.f=0; }

    // get position in steps
    long getPosition() { return spos; }

    // sets current position in steps
    void setPosition(long pos) {
      spos=pos;
      if (spos < smin) spos=smin; if (spos > smax) spos=smax;
      target.part.m=spos; target.part.f=0;
      lastMove=millis();
    }

    // sets target position in steps
    virtual void setTarget(long pos) { }

    // sets target relative position in steps
    virtual void relativeTarget(long pos) { }

    // do automatic movement
    virtual void move() { }

    // follow( (trackingState == TrackingMoveTo) || guideDirAxis1 || guideDirAxis2) );
    virtual void follow(bool slewing) { }

    void savePosition() { writePos(spos); }
  
  protected:
    long readPos() { return nv.readLong(nvAddress); }
    void writePos(long p) { nv.writeLong(nvAddress,(long)p); }
 
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
    int nvTcfCoef=-1;
    int nvTcfEn=-1;

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
