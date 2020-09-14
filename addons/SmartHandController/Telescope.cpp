#include "Telescope.h"
#include "LX200.h"

void Telescope::updateRaDec(boolean immediate)
{
  if (((millis() - lastStateRaDec > BACKGROUND_CMD_RATE) && connected) || immediate)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoRa = GetLX200(":GR#", TempRa) == LX200VALUEGET; if (!hasInfoRa) connected=true; }
    if ((updateSeq%3==2) || immediate) { hasInfoDec = GetLX200(":GD#", TempDec) == LX200VALUEGET; if (!hasInfoDec) connected=true; lastStateRaDec = millis(); }
  }
};
void Telescope::updateAzAlt(boolean immediate)
{
  if (((millis() - lastStateAzAlt > BACKGROUND_CMD_RATE) && connected) || immediate)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoAz = GetLX200(":GZ#", TempAz) == LX200VALUEGET; if (!hasInfoAz) connected = true; }
    if ((updateSeq%3==2) || immediate) { hasInfoAlt = GetLX200(":GA#", TempAlt) == LX200VALUEGET; if (!hasInfoAlt) connected = true; lastStateAzAlt = millis(); }
  }
}
void Telescope::updateTime(boolean immediate)
{
  if (((millis() - lastStateTime > BACKGROUND_CMD_RATE) && connected) || immediate)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoUTC = GetLX200(":GX80#", TempUniversalTime) == LX200VALUEGET; if (!hasInfoUTC) connected = true; }
    if ((updateSeq%3==2) || immediate) { hasInfoSidereal = GetLX200(":GS#", TempSidereal) == LX200VALUEGET; if (!hasInfoSidereal) connected = true; lastStateTime = millis(); }
  }
};
void Telescope::updateTel(boolean immediate)
{
  if (((millis() - lastStateTel > BACKGROUND_CMD_RATE) && connected) || immediate)
  {
    if ((updateSeq%3==0) || immediate) { hasTelStatus = GetLX200(":Gu#", TelStatus) == LX200VALUEGET; if (!hasTelStatus) connected = true; lastStateTel = millis(); }
  }
};

bool Telescope::getRA(double &RA)
{
  char temp[20]="";
  if (GetLX200(":GR#", temp) == LX200VALUEGET) 
  {
    int l=strlen(temp); if (l>0) temp[l-1]=0;
    double f;
    hmsToDouble(&f, temp);
    if ((f>=0) && (f<=24.0)) {
      RA=f*15.0;
      return true;
    }
  }
  return false;
};

bool Telescope::getDec(double &Dec)
{
  char temp[20]="";
  if (GetLX200(":GD#", temp) == LX200VALUEGET) 
  {
    int l=strlen(temp); if (l>0) temp[l-1]=0;
    double f;
    dmsToDouble(&f, temp, true, true);
    if ((f>=-90.0) && (f<=90.0)) {
      Dec=f;
      return true;
    }
  }
  return false;
};

double Telescope::getLstT0()
{
  char temp[20]="";
  double f=0;
  if (GetLX200(":GS#", temp) == LX200VALUEGET) 
  {
    int l=strlen(temp); if (l>0) temp[l-1]=0;
    hmsToDouble(&f, temp);
  }
  return f;
};
double Telescope::getLat()
{
  char temp[20]="";
  double f=-10000;
  if (GetLX200(":Gt#", temp) == LX200VALUEGET) {
    int l=strlen(temp); if (l>0) temp[l-1]=0;
    dmsToDouble(&f, temp, true, false);
  }
  return f;
};
int Telescope::getAlignStars(int *maxStars, int *thisStar, int *numStars)
{
  char temp[20]="";
  if (GetLX200(":A?#", temp) == LX200VALUEGET) {
    int l=strlen(temp); if (l>0) temp[l-1]=0;
    if (l!=4) return false;
    int v=temp[0]-'0'; if ((v<0) || (v>9)) return false; *maxStars=v;
    v=temp[1]-'0'; if ((v<0) || (v>9)) return false; *thisStar=v;
    v=temp[2]-'0'; if ((v<0) || (v>9)) return false; *numStars=v;
  } else return false;
  return true;
}

Telescope::ParkState Telescope::getParkState()
{
  // Park status: 0 not parked, 1 parking, 2 parked, 3 park failed
  if (strlen(TelStatus)<6) return PRK_UNKNOW;
  return (ParkState)(TelStatus[5]&0b00000011);
}
Telescope::TrackState Telescope::getTrackingState()
{
  if (strlen(TelStatus)<1) return TRK_UNKNOW;
  bool tracking=false; bool slewing=false;
  if (!(TelStatus[0]&0b00000010)) slewing=true; else tracking=(!(TelStatus[0]&0b00000001));
  if (slewing) return TRK_SLEWING; else
  if (tracking) return TRK_ON; else return TRK_OFF;
}
bool Telescope::atHome()
{
  if (strlen(TelStatus)<3) return false;
  if (TelStatus[2]&0b00000001) return true; else return false;
}
bool Telescope::isPecEnabled()
{
  // PEC status: 0 ignore, 1 get ready to play, 2 playing, 3 get ready to record, 4 recording
  if (strlen(TelStatus)<5) return false;
  if ((TelStatus[4]&0b00000111)==0) return false; else return true;
}
bool Telescope::isPecPlaying()
{
  // PEC status: 0 ignore, 1 get ready to play, 2 playing, 3 get ready to record, 4 recording
  if (strlen(TelStatus)<5) return false;
  if ((TelStatus[4]&0b00000111)==2) return true; else return false;
}
bool Telescope::isPecRecording()
{
  if (strlen(TelStatus)<5) return false;
  if ((TelStatus[4]&0b00000111)==4) return true; else return false;
}
bool Telescope::isPecWaiting()
{
  if (strlen(TelStatus)<5) return false;
  if (((TelStatus[4]&0b00000111)==1) || ((TelStatus[4]&0b00000111)==3)) return true; else return false;
}
bool Telescope::isGuiding()
{
  if (strlen(TelStatus)<1) return false;
  return (TelStatus[0]&0b00001000);
}
bool Telescope::isMountGEM()
{
  if (strlen(TelStatus)<4) return false;
  return (TelStatus[3]&0b00000001);
}
bool Telescope::isMountAltAz()
{
  if (strlen(TelStatus)<4) return false;
  return (TelStatus[3]&0b00001000);
}
Telescope::PierState Telescope::getPierState()
{
  if (strlen(TelStatus)<4) return PIER_UNKNOW;
  if (TelStatus[3]&0b00010000) return PIER_NONE; else
  if (TelStatus[3]&0b00100000) return PIER_E; else
  if (TelStatus[3]&0b01000000) return PIER_W; else return PIER_UNKNOW;
}
Telescope::TrackRate Telescope::getTrackingRate()
{
  if (strlen(TelStatus)<2) return TR_UNKNOW;
  return (TrackRate)(TelStatus[1]&0b00000011);
}
Telescope::RateCompensation Telescope::getRateCompensation()
{
  RateCompensation rateCompensation=RC_NONE;
  if (TelStatus[0]&0b00010000) { if (TelStatus[0]&0b01000000) rateCompensation=RC_REFR_RA; else rateCompensation=RC_REFR_BOTH; } else
  if (TelStatus[0]&0b00100000) { if (TelStatus[0]&0b01000000) rateCompensation=RC_FULL_RA; else rateCompensation=RC_FULL_BOTH; } else rateCompensation=RC_NONE;
  return rateCompensation;
}
int Telescope::getPulseGuideRate()
{
  if (strlen(TelStatus)<7) return -1;
  return TelStatus[6]&0b00001111;
}
int Telescope::getGuideRate()
{
  if (strlen(TelStatus)<8) return -1;
  return TelStatus[7]&0b00001111;
}
Telescope::Errors Telescope::getError()
{
  if (strlen(TelStatus)<9) return ERR_NONE;
  return (Errors)(TelStatus[8]&0b00001111);
}

bool Telescope::hasFocuser1()
{
  static int focuser1 = -1;
  if (focuser1==-1) {
    char out[20];
    if ((GetLX200(":FA#", out) == LX200VALUEGET)) focuser1=(out[0]=='1'); 
  }
  if (focuser1>0) return true; else return false;
}

bool Telescope::hasFocuser2()
{
  static int focuser2 = -1;
  if (focuser2==-1) {
    char out[20];
    if ((GetLX200(":fA#", out) == LX200VALUEGET)) focuser2=(out[0]=='1'); 
  }
  return (focuser2>0);
}

static int _rotator  = -1;
static int _derotator  = -1;
bool Telescope::hasRotator()
{
  if (_rotator==-1) {
    char out[20];
    if (GetLX200(":GX98#",out) == LX200VALUEGET) {
      _rotator=(out[0]=='R');
      _derotator=(out[0]=='D'); if (_derotator) _rotator=true;
    }
  }
  return (_rotator>0);
}

bool Telescope::hasDeRotator()
{
  if (_rotator==-1) {
    char out[20];
    if (GetLX200(":GX98#",out) == LX200VALUEGET) {
      _rotator=(out[0]=='R');
      _derotator=(out[0]=='D'); if (_derotator) _rotator=true;
    }
  }
  return (_derotator>0);
}

bool Telescope::hasReticle()
{
  return true;  
}

bool Telescope::hasDateTime()
{
  static bool dateTimeKnownValid=false;
  if (dateTimeKnownValid) return true; // once OnStep says the date/time has been set no need to keep asking
  bool dateTime=false; // default is to assume the date/time has been set unless OnStep tells us otherwise
  char out[20];
  if ((GetLX200(":GX89#", out) == LX200VALUEGET)) dateTime=(out[0]=='0');
  if (dateTime==true) dateTimeKnownValid=true;
  return dateTime;
}

bool Telescope::addStar()
{
  if (align == ALI_RECENTER_1 || align == ALI_RECENTER_2 || align == ALI_RECENTER_3 || align == ALI_RECENTER_4 || align == ALI_RECENTER_5 || align == ALI_RECENTER_6 || align == ALI_RECENTER_7 || align == ALI_RECENTER_8 || align == ALI_RECENTER_9) {
    if (SetLX200(":A+#") == LX200VALUESET) {
      if (aliMode == ALIM_ONE || 
         (aliMode == ALIM_TWO   && align == ALI_RECENTER_2) ||
         (aliMode == ALIM_THREE && align == ALI_RECENTER_3) ||
         (aliMode == ALIM_FOUR  && align == ALI_RECENTER_4) ||
         (aliMode == ALIM_FIVE  && align == ALI_RECENTER_5) ||
         (aliMode == ALIM_SIX   && align == ALI_RECENTER_6) ||
         (aliMode == ALIM_SEVEN && align == ALI_RECENTER_7) ||
         (aliMode == ALIM_EIGHT && align == ALI_RECENTER_8) ||
         (aliMode == ALIM_NINE  && align == ALI_RECENTER_9)) { align = ALI_OFF; return true; } else { align = static_cast<AlignState>(align+1); return true; }
    } else {
      align = ALI_OFF;
      return false;
    }
  } else {
    align = ALI_OFF;
    return false;
  }
}

bool Telescope::getT(double &T)
{
  char temp[20]="";
  static double f=0;
  static bool hasValue=false;
  static unsigned long last=0;
  if ((millis()-last>30000) || (!hasValue))
    if (GetLX200(":GX9A#", temp) == LX200VALUEGET) 
    {
      int l=strlen(temp); if (l>0) temp[l-1]=0;
      f=atof(temp);
      if ((f>=-50.0) && (f<=100.0)) { last=millis(); hasValue=true; }
    }
  if (hasValue) { T=f; return true; } else return false;
};

bool Telescope::getP(double &P)
{
  char temp[20]="";
  static double f=0;
  static bool hasValue=false;
  static unsigned long last=0;
  if ((millis()-last>75000) || (!hasValue))
    if (GetLX200(":GX9B#", temp) == LX200VALUEGET) 
    {
      int l=strlen(temp); if (l>0) temp[l-1]=0;
      f=atof(temp);
      if ((f>=500.0) && (f<=2000.0)) { last=millis(); hasValue=true; }
    }
  if (hasValue) { P=f; return true; } else return false;
};

bool Telescope::getH(double &H)
{
  char temp[20]="";
  static double f=0;
  static bool hasValue=false;
  static unsigned long last=0;
  if ((millis()-last>75000) || (!hasValue))
    if (GetLX200(":GX9C#", temp) == LX200VALUEGET) 
    {
      int l=strlen(temp); if (l>0) temp[l-1]=0;
      f=atof(temp);
      if ((f>=0.0) && (f<=100.0)) { last=millis(); hasValue=true; }
    }
  if (hasValue) { H=f; return true; } else return false;
};

bool Telescope::getDP(double &DP)
{
  char temp[20]="";
  static double f=0;
  static bool hasValue=false;
  static unsigned long last=0;
  if ((millis()-last>30000) || (!hasValue))
    if (GetLX200(":GX9E#", temp) == LX200VALUEGET) 
    {
      int l=strlen(temp); if (l>0) temp[l-1]=0;
      f=atof(temp);
      if ((f>=-50.0) && (f<=100.0)) { last=millis(); hasValue=true; }
    }
  if (hasValue) { DP=f; return true; } else return false;
};
