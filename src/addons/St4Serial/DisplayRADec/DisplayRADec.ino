// Serial passthrough example code

// use this if running on Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
#define __ARM_Teensy3__

#define ST4RAw 2 // ST4 RA- West,  send data to OnStep
#define ST4DEs 3 // ST4 DE- South, clock input to ISR
#define ST4DEn 4 // ST4 DE+ North, recv data from OnStep
#define ST4RAe 5 // ST4 RA+ East,  always 12.5 Hz square wave on this pin

#include "St4SerialSlave.h"
#include <U8g2lib.h>  // use Arduino->Sketch->Include Library->Manage Libraries then search for "u8g2" and install
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

void setup() {
  SerialST4.begin();
  SerialST4.setTimeout(3000);

  Serial.begin(9600);
  
  u8g2.begin();
  u8g2.setContrast(0);
  u8g2.setFont(u8g2_font_9x15B_tf);   // choose a suitable font

  delay(1000);
}

void loop() {
  u8g2.clearBuffer();                 // clear the internal memory
 
  SerialST4.write(":GR#");
  delay(50);
  char temp[21]="";
  temp[SerialST4.readBytesUntil('#',temp,20)]=0;
  u8g2.drawStr(0,10,temp);            // write something to the internal memory
  Serial.print("a"); Serial.print(temp);
  
  SerialST4.write(":GD#");
  delay(50);
  temp[SerialST4.readBytesUntil('#',temp,20)]=0;
  u8g2.drawStr(0,30,temp);
  Serial.print(" d"); Serial.println(temp);

  u8g2.sendBuffer();                  // transfer internal memory to the display
  delay(500);
}

