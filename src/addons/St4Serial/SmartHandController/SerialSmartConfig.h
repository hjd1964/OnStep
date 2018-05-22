#pragma once
#ifdef SERIALSMARTCONTROLLER
#define SERIAL_BAUD_DEFAULT 9600
#define Ser Serial
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI

#define OLED_DISPLAY 0
#define B_PIN0 D8
#define B_PIN1 D7
#define B_PIN2 D6
#define B_PIN3 D0
#define B_PIN4 D5
#define B_PIN5 D3
#define B_PIN6 D4

#define B_PIN_UP_0 false
#define B_PIN_UP_1 false
#define B_PIN_UP_2 false
#define B_PIN_UP_3 false
#define B_PIN_UP_4 false
#define B_PIN_UP_5 true
#define B_PIN_UP_6 true
#endif
#endif