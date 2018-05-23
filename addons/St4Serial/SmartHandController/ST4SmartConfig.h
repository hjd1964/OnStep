// -------------------------------------------------------------------------------
// Configuration

#pragma once

#include "Arduino.h"

#ifdef ST4SMARTCONTROLLER
  // use this if running on Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
  #define __ARM_Teensy3__
  
  // the serial interface to/from OnStep (over ST4 pins)
  #define ST4RAw 2                 // ST4 RA- West,  send data to OnStep
  #define ST4DEs 3                 // ST4 DE- South, clock input to ISR
  #define ST4DEn 4                 // ST4 DE+ North, recv data from OnStep
  #define ST4RAe 5                 // ST4 RA+ East,  always 25 Hz square wave on this pin
  
  #include "St4SerialSlave.h"
  #define Ser SerialST4             // you could also, for example, use the Teensy3.2's Serial1 interface "Ser Serial1"
  
  // the hand controller buttons
  #define B_PIN0 21
  #define B_PIN1 15
  #define B_PIN2 16
  #define B_PIN3 17
  #define B_PIN4 20
  #define B_PIN5 22
  #define B_PIN6 23
  
  #define B_PIN_UP_0 true
  #define B_PIN_UP_1 true
  #define B_PIN_UP_2 true
  #define B_PIN_UP_3 true
  #define B_PIN_UP_4 true
  #define B_PIN_UP_5 true
  #define B_PIN_UP_6 true
  
  #define SERIAL_BAUD_DEFAULT 57200 // Default=57200, only used for async comms (not SerialST4)
  #define TIMEOUT_CMD 250           // Default=3000 (3 seconds)
  // choose a 128x64 display supported by U8G2lib (if not listed below there are many many others in u8g2 library example Sketches)
  
  #define OLED_DISPLAY 1
  //U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
  ////U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);
  
  #define LED_PIN_OFF               // Default=_OFF, Flashes LED when trying to connect to OnStep
  
  #define DEBUG_OFF                 // Default=_OFF, use "DEBUG Serial" for example (always 9600 baud)
  #define DEBUGBUTTON_OFF           // Defualt=_OFF, use "DEBUGBUTTON" to activate

#endif

