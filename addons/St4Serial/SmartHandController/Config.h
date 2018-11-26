// -------------------------------------------------------------------------------
// Configuration
// *** user configuration is in SHC_Config.h and TeenAstro_Config.h ***

#pragma once

#include "SHC_Config.h"
#include "TeenAstro_Config.h"

// Enable debugging messages on DebugSer
#define DEBUG_OFF                 // default=_OFF, use "DEBUG_ON" to activate
#define DebugSer Serial           // default=Serial, or Serial1 for example (always 9600 baud)
#define DEBUGBUTTON_OFF           // defualt=_OFF, use "DEBUGBUTTON" to activate

// helper macros for debugging, with less typing
#if defined(DEBUG_ON)
  #define D(x)     DebugSer.print(x)
  #define DH(x,y)  DebugSer.print(x,HEX)
  #define DL(x)    DebugSer.println(x)
  #define DHL(x,y) DebugSer.println(x,HEX)
#else
  #define D(x)
  #define DH(x,y)
  #define DL(x)
  #define DHL(x,y)
#endif

