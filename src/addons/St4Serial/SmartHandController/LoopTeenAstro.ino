// -------------------------------------------------------------------------------
// TeenAstro main loop

void LoopTeenAstro() {
  buttonPressed = false;
  tickButtons();
  if (!buttonPressed)
  {
    LoopMain();
  }

  unsigned long top = millis();
  if (sleepDisplay && !buttonPressed)
  {
    return;
  }
  else if (sleepDisplay && buttonPressed)
  {
    display.setContrast(maxContrast);
    display.sleepOff();
    sleepDisplay = false;
    lowContrast = false;
    time_last_action = millis();
  }
  else if (top - time_last_action > 120000)
  {
    display.sleepOn();
    sleepDisplay = true;
    return;
  }
  if (lowContrast && buttonPressed)
  {
    display.setContrast(maxContrast);
    lowContrast = false;
    time_last_action = top;
  }
  else if (top - time_last_action > 30000 && !lowContrast)
  {
    display.setContrast(0);
    lowContrast = true;
  }
  if (powerCylceRequired)
  {
    display.setFont(u8g2_font_helvR12_tr);
    DisplayMessage2("REBOOT", "DEVICE", 1000);
    return;
  }
  if (top - lastpageupdate > 100)
  {
    update_main(display.getU8g2(), page);
  }
  if (telInfo.connected == false)
  {
    DisplayMessage2("Hand controler", "not connected");
  }
  if (telInfo.connected && (telInfo.getTrackingState() == TRK_SLEWING || telInfo.getParkState() == PRK_PARKING))
  {
    bool stop = (eventbuttons[0] == E_LONGPRESS || eventbuttons[0] == E_LONGPRESSTART || eventbuttons[0] == E_DOUBLECLICK) ? true : false;
    byte it = 1;
    while (!stop && it<5)
    {
      stop =  (eventbuttons[it] == E_LONGPRESS || eventbuttons[it] == E_CLICK || eventbuttons[it] == E_LONGPRESSTART);
      it++;
    }
    if (stop)
    {
      Ser.print(":Q#");
      Ser.flush();
      time_last_action = millis();
      display.sleepOff();
      return;
    }
  }
  else
  {
    buttonCommand = false;
    for (int k = 1; k < 5; k++)
    {
      if (Move[k - 1] && (eventbuttons[k] == E_LONGPRESSSTOP || eventbuttons[k] == E_NONE))
      {
        buttonCommand = true;
        Move[k - 1] = false;
        Ser.print(BreakRC[k - 1]);
        Ser.flush();
        continue;
      }
      else if (!Move[k - 1] && (eventbuttons[k] == E_LONGPRESS || eventbuttons[k] == E_CLICK || eventbuttons[k] == E_LONGPRESSTART))
      {
        buttonCommand = true;
        Move[k - 1] = true;
        Ser.print(RC[k - 1]);
        Ser.flush();
        continue;
      }
    }
    if (buttonCommand)
    {
      time_last_action = millis();
      return;
    }
  }

  if (eventbuttons[0] == E_DOUBLECLICK /*|| eventbuttons[0] == E_CLICK)  && eventbuttons[1] != E_NONE*/)
  {
    menuSpeedRate();
    time_last_action = millis();
    
  }
  else if (eventbuttons[0] == E_CLICK)
  {
    page++;
    if (page > 2) page = 0;
    time_last_action = millis(); 
  }
  else if (eventbuttons[0] == E_LONGPRESS)
  {
    menuMain();
    time_last_action = millis();
  }
}

