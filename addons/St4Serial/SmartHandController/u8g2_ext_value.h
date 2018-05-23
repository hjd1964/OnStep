#pragma once
#include <U8g2lib.h>

uint8_t ext_drawRA(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* Rah, const char* Ram, const char* Ras);

uint8_t ext_drawDec(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* decsign, const char* decdeg, const char* decmin, const char* decsec);

uint8_t ext_drawAz(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* Azdeg, const char* Azmin, const char* Azsec);