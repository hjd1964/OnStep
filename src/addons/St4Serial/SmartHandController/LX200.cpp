#include <Arduino.h>
#include <Ephemeris.h>
#include "Catalog.h"
#include "LX200.h"

// integer numeric conversion with error checking
boolean atoi2(char *a, int *i) {
  char *conv_end;
  long l = strtol(a, &conv_end, 10);

  if ((l < -32767) || (l > 32768) || (&a[0] == conv_end)) return false;
  *i = l;
  return true;
}

void serialRecvFlush() {
  while (Ser.available() > 0) Ser.read();
}

void char2RA(char* txt, unsigned int& hour, unsigned int& minute, unsigned int& second)
{
  char* pEnd;
  hour = (int)strtol(&txt[0], &pEnd, 10);
  minute = (int)strtol(&txt[3], &pEnd, 10);
  second = (int)strtol(&txt[6], &pEnd, 10);
}

void char2DEC(char* txt, int& deg, unsigned int& min, unsigned int& sec)
{
  char* pEnd;
  deg = (int)strtol(&txt[0], &pEnd, 10);
  min = (int)strtol(&txt[4], &pEnd, 10);
  sec = (int)strtol(&txt[7], &pEnd, 10);
}


// this readBytesUntil() lets you know if the "character" was found
byte readBytesUntil2(char character, char buffer[], int length, boolean* characterFound, long timeout) {
  unsigned long startTime = millis() + timeout;
  int pos = 0;
  *characterFound = false;
  while (((long)(startTime - millis()) > 0) && (pos < length)) {
    if (Ser.available()) {
      buffer[pos] = Ser.read();
      if (buffer[pos] == character) { *characterFound = true; break; }
      pos++;
    }
  }
  return pos;
}

// smart LX200 aware command and response over serial
bool readLX200Bytes(char* command, char* recvBuffer, unsigned long timeOutMs) {
  Ser.setTimeout(timeOutMs);

  // clear the read/write buffers
  Ser.flush();
  serialRecvFlush();

  // send the command
  Ser.print(command);

  boolean noResponse = false;
  boolean shortResponse = false;
  if ((command[0] == (char)6) && (command[1] == 0)) shortResponse = true;
  if (command[0] == ':') {
    if (command[1] == '%') {
      Ser.setTimeout(timeOutMs * 4);
    }
    if (command[1] == 'A') {
      if (strchr("W123456789+", command[2])) { shortResponse = true; Ser.setTimeout(timeOutMs * 4); }
    }
    if (command[1] == 'M') {
      if (strchr("ewnsg", command[2])) noResponse = true;
      if (strchr("SA", command[2])) shortResponse = true;
    }
    if (command[1] == 'Q') {
      if (strchr("#ewns", command[2])) noResponse = true;
    }
    if (command[1] == 'S') {
      if (strchr("CLSGtgMNOPrdhoTB", command[2])) shortResponse = true;
    }
    if (command[1] == 'L') {
      if (strchr("BNCDL!", command[2])) noResponse = true;
      if (strchr("o$W", command[2])) shortResponse = true;
    }
    if (command[1] == 'B') {
      if (strchr("+-", command[2])) noResponse = true;
    }
    if (command[1] == 'C') {
      if (strchr("S", command[2])) noResponse = true;
    }
    if (command[1] == 'h') {
      if (strchr("F", command[2])) noResponse = true;
      if (strchr("COPQR", command[2])) { shortResponse = true; Ser.setTimeout(timeOutMs * 2); }
    }
    if (command[1] == 'T') {
      if (strchr("QR+-SLK", command[2])) noResponse = true;
      if (strchr("edrn", command[2])) shortResponse = true;
    }
    if (command[1] == 'U') noResponse = true;
    if ((command[1] == 'W') && (command[2] != '?')) { noResponse = true; }
    if ((command[1] == 'W') && (command[2] == '?')) { shortResponse = true; }
    if ((command[1] == '$') && (command[2] == 'Q') && (command[3] == 'Z')) {
      if (strchr("+-Z/!", command[4])) noResponse = true;
    }
    if (command[1] == 'G') {
      if (strchr("RD", command[2])) { timeOutMs *= 2; }
    }
  }

  if (noResponse) {
    recvBuffer[0] = 0;
    return true;
  }
  else
    if (shortResponse) {
      recvBuffer[Ser.readBytes(recvBuffer, 1)] = 0;
      return (recvBuffer[0] != 0);
    }
    else {
      // get full response, '#' terminated
      unsigned long start = millis();
      int recvBufferPos = 0;
      char b = 0;
      while (millis() - start < timeOutMs && (b != '#')) {
        if (Ser.available()) {
          b = Ser.read();
          recvBuffer[recvBufferPos] = b; recvBufferPos++; if (recvBufferPos > 19) recvBufferPos = 19; recvBuffer[recvBufferPos] = 0;
        }
      }
      return (recvBuffer[0] != 0);
    }
}

bool isOk(LX200RETURN val)
{
  return val >= LX200OK;
}

LX200RETURN SetLX200(char* command)
{
  char out[20];
  memset(out, 0, sizeof(out));
  if (readLX200Bytes(command, out, 100))
    return  LX200VALUESET;
  else
    return LX200SETVALUEFAILED;
}

LX200RETURN GetLX200(char* command, char* output)
{
  memset(output, 0, sizeof(output));
  if (readLX200Bytes(command, output, 100))
    return LX200VALUEGET;
  else
    return LX200GETVALUEFAILED;
}

LX200RETURN GetTimeLX200(unsigned int &hour, unsigned int &minute, unsigned int &second)
{
  char out[20];
  if (GetLX200(":GL#", out) == LX200GETVALUEFAILED)
    return LX200GETVALUEFAILED;
  char2RA(out, hour, minute, second);
  long value = hour * 60 + minute;
  value *= 60;
  value += second;
  char2RA(out, hour, minute, second);
  return LX200VALUEGET;
}

LX200RETURN GetTimeLX200(long &value)
{
  unsigned int hour, minute, second;
  if (!GetTimeLX200(hour, minute, second) == LX200GETVALUEFAILED)
    return LX200GETVALUEFAILED;
  value = hour * 60 + minute;
  value *= 60;
  value += second;
  return LX200VALUEGET;
}

LX200RETURN SetTimeLX200(long &value)
{
  char out[20];
  unsigned int hour, minute, second;
  second = value % 60;
  value /= 60;
  minute = value % 60;
  value /= 60;
  hour = value;
  sprintf(out, ":SL%02d:%02d:%02d#", hour, minute, second);
  if (SetLX200(out) ==  LX200VALUESET)
    return SetLX200(":SG+00#");
  return LX200SETVALUEFAILED;
}

LX200RETURN GetSiteLX200(int& value)
{
  char out[20];
  if (GetLX200(":W?#", out) == LX200VALUEGET)
  {
    value = (int)strtol(&out[0], NULL, 10) + 1;
    return LX200VALUEGET;
  }
  return LX200GETVALUEFAILED;
}

void SetSiteLX200(int& value)
{
  char out[20];
  sprintf(out, ":W%d#", value);
  Ser.print(out);
  Ser.flush();
}

LX200RETURN Move2TargetLX200()
{
  char out[20];
  memset(out, 0, sizeof(out));
  int val = readLX200Bytes(":MS#", out, 100);
  LX200RETURN response;
  switch (out[0])
  {
    //         1=Object below horizon    Outside limits, below the Horizon limit
    //         2=No object selected      Failure to resolve coordinates
    //         4=Position unreachable    Not unparked
    //         5=Busy                    Goto already active
    //         6=Outside limits          Outside limits, above the Zenith limit
  case 0:
    response = LX200GOINGTO;
    break;
  case 1:
    response = LX200BELOWHORIZON;
    break;
  case 2:
    response = LX200NOOBJECTSELECTED;
    break;
  case 4:
    response = LX200PARKED;
    break;
  case 5:
    response = LX200BUSY;
    break;
  case 6:
    response = LX200LIMITS;
    break;
  default:
    response = LX200UNKOWN;
    break;
  }
  return response;
}

LX200RETURN SetTargetRaLX200(uint8_t& vr1, uint8_t& vr2, uint8_t& vr3)
{
  char cmd[20], out[20];
  int iter = 0;
  sprintf(cmd, ":Sr%02u:%02u:%02u#", vr1, vr2, vr3);
  while (iter < 3)
  {
    if (SetLX200(cmd) ==  LX200VALUESET)
    {
      if (GetLX200(":Gr#", out) == LX200VALUEGET)
      {
        unsigned int hour, minute, second;
        char2RA(out, hour, minute, second);
        if (hour == vr1 && minute == vr2 && second == vr3)
        {
          return LX200VALUESET;
        }
      }
    }
    iter++;
  }
  return LX200SETVALUEFAILED;
}

LX200RETURN SetTargetDecLX200(short& vd1, uint8_t& vd2, uint8_t& vd3)
{
  char  cmd[20], out[20];
  int iter = 0;
  sprintf(cmd, ":Sd%+03d:%02u:%02u#", vd1, vd2, vd3);
  while (iter < 3)
  {
    if (SetLX200(cmd) ==  LX200VALUESET)
    {
      if (GetLX200(":Gd#", out) == LX200VALUEGET)
      {
        int deg;
        unsigned int min, sec;
        char2DEC(out, deg, min, sec);
        if (deg == vd1 && min == vd2 && sec == vd3)
        {
          return LX200VALUESET;
        }
      }
    }
    iter++;
  }
  return LX200SETVALUEFAILED;
}

LX200RETURN SyncGotoLX200(bool sync, uint8_t& vr1, uint8_t& vr2, uint8_t& vr3, short& vd1, uint8_t& vd2, uint8_t& vd3)
{
  if (SetTargetRaLX200(vr1, vr2, vr3) ==  LX200VALUESET && SetTargetDecLX200(vd1, vd2, vd3) == LX200VALUESET)
  {
    if (sync)
    {
      Ser.print(":CS#");
      Ser.flush();
      return LX200SYNCED;
    }
    else
    {
      return Move2TargetLX200();
    }
  }
  else
  {
    return LX200SETTARGETFAILED;
  }
}

LX200RETURN SyncGotoLX200(bool sync, float &Ra, float &Dec)
{
  int ivr1, ivr2, ivd1, ivd2;
  float fvr3, fvd3;
  Ephemeris::floatingHoursToHoursMinutesSeconds(Ra, &ivr1, &ivr2, &fvr3);
  Ephemeris::floatingDegreesToDegreesMinutesSeconds(Dec, &ivd1, &ivd2, &fvd3);
  uint8_t vr1, vr2, vr3, vd2, vd3;
  short vd1;
  vr1 = ivr1;
  vr2 = ivr2;
  vr3 = (int)fvr3;
  vd1 = ivd1;
  vd2 = ivd2;
  vd3 = (int)fvd3;
  return SyncGotoLX200(sync, vr1, vr2, vr3, vd1, vd2, vd3);
}



LX200RETURN GetDateLX200(unsigned int &day, unsigned int &month, unsigned int &year)
{
  char out[20];
  if (GetLX200(":GC#", out) == LX200VALUEGET)
  {
    char* pEnd;
    month = strtol(&out[0], &pEnd, 10);
    day = strtol(&out[3], &pEnd, 10);
    year = strtol(&out[6], &pEnd, 10) + 2000L;
    return LX200VALUEGET;
  }
  else
  {
    return LX200GETVALUEFAILED;
  }
}

LX200RETURN SyncGotoCatLX200(bool sync, Catalog cat, int idx)
{
  int epoch;
  unsigned int day, month, year, hour, minute, second;
  if (GetDateLX200(day, month, year) == LX200GETVALUEFAILED)
  {
    return LX200GETVALUEFAILED;
  }
  float ra, dec;
  switch (cat)
  {
  case STAR:
    getcathf(Star_ra[idx], ra);
    getcatdf(Star_dec[idx], dec);
    epoch = 2000;
    break;
  case MESSIER:
    getcathf(Messier_ra[idx], ra);
    getcatdf(Messier_dec[idx], dec);
    epoch = 2000;
    break;
  case HERSCHEL:
    getcathf(Herschel_ra[idx], ra);
    getcatdf(Herschel_dec[idx], dec);
    epoch = 1950;
    break;
  default:
    return LX200UNKOWN;
    break;
  }
  EquatorialCoordinates coo;
  coo.ra = ra;
  coo.dec = dec;
  EquatorialCoordinates cooNow;
  cooNow = Ephemeris::equatorialEquinoxToEquatorialJNowAtDateAndTime(coo, epoch, day, month, year, 0, 0, 0);
  return SyncGotoLX200(sync, cooNow.ra, cooNow.dec);
}
LX200RETURN SyncGotoPlanetLX200(bool sync, unsigned short objSys)
{
  char out[20];
  unsigned int day, month, year, hour, minute, second;

  if (GetDateLX200(day, month, year) == LX200GETVALUEFAILED)
  {
    return LX200GETVALUEFAILED;
  }
  if (GetTimeLX200(hour, minute, second) == LX200GETVALUEFAILED)
  {
    return LX200GETVALUEFAILED;
  }
  Ephemeris Eph;
  SolarSystemObjectIndex objI = static_cast<SolarSystemObjectIndex>(objSys);
  SolarSystemObject obj = Eph.solarSystemObjectAtDateAndTime(objI, day, month, year, hour, minute, second);
  return SyncGotoLX200(sync, obj.equaCoordinates.ra, obj.equaCoordinates.dec);
}

LX200RETURN SyncSelectedStarLX200(unsigned short alignSelectedStar)
{
  if (alignSelectedStar > 0 && alignSelectedStar < 292)
  {
    return SyncGotoCatLX200(false, STAR, alignSelectedStar - 1);
  }
  else
    return LX200UNKOWN;
}

LX200RETURN readReverseLX200(const uint8_t &axis, bool &reverse)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%RR#", out) : GetLX200(":%RD#", out);
  if (ok == LX200VALUEGET)
  {
    reverse = out[0] == '1' ? true : false;
  }
  return ok;
}

LX200RETURN writeReverseLX200(const uint8_t &axis, const bool &reverse)
{
  char text[20];
  sprintf(text, ":$RX%u#", (unsigned int)reverse);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readBacklashLX200(const uint8_t &axis, float &backlash)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%BR#", out) : GetLX200(":%BD#", out);
  if (ok == LX200VALUEGET)
  {
    backlash = (float)strtol(&out[0], NULL, 10);
  }
  return ok;
}
LX200RETURN writeBacklashLX200(const uint8_t &axis, const float &backlash)
{
  char text[20];
  sprintf(text, ":$BX%u#", (unsigned int)backlash);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readTotGearLX200(const uint8_t &axis, float &totGear)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%GR#", out) : GetLX200(":%GD#", out);
  if (ok == LX200VALUEGET)
  {
    totGear = (float)strtol(&out[0], NULL, 10);
  }
  return ok;
}
LX200RETURN writeTotGearLX200(const uint8_t &axis, const float &totGear)
{
  char text[20];
  sprintf(text, ":$GX%u#", (unsigned int)totGear);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readStepPerRotLX200(const uint8_t &axis, float &stepPerRot)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%SR#", out) : GetLX200(":%SD#", out);
  if (ok == LX200VALUEGET)
  {
    stepPerRot = (float)strtol(&out[0], NULL, 10);
  }
  return ok;
}
LX200RETURN writeStepPerRotLX200(const uint8_t &axis, const float &stepPerRot)
{
  char text[20];
  sprintf(text, ":$SX%u#", (unsigned int)stepPerRot);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readMicroLX200(const uint8_t &axis, uint8_t &microStep)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%MR#", out) : GetLX200(":%MD#", out);
  if (ok == LX200VALUEGET)
  {
    long value = strtol(&out[0], NULL, 10);

    if ((value >= 0 && value < 9))
    {
      microStep = value;
      return ok;
    }
    return LX200GETVALUEFAILED;
  }
  return ok;
}
LX200RETURN writeMicroLX200(const uint8_t &axis, const uint8_t &microStep)
{
  char text[20];
  sprintf(text, ":$MX%u#", microStep);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readLowCurrLX200(const uint8_t &axis, uint8_t &lowCurr)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%cR#", out) : GetLX200(":%cD#", out);
  if (ok == LX200VALUEGET)
  {
    long value = strtol(&out[0], NULL, 10);
    if (value >= 0 && value < 256)
    {
      lowCurr = value;
      return ok;
    }
    return LX200GETVALUEFAILED;
  }
  return ok;
}
LX200RETURN writeLowCurrLX200(const uint8_t &axis, const uint8_t &lowCurr)
{
  char text[20];
  sprintf(text, ":$cX%u#", lowCurr);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
LX200RETURN readHighCurrLX200(const uint8_t &axis, uint8_t &highCurr)
{
  char out[20];
  LX200RETURN ok = axis == 1 ? GetLX200(":%CR#", out) : GetLX200(":%CD#", out);
  if (ok == LX200VALUEGET)
  {
    long value = strtol(&out[0], NULL, 10);
    if (value >= 0 && value < 256)
    {
      highCurr = value;
      return ok;
    }
    return LX200GETVALUEFAILED;
  }
  return ok;
}
LX200RETURN writeHighCurrLX200(const uint8_t &axis, const uint8_t &highCurr)
{
  char text[20];
  sprintf(text, ":$CX%u#", highCurr);
  text[3] = axis == 1 ? 'R' : 'D';
  return SetLX200(text);
}
