
// integer numeric conversion with error checking
boolean atoi2(char *a, int *i) {
  char *conv_end;
  long l=strtol(a,&conv_end,10);
  
  if ((l<-32767) || (l>32768) || (&a[0]==conv_end)) return false;
  *i=l;
  return true;
}

// this readBytesUntil() lets you know if the "character" was found
byte readBytesUntil2(char character, char buffer[], int length, boolean* characterFound, long timeout) {
  unsigned long startTime=millis()+timeout;
  int pos=0;
  *characterFound=false;
  while (((long)(startTime-millis())>0) && (pos<length)) {
    if (Serial.available()) {
      buffer[pos]=Serial.read();
      if (buffer[pos]==character) { *characterFound=true; break; }
      pos++;
    }
  }
  return pos;
}

// smart LX200 aware command and response over serial
boolean readLX200Bytes(char* command,char* recvBuffer,long timeOutMs) {
  Serial.setTimeout(timeOutMs);
  
  // clear the read/write buffers
  Serial.flush();
  serialRecvFlush();

  // send the command
  Serial.print(command);

  boolean noResponse=false;
  boolean shortResponse=false;
  if ((command[0]==(char)6) && (command[1]==0)) shortResponse=true;
  if (command[0]==':') {
    if (command[1]=='A') {
      if (strchr("W123456789+",command[2])) { shortResponse=true; Serial.setTimeout(timeOutMs*4); }
    }
    if (command[1]=='M') {
      if (strchr("ewnsg",command[2])) noResponse=true;
      if (strchr("SA",command[2])) shortResponse=true;
    }
    if (command[1]=='Q') {
      if (strchr("#ewns",command[2])) noResponse=true;
    }
    if (command[1]=='S') {
      if (strchr("CLSGtgMNOPrdhoTB",command[2])) shortResponse=true;
    }
    if (command[1]=='L') {
      if (strchr("BNCDL!",command[2])) noResponse=true;
      if (strchr("o$W",command[2])) shortResponse=true;
    }
    if (command[1]=='B') {
      if (strchr("+-",command[2])) noResponse=true;
    }
    if (command[1]=='C') {
      if (strchr("S",command[2])) noResponse=true;
    }
    if (command[1]=='h') {
      if (strchr("FC",command[2])) noResponse=true;
      if (strchr("QPR",command[2])) { shortResponse=true; Serial.setTimeout(timeOutMs*2); }
    }
    if (command[1]=='T') {
      if (strchr("QR+-SLK",command[2])) noResponse=true;
      if (strchr("edrn",command[2])) shortResponse=true;
    }
    if (command[1]=='U') noResponse=true;
    if ((command[1]=='W') && (command[2]!='?')) { noResponse=true; }
    if ((command[1]=='W') && (command[2]=='?')) { shortResponse=true; }
    if ((command[1]=='$') && (command[2]=='Q') && (command[3]=='Z')) {
      if (strchr("+-Z/!",command[4])) noResponse=true;
    }
    if (command[1]=='G') {
      if (strchr("RD",command[2])) { timeOutMs*=2; }
    }
  }

  if (noResponse) {
    recvBuffer[0]=0;
    return true;
  } else
  if (shortResponse) {
    recvBuffer[Serial.readBytes(recvBuffer,1)]=0;
    return (recvBuffer[0]!=0);
  } else {
    // get full response, '#' terminated
    unsigned long timeout=millis()+(unsigned long)timeOutMs;
    int recvBufferPos=0;
    char b=0;
    while (((long)(timeout-millis())>0) && (b!='#')) {
      if (Serial.available()) {
        b=Serial.read();
        recvBuffer[recvBufferPos]=b; recvBufferPos++; if (recvBufferPos>39) recvBufferPos=39; recvBuffer[recvBufferPos]=0;
      }
    }
    return (recvBuffer[0]!=0);
  }
}

char serialRecvFlush() {
  char c=0;
  while (Serial.available()>0) c=Serial.read();
  return c;
}

