#pragma once

enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum MountTypes {MT_UNKNOWN, MT_GEM, MT_FORK, MT_FORKALT, MT_ALTAZM};
enum Errors {ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC};
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
      if (!_valid) {
        Ser.print(":GVP#");
        s[Ser.readBytesUntil('#',s,20)]=0;
        if ((s[0]==0) || (!strstr(s,"On-Step"))) { _valid=false; return false; }

        Ser.print(":GVN#");
        s[Ser.readBytesUntil('#',s,20)]=0;
        if (s[0]==0) { _valid=false; return false; }
        strcpy(_id,"OnStep");
        strcpy(_ver,s);
      }

      Ser.print(":GU#");
      s[Ser.readBytesUntil('#',s,20)]=0;
      if (s[0]==0) { _valid=false; return false; }

      _tracking=false; _slewing=false;
      if (!strstr(s,"N")) _slewing=true; else _tracking=(!strstr(s,"n"));

      _parked      = strstr(s,"P");
      if (strstr(s,"p")) _parked=false;
      _parking     = strstr(s,"I");
      _parkFail    = strstr(s,"F");

      _pecRecorded = strstr(s,"R");
      _pecIgnore   = strstr(s,"/");
      _pecReadyPlay= strstr(s,",");
      _pecPlaying  = strstr(s,"~");
      _pecReadyRec = strstr(s,";");
      _pecRecording= strstr(s,"^");
    
      _atHome      = strstr(s,"H");
      _ppsSync     = strstr(s,"S");
      _guiding     = strstr(s,"G");
      _axisFault   = strstr(s,"f");
      
      if (strstr(s,"r")) { if (strstr(s,"s")) _rateCompensation=RC_REFR_RA; else _rateCompensation=RC_REFR_BOTH; } else
      if (strstr(s,"t")) { if (strstr(s,"s")) _rateCompensation=RC_FULL_RA; else _rateCompensation=RC_FULL_BOTH; } else _rateCompensation=RC_NONE;

      _waitingHome   = strstr(s,"w");
      _pauseAtHome   = strstr(s,"u");
      _buzzerEnabled = strstr(s,"z");

      if (strstr(s,"E")) _mountType=MT_GEM; else
      if (strstr(s,"K")) _mountType=MT_FORK; else
      if (strstr(s,"k")) _mountType=MT_FORKALT; else
      if (strstr(s,"A")) _mountType=MT_ALTAZM; else _mountType=MT_UNKNOWN;

      if (_mountType==MT_GEM) _autoMeridianFlips = strstr(s,"a"); else _autoMeridianFlips=false;

      _lastError=(Errors)(s[strlen(s)-1]-'0');

      if (all) {
        Ser.print(":GX94#"); s[Ser.readBytesUntil('#',s,20)]=0; if (s[0]==0) { _valid=false; return false; }
        _meridianFlips=!strstr(s, "N");
        _pierSide=strtol(&s[0],NULL,10);

        _validStepperDriverStatus = false;
        _stst1 = false; _olb1 = false; _ola1 = false; _s2ga1 = false; _s2gb1 = false; _ot1 = false; _otpw1 = false;
        _stst2 = false; _olb2 = false; _ola2 = false; _s2ga2 = false; _s2gb2 = false; _ot2 = false; _otpw2 = false;
        Ser.print(":GXU1#"); s[Ser.readBytesUntil('#',s,20)]=0;
        if ((s[0]!=0) && (s[0]!='0')) {
          if (strstr(s,"ST")) _stst1=true;
          if (strstr(s,"OA")) _ola1=true;
          if (strstr(s,"OB")) _olb1=true;
          if (strstr(s,"GA")) _s2ga1=true;
          if (strstr(s,"GB")) _s2gb1=true;
          if (strstr(s,"OT")) _ot1=true;
          if (strstr(s,"PW")) _otpw1=true;
          Ser.print(":GXU2#"); s[Ser.readBytesUntil('#',s,20)]=0;
          if ((s[0]!=0) && (s[0]!='0')) {
            _validStepperDriverStatus = true;
            if (strstr(s,"ST")) _stst2=true;
            if (strstr(s,"OA")) _ola2=true;
            if (strstr(s,"OB")) _olb2=true;
            if (strstr(s,"GA")) _s2ga2=true;
            if (strstr(s,"GB")) _s2gb2=true;
            if (strstr(s,"OT")) _ot2=true;
            if (strstr(s,"PW")) _otpw2=true;
          }
        }

        if (_alignMaxStars==-1) {
          Ser.print(":A?#");
          s[Ser.readBytesUntil('#',s,20)]=0;
          _alignMaxStars=3;
          if (s[0]!=0) { if ((s[0]>'0') && (s[0]<='9')) _alignMaxStars=s[0]-'0'; }
        }
      }
      
      _valid=true;
      return true;
    }
    bool getId(char id[]) { if (!_valid) return false; else { strcpy(id,_id); return true; } }
    bool getVer(char ver[]) { if (!_valid) return false; else { strcpy(ver,_ver); return true; } }
    bool valid() { return _valid; }
    bool tracking() { return _tracking; }
    bool slewing() { return _slewing; }
    bool parked() { return _parked; }
    bool parking() { return _parking; }
    bool parkFail() { return _parkFail; }
    bool pecIgnore() { return _pecIgnore; }
    bool pecReadyPlay() { return _pecReadyPlay; }
    bool pecPlaying() { return _pecPlaying; }
    bool pecReadyRec() { return _pecReadyRec; }
    bool pecRecorded() { return _pecRecorded; }
    bool pecRecording() { return _pecRecording; }
    bool atHome() { return _atHome; }
    bool ppsSync() { return _ppsSync; }
    bool guiding() { return _guiding; }

    bool axisFault() { return _axisFault; }
    bool axisStatusValid() { return _validStepperDriverStatus; }
    bool axis1StSt() { return _stst1; }
    bool axis1OLa() { return _ola1; }
    bool axis1OLb() { return _olb1; }
    bool axis1S2Ga() { return _s2ga1; }
    bool axis1S2Gb() { return _s2gb1; }
    bool axis1OT() { return _ot1; }
    bool axis1OTPW() { return _otpw1; }
    bool axis2StSt() { return _stst2; }
    bool axis2OLa() { return _ola2; }
    bool axis2OLb() { return _olb2; }
    bool axis2S2Ga() { return _s2ga2; }
    bool axis2S2Gb() { return _s2gb2; }
    bool axis2OT() { return _ot2; }
    bool axis2OTPW() { return _otpw2; }

    bool waitingHome() { return _waitingHome; }
    bool pauseAtHome() { return _pauseAtHome; }
    bool buzzerEnabled() { return _buzzerEnabled; }
    MountTypes mountType() { return _mountType; }
    RateCompensation rateCompensation() { return _rateCompensation; }
    bool meridianFlips() { return _meridianFlips; }
    bool autoMeridianFlips() { return _autoMeridianFlips; }
    byte pierSide() { return _pierSide; }
    int alignMaxStars() { return _alignMaxStars; }
    Errors lastError() { return _lastError; }
    bool getLastErrorMessage(char message[]) {
      strcpy(message,"");
      if (_lastError==ERR_NONE) strcpy(message,"None"); else
      if (_lastError==ERR_MOTOR_FAULT) strcpy(message,"Motor or Driver Fault"); else
      if (_lastError==ERR_ALT) strcpy(message,"Altitude Min/Max"); else
      if (_lastError==ERR_LIMIT_SENSE) strcpy(message,"Limit Sense"); else
      if (_lastError==ERR_DEC) strcpy(message,"Dec Limit Exceeded"); else
      if (_lastError==ERR_AZM) strcpy(message,"Azm Limit Exceeded"); else
      if (_lastError==ERR_UNDER_POLE) strcpy(message,"Under Pole Limit Exceeded"); else
      if (_lastError==ERR_MERIDIAN) strcpy(message,"Meridian Limit (W) Exceeded"); else
      if (_lastError==ERR_SYNC) strcpy(message,"Sync. ignored >30&deg;"); else
      if (_lastError==ERR_PARK) strcpy(message,"Park Failed"); else
      if (_lastError==ERR_GOTO_SYNC) strcpy(message,"Goto Sync. Failed"); else
        sprintf(message,"Unknown Error, code %d",(int)_lastError);
      return message[0];
    }
  private:
    char _id[10]="";
    char _ver[10]="";
    bool _valid=false;
    bool _tracking=false;
    bool _slewing=false;
    bool _parked=false;
    bool _parking=false;
    bool _parkFail=false;
    bool _pecIgnore=false;
    bool _pecReadyPlay=false;
    bool _pecPlaying=false;
    bool _pecReadyRec=false;
    bool _pecRecorded=false;
    bool _pecRecording=false;
    bool _atHome=false;
    bool _ppsSync=false;
    bool _guiding=false;
    bool _axisFault=false;
    bool _waitingHome=false;
    bool _pauseAtHome=false;
    bool _buzzerEnabled=false;
    MountTypes _mountType=MT_UNKNOWN;
    RateCompensation _rateCompensation=RC_NONE;
    bool _meridianFlips=true;
    bool _autoMeridianFlips=false;
    byte _pierSide=PierSideNone;
    int _alignMaxStars = -1;

    Errors _lastError=ERR_NONE;
    bool _validStepperDriverStatus = false;
    bool _stst1 = false;
    bool _olb1 = false;
    bool _ola1 = false;
    bool _s2ga1 = false;
    bool _s2gb1 = false;
    bool _ot1 = false;
    bool _otpw1 = false;
    bool _valid2 = false;
    bool _stst2 = false;
    bool _olb2 = false;
    bool _ola2 = false;
    bool _s2ga2 = false;
    bool _s2gb2 = false;
    bool _ot2 = false;
    bool _otpw2 = false;

};

MountStatus mountStatus;
