// -----------------------------------------------------------------------------------
// Support for real time clocks (RTC wrapper)

#pragma once
#include "../../lib/Julian.h"

#if defined(RTC_DS3234)
// -----------------------------------------------------------------------------------
// DS3234 RTC support 
// uses the default SPI port and CS (DS3234_CS_PIN from Pins.xxx.h)

#include <SparkFunDS3234RTC.h>  //https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library/archive/master.zip

class rtcw {
  public:
    // initialize (also enables the RTC PPS if available)
    void init() {
      rtc.begin(DS3234_CS_PIN); rtc.update();
      rtc.writeSQW(SQW_SQUARE_1);
      active=true;
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      int y,mo,d,h;
      double m,s;

      greg(JD,&y,&mo,&d); y-=2000; if (y>=100) y-=100;
    
      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;

      int dow=(round(JD)%7)+1;
      rtc.setTime(floor(s), floor(m), h, dow, d, mo, y);
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      if ((rtc.year()>=0) && (rtc.year()<=99) && (rtc.month()>=1) && (rtc.month()<=12) && (rtc.date()>=1) && (rtc.date()<=31) &&
          (rtc.hour()>=0) && (rtc.hour()<=23) && (rtc.minute()>=0) && (rtc.minute()<=59) && (rtc.second()>=0) && (rtc.second()<=59)) {
        int y1=rtc.year(); if (y1>11) y1=y1+2000; else y1=y1+2100;
        JD=julian(y1,rtc.month(),rtc.date());
        LMT=(rtc.hour()+(rtc.minute()/60.0)+(rtc.second()/3600.0));
      }
    }
  private:
    bool active=false;
};

#elif defined(RTC_DS3231)
// -----------------------------------------------------------------------------------
// DS3231 RTC support 
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)

#include <Wire.h>
#include <RtcDS3231.h>          // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3231<TwoWire> _Rtc(HAL_Wire);

class rtcw {
  public:
    // initialize (also enables the RTC PPS if available)
    void init() {
      _Rtc.Begin(); if (!_Rtc.GetIsRunning()) _Rtc.SetIsRunning(true);
      // frequency 0 (1Hz) on the SQW pin
      _Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
      _Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
      active=true;
#if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.end();
#endif
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      if (!active) return;
#if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.begin();
#endif

      int yy,y,mo,d,h;
      double m,s;
    
      greg(JD,&y,&mo,&d); yy=y; y-=2000; if (y>=100) y-=100;
    
      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;
      
      RtcDateTime updateTime = RtcDateTime(yy, mo, d, h, floor(m), floor(s));
      _Rtc.SetDateTime(updateTime);

#if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.end();
#endif
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      if (!active) return;
#if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.begin();
#endif

      RtcDateTime now = _Rtc.GetDateTime();
      if ((now.Year()>=2018) && (now.Year()<=3000) && (now.Month()>=1) && (now.Month()<=12) && (now.Day()>=1) && (now.Day()<=31) &&
          (now.Hour()>=0) && (now.Hour()<=23) && (now.Minute()>=0) && (now.Minute()<=59) && (now.Second()>=0) && (now.Second()<=59)) {
        JD=julian(now.Year(),now.Month(),now.Day());
        LMT=(now.Hour()+(now.Minute()/60.0)+(now.Second()/3600.0));
      }

#if defined(ESP32) & defined(WIRE_END_SUPPORT)
      HAL_Wire.end();
#endif
    }
  private:
    bool active=false;
};

#else
// -----------------------------------------------------------------------------------
// empty class if no RTC is present

class rtcw {
  public:
    // initialize (also enables the RTC PPS if available)
    void init() {
      active=true;
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      (void)JD;
      (void)LMT;
      if (!active) return;
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      (void)JD;
      (void)LMT;
      if (!active) return;
    }

  private:
    bool active=false;
};
#endif

rtcw urtc;
