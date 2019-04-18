#include "u8g2_ext_value.h"

uint8_t ext_DrawFwNumeric(u8g2_t *u8g2, uint8_t x, uint8_t y, const char* text)
{
  int w=0;
  int width=0;
  char ws[2];
  int l=strlen(text);
  for (int i=0; i<l; i++) {
    ws[0]=text[i]; ws[1]=0;
    if ( ((text[i]>='0') && (text[i]<='9')) || (text[i]==':') || (text[i]=='?') || (text[i]==' ') || (text[i]=='\xb0') || (text[i]=='\'') || (text[i]=='+') || (text[i]=='-')) w=u8g2_GetUTF8Width(u8g2,"0"); else w=u8g2_GetUTF8Width(u8g2,ws);
    if (text[i]==':') {
      x+=2; u8g2_DrawUTF8(u8g2, x, y, ws); x-=2;
    } else {
      u8g2_DrawUTF8(u8g2, x, y, ws); 
    }
    x+=w+1;
    width+=w+1;
  }
  return width;
}

uint8_t ext_GetFwNumericWidth(u8g2_t *u8g2, const char* text)
{
  int w=0;
  int width=0;
  char ws[2];
  int l=strlen(text);
  for (int i=0; i<l; i++) {
    ws[0]=text[i]; ws[1]=0;
    if ( ((text[i]>='0') && (text[i]<='9')) || (text[i]==':') || (text[i]=='?') || (text[i]==' ') || (text[i]=='\xb0') || (text[i]=='\'') || (text[i]=='+') || (text[i]=='-')) w=u8g2_GetUTF8Width(u8g2,"0"); else w=u8g2_GetUTF8Width(u8g2,ws);
    width+=w+1;
  }
  return width;
}
