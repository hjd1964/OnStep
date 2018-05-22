// -----------------------------------------------------------------------------------
// Serial ST4 slave

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

void dataClock();
void shcTone();

class Sst4 : public Stream
{
  public:
    void begin(long baudRate);
    void end();
    bool active();
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
    volatile unsigned long lastMs   = 0;

  private:
    byte _recv_head = 0;
    long time_out = 500;
};

extern Sst4 SerialST4;

