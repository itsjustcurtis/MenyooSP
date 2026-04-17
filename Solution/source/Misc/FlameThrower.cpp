/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "FlameThrower.h"

#include "..\macros.h"

#include "..\Scripting\GTAplayer.h"
#include "..\Natives\natives2.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\Model.h"

#include <vector>

namespace FlameThrower // Why have I made this for 'players' when this is sp and folks on mp/fivem probably can't even see these?
{
	Hash _whash = WEAPON_FIREEXTINGUISHER; // WEAPON_STUNGUN
	struct FlameThrowerPlayerAndFx { GTAplayer player; int fxHandle1; }; // , fxHandle2; };
	std::vector<FlameThrowerPlayerAndFx> _flameThrowerPlayerandFxArray;

	bool IsPlayerAdded(GTAplayer player)
	{
		auto& fts = _flameThrowerPlayerandFxArray;
		for (auto& ft : fts)
		{
			if (ft.player.Handle() == player.Handle())
			{
				return true;
			}
		}
		return false;
	}
	void RemovePlayer(GTAplayer player)
	{
		auto& fts = _flameThrowerPlayerandFxArray;
		for (auto it = fts.begin(); it != fts.end();)
		{
			if (it->player.Handle() == player.Handle())
			{
				it = fts.erase(it);
			}
			else ++it;
		}
	}
	void AddPlayer(GTAplayer player)
	{
		auto& fts = _flameThrowerPlayerandFxArray;
		FlameThrowerPlayerAndFx ft;
		ft.player = player;
		ft.fxHandle1 = 0;
		RemovePlayer(player);
		fts.push_back(ft);
	}

	void RemoveSelf()
	{
		RemovePlayer(PLAYER_ID());
	}
	void AddSelf()
	{
		AddPlayer(PLAYER_ID());
	}

	void Tick()
	{
		if (_flameThrowerPlayerandFxArray.empty())
			return;

		GTAplayer myPlayer = PLAYER_ID();
		GTAped playerPed;
		GTAentity wobject;
		Hash whash;
		float scale;
		int gunBone;

		std::string fxAsset1 = "core";
		std::string fxName1 = "ent_sht_flame";

		scale = 2.0f;

		if (!HAS_NAMED_PTFX_ASSET_LOADED(fxAsset1.c_str()))
		{
			REQUEST_NAMED_PTFX_ASSET(fxAsset1.c_str());
		}

		auto& fts = _flameThrowerPlayerandFxArray;
		for (auto it = fts.begin(); it != fts.end();)
		{
			auto& ft = *it;
			auto& player = ft.player;

			if (!player.IsActive())
			{
				it = fts.erase(it);
				continue;
			}

			playerPed = player.GetPed();
			auto& fxHandle1 = ft.fxHandle1;

			whash = playerPed.GetWeapon();

			if ((!player.IsFreeAiming() && !player.IsTargetingAnything()) || whash != _whash)
			{
				if (DOES_PARTICLE_FX_LOOPED_EXIST(fxHandle1))
					REMOVE_PARTICLE_FX(fxHandle1, false);
				++it;
				continue;
			}

			if (player == myPlayer)
			{
				if (!IS_DISABLED_CONTROL_PRESSED(0, INPUT_ATTACK))
				{
					++it;
					continue;
				}
			}

			wobject = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed.Handle(), 0);
			Model wmodel = wobject.Model();
			gunBone = GET_ENTITY_BONE_INDEX_BY_NAME(wobject.Handle(), "Gun_Nuzzle");

			wobject.RequestControlOnce();

			if (!DOES_PARTICLE_FX_LOOPED_EXIST(fxHandle1))
			{
				USE_PARTICLE_FX_ASSET(fxAsset1.c_str());
				fxHandle1 = START_PARTICLE_FX_LOOPED_ON_ENTITY_BONE(fxName1.c_str(), wobject.Handle(), 0.0f, 0.0f, 0.04f, 89.5f, 0.0f, 90.0f, gunBone, scale, 0, 0, 0);
			}

			SET_PARTICLE_FX_LOOPED_EVOLUTION(fxHandle1, "flow", 1.0f, 0);
			SET_PARTICLE_FX_LOOPED_EVOLUTION(fxHandle1, "damage", 1.0f, 0);
			++it;
		}


	}

}



