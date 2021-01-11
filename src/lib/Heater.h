// -----------------------------------------------------------------------------------
// Control a single dew Heater

#pragma once

#ifndef DEW_HEATER_PULSE_WIDTH_MS
  #define DEW_HEATER_PULSE_WIDTH_MS 2000
#endif

class dewHeaterControl {
  public:
    void init(int nvAddress) {
      _nvAddress = nvAddress;
      zero = nv.read(_nvAddress)/10.0 - 5.0;
      if (zero < -5.0) { zero = -5.0; DLF("ERR, dewHeater.init(): NV zero too low (set to -5.0)"); }
      if (zero > 20) { zero = 20.0; DLF("ERR, dewHeater.init(): NV zero too high (set to 20.0)"); }
      span = nv.read(_nvAddress + 1)/10.0 - 5.0;
      if (zero == -5.0 && span == -5.0) span=-4.9; // init. state is ok, no error or warning
      if (span < -5.0) { span = -5.0; DLF("ERR, dewHeater.init(): NV span too low (set to -5.0)"); }
      if (span > 20) { span = 20.0; DLF("ERR, dewHeater.init(): NV span too high (set to 20.0)"); }
      if (zero >= span) { if (span > -5.0) zero = span - 0.1; else span = zero + 0.1; DLF("ERR, dewHeater.init(): NV zero >= span (corrected)"); }
    }

    void poll(float deltaAboveDewPointC) {
      if (isnan(deltaAboveDewPointC)) { heaterOn=false; return; }
      
      if (!enabled) return;

      int switchTimeMs = 0;
      switchTimeMs = map(lround(deltaAboveDewPointC*10.0), lround(zero*10.0), lround(span*10.0), DEW_HEATER_PULSE_WIDTH_MS, 0);
      switchTimeMs = constrain(switchTimeMs, 0, DEW_HEATER_PULSE_WIDTH_MS);
#ifdef DEW_HEATER_MAX_POWER
      switchTimeMs = lround(switchTimeMs*(DEW_HEATER_MAX_POWER/100.0))
#endif
      currentTime = millis();

      if ((long)(currentTime - (lastHeaterCycle + DEW_HEATER_PULSE_WIDTH_MS)) > 0) {
        lastHeaterCycle = currentTime;
      } else
      if (!heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) <= 0) {
        heaterOn = true;
      } else
      if (heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) > 0) {
        heaterOn = false;
      }
    }

    float getZero() {
      return zero;
    }
    void setZero(float t) {
      if (t >= -5.0 && t <= 20.0) {
        zero = t;
        if (zero >= span) zero=span-0.1;
        nv.write(_nvAddress, round((zero + 5.0)*10.0));
      }
    }

    float getSpan() {
      return span;
    }
    void setSpan(float t) {
      if (t >= -5.0 && t <= 20.0) {
        span = t;
        if (span <= zero) span=zero+0.1;
        nv.write(_nvAddress + 1, round((span + 5.0)*10.0));
      }
    }

    bool isEnabled() {
      return enabled;
    }
    void enable(bool state) {
      heaterOn = false;
      enabled = state;
    }

    bool isOn() {
      return heaterOn;
    }

  private:
    unsigned long lastHeaterCycle = 0;
    unsigned long currentTime = 0;

    bool heaterOn = false;
    bool enabled = false;

    float zero = -5;
    float span = 15;

    int _nvAddress = 0;
};
