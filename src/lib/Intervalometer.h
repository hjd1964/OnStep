// -----------------------------------------------------------------------------------
// Control a single dew Heater

#pragma once

class intervalometerControl {
  public:
    void init(int nvAddress) {
      _nvAddress = nvAddress;

      expTime = byteToTime(nv.read(_nvAddress));
      expDelay = byteToTime(nv.read(_nvAddress+1));
      expCount = nv.read(_nvAddress+2);
    }

    void poll() {
      if (!enabled) return;

      if (pressed == P_EXP_START) {
        // count and stop when done
        if (thisCount == 0) { pressed=P_STANDBY; enabled=false; return; }
        thisCount--;
        // start a new exposure
        pressed=P_EXP_DONE;
        expDone=millis()+(unsigned long)(expTime*1000.0); // set exposure time in ms
      } else 
      if (pressed == P_EXP_DONE && (long)(millis()-expDone) > 0) {
        // finish an exposure
        pressed=P_WAIT;
        waitDone=millis()+(unsigned long)(expDelay*1000.0); // set wait time in ms
      } else
      if (pressed == P_WAIT && (long)(millis()-waitDone) > 0) {
        // wait between exposures
        if (millis()-waitDone > 0) pressed=P_EXP_START;
      }
    }

    float getExposure() {
      return expTime;
    }
    void setExposure(float t) {
      if (pressed == P_STANDBY && t >= 0 && t <= 3600) {
        expTime = t;
        nv.write(_nvAddress,timeToByte(expTime));
      }
    }

    float getDelay() {
      return expDelay;
    }
    void setDelay(float t) {
      if (pressed == P_STANDBY && t >= 1 && t <= 3600) {
        expDelay = t;
        nv.write(_nvAddress+1,timeToByte(expDelay));
      }
    }

    float getCurrentCount() {
      return thisCount;
    }
    float getCount() {
      return expCount;
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
      if (enabled) { thisCount=expCount; pressed = P_EXP_START; } else { thisCount=0; pressed = P_STANDBY; }
    }

    bool isOn() {
      return pressed == P_EXP_DONE;
    }

  private:
    uint8_t timeToByte(float t) {
      float v = 10;                         // default is 1 second
      if (t <= 0.0162) v=0; else            // 0.0156 (1/64 second)        (0)
      if (t <= 0.0313) v=1; else            // 0.0313 (1/32 second)        (1)
      if (t <= 0.0625) v=2; else            // 0.0625 (1/16 second)        (2)
      if (t <= 1.0) v=2.0+t*8.0; else       // 0.125 seconds to 1 seconds  (2 to 10)
      if (t <= 10.0) v=6.0+t*4.0; else      // 0.25 seconds to 10 seconds  (10 to 46)
      if (t <= 30.0) v=26.0+t*2.0; else     // 0.5 seconds to 30 seconds   (46 to 86)
      if (t <= 120.0) v=56.0+t; else        // 1 second to 120 seconds     (86 to 176)
      if (t <= 600.0) v=168.0+t/15.0; else  // 15 seconds to 300 seconds   (176 to 208)
      if (t <= 3360.0) v=198.0+t/60.0; else // 1 minute to 56 minutes      (208 to 254)
      if (t <= 3600.0) v=255;               // 1 hour                      (255)
      if (v < 0) v=0;
      if (v > 255) v=255;
      return lround(v);
    }
    float byteToTime(uint8_t b) {
      float v = 1.0;                        // default is 1 second
      if (b == 0) v=0.016125; else          // 0.0156 (1/64 second)        (0)
      if (b == 1) v=0.03125; else           // 0.0313 (1/32 second)        (1)
      if (b == 2) v=0.0625; else            // 0.0625 (1/16 second)        (2)
      if (b <= 10) v=(b-2.0)/8.0; else      // 0.125 seconds to 1 seconds  (2 to 10)
      if (b <= 46) v=(b-6.0)/4.0; else      // 0.25 seconds to 10 seconds  (10 to 46)
      if (b <= 86) v=(b-26.0)/2.0; else     // 0.5 seconds to 30 seconds   (46 to 86)
      if (b <= 176) v=(b-56.0); else        // 1 second to 120 seconds     (86 to 176)
      if (b <= 208) v=(b-168.0)*15.0; else  // 15 seconds to 300 seconds   (176 to 208)
      if (b <= 254) v=(b-198.0)*60.0; else  // 1 minute to 56 minutes      (208 to 254)
      if (b == 255) v=3600.0;               // 1 hour                      (255)
      return v;
    }

    enum Pressed {P_STANDBY, P_EXP_START, P_EXP_START_PHASE2, P_EXP_DONE, P_EXP_DONE_PHASE2, P_WAIT};
    Pressed pressed = P_STANDBY;
    bool enabled = false;

    float expTime = 0;
    float expDelay = 0;
    int expCount = 0;
    int thisCount = 0;

    unsigned long expDone = 0;
    unsigned long waitDone = 0;

    int _nvAddress = 0;
};
