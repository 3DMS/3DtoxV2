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

#ifndef _RUNNINGDURATION
#define _RUNNINGDURATION

#include "EEPROM_functions.h"
#include "config.h"

// This class is responsible for tracking running duration
// it's also responsible for formating data for storing / reading when
// the device is powered on or when we detect we want to save the duration
const unsigned int RUNNING_DURATION_AUTOSAVE_SECONDS = 1L * 60; //1 minutes

class CRunningDuration
{
  public:
  CRunningDuration(CConfig* config);
  void LoadRunningDuration();
  // function responsible for determining weather it's too soon to save the data to EEPROM
  // function used in order to increase EEPROM lifespan
  void CheckAndSaveRunningDuration();
  // function responsible for displaying running duration over the display
  void PrintRunningDuration();
  // incrementing running duration in seconds.
  void IncrementTime(int seconds);
  // resetting running duration to 0 seconds and 0 days.
  void ResetRunningDuration();
  void ResetCounter();
  protected:
  CConfig* _config;

  private:
  // dedicated function that checks if the EEPROM spread memory has looped
  // in that case dumping data to non spread memory to keep track of data.
  bool CheckAndUpdateSpreadMemoryLoop();
  // functions responsible for Spreading running duration over the whole available EEPROM memory
  // this function is dedicated into formating and saving the data to EEPROM
  int SpreadSaveIncrementMinutes(int startAddr);
  // this function is responsible for Reading back the spread data from memory
  int SpreadReadMinutes(int startAddr);
  // saving running days to the memory
  void SaveIncrementDays();
  // checks weather the day we want to save is new or if it's the same as the one saved on EEPROM
  void TestNewDay();

  // variable used as memory ofset to track which byte was the last one used to write new data
  int _currentSpreadWriteAddr = 0;
  bool DisableEEPROMWrite = false;

};
#endif
