// -------------------------------------------------------------------------------
// Configuration

#pragma once

// set to _ON to activate this configuration
#define ST4SMARTCONTROLLER_ON

#ifdef ST4SMARTCONTROLLER_ON
  // the serial interface to/from OnStep (over ST4 pins)
  #define __ARM_Teensy3__           // use this for Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
  #define ST4RAw 2                  // ST4 RA- West,  send data to OnStep
  #define ST4DEs 3                  // ST4 DE- South, clock input to ISR
  #define ST4DEn 4                  // ST4 DE+ North, recv data from OnStep
  #define ST4RAe 5                  // ST4 RA+ East,  always 25 Hz square wave on this pin
  #include "St4SerialSlave.h"
  #define Ser SerialST4             // you could also, for example, use the Teensy3.2's Serial1 interface "Ser Serial1"
  #define SERIAL_BAUD_DEFAULT 57200 // Default=57200, only used for async comms (not SerialST4)
  #define TIMEOUT_CMD 1000          // Default=1000 (1 second)
  #define BACKGROUND_CMD_RATE 300   // Default=300, regulates the background command rate

  // the display, SSD1306 or SH1106
  #define OLED_DISPLAY SSD1306

  // the hand controller buttons
  #define B_PIN0 21                 // Shift
  #define B_PIN1 15                 // N
  #define B_PIN2 16                 // S
  #define B_PIN3 17                 // E
  #define B_PIN4 20                 // W
  #define B_PIN5 22                 // F
  #define B_PIN6 23                 // f
  
  #define B_PIN_UP_0 true           // true for active LOW, false if active HIGH
  #define B_PIN_UP_1 true
  #define B_PIN_UP_2 true
  #define B_PIN_UP_3 true
  #define B_PIN_UP_4 true
  #define B_PIN_UP_5 true
  #define B_PIN_UP_6 true
#endif

