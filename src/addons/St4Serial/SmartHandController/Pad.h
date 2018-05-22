#pragma once
#include <Arduino.h>
#include <OneButton.h>

enum Button { B_SHIFT, B_NORTH, B_SOUTH, B_EAST, B_WEST, B_F, B_f };
enum ButtonEvent { E_NONE, E_CLICK, E_DOUBLECLICK, E_LONGPRESSTART, E_LONGPRESS, E_LONGPRESSSTOP };
extern byte eventbuttons[7];
class Pad
{
  bool m_buttonPressed;
  OneButton *m_buttons[7];
public:
  void setup(const int pin[7], const bool active[7]);
  void attachEvent();
  void tickButtons();
  bool buttonPressed();
};