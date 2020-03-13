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

      // clear then pre-load any user defined DS1820 addresses
      _DS1820_devices=0;
      for (int i=0; i<5; i++) for (int j=0; j<8; j++) _DS1820_address[i][j] = 0;
      if (TELESCOPE_TEMPERATURE > DALLAS_LAST) { for (int j=0; j<8; j++) _DS1820_address[0][j] = (((uint64_t)TELESCOPE_TEMPERATURE) >> (j*8)) & 0xff; _DS1820_devices++; }
      if (DEW_HEATER1_TEMPERATURE > DALLAS_LAST) { for (int j=0; j<8; j++) _DS1820_address[1][j] = (((uint64_t)DEW_HEATER1_TEMPERATURE) >> (j*8)) & 0xff; _DS1820_devices++; }
      if (DEW_HEATER2_TEMPERATURE > DALLAS_LAST) { for (int j=0; j<8; j++) _DS1820_address[2][j] = (((uint64_t)DEW_HEATER2_TEMPERATURE) >> (j*8)) & 0xff; _DS1820_devices++; }
      if (DEW_HEATER3_TEMPERATURE > DALLAS_LAST) { for (int j=0; j<8; j++) _DS1820_address[3][j] = (((uint64_t)DEW_HEATER3_TEMPERATURE) >> (j*8)) & 0xff; _DS1820_devices++; }
      if (DEW_HEATER4_TEMPERATURE > DALLAS_LAST) { for (int j=0; j<8; j++) _DS1820_address[4][j] = (((uint64_t)DEW_HEATER4_TEMPERATURE) >> (j*8)) & 0xff; _DS1820_devices++; }

      // clear then pre-load any user defined DS2413 addresses
  #ifdef DS2413_DEVICES_PRESENT
      _DS2413_devices=0;
      for (int i=0; i<2; i++) for (int j=0; j<8; j++) _DS2413_address[i][j] = 0;
      if (DEW_HEATER1 > DALLAS_LAST && DEW_HEATER1 == DEW_HEATER2) { for (int j=0; j<8; j++) _DS2413_address[0][j] = (DEW_HEATER1 >> (j*8)) & 0xff; _DS2413_devices++; }
      if (DEW_HEATER3 > DALLAS_LAST && DEW_HEATER3 == DEW_HEATER4) { for (int j=0; j<8; j++) _DS2413_address[1][j] = (DEW_HEATER3 >> (j*8)) & 0xff; _DS2413_devices++; }
  #endif
  
      // scan the OneWire bus and record the devices found
      oneWire.reset_search();
      // only search out DS1820's or DS2413's IF none are explicitly specified
      bool searchDS1820 = _DS1820_devices == 0;
      bool searchDS2413 = _DS2413_devices == 0;
      while (oneWire.search(address)) {
        if (oneWire.crc8(address, 7) == address[7]) {
          if (DS18B20.validFamily(address) && searchDS1820) {
            if (_DS1820_devices <= 4) for (int j=0; j<8; j++) _DS1820_address[_DS1820_devices][j]=address[j];
            _DS1820_devices++;
//            Serial.println("Found a DS18B20!");
          }
  #ifdef DS2413_DEVICES_PRESENT
          if (DS2413GPIO.validFamily(address) && searchDS2413) {
            if (_DS2413_devices <= 1) for (int j=0; j<8; j++) _DS2413_address[_DS2413_devices][j]=address[j];
            _DS2413_devices++;
//            Serial.println("Found a DS2413!");
          }
  #endif
        }
      }

      // don't sit and wait for conversion to complete
      DS18B20.setWaitForConversion(false);
      DS18B20.requestTemperatures(false);

      if (_DS1820_devices > 0) _DS1820_found = true;
      if (_DS2413_devices > 0) _DS2413_found = true;
#endif
#if WEATHER != OFF
  #if WEATHER == BME280
      if (bme.begin(&HAL_Wire)) _BME280_found = true; else success = false;
  #elif WEATHER == BME280_0x76
      if (bme.begin(0x76, &HAL_Wire)) _BME280_found = true; else success = false;
  #else
      if (bme.begin()) _BME280_found = true; else { if (bme.begin()) _BME280_found = true; else success = false; }
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
        double f;

        static int phase = 0;
        if (phase >= 600) { phase = 0; _DS1820_count = 0; }

  #ifdef ONEWIRE_DEVICES_PRESENT
        if (_DS2413_found) {
      #if DEW_HEATER1 == DS2413 || DEW_HEATER2 == DS2413
          if (phase%2 == 1 && (_last_dh_state[1] != _dh_state[1] || _last_dh_state[0] != _dh_state[0])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[0],_dh_state[1],_dh_state[0],true)) { phase++; _last_dh_state[1] = _dh_state[1]; _last_dh_state[0] = _dh_state[0]; }
            return;
          }
      #endif
      #if DEW_HEATER3 == DS2413 || DEW_HEATER4 == DS2413
          if (phase%2 == 1 && (_last_dh_state[3] != _dh_state[3] || _last_dh_state[2] != _dh_state[2])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[1],_dh_state[3],_dh_state[2],true)) { phase++; _last_dh_state[3] = _dh_state[3]; _last_dh_state[2] = _dh_state[2]; }
            return;
          }
      #endif
        }

        if (_DS1820_found) {
          if (phase == 0) { if (DS18B20.requestTemperatures(true)) phase++; return; }
    #if TELESCOPE_TEMPERATURE != OFF
          if (phase == 100) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) return; else { _DS1820_count++; if (Tvalid(f)) _tt=f; else _tt=100.0; phase++; return; } }
    #endif
    #if DEW_HEATER1_TEMPERATURE != OFF
          if (phase == 200) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) return; else { _DS1820_count++; if (Tvalid(f)) _dh_t[0]=f; else _dh_t[0]=100.0; phase++; return; } }
    #endif
    #if DEW_HEATER2_TEMPERATURE != OFF
          if (phase == 300) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) return; else { _DS1820_count++; if (Tvalid(f)) _dh_t[1]=f; else _dh_t[1]=100.0; phase++; return; } }
    #endif
    #if DEW_HEATER3_TEMPERATURE != OFF
          if (phase == 400) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) return; else { _DS1820_count++; if (Tvalid(f)) _dh_t[2]=f; else _dh_t[2]=100.0; phase++; return; } }
    #endif
    #if DEW_HEATER4_TEMPERATURE != OFF
          if (phase == 500) { f = DS18B20.getTempC(_DS1820_address[_DS1820_count],true); if (Tpolling(f)) return; else { _DS1820_count++; if (Tvalid(f)) _dh_t[3]=f; else _dh_t[3]=100.0; phase++; return; } }
    #endif
        }
  #endif

  #if WEATHER != OFF
        if (_BME280_found) {
          if (phase == 4) {
            #ifdef ESP32
              HAL_Wire.begin();
            #endif
            _t = bme.readTemperature();
            #if defined(ESP32) & defined(WIRE_END_SUPPORT)
              HAL_Wire.end();
            #endif
            phase++; return;
          }
          if (phase == 8) {
            #ifdef ESP32
              HAL_Wire.begin();
            #endif
            _p = bme.readPressure() / 100.0;
            #if defined(ESP32) & defined(WIRE_END_SUPPORT)
              HAL_Wire.end();
            #endif
            phase++; return;
          }
          if (phase == 12) {
            #ifdef ESP32
              HAL_Wire.begin();
            #endif
            _h = bme.readHumidity();
            #if defined(ESP32) & defined(WIRE_END_SUPPORT)
              HAL_Wire.end();
            #endif
            phase++; return;
          }
        }
  #endif
      phase++;
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
#if DEW_HEATER1_TEMPERATURE == OFF && WEATHER != OFF
      _dh_t[0]=getTemperature();
#endif
#if DEW_HEATER2_TEMPERATURE == OFF && WEATHER != OFF
      _dh_t[1]=getTemperature();
#endif
#if DEW_HEATER3_TEMPERATURE == OFF && WEATHER != OFF
      _dh_t[2]=getTemperature();
#endif
#if DEW_HEATER4_TEMPERATURE == OFF && WEATHER != OFF
      _dh_t[3]=getTemperature();
#endif
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
    uint8_t _DS1820_address[5][8];

    bool _DS2413_found = false;
    int  _DS2413_devices = 0;
    uint8_t _DS2413_address[2][8];

    double _t = 10.0;
    double _p = 1010.0;
    double _h = 70.0;
    double _a = 200.0;
    double _tt = 10.0;
    double _dh_t[4] = {10.0, 10.0, 10.0, 10.0};
    bool _dh_state[4] = {false, false, false, false};
    bool _last_dh_state[4] = {false, false, false, false};

#ifdef ONEWIRE_DEVICES_PRESENT
    bool Tpolling(double f) {
      return (fabs(f-DEVICE_POLLING_C) < 0.001);
    }
    bool Tvalid(double f) {
      return (f > DEVICE_DISCONNECTED_C);
    }
#endif

};
