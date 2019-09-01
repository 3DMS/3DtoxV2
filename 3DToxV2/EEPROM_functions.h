/* 3DTox V2
 * Copyright (C) 2019 by Nicolas Rambaud
 *
 * This file is part of the 3DTox V2 firmware
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License V3.0 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this piece of code.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _EEPROMFUNCTIONS
#define _EEPROMFUNCTIONS

#include <Arduino.h>
#include <EEPROM.h>

// EEPROM data starts at address 4
//EEPROM VERSION
const byte EEPROM_INIT_0 = 0x00;
const byte EEPROM_INIT_1 = 0x00;
const byte EEPROM_INIT_2 = 0x04;

// the 3 first bytes are used to store EEPROM structure version
const byte  EEPROM_BAUDRATE    = 3;   // address of Baudrate setting (int)
const byte  EEPROM_MODE        = 4;   // address of currently selected mode (int)
// bytes add 5-> 9 are kept emtpy for possible future features

//-----------------------------------------------------------------------------
const byte  EEPROM_MINUTES_ADDR = 8;
const byte  EEPROM_HOURS_ADDR = 9;
// The other 2 bytes are used to store running duration days
const byte  EEPROM_DAYS_H_ADDR = 10;  // address to store Days (highByte)
const byte  EEPROM_DAYS_L_ADDR = 11;  // address tp store Days (lowByte)

// The remaining bytes starting at add 12 are used to store the running duration
// Arduino MEGA 2560 has 4KB This leaves 4096-12 = 4084 bytes of memory to spread
// the device duration over all the bytes
// more details are provided about the algorythm inside file RunningDuration.cpp
const byte  EEPPROM_START_ADDR = 12;  // Address to start spreading the writing of duration

class CEEPROM
{
  public:
  static void ResetEEPROM();
  static void SafeWriteEEPROMData(int Addr, byte data);
};

#endif
