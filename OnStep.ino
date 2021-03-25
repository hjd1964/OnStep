/*
 * Title       OnStep
 * by          Howard Dutton
 *
 * Copyright (C) 2012 to 2021 Howard Dutton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Description:
 *   Full featured stepper motor telescope microcontroller for Equatorial and
 *   Alt-Azimuth mounts, with the LX200 derived command set.
 *
 * Author: Howard Dutton
 *   http://www.stellarjourney.com
 *   hjd1964@gmail.com
 *
 * Revision history, and newer versions:
 *   See GitHub: https://github.com/hjd1964/OnStep
 *
 * Documentation:
 *   https://groups.io/g/onstep/wiki/home
 *
 * Discussion, Questions, ...etc
 *   https://groups.io/g/onstep
 */

// Use Config.h to configure OnStep to your requirements

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate          __DATE__
#define FirmwareVersionMajor  4
#define FirmwareVersionMinor  23      // minor version 0 to 99
#define FirmwareVersionPatch  "o"     // for example major.minor patch: 1.3c
#define FirmwareVersionConfig 3       // internal, for tracking configuration file changes
#define FirmwareName          "On-Step"
#define FirmwareTime          __TIME__

#include "Constants.h"

// On first upload OnStep automatically initializes a host of settings in nv memory (EEPROM.)
// This option forces that initialization again.
// Change to ON, upload OnStep and nv will be reset to default. Wait about 30 seconds then set to OFF and upload again.
// *** IMPORTANT: This option must not be left set to true or it will cause excessive wear of EEPROM or FLASH ***
#define NV_FACTORY_RESET OFF

// Enable additional debugging and/or status messages on the specified DebugSer port
// Note that the DebugSer port cannot be used for normal communication with OnStep
#define DEBUG OFF             // default=OFF, use "DEBUG ON" for background errors only, use "DEBUG VERBOSE" for all errors and status messages
#define DebugSer SerialA      // default=SerialA, or Serial4 for example (always 9600 baud)

#include <errno.h>
#include <math.h>

#include "src/sd_drivers/Models.h"
#include "Config.h"
#include "src/pinmaps/Models.h"
#include "src/HAL/HAL.h"
#include "Validate.h"

// Helper macros for debugging, with less typing
#if DEBUG != OFF
  #define D(x)       DebugSer.print(x)
  #define DF(x)      DebugSer.print(F(x))
  #define DL(x)      DebugSer.println(x)
  #define DLF(x)     DebugSer.println(F(x))
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE
  #define V(x)        DebugSer.print(x)
  #define VF(x)       DebugSer.print(F(x))
  #define VL(x)       DebugSer.println(x)
  #define VLF(x)      DebugSer.println(F(x))
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif
// ---------------------------------------------------------------------------------------------------

#include "src/lib/St4SerialMaster.h"
#include "src/lib/FPoint.h"
#include "src/lib/Heater.h"
#include "src/lib/Intervalometer.h"
#include "Globals.h"
#include "src/lib/Julian.h"
#include "src/lib/Misc.h"
#include "src/lib/Sound.h"
#include "src/lib/Coord.h"
#include "Align.h"
#include "src/lib/Library.h"
#include "src/lib/Command.h"
#include "src/lib/TLS.h"
#include "src/lib/Weather.h"
weather ambient;

#if SERIAL_B_ESP_FLASHING == ON || defined(AddonTriggerPin)
  #include "src/lib/flashAddon.h"
  flashAddon fa;
#endif

#if ROTATOR == ON
  #include "src/lib/Rotator.h"
  rotator rot;
#endif

#if FOCUSER1 == ON || FOCUSER2 == ON
  #include "src/lib/Focuser.h"
  #if FOCUSER1 == ON
    #if AXIS4_DRIVER_DC_MODE != OFF
      #include "src/lib/FocuserDC.h"
      focuserDC foc1;
    #else
      #include "src/lib/FocuserStepper.h"
      focuserStepper foc1;
    #endif
  #endif
  #if FOCUSER2 == ON
    #if AXIS5_DRIVER_DC_MODE != OFF
      #include "src/lib/FocuserDC.h"
      focuserDC foc2;
    #else
      #include "src/lib/FocuserStepper.h"
      focuserStepper foc2;
    #endif
  #endif
#endif

// support for TMC2130, TMC5160, etc. stepper drivers in SPI mode
#if (AXIS1_DRIVER_MODEL == TMC_SPI && AXIS2_DRIVER_MODEL == TMC_SPI) || \
    (ROTATOR == ON && AXIS3_DRIVER_MODEL == TMC_SPI) || \
    (FOCUSER1 == ON && AXIS4_DRIVER_MODEL == TMC_SPI) || \
    (FOCUSER2 == ON && AXIS5_DRIVER_MODEL == TMC_SPI)
  #include "src/lib/SoftSPI.h"
  #include "src/lib/TMC_SPI.h"
  #if AXIS1_DRIVER_MODEL == TMC_SPI
    tmcSpiDriver tmcAxis1(Axis1_M2,Axis1_M1,(AXIS1_DRIVER_STATUS == TMC_SPI)?Axis1_M3:-1,Axis1_M0,AXIS1_DRIVER_SUBMODEL,AXIS1_DRIVER_RSENSE);
  #endif
  #if AXIS2_DRIVER_MODEL == TMC_SPI
    tmcSpiDriver tmcAxis2(Axis2_M2,Axis2_M1,(AXIS2_DRIVER_STATUS == TMC_SPI)?Axis2_M3:-1,Axis2_M0,AXIS2_DRIVER_SUBMODEL,AXIS2_DRIVER_RSENSE);
  #endif
  #if AXIS3_DRIVER_MODEL == TMC_SPI
    tmcSpiDriver tmcAxis3(Axis3_M2,Axis3_M1,-1,Axis3_M0,AXIS3_DRIVER_SUBMODEL,AXIS3_DRIVER_RSENSE);
  #endif
  #if AXIS4_DRIVER_MODEL == TMC_SPI
    tmcSpiDriver tmcAxis4(Axis4_M2,Axis4_M1,-1,Axis4_M0,AXIS4_DRIVER_SUBMODEL,AXIS4_DRIVER_RSENSE);
  #endif
  #if AXIS5_DRIVER_MODEL == TMC_SPI
    tmcSpiDriver tmcAxis5(Axis5_M2,Axis5_M1,-1,Axis5_M0,AXIS5_DRIVER_SUBMODEL,AXIS5_DRIVER_RSENSE);
  #endif
#endif

void setup() {
  initPre();

  // initialize the ESP8266 Addon flasher
#if SERIAL_B_ESP_FLASHING == ON
  fa.init(-1,AddonResetPin,AddonBootModePin);
#elif defined(AddonTriggerPin)
  fa.init(AddonTriggerPin,AddonResetPin,AddonBootModePin);
#endif

  // take a half-second to let any connected devices come up before we start setting up pins
  delay(500);

#if DEBUG != OFF
  // initialize USB serial debugging early, so we can use DebugSer.print() for debugging, if needed
  DebugSer.begin(9600); delay(5000); DebugSer.flush(); VLF(""); VLF("");
#endif

  // say hello
  VF("MSG: OnStep "); V(FirmwareVersionMajor); V("."); V(FirmwareVersionMinor); VL(FirmwareVersionPatch);
  VF("MSG: MCU =  "); VF(MCU_STR); V(", "); VF("Pinmap = "); VLF(PINMAP_STR);

  // call hardware specific initialization
  VLF("MSG: Init HAL");
  HAL_Initialize();

  VLF("MSG: Init serial");

  // take a half-second to let the serial buffer empty before possibly restarting the debug port
  delay(500);
  SerialA.begin(SERIAL_A_BAUD_DEFAULT);
#ifdef HAL_SERIAL_B_ENABLED
  #ifdef SERIAL_B_RX
    SerialB.begin(SERIAL_B_BAUD_DEFAULT, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
  #else
    SerialB.begin(SERIAL_B_BAUD_DEFAULT);
  #endif
#endif
#ifdef HAL_SERIAL_C_ENABLED
  SerialC.begin(SERIAL_C_BAUD_DEFAULT);
#endif
#ifdef HAL_SERIAL_D_ENABLED
  #ifdef SERIAL_D_RX
    SerialD.begin(SERIAL_D_BAUD_DEFAULT, SERIAL_8N1, SERIAL_D_RX, SERIAL_D_TX);
  #else
    SerialD.begin(SERIAL_D_BAUD_DEFAULT);
  #endif
#endif
#ifdef HAL_SERIAL_E_ENABLED
  SerialE.begin(SERIAL_E_BAUD_DEFAULT);
#endif
#if TIME_LOCATION_SOURCE == GPS
  #ifdef SERIAL_GPS_RX
    SerialGPS.begin(SERIAL_GPS_BAUD, SERIAL_8N1, SERIAL_GPS_RX, SERIAL_GPS_TX);
  #else
    SerialGPS.begin(SERIAL_GPS_BAUD);
  #endif
#endif
#if ST4_HAND_CONTROL == ON && ST4_INTERFACE != OFF
  SerialST4.begin();
#endif

  // take another two seconds to be sure Serial ports are online
  delay(2000);

  // set pins for input/output as specified in Config.h and PinMap.h
  VLF("MSG: Init pins");
  initPins();

  // get the TLS ready (if present)
  VLF("MSG: Init TLS");
  if (!tls.init()) generalError=ERR_SITE_INIT;
  
  // Check the Non-Volatile Memory
  VF("MSG: Start NV ");
  if (!nv.init()) {
    VLF("");
    SerialA.print("NV (EEPROM) failure!#\r\n");
    while (true) {
      delay(10);
      #ifdef HAL_SERIAL_TRANSMIT
        SerialA.transmit();
      #endif
    }
  }
  V(E2END+1); VLF(" Bytes");

  // if this is the first startup set EEPROM to defaults
  initWriteNvValues();

  // now read any saved values from EEPROM into variables to restore our last state
  VLF("MSG: Read NV settings");
  initReadNvValues();

  // set initial values for some variables
  VLF("MSG: Init startup settings");
  initStartupValues();
  initStartPosition();

  // initialize the Object Library
  VLF("MSG: Init library/catalogs");
  Lib.init();

  // get guiding ready
  VLF("MSG: Init guiding");
  initGuide();
 
  // get weather monitoring ready to go
#ifdef ONEWIRE_DEVICES_PRESENT
  VLF("MSG: Init weather and 1-Wire");
#else
  VLF("MSG: Init weather");
#endif
  if (!ambient.init() && WEATHER_SUPRESS_ERRORS == OFF) generalError=ERR_WEATHER_INIT;

  // setup features
#ifdef FEATURES_PRESENT
  VLF("MSG: Init auxiliary features");
  featuresInit();
#endif

  // this sets up the sidereal timer and tracking rates
  VLF("MSG: Init sidereal timer");
  siderealInterval=nv.readLong(EE_siderealInterval); // the number of 16MHz clocks in one sidereal second (this is scaled to actual processor speed)
  if (siderealInterval < 14360682L || siderealInterval > 17551944L) { DF("ERR, setup(): bad NV siderealInterval ("); D(siderealInterval); DL(")"); siderealInterval=masterSiderealInterval; }
  siderealRate=siderealInterval/stepsPerSecondAxis1;
  timerRateAxis1=siderealRate;
  timerRateAxis2=siderealRate;

  // backlash takeup rates
  backlashTakeupRate=siderealRate/TRACK_BACKLASH_RATE;
  timerRateBacklashAxis1=siderealRate/TRACK_BACKLASH_RATE;
  timerRateBacklashAxis2=(siderealRate/TRACK_BACKLASH_RATE)*timerRateRatio;

  // setup the stepper driver modes
  VLF("MSG: Init motor timers");
  StepperModeTrackingInit();

  // starts the hardware timers that keep sidereal time, move the motors, etc.
  setTrackingRate(DefaultTrackingRate);
  setDeltaTrackingRate();
  initStartTimers();
 
  // tracking autostart
#if TRACK_AUTOSTART == ON
  #if MOUNT_TYPE != ALTAZM

    // tailor behavior depending on TLS presence
    if (!tls.active) {
      VLF("MSG: Tracking autostart - TLS/orientation unknown, limits disabled");
      setHome();
      safetyLimitsOn=false;
    } else {
      if (parkStatus != Parked) {
        VLF("MSG: Tracking autostart - TLS/orientation unknown, limits disabled");
        setHome();
        safetyLimitsOn=false;
      } else {
        // parking implies the orientation of the mount and the location are known
        VLF("MSG: Tracking autostart - assuming TLS/orientation are correct, limits enabled and automatic unpark");
        unPark(true);
      }
    }

    // start tracking
    trackingState=TrackingSidereal;
    enableStepperDrivers();
  #else
    #warning "Tracking autostart ignored for MOUNT_TYPE ALTAZM"
  #endif
#else
  if (parkStatus == Parked) {
    VLF("MSG: Restoring parked telescope pointing state");
    unPark(false);
  }
#endif

  // start rotator if present
#if ROTATOR == ON
  VLF("MSG: Init rotator");
  rot.init(Axis3_STEP,Axis3_DIR,Axis3_EN,EE_rotBaseAxis3,AXIS3_STEP_RATE_MAX,axis3Settings.stepsPerMeasure,axis3Settings.min,axis3Settings.max);
  if (axis3Settings.reverse == ON) rot.setReverseState(HIGH);
  rot.setDisableState(AXIS3_DRIVER_DISABLE);
  
  #if AXIS3_DRIVER_MODEL == TMC_SPI
    tmcAxis3.setup(AXIS3_DRIVER_INTPOL,AXIS3_DRIVER_DECAY_MODE,AXIS3_DRIVER_CODE,axis3Settings.IRUN,axis3Settings.IRUN);
    delay(150);
    tmcAxis3.setup(AXIS3_DRIVER_INTPOL,AXIS3_DRIVER_DECAY_MODE,AXIS3_DRIVER_CODE,axis3Settings.IRUN,axis3SettingsEx.IHOLD);
  #endif
  
  rot.powerDownActive(AXIS3_DRIVER_POWER_DOWN == ON);
#endif

  // start focusers if present
#if FOCUSER1 == ON
  VLF("MSG: Init focuser1");
  foc1.init(Axis4_STEP,Axis4_DIR,Axis4_EN,EE_focBaseAxis4,AXIS4_STEP_RATE_MAX,axis4Settings.stepsPerMeasure,axis4Settings.min*1000.0,axis4Settings.max*1000.0,AXIS4_LIMIT_MIN_RATE);
  if (AXIS4_DRIVER_DC_MODE != OFF) foc1.setPhase1();
  if (axis4Settings.reverse == ON) foc1.setReverseState(HIGH);
  foc1.setDisableState(AXIS4_DRIVER_DISABLE);

  #if AXIS4_DRIVER_MODEL == TMC_SPI
    tmcAxis4.setup(AXIS4_DRIVER_INTPOL,AXIS4_DRIVER_DECAY_MODE,AXIS4_DRIVER_CODE,axis4Settings.IRUN,axis4Settings.IRUN);
    delay(150);
    tmcAxis4.setup(AXIS4_DRIVER_INTPOL,AXIS4_DRIVER_DECAY_MODE,AXIS4_DRIVER_CODE,axis4Settings.IRUN,axis4SettingsEx.IHOLD);
  #endif

  foc1.powerDownActive(AXIS4_DRIVER_POWER_DOWN == ON, AXIS4_DRIVER_POWER_DOWN == STARTUP);
#endif

#if FOCUSER2 == ON
  VLF("MSG: Init focuser2");
  foc2.init(Axis5_STEP,Axis5_DIR,Axis5_EN,EE_focBaseAxis5,AXIS5_STEP_RATE_MAX,axis5Settings.stepsPerMeasure,axis5Settings.min*1000.0,axis5Settings.max*1000.0,AXIS5_LIMIT_MIN_RATE);
  if (AXIS5_DRIVER_DC_MODE != OFF) foc2.setPhase2();
  if (axis5Settings.reverse == ON) foc2.setReverseState(HIGH);
  foc2.setDisableState(AXIS5_DRIVER_DISABLE);

  #if AXIS5_DRIVER_MODEL == TMC_SPI
    tmcAxis5.setup(AXIS5_DRIVER_INTPOL,AXIS5_DRIVER_DECAY_MODE,AXIS5_DRIVER_CODE,axis5Settings.IRUN,axis5Settings.IRUN);
    delay(150);
    tmcAxis5.setup(AXIS5_DRIVER_INTPOL,AXIS5_DRIVER_DECAY_MODE,AXIS5_DRIVER_CODE,axis5Settings.IRUN,axis5SettingsEx.IHOLD);
  #endif

  foc2.powerDownActive(AXIS5_DRIVER_POWER_DOWN == ON, AXIS5_DRIVER_POWER_DOWN == STARTUP);
#endif

  // finally clear the comms channels
  VLF("MSG: Serial buffer flush");
  delay(500);
  SerialA.flush();
  while (SerialA.available()) SerialA.read();
#ifdef HAL_SERIAL_B_ENABLED
  SerialB.flush();
  while (SerialB.available()) SerialB.read();
#endif
#ifdef HAL_SERIAL_C_ENABLED
  SerialC.flush();
  while (SerialC.available()) SerialC.read();
#endif
#ifdef HAL_SERIAL_D_ENABLED
  SerialD.flush();
  while (SerialD.available()) SerialD.read();
#endif
#ifdef HAL_SERIAL_E_ENABLED
  SerialE.flush();
  while (SerialE.available()) SerialE.read();
#endif
  delay(500);

  // prep counters (for keeping time in main loop)
  cli(); siderealTimer=lst; guideSiderealTimer=lst; pecSiderealTimer=lst; sei();
  last_loop_micros=micros();

  VLF("MSG: OnStep is ready"); VL("");
}

void loop() {
  loop2();
  Align.model(0); // GTA compute pointing model, this will call loop2() during extended processing
}

void loop2() {
  // GUIDING -------------------------------------------------------------------------------------------
  ST4();
  if ((trackingState != TrackingMoveTo) && (parkStatus == NotParked)) guide();

#if HOME_SENSE != OFF
  // AUTOMATIC HOMING ----------------------------------------------------------------------------------
  checkHome();
#endif

  // 1/100 SECOND TIMED --------------------------------------------------------------------------------
  cli(); long lstNow=lst; sei();
  if (lstNow != siderealTimer) {
    siderealTimer=lstNow;

#ifdef ESP32
    timerSupervisor(true);
#endif
    
#if AXIS1_PEC == ON
    // PERIODIC ERROR CORRECTION
    pec();
#endif

    // FLASH LED DURING SIDEREAL TRACKING
#if LED_STATUS == ON
    if (trackingState == TrackingSidereal) {
      if (siderealTimer%20L == 0L) { if (ledOn) { digitalWrite(LEDnegPin,HIGH); ledOn=false; } else { digitalWrite(LEDnegPin,LOW); ledOn=true; } }
    }
#endif

    // SIDEREAL TRACKING DURING GOTOS
    // keeps the target where it's supposed to be while doing gotos
    if (trackingState == TrackingMoveTo) {
      moveTo();
      if (lastTrackingState == TrackingSidereal) {
        origTargetAxis1.fixed+=fstepAxis1.fixed;
        origTargetAxis2.fixed+=fstepAxis2.fixed;
        // don't advance the target during meridian flips or sync
        if (getInstrPierSide() == PierSideEast || getInstrPierSide() == PierSideWest) {
          cli();
          targetAxis1.fixed+=fstepAxis1.fixed;
          targetAxis2.fixed+=fstepAxis2.fixed;
          sei();
        }
      }
    }

    // ROTATOR/FOCUSERS, MOVE THE TARGET
#if ROTATOR == ON
    rot.poll(trackingState == TrackingSidereal);
#endif
#if FOCUSER1 == ON
    foc1.poll();
#endif
#if FOCUSER2 == ON
    foc2.poll();
#endif

    // CALCULATE SOME TRACKING RATES, ETC.
    if (lstNow%3 == 0) doFastAltCalc(false);
#if MOUNT_TYPE == ALTAZM
    // figure out the current Alt/Azm tracking rates
    if (lstNow%3 != 0) doHorRateCalc();
#else
    // figure out the current refraction compensated tracking rate
    if (rateCompensation != RC_NONE && lstNow%3 != 0) doRefractionRateCalc();
#endif

    // SAFETY CHECKS
#if LIMIT_SENSE != OFF
    // support for limit switch(es)
    byte limit_1st = digitalRead(LimitPin);
    if (limit_1st == LIMIT_SENSE_STATE) {
      // Wait for a short while, then read again
      delayMicroseconds(50);
      byte limit_2nd = digitalRead(LimitPin);
      if (limit_2nd == LIMIT_SENSE_STATE) {
        // It is still low, there must be a problem
        generalError=ERR_LIMIT_SENSE;
        stopSlewingAndTracking(SS_LIMIT);
      } 
    }
#endif

    // check for fault signal, stop any slew or guide and turn tracking off
#if AXIS1_DRIVER_STATUS == LOW || AXIS1_DRIVER_STATUS == HIGH
    faultAxis1=(digitalRead(Axis1_FAULT) == AXIS1_DRIVER_STATUS);
#elif AXIS1_DRIVER_STATUS == TMC_SPI
    if (lst%2 == 0) faultAxis1=tmcAxis1.error();
#endif
#if AXIS2_DRIVER_STATUS == LOW || AXIS2_DRIVER_STATUS == HIGH
    faultAxis2=(digitalRead(Axis2_FAULT) == AXIS2_DRIVER_STATUS);
#elif AXIS2_DRIVER_STATUS == TMC_SPI
    if (lst%2 == 1) faultAxis2=tmcAxis2.error();
#endif

    if (faultAxis1 || faultAxis2) { generalError=ERR_MOTOR_FAULT; stopSlewingAndTracking(SS_LIMIT_HARD); }

    if (safetyLimitsOn) {
      // check altitude overhead limit and horizon limit
      if (currentAlt < minAlt) { generalError=ERR_ALT_MIN; stopSlewingAndTracking((MOUNT_TYPE == ALTAZM)?SS_LIMIT_AXIS2_MIN:SS_LIMIT); }
      if (currentAlt > maxAlt) { generalError=ERR_ALT_MAX; stopSlewingAndTracking((MOUNT_TYPE == ALTAZM)?SS_LIMIT_AXIS2_MAX:SS_LIMIT); }
    }

    // OPTION TO POWER DOWN AXIS2 IF NOT MOVING
#if AXIS2_DRIVER_POWER_DOWN == ON && MOUNT_TYPE != ALTAZM
    autoPowerDownAxis2();
#endif

    // 0.01S POLLING -------------------------------------------------------------------------------------
#if TIME_LOCATION_SOURCE == GPS
    if ((PPS_SENSE == OFF || ppsSynced) && !tls.active && tls.poll()) {
      SerialGPS.end();
      currentSite=0; nv.update(EE_currentSite,currentSite);

      tls.getSite(latitude,longitude);
      tls.get(JD,LMT);

      timeZone=nv.read(EE_sites+currentSite*25+8)-128;
      timeZone=decodeTimeZone(timeZone);
      UT1=LMT+timeZone;

      nv.writeString(EE_sites+currentSite*25+9,(char*)"GPS");
      setLatitude(latitude);
      nv.writeFloat(EE_sites+currentSite*25+4,longitude);
      updateLST(jd2last(JD,UT1,false));

      if (generalError == ERR_SITE_INIT) generalError=ERR_NONE;

      dateWasSet=true;
      timeWasSet=true;
    }
#endif

    // UPDATE THE UT1 CLOCK
    cli(); long cs=lst; sei();
    double t2=(double)((cs-lst_start)/100.0)/1.00273790935;
    // This just needs to be accurate to the nearest second, it's about 10x better
    UT1=UT1_start+(t2/3600.0);

    // UPDATE AUXILIARY FEATURES
#ifdef FEATURES_PRESENT
    featuresPoll();
#endif
    
    // WEATHER
    if (!isSlewing()) ambient.poll();

    // MONITOR NV CACHE
#if DEBUG == VERBOSE && DEBUG_NV == ON
    static bool lastCommitted=true;
    bool committed=nv.committed();
    if (committed && !lastCommitted) { DLF("MSG: NV commit done"); lastCommitted=committed; }
    if (!committed && lastCommitted) { DLF("MSG: NV data in cache"); lastCommitted=committed; }
#endif

    // TRIGGER ESPFLASH
#if defined(AddonTriggerPin)
    fa.poll();
#endif
  }

  // FASTEST POLLING -----------------------------------------------------------------------------------
#if AXIS1_DRIVER_MODEL == TMC_SPI
  autoModeSwitch();
#endif

#if ROTATOR == ON
  rot.follow(isSlewing());
#endif
#if FOCUSER1 == ON
  foc1.follow(isSlewing());
#endif
#if FOCUSER2 == ON
  foc2.follow(isSlewing());
#endif
  if (!isSlewing()) nv.poll();
  
  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  unsigned long this_loop_micros=micros();
  loop_time=(long)(this_loop_micros-last_loop_micros);
  if (loop_time > worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;
  average_loop_time=(average_loop_time*49+loop_time)/50;

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  unsigned long tempMs=millis();
  static unsigned long housekeepingTimer=0;
  if ((long)(tempMs-housekeepingTimer) > 1000L) {
    housekeepingTimer=tempMs;

#if ROTATOR == ON && MOUNT_TYPE == ALTAZM
    // calculate and set the derotation rate as required
    double h,d; getApproxEqu(&h,&d,true);
    if (trackingState == TrackingSidereal) rot.derotate(h,d);
#endif

    // adjust tracking rate for Alt/Azm mounts
    // adjust tracking rate for refraction
    setDeltaTrackingRate();

    // basic check to see if we're not at home
    if (trackingState != TrackingNone) atHome=false;

#if PPS_SENSE != OFF
    // update clock via PPS
    cli();
    ppsRateRatio=((double)1000000.0/(double)(ppsAvgMicroS));
    if ((long)(micros()-(ppsLastMicroS+2000000UL)) > 0) ppsSynced=false; // if more than two seconds has ellapsed without a pulse we've lost sync
    sei();
  #if LED_STATUS2 == ON
    if (trackingState == TrackingSidereal) {
      if (ppsSynced) { if (led2On) { digitalWrite(LEDneg2Pin,HIGH); led2On=false; } else { digitalWrite(LEDneg2Pin,LOW); led2On=true; } } else { digitalWrite(LEDneg2Pin,HIGH); led2On=false; } // indicate PPS
    }
  #endif
    if (ppsLastRateRatio != ppsRateRatio) { SiderealClockSetInterval(siderealInterval); ppsLastRateRatio=ppsRateRatio; }
#endif

#if LED_STATUS == ON
    // LED indicate PWR on 
    if (trackingState != TrackingSidereal) if (!ledOn) { digitalWrite(LEDnegPin,LOW); ledOn=true; }
#endif
#if LED_STATUS2 == ON
    // LED indicate STOP and GOTO
    if (trackingState == TrackingMoveTo) if (!led2On) { digitalWrite(LEDneg2Pin,LOW); led2On=true; }
  #if PPS_SENSE != OFF
    if (trackingState == TrackingNone) if (led2On) { digitalWrite(LEDneg2Pin,HIGH); led2On=false; }
  #else
    if (trackingState != TrackingMoveTo) if (led2On) { digitalWrite(LEDneg2Pin,HIGH); led2On=false; }
  #endif
#endif

    // SAFETY CHECKS -------------------------------------------------------------------------------------
    // keeps mount from tracking past the meridian limit, past the AXIS1_LIMIT_MAX, or past the Dec limits
    if (safetyLimitsOn) {
      // check for exceeding AXIS1_LIMIT_MIN or AXIS1_LIMIT_MAX
      if (getInstrAxis1() < axis1Settings.min) { generalError=(MOUNT_TYPE==ALTAZM)?ERR_AZM:ERR_UNDER_POLE; stopSlewingAndTracking(SS_LIMIT_AXIS1_MIN); } else
      if (getInstrAxis1() > axis1Settings.max) { generalError=(MOUNT_TYPE==ALTAZM)?ERR_AZM:ERR_UNDER_POLE; stopSlewingAndTracking(SS_LIMIT_AXIS1_MAX); } else
      // check for exceeding Meridian Limits
      if (meridianFlip != MeridianFlipNever) {
        if (getInstrPierSide() == PierSideWest) {
          if (getInstrAxis1() > degreesPastMeridianW && (!(autoMeridianFlip && goToHere(true) == CE_NONE))) { generalError=ERR_MERIDIAN; stopSlewingAndTracking(SS_LIMIT_AXIS1_MAX); }
        } else
        if (getInstrAxis1() < -degreesPastMeridianE) { generalError=ERR_MERIDIAN; stopSlewingAndTracking(SS_LIMIT_AXIS1_MIN); }
      }
    }
    double a2; if (AXIS2_TANGENT_ARM == ON) { cli(); a2=posAxis2/axis2Settings.stepsPerMeasure; sei(); } else a2=getInstrAxis2();
    // check for exceeding AXIS2_LIMIT_MIN or AXIS2_LIMIT_MAX
    if (a2 < axis2Settings.min) { generalError=ERR_DEC; stopSlewingAndTracking(SS_LIMIT_AXIS2_MIN); } else
    if (a2 > axis2Settings.max) { generalError=ERR_DEC; stopSlewingAndTracking(SS_LIMIT_AXIS2_MAX); } else
    // automatically clear error in TA mode
    if (AXIS2_TANGENT_ARM == ON && (trackingState == TrackingSidereal && generalError == ERR_DEC)) generalError=ERR_NONE;

  } else {
    // COMMAND PROCESSING --------------------------------------------------------------------------------
    processCommands();
  }
}

// stops fast motion as required
// SS_ALL_FAST stops slewing but not tracking
// SS_LIMIT stops gotos + spiral guides + tracking
// SS_LIMIT_HARD stops slewing + tracking
// SS_LIMIT_AXIS1_MIN stops gotos + spiral guides + tracking, also stops/blocks RA/Az guides in the wrong direction
// SS_LIMIT_AXIS1_MAX stops gotos + spiral guides + tracking, also stops/blocks RA/Az guides in the wrong direction
// SS_LIMIT_AXIS2_MIN stops gotos + spiral guides + tracking, also stops/blocks Dec/Alt guides in the wrong direction
// SS_LIMIT_AXIS2_MAX stops gotos + spiral guides + tracking, also stops/blocks Dec/Alt guides in the wrong direction
void stopSlewingAndTracking(StopSlewActions ss) {
  if (trackingState == TrackingMoveTo) {
    if (!abortGoto) {
      abortGoto=StartAbortGoto;
      VLF("MSG: Goto aborted");
    }
  } else {
    if (spiralGuide) stopGuideSpiral();
    if (ss == SS_ALL_FAST || ss == SS_LIMIT_HARD) { stopGuideAxis1(); stopGuideAxis2(); } else
    if (ss == SS_LIMIT_AXIS1_MIN) {
      if (guideDirAxis1 == 'e' ) guideDirAxis1='b';
    } else
    if (ss == SS_LIMIT_AXIS1_MAX) {
      if (guideDirAxis1 == 'w' ) guideDirAxis1='b';
    } else
    if (ss == SS_LIMIT_AXIS2_MIN) {
      if (getInstrPierSide() == PierSideWest) { if (guideDirAxis2 == 'n' ) guideDirAxis2='b'; } else if (guideDirAxis2 == 's' ) guideDirAxis2='b';
    } else
    if (ss == SS_LIMIT_AXIS2_MAX) {
      if (getInstrPierSide() == PierSideWest) { if (guideDirAxis2 == 's' ) guideDirAxis2='b'; } else if (guideDirAxis2 == 'n' ) guideDirAxis2='b';
    }
    if (trackingState != TrackingNone) {
      if (ss != SS_ALL_FAST) {
        if (generalError != ERR_DEC) {
          stopGuideAxis1();
          stopGuideAxis2();
          trackingState=TrackingNone;
          VLF("MSG: Limit exceeded guiding/tracking stopped");
        }
      }
    }
  }
}
