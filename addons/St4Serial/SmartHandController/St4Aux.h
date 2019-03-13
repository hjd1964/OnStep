// -----------------------------------------------------------------------------------
// ST4 aux port

#pragma once

#if defined(ST4AuxRAw) && defined(ST4AuxDEs) && defined(ST4AuxDEn) && defined(ST4AuxRAe)

#include "Config.h"
#include "PushButton.h"

class Ast4
{
  public:
    void setup();
    button n,s,e,w;
  private:
    int debounceMs = 20;
};

extern Ast4 auxST4;

#endif
