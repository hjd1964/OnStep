// -------------------------------------------------------------------------------
// Configuration

#pragma once

#include "ST4SmartConfig.h"
#include "SerialSmartConfig.h"

#define DEBUG_OFF                 // default=_OFF, use "DEBUG_ON" to activate
#define DEBUGBUTTON_OFF           // defualt=_OFF, use "DEBUGBUTTON" to activate

// helper macros for debugging, with less typing
#if defined(DEBUG_ON)
  #define DebugSer Serial         // default=Serial, or Serial1 for example (always 9600 baud)
  #define DBG(x)     DebugSer.print(x)
  #define DBG2(x,y)  DebugSer.print(x,y)
  #define DBGL(x)    DebugSer.println(x)
  #define DBGL2(x,y) DebugSer.println(x,y)
#else
  #define DBG(x)   /* nothing */
  #define DBG2(x,y) /* nothing */
  #define DBGL(x)   /* nothing */
  #define DBGL2(x,y) /* nothing */
#endif

