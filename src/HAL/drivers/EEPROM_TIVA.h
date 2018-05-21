/*
  EEPROM.h - EEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  // update added by Luka
  // consolidation to a simple header file by Howard Dutton 1/2/18
  
  // The Energia IDE only has EEPROM.read and EEPROM.write, it does not include EEPROM.update.
  // My patch has been accepted but it will take a while until the next version is released.
  // Until then you can use the included EEPROM_LP.ino and EEPROM_LP.h.
*/

#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>
#include "Energia.h"
#include "driverlib/eeprom.h"
#include "EEPROM_LP.h"

#define BYTES_PER_WORD   4
#define WORDS_PER_BLOCK  16
#define NUM_BLOCKS    32

class EEPROMClass
{
  public:
    uint8_t read(int address) 
    {
      uint32_t byteAddr = address - (address % BYTES_PER_WORD);
      uint32_t wordVal = 0;

      EEPROMRead(&wordVal, byteAddr, 4);
      wordVal = wordVal >> (8*(address % BYTES_PER_WORD));

      return (uint8_t) wordVal;
    }

    void write(int address, uint8_t value)
    {
      uint32_t byteAddr = address - (address % BYTES_PER_WORD);

      uint32_t wordVal = 0;
      EEPROMRead(&wordVal, byteAddr, 4);
      wordVal &= ~(0xFF << (8*(address % BYTES_PER_WORD)));
      wordVal += value << (8*(address % BYTES_PER_WORD));

      EEPROMProgram(&wordVal, byteAddr, 4);
    }

    void update(int address, uint8_t value)
    {
      uint8_t old_value = read(address);
  
      if(value != old_value) write(address, value);
    }
};

EEPROMClass EEPROM;

#endif
