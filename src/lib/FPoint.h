// -----------------------------------------------------------------------------------
// Fixed point math data type

#pragma once

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

// floating point range of +/-255.999999x
uint64_t doubleToFixed(double d) {
  fixed_t x;
  x.fixed = (long)(d*8388608.0);  // shift 23 bits
  x.fixed = x.fixed<<9;
  return x.fixed;
}

// floating point range of +/-255.999999x
double fixedToDouble(fixed_t a) {
  long l = a.fixed>>9;          // shift 9 bits
  return ((double)l/8388608.0); // and 23 more, for 32 bits total
}

#endif
