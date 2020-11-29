// ----------------------------------------------------------------------------------------------------------------
// Read Broadcom BISS-C type encoders

// tested and known to work with Broadcom AS37-H39B-B
// designed according protocol description found in as38-H39e-b-an100.pdf

#if AXIS1_ENC == BC_BISSC || AXIS2_ENC == BC_BISSC
  #ifdef EEPROM_DISABLED
    #error "Absolute encoders require NV/EEPROM which isn't supported on this platform"
  #endif

  #include <EEPROM.h>
  #include "EEProm.h"
  #define ENC_HAS_ABSOLUTE
  
  class BiSSC_Encoder {
    public:
      BiSSC_Encoder(int16_t maPin, int16_t sloPin, int16_t axis) {
        _clkPin=maPin;
        _sloPin=sloPin;
        _axis=axis;
        pinMode(_clkPin,OUTPUT);
        digitalWrite(_clkPin,LOW);
        pinMode(_sloPin,INPUT_PULLUP);
        if (_axis == 1) _offset=nv.readLong(EE_ENC_A1_ZERO);
        if (_axis == 2) _offset=nv.readLong(EE_ENC_A2_ZERO);
      }
      int32_t read() {
        if (readEnc(_position)) {
          return (int32_t)_position + _offset;
        } else return INT32_MAX;
      }
      void write(int32_t v) {
        if (_position != INT32_MAX) {
          if (readEnc(_position)) {
            _offset = v - (int32_t)_position;
          }
        }
      }
      void setZero() {
        if (_axis == 1) nv.writeLong(EE_ENC_A1_ZERO,_offset);
        if (_axis == 2) nv.writeLong(EE_ENC_A2_ZERO,_offset);
        nv.commit();
      }
    private:
      uint32_t _position=0;
      int32_t _offset=0;
      int16_t _clkPin;
      int16_t _sloPin;
      int16_t _axis;
      
      bool readEnc(uint32_t &encPos) {
        bool foundAck=false;
        bool foundStart=false;
        bool foundCds=false;
      
        uint8_t  encErr = 0;
        uint8_t  encWrn = 0;
        uint8_t  encCrc = 0;

        uint32_t encTurns = 0;
      
        // prepare for a reading
        encPos=0;
        encErr=0;
        encWrn=0;
        encCrc=0;
      
        // rate in microseconds, ie 2+2 = 4 = 250KHz
        int rate=2;
      
        // sync phase
        for (int i=0; i<20; i++) {
          digitalWrite(_clkPin,LOW);
          if (digitalRead(_sloPin) == LOW) foundAck = true;
          delayMicroseconds(rate);
          digitalWrite(_clkPin,HIGH);
          delayMicroseconds(rate);
          if (foundAck) break;
        }
      
        // if we have an Ack
        if (foundAck) {
          for (int i=0; i<20; i++) {
            digitalWrite(_clkPin,LOW);
            if (digitalRead(_sloPin) == HIGH) foundStart = true;
            delayMicroseconds(rate);
            digitalWrite(_clkPin,HIGH);
            delayMicroseconds(rate);
            if (foundStart) break;
          }
      
          // if we have an Start
          if (foundStart) {
            digitalWrite(_clkPin,LOW);
            if (digitalRead(_sloPin) == LOW) foundCds = true;
            delayMicroseconds(rate);
            digitalWrite(_clkPin,HIGH);
            delayMicroseconds(rate);
          }
      
          // if we have an Cds, read the data
          if (foundCds) {
      
            // the first 16 bits are the multi-turn count
            for (int i=0; i<16; i++) {
              digitalWrite(_clkPin,LOW);
              if (digitalRead(_sloPin) == HIGH) bitSet(encTurns,15-i);
              delayMicroseconds(rate);
              digitalWrite(_clkPin,HIGH);
              delayMicroseconds(rate);
            }
            
            // the next 23 bits are the encoder absolute position
            for (int i=0; i<23; i++) {
              digitalWrite(_clkPin,LOW);
              if (digitalRead(_sloPin) == HIGH) bitSet(encPos,22-i);
              delayMicroseconds(rate);
              digitalWrite(_clkPin,HIGH);
              delayMicroseconds(rate);
            }
      
            // the Err bit
            digitalWrite(_clkPin,LOW);
            if (digitalRead(_sloPin) == HIGH) encErr=1;
            delayMicroseconds(rate);
            digitalWrite(_clkPin,HIGH);
            delayMicroseconds(rate);
      
            // the Wrn bit
            digitalWrite(_clkPin,LOW);
            if (digitalRead(_sloPin) == HIGH) encWrn=1;
            delayMicroseconds(rate);
            digitalWrite(_clkPin,HIGH);
            delayMicroseconds(rate);
      
            // the last 6 bits are the CRC
            for (int i=0; i<6; i++) {
              digitalWrite(_clkPin,LOW);
              if (digitalRead(_sloPin) == HIGH) bitSet(encCrc,5-i);
              delayMicroseconds(rate);
              digitalWrite(_clkPin,HIGH);
              delayMicroseconds(rate);
            }
          }
        }
      
        // send a CDM (invert)
        digitalWrite(_clkPin,LOW);
        delayMicroseconds(rate * 10);
        digitalWrite(_clkPin,HIGH);
      
        // trap errors
        if (!foundAck) return false;
        if (!foundStart) return false;
        if (!foundCds) return false;
        if (encErr) return false;
        // todo: solve CRC and return false if it fails

        // combine absolute and 9 low order bits of multi-turn count for a 32 bit position
        encPos = encPos | ((encTurns & 0b0111111111) << 23);
        return true;
      }
  };

#if AXIS1_ENC == BC_BISSC
  #define ENC_HAS_ABSOLUTE_AXIS1
  BiSSC_Encoder axis1Pos(AXIS1_ENC_A_PIN,AXIS1_ENC_B_PIN,1);
#endif
#if AXIS2_ENC == BC_BISSC
  #define ENC_HAS_ABSOLUTE_AXIS2
  BiSSC_Encoder axis2Pos(AXIS2_ENC_A_PIN,AXIS2_ENC_B_PIN,2);
#endif

#endif
