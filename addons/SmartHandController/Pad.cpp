
#include "Pad.h"
#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Pinmap.h"

//------------------------------------------------------------------------------

void Pad::setup(const int pin[7], const bool active[7], int nullNS, int nullEW)
{
  shift.init(pin[0],debounceMs,true,0,active[0]);
  n.init(pin[1],debounceMs,true,nullNS,active[1]);
  s.init(pin[2],debounceMs,true,-nullNS,active[2]);
  e.init(pin[3],debounceMs,true,nullEW,active[3]);
  w.init(pin[4],debounceMs,true,-nullEW,active[4]);
  F.init(pin[5],debounceMs,true,0,active[5]);
  f.init(pin[6],debounceMs,true,0,active[6]);
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

bool Pad::nsewPressed()
{
  if (n.isDown() || s.isDown() || e.isDown() || w.isDown()) return true;
  if (n.wasPressed(true) || s.wasPressed(true) || e.wasPressed(true) || w.wasPressed(true)) return true;
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
