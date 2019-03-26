// Alignment menu

void SmartHandController::menuAlignment()
{
  int maxAlignStars, thisStar, numStars;
  bool alignInProgress=false;

  if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=3; thisStar=1; numStars=1; }
  if ((thisStar>0) && (thisStar<=numStars)) alignInProgress=true;

  telInfo.aliMode = Telescope::ALIM_OFF;
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    int starsForAlign=0;
    bool showAlign=false;
    bool clearAlign=false;
    bool resetAlign=false;
  
    char string_list_AlignmentL1[120];
    if (alignInProgress) {
      strcpy(string_list_AlignmentL1,"Resume Align\n""Show Corr\n""Clear Corr\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1:
          switch (numStars) {
            case 1: telInfo.aliMode = Telescope::ALIM_ONE; break;
            case 2: telInfo.aliMode = Telescope::ALIM_TWO; break;
            case 3: telInfo.aliMode = Telescope::ALIM_THREE; break;
            case 4: telInfo.aliMode = Telescope::ALIM_FOUR; break;
            case 5: telInfo.aliMode = Telescope::ALIM_FIVE; break;
            case 6: telInfo.aliMode = Telescope::ALIM_SIX; break;
            case 7: telInfo.aliMode = Telescope::ALIM_SEVEN; break;
            case 8: telInfo.aliMode = Telescope::ALIM_EIGHT; break;
            case 9: telInfo.aliMode = Telescope::ALIM_NINE; break;
          }
          current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
        break;
        case 2: showAlign=true; break;
        case 3: clearAlign=true; break;
        case 4: resetAlign=true; break;
      }
    } else
    if ((maxAlignStars==1) || (maxAlignStars==2)) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break;
        case 2: resetAlign=true; break;
      }
    } else
    if (maxAlignStars==3) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""2-Star Align\n""3-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=2; break; case 3: starsForAlign=3; break;
        case 4: showAlign=true; break;  case 5: clearAlign=true; break; case 6: resetAlign=true; break;
      }
    } else
    if ((maxAlignStars==4) || (maxAlignStars==5)) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""4-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=4; break;
        case 4: showAlign=true; break;  case 5: clearAlign=true; break; case 6: resetAlign=true; break;
      }
    } else
    if (maxAlignStars==6) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""4-Star Align\n""6-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=4; break; case 4: starsForAlign=6; break;
        case 5: showAlign=true; break;  case 6: clearAlign=true; break; case 7: resetAlign=true; break;
      }
    } else
    if (maxAlignStars==9) {
      strcpy(string_list_AlignmentL1,"1-Star Align\n""3-Star Align\n""6-Star Align\n""9-Star Align\n""Show Model\n""Clear Model\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1: starsForAlign=1; break; case 2: starsForAlign=3; break; case 3: starsForAlign=6; break; case 4: starsForAlign=9; break;
        case 5: showAlign=true; break;  case 6: clearAlign=true; break; case 7: resetAlign=true; break;
      }
    }
  
    // handle misc. resulting actions
    if (showAlign) {
      char r2[20]=""; char r3[20]=""; char r4[20]=""; char r5[20]=""; char r8[20]="";
      if ((GetLX200Trim(":GX02#",r2)==LX200VALUEGET) && (GetLX200Trim(":GX03#",r3)==LX200VALUEGET) && 
          (GetLX200Trim(":GX04#",r4)==LX200VALUEGET) && (GetLX200Trim(":GX05#",r5)==LX200VALUEGET) && 
          (GetLX200Trim(":GX08#",r8)==LX200VALUEGET)) {
        char s1[20]=""; strcat(s1,"PE:"); strcat(s1,r2); strcat(s1,", PZ:"); strcat(s1,r3);
        char s2[20]=""; strcat(s2,"DO (cone):"); strcat(s2,r4);
        char s3[20]=""; strcat(s3,"PD:"); strcat(s3,r5); strcat(s3,", TF:"); strcat(s3,r8);
        DisplayLongMessage("Align results (in \")", s1, s2, s3, -1);
      }
    } else  
    if (clearAlign) {
      if ((SetLX200(":SX02,0#")==LX200VALUESET) && (SetLX200(":SX03,0#")==LX200VALUESET) &&
          (SetLX200(":SX04,0#")==LX200VALUESET) && (SetLX200(":SX05,0#")==LX200VALUESET) &&
          (SetLX200(":SX06,0#")==LX200VALUESET) && (SetLX200(":SX07,0#")==LX200VALUESET) &&
          (SetLX200(":SX08,0#")==LX200VALUESET)) DisplayMessageLX200(LX200VALUESET,false); else DisplayMessageLX200(LX200SETVALUEFAILED,false);
    } else
    if (resetAlign) {
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
      if (SetLX200(":hF#") == LX200VALUESET) DisplayMessage("Reset", "Move to Home", -1);
    } else
    if (starsForAlign>0) {
      switch (starsForAlign) {
        case 1: if (SetLX200(":A1#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_ONE;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 2: if (SetLX200(":A2#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_TWO;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 3: if (SetLX200(":A3#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_THREE; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 4: if (SetLX200(":A4#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_FOUR;  else DisplayMessage("Alignment", "Failed!", -1); break;
        case 5: if (SetLX200(":A5#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_FIVE;  else DisplayMessage("Alignment", "Failed!", -1); break;
        case 6: if (SetLX200(":A6#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_SIX;   else DisplayMessage("Alignment", "Failed!", -1); break;
        case 7: if (SetLX200(":A7#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_SEVEN; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 8: if (SetLX200(":A8#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_EIGHT; else DisplayMessage("Alignment", "Failed!", -1); break;
        case 9: if (SetLX200(":A9#") == LX200VALUESET) telInfo.aliMode = Telescope::ALIM_NINE;  else DisplayMessage("Alignment", "Failed!", -1); break;
      }
      if (SetLX200(":R7#") == LX200VALUESET) { DisplayMessage("Guide Rate", "48X Set", 1000);  activeGuideRate=8; }
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
    }
  }

  if (telInfo.aliMode!=Telescope::ALIM_OFF) {
    if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=3; thisStar=1; numStars=1; }
    switch (thisStar) {
      case 1: telInfo.align = Telescope::ALI_SELECT_STAR_1; break;
      case 2: telInfo.align = Telescope::ALI_SELECT_STAR_2; break;
      case 3: telInfo.align = Telescope::ALI_SELECT_STAR_3; break;
      case 4: telInfo.align = Telescope::ALI_SELECT_STAR_4; break;
      case 5: telInfo.align = Telescope::ALI_SELECT_STAR_5; break;
      case 6: telInfo.align = Telescope::ALI_SELECT_STAR_6; break;
      case 7: telInfo.align = Telescope::ALI_SELECT_STAR_7; break;
      case 8: telInfo.align = Telescope::ALI_SELECT_STAR_8; break;
      case 9: telInfo.align = Telescope::ALI_SELECT_STAR_9; break;
      default: telInfo.align = Telescope::ALI_SELECT_STAR_1; break;
    }
  }
}
