// -------------------------------------------------------------------------------
// Initialize

void setupButton()
{
  setticks(buttons[B_SHIFT]);
  buttons[B_SHIFT].attachClick(click_s);
  buttons[B_SHIFT].attachDoubleClick(doubleclick_s);
  buttons[B_SHIFT].attachLongPressStart(longPressStart_s);
  buttons[B_SHIFT].attachLongPressStop(longPressStop_s);
  buttons[B_SHIFT].attachDuringLongPress(longPress_s);

  setticks(buttons[B_NORTH]);
  buttons[B_NORTH].attachClick(click_N);
  //buttons[B_NORTH].attachDoubleClick(doubleclick_N);
  buttons[B_NORTH].attachLongPressStart(longPressStart_N);
  buttons[B_NORTH].attachLongPressStop(longPressStop_N);
  buttons[B_NORTH].attachDuringLongPress(longPress_N);

  setticks(buttons[B_SOUTH]);
  buttons[B_SOUTH].attachClick(click_S);
  //buttons[B_SOUTH].attachDoubleClick(doubleclick_S);
  buttons[B_SOUTH].attachLongPressStart(longPressStart_S);
  buttons[B_SOUTH].attachLongPressStop(longPressStop_S);
  buttons[B_SOUTH].attachDuringLongPress(longPress_S);

  setticks(buttons[B_EAST]);
  buttons[B_EAST].attachClick(click_E);
  //buttons[B_EAST].attachDoubleClick(doubleclick_E);
  buttons[B_EAST].attachLongPressStart(longPressStart_E);
  buttons[B_EAST].attachLongPressStop(longPressStop_E);
  buttons[B_EAST].attachDuringLongPress(longPress_E);

  setticks(buttons[B_WEST]);
  buttons[B_WEST].attachClick(click_W);
  //buttons[B_WEST].attachDoubleClick(doubleclick_W);
  buttons[B_WEST].attachLongPressStart(longPressStart_W);
  buttons[B_WEST].attachLongPressStop(longPressStop_W);
  buttons[B_WEST].attachDuringLongPress(longPress_W);
  buttons[B_WEST].attachClick(click_W);

  setticks(buttons[B_F]);
  buttons[B_F].attachClick(click_F);
  buttons[B_F].attachDoubleClick(doubleclick_F);
  buttons[B_F].attachLongPressStart(longPressStart_F);
  buttons[B_F].attachLongPressStop(longPressStop_F);
  buttons[B_F].attachDuringLongPress(longPress_F);

  setticks(buttons[B_f]);
  buttons[B_f].attachClick(click_f);
  buttons[B_f].attachDoubleClick(doubleclick_f);
  buttons[B_f].attachLongPressStart(longPressStart_f);
  buttons[B_f].attachLongPressStop(longPressStop_f);
  buttons[B_f].attachDuringLongPress(longPress_f);
}

void tickButtons()
{
  delay(10);
  buttonPressed = false;
  for (int k = 0; k < 7; k++)
  {
    delay(1);
    eventbuttons[k] = E_NONE;
    buttons[k].tick();
  }
  for (int k = 0; k < 7; k++)
  {
    if (eventbuttons[k] != 0)
    {
      buttonPressed = true;
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

void setupTeenAstro() {
  Ser.begin(SERIAL_BAUD_DEFAULT);
  Ser.setTimeout(CmdTimeout);

  #ifdef DEBUG
  DEBUG.begin(9600);
  #endif

  initMotor();
  telInfo.lastState = millis();
  setupButton();
  display.begin();
  drawIntro();
  tickButtons();
  delay(2000);
  tickButtons();
  display.firstPage();
  uint8_t x = 0;
  do {
    display.setFont(u8g2_font_helvR14_tr);
    x = (display.getDisplayWidth() - display.getStrWidth("Loading")) / 2;
    display.drawStr(x, 25, "Loading");
    x = (display.getDisplayWidth() - display.getStrWidth("Version 0.0")) / 2;
    display.drawStr(x, 55, "Version 0.0");
  } while (display.nextPage());

  display.firstPage();
  do {
    x = (display.getDisplayWidth() - display.getStrWidth("Ready!")) / 2;
    display.drawStr(x, 40, "Ready!");
  } while (display.nextPage());
  delay(500);
}
