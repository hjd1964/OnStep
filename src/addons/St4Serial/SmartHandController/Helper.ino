// -------------------------------------------------------------------------------
// Helper

void DisplayMessage(const char* txt1, const char* txt2 = NULL, int duration = 0)
{
  uint8_t x;
  uint8_t y = 40;
  display.firstPage();
  do {
    if (txt2 != NULL)
    {
      y = 50;
      x = (display.getDisplayWidth() - display.getStrWidth(txt2)) / 2;
      display.drawStr(x, y, txt2);
      y = 25;
    }
    x= (display.getDisplayWidth() - display.getStrWidth(txt1)) / 2;
    display.drawStr(x, y, txt1);
  } while (display.nextPage());
  if (duration >= 0)
    delay(duration);
  else
  {
    for (;;)
    {
      LoopMain();
      tickButtons();
      delay(50);
      if (buttonPressed)
        break;
    }
  }
}

void DisplayMessage2(const char* txt1, const char* txt2 = NULL, int duration = 0)
{
  uint8_t x;
  uint8_t y = 40;
  display.firstPage();
  do {
    if (txt2 != NULL)
    {
      y = 50;
      x = (display.getDisplayWidth() - display.getStrWidth(txt2)) / 2;
      display.drawStr(x, y, txt2);
      y = 25;
    }
    x = (display.getDisplayWidth() - display.getStrWidth(txt1)) / 2;
    display.drawStr(x, y, txt1);
  } while (display.nextPage());
  delay(duration);
}

void DisplayLongMessage(const char* txt1, const char* txt2 = NULL, const char* txt3 = NULL, const char* txt4 = NULL, int duration = 0)
{
  display.setFont(u8g2_font_helvR10_tr);
  uint8_t h = 15;
  uint8_t x = 0;
  uint8_t y = h;
  display.firstPage();
  do {

    y = h;
    x = (display.getDisplayWidth() - display.getStrWidth(txt1)) / 2;
    display.drawStr(x, y, txt1);
    y += h;
    if (txt2 != NULL)
    {
      x = 0;
      display.drawStr(x, y, txt2);
    }
    else
    {
      y -= 7;
    }
    y += 15;
    if (txt3 != NULL)
    {
      x = 0;
      display.drawStr(x, y, txt3);
    }

    y += 15;
    if (txt4 != NULL)
    {
      x = 0;
      display.drawStr(x, y, txt4);
    }
  } while (display.nextPage());
  if (duration >= 0)
    delay(duration);
  else
  {
    for (;;)
    {
      LoopMain();
      tickButtons();
      delay(50);
      if (buttonPressed)
        break;
    }
  }

  display.setFont(u8g2_font_helvR12_te);
}


bool GetLX200(char* command, char* output, bool silent)
{
  memset(output, 0, sizeof(output));
  bool ok = readLX200Bytes(command, output, 100);
  if (!silent && !ok)
  {
     DisplayMessage("Read Value!", "Failed", 500);
  }
  return ok;
}

bool SetLX200(char* command, bool silent = false)
{
  char out[20];
  memset(out, 0, sizeof(out));
  bool ok =readLX200Bytes(command, out, 100);
  if (!silent)
  {
    if (ok)
      DisplayMessage("Value Set!", NULL, 500);
    else
      DisplayMessage("Value Set!", "Failed", -1);
  }
  return ok;
}


void Move2TargetLX200(bool silent = false)
{
  char out[20];
  memset(out, 0, sizeof(out));
 int val = readLX200Bytes(":MS#", out, 100);
 switch (val)
 {
   //         1=Object below horizon    Outside limits, below the Horizon limit
   //         2=No object selected      Failure to resolve coordinates
   //         4=Position unreachable    Not unparked
   //         5=Busy                    Goto already active
   //         6=Outside limits          Outside limits, above the Zenith limit
 case 0:
   DisplayMessage("Slew to", "Target", 500);
   break;
 case 1:
   DisplayMessage("Object below", "Horizon", -1);
   break;
 case 2:
   DisplayMessage("No Object", "Selected", -1);
   break;
 case 4:
   DisplayMessage("Telescope", "is Parked", -1);
   break;
 case 5:
   DisplayMessage("Telescope", "is busy", -1);
   break;
 case 6:
   DisplayMessage("Outside", "Limits", -1);
   break;
 default:
   char text[15];
   sprintf(text, "Error %d", val);
   DisplayMessage("Unknown", text, -1);
   break;
 }
}

bool SetTargetRaLX200(uint8_t& vr1, uint8_t& vr2, uint8_t& vr3)
{
  char cmd[20],out[20];
  int iter = 0;
  sprintf(cmd, ":Sr%02u:%02u:%02u#", vr1, vr2, vr3);
  while (iter < 3)
  { 
    if (SetLX200(cmd, true))
    {
      if (GetLX200(":Gr#", out, true))
      {
        int hour, minute, second;
        char2RA(out, hour, minute, second);
        if (hour == vr1 && minute == vr2 && second == vr3)
        {
          return true;
        }
      }
    }
    iter++;
  }
  return false;
}

bool SetTargetDecLX200(short& vd1, uint8_t& vd2, uint8_t& vd3)
{
  char  cmd[20],out[20];
  int iter = 0;
  sprintf(cmd, ":Sd%+03d:%02u:%02u#", vd1, vd2, vd3);
  while (iter < 3)
  {
    if (SetLX200(cmd, true))
    {
      if (GetLX200(":Gd#", out, true))
      {
        int deg, min, sec;
        char2DEC(out, deg, min, sec);
        if (deg == vd1 && min == vd2 && sec== vd3)
        {
          return true;
        }
      }
    }
    iter++;
  }
  return false;
}

bool SyncGotoLX200(bool sync, uint8_t& vr1, uint8_t& vr2, uint8_t& vr3, short& vd1, uint8_t& vd2, uint8_t& vd3)
{
 
  bool ok = SetTargetRaLX200(vr1, vr2, vr3);
  ok = ok && SetTargetDecLX200(vd1, vd2, vd3);
  if (ok)
  {
    if (sync)
    {
      Serial.print(":CS#");
      Serial.flush();
      DisplayMessage("Synced with", "Target", 500);
    }
    else
    {
      Move2TargetLX200();
    }
    return true;
  }
  else
  {
    DisplayMessage("Set Target", "Failed!", 500);
  }
  return false;
}

bool SyncGotoCatLX200(bool sync, Catalog cat, int idx)
{
  uint8_t vr1, vr2, vr3, vd2,vd3;
  vd3 = 0;
  short vd1;
  switch (cat)
  {
  case STAR:
    getcathms(Star_ra[idx], vr1, vr2, vr3);
    getcatdms(Star_dec[idx], vd1, vd2);
    break;
  case MESSIER:
    return false;
    break;
  case HERSCHEL:
    getcathms(Herschel_ra[idx], vr1, vr2, vr3);
    getcatdms(Herschel_dec[idx], vd1, vd2);
    break;
  default:
    return false;
    break;
  }

  return SyncGotoLX200(sync, vr1, vr2, vr3, vd1, vd2, vd3);
}

