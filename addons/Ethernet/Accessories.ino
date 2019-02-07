// -----------------------------------------------------------------------------------
// Misc functions to help with commands, etc.

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
  unsigned long endTime=millis()+timeout;
  int pos=0;
  *characterFound=false;
  while (((long)(endTime-millis())>0) && (pos<length)) {
    if (Ser.available()) {
      buffer[pos]=Ser.read();
      if (buffer[pos]==character) { *characterFound=true; break; }
      pos++;
    }
  }
  return pos;
}

// smart LX200 aware command and response over serial
boolean readLX200Bytes(char* command,char* recvBuffer,long timeOutMs) {
  Ser.setTimeout(timeOutMs);
  
  // clear the read/write buffers
  Ser.flush();
  serialRecvFlush();

  // send the command
  Ser.print(command);

  boolean noResponse=false;
  boolean shortResponse=false;
  if ((command[0]==(char)6) && (command[1]==0)) shortResponse=true;
  if ((command[0]==':') || (command[0]==';')) {
    if (command[1]=='G') {
      if (strchr("RD",command[2])) { timeOutMs*=2; }
    } else
    if (command[1]=='M') {
      if (strchr("ewnsg",command[2])) noResponse=true;
      if (strchr("SA",command[2])) shortResponse=true;
    } else
    if (command[1]=='Q') {
      if (strchr("#ewns",command[2])) noResponse=true;
    } else
    if (command[1]=='A') {
      if (strchr("W123456789+",command[2])) { shortResponse=true; timeOutMs=1000; }
    } else
    if ((command[1]=='F') || (command[1]=='f')) {
      if (strchr("+-QZHFS1234",command[2])) noResponse=true;
      if (strchr("A",command[2])) shortResponse=true;
    } else
    if (command[1]=='r') {
      if (strchr("+-PRFC<>Q1234",command[2])) noResponse=true;
      if (strchr("~S",command[2])) shortResponse=true;
    } else
    if (command[1] == 'R') {
      if (strchr("AEGCMS0123456789", command[2])) noResponse=true;
    } else
    if (command[1]=='S') {
      if (strchr("CLSGtgMNOPrdhoTBX",command[2])) shortResponse=true;
    } else
    if (command[1]=='L') {
      if (strchr("BNCDL!",command[2])) noResponse=true;
      if (strchr("o$W",command[2])) shortResponse=true;
    } else
    if (command[1]=='B') {
      if (strchr("+-",command[2])) noResponse=true;
    } else
    if (command[1]=='C') {
      if (strchr("S",command[2])) noResponse=true;
    } else
    if (command[1]=='h') {
      if (strchr("FC",command[2])) { noResponse=true; timeOutMs=1000; }
      if (strchr("QPR",command[2])) { shortResponse=true; timeOutMs*=2; }
    } else
    if (command[1]=='T') {
      if (strchr("QR+-SLK",command[2])) noResponse=true;
      if (strchr("edrn",command[2])) shortResponse=true;
    } else
    if (command[1]=='U') {
      noResponse=true; 
    } else
    if ((command[1]=='W') && (command[2]!='?')) { 
      noResponse=true; 
    } else
    if ((command[1]=='$') && (command[2]=='Q') && (command[3]=='Z')) {
      if (strchr("+-Z/!",command[4])) noResponse=true;
    }

    // override for checksum protocol
    if (command[0]==';') { noResponse=false; shortResponse=false; }
  }

  if (noResponse) {
    recvBuffer[0]=0;
    return true;
  } else
  if (shortResponse) {
    Ser.setTimeout(timeOutMs);
    recvBuffer[Ser.readBytes(recvBuffer,1)]=0;
    return (recvBuffer[0]!=0);
  } else {
    // get full response, '#' terminated
    unsigned long timeout=millis()+(unsigned long)timeOutMs;
    int recvBufferPos=0;
    char b=0;
    while (((long)(timeout-millis())>0) && (b!='#')) {
      if (Ser.available()) {
        b=Ser.read();
        recvBuffer[recvBufferPos]=b; recvBufferPos++; if (recvBufferPos>39) recvBufferPos=39; recvBuffer[recvBufferPos]=0;
      }
    }
    return (recvBuffer[0]!=0);
  }
}

// sends LX200 command and optionally waits for response (w/timeout, up to 20 chars)
enum Responding {R_NONE, R_ONE, R_BOOL, R_STRING};
bool sendCommand(const char command[], char response[], Responding responding=R_STRING) {
  Ser.print(command);
  strcpy(response,"");
  if (responding==R_NONE) return true;
  if (responding==R_ONE) response[Ser.readBytes(response,1)]=0;
  if (responding==R_BOOL) { response[Ser.readBytes(response,1)]=0; if (strlen(response)>0) { if (response[0]=='0') return false; else return true; } }
  if (responding==R_STRING) { boolean found=true; response[readBytesUntil2('#',response,20,&found,WebTimeout)]=0; if (!found) return false; }
  return response[0];
}

char serialRecvFlush() {
  char c=0;
  while (Ser.available()>0) c=Ser.read();
  return c;
}

boolean doubleToDms(char *reply, double *f, boolean fullRange, boolean signPresent) {
  char sign[]="+";
  int  o=0,d1,s1=0;
  double m1,f1;
  f1=*f;

  // setup formatting, handle adding the sign
  if (f1<0) { f1=-f1; sign[0]='-'; }

  f1=f1+0.000139; // round to 1/2 arc-second
  d1=floor(f1);
  m1=(f1-d1)*60.0;
  s1=(m1-floor(m1))*60.0;
  
  char s[]="+%02d*%02d:%02d";
  if (signPresent) { 
    if (sign[0]=='-') { s[0]='-'; } o=1;
  } else {
    strcpy(s,"%02d*%02d:%02d");
  }
  if (fullRange) s[2+o]='3';
 
  sprintf(reply,s,d1,(int)m1,s1);
  return true;
}
