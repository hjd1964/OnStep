// Support for 24LC series I2C EEPROMs

#include <Wire.h>

// Default I2C address of the EEPROM
#ifndef I2C_EEPROM_ADDRESS
  #define I2C_EEPROM_ADDRESS 0x50
  #warning "Using default I2C_EEPROM_ADDRESS. If you want to override it, define it in your HAL file"
#endif

#define I2C_CLOCK 400000

class _eeprom {
public:
  _eeprom() {
    PWire.begin();
    PWire.setClock(I2C_CLOCK);
    lastWrite=millis();
  }

  void write(uint16_t address, uint8_t data)
  {
    if (!__firstCall) {
      // wait for any prior write to complete
      while ((int32_t)(millis()-lastWrite)<5) {};
    } else {
      __firstCall=false;
    }
    
    PWire.beginTransmission(I2C_EEPROM_ADDRESS);
    PWire.write((int)(address >> 8));   // msb
    PWire.write((int)(address & 0xFF)); // lsb
    PWire.write(data);
    PWire.endTransmission();

    lastWrite=millis();
  }

  void update(uint16_t address, uint8_t data)
  {
    if (data!=read(address)) write(address,data);
  }
   
  uint8_t read(uint16_t address) 
  {
    uint8_t result = 0xFF;

    if (!__firstCall) {
      // wait for any prior write to complete
      while ((int32_t)(millis()-lastWrite)<5) {};
    } else {
      __firstCall=false;
    }

    PWire.beginTransmission(I2C_EEPROM_ADDRESS);
    PWire.write((int)(address >> 8));   // msb
    PWire.write((int)(address & 0xFF)); // lsb
    PWire.endTransmission();
   
    PWire.requestFrom(I2C_EEPROM_ADDRESS,1);
    result = PWire.read();
    
    return result;
  }
private:
  uint32_t lastWrite=0;
  bool __firstCall=true;
};

_eeprom EEPROM;


