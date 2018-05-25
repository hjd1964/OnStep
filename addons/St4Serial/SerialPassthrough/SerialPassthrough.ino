// Serial passthrough example code

// use this if running on Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
#define __ARM_Teensy3__

#define ST4RAw 2 // ST4 RA- West,  send data to OnStep
#define ST4DEs 3 // ST4 DE- South, clock input to ISR
#define ST4DEn 4 // ST4 DE+ North, recv data from OnStep
#define ST4RAe 5 // ST4 RA+ East,  always 12.5 Hz square wave on this pin

#include "St4SerialSlave.h"

void setup() {
  SerialST4.begin();
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  if (SerialST4.active()) {
    if (Serial.available()) SerialST4.write(Serial.read());
    if (SerialST4.available()) Serial.write(SerialST4.read());
  }
}

