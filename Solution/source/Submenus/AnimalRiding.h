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
#include "..\Scripting\enums.h"
#include "..\Util\ExePath.h"
#include "..\Util\GTAmath.h"
#include "..\Scripting\Model.h"
#include "..\Misc\GenericLoopedMode.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\WeaponIndivs.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\CustomHelpText.h"
#include "..\Scripting\ModelNames.h"

#include <string>
#include <vector>
#include <pugixml\src\pugixml.hpp>

namespace sub
{
	namespace AnimalRiding
	{
		void PopulateAnimals();
		void ToggleOnOff();
		void Tick();
		bool& Enabled();
		void AnimalRidingMenu();
	}
}