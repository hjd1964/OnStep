/*

u8g2_selection_list.c

selection list with scroll option

Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

Copyright (c) 2016, olikraus@gmail.com
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
#include <U8g2lib.h>
#include "u8g2_ext_catalog.h"
#include "u8g2_ext_value.h"
#include "Catalog.h"
#include "u8g2_ext_event.h"

#define OC_width 18
#define OC_height 10
static unsigned char OC_bits[] U8X8_PROGMEM = {
  0x80, 0x02, 0x00, 0x00, 0x08, 0x00, 0x20, 0x00, 0x00, 0x80, 0x24, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x90, 0x04, 0x00, 0x00, 0x10, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x05, 0x00 };

#define GC_width 18
#define GC_height 10
static unsigned char GC_bits[] U8X8_PROGMEM = {
  0x80, 0x07, 0x00, 0x40, 0x09, 0x00, 0x20, 0x11, 0x00, 0x10, 0x21, 0x00,
  0x10, 0x3f, 0x00, 0xf0, 0x23, 0x00, 0x10, 0x22, 0x00, 0x20, 0x12, 0x00,
  0x40, 0x0a, 0x00, 0x80, 0x07, 0x00 };

#define EN_width 18
#define EN_height 10
static unsigned char EN_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x03, 0xff, 0xff, 0x03, 0xff, 0xff, 0x01, 0xff, 0x7f, 0x00,
  0xff, 0x70, 0x00, 0x7e, 0x60, 0x00, 0x7c, 0x20, 0x00, 0x78, 0x20, 0x00,
  0xf0, 0x00, 0x00, 0xe0, 0x01, 0x00 };

#define PN_width 18
#define PN_height 10
static unsigned char PN_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0xf8, 0x7f, 0x00, 0x1e, 0xe0, 0x01,
  0x07, 0x83, 0x03, 0x07, 0x83, 0x03, 0x1e, 0xe0, 0x01, 0xf8, 0x7f, 0x00,
  0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00 };

#define GX_width 18
#define GX_height 10
static unsigned char GX_bits[] U8X8_PROGMEM = {
  0xf0, 0x3f, 0x00, 0x38, 0x60, 0x00, 0x1c, 0xc0, 0x00, 0x9c, 0x07, 0x00,
  0xf8, 0x3f, 0x00, 0xf0, 0x7f, 0x00, 0x80, 0xe7, 0x00, 0x0c, 0xe0, 0x00,
  0x18, 0x70, 0x00, 0xf0, 0x3f, 0x00 };



#define MY_BORDER_SIZE 1


/*
selection list with string line
returns line height
*/

static uint8_t ext_draw_catalog_list_line(u8g2_t *u8g2, uint8_t y)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  u8g2_uint_t x = 0;
  u8g2_uint_t yy;

  u8g2_uint_t  pixel_width;
  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

  // for Star Catalog
  if (cat_mgr.getCat() == STAR)
  {
    uint8_t step0 = u8g2_GetUTF8Width(u8g2, "dec ");
    char line[16];
    const uint8_t* myfont = u8g2->font;
    u8g2_SetFont(u8g2, u8g2_font_unifont_t_greek);
    u8g2_DrawGlyph(u8g2, 0, y, 944 + cat_mgr.primaryId());
    u8g2_SetFont(u8g2, myfont);

    u8g2_DrawUTF8(u8g2, 16, y, cat_mgr.constellationStr());

    // show RA
    uint8_t vr1, vr2, vr3;
    char txt1[5], txt2[5], txt3[5];
    y += line_height;
    cat_mgr.raHMS(vr1,vr2,vr3);
    memcpy(txt1, u8x8_u8toa(vr1, 2), 3);
    memcpy(txt2, u8x8_u8toa(vr2, 2), 3);
    memcpy(txt3, u8x8_u8toa(vr3, 2), 3);
    u8g2_DrawUTF8(u8g2, x, y, "ra");
    x += step0;
    ext_drawRA(u8g2, x, y, txt1, txt2, txt3);

    // show Dec
    short vd1; uint8_t vd2;
    cat_mgr.decDM(vd1,vd2);
    y += line_height;
    x = 0;
    memcpy(txt1, u8x8_u8toa((uint8_t)abs(vd1), 2), 3);
    memcpy(txt2, u8x8_u8toa(vd2, 2), 3);
    u8g2_DrawUTF8(u8g2, x, y, "dec ");
    x += step0;
    ext_drawDec(u8g2, x, y, vd1 < 0 ? "-" : "+", txt1, txt2, "00");
    return line_height;
  }

  // for Object Catalogs
  char line[16];
  sprintf(line, "%s%u", cat_mgr.catalogStr(), cat_mgr.primaryId());
  if (line == NULL) strcpy(line, ""); u8g2_DrawUTF8(u8g2, 0, y, line);
  y += line_height;

  // show icon for type of object
  u8g2_DrawUTF8(u8g2, 0, y, cat_mgr.constellationStr());
  x += u8g2_GetUTF8Width(u8g2, "WWW");
  if (cat_mgr.objectType()<8) {
    switch (cat_mgr.objectType()) {
      case 0: u8g2_DrawXBMP(u8g2, x - 3, y - EN_height, EN_width, EN_height, EN_bits); break;
      case 1: u8g2_DrawXBMP(u8g2, x - 3, y - GC_height, GC_width, GC_height, GC_bits); break;
      case 2: u8g2_DrawXBMP(u8g2, x - 3, y - GX_height, GX_width, GX_height, GX_bits); break;
      case 3: u8g2_DrawXBMP(u8g2, x - 3, y - OC_height, OC_width, OC_height, OC_bits); break;
      case 4: u8g2_DrawXBMP(u8g2, x - 3, y - PN_height, PN_width, PN_height, PN_bits); break;
      default: break;
    }
    x += GX_width + 5;
  }

  // show magnitude
  u8g2_DrawUTF8(u8g2, x, y, "mag ");
  x += u8g2_GetUTF8Width(u8g2, "mag ");
  sprintf(line, "%0.1f", (float)cat_mgr.magnitude());
  u8g2_DrawUTF8(u8g2, x, y, line);

  // show extra object information
  y += line_height;
  if (cat_mgr.objectType()<8) u8g2_DrawUTF8(u8g2, 0, y, cat_mgr.objectInfoStr());

  y += line_height;
  return line_height;
}

/*
title: NULL for no title, valid str for title line. Can contain mutliple lines, separated by '\n'
uses: cat_mgr which has catalog selected, cursor line set, and filter set.  On return cat_mgr has object of interest selected.
returns false if user has pressed the home key
returns true if user has pressed the select key
side effects:
u8g2_SetFontDirection(u8g2, 0);
u8g2_SetFontPosBaseline(u8g2);
*/
bool ext_UserInterfaceCatalog(u8g2_t *u8g2, Pad* extPad, const char *title)
{
  u8g2_SetFont(u8g2, u8g2_font_helvR10_te);
  u8g2_uint_t yy;

  uint8_t event;

  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;
  uint8_t title_lines = u8x8_GetStringLineCnt(title);
  uint8_t display_lines;

  u8g2_SetFontPosBaseline(u8g2);

  for (;;) {
    u8g2_FirstPage(u8g2);
    do  {
      yy = u8g2_GetAscent(u8g2);
      if (title_lines > 0) {
        yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
        u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));
        yy += 3;
      }
      ext_draw_catalog_list_line(u8g2, yy);
    } while (u8g2_NextPage(u8g2));
    
#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) return true; else
      if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV) return false; else
      if (event == U8X8_MSG_GPIO_MENU_DOWN) { cat_mgr.incIndex(); break; } else
      if (event == U8X8_MSG_GPIO_MENU_UP) { cat_mgr.decIndex(); break; }
    }
  }
}

