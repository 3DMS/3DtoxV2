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

#include "MainSettingsView.h"
#include "lcd.h"
#include "config.h"
#include "Constants.h"
#include "ViewMain.h"
#include "ModeView.h"
#include "BaudrateView.h"
#include "EEPROM_functions.h"

// -------------------- Main Settings View screen----------------
// this class is dedicated into handleling the main settings screen.
// This screen is displayed when user is pressing the knob for the first time
// It mainly contains sub menus based on functionalities

MainSettingsView::MainSettingsView()                      //constructor
{
  _needUpdate = true;                                     // internal variable to determines if the display needs a refresh due to some use change
  lcd_init();                                             // resetting the LCD display. This allows to recover from any garbage screen
  lcd_clear();                                            // clear the display
}

// Handle scroll index when know is turned "up"
void MainSettingsView::Up()
{
  MenuSelectedIndex++;                                    // increase scroll index
  if(MenuSelectedIndex > (MAX_MENU_ITEMS - 1))            // make sure the scroll index doesn't exceed max scroll position
  {
    MenuSelectedIndex = (MAX_MENU_ITEMS - 1);
  }

  _needUpdate = true;                                      // tag the screen to be updated
  Refresh();                                               // force refresh
}

// Handle scoll index when user turns the knob "down"
void MainSettingsView::Down()
{
  MenuSelectedIndex--;
  if(MenuSelectedIndex < 0)                               // making sure the index doesn't go negative
  {
    MenuSelectedIndex = 0;
  }
    _needUpdate = true;
    Refresh();
}

// Select a new Menu when user is pressing on the knob
ViewBase* MainSettingsView::Select()
{
  switch(MenuSelectedIndex)
  {
    case 0:                                             // Return to previous menu
       return new ViewMain();
       break;
    case 1:                                             // Get inside Mode settings
        return new ModeView();
        break;
    case 2:                                             // Get inside the Baudrate settings
        return new BaudrateView();
        break;
    case 3:                                             // Save current settings to EEPROM
      {
          //-------------------------Save baudrate---------------------
          byte BaudrateMode = 0;
          if(_config->RxTxBaudrate == 9600){BaudrateMode = 1;}
          else if(_config->RxTxBaudrate == 57600){BaudrateMode = 2;}
          else if(_config->RxTxBaudrate == 115200){BaudrateMode = 3;}
          else if(_config->RxTxBaudrate == 250000){BaudrateMode = 4;}

          //-----------------------Save Air Quality mode----------------
          byte AQModeMode = 0;
          if(_config->CurrentAQMode == AUTO){AQModeMode = 0;}
          else if(_config->CurrentAQMode == RS_232){AQModeMode = 1;}
          else if(_config->CurrentAQMode == QUIET){AQModeMode = 2;}
          else if(_config->CurrentAQMode == MANUAL){AQModeMode = 3;}

          // writing data to EEPROM
          CEEPROM::SafeWriteEEPROMData(EEPROM_MODE, AQModeMode);
          CEEPROM::SafeWriteEEPROMData(EEPROM_BAUDRATE, BaudrateMode);
      }
      return new ViewMain();
    default:
      return new ViewMain();                      // returns Main view if for any reason an index is not handled properly
    break;
  }
}

// Main refresh function that draws all the sub menus and the small index tick ">"
void MainSettingsView::Refresh()
{
    if(_needUpdate == true)                                           // check whether the display data needs update
    {
        lcd_clear();                                                   // clear the whole screen

        lcd_setCursor(0 , MenuSelectedIndex - viewWindowMinIndex);    // display selected meny arrow ">"
        lcd_print(">");

        for( int i = 0 ;i < 4 ; i++) // max 4 lines                  // draw visible menus.
        {                                                            // here this is fairly simple as there are only sub menus
           lcd_setCursor(1 , i);
           lcd_print(String(MenuLabels[i + viewWindowMinIndex]));
        }
        _needUpdate = false;                                          // prevent from refreshing screen if nothing has changed
    }
}
