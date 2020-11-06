// -----------------------------------------------------------------------------------
// Simple soft SPI routines (CPOL=1, CPHA=1)

#pragma once

class bbspi {
  public:
    void init(int cs, int sck, int miso, int mosi) {
      _cs=cs; _sck=sck; _miso=miso; _mosi=mosi;
    }

    bool begin()
    {
      pinMode(_cs,OUTPUT); digitalWrite(_cs,HIGH);
      delaySPI;
      pinMode(_sck,OUTPUT); digitalWrite(_sck,HIGH);
      if (_miso >= 0) pinMode(_miso,INPUT);
      pinMode(_mosi,OUTPUT);
      delaySPI;
      digitalWrite(_cs,LOW);
      delaySPI;
      
      return true;
    }
    
    void pause() {
      digitalWrite(_cs, HIGH);
      delaySPI;
      digitalWrite(_cs, LOW);
      delaySPI;
    }
    
    void end() {
      digitalWrite(_cs, HIGH);
      delaySPI;
    }
    
    uint8_t transfer(uint8_t data_out)
    {
      uint8_t data_in = 0;
      for(int i=7; i >= 0; i--)
      {
        digitalWrite(_sck,LOW);
        digitalWrite(_mosi,bitRead(data_out,i));
        delaySPI;
        digitalWrite(_sck,HIGH);
        if (_miso >= 0) bitWrite(data_in,i,digitalRead(_miso));
        delaySPI;
      }
      
      return data_in;
    }
    
    uint32_t transfer32(uint32_t data_out)
    {
      uint32_t data_in = 0;
      for(int i=31; i >= 0; i--)
      {
        digitalWrite(_sck,LOW);
        digitalWrite(_mosi,bitRead(data_out,i));
        delaySPI;
        digitalWrite(_sck,HIGH);
        if (_miso >= 0) bitWrite(data_in,i,digitalRead(_miso));
        delaySPI;
      }

      return data_in;
    }
  private:  
    int _cs = 0;
    int _sck = 0;
    int _miso = 0;
    int _mosi = 0;
};
