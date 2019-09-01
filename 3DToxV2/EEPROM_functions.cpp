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


 // ----------------------File content description: -------------------
 // This class is used to store Data into EEPROM
 // Settings are read from Config class
 //

#include "EEPROM_functions.h"

// Resetting EEPROM data with 0x00
// Adding INIT code at location 0 to 2 . this identifies the version of the EEPROM structure

void CEEPROM::ResetEEPROM()
{
#ifdef DEBUG
   Serial.print ("Resetting EEPROM... \r\n");
#endif
  // writing EEPROM structure version to the 3 first bytes
  SafeWriteEEPROMData(0, EEPROM_INIT_0);
  SafeWriteEEPROMData(1, EEPROM_INIT_1);
  SafeWriteEEPROMData(2, EEPROM_INIT_2);
  // clearing other bytes
  for (int i = 3 ; i < EEPROM.length(); i++)
  {
    SafeWriteEEPROMData(i, 0x00);
  }

#ifdef DEBUG
   Serial.print ("Resetting EEPROM Done! \r\n");
#endif
}

// function used to Write simple data to a specific EEPROM address
// in order to increase lifespan of your EEPROOM, a read is made first, and a comparison is made
// to check that we don't write twice the same data at this specific location.
// writing takes longer as you read first but makes sure that you write into EEPROOM only if needed.
void CEEPROM::SafeWriteEEPROMData(int Addr, byte data)
{
  byte EEPROMData = EEPROM.read(Addr);
  if(EEPROMData == data)
  {
    return;
  }
  EEPROM.write(Addr, data);
}
