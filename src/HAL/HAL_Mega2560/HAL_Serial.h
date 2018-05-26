// -----------------------------------------------------------------------------------
// Low overhead communication routines for Serial0, Serial1

class pserial {
  public:
    // these are more compact and faster than the Arduino provided one's
    void begin(unsigned long baud) {
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
    
    boolean available()
    {
      return !(_recv_buffer[_recv_head]==char(0));
    }
    
    char read()
    {
      char c;
      cli();
      _recv_buffer[_recv_tail]=(char)0; // always mark the tail
      c=_recv_buffer[_recv_head];
      sei();
      if (c!=0) _recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
      return c;
    }
    
    void print(const char data[])
    {
      strcpy(_xmit_buffer,data);
      _xmit_index=0;
    }
    
    void putch(char c)
    {
      while ( !( UCSR0A & (1<<UDRE0)) ) {  }
      UDR0 = c;
    }
    
    void putf(double f)
    {
      char temp[20]; dtostrf(f,4,6,temp); puts(temp);
    }
    
    void putl(long l)
    {
      char temp[20]; sprintf(temp,"%ld",l); puts(temp);
    }
    
    void puts(const char data[]) {
      print(data);
      do {} while (transmit());
    }
    
    // Main loop calls this to send characters on their way without interrupts
    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR0A & (1<<UDRE0)) ) { UDR0 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }
    
    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

class pserial1 {
  public:
    void begin(unsigned long baud) {
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
    
    boolean available()
    {
      return !(_recv_buffer[_recv_head]==char(0));
    }
    
    char read()
    {
      char c;
      cli();
      _recv_buffer[_recv_tail]=(char)0; // always mark the tail
      c=_recv_buffer[_recv_head];
      sei();
      if (c!=0) _recv_head++; // buffer is 256 bytes so this byte variable wraps automatically
      return c;
    }
    
    void print(const char data[])
    {
      strcpy(_xmit_buffer,data);
      _xmit_index=0;
    }
    
    void putch(char c)
    {
      while ( !( UCSR1A & (1<<UDRE1)) ) {  }
      UDR1 = c;
    }
    
    void putf(double f)
    {
      char temp[20]; dtostrf(f,4,6,temp); puts(temp);
    }
    
    void putl(long l)
    {
      char temp[20]; sprintf(temp,"%ld",l); puts(temp);
    }
    
    void puts(const char data[])
    {
      print(data);
      do {} while (transmit());
    }
    
    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR1A & (1<<UDRE1)) ) { UDR1 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }

    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

pserial SerialA;
pserial1 SerialB;

// UART Receive Complete Interrupt Handler for Serial0
ISR(USART0_RX_vect)  {
  SerialA._recv_buffer[SerialA._recv_tail]=UDR0; 
  SerialA._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}

// UART Receive Complete Interrupt Handler for Serial1
ISR(USART1_RX_vect)  {
  SerialB._recv_buffer[SerialB._recv_tail]=UDR1; 
  SerialB._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}
