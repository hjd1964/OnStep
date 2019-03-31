// SyncGoto menu

void SmartHandController::menuSyncGoto(bool sync)
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0) {
    const char *string_list_gotoL1 = "Star\nMessier\nCaldwell\nHerschel\n^Filter^\nSolar System\nUser Catalog\nCoordinates\nHome";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, sync ? "Sync" : "Goto", current_selection_L1, string_list_gotoL1);
    switch (current_selection_L1) {
      case 1:
        menuStar(sync);
        break;
      case 2:
        menuMessier(sync);
        break;
      case 3:
	      menuCaldwell(sync);
      break;
      case 4:
        menuHerschel(sync);
        break;
      case 5:
        menuFilters();
        break;
      case 6:
        menuSolarSys(sync);
        break;
      case 7:
        menuUser(sync);
        break;
      case 8:
        menuRADec(sync);
        break;
      case 9:
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

void SmartHandController::menuStar(bool sync)
{
  if (!cat_mgr.isInitialized()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(STAR);
  setCatMgrFilters();
  if (!cat_mgr.setIndex(0)) 
    DisplayMessage(sync ? "Sync Star" : "Goto Star", "No Stars", 2000);
  else
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Star" : "Goto Star")) {
      bool  ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuMessier(bool sync)
{
  if (!cat_mgr.isInitialized()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(MESSIER);
  setCatMgrFilters();
  if (!cat_mgr.setIndex(0)) 
    DisplayMessage(sync ? "Sync Messier" : "Goto Messier", "No Objects", 2000);
  else
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Messier" : "Goto Messier")) {
      bool ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuCaldwell(bool sync)
{
  if (!cat_mgr.isInitialized()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(CALDWELL);
  setCatMgrFilters();
  if (!cat_mgr.setIndex(0)) 
    DisplayMessage(sync ? "Sync Caldwell" : "Goto Caldwell", "No Objects", 2000);
  else
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Caldwell" : "Goto Caldwell")) {
      bool ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
      if (ok) { current_selection_L1 = 0; current_selection_L0 = 0; } // Quit Menu
    }
  }
}

void SmartHandController::menuHerschel(bool sync)
{
  if (!cat_mgr.isInitialized()) { cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0()); }
  cat_mgr.select(HERSCHEL);
  setCatMgrFilters();
  if (!cat_mgr.setIndex(0)) 
    DisplayMessage(sync ? "Sync Herschel" : "Goto Herschel", "No Objects", 2000);
  else
  if (cat_mgr.isInitialized()) {
    if (display->UserInterfaceCatalog(&buttonPad, sync ? "Sync Herschel" : "Goto Herschel")) {
      bool ok = DisplayMessageLX200(SyncGotoCatLX200(sync), false);
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

void SmartHandController::setCatMgrFilters()
{
  cat_mgr.filtersClear();
  cat_mgr.filterAdd(FM_ABOVE_HORIZON);
  if (current_selection_filter_con>1) cat_mgr.filterAdd(FM_CONSTELLATION,current_selection_filter_con-2);
  if (current_selection_filter_type>1) cat_mgr.filterAdd(FM_OBJ_TYPE,current_selection_filter_type-2);
}

// Constellation abbreviation, alphabetical order
const char* Txt_Constellation[89] = {
  "All",
  "And",
  "Ant",
  "Aps",
  "Aql",
  "Aqr",
  "Ara",
  "Ari",
  "Aur",
  "Boo",
  "CMa",
  "CMi",
  "CVn",
  "Cae",
  "Cam",
  "Cap",
  "Car",
  "Cas",
  "Cen",
  "Cep",
  "Cet",
  "Cha",
  "Cir",
  "Cnc", //  22
  "Col", //  23
  "Com", //  24
  "CrA", //  25
  "CrB", //  26
  "Crt", //  27
  "Cru", //  28
  "Crv", //  29
  "Cyg", //  30
  "Del", //  31
  "Dor", //  32
  "Dra", //  33
  "Equ", //  34
  "Eri", //  35
  "For", //  36
  "Gem", //  37
  "Gru", //  38
  "Her", //  39
  "Hor", //  40
  "Hya", //  41
  "Hyi", //  42
  "Ind", //  43
  "LMi", //  44
  "Lac", //  45
  "Leo", //  46
  "Lep", //  47
  "Lib", //  48
  "Lup", //  49
  "Lyn", //  50
  "Lyr", //  51
  "Men", //  52
  "Mic",
  "Mon",
  "Mus",
  "Nor",
  "Oct",
  "Oph",
  "Ori",
  "Pav",
  "Peg",
  "Per",
  "Phe",
  "Pic",
  "PsA",
  "Psc",
  "Pup",
  "Pyx",
  "Ret",
  "Scl",
  "Sco",
  "Sct",
  "Ser",
  "Sex",
  "Sge",
  "Sgr",
  "Tau",
  "Tel",
  "TrA",
  "Tri",
  "Tuc",
  "UMa",
  "UMi",
  "Vel",
  "Vir",
  "Vol",
  "Vul",
};

void SmartHandController::menuFilterCon()
{
  char string_list_fCon[1000]="";
  for (int l=0; l<89; l++) {
    strcat(string_list_fCon,Txt_Constellation[l]);
    if (l<88) strcat(string_list_fCon,"\n");
  }
  int last_selection_filter_con = current_selection_filter_con;
  current_selection_filter_con = display->UserInterfaceSelectionList(&buttonPad, "Filter by Con", current_selection_filter_con, string_list_fCon);
  
  if (current_selection_filter_con == 0) current_selection_filter_con=last_selection_filter_con;
}

const char * Txt_Object_Types[22] = {
  "All",
  "Galaxy",
  "Open Cluster",
  "Star",
  "Double Star",
  "Other",
  "Galaxy Pair",
  "Galaxy Triplet",
  "Galaxy Group",
  "Globular Cluster",
  "Planetary Nebula",
  "Nebula",
  "Hii Region",
  "Cluster + Nebula",
  "Asterism",
  "Reflectn Nebula",
  "SuperNova Rmnnt",
  "Emission Nebula",
  "NonExist",
  "Nova",
  "Duplicate",
  "Dark Nebula",
};
void SmartHandController::menuFilterType()
{
  char string_list_fType[500]="";
  for (int l=0; l<22; l++) {
    strcat(string_list_fType,Txt_Object_Types[l]);
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
