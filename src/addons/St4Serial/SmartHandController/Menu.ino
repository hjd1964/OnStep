// -------------------------------------------------------------------------------
// Menu

#include "Selection_catalog.h"

uint8_t current_selection_L0 = 1;
uint8_t current_selection_L1 = 1;
uint8_t current_selection_L2 = 1;
unsigned short current_selection_Herschel = 1;
unsigned short current_selection_Star = 1;
uint8_t current_selection_L3 = 1;
uint8_t current_selection_L4 = 1;
uint8_t current_selection_speed = 5;

class motor
{
private:
  uint m_idx = 0;
  bool m_reverse = false;
  float m_backlash = 0;
  float m_totGear = 0;
  float m_wormGear = 0;
  float m_stepperGearBox = 0;
  float m_stepPerRot = 0;
  uint8_t m_microStep = 0;
  uint8_t m_lowCurr = 0;
  uint8_t m_highCurr = 0;
  bool readReverse()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%RR#", out, false) : GetLX200(":%RD#", out, false);
    if (ok)
    {
      m_reverse = out[0] == '1' ? true : false;
    }
    return ok;
  }
  bool writeReverse()
  {
    char text[20];
    sprintf(text, ":$RX%u#", (unsigned int)m_reverse);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readBacklash()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%BR#", out, false) : GetLX200(":%BD#", out, false);
    if (ok)
    {
      m_backlash = (float)strtol(&out[0], NULL, 10);
    }
    return ok;
  }
  bool writeBacklash()
  {
    char text[20];
    sprintf(text, ":$BX%u#", (unsigned int)m_backlash);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readTotGear()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%GR#", out, false) : GetLX200(":%GD#", out, false);
    if (ok)
    {
      m_totGear = (float)strtol(&out[0], NULL, 10);
    }
    return ok;
  }
  bool writeTotGear()
  {
    char text[20];
    sprintf(text, ":$GX%u#", (unsigned int)m_totGear);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readStepPerRot()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%SR#", out, false) : GetLX200(":%SD#", out, false);
    if (ok)
    {
      m_stepPerRot = (float)strtol(&out[0], NULL, 10);
    }
    return ok;
  }
  bool writeStepPerRot()
  {
    char text[20];
    sprintf(text, ":$SX%u#", (unsigned int)m_stepPerRot);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readMicro()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%MR#", out, false) : GetLX200(":%MD#", out, false);
    if (ok)
    {
      long value = strtol(&out[0], NULL, 10);
      ok = (value >= 0 && value < 9);
      if (ok)
        m_microStep = value;
    }
    return ok;
  }
  bool writeMicro()
  {
    char text[20];
    sprintf(text, ":$MX%u#", m_microStep);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readLowCurr()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%cR#", out, false) : GetLX200(":%cD#", out, false);
    if (ok)
    {
      long value = strtol(&out[0], NULL, 10);
      ok = (value >= 0 && value < 256);
      if (ok)
        m_lowCurr = value;
    }
    return ok;
  }
  bool writeLowCurr()
  {
    char text[20];
    sprintf(text, ":$cX%u#", m_lowCurr);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }
  bool readHighCurr()
  {
    char out[20];
    bool ok = m_idx == 1 ? GetLX200(":%CR#", out, false) : GetLX200(":%CD#", out, false);
    if (ok)
    {
      long value = strtol(&out[0], NULL, 10);
      ok = (value >= 0 && value < 256);
      if (ok)
        m_highCurr = value;
    }
    return ok;
  }
  bool writeHighCurr()
  {
    char text[20];
    sprintf(text, ":$CX%u#", m_highCurr);
    text[3] = m_idx == 1 ? 'R' : 'D';
    return SetLX200(text);
  }

public:
 
  const float getBacklash()
  {
    return m_backlash;
  }
  const float getTotGear()
  {
    return m_totGear;
  }
  const float getWormGear()
  {
    return m_wormGear;
  }
  const float getStepPerRot()
  {
    return m_stepPerRot;
  }
  const uint8_t getMicro()
  {
    return m_microStep;
  }
  const uint8_t getLowCurr()
  {
    return m_lowCurr;
  }
  const uint8_t getHighCurr()
  {
    return m_highCurr;
  }
  void setIdx(uint idx)
  {
    m_idx = idx;
  }
  void setWorm(unsigned short wormGear)
  {
    m_wormGear = wormGear;
  }
  void setAll(unsigned short wormGear, unsigned short stepperGearBox, unsigned short stepPerRot, unsigned short microStep)
  {
    m_wormGear = wormGear;
    m_stepperGearBox = stepperGearBox;
    m_stepPerRot = stepPerRot;
    m_microStep = microStep;
    ComputeTotGear();
  }
  bool SetStepperGearBox()
  {
    char text[20];
    sprintf(text, "Gear box M%u", m_idx);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), text, "Ratio", &m_stepperGearBox, 1, 100, 5, 0, ""))
    {
      ComputeTotGear();
    }
  }
  bool SetReverse()
  {
    if (!readReverse())
      return false;
    char text[20];
    char * string_list_micro = "Direct\nReversed";
    sprintf(text, "Rotation M%u", m_idx);
    uint8_t choice = onstep_UserInterfaceSelectionList(display.getU8g2(), text, m_reverse + 1, string_list_micro);
    if (choice)
    {
      m_reverse = choice - 1;
      return writeReverse();
    }
    return true;
  }
  bool SetBacklash()
  {
    if (!readTotGear())
      return false;
    char text[20];
    sprintf(text, "Backlash M%u", m_idx);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), text, "", &m_backlash, 0, 1000, 4, 0, " in seconds"))
    {
      return writeBacklash();
    }
    return true;
  }
  bool SetTotGear()
  {
    if (!readTotGear())
      return false;
    char text[20];
    sprintf(text, "Gear M%u", m_idx);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), text, "Ratio", &m_totGear, 1, 10000, 5, 0, ""))
    {
      return writeTotGear();
    }
    return true;
  }
  bool SetStepPerRot()
  {
    if (!readStepPerRot())
      return false;
    char text[20];
    sprintf(text, "Stepper M%u", m_idx);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), text, "", &m_stepPerRot, 1, 400, 3, 0, " Steps"))
    {
      return writeStepPerRot();
    }
    return true;
  }
  bool SetMicro()
  {
    if (!readMicro())
      return false;
    char text[20];
    char * string_list_micro = "16 (~256)\n32\n64\n128\n256";
    sprintf(text, "Stepper M%u", m_idx);
    uint8_t choice = m_microStep - 4 + 1;
    choice = onstep_UserInterfaceSelectionList(display.getU8g2(), text, choice, string_list_micro);
    if (choice)
    {
      m_microStep = choice - 1 + 4;
      return writeMicro();
    }
    return true;
  }
  bool SetLowCurrent()
  {
    if (!readLowCurr())
    {
      return false;
    }
    char text[20];
    sprintf(text, "Low Curr. M%u", m_idx);
    if (onstep_UserInterfaceInputValueInteger(display.getU8g2(), text, "", &m_lowCurr, 10, 200, 3, "0 mA"))
    {
      writeLowCurr();
    }
  }
  bool SetHighCurrent()
  {
    if (!readHighCurr())
    {
      return false;
    }
    char text[20];
    sprintf(text, "High Curr. M%u", m_idx);
    if (onstep_UserInterfaceInputValueInteger(display.getU8g2(), text, "", &m_highCurr, 10, 200, 3, "0 mA"))
    {
      writeHighCurr();
    }
  }

  void ComputeTotGear()
  {
    m_totGear = (unsigned long)((double)m_wormGear*m_stepperGearBox);
    writeTotGear();
  }
  void DisplayMotorSettings()
  {
    readAll();
    char line1[32];
    char line2[32];
    char line3[32];
    char line4[32];
    sprintf(line1, "Motor %u Settings", m_idx);
    m_reverse ? sprintf(line3, "Reversed Rotation") : sprintf(line3,"Direct Rotation");
    sprintf(line4, "Ratio: %u", (unsigned int)m_totGear);
    DisplayLongMessage(line1, NULL, line3, line4, -1);
    sprintf(line2, "%u Steps per Rot.", (unsigned int)m_stepPerRot);
    sprintf(line3, "MicroStep: %u", (unsigned int)pow(2,m_microStep));
    sprintf(line4, "Backlash: %u sec.", (unsigned int)m_backlash);
    DisplayLongMessage(line1, line2, line3, line4, -1);
    sprintf(line3, "Low Curr. %u0 mA", (unsigned int)m_lowCurr);
    sprintf(line4, "High Curr. %u0 mA", (unsigned int)m_highCurr);
    DisplayLongMessage(line1, NULL, line3, line4, -1);
  }
  void readAll()
  {
    readReverse();
    readBacklash();
    readTotGear();
    readStepPerRot();
    readMicro();
    readLowCurr(),
    readHighCurr();
  }
};

motor mountMotor[2];

void initMotor()
{
  mountMotor[0].setIdx(1);
  mountMotor[1].setIdx(2);
}

long angleRA = 0;
long angleDEC = 0;
bool ok = false;

uint8_t onstep_GetMenuEvent()
{
  tickButtons();
  int k = 0;
  for (k = 0; k < 7; k++)
  {
    if (eventbuttons[k] == E_CLICK || eventbuttons[k] == E_LONGPRESSTART || eventbuttons[k] == E_LONGPRESS)
    {
      break;
    }
  }
  switch (k)
  {
  case 0:
    return  U8X8_MSG_GPIO_MENU_HOME;
    break;
  case 1:
    return  U8X8_MSG_GPIO_MENU_UP;
    break;
  case 2:
    return  U8X8_MSG_GPIO_MENU_DOWN;
    break;
  case 3:
    return  U8X8_MSG_GPIO_MENU_PREV;
    break;
  case 4:
    return  U8X8_MSG_GPIO_MENU_NEXT;
    break;
  case 5:
    return  U8X8_MSG_GPIO_MENU_SELECT;
    break;
  case 6:
    return  U8X8_MSG_GPIO_MENU_HOME;
    break;
  default:
    break;
  }
  return 0;
}

void menuMain()
{
  current_selection_L0 = 1;
  while (current_selection_L0 != 0)
  {
    telInfo.updateTel();
    ParkState currentstate = telInfo.getParkState();

    if (currentstate == PRK_PARKED)
    {
      const char *string_list_main_ParkedL0 = "Unpark\n""Settings";
      current_selection_L0 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Main Menu", 0, string_list_main_ParkedL0);
      switch (current_selection_L0)
      {
      case 1:
        break;
      case 2:
        menuSettings();
        break;
      default:
        break;
      }
    }
    else if (currentstate == PRK_UNPARKED)
    {
      const char *string_list_main_UnParkedL0 = "Goto\nSync\nTracking\nSide of Pier\nSettings";
      current_selection_L0 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Main Menu", 0, string_list_main_UnParkedL0);
      switch (current_selection_L0)
      {
      case 1:
        menuSyncGoto(false);
        break;
      case 2:
        menuSyncGoto(true);
        break;
      case 3:
        menuTrack();
        break;
      case 4:
        menuPier();
        break;
      case 5:
        menuSettings();
        break;
      default:
        break;
      }
    }
  }
}

void menuSpeedRate()
{
  char * string_list_Speed = "0.25x\n0.5x\n1.0x\n2.0x\n4.0x\n16.0x\n32.0x\n64.0x\n0.5 Max\nMax";
  current_selection_speed = onstep_UserInterfaceSelectionList(display.getU8g2(), "Set Speed", current_selection_speed, string_list_Speed);
  if (current_selection_speed > 0)
  {
    char cmd[5] = ":Rn#";
    cmd[2] = '0' + current_selection_speed - 1;
    SetLX200(cmd, true);
  }
}

void menuTrack()
{
  telInfo.updateTel();
  TrackState currentstate = telInfo.getTrackingState();
  uint8_t choice;
  if (currentstate == TRK_ON)
  {
    const char *string_list_main_ParkedL0 = "Stop Tracking\n";
    choice = onstep_UserInterfaceSelectionList(display.getU8g2(), "Tracking State", 0, string_list_main_ParkedL0);
    switch (choice)
    {
    case 1:
      char out[20];
      memset(out, 0, sizeof(out));
      if (readLX200Bytes(":Td#", out, 100))
      {
        DisplayMessage("Tracking", "OFF", 500);
      }
      else
      {
        DisplayMessage("Set State", "Failed", 1000);
      }
      break;
    default:
      break;
    }
  }
  else if (currentstate == TRK_OFF)
  {
    const char *string_list_main_UnParkedL0 = "Start Tracking\n";
    current_selection_L0 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Tracking State", 0, string_list_main_UnParkedL0);
    switch (current_selection_L0)
    {
    case 1:
      char out[20];
      memset(out, 0, sizeof(out));
      if (readLX200Bytes(":Te#", out, 100))
      {
        DisplayMessage("Tracking", "ON", 500);
      }
      else
      {
        DisplayMessage("Set State", "Failed", 1000);
      }
      break;
    default:
      break;
    }
  }
  else
  {
    DisplayMessage("currently Tracking", "cannot be changed", 1000);
  }
}

void menuSyncGoto(bool sync)
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_gotoL1 = "Herschel\nStar\nCoordinates\nHome\nPark";
    current_selection_L1 = onstep_UserInterfaceSelectionList(display.getU8g2(), sync ? "Sync" : "Goto", current_selection_L1, string_list_gotoL1);
    switch (current_selection_L1)
    {
    case 1:
      MenuHerschel(sync);
      break;
    case 2:
      MenuStar(sync);
      break;
    case 3:
      MenuRADec(sync);
      break;
    case 4:
    {
      char cmd[5]= ":hX#";
      cmd[2] = sync ? 'F' : 'C';
      if (SetLX200(cmd, true))
      {
        DisplayMessage(sync ? "Reset at" : "Goto", " Home Position", -1);
      }
    }
      break;
    case 5:
    {
      char cmd[5] = ":hX#";
      cmd[2] = sync ? 'O' : 'P';
      if (SetLX200(cmd, false))
      {
        DisplayMessage(sync ? "Reset at" : "Goto", " Park Position", -1);
      }
      break;
    }
    default:
      break;
    }
  }
}

void MenuHerschel(bool sync)
{
  current_selection_Herschel = onstep_UserInterfaceCatalog(display.getU8g2(), sync ? "Sync Herschel" : "Goto Herschel", current_selection_Herschel, HERSCHEL);
  if (current_selection_Herschel != 0)
  {
    ok = SyncGotoCatLX200(sync, HERSCHEL, current_selection_Herschel - 1);
    if (ok)
    {
      // Quit Menu
      current_selection_L1 = 0;
      current_selection_L0 = 0;
    }
  }
}

void menuPier()
{
  telInfo.updateTel();
  uint8_t choice = ((uint8_t)telInfo.getPierState());
  choice = onstep_UserInterfaceSelectionList(display.getU8g2(), "Set Side of Pier", choice, "East\nWest");
  if (choice)
  {
    if (choice == 1)
      ok = SetLX200(":SmE#");
    else
      ok = SetLX200(":SmW#");
    if (ok)
    {
      DisplayMessage("Please Sync", "with a Target", 1000);
      menuSyncGoto(true);
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      ok = SetLX200(":SmN#");
    }
  }
}

void MenuStar(bool sync)
{
  current_selection_Star = onstep_UserInterfaceCatalog(display.getU8g2(), sync ? "Sync Star" : "Goto Star", current_selection_Star, STAR);
  if (current_selection_Star != 0)
  {
    ok = SyncGotoCatLX200(sync, STAR, current_selection_Star - 1);
    if (ok)
    {
      // Quit Menu
      current_selection_L1 = 0;
      current_selection_L0 = 0;
    }
  }
}

void MenuRADec(bool sync)
{
  if (onstep_UserInterfaceInputValueRA(display.getU8g2(), &angleRA))
  {
    uint8_t vr1, vr2, vr3, vd2, vd3;
    short vd1;
    gethms(angleRA, vr1, vr2, vr3);
    if (onstep_UserInterfaceInputValueDec(display.getU8g2(), &angleDEC))
    {
      getdms(angleDEC, vd1, vd2, vd3);
      ok = SyncGotoLX200(sync, vr1, vr2, vr3, vd1, vd2, vd3);
      if (ok)
      {
        // Quit Menu
        current_selection_L1 = 0;
        current_selection_L0 = 0;
      }
    }
  }
}

void menuSettings()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Display\n""Date\n""Time\n""Mount\n""Site\n""Limits\n""Wifi";
    current_selection_L1 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Settings", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
    case 1:
      menuDisplay();
      break;
    case 2:
      menuDate();
      break;
    case 3:
      menuUTCTime();
      break;
    case 4:
      menuMount();
      break;
    case 5:
      menuSite();
      break;
    case 6:
      menuLimits();
      break;
    case 7:
      menuWifi();
    default:
      break;
    }
  }
}

void menuMount()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_Mount = "Predefined\n""Mount type\n""Motor 1\n""Motor 2";
    current_selection_L2 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Mount", current_selection_L2, string_list_Mount);
    switch (current_selection_L2)
    {
    case 1:
      menuPredefinedMount();
      break;
    case 2:
      menuMountType();
      break;
    case 3:
      menuMotor(1);
      break;
    case 4:
      menuMotor(2);
      break;
    default:
      break;
    }
  }
}

void menuMountType()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Equatorial Fork\n""German Equatorial";
    current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Mount Type", current_selection_L3, string_list_Mount);
    switch (current_selection_L3)
    {
    case 1:
      break;
    case 2:
      break;
    default:
      break;
    }
  }
}

void menuPredefinedMount()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Alt Mount\n""ASTRO-PHYSICS\n""Fornax\n""Losmandy\n""Sideres85\n""Sky-Watcher\n""Takahashi\n""Vixen";
    current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Mount", current_selection_L3, string_list_Mount);
    switch (current_selection_L3)
    {
    case 1:
      menuAltMount();
      break;
    case 2:

      break;
    case 3:

      break;
    case 4:

      break;
    case 5:

      break;
    case 6:

    default:
      break;
    }
  }
}

void menuAltMount()
{
  mountType = GEM;
  current_selection_L4 = 1;
  while (current_selection_L4 != 0)
  {
    const char *string_list_Mount = "Alt 5-ADN\n""Alt 6-ADN\n""Alt 7-ADN\n";
    current_selection_L4 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Alt Mount", current_selection_L4, string_list_Mount);
    if (current_selection_L4 > 0)
    {
      switch (current_selection_L4)
      {
      case 1:
        mountMotor[0].setWorm(250);
        mountMotor[1].setWorm(200);
        break;
      case 2:
        mountMotor[0].setWorm(220);
        mountMotor[1].setWorm(200);
        break;
      case 3:
        mountMotor[0].setWorm(270);
        mountMotor[1].setWorm(210);
        break;
      default:
        break;
      }
      DisplayMessage("Set Parameters", "for Motor 1", -1);
      mountMotor[0].SetStepperGearBox();
      mountMotor[0].SetStepPerRot();
      mountMotor[0].SetMicro();
      mountMotor[0].DisplayMotorSettings();
      DisplayMessage("Set Parameters", "for Motor 2", -1);
      mountMotor[1].SetStepperGearBox();
      mountMotor[1].SetStepPerRot();
      mountMotor[1].SetMicro();
      mountMotor[1].DisplayMotorSettings();
    }
  }
}
void menuAP() {}
void menuFornax() {}
void menuLosmandy() {}
void menuSideres() {}
void menuSkyWatcher() {}
void menuTakahashi() {}
void menuVixen() {}

void menuMotor(uint8_t idx)
{
  current_selection_L3 = 1;

  while (current_selection_L3 != 0)
  {
    const char *string_list_Motor = "Show Settings\nRotation\nGear\n""Steps per Rotation\n""Micro Steps\n""Backlash\n""Low Current\n""High Current";
    current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), idx == 1 ? "Motor 1" : "Motor 2", current_selection_L3, string_list_Motor);
    switch (current_selection_L3)
    {
    case 1:
      mountMotor[idx - 1].DisplayMotorSettings();
      break;
    case 2:
      mountMotor[idx - 1].SetReverse();
      break;
    case 3:
      mountMotor[idx - 1].SetTotGear();
      break;
    case 4:
      mountMotor[idx - 1].SetStepPerRot();
      break;
    case 5:
      mountMotor[idx - 1].SetMicro();
      break;
    case 6:
      mountMotor[idx - 1].SetBacklash();
      break;
    case 7:
      mountMotor[idx - 1].SetLowCurrent();
      break;
    case 8:
      mountMotor[idx - 1].SetHighCurrent();
      break;
    default:
      break;
    }
  }
}

void menuSite()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = "Latitude\n""Longitude\n""Select Site";
    current_selection_L2 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Menu Site", current_selection_L2, string_list_SiteL2);
    switch (current_selection_L2)
    {
    case 1:
      menuLatitude();
      break;
    case 2:
      menuLongitude();
      break;
    case 3:
      menuSites();
      break;
    default:
      break;
    }
  }
}

void menuSites()
{
  char out[20];
  if (GetLX200(":W?#", out, false))
  {
    current_selection_L3 = (int)strtol(&out[0], NULL, 10) + 1;
    const char *string_list_SiteL3 = "Site 0\n""Site 1\n""Site 2\n""Site 3";
    current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Menu Sites", current_selection_L3, string_list_SiteL3);
    sprintf(out, ":W%d#", current_selection_L3 - 1);
    Serial.print(out);
    Serial.flush();
  }
}

void menuUTCTime()
{
  char out[20];
  if (GetLX200(":GL#", out, false))
  {
    int hour, minute, second;
    char2RA(out, hour, minute, second);
    long value = hour * 60 + minute;
    value *= 60;
    value += second;
    if (onstep_UserInterfaceInputValueUTCTime(display.getU8g2(), &value))
    {
      second = value % 60;
      value /= 60;
      minute = value % 60;
      value /= 60;
      hour = value;
      sprintf(out, ":SL%02d:%02d:%02d#", hour, minute, second);
      SetLX200(out);
      SetLX200(":SG+00#", true);
    }
  }
}

void menuDisplay()
{
  const char *string_list_Display = "Turn Off\nContrast";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Display", current_selection_L2, string_list_Display);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessage("Press any key", "to turn on", 1500);
      sleepDisplay = true;
      display.sleepOn();
      current_selection_L2 = 0;
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      break;
    case 2:
      menuContrast();
      break;
    default:
      break;
    }
  }
}

void menuContrast()
{
  const char *string_list_Display = "Min\nLow\nHigh\nMax";
  current_selection_L3 = 1;

  current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Set Contrast", current_selection_L3, string_list_Display);
  if (current_selection_L3 > 0)
  {
    maxContrast = (uint)63 * (current_selection_L3 - 1);
    EEPROM.write(14, maxContrast);
    display.setContrast(maxContrast);
  }
}

void menuDate()
{
  char out[20];
  if (GetLX200(":GC#", out, false))
  {
    char* pEnd;
    uint8_t month = strtol(&out[0], &pEnd, 10);
    uint8_t day = strtol(&out[3], &pEnd, 10);
    uint8_t year = strtol(&out[6], &pEnd, 10);
    if (onstep_UserInterfaceInputValueDate(display.getU8g2(), "Date", year, month, day))
    {
      sprintf(out, ":SC%02d/%02d/%02d#", month, day, year);
      SetLX200(out);
    }
  }
}

void menuLatitude()
{
  char out[20];
  if (GetLX200(":Gt#", out, false))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[4], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (onstep_UserInterfaceInputValueLatitude(display.getU8g2(), &angle))
    {
      angle /= 60;
      minute = abs(angle % 60);
      degree = angle / 60;
      sprintf(out, ":St%+03d*%02d#", degree, minute);
      SetLX200(out);
    }
  }
}

void menuLongitude()
{
  char out[20];
  if (GetLX200(":Gg#", out, false))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[5], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (onstep_UserInterfaceInputValueLongitude(display.getU8g2(), &angle))
    {
      angle /= 60;
      minute = abs(angle) % 60;
      degree = angle / 60;
      sprintf(out, ":Sg%+04d*%02d#", degree, minute);
      SetLX200(out);
    }
  }
}

void menuLimits()
{
  const char *string_list_LimitsL2 = "Horizon\n""Overhead\n""Meridian";
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    current_selection_L3 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Limits", current_selection_L3, string_list_LimitsL2);
    switch (current_selection_L3)
    {
    case 1:
      menuHorizon();
      break;
    case 2:
      menuOverhead();
      break;
    case 3:
      menuMeridian();
      break;
    default:
      break;
    }
  }
}

void menuWifi()
{
  const char *string_list = (wifiOn) ? "Wifi off\nShow Password" : "wifi on\nShow Password";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = onstep_UserInterfaceSelectionList(display.getU8g2(), "Wifi", 1, string_list);
    switch (current_selection_L2)
    {
    case 1:
      wifiOn = !wifiOn;
      EEPROM.write(15, wifiOn);
      DisplayMessage("Please", "Reboot!", 3000);
      current_selection_L2 = 0;
      powerCylceRequired = true;
      break;
    case 2:
      DisplayMessage("masterPassword is", "password", 1000);
    default:
      break;
    }
  }
}

void menuHorizon()
{
  char out[20];
  if (GetLX200(":Gh#", out, false))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), "Horizon Limit", "", &angle, -10, 20, 2, 0, " degree"))
    {
      sprintf(out, ":Sh%+03d#", (int)angle);
      SetLX200(out);
    }
  }
}

void menuOverhead()
{
  char out[20];
  if (GetLX200(":Go#", out, false))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    if (onstep_UserInterfaceInputValueFloat(display.getU8g2(), "Overhead Limit", "", &angle, 60, 91, 2, 0, " degree"))
    {
      sprintf(out, ":S0%02d#", (int)angle);
      SetLX200(out);
    }
  }
}

void menuMeridian()
{
  uint8_t angle = 0;
  if (onstep_UserInterfaceInputValueInteger(display.getU8g2(), "Meridian Limit", "", &angle, 0, 20, 2, " degree"))
  {

  }
}

void char2RA(char* txt, int& hour, int& minute, int& second)
{
  char* pEnd;
  hour = (int)strtol(&txt[0], &pEnd, 10);
  minute = (int)strtol(&txt[3], &pEnd, 10);
  second = (int)strtol(&txt[6], &pEnd, 10);
}

void char2DEC(char* txt, int& deg, int& min, int& sec)
{
  char* pEnd;
  deg = (int)strtol(&txt[0], &pEnd, 10);
  min = (int)strtol(&txt[4], &pEnd, 10);
  sec = (int)strtol(&txt[7], &pEnd, 10);
}

void longRa2Ra(long Ra, int& h, int& m, int& s)
{
  h = Ra / 30;
  m = (Ra - h * 30) / 60;
  s = (Ra / 30) % 60;
}

void longDec2Dec(long Dec, int& deg, int& min)
{
  deg = Dec / 60;
  min = Dec % 60;
}
