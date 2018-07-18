
#ifdef ST4SMARTCONTROLLER_ON
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
  
  if ((telInfo.hasTelStatus) && (telInfo.isMountGEM())) {
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
  bool ok = false;
  telInfo.updateTel(true);
  
  // get preferred pier side user setting
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


#ifndef ST4SMARTCONTROLLER_ON
void SmartHandController::menuMount()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_Mount = "Predefined\n""Mount type\n""Motor 1\n""Motor 2";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Mount", current_selection_L2, string_list_Mount);
    switch (current_selection_L2)
    {
    case 1: menuPredefinedMount(); break;
    case 2: menuMountType(); break;
    case 3: menuMotor(1); break;
    case 4: menuMotor(2); break;
    default: break;
    }
  }
}
#endif

void SmartHandController::menuMountType()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Equatorial Fork\n""German Equatorial";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Mount Type", current_selection_L3, string_list_Mount);
    switch (current_selection_L3)
    {
    case 1: break;
    case 2: break;
    default: break;
    }
  }

}

void SmartHandController::menuPredefinedMount()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Alt Mount\n""ASTRO-PHYSICS\n""Fornax\n""Losmandy\n""Sideres85\n""Sky-Watcher\n""Takahashi\n""Vixen";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Mount", current_selection_L3, string_list_Mount);
    switch (current_selection_L3)
    {
    case 1: menuAltMount(); break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    default: break;
    }
  }

}

void SmartHandController::menuAltMount()
{
  current_selection_L4 = 1;
  while (current_selection_L4 != 0)
  {
    const char *string_list_Mount = "Alt 5-ADN\n""Alt 6-ADN\n""Alt 7-ADN\n";
    current_selection_L4 = display->UserInterfaceSelectionList(&buttonPad, "Alt Mount", current_selection_L4, string_list_Mount);
  }
}

void SmartHandController::menuMotor(uint8_t axis)
{
  current_selection_L3 = 1;

  while (current_selection_L3 != 0)
  {
    const char *string_list_Motor = "Show Settings\nRotation\nGear\n""Steps per Rotation\n""Micro Steps\n""Backlash\n""Low Current\n""High Current";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, axis == 1 ? "Motor 1" : "Motor 2", current_selection_L3, string_list_Motor);
    switch (current_selection_L3)
    {
    case 1: break;
    case 2: menuSetReverse(axis); break;
    case 3: menuSetTotGear(axis); break;
    case 4: menuSetStepPerRot(axis); break;
    case 5: menuSetMicro(axis); break;
    case 6: menuSetBacklash(axis); break;
    case 7: menuSetLowCurrent(axis); break;
    case 8: menuSetHighCurrent(axis); break;
    default: break;
    }
  }
}

bool SmartHandController::menuSetStepperGearBox(uint8_t &axis, unsigned short &worm)
{
  char text[20];
  float stepperGearBox = 10;
  sprintf(text, "Gear box M%u", axis);
  if (display->UserInterfaceInputValueFloat(&buttonPad,text, "Ratio", &stepperGearBox, 1, 100, 5, 0, ""))
  {
    return DisplayMessageLX200(writeTotGearLX200(axis, stepperGearBox * worm),false);
  }
}

bool SmartHandController::menuSetReverse(uint8_t &axis)
{
  bool reverse;
  if (!DisplayMessageLX200(readReverseLX200(axis, reverse)))
    return false;
  char text[20];
  char * string_list_micro = "Direct\nReversed";
  sprintf(text, "Rotation M%u", axis);
  uint8_t choice = display->UserInterfaceSelectionList(&buttonPad, text, (uint8_t)reverse + 1, string_list_micro);
  if (choice)
  {
    reverse = (bool)(choice - 1);
    return DisplayMessageLX200(writeReverseLX200(axis, reverse),false);
  }
  return true;
}

bool SmartHandController::menuSetBacklash(uint8_t &axis)
{
  float backlash;
  if (!DisplayMessageLX200(readBacklashLX200(axis, backlash))) return false;
  char text[20];
  sprintf(text, "Backlash Axis%u", axis);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "", &backlash, 0, 999, 3, 0, " arc-sec"))
  {
    return DisplayMessageLX200(writeBacklashLX200(axis, backlash),false);
  }
  return true;
}

bool SmartHandController::menuSetTotGear(uint8_t &axis)
{
  float totGear;
  if (!DisplayMessageLX200(readTotGearLX200(axis, totGear))) return false;
  char text[20];
  sprintf(text, "Gear M%u", axis);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "Ratio", &totGear, 1, 10000, 5, 0, ""))
  {
    return DisplayMessageLX200(writeTotGearLX200(axis, totGear), false);
  }
  return true;
}

bool SmartHandController::menuSetStepPerRot(uint8_t &axis)
{
  float stepPerRot;
  if (!DisplayMessageLX200(readStepPerRotLX200(axis, stepPerRot))) return false;
  char text[20];
  sprintf(text, "Stepper M%u", axis);
  if (display->UserInterfaceInputValueFloat(&buttonPad, text, "", &stepPerRot, 1, 400, 3, 0, " Steps"))
  {
    return DisplayMessageLX200(writeStepPerRotLX200(axis, stepPerRot),false);
  }
  return true;
}

bool SmartHandController::menuSetMicro(uint8_t &axis)
{
  uint8_t microStep;
    if (!DisplayMessageLX200(readMicroLX200(axis, microStep))) return false;
  char text[20];
  char * string_list_micro = "16 (~256)\n32\n64\n128\n256";
  sprintf(text, "Stepper M%u", axis);
  uint8_t choice = microStep - 4 + 1;
  choice = display->UserInterfaceSelectionList(&buttonPad, text, choice, string_list_micro);
  if (choice)
  {
    microStep = choice - 1 + 4;
    return DisplayMessageLX200(writeMicroLX200(axis, microStep),false);
  }
  return true;
}

bool SmartHandController::menuSetLowCurrent(uint8_t &axis)
{
  uint8_t lowCurr;
  if (!DisplayMessageLX200(readLowCurrLX200(axis, lowCurr)))
  {
    return false;
  }
  char text[20];
  sprintf(text, "Low Curr. M%u", axis);
  if (display->UserInterfaceInputValueInteger(&buttonPad, text, "", &lowCurr, 10, 200, 3, "0 mA"))
  {
    return DisplayMessageLX200(writeLowCurrLX200(axis, lowCurr),false);
  }
  return true;
}

bool SmartHandController::menuSetHighCurrent(uint8_t &axis)
{
  uint8_t highCurr;
  if (!DisplayMessageLX200(readHighCurrLX200(axis, highCurr)))
  {
    return false;
  }
  char text[20];
  sprintf(text, "High Curr. M%u", axis);
  if (display->UserInterfaceInputValueInteger(&buttonPad, text, "", &highCurr, 10, 200, 3, "0 mA"))
  {
    return DisplayMessageLX200(writeHighCurrLX200(axis, highCurr));
  }
  return true;
}

void SmartHandController::DisplayMotorSettings(uint8_t &axis)
{
  char line1[32]="";
  char line2[32]="";
  char line3[32]="";
  char line4[32]="";
  bool reverse;
  float backlash,totGear,stepPerRot;
  uint8_t microStep,lowCurr,highCurr;
  sprintf(line1, "Motor %u Settings", axis);
  if (DisplayMessageLX200(readReverseLX200(axis, reverse)))
  {
    reverse ? sprintf(line3, "Reversed Rotation") : sprintf(line3, "Direct Rotation");
  }
  if (DisplayMessageLX200(readTotGearLX200(axis, totGear)))
  {
    sprintf(line4, "Ratio: %u", (unsigned int)totGear);
  }
  DisplayLongMessage(line1, NULL, line3, line4, -1);

  line2[0] = 0;
  line3[0] = 0;
  line4[0] = 0;
  if (DisplayMessageLX200(readStepPerRotLX200(axis, stepPerRot)))
  {
    sprintf(line2, "%u Steps per Rot.", (unsigned int)stepPerRot);
  }
  if (DisplayMessageLX200(readMicroLX200(axis, microStep)))
  {
    sprintf(line3, "MicroStep: %u", (unsigned int)pow(2, microStep));
  }
  if (DisplayMessageLX200(readBacklashLX200(axis, backlash)))
  {
    sprintf(line4, "Backlash: %u sec.", (unsigned int)backlash);
  }
  DisplayLongMessage(line1, line2, line3, line4, -1);
  line2[0] = 0;
  line3[0] = 0;
  line4[0] = 0;
  if (DisplayMessageLX200(readLowCurrLX200(axis, lowCurr)))
  {
    sprintf(line3, "Low Curr. %u0 mA", (unsigned int)lowCurr);
  }
  if (DisplayMessageLX200(readHighCurrLX200(axis, highCurr)))
  {
    sprintf(line4, "High Curr. %u0 mA", (unsigned int)highCurr);
  }

  DisplayLongMessage(line1, NULL, line3, line4, -1);
}

