// -----------------------------------------------------------------------------------
// ST4 aux port

#pragma once

#include "Constants.h"
#include "Locales.h"
#include "Config.h"
#include "Locale.h"
#include "Pinmap.h"
#include "PushButton.h"

#if defined(ST4AuxRAw) && defined(ST4AuxDEs) && defined(ST4AuxDEn) && defined(ST4AuxRAe)

class Ast4
{
  public:
    void setup();
    void tickButtons();
    button n,s,e,w;
  private:
    int debounceMs = 20;
};

extern Ast4 auxST4;

#endif
