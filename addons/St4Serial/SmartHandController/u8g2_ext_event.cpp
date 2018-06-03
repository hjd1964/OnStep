#include <U8g2lib.h>
#include "u8g2_ext_event.h"
#include "Pad.h"

uint8_t ext_GetMenuEvent(Pad* extPad)
{
  extPad->tickButtons();
  if (extPad->shift.wasClicked()) return  U8X8_MSG_GPIO_MENU_HOME;
  if ((extPad->n.isDown() && (extPad->n.timeDown()>1000)) || extPad->n.wasClicked()) { extPad->n.clearPress(); return U8X8_MSG_GPIO_MENU_UP; }
  if ((extPad->s.isDown() && (extPad->s.timeDown()>1000)) || extPad->s.wasClicked()) { extPad->s.clearPress(); return U8X8_MSG_GPIO_MENU_DOWN; }
  if (extPad->e.wasClicked()) return  U8X8_MSG_GPIO_MENU_PREV;
  if (extPad->w.wasClicked()) return  U8X8_MSG_GPIO_MENU_NEXT;
  if (extPad->F.wasClicked()) return  U8X8_MSG_GPIO_MENU_SELECT;
  if (extPad->f.wasClicked()) return  U8X8_MSG_GPIO_MENU_HOME;
  return 0;
}
