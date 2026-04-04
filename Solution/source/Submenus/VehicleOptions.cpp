/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "VehicleOptions.h"

namespace sub
{
	// Vehicle - ped task
	void TaskRappel(GTAped ped, GTAvehicle vehicle)
	{
		ped.RequestControl();
		ped.Task().ClearAll();
		ped.SetIntoVehicle(vehicle, VehicleSeat::SEAT_LEFTREAR);

		GTAped newPed;
		if (vehicle.IsSeatFree(VehicleSeat::SEAT_DRIVER))
		{
			newPed = World::CreatePedInsideVehicle(PedHash::PrologueDriver, vehicle, VehicleSeat::SEAT_DRIVER);
		}

		WAIT(100);

		if (newPed.Exists())
		{
			const Vector3& vehPos = vehicle.GetPosition();
			newPed.BlockPermanentEvents_set(true);
			TASK_HELI_MISSION(newPed.Handle(), vehicle.Handle(), 0, 0, vehPos.x, vehPos.y, vehPos.z, 4, 0.0f, 50.0f, -1.0f, 10000, 100, -1082130432, 0);
			newPed.SetAlwaysKeepTask(true);
		}

		vehicle.Velocity_set(Vector3::Zero());

		ped.RequestControl();
		TASK_RAPPEL_FROM_HELI(ped.Handle(), 1); // 1092616192 0x41200000

		WAIT(100);
		if (newPed.Exists())
		{
			newPed.NoLongerNeeded(); // Too soon, maybe?
		}
	}

	void VehicleMenu()
	{
		bool setVehiclePed = false;
		bool fixCar = false;
		bool toggleCargobobMagnet = false;
		bool toggleBoatAnchor = false;
		bool goToSlamItMenu = false;
		bool funnyVehicleFreezeOff = false;
		bool disableSirenOff = false;
		bool vehicleMenuModShop = false;
		bool vehicleMenuAlphaLevel = false;
		bool rappelHeli = false;
		bool teleportClosestCar = false;
		bool damageAndDefensePlus = false;
		bool damageAndDefenseMinus = false; 
		bool damageAndDefenseCustom = false;
		bool invincibilityOff = false;
		bool invisible = false;
		bool slippyTires = false;
		bool vehicleMenuEngine = false;
		bool interiorLight = false;
		bool multiplyLights = false;
		bool boostOn = false;
		bool setOnFire = false;
		bool superCarModeOn = false;
		bool superCarModeOff = false;
		bool carJumpPlus = false;
		bool carJumpMinus = false;
		bool hydraulicsOn = false;
		bool driveOnWaterOn = false;
		bool driveOnWaterOff = false;
		bool childLocksOn = false;
		bool childLocksOff = false;
		bool doorLocksOn = false;
		bool doorLocksOff = false;
		bool noGravityOn = false;
		bool noGravityOff = false;
		bool speedoOn = false;
		bool collisionsOn = false;
		bool collisionsOff = false;
		bool freezeCarOn = false;
		bool freezeCarOff = false;
		bool deleteCar = false;
		bool slamOn = false;
		bool heavyMassOff = false;

		g_Ped1 = PLAYER_PED_ID();
		g_Ped2 = PLAYER_ID();

		bool amIOnline = NETWORK_IS_IN_SESSION() != 0;
		GTAped myPed = g_Ped1;
		GTAplayer myPlayer = g_Ped2;
		GTAvehicle myVehicle = g_myVeh;
		bool myPedIsInVehicle = myPed.IsInVehicle();
		const Model& myVehicleModel = myVehicle.Model();

		static int fixCarTexterValue = 0;
		static std::vector<std::string> fixCarTexter{ "Full", "Keep Dirt", "Keep windows open", "Keep windows open with Dirt"};
		auto& fixCarTexterVal = fixCarTexterValue;
		bool fixCarPlus = false;
		bool fixCarMinus = false;

		AddTitle("Vehicle Options");
		if (myPedIsInVehicle)
		{
			AddTexter("CMOD_MOD_MNT", fixCarTexterVal, fixCarTexter, fixCar, fixCarPlus, fixCarMinus, true); // Fix & Wash
		}
		AddOption("Vehicle Spawner", funnyVehicleFreezeOff, nullFunc, SUB::SPAWNVEHICLE);
		AddOption("Menyoo Customs", vehicleMenuModShop, nullFunc, -1, true);
		if (myVehicleModel.IsCargobob())
		{
			AddLocal("Cargobob Magnet", myVehicle.IsCargobobHookActive(CargobobHook::Magnet), toggleCargobobMagnet, toggleCargobobMagnet);
		}
		if (DOES_VEHICLE_ALLOW_RAPPEL(g_myVeh))
		{
			AddOption("Rappel From Helicopter", rappelHeli);
		}
		if (myVehicle.GetHasSiren())
		{
			AddToggle("Disable Vehicle Siren", vehicleDisableSiren, null, disableSirenOff);
		}
		if (myVehicleModel.IsBoat())
		{
			AddLocal("Anchor Boat", myVehicle.IsBoatAnchored(), toggleBoatAnchor, toggleBoatAnchor);
		}
		AddOption("Teleport Into Closest Vehicle", teleportClosestCar);
		AddOption("Vehicle Weapons", null, nullFunc, SUB::VEHICLEWEAPONS);
		AddOption("Vehicle Multipliers", null, nullFunc, SUB::VEHICLEMULTIPLIERS);
		AddOption("Speedometers", null, nullFunc, SUB::VEHICLESPEEDOS);
		AddOption("Multi-Platform Neons", null, nullFunc, SUB::MULTIPLATNEONS);
		AddOption("PV Options", null, nullFunc, SUB::PVOPS);
		AddOption("Auto Drive", null, nullFunc, SUB::AUTODRIVESUB);
		AddOption((std::string)"Slam It (" + Game::GetGXTEntry("CMOD_MOD_22_D") + ")", goToSlamItMenu, nullFunc, SUB::VEHICLESLAM);
		AddOption("Opacity (Local)", vehicleMenuAlphaLevel, nullFunc, -1, true);
		AddNumber("Damage & Defense", vehicleDamageAndDefense, 2, damageAndDefenseCustom, damageAndDefensePlus, damageAndDefenseMinus);
		AddToggle("Invincibility (Looped)", vehicleInvincibility, null, invincibilityOff);
		AddLocal("Invisibility", !myVehicle.IsVisible(), vehicleInvisibility, invisible);
		AddToggle("Auto-Repair", vehicleFixLoop);
		AddToggle("Auto-Flip", vehicleFlipLoop);
		AddToggle("Keep Engine & Lights On", selfEngineOn);
		AddTickol("Kill Engine", (GET_VEHICLE_ENGINE_HEALTH(g_myVeh) < 0.0f), vehicleMenuEngine, vehicleMenuEngine, TICKOL::PERCENTAGESTICKER, TICKOL::NONE);
		AddToggle("Rainbow Mode", carColorChange);
		AddLocal("Slidy Tyres", bitVehicleSlippyTires, slippyTires, slippyTires);
		AddLocal("Cruise Control", VehicleCruise::g_vehicleCruise.Enabled(), VehicleCruise::ToggleOnOff, VehicleCruise::ToggleOnOff);
		AddLocal("Tow Mode (ALPHA)", VehicleTow::g_vehicleTow.Enabled(), VehicleTow::ToggleOnOff, VehicleTow::ToggleOnOff);
		AddToggle("Race Boost On Horn", raceBoost, boostOn);
		AddToggle("Infinite Native Boost (for e.g. Oppressor)", unlimitedVehicleBoost);
		AddToggle("SuprKar Mode", superCarMode, superCarModeOn, superCarModeOff);
		AddLocal("Fly Mode", VehicleFly::g_vehicleFly.Enabled(), VehicleFly::ToggleOnOff, VehicleFly::ToggleOnOff);
		AddToggle("Glue to Ground", superGrip);
		AddTexter("Vehicle Jump", carJump, Menu::bitController ? std::vector<std::string>{"Off", "Tap/Press A/X", "Hold A/X"} : std::vector<std::string>{ "Off", "Tap/Press Space", "Hold Space" }, null, carJumpPlus, carJumpMinus);
		AddToggle("Hydraulics", carHydraulics, hydraulicsOn);
		AddToggle("Drive On Water", driveOnWater, driveOnWaterOn, driveOnWaterOff);
		AddToggle("Increased Mass", vehicleHeavyMass, null, heavyMassOff);
		AddTickol("Child Locks", (GET_VEHICLE_DOOR_LOCK_STATUS(g_myVeh) == 4), childLocksOn, childLocksOff, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddTickol("Door Locks", (GET_VEHICLE_DOOR_LOCK_STATUS(g_myVeh) == 2), doorLocksOn, doorLocksOff, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddLocal("No Gravity", bitVehicleGravity, noGravityOn, noGravityOff);
		AddLocal("Freeze Vehicle", bitFreezeVehicle, freezeCarOn, freezeCarOff);
		AddLocal("Set on Fire", myVehicle.IsOnFire(), setOnFire, setOnFire);
		AddLocal("Collision", myVehicle.GetIsCollisionEnabled(), collisionsOn, collisionsOff);
		AddOption("Delete Vehicle", deleteCar);

		if (toggleCargobobMagnet) 
		{
			if (myVehicle.IsCargobobHookActive(CargobobHook::Magnet))
			{
				myVehicle.RequestControl();
				myVehicle.RetractCargobobHook();
				WAIT(50);
				myVehicle.DropCargobobHook(CargobobHook::Hook);
			}
			else
			{
				myVehicle.RequestControl();
				myVehicle.DropCargobobHook(CargobobHook::Magnet);
				myVehicle.CargoBobMagnetGrabVehicle();
			}
		}

		if (toggleBoatAnchor)
		{
			myVehicle.AnchorBoat(!myVehicle.IsBoatAnchored());
		}
		
		if (disableSirenOff) 
		{
			SET_VEHICLE_HAS_MUTED_SIRENS(g_myVeh, FALSE);
		}

		if (slamOn) 
		{ 
			Game::Print::PrintBottomCentre("~b~Note:~s~ If you try hard enough, you can drive on walls too!"); 
			return; 
		}

		if (collisionsOn || collisionsOff) 
		{
			myVehicle.SetIsCollisionEnabled(!myVehicle.GetIsCollisionEnabled());
		}

		if (setVehiclePed) 
		{
			g_Ped4 = g_myVeh;
		}
		if (goToSlamItMenu) 
		{
			sub::VehicleSlam::InitSub(g_myVeh, &vehicleSlam);
		}

		if (vehicleMenuModShop) 
		{
			if (DOES_ENTITY_EXIST(g_myVeh)) 
			{ 
				g_Ped4 = g_myVeh; Menu::NewSetMenu(SUB::MODSHOP); 
			}
			else 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			}
			return;
		}
		if (vehicleMenuAlphaLevel) 
		{
			if (DOES_ENTITY_EXIST(g_myVeh)) 
			{ 
				g_Ped4 = g_myVeh; Menu::NewSetMenu(SUB::ENTITYALPHALEVEL); 
			}
			else 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			}
			return;
		}

		if (rappelHeli) {
			if (myPed.IsInVehicle())
			{
				TaskRappel(myPed.Handle(), myPed.CurrentVehicle());
				return;
			}
		}

		if (teleportClosestCar) 
		{
			const GTAvehicle& tempVehicle = World::GetClosestVehicle(myPed.GetPosition(), FLT_MAX);
			if (tempVehicle.Exists())
			{
				myPed.SetIntoVehicle(tempVehicle, tempVehicle.FirstFreeSeat(SEAT_DRIVER));
			}
			return;
		}

		if (fixCarPlus && fixCarTexterVal < fixCarTexter.size() - 1)
		{
			fixCarTexterVal++;
		}
		else if (fixCarMinus && fixCarTexterVal > 0)
		{
			fixCarTexterVal--;
		}
		if (fixCar) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			}
			else
			{
				std::vector<VehicleWindow> windowsToOpen;
				if (fixCarTexterVal == 1 || fixCarTexterVal == 3)
				{
					for (int i = (int)VehicleWindow::FrontLeftWindow; i < (int)VehicleWindow::Last; i++)
					{
						if (!myVehicle.IsWindowIntact((VehicleWindow)i))
						{
							windowsToOpen.push_back((VehicleWindow)i);
						}
					}
				}


				myVehicle.RequestControlOnce();
				SET_VEHICLE_FIXED(g_myVeh);
				if(fixCarTexterVal == 0 || fixCarTexterVal == 2)
				{
					SET_VEHICLE_DIRT_LEVEL(g_myVeh, 0.0f);
				}
				SET_VEHICLE_ENGINE_CAN_DEGRADE(g_myVeh, 0);
				SET_VEHICLE_ENGINE_HEALTH(g_myVeh, 1250.0f);
				SET_VEHICLE_PETROL_TANK_HEALTH(g_myVeh, 1250.0f);
				SET_VEHICLE_BODY_HEALTH(g_myVeh, 1250.0f);
				SET_VEHICLE_UNDRIVEABLE(g_myVeh, 0);
				if (!GET_IS_VEHICLE_ENGINE_RUNNING(g_myVeh))
				{
					SET_VEHICLE_ENGINE_ON(g_myVeh, 1, 1, 0);
				}

				if (fixCarTexterVal == 1 || fixCarTexterVal == 3)
				{
					for (auto& i : windowsToOpen)
					{
						myVehicle.RollDownWindow(i);
					}
				}
			}
			return;
		}

		if (damageAndDefensePlus) 
		{
			vehicleDamageAndDefense += 0.2f;
			SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
			SET_PLAYER_VEHICLE_DEFENSE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
			return;
		}
		if (damageAndDefenseMinus) 
		{
			vehicleDamageAndDefense -= 0.2f;
			SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
			SET_PLAYER_VEHICLE_DEFENSE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
			return;
		}
		if (damageAndDefenseCustom) 
		{
			std::string inputStr = Game::InputBox("", 10U);
			if (inputStr.length() > 0)
			{
				float oldVal = vehicleDamageAndDefense;
				bool noexc = true;
				try 
				{ 
					vehicleDamageAndDefense = stof(inputStr); 
				}
				catch (...)
				{ 
					noexc = false; 
					vehicleDamageAndDefense = oldVal; 
				}
				if (noexc)
				{
					SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
					SET_PLAYER_VEHICLE_DEFENSE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
				}
			}
			return;
		}

		if (invincibilityOff) 
		{
			if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0))
			{
				myVehicle.RequestControlOnce();
				SetVehicleInvincibleOff(g_myVeh);
			}
			return;
		}

		if (invisible) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				myVehicle.RequestControlOnce();
				myVehicle.SetVisible(true);
			}
			return;
		}

		if (slippyTires) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				bitVehicleSlippyTires = !bitVehicleSlippyTires;
				myVehicle.RequestControlOnce();
				SET_VEHICLE_REDUCE_GRIP(g_myVeh, bitVehicleSlippyTires);
			}
			return;
		}

		if (vehicleMenuEngine) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				if (GET_VEHICLE_ENGINE_HEALTH(g_myVeh) > 0.0f)
				{
					myVehicle.RequestControlOnce();
					SET_VEHICLE_ENGINE_HEALTH(g_myVeh, -1.0f);
					SET_VEHICLE_UNDRIVEABLE(g_myVeh, 1);
					Game::Print::PrintBottomCentre("Engine Killed");
				}
				else
				{
					myVehicle.RequestControlOnce();
					SET_VEHICLE_ENGINE_HEALTH(g_myVeh, 1250.0f);
					SET_VEHICLE_UNDRIVEABLE(g_myVeh, 0);
					Game::Print::PrintBottomCentre("Engine Revived");
				}
			}
			return;
		}

		if (boostOn) 
		{
			Game::Print::PrintBottomLeft("Use the horn for a boost.");
			return;
		}

		if (superCarModeOn) 
		{
			Game::Print::PrintBottomLeft("Use driving controls and handbrake.");
			return;
		}
		if (superCarModeOff) 
		{
			SET_VEHICLE_BOOST_ACTIVE(g_myVeh, 0);
			return;
		}

		if (carJumpPlus) 
		{ 
			if (carJump < 2)
			carJump++; 
			return; 
		}
		if (carJumpMinus) 
		{ 
			if (carJump > 0)
			carJump--; 
			return; 
		}

		if (hydraulicsOn) 
		{
			Game::Print::PrintBottomLeft(oss_ "Use ~b~" << (Menu::bitController ? "LS/L1 + stick movement" : "LeftShift + WASD") << "~s~ for hydraulics.");
			return;
		}

		if (driveOnWaterOn) 
		{
			Game::Print::PrintBottomLeft("You can drive and walk on water now!");
			return;
		}
		if (driveOnWaterOff) 
		{
			GTAprop(g_driveWaterObject).Delete(true);
			return;
		}

		if (childLocksOn) 
		{
			myVehicle.RequestControlOnce();
			SET_VEHICLE_DOORS_LOCKED(g_myVeh, 4);
			return;
		}
		if (childLocksOff) 
		{
			myVehicle.RequestControlOnce();
			SET_VEHICLE_DOORS_LOCKED(g_myVeh, 0);
			return;
		}
		if (doorLocksOn) 
		{
			myVehicle.RequestControlOnce();
			SET_VEHICLE_DOORS_LOCKED(g_myVeh, 2);
			return;
		}
		if (doorLocksOff) 
		{
			myVehicle.RequestControlOnce();
			SET_VEHICLE_DOORS_LOCKED(g_myVeh, 0);
			return;
		}

		if (noGravityOn) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				myVehicle.RequestControlOnce();
				SET_VEHICLE_GRAVITY(g_myVeh, 0);
				bitVehicleGravity = true;
				Game::Print::PrintBottomCentre("Vehicle Gravity ~g~Disabled");
			}
			return;
		}
		if (noGravityOff) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				myVehicle.RequestControlOnce();
				SET_VEHICLE_GRAVITY(g_myVeh, 1);
				bitVehicleGravity = false;
				Game::Print::PrintBottomCentre("Vehicle Gravity ~r~Enabled");
			}
			return;
		}

		if (freezeCarOn) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				bitFreezeVehicle = true;
				myVehicle.RequestControlOnce();
				SET_VEHICLE_FORWARD_SPEED(g_myVeh, 0.0f);
				FREEZE_ENTITY_POSITION(g_myVeh, 1);
				Game::Print::PrintBottomCentre("Vehicle Freeze ~g~On");
			}
			return;
		}

		if (freezeCarOff) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				bitFreezeVehicle = false;
				myVehicle.RequestControlOnce();
				SET_VEHICLE_FORWARD_SPEED(g_myVeh, 8.0f);
				FREEZE_ENTITY_POSITION(g_myVeh, 0);
				Game::Print::PrintBottomCentre("Vehicle Freeze ~r~Off");
			}
			return;
		}

		if (setOnFire) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			}
			else
			{
				if (!myVehicle.IsOnFire())
				{
					myVehicle.RequestControl(200);
					myVehicle.SetProofs(true, false, true, false, false, false, false, true);
					myVehicle.SetFireProof(false);
					WAIT(40);
					myVehicle.SetOnFire(true);
				}
				else
				{
					myVehicle.RequestControl(200);
					myVehicle.SetProofs(true, true, true, false, false, false, false, true);
					myVehicle.SetFireProof(true);
					WAIT(40);
					myVehicle.SetOnFire(false);
				}
			}
			return;
		}

		if (deleteCar) 
		{
			if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0)) Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			else
			{
				myVehicle.Delete(false);
				Game::Print::PrintBottomCentre("Poof!");
				*Menu::currentopATM = 1;
			}
			return;
		}


		if (heavyMassOff) 
		{
			if (myVehicle.Exists())
			{
				SetVehicleInvincibleOff(myVehicle.GetHandle());
				myVehicle.SetFrictionOverride(1.0f);
			}
			return;
		}


	}

	void PVMenu()
	{
		bool pvSave = false;
		bool pvTeleportIntoIt = false;
		bool pvTeleportToIt = false;
		bool pvTeleportItToYou = false;
		bool pvUnsave = false;

		GTAped myPed = PLAYER_PED_ID();
		GTAvehicle myVehicle = g_myVeh;
		GTAvehicle& pv = pvSubVehicleID;

		AddTitle("PV Options");
		AddTickol("Remember Vehicle", pv == myVehicle, pvSave, pvUnsave);
		AddOption("Teleport Into Seat", pvTeleportIntoIt);
		AddOption("Teleport to Vehicle", pvTeleportToIt);
		AddOption("Teleport Vehicle to Self", pvTeleportItToYou);

		if (pvUnsave)
		{
			pv.SetMissionEntity(false);
			GTAblip blip = pv.CurrentBlip();
			if (blip.Exists())
			{
				if (blip.Colour() == 61)
				{
					blip.Remove();
				}
			}
			pv = 0;
			return;
		}

		if (pvSave) 
		{
			if (!myPed.IsInVehicle()) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ You are not in a vehicle.");
			}
			else
			{
				pv = myVehicle;
				pv.SetMissionEntity(true);

				GTAblip blip = pv.CurrentBlip();
				if (!blip.Exists())
				{
					blip = pv.AddBlip();
					if (blip.Exists())
					{
						Model model = pv.Model();
						BlipIcon::BlipIcon sprite = BlipIcon::PersonalVehicleCar;

						if (model.IsHeli())
						{
							sprite = BlipIcon::EnemyHelicopter;
						}
						else if (model.IsPlane())
						{
							sprite = BlipIcon::Plane;
						}
						else if (model.IsBoat())
						{
							sprite = BlipIcon::Boat;
						}
						else if (model.IsBike())
						{
							sprite = BlipIcon::PersonalVehicleBike;
						}
						else if (model.hash == VEHICLE_RHINO)
						{
							sprite = BlipIcon::Tank; // Rhino
						}

						blip.SetIcon(sprite);
						blip.SetScale(0.7f);
						blip.SetColour(BlipColour::Pink);
						blip.SetFriendly(true);
					}
				}
			}
			return;
		}

		if (pvTeleportIntoIt) 
		{
			if (!pv.Exists()) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ No longer in memory.");
			}
			else
			{
				myPed.SetIntoVehicle(pv, pv.FirstFreeSeat(SEAT_DRIVER));
			}
			return;
		}

		if (pvTeleportToIt)
		{
			if (!pv.Exists()) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ No longer in memory.");
			}
			else
			{
				if (myPed.IsInVehicle())
				{
					myVehicle.SetPosition(pv.GetOffsetInWorldCoords(0, pv.Dim1().y + myVehicle.Dim2().y + 0.3f, 0));
				}
				else
				{
					myPed.SetPosition(pv.GetOffsetInWorldCoords(0, 0, pv.Dim2().z + myPed.Dim1().z));
				}
			}
			return;
		}

		if (pvTeleportItToYou) 
		{
			if (!pv.Exists()) 
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ No longer in memory.");
			}
			else
			{
				const Vector3& myPos = myPed.GetPosition();
				pv.RequestControl(600);
				pv.SetPosition(myPos);
			}
			return;
		}

	}

	namespace VehicleAutoDrive
	{
		class MethodsClass final : public GenericLoopedMode
		{
		private:
			GTAped myPed;
			GTAvehicle vehicle;
			Model vehicleModel;
			Vector3 destination;
		public:
			float speed = 20;
			int drivingStyle = 5;
			UINT8 drivingStyleIndex = 0;
			bool bPushEmAway = true;
			bool initialSet = false;

			void TurnOn() override
			{
				GenericLoopedMode::TurnOn();

				initialSet = false;
			}
			void TurnOff() override
			{
				GenericLoopedMode::TurnOff();

				initialSet = false;

				CLEAR_PED_TASKS(myPed.Handle());
			
			}

			void Tick() override
			{
				if (bEnabled)
				{
					MainTick();
				}
			}
			inline void MainTick()
			{
				myPed = PLAYER_PED_ID();

				if (myPed.IsInVehicle() && IS_WAYPOINT_ACTIVE())
				{
					if (!initialSet)
					{
						destination = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint));
					}

					vehicle = myPed.CurrentVehicle();
					vehicleModel = vehicle.Model();

					if (vehicleModel.IsHeli())
					{
						HeliTick();
					}
					else if (vehicleModel.IsPlane())
					{
						PlaneTick();
					}
					else if (vehicleModel.IsBoat())
					{
						BoatTick();
					}
					else
					{
						NormalTick();
					}

					if (bPushEmAway)
					{
						PushEmAway();
					}
				}
				else
				{
					TurnOff();
				}

			}

			inline void NormalTick()
			{
				if (!initialSet)
				{
					ScrHandle tsk;
					OPEN_SEQUENCE_TASK(&tsk);

					TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(0, vehicle.Handle(), destination.x, destination.y, destination.z, speed, drivingStyle, 7.0f);

					CLOSE_SEQUENCE_TASK(tsk);
					TASK_PERFORM_SEQUENCE(myPed.Handle(), tsk);
					CLEAR_SEQUENCE_TASK(&tsk);

					initialSet = true;
				}
			}

			inline void HeliTick()
			{
				if (!initialSet)
				{
					ScrHandle tsk;
					OPEN_SEQUENCE_TASK(&tsk);

					TASK_HELI_MISSION(0, vehicle.Handle(), 0, 0, destination.x, destination.y, destination.z, 4, speed, 50.0f, -1.0f, 100, 200, 0xbf800000, 0);

					CLOSE_SEQUENCE_TASK(tsk);
					TASK_PERFORM_SEQUENCE(myPed.Handle(), tsk);
					CLEAR_SEQUENCE_TASK(&tsk);

					initialSet = true;
				}


			}

			inline void PlaneTick()
			{
				if (!initialSet)
				{
					ScrHandle tsk;
					OPEN_SEQUENCE_TASK(&tsk);

					TASK_PLANE_MISSION(0, vehicle.Handle(), 0, 0, destination.x, destination.y, destination.z, 4, speed, 50.0f, -1.0f, 100.0f, 200.0f, false);

					CLOSE_SEQUENCE_TASK(tsk);
					TASK_PERFORM_SEQUENCE(myPed.Handle(), tsk);
					CLEAR_SEQUENCE_TASK(&tsk);

					initialSet = true;
				}

			}

			inline void BoatTick()
			{
				if (!initialSet)
				{
					ScrHandle tsk;
					OPEN_SEQUENCE_TASK(&tsk);

					TASK_BOAT_MISSION(0, vehicle.Handle(), 0, 0, destination.x, destination.y, destination.z, 4, speed, 786469, 10.0f, 1071);

					CLOSE_SEQUENCE_TASK(tsk);
					TASK_PERFORM_SEQUENCE(myPed.Handle(), tsk);
					CLEAR_SEQUENCE_TASK(&tsk);

					initialSet = true;
				}

			}

			inline void PushEmAway()
			{
				const auto& md = vehicleModel.Dimensions();
				const auto& pos = vehicle.GetPosition();
				const auto& rot = vehicle.Rotation_get();
				const auto& dir = Vector3::RotationToDirection(rot);

				auto ray = RaycastResult::RaycastCapsule(pos, dir, 3.2f + md.Dim1.y, 2.3f, IntersectOptions::Everything, vehicle);

				if (ray.DidHitEntity())
				{
					GTAentity thingInFront = ray.HitEntity();
					thingInFront.ApplyForce(dir * 10.0f);
				}

				const Vector3& myFrontBumper = pos + (dir * md.Dim1.y);

				for (GTAvehicle v : nearbyVehicles)
				{
					if (v.Handle() == vehicle.Handle()) 
					{
						continue;
					}
					if (v.IsInRangeOf(myFrontBumper, 4.0f))
					{
						v.ApplyForce(dir * 10.0f);
					}
				}
			}
		};
		MethodsClass Methods;

		void ToggleOnOff()
		{
			Methods.Toggle();
		}
		void Tick()
		{
			Methods.Tick();
		}

		void AutoDriveMenu()
		{
			GTAped myPed = Game::PlayerPed();

			auto& speed = Methods.speed;
			auto& drivingStyle = Methods.drivingStyle;
			auto& drivingStyleIndex = Methods.drivingStyleIndex;
			auto& bPushEmAway = Methods.bPushEmAway;

			std::vector<std::string> drivingStyleNames;
			for (auto& dsns : DrivingStyle::nameArray)
			{
				drivingStyleNames.push_back(dsns.name);
			}

			bool speedPlus = false;
			bool speedMinus = false;
			bool drivingStylePlus = false;
			bool drivingStyleMinus = false;

			AddTitle("Auto Drive");

			AddLocal("Go To Waypoint", Methods.Enabled(), ToggleOnOff, ToggleOnOff);
			AddNumber("Speed (KMPH)", speed * 3.6f, 1, null, speedPlus, speedMinus);
			AddTexter("Driving Style", drivingStyleIndex, drivingStyleNames, null, drivingStylePlus, drivingStyleMinus);
			AddToggle("Push Other Vehicles Away", bPushEmAway);

			if (speedPlus) 
			{
				if (speed < FLT_MAX) 
				{
					speed += (0.5f / 3.6f);
				}
				myPed.SetDrivingSpeed(speed);
				return;
			}
			if (speedMinus) 
			{
				if (speed > 0) 
				{
					speed -= (0.5f / 3.6f);
				}
				myPed.SetDrivingSpeed(speed);
				return;
			}

			if (drivingStylePlus) 
			{
				if (drivingStyleIndex < drivingStyleNames.size() - 1) 
				{
					drivingStyleIndex++;
				}
				myPed.SetDrivingStyle(DrivingStyle::nameArray[drivingStyleIndex].style);
				return;
			}
			if (drivingStyleMinus) 
			{
				if (drivingStyleIndex > 0)
				{
					drivingStyleIndex--;
				}
				myPed.SetDrivingStyle(DrivingStyle::nameArray[drivingStyleIndex].style);
				return;
			}
		}
	}

	namespace VehicleSlam
	{
		Vehicle& slamVehicle = g_Ped4;
		float* slamValue;
		void InitSub(GTAvehicle veh, float* val)
		{
			slamVehicle = veh.Handle();
			slamValue = val;
		}

		struct NamedSlamValueS { std::string name; float value; };
		std::vector<NamedSlamValueS> vValues_VehicleSlam
		{
			{ "Off", 0.0f },

			{ "I'm A Skyscraper", 0.35f },
			{ "Such High", 0.30f },
			{ "I'm Tall, Aren't I", 0.25f },
			{ "I'm Growing", 0.20f },
			{ "Tip Toeing", 0.10f },

			{ "I Got The Crabs", -0.10f },
			{ "Get Get Get, Get Low", -0.20f },
			{ "Short Is Cute", -0.25f },
			{ "I'm A Ninja Turtle", -0.30f },
			{ "Apple Bottom Jeans", -0.38f }
		};

		void VehicleSlamMenu()
		{
			AddTitle("Slam It");

			bool pressed = 0;
			for (auto& sl : vValues_VehicleSlam)
			{
				pressed = 0;
				AddTickol(sl.name, *slamValue == sl.value, pressed, pressed, TICKOL::CARTHING); if (pressed)
				{
					*slamValue = sl.value;
					if (sl.value <= -0.35f)
					{
						Game::Print::PrintBottomCentre("~b~Note:~s~ You can even drive on walls with this value.");
					}
				}
			}
		}
	}

	void AddvweaponOption(const std::string& text, bool &loopPointer, Hash Hash)
	{
		bool pressed = false;
		AddToggle(text, loopPointer, pressed);
		if (pressed)
		{
			if (Hash)
			{
				if (!HAS_WEAPON_ASSET_LOADED(Hash)) 
				{
					REQUEST_WEAPON_ASSET(Hash, 31, 0);
				}
				Game::Print::PrintBottomLeft("Press ~b~LS/L1/NUM_PLUS~s~ for hax!");
			}
		}
	}
	void VehicleWeaponsMenu()
	{
		bool vehicleWeaponsAllOff = false;

		AddTitle("Vehicle Weapons");
		AddToggle("Display Projectile Path", vehicleWeaponLines);
		AddOption("Turn All Off", vehicleWeaponsAllOff);
		AddvweaponOption("RPG", vehicleRPG, 3204302209);
		AddvweaponOption("Fireworks", vehicleFireworks, 2138347493);
		AddvweaponOption("Guns", vehicleGuns, 3220176749);
		AddvweaponOption("Snowballs", vehicleSnowballs, 126349499);
		AddvweaponOption("Balls", vehicleBalls, 600439132);
		AddvweaponOption("Water Hydrants", vehicleWaterHydrant, 0);
		AddvweaponOption("Flame Leaks", vehicleFlameLeak, 0);
		AddvweaponOption("Green Laser", vehicleLaserGreen, 4026335563);
		AddvweaponOption("Red Laser", vehicleLaserRed, 1566990507);
		AddvweaponOption("Valkyrie Turrets", vehicleTurretsValkyrie, 1097917585);
		AddvweaponOption("Flares", vehicleFlaregun, WEAPON_FLARE);
		AddvweaponOption("Heavy Snipers", vehicleHeavySniper, 205991906);
		AddvweaponOption("Tazers", vehicleTazerWeapon, 911657153);
		AddvweaponOption("Molotovs", vehicleMolotovWeapon, 615608432);
		AddvweaponOption("Combat PDWs", vehicleCombatPDW, 171789620);


		if (vehicleWeaponsAllOff) 
		{
			vehicleRPG = false;
			vehicleFireworks = false;
			vehicleGuns = false;
			vehicleSnowballs = false;
			vehicleBalls = false;
			vehicleWaterHydrant = false;
			vehicleFlameLeak = false;
			vehicleLaserGreen = false;
			vehicleLaserRed = false;
			vehicleTurretsValkyrie = false;
			vehicleFlaregun = false;
			vehicleHeavySniper = false;
			vehicleTazerWeapon = false;
			vehicleMolotovWeapon = false;
			vehicleCombatPDW = false;
			return;
		}
	}

	void VehicleMultipliersMenu()
	{
		bool accelerationMultiplierPlus = false;
		bool accelerationMultiplierMinus = false;
		bool brakeMultiplierPlus = false;
		bool brakeMultiplierMinus = false;
		bool handlingMultiplierPlus = false;
		bool handlingMultiplierMinus = false;

		AddTitle("Multipliers");
		AddNumber("CMOD_STAT_1", accelMult, 0, null, accelerationMultiplierPlus, accelerationMultiplierMinus, true); // Acceleration
		AddNumber(Game::GetGXTEntry("CMOD_STAT_2") + " & Reverse", brakeMult, 0, null, brakeMultiplierPlus, brakeMultiplierMinus); // Braking & Reverse
		AddNumber("CMOD_STAT_3", handlingMult, 0, null, handlingMultiplierPlus, handlingMultiplierMinus, true); // Handling/Traction


		if (accelerationMultiplierPlus) 
		{ 
			if (accelMult < 200) 
			{
				accelMult++; 
				return;
			}
		}

		if (accelerationMultiplierMinus) 
		{ 
			if (accelMult > 0) 
			accelMult--; 
			return;
		}

		if (brakeMultiplierPlus) 
		{ 
			if (brakeMult < 100) 
			{
				brakeMult++; 
				return; 
			}
		}

		if (brakeMultiplierMinus) 
		{ 
			if (brakeMult > 0) 
			{
				brakeMult--; 
				return; 
			}
		}

		if (handlingMultiplierPlus) 
		{ 
			if (handlingMult < 100) 
			{
				handlingMult++; 
				return; 
			}
		}
		if (handlingMultiplierMinus) 
		{ 
			if (handlingMult > 0) 
			{
				handlingMult--; 
				return; 
			}
		}
	}

	void VehicleMultiPlatNeonsMenu()
	{
		bool addCar = 0, 
		setRGBIndexTo3 = 0, 
		clearList = 0,
		intensityPlus = 0, 
		intensityMinus = 0, 
		intensityCustom = 0;

		AddTitle("Neons");
		AddToggle("Toggle", multiPlatNeons, null, clearList);
		AddNumber("Intensity", g_multiPlatNeonsIntensity, 2, intensityCustom, intensityPlus, intensityMinus);
		AddToggle("Rainbow Mode", multiPlatNeonsRainbow);

		AddOption("Set Colour", setRGBIndexTo3, nullFunc, SUB::MSPAINTS_RGB);
		if (*Menu::currentopATM == Menu::printingop)
		{
			AddPresetColourOptionsPreview(g_multiPlatNeonsColor);
		}

		if (intensityPlus) 
		{ 
			g_multiPlatNeonsIntensity += 0.05f; 
		}
		if (intensityMinus) 
		{ 
			g_multiPlatNeonsIntensity -= 0.05f; 
		}

		if (intensityCustom)
		{
			std::string inputStr = Game::InputBox(std::to_string(g_multiPlatNeonsIntensity), 10U, "Enter Value:", std::to_string(g_multiPlatNeonsIntensity));
			if (inputStr.length() > 0)
			{
				try
				{
					g_multiPlatNeonsIntensity = stof(inputStr);
				}
				catch (...)
				{
					Game::Print::PrintErrorInvalidInput(inputStr);
				}
			}
		}

		if (setRGBIndexTo3) 
		{ 
			bitMSPaintsRGBMode = 3;
		}

		if (clearList) 
		{ 
			g_multiPlatNeonsList.clear(); 
		}
	}

	namespace Speedo
	{
		
		struct NamedSpeedoImage
		{
			std::string displayName;
			std::string fileName;
			DxHookIMG::DxTexture id;
		};

		std::vector<NamedSpeedoImage> speedoImagesNames[]
		{
			{
				{ "Orange", "needle_orange", 0 },
				{ "Orange Night", "night_needle_orange", 0 },
				{ "Red", "needle_red", 0 },
				{ "Red Night", "night_needle_red", 0 }
			},
			{
				{ "Cyan", "bg_cyan2", 0 },
				{ "Green", "bg_green2", 0 },
				{ "Yellow", "bg_yellow2", 0 },
				{ "Orange", "bg_orange2", 0 },
				{ "Purple", "bg_purple2", 0 },
				{ "Pink", "bg_pink2", 0 }
			},
			{
				{ "Cyan", "bg_cyan", 0 },
				{ "Green", "bg_green", 0 },
				{ "Yellow", "bg_yellow", 0 },
				{ "Orange", "bg_orange", 0 },
				{ "Purple", "bg_purple", 0 },
				{ "Pink", "bg_pink", 0 }
			}
		};

		SpeedoImage currentSpeedoBG = { speedoImagesNames[2].at(0).fileName, 0 };
		SpeedoImage currentSpeedoNeedle = { speedoImagesNames[0].at(0).fileName, 0 };
		UINT8 speedoAlpha = 0;
		Vector2 speedoPosition = { 0.85f, 0.86f };

		UINT8 loopSpeedo = SPEEDOMODE_OFF;
		bool speedoMPH = false;

		void SetCurrentBgIdFromBgNameForConfig()
		{
			for (auto& sic : speedoImagesNames)
			{
				for (auto& si : sic)
				{
					if (currentSpeedoBG.fileName == si.fileName)
					{
						currentSpeedoBG.id = si.id;
						return;
					}
				}
			}
		}
		void LoadSpeedoImages()
		{
			for (auto& sic : speedoImagesNames)
			{
				for (auto& si : sic)
				{
					si.id.Load(GetPathffA(Pathff::Speedo, true) + si.fileName + ".png");
				}
			}

			SetCurrentBgIdFromBgNameForConfig();
		}

		inline void DrawSpeedoImage(SpeedoImage& bg, SpeedoImage& needle, float speedf, float alpha)
		{
			Vector2 size = { 0.1540f, 0.164f };
			Vector2& pos = Speedo::speedoPosition;

			bg.id.Draw(0, pos, size, 0.0f, RGBA(255, 255, 255, alpha < 170 ? alpha : 170));
			needle.id.Draw(0, pos, size, (speedf > 270.0f ? 270.6f : speedf), RGBA(255, 255, 255, alpha));

			Game::Print::SetupDraw(font_speedo, Vector2(0.33, 0.33), true, false, false, RGBA(0, 153, 153, alpha));
			Game::Print::drawfloat(speedf, 0, pos.x, pos.y - 0.073f);
		}

		void SpeedoTick()
		{
			if (!IS_PLAYER_CONTROL_ON(PLAYER_ID()))
			{
				speedoAlpha = 0;
			}
			else
			{
				float speedf = abs(3.6f * GET_ENTITY_SPEED_VECTOR(g_myVeh, true).y);
				if (speedoMPH)
				{
					speedf *= 0.6214f;
				}

				if (loopSpeedo == SPEEDOMODE_DIGITAL)
				{
					std::string speedStr;
					if (speedoMPH)
					{
						speedStr = std::to_string((int)speedf) + " ~b~MPH";
					}
					else
					{
						speedStr = std::to_string((int)speedf) + " ~b~KMPH";
					}
					Game::Print::SetupDraw(font_speedo, Vector2(0.8f, 0.8f), true, false, false);
					Game::Print::drawstring(speedStr, 0.915f, 0.8f);

				}
				else
				{
					if (speedoAlpha < 255) 
					{
						speedoAlpha += 5;
					}

					UINT8 clockHour = GET_CLOCK_HOURS();
					if (clockHour < 19 && clockHour > 7)
					{
						currentSpeedoNeedle = { speedoImagesNames[0].at(0).fileName, speedoImagesNames[0].at(0).id }; // Day
					}
					else
					{
						currentSpeedoNeedle = { speedoImagesNames[0].at(1).fileName, speedoImagesNames[0].at(1).id }; // Night
					}

					DrawSpeedoImage(currentSpeedoBG, currentSpeedoNeedle, speedf, speedoAlpha);
				}
			}
		}


		void AddOptionSpeedo(const std::string& text, NamedSpeedoImage& img)
		{
			bool pressed = false;
			AddTickol(text, currentSpeedoBG.id.ID() == img.id.ID(), pressed, pressed); if (pressed)
			{
				currentSpeedoBG = { img.fileName, img.id };
			}

		}

		void SpeedoMainMenu()
		{
			std::vector<std::string> vSpeedoUnits{ "Metric", "Imperial" };
			std::vector<std::string> vSpeedoTnames{ "Off", "Digital", "Analogue" };

			bool speedoModePlus = 0;
			bool speedoModeMinus = 0;
			bool speedoUnitPlus = 0;
			bool asPosXPlus = 0;
			bool asPosXMinus = 0;
			bool asPosYPlus = 0;
			bool asPosYMinus = 0;

			AddTitle("Speedometers");
			AddTexter("Speedo", loopSpeedo, vSpeedoTnames, null, speedoModePlus, speedoModeMinus);
			AddTexter("Unit", speedoMPH, vSpeedoUnits, null, speedoUnitPlus, speedoUnitPlus);
			if (loopSpeedo == SPEEDOMODE_ANALOGUE)
			{
				AddOption("Light Themes", null, nullFunc, SUB::VEHICLESPEEDOS_LIGHT);
				AddOption("Dark Themes", null, nullFunc, SUB::VEHICLESPEEDOS_DARK);
				AddNumber("X Position", speedoPosition.x, 3, null, asPosXPlus, asPosXMinus);
				AddNumber("Y Position", speedoPosition.y, 3, null, asPosYPlus, asPosYMinus);
			}

			if (asPosXPlus) 
			{
				speedoPosition.x += 0.005f;
			}
			else if (asPosXMinus) 
			{
				speedoPosition.x -= 0.005f;
			}
			if (asPosYPlus) 
			{
				speedoPosition.y += 0.005f;
			}
			else if (asPosYMinus) 
			{
				speedoPosition.y -= 0.005f;
			}

			if (speedoModePlus) 
			{ 
				if (loopSpeedo < vSpeedoTnames.size() - 1) 
				{
					loopSpeedo++;
				}
			}
			if (speedoModeMinus) 
			{
				if (loopSpeedo  > 0) 
				{
					loopSpeedo--;
				}
			}

			if (speedoUnitPlus) 
			{ 
				speedoMPH = speedoMPH == 0 ? 1 : 0; 
			}


		}
		void SpeedoThemesLight()
		{
			AddTitle("Light Themes");

			for (auto& si : speedoImagesNames[1])
			{
				AddOptionSpeedo(si.displayName, si);
			}

		}
		void SpeedoThemesDark()
		{
			AddTitle("Dark Themes");

			for (auto& si : speedoImagesNames[2])
			{
				AddOptionSpeedo(si.displayName, si);
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(VEHICLEOPS,           sub::VehicleMenu)
REGISTER_SUBMENU(PVOPS,                sub::PVMenu)
REGISTER_SUBMENU(VEHICLEWEAPONS,       sub::VehicleWeaponsMenu)
REGISTER_SUBMENU(VEHICLEMULTIPLIERS,   sub::VehicleMultipliersMenu)
REGISTER_SUBMENU(VEHICLESPEEDOS,       sub::Speedo::SpeedoMainMenu)
REGISTER_SUBMENU(VEHICLESPEEDOS_LIGHT, sub::Speedo::SpeedoThemesLight)
REGISTER_SUBMENU(VEHICLESPEEDOS_DARK,  sub::Speedo::SpeedoThemesDark)
REGISTER_SUBMENU(MULTIPLATNEONS,       sub::VehicleMultiPlatNeonsMenu)
REGISTER_SUBMENU(VEHICLESLAM,          sub::VehicleSlam::VehicleSlamMenu)
REGISTER_SUBMENU(AUTODRIVESUB,         sub::VehicleAutoDrive::AutoDriveMenu)