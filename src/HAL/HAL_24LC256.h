// Support for 24LC256 I2C EEPROM

#include <Wire.h>

#define E2END 32767

class _eeprom {
public:
  _eeprom() {
    lastWrite=millis();
  }

  void write(uint16_t address, uint8_t data)
  {
    // wait for any prior write to complete
    while ((millis()-lastWrite)<5) {};
    
    Wire.beginTransmission(80);
    Wire.write((int)(address >> 8));   // msb
    Wire.write((int)(address & 0xFF)); // lsb
    Wire.write(data);
    Wire.endTransmission();
    lastWrite=millis();
  }

  void update(uint16_t address, uint8_t data)
  {
    if (data!=read(address)) write(address,data);
  }
   
  byte read(uint16_t address) 
  {
    byte result = 0xFF;
   
    // wait for any prior write to complete
    while ((millis()-lastWrite)<5) {};

    Wire.beginTransmission(80);
    Wire.write((int)(address >> 8));   // msb
    Wire.write((int)(address & 0xFF)); // lsb
    Wire.endTransmission();
   
    Wire.requestFrom(80,1);
   
    if (Wire.available()) result = Wire.read();
   
    return result;
  }
private:
  uint32_t lastWrite=0;
};

_eeprom EEPROM;



