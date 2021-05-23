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

  char line[32];

  int dx = u8g2_GetDisplayWidth(u8g2);
  if (cat_mgr.isStarCatalog())
  {  
    // Normal star mode
    // |bAnd              15.2|
    // |Alpheratz         25ly|
    // |                      |
  
    // Double star mode
    // |Σ0060AB ηCas 3.52,7.36|
    // |----name----    2225ly|
    // |Sep 2.5" PA 225       |
  
    // Variable star mode
    // |bAnd         11.2,15.2|
    // |----name----    2225ly|
    // |Per 2.5d              |

    x = 0;

    // Prefix
    sprintf(line,cat_mgr.catalogPrefix());
    if (strstr(line,"Star")) {
      // nothing
    } else
    if (strstr(line,"STF")) {
      u8g2_SetFont(u8g2, LF_GREEK);
      x+=u8g2_DrawGlyph(u8g2, x, y, 931); // sigma
      u8g2_SetFont(u8g2, myfont);
    } else
    if (strstr(line,"STT")) {
      u8g2_SetFont(u8g2, LF_GREEK);
      x+=u8g2_DrawGlyph(u8g2, x, y, 927); // omicron
      x+=u8g2_DrawGlyph(u8g2, x, y, 931); // sigma
      u8g2_SetFont(u8g2, myfont);
    } else {
      x+=u8g2_DrawUTF8(u8g2, x, y, line);
    }

    // Catalog number
    if (!strstr(line,"Star") && !cat_mgr.hasPrimaryIdInPrefix()) {
      long p=cat_mgr.primaryId(); if (p>0) sprintf(line, "%ld", p); else sprintf(line,"?");
      x+=u8g2_DrawUTF8(u8g2, x, y, line);
    }

    // Star SubId
    sprintf(line, "%s", cat_mgr.subIdStr());
    u8g2_SetFont(u8g2, LF_CATALOGS);
    x+=u8g2_DrawUTF8(u8g2, x, y, line);
    u8g2_SetFont(u8g2, myfont);

    // If anything is present on this line so far, add a space
    if (x!=0) {
      sprintf(line, "%s", " ");
      x+=u8g2_DrawUTF8(u8g2, x, y, line);
    }
    
    // Bayer designation of the star (Greek letter) or Fleemstead designation of star (number) just before Con Abv.
    int p=cat_mgr.bayerFlam();
    if ((p>=0) && (p<24)) {
      u8g2_SetFont(u8g2, LF_GREEK);
      x+=u8g2_DrawGlyph(u8g2, x, y, 945 + p);
      u8g2_SetFont(u8g2, myfont);
    } else {
      if (p>24) {
        sprintf(line,"%d",p-24);
        u8g2_SetFont(u8g2, LF_CATALOGS);
        x+=u8g2_DrawUTF8(u8g2, x, y, line);
        u8g2_SetFont(u8g2, myfont);
      } 
    }

    // Constellation Abbreviation
    u8g2_DrawUTF8(u8g2, x, y, cat_mgr.constellationStr());
  
    // Magnitudes
    float mf =cat_mgr.magnitude();
    float mf2=cat_mgr.magnitude2();
    if ((mf2>99) && (mf>99)) {
      sprintf(line, "?.?");
    } else {
      if (mf2>99) {
        dtostrf(mf, 4, 1, line);
        ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y, line);
      } else {
        if (displayMode==DM_INFO) {
          char mfs[8], mf2s[8];
          dtostrf(mf, 4, 1, mfs);
          dtostrf(mf2, 4, 1, mf2s);
          u8g2_SetFont(u8g2, LF_CATALOGS);
          sprintf(line,"%s",mf2s);
          x=ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y+line_height, line);
          sprintf(line,"%s",mfs);
          ext_DrawFwNumeric(u8g2, dx-(ext_GetFwNumericWidth(u8g2, line)+x+4), y+line_height, line);
          u8g2_SetFont(u8g2, myfont);
        }
      }
    }

    // Display line 2 and 3
    x = 0;
    if (displayMode==DM_INFO) {
      y += line_height;
      // Name or note string
      u8g2_DrawUTF8(u8g2, x, y, cat_mgr.objectNameStr());

      // line 3
      x = 0;
      y += line_height;
      if (cat_mgr.isDblStarCatalog()) {
        // |Sep 2.5" PA 225       |
        char seps[16];
        float f=cat_mgr.separation();
        if (f>=0) dtostrf(f, 5, 1, seps); else strcpy(seps,"?.?");
        x=u8g2_DrawUTF8(u8g2, 0, y, "Sep");
        sprintf(line,"%s\"",seps);
        ext_DrawFwNumeric(u8g2, x, y, line);

        int p=cat_mgr.positionAngle();
        if (p>=0) sprintf(line,"%3d\xb0",p); else sprintf(line,"  ?\xb0");
        x=ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y, line);
        u8g2_DrawUTF8(u8g2, dx-(u8g2_GetUTF8Width(u8g2, "PA ")+x), y, "PA ");
      } else
      if (cat_mgr.isVarStarCatalog()) {
        // |Per 2.5d              |
        char pers[16];
        float p=cat_mgr.period();
        // Period 0.00 to 9.99 days, period 10.0 to 3186.6 days, -1 = Unknown, -2 = Irregular
        if (abs(p+1.0)<0.1)           { sprintf(line,L_CAT_PER_UNK); } else
        if (abs(p+2.0)<0.1)           { sprintf(line,L_CAT_PER_IRR); } else
        if ((p>=0.0)  && (p<   10.0)) { dtostrf(p, 7, 2, pers); sprintf(line,L_CAT_PER "%sd",pers); } else
        if ((p>=10.0) && (p<=3186.6)) { dtostrf(p, 7, 1, pers); sprintf(line,L_CAT_PER "%sd",pers); } else sprintf(line,L_CAT_PER " ?");
        ext_DrawFwNumeric(u8g2, x, y, line);
      }
    }
  }
  else {
    // DSO Catalogs (current)
    // |M44 N2632        2.2|
    // |Open Cluster     CnC|
    // |Beehive Cluster     |
    
    // Prefix and catalog number
    x = 0;
    sprintf(line, "%s", cat_mgr.catalogPrefix());
    x += u8g2_DrawUTF8(u8g2, x, y, line);
    if (!cat_mgr.hasPrimaryIdInPrefix()) sprintf(line, "%ld", cat_mgr.primaryId());
    x += u8g2_DrawUTF8(u8g2, x, y, line);
     
    // Object SubId
    u8g2_SetFont(u8g2, LF_CATALOGS);
    sprintf(line, "%s", cat_mgr.subIdStr());
    x += u8g2_DrawUTF8(u8g2, x, y, line);
    u8g2_SetFont(u8g2, myfont);

    // Magnitude
    float mf=cat_mgr.magnitude();
    if (mf>99) sprintf(line, "?.?"); else dtostrf(mf, 3, 1, line);
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
    char epoch[6];
    uint8_t vr1, vr2, vr3;
    short vd1; uint8_t vd2, vd3;

    // RA
    sprintf(epoch,"%04d",cat_mgr.epoch());
    cat_mgr.raHMS(vr1,vr2,vr3);
    sprintf(line," %02d:%02d:%02d",vr1,vr2,vr3);
    y += line_height;
    x = u8g2_DrawUTF8(u8g2, 0, y, L_RA); u8g2_SetFont(u8g2, LF_CATALOGS); u8g2_DrawUTF8(u8g2, x, y, epoch); u8g2_SetFont(u8g2, myfont);
    ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y, line);

    // Declination
    cat_mgr.decDMS(vd1,vd2, vd3);
    sprintf(line,"%+02d\xb0%02d'%02d",vd1,vd2,vd3);
    y += line_height;
    x = u8g2_DrawUTF8(u8g2, 0, y, L_DE); u8g2_SetFont(u8g2, LF_CATALOGS); u8g2_DrawUTF8(u8g2, x, y, epoch); u8g2_SetFont(u8g2, myfont);
    ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y, line);
  }

  if (displayMode==DM_HOR_COORDS) {
    static short va1, vz1; 
    static uint8_t va2, va3, vz2, vz3;

    u8g2_SetFont(u8g2, myfont);

    // Horizon Coords
    if (firstPass) cat_mgr.azmDMS(vz1, vz2, vz3);
    if (firstPass) cat_mgr.altDMS(va1, va2, va3);
    vz2=vz2/6;
    va2=va2/6;
    sprintf(line,"%3d.%d\xb0",vz1,vz2);
    x=0; y += line_height;
    x += u8g2_DrawUTF8(u8g2, x, y, "Az")+2;
    x += (ext_DrawFwNumeric(u8g2, x, y, line));
    sprintf(line,"%3d.%d\xb0",va1,va2);
    x=dx-ext_DrawFwNumeric(u8g2, dx-ext_GetFwNumericWidth(u8g2, line), y, line);
    u8g2_DrawUTF8(u8g2, x-(u8g2_GetUTF8Width(u8g2, "Alt")+4), y, "Alt");
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
  u8g2_SetFont(u8g2, LF_STANDARD);
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
  if (strstr(line2,"UNK")) strcpy(line2,L_CAT_UNK "Unknown"); else
  if (strstr(line2,"OC"))  strcpy(line2,L_CAT_OC "Open Cluster"); else
  if (strstr(line2,"GC"))  strcpy(line2,L_CAT_GC "Globular Cluster"); else
  if (strstr(line2,"PN"))  strcpy(line2,L_CAT_PN "Planetary Nebula"); else
  if (strstr(line2,"SG"))  strcpy(line2,L_CAT_SG "Spirial Galaxy"); else
  if (strstr(line2,"EG"))  strcpy(line2,L_CAT_EG "Eliptical Galaxy"); else
  if (strstr(line2,"IG"))  strcpy(line2,L_CAT_IG "Irregular Galaxy"); else
  if (strstr(line2,"KNT")) strcpy(line2,L_CAT_KNT "Knot"); else
  if (strstr(line2,"SNR")) strcpy(line2,L_CAT_SNR "SuperNova Rmnnt"); else
  if (strstr(line2,"GAL")) strcpy(line2,L_CAT_GAL "Galaxy"); else
  if (strstr(line2,"CN"))  strcpy(line2,L_CAT_CN "Cluster + Nebula"); else
  if (strstr(line2,"STR")) strcpy(line2,L_CAT_STR "Star"); else
  if (strstr(line2,"PLA")) strcpy(line2,L_CAT_PLA "Planet"); else
  if (strstr(line2,"CMT")) strcpy(line2,L_CAT_CMT "Comet"); else
  if (strstr(line2,"AST")) strcpy(line2,L_CAT_AST "Asteroid");
    
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
  u8g2_SetFont(u8g2, LF_STANDARD);
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
