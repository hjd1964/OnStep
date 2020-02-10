// -----------------------------------------------------------------------------------------------------------------------------
// Weather related functions

#pragma once

#if defined(WEATHER_BME280_ON) || defined(WEATHER_BME280) || defined(WEATHER_BME280SPI_ON) || defined(WEATHER_BME280SPI)
  #include <Adafruit_BME280.h> // https://github.com/adafruit/Adafruit_BME280_Library/tree/156a0537d6b21aaab1d1f104a7001a38ca1ffce3
                               // and https://github.com/adafruit/Adafruit_Sensor
  #if defined(WEATHER_BME280SPI_ON)
    Adafruit_BME280 bme(BME280_CS_PIN); // hardware SPI
//  Adafruit_BME280 bme(BME280_CS_PIN, SSPI_MOSI, SSPI_MISO, SSPI_SCK); // software SPI
  #elif defined(WEATHER_BME280SPI)
    Adafruit_BME280 bme(WEATHER_BME280SPI); // hardware SPI (user defined CS)
//  Adafruit_BME280 bme(WEATHER_BME280SPI, SSPI_MOSI, SSPI_MISO, SSPI_SCK); // software SPI (user defined CS)
  #else
    Adafruit_BME280 bme;
  #endif
#endif

#ifdef TEMPERATURE_DS1820_ON
  #include <OneWire.h>                    // added via built in Arduino IDE library manager
  #include <DallasTemperature.h>          // added via built in Arduino IDE library manager
  OneWire oneWire(OneWirePin);
  DallasTemperature DS18B20(&oneWire);
#endif

class weather {
  public:
    void init() {
#if defined(WEATHER_BME280_ON) || defined(WEATHER_BME280) || defined(WEATHER_BME280SPI_ON) || defined(WEATHER_BME280SPI)
      #if defined(WEATHER_BME280)
        if (bme.begin(WEATHER_BME280,&HAL_Wire)) _disabled=false;
      #elif defined(WEATHER_BME280_ON)
        if (bme.begin(&HAL_Wire)) _disabled=false;
      #else
        if (bme.begin()) _disabled=false;
      #endif
      if (!_disabled) {
        _t=bme.readTemperature();
        _p=bme.readPressure()/100.0;
        _h=bme.readHumidity();
      }
  #ifdef ESP32
      HAL_Wire.end();
  #endif
#endif
#ifdef TEMPERATURE_DS1820_ON
      _disabled=false;
      if (!_disabled) {
        DS18B20.requestTemperatures(); 
        _t=DS18B20.getTempCByIndex(0);
      }
#endif
    }

    // designed for a 1s polling interval to refresh readings once a minute
    void poll() {
#if defined(WEATHER_BME280_ON) || defined(WEATHER_BME280) || defined(WEATHER_BME280SPI_ON) || defined(WEATHER_BME280SPI)
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
#ifdef TEMPERATURE_DS1820_ON
      if (!_disabled) {
        static int phase=0;
        if (phase==10) { DS18B20.requestTemperatures(); _t=DS18B20.getTempCByIndex(0); } 
        if (phase==30) ;    //nothing to do here
        if (phase==50) ;    //nothing to do here
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
      // a more accurate formula?
      // return 243.04*(log(_h/100.0)+((17.625*_t)/(243.04+_t)))/(17.625-log(_h/100.0)-((17.625*_t)/(243.04+_t)));
    }

  private:
    bool _disabled = true;
    double _t = 10.0;
    double _p = 1010.0;
    double _h = 70.0;
    double _a = 200.0;
};
