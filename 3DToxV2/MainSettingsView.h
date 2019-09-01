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

#ifndef _VIEWMAINSETTINGS
#define _VIEWMAINSETTINGS
#include "config.h"
#include "ViewBase.h"

class MainSettingsView : public ViewBase{

public:
MainSettingsView(); //constructor

//ViewBase is a common interface that all views need to override
void Up() override;
void Down() override;
ViewBase* Select() override;
void Refresh() override;

 static const int MAX_MENU_ITEMS = 4;
 char *MenuLabels[MAX_MENU_ITEMS] = {"..", "Select Mode","Bauderate", "Save"}; // possible menus to display

};
#endif  //_VIEWMAINSETTINGS
