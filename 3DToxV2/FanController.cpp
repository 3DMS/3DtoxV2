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
 // this file is dedicated to drive in speed the powerfull fan that sucks
 // the air inside the 3DToxV2
 // It can also measure the speed of the fan by using the Hall sensor output of the fan
 // In order to drive the fan speed, we are using the PWM input of the Fan


#include "Arduino.h"
#include "FanController.h"

FanController::FanController(byte sensorPin,
	 													unsigned int sensorThreshold,
														unsigned int speedDivider,
														byte pwmPin)
{
	_sensorPin = sensorPin;                                  // Hall sensor output wire of the fan
                                                           // this wire will give us the actual rpm of the fan
	pinMode(sensorPin, INPUT_PULLUP);                        // Setting this wire pin into
	_sensorInterruptPin = digitalPinToInterrupt(sensorPin);  // using digital interrupt here to mearure rpm
	_sensorThreshold = sensorThreshold;                      // the amount of time in milliseconds that we will use to measure RPM (usually 1000)
	_speed_divider = speedDivider;                           // the Divider corresponds to the amount of impulsion received per fan turn
	_pwmPin = pwmPin;
	pinMode(pwmPin, OUTPUT);
	_pwmDutyCycle = 100;                                     // default fan speed we want at startup
}

// starting the fan speed measurement
void FanController::begin()
{
	static byte instance;
	_instance = instance;
	_instances[instance] = this;
	digitalWrite(_sensorPin, HIGH);                         // Enable pullup on _sensorPin
	setDutyCycle(_pwmDutyCycle);                            // set default fan speed
	AttachInterrupt();                                      // Mapping interruption with callbacks
	instance++;
}

// function used to retrieve speed value in RPM
// this function will perform computation only when called after the _sensorThreshold period
// otherwise it will send the previously calculated speed
unsigned int FanController::getSpeed() {
	unsigned int elapsed = millis() - _lastMillis;
	if ((elapsed > _sensorThreshold)
			&& (elapsed > 0)
			&& (_speed_divider > 0))
	{
		detachInterrupt(_sensorInterruptPin);                                     // detaching interrupts to prevent interruption from happening when performing computation
		double correctionFactor = (double)_sensorThreshold / elapsed;             // creating correction factor based on actual elapsed duration
		_lastReading = correctionFactor * _pulses * 60 / _speed_divider;          // Computing RPM
		_pulses = 0;																															// resetting the count of pulses from FAN HAL effect sensor
		_lastMillis = millis();																										// setting new starting timestamp for next computation
		AttachInterrupt();																												// attaching back interrupt to resume measurement
	}
	return _lastReading;
}

// function dedicated to setting fan PWM duty cycle to control it's speed
void FanController::setDutyCycle(byte dutyCycle) {
	_pwmDutyCycle = min(dutyCycle, 100);
	analogWrite(_pwmPin, (int)(2.55 * _pwmDutyCycle));		// setting speed using arduino analogWrite and providing dutyCycle as int from 0-255
}

// getting last dutycycle used to drive fan speed
byte FanController::getDutyCycle() {
	return _pwmDutyCycle;
}

// attaching interrupt based on the the instance index
void FanController::AttachInterrupt()
{
	switch (_instance)
	{
	case 0:
		attachInterrupt(_sensorInterruptPin, TriggerExt0, FALLING);
		break;
	case 1:
		attachInterrupt(_sensorInterruptPin, TriggerExt1, FALLING);
		break;
	case 2:
		attachInterrupt(_sensorInterruptPin, TriggerExt2, FALLING);
		break;
	case 3:
		attachInterrupt(_sensorInterruptPin, TriggerExt3, FALLING);
		break;
	case 4:
		attachInterrupt(_sensorInterruptPin, TriggerExt4, FALLING);
		break;
	case 5:
		attachInterrupt(_sensorInterruptPin, TriggerExt5, FALLING);
		break;
	}
}

FanController * FanController::_instances[6];

// incrementing amount of pulses when an interrupt happens
void FanController::Trigger()
{
	_pulses++;
}

// calling pulse increment when an interrupt is triggered based on instance index
void FanController::TriggerCaller(byte instance)
{
	if (FanController::_instances[instance] != nullptr)
	{
		FanController::_instances[instance]->Trigger();
	}
}

void FanController::TriggerExt0() { FanController::TriggerCaller(0); }
void FanController::TriggerExt1() { FanController::TriggerCaller(1); }
void FanController::TriggerExt2() { FanController::TriggerCaller(2); }
void FanController::TriggerExt3() { FanController::TriggerCaller(3); }
void FanController::TriggerExt4() { FanController::TriggerCaller(4); }
void FanController::TriggerExt5() { FanController::TriggerCaller(5); }
