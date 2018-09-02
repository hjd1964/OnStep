// -----------------------------------------------------------------------------------
// non-volatile storage for AT24C32 (often on DS3231 RTC modules with I2C address 0x57)

#pragma once

#include <Wire.h>

// I2C EEPROM Address on DS3231 RTC module
#define I2C_EEPROM_ADDRESS 0x57
#define E2END 4095
#define I2C_CLOCK 400000

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

class nvs {
  public:    
    void init() {
      HAL_Wire.begin();
      HAL_Wire.setClock(I2C_CLOCK);
      _eeprom_addr = I2C_EEPROM_ADDRESS;
    }

    void poll() {
    }

    uint8_t read(int i) {
      uint8_t j;
      ee_read(i,&j,1);
      return j;
    }

    void update(int i, byte j) {
      uint8_t k;
      ee_read(i,&k,1);
      if (j!=k) ee_write(i,j);
    }

    void write(int i, byte j) {
      update(i,j);
    }

    // write int numbers into EEPROM at position i (2 uint8_ts)
    void writeInt(int i, int j) {
      uint8_t *k = (uint8_t*)&j;
      update(i+0,*k); k++;
      update(i+1,*k);
    }
    
    // read int numbers from EEPROM at position i (2 uint8_ts)
    int readInt(int i) {
      uint16_t j;
      ee_read(i,(uint8_t*)&j,2);
      return j;
    }

    // write 4 uint8_t long into EEPROM at position i (4 uint8_ts)
    void writeLong(int i, long l) {
      writeQuad(i,(byte*)&l);
    }
    
    // read 4 uint8_t long from EEPROM at position i (4 uint8_ts)
    long readLong(int i) {
      long l;
      readQuad(i,(byte*)&l);
      return l;
    }
            
    // write 4 uint8_t float into EEPROM at position i (4 uint8_ts)
    void writeFloat(int i, float f) {
      writeQuad(i,(byte*)&f);
    }
    
    // read 4 uint8_t float from EEPROM at position i (4 uint8_ts)
    float readFloat(int i) {
      float f;
      readQuad(i,(byte*)&f);
      return f;
    }

    // write 4 uint8_t variable into EEPROM at position i (4 uint8_ts)
    void writeQuad(int i, byte *v) {
      update(i+0,*v); v++;
      update(i+1,*v); v++;
      update(i+2,*v); v++;
      update(i+3,*v);
    }
    
    // read 4 uint8_t variable from EEPROM at position i (4 uint8_ts)
    void readQuad(int i, byte *v) {
      ee_read(i,v,4);
    }

    // write String into EEPROM at position i (16 bytes)
    void writeString(int i, char l[]) {
      int l1;
      for (l1=0; l1<16; l1++) {
        update(i+l1,*l); l++;
      }
    }
    
    // read String from EEPROM at position i (16 bytes)
    void readString(int i, char l[]) {
      ee_read(i,(byte*)l,16);
    }

    // read count bytes from EEPROM starting at position i
    void readBytes(int i, byte *v, byte count) {
      ee_read(i,v,count);
    }

private:
  // Address of the I2C EEPROM
  uint8_t _eeprom_addr;
  uint32_t nextOpMs=millis()+10UL;

  void ee_write(int offset, byte data) {
    while ((int32_t)(millis()-nextOpMs)<0) {}

    HAL_Wire.beginTransmission(_eeprom_addr);
    HAL_Wire.write(MSB(offset));
    HAL_Wire.write(LSB(offset));
    HAL_Wire.write(data);
    HAL_Wire.endTransmission();
    nextOpMs=millis()+10UL;
  }

  void ee_read(int offset, byte *data, byte count) {
    while ((int32_t)(millis()-nextOpMs)<0) {}

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

