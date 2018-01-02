#include <EEPROM.h>

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#define RefractionRateRange 30

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

