// -----------------------------------------------------------------------------------
// Communication routines for Serial0, Serial1, and bit banged SPI

class pserial {
  public:
    void begin(unsigned long baud);
    boolean available();
    char read();
    void print(const char data[]);
    void putch(char c);
    void putf(double f);
    void puts(const char data[]);
    boolean transmit();
#if defined(__AVR_ATmega2560__)
    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
#endif
};

class pserial1 {
  public:
    void begin(unsigned long baud);
    boolean available();
    char read();
    void print(const char data[]);
    void putch(char c);
    void putf(double f);
    void puts(const char data[]);
    boolean transmit();
#if defined(__AVR_ATmega2560__)
    volatile char _recv_buffer[256]   = "";
    volatile byte _recv_tail = 0;
  private:
    byte _recv_head          = 0;
    char _xmit_buffer[50]    = "";
    byte _xmit_index         = 0;
#endif
};

class bbspi {
  public:  
    void begin(int cs, int sck, int miso, int mosi);
    void pause();
    void end();
    uint8_t transfer(uint8_t data_out);
    uint32_t transfer32(uint32_t data_out);
  private:  
    int _cs = 0;
    int _sck = 0;
    int _miso = 0;
    int _mosi = 0;
};


