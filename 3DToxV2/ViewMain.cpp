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

//---------------------- Main view displaying current status of the device

#include "ViewBase.h"
#include "ViewMain.h"
#include "lcd.h"
#include "config.h"
#include "utility.h"
#include "Constants.h"
#include "MainSettingsView.h"

//constructor
ViewMain::ViewMain()
{
  lcd_init();             // initializing the LCD. this also prevent from displaying garbage if transmission gets corrupted
  lcd_clear();            // clear the display from possible garbage or previous data
}

// triggered when rotating encoder moves "upward"
// Up and Down events will be only used in "MANUAL" mode.
// user will be able to adjust manually the fan speed
void ViewMain::Up()
{
  // checking is we are in MANUAL mode here
  if(_config->CurrentAQMode == AUTO || _config->CurrentAQMode == RS_232 || _config->CurrentAQMode == QUIET)
  {
    return;
  }
  //it's likely that we will update the fan speed here so we tag the refresh flag so the screen is refreshed
  bool refreshIsNeeded = true;
  _config->CurrentPwmDutyCyclePercent += FAN_SPEED_INCREMENT;

  // THe screen won't update if the current speed is already à max!
  // the fan speed won't be updated more than 100%
  if(_config->CurrentPwmDutyCyclePercent > 100)
  {
    _config->CurrentPwmDutyCyclePercent = 100;
    refreshIsNeeded = false;                  // don't refresh
  }


  _config->waitCyclesBeforeUpdatingSpeed = REFRESH_RATE_DIVIDER;
  if( refreshIsNeeded == true )
  {
    Refresh();                              // ask for screen refresh if needed
  }
}

// same as Up function call except that the fan speed will be decreased and checked >= 0
void ViewMain::Down()
{
  if(_config->CurrentAQMode == AUTO || _config->CurrentAQMode == RS_232 || _config->CurrentAQMode == QUIET)
  {
    return;
  }
  bool refreshIsNeeded = true;
  _config->CurrentPwmDutyCyclePercent -= FAN_SPEED_INCREMENT;
  if(_config->CurrentPwmDutyCyclePercent < 0)
  {
    _config->CurrentPwmDutyCyclePercent = 0;
    refreshIsNeeded = false;
  }
  _config->waitCyclesBeforeUpdatingSpeed = REFRESH_RATE_DIVIDER;
  if( refreshIsNeeded == true )
  {
    Refresh();
  }
}

// when pushing on the knob MainSettings view is displayed
ViewBase* ViewMain::Select()
{
  lcd_clear();
  return new MainSettingsView();
}

// main function that will format and print running duration data on LCD screen
void ViewMain::DisplayCurrentDuration(char* cStringBuffer)
{
    lcd_setCursor(0,0);                                                 // displays the data on the first LCD row
    int totalMinutes = _config->Minutes +  _config->EEPROMMinutes;
    int hoursToReport = (int)(totalMinutes / 60);
    int minutes =  (totalMinutes) % 60;
    int TotalHours = hoursToReport + _config->Hours +  _config->EEPROMHours;
    int daysToReport = (TotalHours) / 24;
    int hours = (TotalHours) % 24;
    int days = daysToReport + _config->Days + _config->EEPROMDays;

    // formats the data to something readable
    sprintf(cStringBuffer,"  %04dJ %02dH:%02dm:%02ds",days, hours, minutes, _config->Seconds);
    lcd_print(cStringBuffer);
}

// formats and display Current Air Quality mode and fan speed in percent
void ViewMain::DisplayCurrentAQ(char* cStringBuffer)
{
  lcd_setCursor(0,3);                                       // displaying data on last display row
  sprintf(cStringBuffer,"Mode:%-09s V:%03s%%",
          AQMODE_STRING[_config->CurrentAQMode],
         itostr3left(_config->CurrentPwmDutyCyclePercent)); // converting integer into a fixed 3 caracter string representation
  lcd_print(cStringBuffer);
}

// formats and display Air quality status on lcd screen
void ViewMain::DisplayCurrentPM(char* cStringBuffer)
{
  int pm25Avg = _config->_pm25->GetAvgPM2_5();

  // formats and display air quality status as a level from 0 to 7
  lcd_print("Air Quality    ");
  lcd_setCursor(14,1);
  lcd_print(" (");
  int currentLevel = 7 - _config->_pm25->ConvertPM2_5ToAirQualityStatus(pm25Avg);
  lcd_print(String(currentLevel));
  lcd_print("/7)");

  // displays here the textual representation of the air quality level
  lcd_setCursor(0,2);
  sprintf(cStringBuffer,"%-20s",
         AQ_STRING[_config->_pm25->ConvertPM2_5ToAirQualityStatus(pm25Avg)]);
  lcd_print(cStringBuffer);
}

// first screen to be displayed with the initilization period displayed
// This screens also displays the current firmware version
// this screen will be displayed untill IgnoreFirstValuesInitValue goes to 0.
// so to increase or decrease this period, just update this value
// the screen can also be removed if used is pressing the knob to go directly into settings view
void ViewMain::DisplayBootScreen(char* cStringBuffer)
{
  lcd_setCursor(0,0);
  lcd_print("      3DTox V2");

  lcd_setCursor(0,2);
  unsigned int percent = (_config->IgnoreFirstValuesInitValue - _config->IgnoreFirstValues) * 100 / _config->IgnoreFirstValuesInitValue;
  sprintf(cStringBuffer, "   ANALYSING %3d%%", percent);
  lcd_print(cStringBuffer);

  lcd_setCursor(0,3);
  if(FIRMWARE_VERSION_REVISION < 10)
  {
    sprintf(cStringBuffer, "  FIRMWARE  V%1d.%1d.%1d", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_REVISION);
  }
  else
  {
    sprintf(cStringBuffer, " FIRMWARE V%1d.%1d.%2d", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_REVISION);
  }
  lcd_print(cStringBuffer);
}

void ViewMain::DisplayFanSpeedOrHotEndTemperatureIfAvailable(char* cStringBuffer)
{
  lcd_setCursor(15,2);
  // in RS232 mode HotEndTemp will be set and different from -1
  // in other modes HotEndTemp will be set to -1
  // displays the current hot end temperature received from RS232 when Available
  // displays T:--- when not available
  if(_config->HotEndTemp > -1) // in RS232 mode
  {
    sprintf(cStringBuffer,"T:%3d",
               (int)(_config->HotEndTemp));
    lcd_print(cStringBuffer);
  }
  else if((_config->CurrentAQMode == RS_232)
    && (_config->hasSerialComTimedOut == true))
  {
      lcd_print("T:---");
  }
  else                        // in other modes we display fan RPM instead
  {
    sprintf(cStringBuffer,"%5d",
           _config->Rpm1);
    lcd_print(cStringBuffer);
  }
}

// updating display
// data will be displayed based on current settings and input data from Rs232 comunication module
void ViewMain::Refresh()
{
  char cStringBuffer[20];

  lcd_setCursor(0,1);                 // resets cursot position to the top left corner of the screen
  // checks wether we are still in the initializing sequence
  if(_config->IgnoreFirstValues > 0)
  {
    DisplayBootScreen(&cStringBuffer[0]);
    return;
  }

  // Displays after initializing period has elapsed
  // part of Air quality textual status and Température or fan speed are displayed on the same LCD line
  DisplayCurrentPM(&cStringBuffer[0]);
  DisplayFanSpeedOrHotEndTemperatureIfAvailable(&cStringBuffer[0]);

  DisplayCurrentAQ(&cStringBuffer[0]);

  DisplayCurrentDuration(&cStringBuffer[0]);
}
