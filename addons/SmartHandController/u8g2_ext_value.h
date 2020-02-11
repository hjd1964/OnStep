#pragma once
#include <U8g2lib.h>

uint8_t ext_DrawFwNumeric(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* text);
uint8_t ext_GetFwNumericWidth(u8g2_t *u8g2, const char* text);
