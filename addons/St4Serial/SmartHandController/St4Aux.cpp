// -----------------------------------------------------------------------------------
// ST4 aux port

#include <Arduino.h>
#include "St4Aux.h"
#include "Config.h"
#include "PushButton.h"

#if defined(ST4AuxRAw) && defined(ST4AuxDEs) && defined(ST4AuxDEn) && defined(ST4AuxRAe)

void Ast4::tickButtons() {
  n.poll();
  s.poll();
  e.poll();
  w.poll();
}

void Ast4::setup() {
  n.init(ST4AuxDEn,debounceMs,false);
  s.init(ST4AuxDEs,debounceMs,false);
  e.init(ST4AuxRAe,debounceMs,false);
  w.init(ST4AuxRAw,debounceMs,false);
}

Ast4 auxST4;

#endif
