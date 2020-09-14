#pragma once
#include <Arduino.h>

class Telescope
{
public:
  enum Errors {
    ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC,
    ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED, ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT
  };

  enum AlignMode { ALIM_OFF, ALIM_ONE, ALIM_TWO, ALIM_THREE, ALIM_FOUR, ALIM_FIVE, ALIM_SIX, ALIM_SEVEN, ALIM_EIGHT, ALIM_NINE };
  enum AlignState {
    ALI_OFF,
    ALI_SELECT_STAR_1, ALI_SLEW_STAR_1, ALI_RECENTER_1,
    ALI_SELECT_STAR_2, ALI_SLEW_STAR_2, ALI_RECENTER_2,
    ALI_SELECT_STAR_3, ALI_SLEW_STAR_3, ALI_RECENTER_3,
    ALI_SELECT_STAR_4, ALI_SLEW_STAR_4, ALI_RECENTER_4,
    ALI_SELECT_STAR_5, ALI_SLEW_STAR_5, ALI_RECENTER_5,
    ALI_SELECT_STAR_6, ALI_SLEW_STAR_6, ALI_RECENTER_6,
    ALI_SELECT_STAR_7, ALI_SLEW_STAR_7, ALI_RECENTER_7,
    ALI_SELECT_STAR_8, ALI_SLEW_STAR_8, ALI_RECENTER_8,
    ALI_SELECT_STAR_9, ALI_SLEW_STAR_9, ALI_RECENTER_9
  };
  enum Mount { GEM, FEM };
  enum TrackState { TRK_OFF, TRK_ON, TRK_SLEWING, TRK_UNKNOW };
  enum TrackRate { TR_SIDEREAL, TR_LUNAR, TR_SOLAR, TR_KING, TR_UNKNOW };
  enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
  enum ParkState { PRK_UNPARKED, PRK_PARKING, PRK_PARKED, PRK_FAILED, PRK_UNKNOW };
  enum PierState { PIER_NONE, PIER_E, PIER_W, PIER_UNKNOW };

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
  char TempUniversalTime[20];
  char TempSidereal[20];
  unsigned long lastStateTime;
  char TelStatus[20];
  unsigned long lastStateTel;
  unsigned long updateSeq=0;
public:
  bool connected = true;
  bool hasInfoRa = false;
  bool hasInfoDec = false;
  bool hasInfoAz = false;
  bool hasInfoAlt = false;
  bool hasInfoUTC = false;
  bool hasInfoSidereal = false;
  bool hasTelStatus = false;
  unsigned long lastState;
  void updateRaDec(boolean immediate=false);
  void updateAzAlt(boolean immediate=false);
  void updateTime(boolean immediate=false);
  void updateTel(boolean immediate=false);
  bool getRA(double &RA);
  bool getDec(double &Dec);
  double getLstT0();
  double getLat();
  int getAlignStars(int *maxStars, int *thisStar, int *numStars);
  ParkState getParkState();
  TrackState getTrackingState();
  bool atHome();
  bool isPecEnabled();
  bool isPecPlaying();
  bool isPecRecording();
  bool isPecWaiting();
  bool isGuiding();
  bool isMountGEM();
  bool isMountAltAz();
  bool getT(double &T);
  bool getP(double &P);
  bool getH(double &H);
  bool getDP(double &DP);
  PierState getPierState();
  TrackRate getTrackingRate();
  RateCompensation getRateCompensation();
  int getGuideRate();
  int getPulseGuideRate();
  Errors getError();
  bool hasFocuser1();
  bool hasFocuser2();
  bool hasRotator();
  bool hasDeRotator();
  bool hasReticle();
  bool hasDateTime();
  bool addStar();

private:
};
