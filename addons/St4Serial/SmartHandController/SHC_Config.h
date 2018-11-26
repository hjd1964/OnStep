// -------------------------------------------------------------------------------
// Configuration

#pragma once

// set to _ON to activate this configuration
#define SHC_ON

#ifdef SHC_ON
  // The serial interface to/from OnStep
  #define Ser SerialST4             // You could also, for example, use the Teensy3.2's Serial3 interface "Ser Serial3"
  #define SERIAL_BAUD_DEFAULT 57200 // Default=57200, only used for async comms (not SerialST4)
  
  // Select the display type 0.96" is SSD1306 and 1.3" is SH1106
  #define OLED_DISPLAY SH1106

  // THAT'S IT FOR USER CONFIGURATION!

  // -------------------------------------------------------------------------------

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

    #define ST4RAw 2    // ST4 RA- West,  send data to OnStep
    #define ST4DEs 3    // ST4 DE- South, clock input to ISR
    #define ST4DEn 4    // ST4 DE+ North, recv data from OnStep
    #define ST4RAe 5    // ST4 RA+ East,  always 25 Hz square wave on this pin
  #endif
  
  #ifdef ESP32
    #define B_PIN0 33   // Shift
    #define B_PIN1 25   // N
    #define B_PIN2 32   // S
    #define B_PIN3 36   // E
    #define B_PIN4 34   // W
    #define B_PIN5 39   // F
    #define B_PIN6 35   // f

    #define ST4RAw 26   // ST4 RA- West,  send data to OnStep
    #define ST4DEs 27   // ST4 DE- South, clock input to ISR
    #define ST4DEn 14   // ST4 DE+ North, recv data from OnStep
    #define ST4RAe 23   // ST4 RA+ East,  always 12.5 Hz square wave on this pin
  #endif 

  #include "St4SerialSlave.h"

  #define TIMEOUT_CMD         1000  // Default=1000 (1 second)
  #define BACKGROUND_CMD_RATE 300   // Default=300, regulates the background command rate

#endif
