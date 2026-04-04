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

#include <string>

#include "..\macros.h"
#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\ExePath.h"
#include "..\Misc\GenericLoopedMode.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\Game.h"

#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\VehicleSpawner.h"

typedef char *PCHAR;

namespace sub
{
	namespace GhostRiderMode
	{
		void ToggleOnOff();
		void Tick();

		extern std::string outfitFileName;
		void ApplyGhostRiderOutfit();
		void SpawnGhostRiderRide();
		void Sub_GhostRiderMode();
	}
}