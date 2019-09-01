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

#ifndef _LCD
#define _LCD

#include <LiquidCrystal.h>
#include "Constants.h"

#define LCD_CLASS LiquidCrystal
#define HARDWARE_CHAR_OUT(C) lcd.write((char)(C))

// UTF scring encodings used to display special caracters
const PROGMEM uint8_t utf_recode[] =
           { // 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f   This is relative complete.
               0x20,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0x22,0xA9,0xAA,0xAB,0x3F,0x3F,0xAE,0x3F,  // c2a ¡¢£¤¥¦§¨©ª«¬­®¯
             //' '   ¡    ¢    £    ¤    ¥    ¦    §    "    ©    ª    «    ?    ?    ®    ?
               0xB0,0xB1,0xB2,0xB3,0x27,0xB5,0xB6,0xB7,0x2C,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,  // c2b °±²³´µ¶·¸¹º»¼½¾¿
             // °    ±    ³    ²    ?    µ    ¶    ·    ,    ¹    º    »    ¼    ½    ¾    ¿
               0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,  // c38 ÀÁÃÄÅÆÇÈÉÊËÌÍÎÏ
             // À    Á    Â    Ã    Ä    Å    Æ    Ç    È    É    Ê    Ë    Ì    Í    Î    Ï
               0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,  // c39 ÐÑÓÔÕÖ×ØÙÚÛÜÝÞß
             // Ð    Ñ    Ò    Ó    Ô    Õ    Ö    ×    Ø    Ù    Ú    Û    Ü    Ý    Þ    ß
               0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,  // c3a àáãäåæçèéêëìíîï
             // à    á    â    ã    ä    å    æ    ç    è    é    ê    ë    ì    í    î    ï
               0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF   // c3b ðñóôõö÷øùúûüýþÿ
             // ð    ñ    ò    ó    ô    õ    ö    ÷    ø    ù    ú    û    ü    ý    þ    ÿ
           };
char charset_mapper(const char c);
void lcd_print(const String &s);
void lcd_print(char* data, int length);
void lcd_init();
void lcd_off();
void lcd_clear();
void lcd_home();
void lcd_setCursor(int col, int row);

#endif
