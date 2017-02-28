// -----------------------------------------------------------------------------------
// Communication routines for Serial0, Serial1, and bit banged SPI

#if defined(__AVR_ATmega2560__)

// these are more compact and faster than the Arduino provided one's
void pserial::begin(unsigned long baud) {
  unsigned int ubrr=F_CPU/16/baud-1;

  _xmit_index=0;
  _xmit_buffer[0]=0;
  _recv_head =0;
  _recv_tail =0;
  _recv_buffer[0]=0;

  // Set baud rate
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  
  // Disable U2X mode
  UCSR0A = 0;
  
  // Enable receiver and transmitter
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0)   |
           (0 << UPM01)  | (0 << UPM00)  | (0 << UMSEL01) |
           (0 << UMSEL00);
}

boolean pserial::available()
{
  return !(_recv_buffer[_recv_head]==char(0));
}

char pserial::read()
{
  char c;
  cli();
  _recv_buffer[_recv_tail]=(char)0; // always mark the tail
  c=_recv_buffer[_recv_head];
  sei();
  if (c!=0) _recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
  return c;
}

void pserial::print(const char data[])
{
  strcpy(_xmit_buffer,data);
  _xmit_index=0;
}

void pserial::putch(char c)
{
  while ( !( UCSR0A & (1<<UDRE0)) ) {  }
  UDR0 = c;
}

void pserial::putf(double f)
{
  char temp[20]; dtostrf(f,4,6,temp); puts(temp);
}

void pserial::puts(const char data[]) {
  print(data);
  do {} while (transmit());
}

// Main loop calls this to send characters on their way without interrupts
boolean pserial::transmit()
{
  if (_xmit_buffer[_xmit_index]==(char)0) return false;
  if ( ( UCSR0A & (1<<UDRE0)) ) { UDR0 = _xmit_buffer[_xmit_index]; _xmit_index++; }
  return true;
}

void pserial1::begin(unsigned long baud) {
  unsigned int ubrr=F_CPU/16/baud-1;

  _xmit_index=0;
  _xmit_buffer[0]=0;
  _recv_head =0;
  _recv_tail =0;
  _recv_buffer[0]=0;

  // Set baud rate
  UBRR1H = (unsigned char)(ubrr>>8);
  UBRR1L = (unsigned char)ubrr;
  
  // Disable U2X mode
  UCSR1A = 0;
  
  // Enable receiver and transmitter
  UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (0 << USBS1)   |
           (0 << UPM11)  | (0 << UPM10)  | (0 << UMSEL11) |
           (0 << UMSEL10);
}

boolean pserial1::available()
{
  return !(_recv_buffer[_recv_head]==char(0));
}

char pserial1::read()
{
  char c;
  cli();
  _recv_buffer[_recv_tail]=(char)0; // always mark the tail
  c=_recv_buffer[_recv_head];
  sei();
  if (c!=0) _recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
  return c;
}

void pserial1::print(const char data[])
{
  strcpy(_xmit_buffer,data);
  _xmit_index=0;
}

void pserial1::putch(char c)
{
  while ( !( UCSR1A & (1<<UDRE1)) ) {  }
  UDR1 = c;
}

void pserial1::putf(double f)
{
  char temp[20]; dtostrf(f,4,6,temp); puts(temp);
}

void pserial1::puts(const char data[])
{
  print(data);
  do {} while (transmit());
}

boolean pserial1::transmit()
{
  if (_xmit_buffer[_xmit_index]==(char)0) return false;
  if ( ( UCSR1A & (1<<UDRE1)) ) { UDR1 = _xmit_buffer[_xmit_index]; _xmit_index++; }
  return true;
}

#else

// on non-AVR platforms, use the built-in serial stuff...
void pserial::begin(unsigned long baud) { Serial.begin(baud); }
boolean pserial::available() { return Serial.available(); }
char pserial::read() { return Serial.read(); }
void pserial::print(const char data[]) { Serial.print(data); }
void pserial::putch(char c) { Serial.print(c); }
void pserial::putf(double f) { char temp[20]; dtostrf(f,4,6,temp); puts(temp); }
void pserial::puts(const char data[]) { Serial.print(data); do {} while (transmit()); }
boolean pserial::transmit() { return false; }

#if !defined(__TM4C1294NCPDT__) && !defined(__TM4C1294XNCZAD__)

// on non-AVR platforms, use the built-in serial stuff...
void pserial1::begin(unsigned long baud) { Serial1.begin(baud); }
boolean pserial1::available() { return Serial1.available(); }
char pserial1::read() { return Serial1.read(); }
void pserial1::print(const char data[]) { Serial1.print(data); }
void pserial1::putch(char c) { Serial1.print(c); }
void pserial1::putf(double f) { char temp[20]; dtostrf(f,4,6,temp); puts(temp); }
void pserial1::puts(const char data[]) { print(data); do {} while (transmit()); }
boolean pserial1::transmit() { return false; }

#else

// use serial7 as serial1 (serial1) as serial1 is not readily available on the board
// if you really want to use serial1 you will need to do some soldering
void pserial1::begin(unsigned long baud) { Serial7.begin(baud); }
boolean pserial1::available() { return Serial7.available(); }
char pserial1::read() { return Serial7.read(); }
void pserial1::print(const char data[]) { Serial7.print(data); }
void pserial1::putch(char c) { Serial7.print(c); }
void pserial1::putf(double f) { char temp[20]; dtostrf(f,4,6,temp); puts(temp); }
void pserial1::puts(const char data[]) { print(data); do {} while (transmit()); }
boolean pserial1::transmit() { return false; }

#endif

#endif

// -----------------------------------------------------------------------------------
// Simple soft SPI routines (CPOL=1, CPHA=1)

void bbspi::begin(int cs, int sck, int miso, int mosi)
{
  _cs=cs; pinMode(cs,OUTPUT); digitalWrite(cs,HIGH);
  delayMicroseconds(1);
  _sck=sck; pinMode(_sck,OUTPUT); digitalWrite(_sck,HIGH);
  _miso=miso; pinMode(_miso,INPUT);
  _mosi=mosi; pinMode(_mosi,OUTPUT);
  digitalWrite(cs,LOW);
  delayMicroseconds(1);
}

void bbspi::pause() {
  digitalWrite(_cs, HIGH);
  delayMicroseconds(1);
  digitalWrite(_cs, LOW);
  delayMicroseconds(1);
}

void bbspi::end() {
  delayMicroseconds(1);
  digitalWrite(_cs, HIGH);
}

uint8_t bbspi::transfer(uint8_t data_out)
{
  uint8_t data_in = 0;
  
  for(int i=7; i>=0; i--)
  {
    digitalWrite(_sck,LOW);
    digitalWrite(_mosi,bitRead(data_out,i));
    delayMicroseconds(1);
    digitalWrite(_sck,HIGH);
    bitWrite(data_in,i,digitalRead(_miso));
    delayMicroseconds(1);
  }
  
  return data_in;
}

uint32_t bbspi::transfer32(uint32_t data_out)
{
  uint32_t data_in = 0;
  
  for(int i=31; i>=0; i--)
  {
    digitalWrite(_sck,LOW);
    digitalWrite(_mosi,bitRead(data_out,i));
    delayMicroseconds(1);
    digitalWrite(_sck,HIGH);
    bitWrite(data_in,i,digitalRead(_miso));
    delayMicroseconds(1);
  }

  return data_in;
}


