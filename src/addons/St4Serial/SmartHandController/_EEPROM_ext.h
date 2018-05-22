#pragma once
#include <Arduino.h>
// -----------------------------------------------------------------------------------
// EEPROM related functions 

// write int numbers into EEPROM at position i (2 bytes)
void EEPROM_writeInt(int i, int j);

// read int numbers from EEPROM at position i (2 bytes)
int EEPROM_readInt(int i);

// write 4 byte variable into EEPROM at position i (4 bytes)
void EEPROM_writeQuad(int i, byte *v);
// read 4 byte variable from EEPROM at position i (4 bytes)
void EEPROM_readQuad(int i, byte *v);

// write String into EEPROM at position i (40 bytes)
void EEPROM_writeString(int i, char l[]);

// read String from EEPROM at position i (40 bytes)
void EEPROM_readString(int i, char l[]);

// write 4 byte float into EEPROM at position i (4 bytes)
void EEPROM_writeFloat(int i, float f);

// read 4 byte float from EEPROM at position i (4 bytes)
float EEPROM_readFloat(int i);

  // write 4 byte long into EEPROM at position i (4 bytes)
  void EEPROM_writeLong(int i, long l);

// read 4 byte long from EEPROM at position i (4 bytes)
  long EEPROM_readLong(int i);