#pragma once
#include <Arduino.h>
#include "PushButton.h"

class Pad
{
public:
  void setup(const int pin[7], const bool active[7], int nullNS, int nullEW);

  void tickButtons();
  bool anyPressed();
  bool nsewPressed();  
  void waitForPress();
  void clearAllPressed();

  button shift,n,s,e,w,F,f;

private:
  int debounceMs = 30;
};
