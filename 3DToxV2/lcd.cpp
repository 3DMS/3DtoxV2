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

#include "lcd.h"

LCD_CLASS lcd(LCD_PINS_RS,
              LCD_PINS_ENABLE,
              LCD_PINS_D4,
              LCD_PINS_D5,
              LCD_PINS_D6,
              LCD_PINS_D7); 

void lcd_print(const String &s)
{
  lcd.print(s);
}
void lcd_print(char* data, int length)
{
	for (int i = 0; i< length; i++)
	{
		lcd.print(charset_mapper(*(data+i)));
	}
}

char charset_mapper(const char c) {
    static uint8_t utf_hi_char; // UTF-8 high part
    static bool seen_c2 = false;
    uint8_t d = c;
    if (d >= 0x80u) { // UTF-8 handling
      if (d >= 0xC0u && !seen_c2) {
        utf_hi_char = d - 0xC2u;
        seen_c2 = true;
        return 0;
      }
      else if (seen_c2) {
        d &= 0x3Fu;
        #ifndef MAPPER_ONE_TO_ONE
          HARDWARE_CHAR_OUT(pgm_read_byte_near(utf_recode + d + (utf_hi_char << 6) - 0x20));
        #else
          HARDWARE_CHAR_OUT(0x80u + (utf_hi_char << 6) + d);
        #endif
      }
      else {
        HARDWARE_CHAR_OUT('?');
      }
    }
    else {
      HARDWARE_CHAR_OUT(c);
    }
    seen_c2 = false;
    return 1;
  }

void lcd_init()
{
	lcd.begin(LCD_COLUMNS, LCD_ROWS);
	lcd.noBlink();
}
void lcd_setCursor(int col, int row)
{
	lcd.setCursor(col, row);
}
void lcd_off()
{
	lcd.noDisplay();
}
void lcd_clear()
{
	lcd.clear();
}
void lcd_home()
{
	lcd.home();
}
