// Alignment menu

void SmartHandController::menuAlignment()
{
  int maxAlignStars, thisStar, numStars;
  bool alignInProgress=false;

  if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=1; thisStar=1; numStars=1; }
  if (maxAlignStars > 9 || numStars > 9)  { maxAlignStars=1; thisStar=1; numStars=1; }
  if (thisStar > 0 && thisStar <= numStars) alignInProgress=true;

  telInfo.aliMode = Telescope::ALIM_OFF;
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    int starsForAlign=0;
    bool showAlign=false;
    bool clearAlign=false;
    bool resetAlign=false;
    bool refinePA=false;
  
    char string_list_AlignmentL1[200];
    if (alignInProgress) {
      strcpy(string_list_AlignmentL1,L_ALGN_RESUME "\n" L_ALGN_SHOW_CORR "\n" L_ALGN_CLEAR_CORR "\n" L_ALGN_RESET_HOME);
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, L_ALIGNMENT, current_selection_L1, string_list_AlignmentL1);
      switch (current_selection_L1) {
        case 1:
          telInfo.aliMode = (Telescope::AlignMode)numStars;
          current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
        break;
        case 2: showAlign=true; break;
        case 3: clearAlign=true; break;
        case 4: resetAlign=true; break;
      }
    } 
    else
    {
      strcpy(string_list_AlignmentL1,"");
      char s[20];
      for (int i=1; i<=maxAlignStars; i++) {
        sprintf(s,"%d-" L_ALGN_STAR_ALIGN "\n",i);
        strcat(string_list_AlignmentL1,s);
      }
      strcat(string_list_AlignmentL1,L_ALGN_SHOW_MODEL "\n" L_ALGN_CLEAR_MODEL "\n" L_ALGN_RESET_HOME);
      if (!telInfo.isMountAltAz()) {
        sprintf(s,"\nRefine PA");
        strcat(string_list_AlignmentL1,s);
      }
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, L_ALIGNMENT, current_selection_L1, string_list_AlignmentL1);
      if (current_selection_L1<=maxAlignStars) {
        starsForAlign=current_selection_L1;
      } else {
        if (current_selection_L1==maxAlignStars+1) showAlign=true;
        if (current_selection_L1==maxAlignStars+2) clearAlign=true;
        if (current_selection_L1==maxAlignStars+3) resetAlign=true;
        if (current_selection_L1==maxAlignStars+4) refinePA=true;
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
      if (SetLX200(":hF#") == LX200VALUESET) DisplayMessage(L_ALGN_RESUME0, L_ALGN_RESUME1, -1);
    } else
    if (starsForAlign>0) {
      // check if date/time is set, if not set it
      if (!telInfo.hasDateTime()) { DisplayMessage(L_ALGN_RESUME2, L_ALGN_RESUME3, 2000); menuLocalDateTime(); }

      // start the align
      char s[20]; sprintf(s,":A%d#",starsForAlign);
      if (SetLX200(s) == LX200VALUESET) telInfo.aliMode = (Telescope::AlignMode)starsForAlign; else DisplayMessage(L_ALIGNMENT, L_FAILED "!", -1);
      if (SetLX200(":R7#") == LX200VALUESET) { DisplayMessage(L_ALGN_RESUME4, L_ALGN_RESUME5, 1000);  activeGuideRate=8; }
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
    } else
    if (refinePA) {
      DisplayMessage(L_ALGN_REFINE_MSG1, L_ALGN_REFINE_MSG2, 3500);
      DisplayMessage(L_ALGN_REFINE_MSG3, L_ALGN_REFINE_MSG4, 3500);
      DisplayMessage(L_ALGN_REFINE_MSG5, L_ALGN_REFINE_MSG6, 3500);
      DisplayMessage(L_ALGN_REFINE_MSG7, L_ALGN_REFINE_MSG8, 3500);
      DisplayMessage(L_ALGN_REFINE_MSG9, L_ALGN_REFINE_MSG10,4500);
      DisplayMessage(L_ALGN_REFINE_MSG11,L_ALGN_REFINE_MSG12,3500);
      if (display->UserInterfaceInputValueBoolean(&buttonPad, L_ALGN_REFINE_MSG13, &refinePA)) {
        if ((refinePA) && (SetLX200(":MP#") == LX200VALUESET)) DisplayMessage(L_ALGN_REFINE_MSG14, L_ALGN_REFINE_MSG15, -1);
        if (refinePA) { current_selection_L0 = 0; current_selection_L1 = 0; } // Quit Menu
      }
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
