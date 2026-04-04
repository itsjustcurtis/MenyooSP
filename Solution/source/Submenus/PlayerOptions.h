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
#include "..\Util\GTAmath.h"
#include "..\Util\FileLogger.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Memory\GTAmemory.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Camera.h"

#include "..\Misc\SmashAbility.h"

#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\EntityManagement.h"

#include <string>
#include <vector>

typedef unsigned short UINT16;

namespace sub
{
	void PlayerOptionsMenu();
	
	namespace PedConfigFlagManager
	{
		struct NamedPedFlagS 
		{ 
			UINT16 id; 
			std::string title; 
		};
		extern std::vector<NamedPedFlagS> pedFlags;

		void FlagListMenu();
		extern int flagID;
		void CustomFlagSetterMenu();
	}

	void CloneCompanionMenu();

}




