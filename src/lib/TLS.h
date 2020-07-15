// -----------------------------------------------------------------------------------
// Time/Location source support

#pragma once
#include "Julian.h"

#if TIME_LOCATION_SOURCE == GPS
// -----------------------------------------------------------------------------------
// GPS TLS support
// uses the specified serial port

#include <TinyGPS++.h>          // http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;

#ifndef SerialGPS
  #error "Configuration (Config.h): Setting TLS GPS, requires adding a line to identify the serial port '#define SerialGPS Serial6' for example."
#endif
#ifndef SerialGPSBaud
  #define SerialGPSBaud 4800
#endif

class timeLocationSource {
  public:
    bool active=false;
    bool serialActive=false;

    // initialize
    bool init() {
      active=false;
      serialActive=false;
      return active;
    }

    boolean poll() {
      if (!serialActive) { SerialGPS.begin(SerialGPSBaud); serialActive=true; }
      if (gps.location.isValid() && siteIsValid() && gps.date.isValid() && gps.time.isValid() && timeIsValid()) { active=true; return true; }
      while (SerialGPS.available() > 0) gps.encode(SerialGPS.read());
      return false;
    }

    boolean timeIsValid() {
      if ((gps.date.year() >= 0) && (gps.date.year() <= 3000) && (gps.date.month() >= 1) && (gps.date.month() <= 12) && (gps.date.day() >= 1) && (gps.date.day() <= 31) &&
          (gps.time.hour() >= 0) && (gps.time.hour() <= 23) && (gps.time.minute() >= 0) && (gps.time.minute() <= 59) && (gps.time.second() >= 0) && (gps.time.second() <= 59)) return true; else return false;
    }

    boolean siteIsValid() {
      if (gps.location.lat() >= -90 && gps.location.lat() <= 90 && gps.location.lng() >= -360 && gps.location.lng() <= 360) return true; else return false;
    }

    // set the GPS's time (does nothing)
    void set(double JD, double LMT) {
    }
    
    // get the GPS's time (local standard time)
    void get(double &JD, double &LMT) {
      if (!active) return;
      if (!timeIsValid()) return;

      JD=julian(gps.date.year(),gps.date.month(),gps.date.day());
      LMT=(gps.time.hour()+(gps.time.minute()/60.0)+(gps.time.second()/3600.0))-timeZone;
      if (LMT < 0)   { LMT+=24.0; JD-=1; }
      if (LMT >= 24) { LMT-=24.0; JD+=1; }
    }

    // get the GPS's location
    void getSite(double &LAT, double &LONG) {
      if (!active) return;
      if (!siteIsValid()) return;

      LAT =gps.location.lat();
      LONG=-gps.location.lng();
    }
};

#elif TIME_LOCATION_SOURCE == DS3234S
// -----------------------------------------------------------------------------------
// DS3234 RTC support 
// uses the default SPI port and CS (DS3234_CS_PIN from Pins.xxx.h)

#include <SparkFunDS3234RTC.h>  // https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library/archive/master.zip

class timeLocationSource {
  public:
    bool active=false;

    // initialize (also enables the RTC PPS if available)
    bool init() {
      rtc.begin(DS3234_CS_PIN);
      rtc.update();
      rtc.writeSQW(SQW_SQUARE_1);
      active=true;
      return active;
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      int y,mo,d,h;
      double m,s;

      greg(JD,&y,&mo,&d); y-=2000; if (y >= 100) y-=100;
    
      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;

      int dow=(round(JD)%7)+1;
      rtc.setTime(floor(s), floor(m), h, dow, d, mo, y);
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      if ((rtc.year() >= 0) && (rtc.year() <= 99) && (rtc.month() >= 1) && (rtc.month() <= 12) && (rtc.date() >= 1) && (rtc.date() <= 31) &&
          (rtc.hour() >= 0) && (rtc.hour() <= 23) && (rtc.minute() >= 0) && (rtc.minute() <= 59) && (rtc.second() >= 0) && (rtc.second() <= 59)) {
        int y1=rtc.year(); if (y1>11) y1=y1+2000; else y1=y1+2100;
        JD=julian(y1,rtc.month(),rtc.date());
        LMT=(rtc.hour()+(rtc.minute()/60.0)+(rtc.second()/3600.0));
      }
    }

    // get the location (does nothing)
    void getSite(double &LAT, double &LONG) {
    }

};

#elif TIME_LOCATION_SOURCE == DS3231
// -----------------------------------------------------------------------------------
// DS3231 RTC support 
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)

#include <Wire.h>
#include <RtcDS3231.h>          // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3231<TwoWire> _Rtc(HAL_Wire);

class timeLocationSource {
  public:
    bool active=false;

    // initialize (also enables the RTC PPS if available)
    bool init() {
      HAL_Wire.begin();
      HAL_Wire.beginTransmission(0x68);
      bool error = HAL_Wire.endTransmission() != 0;
      if (!error) {
        _Rtc.Begin();
        if (!_Rtc.GetIsRunning()) _Rtc.SetIsRunning(true);
  
        // see if the RTC is present
        if (_Rtc.GetIsRunning()) {
          // frequency 0 (1Hz) on the SQW pin
          _Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
          _Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
          active=true;
        }
      }

      return active;
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      if (!active) return;
      int yy,y,mo,d,h;
      double m,s;
    
      greg(JD,&y,&mo,&d); yy=y; y-=2000; if (y >= 100) y-=100;
    
      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;
      
      RtcDateTime updateTime = RtcDateTime(yy, mo, d, h, floor(m), floor(s));
      _Rtc.SetDateTime(updateTime);
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      if (!active) return;

      RtcDateTime now = _Rtc.GetDateTime();
      if ((now.Year() >= 2018) && (now.Year() <= 3000) && (now.Month() >= 1) && (now.Month() <= 12) && (now.Day() >= 1) && (now.Day() <= 31) &&
          (now.Hour() >= 0) && (now.Hour() <= 23) && (now.Minute() >= 0) && (now.Minute() <= 59) && (now.Second() >= 0) && (now.Second() <= 59)) {
        JD=julian(now.Year(),now.Month(),now.Day());
        LMT=(now.Hour()+(now.Minute()/60.0)+(now.Second()/3600.0));
      }
    }

    // get the location (does nothing)
    void getSite(double &LAT, double &LONG) {
    }
};

#elif TIME_LOCATION_SOURCE == DS3234M
// -----------------------------------------------------------------------------------
// DS3234 RTC support 
// uses the default SPI port

#include <SPI.h>
#include <RtcDS3234.h>          // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3234<SPIClass> _Rtc(SPI, DS3234_CS_PIN);

class timeLocationSource {
  public:
    bool active=false;

    // initialize (also enables the RTC PPS if available)
    bool init() {
      SPI.begin();
      _Rtc.Begin();
      if (!_Rtc.GetIsRunning()) _Rtc.SetIsRunning(true);

      // see if the RTC is present
      if (_Rtc.GetIsRunning()) {
        // frequency 0 (1Hz) on the SQW pin
        _Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeClock);
        _Rtc.SetSquareWavePinClockFrequency(DS3234SquareWaveClock_1Hz);
        active=true;
      }
  
      return active;
    }

    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      if (!active) return;

      int yy,y,mo,d,h;
      double m,s;
    
      greg(JD,&y,&mo,&d); yy=y; y-=2000; if (y >= 100) y-=100;
    
      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;
      
      RtcDateTime updateTime = RtcDateTime(yy, mo, d, h, floor(m), floor(s));
      _Rtc.SetDateTime(updateTime);
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      if (!active) return;

      RtcDateTime now = _Rtc.GetDateTime();
      if ((now.Year() >= 2018) && (now.Year() <= 3000) && (now.Month() >= 1) && (now.Month() <= 12) && (now.Day() >= 1) && (now.Day() <= 31) &&
          (now.Hour() >= 0) && (now.Hour() <= 23) && (now.Minute() >= 0) && (now.Minute() <= 59) && (now.Second() >= 0) && (now.Second() <= 59)) {
        JD=julian(now.Year(),now.Month(),now.Day());
        LMT=(now.Hour()+(now.Minute()/60.0)+(now.Second()/3600.0));
      }
    }

    // get the location (does nothing)
    void getSite(double &LAT, double &LONG) {
    }
};

#elif TIME_LOCATION_SOURCE == TEENSY
// -----------------------------------------------------------------------------------
// TEENSY 3.2 RTC support 
 
#include <TimeLib.h>            //https://github.com/PaulStoffregen/Time/archive/master.zip

class timeLocationSource {
  public:
    bool active=false;

    // initialize (also enables the RTC PPS if available)
    bool init() {
      active=true;
      return active;
    }
 
    // set the RTC's time (local standard time)
    void set(double JD, double LMT) {
      int y,mo,d,h;
      double m,s;
      
      greg(JD,&y,&mo,&d); y-=2000; if (y >= 100) y-=100;

      double f1=fabs(LMT)+0.000139;
      h=floor(f1);
      m=(f1-h)*60.0;
      s=(m-floor(m))*60.0;
      //  dow=(round(J)%7)+1;

      setTime(h, floor(m), floor(s), d, mo, y);   //set current system time

      unsigned long TeensyTime;
      TeensyTime = now();                         //get time in epoch
      Teensy3Clock.set(TeensyTime);               //set Teensy time
    }
    
    // get the RTC's time (local standard time)
    void get(double &JD, double &LMT) {
      unsigned long TeensyTime;

      TeensyTime = Teensy3Clock.get();            //get time from Teensy RTC
      setTime(TeensyTime);                        //set system time

      if ((year() >= 0) && (year() <= 3000) && (month() >= 1) && (month() <= 12) && (day() >= 1) && (day() <= 31) &&
          (hour() >= 0) && (hour() <= 23) && (minute() >= 0) && (minute() <= 59) && (second() >= 0) && (second() <= 59)) 
      {
          int y1=year();
          JD=julian(y1,month(),day());
          LMT=(hour()+(minute()/60.0)+(second()/3600.0));
      }
    }

    // get the location (does nothing)
    void getSite(double &LAT, double &LONG) {
    }

};

#else
// -----------------------------------------------------------------------------------
// empty class if no TLS is present

class timeLocationSource {
  public:
    bool active=false;

    // initialize (also enables the RTC PPS if available)
    bool init() {
      active=false;
      return true;
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

    // get the location (does nothing)
    void getSite(double &LAT, double &LONG) {
    }
};
#endif

timeLocationSource tls;
