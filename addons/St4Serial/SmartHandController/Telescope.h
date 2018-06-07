#pragma once
#include <Arduino.h>
#include "Catalog.h"

class Telescope
{
public:
  enum Errors { ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC };

  enum AlignMode { ALIM_OFF, ALIM_ONE, ALIM_TWO, ALIM_THREE, ALIM_FOUR, ALIM_SIX };
  enum AlignState {
    ALI_OFF,
    ALI_SELECT_STAR_1, ALI_SLEW_STAR_1, ALI_RECENTER_1,
    ALI_SELECT_STAR_2, ALI_SLEW_STAR_2, ALI_RECENTER_2,
    ALI_SELECT_STAR_3, ALI_SLEW_STAR_3, ALI_RECENTER_3,
    ALI_SELECT_STAR_4, ALI_SLEW_STAR_4, ALI_RECENTER_4,
    ALI_SELECT_STAR_5, ALI_SLEW_STAR_5, ALI_RECENTER_5,
    ALI_SELECT_STAR_6, ALI_SLEW_STAR_6, ALI_RECENTER_6
  };
  enum Mount { GEM, FEM };
  enum TrackState { TRK_OFF, TRK_ON, TRK_SLEWING, TRK_UNKNOW };
  enum ParkState { PRK_UNPARKED, PRK_PARKED, PRK_FAILED, PRK_PARKING, PRK_UNKNOW };
  enum PierState { PIER_E, PIER_W, PIER_UNKNOW };

public: 
  AlignState      align = ALI_OFF;
  AlignMode       aliMode = ALIM_ONE;
  unsigned short  alignSelectedStar = 1;
  int             alignMaxNumStars = -1;

  Errors lastError = Telescope::ERR_NONE;

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
  unsigned long updateSeq=0;
public:
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
  void updateRaDec(boolean immediate=false);
  void updateAzAlt(boolean immediate=false);
  void updateTime(boolean immediate=false);
  void updateTel(boolean immediate=false);
  double getLstT0();
  double getLat();
  int getAlignStars(int *maxStars, int *thisStar, int *numStars);
  ParkState getParkState();
  TrackState getTrackingState();
  bool atHome();
  bool isGuiding();
  bool isMountGEM();
  PierState getPierState();
  Errors getError();
  bool addStar();

private:
};

