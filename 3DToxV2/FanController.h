/*
  FanController.h - Library to control up to six fans.
  Created by Giorgio Aresu, November 13, 2016.
  Released into the public domain.

	Updated by Nicolas Rambaud on 23 th of August 2019
	for 3Dtox V2 purpose
*/

#ifndef FanController_h
#define FanController_h

#include "Arduino.h"

class FanController
{
	public:
		FanController(byte sensorPin,
			 						unsigned int sensorThreshold,
									unsigned int speedDivider = 1,
								  byte pwmPin = 0);
		void begin();
		unsigned int getSpeed();
		void setDutyCycle(byte dutyCycle);
		byte getDutyCycle();
	private:
		static FanController *_instances[6];
		byte _sensorPin;
		byte _sensorInterruptPin;
		byte _sensorThreshold;
		byte _speed_divider;
		byte _pwmPin;
		byte _pwmDutyCycle;
		byte _instance;
		unsigned int _lastReading;
		volatile unsigned int _pulses;
		unsigned long _lastMillis;
		void Trigger();
		void AttachInterrupt();
		static void TriggerCaller(byte instance);
		static void TriggerExt0();
		static void TriggerExt1();
		static void TriggerExt2();
		static void TriggerExt3();
		static void TriggerExt4();
		static void TriggerExt5();
};

#endif
