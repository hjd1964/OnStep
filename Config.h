// -----------------------------------------------------------------------------------
// Universal configuration for OnStep

// ********** this file contains NO user settings, see the Config.xxx.h file appropraite for your hardware **************
// (though you could replace this file contents w/your settings and remove all of the Config.xxx.h files if you wanted to)

#pragma once

#include "Config.Classic.h"
#include "Config.MaxESP.h"
#include "Config.MaxPCB.h"
//#include "Config.Mega2560Alt.h"
#include "Config.MiniPCB.h"
#include "Config.Ramps14.h"
#include "Config.STM32.h"
//#include "Config.TM4C.h"

#ifdef Classic_ON
  #define Configuration_Found
#endif
#ifdef MiniPCB_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxPCB_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxESP_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxESP2_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef MaxESP3_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef Ramps14_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef Mega2560Alt_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32CZ_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32Black_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef STM32Blue_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif
#ifdef TM4C_ON
  #ifdef Configuration_Found
    #define Configuration_Duplicate
  #else
    #define Configuration_Found
  #endif
#endif

#ifdef Configuration_Duplicate
  #error "You have more than one Config.xxx.h file enabled, ONLY ONE can be enabled with _ON."
#endif

#ifndef Configuration_Found
  #error "Choose ONE Config.xxx.h file and enable it for use by turning it _ON."
#endif

// configuration file version

#ifdef FileVersionConfig
  #if FileVersionConfig < FirmwareVersionConfig
    // firmware version 2 is compatible with file version 1
    #if (FileVersionConfig==1) && (FirmwareVersionConfig==2)
      #warning "Configuration: There have been changes to the configuration file format, but OnStep is still backwards compatible for now."
    #else
      #error "Configuration: There have been changes to the configuration file format.  You'll have to make a new Config.xxx.h file."
    #endif
  #elif FileVersionConfig > FirmwareVersionConfig
    #error "Configuration: Configuration file version mismatch."
  #endif
#else
  #if (FirmwareVersionConfig == 1) || (FirmwareVersionConfig == 2)
    #warning "Configuration: Config.xxx.h file version isn't specified (pre-version 1?)"
  #else
    #error "Configuration: There have been changes to the configuration file format.  You'll have to make a new Config.xxx.h file."
  #endif
#endif
