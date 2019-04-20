#pragma once

// -------------------------------------------------------------------------------
// Configuration

// Select the display type by commenting out the one you don't use and leaving the other
#define OLED_DISPLAY SH1106       // SH1106 is the 1.3"
//#define OLED_DISPLAY SSD1306    // SSD1306 is the 0.96"

// The serial interface to/from OnStep
#define Ser SerialST4             // You could also, for example, use the Teensy3.2's Serial3 interface "Ser Serial3"
#define SERIAL_BAUD_DEFAULT 57200 // Default=57200, only used for async comms (not SerialST4)

// Utility Light, default=_OFF
// UTILITY_LIGHT n, where n=0 to 255 activates this feature and sets default brightness
#define UTILITY_LIGHT_OFF

// Default=_ON, _OFF to disable ambient conditions in the display rotation
#define AMBIENT_CONDITIONS_ON

// Use 24 hour format for entering time, default true
#define USE_24HR_TIME true

// Allow menus to wrap, default false
#define ALLOW_MENU_WRAP false

// Use _ON to enable "pass-through" Aux ST4 port, default=_OFF
#define AUX_ST4_OFF

// Configure defaults for display dimming and blanking (in milliseconds, 0 disables)
#define DISPLAY_DIM_TIME     30000        // original value 30000
#define DISPLAY_BLANK_TIME  120000        // original value 120000

// THAT IS ALL FOR USER CONFIGURABLE PARAMETERS! Do not change anything below this line!
// -------------------------------------------------------------------------------------

// If we are not on an ESP32, then assume a Teensy 3.2
#ifndef ESP32
  #define __ARM_Teensy3__
#endif

// Default pin state; true for active LOW, false if active HIGH
#define B_PIN_UP_0 true
#define B_PIN_UP_1 true
#define B_PIN_UP_2 true
#define B_PIN_UP_3 true
#define B_PIN_UP_4 true
#define B_PIN_UP_5 true
#define B_PIN_UP_6 true

// The hand controller buttons
#ifdef  __ARM_Teensy3__ 
  #define B_PIN0 21   // Shift
  #define B_PIN1 15   // N
  #define B_PIN2 16   // S
  #define B_PIN3 17   // E
  #define B_PIN4 20   // W
  #define B_PIN5 22   // F
  #define B_PIN6 23   // f

  #define ST4RAw 2    // ST4 (OnStep) RA- West,  send data to OnStep
  #define ST4DEs 3    // ST4 (OnStep) DE- South, clock input to ISR
  #define ST4DEn 4    // ST4 (OnStep) DE+ North, recv data from OnStep
  #define ST4RAe 5    // ST4 (OnStep) RA+ East,  always 25 Hz square wave on this pin

  #ifdef AUX_ST4_ON
    #define ST4AuxRAw 6 // ST4 (Aux) RA- West
    #define ST4AuxDEs 7 // ST4 (Aux) DE- South
    #define ST4AuxDEn 8 // ST4 (Aux) DE+ North
    #define ST4AuxRAe 9 // ST4 (Aux) RA+ East
  #endif

  #define UTILITY_LIGHT_PIN  10    // Pin10 used as PWM output to drive utility lamp LED
#endif

#ifdef ESP32
  // workaround for ESP32 crashing on EEPROM commit
  #define DISABLE_EEPROM_COMMIT_ON

  #define B_PIN0 33   // Shift
  #define B_PIN1 25   // N
  #define B_PIN2 32   // S
  #define B_PIN3 36   // E
  #define B_PIN4 34   // W
  #define B_PIN5 39   // F
  #define B_PIN6 35   // f

  #define ST4RAw 23   // ST4 (OnStep) RA- West,  send data to OnStep
  #define ST4DEs 14   // ST4 (OnStep) DE- South, clock input to ISR
  #define ST4DEn 27   // ST4 (OnStep) DE+ North, recv data from OnStep
  #define ST4RAe 26   // ST4 (OnStep) RA+ East,  always 12.5 Hz square wave on this pin

  #ifdef AUX_ST4_ON
    #define ST4AuxRAw 19 // ST4 (Aux) RA- West
    #define ST4AuxDEs 18 // ST4 (Aux) DE- South
    #define ST4AuxDEn 17 // ST4 (Aux) DE+ North
    #define ST4AuxRAe 5  // ST4 (Aux) RA+ East
  #endif

  #define UTILITY_LIGHT_PIN  13    // GPIO13 used as PWM output to drive utility lamp LED
#endif 

#include "St4SerialSlave.h"
#include "St4Aux.h"

#define TIMEOUT_CMD         1000  // Default=1000 (1 second)
#define BACKGROUND_CMD_RATE 300   // Default=300, regulates the background command rate

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
