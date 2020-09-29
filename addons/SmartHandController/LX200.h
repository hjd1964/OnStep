#pragma once

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Pinmap.h"

enum LX200RETURN {
  LX200NOTOK, LX200SETVALUEFAILED, LX200GETVALUEFAILED, LX200SETTARGETFAILED, LX200NOOBJECTSELECTED, LX200UNKOWN,
  
  LX200_GOTO_ERR_BELOW_HORIZON, LX200_GOTO_ERR_ABOVE_OVERHEAD, LX200_GOTO_ERR_STANDBY, LX200_GOTO_ERR_PARK, LX200_GOTO_ERR_GOTO, 
  LX200_GOTO_ERR_OUTSIDE_LIMITS, LX200_GOTO_ERR_HARDWARE_FAULT,LX200_GOTO_ERR_IN_MOTION, LX200_GOTO_ERR_UNSPECIFIED,
  
  LX200OK, LX200VALUESET, LX200VALUEGET, LX200SYNCED, LX200_GOTO_GOINGTO
};

bool isOk(LX200RETURN val);
LX200RETURN GetLX200(char* command, char* output);
LX200RETURN GetLX200(const char* command, char* output); // overloaded to allow const char* strings without compiler warnings, similar follow below
LX200RETURN GetLX200Trim(char* command, char* output);
LX200RETURN GetLX200Trim(const char* command, char* output);
LX200RETURN GetTimeLX200(unsigned int &hour, unsigned int &minute, unsigned int &second, boolean ut=false);
LX200RETURN GetTimeLX200(long &value, boolean ut=false);
LX200RETURN SetLX200(char* command);
LX200RETURN SetLX200(const char* command);
LX200RETURN SetTimeLX200(long &value);
LX200RETURN GetSiteLX200(int &value);
void SetSiteLX200(int &value);
LX200RETURN Move2TargetLX200();
LX200RETURN SetTargetRaLX200(int vr1, int vr2, int vr3);
LX200RETURN SetTargetDecLX200(int vd1, int vd2, int vd3);
LX200RETURN SyncGotoLX200(bool, float &Ra, float &Dec);
LX200RETURN SyncSelectedStarLX200(long alignSelectedStar);
LX200RETURN GetDateLX200(unsigned int &day, unsigned int &month, unsigned int &year, boolean ut=false);
LX200RETURN SyncGotoCatLX200(bool sync);
LX200RETURN SyncGotoPlanetLX200(bool sync, unsigned short obj);
LX200RETURN readBacklashLX200(const uint8_t &axis, float &backlash);
LX200RETURN writeBacklashLX200(const uint8_t &axis, const float &backlash);
LX200RETURN readFocTCCoefLX200(const uint8_t &foc, float &tccoef);
LX200RETURN writeFocTCCoefLX200(const uint8_t &foc, const float &tccoef);
LX200RETURN readFocBacklashLX200(const uint8_t &foc, float &tccoef);
LX200RETURN writeFocBacklashLX200(const uint8_t &foc, const float &tccoef);
LX200RETURN readFocTCDeadbandLX200(const uint8_t &foc, float &tccoef);
LX200RETURN writeFocTCDeadbandLX200(const uint8_t &foc, const float &tccoef);

boolean hmsToDouble(double *f, char *hms);
boolean dmsToDouble(double *f, char *dms, boolean sign_present, boolean highPrecision);
