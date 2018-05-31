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
    if ((updateSeq%4==2) || immediate) { hasInfoUTC = GetLX200(":GL#", TempUTC) == LX200VALUEGET; if (!hasInfoUTC) connected = true; }
    if ((updateSeq%4==3) || immediate) { hasInfoSideral = GetLX200(":GS#", TempSideral) == LX200VALUEGET; if (!hasInfoSideral) connected = true; lastStateTime = millis(); }
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
bool Telescope::isGuiding()
{
  return  strchr(&TelStatus[0], 'G') != NULL;
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
void Telescope::addStar()
{
  if (align == ALI_RECENTER_1 || align == ALI_RECENTER_2 || align == ALI_RECENTER_3)
  {
    if (SetLX200(":A+#")==LX200VALUESET)
    {
      bool done = false;
      if (aliMode == ALIM_ONE
        || (aliMode == ALIM_TWO && align == ALI_RECENTER_2)
        || (aliMode == ALIM_THREE && align == ALI_RECENTER_3))
      {
        //TODO DisplayMessage("Alignment", "Success!", -1);
        align = ALI_OFF;
      }
      else
      {
        align = static_cast<AlignState>(align+1);
        //TODO DisplayMessage("Add Star", "Success!", -1);
      }

    }
    else
    {
      //TODO DisplayMessage("Add Star", "Failed!", -1);
      align = ALI_OFF;
    }
  }
  else
  {
    //TODO DisplayMessage("Failed!", "Wrong State", -1);
    align = ALI_OFF;
  }
}
