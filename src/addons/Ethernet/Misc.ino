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
  unsigned long startTime=millis()+timeout;
  int pos=0;
  *characterFound=false;
  while (((long)(startTime-millis())>0) && (pos<length)) {
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
  if (command[0]==':') {
    if (command[1]=='A') {
      if (strchr("W123456789+",command[2])) { shortResponse=true; Ser.setTimeout(timeOutMs*4); }
    }
    if ((command[1]=='F') || (command[1]=='f')) {
      if (strchr("+-QGZHFS1234",command[2])) noResponse=true;
      if (strchr("A",command[2])) shortResponse=true;
    }
    if (command[1]=='r') {
      if (strchr("+-PRFC<>Q1234",command[2])) noResponse=true;
      if (strchr("~S",command[2])) shortResponse=true;
    }
    if (command[1]=='M') {
      if (strchr("ewnsg",command[2])) noResponse=true;
      if (strchr("SA",command[2])) shortResponse=true;
    }
    if (command[1]=='Q') {
      if (strchr("#ewns",command[2])) noResponse=true;
    }
    if (command[1]=='S') {
      if (strchr("CLSGtgMNOPrdhoTBX",command[2])) shortResponse=true;
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
      if (strchr("QPR",command[2])) { shortResponse=true; Ser.setTimeout(timeOutMs*2); }
    }
    if (command[1]=='T') {
      if (strchr("QR+-SLK",command[2])) noResponse=true;
      if (strchr("edrn",command[2])) shortResponse=true;
    }
    if (command[1]=='U') noResponse=true;
    if ((command[1]=='W') && (command[2]!='?')) { noResponse=true; }
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


class MountStatus {
  public:
    bool update() {
      char s[20] = "";
      Ser.print(":GU#");
      s[Ser.readBytesUntil('#',s,20)]=0;
      if (s[0]==0) return false;

      parked      = strstr(s,"P");
      if (strstr(s,"p")) parked=false;
      parking     = strstr(s,"I");
      parkFail    = strstr(s,"F");
      pecRecorded = strstr(s,"R");
    
      atHome      = strstr(s,"H");
      ppsSync     = strstr(s,"S");
      guiding     = strstr(s,"G");
      axisFault   = strstr(s,"f");
      
      if (strstr(s,"r")) { if (strstr(s,"s")) rateCompensation=RC_REFR_RA; else rateCompensation=RC_REFR_BOTH; } else
      if (strstr(s,"t")) { if (strstr(s,"s")) rateCompensation=RC_FULL_RA; else rateCompensation=RC_FULL_BOTH; } else rateCompensation=RC_NONE;

      waitingHome   = strstr(s,"w");
      pauseAtHome   = strstr(s,"u");
      buzzerEnabled = strstr(s,"z");

      if (strstr(s,"E")) type=MT_GEM; else
      if (strstr(s,"K")) type=MT_FORK; else
      if (strstr(s,"k")) type=MT_FORKALT; else
      if (strstr(s,"A")) type=MT_ALTAZM; else type=MT_UNKNOWN;

      lastError=(Errors)(s[strlen(s)-1]-'0');

      return true;
    }
    bool lastErrorMessage(char message[]) {
      strcpy(message,"None");
      if (lastError==ERR_MOTOR_FAULT) strcpy(message,"Motor or Driver Fault"); else
      if (lastError==ERR_ALT) strcpy(message,"Altitude Min/Max"); else
      if (lastError==ERR_LIMIT_SENSE) strcpy(message,"Limit Sense"); else
      if (lastError==ERR_DEC) strcpy(message,"Dec Limit Exceeded"); else
      if (lastError==ERR_AZM) strcpy(message,"Azm Limit Exceeded"); else
      if (lastError==ERR_UNDER_POLE) strcpy(message,"Under Pole Limit Exceeded"); else
      if (lastError==ERR_MERIDIAN) strcpy(message,"Meridian Limit (W) Exceeded"); else
      if (lastError==ERR_SYNC) strcpy(message,"Sync. ignored >30&deg;");
      return message;
    }
    bool parked=false;
    bool parking=false;
    bool parkFail=false;
    bool pecRecorded=false;
    bool atHome=false;
    bool ppsSync=false;
    bool guiding=false;
    bool axisFault=false;
    bool waitingHome=false;
    bool pauseAtHome=false;
    bool buzzerEnabled=false;
    MountTypes type=MT_UNKNOWN;
    RateCompensation rateCompensation=RC_NONE;
  private:
    int lastError=0;
} mountStatus;

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

