// Alignment menu

void SmartHandController::menuAlignment()
{
  int maxAlignStars, thisStar, numStars;
  bool alignInProgress=false;

  if (!telInfo.getAlignStars(&maxAlignStars, &thisStar, &numStars)) { maxAlignStars=1; thisStar=1; numStars=1; }
  if (maxAlignStars>9) { maxAlignStars=1; thisStar=1; numStars=1; }
  if (numStars>9)      { maxAlignStars=1; thisStar=1; numStars=1; }
  if ((thisStar>0) && (thisStar<=numStars)) alignInProgress=true;

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
      strcpy(string_list_AlignmentL1,"Resume Align\n""Show Corr\n""Clear Corr\n""Reset Home");
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
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
        sprintf(s,"%d-Star Align\n",i);
        strcat(string_list_AlignmentL1,s);
      }
      strcat(string_list_AlignmentL1,"Show Model\n""Clear Model\n""Reset Home");
      if (!telInfo.isMountAltAz()) {
        sprintf(s,"\nRefine PA");
        strcat(string_list_AlignmentL1,s);
      }
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Alignment", current_selection_L1, string_list_AlignmentL1);
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
      if (SetLX200(":hF#") == LX200VALUESET) DisplayMessage("Reset", "Move to Home", -1);
    } else
    if (starsForAlign>0) {
      char s[20]; sprintf(s,":A%d#",starsForAlign);
      if (SetLX200(s) == LX200VALUESET) telInfo.aliMode = (Telescope::AlignMode)starsForAlign; else DisplayMessage("Alignment", "Failed!", -1);
      if (SetLX200(":R7#") == LX200VALUESET) { DisplayMessage("Guide Rate", "48X Set", 1000);  activeGuideRate=8; }
      current_selection_L1 = 0; current_selection_L0 = 0; // Quit Menu
    } else
    if (refinePA) {
      DisplayMessage("Setup & 3+ star",   "align mount." ,     3500);
      DisplayMessage("Goto bright star",  "near NCP/SCP w/",   3500);
      DisplayMessage("Dec in 50 to 80" ,  "deg range N/S.",    3500);
      DisplayMessage("Answr YES below.",  "Use Polar Align",   3500);
      DisplayMessage("adjust controls to","center star again.",4500);
      DisplayMessage("Optionally align",  "the mount again.",  3500);
      if (display->UserInterfaceInputValueBoolean(&buttonPad, "Refine PA?", &refinePA)) {
        if ((refinePA) && (SetLX200(":MP#") == LX200VALUESET)) DisplayMessage("Center star again", "using PA controls", -1);
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
