#pragma once

// on/off, etc.
#define OFF -1
#define ON  -2
#define JS1 -3

#define TIMEOUT_CMD         1000  // Default=1000 (1 second)
#define BACKGROUND_CMD_RATE 300   // Default=300, regulates the background command rate

// enable debugging messages on DebugSer
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
