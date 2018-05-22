#include "Pad.h"

byte eventbuttons[7] = { E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE ,E_NONE };

void click_s() {
  eventbuttons[B_SHIFT] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button s click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_s() {
  eventbuttons[B_SHIFT] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button s doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button s longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button s longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button s longPress stop");
#endif // DEBUGBUTTON
} // longPressStop2


  //------------------------------------------------------------------------------

void click_N() {
  eventbuttons[B_NORTH] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button N click.");
#endif // DEBUGBUTTON
} // click2

  //void doubleclick_N() {
  //  eventbuttons[B_NORTH] = E_DOUBLECLICK;
  //#ifdef DEBUGBUTTON
  //  Ser.println("Button N doubleclick.");
  //#endif // DEBUGBUTTON
  //} // doubleclick2

void longPressStart_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button N longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_N() {
  eventbuttons[B_NORTH] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button N longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button N longPress stop");
#endif // DEBUGBUTTON
} // long

  //------------------------------------------------------------------------------
void click_S() {
  eventbuttons[B_SOUTH] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button S click.");
#endif // DEBUGBUTTON
} // click2

  //void doubleclick_S() {
  //  eventbuttons[B_SOUTH] = E_DOUBLECLICK;
  //#ifdef DEBUGBUTTON
  //  Ser.println("Button S doubleclick.");
  //#endif // DEBUGBUTTON
  //} // doubleclick2

void longPressStart_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button S longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button S longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button S longPress stop");
#endif // DEBUGBUTTON
} // long


  //------------------------------------------------------------------------------
void click_E() {
  eventbuttons[B_EAST] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button E click.");
#endif // DEBUGBUTTON
} // click2

  //void doubleclick_E() {
  //  eventbuttons[B_EAST] = E_DOUBLECLICK;
  //#ifdef DEBUGBUTTON
  //  Ser.println("Button E doubleclick.");
  //#endif // DEBUGBUTTON
  //} // doubleclick2

void longPressStart_E() {
  eventbuttons[B_EAST] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button E longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_E() {
  eventbuttons[B_EAST] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button E longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_E() {
  eventbuttons[B_EAST] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button E longPress stop");
#endif // DEBUGBUTTON
} // long

  //------------------------------------------------------------------------------
void click_W() {
  eventbuttons[B_WEST] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button W click.");
#endif // DEBUGBUTTON
} // click2

  //void doubleclick_W() {
  //  eventbuttons[B_WEST] = E_DOUBLECLICK;
  //#ifdef DEBUGBUTTON
  //  Ser.println("Button W doubleclick.");
  //#endif // DEBUGBUTTON
  //} // doubleclick2

void longPressStart_W() {
  eventbuttons[B_WEST] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button W longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_W() {
  eventbuttons[B_WEST] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button W longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_W() {
  eventbuttons[B_WEST] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button W longPress stop");
#endif // DEBUGBUTTON
} // long

  //------------------------------------------------------------------------------
void click_F() {
  eventbuttons[B_F] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button F click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_F() {
  eventbuttons[B_F] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button F doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_F() {
  eventbuttons[B_F] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button F longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_F() {
  eventbuttons[B_F] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button F longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_F() {
  eventbuttons[B_F] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button F longPress stop");
#endif // DEBUGBUTTON
} // long

  //------------------------------------------------------------------------------
void click_f() {
  eventbuttons[B_f] = E_CLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button f click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_f() {
  eventbuttons[B_f] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  Ser.println("Button f doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_f() {
  eventbuttons[B_f] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  Ser.println("Button f longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_f() {
  eventbuttons[B_f] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  Ser.println("Button f longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_f() {
  eventbuttons[B_f] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  Ser.println("Button f longPress stop");
#endif // DEBUGBUTTON
} // long

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
