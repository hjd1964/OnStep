// SyncGoto menu

void SmartHandController::menuSyncGoto(bool sync)
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0) {
    char string_list_gotoL1[60+cat_mgr.numCatalogs()*10]="";

    // build the list of star/dso catalogs
    for (int i=0; i<cat_mgr.numCatalogs(); i++) {
      cat_mgr.select(i);
      strcat(string_list_gotoL1,cat_mgr.catalogTitle()); strcat(string_list_gotoL1,"\n");
    }
    // add the normal filtering, solarsys, etc. items
    strcat(string_list_gotoL1,"^Filter^\nSolar System\nUser Catalog\nCoordinates\nHome");
    
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, sync ? "Sync" : "Goto", current_selection_L1, string_list_gotoL1);

    if (current_selection_L1 != 0) {
      if ((current_selection_L1>0) && (current_selection_L1<=cat_mgr.numCatalogs())) {
        menuCatalog(sync,current_selection_L1-1);
      } else
      switch (current_selection_L1-cat_mgr.numCatalogs()) {
        case 1:
          menuFilters();
          break;
        case 2:
          menuSolarSys(sync);
          break;
        case 3:
          menuUser(sync);
          break;
        case 4:
          menuRADec(sync);
          break;
        case 5:
        {
          boolean GotoHome=false; 
          DisplayMessage("Goto Home will", "clear the Model", 2000);
          if (display->UserInterfaceInputValueBoolean(&buttonPad, "Goto Home?", &GotoHome)) {
            if (GotoHome) {
              char cmd[5];
              sprintf(cmd, ":hX#");
              cmd[2] = sync ? 'F' : 'C';
              if (SetLX200(cmd) == LX200VALUESET) DisplayMessage(sync ? "Reset at" : "Goto", " Home Position", -1);
              // Quit Menu
              current_selection_L1 = 0;
              current_selection_L0 = 0;
            }
          }
        }
      }
    }
  }
}

void SmartHandController::menuCatalog(bool sync, int number)
{
  if (!cat_mgr.isInitialized()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(number);

  char title[20]; if (sync) strcpy(title,"Sync "); else strcpy(title,"Goto "); strcat(title,cat_mgr.catalogTitle());
  if (setCatMgrFilters()) strcat(title," Ÿ");

  if (!cat_mgr.setIndex(0)) {
    DisplayMessage(cat_mgr.catalogTitle(), "No Object", 2000);
  } else
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, title)) {
      bool  ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuSolarSys(bool sync)
{
  if (current_selection_SolarSys<1) current_selection_SolarSys=1;

  const char *string_list_SolarSyst = "Sun\nMercury\nVenus\nMars\nJupiter\nSaturn\nUranus\nNeptune\nMoon";
  current_selection_SolarSys = display->UserInterfaceSelectionList(&buttonPad, sync ? "Sync" : "Goto", current_selection_SolarSys, string_list_SolarSyst);
  
  if (current_selection_SolarSys == 0) return;
  current_selection_SolarSys > 3 ? current_selection_SolarSys : current_selection_SolarSys--;
  
  if (current_selection_SolarSys == 0)
  { 
    boolean GotoSun=false; 
    DisplayMessage("Pointing at the Sun", "can be dangerous", 2000);
    if (display->UserInterfaceInputValueBoolean(&buttonPad, "Goto Sun?", &GotoSun))
      { if (!GotoSun) return;
      }
    else return;
  }
  
  bool ok = DisplayMessageLX200(SyncGotoPlanetLX200(sync, current_selection_SolarSys),false);
  if (current_selection_SolarSys != 0 && ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
}

void SmartHandController::menuFilters()
{
  const char *string_list_SolarSyst = "Clear all filters\nBy Constellation\nBy Type";

  current_selection_L2 = 1;
  while (current_selection_L2 != 0) {
    current_selection_L2 = display->UserInterfaceSelectionList(&buttonPad, "Filter Objects", current_selection_L2, string_list_SolarSyst);

    switch (current_selection_L2) {
      case 1:
        current_selection_filter_con=1;
        current_selection_filter_type=1;
        DisplayMessage("Filters", "Cleared", 1000);
      break;
      case 2:
        menuFilterCon();
      break;
      case 3:
        menuFilterType();
      break;
    }
    if (current_selection_L2 == 0) return;
  }
}

bool SmartHandController::setCatMgrFilters()
{
  cat_mgr.filtersClear();
  cat_mgr.filterAdd(FM_ABOVE_HORIZON);
  bool extraFilterActive=false;
  if (current_selection_filter_con>1) { cat_mgr.filterAdd(FM_CONSTELLATION,current_selection_filter_con-2); extraFilterActive=true; }
  if (current_selection_filter_type>1) { cat_mgr.filterAdd(FM_OBJ_TYPE,current_selection_filter_type-2); extraFilterActive=true; }
  return extraFilterActive;
}

void SmartHandController::menuFilterCon()
{
  char string_list_fCon[1000]="";
  for (int l=0; l<89; l++) {
    if (l==0) strcat(string_list_fCon,"All"); else strcat(string_list_fCon,cat_mgr.constellationCodeToStr(l-1));
    if (l<88) strcat(string_list_fCon,"\n");
  }
  int last_selection_filter_con = current_selection_filter_con;
  current_selection_filter_con = display->UserInterfaceSelectionList(&buttonPad, "Filter by Con", current_selection_filter_con, string_list_fCon);
  
  if (current_selection_filter_con == 0) current_selection_filter_con=last_selection_filter_con;
}

void SmartHandController::menuFilterType()
{
  char string_list_fType[500]="";
  for (int l=0; l<22; l++) {
    if (l==0) strcat(string_list_fType,"All"); else strcat(string_list_fType,cat_mgr.objectTypeCodeToStr(l-1));
    if (l<21) strcat(string_list_fType,"\n");
  }
  int last_selection_filter_type = current_selection_filter_type;
  current_selection_filter_type = display->UserInterfaceSelectionList(&buttonPad, "Filter by Type", current_selection_filter_type, string_list_fType);
  
  if (current_selection_filter_type == 0) current_selection_filter_type=last_selection_filter_type;
}

void SmartHandController::menuUser(bool sync)
{
  char string_list_UserCatalogs[240] = "";

  // read user catalogs names into a list
  int userCatalog[15];
  int i=0;
  char temp[10];
  char temp1[500];
  for (int l=0; l<=14; l++) {
    strcpy(temp,":Lo0#"); temp[3]=l+'0';
    SetLX200(temp);
    SetLX200(":L$#");
    GetLX200(":LI#",temp1);
    
    int len = strlen(temp1);
    if (len>4) temp1[len-5]=0;
    if (temp1[0]==0) continue;

    if (l!=0) strcat(string_list_UserCatalogs,"\n");
    strcat(string_list_UserCatalogs,(char*)(&temp1[1]));
    userCatalog[i++]=l;
  }

  if (i>0) {
    if (current_selection_UserCatalog<1) current_selection_UserCatalog=1;
    int last_selection_UserCatalog = current_selection_UserCatalog;
    while (current_selection_UserCatalog>0) {
      current_selection_UserCatalog = display->UserInterfaceSelectionList(&buttonPad, "Select User Cat", current_selection_UserCatalog, string_list_UserCatalogs);

      if (current_selection_UserCatalog>0) {
        // select user catalog
        strcpy(temp,":Lo0#"); temp[3]=userCatalog[current_selection_UserCatalog-1]+'0';
        SetLX200(temp);
    
        // show the catalog objects
        if (display->UserInterfaceUserCatalog(&buttonPad, sync ? "Sync User Item" : "Goto User Item")) {
          bool ok = DisplayMessageLX200(SetLX200(":LIG#"));
          if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; current_selection_UserCatalog = 0; } // Quit all menus, selection succeeded
        }
      }
    }
    if (current_selection_UserCatalog==0) current_selection_UserCatalog=last_selection_UserCatalog;
    
  } else {
    DisplayMessage("Select User Cat", "No Catalogs", 2000);
    current_selection_L1 = 0;
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
