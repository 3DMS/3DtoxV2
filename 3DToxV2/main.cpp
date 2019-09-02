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

#include "main.h"
#include "EEPROM_functions.h"
#include "Constants.h"
#include "macros.h"
#include <TimerOne.h>
#include "Pwm.h"
#include "lcd.h"

#include "Pm25.h"
#include "utility.h"
#include "ViewBase.h"
#include "ViewMain.h"
#include "CmdParser/CmdBuffer.hpp"
#include "CmdParser/CmdCallback.hpp"
#include "CmdParser/CmdParser.hpp"


// Setting up FanController here
FanController fan(PWM_FAN_INPUT_PIN_1, MEASUREMENT_PERIOD_MS, RPM_SPEED_DEVIDER, PWM_OUTPUT_CONTROL_PIN);

// setup all IO pins here
void SetupPins()
{
  // rotary encorder pins pullups
  pinMode(BTN_EN1, INPUT_PULLUP);
  pinMode(BTN_EN2, INPUT_PULLUP);
  pinMode(BTN_ENC, INPUT_PULLUP);

  // Fan power mosfet pin + PWM pin
  pinMode(PWM_FAN_POWER_OUTPUT_PIN, OUTPUT);  // Board D8 pin output
  digitalWrite(PWM_FAN_POWER_OUTPUT_PIN, LOW);  //Fan is disabled at Setup
  pinMode(PWM_OUTPUT_CONTROL_PIN, OUTPUT);    // PWM fan PWM control pin output

  // SD card detection pin and Chip select
  pinMode(SD_DETECT_PIN, INPUT_PULLUP);
  pinMode(CS_PIN, OUTPUT);

  // serial pins
  pinMode(SERIAL_POWER_PIN, OUTPUT);        // IO pin that will power up the RS232 transciever
                                            // this pins in needed if you want to flash your board
                                            // without unconnecting the RS232 module
  digitalWrite(SERIAL_POWER_PIN, LOW);      // starting with RS232 module OFF

  pinAsOutput(BEEPER_PIN);                  // setting up Beeper pin
}

// Function used only if you want to setup your own duration on the LCD
// Change values to set your own duration
void SetupInitialRunningDuration ()
{
  #ifdef FIRSTRUN
    CEEPROM::ResetEEPROM();
    // set initial data
    CEEPROM::SafeWriteEEPROMData(EEPROM_HOURS_ADDR,_config->maxMemoryHours);
    CEEPROM::SafeWriteEEPROMData(EEPROM_MINUTES_ADDR,_config->maxMemoryMinutes);
    CEEPROM::SafeWriteEEPROMData(EEPROM_DAYS_L_ADDR,_config->maxMemoryDays);

    for( int i = EEPPROM_START_ADDR; i < 4*60; i++)
    {
      CEEPROM::SafeWriteEEPROMData(i,0x01);
    }
  #endif
}

// loading Running duration + settings from EEPROM
void LoadDataFromEeprom ()
{
    _runningDuration->LoadRunningDuration();
    byte AqMode       = EEPROM.read(EEPROM_MODE);           // possible modes: AUTO;//QUIET; // MANUAL;
    byte BaudrateMode = EEPROM.read(EEPROM_BAUDRATE);       // possible values {"9600", "57600", "115200", "250000"};

    //-------------------loading Air Quality mode
    if(AqMode == 0){_config->CurrentAQMode = AUTO;}
    else if(AqMode == 1){_config->CurrentAQMode = RS_232;}
    else if(AqMode == 2){_config->CurrentAQMode = QUIET;}
    else if(AqMode == 3){_config->CurrentAQMode = MANUAL;}
    //-------------------Loading RX/TX baudrate speed
    if(BaudrateMode == 1){_config->RxTxBaudrate = 9600;}
    else if(BaudrateMode == 2){_config->RxTxBaudrate = 57600;}
    else if(BaudrateMode == 3){_config->RxTxBaudrate = 115200;}
    else if(BaudrateMode == 4){_config->RxTxBaudrate = 250000;}
}


void setup() {

  _config->_pm25 = new CPm25((int)SET_PIN, (int)RESET_PIN);       // setup Air quality sensor pins
  _currentView   = new ViewMain();                                // loading main view
  _currentView->SetConfig(_config);                               // providing current config to main view
  SetupPins();                                                    // setup IO pins

  lcd_init();                                                     // Init LCD display
  lcd_clear();                                                    // clear LCD

// initialize rotary encoder state
  LastStateBTN_ENC = digitalReadFast(BTN_ENC);
  LastStateBTN_EN1 = digitalReadFast(BTN_EN1);
  LastStateBTN_EN2 = digitalReadFast(BTN_EN2);

  // using software Serial because Hardware serial won't work using the RS232 modules
  _SoftwareSerial.begin(_config->RxTxBaudrate);
  digitalWrite(SERIAL_POWER_PIN, HIGH);                            // enable RS232 module by powering it on
  SetupInitialRunningDuration();                                   // SetupInitialRunningDuration => working if FIRSTRUN define is enabled (see config.h)
  LoadDataFromEeprom();                                            // load settings from EEPROM
  ConfigureRegisters();                                            // COnfigure registers for timings

  fan.begin();                                                     // Startup the fan
  _config->CurrentPwmDutyCyclePercent = 100;                       // setting default fan speed to 100% speed
  SetFanSpeed(_config->CurrentPwmDutyCyclePercent);
}

// function dedidacted to configureing registers mainly for 1Hz interrupt
// and for Fan HAl effect sensor interrupt
void ConfigureRegisters()
{
  cli();//disable interrupts
  //configure clock rate
  SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);
  SPSR |= (1<<SPI2X);

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;   // set entire TCCR1A register to 0
  TCCR1B = 0;   // same for TCCR1B
  TCNT1  = 0;   //initialize counter value to 0

  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  ///---------------------------------------------
  EICRA |= (1 << ISC00) | (1 << ISC01);// set INT0 to trigger on ANY logic change
  PCMSK0 |= (1 << PCINT0);

  // Don't change the PWM frequency inthat in that configuration
  // It changes how the timer0 is behaving and the fan speed counter will be messed up
  //setPin5and6PwmFrequency62500hz();

  sei();//enable interrupts
}

// setup SD card reader
bool InitializeSDCard()
{
  if(_config->SDCARD_INITIALIZED == true)
  {
    return true;
  }
  if (digitalRead(SD_DETECT_PIN) == HIGH) // check if SD card is present
  {
    return false;
  }

  if(!SD.begin(CS_PIN)) {                 // Initializing SD card if possible
      return false;
  }
  _config->SDCARD_INITIALIZED = true;
  return true;
}

// unloading SD card module
void UninitSDCard()
{
  if(_config->SDCARD_INITIALIZED == false)
  {
    return;
  }
  SD.end();//SD lib 1.2.2
  _config->SDCARD_INITIALIZED = false;
}

void Interrupt1()
{
  _config->TickCounter1++;
}


// mostly getting info from Air Quality sensor
void MonitorAttachedDevices()
{

  double ticker1 = _config->TickCounter1;
  ResetCounters();

  _config->_pm25->ReadValues();

  if(_config->IgnoreFirstValues > 0)// ignoring the very first measurements to prevent false alarm;
  {
    _config->IgnoreFirstValues--;
  }
}

// checking if Baudrate settings has changed and applying new settings if needed
void ResetComIfNeeded()
{
  if(_config->hasBaudrateChanged == true)
  {
    SetNewBaudrate(_config->RxTxBaudrate);
    _config->hasBaudrateChanged = false;
  }
}

// checking if fan speed has been updated and applying new speed to it
void UpdateFanSpeedIfNeeded()
{
  // adjust fan speed based on current settings
  if( _config->waitCyclesBeforeUpdatingSpeed == 0)
  {
    byte currentFanSpeed = GetPWMFanDutyCycle();

    if(currentFanSpeed != _config->CurrentPwmDutyCyclePercent)
    {
      SetFanSpeed(_config->CurrentPwmDutyCyclePercent);
    }
  }
  else {
    _config->waitCyclesBeforeUpdatingSpeed -= 1;
  }
}

// logging current telemetry stored in config object into to SD card
void LogDataToSdIfAvailable(AirQualityStatus currentAQStatus)
{
  char cdataString[128];
  sprintf(cdataString,"%04dJ %02dH:%02dm:%02ds|MODE:%-09s|PM1:%3i| PM2.5:%3i| PM10:%3i| SPEED:%3i%%| RPM:%5i| AQ:%14s|T:%3i",
                      _config->Days, _config->Hours, _config->Minutes, _config->Seconds,
                      AQMODE_STRING[_config->CurrentAQMode],
                      _config->_pm25->GetPM01(),
                      _config->_pm25->GetPM2_5(),
                      _config->_pm25->GetPM10(),
                      _config->CurrentPwmDutyCyclePercent,
                      _config->Rpm1,
                      AQ_STRING[currentAQStatus],
                     (int)_config->HotEndTemp);
  String dataString = String(cdataString);
  if(digitalRead(SD_DETECT_PIN) == LOW)//check if sd card is present
  {
    if(InitializeSDCard())
    {
      SDFile dataFile = SD.open("datalog.txt", FILE_WRITE);
      // if the file is available, write to it:
      if (dataFile)
      {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
      }
    }
  }
  else
  {
    UninitSDCard();
  }
}

// Norml mode speed management
void HandleFanSpeedForNonManualModes(AirQualityStatus currentAQStatus)
{
  if(_config->CurrentAQMode !=  MANUAL)
  {
    _config->CurrentPwmDutyCyclePercent = _config->_pm25->GetSpeedBasedOnAQMode(_config->CurrentAQMode, currentAQStatus);

    if( ((_config->HotEndTemp > -1.0 && _config->HotEndTemp < 100)
          && (_config->CurrentAQMode != MANUAL))
        ||((_config->CurrentAQMode == RS_232)
          && (_config->hasSerialComTimedOut == true)))
    {
      _config->CurrentPwmDutyCyclePercent = 0;
      SetFanSpeed(_config->CurrentPwmDutyCyclePercent);
    }
  }
}

// check if rotary encoder has been pressed
// if it has been pressed for more than RESET_COUNTER_TRIGGER_LIMIT_IN_SEC the running duration counter will be reset
//
void HandleEncoderButtonPress()
{
  if(digitalRead(BTN_ENC) == LOW)
  {
    ResetDownTick++;
    if(ResetDownTick >= RESET_COUNTER_TRIGGER_LIMIT_IN_SEC)
    {
      Beep();
      //Trigger clock counter reset
      _runningDuration->ResetCounter();
      CEEPROM::ResetEEPROM();
      resetFunc(); //call reset
    }
  }
  else
  {
    ResetDownTick = 0;
  }
}

// reading hot end  and heat bed temperatures from 3D printer data
// updating internal variable for later use
// Data is extracted from standard 3D printer Gcode
// first we send M105 command to 3D printer
// Then 3D printer replies with temprature data
void HandleComMessages()
{
  SerialPrint("M105");

    if(serialBuffer.readFromSerial(&_SoftwareSerial, MAIN_LOOP_DELAY))
    {
      if (serialParser.parseCmd(serialBuffer.getStringFromBuffer()) != CMDPARSER_ERROR)
      {
        int parameterCount = serialParser.getParamCount();
        String cmd0 =  serialParser.getCmdParam(0);
        if(parameterCount == 6 && cmd0 == "ok")
        {
          String param = serialParser.getCmdParam(1);
          if( param[0] =='T')
          {
            serialTimeout = 0;
            _config->hasSerialComTimedOut = false;
            int delimIdx = param.indexOf(':');
            if(delimIdx != -1)
            {
              String HotendTemp = param.substring(delimIdx + 1);

               param    = serialParser.getCmdParam(3);
               delimIdx = param.indexOf(':');
               if(delimIdx > -1)
               {
                 String BedTemp = param.substring(delimIdx + 1);

                 if((BedTemp.indexOf(".") != -1) && (HotendTemp.indexOf(".") != -1))
                 {
                   _config->HotEndTemp = HotendTemp.toFloat();
                   _config->BedEndTemp = BedTemp.toFloat();
                 }
               }
             }
           }
        }else {
          serialTimeout++;
        }
      }else
      {
        serialTimeout++;
      }
    }else
    {
      serialTimeout++;
    }

    // special case to handle COm timeout if wire is disconnected, or printer is shutdown
    if(serialTimeout >= 10)
    {
      _config->HotEndTemp = -1.0;
      _config->hasBaudrateChanged = true;
      _config->hasSerialComTimedOut = true;
      serialTimeout  = 0;
    }
}


// main loop
void loop()
{
  // checking config and reset Com settings if used has updated baudarate
  ResetComIfNeeded();
  // depending on working mode, the fan speed is adjusted here
  UpdateFanSpeedIfNeeded();
  RefreshRateDividerValue -= 1; // counter to limit LCD screen updates

  // determining if LCD screen needs to be updated because user has pushe the rotary encoder
  bool needsDisplayRefresh = HandleRotaryEncoder();
  //checking if we need to update the Display
  //checking if either the rotary encoder has changed or display time has elapsed
  if(RefreshRateDividerValue > 0 )//&& needsDisplayRefresh == false) // uncomment this piece of code if you need refresh right after user rotates encoder
  {
    delay(MAIN_LOOP_DELAY);//simple delay for next loop to happen
    return;
  }
  RefreshRateDividerValue = REFRESH_RATE_DIVIDER;

  // handle Fan speed readings
  _config->Rpm1 = GetPWMFanSpeed();
  // capping RPM values in order to prevent unexpected behavior
  if(_config->Rpm1 < 0)
  {
    _config->Rpm1 = 0;
  }
  // refresh current view
  if( _currentView != NULL)
  {
    _currentView->Refresh();
  }

  // Handle Other features here
  if(_config->IgnoreFirstValues == 0)
  {
    int pm25Avg = _config->_pm25->GetAvgPM2_5(); // read Air quality data
    AirQualityStatus currentAQStatus = _config->_pm25->ConvertPM2_5ToAirQualityStatus(pm25Avg); // Compute Air Quality Status based on Air quality data
    HandleFanSpeedForNonManualModes(currentAQStatus); // adjust fan speed based on Air quality level
    LogDataToSdIfAvailable(currentAQStatus);          // Data logging into SD card when possible
  }

  HandleEncoderButtonPress(); // checking if encoder button has been pressed
  HandleComMessages();        // check COM messages to retrieve Hot end temperature when available
  _runningDuration->CheckAndSaveRunningDuration();  // update running duration when needed
}

// reset counter used to compute fan speed
void ResetCounters()
{
  _config->TickCounter1 = 0;
}

unsigned int GetPWMFanSpeed()
{
   return fan.getSpeed(); // Send the command to get RPM
}

// reading Fan Duty cycle
byte GetPWMFanDutyCycle()
{
  return fan.getDutyCycle();
}

// setting fan speed here
void SetFanSpeed(unsigned int SpeedPercentage)
{
  fan.setDutyCycle(max(min(SpeedPercentage, 100), 0));
  // need to reset fan power supply in order for RPM to be applied
  digitalWrite(PWM_FAN_POWER_OUTPUT_PIN, LOW);
  if(SpeedPercentage > 0)
  {
    delay(1);
    digitalWrite(PWM_FAN_POWER_OUTPUT_PIN, HIGH);
  }
}

// interrupt called every 1 second. used to track running duration.
// !!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!
// it's ISR interrupt so don't put any heavy work in here as it may result in unexpected behavior
//interrupt @ 1Hz
ISR(TIMER1_COMPA_vect){
  MonitorAttachedDevices();
  if(_config->CurrentPwmDutyCyclePercent > 0)
  {
    _runningDuration->IncrementTime(1);
  }
}

// check if any action were performed on the knob: Either pressed, or rotated Clock wise or Counter clock wise
bool HandleRotaryEncoder()
{
  bool hasStatusChanged = false;
  //read current state of encoder before use.
  //maker sure the state doesn't change during this process
  bool currentStateEnc = digitalReadFast(BTN_ENC);
  bool currentStateEn1 = digitalReadFast(BTN_EN1);
  bool currentStateEn2 = digitalReadFast(BTN_EN2);

  //button was pressed
  if(currentStateEnc != LastStateBTN_ENC)
  {
    // when knob is pressed we handle, calling the Select from current view will
    // return a pointer to a new view
    // In that case we need to delete the previous view and display the new one
    if(currentStateEnc == true)//button released
    {
        Beep();
        ViewBase* newView = _currentView->Select();
        //changing reference tuo current view if the current view returned a valid pointer
        if(newView != NULL)
        {
          delete _currentView;
          _currentView = NULL;
          _currentView = newView;
          _currentView->SetConfig(_config);
          _currentView->Refresh();
        }
    }
    LastStateBTN_ENC = currentStateEnc;
    hasStatusChanged = true;
  }
      //checking if the encoder has turned and forwarding the event to the selected view
    if(currentStateEn1 != LastStateBTN_EN1 || currentStateEn2 != LastStateBTN_EN2)
    {
        //checking clockwise direction-------------------------
        //determining the rotation direction
        if(_currentView != NULL)
        {
          if(IsTurningdCW(currentStateEn1, currentStateEn2, LastStateBTN_EN1, LastStateBTN_EN2) == true)
          {
            if(lastRotationWasCW == true)
            {
              rotationDivider -=1;
              if(rotationDivider == 0)
              {
                Beep();
#ifdef INVERT_ENCODER
                _currentView->Up();     // forwarding event to current view
#else
                _currentView->Down();   // forwarding event to current view
#endif
                rotationDivider = ROTATION_DIVIDER;
                hasStatusChanged = true;
              }
            }
            else
            {
              lastRotationWasCW = true;
              rotationDivider   = ROTATION_DIVIDER;
            }
          }
          else{
            if(lastRotationWasCW == false)
            {
              rotationDivider -= 1;
              if(rotationDivider == 0)
              {
                Beep();
#ifdef INVERT_ENCODER
                _currentView->Down(); // forwarding event to current view
#else
                _currentView->Up();   // forwarding event to current view
#endif
                rotationDivider = ROTATION_DIVIDER;
                hasStatusChanged = true;
              }
            }
            else
            {
              lastRotationWasCW = false;
              rotationDivider   = ROTATION_DIVIDER;
            }
          }
        }
        //updating last state
        LastStateBTN_EN1 = currentStateEn1;
        LastStateBTN_EN2 = currentStateEn2;
    }
    return hasStatusChanged;
  }

  // Computing encoder states to determine the rotation direction
  bool IsTurningdCW(bool currentStateEnc1,
                    bool currentStateEnc2,
                    bool previousStateEnc1,
                    bool previousStateEnc2)
  {
    if((currentStateEnc1 == false && currentStateEnc2 == true && previousStateEnc1 == true && previousStateEnc2 == true)
      || (currentStateEnc1 == false && currentStateEnc2 == false && previousStateEnc1 == false && previousStateEnc2 == true)
      || (currentStateEnc1 == true && currentStateEnc2 == false && previousStateEnc1 == false && previousStateEnc2 == false)
      || (currentStateEnc1 == true && currentStateEnc2 == true && previousStateEnc1 == true && previousStateEnc2 == false)
      )
    {
      return true;
    }
    else {
      return false;
    }
  }

  // setting baudrate
  void SetNewBaudrate(long baudrate)
  {
    _SoftwareSerial.flush();// wait for last transmitted data to be sent
    _SoftwareSerial.begin(baudrate);
  }

  // perform Beep using the LCD buzzer
  void Beep()
  {
    digitalWrite(BEEPER_PIN, HIGH);
    delay(50);
    digitalWrite(BEEPER_PIN, LOW);
  }

// sending data throug Serial connection. mainly used to send M105 to 3D printer
void SerialPrint(const String &s)
{
  _SoftwareSerial.println(s);
}
