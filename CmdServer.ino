// -----------------------------------------------------------------------------------
// Cmd channel server

#ifdef ETHERNET_ON
EthernetServer cmdserver(9999);

// t is the timeout in ms, 0 for never
void CmdServer::init(long t) {
  cmdserver.begin();
  timeout=t;
#ifdef CMDSERVER_DEBUG_ON
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
#endif
}

void CmdServer::handleClient() {
  if (!haveClient) {
    // new client connect
    client = cmdserver.available();
    if (client) {
      lastAccess=millis();
      haveClient = true;
#ifdef CMDSERVER_DEBUG_ON
      Serial.println("client connected");
#endif
    }
  } else {
    // client disconnect
    if (!client.connected() || ((timeout!=0) && (millis()-lastAccess>timeout))) {
      client.stop();
      client = EthernetClient();
      haveClient = false;
#ifdef CMDSERVER_DEBUG_ON
      Serial.print("client disconnected");
      if (millis()-lastAccess>timeout) Serial.println(" (timed out)"); else Serial.println("");
#endif
    }
  }
}

int CmdServer::available() {
  if (!haveClient) return 0;
  if (!client.connected()) return 0;
  return client.available();
}

char CmdServer::read() {
  lastAccess=millis();
  return client.read();
}

void CmdServer::print(char s[]) {
  lastAccess=millis();
  client.print(s);
}
#endif
