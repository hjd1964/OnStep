// -------------------------------------------------------------------------------
// Configuration

#pragma once

#include "ST4SmartConfig.h"
#include "SerialSmartConfig.h"

#define DEBUG_OFF                 // default=_OFF, use "DEBUG_ON" to activate
#define DEBUGBUTTON_OFF           // defualt=_OFF, use "DEBUGBUTTON" to activate
#define DebugSer Serial           // default=Serial, or Serial1 for example (always 9600 baud)

//#define SERIALSMARTCONTROLLER
#ifdef SERIALSMARTCONTROLLER
  #include "SerialSmartConfig.h"
#endif

