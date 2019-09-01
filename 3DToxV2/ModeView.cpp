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


 // -------------------------------Mode view-------------------
 // this screen is dedicated into displaying Working modes
 // the amount of possible modes to display exceed 4.
 // In order to be able to scroll down we are using 2 variables
 // viewWindowMinIndex and  viewWindowMaxIndex
 // These are representing a visible window of indexes to display

#include "ModeView.h"
#include "lcd.h"
#include "config.h"
#include "utility.h"
#include "Constants.h"
#include "Pm25.h"
#include "MainSettingsView.h"

ModeView::ModeView() //constructor
{
  _needUpdate = true;                                 // tag screen for refresh
  lcd_init();                                         // resetting the LCD display. This allows to recover from any garbage screen
  lcd_clear();                                        // clear the display
}

// Handle scroll index when know is turned "up"
void ModeView::Up()
{
  MenuSelectedIndex++;
  if(MenuSelectedIndex > (MAX_MENU_ITEMS - 1))
  {
    MenuSelectedIndex = (MAX_MENU_ITEMS - 1);
  }
  // Handle the visible index window
  if(MenuSelectedIndex > viewWindowMaxIndex)
  {
    viewWindowMinIndex += 1;
    viewWindowMaxIndex += 1;
  }
  _needUpdate = true;                               // tag screen for refresh
  Refresh();                                        // force refresh
}

// Handle scroll index when know is turned "down"
void ModeView::Down()
{
  MenuSelectedIndex--;
  if(MenuSelectedIndex < 0)
  {
    MenuSelectedIndex = 0;
  }

  // Handle the visible index window
  if(MenuSelectedIndex < viewWindowMinIndex)
  {
    viewWindowMinIndex -= 1;
    viewWindowMaxIndex -= 1;
  }
  Refresh();                                        // tag screen for refresh
  _needUpdate = true;                               // force refresh
}

// updating current working mode in the shared configuration object
ViewBase* ModeView::Select()
{
  switch(MenuSelectedIndex)
  {
    case 0:
     break;
    case 1:
      _config->CurrentAQMode = AUTO;
    break;
    case 2:
      _config->CurrentAQMode = RS_232;
    break;
    case 3:
      _config->CurrentAQMode = QUIET;
      break;
    case 4:
      _config->CurrentAQMode = MANUAL;
    break;
    default: // in this default case we won't update the setting
    break;
  }
  return new MainSettingsView();        // Once a mode is selected, We return to previous screen
}

// Main refresh function for this screen
void ModeView::Refresh()
{
  if( _needUpdate == false)           // prevent refresh if no changes were done
  {
      return;
  }
  lcd_clear();                        // clear display

  lcd_setCursor(0,MenuSelectedIndex - viewWindowMinIndex);      // Draw menu arrow
  lcd_print(">");

  // draw the air quality modes
  // Only 4 items can be displayed, so viewWindowMinIndex is in use here
   for( int i= 0 ;i < 4; i++) // max 4 lines
   {
       lcd_setCursor(1,i);
       String currentLabel = String(MenuLabels[i + viewWindowMinIndex]);
       lcd_print(currentLabel);
       if(currentLabel == String(AQMODE_STRING[_config->CurrentAQMode]))
       {
          lcd_print(" (*)");
       }
   }
}
