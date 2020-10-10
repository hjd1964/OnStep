// Settings menu

void SmartHandController::menuSettings()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    char string_list_SettingsL1[150] = "";

    int i=1; int index[11];
    index[1]=i++; strcat(string_list_SettingsL1,L_SET_DATE_TIME);
    index[2]=i++; strcat(string_list_SettingsL1,"\n" L_SET_SITE);
    if (telInfo.hasFocuser1()) { index[3]=i++; if (telInfo.hasFocuser2()) strcat(string_list_SettingsL1,"\n" L_SET_FOCUSER1); else strcat(string_list_SettingsL1,"\n" L_SET_FOCUSER); } else index[3]=-1;
    if (telInfo.hasFocuser2()) { index[4]=i++; strcat(string_list_SettingsL1,"\n" L_SET_FOCUSER2); } else index[4]=-1;
    if (telInfo.hasRotator())  { index[5]=i++; strcat(string_list_SettingsL1,"\n" L_SET_ROTATOR); } else index[5]=-1;
    index[6]=i++; strcat(string_list_SettingsL1,"\n" L_SET_DISPLAY);
    index[7]=i++; strcat(string_list_SettingsL1,"\n" L_SET_BUZ);
    if (telInfo.isMountGEM()) { index[8]=i++; strcat(string_list_SettingsL1,"\n" L_SET_MERIDIAN_FLIP); } else index[8]=-1;
    index[9]=i++; strcat(string_list_SettingsL1,"\n" L_SET_CONFIG);
    index[10]=i++; strcat(string_list_SettingsL1,"\n" L_SET_VERSION);

    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, L_SETTINGS, current_selection_L1, string_list_SettingsL1);
    if (current_selection_L1==index[1]) menuLocalDateTime();
    if (current_selection_L1==index[2]) menuSite();
    if (current_selection_L1==index[3]) menuFocuser1();
    if (current_selection_L1==index[4]) menuFocuser2();
    if (current_selection_L1==index[5]) menuRotator();
    if (current_selection_L1==index[6]) menuDisplay();
    if (current_selection_L1==index[7]) menuSound();
    if (current_selection_L1==index[8]) menuMeridianFlips();
    if (current_selection_L1==index[9]) menuMount();
    if (current_selection_L1==index[10]) menuFirmware();
  }
}

void SmartHandController::menuLocalDateTime()
{
  char out[20];
  // Date
  if (DisplayMessageLX200(GetLX200(":GC#", out)))
  {
    char* pEnd;
    uint8_t month = strtol(&out[0], &pEnd, 10);
    uint8_t day = strtol(&out[3], &pEnd, 10);
    uint8_t year = strtol(&out[6], &pEnd, 10);
    if (display->UserInterfaceInputValueDate(&buttonPad, L_SET_LOCAL_DATE, year, month, day))
    {
      sprintf(out, ":SC%02d/%02d/%02d#", month, day, year); DisplayMessageLX200(SetLX200(out),false);
      // Time
      long value;
      boolean pmf=false;
      boolean dst=false;
      if (DisplayMessageLX200(GetTimeLX200(value))) {
        if ((!hrs24) && (value>=43200)) { value-=43200; pmf=true; }
        if (display->UserInterfaceInputValueTime(&buttonPad, &value, hrs24)) {
          if (hrs24 || (display->UserInterfaceInputValueBoolean(&buttonPad, L_SET_LOCAL_PM "?", &pmf))) {
            if (pmf) value+=43200; // AM or PM?
            if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SET_LOCAL_DST "?", &dst)) {
              if (dst) value-=3600; // Dst?
              DisplayMessageLX200(SetTimeLX200(value),false);
            }
          }
        }
      }
    }
  }
}

void SmartHandController::menuDisplay()
{
  const char *string_list_Display = L_SET_DISP_OFF "\n" L_SET_DISP_CONT "\n" L_SET_DISP_DIM_TO "\n" L_SET_DISP_BLANK_TO;
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_DISPLAY, current_selection_L2, string_list_Display);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessage(L_SET_DISP_MSG1, L_SET_DISP_MSG2, 1500);
      sleepDisplay = true;
      display->sleepOn();
      current_selection_L2 = 0;
      current_selection_L1 = 0;
      current_selection_L0 = 0;
      break;
    case 2:
      menuContrast();
      break;
    case 3:
      menuDimTimeout();
      break;
    case 4:
      menuBlankTimeout();
      break;
    }
  }
}

void SmartHandController::menuContrast()
{
  const char *string_list_Display = L_SET_DISP_MIN "\n" L_SET_DISP_LOW "\n" L_SET_DISP_HIGH "\n" L_SET_DISP_MAX;
  current_selection_L3 = 1;

  current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, L_SET_DISP_CONTRAST, current_selection_L3, string_list_Display);
  if (current_selection_L3 > 0)
  {
    maxContrast = (uint)63 * (current_selection_L3 - 1);
    display->setContrast(maxContrast);
    nv.writeLong(EE_dispMaxContrast,(long)maxContrast);
  }
}

void SmartHandController::menuDimTimeout()
{
  const char *string_list_Display = L_DISABLE "\n""30 " L_SEC_ABV "\n""60 " L_SEC_ABV;
  current_selection_L3 = 2;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, L_SET_DISP_DIM_TO, current_selection_L3, string_list_Display);
    display_dim_time = (current_selection_L3 - 1) * 30000;
    nv.writeLong(EE_dispDimTimeout,(long)display_dim_time);
  }
}

void SmartHandController::menuBlankTimeout()
{
  const char *string_list_Display = L_DISABLE "\n""1 " L_MIN_ABV "\n""2 " L_MIN_ABV "\n""3 " L_MIN_ABV "\n""4 " L_MIN_ABV "\n""5 " L_MIN_ABV;
  current_selection_L3 = 3;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, L_SET_DISP_BLANK_TO, current_selection_L3, string_list_Display);
    display_blank_time = (current_selection_L3 - 1) * 60 * 1000;
    nv.writeLong(EE_dispBlankTimeout,(long)display_blank_time);
  }
}

void SmartHandController::menuSound()
{
  boolean sound = false;
  if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SET_BUZZER, &sound)) {
    if (sound) DisplayMessageLX200(SetLX200(":SX97,1#"),false); else DisplayMessageLX200(SetLX200(":SX97,0#"),false);
  }
}

void SmartHandController::menuMeridianFlips()
{
  const char *string_list = L_NOW "!" "\n" L_SET_MF_AUTO "\n" L_SET_MF_PAUSE;
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    boolean autoflip = false;
    boolean pause = false;
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_SET_MF, current_selection_L2, string_list);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":MN#"),false);
      break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SET_MF_AF, &autoflip)) {
        if (autoflip) DisplayMessageLX200(SetLX200(":SX95,1#"),false); else DisplayMessageLX200(SetLX200(":SX95,0#"),false);
      }
      break;
    case 3:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SET_MF_PF, &pause)) {
        if (pause) DisplayMessageLX200(SetLX200(":SX97,1#"),false); else DisplayMessageLX200(SetLX200(":SX97,0#"),false);
      }
      break;
    }
  }
}

#include "MenuMount.h"

void SmartHandController::menuSite()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = L_SET_SITE_SELECT "\n" L_SET_SITE_LAT "\n" L_SET_SITE_LONG "\n" L_SET_SITE_UTC;
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_SET_SITE_TITLE, current_selection_L2, string_list_SiteL2);
    switch (current_selection_L2)
    {
    case 1: menuSites(); break;
    case 2: menuLatitude(); break;
    case 3: menuLongitude(); break;
    case 4: menuZone(); break;
    }
  }
}

void SmartHandController::menuSites()
{
  int val;

  if (DisplayMessageLX200(GetSiteLX200(val)))
  {
    current_selection_L3 = val;
    const char *string_list_SiteL3 = L_SET_SITE_NUM " 1" "\n" L_SET_SITE_NUM " 2" "\n" L_SET_SITE_NUM " 3" "\n" L_SET_SITE_NUM " 4";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, L_SET_SITE_NUM_TITLE, current_selection_L3, string_list_SiteL3);
    if (current_selection_L3 != 0)
    {
      val = current_selection_L3 - 1;
      SetSiteLX200(val);
    }
  }
}

void SmartHandController::menuLatitude()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Gt#", out)))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[4], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (display->UserInterfaceInputValueLatitude(&buttonPad, &angle))
    {
      angle /= 60;
      minute = abs(angle % 60);
      degree = angle / 60;
      sprintf(out, ":St%+03d*%02d#", degree, minute);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuLongitude()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Gg#", out)))
  {
    char* pEnd;
    int degree = (int)strtol(&out[0], &pEnd, 10);
    int minute = (int)strtol(&out[5], &pEnd, 10);
    long angle = degree * 60;
    degree > 0 ? angle += minute : angle -= minute;
    angle *= 60;
    if (display->UserInterfaceInputValueLongitude(&buttonPad, &angle))
    {
      angle /= 60;
      minute = abs(angle) % 60;
      degree = angle / 60;
      sprintf(out, ":Sg%+04d*%02d#", degree, minute);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuZone()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":GG#", out)))
  {
    char* pEnd;
    int hr = (int)strtol(&out[0], &pEnd, 10);

    boolean negative=false;
    if (hr<0) negative=true;
    uint8_t b=abs(hr);
  
    if (display->UserInterfaceInputValueInteger(&buttonPad, "UTC " L_OFFSET_ABV " (-" L_TZ_ABV ")", "", &b, 0, 14, 2, " " L_HRS_ABV))
    {
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "UTC " L_OFFSET_ABV " - ?", &negative)) {
        hr=b;
        if (negative) hr=-hr;
        sprintf(out, ":SG%+02d#", hr);
        DisplayMessageLX200(SetLX200(out),false);
      }
    }
  }
}

void SmartHandController::menuFirmware()
{
  char out[20];
  
  sprintf(out,"SHC %s", _version);
  DisplayMessage(out, __DATE__, 3000);

  char temp1[20];
  char temp2[20];
  if ( (DisplayMessageLX200(GetLX200(":GVN#", temp1)))&&(DisplayMessageLX200(GetLX200(":GVD#", temp2))) )
  { for (char* p = temp1; (p = strchr(p, '#')); ++p) { *p = 0;} 
    for (char* p = temp2; (p = strchr(p, '#')); ++p) { *p = 0;} 
    sprintf(out,"OnStep %s",temp1);
    DisplayMessage(out, temp2, 3000);
  }
}

void SmartHandController::menuFocuser1()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = L_FOC_RET_HOME "\n" L_FOC_AT_HOME "\n" L_FOC_BACKLASH "\n" L_FOC_TC "\n" L_FOC_TC_COEF "\n" L_FOC_TC_DEADBAND;
    
    if (telInfo.hasFocuser2())
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_FOCUSER " 1", current_selection_L2, string_list_SiteL2);
    else
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_FOCUSER, current_selection_L2, string_list_SiteL2);
    bool isOk=false;
    uint8_t foc = 1;
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":FA1#:Fh#"),false);
      break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_FOC_AT_HALF, &isOk)) {
        if (isOk) { DisplayMessageLX200(SetLX200(":FA1#:FH#"),false); }
      }
      break;
    case 3:
      menuSetFocBacklash(foc);
      break;
    case 4:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_FOC_TC, &isOk)) {
        if (isOk)
        {
          if (DisplayMessageLX200(SetLX200(":FA1#")))
            DisplayMessageLX200(SetLX200(":Fc1#"),false);
        }
        else
        {
          if (DisplayMessageLX200(SetLX200(":FA1#")))
            DisplayMessageLX200(SetLX200(":Fc0#"),false);
        }
      }
      break;
    case 5:
      menuSetFocTCCoef(foc);
      break;
    case 6:
      menuSetFocTCDeadband(foc);
      break;
    }
  }
}

void SmartHandController::menuFocuser2()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = L_FOC_RET_HOME "\n" L_FOC_AT_HOME "\n" L_FOC_BACKLASH "\n" L_FOC_TC "\n" L_FOC_TC_COEF "\n" L_FOC_TC_DEADBAND;
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_FOCUSER " 2", current_selection_L2, string_list_SiteL2);
    bool isOk=false;
    uint8_t foc = 2;
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":FA2#:Fh#"),false);
    break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_FOC_AT_HALF, &isOk)) {
        if (isOk) { DisplayMessageLX200(SetLX200(":FA2#:FH#"),false); }
      }
    break;
    case 3:
      menuSetFocBacklash(foc);
      break;
    case 4:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_FOC_TC, &isOk)) {
        if (isOk)
        {
          if (DisplayMessageLX200(SetLX200(":FA2#")))
            DisplayMessageLX200(SetLX200(":Fc1#"),false);
        }
        else
        {
          if (DisplayMessageLX200(SetLX200(":FA2#")))
            DisplayMessageLX200(SetLX200(":Fc0#"),false);
        }
      }
      break;
    case 5:
      menuSetFocTCCoef(foc);
      break;
    case 6:
      menuSetFocTCDeadband(foc);
      break;
    }
  }
}

bool SmartHandController::menuSetFocBacklash(uint8_t &foc)
{
  float backlash;
  if (!DisplayMessageLX200(readFocBacklashLX200(foc, backlash))) return false;
  char text[20];
  if (foc == 1 && !telInfo.hasFocuser2())
    sprintf(text, "Focuser " L_FOC_BACKLASH);
  else
    sprintf(text, "Foc.%u " L_FOC_BACKLASH, foc);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "", &backlash, 0, 999, 3, 0, " " L_FOC_BL_UNITS))
  {
    return DisplayMessageLX200(writeFocBacklashLX200(foc, backlash),false);
  }
  return true;
}

bool SmartHandController::menuSetFocTCCoef(uint8_t &foc)
{
  float tccoef;
  if (!DisplayMessageLX200(readFocTCCoefLX200(foc, tccoef))) return false;
  char text[20];
  if (foc == 1 && !telInfo.hasFocuser2())
    sprintf(text, "Focuser " L_FOC_TC_COEF);
  else
    sprintf(text, "Foc.%u " L_FOC_TC_COEF, foc);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "", &tccoef, -999, 999, 4, 0, " " L_MICRON_PER_C))
  {
    return DisplayMessageLX200(writeFocTCCoefLX200(foc, tccoef),false);
  }
  return true;
}

bool SmartHandController::menuSetFocTCDeadband(uint8_t &foc)
{
  float deadband;
  if (!DisplayMessageLX200(readFocTCDeadbandLX200(foc, deadband))) return false;
  char text[20];
  if (foc == 1 && !telInfo.hasFocuser2())
    sprintf(text, "Focuser " L_FOC_TC_DEADBAND);
  else
    sprintf(text, "Foc.%u " L_FOC_TC_DEADBAND, foc);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "", &deadband, 1, 999, 3, 0, " " L_FOC_TC_DB_UNITS))
  {
    return DisplayMessageLX200(writeFocTCDeadbandLX200(foc, deadband),false);
  }
  return true;
}

void SmartHandController::menuRotator()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    char string_list_SiteL2[80];
    if (telInfo.hasDeRotator())
      strcpy(string_list_SiteL2,L_ROT_RET_HOME "\n" L_ROT_AT_HOME "\n" L_ROT_DEROT "\n" L_ROT_PA "\n" L_ROT_REV);
    else
      strcpy(string_list_SiteL2,L_ROT_RET_HOME "\n" L_ROT_AT_HOME);
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, L_ROTATOR, current_selection_L2, string_list_SiteL2);
    bool isOk=false;
    switch (current_selection_L2)
    {
    case 1: SetLX200(":rC#"); DisplayMessage(L_VALUE, L_SETV "!", 1500); break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_ROT_AT_HOME_ZERO "?", &isOk)) {
        if (isOk) { SetLX200(":rF#"); DisplayMessage(L_VALUE, L_SETV "!", 1500); }
      }
    break;
    case 3:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_ROT_DEROT " " L_ON "?", &isOk)) {
        if (isOk) { SetLX200(":r+#"); DisplayMessage(L_VALUE, L_SETV "!", 1500); } else { SetLX200(":r-#"); DisplayMessage(L_VALUE, L_SETV "!", 1500); }
      }
    break;
    case 4: DisplayMessageLX200(SetLX200(":rP#"),true); break;
    case 5:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_ROT_REVERSE "?", &isOk)) {
        if (isOk) { SetLX200(":rR#"); DisplayMessage(L_VALUE, L_SETV "!", 1500); }
      }
    break;
    }
  }
}
