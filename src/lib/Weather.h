// -----------------------------------------------------------------------------------------------------------------------------
// Weather related functions

#pragma once

#include "Heater.h"

#if WEATHER != OFF

  #if WEATHER == BME280 || WEATHER == BME280_0x76 || WEATHER == BME280_SPI
    #include <Adafruit_BME280.h>          // https://github.com/adafruit/Adafruit_BME280_Library/tree/156a0537d6b21aaab1d1f104a7001a38ca1ffce3
                                          // and https://github.com/adafruit/Adafruit_Sensor
  #endif

  #if WEATHER == BME280 || WEATHER == BME280_0x76
    Adafruit_BME280 bmx;
  #elif WEATHER == BME280_SPI && defined(SSPI_SHARED)
    Adafruit_BME280 bmx(BME280_CS_PIN, SSPI_MOSI, SSPI_MISO, SSPI_SCK);   // software SPI
  #elif WEATHER == BME280_SPI
    Adafruit_BME280 bmx(BME280_CS_PIN);                                   // hardware SPI
  #endif
    
  // BMP280 is the BME280 without humidity 
  #if WEATHER == BMP280 || WEATHER == BMP280_0x76 || WEATHER == BMP280_SPI
    #include <Adafruit_BMP280.h>          // https://github.com/adafruit/Adafruit_BMP280_Library
                                          // and https://github.com/adafruit/Adafruit_Sensor
  #endif

  #if WEATHER == BMP280 || WEATHER == BMP280_0x76
    Adafruit_BMP280 bmx(&HAL_Wire); 
  #elif WEATHER == BMP280_SPI
    Adafruit_BMP280 bmx(BMP280_CS_PIN);                                   // hardware SPI
  #endif
    
#endif

#ifdef ONEWIRE_DEVICES_PRESENT
  #include <OneWire.h>                    // my modified OneWire library https://github.com/hjd1964/OneWire
  OneWire oneWire(OneWirePin);

  #ifdef DS1820_DEVICES_PRESENT
    #include <DallasTemperature.h>        // my modified DallasTemperature library https://github.com/hjd1964/Arduino-Temperature-Control-Library
    DallasTemperature DS18B20(&oneWire);
  #endif
 
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
  #ifdef DS1820_DEVICES_PRESENT
      for (int i=0; i<9; i++) for (int j=0; j<8; j++) _DS1820_address[i][j] = 0;
      if (TELESCOPE_TEMPERATURE != OFF && TELESCOPE_TEMPERATURE != DS1820) { for (int j=0; j<8; j++) _DS1820_address[0][j] = (((uint64_t)TELESCOPE_TEMPERATURE) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE1_TEMP != OFF && FEATURE1_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[1][j] = (((uint64_t)FEATURE1_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE2_TEMP != OFF && FEATURE2_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[2][j] = (((uint64_t)FEATURE2_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE3_TEMP != OFF && FEATURE3_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[3][j] = (((uint64_t)FEATURE3_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE4_TEMP != OFF && FEATURE4_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[4][j] = (((uint64_t)FEATURE4_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE5_TEMP != OFF && FEATURE5_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[5][j] = (((uint64_t)FEATURE5_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE6_TEMP != OFF && FEATURE6_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[6][j] = (((uint64_t)FEATURE6_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE7_TEMP != OFF && FEATURE7_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[7][j] = (((uint64_t)FEATURE7_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
      if (FEATURE8_TEMP != OFF && FEATURE8_TEMP != DS1820) { for (int j=0; j<8; j++) _DS1820_address[8][j] = (((uint64_t)FEATURE8_TEMP) >> ((7-j)*8)) & 0xff; _DS1820_devices++; }
  #endif
  
      // clear then pre-load any user defined DS2413 addresses
      _DS2413_devices=0;
  #ifdef DS2413_DEVICES_PRESENT
      for (int i=0; i<4; i++) for (int j=0; j<8; j++) _DS2413_address[i][j] = 0;
      if (FEATURE1_PIN > 255 && FEATURE1_PIN != DS2413) { for (int j=0; j<8; j++) _DS2413_address[0][j] = (FEATURE1_PIN >> ((7-j)*8)) & 0xff; _DS2413_devices++; }
      if (FEATURE3_PIN > 255 && FEATURE3_PIN != DS2413) { for (int j=0; j<8; j++) _DS2413_address[1][j] = (FEATURE3_PIN >> ((7-j)*8)) & 0xff; _DS2413_devices++; }
      if (FEATURE5_PIN > 255 && FEATURE5_PIN != DS2413) { for (int j=0; j<8; j++) _DS2413_address[2][j] = (FEATURE5_PIN >> ((7-j)*8)) & 0xff; _DS2413_devices++; }
      if (FEATURE7_PIN > 255 && FEATURE7_PIN != DS2413) { for (int j=0; j<8; j++) _DS2413_address[3][j] = (FEATURE7_PIN >> ((7-j)*8)) & 0xff; _DS2413_devices++; }
  #endif
  
      // scan the OneWire bus and record the devices found
      oneWire.reset_search();

      // only search out DS1820's or DS2413's IF none are explicitly specified
  #if defined(DS1820_DEVICES_PRESENT) || FEATURE_LIST_DS == ON
      bool searchDS1820 = _DS1820_devices == 0;
  #endif
  #if defined(DS2413_DEVICES_PRESENT) || FEATURE_LIST_DS == ON
      bool searchDS2413 = _DS2413_devices == 0;
  #endif
  #if FEATURE_LIST_DS == ON
      bool _DS1820_detected=false, _DS2413_detected=false;
      Serial.println("Dallas/Maxim OneWire bus device s/n's:");
  #endif

      while (oneWire.search(address)) {
        if (oneWire.crc8(address, 7) == address[7]) {
  #if defined(DS1820_DEVICES_PRESENT) || FEATURE_LIST_DS == ON
          if (address[0] == 0x10 || address[0] == 0x28) {
            if (searchDS1820) {
              if (_DS1820_count == 0 && TELESCOPE_TEMPERATURE != DS1820) _DS1820_count++;
              if (_DS1820_count == 1 && FEATURE1_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 2 && FEATURE2_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 3 && FEATURE3_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 4 && FEATURE4_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 5 && FEATURE5_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 6 && FEATURE6_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 7 && FEATURE7_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count == 8 && FEATURE8_TEMP != DS1820) _DS1820_count++;
              if (_DS1820_count <= 8) for (int j=0; j<8; j++) { _DS1820_address[_DS1820_count][j]=address[j]; _DS1820_devices++; }
              _DS1820_count++;
            }
    #if FEATURE_LIST_DS == ON
            _DS1820_detected=true;
            if (address[0] == 0x10) Serial.print("DS18S20: 0x"); else Serial.print("DS18B20: 0x");
            for (int j=0; j<8; j++) { if (address[j] < 16) Serial.print("0"); Serial.print(address[j],HEX); }
            if (searchDS1820) {
              if (_DS1820_count == 1) Serial.print(" auto-assigned to TELESCOPE_TEMPERATURE"); else
              if (_DS1820_count <= 9) { Serial.print(" auto-assigned to FEATURE"); Serial.print(_DS1820_count-1); Serial.print("_TEMP"); } else Serial.print(" not assigned");
            } else Serial.print(" auto-assign disabled");
            Serial.println();
    #endif
          }
  #endif
  #if defined(DS2413_DEVICES_PRESENT) || FEATURE_LIST_DS == ON
          if (address[0] == 0x3A) {
            if (searchDS2413) {
              if (_DS2413_count == 0 && FEATURE1_PIN != DS2413) _DS2413_count++;
              if (_DS2413_count == 1 && FEATURE3_PIN != DS2413) _DS2413_count++;
              if (_DS2413_count == 2 && FEATURE5_PIN != DS2413) _DS2413_count++;
              if (_DS2413_count == 3 && FEATURE7_PIN != DS2413) _DS2413_count++;
              if (_DS2413_count <= 3) { for (int j=0; j<8; j++) _DS2413_address[_DS2413_count][j]=address[j]; _DS2413_devices++; }
              _DS2413_count++;
            }
    #if FEATURE_LIST_DS == ON
            _DS2413_detected=true;
            Serial.print("DS2413:  0x"); for (int j=0; j<8; j++) { if (address[j] < 16) Serial.print("0"); Serial.print(address[j],HEX); }
            if (searchDS2413) {
              if (_DS2413_count <= 4) { Serial.print(" auto-assigned to FEATURE"); Serial.print((_DS2413_count-1)*2+1); Serial.print("_PIN"); } else Serial.print(" not assigned");
            } else Serial.print(" auto-assign disabled");
            Serial.println();
    #endif
          }
  #endif
        }
      }

  #if FEATURE_LIST_DS == ON
      if (!_DS1820_detected) Serial.println("No DS18B20 or DS18S20 devices found");
      if (!_DS2413_detected) Serial.println("No DS2413 devices found");
  #endif

  #ifdef DS1820_DEVICES_PRESENT
      // don't sit and wait for conversion to complete
      DS18B20.setWaitForConversion(false);
  #endif

      if (_DS1820_devices > 0) _DS1820_found = true;
      if (_DS2413_devices > 0) _DS2413_found = true;
#endif
#if WEATHER != OFF
  #if WEATHER == BME280
      if (bmx.begin(0x77, &HAL_Wire)) _BME280_found = true; else { success = false; DLF("WRN, ambient.init(): BME280_0x77 not found"); }
  #elif WEATHER == BME280_0x76
      if (bmx.begin(0x76, &HAL_Wire)) _BME280_found = true; else { success = false; DLF("WRN, ambient.init(): BME280_0x76 not found"); } 
  #elif WEATHER == BMP280 || WEATHER == BMP280_SPI || WEATHER == BME280_SPI
      if (bmx.begin()) _BMP280_found = true; else { success = false; DLF("WRN, ambient.init(): BME280_SPI not found"); }
  #elif WEATHER == BMP280_0x76
      if (bmx.begin(0x76)) _BMP280_found = true; else { success = false; DLF("WRN, ambient.init(): BMP280_0x76 not found"); }
  #elif WEATHER == BMP280 || WEATHER == BMP280_SPI
      if (bmx.begin()) _BMP280_found = true; else { success = false;  DLF("WRN, ambient.init(): BMP280_SPI not found"); }
  #else
      #error "Configuration (Config.h): Setting WEATHER unknown value!"
  #endif
  // follow any I2C device in-library init with a reset of the I2C bus speed
  #if WEATHER == BME280 || WEATHER == BME280_0x76 || WEATHER == BMP280_0x76 || WEATHER == BMP280
    #ifdef HAL_WIRE_RESET_AFTER_CONNECT
      HAL_Wire.end();
      HAL_Wire.begin();
    #endif
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
  #endif
#endif
      return success;
    }

    // designed for a 0.01s polling interval, 5 seconds to refresh everything
    void poll() {
#if WEATHER != OFF || defined(ONEWIRE_DEVICES_PRESENT)
      static bool firstScanDone = false;
      if (_BME280_found || _BMP280_found || _DS1820_found || _DS2413_found) {

        static int phase = 0;
        if (phase >= 500) { phase = 0; _DS1820_count = 0; firstScanDone = true; }

  #ifdef DS2413_DEVICES_PRESENT
        if (_DS2413_found) {
          if (phase%2 == 0) for (int i=0; i<8; i++) _this_ds2413_state[i]=_ds2413_state[i];
    #if (FEATURE1_PIN & DS_MASK) == DS2413 || (FEATURE2_PIN & DS_MASK) == DS2413
          if (phase%2 == 1 && (_last_ds2413_state[1] != _this_ds2413_state[1] || _last_ds2413_state[0] != _this_ds2413_state[0])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[0],_this_ds2413_state[1],_this_ds2413_state[0],true)) { phase++; _last_ds2413_state[1] = _this_ds2413_state[1]; _last_ds2413_state[0] = _this_ds2413_state[0]; }
            if (!DS2413GPIO.success()) { _ds2413_failures[0]++; VLF("WRN, DS2413 comms: features 1/2"); }
            return;
          }
    #endif
    #if (FEATURE3_PIN & DS_MASK) == DS2413 || (FEATURE4_PIN & DS_MASK) == DS2413
          if (phase%2 == 1 && (_last_ds2413_state[3] != _this_ds2413_state[3] || _last_ds2413_state[2] != _this_ds2413_state[2])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[1],_this_ds2413_state[3],_this_ds2413_state[2],true)) { phase++; _last_ds2413_state[3] = _this_ds2413_state[3]; _last_ds2413_state[2] = _this_ds2413_state[2]; }
            if (!DS2413GPIO.success()) { _ds2413_failures[1]++; VLF("WRN, DS2413 comms: features 3/4"); }
            return;
          }
    #endif
    #if (FEATURE5_PIN & DS_MASK) == DS2413 || (FEATURE6_PIN & DS_MASK) == DS2413
          if (phase%2 == 1 && (_last_ds2413_state[5] != _this_ds2413_state[5] || _last_ds2413_state[4] != _this_ds2413_state[4])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[2],_this_ds2413_state[5],_this_ds2413_state[4],true)) { phase++; _last_ds2413_state[5] = _this_ds2413_state[5]; _last_ds2413_state[4] = _this_ds2413_state[4]; }
            if (!DS2413GPIO.success()) { _ds2413_failures[2]++; VLF("WRN, DS2413 comms: features 5/6"); }
            return;
          }
    #endif
    #if (FEATURE7_PIN & DS_MASK) == DS2413 || (FEATURE8_PIN & DS_MASK) == DS2413
          if (phase%2 == 1 && (_last_ds2413_state[7] != _this_ds2413_state[7] || _last_ds2413_state[6] != _this_ds2413_state[6])) {
            if (DS2413GPIO.setStateByAddress(_DS2413_address[3],_this_ds2413_state[7],_this_ds2413_state[6],true)) { phase++; _last_ds2413_state[7] = _this_ds2413_state[7]; _last_ds2413_state[6] = _this_ds2413_state[6]; }
            if (!DS2413GPIO.success()) { _ds2413_failures[3]++; VLF("WRN, DS2413 comms: features 7/8"); } // If your compile fails you need to update the DS2413 library: https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
            return;
          }
    #endif
        }
  #endif

  #ifdef DS1820_DEVICES_PRESENT
        if (_DS1820_found) {
          float f;
          if (phase == 0) { if (DS18B20.requestTemperatures(true)) phase++; return; }
    #if TELESCOPE_TEMPERATURE != OFF
          if (phase == 50) { f = DS18B20.getTempC(_DS1820_address[0],true); if (Tpolling(f)) return; else { _DS1820_count++; _tt=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE1_TEMP & DS_MASK) == DS1820
          if (phase == 100) { f = DS18B20.getTempC(_DS1820_address[1],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[0]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE2_TEMP & DS_MASK) == DS1820
          if (phase == 150) { f = DS18B20.getTempC(_DS1820_address[2],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[1]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE3_TEMP & DS_MASK) == DS1820
          if (phase == 200) { f = DS18B20.getTempC(_DS1820_address[3],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[2]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE4_TEMP & DS_MASK) == DS1820
          if (phase == 250) { f = DS18B20.getTempC(_DS1820_address[4],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[3]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE5_TEMP & DS_MASK) == DS1820
          if (phase == 300) { f = DS18B20.getTempC(_DS1820_address[5],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[4]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE6_TEMP & DS_MASK) == DS1820
          if (phase == 350) { f = DS18B20.getTempC(_DS1820_address[6],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[5]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE7_TEMP & DS_MASK) == DS1820
          if (phase == 400) { f = DS18B20.getTempC(_DS1820_address[7],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[6]=Tvalidated(f); phase++; return; } }
    #endif
    #if (FEATURE8_TEMP & DS_MASK) == DS1820
          if (phase == 450) { f = DS18B20.getTempC(_DS1820_address[8],true); if (Tpolling(f)) return; else { _DS1820_count++; _dh_t[7]=Tvalidated(f); phase++; return; } }
    #endif
        }
  #endif

  #if WEATHER != OFF
        if (_BME280_found || _BMP280_found) {
          if (phase == 4) { _t = bmx.readTemperature(); phase++; return; }
          if (phase == 8) { _p = bmx.readPressure() / 100.0; phase++; return; }
    #if WEATHER == BME280 || WEATHER == BME280_0x76 || WEATHER == BME280_SPI
          if (phase == 12) { _h = bmx.readHumidity(); phase++; return; }
    #endif
        }
    #if WEATHER_SUPRESS_ERRORS == OFF
        else { _t=NAN; _p=NAN; _h=NAN; }
    #endif
  #endif
      phase++;
      }
      if (!firstScanDone) return;
#endif
      
#if TELESCOPE_TEMPERATURE == OFF
      // use primary temperature for the telescope temperature if TELESCOPE_TEMPERATURE
      _tt = _t;
#else
  #if WEATHER == OFF
      // use telescope temperature for the primary temperature if WEATHER = OFF
      _t = _tt;
  #endif
#endif

      // once every two second timed
      static uint16_t p = 1;
      if ((p++)%200 == 0) {
        // apply a 10 sample rolling average to the ambient temperature
        static byte taNanCount=0;
        static bool firstTSample=true; if (firstTSample) { firstTSample=false; _ta=_t; if (!isnan(_t)) taNanCount=0; }
        if (isnan(_t)) {
          if (taNanCount < 15) {
            taNanCount++;
            VLF("WRN, misc. comms: ambient temp");
          } else {
            _ta=NAN;
            firstTSample=true;
            VLF("WRN, misc. isnan: ambient temp disabled");
          }
        } else { taNanCount=0; _ta=(_ta*9.0+_t)/10.0; }

        // apply a 10 sample rolling average to the telescope temperature
        static byte ttNanCount=0;
        static bool firstTTSample=true; if (firstTTSample) { firstTTSample=false; _tta=_tt; if (!isnan(_tt)) ttNanCount=0; }
        if (isnan(_tt)) {
          if (ttNanCount < 15) {
            ttNanCount++;
            VLF("WRN, DS1820 comms: telescope");
          } else {
            _tta=NAN;
            firstTTSample=true;
            VLF("WRN, DS1820 isnan: telescope disabled");
          }
        } else { ttNanCount=0; _tta=(_tta*9.0+_tt)/10.0; }
 
 #ifdef DS1820_DEVICES_PRESENT
        // apply a 2 sample rolling average to the feature temperatures
        static byte ftNanCount[8]={0,0,0,0,0,0,0,0};
        static bool firstFTSample[8]={true,true,true,true,true,true,true,true};
        for (int i=0; i<8; i++) {
  #if FEATURE1_TEMP == OFF
          if (i==0) continue;
  #endif
  #if FEATURE2_TEMP == OFF
          if (i==1) continue;
  #endif
  #if FEATURE3_TEMP == OFF
          if (i==2) continue;
  #endif
  #if FEATURE4_TEMP == OFF
          if (i==3) continue;
  #endif
  #if FEATURE5_TEMP == OFF
          if (i==4) continue;
  #endif
  #if FEATURE6_TEMP == OFF
          if (i==5) continue;
  #endif
  #if FEATURE7_TEMP == OFF
          if (i==6) continue;
  #endif
  #if FEATURE8_TEMP == OFF
          if (i==7) continue;
  #endif
          if (firstFTSample[i]) { firstFTSample[i]=false; _dh_ta[i]=_dh_t[i]; if (!isnan(_dh_t[i])) ftNanCount[i]=0; }
          if (isnan(_dh_t[i])) {
            if (ftNanCount[i] < 15) {
              ftNanCount[i]++;
              VF("WRN, DS1820 comms: feature "); VL(i+1);
            } else {
              _dh_ta[i]=NAN;
              firstFTSample[i]=true;
              VF("WRN, DS1820 isnan: feature "); V(i+1); VLF(" disabled");
            }
          } else { ftNanCount[i]=0; _dh_ta[i]=(_dh_ta[i]+_dh_t[i])/2.0; }

        }
#endif
      }

#ifdef DS2413_DEVICES_PRESENT
      // turn off features that have an error rate of above 2 per minute
      if (p%6000 == 0) {
        for (int i=0; i<4; i++) { 
          if (_ds2413_failures[i] > 0) {
            _ds2413_failures[i]--;
            _ds2413_state[i*2]=0; _last_ds2413_state[i*2]=INVALID;
            _ds2413_state[i*2+1]=0; _last_ds2413_state[i*2+1]=INVALID;
          }
          if (_ds2413_failures[i] > 2) {
            _ds2413_failures[i]=0;
            VF("WRN, DS2413 epm>2: features ");
            if (i == 0) VF("1/2"); if (i == 1) VF("3/4"); if (i == 2) VF("5/6"); if (i == 3) VF("7/8");
            VLF(" disabled");
          }
        }
      }
#endif
    }

    // get temperature in deg. C
    float getTemperature() {
      return _ta;
    }
    
    // get telescope temperature in deg. C
    float getTelescopeTemperature() {
      return _tta;
    }
    
    // get feature temperature in deg. C
    float getFeatureTemperature(int index) {
      if (index < 0 || index > 7) return 100.0;
#if FEATURE1_TEMP == OFF
      _dh_ta[0]=getTemperature();
#endif
#if FEATURE2_TEMP == OFF
      _dh_ta[1]=getTemperature();
#endif
#if FEATURE3_TEMP == OFF
      _dh_ta[2]=getTemperature();
#endif
#if FEATURE4_TEMP == OFF
      _dh_ta[3]=getTemperature();
#endif
#if FEATURE5_TEMP == OFF
      _dh_ta[4]=getTemperature();
#endif
#if FEATURE6_TEMP == OFF
      _dh_ta[5]=getTemperature();
#endif
#if FEATURE7_TEMP == OFF
      _dh_ta[6]=getTemperature();
#endif
#if FEATURE8_TEMP == OFF
      _dh_ta[7]=getTemperature();
#endif
      return _dh_ta[index];
    }
    
    // set temperature in deg. C
    void setTemperature(float t) {
      _t = t;
    }
    
    // get barometric pressure in hPa/mb
    float getPressure() {
      return _p;
    }
    
    // get barometric pressure in hPa/mb
    void setPressure(float p) {
      _p = p;
    }
    
    // get relative humidity in %
    float getHumidity() {
      return _h;
    }
    
    // set relative humidity in %
    void setHumidity(float h) {
      _h = h;
    }
    
    // get altitude in meters
    float getAltitude() {
      return _a;
    }
    
    // set altitude in meters
    void setAltitude(float a) {
      _a = a;
    }
    
    // get dew point in deg. C
    // accurate to +/- 1 deg. C for RH above 50%
    float getDewPoint() {
      return _ta - ((100.0 - _h) / 5.0);
      // a more accurate formula?
      // return 243.04*(log(_h/100.0)+((17.625*_ta)/(243.04+_ta)))/(17.625-log(_h/100.0)-((17.625*_ta)/(243.04+_ta)));
    }

    // four DS2413 OneWire GPIO's are supported, this sets each output on or off
    int getDS2413State(int feature) {
      if (feature >= 0 && feature <= 7) return _ds2413_state[feature]; else return 0;
    }
    void setDS2413State(int feature, bool state) {
      if (feature >= 0 && feature <= 7) _ds2413_state[feature]=state;
    }
    bool getDS2413Failure(int feature) {
      if (feature >= 0 && feature <= 7) {
        if (feature == 0 || feature == 1) return _ds2413_failures[0] > 2; else
        if (feature == 2 || feature == 3) return _ds2413_failures[1] > 2; else
        if (feature == 4 || feature == 5) return _ds2413_failures[2] > 2; else
        if (feature == 6 || feature == 7) return _ds2413_failures[3] > 2; else return true;
      } else return true;
    }
    
  private:
    bool _BME280_found = false;
    bool _BMP280_found = false;

    bool _DS1820_found = false;
    int  _DS1820_devices = 0;
    int  _DS1820_count = 0;
    uint8_t _DS1820_address[9][8];

    bool _DS2413_found = false;
    int  _DS2413_devices = 0;
    int  _DS2413_count = 0;
    uint8_t _DS2413_address[4][8];

    float _t = 10.0;
    float _ta = 10.0;
    float _p = 1010.0;
    float _h = 70.0;
    float _a = 200.0;
    float _tt = 10.0;
    float _tta = 10.0;
    float _dh_t[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    float _dh_ta[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    bool _ds2413_state[8] = {false, false, false, false, false, false, false, false};
    int16_t _ds2413_failures[4] = {0, 0, 0, 0};
    int16_t _this_ds2413_state[8] = {false, false, false, false, false, false, false, false};
    int16_t _last_ds2413_state[8] = {INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID};

#ifdef DS1820_DEVICES_PRESENT
    bool Tpolling(float f) {
      return (fabs(f-DEVICE_POLLING_C) < 0.001);
    }
    float Tvalidated(float f) {
      if (fabs(f-DEVICE_DISCONNECTED_C) < 0.001) return NAN;
      if (f < -100 || f > 70) return NAN;
      return f;
    }
#endif

};
