/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "MiscOptions.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"
#include "..\Util\GTAmath.h"
#include "..\Util\FileLogger.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\TimecycleModification.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GameplayCamera.h"

#include "..\Misc\Gta2Cam.h"
#include "..\Misc\ManualRespawn.h"
#include "..\Misc\MeteorShower.h"
#include "..\Misc\JumpAroundMode.h"
#include "..\Misc\FpsCounter.h"

#include "..\Submenus\Spooner\EntityManagement.h"

#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace sub
{
	void MiscOps()
	{
		g_Ped1 = PLAYER_PED_ID();

		const std::vector<std::string> explosions_wp_names{ "Off", "Visible & Shaky", "Visible", "Invisible" };

		bool misc_freecam_on = 0, misc_freecam_off = 0,
			autokill_plus = 0, autokill_minus = 0,
			misc_massacre_mode_on = 0,
			misc_massacre_mode_off = 0,
			jumpAround_on = 0,
			jumpAround_off = 0,
			blackout_off = 0,
			explosions_wp_plus = 0,
			explosions_wp_minus = 0,
			//veh_collision_on = 0,
			misc_pedPopu_off = 0,
			misc_vehPopu_off = 0;

		AddTitle("Misc Options");
		//	AddToggle("First Person View", loop_fps_cam_rot, fps_mode_on, fps_mode_on);
		AddToggle("FreeCam (No-Clip)", noClip, misc_freecam_on, misc_freecam_off);
		AddLocal("Top-Down View", GTA2Cam::g_gta2Cam.Enabled(), GTA2Cam::ToggleOnOff, GTA2Cam::ToggleOnOff);
		AddLocal("Manual Respawn", ManualRespawn::g_manualRespawn.Enabled(), ManualRespawn::ToggleOnOff, ManualRespawn::ToggleOnOff);
		AddTexter("Auto-kill Enemies", autoKillEnemies, std::vector<std::string>{"Off", "Weak", "Radical"}, null, autokill_plus, autokill_minus);
		AddLocal("Meteor Shower Mode", MeteorShower::g_meteorShower.Enabled(), MeteorShower::ToggleOnOff, MeteorShower::ToggleOnOff);
		AddToggle("EMP Mode (For Night-time)", blackoutMode, null, blackout_off);
		AddToggle("Simple Blackout Mode (For Night-time)", simpleBlackoutMode, null, blackout_off);
		AddToggle("Jump-Around Mode", JumpAroundMode::bEnabled, jumpAround_on, jumpAround_off);
		AddToggle("Fireworks Ahoy", fireworksDisplay);
		AddToggle("Massacre Mode", massacreMode, misc_massacre_mode_on, misc_massacre_mode_off);
		AddToggle("Restricted Area Access", restrictedAreasAccess);
		AddTexter("Explosions At Waypoint", explostionWP, explosions_wp_names, null, explosions_wp_plus, explosions_wp_minus);
		AddToggle("Decreased Ped Population", pedPopulation, null, misc_pedPopu_off);
		AddToggle("Decreased Vehicle Population", vehiclePopulation, null, misc_vehPopu_off);
		AddToggle("Decreased Weapon Pickups", clearWeaponPickups);
		AddOption("Cutscene Player", null, nullFunc, SUB::CUTSCENEPLAYER);
		AddOption("TV Player", null, nullFunc, SUB::TVCHANNELSTUFF_TV);
		AddOption("Radio", null, nullFunc, SUB::RADIOSUB);
		AddOption("Animal Riding (SP)", null, nullFunc, SUB::ANIMALRIDING);
		AddOption("Clear Area", null, nullFunc, SUB::CLEARAREA);
		AddOption("Vision Hax", null, nullFunc, SUB::TIMECYCLES);
		AddOption("Map Mods (Old)", null, nullFunc, SUB::MAPMODS);
		AddOption("HUD Options", null, nullFunc, SUB::HUDOPTIONS);
		AddOption("Game Camera Options", null, nullFunc, SUB::GAMECAMOPTIONS);

		AddBreak("Not So Fun");

		bool bDeleteAllCams = false;
		AddOption("Delete All Cameras", bDeleteAllCams); if (bDeleteAllCams)
		{
			World::RenderingCamera_set(0);
			World::DestroyAllCameras();
			World::RenderingCamera_set(0);
		}

		AddOption("Rectangle Draw Tool (Mouse) (ALPHA) [DEV]", null, DrawToolSub_, -1, true);

		bool bEnableCellphoneYsc = false;
		AddTickol("In-Game Mobile Phone", GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(0xF292D030) > 0, bEnableCellphoneYsc, bEnableCellphoneYsc, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bEnableCellphoneYsc)
		{
			if (GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(0xF292D030) > 0) // cellphone_controller
			{
				TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("cellphone_controller");
			}
			else
			{
				Game::RequestScript("cellphone_controller", 1424);
			}
		}

		enum eYscScriptTexterIndex { YSCSCRIPTTEXTER_LOAD, YSCSCRIPTTEXTER_UNLOAD };
		static UINT8 _____yscScript_texter_index = YSCSCRIPTTEXTER_LOAD;
		std::vector<std::string> vYscScriptTexter{ "Load", "Unload" };
		bool yscScript_input = false, yscScript_plus = false, yscScript_minus = false;
		AddTexter("YSC Script [DEV]", _____yscScript_texter_index, vYscScriptTexter, yscScript_input, yscScript_plus, yscScript_minus);
		if (yscScript_plus) { if (_____yscScript_texter_index < vYscScriptTexter.size() - 1) _____yscScript_texter_index++; }
		if (yscScript_minus) { if (_____yscScript_texter_index > 0) _____yscScript_texter_index--; }
		if (yscScript_input)
		{
			std::string inputStr = std::string(Game::InputBox("", 65U, "Enter script name:"));
			if (inputStr.length() > 0)
			{
				static const std::map<std::string, UINT16> vYscStackSizes
				{
					{ "achievement_controller", 1424 },
					{ "ambient_sonar", 1424 },
					{ "ambient_tonya", 1424 },
					{ "ambient_tonyacall2", 1424 },
					{ "ambient_tonyacall5", 1424 },
					{ "am_mp_property_int", 11048 },
					{ "am_mp_yacht", 5000 },
					{ "am_pi_menu", 9800 },
					{ "appchecklist", 2552 },
					{ "appchecklist", 3800 },
					{ "appcontacts", 3800 },
					//{ "appinternet", 2552 },
					//{ "appinternet", 3584 },
					{ "appinternet", 3800 },
					{ "appmpjoblistnew", 2552 },
					{ "appmpjoblistnew", 3800 },
					{ "blip_controller", 1424 },
					{ "bootycallhandler", 1424 },
					{ "buddydeathresponse", 1424 },
					{ "building_controller", 1424 },
					{ "candidate_controller", 1424 },
					{ "carwash2", 1424 },
					{ "celebrations", 3650 },
					{ "cellphone_controller", 1424 },
					{ "cellphone_flashhand", 1424 },
					{ "cheat_controller", 1424 },
					{ "completionpercentage_controller", 1424 },
					{ "context_controller", 1424 },
					{ "controller_ambientarea", 1424 },
					//{ "controller_races", 128 },
					{ "controller_races", 1424 },
					{ "controller_towing", 1424 },
					{ "country_race", 3650 },
					{ "dialogue_handler", 1424 },
					{ "drunk_controller", 1424 },
					{ "email_controller", 1424 },
					{ "emergencycall", 512 },
					{ "emergencycalllauncher", 1424 },
					{ "event_controller", 1424 },
					{ "fake_interiors", 1424 },
					{ "flow_controller", 1424 },
					{ "fmmc_launcher", 14000 },
					{ "fm_capture_creator", 18000 },
					{ "fm_deathmatch_creator", 18000 },
					{ "fm_lts_creator", 18000 },
					{ "fm_maintain_cloud_header_data", 1424 },
					{ "fm_main_menu", 3650 },
					{ "fm_mission_controller", 31000 },
					{ "fm_mission_creator", 18000 },
					{ "fm_race_creator", 18000 },
					{ "freemode", 21512 },
					{ "freemode_init", 3650 },
					{ "ingamehud", 3650 },
					{ "maintransition", 8032 },
					{ "maude_postbailbond", 1424 },
					{ "mission_stat_alerter", 1424 },
					{ "mission_stat_watcher", 1828 },
					{ "mpstatsinit", 1424 },
					{ "mrsphilips2", 18000 },
					{ "net_cloud_mission_loader", 2050 },
					{ "net_rank_tunable_loader", 1424 },
					{ "net_tunable_check", 1424 },
					{ "pickup_controller", 1424 },
					{ "player_controller", 1424 },
					{ "postrc_barry1and2", 1424 },
					{ "randomchar_controller", 1424 },
					{ "restrictedareas", 1424 },
					{ "selector", 1424 },
					{ "shop_controller", 1424 },
					{ "social_controller", 1828 },
					{ "stats_controller", 1424 },
					{ "stock_controller", 1424 },
					{ "taxilauncher", 1424 },
					{ "taxiservice", 1828 },
					{ "tennis_family", 3650 },
					{ "traffick_air", 18000 },
					{ "ugc_global_registration", 128 },
					{ "vehicle_gen_controller", 1828 }
				};
				boost::to_lower(inputStr);
				switch (_____yscScript_texter_index)
				{
				case eYscScriptTexterIndex::YSCSCRIPTTEXTER_LOAD:
					Game::RequestScript(inputStr.c_str(), vYscStackSizes.count(inputStr) ? vYscStackSizes.at(inputStr) : 14000);
					break;
				case eYscScriptTexterIndex::YSCSCRIPTTEXTER_UNLOAD:
					TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME(inputStr.c_str());
					break;
				}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::YscScriptLoadInput, std::string(), 64U, "Enter script name:");
			//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_____yscScript_texter_index);
		}


		if (misc_freecam_on)
		{
			noClipToggle = false;
			Game::Print::PrintBottomCentre("Press ~b~" + VkCodeToStr(BindNoClip) + "~s~ OR ~b~X+LS~s~ OR ~b~Square+L3~s~ to toggle FreeCam.");
			return;
		}

		if (misc_freecam_off) { if (noClipToggle) SetNoclipOff1(); SetNoclipOff2(); return; }

		if (autokill_plus) { if (autoKillEnemies < 2) autoKillEnemies++; return; }
		if (autokill_minus) { if (autoKillEnemies > 0) autoKillEnemies--; return; }

		if (misc_massacre_mode_on) { World::ClearWeatherOverride(); World::SetWeatherOverTime(WeatherType::Thunder, 4000); return; }
		if (misc_massacre_mode_off) { World::ClearWeatherOverride(); World::SetWeatherOverTime(WeatherType::ExtraSunny, 4000); return; }

		if (jumpAround_on || jumpAround_off)
		{
			JumpAroundMode::StartJumping(jumpAround_on);
		}

		if (blackout_off)
		{
			SET_ARTIFICIAL_LIGHTS_STATE(FALSE);
			SET_ARTIFICIAL_VEHICLE_LIGHTS_STATE(TRUE);
		}

		if (explosions_wp_plus) { if (explostionWP < explosions_wp_names.size() - 1) explostionWP++; return; }
		if (explosions_wp_minus) { if (explostionWP > 0) explostionWP--; return; }

		if (misc_pedPopu_off) { SET_PED_POPULATION_BUDGET(1); return; }
		if (misc_vehPopu_off) { SET_VEHICLE_POPULATION_BUDGET(1); return; }

	}

	void Timecycles()
	{
		bool timecycles_reset = 0, timecycles_input = 0,
			heat_vision_on = 0, night_vision_on = 0, night_vision_off = 0,
			strength_plus = 0, strength_minus = 0;

		AddTitle("Vision Hax");
		AddLocal("Heat Vision", GET_USINGSEETHROUGH(), heat_vision_on, heat_vision_on);
		AddToggle("Heat Vision On Aim", hvSnipers);
		AddToggle("Night Vision (SP)", bitNightVision, night_vision_on, night_vision_off);

		AddBreak("---Timecycle Hax---");
		AddNumber("Timecycle Strength", currentTimecycleStrength, 2, null, strength_plus, strength_minus);
		AddOption("Reset", timecycles_reset);

		for (auto& i : TimecycleModification::vTimecycles)
		{
			bool bModPressed = false;
			AddOption(i.second, bModPressed); if (bModPressed)
			{
				TimecycleModification::SetMod(i.first);
				TimecycleModification::SetModStrength(currentTimecycleStrength);
			}
		}

		AddBreak("---Custom---");
		AddOption("Input Custom", timecycles_input);

		if (heat_vision_on) { SET_SEETHROUGH(GET_USINGSEETHROUGH() ? FALSE : TRUE); return; }

		if (night_vision_on) { SET_NIGHTVISION(TRUE); return; }
		if (night_vision_off) { SET_NIGHTVISION(FALSE); return; }


		if (timecycles_reset)
		{
			TimecycleModification::ClearMod();
			currentTimecycleStrength = 0.9f;
			return;
		}
		if (timecycles_input)
		{
			std::string inputStr = Game::InputBox("DEFAULT", 28U);
			if (inputStr.length())
			{
				SET_TIMECYCLE_MODIFIER(inputStr.c_str());
				SET_TIMECYCLE_MODIFIER_STRENGTH(currentTimecycleStrength);
			}
			return;
		}


		if (strength_plus)
		{
			if (currentTimecycleStrength < 3.0f) currentTimecycleStrength += 0.02f;
			TimecycleModification::SetModStrength(currentTimecycleStrength);
			return;
		}
		if (strength_minus)
		{
			if (currentTimecycleStrength > 0.0f) currentTimecycleStrength -= 0.02f;
			TimecycleModification::SetModStrength(currentTimecycleStrength);
			return;
		}

	}

	void ClearAreaSub()
	{
		bool cleararea_vehicles = 0, cleararea_peds = 0, cleararea_objects = 0, cleararea_all = 0,
			cleararea_radius_plus = 0, cleararea_radius_minus = 0, cleararea_radius_input = 0;

		AddTitle("Clear Area");

		AddNumber("Range To Clear", g_clearAreaRadius, 2, cleararea_radius_input, cleararea_radius_plus, cleararea_radius_minus);
		if (*Menu::currentopATM == Menu::printingop)
			sub::Spooner::EntityManagement::DrawRadiusDisplayingMarker(GET_ENTITY_COORDS(g_Ped1, 1), g_clearAreaRadius);

		AddOption("Vehicles", cleararea_vehicles);
		AddOption("Peds", cleararea_peds);
		AddOption("Objects", cleararea_objects);
		AddOption("All", cleararea_all);

		//if (cleararea_vehicles){ clear_area_of_vehicles_around_entity(Static_241, _globalClearArea_radius); return; }
		//if (cleararea_peds){ clear_area_of_peds_around_entity(Static_241, _globalClearArea_radius); return; }
		if (cleararea_vehicles) { clear_area_of_entities(EntityType::VEHICLE, GET_ENTITY_COORDS(g_Ped1, 1), g_clearAreaRadius, { GET_VEHICLE_PED_IS_IN(g_Ped1, 0) }); return; }
		if (cleararea_peds) { clear_area_of_entities(EntityType::PED, GET_ENTITY_COORDS(g_Ped1, 1), g_clearAreaRadius, { g_Ped1 }); return; }
		if (cleararea_objects) { clear_area_of_entities(EntityType::PROP, GET_ENTITY_COORDS(g_Ped1, 1), g_clearAreaRadius, {}); return; }
		if (cleararea_all) { clear_area_of_entities(EntityType::ALL, GET_ENTITY_COORDS(g_Ped1, 1), g_clearAreaRadius, { g_Ped1, GET_VEHICLE_PED_IS_IN(g_Ped1, 0) }); return; }

		if (cleararea_radius_input)
		{
			std::string inputStr = Game::InputBox("", 6U, "", std::to_string(g_clearAreaRadius).substr(0, 5));
			if (inputStr.length() > 0)
			{
				try
				{
					g_clearAreaRadius = stof(inputStr);
				}
				catch (...) 
				{ 
					Game::Print::PrintErrorInvalidInput(inputStr); 
					addlog(ige::LogType::LOG_ERROR, "Invalid input radius entered: " + inputStr);
				}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SetArg1Float, std::string(), 5U, std::string(), std::to_string(_globalClearArea_radius).substr(0, 5));
			//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_globalClearArea_radius);
		}
		if (cleararea_radius_plus) { if (g_clearAreaRadius < FLT_MAX) g_clearAreaRadius += 0.5f; return; }
		if (cleararea_radius_minus) { if (g_clearAreaRadius > 0.0f) g_clearAreaRadius -= 0.5f; return; }

	}

	void DrawToolSub_() // Hey yo this is broken but I'm keeping it like all those other things I've labelled (ALPHA) :D
	{
		Vector2 startPos;
		Vector2 sizePos;
		Vector2 Pos;

		for (;;)
		{
			WAIT(0);
			SET_MOUSE_CURSOR_THIS_FRAME();
			DISABLE_ALL_CONTROL_ACTIONS(1);

			Pos = MouseSupport::MousePosition();

			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_CURSOR_ACCEPT))
			{
				startPos = Pos;
			}
			if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
			{
				sizePos.x = (Pos.x - startPos.x);
				sizePos.y = (Pos.y - startPos.y);
			}

			SET_SCRIPT_GFX_ALIGN(76, 84);
			SET_SCRIPT_GFX_ALIGN_PARAMS(-0.05f, -0.05f, 0.0f, 0.0f);
			DRAW_RECT((startPos.x + sizePos.x) / 2, (startPos.y + sizePos.y) / 2, sizePos.x, sizePos.y, 107, 0, 107, 225, 0);


			DRAW_SPRITE("CommonMenu", "Gradient_Bgd", 0.90, 0.14, 0.15, 0.15, 0, 255, 255, 255, 210, false, 0);
			Game::Print::SetupDraw(7, Vector2(0.4, 0.4), true, false, false);
			Game::Print::drawstring("Details", 0.90, 0.0675);

			char str[30];

			Game::Print::SetupDraw(0, Vector2(0, 0.33), true, false, false);
			sprintf_s(str, "X - %f", startPos.x + sizePos.x);
			Game::Print::drawstring(str, 0.90, 0.0975);

			Game::Print::SetupDraw(0, Vector2(0, 0.33), true, false, false);
			sprintf_s(str, "Y - %f", startPos.y + sizePos.y);
			Game::Print::drawstring(str, 0.90, 0.1275);

			Game::Print::SetupDraw(0, Vector2(0, 0.33), true, false, false);
			sprintf_s(str, "SizeX - %f", sizePos.x);
			Game::Print::drawstring(str, 0.90, 0.1575);

			Game::Print::SetupDraw(0, Vector2(0, 0.33), true, false, false);
			sprintf_s(str, "SizeY - %f", sizePos.y);
			Game::Print::drawstring(str, 0.90, 0.1875);

			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_CANCEL))
				break;

		}

	}

	void RadioSub_()
	{
		GTAped ped = g_Ped1;
		GTAvehicle vehicle = ped.CurrentVehicle();
		bool mobile_radio_on = 0, mobile_radio_off = 0,
			radio_forward = 0;
		auto& frozenStation = g_frozenRadioStation;

		AddTitle("Radio");

		UINT8 vStationIds[] = { 255U, 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U };

		if (ped.Equals(Game::PlayerPed())) // if this is our ped
		{
			AddLocal("Mobile Radio", IS_MOBILE_PHONE_RADIO_ACTIVE(), mobile_radio_on, mobile_radio_off);
			AddOption("Skip Track", radio_forward);
			AddBreak("---Stations---");

			for (UINT8 i : vStationIds)
			{
				bool stationPressed = false;
				AddTickol(GET_RADIO_STATION_NAME(i), GET_PLAYER_RADIO_STATION_INDEX() == i, stationPressed, null, frozenStation == i ? TICKOL::TICK2 : TICKOL::TICK, TICKOL::NONE, true); if (stationPressed)
				{
					SET_RADIO_TO_STATION_INDEX(i);
					if (ped.IsInVehicle() && vehicle.Exists())
					{
						vehicle.RequestControl();
						SET_VEHICLE_RADIO_ENABLED(vehicle.Handle(), true);
						/*SET_VEHICLE_RADIO_ENABLED(vehicle.Handle(), i != RadioStation::Off);
						if (i != RadioStation::Off)*/
						SET_VEH_RADIO_STATION(vehicle.Handle(), GET_RADIO_STATION_NAME(i));
					}
				}
				if (Menu::printingop == *Menu::currentopATM)
				{
					bool bIsCurrentlyFrozen = frozenStation == i;
					if (Menu::bitController)
					{
						Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsCurrentlyFrozen ? "Freeze" : "Unfreeze") + (std::string)" station");

						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
						{
							if (bIsCurrentlyFrozen)
								frozenStation = -1;
							else
								frozenStation = i;
						}
					}
					else
					{
						Menu::add_IB(VirtualKey::B, (!bIsCurrentlyFrozen ? "Freeze" : "Unfreeze") + (std::string)" station");

						if (IsKeyJustUp(VirtualKey::B))
						{
							if (bIsCurrentlyFrozen)
								frozenStation = -1;
							else
								frozenStation = i;
						}
					}
				}
			}

		}
		else // if this isn't our ped
		{
			for (UINT8 i : vStationIds)
			{
				bool stationPressed = false;
				AddOption(GET_RADIO_STATION_NAME(i), stationPressed, nullFunc, -1, false, true); if (stationPressed)
				{
					if (vehicle.Exists())
					{
						vehicle.RequestControl();
						SET_VEHICLE_RADIO_ENABLED(vehicle.Handle(), true);
						/*SET_VEHICLE_RADIO_ENABLED(vehicle.Handle(), i != RadioStation::Off);
						if (i != RadioStation::Off)*/ SET_VEH_RADIO_STATION(vehicle.Handle(), GET_RADIO_STATION_NAME(i));

					}
				}

			}
		}

		if (mobile_radio_on) {
			SET_FRONTEND_RADIO_ACTIVE(true);
			SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(true);
			SET_MOBILE_PHONE_RADIO_STATE(1);
			PLAY_SOUND_FROM_ENTITY(-1, "Radio_On", g_Ped1, "TAXI_SOUNDS", 0, 0);
		}
		if (mobile_radio_off) {
			//SET_FRONTEND_RADIO_ACTIVE(false);
			SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(false);
			SET_MOBILE_PHONE_RADIO_STATE(0);
			PLAY_SOUND_FROM_ENTITY(-1, "Radio_Off", g_Ped1, "TAXI_SOUNDS", 0, 0);
		}

		if (radio_forward) {
			SKIP_RADIO_FORWARD();
			Game::Print::PrintBottomCentre(oss_ << Game::GetGXTEntry(GET_PLAYER_RADIO_STATION_NAME()) << " - next track");
		}

	}

	namespace WaterHack_catind
	{
		class WaterHack final
		{
		private:
			bool bEnabled = false;
		public:
			WaterHack()
			{
			}
			bool& Enabled()
			{
				return bEnabled;
			}
			float radius = 0.0f;
			float height = 5.0f;

			void Tick()
			{
				if (bEnabled)
				{
					DoWaterHack();
				}
			}
			inline void DoWaterHack()
			{
				GTAped ped = PLAYER_PED_ID();

				const Vector3& centre = ped.GetPosition();

				std::vector<Vector3> points;
				centre.PointsOnCircle(points, this->radius, this->radius < 10.0f ? 60.0f : 13.0f, 3.5f, true);

				for (auto& current : points)
				{
					WATER::MODIFY_WATER(current.x, current.y, current.z, this->height);
				}
			}
		};

		WaterHack g_waterHack;

		void Tick()
		{
			g_waterHack.Tick();
		}

		void Sub_WaterHack()
		{
			float &radius = g_waterHack.radius, &height = g_waterHack.height;
			bool radius_plus = 0, radius_minus = 0,
				height_plus = 0, height_minus = 0,
				radius_input = 0, height_input = 0;

			AddTitle("Water Hack");
			AddToggle("Toggle", g_waterHack.Enabled());
			AddNumber("Radius", radius, 1, radius_input, radius_plus, radius_minus);
			AddNumber("Height", height, 1, height_input, height_plus, height_minus);


			if (radius_plus) {
				if (radius < 650.0f)
					radius += 3.0f;
				return;
			}
			if (radius_minus) {
				if (radius > 0.0f)
					radius -= 3.0f;
				return;
			}
			if (radius_input) {
				std::string inputStr = Game::InputBox("", 10U, "", std::to_string(radius).substr(0, 9));
				if (inputStr.length() > 0)
				{
					float oldVal = radius;
					try
					{
						radius = stof(inputStr);
						if (radius > 650.0f)
							radius = 650.0f;
						else if (radius < 0.0f)
							radius = 0.0f;
					}
					catch (...) 
					{ 
						radius = oldVal; 	
						addlog(ige::LogType::LOG_ERROR, "Invalid radius entered: " + std::to_string(radius) + ", returning to oldVal: " + std::to_string(oldVal));
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::WaterHackRadius, std::string(), 9U, std::string(), std::to_string(radius).substr(0, 9));
				return;
			}

			if (height_plus) {
				if (height < 800.0f)
					height += 0.1f;
				return;
			}
			if (height_minus) {
				if (height > -800.0f)
					height -= 0.1f;
				return;
			}
			if (height_input) {
				std::string inputStr = Game::InputBox("", 10U, "", std::to_string(height).substr(0, 9));
				if (inputStr.length() > 0)
				{
					try
					{
						height = stof(inputStr);
						height = (std::min)(800.0f, (std::max)(-800.0f, height));
					}
					catch (...)
					{
						addlog(ige::LogType::LOG_ERROR, "Invalid height entered: " + std::to_string(height));
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::WaterHackHeight, std::string(), 9U, std::string(), std::to_string(height).substr(0, 9));
				return;
				}


		}
	}

	namespace TVChannelStuff_catind
	{
		bool loop_basictv = false;

		std::string currentTvChannelLabel;
		std::map<std::string, std::string> vTvPlaylists
		{
			{ "CNT", "PL_STD_CNT" },
			{ "Weazel News", "PL_STD_WZL" },
			{ "Weazel News Transition", "PL_MP_WEAZEL" },
			{ "Lotto Adverts", "PL_LO_CNT" },
			{ "Piswasser Adverts", "PL_LO_WZL" },
			{ "Righteous Slaughter", "PL_LO_RS" },
			{ "Righteous Slaughter Cutscene", "PL_LO_RS_CUTSCENE" },
			{ "Workout", "PL_SP_WORKOUT" },
			{ "Life Invader", "PL_SP_INV" },
			{ "Life Invader Explosion", "PL_SP_INV_EXP" },
			{ "Weazel Adverts", "PL_SP_PLSH1_INTRO" },
			{ "Fame or Shame Episode 1", "PL_LES1_FAME_OR_SHAME" },
			{ "Fame or Shame Episode 2", "PL_STD_WZL_FOS_EP2" },
			{ "Capolavoro", "PL_CINEMA_ARTHOUSE" },
			{ "The Loneliest Robot", "PL_CINEMA_CARTOON" },
			{ "Meltdown", "PL_CINEMA_ACTION" },
			{ "Cinema (No Meltdown)", "PL_CINEMA_MULTIPLAYER_NO_MELTDOWN" },
			{ "Jack Howitzer", "PL_WEB_HOWITZER" },
			{ "Kung Fu Rainbow LazerForce", "PL_WEB_KFLF" },
			{ "Republican Space Rangers", "PL_WEB_RANGERS" },
			{ "CCTV", "PL_MP_CCTV" }
		};

		void DrawTvWhereItsSupposedToBe()
		{
			Vector2 scale = { 0.2f, 0.2f };
			Vector2 pos = { menuPos.x > 0.45f ? 0.0f + scale.x / 2 : 1.0f - scale.x / 2, 0.1f + scale.y / 2 };

			GRAPHICS::SET_TV_AUDIO_FRONTEND(true);
			GRAPHICS::DRAW_TV_CHANNEL(pos.x, pos.y, scale.x, scale.y, 0.0f, 255, 255, 255, 250);
		}

		void Sub_TV()
		{
			float min_volume = -36.0f;
			float max_volume = 0.0f;
			auto currentChannel = GRAPHICS::GET_TV_CHANNEL();
			auto currentVolume = GRAPHICS::GET_TV_VOLUME();

			AddTitle("TV");

			AddToggle("Toggle Player", loop_basictv);

			bool bVolume_plus = false, bVolume_minus = false;
			AddNumber("Volume", currentVolume + (max_volume - min_volume), 0, null, bVolume_plus, bVolume_minus);
			if (bVolume_plus) { if (currentVolume < max_volume) { currentVolume = round(currentVolume + 1.0f); SET_TV_VOLUME(currentVolume); } }
			if (bVolume_minus) { if (currentVolume > min_volume) { currentVolume = round(currentVolume - 1.0f); SET_TV_VOLUME(currentVolume); } }

			AddBreak("---Playlists---");
			for (auto& pl : vTvPlaylists)
			{
				bool bPlaylistPressed = false;
				AddOption(pl.first, bPlaylistPressed); if (bPlaylistPressed)
				{
					GRAPHICS::IS_TVSHOW_CURRENTLY_PLAYING(80996397); //0x0AD973CA1E077B60
					GRAPHICS::SET_TV_CHANNEL(-1);
					GRAPHICS::SET_TV_CHANNEL_PLAYLIST(0, pl.second.c_str(), 1);
					GRAPHICS::SET_TV_CHANNEL(0);
				}
			}
		}
	}

	namespace HudOptions_catind
	{
		bool loop_revealMinimap = false;

		void Sub_HudOptions()
		{
			AddTitle("HUD Options");

			bool bRevealMinimap_toggle = false;
			AddToggle("Reveal Entire Minimap", loop_revealMinimap, bRevealMinimap_toggle, bRevealMinimap_toggle); if (bRevealMinimap_toggle)
			{
				SET_MINIMAP_HIDE_FOW(loop_revealMinimap);
			}

			AddToggle("Display XYZH Coords", xyzhCoords);
			AddToggle("Display FPS", FPSCounter::bDisplayFps);
			AddToggle("Hide HUD", hideHUD);
			AddToggle("Show Full HUD", showFullHUD);

			AddBreak("Component Colours");
			for (auto& h : std::vector<std::pair<int, std::string>>
			{
				{ HudColour::PURE_WHITE, "Map Blips" },
				{ HudColour::WAYPOINT, "Waypoint" },
				{ HudColour::WHITE, "Pausemenu Text & Highlighting" },
				{ HudColour::PAUSE_BG, "PauseMenu Background" },
				{ HudColour::PAUSEMAP_TINT, "PauseMenu Map Tint" },
				{ HudColour::BLACK, "Selected Text And Notifications" },
				{ HudColour::FREEMODE, "Freemode" },
				{ HudColour::MICHAEL, "Michael" },
				{ HudColour::FRANKLIN, "Franklin" },
				{ HudColour::TREVOR, "Trevor" }
			})
			{
				bool bCompSelected = false;
				AddOption(h.second, bCompSelected, nullFunc, SUB::MSPAINTS_RGB); if (bCompSelected)
				{
					g_Ped4 = h.first;
					bitMSPaintsRGBMode = 10;
				}
			}

			AddBreak("All Colours");
			for (int i = 0; i < HudColour::vHudColours.size(); i++)
			{
				bool bCompSelected = false;
				AddOption(HudColour::vHudColours[i], bCompSelected, nullFunc, SUB::MSPAINTS_RGB); if (bCompSelected)
				{
					g_Ped4 = i;
					bitMSPaintsRGBMode = 10;
				}
			}
		}
	}

	namespace GameCamOptions_catind
	{
		float _shakeAmplitude = 1.0f;
		INT8 _shakeId = -1;

		void Sub_GameCamOptions()
		{
			AddTitle("Game Camera");

			auto& vShakeNames = CameraShakeNames;

			bool bShakeType_plus = false, bShakeType_minus = false;
			AddTexter("Shake Type", _shakeId < 0 ? 0 : _shakeId, _shakeId < 0 ? std::vector<std::string>{"None"} : vShakeNames, null, bShakeType_plus, bShakeType_minus);
			if (bShakeType_plus) { if (_shakeId < (INT8)(vShakeNames.size() - 1)) { _shakeId++; GameplayCamera::Shake(static_cast<CameraShake>(_shakeId), _shakeAmplitude); } }
			if (bShakeType_minus) { if (_shakeId > -1) { _shakeId--; if (_shakeId < 0) GameplayCamera::StopShaking(true); else { GameplayCamera::Shake(static_cast<CameraShake>(_shakeId), _shakeAmplitude); } } }

			bool bShakeAmp_plus = false, bShakeAmp_minus = false;
			AddNumber("Shake Amplitude", _shakeAmplitude, 2, null, bShakeAmp_plus, bShakeAmp_minus);
			if (bShakeAmp_plus) { _shakeAmplitude += 0.05f; if (!GameplayCamera::IsShaking()) { GameplayCamera::Shake(static_cast<CameraShake>(_shakeId), _shakeAmplitude); } else GameplayCamera::ShakeAmplitude_set(_shakeAmplitude); }
			if (bShakeAmp_minus) { _shakeAmplitude -= 0.05f; if (!GameplayCamera::IsShaking()) { GameplayCamera::Shake(static_cast<CameraShake>(_shakeId), _shakeAmplitude); } else GameplayCamera::ShakeAmplitude_set(_shakeAmplitude); }
		}
	}
}

#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(TIMECYCLES,  			sub::Timecycles)
REGISTER_SUBMENU(CLEARAREA,   			sub::ClearAreaSub)
REGISTER_SUBMENU(WATERHACK,   			sub::WaterHack_catind::Sub_WaterHack)
REGISTER_SUBMENU(MISCOPS,     			sub::MiscOps)
REGISTER_SUBMENU(TVCHANNELSTUFF_TV,    	sub::TVChannelStuff_catind::Sub_TV)
REGISTER_SUBMENU(HUDOPTIONS,           	sub::HudOptions_catind::Sub_HudOptions)
REGISTER_SUBMENU(GAMECAMOPTIONS,       	sub::GameCamOptions_catind::Sub_GameCamOptions)
REGISTER_SUBMENU(RADIOSUB,             	sub::RadioSub_)




