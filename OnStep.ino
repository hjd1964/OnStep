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

#define FirmwareVersionMinor  8
#define FirmwareVersionPatch  "l"     // for example major.minor patch: 1.3c
#define FirmwareVersionConfig 2       // internal, for tracking configuration file changes

#define FirmwareName          "On-Step"
#define FirmwareTime          __TIME__

#include <errno.h>
#include <math.h>

#include "Constants.h"

#include "Config.Classic.h"
#include "Config.MiniPCB.h"
#include "Config.MaxPCB.h"
#include "Config.Ramps14.h"
#include "Config.Mega2560Alt.h"
#include "Config.TM4C.h"
#include "Config.STM32.h"
#include "Validate.h"

#include "src/HAL/HAL.h"
#include "src/lib/St4SerialMaster.h"

// Enable debugging messages on DebugSer -------------------------------------------------------------
#define DEBUG_OFF              // default=_OFF, use "DEBUG_ON" to activate
#define DebugSer SerialA       // default=Serial, or Serial1 for example (always 9600 baud)

// Helper macros for debugging, with less typing
#if defined(DEBUG_ON)
  #define D(x)       DebugSer.print(x)
  #define DH(x)      DebugSer.print(x,HEX)
  #define DL(x)      DebugSer.println(x)
  #define DHL(x,y)   DebugSer.println(x,HEX)
#else
  #define D(x)
  #define DH(x,y)
  #define DL(x)
  #define DHL(x,y)
#endif
// ---------------------------------------------------------------------------------------------------

#include "src/lib/SoftSPI.h"
#include "src/lib/FPoint.h"
#include "src/lib/Julian.h"
#ifdef MODE_SWITCH_BEFORE_SLEW_SPI
#include "src/lib/TMC2130.h"
//               SS      ,SCK     ,MISO     ,MOSI
tmc2130 tmcAxis1(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
tmc2130 tmcAxis2(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
#endif

#include "Globals.h"

#include "src/lib/Library.h"
#include "src/lib/Command.h"
#include "Align.h"

#ifdef ROTATOR_ON
  #include "src/lib/Rotator.h"
  rotator rot;
#endif

#if defined(FOCUSER1_ON) || defined(FOCUSER2_ON)
  #include "src/lib/Focuser.h"
  #ifdef FOCUSER1_ON
    focuser foc1;
  #endif
  #ifdef FOCUSER2_ON
    focuser foc2;
  #endif
#endif

// use weather sensors (temperature, pressure, humidity) if present
#include "src/lib/Weather.h"
weather ambient;

// Forces initialialization of a host of settings in nv. OnStep does this automatically,
// most likely, you will want to leave this alone
// set to true to keep automatic initilization from happening.
// This is a one-time operation: change to true, upload to the MCU, then set to false
// and upload again
#define INIT_KEY false

void setup() {

#ifdef DEBUG_ON
  // Initialize USB serial debugging early, so we can use DebugSer.print() for debugging, if needed
  DebugSer.begin(9600);
  delay(5000);
#endif
  
  // initialize the Non-Volatile Memory
  nv.init();

  // initialize the Object Library
  Lib.init();
  
  // get weather monitoring ready to go
  ambient.init();
  
  // set initial values for some variables
  Init_Startup_Values();

  // set pins for input/output as specified in Config.h and PinMap.h
  Init_Pins();
  
  Init_Guide();

  // if this is the first startup set EEPROM to defaults
  Init_WriteNV_Values();
  
  // this sets up the sidereal timer and tracking rates
  siderealInterval=nv.readLong(EE_siderealInterval); // the number of 16MHz clocks in one sidereal second (this is scaled to actual processor speed)
  SiderealRate=siderealInterval/StepsPerSecondAxis1;
  timerRateAxis1=SiderealRate;
  timerRateAxis2=SiderealRate;

  // backlash takeup rates
  TakeupRate=SiderealRate/BacklashTakeupRate;
  timerRateBacklashAxis1=SiderealRate/BacklashTakeupRate;
  timerRateBacklashAxis2=(SiderealRate/BacklashTakeupRate)*timerRateRatio;

  // now read any saved values from EEPROM into varaibles to restore our last state
  Init_ReadNV_Values();

  SetTrackingRate(default_tracking_rate);
  SetDeltaTrackingRate();

  // starts the hardware timers that keep sidereal time, move the motors, etc.
  Init_Start_Timers();

  SerialA.begin(9600);
#ifdef HAL_SERIAL_B_ENABLED
  SerialB.begin(SERIAL_B_BAUD_DEFAULT);
#endif
#ifdef HAL_SERIAL_C_ENABLED
  SerialC.begin(SERIAL_C_BAUD_DEFAULT);
#endif
#ifdef ST4_HAND_CONTROL_ON
  SerialST4.begin();
#endif
 
  // autostart tracking
#if defined(AUTOSTART_TRACKING_ON) && (defined(MOUNT_TYPE_GEM) || defined(MOUNT_TYPE_FORK) || defined(MOUNT_TYPE_FORKALT))
  // telescope should be set in the polar home (CWD) for a starting point
  // this command sets indexAxis1, indexAxis2, azmCor=0; altCor=0;
  setHome();

  // orientation is unknown
  safetyLimitsOn=false;

  // start tracking
  trackingState=TrackingSidereal;
  EnableStepperDrivers();
#endif

  // start rotator if present
#ifdef ROTATOR_ON
  rot.init(Axis3StepPin,Axis3DirPin,Axis3_EN,MaxRateAxis3,StepsPerDegreeAxis3);
  rot.setMin(MinAxis3);
  rot.setMax(MaxAxis3);
  #ifdef AXIS3_REVERSE_ON
    rot.setReverseState(HIGH);
  #endif
  #ifdef AXIS3_DISABLE
    rot.setDisableState(AXIS3_DISABLE);
    rot.powerDownActive(true);
  #endif
#endif

  // start focusers if present
#ifdef FOCUSER1_ON
  foc1.init(Axis4StepPin,Axis4DirPin,Axis4_EN,EE_posAxis4,MaxRateAxis4,StepsPerMicrometerAxis4);
  foc1.setMin(MinAxis4*1000.0);
  foc1.setMax(MaxAxis4*1000.0);
  #ifdef AXIS4_REVERSE_ON
    foc1.setReverseState(HIGH);
  #endif
  #ifdef AXIS4_DISABLE
    foc1.setDisableState(AXIS4_DISABLE);
    foc1.powerDownActive(true);
  #endif
#endif
#ifdef FOCUSER2_ON
  foc2.init(Axis5StepPin,Axis5DirPin,Axis5_EN,EE_posAxis5,MaxRateAxis5,StepsPerMicrometerAxis5);
  foc2.setMin(MinAxis5*1000.0);
  foc2.setMax(MaxAxis5*1000.0);
  #ifdef AXIS5_REVERSE_ON
    foc2.setReverseState(HIGH);
  #endif
  #ifdef AXIS5_DISABLE
    foc2.setDisableState(AXIS4_DISABLE);
    foc2.powerDownActive(true);
  #endif
#endif

  // prep counters (for keeping time in main loop)
  cli(); siderealTimer=lst; guideSiderealTimer=lst; PecSiderealTimer=lst; sei();
  last_loop_micros=micros();
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

#ifdef HOME_SENSE_ON
  // AUTOMATIC HOMING ----------------------------------------------------------------------------------
  checkHome();
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
      nv.update(EE_pecTable+pecAutoRecord,pecBuffer[pecAutoRecord]);
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

    // ROTATOR/FOCUSERS, MOVE THE TARGET
#if defined(ROTATOR_ON)
    rot.move(trackingState==TrackingSidereal);
#endif
#if defined(FOCUSER1_ON)
    foc1.move();
#endif
#if defined(FOCUSER2_ON)
    foc2.move();
#endif

    // CALCULATE SOME TRACKING RATES, ETC.
    if (lst%3==0) do_fastalt_calc();
#ifdef MOUNT_TYPE_ALTAZM
    // figure out the current Alt/Azm tracking rates
    if (lst%3!=0) do_altAzmRate_calc();
#else
    // figure out the current refraction compensated tracking rate
    if ((rateCompensation!=RC_NONE) && (lst%3!=0)) do_refractionRate_calc();
#endif

    // SAFETY CHECKS
    // support for limit switch(es)
#ifdef LIMIT_SENSE_ON
    byte ls1=digitalRead(LimitPin); delayMicroseconds(50); byte ls2=digitalRead(LimitPin);
    if ((ls1==LOW) && (ls2==LOW)) { lastError=ERR_LIMIT_SENSE; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif

    if (safetyLimitsOn) {
      // check for fault signal, stop any slew or guide and turn tracking off
#ifdef AXIS1_FAULT
  #if AXIS1_FAULT==LOW
      faultAxis1=(digitalRead(Axis1_FAULT)==LOW);
  #endif
  #if AXIS1_FAULT==HIGH
      faultAxis1=(digitalRead(Axis1_FAULT)==HIGH);
  #endif
  #if AXIS1_FAULT==TMC2130
      if (lst%2==0) faultAxis1=tmcAxis1.error();
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
      if (lst%2==1) faultAxis2=tmcAxis2.error();
  #endif
#endif

    if (faultAxis1 || faultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }

    // check altitude overhead limit and horizon limit
    if ((currentAlt<minAlt) || (currentAlt>maxAlt)) { lastError=ERR_ALT; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
    }

    // UPDATE THE UT1 CLOCK
    cli(); long cs=lst; sei();
    double t2=(double)((cs-lst_start)/100.0)/1.00273790935;
    // This just needs to be accurate to the nearest second, it's about 10x better
    UT1=UT1_start+(t2/3600.0);
  }

  // ROTATOR/DEROTATOR/FOCUSERS ------------------------------------------------------------------------
#ifdef ROTATOR_ON
  rot.follow();
#endif
#ifdef FOCUSER1_ON
  foc1.follow(isSlewing());
#endif
#ifdef FOCUSER2_ON
  foc2.follow(isSlewing());
#endif
  
  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  unsigned long tempMs=millis();

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  static unsigned long housekeepingTimer=0;
  if ((long)(tempMs-housekeepingTimer)>0) {
    housekeepingTimer=tempMs+1000UL;

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    // calculate and set the derotation rate as required
    double h,d; getApproxEqu(&h,&d,true);
    if (trackingState==TrackingSidereal) rot.derotate(h,d);
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
    if (safetyLimitsOn) {
      if (meridianFlip!=MeridianFlipNever) {
        if (pierSide==PierSideWest) {
          cli(); long p1=posAxis1+indexAxis1Steps; sei();
          if (p1>(minutesPastMeridianW*(long)StepsPerDegreeAxis1/4L)) {
            if (autoMeridianFlip) {
              if (goToHere(true)) { lastError=ERR_MERIDIAN; trackingState=TrackingNone; }
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
    }
    // check for exceeding MinDec or MaxDec
#ifndef MOUNT_TYPE_ALTAZM
    if ((currentDec<MinDec) || (currentDec>MaxDec)) { lastError=ERR_DEC; if (trackingState==TrackingMoveTo) abortSlew=true; else trackingState=TrackingNone; }
#endif

    // update weather info
    if (!isSlewing()) ambient.poll();

  } else {
    // COMMAND PROCESSING --------------------------------------------------------------------------------
    processCommands();
  }
}

