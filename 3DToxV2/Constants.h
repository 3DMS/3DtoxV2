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
// This files contains all the contants used by the different modules.
// This is a single place to tweak or change any behavior of the system.
// There should not be any hardcoded values inside other files

#ifndef _CONSTANTS
#define _CONSTANTS
#include <Arduino.h>

const int  RESET_COUNTER_TRIGGER_LIMIT_IN_SEC = 9;

  const int FIRMWARE_VERSION_MAJOR     = 1;
  const int FIRMWARE_VERSION_MINOR     = 0;
  const int FIRMWARE_VERSION_REVISION  = 0;

  const int MAX_FAN_RPM               = 19000;  // used to match the max fan speed and provide an overall speed based on 100%
  const byte PWM_FAN_INPUT_PIN_1      = 19;     // blue wire Z+
  const byte PWM_OUTPUT_CONTROL_PIN   = 4;      // D4 yellow wirepin 11 => Timer 2, pin 5= Timer0  D4: Servo 1ère ligne
  const byte PWM_FAN_POWER_OUTPUT_PIN = 9;      // PIND D9


   const int maxSpeed = 100;                    // max speed SpeedPercentage
   const int MEASUREMENT_PERIOD_MS  = 1000;     // measurement period in ms. must be >= 1000
   // The Speed sensor on the fan might has 2 magnets, so the hall sensor will detec 2 pulses for each fan rotation
   // Fan model THA0412BN = 2
   const int RPM_SPEED_DEVIDER      = 1;
   const int DISPLAY_PERIOD_MS      = 1000;     // The refresh period of the main screen in milliseconds
   const int MAIN_LOOP_DELAY        = 5;        // delay for the main loop in milliseconds

  const int WARNING_SPEED  = 1000;              // minimum speed in RPM used to display possible warnings on screen
  const int CRITICAL_SPEED = 60;                // minimum speed in RPM where the FAN is considered stopped or blocked

  const int FAN_SPEED_INCREMENT = 10;           // fan speed increment in % for the manual mode

  //_SoftwareSerial is used on AUX 2 port
  const int sSerialRxPin = 63;                  // pin 63 of port AUX2 is used for RX pin
  const int sSerialTxPin = 40;                  // Pin 40 of port AUX2 is used for TX pin
  const int SERIAL_POWER_PIN = 42;              // Pin 42 is used to power on or off the RS232 modules 20mA is suffisent for the RS232 module to work


    // Air Quality sensor pins for class PM25Pins
    // For This device Serial3 is used and corresponds to pins 14(tx) - 15(rx)
  const int SET_PIN     = 64;                   // AUX 2 port Pin 64 used for the Set pin
  const int RESET_PIN   = 44;                   // AUX 2 port Pin 44 is used for the Reset Pin

  //SD card pins
  const byte CLK_PIN  = 52;                     // PORT LCD2 pin 52
  const byte M0SI_PIN = 51;                     // PORT LCD2 Pin51
  const byte CS_PIN   = 53;                     // PORT LCD2 Pin 53

  const byte SD_DETECT_PIN = 49;                // PORT LCD2 Pin 49
  const byte KILL_PIN = 41;                     // PORT LCD2 Pin 41

  const byte BEEPER_PIN = 37;                   // PORT LCD1 Pin 37

#ifndef MOSFET_D_PIN
  #define MOSFET_D_PIN     -1
#endif
#ifndef RAMPS_D8_PIN
  #define RAMPS_D8_PIN      8                   // Mosfet used on D8 originally to power the 3D printer BED
#endif
#ifndef RAMPS_D9_PIN
  #define RAMPS_D9_PIN      9                   // Mosfet used on D9 originally used to power the Fan Output
#endif
#ifndef RAMPS_D10_PIN
  #define RAMPS_D10_PIN    10                   // Mosfet used on D10 originally used to power the hot end 0
#endif
#define HEATER_0_PIN       RAMPS_D10_PIN       // Mosfet used on D10 originally used to power the hot end 0

#define FAN_PIN        RAMPS_D9_PIN            // Defining Fan PIN to D9

#define SDSS              53                   // SDcard SSDSS pin

// LCD configuration for 20x4 display
const byte REFRESH_RATE_DIVIDER = DISPLAY_PERIOD_MS / MAIN_LOOP_DELAY; // LCD refresh rate divider
const int LCD_COLUMNS = 20;
const int LCD_ROWS    = 4;


// LCD Knob rotary encoder
#define INVERT_ENCODER
#define ROTATION_DIVIDER  2                   // amount of pulses received by the rotary encoder before increasing the rotation by 1
#define BEEPER_PIN        37                  // Buzzer pin on port LCD1 Pin 37
#define BTN_EN1           31                  // Knob encoder 1 pin on port LCD2 Pin 31
#define BTN_EN2           33                  // Knob encoder 1 pin on port LCD2 Pin 33
#define BTN_ENC           35                  // Knob push button pin on port LCD1 Pin 35

#define LCD_PINS_RS     16                    // LCD RS pin on Port LCD1 Pin 16
#define LCD_PINS_ENABLE 17                    // LCD ENABLE pin on Port LCD1 Pin 17
#define LCD_PINS_D4     23                    // LCD Data pin 4 on Port LCD1 Pin 23
#define LCD_PINS_D5     25                    // LCD Data pin 5 on Port LCD1 Pin 25
#define LCD_PINS_D6     27                    // LCD Data pin 6 on Port LCD1 Pin 27
#define LCD_PINS_D7     29                    // LCD Data pin 7 on Port LCD1 Pin 29

  #endif
