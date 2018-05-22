#include "u8g2_ext_value.h"

uint8_t ext_drawRA(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* Rah, const char* Ram, const char* Ras)
{
  u8g2_uint_t step0 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00") + u8g2_GetUTF8Width(u8g2, "0") / 3;
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0") - u8g2_GetUTF8Width(u8g2, "0") / 3;
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step0 + 1;
    u8g2_DrawUTF8(u8g2, x, y, Ras);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, Ram);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, Rah);
  }
  else
  {
    x += u8g2_GetUTF8Width(u8g2, "+") + u8g2_GetUTF8Width(u8g2, "0") / 3;
    u8g2_DrawUTF8(u8g2, x, y, Rah);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, Ram);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x += step2;
    x += u8g2_DrawUTF8(u8g2, x, y, Ras);
    return x;
  }

}

uint8_t ext_drawDec(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* decsign, const char* decdeg, const char* decmin, const char* decsec)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0");
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step1 + 1;
    u8g2_DrawUTF8(u8g2, x, y, decsec);
    x -= step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x -= step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, decmin);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, decdeg);
    x -= u8g2_GetUTF8Width(u8g2, "+");
    if (decsign[0] == '-')
    {
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
    else
    {
      x -= u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
  }
  else
  {
    if (decsign[0] == '-')
    {
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
    else
    {
      u8g2_DrawUTF8(u8g2, x, y, decsign);
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
    }
    x += u8g2_GetUTF8Width(u8g2, "+");
    u8g2_DrawUTF8(u8g2, x, y, decdeg);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, decmin);
    x += step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x += step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    x += u8g2_DrawUTF8(u8g2, x, y, decsec);
  }

}

uint8_t ext_drawAz(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* Azdeg, const char* Azmin, const char* Azsec)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  u8g2_uint_t step0 = u8g2_GetUTF8Width(u8g2, "000");
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0");
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step1 + 1;
    u8g2_DrawUTF8(u8g2, x, y, Azsec);
    x -= step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x -= step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, Azmin);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x -= step0;
    u8g2_DrawUTF8(u8g2, x, y, Azdeg);
  }
  else
  {
    u8g2_DrawUTF8(u8g2, x, y, Azdeg);
    x += step1 + u8g2_GetUTF8Width(u8g2, "0");
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, Azmin);
    x += step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x += step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    x += u8g2_DrawUTF8(u8g2, x, y, Azsec);
  }
}
