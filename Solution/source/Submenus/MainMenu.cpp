/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "MainMenu.h"

namespace sub
{
	void MainMenu()
	{
		static bool firstLoad = true;
		if(firstLoad)
		{
			firstLoad = false;
			addlog(ige::LogType::LOG_TRACE, "First Load of MainMenu Submenu");
		}

		AddTitle("MENYOO");
		AddOption("Players", null, nullFunc, SUB::PLAYERSSUB);
		AddOption("Player Options", null, nullFunc, SUB::PLAYEROPS);
		AddOption("Vehicle Options", null, nullFunc, SUB::VEHICLEOPS);
		AddOption("Teleport Options", null, nullFunc, SUB::TELEPORTOPS);
		AddOption("Weapon Options", null, nullFunc, SUB::WEAPONOPS);
		AddOption("Weather Options", null, nullFunc, SUB::WEATHEROPS);
		AddOption("Time Options", null, nullFunc, SUB::TIMEOPS);
		AddOption("Bodyguard Options", null, nullFunc, SUB::BODYGUARDMAINMENU);
		AddOption("Object Spooner", null, nullFunc, SUB::SPOONER_MAIN);
		AddOption("Misc Options", null, nullFunc, SUB::MISCOPS);
		AddOption("Settings", null, nullFunc, SUB::SETTINGS);

		g_Ped2 = PLAYER_ID();
		g_Ped1 = PLAYER_PED_ID();
		g_PlayerName = GET_PLAYER_NAME(g_Ped2);
		g_Ped3 = GET_PLAYER_GROUP(g_Ped2);
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(MAINMENU, sub::MainMenu)



