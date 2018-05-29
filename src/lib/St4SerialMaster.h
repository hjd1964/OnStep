// ------------------------------------------------------------------------------------
// Serial ST4 master

#pragma once

/*
ST4 port data communication scheme:

5V power ---- Teensy3.2, Nano, etc.
Gnd ---------

HC              Signal               OnStep
RAw  ----        Data         --->   Recv. data
DEs  <---        Clock        ----   Clock
DEn  <---        Data         ----   Send data
RAe  ---- 12.5Hz Square wave  --->   100% sure SHC is present, switches DEs & DEn to OUTPUT

Data is exchanged on clock edges similar to SPI so is timing insensitive (runs with interrupts enabled.)

One data byte is exchanged (in both directions w/basic error detection and recovery.)  A value 0x00 byte 
means "no data" and is ignored on both sides.  Mega2560 hardware runs at (fastest) 10mS/byte (100 Bps) and 
all others (Teensy3.x, etc.) at 2mS/byte (500 Bps.)
*/

#include "Stream.h"

class Mst4 : public Stream
{
  public:
    void begin();
    void begin(long baud);
    
    void end();

    // recvs one char and transmits one char to/from buffers; recvd chars <32 are returned directly and bypass the buffer
    inline char poll() {
      char c=0;
      if (trans(&c,_xmit_buffer[_xmit_head])) {
        // data going out was good?
        if (!_send_error) {
          if (_xmit_buffer[_xmit_head]!=(char)0) _xmit_head++;
        }
        // data coming in was good?
        if (!_recv_error) {
          if (c>=(char)32) { _recv_buffer[_recv_tail]=c; _recv_tail++; _recv_buffer[_recv_tail]=(char)0; return (char)0; } else return c;
        } else return (char)0;
      } else return (char)0;
    }
    
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

    volatile char _xmit_buffer[256] = "";
    volatile byte _xmit_head        = 0;
    volatile byte _xmit_tail        = 0;
    volatile char _recv_buffer[256] = "";
    volatile byte _recv_tail        = 0;
    volatile bool _frame_error      = false;
    volatile bool _send_error       = false;
    volatile bool _recv_error       = false;

  private:
    inline bool trans(char *data_in, uint8_t data_out)
    {
      static unsigned long lastMicros=0;

      // SHC_CLOCK HIGH for more than 1500 us means that a pair of data bytes is done being exchanged
      #ifdef HAL_SLOW_PROCESSOR
        #define XMIT_TIME 20
        if ((micros()-lastMicros)<10000L) return false;
      #else
        #define XMIT_TIME 40
        if ((micros()-lastMicros)<2000L) return false;
      #endif

      uint8_t s_parity=0;
      uint8_t r_parity=0;

      // assume no errors
      _frame_error=false;
      _send_error=false;
      _recv_error=false;
      
      // start bit
      digitalWrite(ST4DEs,LOW);                        // clock
      digitalWrite(ST4DEn,LOW);                        // send start bit
      delayMicroseconds(XMIT_TIME);
      digitalWrite(ST4DEs,HIGH);                       // clock
      if (digitalRead(ST4RAw)!=LOW) _frame_error=true; // recv start bit
      delayMicroseconds(XMIT_TIME);
      if (_frame_error) { lastMicros=micros(); return false; }

      for (int i=7; i>=0; i--)
      {
        uint8_t state=bitRead(data_out,i); s_parity+=state;
        digitalWrite(ST4DEs,LOW);                      // clock
        digitalWrite(ST4DEn,state);                    // send data bit
        delayMicroseconds(XMIT_TIME);
        digitalWrite(ST4DEs,HIGH);                     // clock
        state=digitalRead(ST4RAw); r_parity+=state;    // recv data bit
        bitWrite(*data_in,i,state);                    
        delayMicroseconds(XMIT_TIME);
      }
      
      // parity bit
      digitalWrite(ST4DEs,LOW);                        // clock
      digitalWrite(ST4DEn,s_parity&1);                 // send parity bit
      delayMicroseconds(XMIT_TIME);
      digitalWrite(ST4DEs,HIGH);                       // clock
      if ((r_parity&1)!=digitalRead(ST4RAw)) _recv_error=true; // recv parity bit
      delayMicroseconds(XMIT_TIME);

      // parity ck bit
      digitalWrite(ST4DEs,LOW);                        // clock
      digitalWrite(ST4DEn,_recv_error);                // send local parity check
      delayMicroseconds(XMIT_TIME);
      digitalWrite(ST4DEs,HIGH);                       // clock
      if (digitalRead(ST4RAw)==HIGH) _send_error=true; // recv remote parity, ok?
      delayMicroseconds(XMIT_TIME);

      // stop bit
      digitalWrite(ST4DEs,LOW);                        // clock
      digitalWrite(ST4DEn,LOW);                        // send
      delayMicroseconds(XMIT_TIME);
      digitalWrite(ST4DEs,HIGH);                       // clock
      if (digitalRead(ST4RAw)!=LOW) _frame_error=true; // recv stop bit
      delayMicroseconds(XMIT_TIME);

      lastMicros=micros();
      if (_frame_error) return false; else return true;
    }

    byte _recv_head = 0;
};

void Mst4::begin() {
  _xmit_head=0; _xmit_tail=0; _xmit_buffer[0]=0;
  _recv_head=0; _recv_tail=0; _recv_buffer[0]=0;
}

void Mst4::begin(long baud) {
  begin();
}

void Mst4::end() {
  _xmit_head=0; _xmit_tail=0; _xmit_buffer[0]=0;
  _recv_head=0; _recv_tail=0; _recv_buffer[0]=0;
}

size_t Mst4::write(uint8_t data) {
  unsigned long t_start=millis();
  byte xh=_xmit_head; xh--; while (_xmit_tail == xh) { poll(); if ((millis()-t_start)>_timeout) return 0; }
  _xmit_buffer[_xmit_tail]=data; _xmit_tail++;
  _xmit_buffer[_xmit_tail]=0;
  return 1;
}

size_t Mst4::write(const uint8_t *data, size_t quantity) {
  // fail if trying to write more than the buffer can hold
  if ((int)quantity>254) return 0;

  for (int i=0; i<(int)quantity; i++) { if (!write(data[i])) return 0; }
  return 1;
}

int Mst4::available(void) {
  int a=0;
  for (byte b=_recv_head; _recv_buffer[b]!=(char)0; b++) a++;
  return a;
}

int Mst4::read(void) {
  char c=_recv_buffer[_recv_head]; if (c!=0) _recv_head++;
  if (c==0) c=-1;
  return c;
}

int Mst4::peek(void) {
  int c=_recv_buffer[_recv_head];
  if (c==0) c=-1;

  return c;
}

void Mst4::flush(void) {
  unsigned long startMs=millis();
  int c;
  do {
    poll();
    c=_xmit_buffer[_xmit_head];
  } while ((c!=0) || ((millis()-startMs)<_timeout));
}

Mst4 SerialST4;

