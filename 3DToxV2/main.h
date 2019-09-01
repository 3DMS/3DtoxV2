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

#ifndef _MAIN
#define _MAIN

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

#include "Constants.h"
#include "EEPROM_functions.h"
#include <LiquidCrystal.h>

#include <SPI.h>
#include "SD.h"
#include "ViewBase.h"
#include "digitalWriteFast.h"
#include "FanController.h"
#include "RunningDuration.h"

#include "CmdParser/CmdParser.hpp"
#include "CmdParser/CmdCallback.hpp"

#include <SoftwareSerial.h>

void Interrupt1();
void IncrementTime(int seconds);
void MonitorAttachedDevices();
void ResetCounters();

void MainProcess();
bool InitializeSDCard();
void UninitSDCard();

void SetupPins();
void SetupInitialRunningDuration();
void LoadDataFromEeprom();
void ConfigureRegisters();
void ResetComIfNeeded();
void UpdateFanSpeedIfNeeded();
void HandleFanSpeedForNonManualModes(AirQualityStatus currentAQStatus);
void LogDataToSdIfAvailable(AirQualityStatus currentAQStatus);

void HandleEncoderButtonPress();
void HandleComMessages();

void(* resetFunc) (void) = 0;//declare reset function at address 0

int ResetDownTick = 0;

CConfig* _config = new CConfig();
CRunningDuration* _runningDuration = new CRunningDuration(_config);
ViewBase* _currentView;
volatile uint8_t portbhistory = 0xFF;     // default is high because the pull-up
int RefreshRateDividerValue = 10;

CmdCallback<1> cmdCallback;
char strTemp[] = "ok T:";

SoftwareSerial _SoftwareSerial(sSerialRxPin, sSerialTxPin);
CmdParser serialParser;
CmdBuffer<92> serialBuffer;
int serialTimeout = 0;

unsigned int GetPWMFanSpeed();
byte GetPWMFanDutyCycle();
void SetFanSpeed(unsigned int);

bool LastStateBTN_EN1 = 0;
bool LastStateBTN_EN2 = 0;
bool LastStateBTN_ENC = 0;

bool HandleRotaryEncoder(); // returns true if Rotary encoder has changed
bool IsTurningdCW(bool currentStateEnc1,
                  bool currentStateEnc2,
                  bool previousStateEnc1,
                  bool previousStateEnc2);
int rotationDivider = ROTATION_DIVIDER;
bool lastRotationWasCW = true;
void Beep();
void SetNewBaudrate(long baudrate);
void SerialPrint(const String &s);

#endif
