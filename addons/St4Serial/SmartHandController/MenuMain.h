
// Feature Key Menu (double click)
void SmartHandController::menuFeatureKey()
{
  static unsigned short current_selection_feature_mode = 1;
  unsigned short last_selection_feature_mode = current_selection_feature_mode;

  char string_feature_Modes[120] = "Guide Rate";

  int i=1,j=-1,k=-1,l=-1,m=-1,n=-1;
  #ifdef UTILITY_LIGHT
    { i++; j=i; strcat(string_feature_Modes,"\nUtility Light"); }
  #endif
  if (telInfo.hasReticle()) { i++; k=i; strcat(string_feature_Modes,"\nReticle"); }
  if (telInfo.hasFocuser1()) { i++; l=i; if (telInfo.hasFocuser2()) strcat(string_feature_Modes,"\nFocuser 1"); else strcat(string_feature_Modes,"\nFocuser"); }
  if (telInfo.hasFocuser2()) { i++; m=i; strcat(string_feature_Modes,"\nFocuser 2"); }
  if (telInfo.hasRotator())  { i++; n=i; strcat(string_feature_Modes,"\nRotator");   }

  current_selection_feature_mode = display->UserInterfaceSelectionList(&buttonPad, "Feature Keys", current_selection_feature_mode, string_feature_Modes);

  if (last_selection_feature_mode>0) {
    if (current_selection_feature_mode==1) featureKeyMode=1; else // guide rate
    if (current_selection_feature_mode==j) featureKeyMode=2; else // util. light
    if (current_selection_feature_mode==k) featureKeyMode=3; else // reticule
    if (current_selection_feature_mode==l) featureKeyMode=4; else // focuser 1
    if (current_selection_feature_mode==m) featureKeyMode=5; else // focuser 2
    if (current_selection_feature_mode==n) featureKeyMode=6; else // rotator
    { featureKeyMode=1; current_selection_feature_mode=1; } // default to guide rate
  } else current_selection_feature_mode = last_selection_feature_mode;
}

// Main Menu
void SmartHandController::menuMain()
{
  current_selection_L0 = 1;
  while (current_selection_L0 != 0)
  {
    boolean sync=false;
    if (!telInfo.isMountAltAz()) {
      const char *string_list_main_UnParkedL0 = "Goto\n""Sync\n""Align\n""Parking\n""Tracking\n""PEC\n""Settings";
      current_selection_L0 = display->UserInterfaceSelectionList(&buttonPad, "Main Menu", current_selection_L0, string_list_main_UnParkedL0);
      switch (current_selection_L0) {
        case 1: if (menuSyncGoto(false)==MR_QUIT) return; break;
        case 2: if (display->UserInterfaceInputValueBoolean(&buttonPad, "Sync Here?", &sync)) if (sync) DisplayMessageLX200(SetLX200(":CS#"),false); break;
        case 3: menuAlignment(); break;
        case 4: menuParking(); break;
        case 5: menuTracking(); break;
        case 6: menuPEC(); break;
        case 7: menuSettings(); break;
      }
    } else {
      const char *string_list_main_UnParkedL0 = "Goto\n""Sync\n""Align\n""Parking\n""Tracking\n""Settings";
      current_selection_L0 = display->UserInterfaceSelectionList(&buttonPad, "Main Menu", current_selection_L0, string_list_main_UnParkedL0);
      switch (current_selection_L0) {
        case 1: if (menuSyncGoto(false)==MR_QUIT) return; break;
        case 2: if (display->UserInterfaceInputValueBoolean(&buttonPad, "Sync Here?", &sync)) if (sync) DisplayMessageLX200(SetLX200(":CS#"),false); break;
        case 3: menuAlignment(); break;
        case 4: menuParking(); break;
        case 5: menuTracking(); break;
        case 6: menuSettings(); break;
      }
    }
  }
}

#include "MenuSyncGoto.h"
#include "MenuAlignment.h"

// Parking Menu
void SmartHandController::menuParking()
{
  Telescope::ParkState currentstate = telInfo.getParkState();
  if (currentstate == Telescope::PRK_PARKED) { }
  if (currentstate == Telescope::PRK_UNPARKED) { }

  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Park\n""Un-Park\n""Set-Park";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Parking", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
      case 1:
        if (SetLX200(":hP#")== LX200VALUESET) {
          DisplayMessage("Parking", "scope", 500); 
          current_selection_L1 = 0;
          current_selection_L0 = 0;
        } else DisplayMessage("Park", "Failed", 1000);
      break;
      case 2:
        if (SetLX200(":hR#")== LX200VALUESET) {
          DisplayMessage("Un-Parking", "scope", 500); 
          current_selection_L1 = 0;
        } else DisplayMessage("Un-Park", "Failed", 1000);
        break;
      case 3: 
        boolean SetP=false; 
        if (display->UserInterfaceInputValueBoolean(&buttonPad, "Set-Park?", &SetP)) {
          if (SetP) {
            if (SetLX200(":hQ#")== LX200VALUESET) {
              DisplayMessage("Set-Park", "OK", 500);
              current_selection_L1 = 0;
            } else DisplayMessage("Set-Park", "Failed", 1000); 
          } else DisplayMessage("Set-Park", "Canceled", 500);
        } else DisplayMessage("Set-Park", "Canceled", 500);
      break;
    }
  }
}

void SmartHandController::menuTracking()
{
  Telescope::TrackState currentstate = telInfo.getTrackingState();

  if (telInfo.isMountAltAz()) {
    const char *string_list;
    current_selection_L1 = 1;
    while (current_selection_L1 != 0)
    {
      if (currentstate == Telescope::TRK_ON) {
        string_list = "Stop\n""Sidereal\n""Solar\n""Lunar\n""Rate Reset\n""Rate +\n""Rate -";
      } else {
        string_list = "Start\n""Sidereal\n""Solar\n""Lunar\n""Rate Reset\n""Rate +\n""Rate -";
      }
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Tracking", current_selection_L1, string_list);
      switch (current_selection_L1)
      {
      case 1:
        if (currentstate == Telescope::TRK_ON) {
          if (SetLX200(":Td#")== LX200VALUESET) { DisplayMessage("Tracking", "OFF", 500); currentstate=Telescope::TRK_OFF; } else DisplayMessage("Set State", "Failed", 1000);
        } else {
          if (SetLX200(":Te#")== LX200VALUESET) { DisplayMessage("Tracking", "ON", 500); currentstate=Telescope::TRK_ON; } else DisplayMessage("Set State", "Failed", 1000);
        }
      break;
      case 2: DisplayMessageLX200(SetLX200(":TQ#"),false); break;
      case 3: DisplayMessageLX200(SetLX200(":TS#"),false); break;
      case 4: DisplayMessageLX200(SetLX200(":TL#"),false); break;
      case 5: DisplayMessageLX200(SetLX200(":TR#"),false); break;
      case 6: DisplayMessageLX200(SetLX200(":T+#"),false); break;
      case 7: DisplayMessageLX200(SetLX200(":T-#"),false); break;
      }
    }
  } else {
    const char *string_list;
    current_selection_L1 = 1;
    while (current_selection_L1 != 0)
    {
      if (currentstate == Telescope::TRK_ON) {
        string_list = "Stop\n""Sidereal\n""Solar\n""Lunar\n""Comp Full\n""Comp Refr\n""Comp Off\n""Comp Sngl Ax\n""Comp Dual Ax\n""Rate Reset\n""Rate +0.02Hz\n""Rate -0.02Hz";
      } else {
        string_list = "Start\n""Sidereal\n""Solar\n""Lunar\n""Comp Full\n""Comp Refr\n""Comp Off\n""Comp Sngl Ax\n""Comp Dual Ax\n""Rate Reset\n""Rate +0.02Hz\n""Rate -0.02Hz";
      }
      current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "Tracking", current_selection_L1, string_list);
      switch (current_selection_L1)
      {
      case 1:
        if (currentstate == Telescope::TRK_ON) {
          if (SetLX200(":Td#")== LX200VALUESET) { DisplayMessage("Tracking", "OFF", 500); currentstate=Telescope::TRK_OFF; } else DisplayMessage("Set State", "Failed", 1000);
        } else {
          if (SetLX200(":Te#")== LX200VALUESET) { DisplayMessage("Tracking", "ON", 500); currentstate=Telescope::TRK_ON; } else DisplayMessage("Set State", "Failed", 1000);
        }
      break;
      case 2:  DisplayMessageLX200(SetLX200(":TQ#"),false); break;
      case 3:  DisplayMessageLX200(SetLX200(":TS#"),false); break;
      case 4:  DisplayMessageLX200(SetLX200(":TL#"),false); break;
      case 5:  DisplayMessageLX200(SetLX200(":To#"),false); break;
      case 6:  DisplayMessageLX200(SetLX200(":Tr#"),false); break;
      case 7:  DisplayMessageLX200(SetLX200(":Tn#"),false); break;
      case 8:  DisplayMessageLX200(SetLX200(":T1#"),false); break;
      case 9:  DisplayMessageLX200(SetLX200(":T2#"),false); break;
      case 10: DisplayMessageLX200(SetLX200(":TR#"),false); break;
      case 11: DisplayMessageLX200(SetLX200(":T+#"),false); break;
      case 12: DisplayMessageLX200(SetLX200(":T-#"),false); break;
      }
    }
  }
}

#include "MenuSettings.h"

// PEC Menu
void SmartHandController::menuPEC()
{
  current_selection_L1 = 1;
  while (current_selection_L1 != 0)
  {
    const char *string_list_SettingsL1 = "Play\n""Stop\n""Clear\n""Record\n""Write to NV";
    current_selection_L1 = display->UserInterfaceSelectionList(&buttonPad, "PEC", current_selection_L1, string_list_SettingsL1);
    switch (current_selection_L1)
    {
    case 1: DisplayMessageLX200(SetLX200(":$QZ+#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Playing", 1000); break;
    case 2: DisplayMessageLX200(SetLX200(":$QZ-#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Stopped", 1000); break;
    case 3: DisplayMessageLX200(SetLX200(":$QZZ#"),false); break;
    case 4: DisplayMessageLX200(SetLX200(":$QZ/#"),true); current_selection_L0=0; current_selection_L1=0; DisplayMessage("PEC", "Recording", 1000); break;
    case 5: DisplayMessageLX200(SetLX200(":$QZ!#"),false); break;
    default: break;
    }
  }
}
