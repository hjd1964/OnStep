// -------------------------------------------------------------------------------
// TelState class header file

bool GetLX200(char* command, char* output, bool silent);
bool SetLX200(char* command, bool silent);

bool buttonCommand = false;
class TelState
{
public:
  char TempRa[20];
  char TempDec[20];
  unsigned long lastStateRaDec;
  char TempAz[20];
  char TempAlt[20];
  unsigned long lastStateAzAlt;
  char TempUTC[20];
  char TempSideral[20];
  unsigned long lastStateTime;
  char TelStatus[20];
  char sideofpier[20];
  unsigned long lastStateTel;
  bool connected = true;
  bool hasInfoRa = false;
  bool hasInfoDec = false;
  bool hasInfoAz = false;
  bool hasInfoAlt = false;
  bool hasInfoUTC = false;
  bool hasInfoSideral = false;
  bool hasPierInfo = false;
  bool hasTelStatus = false;
  unsigned long lastState;

  void updateRaDec()
  {
    if (millis() - lastStateRaDec > 100 && connected)
    {
      hasInfoRa = GetLX200(":GR#", TempRa, true);
      hasInfoDec = GetLX200(":GD#", TempDec, true);
      lastStateRaDec = millis();
      if (!hasInfoRa && !hasInfoDec)
      {
        connected = true;
      }
    }
  };
  void updateAzAlt()
  {
    if (millis() - lastStateAzAlt > 100 && connected)
    {
      hasInfoAz = GetLX200(":GZ#", TempAz, true);
      hasInfoAlt = GetLX200(":GA#", TempAlt, true);
      lastStateAzAlt = millis();
      if (!hasInfoAz && !hasInfoAlt)
      {
        connected = true;
      }
    }
  }
  void updateTime()
  {
    if (millis() - lastStateTime > 100 && connected)
    {
      hasInfoUTC = GetLX200(":GL#", TempUTC, true);
      hasInfoSideral = GetLX200(":GS#", TempSideral, true);
      lastStateTime = millis();
      if (!hasInfoUTC && !hasInfoSideral)
      {
        connected = true;
      }
    }

  };

  void updateTel()
  {
    if (millis() - lastStateTel > 100 && connected)
    {
      hasPierInfo = GetLX200(":Gm#", sideofpier, true);
      hasTelStatus = GetLX200(":GU#", TelStatus, true);
      lastStateTel = millis();
    }
    if (!hasPierInfo && !hasTelStatus)
    {
      connected = true;
    }
  };
  ParkState getParkState()
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
  TrackState getTrackingState()
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
  bool atHome()
  {
    return strchr(&TelStatus[0], 'H') != NULL;
  }
  bool isGuiding()
  {
    return  strchr(&TelStatus[0], 'G') != NULL;
  }
  PierState getPierState()
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
  Errors getError()
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
} telInfo;

