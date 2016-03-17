// -----------------------------------------------------------------------------------
// Communication routines for Serial0 and Serial1
// these are more compact and faster than the Arduino provided one's

#if defined(__AVR__)

void Serial_Init(unsigned int baud) {
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

void Serial1_Init(unsigned int baud) {
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

void Serial_Init(unsigned int baud) {
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

void Serial1_Init(unsigned int baud) {
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
