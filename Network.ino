#if defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__) || defined(W5100_ON)

// provice the same functions as for serial

// Enter a MAC address and IP address for your controller below. MAC address will be on the sticker on the Tiva Connected Launchpad.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 55);
IPAddress myDns(192,168,1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


// Network server port
EthernetServer server(9999);
EthernetClient client;
boolean alreadyConnected = false; // whether or not the client was connected previously


void Ethernet_Init() {
  // initialize the ethernet device
#if defined(ETHERNET_USE_DHCP)
  Ethernet.begin(mac);
#else
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
#endif

  server.begin();
}

void Ethernet_send(const char data[]) {
  if (!client)
   return;
   
  client.flush();
   
  client.print(data);
  do {} while (Serial_transmit());
}

void Ethernet_print(const char data[]) {
  if (!client)
   return;
   
   server.print(data);
 }
 
 boolean Ethernet_transmit() {
   return false;
 }
 
 boolean Ethernet_available() {
   client = server.available();
   return client;
 }
 
 char Ethernet_read() {
   if (!client)
   return -1;
   
   return client.read();
 }
#endif
