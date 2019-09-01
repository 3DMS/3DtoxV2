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

#ifndef _PWM
#define _PWM
#include <Arduino.h>
#include "FanController.h" //lib FanController
#include "Constants.h"

void InitializePwmFanController();
void setPwmFrequency(int pin, int divisor);
void setPin5and6PwmFrequency1Khz();
void setPin5and6PwmDutyCycle(int percent);
#endif
