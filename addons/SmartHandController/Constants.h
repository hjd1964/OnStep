#pragma once

// Enable debug and/or status messages to be passed to OnStep for display using its debug facilities
// default "DEBUG OFF", use "DEBUG ON" for background errors only, use "DEBUG VERBOSE" for all errors and status messages
#define DEBUG VERBOSE

// on/off, etc.
#define OFF -1
#define ON  -2
#define JS1 -3

#define TIMEOUT_CMD         1000  // Default=1000 (1 second)
#define BACKGROUND_CMD_RATE 300   // Default=300, regulates the background command rate

// Helper macros for debugging, with less typing
// Use VF or VLF for strings (supports embedded spaces)
#if DEBUG != OFF
  #define D(x)       { Ser.print(":EC"); Ser.print(x); Ser.print("#"); }
  #define DF(x)      { Ser.print(":EC"); { char s1[255]; strcpy(s1,x); for (unsigned int i=0; i < strlen(s1); i++) if (s1[i]==' ') s1[i]='_'; Ser.print(s1); } Ser.print("#"); }
  #define DL(x)      { Ser.print(":EC"); Ser.print(x); Ser.print("&#"); }
  #define DLF(x)     { Ser.print(":EC"); { char s1[255]; strcpy(s1,x); for (unsigned int i=0; i < strlen(s1); i++) if (s1[i]==' ') s1[i]='_'; Ser.print(s1); } Ser.print("&#"); }
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE
  #define V(x)       D(x)
  #define VF(x)      DF(x)
  #define VL(x)      DL(x)
  #define VLF(x)     DLF(x)
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif

// set MCU string
#if defined(ESP8266)
  #define MCU_STR "ESP8266"
#elif defined(ESP32)
  #define MCU_STR "ESP32"
#elif defined(_mk20dx128_h_)
  #define MCU_STR "Teensy3.0"
#elif defined(__MK20DX128__)
  #define MCU_STR "Teensy3.1"
#elif defined(__MK20DX256__)
  #define MCU_STR "Teensy3.2"
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #define MCU_STR "Teensy4.0"
#else
  #define MCU_STR "Unknown"
#endif
