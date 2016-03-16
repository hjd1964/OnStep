// -----------------------------------------------------------------------------------
// Launchpad (TIVA) libraries

#if defined(__TM4C123GH6PM__) || defined(__LM4F120H5QR__) || defined(__TM4C1294NCPDT__) || defined(__TM4C1294XNCZAD__)

#include "Energia.h"
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include "inc/hw_ints.h"

// Energia only has EEPROM.read and EEPROM.write, it does not include EEPROM.update. My patch has been accepted but it will take a while until the next version is released. Until then you can use the included EEPROM_LP.ino and EEPROM_LP.h.
#include "EEPROM_LP.h"

#else

#include "EEPROM.h"

#endif
