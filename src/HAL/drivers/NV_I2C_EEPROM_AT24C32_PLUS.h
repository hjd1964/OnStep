// -----------------------------------------------------------------------------------
// non-volatile storage for AT24C32 (often on DS3231 RTC modules with I2C address 0x57)
// read & write caching version of driver, uses 5K of RAM
// this is specifically for the Teensy3.5 or 3.6 where there's 4095 bytes of EEPROM built-in

#pragma once

#include <Wire.h>
#include "EEPROM.h"

// I2C EEPROM Address on DS3231 RTC module
#define I2C_EEPROM_ADDRESS 0x57
#undef E2END
#define E2END2 4095
#define E2END 8191
#define I2C_CLOCK 400000

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

class nvs {
  public:    
    bool init() {
      HAL_Wire.begin();
      HAL_Wire.setClock(I2C_CLOCK);
      _eeprom_addr = I2C_EEPROM_ADDRESS;

      // mark entire read cache as dirty
      for (int i=0; i < 512; i++) cacheReadState[i]=255;
      // mark entire write cache as clean
      for (int i=0; i < 512; i++) cacheWriteState[i]=0;

      HAL_Wire.beginTransmission(I2C_EEPROM_ADDRESS);
      bool error = HAL_Wire.endTransmission();
      return !error;
    }

    // move data to/from the cache
    void poll() {
      static int i=4095;
      uint8_t j;
      int dirtyW, dirtyR;

      // just exit if waiting for an EEPROM write to finish
      if ((int32_t)(millis()-nextOpMs) < 0) return;

      // check 20 byte chunks of cache for data that needs processing so < about 2s to check the entire cache
      for (int j=0; j < 20; j++) {
        i++; if (i > 4095) i=0;
        dirtyW=bitRead(cacheWriteState[i/8],i%8);
        dirtyR=bitRead(cacheReadState[i/8],i%8);
        if (dirtyW || dirtyR) break;
      }

      // write data as required
      if (dirtyW) {
        j=cache[i];        // get data from cache
        ee_write(i,j);     // store in EEPROM
        bitWrite(cacheWriteState[i/8],i%8,0); // clean
      } else {
        // read data as required
        if (dirtyR) {
          ee_read(i,&j,1); // get data from EEPROM
          cache[i]=j;      // store in cache
          bitWrite(cacheReadState[i/8],i%8,0); // clean
        }
      }
    }

    uint8_t read(int i) {
      if (i > E2END2) {
        i=i-(E2END2+1);
        int dirty=bitRead(cacheReadState[i/8],i%8);
        if (dirty) {
          uint8_t j;
          ee_read(i,&j,1);
          
          // store and mark as clean
          cache[i]=j;
          bitWrite(cacheReadState[i/8],i%8,0);
  
          return j;
        } else return cache[i];
      } else {
        return EEPROM.read(i);
      }
    }

    void update(int i, byte j) {
      if (i > E2END2) {
        uint8_t k=read(i);
        i=i-(E2END2+1);
        if (j != k) {
          // store
          cache[i]=j;
  
          // mark write as dirty (needs to be written)
          bitWrite(cacheWriteState[i/8],i%8,1);
  
          // mark read as clean (so we don't overwrite the cache)
          bitWrite(cacheReadState[i/8],i%8,0);
        }
      } else {
        EEPROM.update(i, j);
      }
    }

    void write(int i, byte j) {
      update(i,j);
    }

    // write int numbers into EEPROM at position i (2 bytes)
    void writeInt(int i, int j) {
      uint8_t *k = (uint8_t*)&j;
      update(i + 0, *k); k++;
      update(i + 1, *k);
    }

    // read int numbers from EEPROM at position i (2 bytes)
    int readInt(int i) {
      uint16_t j;
      uint8_t *k = (uint8_t*)&j;
      *k = read(i + 0); k++;
      *k = read(i + 1);
      return j;
    }

    // write 4 byte variable into EEPROM at position i (4 bytes)
    void writeQuad(int i, byte *v) {
      update(i + 0, *v); v++;
      update(i + 1, *v); v++;
      update(i + 2, *v); v++;
      update(i + 3, *v);
    }

    // read 4 byte variable from EEPROM at position i (4 bytes)
    void readQuad(int i, byte *v) {
      *v = read(i + 0); v++;
      *v = read(i + 1); v++;
      *v = read(i + 2); v++;
      *v = read(i + 3);
    }

    // write String into EEPROM at position i (16 bytes)
    void writeString(int i, char l[]) {
      for (int l1 = 0; l1 < 16; l1++) {
        update(i + l1, *l); l++;
      }
    }

    // read String from EEPROM at position i (16 bytes)
    void readString(int i, char l[]) {
      for (int l1 = 0; l1 < 16; l1++) {
        *l = read(i + l1); l++;
      }
    }

    // write 4 byte float into EEPROM at position i (4 bytes)
    void writeFloat(int i, float f) {
      writeQuad(i, (byte*)&f);
    }

    // read 4 byte float from EEPROM at position i (4 bytes)
    float readFloat(int i) {
      float f;
      readQuad(i, (byte*)&f);
      return f;
    }

    // write 4 byte long into EEPROM at position i (4 bytes)
    void writeLong(int i, long l) {
      writeQuad(i, (byte*)&l);
    }

    // read 4 byte long from EEPROM at position i (4 bytes)
    long readLong(int i) {
      long l;
      readQuad(i, (byte*)&l);
      return l;
    }

    // read count bytes from EEPROM starting at position i
    void readBytes(uint16_t i, byte *v, uint8_t count) {
      for (int j=0; j < count; j++) { *v = read(i + j); v++; }
    }

    // write count bytes to EEPROM starting at position i
    void writeBytes(uint16_t i, byte *v, uint8_t count) {
      for (int j=0; j < count; j++) { write(i + j,*v); v++; }
    }

private:
  // Address of the I2C EEPROM
  uint8_t _eeprom_addr;
  uint32_t nextOpMs=0;
  uint8_t cache[4096];
  uint8_t cacheReadState[512];
  uint8_t cacheWriteState[512];

  void ee_write(int offset, byte data) {
    while ((int32_t)(millis()-nextOpMs) < 0) {}

    HAL_Wire.beginTransmission(_eeprom_addr);
    HAL_Wire.write(MSB(offset));
    HAL_Wire.write(LSB(offset));
    HAL_Wire.write(data);
    HAL_Wire.endTransmission();
    nextOpMs=millis()+10UL;
  }

  void ee_read(int offset, byte *data, byte count) {
    while ((int32_t)(millis()-nextOpMs) < 0) {}

    HAL_Wire.beginTransmission(_eeprom_addr);
    HAL_Wire.write(MSB(offset));
    HAL_Wire.write(LSB(offset));
    HAL_Wire.endTransmission();

    HAL_Wire.requestFrom(_eeprom_addr, (uint8_t)count);
    while (HAL_Wire.available()) {
      *data = HAL_Wire.read(); data++;
    }
  }
};

nvs nv;
