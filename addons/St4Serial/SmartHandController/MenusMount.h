
void SmartHandController::menuMount()
{
  current_selection_L2 = 1;
  while (current_selection_L2 != 0)
  {
    const char *string_list_Mount = "Predefined\n""Mount type\n""Motor 1\n""Motor 2";
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Mount", current_selection_L2, string_list_Mount);
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

void SmartHandController::menuMountType()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Equatorial Fork\n""German Equatorial";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Mount Type", current_selection_L3, string_list_Mount);
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

void SmartHandController::menuPredefinedMount()
{
  current_selection_L3 = 1;
  while (current_selection_L3 != 0)
  {
    const char *string_list_Mount = "Alt Mount\n""ASTRO-PHYSICS\n""Fornax\n""Losmandy\n""Sideres85\n""Sky-Watcher\n""Takahashi\n""Vixen";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, "Mount", current_selection_L3, string_list_Mount);
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

void SmartHandController::menuAltMount()
{
  current_selection_L4 = 1;
  while (current_selection_L4 != 0)
  {
    const char *string_list_Mount = "Alt 5-ADN\n""Alt 6-ADN\n""Alt 7-ADN\n";
    current_selection_L4 = display->UserInterfaceSelectionList(&buttonPad, "Alt Mount", current_selection_L4, string_list_Mount);
    //if (current_selection_L4 > 0)
    //{
    //  switch (current_selection_L4)
    //  {
    //  case 1:
    //    mountMotor[0].setWorm(250);
    //    mountMotor[1].setWorm(200);
    //    break;
    //  case 2:
    //    mountMotor[0].setWorm(220);
    //    mountMotor[1].setWorm(200);
    //    break;
    //  case 3:
    //    mountMotor[0].setWorm(270);
    //    mountMotor[1].setWorm(210);
    //    break;
    //  default:
    //    break;
    //  }
    //  DisplayMessage("Set Parameters", "for Motor 1", -1);

    //  DisplayMessage("Set Parameters", "for Motor 2", -1);

    //}
  }
}

//void SmartHandController::menuAP() {}
//void SmartHandController::menuFornax() {}
//void SmartHandController::menuLosmandy() {}
//void SmartHandController::menuSideres() {}
//void SmartHandController::menuSkyWatcher() {}
//void SmartHandController::menuTakahashi() {}
//void SmartHandController::menuVixen() {}

void SmartHandController::menuMotor(uint8_t axis)
{
  current_selection_L3 = 1;

  while (current_selection_L3 != 0)
  {
    const char *string_list_Motor = "Show Settings\nRotation\nGear\n""Steps per Rotation\n""Micro Steps\n""Backlash\n""Low Current\n""High Current";
    current_selection_L3 = display->UserInterfaceSelectionList(&buttonPad, axis == 1 ? "Motor 1" : "Motor 2", current_selection_L3, string_list_Motor);
    switch (current_selection_L3)
    {
    case 1:
      //menuDisplayMotorSettings(axis);
      break;
    case 2:
      menuSetReverse(axis);
      break;
    case 3:
      menuSetTotGear(axis);
      break;
    case 4:
      menuSetStepPerRot(axis);
      break;
    case 5:
      menuSetMicro(axis);
      break;
    case 6:
      menuSetBacklash(axis);
      break;
    case 7:
      menuSetLowCurrent(axis);
      break;
    case 8:
      menuSetHighCurrent(axis);
      break;
    default:
      break;
    }
  }
}

