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

#ifndef _VIEWMODE
#define _VIEWMODE
#include "config.h"
#include "ViewBase.h"

class ModeView : public ViewBase{

public:
ModeView(); //constructor
//public:
  void Up() override;
  void Down() override;
  ViewBase* Select() override;
  void Refresh() override;


 static const int MAX_MENU_ITEMS = 5;
 char *MenuLabels[MAX_MENU_ITEMS] = {"..", "AUTO","RS_232", "QUIET", "MANUAL"};
};
#endif  //_VIEWMODE
