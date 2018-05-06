// Serial passthrough example code

// use this if running on Teensy3.x (or Teensy LC) hardware, otherwise comment out and assumes Atmel AVR
#define __ARM_Teensy3__

#define ST4RAw 21 // ST4 RA- West,  send data to OnStep
#define ST4DEs 22 // ST4 DE- South, clock input to ISR
#define ST4DEn 23 // ST4 DE+ North, recv data from OnStep
#define ST4RAe 17 // ST4 RA+ East,  always 25 Hz square wave on this pin

#include "St4SerialSlave.h"

void setup() {
  PSerialST4.begin();
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  if (PSerialST4.active()) {
    if (Serial.available()) PSerialST4.write(Serial.read());
    if (PSerialST4.available()) Serial.write(PSerialST4.read());
  }
}

