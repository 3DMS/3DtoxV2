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
// This file contains the default settings for this firmware.
// The values set in this file will be applied at first boot
// untill a manual save is performed.
// These values will be overriden at startup if saved settings are
// detected inside EEPROM


#ifndef _CONFIG
#define _CONFIG
#include "Pm25.h"
#include "Constants.h"


// Uncomment this line if you are performing your first run
// In this mode the Date and Time counter will be reset in EEPROM
// Default Values will also be saved in EEPROM

//#define FIRSTRUN
//#define DEBUG


enum eStatus
{
  sOK,
  sWARNING,
  sERROR,
  sIDLE
};

// Main configuration class
// This class will be passed down the different other classes
// It contains the current configuration loaded from EEPROM at startup
// It will be updated along the way if the user is changing any setting from the LCD
// It is also used when saving the current settings values into EEPROM
class CConfig
{

public:
  long TickCounter1 = 0;

  // At startup the first PM values are ignored and not taken into account.
  const int IgnoreFirstValuesInitValue = 10;
  int IgnoreFirstValues = IgnoreFirstValuesInitValue; //ignore the first values from fan speed to prevent alarm at startup

  // Initial RPM at startup
  // this value is updated every seconds or so from the Main class once a new computed value is available
  int Rpm1 = 0;
  // Pointer to Cpm25 class used to read information from the Air Quality device
  CPm25* _pm25;
  // Boolean used to check if the SD card has already been initialized
  bool SDCARD_INITIALIZED = false;
  eStatus Status          = sIDLE;

  // Current speed target for the fan
  int CurrentPwmDutyCyclePercent = 100;

  // variables used to track knob button states
  boolean buttonPressed = 0;
  byte oldButtonState   = HIGH;
  int waitCyclesBeforeUpdatingSpeed = 0;

  // Currently selected Air quality mode
  // Possible values are AUTO RS_232 AUTO_QUIET MANUAL
  AirQualityMODE CurrentAQMode      = AUTO;

  // Running duration
  // used to display overall filter usage time
  // this timer is paused if the fan speed drops to 0

  // data extracted from EEPROM spread
 int EEPROMMinutes  = 0;
 int EEPROMHours    = 0;
 int EEPROMDays     = 0;

 // data including EEPROM spread +
 int  Seconds  = 0;
 int  Minutes  = 0;
 int  Hours    = 0;
 int  Days     = 0;

 const int MinutesInDay = 1440;// 1440 = 60*24 = amount of minutes per day
 const int MinutesInHour = 60;
 // default values here for 4KB EEPROM
 int maxMemoryDays = 2; // 2 = 2880 minutes
 int maxMemoryHours = 18;// 18 = 1080 minutes
 int maxMemoryMinutes = 40; // it should remain 40 minutes

 // Varialbe used to prevent saving the running duration too often and increase EEPROM lifespan
 int RunningDurationLastSaveSeconds = 0;

 //Current Baudrate
 long  RxTxBaudrate       = 115200;
 // Variable used to check weather we need to re-initialize and reconfigure COM connection
 bool  hasBaudrateChanged = false;

 // Variables used to track Hotend and Heat bed temperatures when received from COM port
 float HotEndTemp = -1.0;
 float BedEndTemp = -1.0;

 // Variable used to handle the cases when data hasn't been received from COM port for some times
 // It is used to update the display information and notice the user that the COM has been lost
 bool hasSerialComTimedOut = true;
private:

};
#endif
