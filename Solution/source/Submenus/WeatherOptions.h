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

#include "..\Natives\natives2.h"
#include "..\Memory\GTAmemory.h"
#include "..\Scripting\World.h"
#include "..\Util\FileLogger.h"

#include <string>
#include <map>
#include <array>

namespace sub
{
	void WeatherMenu();

	namespace WeatherClouds
	{
		extern const std::array<std::string, 20> cloudNames;
		void CloudMenu();
	}
}