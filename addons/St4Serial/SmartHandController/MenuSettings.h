// Settings menu

void SmartHandController::menuSettings()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    char string_list_SettingsL1[150] = "";

    int i=1; int index[11];
    index[1]=i++; strcat(string_list_SettingsL1,"Date/Time");
    index[2]=i++; strcat(string_list_SettingsL1,"\nSite");
    if (telInfo.hasFocuser1()) { index[3]=i++; if (telInfo.hasFocuser2()) strcat(string_list_SettingsL1,"\nFocuser1"); else strcat(string_list_SettingsL1,"\nFocuser"); } else index[3]=-1;
    if (telInfo.hasFocuser2()) { index[4]=i++; strcat(string_list_SettingsL1,"\nFocuser2"); } else index[4]=-1;
    if (telInfo.hasRotator())  { index[5]=i++; strcat(string_list_SettingsL1,"\nRotator"); } else index[5]=-1;
    index[6]=i++; strcat(string_list_SettingsL1,"\nDisplay");
    index[7]=i++; strcat(string_list_SettingsL1,"\nBuzzer");
    if (telInfo.isMountGEM()) { index[8]=i++; strcat(string_list_SettingsL1,"\nMeridian Flip"); } else index[8]=-1;
    index[9]=i++; strcat(string_list_SettingsL1,"\nConfiguration");
    index[10]=i++; strcat(string_list_SettingsL1,"\nFirmware Ver");

    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
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
    if (display->UserInterfaceInputValueDate(&buttonPad, "Local Date", year, month, day))
    {
      sprintf(out, ":SC%02d/%02d/%02d#", month, day, year); DisplayMessageLX200(SetLX200(out),false);
      // Time
      long value;
      boolean pmf=false;
      boolean dst=false;
      if (DisplayMessageLX200(GetTimeLX200(value))) {
        if ((!hrs24) && (value>=43200)) { value-=43200; pmf=true; }
        if (display->UserInterfaceInputValueTime(&buttonPad, &value, hrs24)) {
          if (hrs24 || (display->UserInterfaceInputValueBoolean(&buttonPad, "Local Time PM?", &pmf))) {
            if (pmf) value+=43200; // AM or PM?
            if (display->UserInterfaceInputValueBoolean(&buttonPad, "Local Time DST?", &dst)) {
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
  const char *string_list_Display = "Turn Off\nContrast\nDim Timeout\nBlank Timeout";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Display", current_selection_L2, string_list_Display);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessage("Press any key", "to turn on", 1500);
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
  const char *string_list_Display = "Min\nLow\nHigh\nMax";
  current_selection_L3 = 1;

  current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Set Contrast", current_selection_L3, string_list_Display);
  if (current_selection_L3 > 0)
  {
    maxContrast = (uint)63 * (current_selection_L3 - 1);
    display->setContrast(maxContrast);
    nv.writeLong(EE_dispMaxContrast,(long)maxContrast);
  }
}

void SmartHandController::menuDimTimeout()
{
  const char *string_list_Display = "Disable\n30 sec\n60 sec";
  current_selection_L3 = 2;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Dim Timeout", current_selection_L3, string_list_Display);
    display_dim_time = (current_selection_L3 - 1) * 30000;
    nv.writeLong(EE_dispDimTimeout,(long)display_dim_time);
  }
}

void SmartHandController::menuBlankTimeout()
{
  const char *string_list_Display = "Disable\n1 min\n2 min\n3 min\n4 min\n5 min";
  current_selection_L3 = 3;

  if (current_selection_L3 > 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Blank Timeout", current_selection_L3, string_list_Display);
    display_blank_time = (current_selection_L3 - 1) * 60 * 1000;
    nv.writeLong(EE_dispBlankTimeout,(long)display_blank_time);
  }
}

void SmartHandController::menuSound()
{
  boolean sound = false;
  if (display->UserInterfaceInputValueBoolean(&buttonPad, "Buzzer On?", &sound)) {
    if (sound) DisplayMessageLX200(SetLX200(":SX97,1#"),false); else DisplayMessageLX200(SetLX200(":SX97,0#"),false);
  }
}

void SmartHandController::menuMeridianFlips()
{
  const char *string_list = "Now!\n""Automatic\n""Pause at Home";
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    boolean autoflip = false;
    boolean pause = false;
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Meridian Flip", current_selection_L2, string_list);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":MN#"),false);
      break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Auto Flip?", &autoflip)) {
        if (autoflip) DisplayMessageLX200(SetLX200(":SX95,1#"),false); else DisplayMessageLX200(SetLX200(":SX95,0#"),false);
      }
      break;
    case 3:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Pause Flip?", &pause)) {
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
    const char *string_list_SiteL2 = "Select Site\n""Latitude\n""Longitude\n""UTC Offset";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Menu Site", current_selection_L2, string_list_SiteL2);
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
    const char *string_list_SiteL3 = "Site 1\n""Site 2\n""Site 3\n""Site 4";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Menu Sites", current_selection_L3, string_list_SiteL3);
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
  
    if (display->UserInterfaceInputValueInteger(&buttonPad, "UTC Ofs (-Zone)", "", &b, 0, 14, 2, " hrs"))
    {
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "UTC Ofs - ?", &negative)) {
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
    const char *string_list_SiteL2 = "Return Home\n""At Home";
    
    if (telInfo.hasFocuser2())
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Focuser 1", current_selection_L2, string_list_SiteL2);
    else
      current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Focuser", current_selection_L2, string_list_SiteL2);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":FA1#:FS0#"),false);
      break;
    case 2:
      bool isOk=false;
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "At Home/Zero?", &isOk)) {
        if (isOk) { DisplayMessageLX200(SetLX200(":FA1#:FZ#"),false); }
      }
      break;
    }
  }
}

void SmartHandController::menuFocuser2()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_SiteL2 = "Return Home\n""At Home";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Focuser 2", current_selection_L2, string_list_SiteL2);
    switch (current_selection_L2)
    {
    case 1:
      DisplayMessageLX200(SetLX200(":FA2#:FS0#"),false);
    break;
    case 2:
      bool isOk=false;
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "At Home/Zero?", &isOk)) {
        if (isOk) { DisplayMessageLX200(SetLX200(":FA2#:FZ#"),false); }
      }
    break;
    }
  }
}

void SmartHandController::menuRotator()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    char string_list_SiteL2[80];
    if (telInfo.hasDeRotator())
      strcpy(string_list_SiteL2,"Return Home\n""At Home\n""De-rotate\nMove to PA\nReverse");
    else
      strcpy(string_list_SiteL2,"Return Home\n""At Home");
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Rotator", current_selection_L2, string_list_SiteL2);
    bool isOk=false;
    switch (current_selection_L2)
    {
    case 1: SetLX200(":rC#"); DisplayMessage("Value", "Set!", 1500); break;
    case 2:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "At Home/Zero?", &isOk)) {
        if (isOk) { SetLX200(":rF#"); DisplayMessage("Value", "Set!", 1500); }
      }
    break;
    case 3:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "De-rotate On?", &isOk)) {
        if (isOk) { SetLX200(":r+#"); DisplayMessage("Value", "Set!", 1500); } else { SetLX200(":r-#"); DisplayMessage("Value", "Set!", 1500); }
      }
    break;
    case 4: DisplayMessageLX200(SetLX200(":rP#"),true); break;
    case 5:
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Reverse?", &isOk)) {
        if (isOk) { SetLX200(":rR#"); DisplayMessage("Value", "Set!", 1500); }
      }
    break;
    }
  }
}
