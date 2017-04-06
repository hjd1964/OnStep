// -----------------------------------------------------------------------------------
// Cmd channel server

#ifdef ETHERNET_ON

// if above is used, enter a MAC address and IP address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// the IP address will be dependent on your local network. Gateway and subnet are optional:
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// misc.
typedef void (* webFunction) (EthernetClient *client);

class CmdServer {
  public:
    void init(long t);
    void handleClient();
    int available();
    char read();
    void print(char s[]);
  private:
    bool haveClient=false;
    EthernetClient client;
    unsigned long timeout=0;
    unsigned long lastAccess=0;
};
#endif
