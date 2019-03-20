#include "Telescope.h"
#include "LX200.h"

void Telescope::updateRaDec(boolean immediate)
{
  if ((millis() - lastStateRaDec > BACKGROUND_CMD_RATE) && ((updateSeq%3==1) || (updateSeq%3==2) || immediate) && connected)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoRa = GetLX200(":GR#", TempRa) == LX200VALUEGET; if (!hasInfoRa) connected=true; }
    if ((updateSeq%3==2) || immediate) { hasInfoDec = GetLX200(":GD#", TempDec) == LX200VALUEGET; if (!hasInfoDec) connected=true; lastStateRaDec = millis(); }
  }
};
void Telescope::updateAzAlt(boolean immediate)
{
  if ((millis() - lastStateAzAlt > BACKGROUND_CMD_RATE) && ((updateSeq%3==1) || (updateSeq%3==2) || immediate) && connected)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoAz = GetLX200(":GZ#", TempAz) == LX200VALUEGET; if (!hasInfoAz) connected = true; }
    if ((updateSeq%3==2) || immediate) { hasInfoAlt = GetLX200(":GA#", TempAlt) == LX200VALUEGET; if (!hasInfoAlt) connected = true; lastStateAzAlt = millis(); }
  }
}
void Telescope::updateTime(boolean immediate)
{
  if ((millis() - lastStateTime > BACKGROUND_CMD_RATE) && ((updateSeq%3==1) || (updateSeq%3==2) || immediate) && connected)
  {
    if ((updateSeq%3==1) || immediate) { hasInfoUTC = GetLX200(":GL#", TempLocalTime) == LX200VALUEGET; if (!hasInfoUTC) connected = true; }
    if ((updateSeq%3==2) || immediate) { hasInfoSidereal = GetLX200(":GS#", TempSidereal) == LX200VALUEGET; if (!hasInfoSidereal) connected = true; lastStateTime = millis(); }
  }
};
void Telescope::updateTel(boolean immediate)
{
  if ((millis() - lastStateTel > BACKGROUND_CMD_RATE) && ((updateSeq%3==0) || immediate) && connected)
  {
    if ((updateSeq%3==0) || immediate) { hasTelStatus = GetLX200(":Gu#", TelStatus) == LX200VALUEGET; if (!hasTelStatus) connected = true; lastStateTel = millis(); }
  }
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
