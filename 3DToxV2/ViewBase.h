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

#ifndef _VIEWBASE
#define _VIEWBASE
#include "config.h"

class ViewBase {

public:
//constructor
 void SetConfig(CConfig* config);
 //pure virtual functions used as interface by other views
 // each view will have to implement this interface so that the screen Knob
 // events can be forwarded into each views as they are instanciated
 virtual void Up()          = 0;
 virtual void Down()        = 0;
 virtual ViewBase* Select() = 0;
 virtual void Refresh()     = 0;

protected:
  int SelectionIndex = 0;
  CConfig* _config;
  bool _needUpdate = true;

  int MenuSelectedIndex   = 0; // variable used to track the current menu selected index
  int viewWindowMinIndex  = 0; // used to determin which menus to display when the amount of menus are supperior to the amount of lines on the LCD
  int viewWindowMaxIndex  = 3; // this can be overriden by child class
};

#endif //_VIEWBASE
