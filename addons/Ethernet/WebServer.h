// -----------------------------------------------------------------------------------
// Web server

// Turn ON to allow webserver debug messages
#define WEBSERVER_DEBUG OFF

#if WEBSERVER_DEBUG == ON
  #define W(x) V(x)
  #define WF(x) VF(x)
  #define WL(x) VL(x)
  #define WLF(x) VLF(x)
#else
  #define W(x)
  #define WF(x)
  #define WL(x)
  #define WLF(x)
#endif

// misc.
#define WebSocketTimeOut 10000
#define HANDLER_COUNT_MAX     21

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
    webFunction handlers[HANDLER_COUNT_MAX];
    String handlers_fn[HANDLER_COUNT_MAX];
    int handler_count;
    
    String parameters[HANDLER_COUNT_MAX];
    String values[HANDLER_COUNT_MAX];
    int parameter_count;
};

const char http_defaultHeader[] PROGMEM = "HTTP/1.1 200 OK\r\n" "Content-Type: text/html\r\n" "Connection: close\r\n" "\r\n";
