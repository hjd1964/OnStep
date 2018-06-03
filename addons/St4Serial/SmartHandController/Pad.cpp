#include "Pad.h"
#include "Config.h"

//------------------------------------------------------------------------------

void Pad::setup(const int pin[7], const bool active[7])
{
  shift.init(pin[0],debounceMs,true,active[0]);
  n.init(pin[1],debounceMs,true,active[1]);
  s.init(pin[2],debounceMs,true,active[2]);
  e.init(pin[3],debounceMs,true,active[3]);
  w.init(pin[4],debounceMs,true,active[4]);
  F.init(pin[5],debounceMs,true,active[5]);
  f.init(pin[6],debounceMs,true,active[6]);
}

void Pad::tickButtons()
{
  shift.poll();
  n.poll();
  s.poll();
  e.poll();
  w.poll();
  F.poll();
  f.poll();
}

bool Pad::anyPressed()
{
  if (shift.isDown() || n.isDown() || s.isDown() || e.isDown() || w.isDown() || f.isDown() || F.isDown()) return true;
  if (shift.wasPressed(true) || n.wasPressed(true) || s.wasPressed(true) || e.wasPressed(true) || w.wasPressed(true) || f.wasPressed(true) || F.wasPressed(true)) return true;
  return false;
}

void Pad::waitForPress() {
  for (;;) { tickButtons(); delay(5); if (anyPressed()) break; }
}

void Pad::clearAllPressed() {
  shift.clearPress();
  n.clearPress();
  s.clearPress();
  e.clearPress();
  w.clearPress();
  F.clearPress();
  f.clearPress();
}

