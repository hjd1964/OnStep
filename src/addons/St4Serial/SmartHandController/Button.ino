// -------------------------------------------------------------------------------
// Button call-backs

void click_s() {
  eventbuttons[B_SHIFT] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button s click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_s() {
  eventbuttons[B_SHIFT] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button s doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button s longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button s longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_s() {
  eventbuttons[B_SHIFT] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button s longPress stop");
#endif // DEBUGBUTTON
} // longPressStop2

//------------------------------------------------------------------------------
void click_N() {
  eventbuttons[B_NORTH] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button N click.");
#endif // DEBUGBUTTON
} // click2

//void doubleclick_N() {
//  eventbuttons[B_NORTH] = E_DOUBLECLICK;
//#ifdef DEBUGBUTTON
//  DEBUG.println("Button N doubleclick.");
//#endif // DEBUGBUTTON
//} // doubleclick2

void longPressStart_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button N longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_N() {
  eventbuttons[B_NORTH] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button N longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_N() {
  eventbuttons[B_NORTH] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button N longPress stop");
#endif // DEBUGBUTTON
} // long

//------------------------------------------------------------------------------
void click_S() {
  eventbuttons[B_SOUTH] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button S click.");
#endif // DEBUGBUTTON
} // click2

//void doubleclick_S() {
//  eventbuttons[B_SOUTH] = E_DOUBLECLICK;
//#ifdef DEBUGBUTTON
//  DEBUG.println("Button S doubleclick.");
//#endif // DEBUGBUTTON
//} // doubleclick2

void longPressStart_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button S longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button S longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_S() {
  eventbuttons[B_SOUTH] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button S longPress stop");
#endif // DEBUGBUTTON
} // long

//------------------------------------------------------------------------------
void click_E() {
  eventbuttons[B_EAST] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button E click.");
#endif // DEBUGBUTTON
} // click2

//void doubleclick_E() {
//  eventbuttons[B_EAST] = E_DOUBLECLICK;
//#ifdef DEBUGBUTTON
//  DEBUG.println("Button E doubleclick.");
//#endif // DEBUGBUTTON
//} // doubleclick2

void longPressStart_E() {
  eventbuttons[B_EAST] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button E longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_E() {
  eventbuttons[B_EAST] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button E longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_E() {
  eventbuttons[B_EAST] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button E longPress stop");
#endif // DEBUGBUTTON
} // long

//------------------------------------------------------------------------------
void click_W() {
  eventbuttons[B_WEST] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button W click.");
#endif // DEBUGBUTTON
} // click2

//void doubleclick_W() {
//  eventbuttons[B_WEST] = E_DOUBLECLICK;
//#ifdef DEBUGBUTTON
//  Serial.println("Button W doubleclick.");
//#endif // DEBUGBUTTON
//} // doubleclick2

void longPressStart_W() {
  eventbuttons[B_WEST] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button W longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_W() {
  eventbuttons[B_WEST] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button W longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_W() {
  eventbuttons[B_WEST] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button W longPress stop");
#endif // DEBUGBUTTON
} // long

//------------------------------------------------------------------------------
void click_F(){
  eventbuttons[B_F] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button F click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_F() {
  eventbuttons[B_F] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button F doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_F() {
  eventbuttons[B_F] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button F longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_F() {
  eventbuttons[B_F] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button F longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_F() {
  eventbuttons[B_F] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button F longPress stop");
#endif // DEBUGBUTTON
} // long

//------------------------------------------------------------------------------
void click_f() {
  eventbuttons[B_f] = E_CLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button f click.");
#endif // DEBUGBUTTON
} // click2

void doubleclick_f() {
  eventbuttons[B_f] = E_DOUBLECLICK;
#ifdef DEBUGBUTTON
  DEBUG.println("Button f doubleclick.");
#endif // DEBUGBUTTON
} // doubleclick2

void longPressStart_f() {
  eventbuttons[B_f] = E_LONGPRESSTART;
#ifdef DEBUGBUTTON
  DEBUG.println("Button f longPress start");
#endif // DEBUGBUTTON
} // longPressStart2

void longPress_f() {
  eventbuttons[B_f] = E_LONGPRESS;
#ifdef DEBUGBUTTON
  DEBUG.println("Button f longPress...");
#endif // DEBUGBUTTON
} // longPress2

void longPressStop_f() {
  eventbuttons[B_f] = E_LONGPRESSSTOP;
#ifdef DEBUGBUTTON
  DEBUG.println("Button f longPress stop");
#endif // DEBUGBUTTON
} // long

void setticks(OneButton &inputButton)
{
  inputButton.setClickTicks(300);
  inputButton.setDebounceTicks(30);
  inputButton.setPressTicks(300);
}

