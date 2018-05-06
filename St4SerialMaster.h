// ------------------------------------------------------------------------------------
// Serial ST4 master

/*
Smart ST4 hand controller (SHC) data communication scheme:

5V power ---- Teensy3.2, Nano, etc.
Gnd ---------

HC              Signal               OnStep
RAw  ----        Data         --->   Recv. data
DEs  <---        Clock        ----   Clock
DEn  <---        Data         ----   Send data
RAe  ---- 12.5Hz Square wave  --->   100% sure SHC is present, switches DEs & DEn to OUTPUT

Data is exchanged on clock edges similar to SPI so is timing insensitive (no need to disable interrupts.)

One data byte is exchanged for each pass of OnStep's main-loop, this takes about 1ms.  The maximum main-loop time is 10ms in OnStep so 100 bytes/second minimum (Mega2560, even during slews.)

Data flows constantly in both directions.
A value 0x00 byte is ignored on both sides.

*/

#include "Stream.h"

class Mst4 : public Stream
{
  public:
    void begin();
    void end();

    // recvs one char and transmits one char to/from buffers; recvd chars <32 are returned directly and bypass the buffer
    inline char poll() {
      char c=trans(_xmit_buffer[_xmit_head]);
      if (_xmit_buffer[_xmit_head]!=(char)0) _xmit_head++;
      if (c>=(char)32) { _recv_buffer[_recv_tail]=c; _recv_tail++; _recv_buffer[_recv_tail]=(char)0; return (char)0; } else return c;
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

  private:
    inline uint8_t trans(uint8_t data_out)
    {
      uint8_t data_in = 0;
      
      delayMicroseconds(100); // SHC_CLOCK HIGH for > 100 us means data byte is done on remote
      for (int i=7; i>=0; i--)
      {
        digitalWrite(ST4DEs,LOW); // clock
        digitalWrite(ST4DEn,bitRead(data_out,i)); // send
        delayMicroseconds(50);
        digitalWrite(ST4DEs,HIGH); // clock
        bitWrite(data_in,i,digitalRead(ST4RAw)); // recv
        delayMicroseconds(50);
      }
      return data_in;
    }

    byte _recv_head = 0;
    long time_out = 500;
};

void Mst4::begin() {
  flush();
}

void Mst4::end() {
}

size_t Mst4::write(uint8_t data) {
  unsigned long t_start=millis();
  while (_xmit_tail+1 == _xmit_head) { poll(); if ((long)(millis()-t_start)>time_out) return 0; }
  noInterrupts();
  _xmit_buffer[_xmit_tail]=data; _xmit_tail++;
  _xmit_buffer[_xmit_tail]=0;
  interrupts();
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
  noInterrupts();
  char c=_recv_buffer[_recv_head]; if (c!=0) _recv_head++;
  interrupts();
  if (c==0) c=-1;
  return c;
}

int Mst4::peek(void) {
  noInterrupts();
  int c=_recv_buffer[_recv_head];
  interrupts();
  if (c==0) c=-1;

  return c;
}

void Mst4::flush(void) {
  _xmit_head=0;
  _xmit_tail=0;
  _xmit_buffer[0]=0;
  _recv_head=0;
  _recv_tail=0;
  _recv_buffer[0]=0;
}

Mst4 PSerialST4;

