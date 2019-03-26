
#include "u8g2_ext_input.h"
#include "u8g2_ext_event.h"
/*
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

uint8_t ext_UserInterfaceInputValueBoolean(u8g2_t *u8g2, Pad* extPad, const char *title, boolean *value)
{
  u8g2_SetFont(u8g2, u8g2_font_helvR12_te);
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t  x, xx;

  boolean local_value = *value;
  uint8_t event;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);

  /* calculate overall height of the input value box */
  height = 1;  /* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;

  /* calculate offset from top */
  y = 0;
  if (pixel_height < u8g2_GetDisplayHeight(u8g2))
  {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }

  /* calculate offset from left for the label */
  x = 0;
  pixel_width = u8g2_GetUTF8Width(u8g2, "X") * 3;
  if (pixel_width < u8g2_GetDisplayWidth(u8g2))
  {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }
  
  /* event loop */
  for (;;)
  {
    u8g2_FirstPage(u8g2);
    do {
      /* render */
      yy = y + line_height / 2;
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      yy += line_height / 2;
      xx = x;
      
      if (local_value) xx += u8g2_DrawUTF8(u8g2, xx, yy, "Yes"); else xx += u8g2_DrawUTF8(u8g2, xx, yy, "No");
      
    } while (u8g2_NextPage(u8g2));

#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) { *value = local_value; return 1; }
      else if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV) return 0;
      else if (event == U8X8_MSG_GPIO_MENU_UP || event == U8X8_MSG_GPIO_MENU_DOWN) { local_value=!local_value; break; }
    }
  }
}

uint8_t ext_UserInterfaceInputValueInteger(u8g2_t *u8g2, Pad* extPad, const char *title, const char *pre, uint8_t *value, uint8_t lo, uint8_t hi, uint8_t digits, const char *post)
{
  u8g2_SetFont(u8g2, u8g2_font_helvR12_te);
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t  x, xx;

  uint8_t local_value = *value;
  //uint8_t r; /* not used ??? */
  uint8_t event;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);

  /* calculate overall height of the input value box */
  height = 1;	/* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;

  /* calculate offset from top */
  y = 0;
  if (pixel_height < u8g2_GetDisplayHeight(u8g2))
  {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }

  /* calculate offset from left for the label */
  x = 0;
  pixel_width = u8g2_GetUTF8Width(u8g2, pre);
  pixel_width += u8g2_GetUTF8Width(u8g2, "0") * digits;
  pixel_width += u8g2_GetUTF8Width(u8g2, post);
  if (pixel_width < u8g2_GetDisplayWidth(u8g2)) {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }
  
  /* event loop */
  for (;;) {
    u8g2_FirstPage(u8g2);
    do {
      /* render */
      yy = y + line_height / 2;
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      yy += line_height / 2;
      xx = x;
      xx += u8g2_DrawUTF8(u8g2, xx, yy, pre);
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(local_value, digits));
      u8g2_DrawUTF8(u8g2, xx, yy, post);
    } while (u8g2_NextPage(u8g2));

#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) { *value = local_value; return 1; }
      else if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV) { return 0; }
      else if (event == U8X8_MSG_GPIO_MENU_UP) { if (local_value >= hi) local_value = lo; else local_value++; break; }
      else if (event == U8X8_MSG_GPIO_MENU_DOWN) { if (local_value <= lo) local_value = hi; else local_value--; break; }
    }
  }
}

/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/

uint8_t ext_UserInterfaceInputValueFloat(u8g2_t *u8g2, Pad* extPad, const char *title, const char *pre, float *value, float lo, float hi, uint8_t len, uint8_t dec, const char *post)
{
  u8g2_SetFont(u8g2, u8g2_font_helvR12_te);
  static char outstr[15];
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t  x, xx;
  float incr_ref = pow10f(dec*-1.);
  float incr = 0;
  float local_value = *value;
  //uint8_t r; /* not used ??? */
  uint8_t event;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);

  /* calculate overall height of the input value box */
  height = 1;	/* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;


  /* calculate offset from top */
  y = 0;
  if (pixel_height < u8g2_GetDisplayHeight(u8g2))
  {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }

  /* calculate offset from left for the label */
  x = 0;
  pixel_width = u8g2_GetUTF8Width(u8g2, pre);
  pixel_width += u8g2_GetUTF8Width(u8g2, "+");
  pixel_width += u8g2_GetUTF8Width(u8g2, "0") * len;
  pixel_width += u8g2_GetUTF8Width(u8g2, post);
  if (pixel_width < u8g2_GetDisplayWidth(u8g2))
  {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }
  /* event loop */
  for (;;)
  {
    u8g2_FirstPage(u8g2);
    do
    {
      /* render */
      yy = y + line_height / 2;
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      yy += line_height / 2;
      xx = x;
      xx += u8g2_DrawUTF8(u8g2, xx, yy, pre);
      if (lo < 0)
      {
        local_value < 0 ? u8g2_DrawUTF8(u8g2, xx, yy, "-") : u8g2_DrawUTF8(u8g2, xx, yy, "+");
        xx += u8g2_GetUTF8Width(u8g2, "+");
      }
      dtostrf(abs(local_value), len, dec, outstr);
      xx += u8g2_DrawUTF8(u8g2, xx, yy, outstr);
      u8g2_DrawUTF8(u8g2, xx, yy, post);
    } while (u8g2_NextPage(u8g2));

#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;)
    {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT)
      {
        *value = local_value;
        return 1;
      }
      else if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV)
      {
        return 0;
      }
      else if (event == U8X8_MSG_GPIO_MENU_UP)
      {
        if (local_value >= hi)
        {
          local_value = lo;
          incr = 0;
        }
        else
        {
          incr < 0 ? incr = 0 : incr += incr_ref;
          local_value += incr;
        }
        break;
      }
      else if (event == U8X8_MSG_GPIO_MENU_DOWN)
      {
        if (local_value <= lo)
        {
          local_value = hi;
          incr = 0;
        }
        else
        {
          incr > 0 ? incr = 0 : incr -= incr_ref;
          local_value += incr;
        }
        break;
      }
      else
      {
        incr = 0;
      }
    }
  }
}

/*
return:
0: value is not changed (HOME/Break Button pressed)
1: value has been updated
*/

uint8_t ext_UserInterfaceInputValueDMS(u8g2_t *u8g2, Pad* extPad, const char *title, long *value, long lo, long hi, 
                                       uint8_t digits1,
                                       char* symb1, char* symb2, char* symb3, 
                                       char* symb_plus, char* symb_minus, bool display_seconds) {
  u8g2_SetFont(u8g2, u8g2_font_helvR12_te);
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t pixel_width_sign;
  u8g2_uint_t  x, xx;
  float incr_ref = 1;
  if (!display_seconds) incr_ref = 60;
  float incr = 0;
  uint8_t v1 = 0;
  uint8_t v2 = 0;
  uint8_t v3 = 0;
  long local_value = *value;
  gethms(abs(local_value), v1, v2, v3);

  //uint8_t r; /* not used ??? */
  uint8_t event;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);

  /* calculate overall height of the input value box */
  height = 1;	/* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;

  /* calculate offset from top */
  y = 0;
  if (pixel_height < u8g2_GetDisplayHeight(u8g2)) {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }

  /* calculate offset from left for the label */
  x = 0;
  pixel_width_sign = u8g2_GetUTF8Width(u8g2, symb_plus);
  if (u8g2_GetUTF8Width(u8g2, symb_minus) > pixel_width_sign) pixel_width_sign = u8g2_GetUTF8Width(u8g2, symb_minus);
  pixel_width = pixel_width_sign;
  pixel_width += u8g2_GetUTF8Width(u8g2, symb1);
  pixel_width += u8g2_GetUTF8Width(u8g2, symb2);
  pixel_width += u8g2_GetUTF8Width(u8g2, "0") * 5;
  if (display_seconds) {
    pixel_width += u8g2_GetUTF8Width(u8g2, "0") * 2;
    pixel_width += u8g2_GetUTF8Width(u8g2, symb3);
  }

  if (pixel_width < u8g2_GetDisplayWidth(u8g2)) {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }
  
  /* event loop */
  for (;;) {
    u8g2_FirstPage(u8g2);
    do {
      /* render */
      yy = y + line_height / 2;
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      yy += line_height / 2;
      xx = x;
      local_value < 0 ? u8g2_DrawUTF8(u8g2, xx, yy, symb_minus) : u8g2_DrawUTF8(u8g2, xx, yy, symb_plus);
      xx += pixel_width_sign;
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(v1, digits1));
      xx += u8g2_DrawUTF8(u8g2, xx, yy, symb1);
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(v2, 2));
      xx += u8g2_DrawUTF8(u8g2, xx, yy, symb2);
      if (display_seconds) {
        xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(v3, 2));
        xx += u8g2_DrawUTF8(u8g2, xx, yy, symb3);
      }
    } while (u8g2_NextPage(u8g2));

#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) {
        *value = (long)v1 * 3600 + (long)v2 * 60;
        if (display_seconds) *value += v3;
        if (local_value < 0) *value *= -1;
        return 1;
      }
      else if (event == U8X8_MSG_GPIO_MENU_PREV) { return 0; }
      else if (event == U8X8_MSG_GPIO_MENU_UP)
      {
        if (local_value >= hi) {
          local_value = lo;
          incr = 0;
          gethms(abs(local_value), v1, v2, v3);
        } else {
          if (incr > 0) { if (display_seconds) incr *= 1.1; incr += incr_ref / 2; } else incr = incr_ref;

          if (incr > 3600) incr = 3600;
          local_value += incr;
          
          gethms(abs(local_value), v1, v2, v3);
        }
        break;
      }
      else if (event == U8X8_MSG_GPIO_MENU_DOWN) {
        if (local_value <= lo) {
          local_value = hi;
          incr = 0;
          gethms(abs(local_value), v1, v2, v3);
        } else {
          if (incr < 0) { if (display_seconds) incr *= 1.1; incr -= incr_ref / 2; } else incr = -incr_ref;

          if (incr < -3600) incr = -3600;
          local_value += incr;

          gethms(abs(local_value), v1, v2, v3);
        }
        break;
      }
      else { incr = 0; }
    }
  }
}

uint8_t ext_UserInterfaceInputValueDate(u8g2_t *u8g2, Pad* extPad, const char *title, uint8_t& year, uint8_t& month, uint8_t& day)
{
  char* symb = "/";
  u8g2_SetFont(u8g2, u8g2_font_helvR12_te);
  //display.enableUTF8Print();
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t  x, xx;
  float incr_ref = 1;

  float incr = 0;
  uint8_t local_year = year;
  uint8_t local_month = month;
  uint8_t local_day = day;

  uint8_t event;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);

  /* calculate overall height of the input value box */
  height = 1;	/* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;

  /* calculate offset from top */
  y = 0;
  if (pixel_height < u8g2_GetDisplayHeight(u8g2)) {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }

  /* calculate offset from left for the label */
  x = 0;

  pixel_width = 0;
  pixel_width += u8g2_GetUTF8Width(u8g2, symb);
  pixel_width += u8g2_GetUTF8Width(u8g2, symb);
  pixel_width += u8g2_GetUTF8Width(u8g2, "0") * 8;

  if (pixel_width < u8g2_GetDisplayWidth(u8g2)) {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }
  /* event loop */
  for (;;) {
    u8g2_FirstPage(u8g2);
    do {
      /* render */
      yy = y + line_height / 2;
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      yy += line_height / 2;
      xx = x;
      xx += u8g2_DrawUTF8(u8g2, xx, yy, "20");
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(local_year, 2));
      xx += u8g2_DrawUTF8(u8g2, xx, yy, symb);
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(local_month, 2));
      xx += u8g2_DrawUTF8(u8g2, xx, yy, symb);
      xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_u8toa(local_day, 2));
    } while (u8g2_NextPage(u8g2));

#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) {
        year = local_year;
        month = local_month;
        day = local_day;
        return 1;
      }
      else if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV) { return 0; }
      else if (event == U8X8_MSG_GPIO_MENU_UP)
      {
        if (incr > 0) { incr = 1.05*incr + incr_ref / 2; } else incr = incr_ref;
        if (incr > 30) incr = 30;

        add_days(local_year, local_month, local_day, incr);
        break;
      }
      else if (event == U8X8_MSG_GPIO_MENU_DOWN)
      {
        if (incr < 0) { incr = 1.05*incr - incr_ref / 2; } else incr = -incr_ref;
        if (incr < -30) incr = -30;

        supress_days(local_year, local_month, local_day, -incr);
        break;
      }
      else { incr = 0; }
    }
  }
}

uint8_t ext_UserInterfaceInputValueRA(u8g2_t *u8g2, Pad *extPad, long *value)
{
  return ext_UserInterfaceInputValueDMS(u8g2, extPad, "Right Asc.", value, 0, 86399, 2, "h", "m", "s", "", "", true);
}

uint8_t ext_UserInterfaceInputValueDec(u8g2_t *u8g2, Pad *extPad, long *value)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  return ext_UserInterfaceInputValueDMS(u8g2, extPad, "Declination", value, -324000, 324000, 2, DEGREE_SYMBOL, "'", "\"", "+", "-", true);
}

uint8_t ext_UserInterfaceInputValueTime(u8g2_t *u8g2, Pad *extPad, long *value)
{
  return ext_UserInterfaceInputValueDMS(u8g2, extPad, "Local Time", value, 0, 43199, 2, ":", ":", "", "", "", true);
}

uint8_t ext_UserInterfaceInputValueLatitude(u8g2_t *u8g2, Pad *extPad, long *value)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  return ext_UserInterfaceInputValueDMS(u8g2, extPad, "Latitude", value, -324000, 324000, 2, DEGREE_SYMBOL, "'", "\"", "N ", "S ", false);
}

uint8_t ext_UserInterfaceInputValueLongitude(u8g2_t *u8g2, Pad *extPad, long *value)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  return ext_UserInterfaceInputValueDMS(u8g2, extPad, "Longitude", value, -648000, 648000, 3, DEGREE_SYMBOL, "'", "\"", "W ", "E ", false);
}

void _gethms(const long& v, uint8_t& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = v % 60;
  v2 = (v / 60) % 60;
  v1 = v / 3600;
}

/*
void _getdms(const long& v, short& v1, uint8_t& v2, uint8_t& v3)
{
  v3 = abs(v) % 60;
  v2 = (abs(v) / 60) % 60;
  v1 = v / 3600;
}
*/

void add_days(uint8_t& year, uint8_t& month, uint8_t& day, int days2add)
{
#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) )) 
  uint16_t months[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (LEAP_YEAR(2000 + (int)year))
  {
    months[2] = 29;
  }

  while (days2add > 0)
  {
    if (day < months[month])
    {
      day++;
      days2add--;
      continue;
    }
    if (month < 12)
    {
      day = 1;
      month++;
      days2add--;
      continue;
    }
    if (year == 99)
    {
      return;
    }
    year += 1;
    month = 1;
    day = 1;
    days2add--;
    if (LEAP_YEAR(2000 + (int)year))
      months[2] = 29;
    else
      months[2] = 28;
  }
}

void supress_days(uint8_t& year, uint8_t& month, uint8_t& day, int days2supress)
{
#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) )) 
  uint16_t months[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (LEAP_YEAR(2000 + (int)year))
  {
    months[2] = 29;
  }

  while (days2supress > 0)
  {

    if (day > 1)
    {
      day--;
      days2supress--;
      continue;
    }
    if (month > 1)
    {
      month--;
      day = months[month];
      days2supress--;
      continue;
    }
    if (year == 0)
    {
      return;
    }
    year -= 1;
    month = 12;
    day = 31;
    days2supress--;
    if (LEAP_YEAR(2000 + (int)year))
      months[2] = 29;
    else
      months[2] = 28;
  }
}

