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

#include "..\Scripting\GTAentity.h"

#include <string>
#include <vector>

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"

#include "..\Submenus\WeaponOptions.h"
#include "..\Util\keyboard.h"

#include "..\Scripting\Game.h"

#include "..\Util\ExePath.h"

#include <windows.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <json\single_include\nlohmann\json.hpp>

namespace sub
{
	namespace PtfxSubs
	{
		extern int ITEMS_PER_PAGE;
		struct PtfxS
		{
			std::string name;
			std::string asset;
			std::string fx;
		};

		inline bool operator==(const PtfxS& a, const PtfxS& b) 
		{
			return a.name == b.name && a.asset == b.asset && a.fx == b.fx;
		}

		extern const std::vector<PtfxS> PTFX;

		typedef struct 
		{ 
			GTAentity entity; 
			std::string asset;
			std::string fx; 
		} PtfxlopS;

		extern std::vector<PtfxlopS> fxLoops;
		void AddEntityToPtfxLops(const PtfxS& effect, const GTAentity& entity);
		void AddOptionEntity(const PtfxS& effect, GTAentity entity);
		void AddOptionLoopOnEntity(const PtfxS& effect, GTAentity entity);
		void PTFXFavourites();
		void PTFXSub();

	}

}




