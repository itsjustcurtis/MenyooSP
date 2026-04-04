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
#include "Players.h"

namespace sub
{
	void PlayersMenu()
	{
		AddTitle("Players");

		for (int i = 0; i < GAME_PLAYERCOUNT; i++)
		{
			if (NETWORK_IS_PLAYER_ACTIVE(i))
			{
				bool bPlayerPressed = false;
				AddOption(GET_PLAYER_NAME(i), bPlayerPressed);
				if (bPlayerPressed)
				{
					g_Ped2 = i;
					g_Ped1 = GET_PLAYER_PED(g_Ped2); // Store ped
					g_PlayerName = GET_PLAYER_NAME(g_Ped2); // Store name
					Menu::NewSetMenu(SUB::PLAYERSSUBAMENU);
				}
			}
		}
	}

	void PlayersAMenu()
	{
		bool spectateOn = false;
		bool spectateOff = false;
		bool setWaypoint = false;

		std::string spectatePlayerStr = "CM_SPECTATE";
		if (Game::DoesGXTEntryExist(spectatePlayerStr))
		{
			spectatePlayerStr = Game::GetGXTEntry(spectatePlayerStr);
			spectatePlayerStr = spectatePlayerStr.substr(0, spectatePlayerStr.find('('));
		}
		else 
		{
			spectatePlayerStr = "Spectate Player";
		}

		AddTitle(g_PlayerName); // Title = player name
		AddOption("Set Waypoint To Player", setWaypoint);
		AddLocal(spectatePlayerStr, spectatePlayer == g_Ped2, spectateOn, spectateOff); // Spectate Player

		if (spectateOn)
		{
			Ped ped;
			spectatePlayer = g_Ped2;
			for (int i = 0; i < GAME_PLAYERCOUNT; i++)
			{
				if (!NETWORK_IS_PLAYER_ACTIVE(i)) 
				{
					continue;
				}
				ped = GET_PLAYER_PED(i);
				if (!DOES_ENTITY_EXIST(ped)) 
				{
					continue;
				}
				NETWORK_SET_IN_SPECTATOR_MODE_EXTENDED(0, ped, 1);
				NETWORK_SET_IN_SPECTATOR_MODE(false, ped);
			}
			ped = GET_PLAYER_PED(spectatePlayer);
			if (DOES_ENTITY_EXIST(ped))
			{
				STAT_SET_BOOL(GET_HASH_KEY("MPPLY_CAN_SPECTATE"), true, true);
				NETWORK_SET_IN_SPECTATOR_MODE(true, ped);
			}
		}
		if (spectateOff)
		{
			Ped ped;
			for (int i = 0; i < GAME_PLAYERCOUNT; i++)
			{
				if (!NETWORK_IS_PLAYER_ACTIVE(i)) 
				{
					continue;
				}
				ped = GET_PLAYER_PED(i);
				if (!DOES_ENTITY_EXIST(ped)) 
				{
					continue;
				}
				NETWORK_SET_IN_SPECTATOR_MODE_EXTENDED(0, ped, 1);
				NETWORK_SET_IN_SPECTATOR_MODE(false, ped);
			}
			NETWORK_SET_ACTIVITY_SPECTATOR(false);
			spectatePlayer = -1;
		}

		if (setWaypoint)
		{
			GTAplayer player = g_Ped2;
			if (player.IsActive())
			{
				const GTAped& playerPed = player.GetPed();
				if (playerPed.IsAlive())
				{
					const Vector3& pos = playerPed.GetPosition();
					SET_NEW_WAYPOINT(pos.x, pos.y);
				}
				else
				{
					SET_WAYPOINT_OFF();
				}
			}
			else
			{
				SET_WAYPOINT_OFF();
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(PLAYERSSUB,  		sub::PlayersMenu)
REGISTER_SUBMENU(PLAYERSSUBAMENU, 	sub::PlayersAMenu)




