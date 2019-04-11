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
#include "u8g2_ext_event.h"
#include "LX200.h"
#include "Catalog.h"

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

enum CATALOG_DISPLAY_MODES {DM_INFO, DM_EQ_COORDS, DM_HOR_COORDS};

/*
selection list with string line
returns line height
*/

static uint8_t ext_draw_catalog_list_line(u8g2_t *u8g2, uint8_t y, CATALOG_DISPLAY_MODES displayMode, bool firstPass)
{
  u8g2_uint_t x = 0;

  uint8_t step0;
  const uint8_t* myfont = u8g2->font;
  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

  char line[16];

  int dx = u8g2_GetDisplayWidth(u8g2);
  if (cat_mgr.isStarCatalog())
  {  
    // Normal star mode
    // |bAnd            15.2|
    // |Alpheratz       25ly|
    // |                    |
  
    // Double star mode
    // |bAnd       11.2,15.2|
    // |----name----  2225ly|
    // |Sep 2.5" PA 225     |
  
    // Variable star mode
    // |bAnd       11.2,15.2|
    // |----name----  2225ly|
    // |Per 2.5d            |

    // Bayer designation of the star (Greek letter) or Fleemstead designation of star (number)
    u8g2_SetFont(u8g2, u8g2_font_unifont_t_greek);
    x = 0;
    int p=cat_mgr.primaryId();
    if (p<0) u8g2_DrawUTF8(u8g2, x, y, "?"); else
    if (p<24) u8g2_DrawGlyph(u8g2, x, y, 945 + p); else 
    { sprintf(line,"%d",p); u8g2_DrawUTF8(u8g2, x, y, line); }

    // Constellation Abbreviation
    u8g2_SetFont(u8g2, u8g2_font_helvR10_tf);
    x = 10;
    u8g2_DrawUTF8(u8g2, x, y, cat_mgr.constellationStr());
  
    // Magnitude
    float mf=cat_mgr.magnitude();
    if (abs(mf-99.90)<0.001) sprintf(line, "?.?"); else dtostrf(mf, 3, 1, line);
    step0 = u8g2_GetUTF8Width(u8g2, line);
    u8g2_DrawUTF8(u8g2, dx-step0, y, line);

    // RA/Dec were here
    // Display lines 2 and 3
    if (displayMode==DM_INFO) {
      y += line_height;
      x = 0;
      // Common name for the star
      u8g2_DrawUTF8(u8g2, x, y, cat_mgr.objectNameStr());
    }
  }
  else {
    // DSO Catalogs (current)
    // |M44 N2632        2.2|
    // |Open Cluster     CnC|
    // |Beehive Cluster     |
    
    // Prefix and catalog number
    x = 0;
    sprintf(line, "%s%u", cat_mgr.catalogPrefix(), cat_mgr.primaryId());
    step0 = u8g2_GetUTF8Width(u8g2, "N8888");
    int x1= u8g2_GetUTF8Width(u8g2, line);
    u8g2_DrawUTF8(u8g2, x, y, line);
     
    // Object SubID
    u8g2_SetFont(u8g2, u8g2_font_helvR08_tr);
    sprintf(line, "%s", cat_mgr.subIdStr());
    u8g2_DrawUTF8(u8g2, x+x1, y, line);
    x += step0;
    step0 = u8g2_GetUTF8Width(u8g2, "A&B-C");
    u8g2_SetFont(u8g2, myfont);

    // Magnitude
    float mf=cat_mgr.magnitude();
    if (abs(mf-99.90)<0.001) sprintf(line, "?.?"); else dtostrf(mf, 3, 1, line);
    step0 = u8g2_GetUTF8Width(u8g2, line);
    u8g2_DrawUTF8(u8g2, dx-step0, y, line);

    // Display lines 2 and 3
    if (displayMode==DM_INFO) {
      y += line_height;
      x = 0;

      // Object type text
      u8g2_DrawUTF8(u8g2, x, y, cat_mgr.objectTypeStr());
  
      // Constellation Abberviation
      sprintf(line,"%s",cat_mgr.constellationStr());
      step0 = u8g2_GetUTF8Width(u8g2, line);
      u8g2_DrawUTF8(u8g2, dx-step0, y, line);
  
      // Object Name, when the DSO catalogs include it
      y += line_height;
      x = 0;
      u8g2_DrawUTF8(u8g2, x, y, cat_mgr.objectNameStr());
    }
  }

  if (displayMode==DM_EQ_COORDS) {
    char txt1[5], txt2[5], txt3[5];
    uint8_t vr1, vr2, vr3;
    short vd1; uint8_t vd2, vd3;

    u8g2_SetFont(u8g2, myfont);
    step0 = u8g2_GetUTF8Width(u8g2, "XXXXX ");

    // RA
    cat_mgr.raHMS(vr1,vr2,vr3);
    sprintf(line," %02d:%02d:%02d",vr1,vr2,vr3);
    y += line_height;
    u8g2_DrawUTF8(u8g2, 8, y, "RA");
    ext_drawFixedWidthNumeric(u8g2, step0, y, line);

    // Declination
    cat_mgr.decDMS(vd1,vd2, vd3);
    sprintf(line,"%s%02d\xb0%02d'%02d",vd1 < 0 ? "-" : "+",abs(vd1),vd2,vd3);
    y += line_height;
    u8g2_DrawUTF8(u8g2, 8, y, "DE");
    ext_drawFixedWidthNumeric(u8g2, step0, y, line);
  }

  if (displayMode==DM_HOR_COORDS) {
    static short va1, vz1; 
    static uint8_t va2, va3, vz2, vz3;

    u8g2_SetFont(u8g2, myfont);
    step0 = u8g2_GetUTF8Width(u8g2, "XXXXX ");

    // Az
    if (firstPass) cat_mgr.azmDMS(vz1, vz2, vz3);
    sprintf(line,"%03d\xb0%02d'%02d",vz1,vz2,vz3);
    y += line_height;
    u8g2_DrawUTF8(u8g2, 8, y, "Az.");
    ext_drawFixedWidthNumeric(u8g2, step0, y, line);

    // Alt
    if (firstPass) cat_mgr.altDMS(va1, va2, va3);
    sprintf(line,"%s%02d\xb0%02d'%02d",va1 < 0 ? "-" : "+",abs(va1),va2,va3);
    y += line_height;
    u8g2_DrawUTF8(u8g2, 8, y, "Alt.");
    ext_drawFixedWidthNumeric(u8g2, step0, y, line);
  }

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
  u8g2_SetFont(u8g2, u8g2_font_helvR10_tf);
  u8g2_uint_t yy;

  uint8_t event;
  CATALOG_DISPLAY_MODES thisDisplayMode=DM_INFO;

  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;
  uint8_t title_lines = u8x8_GetStringLineCnt(title);

  u8g2_SetFontPosBaseline(u8g2);

  for (;;) {
    bool firstPass=true;
    u8g2_FirstPage(u8g2);
    do  {
      yy = u8g2_GetAscent(u8g2);
      if (title_lines > 0) {
        yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
        u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));
        yy += 3;
      }
      ext_draw_catalog_list_line(u8g2, yy, thisDisplayMode, firstPass);
      firstPass=false;
    } while (u8g2_NextPage(u8g2));
    
#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    int scrollSpeed;
    if (cat_mgr.getMaxIndex()>1000) scrollSpeed=20; else scrollSpeed=2;

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) return true; else
      if (event == U8X8_MSG_GPIO_MENU_HOME) { thisDisplayMode=(CATALOG_DISPLAY_MODES)((int)thisDisplayMode+1); if (thisDisplayMode>DM_HOR_COORDS) thisDisplayMode=DM_INFO; break; }
      if (event == U8X8_MSG_GPIO_MENU_PREV) return false; else
      if (event == U8X8_MSG_GPIO_MENU_DOWN) { cat_mgr.incIndex(); break; } else
      if (event == MSG_MENU_DOWN_FAST)      { for (int i=0; i<scrollSpeed; i++) cat_mgr.incIndex(); break; } else
      if (event == U8X8_MSG_GPIO_MENU_UP)   { cat_mgr.decIndex(); break; } else
      if (event == MSG_MENU_UP_FAST)        { for (int i=0; i<scrollSpeed; i++) cat_mgr.decIndex(); break; }

      // auto-refresh display
      static unsigned long lastRefresh=0;
      if ((thisDisplayMode==DM_HOR_COORDS) && (millis()-lastRefresh>2000)) { lastRefresh=millis(); break; }
    }
  }
}

/*
selection list with string line
returns line height
*/

static uint8_t ext_draw_user_catalog_list_line(u8g2_t *u8g2, uint8_t y)
{
  u8g2_uint_t x = 0;

  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

  char line1[16];
  char line2[16];

  // user object catalog
  GetLX200(":LI#",line1);

  bool nextString=false;
  int j=0;
  int len=strlen(line1); if (len>15) len=15;
  for (int i=0; i<len; i++) {
    if (line1[i]==',') { line1[i]=0; nextString=true; continue; }
    if (nextString) { if (line1[i]=='#') { line2[j++]=0; break; } line2[j++]=line1[i]; }
  }

  // "UNK",  "OC",  "GC",  "PN",  "DN",  "SG",  "EG",  "IG", "KNT", "SNR", "GAL",  "CN", "STR", "PLA", "CMT", "AST"
  if (strstr(line2,"UNK")) strcpy(line2,"Unknown"); else
  if (strstr(line2,"OC"))  strcpy(line2,"Open Cluster"); else
  if (strstr(line2,"GC"))  strcpy(line2,"Globular Cluster"); else
  if (strstr(line2,"PN"))  strcpy(line2,"Planetary Nebula"); else
  if (strstr(line2,"SG"))  strcpy(line2,"Spirial Galaxy"); else
  if (strstr(line2,"EG"))  strcpy(line2,"Eliptical Galaxy"); else
  if (strstr(line2,"IG"))  strcpy(line2,"Irregular Galaxy"); else
  if (strstr(line2,"KNT")) strcpy(line2,"Knot"); else
  if (strstr(line2,"SNR")) strcpy(line2,"SuperNova Rmnnt"); else
  if (strstr(line2,"GAL")) strcpy(line2,"Galaxy"); else
  if (strstr(line2,"CN"))  strcpy(line2,"Cluster + Nebula"); else
  if (strstr(line2,"STR")) strcpy(line2,"Star"); else
  if (strstr(line2,"PLA")) strcpy(line2,"Planet"); else
  if (strstr(line2,"CMT")) strcpy(line2,"Comet"); else
  if (strstr(line2,"CMT")) strcpy(line2,"Asteroid");
    
  // null object
  if ((line1[0]==0) || (line1[0]=='$')) return 0;

  // object name
  x = 0;
  u8g2_DrawUTF8(u8g2, x, y, line1);

  // Object type text
  y += line_height;
  x = 0;
  u8g2_DrawUTF8(u8g2, x, y, line2);

  return line_height;
}

/*
title: NULL for no title, valid str for title line. Can contain mutliple lines, separated by '\n'
returns false if user has pressed the home key
returns true if user has pressed the select key
side effects:
u8g2_SetFontDirection(u8g2, 0);
u8g2_SetFontPosBaseline(u8g2);
*/
bool ext_UserInterfaceUserCatalog(u8g2_t *u8g2, Pad* extPad, const char *title)
{
  u8g2_SetFont(u8g2, u8g2_font_helvR10_tf);
  u8g2_uint_t yy;

  uint8_t event;

  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;
  uint8_t title_lines = u8x8_GetStringLineCnt(title);

  u8g2_SetFontPosBaseline(u8g2);
  
  char oppositeMove[20]="";
  
  for (;;) {
    u8g2_FirstPage(u8g2);
    do  {
      yy = u8g2_GetAscent(u8g2);
      if (title_lines > 0) {
        yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
        u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));
        yy += 3;
      }
      if (ext_draw_user_catalog_list_line(u8g2, yy)==0) {
        // display of item failed due to it being a key or beyond end of list
        if (strlen(oppositeMove)>0) { SetLX200(oppositeMove); strcpy(oppositeMove,""); }
      }
    } while (u8g2_NextPage(u8g2));
    
#ifdef U8G2_REF_MAN_PIC
    return 0;
#endif

    for (;;) {
      event = ext_GetMenuEvent(extPad);
      if (event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT) return true; else
      if (event == U8X8_MSG_GPIO_MENU_HOME || event == U8X8_MSG_GPIO_MENU_PREV) return false; else
      if (event == U8X8_MSG_GPIO_MENU_DOWN || event == MSG_MENU_DOWN_FAST) { strcpy(oppositeMove,":LB#"); SetLX200(":LN#"); break; } else
      if (event == U8X8_MSG_GPIO_MENU_UP || event == MSG_MENU_UP_FAST) { strcpy(oppositeMove,":LN#"); SetLX200(":LB#"); break; }
    }
  }
}
