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
 * 
 *
 * Revision History, see GitHub
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * Arduino Stepper motor controller for Losmandy G11 mounts (and others)
 * with LX200 derived command set.
 *
 */

// Use Config.xxxxx.h to configure OnStep to your requirements

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate   "01 02 18"
#define FirmwareNumber "1.0a"
#define FirmwareName   "On-Step"
#define FirmwareTime   "12:00:00"

// some defines to help with configuration
#define TMC_LOWPWR      64
#define TMC_STEALTHCHOP 32
#define TMC_NINTPOL     16
#define SYNC_ANYWHERE_ON

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

#ifdef MODE_SWITCH_BEFORE_SLEW_SPI
#include "TMC2130.h"
//               SS      ,SCK     ,MISO     ,MOSI
tmc2130 tmcAxis1(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
tmc2130 tmcAxis2(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
#endif

#ifdef RTC_DS3234
#include <SparkFunDS3234RTC.h>  //https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library/archive/master.zip
#define DS3234_CS_PIN 10
#endif

// forces initialialization of a host of settings in EEPROM. OnStep does this automatically, most likely, you will want to leave this alone
#define INIT_KEY false    // set to true to keep automatic initilization from happening.  This is a one-time operation... upload to the Arduino, then set to false and upload again
#define initKey 915307548 // unique identifier for the current initialization format, do not change

void setup() {
  // set initial values for some variables
  Init_Startup_Values();

#ifndef Ramps14_ON
  // Translate microsteps to mode bits
  
  // The RAMPS 1.4 board has the M0/M1/M2 pins not connected
  // to MCU pins, and hence on the fly stepping cannot be used
  Axis1_Microsteps = TranslateMicrosteps(1, AXIS1_DRIVER_MODEL, AXIS1_MICROSTEPS);
  Axis2_Microsteps = TranslateMicrosteps(2, AXIS2_DRIVER_MODEL, AXIS2_MICROSTEPS);

  // We don't define directly to a function, so that we calculate the values only once,
  // and avoid execution of thiscode every time this define is referenced
  #define AXIS1_MODE Axis1_Microsteps
  #define AXIS2_MODE Axis2_Microsteps
#endif

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
  TakeupRate=round(SiderealRate/1.1);
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
    // do de-rotate movement
    if (deRotate && (trackingState==TrackingSidereal)) {
      targetAxis3.fixed+=amountRotateAxis3.fixed;
      double f=(long)targetAxis3.part.m; f/=(double)StepsPerDegreeAxis3;
      if ((f<(double)MinAxis3) || (f>(double)MaxAxis3)) { deRotate=false; amountRotateAxis3.fixed=0; }
    }
#endif
#if defined(FOCUSER1_ON)
    // do automatic movement
    targetAxis4.fixed+=amountMoveAxis4.fixed;
    { double f=(long)targetAxis4.part.m; f/=(double)StepsPerMicrometerAxis4;
    if ((f<(double)MinAxis4*1000.0) || (f>(double)MaxAxis4*1000.0)) amountMoveAxis4.fixed=0; }
#endif
#if defined(FOCUSER2_ON)
    // do automatic movement
    targetAxis5.fixed+=amountMoveAxis5.fixed;
    { double f=(long)targetAxis5.part.m; f/=(double)StepsPerMicrometerAxis5;
    if ((f<(double)MinAxis5*1000.0) || (f>(double)MaxAxis5*1000.0)) amountMoveAxis5.fixed=0; }
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
#ifdef AXIS1_FAULT_LOW
    faultAxis1=(digitalRead(Axis1_FAULT)==LOW);
#endif
#ifdef AXIS1_FAULT_HIGH
    faultAxis1=(digitalRead(Axis1_FAULT)==HIGH);
#endif
#ifdef AXIS1_FAULT_SPI
  if (lst%2==0) {
    BBSpi.begin(Axis1_M2,Axis1_M1,Axis1_Aux,Axis1_M0);
    faultAxis1=TMC2130_error();
    BBSpi.end();
  }
#endif
#ifdef AXIS2_FAULT_LOW
    faultAxis2=(digitalRead(Axis2_FAULT)==LOW);
#endif
#ifdef AXIS2_FAULT_HIGH
    faultAxis2=(digitalRead(Axis2_FAULT)==HIGH);
#endif
#ifdef AXIS2_FAULT_SPI
  if (lst%2==1) {
    BBSpi.begin(Axis2_M2,Axis2_M1,Axis2_Aux,Axis2_M0);
    faultAxis2=TMC2130_error();
    BBSpi.end();
  }
#endif
#if defined(MODE_SWITCH_BEFORE_SLEW_SPI) && defined(STALL_GUARD_ON)
  tmcAxis1.sgUpdateResult();
  tmcAxis2.sgUpdateResult();
  static int sgCountDown=200;
  if (trackingState==TrackingSidereal) {
    if (sgCountDown>0) sgCountDown--; else {
      // stallGuard, update SG_RESULTs, stop the slew if we hit the slew limit (0)
      bool sgFaultAxis1=false; if ((EEPROM_readInt(EE_sgLimitAxis1)>0) && (tmcAxis1.sgResult<EEPROM_readInt(EE_sgLimitAxis1))) sgFaultAxis1=true;
      bool sgFaultAxis2=false; if ((EEPROM_readInt(EE_sgLimitAxis2)>0) && (tmcAxis2.sgResult<EEPROM_readInt(EE_sgLimitAxis2))) sgFaultAxis2=true;
      if (sgFaultAxis1 || sgFaultAxis2) { lastError=ERR_MOTOR_FAULT; if (trackingState==TrackingMoveTo) abortSlew=true; else { trackingState=TrackingNone; if (guideDirAxis1) guideDirAxis1='b'; if (guideDirAxis2) guideDirAxis2='b'; } }
    }
  } else {
    sgCountDown=200;
  }
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

unsigned long tempMs;

  // ROTATOR/DEROTATOR ---------------------------------------------------------------------------------
#ifdef ROTATOR_ON
  tempMs=millis();
  if ((long)(tempMs-axis3Ms)>0) {
    axis3Ms=tempMs+(unsigned long)MaxRateAxis3;

    if ((posAxis3<(long)targetAxis3.part.m) && (posAxis3<((double)MaxRot*(double)StepsPerDegreeAxis3))) {
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3++;
    }
    if ((posAxis3>(long)targetAxis3.part.m) && (posAxis3>((double)MinRot*(double)StepsPerDegreeAxis3))) {
      digitalWrite(Axis3StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis3DirPin,AXIS3_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis3StepPin,HIGH); posAxis3--;
    }
  }
#endif

  // FOCUSER1 -------------------------------------------------------------------------------------------
#ifdef FOCUSER1_ON
  tempMs=millis();
  if ((long)(tempMs-axis4Ms)>0) {
    axis4Ms=tempMs+(unsigned long)MaxRateAxis4;

    if ((posAxis4<(long)targetAxis4.part.m) && (posAxis4<((double)MaxAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4++;
    }
    if ((posAxis4>(long)targetAxis4.part.m) && (posAxis4>((double)MinAxis4*1000.0*(double)StepsPerMicrometerAxis4))) {
      digitalWrite(Axis4StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis4DirPin,AXIS4_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis4StepPin,HIGH); posAxis4--;
    }
  }
#endif

  // FOCUSER2 -------------------------------------------------------------------------------------------
#ifdef FOCUSER2_ON
  tempMs=millis();
  if ((long)(tempMs-axis5Ms)>0) {
    axis5Ms=tempMs+(unsigned long)MaxRateAxis5;

    if ((posAxis5<(long)targetAxis5.part.m) && (posAxis5<((double)MaxAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_FORWARD); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5++;
    }
    if ((posAxis5>(long)targetAxis5.part.m) && (posAxis5>((double)MinAxis5*1000.0*(double)StepsPerMicrometerAxis5))) {
      digitalWrite(Axis5StepPin,LOW); delayMicroseconds(10);
      digitalWrite(Axis5DirPin,AXIS5_REVERSE); delayMicroseconds(10);
      digitalWrite(Axis5StepPin,HIGH); posAxis5--;
    }
  }
#endif

  // WORKLOAD MONITORING -------------------------------------------------------------------------------
  long this_loop_micros=micros(); 
  loop_time=this_loop_micros-last_loop_micros;
  if (loop_time>worst_loop_time) worst_loop_time=loop_time;
  last_loop_micros=this_loop_micros;

  // 1 SECOND TIMED ------------------------------------------------------------------------------------
  tempMs=millis();
  if ((long)(tempMs-housekeepingTimer)>0) {
    housekeepingTimer=tempMs+1000UL;

#if defined(ROTATOR_ON) && defined(MOUNT_TYPE_ALTAZM)
    // calculate new de-rotation rate if needed
    if (deRotate && (trackingState==TrackingSidereal)) {
      double h,d;
      getApproxEqu(&h,&d,true);
      double pr=ParallacticRate(h,d)*(double)StepsPerDegreeAxis3; // in steps per second
      if (deRotateReverse) pr=-pr;
      amountRotateAxis3.fixed=doubleToFixed(pr/100.0);            // in steps per 1/100 second
    }
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
    if (trackingState==TrackingNone) if (LED2_ON) { digitalWrite(LEDneg2Pin,HIGH); LED2_ON=false; }
    if (trackingState==TrackingMoveTo) if (!LED2_ON) { digitalWrite(LEDneg2Pin,LOW); LED2_ON=true; }
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

