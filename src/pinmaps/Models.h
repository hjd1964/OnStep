// -------------------------------------------------------------------------------------------------
// Loads pinmap model for current configuration

#if PINMAP == Classic
  #include "Pins.Classic.h"
#endif
#if PINMAP == ClassicShield
  #include "Pins.Classic.h"
#endif
#if PINMAP == ClassicInstein
  #include "Pins.Classic.h"
#endif
#if PINMAP == Mega2560Alt
  #include "Pins.Mega2560Alt.h"
#endif
#if PINMAP == MiniPCB
  #include "Pins.MiniPCB.h"
#endif
#if PINMAP == MiniPCB13
  #include "Pins.MiniPCB.h"
#endif
#if PINMAP == MiniPCB2
  #include "Pins.MiniPCB.h"
#endif
#if PINMAP == MaxPCB
  #include "Pins.MaxPCB.h"
#endif
#if PINMAP == MaxPCB2
  #include "Pins.MaxPCB.h"
#endif
#if PINMAP == MaxPCB3
  #include "Pins.MaxPCB.h"
#endif
#if PINMAP == MaxESP2
  #include "Pins.MaxESP2.h"
#endif
#if PINMAP == MaxESP3
  #include "Pins.MaxESP3.h"
#endif
#if PINMAP == CNC3
  #include "Pins.CNC3.h"
#endif
#if PINMAP == Ramps14 || PINMAP == MksGenL1 || PINMAP == MksGenL2 || PINMAP == MksGenL21
  #include "Pins.Ramps14.h"
#endif
#if PINMAP == STM32Blue
  #include "Pins.STM32B.h"
#endif
#if PINMAP == FYSETC_S6
  #include "Pins.FYSETC_S6.h"
#endif
#if PINMAP == STM32Black
  #include "Pins.STM32B.h"
#endif
