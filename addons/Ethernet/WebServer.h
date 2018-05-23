// -----------------------------------------------------------------------------------
// Web server

// misc.
#define WebSocketTimeOut 10000
typedef void (* webFunction) (EthernetClient *client);

class WebServer {
  public:
    void init();
    void handleClient();
    void setResponseHeader(const char *str);
    void on(String fn, webFunction handler);
#ifdef SD_CARD_ON
    void on(String fn);
#endif
    void onNotFound(webFunction handler);
    String arg(String id);

    bool SDfound=false;
  private:
#ifdef SD_CARD_ON
    void sdPage(String fn, EthernetClient *client);
#endif

    String inputBuffer;
    char responseHeader[200]="";
#ifdef SD_CARD_ON
    bool modifiedSinceFound=false;
#endif

    webFunction notFoundHandler=NULL;
    webFunction handlers[20];
    String handlers_fn[20];
    int handler_count;
    
    String parameters[20];
    String values[20];
    int parameter_count;
};

const char http_defaultHeader[] PROGMEM = "HTTP/1.1 200 OK\r\n" "Content-Type: text/html\r\n" "Connection: close\r\n" "\r\n";

