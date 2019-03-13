// -----------------------------------------------------------------------------------
// ST4 aux port

#include <Arduino.h>
#include "St4Aux.h"
#include "Config.h"
#include "PushButton.h"

#if defined(ST4AuxRAw) && defined(ST4AuxDEs) && defined(ST4AuxDEn) && defined(ST4AuxRAe)

void Ast4::setup() {
  n.init(ST4AuxDEn,debounceMs,true,LOW);
  s.init(ST4AuxDEs,debounceMs,true,LOW);
  e.init(ST4AuxRAe,debounceMs,true,LOW);
  w.init(ST4AuxRAw,debounceMs,true,LOW);
}

Ast4 auxST4;

#endif
