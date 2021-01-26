// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#pragma once

#ifndef EEPROM_DISABLED
  #include "EEPROM.h"
#endif

class nvs {
  public:

#ifndef EEPROM_DISABLED
    bool init() {
#if defined(ESP8266) || defined(ESP32)
      EEPROM.begin(1024);
#endif
      return true;
    }
    
    void commit() {
#if defined(ESP8266) || defined(ESP32) || defined(ARDUINO_ARCH_SAMD)
      EEPROM.commit();
#endif
    }

    byte read(int i) {
      return EEPROM.read(i);
    }

    void update(int i, byte j) {
      byte k = EEPROM.read(i);
      if (k != j) EEPROM.write(i, j);
    }

    void write(int i, byte j) {
      update(i, j);
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

    // write String into EEPROM at position i (40 bytes)
    void writeString(int i, char l[]) {
      for (int l1 = 0; l1 < 40; l1++) {
        update(i + l1, *l); l++;
      }
    }

    // read String from EEPROM at position i (40 bytes)
    void readString(int i, char l[]) {
      for (int l1 = 0; l1 < 40; l1++) {
        *l = read(i + l1); l++;
      }
    }

    // write float into EEPROM at position i (4 bytes)
    void writeFloat(int i, float f) {
      writeBytes(i, (byte*)&f, 4);
    }

    // read float from EEPROM at position i (4 bytes)
    float readFloat(int i) {
      float f;
      readBytes(i, (byte*)&f, 4);
      return f;
    }

    // write double into EEPROM at position i (8 bytes)
    void writeDouble(int i, double d) {
      writeBytes(i, (byte*)&d, 8);
    }

    // read double from EEPROM at position i (8 bytes)
    double readDouble(int i) {
      double d;
      readBytes(i, (byte*)&d, 8);
      return d;
    }

    // write long into EEPROM at position i (4 bytes)
    void writeLong(int i, long l) {
      writeBytes(i, (byte*)&l, 4);
    }

    // read long from EEPROM at position i (4 bytes)
    long readLong(int i) {
      long l;
      readBytes(i, (byte*)&l, 4);
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
#else
    bool init() {}
    void commit() {}
    byte read(int i) {}
    void update(int i, byte j) {}
    void write(int i, byte j) {}
    void writeInt(int i, int j) {}
    int readInt(int i) {}
    void writeString(int i, char l[]) {}
    void readString(int i, char l[]) {}
    void writeFloat(int i, float f) {}
    float readFloat(int i) {}
    void writeDouble(int i, double d) {}
    double readDouble(int i) {}
    void writeLong(int i, long l) {}
    long readLong(int i) {}
    void readBytes(uint16_t i, byte *v, uint8_t count) {}
    void writeBytes(uint16_t i, byte *v, uint8_t count) {}
#endif
};

nvs nv;
