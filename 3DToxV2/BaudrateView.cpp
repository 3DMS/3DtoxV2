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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "BaudrateView.h"
#include "lcd.h"
#include "config.h"
#include "utility.h"
#include "Constants.h"
#include "MainSettingsView.h"

// constructor
BaudrateView::BaudrateView()
{
  //initializing the display when switching screen
  lcd_init();
  lcd_clear();
}

// Called when the knob is rolled up
// The MenuSelectedIndex is increased by 1
// the call to refresh is made to redraw the display without
// the selector placed on the proper line
void BaudrateView::Up()
{
  MenuSelectedIndex++;
  if(MenuSelectedIndex > (MAX_MENU_ITEMS - 1))
  {
    MenuSelectedIndex = (MAX_MENU_ITEMS - 1);
  }
  if(MenuSelectedIndex > viewWindowMaxIndex)
  {
    viewWindowMinIndex += 1;
    viewWindowMaxIndex += 1;
  }
  Refresh();
}

// Called when the knob is rolled down
// The MenuSelectedIndex is decresed by 1
// the call to refresh is made to redraw the display without
// the selector placed on the proper line
void BaudrateView::Down()
{
  MenuSelectedIndex--;
  if(MenuSelectedIndex < 0)
  {
    MenuSelectedIndex = 0;
  }
  if(MenuSelectedIndex < viewWindowMinIndex)
  {
    viewWindowMinIndex -= 1;
    viewWindowMaxIndex -= 1;
  }
    Refresh();
}

// Called when selecting a particular Baudrate
// Here we check the MenuSelectedIndex to determin which menu
// has been selected when pressing on the knob to validate
ViewBase* BaudrateView::Select()
{
  _config->hasBaudrateChanged = true;
  switch(MenuSelectedIndex)
  {
    case 0:
    break;
    case 1:
     _config->RxTxBaudrate = 9600;
     break;
    case 2:
      _config->RxTxBaudrate = 57600;
    break;
    case 3:
      _config->RxTxBaudrate = 115200;
      break;
    case 4:
      _config->RxTxBaudrate = 250000;
    break;
    default:
    break;
  }
  return new MainSettingsView();
}

// Called to Refresh the current screen display
// The index MenuSelectedIndex determines which line was selected
// The selector (char '>'') is placed on the currently selected licenses
// As the number of choices exceed the availables lines on the screen
// viewWindowMinIndex is used to shift up or down the information to display

void BaudrateView::Refresh()
{
  //char cStringBuffer[20];
  lcd_clear();
 // display selected meny arrow
  lcd_setCursor(0, MenuSelectedIndex - viewWindowMinIndex);
  lcd_print(">");
 for( int i= 0 ;i < 4; i++)
 {
   lcd_setCursor(1, i);
   String baudrate = String(MenuLabels[i + viewWindowMinIndex]);
   lcd_print(baudrate);

   if(baudrate.toInt() == _config->RxTxBaudrate)
   {
       lcd_print(" (*)");
   }
 }
}
