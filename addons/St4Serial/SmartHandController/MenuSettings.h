// Settings menu

void SmartHandController::menuSettings()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    if (telInfo.isMountGEM()) {
#ifdef UTILITY_LIGHT      
      const char *string_list_SettingsL1 = "Date/Time\n""Display\n""Utility light\n""Buzzer\n""Meridian Flip\n""Configuration\n""Site\n""Firmware Ver";
#else      
      const char *string_list_SettingsL1 = "Date/Time\n""Display\n""Buzzer\n""Meridian Flip\n""Configuration\n""Site\n""Firmware Ver";
#endif      
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
      switch (current_selection_L1)
      {
      case 1: menuLocalDateTime(); break;
      case 2: menuDisplay(); break;
#ifdef UTILITY_LIGHT      
      case 3: menuUtilityLight(); break;
      case 4: menuSound(); break;
      case 5: menuMeridianFlips(); break;
      case 6: menuMount(); break;
      case 7: menuSite(); break;
      case 8: menuFirmware(); break;
#else      
      case 3: menuSound(); break;
      case 4: menuMeridianFlips(); break;
      case 5: menuMount(); break;
      case 6: menuSite(); break;
      case 7: menuFirmware(); break;
#endif      
      }
    } else {
      const char *string_list_SettingsL1 = "Date/Time\n""Display\n""Buzzer\n""Configuration\n""Site";
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Settings", current_selection_L1, string_list_SettingsL1);
      switch (current_selection_L1)
      {
      case 1: menuLocalDateTime(); break;
      case 2: menuDisplay(); break;
      case 3: menuSound(); break;
      case 4: menuMount(); break;
      case 5: menuSite(); break;
      }
    }
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
        if (value>=43200) { value-=43200; pmf=true; }
        if (display->UserInterfaceInputValueTime(&buttonPad, &value)) {
          if (display->UserInterfaceInputValueBoolean(&buttonPad, "Local Time PM?", &pmf)) {
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
    //EEPROM.write(14, maxContrast);
    //EEPROM.commit();
    display->setContrast(maxContrast);
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
    //EEPROM.writeLong(16, display_dim_time);
    //EEPROM.commit();
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
    //EEPROM.writeLong(20, display_blank_time);
    //EEPROM.commit();
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

#ifdef UTILITY_LIGHT
void SmartHandController::menuUtilityLight()
{
  const char *string_utility_light = "Off\nMin\nLow\nMedium\nHigh\nMax";
  uint8_t last_selection = current_selection_utility_light;

  current_selection_utility_light = display->UserInterfaceSelectionList(&buttonPad, "Set Brightness", current_selection_utility_light, string_utility_light);

  if (current_selection_utility_light > 0)
  {
    int i;
    switch(current_selection_utility_light) 
    {
      case 1: i=0;   break;
      case 2: i=15;  break;
      case 3: i=31;  break;
      case 4: i=63;  break;
      case 5: i=127; break;
      case 6: i=255; break;
      default: i=127; break;
    }
#ifdef ESP32
    ledcWrite(0, i);
#else
    analogWrite(UTILITY_LIGHT_PIN, i);
#endif
  } else  current_selection_utility_light = last_selection;
}
#endif
