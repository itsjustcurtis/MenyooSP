/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "PlayerOptions.h"

namespace sub
{
	void PlayerOptionsMenu()
	{
		dict2.clear();

		bool goToAlphaLevel = false;
		bool replenishPlayer = false;
		bool invincibilityOff = false;
		bool invisibilityOff = false;
		bool noRagdollOff = false;
		bool seatbeltOff = false;
		bool supermanOn = false;
		bool supermanAutoOn = false;
		bool ignoredByEveryoneOff = false;
		bool neverWantedOn = false;
		bool neverWantedOff = false;
		bool wantedPlus = false;
		bool wantedMinus = false;
		bool wantedFreezeOn = false;
		bool wantedFreezeOff = false;
		bool burnModeOn = false;
		bool burnModeOff = false;
		bool movementSpeedModifierPlus = false;
		bool movementSpeedModifierMinus = false;
		bool sweatPlus = false;
		bool sweatMinus = false;
		bool noiseValuePlus = false;
		bool noiseValueMinus = false;
		bool collisionOn = false;
		bool collisionOff = false;
		bool forceFieldPlus = false;
		bool forceFieldMinus = false;
		bool heightPlus = false;
		bool heightMinus = false;
		bool movementSpeedPlus = false;
		bool movementSpeedMinus = false;
		bool swimSpeedPlus = false;
		bool swimSpeedMinus = false;

		std::vector<std::string> forceFieldNames{ "Off", "Push Out", "Destroy" };


		float& height = g_playerVerticalElongationMultiplier;
		float movementSpeed = 0.0f, swimSpeed = 0.0f;

		bool butAmIOnline = NETWORK_IS_IN_SESSION() != 0;

		g_Ped1 = PLAYER_PED_ID();
		g_Ped2 = PLAYER_ID();

		GTAped myPed = g_Ped1;
		GTAplayer myPlayer = g_Ped2;

		int wantedLevel = myPlayer.GetWantedLevel();

		AddTitle("Player Options");
		AddOption("Basic Stats", null, nullFunc, SUB::SPSTATMANAGER);
		AddOption("Model Changer", null, nullFunc, SUB::MODELCHANGER);

		AddOption("Wardrobe", null, nullFunc, SUB::COMPONENTS);
		if (g_cam_componentChanger.Exists())
		{
			g_cam_componentChanger.SetActive(false);
			g_cam_componentChanger.Destroy();
			World::RenderingCamera_set(0);
		}
		
		AddOption("Animations", null, nullFunc, SUB::ANIMATIONSUB);
		AddOption("Scenario Actions", null, nullFunc, SUB::AnimationTaskScenarios);
		AddOption("Moods", null, nullFunc, SUB::FACIALMOOD);
		AddOption("Movement Styles", null, nullFunc, SUB::MOVEMENTGROUP);
		AddOption("Speech Player", null, nullFunc, SUB::SPEECHPLAYER);
		AddOption("Voice Changer", null, nullFunc, SUB::VOICECHANGER);
		AddOption("Ped Flags", null, nullFunc, SUB::PEDFLAGMANAGER_NAMEDLIST);
		AddOption("TriggerFX", null, nullFunc, SUB::PTFXSUB);
		AddOption("Breathe Stuff", null, nullFunc, SUB::BREATHESTUFF);
		AddOption("Ghost Rider Mode", null, nullFunc, SUB::GHOSTRIDERMODE);

		AddOption("Opacity (Local)", goToAlphaLevel, nullFunc, SUB::ENTITYALPHALEVEL);
		if (goToAlphaLevel)
		{
			g_Ped4 = myPed.Handle();
		}

		AddOption("Cloning Options", null, nullFunc, SUB::CLONECOMPANIONSUB);

		AddOption("Replenish Player", replenishPlayer);
		AddToggle("Refill Health When In Cover", selfRefillHealthInCover);
		AddToggle("Invincibility", playerInvincibility, null, invincibilityOff);
		AddLocal("Invisibility", !myPed.IsVisible(), invisibilityOff, invisibilityOff);
		AddToggle("No Ragdoll", playerNoRagdoll, null, noRagdollOff);
		AddToggle("Seatbelt", playerSeatbelt, null, seatbeltOff);
		AddToggle("Unlimited Special Ability (SP)", playerUnlimitedAbility);
		AddToggle("Auto-Clean", playerAutoClean);
		AddToggle("Super Run", superRun);
		AddToggle("Super Jump", superJump);
		AddToggle("Walk underwater", playerWalkUnderwater);
		AddTexter("Forcefield", forceField, forceFieldNames, null, forceFieldPlus, forceFieldMinus);
		AddLocal("Smash Ability", SmashAbility::g_smashAbility.Enabled(), SmashAbility::ToggleOnOff, SmashAbility::ToggleOnOff);
		AddToggle("Fly Manual", superman, supermanOn);
		AddToggle("Fly Auto", supermanAuto, supermanAutoOn);
		AddToggle("Ignored By Everyone", ignoredByEveryone, null, ignoredByEveryoneOff);
		AddNumber("Wanted Level", wantedLevel, 0, null, wantedPlus, wantedMinus);
		if (wantedLevel > 0)
		{
			AddLocal("Freeze Wanted Level", selfFreezeWantedLevel, wantedFreezeOn, wantedFreezeOff);
		}
		else
		{
			AddToggle("Never Wanted", neverWanted, neverWantedOn, neverWantedOff);
		}
		AddToggle("Burn Mode", playerBurn, burnModeOn, burnModeOff);
		AddNumber("Height (Elongation) - Experimental", height, 2, null, heightPlus, heightMinus);
		AddNumber("Movement Speed (Alt)", swimSpeedMult, 2, null, movementSpeedModifierPlus, movementSpeedModifierMinus);
		AddNumber("Sweat Level", selfSweatMult, 2, null, sweatPlus, sweatMinus);
		AddNumber("Noise Level", playerNoiseMult, 2, null, noiseValuePlus, noiseValueMinus);
		AddLocal("Collision", myPed.GetIsCollisionEnabled(), collisionOn, collisionOff);


		if (collisionOn || collisionOff)
		{
			myPed.SetIsCollisionEnabled(!myPed.GetIsCollisionEnabled());
		}

		if (replenishPlayer) 
		{
			addlog(ige::LogType::LOG_TRACE, "Replenishing Player");
			myPed.SetHealth(myPed.GetMaxHealth());
			myPed.Armour_set(myPlayer.MaxArmour_get());
			PedDamageTextures::ClearAllBloodDamage(myPed);
			PedDamageTextures::ClearAllVisibleDamage(myPed);
			return;
		}

		if (invincibilityOff) 
		{
			addlog(ige::LogType::LOG_TRACE, "Turning Off Invincibility");
			SET_PLAYER_INVINCIBLE(PLAYER_ID(), 0);
			SetPedInvincibleOff(PLAYER_PED_ID());
			return;
		}

		if (invisibilityOff)
		{
			myPed.SetVisible(!myPed.IsVisible());
		}

		if (noRagdollOff) 
		{ 
			SetPedNoRagdollOff(PLAYER_PED_ID()); 
			return; 
		}
		if (seatbeltOff) 
		{ 
			SetPedSeatbeltOff(PLAYER_PED_ID()); 
			return; 
		}

		if (forceFieldPlus) 
		{ 
			if (forceField < forceFieldNames.size() - 1) 
			{
				forceField++; 
				return; 
			}
		}
		if (forceFieldMinus) 
		{ 
			if (forceField > 0) 
			{
				forceField--; 
				return; 
			}
		}

		if (supermanOn) 
		{
			if (Menu::bitController) 
			{
				Game::Print::PrintBottomLeft("RT for Up. LT for Down. RB for Boost. A for Brake.");
			}
			else 
			{
				Game::Print::PrintBottomLeft("Num7 for Up. Num1 for Down. NumPlus for Boost. NumMinus for Brake.");
			}
			return;
		}

		if (supermanAutoOn) 
		{
			Vector3 Pos = GET_ENTITY_COORDS(g_Ped1, 1);
			CREATE_AMBIENT_PICKUP(PICKUP_PARACHUTE, Pos.x, Pos.y, Pos.z, 0, 300, 1, 0, 1);
			TASK_PARACHUTE(g_Ped1, true, false);
			APPLY_FORCE_TO_ENTITY(PLAYER_PED_ID(), 1, 0.0f, 0.0f, 10.0f, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
			if (Menu::bitController) 
			{
				Game::Print::PrintBottomLeft("Press ~b~A~s~ for temporary brake.");
			}
			else 
			{
				Game::Print::PrintBottomLeft("Press ~b~NUMPLUS~s~ for temporary brake.");
			}
			return;
		}

		if (ignoredByEveryoneOff) 
		{
			Player temp = PLAYER_ID();
			SET_POLICE_IGNORE_PLAYER(temp, neverWanted);
			SET_EVERYONE_IGNORE_PLAYER(temp, 0);
			SET_PLAYER_CAN_BE_HASSLED_BY_GANGS(temp, 1);
			SET_IGNORE_LOW_PRIORITY_SHOCKING_EVENTS(temp, 0);
			return;
		}

		if (neverWantedOn) 
		{
			SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, 0);
			SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 0);
			return;
		}
		if (neverWantedOff) 
		{
			SET_MAX_WANTED_LEVEL(6);
			SET_WANTED_LEVEL_MULTIPLIER(1.0f);
			return;
		}
		if (wantedPlus) 
		{
			if (wantedLevel < 6)
			{
				wantedLevel += 1;
				SET_MAX_WANTED_LEVEL(6);
				SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), wantedLevel, 0);
				SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 0);
				neverWanted = false;
			}
			if (selfFreezeWantedLevel != 0) 
			{
				selfFreezeWantedLevel = wantedLevel;
			}
			return;
		}
		if (wantedMinus) 
		{
			if (wantedLevel > 0)
			{
				if (wantedLevel == 1)
				{
					wantedLevel -= 1;
					SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), wantedLevel, 0);
				}
				else
				{
					wantedLevel -= 1;
					SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), wantedLevel, 0);
				}
				SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 0);
				neverWanted = false;
			}
			if (selfFreezeWantedLevel != 0) 
			{
				selfFreezeWantedLevel = wantedLevel;
			}
			return;
		}

		if (wantedFreezeOn)
		{
			selfFreezeWantedLevel = Game::Player().GetWantedLevel();
			return;
		}
		if (wantedFreezeOff) 
		{
			selfFreezeWantedLevel = 0;
			return;
		}

		if (burnModeOn) 
		{
			if (GET_PLAYER_INVINCIBLE(g_Ped2)) 
			{
				SET_PLAYER_INVINCIBLE(g_Ped2, 0);
			}
			SetPedInvincibleOff(g_Ped1);
			WAIT(130);
			if (!IS_ENTITY_ON_FIRE(g_Ped1)) 
			{
				START_ENTITY_FIRE(g_Ped1);
			}
			Game::Print::PrintBottomCentre("~b~Note:~s~ If you're not on fire yet, kill yourself.");
			return;
		}
		if (burnModeOff) 
		{
			if (IS_ENTITY_ON_FIRE(g_Ped1)) 
			{
				STOP_ENTITY_FIRE(g_Ped1);
			}
			return;
		}

		if (heightPlus) 
		{ 
			if (height < 2.5f) 
			{ 
				height += 0.1f; GeneralGlobalHax::SetPlayerHeight(height); 
			}
		}
		if (heightMinus) 
		{ 
			if (height > -2.5f) 
			{ 
				height -= 0.1f; GeneralGlobalHax::SetPlayerHeight(height); 
			} 
		}
		if (movementSpeedPlus) 
		{ 
			if (movementSpeed < 9.0f) 
			{ 
				movementSpeed += 0.1f; GeneralGlobalHax::SetPlayerMovementSpeed(movementSpeed); 
			} 
		}
		if (movementSpeedMinus) 
		{ 
			if (movementSpeed > 0.0f) 
			{ 
				movementSpeed -= 0.1f; GeneralGlobalHax::SetPlayerMovementSpeed(movementSpeed); 
			} 
		}
		if (swimSpeedPlus) 
		{ 
			if (swimSpeed < 9.0f) 
			{ 
				swimSpeed += 0.1f; GeneralGlobalHax::SetPlayerSwimSpeed(swimSpeed); 
			} 
		}
		if (swimSpeedMinus) 
		{ 
			if (swimSpeed > 0.0f) 
			{ 
				swimSpeed -= 0.1f; GeneralGlobalHax::SetPlayerSwimSpeed(swimSpeed); 
			} 
		}

		if (movementSpeedModifierPlus) 
		{
			if (swimSpeedMult < 1.40f) 
			{
				swimSpeedMult += 0.1f;
			}
			return;
		}
		if (movementSpeedModifierMinus) 
		{
			if (swimSpeedMult > 0.0f) 
			{
				swimSpeedMult -= 0.1f;
			}
			return;
		}

		if (sweatPlus) 
		{ 
			if (selfSweatMult < 5.5f) 
			{
				selfSweatMult += 0.1f;
			}
			return; 
		}
		if (sweatMinus) 
		{ 
			if (selfSweatMult > 0.0f) 
			{
				selfSweatMult -= 0.1f;
			}
			if (selfSweatMult == 0.0f) 
			{ 
				SET_PED_SWEAT(g_Ped1, selfSweatMult); 
				CLEAR_PED_WETNESS(g_Ped1); 
			} 
			return; 
		}
		if (sweatMinus) 
		{ 
			if (selfSweatMult > 0.0f) 
			{
				selfSweatMult -= 0.1f;
			}
			if (selfSweatMult == 0.0f) 
			{ 
				SET_PED_SWEAT(g_Ped1, selfSweatMult); 
				CLEAR_PED_WETNESS(g_Ped1); 
			} 
			return; 
		}
		if (noiseValuePlus) 
		{ 
			if (playerNoiseMult < 10.0f) 
			{
				playerNoiseMult += 0.1f;
			}
			return; 
		}
		if (noiseValueMinus) 
		{ 
			if (playerNoiseMult > 0.0f) 
			{
				playerNoiseMult -= 0.1f;
			}
			return; 
		}
	}

	namespace PedConfigFlagManager
	{
		std::vector<NamedPedFlagS> pedFlags
		{
			{ ePedConfigFlags::WillFlyThruWindscreen, "Can Fall Out Through Windscreen" },
			{ ePedConfigFlags::InVehicle, "Is (Nearby) Car (MAY CRASH)" },
			{ ePedConfigFlags::IsAimingGun, "Is Aiming Check" },
			{ ePedConfigFlags::ForcedAim, "Is Aiming" },
			{ ePedConfigFlags::_0x5FED6BFD, "Has Overflowing Diaper (MAY CRASH)" },
			{ ePedConfigFlags::IsInjured, "Is Injured" },
			{ ePedConfigFlags::HasHurtStarted, "Is Injured In Combat" },
			{ ePedConfigFlags::_Shrink, "Is Short Heighted (Small)" },
		};

		void FlagListMenu()
		{
			GTAped ped = g_Ped1;

			AddTitle("Ped Flags");
			AddOption("Custom", null, nullFunc, SUB::PEDFLAGMANAGER_CUSTOM);

			for (auto& f : pedFlags)
			{
				bool bFlagPressed = false;
				BOOL flagStatus = GET_PED_CONFIG_FLAG(ped.Handle(), f.id, true);
				AddLocal(f.title, flagStatus, bFlagPressed, bFlagPressed); if (bFlagPressed)
				{
					SET_PED_CONFIG_FLAG(ped.Handle(), f.id, !flagStatus);
				}
			}
		}

		int flagID = 0;
		void CustomFlagSetterMenu()
		{
			GTAped ped = g_Ped1;
			bool idInput = false;
			bool idPlus = false;
			bool idMinus = false; 
			bool idToggle = false;
			BOOL flagStatus = GET_PED_CONFIG_FLAG(ped.Handle(), flagID, true);

			AddTitle("Custom");
			AddNumber("ID", flagID, 0, idInput, idPlus, idMinus);
			AddLocal("Status", flagStatus, idToggle, idToggle);

			if (idPlus) 
			{ 
				if (flagID < INT_MAX) 
				{ 
					flagID++; 
				}
			}
			if (idMinus) 
			{ 
				if (flagID > 0) 
				{ 
					flagID--; 
				} 
			}
			if (idInput)
			{
				std::string inputStr = Game::InputBox("", 10U);
				if (inputStr.length() > 0)
				{
					try 
					{
						flagID = stoi(inputStr); 
					}
					catch (...) 
					{ 
						Game::Print::PrintErrorInvalidInput(inputStr);
						addlog(ige::LogType::LOG_ERROR, "Invalid flagID entered: " + inputStr);
					}
				}
			}

			if (idToggle)
			{
				SET_PED_CONFIG_FLAG(ped.Handle(), flagID, !flagStatus);
			}
		}
	}

	void CloneCompanionMenu()
	{
		GTAplayer player = g_Ped2;
		GTAped playerPed = g_Ped1;

		if (!playerPed.Exists())
		{
			Game::Print::PrintBottomCentre("~r~Error:~s~ No longer in memory.");
			addlog(ige::LogType::LOG_WARNING, "Cannot start clone menu, playerPed No longer in memory");
			Menu::SetPreviousMenu();
			return;
		}

		AddTitle("Cloning");

		bool cloneNeutralPressed = false;
		AddOption("Clone As Neutral", cloneNeutralPressed); 
		if (cloneNeutralPressed)
		{
			GTAped clone = playerPed.Clone(playerPed.GetHeading(), true, true);
			auto cloneNetId = clone.NetID();
			Game::RequestControlOfId(cloneNetId);
			SET_NETWORK_ID_CAN_MIGRATE(cloneNetId, true);
			SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(cloneNetId, true);
			clone.BlockPermanentEvents_set(true);
			SET_PED_COMBAT_ABILITY(cloneNetId, 100);
			clone.SetMissionEntity(true);
			clone.NoLongerNeeded();

			Game::Print::PrintBottomLeft(oss_ << "Cloned ~b~" << player.GetName());
		}

		bool cloneCompanionPressed = false;
		AddOption("Clone As Companion (7 Max)", cloneCompanionPressed); 
		if (cloneCompanionPressed)
		{
			GTAped clone = playerPed.Clone(playerPed.GetHeading(), true, true);
			clone.RequestControl(300);
			auto cloneNetId = clone.NetID();
			Game::RequestControlOfId(cloneNetId);
			SET_NETWORK_ID_CAN_MIGRATE(cloneNetId, true);
			SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(cloneNetId, true);
			SET_PED_COMBAT_ABILITY(cloneNetId, 200);
			Hash weaponToGive = WEAPON_APPISTOL;
			GIVE_DELAYED_WEAPON_TO_PED(clone.Handle(), weaponToGive, 9999, true);
			clone.SetWeapon(weaponToGive);
			clone.FiringPattern_set(FiringPattern::FullAuto);
			clone.SetShootRate(100);
			sub::Spooner::SpoonerEntity cloneEnt;
			cloneEnt.dynamic = true;
			cloneEnt.handle = clone;
			cloneEnt.hashName = player.GetName() + "\'s Clone Companion";
			cloneEnt.isStill = false;
			cloneEnt.type = EntityType::PED;
			sub::Spooner::EntityManagement::AddEntityToDb(cloneEnt);

			PedGroup grp;
			if (playerPed.IsInGroup())
			{
				grp = playerPed.CurrentPedGroup_get();
			}
			else
			{
				grp = PedGroup::CreateNewGroup();
				grp.Add(playerPed, true);
			}
			grp.Add(clone, false);
			grp.SetSeparationRange(100.0f);
			grp.SetFormationSpacing(1.5f);

			Game::Print::PrintBottomLeft(oss_ << "Cloned ~b~" << player.GetName() << "~s~ and made the clone " << (playerPed.Gender_get() == Gender::Female ? "her" : "his") << " companion.");
			Game::Print::PrintBottomLeft("Clone added to Spooner Database as a persistent entity.");
		}

		bool cloneEnemyPressed = false;
		AddOption("Clone As Enemy", cloneEnemyPressed); 
		if (cloneEnemyPressed)
		{
			GTAped clone = playerPed.Clone(playerPed.GetHeading(), true, true);
			clone.RequestControl(300);
			auto cloneNetId = clone.NetID();
			Game::RequestControlOfId(cloneNetId);
			SET_NETWORK_ID_CAN_MIGRATE(cloneNetId, true);
			SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(cloneNetId, true);
			clone.BlockPermanentEvents_set(true);
			SET_PED_COMBAT_ABILITY(cloneNetId, 100);
			Hash weaponToGive = WEAPON_APPISTOL;
			GIVE_DELAYED_WEAPON_TO_PED(clone.Handle(), weaponToGive, 9999, true);
			clone.SetWeapon(weaponToGive);
			clone.FiringPattern_set(FiringPattern::FullAuto);
			clone.SetShootRate(100);
			sub::Spooner::SpoonerEntity cloneEnt;
			cloneEnt.dynamic = true;
			cloneEnt.handle = clone;
			cloneEnt.hashName = player.GetName() + "\'s Clone Enemy";
			cloneEnt.isStill = false;
			cloneEnt.type = EntityType::PED;
			sub::Spooner::EntityManagement::AddEntityToDb(cloneEnt);
			TASK_COMBAT_PED(clone.Handle(), playerPed.Handle(), 0, 16);

			TaskSequence squ;
			TASK_COMBAT_PED_TIMED(0, playerPed.Handle(), 0, 10000);
			TASK_SHOOT_AT_ENTITY(0, playerPed.Handle(), 10000, FiringPattern::FullAuto);
			TASK_PUT_PED_DIRECTLY_INTO_MELEE(0, playerPed.Handle(), 0.0f, -1.0f, 0.0f, 0);
			TASK_COMBAT_HATED_TARGETS_AROUND_PED(0, 25.0f, 0);
			squ.Close(true); // Repeat task sequence if it ends.
			squ.MakePedPerform(clone);
			clone.SetAlwaysKeepTask(true); // May ruin the task sequence. Not sure.
			squ.Clear();

			Game::Print::PrintBottomLeft(oss_ << "Cloned ~b~" << player.GetName() << "~s~ and made the clone " << (playerPed.Gender_get() == Gender::Female ? "her" : "his") << " enemy.");
			Game::Print::PrintBottomLeft("Clone added to Spooner Database as a persistent entity.");
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(CLONECOMPANIONSUB,   			sub::CloneCompanionMenu)
REGISTER_SUBMENU(PLAYEROPS,           			sub::PlayerOptionsMenu)
REGISTER_SUBMENU(PEDFLAGMANAGER_NAMEDLIST, 		sub::PedConfigFlagManager::FlagListMenu)
REGISTER_SUBMENU(PEDFLAGMANAGER_CUSTOM,    		sub::PedConfigFlagManager::CustomFlagSetterMenu)