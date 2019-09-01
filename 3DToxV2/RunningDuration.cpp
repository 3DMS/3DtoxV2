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

#include "RunningDuration.h"

// This class is dedicated into tracking running duration data and formating it in order to spread the data overthe whole EEPROM Memory
// This class mainly optimizes the lifespan of the EEPROM of your board.
// Each EEPROM memory byte has a lifespan of about 10k writes.
// If we write data every minute on the same memory block, the lifespan would be about 10K minutes => 166.666 hours => 6.94 days! only
// So using this method would kill your EEPROM memory within 1 week.

// -------------------------------------------why this is needed-------------------------------

// In order to prevent this from happening we are using a method used in automobile, that spreads the data over the whole memory.
// Let's take an example using 1000 bytes.
// If we want to save data every minutes as the previous example, in order to spread data over the memory we would write into a different
// memory block every minutes. So the new lifespan in this example would be 1000 blocks x 10K = 1 000 000 minutes (1 million) => 16666,666 hours, 694.4 days => 1.9 year.
// which is much better!
// The best part in our case is that the Mega 2560 has 4KB of memory, so the real EEPROM memory lifespan is 4 times the one we calculated above => 7.6 years! of non stop running.
// So in real life, you can use your device for about 10 years as it won't run 24Hours a day non stop.

//  ----------------------------------------- The algorythm --------------------------------------
// in order to spread data over a whole memory range, we will ADD every new unit (here we are writing every 1 minutes)
// to the next memory address.
// we are using a variable to track the last written memory offset
// when we reach the last memory address, we come back to the first memory address and we ADD 1 to the existing data
// here is an example over 3 bytes!
// first all our bytes are 0
// offset: 0 | memory state 0x00 0x00 0x00
// then we add 1 minute to the first byte
// offset: 0 | memory state 0x01 0x00 0x00
// then we increment our writing offset by
// offset: 1 | memory state 0x01 0x00 0x00
// then we +1 to the current memory location using the offset
// offset: 1 | memory state 0x01 0x01 0x00
// +1 to the offset
// offset: 2 | memory state 0x01 0x01 0x00
// +1 to data @ add 2
// offset: 2 | memory state 0x01 0x01 0x01
// +1 to offset. here we reached the end of memory, so we reset our offset to 0
// offset: 0 | memory state 0x01 0x01 0x01
// +1 to data
// offset: 2 | memory state 0x02 0x01 0x01
// you repeat untill you reach the max value of each memory block
// Note: that this counter over 3 bytes can store 3 * 255 minutes => 12.75 hours before resetting. each byte adding 255 minutes (4h 15 minutes).
// the duration in minutes is then reset to 0 minutes once each block has reach it's max value
// In order to understand how many minutes we can store over 4000 bytes the calculation is simple: 4000*255 = 1 020 000 minutes => 17000 hours -> 708.3 days => 1.94 years
// In order to make sure the counter doesn't reset after about 2 years, we have a separate days counter spread over 2 bytes (65025 days) => 178 years
// This Days counter is incremented by 1 every time the minutes counter has reached 1 day.
// the minutes counter is then reset to 0


CRunningDuration::CRunningDuration(CConfig* config)
{
   this->_config  = config;
   // calculating here the epprom memory capacity in minutes.
   // we we store minutes corresponding in 1 day only then we are using only 1440 bytes over the whole memory capacity.
   // the other remaining bytes will never be used
   // recalculating here the max memory capacity based on real EEPROM size and EEPPROM_START_ADDR
   int maxMemorySize = EEPROM.length() - EEPPROM_START_ADDR - 1;
   this->_config->maxMemoryDays    = (int)(maxMemorySize / this->_config->MinutesInDay);
   this->_config->maxMemoryHours   = (int)((maxMemorySize - this->_config->maxMemoryDays * this->_config->MinutesInDay) / this->_config->MinutesInHour);
   this->_config->maxMemoryMinutes = (int)(maxMemorySize - (this->_config->maxMemoryDays * this->_config->MinutesInDay) - (this->_config->maxMemoryHours * this->_config->MinutesInHour));
}

// This function loads the current running duration based on Days + minutes spread over 4000 bytes
// if the Memory version read doesn't match the current version, the memory is reseted
void CRunningDuration::LoadRunningDuration()
{
    byte InitSequence0 = EEPROM.read(0);
    byte InitSequence1 = EEPROM.read(1);
    byte InitSequence2 = EEPROM.read(2);

    if(InitSequence0 != EEPROM_INIT_0 || InitSequence1 != EEPROM_INIT_1 || InitSequence2 != EEPROM_INIT_2)
    {
      CEEPROM::ResetEEPROM();
    }

    this->_currentSpreadWriteAddr = this->SpreadReadMinutes(EEPPROM_START_ADDR);
}

// this function resets running duration to 0 and also resets EEPROM data
void CRunningDuration::ResetCounter()
{
    DisableEEPROMWrite = true;
    ResetRunningDuration();
    CEEPROM::ResetEEPROM();
    this->_currentSpreadWriteAddr = this->SpreadReadMinutes(EEPPROM_START_ADDR);
    DisableEEPROMWrite = false;
}

// This function spread read the minutes from memory and returns the next addr position to write
int CRunningDuration::SpreadReadMinutes(int startAddr)
{
    this->ResetRunningDuration(); // reset current days / hours / minutes from local varibles
    byte currentByte    = 0x00;   // variable used to track memory data
    byte previousByte   = 0x00;
    int  resumeWriteAddr = startAddr; // current addres to start resuming writting

    byte HDay = EEPROM.read(EEPROM_DAYS_H_ADDR); // reading current High byte for Days. 1 = 256 days
    byte LDay = EEPROM.read(EEPROM_DAYS_L_ADDR); // reading current low byte days : 1 = 1 day
    byte HOURS    =  EEPROM.read(EEPROM_HOURS_ADDR);
    byte MINUTES  =  EEPROM.read(EEPROM_MINUTES_ADDR);
    // note here Days is int => 16 bits = 2 bytes
    this->_config->Days     = HDay * 256;  //read days HByte and converting to real days
    this->_config->Days     += LDay;       //read days LByte
    this->_config->Hours    = HOURS;
    this->_config->Minutes  = MINUTES;

// this sections is only used for serial debugging
#ifdef DEBUG
    Serial.print ("Available EEPROM size:");
    Serial.print (EEPROM.length() - startAddr, DEC);
    Serial.print (" Bytes\r\nStart Read Addr: ");
    Serial.print (resumeWriteAddr, DEC);
    Serial.print ("\r\nLoaded HDay:0x");
    Serial.print (HDay, HEX);
    Serial.print ("\r\nLoaded LDay:0x");
    Serial.print (LDay, HEX);
    Serial.print ("\r\nLoaded Hours:0x");
    Serial.print (this->_config->Hours, HEX);
    Serial.print ("\r\nLoaded Minutes:0x");
    Serial.print (this->_config->Minutes, HEX);
    Serial.print ("\r\nMemory Dump:\r\n");
#endif

//update minutes by directly reading bytes from EEPROM
    for (int i = (startAddr); i < EEPROM.length(); i++)
    {
      currentByte = EEPROM.read(i);
#ifdef DEBUG
      Serial.print ("0x");
      Serial.print (currentByte, HEX);
      Serial.print (", ");
#endif
    // if current byte is 0 this means that we have not yet written in it so there is no more data to read
      if( currentByte == 0x00)
      {
#ifdef DEBUG
        Serial.print ("\r\nLast Write Addr:");
        Serial.print (i, DEC);
        Serial.print ("\r\n");
#endif
        return i;
      }
      else if( currentByte > 0x00 && previousByte == 0x00) // checking here that only the first byte has been written
      {
        this->_config->EEPROMMinutes = currentByte; // asigning new value to current minutes (obviously it's always 1).
        previousByte = currentByte; //tracking previous byte for next iteration
      }
      else if(currentByte > 0x00 && previousByte != 0x00) // case of nominal usage
      {
          this->_config->EEPROMMinutes += currentByte; // incrementing current minutes by the amount of minutes the current byte has stored
          if(currentByte < previousByte)
          {
            resumeWriteAddr = i;
          }
          previousByte = currentByte;// tracking current byte as previous for next iteration
      }

      if( this->_config->EEPROMMinutes >= 60) // handling max amount of minutes and increments to hours and days
      {
          this->_config->EEPROMHours += (int)(this->_config->EEPROMMinutes / 60);
          this->_config->EEPROMDays  += (int)(this->_config->EEPROMHours / 24);

          this->_config->EEPROMHours = this->_config->EEPROMHours % 24;
          this->_config->EEPROMMinutes = this->_config->EEPROMMinutes % 60;
      }
  }
  //only used for serial debug
#ifdef DEBUG
    Serial.print ("\r\nLast Write Addr:");
    Serial.print (resumeWriteAddr, DEC);
    Serial.print ("\r\n");
#endif

  return resumeWriteAddr;// returning last address to use for next write
}

// returns true if the memory has reach it's end
bool CRunningDuration::CheckAndUpdateSpreadMemoryLoop()
{
  // detection EEPROM memory max capacity
  // in this case we will dump current duration contained inside EEPROM spread data into non spread memory
  if( this->_config->EEPROMHours == this->_config->maxMemoryHours && this->_config->EEPROMMinutes == this->_config->maxMemoryMinutes && this->_config->EEPROMDays == this->_config->maxMemoryDays)// we have reached 1 new day
  {
    // during this time when we are accessing EEPROM, we want to prevent any measurments that could be made wrong
    this->_config->IgnoreFirstValues += 2;

    // Updating Minutes and Hours based on duration spread into EEPROM and duration not spread
    // adjusting new values to write into non spread memory
    int newHours = this->_config->Hours + this->_config->EEPROMHours;
    if(newHours >= 24)
    {
      newHours -=24;
      this->_config->Days += 1;
    }
    this->_config->Hours = newHours;

    int newMinutes = this->_config->Minutes + this->_config->EEPROMMinutes;
    if(newMinutes >= 60)
    {
      newMinutes -=24;
      this->_config->Hours += 1;
    }
    this->_config->Minutes = newMinutes;

    // updating non spread memory
    CEEPROM::SafeWriteEEPROMData(EEPROM_HOURS_ADDR,this->_config->Hours);
    CEEPROM::SafeWriteEEPROMData(EEPROM_MINUTES_ADDR,this->_config->Minutes);

    byte HDays = (byte)((this->_config->Days ) / 255);
    byte LDays = (byte)((this->_config->Days ) % 255);

    CEEPROM::SafeWriteEEPROMData(EEPROM_DAYS_H_ADDR,HDays);
    CEEPROM::SafeWriteEEPROMData(EEPROM_DAYS_L_ADDR,LDays);

    // resetting spread memory
    for(int i = EEPPROM_START_ADDR; i < EEPROM.length(); i++)//cleaning minutes data
    {
      CEEPROM::SafeWriteEEPROMData(i,0x00);
    }

    //resetting EEPROM variables
    this->_config->EEPROMDays     = 0;
    this->_config->EEPROMHours    = 0;
    this->_config->EEPROMMinutes  = 0;
    return true;
  }

  return false;
}

// saving days + minutes into memory using the spread mecanism
int CRunningDuration::SpreadSaveIncrementMinutes(int startAddr)
{
  // prevent any writting if we are already making one at the same time
  if(DisableEEPROMWrite == true)
  {
    return startAddr;
  }

  DisableEEPROMWrite = true;

  int currentAddr  = startAddr;

  if(CheckAndUpdateSpreadMemoryLoop())
  {
    currentAddr = EEPPROM_START_ADDR;
  }

  byte currentByte = EEPROM.read(currentAddr);
  CEEPROM::SafeWriteEEPROMData(currentAddr,currentByte + 1);
  if( currentAddr == (EEPROM.length() - 1) )
  {
    currentAddr = EEPPROM_START_ADDR;//skip location for days
  }
  else
  {
    currentAddr++;
  }
  DisableEEPROMWrite = false;

  return currentAddr;
}

// resets local variables that store current running duration
void CRunningDuration::ResetRunningDuration()
{
  this->_config->EEPROMDays     = 0;
  this->_config->EEPROMHours    = 0;
  this->_config->EEPROMMinutes  = 0;

  this->_config->Seconds = 0;
  this->_config->Minutes = 0;
  this->_config->Hours   = 0;
  this->_config->Days    = 0;
}

// mechanism used in order to reduce the EEPROM writing frequency
void CRunningDuration::CheckAndSaveRunningDuration()
{
  if( this->_config->RunningDurationLastSaveSeconds >= RUNNING_DURATION_AUTOSAVE_SECONDS)
  {
    _currentSpreadWriteAddr = SpreadSaveIncrementMinutes(_currentSpreadWriteAddr);
    this->_config->RunningDurationLastSaveSeconds = 0;
#ifdef DEBUG
   Serial.print ("Saving Running Duration \r\n");
   PrintRunningDuration();
#endif
  }
}


void CRunningDuration::PrintRunningDuration()
{
#ifdef DEBUG
   Serial.print (this->_config->Days, DEC);
   Serial.print ("D ");
   Serial.print (this->_config->Hours, DEC);
   Serial.print ("H:");
   Serial.print (this->_config->Minutes, DEC);
   Serial.print ("m:");
   Serial.print (this->_config->Seconds, DEC);
   Serial.print ("s\r\n");
#endif
}

void CRunningDuration::IncrementTime(int seconds)
{
  this->_config->Seconds += seconds;
  if(this->_config->Seconds >= 60)
  {
      this->_config->Seconds -= 60;
      this->_config->Minutes++;
  }
  if(this->_config->Minutes >= 60)
  {
     this->_config->Minutes -= 60;
     this->_config->Hours++;
  }
  if( _config->Hours >= 24)
  {
    this->_config->Hours -= 24;
    this->_config->Days++;
  }

  this->_config->RunningDurationLastSaveSeconds++;

#ifdef DEBUG
    Serial.print ("Running since ");
    this->PrintRunningDuration();
#endif
}

// dedicated function filling the spread memory with 1s in order to test that when the address resets
// the counter is properly saved into eeprom memory and the spread memory is reset to 0.
// Note: only used for testing
void CRunningDuration::TestNewDay()
{
  const int TotalMinutesIn1DayMin1 = (24 * 60) - 1;
  const int AvailableEEPROMSize = (EEPROM.length() - EEPPROM_START_ADDR);
  int MemoryLimitIndex          =  TotalMinutesIn1DayMin1 % AvailableEEPROMSize;//TotalMinutesIn1DayMin1 - AvailableEEPROMSize + 4;
  int LoopCount                 = (int)(TotalMinutesIn1DayMin1 / AvailableEEPROMSize);

  CEEPROM::SafeWriteEEPROMData(EEPROM_DAYS_H_ADDR,0);
  CEEPROM::SafeWriteEEPROMData(EEPROM_DAYS_L_ADDR,0);

#ifdef DEBUG
  Serial.print ("TestNewDay()\r\nTotalMinutesInDayMinus1: ");
  Serial.print (TotalMinutesIn1DayMin1, DEC);
  Serial.print ("\r\nAvailableEEPROMSize: ");
  Serial.print (AvailableEEPROMSize, DEC);
  Serial.print ("\r\nMemoryLimitIndex: ");
  Serial.print (MemoryLimitIndex, DEC);
  Serial.print ("\r\nLoopCount: ");
  Serial.print (LoopCount, DEC);
  Serial.print ("\r\n");
  this->PrintRunningDuration();
#endif

  for (int i = EEPPROM_START_ADDR; i < EEPROM.length(); i++)
  {
      if((i - EEPPROM_START_ADDR) < MemoryLimitIndex)
      {
        CEEPROM::SafeWriteEEPROMData(i, LoopCount + 1);
      }
      else
      {
        CEEPROM::SafeWriteEEPROMData(i, LoopCount);
      }
  }
}
