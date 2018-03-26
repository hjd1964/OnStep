/*
 * Title       OnStep
 * by          Howard Dutton
 *
 * Copyright (C) 2012 to 2018 Howard Dutton
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
 * Revision history, and newer versions::
 *   See GitHub: https://github.com/hjd1964/OnStep
 *
 * Documentation:
 *   https://groups.io/g/onstep/wiki/home
 *
 * Discussion, Questions, ...etc
 *   https://groups.io/g/onstep
 */

// Use Config.xxx.h to configure OnStep to your requirements

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate          __DATE__
#define FirmwareVersionMajor  1
#define FirmwareVersionMinor  4
#define FirmwareVersionPatch  "b"     // for example major.minor patch: 1.3c
#define FirmwareVersionConfig 2       // internal, for tracking configuration file changes
#define FirmwareName          "On-Step"
#define FirmwareTime          __TIME__

#include "Constants.h"

#include "src/HAL/HAL.h"

#include "Config.Classic.h"
#include "Config.MiniPCB.h"
#include "Config.MaxPCB.h"
#include "Config.Ramps14.h"
#include "Config.Mega2560Alt.h"
#include "Config.TM4C.h"
#include "Config.STM32.h"

#include "Validate.h"

#include "errno.h"
#include "math.h"
#include "FPoint.h"
#include "SoftSPI.h"
#include "Library.h"
#include "Align.h"
#include "Command.h"
#include "Globals.h"
#include "Julian.h"

#ifdef MODE_SWITCH_BEFORE_SLEW_SPI
#include "TMC2130.h"
//               SS      ,SCK     ,MISO     ,MOSI
tmc2130 tmcAxis1(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
tmc2130 tmcAxis2(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
#endif

// use an RTC (Real Time Clock) if present
#include "RTCw.h"
rtcw urtc;

// forces initialialization of a host of settings in EEPROM. OnStep does this automatically, most likely, you will want to leave this alone
#define INIT_KEY false    // set to true to keep automatic initilization from happening.  This is a one-time operation... upload to the Arduino, then set to false and upload again
#define initKey 915307548 // unique identifier for the current initialization format, do not change

void setup() {
  // set initial values for some variables
  Init_Startup_Values();

  // set pins for input/output as specified in Config.h and PinMap.h
  Init_Pins();

  // if this is the first startup set EEPROM to defaults
  Init_EEPROM_Values();
  
  // this sets up the sidereal timer and tracking rates
  siderealInterval=EEPROM_readLong(EE_siderealInterval); // the number of 16MHz clocks in one sidereal second (this is scaled to actual processor speed)
  SiderealRate=siderealInterval/StepsPerSecondAxis1;
  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;

  // backlash takeup rates
  TakeupRate=SiderealRate/BacklashTakeupRate;
  timerRateBacklashAxis1=SiderealRate/BacklashTakeupRate;
  timerRateBacklashAxis2=(SiderealRate/BacklashTakeupRate)*timerRateRatio;

  // now read any saved values from EEPROM into varaibles to restore our last state
  Init_ReadEEPROM_Values();

  SetTrackingRate(default_tracking_rate);
  SetDeltaTrackingRate();

  // starts the hardware timers that keep sidereal time, move the motors, etc.
  Init_Start_Timers();

  // get ready for serial communications
  PSerial.begin(9600);
#ifdef HAL_SERIAL1_ENABLED
  PSerial1.begin(SERIAL1_BAUD_DEFAULT);
#endif
#ifdef HAL_SERIAL4_ENABLED
  PSerial4.begin(SERIAL4_BAUD_DEFAULT);
#endif
 
  // autostart tracking
#if defined(AUTOSTART_TRACKING_ON) && (defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORKALT))
  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
  setHome();

  // start tracking
  trackingState=TrackingSidereal;
  EnableStepperDrivers();
#endif

  // prep counters (for keeping time in main loop)
  cli(); siderealTimer=lst; guideSiderealTimer=lst; PecSiderealTimer=lst; sei();
  housekeepingTimer=millis()+1000UL; 
  last_loop_micros=micros();
#ifdef ROTATOR_ON
  axis3Ms=millis()+(unsigned long)MaxRateAxis3;
#endif
#ifdef FOCUSER1_ON
  axis4Ms=millis()+(unsigned long)MaxRateAxis4;
#endif
#ifdef FOCUSER2_ON
  axis5Ms=millis()+(unsigned long)MaxRateAxis5;
#endif
}

void loop() {

  // GUIDING -------------------------------------------------------------------------------------------
  ST4();
  if ((trackingState!=TrackingMoveTo) && (parkStatus==NotParked)) Guide();

#ifndef MOUNT_TYPE_ALTAZM
  // PERIODIC ERROR CORRECTION -------------------------------------------------------------------------
  if ((trackingState==TrackingSidereal) && (parkStatus==NotParked) && (!((guideDirAxis1 || guideDirAxis2) && (activeGuideRate>GuideRate1x)))) { 
    // only active while sidereal tracking with a guide rate that makes sense
    Pec();
  } else DisablePec();
#endif

  // 1/100 SECOND TIMED --------------------------------------------------------------------------------
  cli(); long tempLst=lst; sei();
  if (tempLst!=siderealTimer) {
    siderealTimer=tempLst;

#ifndef MOUNT_TYPE_ALTAZM
    // WRITE PERIODIC ERROR CORRECTION TO EEPROM
    if (pecAutoRecord>0) {
      // write PEC table to EEPROM, should do several hundred bytes/second
      pecAutoRecord--;
      EEPROM.update(EE_pecTable+pecAutoRecord,pecBuffer[pecAutoRecord]);
    }
#endif

    // FLASH LED DURING SIDEREAL TRACKING
    if (trackingState==TrackingSidereal) {
#ifdef STATUS_LED_PINS_ON
      if (siderealTimer%20L==0L) { if (LED_ON) { digitalWrite(LEDnegPin,HIGH); LED_ON=false; } else { digitalWrite(LEDnegPin,LOW); LED_ON=true; } }
#endif
    }

    // SIDEREAL TRACKING DURING GOTOS
    // keeps the target where it's supposed to be while doing gotos
    if (trackingState==TrackingMoveTo) {
      moveTo();
      if (lastTrackingState==TrackingSidereal) {
        // origTargetAxisn isn't used in Alt/Azm mode since meridian flips never happen
        origTargetAxis1.fixed+=fstepAxis1.fixed;
        // don't advance the target during meridian flips
        if ((pierSide==PierSideEast) || (pierSide==PierSideWest)) {
          cli();
          targetAxis1.fixed+=fstepAxis1.fixed;
          targetAxis2.fixed+=fstepAxis2.fixed;
          sei();
        }
      }
    }

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    RotatorMove();
#endif
#if defined(FOCUSER1_ON)
    Focuser1Move();
#endif
#if defined(FOCUSER2_ON)
    Focuser2Move();
#endif

    // figure out the current Altitude
    if (lst%3==0) do_fastalt_calc();

#ifdef MOUNT_TYPE_ALTAZM
    // figure out the current Alt/Azm tracking rates
    if (lst%3!=0) do_altAzmRate_calc();
#else
    // figure out the current refraction compensated tracking rate
    if (refraction && (lst%3!=0)) do_refractionRate_calc();
#endif

    // SAFETY CHECKS
    // support for limit switch(es)
#ifdef LIMIT_SENSE_ON
    byte ls1=digitalRead(LimitPin); delayMicroseconds(50); byte ls2=digitalRead(LimitPin);
    if ((ls1==LOW) && (ls2==LOW)) { lastError=ERR_LIMIT_SENSE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif
    // check for fault signal, stop any slew or guide and turn tracking off
#ifdef AXIS1_FAULT
  #if AXIS1_FAULT==LOW
    faultAxis1=(digitalRead(Axis1_FAULT)==LOW);
  #endif
  #if AXIS1_FAULT==HIGH
    faultAxis1=(digitalRead(Axis1_FAULT)==HIGH);
  #endif
  #if AXIS1_FAULT==TMC2130
    if (lst%2==0) tmcAxis1.error();
  #endif
#endif
#ifdef AXIS2_FAULT
  #if AXIS2_FAULT==LOW
    faultAxis2=(digitalRead(Axis2_FAULT)==LOW);
  #endif
  #if AXIS2_FAULT==HIGH
    faultAxis2=(digitalRead(Axis2_FAULT)==HIGH);
  #endif
  #if AXIS2_FAULT==TMC2130
    if (lst%2==1) tmcAxis2.error();
  #endif
#endif

    if (faultAxis1 || faultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }
    // check altitude overhead limit and horizon limit
    if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { lastError=ERR_ALT; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }

    // UPDATE THE UT1 CLOCK
    cli(); long cs=lst; sei();
    double t2=(double)((cs-lst_start)/100.0)/1.00273790935;
    // This just needs to be accurate to the nearest second, it's about 10x better
    UT1=UT1_start+(t2/3600.0);
  }

  // ROTATOR/DEROTATOR/FOCUSERS ------------------------------------------------------------------------
#ifdef ROTATOR_ON
  RotatorFollow();
#endif

#ifdef FOCUSER1_ON
  Focuser1Follow();
#endif

#ifdef FOCUSER2_ON
  Focuser2Follow();
#endif
  
  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  unsigned long tempMs=millis();
  if ((long)(tempMs-housekeepingTimer)>0) {
    housekeepingTimer=tempMs+1000UL;

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    DeRotate();
#endif

    // adjust tracking rate for Alt/Azm mounts
    // adjust tracking rate for refraction
    SetDeltaTrackingRate();

    // basic check to see if we're not at home
    if (trackingState!=TrackingNone) atHome=false;

#ifdef PEC_SENSE
    // see if we're on the PEC index
    if (trackingState==TrackingSidereal) 
    pecAnalogValue = analogRead(AnalogPecPin);
#endif
    
#if defined(PPS_SENSE_ON) || defined(PPS_SENSE_PULLUP)
    // update clock via PPS
    if (trackingState==TrackingSidereal) {
      cli();
      PPSrateRatio=((double)1000000.0/(double)(PPSavgMicroS));
      if ((long)(micros()-(PPSlastMicroS+2000000UL))>0) PPSsynced=false; // if more than two seconds has ellapsed without a pulse we've lost sync
      sei();
  #ifdef STATUS_LED2_PINS_ON
      if (PPSsynced) { if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; } else { digitalWrite(LEDneg2Pin,LOW); LED2_ON=true; } } else { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; } // indicate PPS
  #endif
      if (LastPPSrateRatio!=PPSrateRatio) { SiderealClockSetInterval(siderealInterval); LastPPSrateRatio=PPSrateRatio; }
    }
#endif

#ifdef STATUS_LED_PINS_ON
    // LED indicate PWR on 
    if (trackingState!=TrackingSidereal) if (!LED_ON) { digitalWrite(LEDnegPin,LOW); LED_ON=true; }
#endif
#ifdef STATUS_LED2_PINS_ON
    // LED indicate STOP and GOTO
    if (trackingState==TrackingMoveTo) if (!LED2_ON) { digitalWrite(LEDneg2Pin,LOW); LED2_ON=true; }
#if defined(PPS_SENSE_ON) || defined(PPS_SENSE_PULLUP)
    if (trackingState==TrackingNone) if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; }
#else
    if (trackingState!=TrackingMoveTo) if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; }
#endif
#endif

    // SAFETY CHECKS, keeps mount from tracking past the meridian limit, past the UnderPoleLimit, or past the Dec limits
    if (meridianFlip!=MeridianFlipNever) {
      if (pierSide==PierSideWest) {
        cli(); long p1=posAxis1+indexAxis1Steps; sei();
        if (p1>(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L)) {
          // do an automatic meridian flip and continue if just tracking
          // checks: enabled && not too far past the meridian (doesn't make sense) && not in inaccessible area between east and west limits && finally that a slew isn't happening
          if (autoMeridianFlip && (p1<(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L+(1.0/60.0)*(long)StepsPerDegreeAxis1)) && (p1>(-minutesPastMeridianE*(long)StepsPerDegreeAxis1/4L)) && (trackingState!=TrackingMoveTo)) {
            double newRA,newDec;
            getEqu(&newRA,&newDec,false);
            if (goToEqu(newRA,newDec)) { // returns 0 on success
              lastError=ERR_MERIDIAN;
              trackingState=TrackingNone;
            }
          } else {
            lastError=ERR_MERIDIAN; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone;
          }
        }
      }
      if (pierSide==PierSideEast) { cli(); if (posAxis1+indexAxis1Steps>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei(); }
    } else {
#ifndef MOUNT_TYPE_ALTAZM
      // when Fork mounted, ignore pierSide and just stop the mount if it passes the UnderPoleLimit
      cli(); if (posAxis1+indexAxis1Steps>(UnderPoleLimit*15L*(long)StepsPerDegreeAxis1)) { lastError=ERR_UNDER_POLE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#else
      // when Alt/Azm mounted, just stop the mount if it passes MaxAzm
      cli(); if (posAxis1+indexAxis1Steps>((long)MaxAzm*(long)StepsPerDegreeAxis1)) { lastError=ERR_AZM; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; } sei();
#endif
    }
    // check for exceeding MinDec or MaxDec
#ifndef MOUNT_TYPE_ALTAZM
    if ((getApproxDec()<MinDec) || (getApproxDec()>MaxDec)) { lastError=ERR_DEC; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif

  } else {
    // COMMAND PROCESSING --------------------------------------------------------------------------------
    processCommands();
  }
}

