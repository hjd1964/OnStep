#include "Pad.h"
#include "Config.h"

byte eventbuttons[7] = { E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE };

void click_s() {
  eventbuttons[B_SHIFT] = E_CLICK;
  DL("Button s click.");
} // click2

void doubleclick_s() {
  eventbuttons[B_SHIFT] = E_DOUBLECLICK;
  DL("Button s doubleclick.");
} // doubleclick2

void longPressStart_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSTART;
  DL("Button s longPress start");
} // longPressStart2

void longPress_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESS;
  DL("Button s longPress...");
} // longPress2

void longPressStop_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSSTOP;
  DL("Button s longPress stop");
} // longPressStop2


//------------------------------------------------------------------------------
void click_N() {
  eventbuttons[B_NORTH] = E_CLICK;
  DL("Button N click.");
} // click2

  //void doubleclick_N() {
  //  eventbuttons[B_NORTH] = E_DOUBLECLICK;
  //#ifdef DEBUGBUTTON
  //  DL("Button N doubleclick.");
  //#endif // DEBUGBUTTON
  //} // doubleclick2

void longPressStart_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSTART;
  DL("Button N longPress start");
} // longPressStart2

void longPress_N() {
  eventbuttons[B_NORTH] = E_LONGPRESS;
  DL("Button N longPress...");
} // longPress2

void longPressStop_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSSTOP;
  DL("Button N longPress stop");
} // long

//------------------------------------------------------------------------------
void click_S() {
  eventbuttons[B_SOUTH] = E_CLICK;
  DL("Button S click.");
} // click2

  //void doubleclick_S() {
  //  eventbuttons[B_SOUTH] = E_DOUBLECLICK;
  //  DL("Button S doubleclick.");
  //} // doubleclick2

void longPressStart_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSTART;
  DL("Button S longPress start");
} // longPressStart2

void longPress_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESS;
  DL("Button S longPress...");
} // longPress2

void longPressStop_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSSTOP;
  DL("Button S longPress stop");
} // long


//------------------------------------------------------------------------------
void click_E() {
  eventbuttons[B_EAST] = E_CLICK;
  DL("Button E click.");
} // click2

  //void doubleclick_E() {
  //  eventbuttons[B_EAST] = E_DOUBLECLICK;
  //  DL("Button E doubleclick.");
  //} // doubleclick2

void longPressStart_E() {
  eventbuttons[B_EAST] = E_LONGPRESSTART;
  DL("Button E longPress start");
} // longPressStart2

void longPress_E() {
  eventbuttons[B_EAST] = E_LONGPRESS;
  DL("Button E longPress...");
} // longPress2

void longPressStop_E() {
  eventbuttons[B_EAST] = E_LONGPRESSSTOP;
  DL("Button E longPress stop");
} // long

//------------------------------------------------------------------------------
void click_W() {
  eventbuttons[B_WEST] = E_CLICK;
  DL("Button W click.");
} // click2

  //void doubleclick_W() {
  //  eventbuttons[B_WEST] = E_DOUBLECLICK;
  //  DL("Button W doubleclick.");
  //} // doubleclick2

void longPressStart_W() {
  eventbuttons[B_WEST] = E_LONGPRESSTART;
  DL("Button W longPress start");
} // longPressStart2

void longPress_W() {
  eventbuttons[B_WEST] = E_LONGPRESS;
  DL("Button W longPress...");
} // longPress2

void longPressStop_W() {
  eventbuttons[B_WEST] = E_LONGPRESSSTOP;
  DL("Button W longPress stop");
} // long

//------------------------------------------------------------------------------
void click_F() {
  eventbuttons[B_F] = E_CLICK;
  DL("Button F click.");
} // click2

void doubleclick_F() {
  eventbuttons[B_F] = E_DOUBLECLICK;
  DL("Button F doubleclick.");
} // doubleclick2

void longPressStart_F() {
  eventbuttons[B_F] = E_LONGPRESSTART;
  DL("Button F longPress start");
} // longPressStart2

void longPress_F() {
  eventbuttons[B_F] = E_LONGPRESS;
  DL("Button F longPress...");
} // longPress2

void longPressStop_F() {
  eventbuttons[B_F] = E_LONGPRESSSTOP;
  DL("Button F longPress stop");
} // long

//------------------------------------------------------------------------------
void click_f() {
  eventbuttons[B_f] = E_CLICK;
  DL("Button f click.");
} // click2

void doubleclick_f() {
  eventbuttons[B_f] = E_DOUBLECLICK;
  DL("Button f doubleclick.");
} // doubleclick2

void longPressStart_f() {
  eventbuttons[B_f] = E_LONGPRESSTART;
  DL("Button f longPress start");
} // longPressStart2

void longPress_f() {
  eventbuttons[B_f] = E_LONGPRESS;
  DL("Button f longPress...");
} // longPress2

void longPressStop_f() {
  eventbuttons[B_f] = E_LONGPRESSSTOP;
  DL("Button f longPress stop");
} // long

//------------------------------------------------------------------------------
void Pad::attachEvent()
{
  m_buttons[B_SHIFT]->attachClick(click_s);
  m_buttons[B_SHIFT]->attachDoubleClick(doubleclick_s);
  m_buttons[B_SHIFT]->attachLongPressStart(longPressStart_s);
  m_buttons[B_SHIFT]->attachLongPressStop(longPressStop_s);
  m_buttons[B_SHIFT]->attachDuringLongPress(longPress_s);

  m_buttons[B_NORTH]->attachClick(click_N);
  //m_buttons[B_NORTH]->attachDoubleClick(doubleclick_N);
  m_buttons[B_NORTH]->attachLongPressStart(longPressStart_N);
  m_buttons[B_NORTH]->attachLongPressStop(longPressStop_N);
  m_buttons[B_NORTH]->attachDuringLongPress(longPress_N);

  m_buttons[B_SOUTH]->attachClick(click_S);
  //m_buttons[B_SOUTH]->attachDoubleClick(doubleclick_S);
  m_buttons[B_SOUTH]->attachLongPressStart(longPressStart_S);
  m_buttons[B_SOUTH]->attachLongPressStop(longPressStop_S);
  m_buttons[B_SOUTH]->attachDuringLongPress(longPress_S);

  m_buttons[B_EAST]->attachClick(click_E);
  //m_buttons[B_EAST]->attachDoubleClick(doubleclick_E);
  m_buttons[B_EAST]->attachLongPressStart(longPressStart_E);
  m_buttons[B_EAST]->attachLongPressStop(longPressStop_E);
  m_buttons[B_EAST]->attachDuringLongPress(longPress_E);

  m_buttons[B_WEST]->attachClick(click_W);
  //m_buttons[B_WEST]->attachDoubleClick(doubleclick_W);
  m_buttons[B_WEST]->attachLongPressStart(longPressStart_W);
  m_buttons[B_WEST]->attachLongPressStop(longPressStop_W);
  m_buttons[B_WEST]->attachDuringLongPress(longPress_W);
  m_buttons[B_WEST]->attachClick(click_W);

  m_buttons[B_F]->attachClick(click_F);
  m_buttons[B_F]->attachDoubleClick(doubleclick_F);
  m_buttons[B_F]->attachLongPressStart(longPressStart_F);
  m_buttons[B_F]->attachLongPressStop(longPressStop_F);
  m_buttons[B_F]->attachDuringLongPress(longPress_F);

  m_buttons[B_f]->attachClick(click_f);
  m_buttons[B_f]->attachDoubleClick(doubleclick_f);
  m_buttons[B_f]->attachLongPressStart(longPressStart_f);
  m_buttons[B_f]->attachLongPressStop(longPressStop_f);
  m_buttons[B_f]->attachDuringLongPress(longPress_f);

}
void Pad::setup(const int pin[7], const bool active[7])
{
  for (int k = 0; k < 7; k++)
  {
    m_buttons[k] = new OneButton(pin[k], active[k]);
    m_buttons[k]->setClickTicks(300);
    m_buttons[k]->setDebounceTicks(30);
    m_buttons[k]->setPressTicks(300);
  }
  attachEvent();
}
void Pad::tickButtons()
{
  delay(1);
  m_buttonPressed = false;
  for (int k = 0; k < 7; k++)
  {
    delay(1);
    eventbuttons[k] = E_NONE;
    m_buttons[k]->tick();
  }
  for (int k = 0; k < 7; k++)
  {
    if (eventbuttons[k] != 0)
    {
      m_buttonPressed = true;
      break;
    }
  }
  for (int k = 1; k < 6; k += 2)
  {
    if (eventbuttons[k] == eventbuttons[k + 1])
    {
      eventbuttons[k] = E_NONE;
      eventbuttons[k + 1] = E_NONE;
    }
  }
}

bool Pad::buttonPressed()
{
  return m_buttonPressed;
}

