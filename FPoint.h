// -----------------------------------------------------------------------------------
// fixed point math data type

#include "Arduino.h"

#ifndef FPoint_h
#define FPoint_h

typedef struct {
  uint32_t f;
  uint32_t m;
} fixedBase_t;

typedef union {
  fixedBase_t part;
  uint64_t fixed;
} fixed_t;

#endif
