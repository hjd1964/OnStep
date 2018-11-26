// -------------------------------------------------------------------------------
// Configuration

#pragma once

// set to _ON to activate this configuration
#define TEEN_ASTRO_OFF

#ifdef TEEN_ASTRO_ON
  #ifdef ARDUINO_ESP8266_WEMOS_D1MINI
    // The serial interface to/from OnStep
    #define Ser Serial
    #define SERIAL_BAUD_DEFAULT 9600
    #define TIMEOUT_CMD 100           // Default=100 (0.1 seconds)
    #define BACKGROUND_CMD_RATE 150   // Defualt=150, regulates the background command rate

    // The display, SSD1306 or SH1106
    #define OLED_DISPLAY SH1106

    // The hand controller buttons
    #define B_PIN0 D8                 // Shift
    #define B_PIN1 D7                 // N
    #define B_PIN2 D6                 // S
    #define B_PIN3 D0                 // E
    #define B_PIN4 D5                 // W
    #define B_PIN5 D3                 // F
    #define B_PIN6 D4                 // f
    
    #define B_PIN_UP_0 false          // true for active LOW, false if active HIGH
    #define B_PIN_UP_1 false
    #define B_PIN_UP_2 false
    #define B_PIN_UP_3 false
    #define B_PIN_UP_4 false
    #define B_PIN_UP_5 true
    #define B_PIN_UP_6 true
  #endif
#endif

