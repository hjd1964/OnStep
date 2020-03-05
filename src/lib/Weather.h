// -----------------------------------------------------------------------------------------------------------------------------
// Weather related functions

#pragma once

#include "Heater.h"

#if WEATHER != OFF
  #include <Adafruit_BME280.h>            // https://github.com/adafruit/Adafruit_BME280_Library/tree/156a0537d6b21aaab1d1f104a7001a38ca1ffce3
                                          // and https://github.com/adafruit/Adafruit_Sensor
  #if WEATHER == BME280 || WEATHER == BME280_0x76
    Adafruit_BME280 bme;
  #elif WEATHER == BME280_SPI
    Adafruit_BME280 bme(BME280_CS_PIN);                                   // hardware SPI
  //Adafruit_BME280 bme(BME280_CS_PIN, SSPI_MOSI, SSPI_MISO, SSPI_SCK);   // software SPI
  #endif
#endif

#ifdef ONEWIRE_DEVICES_PRESENT
  #include <OneWire.h>                    // my modified OneWire library https://github.com/hjd1964/OneWire
  OneWire oneWire(OneWirePin);

  #include <DallasTemperature.h>          // my modified DallasTemperature library https://github.com/hjd1964/Arduino-Temperature-Control-Library
  DallasTemperature DS18B20(&oneWire);

  // DEW_HEATER pin pointing at a DS serial number?
  #ifdef DS2413_DEVICES_PRESENT
    #include <DallasGPIO.h>               // my modified DallasGPIO library https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
    DallasGPIO DS2413GPIO(&oneWire);
  #endif
#endif

class weather {
  public:
    bool init() {
      bool success = true;
#ifdef ONEWIRE_DEVICES_PRESENT
      uint8_t address[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

      // clear address arrays
      for (int i=0; i<4; i++) for (int j=0; j<8; j++) _DS1820_address[i][j] = 0;
    
      // scan the OneWire bus and record the devices found
      oneWire.reset_search();
      while (oneWire.search(address)) {
        if (oneWire.crc8(address, 7) == address[7]) {
          if (DS18B20.validFamily(address)) {
            for (int j=0; j<8; j++) _DS1820_address[_DS1820_devices][j]=address[j];
            _DS1820_devices++;
//            Serial.println("Found a DS18B20!");
          }
  #ifdef DS2413_DEVICES_PRESENT
          if (DS2413GPIO.validFamily(address)) {
            for (int j=0; j<8; j++) _DS2413_address[_DS2413_devices][j]=address[j];
            _DS2413_devices++;
//            Serial.println("Found a DS2413!");
          }
  #endif
        }
      }

      // don't sit and wait for conversion to complete
      DS18B20.setWaitForConversion(false);

      if (_DS1820_devices > 0) _DS1820_found = true;
      if (_DS2413_devices > 0) _DS2413_found = true;
#endif
#if WEATHER != OFF
  #if WEATHER == BME280
      if (bme.begin(&HAL_Wire)) _BME280_found = true; else success = false;
  #elif WEATHER == BME280_0x76
      if (bme.begin(0x76, &HAL_Wire)) _BME280_found = true; else success = false;
  #else
      if (bme.begin()) _BME280_found = true; else success = false;
  #endif
  #if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.end();
  #endif
#endif
      return success;
    }

    // designed for a 1s polling interval
    void poll() {
#if WEATHER != OFF || defined(ONEWIRE_DEVICES_PRESENT)
      if (_BME280_found || _DS1820_found || _DS2413_found) {
        static int phase = 0;
        bool advance = true;
        double f;

  #if WEATHER != OFF
        if (_BME280_found) {
    #ifdef ESP32
          if (phase == 4 || phase == 8 || phase == 12) HAL_Wire.begin();
    #endif
          if (phase == 4) _t = bme.readTemperature();
          if (phase == 8) _p = bme.readPressure() / 100.0;
          if (phase == 12) _h = bme.readHumidity();
    #if defined(ESP32) & defined(WIRE_END_SUPPORT)
          if (phase == 4 || phase == 8 || phase == 12) HAL_Wire.end();
    #endif
        }
  #endif

  #ifdef ONEWIRE_DEVICES_PRESENT
        if (_DS2413_found) {
    #if DEW_HEATER1 == DS2413 || DEW_HEATER2 == DS2413
//          if (phase == 49) { Serial.print("ds2413 channel 1 and 2>"); }
          if (phase == 50) { if (!DS2413GPIO.setStateByAddress(_DS2413_address[_DS2413_count],_dh_state[1],_dh_state[0],true)) advance=false; else _DS2413_count++; }
//          if (phase == 51) Serial.println("<");
    #endif
    #if DEW_HEATER3 == DS2413 || DEW_HEATER4 == DS2413
//          if (phase == 59) { Serial.print("ds2413 channel 3 and 4>"); }
          if (phase == 60) { if (!DS2413GPIO.setStateByAddress(_DS2413_address[_DS2413_count],_dh_state[3],_dh_state[2],true)) advance=false; else _DS2413_count++; }
//          if (phase == 61) Serial.println("<");
    #endif
        }

        if (_DS1820_found) {
          if (phase == 0) { advance=DS18B20.requestTemperatures(true); _DS1820_count = 0; _DS2413_count = 0; }
    #if TELESCOPE_TEMPERATURE != OFF
          if (phase == 100) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) advance=false; else { _DS1820_count++; if (Tvalid(f)) _tt=f; else _tt=100.0; } }
//          if (phase == 101) { Serial.print("tt="); Serial.println(_tt); }
    #endif
    #if DEW_HEATER1_TEMPERATURE != OFF
          if (phase == 104) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) advance=false; else { _DS1820_count++; if (Tvalid(f)) _dh_t[0]=f; else _dh_t[0]=100.0; } }
//          if (phase == 105) { Serial.print("dh_t0="); Serial.println(_dh_t[0]); }
    #endif
    #if DEW_HEATER2_TEMPERATURE != OFF
          if (phase == 108) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) advance=false; else { _DS1820_count++; if (Tvalid(f)) _dh_t[1]=f; else _dh_t[1]=100.0; } }
//          if (phase == 109) { Serial.print("dh_t0="); Serial.println(_dh_t[0]); }
    #endif
    #if DEW_HEATER3_TEMPERATURE != OFF
          if (phase == 112) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) advance=false; else { _DS1820_count++; if (Tvalid(f)) _dh_t[2]=f; else _dh_t[2]=100.0; } }
    #endif
    #if DEW_HEATER4_TEMPERATURE != OFF
          if (phase == 116) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) advance=false; else { _DS1820_count++; if (Tvalid(f)) _dh_t[2]=f; else _dh_t[2]=100.0; } }
    #endif
        }
  #endif

        if (advance) phase++;
        if (phase == 120) phase = 0;
      }
#endif
    }

    // get temperature in deg. C
    double getTemperature() {
#if TELESCOPE_TEMPERATURE != OFF && WEATHER == OFF
          _t = _tt;
#endif
      return _t;
    }
    
    // get telescope temperature in deg. C
    double getTelescopeTemperature() {
#if TELESCOPE_TEMPERATURE == OFF && WEATHER != OFF
      _tt = _t;
#endif
      return _tt;
    }
    
    // get dew heater temperature in deg. C
    double getDewHeaterTemperature(int index) {
      return _dh_t[index];
    }
    
    // set temperature in deg. C
    void setTemperature(double t) {
      _t = t;
    }
    
    // get barometric pressure in hPa/mb
    double getPressure() {
      return _p;
    }
    
    // get barometric pressure in hPa/mb
    void setPressure(double p) {
      _p = p;
    }
    
    // get relative humidity in %
    double getHumidity() {
      return _h;
    }
    
    // set relative humidity in %
    void setHumidity(double h) {
      _h = h;
    }
    
    // get altitude in meters
    double getAltitude() {
      return _a;
    }
    
    // set altitude in meters
    void setAltitude(double a) {
      _a = a;
    }
    
    // get dew point in deg. C
    // accurate to +/- 1 deg. C for RH above 50%
    double getDewPoint() {
      return _t - ((100.0 - _h) / 5.0);
      // a more accurate formula?
      // return 243.04*(log(_h/100.0)+((17.625*_t)/(243.04+_t)))/(17.625-log(_h/100.0)-((17.625*_t)/(243.04+_t)));
    }

    // two DS2413 OneWire GPIO's are supported, this sets the outputs on or off
    void setDewHeaterState(int heaterNumber, bool state) {
      if (heaterNumber >= 1 && heaterNumber <= 4) _dh_state[heaterNumber-1]=state;
    }

  private:
    bool _BME280_found = false;

    bool _DS1820_found = false;
    int  _DS1820_devices = 0;
    int  _DS1820_count = 0;
    uint8_t _DS1820_address[4][8];

    bool _DS2413_found = false;
    int  _DS2413_devices = 0;
    int  _DS2413_count = 0;
    uint8_t _DS2413_address[2][8];

    double _t = 10.0;
    double _p = 1010.0;
    double _h = 70.0;
    double _a = 200.0;
    double _tt = 10.0;
    double _dh_t[4] = {10.0, 10.0, 10.0, 10.0};
    bool _dh_state[4] = {false, false, false, false};

#ifdef ONEWIRE_DEVICES_PRESENT
    bool Tpolling(double f) {
      return (fabs(f-DEVICE_POLLING_C) < 0.001);
    }
    bool Tvalid(double f) {
      return (f > DEVICE_DISCONNECTED_C);
    }
#endif

};
