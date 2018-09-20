// -----------------------------------------------------------------------------------------------------------------------------
// Weather related functions

#pragma once

#ifdef WEATHER_BME280_ON
#include <Adafruit_BME280.h> // https://github.com/adafruit/Adafruit_BME280_Library and https://github.com/adafruit/Adafruit_Sensor
Adafruit_BME280 bme;
#endif

class weather {
  public:
    void init() {
#ifdef WEATHER_BME280_ON
      if (bme.begin(&HAL_Wire)) _disabled=false;
      if (!_disabled) {
        _t=bme.readTemperature();
        _p=bme.readPressure()/100.0;
        _h=bme.readHumidity();
      }
#ifdef ESP32
      HAL_Wire.end();
#endif
#endif
    }

    // designed for a 1s polling interval to refresh readings once a minute
    void poll() {
#ifdef WEATHER_BME280_ON
      if (!_disabled) {
        static int phase=0;
#ifdef ESP32
        if ((phase==10) || (phase==30) || (phase==50)) HAL_Wire.begin();
#endif
        if (phase==10) _t=bme.readTemperature();
        if (phase==30) _p=bme.readPressure()/100.0;
        if (phase==50) _h=bme.readHumidity();
#ifdef ESP32
        if ((phase==10) || (phase==30) || (phase==50)) HAL_Wire.end();  
#endif
        phase++; if (phase==60) phase=0;
      }
#endif
    }

    // get temperature in deg. C
    double getTemperature() {
      return _t;
    }

    // set temperature in deg. C
    void setTemperature(double t) {
      _t=t;
    }
    
    // get barometric pressure in hPa/mb
    double getPressure() {
      return _p;
    }

    // get barometric pressure in hPa/mb
    void setPressure(double p) {
      _p=p;
    }

    // get relative humidity in %
    double getHumidity() {
      return _h;
    }

    // set relative humidity in %
    void setHumidity(double h) {
      _h=h;
    }

    // get altitude in meters
    double getAltitude() {
      return _a;
    }

    // set altitude in meters
    void setAltitude(double a) {
      _a=a;
    }
    
    // get dew point in deg. C
    // accurate to +/- 1 deg. C for RH above 50%
    double getDewPoint() {
      return _t-((100.0-_h)/5.0);
    }

  private:
    bool _disabled = true;
    double _t = 10.0;
    double _p = 1010.0;
    double _h = 70.0;
    double _a = 200.0;
};

