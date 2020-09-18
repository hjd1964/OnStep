/*
 * Hardware Abstraction Layer (HAL) for OnStep
 * 
 * Copyright (C) 2018 Khalid Baheyeldin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HAL_H
#define _HAL_H

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

#if defined(__AVR_ATmega1280__)
  #define MCU_STR "Mega1280"
  #include "HAL_Mega2560/HAL_Mega2560.h"

#elif defined(__AVR_ATmega2560__)
  #define MCU_STR "Mega2560"
  #include "HAL_Mega2560/HAL_Mega2560.h"

#elif defined(_mk20dx128_h_) || defined(__MK20DX128__)
  // Teensy 3.0
  #define MCU_STR "Teensy3.0"
  #include "HAL_Teensy_3/HAL_Teensy_3.h"

#elif defined(__MK20DX256__)
  // Teensy 3.2
  #define MCU_STR "Teensy3.2"
  #include "HAL_Teensy_3/HAL_Teensy_3.h"

#elif defined(__MK64FX512__)
  // Teensy 3.5
  #define MCU_STR "Teensy3.5"
  #include "HAL_Teensy_3/HAL_Teensy_3.h"

#elif defined(__MK66FX1M0__)
  // Teensy 3.6
  #define MCU_STR "Teensy3.6"
  #include "HAL_Teensy_3/HAL_Teensy_3.h"

#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  // Teensy 4.0
  #define MCU_STR "Teensy4.0"
  #include "HAL_Teensy_4/HAL_Teensy_4.h"

#elif defined(__STM32F1__)
  // STM32F103C8/CB: 72MHz, 128K flash, 64K RAM
  #define MCU_STR "STM32F1"
  #include "HAL_STM32F1/HAL_STM32F1.h"
  
#elif defined(STM32F103xB)
  // STM32F103C8/CB: 72MHz, 128K flash, 64K RAM (using STM32duino)
  #define MCU_STR "STM32F103"
  #include "HAL_STM32F1/HAL_STM32duinoF1.h"

#elif defined(STM32F411xE)
  // Blackpill board with STM32F411CE
  #define MCU_STR "STM32F411"
  #include "HAL_STM32F4/HAL_STM32F4.h"

#elif defined(STM32F446xx)
  // FYSETC S6 board with STM32F446
  #define MCU_STR "STM32F446"
  #include "HAL_STM32F4/HAL_STM32F4.h"

#elif defined(ESP32)
  // ESP32
  #define MCU_STR "ESP32"
  #include "HAL_ESP32/HAL_ESP32.h"

#elif defined(__SAM3X8E__)
  // Arduino Due
  #define MCU_STR "SAM3X8E (Arduino DUE)"
  #include "HAL_Due/HAL_Due.h"  
  
#else
  #error "Unsupported Platform! If this is a new platform, it needs the appropriate entries in the HAL directory."
#endif

#endif // _HAL_H
