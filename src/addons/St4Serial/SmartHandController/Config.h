// -------------------------------------------------------------------------------
// Configuration

// use this if running on Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
#define __ARM_Teensy3__

// the serial interface to/from OnStep (over ST4 pins)
#define Ser SerialST4             // you could also, for example, use the Teensy3.2's Serial1 interface "Ser Serial1"
#define ST4RAw 2                 // ST4 RA- West,  send data to OnStep
#define ST4DEs 3                 // ST4 DE- South, clock input to ISR
#define ST4DEn 4                 // ST4 DE+ North, recv data from OnStep
#define ST4RAe 5                 // ST4 RA+ East,  always 25 Hz square wave on this pin
#include "St4SerialSlave.h"

// the hand controller buttons
OneButton buttons[7] = { OneButton(21, false),
                        OneButton(15, false), OneButton(16, false),OneButton(17, false),OneButton(20, false),
                        OneButton(22, true), OneButton(23, true) };

// choose a 128x64 display supported by U8G2lib (if not listed below there are many many others in u8g2 library example Sketches)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0);

#define LED_PIN_OFF               // Default=_OFF, Flashes LED when trying to connect to OnStep

#define SERIAL_BAUD_DEFAULT 57200 // Default=57200, only used for async comms (not SerialST4)

#define TIMEOUT_CMD 5000          // Default=3000 (3 seconds)

#define DEBUG_OFF                 // Default=_OFF, use "DEBUG Serial" for example (always 9600 baud)
#define DEBUGBUTTON_OFF           // Defualt=_OFF, use "DEBUGBUTTON" to activate

// -------------------------------------------------------------------------------

