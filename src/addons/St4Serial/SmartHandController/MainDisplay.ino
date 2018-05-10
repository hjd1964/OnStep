// -------------------------------------------------------------------------------
// Main display

#define MY_BORDER_SIZE 1

#define wifi_width 16
#define wifi_height 16
static unsigned char wifi_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x20, 0x40, 0x4e, 0x00, 0x11,
  0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0xfe, 0x7f, 0x02, 0x40, 0xda, 0x5f,
  0xda, 0x5f, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00 };

#define wifi2_width 16
#define wifi2_height 16
static unsigned char wifi2_bits[] U8X8_PROGMEM = {
  0x80, 0x03, 0x60, 0x0c, 0x10, 0x10, 0xc8, 0x27, 0x24, 0x48, 0x94, 0x53,
  0x40, 0x04, 0x00, 0x01, 0x00, 0x01, 0xff, 0xff, 0x01, 0x80, 0x5d, 0x95,
  0x1d, 0x80, 0xdd, 0xbf, 0x01, 0x80, 0xff, 0xff };


#define home_width 16
#define home_height 16
static unsigned char home_bits[] U8X8_PROGMEM = {
  0x00, 0x02, 0x00, 0x07, 0x80, 0x0f, 0xc0, 0x1f, 0x80, 0x3f, 0x00, 0x7f,
  0x00, 0x7e, 0x00, 0x7f, 0x80, 0xfb, 0xc0, 0xc1, 0xe0, 0x01, 0xbc, 0x49,
  0x9e, 0x49, 0x9e, 0x79, 0x8c, 0x49, 0x80, 0x49 };


#define parked_width 16
#define parked_height 16
static unsigned char parked_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0xfe, 0x7f, 0x02, 0x40, 0x02, 0x40, 0xe2, 0x43, 0x62, 0x46,
  0x62, 0x46, 0x62, 0x46, 0xe2, 0x43, 0x62, 0x40, 0x62, 0x40, 0x62, 0x40,
  0x62, 0x40, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00 };

#define parking_width 16
#define parking_height 16
static unsigned char parking_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x80, 0x99, 0x81, 0x99, 0x81,
  0x99, 0x81, 0xf9, 0x80, 0x19, 0x80, 0x99, 0x84, 0x99, 0x8d, 0x99, 0x9f,
  0x81, 0x8d, 0x81, 0x84, 0x01, 0x80, 0xff, 0xff };

#define parkingFailed_width 16
#define parkingFailed_height 16
static unsigned char parkingFailed_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x01, 0x80, 0x01, 0x80, 0xf9, 0x90, 0x99, 0x91, 0x99, 0x91,
  0x99, 0x91, 0xf9, 0x90, 0x19, 0x90, 0xd9, 0x93, 0x59, 0x90, 0xd9, 0x91,
  0x41, 0x80, 0x41, 0x90, 0x01, 0x80, 0xff, 0xff };


#define guiding_width 16
#define guiding_height 16
static unsigned char guiding_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0x20, 0x04, 0x10, 0x08,
  0x08, 0x10, 0x8e, 0x71, 0x8e, 0x71, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04,
  0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00 };


#define no_tracking_width 16
#define no_tracking_height 16
static unsigned char no_tracking_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x38, 0x1c, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e,
  0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e, 0x7c, 0x3e,
  0x7c, 0x3e, 0x7c, 0x3e, 0x38, 0x1c, 0x00, 0x00 };

#define tracking_width 16
#define tracking_height 16
static unsigned char tracking_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00, 0x3e, 0x00,
  0x7e, 0x00, 0xfe, 0x38, 0xfe, 0x44, 0x7e, 0x44, 0x3e, 0x20, 0x1e, 0x10,
  0x0e, 0x10, 0x06, 0x00, 0x02, 0x10, 0x00, 0x00 };

#define tracking_S_width 16
#define tracking_S_height 16
static unsigned char tracking_S_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x1e, 0x00,
  0x3e, 0x00, 0x7e, 0x00, 0xfe, 0x38, 0x7e, 0x04, 0x3e, 0x04, 0x1e, 0x18,
  0x0e, 0x20, 0x06, 0x20, 0x02, 0x1c, 0x00, 0x00 };

#define sleewing_width 16
#define sleewing_height 16
static unsigned char sleewing_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x06, 0x03, 0x0e, 0x07, 0x1e, 0x0f,
  0x3e, 0x1f, 0x7e, 0x3f, 0xfe, 0x7f, 0x7e, 0x3f, 0x3e, 0x1f, 0x1e, 0x0f,
  0x0e, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00, 0x00 };


#define W_width 16
#define W_height 16
static unsigned char W_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0x24, 0x12,
  0x22, 0x22, 0x22, 0x22, 0xa2, 0x22, 0xa2, 0x22, 0x42, 0x21, 0x44, 0x11,
  0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };


#define E_width 16
#define E_height 16
static unsigned char E_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x10, 0x04, 0x08, 0x08, 0xe4, 0x13,
  0x22, 0x20, 0x22, 0x20, 0xe2, 0x21, 0x22, 0x20, 0x22, 0x20, 0xe4, 0x13,
  0x08, 0x08, 0x10, 0x04, 0xe0, 0x03, 0x00, 0x00 };

#define ErrDe_width 16
#define ErrDe_height 16
static unsigned char ErrDe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1,
  0x2e, 0xb1, 0x00, 0xb0, 0x1e, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2,
  0xa2, 0x83, 0xa2, 0xb0, 0x9e, 0xb3, 0x00, 0x80 };

#define ErrHo_width 16
#define ErrHo_height 16
static unsigned char ErrHo_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1,
  0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x22, 0xb0, 0x22, 0xb3, 0xbe, 0xb4,
  0xa2, 0x84, 0xa2, 0xb4, 0x22, 0xb3, 0x00, 0x80 };

#define ErrMe_width 16
#define ErrMe_height 16
static unsigned char ErrMe_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1,
  0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb2,
  0xa2, 0x83, 0xa2, 0xb0, 0xa2, 0xb3, 0x00, 0x80 };

#define ErrMf_width 16
#define ErrMf_height 16
static unsigned char ErrMf_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1,
  0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x36, 0xb0, 0xaa, 0xb3, 0xa2, 0xb0,
  0xa2, 0x81, 0xa2, 0xb0, 0xa2, 0xb0, 0x00, 0x80 };

#define ErrUp_width 16
#define ErrUp_height 16
static unsigned char ErrUp_bits[] U8X8_PROGMEM = {
  0xff, 0xff, 0x00, 0x80, 0x0e, 0xb0, 0x02, 0xb0, 0x66, 0xb3, 0x22, 0xb1,
  0x2e, 0xb1, 0x00, 0xb0, 0x22, 0xb0, 0x22, 0xb0, 0xa2, 0xb3, 0xa2, 0xb2,
  0xa2, 0x83, 0xa2, 0xb0, 0x9c, 0xb0, 0x00, 0x80 };


void update_main(u8g2_t *u8g2, u8g2_uint_t page)
{
  display.setFont(u8g2_font_helvR12_te);
  u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "44");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "4") + 1;
  telInfo.connected = true;
  telInfo.updateTel();
  if (telInfo.connected == false)
  {
    return;
  }

  if (page == 0)
  {
    telInfo.updateRaDec();
  }
  else if (page == 1)
  {
    telInfo.updateAzAlt();
  }
  else
  {
    telInfo.updateTime();
  }
  u8g2_FirstPage(u8g2);

  do
  {
    u8g2_uint_t x = u8g2_GetDisplayWidth(u8g2);
    int k = 0;
    if (wifiOn)
      display.drawXBMP(0, 0, wifi_width, wifi_height, wifi_bits);
    
    if (telInfo.hasTelStatus)
    {
      ParkState curP = telInfo.getParkState();
      TrackState curT = telInfo.getTrackingState();
      if (curP == PRK_PARKED)
      {
        display.drawXBMP(x - parked_width, 0, parked_width, parked_height, parked_bits);
        x -= parked_width + 1;
      }
      else if (curP == PRK_PARKING)
      {
        display.drawXBMP(x - parking_width, 0, parking_width, parking_height, parking_bits);
        x -= parking_width + 1;
      }
      else if (telInfo.atHome())
      {
        display.drawXBMP(x - home_width, 0, home_width, home_width, home_bits);
        x -= parked_width + 1;
      }
      else
      {
        if (curT == TRK_SLEWING)
        {
          display.drawXBMP(x - sleewing_width, 0, sleewing_width, sleewing_width, sleewing_bits);
          x -= sleewing_width + 1;
        }
        else if (curT == TRK_ON)
        {
          display.drawXBMP(x - tracking_S_width, 0, tracking_S_width, tracking_S_width, tracking_S_bits);
          x -= tracking_S_width + 1;
        }
        else if (curT == TRK_OFF)
        {
          display.drawXBMP(x - no_tracking_width, 0, no_tracking_width, no_tracking_width, no_tracking_bits);
          x -= no_tracking_width + 1;
        }

        if (curP == PRK_FAILED)
        {
          display.drawXBMP(x - parkingFailed_width, 0, parkingFailed_width, parkingFailed_height, parkingFailed_bits);
          x -= parkingFailed_width + 1;
        }
        if (telInfo.hasPierInfo)
        {
          PierState CurP = telInfo.getPierState();
          if (CurP == PIER_E)
          {
            display.drawXBMP(x - E_width, 0, E_width, E_height, E_bits);
            x -= E_width + 1;
          }
          else if (CurP == PIER_W)
          {
            display.drawXBMP(x - W_width, 0, W_width, W_height, W_bits);
            x -= W_width + 1;
          }
          if (telInfo.isGuiding())
          {
            display.drawXBMP(x - guiding_width, 0, guiding_width, guiding_height, guiding_bits);
            x -= guiding_width + 1;
          }
        }
      }

      lastError = telInfo.getError();
      if (lastError == ERR_MOTOR_FAULT)
      {
        display.drawXBMP(x - ErrMf_width, 0, ErrMf_width, ErrMf_height, ErrMf_bits);
        x -= ErrMf_width + 1;
      }
      else if (lastError == ERR_ALT)
      {
        display.drawXBMP(x - ErrHo_width, 0, ErrHo_width, ErrHo_height, ErrHo_bits);
        x -= ErrHo_width + 1;
      }
      else if (lastError == ERR_DEC)
      {
        display.drawXBMP(x - ErrDe_width, 0, ErrDe_width, ErrDe_height, ErrDe_bits);
        x -= ErrDe_width + 1;
      }
      else if (lastError == ERR_UNDER_POLE)
      {
        display.drawXBMP(x - ErrUp_width, 0, ErrUp_width, ErrUp_height, ErrUp_bits);
        x -= ErrUp_width + 1;
      }
      else if (lastError == ERR_MERIDIAN)
      {
        display.drawXBMP(x - ErrMe_width, 0, ErrMe_width, ErrMe_height, ErrMe_bits);
        x -= ErrMe_width + 1;
      }
    }

    if (page == 0)
    {
      if (telInfo.hasInfoRa && telInfo.hasInfoDec)
      {
        char Rah[3];
        char Ram[3];
        char Ras[3];
        char decsign[2];
        char decdeg[3];
        char decmin[3];
        char decsec[3];
        memcpy(Rah, telInfo.TempRa, 2);
        Rah[2] = '\0';
        memcpy(Ram, &telInfo.TempRa[3], 2);
        Ram[2] = '\0';
        memcpy(Ras, &telInfo.TempRa[6], 2);
        Ras[2] = '\0';
        memcpy(decsign, telInfo.TempDec, 1);
        decsign[1] = '\0';
        memcpy(decdeg, &telInfo.TempDec[1], 2);
        decdeg[2] = '\0';
        memcpy(decmin, &telInfo.TempDec[4], 2);
        decmin[2] = '\0';
        memcpy(decsec, &telInfo.TempDec[7], 2);
        decsec[2] = '\0';

        u8g2_uint_t y = 36;
        x = u8g2_GetDisplayWidth(u8g2);

        drawRA(u8g2, x, y, Rah, Ram, Ras);
        u8g2_DrawUTF8(u8g2, 0, y, "RA");

        y += line_height + 4;
        u8g2_DrawUTF8(u8g2, 0, y, "Dec");
        drawDec(u8g2, x, y, decsign, decdeg, decmin, decsec);
      }
    }
    else if (page == 1)
    {
      if (telInfo.hasInfoAz && telInfo.hasInfoAlt)
      {
        char Azdeg[4];
        char Azm[3];
        char Azs[3];
        char Altsign[2];
        char Altdeg[3];
        char Altmin[3];
        char Altsec[3];
        memcpy(Azdeg, telInfo.TempAz, 3);
        Azdeg[3] = '\0';
        memcpy(Azm, &telInfo.TempAz[4], 2);
        Azm[2] = '\0';
        memcpy(Azs, &telInfo.TempAz[7], 2);
        Azs[2] = '\0';
        memcpy(Altsign, telInfo.TempAlt, 1);
        Altsign[1] = '\0';
        memcpy(Altdeg, &telInfo.TempAlt[1], 2);
        Altdeg[2] = '\0';
        memcpy(Altmin, &telInfo.TempAlt[4], 2);
        Altmin[2] = '\0';
        memcpy(Altsec, &telInfo.TempAlt[7], 2);
        Altsec[2] = '\0';

        u8g2_uint_t y = 36;
        u8g2_uint_t startpos = u8g2_GetUTF8Width(u8g2, "123456");
        x = startpos;
        x = u8g2_GetDisplayWidth(u8g2);
        u8g2_DrawUTF8(u8g2, 0, y, "Az.");
        drawAz(u8g2, x, y, Azdeg, Azm, Azs);

        y += line_height + 4;
        x = startpos;
        x = u8g2_GetDisplayWidth(u8g2);

        drawDec(u8g2, x, y, Altsign, Altdeg, Altmin, Altsec);
        u8g2_DrawUTF8(u8g2, 0, y, "Alt.");


      }
    }
    else if (page == 2)
    {
      if (telInfo.hasInfoUTC && telInfo.hasInfoSideral)
      {
        char Rah[3];
        char Ram[3];
        char Ras[3];
        u8g2_uint_t y = 36;

        x = u8g2_GetDisplayWidth(u8g2);
        memcpy(Rah, telInfo.TempUTC, 2);
        Rah[2] = '\0';
        memcpy(Ram, &telInfo.TempUTC[3], 2);
        Ram[2] = '\0';
        memcpy(Ras, &telInfo.TempUTC[6], 2);
        Ras[2] = '\0';
        u8g2_DrawUTF8(u8g2, 0, y, "UTC");
        drawRA(u8g2, x, y, Rah, Ram, Ras);

        y += line_height + 4;
        memcpy(Rah, telInfo.TempSideral, 2);
        Rah[2] = '\0';
        memcpy(Ram, &telInfo.TempSideral[3], 2);
        Ram[2] = '\0';
        memcpy(Ras, &telInfo.TempSideral[6], 2);
        Ras[2] = '\0';
        u8g2_DrawUTF8(u8g2, 0, y, "Sideral");
        drawRA(u8g2, x, y, Rah, Ram, Ras);
      }
    }

  } while (u8g2_NextPage(u8g2));
  lastpageupdate = millis();
}

u8g2_uint_t drawRA(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char* Rah, const char* Ram, const char* Ras)
{
  u8g2_uint_t step0 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00") + u8g2_GetUTF8Width(u8g2, "0") / 3;
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0") - u8g2_GetUTF8Width(u8g2, "0") / 3;
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step0 + 1;
    u8g2_DrawUTF8(u8g2, x, y, Ras);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, Ram);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, Rah);
  }
  else
  {
    x += u8g2_GetUTF8Width(u8g2, "+") + u8g2_GetUTF8Width(u8g2, "0") / 3;
    u8g2_DrawUTF8(u8g2, x, y, Rah);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, Ram);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, ":");
    x += step2;
    x += u8g2_DrawUTF8(u8g2, x, y, Ras);
    return x;
  }
}

u8g2_uint_t drawDec(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char* decsign, const char* decdeg, const char* decmin, const char* decsec)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0");
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step1 + 1;
    u8g2_DrawUTF8(u8g2, x, y, decsec);
    x -= step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x -= step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, decmin);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x -= step1;
    u8g2_DrawUTF8(u8g2, x, y, decdeg);
    x -= u8g2_GetUTF8Width(u8g2, "+");
    if (decsign[0] == '-')
    {
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
    else
    {
      x -= u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
  }
  else
  {
    if (decsign[0] == '-')
    {
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
      u8g2_DrawUTF8(u8g2, x, y, decsign);
    }
    else
    {
      u8g2_DrawUTF8(u8g2, x, y, decsign);
      x += u8g2_GetUTF8Width(u8g2, "0") / 3;
    }
    x += u8g2_GetUTF8Width(u8g2, "+");
    u8g2_DrawUTF8(u8g2, x, y, decdeg);
    x += step1;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, decmin);
    x += step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x += step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    x += u8g2_DrawUTF8(u8g2, x, y, decsec);
  }
}

u8g2_uint_t drawAz(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char* Azdeg, const char* Azmin, const char* Azsec)
{
  char DEGREE_SYMBOL[] = { 0xB0, '\0' };
  u8g2_uint_t step0 = u8g2_GetUTF8Width(u8g2, "000");
  u8g2_uint_t step1 = u8g2_GetUTF8Width(u8g2, "00");
  u8g2_uint_t step2 = u8g2_GetUTF8Width(u8g2, "0");
  if (x == u8g2_GetDisplayWidth(u8g2))
  {
    x -= step1 + 1;
    u8g2_DrawUTF8(u8g2, x, y, Azsec);
    x -= step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x -= step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, Azmin);
    x -= step2;
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x -= step0;
    u8g2_DrawUTF8(u8g2, x, y, Azdeg);
  }
  else
  {
    u8g2_DrawUTF8(u8g2, x, y, Azdeg);
    x += step1 + u8g2_GetUTF8Width(u8g2, "0");
    u8g2_DrawUTF8(u8g2, x, y, DEGREE_SYMBOL);
    x += step2;
    u8g2_DrawUTF8(u8g2, x, y, Azmin);
    x += step1 + u8g2_GetUTF8Width(u8g2, " ") / 2;
    u8g2_DrawUTF8(u8g2, x, y, "'");
    x += step2 - u8g2_GetUTF8Width(u8g2, " ") / 2;
    x += u8g2_DrawUTF8(u8g2, x, y, Azsec);
  }
}

