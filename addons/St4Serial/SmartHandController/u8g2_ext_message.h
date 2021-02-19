#pragma once
#include <U8g2lib.h>
#include "u8g2_ext_event.h"
u8g2_uint_t ext_UserInterfaceMessage(u8g2_t *u8g2, Pad *extPad,const char *title1, const char *title2, const char *title3, const char *buttons);
