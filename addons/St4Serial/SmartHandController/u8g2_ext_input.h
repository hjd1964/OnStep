#pragma once
#include <U8g2lib.h>
#include "u8g2_ext_event.h"
/*

u8g2_ext.c

Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

Copyright (c) 2018,
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/





/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/

uint8_t ext_UserInterfaceInputValueBoolean(u8g2_t *u8g2, Pad *extPad, const char *title, boolean *value);

/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/

uint8_t ext_UserInterfaceInputValueInteger(u8g2_t *u8g2, Pad *extPad, const char *title, const char *pre, uint8_t *value, uint8_t lo, uint8_t hi, uint8_t digits, const char *post);

/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/

uint8_t ext_UserInterfaceInputValueFloat(u8g2_t *u8g2, Pad *extPad, const char *title, const char *pre, float *value, float lo, float hi, uint8_t len, uint8_t dec, const char *post);

/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/



uint8_t ext_UserInterfaceInputValueDMS(u8g2_t *u8g2, Pad *extPad, const char *title, long *value, long lo, long hi,
  uint8_t digits1,

  char* symb1, char* symb2, char* symb3,
  char* symb_plus, char* symb_minus,
  bool display_seconds);


uint8_t ext_UserInterfaceInputValueDate(u8g2_t *u8g2, Pad *extPad, const char *title, uint8_t& year, uint8_t& month, uint8_t& day);

uint8_t ext_UserInterfaceInputValueRA(u8g2_t *u8g2, Pad *extPad, long *value);


uint8_t ext_UserInterfaceInputValueDec(u8g2_t *u8g2, Pad *extPad, long *value);


uint8_t ext_UserInterfaceInputValueTime(u8g2_t *u8g2, Pad *extPad, long *value);


uint8_t ext_UserInterfaceInputValueLatitude(u8g2_t *u8g2, Pad *extPad, long *value);


uint8_t ext_UserInterfaceInputValueLongitude(u8g2_t *u8g2, Pad *extPad, long *value);


void gethms(const long& v, uint8_t& v1, uint8_t& v2, uint8_t& v3);


//void getdms(const long& v, short& v1, uint8_t& v2, uint8_t& v3);


void add_days(uint8_t& year, uint8_t& month, uint8_t& day, int days2add);

void supress_days(uint8_t& year, uint8_t& month, uint8_t& day, int days2supress);


