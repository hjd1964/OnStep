
void SmartHandController::menuSyncGoto(bool sync)
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0) {
    const char *string_list_gotoL1 = "Messier\nStar\nSolar System\nHerschel\nCoordinates\nHome";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, sync ? "Sync" : "Goto", current_selection_L1, string_list_gotoL1);
    switch (current_selection_L1) {
      case 1:
        menuMessier(sync);
        break;
      case 2:
        menuStar(sync);
        break;
      case 3:
        menuSolarSys(sync);
        break;
      case 4:
        menuHerschel(sync);
        break;
      case 5:
        menuRADec(sync);
        break;
      case 6:
      {
        char cmd[5];
        sprintf(cmd, ":hX#");
        cmd[2] = sync ? 'F' : 'C';
        if (SetLX200(cmd) == LX200VALUESET) DisplayMessage(sync ? "Reset at" : "Goto", " Home Position", -1);
        // Quit Menu
        current_selection_L1 = 0;
        current_selection_L0 = 0;
      }
      break;
      default:
      break;
    }
  }
}

void SmartHandController::menuMessier(bool sync)
{
  if (!cat_mgr.canFilter()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(MESSIER);
  cat_mgr.filter(FM_ABOVE_HORIZON);
  cat_mgr.setIndex(0);
  
  if (cat_mgr.canFilter()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Messier" : "Goto Messier")) {
      bool ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuStar(bool sync)
{
  if (!cat_mgr.canFilter()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(STAR);
  cat_mgr.filter(FM_ABOVE_HORIZON);
  cat_mgr.setIndex(0);
  if (cat_mgr.canFilter()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Star" : "Goto Star")) {
      bool  ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuSolarSys(bool sync)
{
  if (current_selection_SolarSys<1) current_selection_SolarSys=1;

  const char *string_list_SolarSyst = "Sun\nMercury\nVenus\nMars\nJupiter\nSaturn\nUranus\nNeptun\nMoon\n";
  current_selection_SolarSys = display->UserInterfaceSelectionList(&buttonPad, sync ? "Sync" : "Goto", current_selection_SolarSys, string_list_SolarSyst);
  
  if (current_selection_SolarSys == 0) return;
  current_selection_SolarSys > 3 ? current_selection_SolarSys : current_selection_SolarSys--;
  
  bool ok = DisplayMessageLX200(SyncGotoPlanetLX200(sync, current_selection_SolarSys),false);
  if (current_selection_SolarSys != 0 && ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
}

void SmartHandController::menuHerschel(bool sync)
{
  if (!cat_mgr.canFilter()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(HERSCHEL);
  cat_mgr.filter(FM_ABOVE_HORIZON);
  cat_mgr.setIndex(0);
  
  if (cat_mgr.canFilter()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Herschel" : "Goto Herschel")) {
      bool ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuRADec(bool sync)
{
  if (display->UserInterfaceInputValueRA(&buttonPad, &angleRA))
  {
//    uint8_t vr1, vr2, vr3, vd2, vd3;
//    short vd1;
//    gethms(angleRA, vr1, vr2, vr3);
    float fR;
    secondsToFloat(angleRA,fR);
    if (display->UserInterfaceInputValueDec(&buttonPad, &angleDEC))
    {
      float fD;
      secondsToFloat(angleDEC,fD);
//      getdms(angleDEC, vd1, vd2, vd3);
//      bool ok = DisplayMessageLX200(SyncGotoLX200(sync, vr1, vr2, vr3, vd1, vd2, vd3));
      bool ok = DisplayMessageLX200(SyncGotoLX200(sync, fR, fD));
      if (ok)
      {
        // Quit Menu
        current_selection_L1 = 0;
        current_selection_L0 = 0;
      }
    }
  }
}

