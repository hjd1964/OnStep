// -----------------------------------------------------------------------------------
// ST4 aux port

/*
ST4 aux port
*/

#include "St4Aux.h"
#include <Arduino.h>
#include "Config.h"

#if !defined(ST4AuxRAw) || !defined(ST4AuxDEs) || !defined(ST4AuxDEn) || !defined(ST4AuxRAe)
  Ast4::Ast4(void) {}
  bool Ast4::guideNorth() { return false; }
  bool Ast4::guideSouth() { return false; }
  bool Ast4::guideEast() { return false; }
  bool Ast4::guideWest() { return false; }
#else
  Ast4::Ast4(void) {
    pinMode(ST4AuxRAw,INPUT);
    pinMode(ST4AuxDEs,INPUT);
    pinMode(ST4AuxDEn,INPUT);
    pinMode(ST4AuxRAe,INPUT);
  }
  bool Ast4::guideNorth() { v=digitalRead(ST4AuxRAw); delayMicroseconds(2); v1=digitalRead(ST4AuxRAw); return (v&v1==LOW); }
  bool Ast4::guideSouth() { v=digitalRead(ST4AuxDEs); delayMicroseconds(2); v1=digitalRead(ST4AuxDEs); return (v&v1==LOW); }
  bool Ast4::guideEast() { v=digitalRead(ST4AuxDEn); delayMicroseconds(2); v1=digitalRead(ST4AuxDEn); return (v&v1==LOW); }
  bool Ast4::guideWest() { v=digitalRead(ST4AuxRAe); delayMicroseconds(2); v1=digitalRead(ST4AuxRAe); return (v&v1==LOW); }
#endif

Ast4 auxST4;
