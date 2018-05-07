// ------------------------------------------------------------------------------------
// Serial ST4 slave

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

#ifdef __ARM_Teensy3__
  IntervalTimer Timer1;  // built into Teensyduino
#else
  #include <TimerOne.h>  // from https://github.com/PaulStoffregen/TimerOne
#endif

void dataClock();
void shcTone();

class Sst4 : public Stream
{
  public:
    void begin();
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
    volatile unsigned long last=0;

  private:
    byte _recv_head = 0;
    long time_out = 500;
};

void Sst4::begin() {
  flush();
  
  pinMode(ST4DEs,INPUT_PULLUP);
  pinMode(ST4DEn,INPUT_PULLUP);
  pinMode(ST4RAe,OUTPUT);
  pinMode(ST4RAw,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ST4DEs),dataClock,CHANGE);

#ifdef __ARM_Teensy3__
  Timer1.begin(shcTone,40000);
#else
  Timer1.initialize(40000);
  Timer1.attachInterrupt(shcTone);
#endif
}

void Sst4::end() {
  pinMode(ST4RAe,INPUT_PULLUP);
  pinMode(ST4RAw,INPUT_PULLUP);
  detachInterrupt(digitalPinToInterrupt(ST4DEs));
#ifdef __ARM_Teensy3__
  Timer1.end();
#else
  Timer1.stop();
#endif
}

bool Sst4::active() {
  static unsigned long comp=0;
  bool result=false;
  noInterrupts();
  if (comp!=last) { result=true; comp=last; }
  interrupts();
  return result;
}

size_t Sst4::write(uint8_t data) {
  unsigned long t_start=millis();
  while (_xmit_tail+1 == _xmit_head) { if ((long)(millis()-t_start)>time_out) return 0; }
  noInterrupts();
  _xmit_buffer[_xmit_tail]=data; _xmit_tail++;
  _xmit_buffer[_xmit_tail]=0;
  interrupts();
  return 1;
}

size_t Sst4::write(const uint8_t *data, size_t quantity) {
  // fail if trying to write more than the buffer can hold
  if ((int)quantity>254) return 0;

  for (int i=0; i<(int)quantity; i++) { if (!write(data[i])) return 0; }
  return 1;
}

int Sst4::available(void) {
  int a=0;
  noInterrupts();
  for (byte b=_recv_head; _recv_buffer[b]!=(char)0; b++) a++;
  interrupts();
  return a;
}

int Sst4::read(void) {
  noInterrupts();
  int c=_recv_buffer[_recv_head]; if (c!=0) _recv_head++;
  interrupts();
  if (c==0) c=-1;
  return c;
}

int Sst4::peek(void) {
  noInterrupts();
  int c=_recv_buffer[_recv_head];
  interrupts();
  if (c==0) c=-1;

  return c;
}

void Sst4::flush(void) {
  _xmit_head=0;
  _xmit_tail=0;
  _xmit_buffer[0]=0;
  _recv_head=0;
  _recv_tail=0;
  _recv_buffer[0]=0;
}

Sst4 PSerialST4;

volatile int i=8;
volatile uint8_t data_in = 0;
volatile uint8_t data_out = 0;

void dataClock() {
  unsigned long temp=micros();
  long ellapsed=(long)(temp-PSerialST4.last);
  PSerialST4.last=temp;

  if (digitalRead(ST4DEs)==HIGH) {
    bitWrite(data_in,i,digitalRead(ST4DEn)); // recv
    if (i==0) {
      if (data_in!=0) {
        PSerialST4._recv_buffer[PSerialST4._recv_tail]=(char)data_in; 
        PSerialST4._recv_tail++;
      }
      PSerialST4._recv_buffer[PSerialST4._recv_tail]=(char)0;
    }
  } else {
    if (ellapsed>200) {
      i=8;
      data_out=PSerialST4._xmit_buffer[PSerialST4._xmit_head]; 
      if (data_out!=0) PSerialST4._xmit_head++;
    }
    i--; if (i<0) i=0;
    digitalWrite(ST4RAw,bitRead(data_out,i)); // send
  }
}

volatile boolean __tone_state=false;
void shcTone() {
  if (__tone_state) { __tone_state=false; digitalWrite(ST4RAe,HIGH); digitalWrite(ST4RAw,LOW); } else  { __tone_state=true; digitalWrite(ST4RAe,LOW); digitalWrite(ST4RAe,LOW); }
}

