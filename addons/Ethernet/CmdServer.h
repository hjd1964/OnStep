// -----------------------------------------------------------------------------------
// Cmd channel server

// misc.
typedef void (* webFunction) (EthernetClient *client);

class CmdServer {
  public:
    void init(int port, long t);
    void handleClient();
    int available();
    char read();
    void print(char s[]);
  private:
    EthernetClient client;

    int thisPort=0;
    long timeout=0;
    bool haveClient=false;
    unsigned long lastAccess=0;
};
