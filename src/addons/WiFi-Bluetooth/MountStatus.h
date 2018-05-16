#pragma once

enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum MountTypes {MT_UNKNOWN, MT_GEM, MT_FORK, MT_FORKALT, MT_ALTAZM};
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC};
#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22

class MountStatus {
  public:
    bool update(bool all=false) {
      char s[20] = "";
      Ser.print(":GU#");
      s[Ser.readBytesUntil('#',s,20)]=0;
      if (s[0]==0) { valid=false; return false; }

      tracking=false; slewing=false;
      if (!strstr(s,"N")) slewing=true; else tracking=(!strstr(s,"n"));

      parked      = strstr(s,"P");
      if (strstr(s,"p")) parked=false;
      parking     = strstr(s,"I");
      parkFail    = strstr(s,"F");
      pecRecorded = strstr(s,"R");
      pecRecording= strstr(s,"W");
    
      atHome      = strstr(s,"H");
      ppsSync     = strstr(s,"S");
      guiding     = strstr(s,"G");
      axisFault   = strstr(s,"f");
      
      if (strstr(s,"r")) { if (strstr(s,"s")) rateCompensation=RC_REFR_RA; else rateCompensation=RC_REFR_BOTH; } else
      if (strstr(s,"t")) { if (strstr(s,"s")) rateCompensation=RC_FULL_RA; else rateCompensation=RC_FULL_BOTH; } else rateCompensation=RC_NONE;

      waitingHome   = strstr(s,"w");
      pauseAtHome   = strstr(s,"u");
      buzzerEnabled = strstr(s,"z");

      if (strstr(s,"E")) mountType=MT_GEM; else
      if (strstr(s,"K")) mountType=MT_FORK; else
      if (strstr(s,"k")) mountType=MT_FORKALT; else
      if (strstr(s,"A")) mountType=MT_ALTAZM; else mountType=MT_UNKNOWN;

      lastError=(Errors)(s[strlen(s)-1]-'0');

      if (all) {
        Ser.print(":GX94#"); s[Ser.readBytesUntil('#',s,20)]=0; if (s[0]==0) { valid=false; return false; }
        meridianFlips=!strstr(s, "N");
        pierSide=strtol(&s[0],NULL,10);

        if ((mountType==MT_GEM) || (mountType==MT_FORK)) {
          Ser.print(":GX95#"); s[Ser.readBytesUntil('#',s,20)]=0; if (s[0]==0) { valid=false; return false; }
          autoMeridianFlips=(!strstr(s,"0"));
        } else autoMeridianFlips=false;

        if (alignMaxStars==-1) {
          Ser.print(":A?#");
          s[Ser.readBytesUntil('#',s,20)]=0;
          alignMaxStars=3;
          if (s[0]!=0) { if ((s[0]>'0') && (s[0]<='9')) alignMaxStars=s[0]-'0'; }
        }
      }
      
      valid=true;
      return true;
    }
    bool getLastErrorMessage(char message[]) {
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
    bool valid=false;

    bool tracking=false;
    bool slewing=false;
    bool parked=false;
    bool parking=false;
    bool parkFail=false;
    bool pecRecorded=false;
    bool pecRecording=false;
    bool atHome=false;
    bool ppsSync=false;
    bool guiding=false;
    bool axisFault=false;
    bool waitingHome=false;
    bool pauseAtHome=false;
    bool buzzerEnabled=false;
    MountTypes mountType=MT_UNKNOWN;
    RateCompensation rateCompensation=RC_NONE;
    
    bool meridianFlips=true;
    bool autoMeridianFlips=false;
    byte pierSide=PierSideNone;

    int alignMaxStars = -1;

    int lastError=0;
  private:
};

MountStatus mountStatus;
