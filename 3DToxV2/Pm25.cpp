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

#include <Arduino.h>
#include "Pm25.h"

// THis class is dedicated into managing the PM25 air quality sensor

CPm25::CPm25(int pin_set, int pin_reset)
{
  SET_PIN   = pin_set;
  RESET_PIN = pin_reset;

  Serial3.begin(9600);         // using serial3 that is mapped on special pins
  Serial3.setTimeout(1500);    // set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
  pinMode(SET_PIN, OUTPUT);    // 1 = the module works in continuous sampling mode, it will upload the sample data after the end of each sampling. (The sampling response time is 1000ms)
                               // 0, the module enters a low-power standby mode.
  pinMode(RESET_PIN, OUTPUT);
  WakeUp();                    // waking up the device
}

// function that disable the device
void CPm25::Sleep()
{
 digitalWrite(SET_PIN, LOW);
 digitalWrite(RESET_PIN, HIGH);
}

// enable the device
void CPm25::WakeUp()
{
 digitalWrite(SET_PIN, HIGH);
 digitalWrite(RESET_PIN, HIGH);
}

// read the data from RX buffer, automatically sent by the sensor
void CPm25::ReadValues()
{
  if (Serial3.available() == 0)           // check if we received data from the sensor
  {
    return;
  }

  // when the sensor is sending data, the stream will start by 0x424d
  if( Serial3.find(0x42) )                // first starting byte 0x42
  {
      Serial3.readBytes(buf,LENG);

      if(buf[0] == 0x4d)                  // 2nd starting byte
      {
        if(checkValue(buf,LENG))
        {
          PM01Value  = transmitPM01(buf); //count PM1.0 value of the air detector module
          PM2_5Value = transmitPM2_5(buf);//count PM2.5 value of the air detector module
          PM10Value  = transmitPM10(buf); //count PM10 value of the air detector module

          // each type of data is stored inside dedicated lists.
          // These lists will be averaged in order to smooth the device reaction
          updateList(&ListAvgPM01 , PM01Value );
          updateList(&ListAvgPM2_5, PM2_5Value);
          updateList(&ListAvgPM10 , PM10Value );
        }
      }
    }
}

// storing new measurement into a fifo
void CPm25::updateList(LinkedList<int> *list, int newValue)
{
  if(list == NULL)
  {
    return;
  }
  if(newValue == -1)
  {
    return;
  }
  if(list->size() >= AVG_LIST_SAMPLE_SIZE)
  {
    list->remove(0);//remove the first item
  }
    list->add(newValue);//add new item at the end
}

// computing and checking CRC of the received data from the sensor
char CPm25::checkValue(unsigned char *thebuf, char len)
{
  char receiveflag = 0;
  int receiveSum   = 0;

  for(int i = 0; i < (len - 2); i++)
  {
    receiveSum = receiveSum + thebuf[i];
  }
  receiveSum = receiveSum + 0x42;

  if(receiveSum == ((thebuf[len - 2] << 8) + thebuf[len - 1]))  //check CRC the serial data
  {
    receiveSum  = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

// extracting PM01 value from the received buffer
int CPm25::transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val = ((thebuf[3] << 8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

// extracting PM2.5 value from the received buffer
int CPm25::transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val = ((thebuf[5] << 8) + thebuf[6]);//count PM2.5 value of the air detector module
  return PM2_5Val;
  }

// extracting PM10 value from the received buffer
int CPm25::transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[7] << 8) + thebuf[8]); //count PM10 value of the air detector module
  return PM10Val;
}

// Extrapolating Fan speed based on current Air quality
// Depending on the current working mode, th efan speed will vary
int CPm25::GetSpeedBasedOnAQMode(AirQualityMODE mode,AirQualityStatus status )
{
  switch(mode)
  {
    case AUTO:
    case MANUAL:// in manual mode this speed is just ignored
        return FindFanSpeedInsideFanSpeedTranslationMap(&NormalFanSpeedMap[0], status);
    case QUIET:
        return FindFanSpeedInsideFanSpeedTranslationMap(&QuietFanSpeedMap[0], status);
    default: //other undifined modes
      return 100;
    break;
  }
}

// Using Mapping to extract fan speed based on air quality status
// It's very easy to add more speed profiles by adding a new map inside Pm25.h and linking it into function GetSpeedBasedOnAQMode
int CPm25::FindFanSpeedInsideFanSpeedTranslationMap(IntTranslationMap *map, AirQualityStatus status)
{
  for (int i = 0; i < MAPSIZE; i++)
  {
    if(map[i].key == status)
    {
      return map[i].value;
    }
  }
}

// dedicated function to convert Pm values 2.5 into AirQuality level
// this level can be adjusted inside PM2.5.h
AirQualityStatus CPm25::ConvertPM2_5ToAirQualityStatus(int PM_25_Value)
{
  return ComputePm25ToAirQualitStatus(&Pm25ToAirQualityStatusMap[0], PM_25_Value);
}

AirQualityStatus CPm25::ComputePm25ToAirQualitStatus(IntTranslationMap *map, int pm25)
{
  for( int i = 0; i < MAPSIZE - 1; i++)
  {
    if(pm25 <= map[i].key)
    {
      return map[i].value;
    }
  }
  return map[MAPSIZE - 1].value; //default value
}

int CPm25::GetAvgPM01()
{
  return GetAvgPM(&ListAvgPM01);
}
int CPm25::GetAvgPM2_5()
{
  return GetAvgPM(&ListAvgPM2_5);
}
int CPm25::GetAvgPM10()
{
  return GetAvgPM(&ListAvgPM10);
}

int CPm25::GetAvgPM(LinkedList<int> *list)
{
  int listSize = list->size();
  if(listSize == 0)
  {
    return -1;
  }

  long sum = 0;
  for( int i = 0; i < listSize; i++)
  {
     sum += list->get(i);
  }
  return (int)(sum / listSize);
}
int CPm25::GetAvgListSize()
{
  return ListAvgPM2_5.size();
}
