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

#ifndef _PM25
#define _PM25

#include <Arduino.h>
#include "LinkedList.h"
#include "utility.h"

#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
#define AVG_LIST_SAMPLE_SIZE  10
#define FILTER_INSTALLED

//set of values without filter
//values from https://docs-emea.rs-online.com/webdocs/1665/0900766b816656b2.pdf Page 9
#ifndef FILTER_INSTALLED
  #define SENSITIVITY 10/10

  #define VERY_GOOD_VALUE 10
  #define GOOD_VALUE 50
  #define MODERATE_VALUE 100
  #define BAD_VALUE 150
  #define UNHEALTHY_VALUE 200
  #define VERY_UNHEALTHY_VALUE 300
  #define HAZARDOUS_VALUE 500
#else
// set of values with clean filter
//good whithout encents
  #define SENSITIVITY 10/10

  #define VERY_GOOD_VALUE 1
  #define GOOD_VALUE 2
  #define MODERATE_VALUE 3
  #define BAD_VALUE 4
  #define UNHEALTHY_VALUE 6
  #define VERY_UNHEALTHY_VALUE 8
  #define HAZARDOUS_VALUE 10

#endif

//have enum in sync and being able to get string from enum name
#define FOREACH_AQ(AQ) \
        AQ(VERY_GOOD)   \
        AQ(GOOD)   \
        AQ(MODERATE)  \
        AQ(BAD)   \
        AQ(UNHEALTHY)  \
        AQ(VERY_UNHEALTHY)  \
        AQ(HAZARDOUS)  \
        AQ(COMPUTING)  \

enum AirQualityStatus {
    FOREACH_AQ(GENERATE_ENUM)
};

static const char *AQ_STRING[] = {
    FOREACH_AQ(GENERATE_STRING)
};

#define FOREACH_AQ_MODE(AQ) \
        AQ(MANUAL)   \
        AQ(AUTO)  \
        AQ(RS_232)  \
        AQ(QUIET)  \

enum AirQualityMODE {
    FOREACH_AQ_MODE(GENERATE_ENUM)
};

static const char *AQMODE_STRING[] = {
    FOREACH_AQ_MODE(GENERATE_STRING)
};

typedef struct {
  int key;
  int value;
} IntTranslationMap;

#define MAPSIZE 7

class CPm25
{
  int SET_PIN;
  int RESET_PIN;

  const IntTranslationMap QuietFanSpeedMap[MAPSIZE] = {  {VERY_GOOD, 40},
                                                         {GOOD, 45},
                                                         {MODERATE, 50},
                                                         {BAD, 60},
                                                         {UNHEALTHY, 70},
                                                         {VERY_UNHEALTHY, 70},
                                                         {HAZARDOUS, 70},
                                                       };
  const IntTranslationMap NormalFanSpeedMap[MAPSIZE] = {{VERY_GOOD, 50},
                                                        {GOOD, 60},
                                                        {MODERATE, 70},
                                                        {BAD, 80},
                                                        {UNHEALTHY, 90},
                                                        {VERY_UNHEALTHY, 100},
                                                        {HAZARDOUS, 100},
                                                      };

  const IntTranslationMap Pm25ToAirQualityStatusMap[MAPSIZE] = {{VERY_GOOD_VALUE * SENSITIVITY, VERY_GOOD},
                                                                {GOOD_VALUE * SENSITIVITY, GOOD},
                                                                {MODERATE_VALUE * SENSITIVITY, MODERATE},
                                                                {BAD_VALUE * SENSITIVITY, BAD},
                                                                {UNHEALTHY_VALUE * SENSITIVITY, UNHEALTHY},
                                                                {VERY_UNHEALTHY_VALUE * SENSITIVITY, VERY_UNHEALTHY},
                                                                {HAZARDOUS_VALUE * SENSITIVITY, HAZARDOUS},
                                                              };

public:
  CPm25(int pin_set,int pin_reset);
  void ReadValues();
  void Sleep();
  void WakeUp();
  int GetSpeedBasedOnAQMode(AirQualityMODE mode,AirQualityStatus status );

  int GetAvgListSize();
  int GetAvgPM01();                         //ug/m3
  int GetAvgPM2_5();                        //ug/m3
  int GetAvgPM10();                         //ug/m3
  int GetPM01()  { return PM01Value; }      //ug/m3
  int GetPM2_5()  { return PM2_5Value; }    //ug/m3
  int GetPM10()  { return PM10Value; }      //ug/m3

  AirQualityStatus ConvertPM2_5ToAirQualityStatus(int PM_25_Value);
//EU limits:
//2.5=> 20/25 ug/m3//combustion particules, organic compound, metals, etc...
//10 => 40-50 ug/m3 // dust, pollen, mold etc...
//PM1 & PM2.5 can get into the lungs
//http://www.irceline.be/fr/documentation/faq/quappelle-t-on-particules-fines

protected:
private:
  HardwareSerial* _hSerial;
  unsigned char buf[LENG];
  int PM01Value = -1;          //define PM1.0 value of the air detector module
  int PM2_5Value= -1;         //define PM2.5 value of the air detector module
  int PM10Value = -1;         //define PM10 value of the air detector module
  int AvgPM01Value  = -1;
  int AvgPM2_5Value = -1;
  int AvgPM10Value  = -1;

  void updateList(LinkedList<int> *list, int newValue);

  int GetAvgPM(LinkedList<int> *list);//ug/m3
  LinkedList<int> ListAvgPM01  = LinkedList<int>();
  LinkedList<int> ListAvgPM2_5 = LinkedList<int>();
  LinkedList<int> ListAvgPM10  = LinkedList<int>();

  char checkValue(unsigned char *thebuf, char len);
  int transmitPM01(unsigned char *thebuf);
  int transmitPM2_5(unsigned char *thebuf);
  int transmitPM10(unsigned char *thebuf);

  int FindFanSpeedInsideFanSpeedTranslationMap(IntTranslationMap *map, AirQualityStatus status);
  AirQualityStatus ComputePm25ToAirQualitStatus(IntTranslationMap *map, int pm25);

};
#endif
