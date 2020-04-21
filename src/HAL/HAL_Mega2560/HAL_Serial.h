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

    // Main loop calls this to send characters on their way without interrupts
    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR0A & (1<<UDRE0)) ) { UDR0 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }

    void flush()
    {
      while (transmit()) {};
    }
    
    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

pserial SerialA;

// UART Receive Complete Interrupt Handler for Serial0
ISR(USART0_RX_vect)  {
  SerialA._recv_buffer[SerialA._recv_tail]=UDR0; 
  SerialA._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}

#ifdef HAL_SERIAL_B_ENABLED
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

    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR1A & (1<<UDRE1)) ) { UDR1 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }

    void flush()
    {
      while (transmit()) {};
    }

    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

pserial1 SerialB;

// UART Receive Complete Interrupt Handler for Serial1
ISR(USART1_RX_vect)  {
  SerialB._recv_buffer[SerialB._recv_tail]=UDR1; 
  SerialB._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}
#endif

#ifdef HAL_SERIAL_C_ENABLED
#ifdef HAL_SERIAL_C_SERIAL2
class pserial2 {
  public:
    void begin(unsigned long baud) {
      unsigned int ubrr=F_CPU/16/baud-1;
    
      _xmit_index=0;
      _xmit_buffer[0]=0;
      _recv_head =0;
      _recv_tail =0;
      _recv_buffer[0]=0;
    
      // Set baud rate
      UBRR2H = (unsigned char)(ubrr>>8);
      UBRR2L = (unsigned char)ubrr;
      
      // Disable U2X mode
      UCSR2A = 0;

      // Enable receiver and transmitter
      UCSR2B = (1<<RXEN2) | (1<<TXEN2) | (1<<RXCIE2);
    
      // 8-bit, 1 stop bit, no parity, asynchronous UART
      UCSR2C = (1 << UCSZ21) | (1 << UCSZ20) | (0 << USBS2)   |
               (0 << UPM21)  | (0 << UPM20)  | (0 << UMSEL21) |
               (0 << UMSEL20);
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

    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR2A & (1<<UDRE2)) ) { UDR2 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }

    void flush()
    {
      while (transmit()) {};
    }

    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

pserial2 SerialC;

// UART Receive Complete Interrupt Handler for Serial3
ISR(USART2_RX_vect)  {
  SerialC._recv_buffer[SerialC._recv_tail]=UDR2; 
  SerialC._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}

#else

class pserial3 {
  public:
    void begin(unsigned long baud) {
      unsigned int ubrr=F_CPU/16/baud-1;
    
      _xmit_index=0;
      _xmit_buffer[0]=0;
      _recv_head =0;
      _recv_tail =0;
      _recv_buffer[0]=0;
    
      // Set baud rate
      UBRR3H = (unsigned char)(ubrr>>8);
      UBRR3L = (unsigned char)ubrr;
      
      // Disable U2X mode
      UCSR3A = 0;

      // Enable receiver and transmitter
      UCSR3B = (1<<RXEN3) | (1<<TXEN3) | (1<<RXCIE3);
    
      // 8-bit, 1 stop bit, no parity, asynchronous UART
      UCSR3C = (1 << UCSZ31) | (1 << UCSZ30) | (0 << USBS3)   |
               (0 << UPM31)  | (0 << UPM30)  | (0 << UMSEL31) |
               (0 << UMSEL30);
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

    boolean transmit()
    {
      if (_xmit_buffer[_xmit_index]==(char)0) return false;
      if ( ( UCSR3A & (1<<UDRE3)) ) { UDR3 = _xmit_buffer[_xmit_index]; _xmit_index++; }
      return true;
    }

    void flush()
    {
      while (transmit()) {};
    }

    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
};

pserial3 SerialC;

// UART Receive Complete Interrupt Handler for Serial3
ISR(USART3_RX_vect)  {
  SerialC._recv_buffer[SerialC._recv_tail]=UDR3; 
  SerialC._recv_tail++; // buffer is 256 bytes so this byte variable wraps automatically
}
#endif
#endif

