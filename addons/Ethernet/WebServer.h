// -----------------------------------------------------------------------------------
// Web server

// misc.
#define WebSocketTimeOut 10000
#define WEBHANDLERCOUNT     21

typedef void (* webFunction) (EthernetClient *client);

class WebServer {
  public:
    void init();
    void handleClient();
    void setResponseHeader(const char *str);
    void on(String fn, webFunction handler);
#if SD_CARD == ON
    void on(String fn);
#endif
    void onNotFound(webFunction handler);
    String arg(String id);

    bool SDfound=false;
  private:
#if SD_CARD == ON
    void sdPage(String fn, EthernetClient *client);
#endif

    String inputBuffer;
    char responseHeader[200]="";
#if SD_CARD == ON
    bool modifiedSinceFound=false;
#endif

    webFunction notFoundHandler=NULL;
    webFunction handlers[WEBHANDLERCOUNT];
    String handlers_fn[WEBHANDLERCOUNT];
    int handler_count;
    
    String parameters[WEBHANDLERCOUNT];
    String values[WEBHANDLERCOUNT];
    int parameter_count;
};

const char http_defaultHeader[] PROGMEM = "HTTP/1.1 200 OK\r\n" "Content-Type: text/html\r\n" "Connection: close\r\n" "\r\n";
