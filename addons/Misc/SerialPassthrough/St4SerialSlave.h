// -----------------------------------------------------------------------------------
// Serial ST4 slave

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

void Sst4::begin(long baudRate=9600) {
  _xmit_head=0; _xmit_tail=0; _xmit_buffer[0]=0;
  _recv_head=0; _recv_tail=0; _recv_buffer[0]=0;
  
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
  _xmit_head=0; _xmit_tail=0; _xmit_buffer[0]=0;
  _recv_head=0; _recv_tail=0; _recv_buffer[0]=0;
}

bool Sst4::active() {
  static unsigned long comp=0;
  bool result=false;
  noInterrupts();
  if (comp!=lastMs) { result=true; comp=lastMs; }
  interrupts();
  return result;
}

size_t Sst4::write(uint8_t data) {
  unsigned long t_start=millis();
  byte xh=_xmit_head; xh--; while (_xmit_tail == xh) { if ((millis()-t_start)>_timeout) return 0; }
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
  unsigned long startMs=millis();
  int c;
  do {
    noInterrupts();
    c=_xmit_buffer[_xmit_head];
    interrupts();
  } while ((c!=0) && ((millis()-startMs)<_timeout));
}

Sst4 SerialST4;

volatile uint8_t data_in = 0;
volatile uint8_t data_out = 0;

void dataClock() {
  static volatile unsigned long t=0;
  static volatile int i=9;
  static volatile boolean frame_error=false;
  static volatile boolean send_error=false;
  static volatile boolean recv_error=false;
  static volatile uint8_t s_parity=0;
  static volatile uint8_t r_parity=0;
  volatile uint8_t state=0;
  
  SerialST4.lastMs=millis();
  unsigned long t1=t; t=micros();
  volatile unsigned long elapsed=t-t1;

  if (digitalRead(ST4DEs)==HIGH) {
    state=digitalRead(ST4DEn); 
    if (i==8) { if (state!=LOW) frame_error=true; }          // recv start bit
    if ((i>=0) && (i<=7)) { r_parity+=state; bitWrite(data_in,i,state); } // recv data bit
    if (i==-1) { if ((r_parity&1)!=state) recv_error=true; } // recv parity bit
    if (i==-2) { if (state==HIGH) send_error=true; }         // recv remote parity, ok?
    if (i==-3) {                                             // recv stop bit
      if (state!=LOW) frame_error=true;

      if ((!frame_error) && (!recv_error)) {
        if (data_in!=0) {
          SerialST4._recv_buffer[SerialST4._recv_tail]=(char)data_in; 
          SerialST4._recv_tail++;
        }
        SerialST4._recv_buffer[SerialST4._recv_tail]=(char)0;
      }
    }
  } else {
    if ((elapsed>1500L) || (i==-3)) {
      i=9; 
      r_parity=0; s_parity=0;
      recv_error=false;

      // send the same data again?
      if ((!send_error) && (!frame_error)) {
        data_out=SerialST4._xmit_buffer[SerialST4._xmit_head]; 
        if (data_out!=0) SerialST4._xmit_head++;
      } else { send_error=false; frame_error=false; }
    }
    i--;
    if (i==8) { digitalWrite(ST4RAw,LOW); }                  // send start bit
    if ((i>=0) && (i<=7)) {                                  // send data bit
      state=bitRead(data_out,i); s_parity+=state; digitalWrite(ST4RAw,state);
    }
    if (i==-1) { digitalWrite(ST4RAw,s_parity&1); }          // send parity bit
    if (i==-2) { digitalWrite(ST4RAw,recv_error); }          // send local parity check
    if (i==-3) { digitalWrite(ST4RAw,LOW); }                 // send stop bit
  }
}

// this routine keeps a 12.5Hz "tone" on the RAe pin (always) but also on the
// RAw pin when the data comms clock from OnStep isn't running
void shcTone() {
  static volatile boolean tone_state=false;
  if (tone_state) { 
    tone_state=false; 
    digitalWrite(ST4RAe,HIGH); 
    if (millis()-SerialST4.lastMs>2000L) digitalWrite(ST4RAw,HIGH);
  } else  {
    tone_state=true;
    digitalWrite(ST4RAe,LOW);
    if (millis()-SerialST4.lastMs>2000L) digitalWrite(ST4RAw,LOW); 
  }
}

