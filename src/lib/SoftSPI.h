// -----------------------------------------------------------------------------------
// Simple soft SPI routines (CPOL=1, CPHA=1)

#pragma once

class bbspi {
  public:  
    void begin(int cs, int sck, int miso, int mosi)
    {
      _cs=cs; pinMode(cs,OUTPUT); digitalWrite(cs,HIGH);
      delayMicroseconds(1);
      _sck=sck; pinMode(_sck,OUTPUT); digitalWrite(_sck,HIGH);

      _miso=miso; 
#ifndef ESP8266_CONTROL_ON
      pinMode(_miso,INPUT);
#endif
      _mosi=mosi; pinMode(_mosi,OUTPUT);
      digitalWrite(cs,LOW);
      delayMicroseconds(1);
    }
    
    void pause() {
      digitalWrite(_cs, HIGH);
      delayMicroseconds(1);
      digitalWrite(_cs, LOW);
      delayMicroseconds(1);
    }
    
    void end() {
      delayMicroseconds(1);
      digitalWrite(_cs, HIGH);
    }
    
    uint8_t transfer(uint8_t data_out)
    {
      uint8_t data_in = 0;
      
      for(int i=7; i>=0; i--)
      {
        digitalWrite(_sck,LOW);
        digitalWrite(_mosi,bitRead(data_out,i));
        delayMicroseconds(1);
        digitalWrite(_sck,HIGH);
#ifdef ESP8266_CONTROL_ON
        bitWrite(data_in,i,0);
#else
        bitWrite(data_in,i,digitalRead(_miso));
#endif
        delayMicroseconds(1);
      }
      
      return data_in;
    }
    
    uint32_t transfer32(uint32_t data_out)
    {
      uint32_t data_in = 0;
      
      for(int i=31; i>=0; i--)
      {
        digitalWrite(_sck,LOW);
        digitalWrite(_mosi,bitRead(data_out,i));
        delayMicroseconds(1);
        digitalWrite(_sck,HIGH);
#ifdef ESP8266_CONTROL_ON
        bitWrite(data_in,i,0);
#else
        bitWrite(data_in,i,digitalRead(_miso));
#endif
        delayMicroseconds(1);
      }
    
      return data_in;
    }
  private:  
    int _cs = 0;
    int _sck = 0;
    int _miso = 0;
    int _mosi = 0;
};

