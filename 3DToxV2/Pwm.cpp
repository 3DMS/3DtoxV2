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

#include "Pwm.h"

// Based on https://playground.arduino.cc/Code/PwmFrequency/
// The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
// The base frequency for pins 5 and 6 is 62500 Hz.

// The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,256, and 1024.
// The divisors available on pins 3 and 11 are: 1, 8, 32, 64, 128, 256, and 1024

// Pins 5 and 6 are paired on timer0
// Pins 9 and 10 are paired on timer1
// Pins 3 and 11 are paired on timer2

// Changes on pins 3, 5, 6, or 11 may cause the delay() and millis()
// Changes on pins 9 or 10 will cause the Servo library to function incorrectly


// setup PWM pre and post scalers
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor)
    {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6)
    {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
  else if(pin == 3 || pin == 11)
  {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

// setup PWM for specific 1Khz frequency 976.5625 Hz
void setPin5and6PwmFrequency1Khz()
{
  setPwmFrequency(5,64);
}

//7812.5 Hz
void setPin5and6PwmFrequency7812hz()
{
  setPwmFrequency(5,8);
}


void setPin5and6PwmFrequency62500hz()
{
  setPwmFrequency(5,1);
}
// 244.14 Hz
void setPin5and6PwmFrequency244hz()
{
  setPwmFrequency(5,256);
}

// 61.03 Hz
void setPin5and6PwmFrequency61hz()
{
  setPwmFrequency(5,1024);
}

// setup PWM duty cycle to drive the fan speed
void setPin5and6PwmDutyCycle(int percent)
{
    int counter = (int)((percent * 255) / 100) + 1;
    if(counter >= 255)
    {
      counter = 255;
    }
    else if(counter < 0)
    {
      counter = 0;
    }
  OCR0A = counter;
  OCR0B = counter;
}
