// Support for 24LC256 I2C EEPROM

// The full EEPROM size causes problems
//#define E2END 32767
#define E2END 8191

#include <Wire.h>
#define PWire Wire

class _eeprom {
public:
  _eeprom() {
    PWire.begin();
    PWire.setClock(400000);
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
    
    PWire.beginTransmission(80);
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

    PWire.beginTransmission(80);
    PWire.write((int)(address >> 8));   // msb
    PWire.write((int)(address & 0xFF)); // lsb
    PWire.endTransmission();
   
    PWire.requestFrom(80,1);
    result = PWire.read();
    
    return result;
  }
private:
  uint32_t lastWrite=0;
  bool __firstCall=true;
};

_eeprom EEPROM;



