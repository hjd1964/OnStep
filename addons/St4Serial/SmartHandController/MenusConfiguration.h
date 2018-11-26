
#ifdef SHC_ON
void SmartHandController::menuMount()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_Mount = "Goto Speed\n""Backlash\n""Limits\n""Pier Side";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Configuration", current_selection_L2, string_list_Mount);
    switch (current_selection_L2)
    {
    case 1: menuGotoSpeed(); break;
    case 2: menuBacklash(); break;
    case 3: menuLimits(); break;
    case 4: menuPier(); break;
    default: break;
    }
  }
}

void SmartHandController::menuGotoSpeed()
{
  char string_list_LimitsL2[80];
  strcpy(string_list_LimitsL2,"Fastest\n""Faster\n""Default Speed\n""Slower\n""Slowest");

  char mr[20]=""; boolean ok = GetLX200Trim(":GX93#",mr) == LX200VALUEGET;
  if (ok) {
    long rateDefault=atol(mr);
    ok = GetLX200Trim(":GX92#",mr) == LX200VALUEGET;
    if (ok) {
      long rateCurrent=atol(mr);
      double r=(double)rateDefault/(double)rateCurrent;
      if (r>1.75) current_selection_L3 = 1; else
      if (r>1.25) current_selection_L3 = 2; else
      if (r>0.875) current_selection_L3 = 3; else
      if (r>0.625) current_selection_L3 = 4; else current_selection_L3 = 5;
    } else current_selection_L3 = 1;
  } else current_selection_L3 = 1;

  while (current_selection_L3 != 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Goto Speed", current_selection_L3, string_list_LimitsL2);
    switch (current_selection_L3)
    {
    case 1: SetLX200(":SX93,1#"); DisplayMessage("Goto Speed", "2X", 1500); break;
    case 2: SetLX200(":SX93,2#"); DisplayMessage("Goto Speed", "1.5X", 1500); break;
    case 3: SetLX200(":SX93,3#"); DisplayMessage("Goto Speed", "1X", 1500); break;
    case 4: SetLX200(":SX93,4#"); DisplayMessage("Goto Speed", "0.75X", 1500); break;
    case 5: SetLX200(":SX93,5#"); DisplayMessage("Goto Speed", "0.5X", 1500); break;
    default: break;
    }
  }
}

void SmartHandController::menuBacklash()
{
  char string_list_LimitsL2[80];
  uint8_t bl;
  strcpy(string_list_LimitsL2,"Axis1 RA/Az\n""Axis2 Dec/Alt");
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Backlash", current_selection_L3, string_list_LimitsL2);
    switch (current_selection_L3)
    {
    case 1: bl=1; menuSetBacklash(bl); break;
    case 2: bl=2; menuSetBacklash(bl); break;
    default: break;
    }
  }
}

void SmartHandController::menuLimits()
{
  char string_list_LimitsL2[80];
 
  if (telInfo.isMountGEM()) {
    strcpy(string_list_LimitsL2,"Horizon\n""Overhead\n""Meridian E\n""Meridian W");
  } else {
    strcpy(string_list_LimitsL2,"Horizon\n""Overhead");
  }
  
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Limits", current_selection_L3, string_list_LimitsL2);
    switch (current_selection_L3)
    {
    case 1:
      menuHorizon();
      break;
    case 2:
      menuOverhead();
      break;
    case 3:
      menuMeridianE();
      break;
    case 4:
      menuMeridianW();
      break;
    default:
      break;
    }
  }
}

void SmartHandController::menuHorizon()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Gh#", out)))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    if (display->UserInterfaceInputValueFloat(&buttonPad, "Horizon Limit", "", &angle, -10, 20, 2, 0, " degree"))
    {
      sprintf(out, ":Sh%+03d#", (int)angle);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuOverhead()
{
  char out[20];
  if (DisplayMessageLX200(GetLX200(":Go#", out)))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    if (display->UserInterfaceInputValueFloat(&buttonPad, "Overhead Limit", "", &angle, 60, 91, 2, 0, " degree"))
    {
      sprintf(out, ":So%02d#", (int)angle);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuMeridianE()
{
  char out[20] = "";
  if (DisplayMessageLX200(GetLX200(":GXE9#", out)))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    angle = round((angle * 15.0) / 60.0);
    if (display->UserInterfaceInputValueFloat(&buttonPad, "Meridn Limit E", "", &angle, -45, 45, 2, 0, " degree"))
    {
      angle = round((angle * 60.0) / 15.0);
      sprintf(out, ":SXE9,%+02d#", (int)angle);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuMeridianW()
{
  char out[20] = "";
  if (DisplayMessageLX200(GetLX200(":GXEA#", out)))
  {
    float angle = (float)strtol(&out[0], NULL, 10);
    angle = round((angle * 15.0) / 60.0);
    if (display->UserInterfaceInputValueFloat(&buttonPad, "Meridn Limit W", "", &angle, -45, 45, 2, 0, " degree"))
    {
      angle = round((angle * 60.0) / 15.0);
      sprintf(out, ":SXEA,%+02d#", (int)angle);
      DisplayMessageLX200(SetLX200(out),false);
    }
  }
}

void SmartHandController::menuPier()
{
  // get preferred pier side user setting
  bool ok = false;
  char ppsState[20]=""; ok = GetLX200(":GX96#",ppsState) == LX200VALUEGET;
  if (ok) {
    uint8_t choice = 1;
    if (ppsState[0]=='B') choice=1; else
    if (ppsState[0]=='E') choice=2; else
    if (ppsState[0]=='W') choice=3;
    
    choice = display->UserInterfaceSelectionList(&buttonPad, "Preferred Pier Side", choice, "Best\nEast\nWest");
    if (choice) {
      if (choice == 1) ok = DisplayMessageLX200(SetLX200(":SX96,B#"),false); else
      if (choice == 2) ok = DisplayMessageLX200(SetLX200(":SX96,E#"),false); else
      if (choice == 3) ok = DisplayMessageLX200(SetLX200(":SX96,W#"),false);
    }
  }
}
#endif

