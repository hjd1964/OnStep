// SyncGoto menu

MENU_RESULT SmartHandController::menuSyncGoto(bool sync)
{
  static int current_selection = 1;

  while (true) {
    // build the list of star/dso catalogs
    char string_list_gotoL1[60+cat_mgr.numCatalogs()*10]="";
    int  catalog_index[cat_mgr.numCatalogs()];
    int  catalog_index_count=0;
    char title[16]="";
    char thisSubmenu[16]="";
    char lastSubmenu[16]="";
    for (int i=1; i<=cat_mgr.numCatalogs(); i++) {
      cat_mgr.select(i-1);
      strcpy(title,cat_mgr.catalogTitle());
      strcpy(lastSubmenu,thisSubmenu);
      strcpy(thisSubmenu,cat_mgr.catalogSubMenu());

      bool duplicate=false;
      if (strlen(thisSubmenu)>0) {
        if ((strlen(thisSubmenu)==strlen(lastSubmenu)) && (strstr(thisSubmenu,lastSubmenu))) duplicate=true;
      }
      if ((!duplicate) && (strlen(thisSubmenu)>0)) { strcpy(title,thisSubmenu); strcat(title,">"); }

      // add it to the list (if it isn't a duplicate)
      if (!duplicate) {
        strcat(string_list_gotoL1,title); strcat(string_list_gotoL1,"\n");
        if (strlen(thisSubmenu)==0) catalog_index[catalog_index_count++]=i; else catalog_index[catalog_index_count++]=-i;
      }
    }
    // add the normal filtering, solarsys, etc. items
    strcat(string_list_gotoL1,L_SG_SOLSYS ">" "\n");
    if (sync) strcat(string_list_gotoL1,L_SG_HERE ">"); else strcat(string_list_gotoL1,L_SG_USER ">" "\n" L_SG_FILTERS "\n" L_SG_COORDS "\n" L_SG_HOME);

    int selection = display->UserInterfaceSelectionList(&buttonPad, sync ? L_SG_SYNC : L_SG_GOTO, current_selection, string_list_gotoL1);
    if (selection == 0) return MR_CANCEL;
    current_selection=selection;

    if (current_selection<=catalog_index_count) {
      int catalogNum=catalog_index[current_selection-1];
      if (catalogNum>=0) { if (menuCatalog(sync,catalogNum-1)==MR_QUIT) return MR_QUIT; } else { if (subMenuSyncGoto(sync,(-catalogNum)-1)==MR_QUIT) return MR_QUIT; }
    } else
    switch (current_selection-catalog_index_count) {
      case 1:
        if (menuSolarSys(sync)==MR_QUIT) return MR_QUIT;
        break;
      case 2:
        if (sync) {
          bool isYes=true;
          if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SG_HERE "?", &isYes)) if (isYes) { DisplayMessageLX200(SetLX200(":CS#"),false); return MR_QUIT; }
        } else {
          if (menuUser(sync)==MR_QUIT) return MR_QUIT;
        }
        break;
      case 3:
        menuFilters();
        break;
      case 4:
        if (menuRADec(sync)==MR_QUIT) return MR_QUIT;
        break;
      case 5:
      {
        boolean GotoHome=false; 
        DisplayMessage(L_SG_HOME1, L_SG_HOME2, 2000);
        if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SG_HOME3 "?", &GotoHome)) {
          if (GotoHome) {
            char cmd[5];
            sprintf(cmd, ":hX#");
            cmd[2] = sync ? 'F' : 'C';
            if (SetLX200(cmd) == LX200VALUESET) DisplayMessage(sync ? L_SG_HOME4 : L_SG_HOME5, " " L_SG_HOME6, -1);
            return MR_QUIT;
          }
        }
      }
    }
  }
}

MENU_RESULT SmartHandController::subMenuSyncGoto(char sync, int subMenuNum)
{
  static uint8_t current_selection[64] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  while (true) {
    // build the list of star/dso catalogs
    char string_list_gotoL1[60+cat_mgr.numCatalogs()*10]="";
    int  catalog_index[cat_mgr.numCatalogs()];
    int  catalog_index_count=0;
    char title[16]="";
    char lastSubmenu[16]="";
    char thisSubmenu[16]="";
    // build the list of subMenu catalogs
    for (int i=subMenuNum; i<cat_mgr.numCatalogs(); i++) {
      cat_mgr.select(i);
      strcpy(title,cat_mgr.catalogTitle());
      if (i==subMenuNum) strcpy(lastSubmenu,cat_mgr.catalogSubMenu()); else strcpy(lastSubmenu,thisSubmenu);
      strcpy(thisSubmenu,cat_mgr.catalogSubMenu());

      bool duplicate=false;
      if (strlen(thisSubmenu)>0) { 
        if ((strlen(thisSubmenu)==strlen(lastSubmenu)) && (strstr(thisSubmenu,lastSubmenu))) duplicate=true; else break;
      }

      // add it to the list (if it isn't a duplicate)
      if (duplicate) {
        if (i!=subMenuNum) strcat(string_list_gotoL1,"\n"); strcat(string_list_gotoL1,title);
        catalog_index[catalog_index_count++]=i;
      }
    }

    if (sync) strcpy(title,L_SG_SYNC " "); else strcpy(title,L_SG_GOTO " "); strcat(title, lastSubmenu);
    int selection = display->UserInterfaceSelectionList(&buttonPad, title, current_selection[subMenuNum], string_list_gotoL1);
    if (selection == 0) return MR_CANCEL;
    current_selection[subMenuNum]=selection;
    
    if (current_selection[subMenuNum]<=cat_mgr.numCatalogs()) {
      int catalogNum=catalog_index[current_selection[subMenuNum]-1];
      if ((catalogNum>=0) && (catalogNum<cat_mgr.numCatalogs())) {
        if (menuCatalog(sync,catalogNum)==MR_QUIT) return MR_QUIT;
      }
    }
  }
}

MENU_RESULT SmartHandController::menuCatalog(bool sync, int number)
{
  cat_mgr.setLat(telInfo.getLat()); cat_mgr.setLstT0(telInfo.getLstT0());
  cat_mgr.select(number);

  char title[20]; if (sync) strcpy(title,L_SG_SYNC " "); else strcpy(title,L_SG_GOTO " "); strcat(title,cat_mgr.catalogTitle());
  setCatMgrFilters(); if (cat_mgr.hasActiveFilter()) strcat(title," \xa5");

  if (cat_mgr.isInitialized()) {
    if (cat_mgr.setIndex(cat_mgr.getIndex())) {
      if (display->UserInterfaceCatalog(&buttonPad, title)) {
        if (DisplayMessageLX200(SyncGotoCatLX200(sync), false)) return MR_QUIT;
      }
    } else DisplayMessage(cat_mgr.catalogTitle(), L_SG_NO_OBJECT, 2000);
  } else DisplayMessage(cat_mgr.catalogTitle(), L_SG_NO_INIT "?", 2000);
  return MR_CANCEL;
}

MENU_RESULT SmartHandController::menuSolarSys(bool sync)
{
  static int current_selection = 1;
  if (current_selection<1) current_selection=1;

  const char *string_list_SolarSyst = L_SG_SUN "\n" L_SG_MER "\n" L_SG_VEN "\n" L_SG_MAR "\n" L_SG_JUP "\n" L_SG_SAT "\n" L_SG_URA "\n" L_SG_NEP "\n" L_SG_MOON;
  current_selection = display->UserInterfaceSelectionList(&buttonPad, sync ? L_SG_SSOL : L_SG_GSOL, current_selection, string_list_SolarSyst);
  if (current_selection == 0) return MR_CANCEL;

  if (current_selection>3) current_selection++;
  if (current_selection == 1)
  { 
    DisplayMessage(L_SG_SOL_WARN1, L_SG_SOL_WARN2, 2000);
    boolean GotoSun=false;
    if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SG_GSUN "?", &GotoSun)) { if (!GotoSun) return MR_CANCEL; } else return MR_CANCEL;
  }

  if (DisplayMessageLX200(SyncGotoPlanetLX200(sync, current_selection-1),false)) return MR_QUIT;
  return MR_CANCEL;
}

MENU_RESULT SmartHandController::menuUser(bool sync)
{
  static int current_selection_UserCatalog = 1;
  if (current_selection_UserCatalog<1) current_selection_UserCatalog=1;

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

  // no catalogs found, just exit
  if (i<=0) { DisplayMessage(L_SG_USER_MSG1, L_SG_USER_MSG2, 2000); return MR_OK; }

  int last_selection_UserCatalog = current_selection_UserCatalog;
  while (true) {
    current_selection_UserCatalog = display->UserInterfaceSelectionList(&buttonPad, sync ? L_SG_SYNC_USER : L_SG_GOTO_USER, current_selection_UserCatalog, string_list_UserCatalogs);
    if (current_selection_UserCatalog==0) { current_selection_UserCatalog=last_selection_UserCatalog; return MR_CANCEL; }

    // select this user catalog
    strcpy(temp,":Lo0#"); temp[3]=userCatalog[current_selection_UserCatalog-1]+'0';
    SetLX200(temp);

    // show the catalog objects
    if (display->UserInterfaceUserCatalog(&buttonPad, sync ? L_SG_SYNC_USER_ITEM : L_SG_GOTO_USER_ITEM)) {
      if (DisplayMessageLX200(SetLX200(":LIG#"))) return MR_QUIT;
    }
  }
}

MENU_RESULT SmartHandController::menuFilters()
{
  static int current_selection = 1;
  current_selection = 1;
  while (true) {
    char string_list_Filters[200] = L_SG_FILT_RESET;
    char s[8];
    if (current_selection_filter_above) strcpy(s,"\xb7"); else strcpy(s,"");
    strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_HOR); strcat(string_list_Filters,s);
    if (current_selection_filter_con>1) strcpy(s,"\xb7"); else strcpy(s,"");
    strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_CON); strcat(string_list_Filters,s);
    if (current_selection_filter_type>1) strcpy(s,"\xb7"); else strcpy(s,"");
    strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_TYP); strcat(string_list_Filters,s);
    if (current_selection_filter_byMag>1) strcpy(s,"\xb7"); else strcpy(s,"");
    strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_MAG); strcat(string_list_Filters,s);
    if (current_selection_filter_nearby>1) strcpy(s,"\xb7"); else strcpy(s,"");
    strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_NEAR); strcat(string_list_Filters,s);
    if (cat_mgr.hasVarStarCatalog()) {
      if (current_selection_filter_varmax>1) strcpy(s,"\xb7"); else strcpy(s,"");
      strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_VAR_MAX); strcat(string_list_Filters,s);
    }
    if (cat_mgr.hasDblStarCatalog()) {
      if (current_selection_filter_dblmin>1) strcpy(s,"\xb7"); else strcpy(s,"");
      strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_DBL_MIN); strcat(string_list_Filters,s);
      if (current_selection_filter_dblmax>1) strcpy(s,"\xb7"); else strcpy(s,"");
      strcat(string_list_Filters,"\n"); strcat(string_list_Filters,s); strcat(string_list_Filters,L_SG_FILT_DBL_MAX); strcat(string_list_Filters,s);
    }
    current_selection = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_ALLOW, current_selection, string_list_Filters);
    if (current_selection == 0) return MR_CANCEL;
    
    switch (current_selection) {
      case 1:
        current_selection_filter_above=true;
        current_selection_filter_con=1;
        current_selection_filter_type=1;
        current_selection_filter_byMag=1;
        current_selection_filter_nearby=1;
        current_selection_filter_dblmin=1;
        current_selection_filter_dblmax=1;
        current_selection_filter_varmax=1;
        DisplayMessage(L_SG_FILT_MSG1, L_SG_FILT_MSG2, 1000);
      break;
      case 2:
        if (display->UserInterfaceInputValueBoolean(&buttonPad, L_SG_FILT_MSG3 "?", &current_selection_filter_above)) {
          if (current_selection_filter_above) DisplayMessage(L_SG_FILTER, L_ON, 1000); else DisplayMessage(L_SG_FILTER, L_OFF, 1000);
        }
      break;
      case 3:
        return menuFilterCon();
      break;
      case 4:
        return menuFilterType();
      break;
      case 5:
        return menuFilterByMag();
      break;
      case 6:
        return menuFilterNearby();
      break;
      case 7:
        if (cat_mgr.hasVarStarCatalog()) return menuFilterVarMaxPer(); else { if (cat_mgr.hasDblStarCatalog()) return menuFilterDblMinSep(); }
      break;
      case 8:
        if (cat_mgr.hasVarStarCatalog()) { if (cat_mgr.hasDblStarCatalog()) return menuFilterDblMinSep(); } else { if (cat_mgr.hasDblStarCatalog()) return menuFilterDblMaxSep(); }
      break;
      case 9:
        if (cat_mgr.hasVarStarCatalog()) { if (cat_mgr.hasDblStarCatalog()) return menuFilterDblMaxSep(); }
      break;
    }
  }
}

void SmartHandController::setCatMgrFilters()
{
  static bool lastTeleEquIsSet=false;
  cat_mgr.filtersClear();
  
  if (current_selection_filter_above)   cat_mgr.filterAdd(FM_ABOVE_HORIZON);
  if (current_selection_filter_con>1)   cat_mgr.filterAdd(FM_CONSTELLATION,current_selection_filter_con-2);
  if (current_selection_filter_type>1)  cat_mgr.filterAdd(FM_OBJ_TYPE,current_selection_filter_type-2);
  if (current_selection_filter_byMag>1) cat_mgr.filterAdd(FM_BY_MAG,current_selection_filter_byMag-2);
  if (current_selection_filter_nearby>1) {
    if (lastTeleEquIsSet) {
      cat_mgr.filterAdd(FM_NEARBY,current_selection_filter_nearby-2);
    } else
    {
      double r,d;
      if (telInfo.getRA(r) && telInfo.getDec(d)) {
        cat_mgr.setLastTeleEqu(r,d);
        lastTeleEquIsSet=true;
        cat_mgr.filterAdd(FM_NEARBY,current_selection_filter_nearby-2);
      } else current_selection_filter_nearby=1;
    }
  } else lastTeleEquIsSet=false;
  if (current_selection_filter_dblmin>1) cat_mgr.filterAdd(FM_DBL_MIN_SEP,current_selection_filter_dblmin-2);
  if (current_selection_filter_dblmax>1) cat_mgr.filterAdd(FM_DBL_MAX_SEP,current_selection_filter_dblmax-2);
  if (current_selection_filter_varmax>1) cat_mgr.filterAdd(FM_VAR_MAX_PER,current_selection_filter_varmax-2);
}

MENU_RESULT SmartHandController::menuFilterCon()
{
  char string_list_fCon[1000]="";
  for (int l=0; l<89; l++) {
    if (l==0) strcat(string_list_fCon,L_ALL); else strcat(string_list_fCon,cat_mgr.constellationCodeToStr(l-1));
    if (l<88) strcat(string_list_fCon,"\n");
  }
  int last_selection_filter_con = current_selection_filter_con;
  current_selection_filter_con = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_CON, current_selection_filter_con, string_list_fCon);
  if (current_selection_filter_con == 0) { current_selection_filter_con=last_selection_filter_con; return MR_CANCEL; }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterType()
{
  char string_list_fType[500]="";
  for (int l=0; l<22; l++) {
    if (l==0) strcat(string_list_fType,L_ALL); else strcat(string_list_fType,cat_mgr.objectTypeCodeToStr(l-1));
    if (l<21) strcat(string_list_fType,"\n");
  }
  int last_selection_filter_type = current_selection_filter_type;
  current_selection_filter_type = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_TYPE, current_selection_filter_type, string_list_fType);
  if (current_selection_filter_type == 0) { current_selection_filter_type=last_selection_filter_type; return MR_CANCEL; }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterByMag()
{
  const char* string_list_fMag=L_ALL "\n" "10th 1.6\"/40mm" "\n" "12th   3\"/ 80mm" "\n" "13th  4\"/100mm" "\n" "14th  7\"/180mm" "\n" "15th 12\"/300mm" "\n" "16th 17\"/430mm" "\n" "17th 22\"/560mm";
  int last_selection_filter_byMag = current_selection_filter_byMag;

  current_selection_filter_byMag = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_MAG, current_selection_filter_byMag, string_list_fMag);
  if (current_selection_filter_byMag == 0) { current_selection_filter_byMag=last_selection_filter_byMag; return MR_CANCEL; }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterNearby()
{
  const char* string_list_fNearby=L_OFF "\n" L_WITHIN "  1°" "\n" L_WITHIN "  5°" "\n" L_WITHIN " 10°" "\n" L_WITHIN " 15°";
  int last_selection_filter_nearby = current_selection_filter_nearby;
  current_selection_filter_nearby = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_NEAR, current_selection_filter_nearby, string_list_fNearby);
  if (current_selection_filter_nearby == 0) { current_selection_filter_nearby=last_selection_filter_nearby; return MR_CANCEL; }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterDblMinSep()
{
  const char* string_list_fDblMin="Off" "\n" ">= 0.2\"" "\n" ">= 0.5\"" "\n" ">= 1.0\"" "\n" ">= 1.5\"" "\n" ">= 2.0\"" "\n" ">= 3.0\"" "\n" ">= 5.0\"" "\n" ">= 10\"" "\n" ">= 20\"" "\n" ">= 50\"";
  int last_selection_filter_dblmin = current_selection_filter_dblmin;

  while (true) {
    current_selection_filter_dblmin = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_SEP_MIN, current_selection_filter_dblmin, string_list_fDblMin);
    if (current_selection_filter_dblmin == 0) { current_selection_filter_dblmin=last_selection_filter_dblmin; return MR_CANCEL; }
    
    if (current_selection_filter_dblmin<=1) break;                               // abort or inactive
    if (current_selection_filter_dblmax<=1) break;                               // any minimum is ok
    if (current_selection_filter_dblmin<=current_selection_filter_dblmax) break; // minimum is below max, all is well exit
    DisplayMessage(L_SG_SEP_MIN_MSG1, L_SG_SEP_MIN_MSG2, 2000);                  // provide a hint
    current_selection_filter_dblmin=current_selection_filter_dblmax;             // 
  }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterDblMaxSep()
{
  const char* string_list_fDblMax="Off" "\n" "<= 0.5\"" "\n" "<= 1.0\"" "\n" "<= 1.5\"" "\n" "<= 2.0\"" "\n" "<= 3.0\"" "\n" "<= 5.0\"" "\n" "<= 10\"" "\n" "<= 20\"" "\n" "<= 50\"" "\n" "<= 100\"";
  int last_selection_filter_dblmax = current_selection_filter_dblmax;

  while (true) {
    current_selection_filter_dblmax = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_SEP_MAX, current_selection_filter_dblmax, string_list_fDblMax);
    if (current_selection_filter_dblmax == 0) { current_selection_filter_dblmax=last_selection_filter_dblmax; return MR_CANCEL; }
    if (current_selection_filter_dblmax<=1) break;                               // abort or inactive
    if (current_selection_filter_dblmin<=1) break;                               // any maximum is ok
    if (current_selection_filter_dblmax>=current_selection_filter_dblmin) break; // maximum is above min, all is well exit
    DisplayMessage(L_SG_SEP_MAX_MSG1, L_SG_SEP_MAX_MSG2, 2000);                  // provide a hint
    current_selection_filter_dblmax=current_selection_filter_dblmin;             // 
  }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuFilterVarMaxPer()
{
  const char* string_list_fVarMax=L_OFF "\n" " <= 0.5 " L_DAYS "\n" "<= 1.0 " L_DAYS "\n" "<= 2.0 " L_DAYS "\n" "<= 5.0 " L_DAYS "\n" "<= 10 " L_DAYS "\n" "<= 20 " L_DAYS "\n" "<= 50 " L_DAYS "\n" "<= 100 " L_DAYS;
  int last_selection_filter_varmax = current_selection_filter_varmax;
  current_selection_filter_varmax = display->UserInterfaceSelectionList(&buttonPad, L_SG_FILT_BY_PER_MAX, current_selection_filter_varmax, string_list_fVarMax);
  if (current_selection_filter_varmax == 0) { current_selection_filter_dblmax=last_selection_filter_varmax; return MR_CANCEL; }
  return MR_OK;
}

MENU_RESULT SmartHandController::menuRADec(bool sync)
{
  if (display->UserInterfaceInputValueRA(&buttonPad, &angleRA))
  {
    float fR;
    secondsToFloat(angleRA,fR);
    if (display->UserInterfaceInputValueDec(&buttonPad, &angleDEC))
    {
      float fD;
      secondsToFloat(angleDEC,fD);
      if (DisplayMessageLX200(SyncGotoLX200(sync, fR, fD))) return MR_QUIT;
    }
  }
  return MR_CANCEL;
}
