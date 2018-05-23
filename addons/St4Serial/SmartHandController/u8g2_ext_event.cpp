#include <U8g2lib.h>
#include "u8g2_ext_event.h"

uint8_t ext_GetMenuEvent(Pad* extPad)
{
  extPad->tickButtons();
  int k = 0;
  for (k = 0; k < 7; k++)
  {
    if (eventbuttons[k] == E_CLICK || eventbuttons[k] == E_LONGPRESSTART || eventbuttons[k] == E_LONGPRESS)
    {
      break;
    }
  }
  switch (k)
  {
  case 0:
    return  U8X8_MSG_GPIO_MENU_HOME;
    break;
  case 1:
    return  U8X8_MSG_GPIO_MENU_UP;
    break;
  case 2:
    return  U8X8_MSG_GPIO_MENU_DOWN;
    break;
  case 3:
    return  U8X8_MSG_GPIO_MENU_PREV;
    break;
  case 4:
    return  U8X8_MSG_GPIO_MENU_NEXT;
    break;
  case 5:
    return  U8X8_MSG_GPIO_MENU_SELECT;
    break;
  case 6:
    return  U8X8_MSG_GPIO_MENU_HOME;
    break;
  default:
    break;
  }
  return 0;
}