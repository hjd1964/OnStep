#include "Telescope.h"
#include "LX200.h"

void Telescope::updateRaDec(boolean immediate)
{
  if ((millis() - lastStateRaDec > BACKGROUND_CMD_RATE) && ((updateSeq%4==2) || (updateSeq%4==3) || immediate) && connected)
  {
    if ((updateSeq%4==2) || immediate) { hasInfoRa = GetLX200(":GR#", TempRa) == LX200VALUEGET; if (!hasInfoRa) connected=true; }
    if ((updateSeq%4==3) || immediate) { hasInfoDec = GetLX200(":GD#", TempDec) == LX200VALUEGET; if (!hasInfoDec) connected=true; lastStateRaDec = millis(); }
  }
};
void Telescope::updateAzAlt(boolean immediate)
{
  if ((millis() - lastStateAzAlt > BACKGROUND_CMD_RATE) && ((updateSeq%4==2) || (updateSeq%4==3) || immediate) && connected)
  {
    if ((updateSeq%4==2) || immediate) { hasInfoAz = GetLX200(":GZ#", TempAz) == LX200VALUEGET; if (!hasInfoAz) connected = true; }
    if ((updateSeq%4==3) || immediate) { hasInfoAlt = GetLX200(":GA#", TempAlt) == LX200VALUEGET; if (!hasInfoAlt) connected = true; lastStateAzAlt = millis(); }
  }
}
void Telescope::updateTime(boolean immediate)
{
  if ((millis() - lastStateTime > BACKGROUND_CMD_RATE) && ((updateSeq%4==2) || (updateSeq%4==3) || immediate) && connected)
  {
    if ((updateSeq%4==2) || immediate) { hasInfoUTC = GetLX200(":GL#", TempLocalTime) == LX200VALUEGET; if (!hasInfoUTC) connected = true; }
    if ((updateSeq%4==3) || immediate) { hasInfoSidereal = GetLX200(":GS#", TempSidereal) == LX200VALUEGET; if (!hasInfoSidereal) connected = true; lastStateTime = millis(); }
  }
};
void Telescope::updateTel(boolean immediate)
{
  if ((millis() - lastStateTel > BACKGROUND_CMD_RATE) && ((updateSeq%4==0) || (updateSeq%4==1) || immediate) && connected)
  {
    if ((updateSeq%4==0) || immediate) { hasPierInfo = GetLX200(":Gm#", sideofpier) == LX200VALUEGET; if (!hasPierInfo) connected = true; }
    if ((updateSeq%4==1) || immediate) { hasTelStatus = GetLX200(":GU#", TelStatus) == LX200VALUEGET; if (!hasTelStatus) connected = true; lastStateTel = millis(); }
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
    f=((unsigned long)(f*3600000.0)-millis());
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
  if (strchr(&TelStatus[0], 'P') != NULL)
  {
    return PRK_PARKED;
  }
  else if (strchr(&TelStatus[0], 'p') != NULL)
  {
    return PRK_UNPARKED;
  }
  else if (strchr(&TelStatus[0], 'I') != NULL)
  {
    return PRK_PARKING;
  }
  else if (strchr(&TelStatus[0], 'F') != NULL)
  {
    return PRK_FAILED;
  }
  return PRK_UNKNOW;
}
Telescope::TrackState Telescope::getTrackingState()
{
  if (strchr(&TelStatus[0], 'N') == NULL)
  {
    return TRK_SLEWING;
  }
  else if (strchr(&TelStatus[0], 'n') == NULL)
  {
    return TRK_ON;
  }
  if (strchr(&TelStatus[0], 'n') != NULL && strchr(&TelStatus[0], 'N') != NULL)
  {
    return TRK_OFF;
  }
  return TRK_UNKNOW;
}
bool Telescope::atHome()
{
  return strchr(&TelStatus[0], 'H') != NULL;
}
bool Telescope::isPecPlaying()
{
  // PEC Status is one of "/,~;^" for ignore, wait play, play, wait rec, rec
  // "." is an index detect but isn't implemented yet
  return strchr(&TelStatus[0], '~') != NULL;
}
bool Telescope::isPecRecording()
{
  return strchr(&TelStatus[0], '^') != NULL;
}
bool Telescope::isPecWaiting()
{
  return (strchr(&TelStatus[0], ',') != NULL) || (strchr(&TelStatus[0], ';') != NULL);
}
bool Telescope::isGuiding()
{
  return  strchr(&TelStatus[0], 'G') != NULL;
}
bool Telescope::isMountGEM()
{
  return strchr(&TelStatus[0], 'E') != NULL;
}
bool Telescope::isMountAltAz()
{
  return strchr(&TelStatus[0], 'A') != NULL;
}
Telescope::PierState Telescope::getPierState()
{
  if (strchr(&sideofpier[0], 'E') != NULL)
  {
    return PIER_E;
  }
  else if (strchr(&sideofpier[0], 'W') != NULL)
  {
    return PIER_W;
  }
  return PIER_UNKNOW;
}
Telescope::Errors Telescope::getError()
{
  if (strlen(TelStatus) > 2)
  {
    int l = strlen(TelStatus) - 2;
    switch (TelStatus[l])
    {
    case '1':
      return ERR_MOTOR_FAULT;
    case '2':
      return ERR_ALT;
    case '4':
      return ERR_DEC;
    case '6':
      return ERR_UNDER_POLE;
    case '7':
      return ERR_MERIDIAN;
    default:
      return ERR_NONE;
    }
  }
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


