/*
 * Title       Smart Hand Controller (based on TeenAstro)
 *
 * Copyright (C) 2018 Charles Lemaire, Howard Dutton
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
 * Author: Charles Lemaire, https://pixelstelescopes.wordpress.com/teenastro/
 * Author: Howard Dutton, http://www.stellarjourney.com, hjd1964@gmail.com
 *
 *
 * Description
 *
 * Smart Hand controller addon for OnStep
 * for the actual hardware see: https://easyeda.com/hdutton/HC-20e242d665db4c85bb565a0cd0b52233
 *
 */

#define Product "SHC"
#define Version "0.1a 05 10 18"

#include <U8x8lib.h>   // see below
#include <U8g2lib.h>   // use Arduino->Sketch->Include Library->Manage Libraries then search for "u8g2" and install

#include <OneButton.h> // use Arduino->Sketch->Include Library->Manage Libraries then search for "onebutton" and install

#include <EEPROM.h>
#include "Config.h"
#include "Selection_catalog.h"
#include "Globals.h"
#include "TelState.h"

void setup(void){
  setupTeenAstro();
  //setupWifi();
}

void LoopMain() {
  // timing critical stuff goes here and runs as often as possible, even while TeenAstro is blocking
  //updateWifi();
}

void loop() {
  LoopTeenAstro();
}


