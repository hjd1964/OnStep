// -----------------------------------------------------------------------------------
// Communication routines for Serial0 and Serial1
// these are more compact and faster than the Arduino provided one's

void lookf(const char comment[], double f)
{
  char temp[10];
  Serial1_send(comment); dtostrf(f,4,6,temp); Serial1_send(temp); Serial1_send("\r\n");
}

#if defined(__AVR_ATmega2560__)

void Serial_Init(unsigned long baud) {
  unsigned int ubrr=F_CPU/16/baud-1;

  Serial_xmit_index=0;
  Serial_xmit_buffer[0]=0;
  Serial_recv_head =0;
  Serial_recv_tail =0;
  Serial_recv_buffer[0]=0;

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

void Serial_send(const char data[]) {
  Serial_print(data);
  do {} while (Serial_transmit());
}

// Loads up a string to send
void Serial_print(const char data[])
{
  strcpy(Serial_xmit_buffer,data);
  Serial_xmit_index=0;
}

// Main loop calls this to send characters on their way without interrupts
boolean Serial_transmit()
{
  if (Serial_xmit_buffer[Serial_xmit_index]==(char)0) return false;
  if ( ( UCSR0A & (1<<UDRE0)) ) { UDR0 = Serial_xmit_buffer[Serial_xmit_index]; Serial_xmit_index++; }
  return true;
}

// For debugging, sends one char at a time to Serial0 interface
boolean Serial_char(char c)
{
  while ( !( UCSR0A & (1<<UDRE0)) ) {  }
  UDR0 = c;
  return true;
}

boolean Serial_available()
{
  return !(Serial_recv_buffer[Serial_recv_head]==char(0));
}

char Serial_read()
{
  char c;
  cli();
  Serial_recv_buffer[Serial_recv_tail]=(char)0; // always mark the tail
  c=Serial_recv_buffer[Serial_recv_head];
  sei();
  if (c!=0) Serial_recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
  return c;
}

void Serial1_Init(unsigned long baud) {
  unsigned int ubrr=F_CPU/16/baud-1;

  Serial1_xmit_index=0;
  Serial1_xmit_buffer[0]=0;
  Serial1_recv_head =0;
  Serial1_recv_tail =0;
  Serial1_recv_buffer[0]=0;

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

void Serial1_send(const char data[])
{
  Serial1_print(data);
  do {} while (Serial1_transmit());
}

// Loads up a string to sen
void Serial1_print(const char data[])
{
  strcpy(Serial1_xmit_buffer,data);
  Serial1_xmit_index=0;
}

// Main loop calls this to send characters on their way without interrupts
boolean Serial1_transmit()
{
  if (Serial1_xmit_buffer[Serial1_xmit_index]==(char)0) return false;
  if ( ( UCSR1A & (1<<UDRE1)) ) { UDR1 = Serial1_xmit_buffer[Serial1_xmit_index]; Serial1_xmit_index++; }
  return true;
}

boolean Serial1_available()
{
  return !(Serial1_recv_buffer[Serial1_recv_head]==char(0));
}

char Serial1_read()
{
  char c;
  cli();
  Serial1_recv_buffer[Serial1_recv_tail]=(char)0; // always mark the tail
  c=Serial1_recv_buffer[Serial1_recv_head];
  sei();
  if (c!=0) Serial1_recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
  return c;
}

#else  // on non-AVR platforms, try to use the built-in serial stuff...

void Serial_Init(unsigned long baud) {
  Serial.begin(baud);
}

void Serial_send(const char data[]) {
  Serial.print(data);
  do {} while (Serial_transmit());
}

void Serial_print(const char data[]) {
  Serial.print(data);
}

boolean Serial_transmit() {
  return false;
}

boolean Serial_available() {
  return Serial.available();
}

char Serial_read() {
  return Serial.read();
}

#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)
// use serial7 as serial1 (serial1) as serial1 is not readily available on the board
// if you really want to use serial1 you will need to do some soldering
void Serial1_Init(unsigned long baud) {
  Serial7.begin(baud);
}

void Serial1_send(const char data[])
{
  Serial1_print(data);
  do {} while (Serial1_transmit());
}

void Serial1_print(const char data[]) {
  Serial7.print(data);
}

boolean Serial1_transmit() {
  return false;
}

boolean Serial1_available() {
  return Serial7.available();
}

char Serial1_read() {
  return Serial7.read();
}
#else
void Serial1_Init(unsigned long baud) {
  Serial1.begin(baud);
}

void Serial1_send(const char data[])
{
  Serial1_print(data);
  do {} while (Serial1_transmit());
}

void Serial1_print(const char data[]) {
  Serial1.print(data);
}

boolean Serial1_transmit() {
  return false;
}

boolean Serial1_available() {
  return Serial1.available();
}

char Serial1_read() {
  return Serial1.read();
}
#endif

#endif

// -----------------------------------------------------------------------------------
// Simple soft SPI routines (CPOL=1, CPHA=1)
int _cs = 0;
int _sck = 0;
int _miso = 0;
int _mosi = 0;

void spiStart(int cs, int sck, int miso, int mosi)
{
  _cs=cs; pinMode(cs,OUTPUT); digitalWrite(cs,HIGH);
  delayMicroseconds(1);
  _sck=sck; pinMode(_sck,OUTPUT); digitalWrite(_sck,HIGH);
  _miso=miso; pinMode(_miso,INPUT);
  _mosi=mosi; pinMode(_mosi,OUTPUT);
  digitalWrite(cs,LOW);
  delayMicroseconds(1);
}

void spiEnd() {
  delayMicroseconds(1);
  digitalWrite(_cs, HIGH);
}

void spiPause() {
  digitalWrite(_cs, HIGH);
  delayMicroseconds(1);
  digitalWrite(_cs, LOW);
  delayMicroseconds(1);
}

uint8_t spiTransfer(uint8_t data_out)
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

uint32_t spiTransfer32(uint32_t data_out)
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


