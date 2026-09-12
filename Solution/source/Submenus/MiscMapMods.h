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

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAprop.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"

#include "..\Submenus\Teleport\TeleMethods.h"

#include <pugixml\src\pugixml.hpp>
#include <string>
#include <vector>

typedef int Object;
typedef unsigned long DWORD, Hash;

namespace sub
{
	namespace MapMods
	{
		void UnloadAllMapMods();
		void MapMods();
		void MapMods2();
	}
}