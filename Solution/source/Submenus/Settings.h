/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"
#include "..\Menu\MenuConfig.h"
#include "..\Menu\Language.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Natives\types.h" // RGBA
#include "..\Scripting\enums.h"
#include "..\Scripting\Game.h"

#include <string>
#include <math.h>

typedef signed char INT8;
typedef unsigned char UINT8;

class RGBA;

namespace sub
{
	void Settings();

	void SettingsMenuPos();
	extern UINT8 settingsHUDColor;

	void AddsettingscolOption(const std::string& text, RGBA& feature);
	void SettingsColours();
	void SettingsColours2();

	void AddsettingsfonOption(const std::string& text, int font_index, INT8& feature);
	void SettingsFonts();
	void SettingsFonts2();

	void SettingsLanguage();
	void SettingsThemesMain();
}




