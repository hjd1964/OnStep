#include <EEPROM.h>

// This platform doesn't support true double precision math
#define NoDoublePrecision

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))
