// -----------------------------------------------------------------------------------
// Control a single dew Heater

#pragma once

class intervalometerControl {
  public:
    void init(int pin, int nvAddress) {
      int i;
      _pin = pin;
      if (_pin >= 0 && _pin <= 255) pinMode(_pin, OUTPUT);
      _nvAddress = nvAddress;

      i = nv.read(_nvAddress); if (i > 180) expTime=180.0+(i-180.0)*15.0; else expTime=i;
      i = nv.read(_nvAddress+1); if (i > 180) expDelay=180.0+(i-180.0)*15.0; else expDelay=i;
      expCount = nv.read(_nvAddress+2);

    }

    void poll() {
      if (!enabled) return;

      if (pressed == P_START) {
        // count and stop when done
        thisCount--; if (thisCount < 0) { pressed=P_STANDBY; enabled=false; return; }
        // start a new exposure
        pressed=P_EXPOSE;
        if (_pin >= 0 && _pin <= 255) digitalWrite(_pin, HIGH);
        expDone=millis()+(unsigned long)(expTime*1000.0); // set exposure time in ms
      } else 
      if (pressed == P_EXPOSE && (long)(millis()-expDone) > 0) {
        // finish an exposure
        pressed=P_WAIT;
        if (_pin >= 0 && _pin <= 255) digitalWrite(_pin, LOW);
        waitDone=millis()+(unsigned long)(expDelay*1000.0); // set wait time in ms
      } else
      if (pressed == P_WAIT && (long)(millis()-waitDone) > 0) {
        // wait between exposures
        if (millis()-waitDone > 0) pressed=P_START;
      }
    }

    float getExposure() {
      return expTime;
    }
    void setExposure(float t) {
      if (pressed == P_STANDBY && t >= 0 && t <= 1305) {
        expTime = t;
        int i;
        if (expTime > 180) i=(expTime-180.0)/15.0+180.0; else i=expTime; nv.write(_nvAddress,i);
      }
    }

    float getDelay() {
      return expDelay;
    }
    void setDelay(float t) {
      if (pressed == P_STANDBY && t >= 1 && t <= 1305) {
        expDelay = t;
        int i;
        if (expDelay > 180) i=(expDelay-180.0)/15.0+180.0; else i=expDelay; nv.write(_nvAddress+1,i);
      }
    }

    float getCount() {
      if (pressed == P_STANDBY) return expCount; else return thisCount;
    }
    void setCount(float c) {
      if (pressed == P_STANDBY && c >= 0 && c <= 255) {
        expCount = c;
        nv.write(_nvAddress+2,expCount);
      }
    }

    bool isEnabled() {
      return enabled;
    }
    void enable(bool state) {
      enabled = state;
      if (_pin >= 0  && _pin <= 255) digitalWrite(_pin, LOW);
      if (enabled) { thisCount=expCount; pressed = P_START; } else { thisCount=0; pressed = P_STANDBY; }
    }

    bool isOn() {
      return pressed == P_EXPOSE;
    }

  private:
    enum Pressed {P_STANDBY, P_START, P_EXPOSE, P_WAIT};
    Pressed pressed = P_STANDBY;
    bool enabled = false;

    float expTime = 0;
    float expDelay = 0;
    int expCount = 0;
    int thisCount = 0;

    unsigned long expDone = 0;
    unsigned long waitDone = 0;

    int _pin = 0;
    int _nvAddress = 0;
};
