/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "PedAnimation.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\enums.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Game.h"
#include "..\Util\keyboard.h"
#include "..\Util\StringManip.h"

#include "..\Submenus\Spooner\Databases.h"
#include "..\Submenus\Spooner\EntityManagement.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <pugixml\src\pugixml.hpp>

namespace sub
{
	void AddAnimOption(const std::string& text, const std::string& animDict, std::string animName = "", bool &extraOptionCode = null);
	namespace AnimationMenu
	{
		std::string  searchStr = std::string();

		void ClearSearchStr()
		{ 
			searchStr.clear(); 
		}

		const std::vector<AnimationMenu::NamedAnimation> presetPedAnims
		{
			{ "Idle - Conceal Weapon 1", "anim@miss@low@fin@vagos@", "idle_ped01" },
			{ "Idle - Conceal Weapon 2", "anim@miss@low@fin@vagos@", "idle_ped02" },
			{ "Idle - Conceal Weapon 3", "anim@miss@low@fin@vagos@", "idle_ped03" },
			{ "Idle - Conceal Weapon 4", "anim@miss@low@fin@vagos@", "idle_ped04" },
			{ "Idle - Conceal Weapon 5", "anim@miss@low@fin@vagos@", "idle_ped05" },
			{ "Idle - Conceal Weapon 6", "anim@miss@low@fin@vagos@", "idle_ped06" },
			{ "Idle - Conceal Weapon 7", "anim@miss@low@fin@vagos@", "idle_ped07" },
			{ "Idle - Conceal Weapon 8", "anim@miss@low@fin@vagos@", "idle_ped08" },
			{ "Surrender", "random@arrests@busted", "idle_a" },
			{ "Police - Use Radio", "random@arrests", "generic_radio_chatter" },
			{ "Sniper Crouch", "missfbi2", "franklin_sniper_crouch" },
			{ "Crouch Walking", "move_weapon@rifle@generic", "walk_crouch" },
			{ "Hip Hop Dance", "missfbi3_sniping", "dance_m_default" },
			{ "Private Dance", "mini@strip_club@private_dance@part1", "priv_dance_p1" },
			{ "Move to the Beat 1", "amb@world_human_strip_watch_stand@male_a@base", "base" },
			{ "Move to the Beat 2", "amb@world_human_strip_watch_stand@male_b@base", "base" },
			{ "Move to the Beat 3", "amb@world_human_strip_watch_stand@male_c@base", "base" },
			{ "Move to the Beat 4", "amb@world_human_strip_watch_stand@male_c@idle_a", "idle_a" },
			{ "Move to the Beat 5", "amb@world_human_partying@female@partying_beer@base", "base" },
			{ "Mountain Dance", "special_ped@mountain_dancer@monologue_1@monologue_1a", "mtn_dnc_if_you_want_to_get_to_heaven" },
			{ "Bump and Grind", "mini@strip_club@private_dance@idle", "priv_dance_idle" },
			{ "Corny Dancing", "anim@mp_player_intincardancebodhi@rds@", "idle_a" },
			{ "Dirty Dancing", "oddjobs@assassinate@multi@yachttarget@lapdance", "yacht_ld_f" },
			{ "Crazy Dancing", "misschinese2_crystalmazemcs1_ig", "dance_loop_tao" },
			{ "Shake Your Butt", "switch@trevor@mocks_lapdance", "001443_01_trvs_28_idle_stripper" },
			{ "Jerking Off", "switch@trevor@jerking_off", "trev_jerking_off_loop" },
			{ "Butt Scratch", "mp_player_int_upperarse_pick", "mp_player_int_arse_pick" },
			{ "Peeing", "misscarsteal2peeing", "peeing_loop" },
			{ "Pooping", "missfbi3ig_0", "shit_loop_trev" },
			{ "Pick Nose", "anim@mp_player_intincarnose_pickstd@rps@", "idle_a" },
			{ "Wash Hands", "missheist_agency3aig_23", "urinal_sink_loop" },
			{ "Look at Fingernails", "missfbi3_camcrew", "base_gal" },
			{ "Brush Shoulders Off", "missfbi3_camcrew", "base_gal" },
			{ "Drunk", "move_m@drunk@verydrunk_idles@", "fidget_01" },
			{ "Twitchy", "oddjobs@bailbond_hobotwitchy", "idle_a" },
			{ "Ow, My Head", "misscarsteal4@actor", "dazed_idle" },
			{ "So Depressed", "oddjobs@bailbond_hobodepressed", "idle_a" },
			{ "Kick Rocks", "timetable@ron@ig_1", "ig_1_idle_a" },
			{ "Looking for Something", "missmic_4premierejimwaitbef_prem", "wait_idle_a" },
			{ "Bend Over to Look", "switch@franklin@admire_motorcycle", "base_franklin" },
			{ "Examining", "missfbi5ig_15", "look_into_microscope_c_scientistb" },
			{ "It's a Bird, It's a Plane", "oddjobs@basejump@", "ped_a_loop" },
			{ "Flip the Bird", "anim@mp_player_intincarfingerbodhi@ds@", "idle_a" },
			{ "Yelling at Someone", "misscarsteal4@actor", "actor_berating_loop" },
			{ "Getting Yelled At", "misscarsteal4", "assistant_loop" },
			{ "Arguing Girl", "missfbi3_camcrew", "first_action_gal" },
			{ "Arguing Guy", "missfbi3_camcrew", "first_action_guy" },
			{ "Kicking Guy on Ground 1", "missheistdockssetup1ig_13@kick_idle", "guard_beatup_kickidle_guard1" },
			{ "Kicking Guy on Ground 2", "missheistdockssetup1ig_13@kick_idle", "guard_beatup_kickidle_guard2" },
			{ "Getting Kicked", "missheistdockssetup1ig_13@kick_idle", "guard_beatup_kickidle_dockworker" },
			{ "Tough Guy - Arms Crossed", "missdocksshowoffcar@idle_a", "idle_b_5" },
			{ "Stand at Rail", "anim@amb@yacht@rail@standing@male@variant_01@", "base" },
			{ "Out of Breath", "re@construction", "out_of_breath" },
			{ "Base Jump Prep", "oddjobs@bailbond_mountain", "base_jump_idle" },
			{ "Jazzercise 1", "timetable@tracy@ig_5@idle_a", "idle_c" },
			{ "Jazzercise 2", "timetable@tracy@ig_5@idle_b", "idle_e" },
			{ "Warmup Stretching 1", "mini@triathlon", "idle_a" },
			{ "Warmup Stretching 2", "mini@triathlon", "ig_2_gen_warmup_01" },
			{ "Jog in Place", "amb@world_human_jog_standing@male@fitbase", "base" },
			{ "Cat - Scratch Ear", "creatures@cat@player_action@", "action_a" },
			{ "Dog - Sit", "creatures@dog@move", "sit_loop" },
			{ "Dog - Walk in Circles", "creatures@dog@move", "idle_turn_l" },
			{ "Rabbit - Scratch Ear", "creatures@rabbit@player_action@", "action_a" },
			{ "Monkey - Idle 1", "missfbi5ig_30monkeys", "monkey_a_idle" },
			{ "Monkey - Idle 2", "missfbi5ig_30monkeys", "monkey_b_idle" },
			{ "Monkey - Idle 3", "missfbi5ig_30monkeys", "monkey_c_idle" },
			{ "Monkey - Freakout 1", "missfbi5ig_30monkeys", "monkey_a_freakout_loop" },
			{ "Monkey - Freakout 2", "missfbi5ig_30monkeys", "monkey_b_freakout_loop" },
			{ "Monkey - Freakout 3", "missfbi5ig_30monkeys", "monkey_c_freakout_loop" }
		};

		std::map<std::string, std::vector<std::string>> allPedAnims;
		std::pair<const std::string, std::vector<std::string>>* selectedAnimDictPtr = nullptr;

		void PopulateAllPedAnimsList()
		{
			std::ifstream fin(GetPathffA(Pathff::Main, true) + "PedAnimList.txt");

			if (fin.is_open())
			{
				allPedAnims.clear();

				size_t space;
				std::string lineLeft;
				std::string lineRight;

				for (std::string line; std::getline(fin, line);)
				{
					if (line.length() > 2)
					{
						space = line.find(' ');
						if (space != std::string::npos)
						{
							lineLeft = line.substr(0, space);
							lineRight = line.substr(space + 1);
							allPedAnims[lineLeft].push_back(lineRight);
						}
					}
				}
				fin.close();
			}
		}

		void AllPedAnimsMenu()
		{
			auto& searchStr = dict;
			selectedAnimDictPtr = nullptr;
			bool searchObj = false;
			bool currentPressed;
			bool notFoundInDict;

			AddTitle("All Animations");

			AddOption(searchStr.empty() ? "SEARCH" : boost::to_upper_copy(searchStr), searchObj, nullFunc, -1, true); 
			if (searchObj)
			{
				searchStr = Game::InputBox(searchStr, 126U, "SEARCH", searchStr);
				boost::to_lower(searchStr);
			}

			AddTickol("Stop Animation", true, AnimationStopAnimationCallback, AnimationStopAnimationCallback, TICKOL::CROSS);

			for (auto& current : allPedAnims)
			{
				if (!current.second.empty())
				{
					if (!searchStr.empty())
					{
						if (current.first.find(searchStr) == std::string::npos)
						{
							notFoundInDict = true;
							for (auto& current2 : current.second)
							{
								if (current2.find(searchStr) != std::string::npos)
								{
									notFoundInDict = false;
									break;
								}
							}
							if (notFoundInDict)
							{
								continue;
							}
						}
					}

					currentPressed = false;
					AddOption(current.first, currentPressed); if (currentPressed)
					{
						selectedAnimDictPtr = &current;
						Menu::SetSub_delayed = SUB::ANIMATIONSUB_ALLPEDANIMS_INDICT;
					}
				}
			}
		}
		void Sub_AllPedAnims_InDict()
		{
			if (selectedAnimDictPtr == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}
			auto& selectedDict = *selectedAnimDictPtr;
			AddTitle(selectedDict.first);

			for (auto& current : selectedDict.second)
			{
				AddAnimOption(current, selectedDict.first, current);
			}

			AddOption("Settings", null, nullFunc, SUB::ANIMATIONSUB_SETTINGS);
		}
	}

	std::string g_customAnimDict = "Enter Dictionary";
	std::string g_customAnimName = "Enter Name";
	float g_customAnimSpeed = 4;
	float g_customAnimSpeedMult = -4; 
	float g_CustomAnimRate = 0;
	int g_customAnimDuration = -1;
	int g_customAnimFlag = AnimFlag::Loop;
	bool g_customAnimLockPos = false;

	void GetFavouriteAnimations(std::vector<std::pair<std::string, std::string>>& result)
	{
		result.clear();
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml").c_str()).status == pugi::status_ok)
		{
			auto nodeAnims = doc.child("PedAnims");
			for (auto nodeAnim = nodeAnims.first_child(); nodeAnim; nodeAnim = nodeAnim.next_sibling())
			{
				std::string dict = nodeAnim.attribute("dict").as_string();
				std::string name = nodeAnim.attribute("name").as_string();
				result.push_back(std::make_pair(dict, name));
			}
		}
	}

	bool IsAnimationAFavourite(const std::string animDict, const std::string& animName)
	{
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml").c_str()).status != pugi::status_ok)
		{
			return false;
		}

		auto nodeAnims = doc.child("PedAnims");
		for (auto nodeAnim = nodeAnims.first_child(); nodeAnim; nodeAnim = nodeAnim.next_sibling())
		{
			std::string dict = nodeAnim.attribute("dict").as_string();
			std::string name = nodeAnim.attribute("name").as_string();
			if (animDict == dict && animName == name)
			{
				return true;
			}
		}
		return false;
	}

	void AddAnimationToFavourites(const std::string animDict, const std::string& animName)
	{
		pugi::xml_document doc;
		std::string filePath = GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml";
		if (doc.load_file((const char*)filePath.c_str()).status != pugi::status_ok)
		{
			doc.reset();
			auto nodeDecleration = doc.append_child(pugi::node_declaration);
			nodeDecleration.append_attribute("version") = "1.0";
			nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
			doc.append_child("PedAnims");
		}

		auto nodeAnims = doc.child("PedAnims");
		auto nodeAnim = nodeAnims.append_child("Anim");
		nodeAnim.append_attribute("dict") = animDict.c_str();
		nodeAnim.append_attribute("name") = animName.c_str();

		doc.save_file((const char*)filePath.c_str());
	}

	void RemoveAnimationFromFavourites(const std::string animDict, const std::string& animName)
	{
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml").c_str()).status != pugi::status_ok)
		{
			return;
		}

		auto nodeAnims = doc.child("PedAnims");
		for (auto nodeAnim = nodeAnims.first_child(); nodeAnim; nodeAnim = nodeAnim.next_sibling())
		{
			std::string dict = nodeAnim.attribute("dict").as_string();
			std::string name = nodeAnim.attribute("name").as_string();
			if (animDict == dict && animName == name)
			{
				nodeAnims.remove_child(nodeAnim);
			}
		}

		doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml").c_str());
	}

	void AddAnimOption(const std::string& text, const std::string& animDict, std::string animName, bool &extraOptionCode)
	{
		if (animName.length() == 0)
		{
			animName = text;
		}
		bool pressed = false;
		AddTickol(text, IS_ENTITY_PLAYING_ANIM(g_Ped1, animDict.c_str(), animName.c_str(), 3), pressed, pressed, TICKOL::MANWON); 
		if (pressed)
		{
			GTAped ped = g_Ped1;
			GTAentity att;
			auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
			}

			ped.RequestControl();
			ped.Task().PlayAnimation(animDict, animName, g_customAnimSpeed, g_customAnimSpeedMult, g_customAnimDuration, g_customAnimFlag, g_CustomAnimRate, g_customAnimLockPos);

			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				spe.lastAnimation.dict = animDict;
				spe.lastAnimation.name = animName;
				if (att.Exists() && spe.attachmentArgs.isAttached)
				{
					spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
				}
				spe.taskSequence.Reset();
				if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
				{
					sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
					sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
					sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
				}
			}
			extraOptionCode = true;
		}

		if (Menu::printingop == *Menu::currentopATM)
		{
			bool isAFavourite = IsAnimationAFavourite(animDict, animName);
			if (Menu::bitController)
			{
				Menu::add_IB(INPUT_SCRIPT_RLEFT, (!isAFavourite ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
				{
					!isAFavourite ? AddAnimationToFavourites(animDict, animName) : RemoveAnimationFromFavourites(animDict, animName);
				}
			}
			else
			{
				Menu::add_IB(VirtualKey::B, (!isAFavourite ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IsKeyJustUp(VirtualKey::B))
				{
					!isAFavourite ? AddAnimationToFavourites(animDict, animName) : RemoveAnimationFromFavourites(animDict, animName);
				}
			}
		}
	}

	void AnimationStopAnimationCallback()
	{
		GTAped ped = g_Ped1;
		GTAentity att;
		auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
		if (spi >= 0)
		{
			auto& spe = sub::Spooner::Databases::EntityDb[spi];
			Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
		}

		GTAvehicle veh = ped.CurrentVehicle();
		bool isInVehicle = veh.Exists();
		VehicleSeat vehSeat;
		if (isInVehicle)
		{
			vehSeat = ped.GetCurrentVehicleSeat();
		}
		ped.Task().ClearAllImmediately();
		if (isInVehicle)
		{
			ped.SetIntoVehicle(veh, vehSeat);
		}

		if (spi >= 0)
		{
			auto& spe = sub::Spooner::Databases::EntityDb[spi];
			spe.lastAnimation.dict.clear();
			spe.lastAnimation.name.clear();
			if (att.Exists() && spe.attachmentArgs.isAttached)
			{
				spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
			}
			spe.taskSequence.Reset();
			if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
			{
				sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
				sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
				sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
			}
		}
	}

	void PedAnimationMenu()
	{
		auto& searchStr = dict;
		dict.clear();

		bool dictSetDeer = false; 
		bool dictSetCow = false;
		bool dictSetShark = false;
		bool dictSetGuardReact = false;
		bool dictSetRandomArrests = false;
		bool dictSetSwat = false;
		bool dictSetMissionRappel = false;
		bool dictSetGesturesSitting = false;

		SET_PED_CAN_PLAY_AMBIENT_ANIMS(g_Ped1, TRUE);
		SET_PED_CAN_PLAY_AMBIENT_BASE_ANIMS(g_Ped1, TRUE);
		SET_PED_CAN_PLAY_GESTURE_ANIMS(g_Ped1, TRUE);
		SET_PED_CAN_PLAY_VISEME_ANIMS(g_Ped1, TRUE, TRUE);
		SET_PED_IS_IGNORED_BY_AUTO_OPEN_DOORS(g_Ped1, TRUE);

		AddTitle("Animations");
		AddTickol("Stop Animation", true, AnimationStopAnimationCallback, AnimationStopAnimationCallback, TICKOL::CROSS);
		AddAnimOption("Pole Dance", "mini@strip_club@pole_dance@pole_dance3", "pd_dance_03");
		AddAnimOption("Hood Dance", "missfbi3_sniping", "dance_m_default");
		AddAnimOption("Burning", "ragdoll@human", "on_fire");
		AddAnimOption("Getting Stunned", "ragdoll@human", "electrocute");
		AddAnimOption("Private Dance", "mini@strip_club@private_dance@part1", "priv_dance_p1");
		AddAnimOption("The Rear Abundance", "rcmpaparazzo_2", "shag_loop_poppy");
		AddAnimOption("The Invisible Man", "rcmpaparazzo_2", "shag_loop_a");
		AddAnimOption("Push ups", "amb@world_human_push_ups@male@base", "base");
		AddAnimOption("Sit ups", "amb@world_human_sit_ups@male@base", "base");
		AddAnimOption("Wave Yo' Arms", "random@car_thief@victimpoints_ig_3", "arms_waving");
		AddAnimOption("Give BJ to Driver", "mini@prostitutes@sexnorm_veh", "bj_loop_prostitute");
		AddAnimOption("Pleasure Driver", "mini@prostitutes@sexnorm_veh", "sex_loop_prostitute");
		AddAnimOption("Mime", "special_ped@mime@monologue_8@monologue_8a", "08_ig_1_wall_ba_0");
		AddAnimOption("Mime 2", "special_ped@mime@monologue_7@monologue_7a", "11_ig_1_run_aw_0");
		AddAnimOption("Throw", "switch@franklin@throw_cup", "throw_cup_loop");
		AddAnimOption("Smoke Coughing", "timetable@gardener@smoking_joint", "idle_cough");
		AddAnimOption("Chilling with Friends", "friends@laf@ig_1@base", "base");
		AddAnimOption("They Think We Dumb", "timetable@ron@they_think_were_stupid", "they_think_were_stupid");
		AddAnimOption("Come Here", "gestures@m@standing@fat", "gesture_come_here_hard");
		AddAnimOption("No Way", "gestures@m@standing@fat", "gesture_no_way");
		AddAnimOption("They're Gonna Kill Me", "random@bicycle_thief@ask_help", "my_dads_going_to_kill_me");
		AddAnimOption("You Gotta Help Me", "random@bicycle_thief@ask_help", "please_man_you_gotta_help_me");
		AddAnimOption("Sleep", "savecouch@", "t_sleep_loop_couch");
		AddAnimOption("Sleep 2", "savem_default@", "m_sleep_r_loop");
		AddAnimOption("Sleep 3", "timetable@tracy@sleep@", "idle_c");
		AddAnimOption("Meditate", "rcmcollect_paperleadinout@", "meditiate_idle");
		AddAnimOption("Fap", "switch@trevor@jerking_off", "trev_jerking_off_loop");
		AddAnimOption("Yeah Yeah Yeah", "special_ped@jessie@michael_1@michael_1b", "jessie_ig_2_yeahyeahyeah_1");
		AddAnimOption("Idle On Laptop", "switch@franklin@on_laptop", "001927_01_fras_v2_4_on_laptop_idle");
		AddAnimOption("Hands Up", "random@arrests", "idle_2_hands_up");
		AddAnimOption("Stand Still, Arms Spread", "mp_sleep", "bind_pose_180");

		AddOption("Sitting Animations", dictSetGesturesSitting, nullFunc, SUB::ANIMATIONSUB_GESTSIT);
		AddOption("Rappel Movements", dictSetMissionRappel, nullFunc, SUB::ANIMATIONSUB_MISSRAPPEL);
		AddOption("Arrest Movements", dictSetRandomArrests, nullFunc, SUB::ANIMATIONSUB_RANDARREST);
		AddOption("Swat Movements", dictSetSwat, nullFunc, SUB::ANIMATIONSUB_SWAT);
		AddOption("Guard Movements", dictSetGuardReact, nullFunc, SUB::ANIMATIONSUB_GUARDREAC);
		AddOption("Deer Movements", dictSetDeer, nullFunc, SUB::ANIMATIONSUB_DEER);
		AddOption("Cow Movements", dictSetCow, nullFunc, SUB::ANIMATIONSUB_DEER);
		AddOption("Shark Movements", dictSetShark, nullFunc, SUB::ANIMATIONSUB_SHARK);
		AddOption("All Animations", null, nullFunc, SUB::ANIMATIONSUB_ALLPEDANIMS);
		AddOption("Custom Input", null, nullFunc, SUB::ANIMATIONSUB_CUSTOM);
		AddOption("Favourites", null, nullFunc, SUB::ANIMATIONSUB_FAVOURITES);
		AddOption("Settings", null, nullFunc, SUB::ANIMATIONSUB_SETTINGS);


		if (dictSetDeer) 
		{ 
			dict = "creatures@deer@move"; 
			return; 
		}
		if (dictSetCow) 
		{
			dict = "creatures@cow@move"; 
			return; 
		}
		if (dictSetShark) 
		{ 
			dict = "creatures@shark@move"; 
			return; 
		}
		if (dictSetGuardReact) 
		{ 
			dict = "guard_reactions"; 
			return; 
		}
		if (dictSetRandomArrests) 
		{ 
			dict = "random@arrests"; 
			return; 
		}
		if (dictSetSwat) 
		{ 
			dict = "swat"; 
			return; 
		}
		if (dictSetMissionRappel) 
		{ 
			dict = "missrappel"; 
			return;
		}
		if (dictSetGesturesSitting) 
		{ 
			dict = "gestures@m@sitting@generic@casual"; 
			return; 
		}

	}
	void AnimationSub_Settings()
	{
		bool speedPlus = false;
		bool speedMinus = false;
		bool speedMultPlus = false; 
		bool speedMultMinus = false; 
		bool durationPlus = false;
		bool durationMinus = false; 
		bool ratePlus = false;
		bool rateMinus = false;
		bool flagPlus = false; 
		bool flagMinus = false;
		bool toggleLockPosition = false;

		AddTitle("Settings");
		AddNumber("Blend-In Speed", g_customAnimSpeed, 2, null, speedPlus, speedMinus);
		AddNumber("Blend-Out Speed", g_customAnimSpeedMult, 2, null, speedMultPlus, speedMultMinus);
		AddNumber("Duration (ms)", g_customAnimDuration, 0, null, durationPlus, durationMinus);
		AddTexter("Flag", 0, std::vector<std::string>{ AnimFlag::vFlagNames[g_customAnimFlag] }, null, flagPlus, flagMinus);
		AddNumber("Playback Rate", g_CustomAnimRate, 2, null, ratePlus, rateMinus);
		AddTickol("Lock Position", g_customAnimLockPos, toggleLockPosition, toggleLockPosition, TICKOL::BOXTICK, TICKOL::BOXBLANK); 
		if (toggleLockPosition)
		{
			g_customAnimLockPos = !g_customAnimLockPos;
		}


		if (speedPlus) 
		{ 
			if (g_customAnimSpeed < FLT_MAX) 
			{
				g_customAnimSpeed += 0.1f;
				return;
			}
		}
		if (speedMinus) 
		{ 
			if (g_customAnimSpeed > 0) 
			{
				g_customAnimSpeed -= 0.1f; 
				return; 
			}
		}
		if (speedMultPlus) 
		{ 
			if (g_customAnimSpeedMult < FLT_MAX) 
			{
				g_customAnimSpeedMult += 0.1f;
				return;
			}
		}
		if (speedMultMinus) 
		{ 
			if (g_customAnimSpeedMult > 0 - FLT_MAX) 
			{
				g_customAnimSpeedMult -= 0.1f;
				return;
			}
		}
		if (durationPlus) 
		{ 
			if (g_customAnimDuration < INT_MAX) 
			{
				g_customAnimDuration += 100;
				return;
			}
		}
		if (durationMinus) 
		{ 
			if (g_customAnimDuration > -1) 
			{
				g_customAnimDuration -= 100;
				return;
			}
			for (auto it = AnimFlag::vFlagNames.begin(); it != AnimFlag::vFlagNames.end(); ++it)
			{
				if (it->first == g_customAnimFlag)
				{
					++it;
					if (it != AnimFlag::vFlagNames.end())
					{
						g_customAnimFlag = it->first;
					}
					break;
				}
			}
			return;
		};
		if (flagMinus)
		{
			for (auto it = AnimFlag::vFlagNames.rbegin(); it != AnimFlag::vFlagNames.rend(); ++it)
			{
				if (it->first == g_customAnimFlag)
				{
					++it;
					if (it != AnimFlag::vFlagNames.rend())
					{
						g_customAnimFlag = it->first;
					}
					break;
				}
			}
			return;
		};
		if (ratePlus) 
		{ 
			if (g_CustomAnimRate < FLT_MAX) 
			{
				g_CustomAnimRate += 0.1f;
				return;
			}
		}
		if (rateMinus) 
		{ 
			if (g_CustomAnimRate > 0) 
			{
				g_CustomAnimRate -= 0.1f;
				return;
			}
		}

	}
	void AnimationFavouritesMenu()
	{
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + "FavouriteAnims.xml").c_str()).status != pugi::status_ok)
		{
			Game::Print::PrintBottomCentre("~r~Error~s~: No favourites found. Go to ~b~Custom Input~s~ and add an animation to the favourites.");
			Menu::SetPreviousMenu();
			return;
		}
		auto nodeAnims = doc.child("PedAnims");

		Menu::OnSubBack = AnimationMenu::ClearSearchStr;
		auto& searchStr = AnimationMenu::searchStr;

		AddTitle("Favourites");

		bool searchPressed = false;
		AddOption(searchStr.empty() ? "SEARCH" : boost::to_upper_copy(searchStr), searchPressed, nullFunc, -1, true); 
		if (searchPressed)
		{
			searchStr = Game::InputBox(searchStr, 126U, "SEARCH", searchStr);
			boost::to_lower(searchStr);
		}

		for (auto nodeAnim = nodeAnims.first_child(); nodeAnim; nodeAnim = nodeAnim.next_sibling())
		{
			std::string dict = nodeAnim.attribute("dict").as_string();
			std::string name = nodeAnim.attribute("name").as_string();

			if (!searchStr.empty())
			{
				if (dict.find(searchStr) == std::string::npos && name.find(searchStr) == std::string::npos)
				{
					continue;
				}
			}

			AddAnimOption(dict + ", " + name, dict, name);
		}
	}

	void AnimationSub_Custom()
	{
		auto& sub_animDict = g_customAnimDict;
		auto& sub_animName = g_customAnimName;
		bool inputDict = false;
		bool inputName = false;
		bool apply = false;
		bool stop = false;
		bool addToFavourites = false;
		bool removeFromFavourites = false;
		bool flagPlus = false;
		bool flagMinus = false;
		bool speedPlus = false;
		bool speedMinus = false; 
		bool speedMultiplierPlus = false; 
		bool speedMultiplierMinus = false;
		bool durationPlus = false;
		bool durationMinus = false; 
		bool ratePlus = false;
		bool rateMinus = false;
		bool toggleLockPosition = false;
		bool bIsAFavourite = IsAnimationAFavourite(sub_animDict, sub_animName);

		AddTitle("Custom Animation");
		AddOption(sub_animDict, inputDict);
		AddOption(sub_animName, inputName);
		AddOption("Apply", apply);
		AddOption("Stop", stop);
		AddTickol("Favourite", bIsAFavourite, addToFavourites, removeFromFavourites, TICKOL::BOXTICK, TICKOL::BOXBLANK);
		AddBreak("---Settings---");
		AddNumber("Blend-In Speed", g_customAnimSpeed, 2, null, speedPlus, speedMinus);
		AddNumber("Blend-Out Speed", g_customAnimSpeedMult, 2, null, speedMultiplierPlus, speedMultiplierMinus);
		AddNumber("Duration (ms)", g_customAnimDuration, 0, null, durationPlus, durationMinus);
		AddTexter("Flag", 0, std::vector<std::string>{ AnimFlag::vFlagNames[g_customAnimFlag] }, null, flagPlus, flagMinus);
		AddNumber("Playback Rate", g_CustomAnimRate, 2, null, ratePlus, rateMinus);
		AddTickol("Lock Position", g_customAnimLockPos, toggleLockPosition, toggleLockPosition, TICKOL::BOXTICK, TICKOL::BOXBLANK); 
		if (toggleLockPosition)
		{
			g_customAnimLockPos = !g_customAnimLockPos;
		}



		if (speedPlus) 
		{ 
			if (g_customAnimSpeed < FLT_MAX) 
			{
				g_customAnimSpeed += 0.1f; 
			}
			return; 
		};
		if (speedMinus) 
		{ 
			if (g_customAnimSpeed > 0) 
			{
				g_customAnimSpeed -= 0.1f; 
			}
			return; 
		}
		if (speedMultiplierPlus) 
		{ 
			if (g_customAnimSpeedMult < FLT_MAX) 
			{
				g_customAnimSpeedMult += 0.1f; 
			}
			return; 
		};
		if (speedMultiplierMinus) 
		{ 
			if (g_customAnimSpeedMult > 0 - FLT_MAX) 
			{
				g_customAnimSpeedMult -= 0.1f; 
			}
			return; 
		}
		if (durationPlus) 
		{ 
			if (g_customAnimDuration < INT_MAX) 
			{
				g_customAnimDuration += 100; 
			}
			return; 
		};
		if (durationMinus) 
		{ 
			if (g_customAnimDuration > -1) 
			{
				g_customAnimDuration -= 100; 
			}
			return; 
		}
		if (flagPlus)
		{
			for (auto it = AnimFlag::vFlagNames.begin(); it != AnimFlag::vFlagNames.end(); ++it)
			{
				if (it->first == g_customAnimFlag)
				{
					++it;
					if (it != AnimFlag::vFlagNames.end())
					{
						g_customAnimFlag = it->first;
					}
					break;
				}
			}
			return;
		};
		if (flagMinus)
		{
			for (auto it = AnimFlag::vFlagNames.rbegin(); it != AnimFlag::vFlagNames.rend(); ++it)
			{
				if (it->first == g_customAnimFlag)
				{
					++it;
					if (it != AnimFlag::vFlagNames.rend())
					{
						g_customAnimFlag = it->first;
					}
					break;
				}
			}
			return;
		};
		if (ratePlus) 
		{ 
			if (g_CustomAnimRate < FLT_MAX) 
			{
				g_CustomAnimRate += 0.1f; 
			}
			return; 
		};
		if (rateMinus) 
		{ 
			if (g_CustomAnimRate > 0) 
			{
				g_CustomAnimRate -= 0.1f; 
			}
			return; 
		}


		if (addToFavourites)
		{
			AddAnimationToFavourites(sub_animDict, sub_animName);
		}
		if (removeFromFavourites)
		{
			RemoveAnimationFromFavourites(sub_animDict, sub_animName);
		}

		if (apply)
		{
			GTAped ped = g_Ped1;
			GTAentity att;
			auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
			}

			ped.RequestControl();
			ped.Task().PlayAnimation(sub_animDict, sub_animName, g_customAnimSpeed, g_customAnimSpeedMult, g_customAnimDuration, g_customAnimFlag, g_CustomAnimRate, g_customAnimLockPos);

			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				spe.lastAnimation.dict = sub_animDict;
				spe.lastAnimation.name = sub_animName;
				if (att.Exists() && spe.attachmentArgs.isAttached)
				{
					spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
				}
				spe.taskSequence.Reset();
				if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
				{
					sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
					sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
					sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
				}
			}
			return;
		}

		if (stop)
		{
			GTAped ped = g_Ped1;
			GTAentity att;
			auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
			}

			GTAvehicle veh = ped.CurrentVehicle();
			bool isInVehicle = veh.Exists();
			VehicleSeat vehSeat;
			if (isInVehicle)
			{
				vehSeat = ped.GetCurrentVehicleSeat();
			}
			ped.Task().ClearAllImmediately();
			if (isInVehicle)
			{
				ped.SetIntoVehicle(veh, vehSeat);
			}

			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				spe.lastAnimation.dict.clear();
				spe.lastAnimation.name.clear();
				if (att.Exists() && spe.attachmentArgs.isAttached)
				{
					spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
				}
				spe.taskSequence.Reset();
				if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
				{
					sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
					sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
					sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
				}
			}
			return;
		}

		if (inputDict)
		{
			std::string inputStr = Game::InputBox(g_customAnimDict, 126U, "Enter dict:", g_customAnimDict);
			if (inputStr.length() > 0)
			{
				g_customAnimDict = inputStr;
			}
			else 
			{
				Game::Print::PrintErrorInvalidInput(inputStr);
			}
			return;
		}

		if (inputName)
		{
			std::string inputStr = Game::InputBox(g_customAnimName, 126U, "Enter name:", g_customAnimName);
			if (inputStr.length() > 0)
			{
				g_customAnimName = inputStr;
			}
			else 
			{
				Game::Print::PrintErrorInvalidInput(inputStr);
			}
			return;
		}
	}

	void DeerAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Animalations");
		AddAnimOption("Idle Turn Left", tempDict, "idle_turn_l");
		AddAnimOption("Idle Turn Right", tempDict, "idle_turn_r");
		AddAnimOption("gallop_turn_l", tempDict);
		AddAnimOption("gallop_turn_r", tempDict);
		AddAnimOption("walk_turn_l", tempDict);
		AddAnimOption("walk_turn_r", tempDict);
		AddAnimOption("walk_start_0_l", tempDict);
		AddAnimOption("walk_start_0_r", tempDict);
		AddAnimOption("walk_start_90_l", tempDict);
		AddAnimOption("walk_start_90_r", tempDict);
		AddAnimOption("walk_backwards", tempDict);
		AddAnimOption("gallop", tempDict);
		AddAnimOption("canter", tempDict);
		AddAnimOption("trot", tempDict);
		AddAnimOption("dead_left", tempDict);
		AddAnimOption("dying", tempDict);
	}
	
	void SharkAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Animalations");
		AddAnimOption("attack_player", tempDict, "attack_player");
		AddAnimOption("swim_turn_r", tempDict, "swim_turn_r");
		AddAnimOption("swim_turn_l", tempDict, "swim_turn_l");
		AddAnimOption("dead_left", tempDict, "dead_left");
		AddAnimOption("dead_right", tempDict, "dead_right");
		AddAnimOption("dead_up", tempDict, "dead_up");
		AddAnimOption("dying", tempDict, "dying");
		AddAnimOption("attack_cam", tempDict, "attack_cam");
		AddAnimOption("attack", tempDict, "attack");
		AddAnimOption("attack_onspot", tempDict, "attack_onspot");
		AddAnimOption("attack_jump", tempDict, "attack_jump");
		AddAnimOption("swim", tempDict, "swim");
		AddAnimOption("accelerate", tempDict, "accelerate");
	}

	void MissionRappelAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Swat Animations");
		AddAnimOption("rappel_to_free_rope_prop", tempDict);
		AddAnimOption("rope_slide", tempDict);
		AddAnimOption("rappel_jump_a_prop", tempDict);
		AddAnimOption("rappel_jump_c", tempDict);
		AddAnimOption("rappel_walk", tempDict);
		AddAnimOption("rappel_jump_c_prop", tempDict);
		AddAnimOption("rappel_loop", tempDict);
		AddAnimOption("rappel_to_free_rope", tempDict);
		AddAnimOption("rope_idle", tempDict);
		AddAnimOption("rappel_walk_prop", tempDict);
		AddAnimOption("rappel_idle", tempDict);
		AddAnimOption("rappel_idle_prop", tempDict);
		AddAnimOption("rappel_intro_player", tempDict);
		AddAnimOption("land_action", tempDict);
		AddAnimOption("land_crouched", tempDict);
		AddAnimOption("rappel_jump_a", tempDict);
		AddAnimOption("land", tempDict);
	}

	void GestureSitAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Sitting Animations");
		AddAnimOption("Sit n Shit", "timetable@trevor@on_the_toilet", "trevonlav_struggleloop");
		AddAnimOption("Smoke Meth", "timetable@trevor@smoking_meth@base", "base");
		AddAnimOption("Michael Exit Chair", "switch@michael@sitting", "exit_forward_chair");
		AddAnimOption("Michael Exit Chair 2", "switch@michael@sitting", "exit_forward");
		AddAnimOption("Michael Idle", "switch@michael@sitting", "idle_chair");
		AddAnimOption("Michael Idle 2", "switch@michael@sitting", "idle");
		AddAnimOption("Sitting On Car Bonnet", "switch@michael@sitting_on_car_bonnet", "sitting_on_car_bonnet_loop");
		AddAnimOption("Sitting On Car Premiere", "switch@michael@sitting_on_car_premiere", "sitting_on_car_premiere_loop_player");
		AddAnimOption("sitting_stungun_idk", "stungun@sitting", "damage_vehicle");
		AddAnimOption("gesture_come_here_hard", tempDict);
		AddAnimOption("gesture_come_here_soft", tempDict);
		AddAnimOption("gesture_me_hard", tempDict);
		AddAnimOption("gesture_you_hard", tempDict);
		AddAnimOption("gesture_no_way", tempDict);
		AddAnimOption("gesture_why_left", tempDict);
		AddAnimOption("gesture_nod_no_hard", tempDict);
		AddAnimOption("gesture_hello", tempDict);
		AddAnimOption("gesture_i_will", tempDict);
		AddAnimOption("getsure_its_mine", tempDict);
		AddAnimOption("gesture_me", tempDict);
		AddAnimOption("gesture_you_soft", tempDict);
		AddAnimOption("gesture_what_hard", tempDict);
		AddAnimOption("gesture_pleased", tempDict);
		AddAnimOption("gesture_shrug_soft", tempDict);
		AddAnimOption("gesture_point", tempDict);
		AddAnimOption("gesture_shrug_hard", tempDict);
		AddAnimOption("gesture_why", tempDict);
		AddAnimOption("gesture_nod_no_soft", tempDict);
		AddAnimOption("gesture_nod_yes_hard", tempDict);
		AddAnimOption("gesture_what_soft", tempDict);
		AddAnimOption("gesture_nod_yes_soft", tempDict);
		AddAnimOption("gesture_damn", tempDict);
		AddAnimOption("gesture_displeased", tempDict);
		AddAnimOption("gesture_easy_now", tempDict);
		AddAnimOption("gesture_hand_down", tempDict);
		AddAnimOption("gesture_hand_left", tempDict);
		AddAnimOption("gesture_bring_it_on", tempDict);
		AddAnimOption("gesture_bye_hard", tempDict);
		AddAnimOption("gesture_bye_soft", tempDict);
		AddAnimOption("gesture_head_no", tempDict);
		AddAnimOption("gesture_hand_right", tempDict);
	}

	void SwatAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Swat Animations");
		AddAnimOption("understood", tempDict);
		AddAnimOption("you_back", tempDict);
		AddAnimOption("rally_point", tempDict);
		AddAnimOption("you_fwd", tempDict);
		AddAnimOption("you_left", tempDict);
		AddAnimOption("you_right", tempDict);
		AddAnimOption("freeze", tempDict);
		AddAnimOption("go_fwd", tempDict);
		AddAnimOption("come", tempDict);
	}

	void GuardReactAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Guard Animations");
		AddAnimOption("1hand_fwd_fire_additive", tempDict);
		AddAnimOption("1hand_turn0r", tempDict);
		AddAnimOption("1hand_turn90r", tempDict);
		AddAnimOption("1hand_turn180r", tempDict);
		AddAnimOption("1hand_turn0l", tempDict);
		AddAnimOption("1hand_turn90l", tempDict);
		AddAnimOption("1hand_turn180l", tempDict);
		AddAnimOption("1hand_aim_med_sweep", tempDict);
		AddAnimOption("1hand_aiming_cycle", tempDict);
		AddAnimOption("1hand_aiming_to_idle", tempDict);
		AddAnimOption("med_down", tempDict);
		AddAnimOption("1hand_aim_additive", tempDict);
		AddAnimOption("1hand_aim_high_sweep", tempDict);
		AddAnimOption("1hand_aim_low_sweep", tempDict);
		AddAnimOption("1hand_right_trans", tempDict);
		AddAnimOption("1hand_left_trans", tempDict);
	}

	void RandomArrestAnimationMenu()
	{
		const std::string& tempDict = dict;

		AddTitle("Arrest Animations");
		AddAnimOption("kneeling_arrest_get_up", tempDict);
		AddAnimOption("generic_radio_enter", tempDict);
		AddAnimOption("kneeling_arrest_escape", tempDict);
		AddAnimOption("generic_radio_chatter", tempDict);
		AddAnimOption("cop_gunaimed_door_open_idle", tempDict);
		AddAnimOption("generic_radio_exit", tempDict);
		AddAnimOption("thanks_male_05", tempDict);
		AddAnimOption("radio_exit", tempDict);
		AddAnimOption("radio_enter", tempDict);
		AddAnimOption("radio_chatter", tempDict);
		AddAnimOption("kneeling_arrest_idle", tempDict);
		AddAnimOption("idle_c", tempDict);
		AddAnimOption("idle_2_hands_up", tempDict);
		AddAnimOption("arrest_walk", tempDict);
		AddAnimOption("idle_a", "random@arrests@busted", "idle_a");
		AddAnimOption("idle_b", "random@arrests@busted", "idle_b");
		AddAnimOption("enter", "random@arrests@busted", "enter");
		AddAnimOption("idle_c", "random@arrests@busted", "idle_c");
		AddAnimOption("exit", "random@arrests@busted", "exit");
	}

	namespace AnimationTaskScenarios
	{
#pragma region scenariovector
		std::vector<std::string> vValues_TaskScenarios{ "bbq_registered", "bum_wash_mist_triggered", "bum_wash_scoop_triggered", "chaining_entry", "chaining_exit", "cigarette", "code_human_cower", "code_human_cower_female", "code_human_cower_male", "code_human_cower_stand",
			"code_human_cower_stand_female", "code_human_cower_stand_male", "code_human_cross_road_wait", "code_human_cross_road_wait_female", "code_human_cross_road_wait_male", "code_human_medic_kneel", "code_human_medic_tend_to_dead", "code_human_medic_time_of_death", "code_human_park_car", "code_human_police_crowd_control", "code_human_police_investigate",
			"code_human_stand_cower", "drill debris", "drive", "ear_to_text", "ear_to_text_fat", "ear_to_text_female", "ear_to_text_female_fat", "ear_to_text_leaning_female", "ear_to_text_leaning_male", "ear_to_text_male", "ear_to_text_male_fat",
			"exhale", "exhale_nose", "feeding_animal_small_triggered", "gardener_plant_dusty_hands_triggered", "gardener_plant_soil_debris_triggered", "grazing_animal_large_triggered", "leaf blower registered", "paparazzi_flash_triggered", "park_vehicle", "prop_bird_in_tree", "prop_bird_telegraph_pole",
			"prop_human_atm", "prop_human_atm_female", "prop_human_atm_female_player", "prop_human_atm_male", "prop_human_atm_male_player", "prop_human_bbq", "prop_human_bum_bin", "prop_human_bum_shopping_cart", "prop_human_movie_bulb", "prop_human_movie_studio_light", "prop_human_muscle_chin_ups",
			"prop_human_muscle_chin_ups_army", "prop_human_muscle_chin_ups_prison", "prop_human_parking_meter", "prop_human_parking_meter_female", "prop_human_parking_meter_male", "prop_human_seat_armchair", "prop_human_seat_armchair_female_arms_folded", "prop_human_seat_armchair_female_generic", "prop_human_seat_armchair_female_legs_crossed", "prop_human_seat_armchair_male_elbows_on_knees", "prop_human_seat_armchair_male_generic",
			"prop_human_seat_armchair_male_left_elbow_on_knee", "prop_human_seat_bar", "prop_human_seat_bar_female_elbows_on_bar", "prop_human_seat_bar_female_left_elbow_on_bar", "prop_human_seat_bar_male_elbows_on_bar", "prop_human_seat_bar_male_hands_on_bar", "prop_human_seat_bench", "prop_human_seat_bench_all", "prop_human_seat_bench_and_beer", "prop_human_seat_bench_and_beer_and_food", "prop_human_seat_bench_and_coffee",
			"prop_human_seat_bench_and_coffee_and_food", "prop_human_seat_bench_and_food", "prop_human_seat_bench_any", "prop_human_seat_bench_drink", "prop_human_seat_bench_drink_beer", "prop_human_seat_bench_drink_beer_female", "prop_human_seat_bench_drink_beer_male", "prop_human_seat_bench_drink_female_generic", "prop_human_seat_bench_drink_male_generic", "prop_human_seat_bench_female", "prop_human_seat_bench_female_arms_folded",
			"prop_human_seat_bench_female_food", "prop_human_seat_bench_female_generic_skinny", "prop_human_seat_bench_female_legs_crossed", "prop_human_seat_bench_food", "prop_human_seat_bench_male", "prop_human_seat_bench_male_elbows_on_knees", "prop_human_seat_bench_male_food", "prop_human_seat_bench_male_generic_skinny", "prop_human_seat_bench_male_left_elbow_on_knee", "prop_human_seat_bus_stop_wait", "prop_human_seat_bus_stop_wait_female_arms_folded",
			"prop_human_seat_bus_stop_wait_female_generic", "prop_human_seat_bus_stop_wait_female_legs_crossed", "prop_human_seat_bus_stop_wait_male_elbows_on_knees", "prop_human_seat_bus_stop_wait_male_generic", "prop_human_seat_bus_stop_wait_male_left_elbow_on_knee", "prop_human_seat_chair", "prop_human_seat_chair_all", "prop_human_seat_chair_and_beer", "prop_human_seat_chair_and_beer_and_food", "prop_human_seat_chair_and_coffee", "prop_human_seat_chair_and_coffee_and_food",
			"prop_human_seat_chair_and_food", "prop_human_seat_chair_any", "prop_human_seat_chair_drink", "prop_human_seat_chair_drink_beer", "prop_human_seat_chair_drink_beer_female", "prop_human_seat_chair_drink_beer_male", "prop_human_seat_chair_drink_female_generic", "prop_human_seat_chair_drink_male_generic", "prop_human_seat_chair_female_arms_folded", "prop_human_seat_chair_female_food", "prop_human_seat_chair_female_generic",
			"prop_human_seat_chair_female_generic_mp_player", "prop_human_seat_chair_female_generic_skinny", "prop_human_seat_chair_female_heels_mp_player", "prop_human_seat_chair_female_legs_crossed", "prop_human_seat_chair_food", "prop_human_seat_chair_male_elbows_on_knees", "prop_human_seat_chair_male_food", "prop_human_seat_chair_male_generic", "prop_human_seat_chair_male_generic_mp_player", "prop_human_seat_chair_male_generic_skinny", "prop_human_seat_chair_male_left_elbow_on_knee",
			"prop_human_seat_chair_mp_player", "prop_human_seat_chair_upright", "prop_human_seat_computer", "prop_human_seat_computer_female", "prop_human_seat_computer_male", "prop_human_seat_deckchair", "prop_human_seat_deckchair_any", "prop_human_seat_deckchair_drink", "prop_human_seat_deckchair_female", "prop_human_seat_deckchair_female_drink", "prop_human_seat_deckchair_male",
			"prop_human_seat_deckchair_male_generic_drink", "prop_human_seat_muscle_bench_press", "prop_human_seat_muscle_bench_press_prison", "prop_human_seat_sewing", "prop_human_seat_sewing_female", "prop_human_seat_strip_watch", "prop_human_seat_strip_watch_bit_thuggy", "prop_human_seat_strip_watch_bouncy_guy", "prop_human_seat_strip_watch_female", "prop_human_seat_sunlounger", "prop_human_seat_sunlounger_female",
			"prop_human_seat_sunlounger_male", "prop_human_seat_train", "prop_human_stand_impatient", "standing", "sweeping_litter_triggered", "walk", "welding_torch_spark_registered", "world_boar_grazing", "world_cat_sleeping_ground", "world_cat_sleeping_ledge", "world_chickenhawk_feeding",
			"world_chickenhawk_standing", "world_cormorant_standing", "world_cow_grazing", "world_coyote_howl", "world_coyote_rest", "world_coyote_walk", "world_coyote_wander", "world_crow_feeding", "world_crow_standing", "world_deer_grazing", "world_dog_barking_retriever",
			"world_dog_barking_rottweiler", "world_dog_barking_shepherd", "world_dog_barking_small", "world_dog_sitting__rottweiler", "world_dog_sitting_retriever", "world_dog_sitting_rottweiler", "world_dog_sitting_shepherd", "world_dog_sitting_small", "world_dolphin_swim", "world_fish_flee", "world_fish_idle",
			"world_gull_feeding", "world_gull_standing", "world_hen_flee", "world_hen_pecking", "world_hen_standing", "world_human_aa_coffee", "world_human_aa_smoke", "world_human_binoculars", "world_human_binoculars_female", "world_human_binoculars_male", "world_human_bum_freeway",
			"world_human_bum_slumped", "world_human_bum_slumped_laying_on_left_side", "world_human_bum_slumped_laying_on_right_side", "world_human_bum_standing", "world_human_bum_standing_male_depressed", "world_human_bum_standing_male_drunk", "world_human_bum_standing_male_twitchy", "world_human_bum_wash", "world_human_bum_wash_high", "world_human_bum_wash_low", "world_human_car_park_attendant",
			"world_human_cheering", "world_human_cheering_female_a", "world_human_cheering_female_b", "world_human_cheering_female_c", "world_human_cheering_female_d", "world_human_cheering_male_a", "world_human_cheering_male_b", "world_human_cheering_male_d", "world_human_cheering_male_e", "world_human_clipboard", "world_human_clipboard_male",
			"world_human_const_drill", "world_human_cop_idles", "world_human_cop_idles_female", "world_human_cop_idles_male", "world_human_drinking", "world_human_drinking_beer_female", "world_human_drinking_beer_male", "world_human_drinking_coffee_female", "world_human_drinking_coffee_male", "world_human_drinking_fat_beer_male", "world_human_drinking_fat_coffee_female",
			"world_human_drug_dealer", "world_human_drug_dealer_hard", "world_human_drug_dealer_soft", "world_human_gardener_leaf_blower", "world_human_gardener_plant", "world_human_gardener_plant_female", "world_human_gardener_plant_male", "world_human_golf_player", "world_human_golf_player_male", "world_human_guard_patrol", "world_human_guard_stand",
			"world_human_guard_stand_army", "world_human_hammering", "world_human_hang_out_street", "world_human_hang_out_street_female_arm_side", "world_human_hang_out_street_female_arms_crossed", "world_human_hang_out_street_female_hold_arm", "world_human_hang_out_street_male_a", "world_human_hang_out_street_male_b", "world_human_hang_out_street_male_c", "world_human_hiker", "world_human_hiker_female",
			"world_human_hiker_male", "world_human_hiker_standing", "world_human_hiker_standing_female", "world_human_hiker_standing_male", "world_human_human_statue", "world_human_janitor", "world_human_jog", "world_human_jog_female", "world_human_jog_male", "world_human_jog_standing", "world_human_jog_standing_female",
			"world_human_jog_standing_male", "world_human_jog_standing_male_fit", "world_human_leaning", "world_human_leaning_female_coffee", "world_human_leaning_female_hand_up", "world_human_leaning_female_holding_elbow", "world_human_leaning_female_mobile", "world_human_leaning_female_smoking", "world_human_leaning_female_texting", "world_human_leaning_male_beer", "world_human_leaning_male_coffee",
			"world_human_leaning_male_foot_up", "world_human_leaning_male_hands_together", "world_human_leaning_male_legs_crossed", "world_human_leaning_male_mobile", "world_human_leaning_male_smoking", "world_human_leaning_male_texting", "world_human_maid_clean", "world_human_mobile_film_shocking", "world_human_mobile_film_shocking_female", "world_human_mobile_film_shocking_male", "world_human_muscle_flex",
			"world_human_muscle_flex_arms_at_side", "world_human_muscle_flex_arms_in_front", "world_human_muscle_free_weights", "world_human_muscle_free_weights_male_barbell", "world_human_musician", "world_human_musician_male_bongos", "world_human_musician_male_guitar", "world_human_paparazzi", "world_human_partying", "world_human_partying_female_partying_beer", "world_human_partying_female_partying_cellphone",
			"world_human_partying_male_partying_beer", "world_human_picnic", "world_human_picnic_female", "world_human_picnic_male", "world_human_power_walker", "world_human_prostitute_cokehead", "world_human_prostitute_crackhooker", "world_human_prostitute_french", "world_human_prostitute_high_class", "world_human_prostitute_hooker", "world_human_prostitute_low_class",
			"world_human_push_ups", "world_human_seat_ledge", "world_human_seat_ledge_eating", "world_human_seat_ledge_eating_female", "world_human_seat_ledge_eating_male", "world_human_seat_ledge_female", "world_human_seat_ledge_male", "world_human_seat_steps", "world_human_seat_steps_female", "world_human_seat_steps_male_elbows_on_knees", "world_human_seat_steps_male_elbows_on_knees_skinny",
			"world_human_seat_steps_male_hands_in_lap", "world_human_seat_wall", "world_human_seat_wall_eating", "world_human_seat_wall_eating_female", "world_human_seat_wall_eating_male", "world_human_seat_wall_female", "world_human_seat_wall_male", "world_human_seat_wall_tablet", "world_human_seat_wall_tablet_female", "world_human_seat_wall_tablet_male", "world_human_security_shine_torch",
			"world_human_sit_ups", "world_human_smoking", "world_human_smoking_fat_female", "world_human_smoking_fat_male_a", "world_human_smoking_fat_male_b", "world_human_smoking_female", "world_human_smoking_male_a", "world_human_smoking_male_b", "world_human_smoking_pot", "world_human_smoking_pot_female", "world_human_smoking_pot_female_fat",
			"world_human_smoking_pot_male", "world_human_stand_fire", "world_human_stand_fire_male", "world_human_stand_fishing", "world_human_stand_impatient", "world_human_stand_impatient_female", "world_human_stand_impatient_male", "world_human_stand_impatient_upright", "world_human_stand_mobile", "world_human_stand_mobile_call_female", "world_human_stand_mobile_call_male",
			"world_human_stand_mobile_fat_call_female", "world_human_stand_mobile_fat_call_male", "world_human_stand_mobile_fat_text_female", "world_human_stand_mobile_fat_text_male", "world_human_stand_mobile_text_female", "world_human_stand_mobile_text_male", "world_human_stand_mobile_upright", "world_human_strip_watch_stand", "world_human_strip_watch_stand_male_a", "world_human_strip_watch_stand_male_b", "world_human_strip_watch_stand_male_c",
			"world_human_stupor", "world_human_stupor_male", "world_human_stupor_male_looking_left", "world_human_stupor_male_looking_right", "world_human_sunbathe", "world_human_sunbathe_back", "world_human_sunbathe_back", "world_human_sunbathe_back_female", "world_human_sunbathe_back_male", "world_human_sunbathe_female", "world_human_sunbathe_male",
			"world_human_superhero", "world_human_superhero_space_pistol", "world_human_superhero_space_rifle", "world_human_swimming", "world_human_tennis_player", "world_human_tourist_map", "world_human_tourist_map_female", "world_human_tourist_map_male", "world_human_tourist_mobile", "world_human_tourist_mobile_female", "world_human_tourist_mobile_male",
			"world_human_vehicle_mechanic", "world_human_welding", "world_human_window_shop_browse", "world_human_window_shop_browse_female", "world_human_window_shop_browse_male", "world_human_yoga", "world_human_yoga_female", "world_human_yoga_male", "world_lookat_point", "world_mountain_lion_rest", "world_mountain_lion_wander",
			"world_orca_swim", "world_pig_grazing", "world_pigeon_feeding", "world_pigeon_standing", "world_rabbit_eating", "world_rabbit_feeding", "world_rabbit_flee", "world_rats_eating", "world_rats_fleeing", "world_shark_hammerhead_swim", "world_shark_swim",
			"world_stingray_swim", "world_vehicle_ambulance", "world_vehicle_attractor", "world_vehicle_bicycle_bmx", "world_vehicle_bicycle_bmx_ballas", "world_vehicle_bicycle_bmx_family", "world_vehicle_bicycle_bmx_harmony", "world_vehicle_bicycle_bmx_vagos", "world_vehicle_bicycle_mountain", "world_vehicle_bicycle_road", "world_vehicle_bike_off_road_race",
			"world_vehicle_biker", "world_vehicle_boat_idle", "world_vehicle_boat_idle_alamo", "world_vehicle_boat_idle_marquis", "world_vehicle_broken_down", "world_vehicle_businessmen", "world_vehicle_cluckin_bell_trailer", "world_vehicle_construction_passengers", "world_vehicle_construction_solo", "world_vehicle_distant_empty_ground", "world_vehicle_drive_passengers",
			"world_vehicle_drive_passengers_limited", "world_vehicle_drive_solo", "world_vehicle_empty", "world_vehicle_farm_worker", "world_vehicle_fire_truck", "world_vehicle_heli_lifeguard", "world_vehicle_mariachi", "world_vehicle_mechanic", "world_vehicle_military_planes_big", "world_vehicle_military_planes_small", "world_vehicle_park_parallel",
			"world_vehicle_park_perpendicular_nose_in", "world_vehicle_passenger_exit", "world_vehicle_police", "world_vehicle_police_bike", "world_vehicle_police_car", "world_vehicle_police_next_to_car", "world_vehicle_quarry", "world_vehicle_salton", "world_vehicle_salton_dirt_bike", "world_vehicle_security_car", "world_vehicle_streetrace",
			"world_vehicle_tandl", "world_vehicle_tourbus", "world_vehicle_tourist", "world_vehicle_tractor", "world_vehicle_tractor_beach", "world_vehicle_truck_logs", "world_vehicle_trucks_trailers", "world_whale_swim" };
#pragma endregion
#pragma region named scenariovector
		std::vector<NamedScenario> vNamedScenarios
		{
			{ "Drilling", "WORLD_HUMAN_CONST_DRILL" },
			{ "Hammering", "WORLD_HUMAN_HAMMERING" },
			{ "Mechanic", "WORLD_HUMAN_VEHICLE_MECHANIC" },
			{ "Janitor", "WORLD_HUMAN_JANITOR" },
			{ "Hang Out", "WORLD_HUMAN_HANG_OUT_STREET" },
			{ "Play Guitar", "WORLD_HUMAN_MUSICIAN_MALE_BONGOS" },
			{ "Play Bongos", "WORLD_HUMAN_MUSICIAN_MALE_GUITAR" },
			{ "Clipboard", "WORLD_HUMAN_CLIPBOARD" },
			{ "Smoking", "WORLD_HUMAN_SMOKING" },
			{ "Smoking 2", "WORLD_HUMAN_AA_SMOKE" },
			{ "Smoking Weed", "WORLD_HUMAN_SMOKING_POT" },
			{ "Standing With Phone", "WORLD_HUMAN_STAND_MOBILE" },
			{ "Standing With Phone 2", "WORLD_HUMAN_STAND_MOBILE_UPRIGHT" },
			{ "Standing Guard", "WORLD_HUMAN_GUARD_STAND" },
			{ "Standing Impatiently", "WORLD_HUMAN_STAND_IMPATIENT" },
			{ "Standing Impatiently 2", "WORLD_HUMAN_STAND_IMPATIENT_UPRIGHT" },
			{ "Soldier Stand", "WORLD_HUMAN_GUARD_STAND_ARMY" },
			{ "Hobo Stand", "WORLD_HUMAN_BUM_STANDING" },
			{ "Doing Pushups", "WORLD_HUMAN_PUSH_UPS" },
			{ "Lifting Weights", "WORLD_HUMAN_MUSCLE_FREE_WEIGHTS" },
			{ "Flexing", "WORLD_HUMAN_MUSCLE_FLEX" },
			{ "Drug Dealer", "WORLD_HUMAN_DRUG_DEALER_HARD" },
			{ "Hooker", "WORLD_HUMAN_PROSTITUTE_LOW_CLASS" },
			{ "Hooker 2", "WORLD_HUMAN_PROSTITUTE_HIGH_CLASS" },
			{ "Drunk", "WORLD_HUMAN_STUPOR" },
			{ "Drinking", "WORLD_HUMAN_DRINKING" },
			{ "Drinking Coffee", "WORLD_HUMAN_AA_COFFEE" },
			{ "Binoculars", "WORLD_HUMAN_BINOCULARS" },
			{ "Welding", "WORLD_HUMAN_WELDING" },
			{ "Shocked", "WORLD_HUMAN_MOBILE_FILM_SHOCKING" },
			{ "Taking Pictures", "WORLD_HUMAN_PAPARAZZI" },
			{ "Medic", "CODE_HUMAN_MEDIC_KNEEL" },
			{ "Window Shopping", "WORLD_HUMAN_WINDOW_SHOP_BROWSE" },
			{ "Cleaning", "WORLD_HUMAN_MAID_CLEAN" },
			{ "Doing Yoga", "WORLD_HUMAN_YOGA" },
			{ "Tourist Map", "WORLD_HUMAN_TOURIST_MAP" },
			{ "Tennis Player", "WORLD_HUMAN_TENNIS_PLAYER" },
			{ "Sunbathing", "WORLD_HUMAN_SUNBATHE" },
			{ "Sunbathing 2", "WORLD_HUMAN_SUNBATHE_BACK" },
			{ "Fishing", "WORLD_HUMAN_STAND_FISHING" },
			{ "Shining Torch", "WORLD_HUMAN_SECURITY_SHINE_TORCH" },
			{ "Picnic", "WORLD_HUMAN_PICNIC" },
			{ "Partying", "WORLD_HUMAN_PARTYING" },
			{ "Leaning", "WORLD_HUMAN_LEANING" },
			{ "Jog Standing", "WORLD_HUMAN_JOG_STANDING" },
			{ "Human Statue", "WORLD_HUMAN_HUMAN_STATUE" },
			{ "Hanging Out (Street)", "WORLD_HUMAN_HANG_OUT_STREET" },
			{ "Golf Player", "WORLD_HUMAN_GOLF_PLAYER" },
			{ "Gardening", "WORLD_HUMAN_GARDENER_PLANT" },
			{ "Drug Dealing", "WORLD_HUMAN_DRUG_DEALER_HARD" },
			{ "Cheering", "WORLD_HUMAN_CHEERING" },
			{ "Parking Attendant", "WORLD_HUMAN_CAR_PARK_ATTENDANT" },
			{ "Wash", "WORLD_HUMAN_BUM_WASH" },
			{ "Holding Sign", "WORLD_HUMAN_BUM_FREEWAY" },
			{ "Laying Down (Hobo)", "WORLD_HUMAN_BUM_SLUMPED" },
			{ "BBQ", "PROP_HUMAN_BBQ" },
		};
#pragma endregion

		auto& searchStr = dict;

		void AddScenarioOption(const std::string& text, const std::string& scenarioLabel, int delay = -1, bool playEnterAnim = true)
		{
			bool pressed = false;
			AddTickol(text, IS_PED_USING_SCENARIO(g_Ped1, scenarioLabel.c_str()), pressed, pressed, TICKOL::MANWON);
			if (pressed)
			{
				GTAped ped = g_Ped1;
				GTAentity att;
				auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
				if (spi >= 0)
				{
					auto& spe = sub::Spooner::Databases::EntityDb[spi];
					Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
				}

				ped.Task().ClearAllImmediately();
				if (!ped.Task().IsUsingScenario(scenarioLabel))
				{
					TASK_START_SCENARIO_IN_PLACE(g_Ped1, scenarioLabel.c_str(), delay, playEnterAnim);
				}

				if (scenarioLabel.find("MUSICIAN") != std::string::npos)
				{
					ped.Task().ClearAllImmediately();
					TASK_START_SCENARIO_IN_PLACE(g_Ped1, "WORLD_HUMAN_MUSICIAN", delay, playEnterAnim);
				}

				if (spi >= 0)
				{
					auto& spe = sub::Spooner::Databases::EntityDb[spi];
					spe.lastAnimation.dict.clear();
					spe.lastAnimation.name = scenarioLabel;
					if (att.Exists() && spe.attachmentArgs.isAttached)
					{
						spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
					}
					spe.taskSequence.Reset();
					if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
					{
						sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
						sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
						sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
					}
				}

			}
		}

		void stopScenarioPls()
		{
			GTAped ped = g_Ped1;
			GTAentity att;
			auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(ped);
			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
			}

			GTAvehicle veh = ped.CurrentVehicle();
			bool isInVehicle = veh.Exists();
			VehicleSeat vehSeat;
			if (isInVehicle)
			{
				vehSeat = ped.GetCurrentVehicleSeat();
			}
			ped.Task().ClearAllImmediately();
			if (isInVehicle)
			{
				ped.SetIntoVehicle(veh, vehSeat);
			}

			if (spi >= 0)
			{
				auto& spe = sub::Spooner::Databases::EntityDb[spi];
				spe.lastAnimation.dict.clear();
				spe.lastAnimation.name.clear();
				if (att.Exists() && spe.attachmentArgs.isAttached)
				{
					spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
				}
				spe.taskSequence.Reset();
				if (sub::Spooner::selectedEntity.handle.Equals(spe.handle))
				{
					sub::Spooner::selectedEntity.lastAnimation.dict = spe.lastAnimation.dict;
					sub::Spooner::selectedEntity.lastAnimation.name = spe.lastAnimation.name;
					sub::Spooner::selectedEntity.taskSequence = spe.taskSequence;
				}
			}
		}

		void AnimationTaskScenarios1()
		{
			bool clearSearchStr = false;
			bool musicianOn = false;

			AddTitle("Scenarios");
			AddOption("ALL SCENARIOS", clearSearchStr, nullFunc, SUB::AnimationTaskScenarios2);
			if (clearSearchStr)
			{
				searchStr.clear();
			}

			AddTickol("End Scenarios", true, stopScenarioPls, stopScenarioPls, TICKOL::CROSS);

			for (auto& scen : vNamedScenarios)
			{
				AddScenarioOption(scen.name, scen.label);
			}
		}

		void AnimationTaskScenarios2()
		{
			bool searchObj = false;

			AddTitle("All Scenarios");
			AddOption(searchStr.empty() ? "SEARCH" : boost::to_upper_copy(searchStr), searchObj, nullFunc, -1, true); 
			if (searchObj)
			{
				searchStr = Game::InputBox(searchStr, 126U, "SEARCH", searchStr);
				boost::to_lower(searchStr);
			}

			AddTickol("End Scenarios", true, stopScenarioPls, stopScenarioPls, TICKOL::CROSS);

			for (auto& current : vValues_TaskScenarios)
			{
				if (!searchStr.empty()) 
				{ 
					if (current.find(searchStr) == std::string::npos) 
					continue; 
				}
				AddScenarioOption(current, current);
			}
		}
	}

	std::string GetPedMovementClipSet(const GTAentity& ped)
	{
		auto it = g_pedListMovGroup.find(ped.GetHandle());
		if (it != g_pedListMovGroup.end())
		{
			return it->second;
		}
		return std::string();
	}

	void SetPedMovementClipSet(GTAentity ped, const std::string& setName)
	{
		Game::RequestAnimSet(setName);
		SET_PED_MOVEMENT_CLIPSET(ped.Handle(), setName.c_str(), 0x3E800000);
		WAIT(30);
		Vector3 coord = GET_ENTITY_COORDS(ped.Handle(), 1);
		SET_ENTITY_COORDS(g_Ped1, coord.x, coord.y, coord.z + 0.05f, 0, 0, 0, 1);
		FREEZE_ENTITY_POSITION(ped.Handle(), 0);
		g_pedListMovGroup[ped.Handle()] = setName;
	}

	std::string GetPedWeaponMovementClipSet(const GTAentity& ped)
	{
		auto it = g_pedListWMovGroup.find(ped.GetHandle());
		if (it != g_pedListWMovGroup.end())
		{
			return it->second;
		}
		return std::string();
	}

	void SetPedWeaponMovementClipSet(GTAentity ped, const std::string& setName)
	{
		Game::RequestAnimSet(setName);
		SET_PED_WEAPON_MOVEMENT_CLIPSET(ped.Handle(), setName.c_str());
		WAIT(30);
		const Vector3& coord = GET_ENTITY_COORDS(ped.Handle(), 1);
		SET_ENTITY_COORDS(g_Ped1, coord.x, coord.y, coord.z + 0.05f, 0, 0, 0, 1);
		FREEZE_ENTITY_POSITION(ped.Handle(), 0);
		g_pedListWMovGroup[ped.Handle()] = setName;
	}

	void AddMoveGroupOption(const std::string& text, std::string moveGroup = "", bool &extraOptionCode = null)
	{
		if (moveGroup.length() == 0)
		{
			moveGroup = text;
		}

		bool moveGroupIsActive = GetPedMovementClipSet(g_Ped1) == moveGroup;
		bool pressed = false;
		AddTickol(text, moveGroupIsActive, pressed, pressed); if (pressed)
		{
			SetPedMovementClipSet(g_Ped1, moveGroup);
			extraOptionCode = true;
		}
	}

	void AddWMoveGroupOption(const std::string& text, std::string moveGroup = "", bool &extraOptionCode = null)
	{
		if (moveGroup.length() == 0)
		{
			moveGroup = text;
		}

		bool bMovGrpIsActive = GetPedWeaponMovementClipSet(g_Ped1) == moveGroup;

		bool pressed = false;
		AddTickol(text, bMovGrpIsActive, pressed, pressed); if (pressed)
		{
			SetPedWeaponMovementClipSet(g_Ped1, moveGroup);
			extraOptionCode = true;
		}
	}

	void MovementGroupMenu()
	{
		auto mgit = g_pedListMovGroup.find(g_Ped1);
		bool mgitIsValid = mgit != g_pedListMovGroup.end();

		auto wmgit = g_pedListWMovGroup.find(g_Ped1);
		bool wmgitIsValid = mgit != g_pedListWMovGroup.end();

		bool movementGroupReset = 0;
		bool movementGroupResetW = 0;

		AddTitle("Movement Styles");
		AddTickol("Default", !mgitIsValid, movementGroupReset, movementGroupReset);
		AddMoveGroupOption("Generic Male", "move_m@generic");
		AddMoveGroupOption("Generic Female", "move_f@generic");
		AddMoveGroupOption("Policeman", "move_cop@action");
		AddMoveGroupOption("Drunk", "move_m@drunk@a");
		AddMoveGroupOption("Moderate Drunk", "move_m@drunk@moderatedrunk");
		AddMoveGroupOption("Moderate Drunk 2", "move_m@drunk@moderatedrunk_head_up");
		AddMoveGroupOption("Slightly Drunk", "move_m@drunk@slightlydrunk");
		AddMoveGroupOption("Very Drunk", "move_m@drunk@verydrunk");
		AddMoveGroupOption("Gangster", "move_m@gangster@generic");
		AddMoveGroupOption("Hipster", "move_m@hipster@a");
		AddMoveGroupOption("Hobo", "move_m@hobo@a");
		AddMoveGroupOption("Hobo2", "move_m@hobo@b");
		AddMoveGroupOption("Obese", "move_m@fat@a");
		AddMoveGroupOption("Obese2", "move_f@fat@a");
		AddMoveGroupOption("Lester", "move_lester_CaneUp");
		AddMoveGroupOption("Film Female", "move_f@film_reel");
		AddMoveGroupOption("Cool Jog", "move_m@jog@");
		AddMoveGroupOption("Leaf Blower", "move_m@leaf_blower");
		AddMoveGroupOption("Tool Belt Walk", "move_m@tool_belt@a");
		AddMoveGroupOption("Tool Belt Walk 2", "move_f@tool_belt@a");
		AddMoveGroupOption("Appealing", "move_f@sexy@a");
		AddMoveGroupOption("Amanda - Bag", "move_characters@amanda@bag");
		AddMoveGroupOption("Michael - Fire", "move_characters@michael@fire");
		AddMoveGroupOption("Franklin - Fire", "move_characters@franklin@fire");
		AddMoveGroupOption("Jimmy - Nervous", "move_characters@jimmy@nervous@");
		AddMoveGroupOption("Jimmy - Slow", "move_characters@jimmy@slow@");
		AddMoveGroupOption("Alien", "move_m@alien");
		AddMoveGroupOption("Brave", "move_m@brave");
		AddMoveGroupOption("Brave2", "move_m@brave@a");
		AddMoveGroupOption("Brave3", "move_m@brave@b");
		AddMoveGroupOption("Brave4", "move_m@brave@fallback");
		AddMoveGroupOption("BraveStill", "move_m@brave@idle_a");
		AddMoveGroupOption("BraveStill2", "move_m@brave@idle_b");
		AddMoveGroupOption("Business", "move_m@business@a");
		AddMoveGroupOption("Business2", "move_m@business@b");
		AddMoveGroupOption("Business3", "move_m@business@c");
		AddMoveGroupOption("Casual", "move_m@casual@a");
		AddMoveGroupOption("Casual2", "move_m@casual@b");
		AddMoveGroupOption("Casual3", "move_m@casual@c");
		AddMoveGroupOption("Casual4", "move_m@casual@d");
		AddMoveGroupOption("Casual5", "move_m@casual@e");
		AddMoveGroupOption("Casual6", "move_m@casual@f");
		AddMoveGroupOption("Clipboard", "move_m@clipboard");
		AddMoveGroupOption("Coward", "move_m@coward");
		AddMoveGroupOption("Burning", "move_m@fire");
		AddMoveGroupOption("Flee", "move_m@flee@a");
		AddMoveGroupOption("Flee2", "move_m@flee@b");
		AddMoveGroupOption("Flee3", "move_m@flee@c");
		AddMoveGroupOption("Flee4", "move_f@flee@a");
		AddMoveGroupOption("Flee5", "move_f@flee@b");
		AddMoveGroupOption("Flee6", "move_f@flee@c");
		AddMoveGroupOption("Hiking", "move_m@hiking");
		AddMoveGroupOption("Hiking2", "move_f@hiking");
		AddMoveGroupOption("Hurry", "move_m@hurry@a");
		AddMoveGroupOption("Hurry2", "move_m@hurry@b");
		AddMoveGroupOption("Hurry3", "move_m@hurry@c");
		AddMoveGroupOption("Hurry4", "move_f@hurry@a");
		AddMoveGroupOption("Hurry5", "move_f@hurry@b");
		AddMoveGroupOption("Injured", "move_m@injured");
		AddMoveGroupOption("Injured2", "move_injured_generic");
		AddMoveGroupOption("Injured3", "move_f@injured");
		AddMoveGroupOption("Intimidation", "move_m@intimidation@1h");
		AddMoveGroupOption("Intimidation2", "move_m@intimidation@cop@unarmed");
		AddMoveGroupOption("Intimidation3", "move_m@intimidation@unarmed");
		AddMoveGroupOption("Muscular", "move_m@muscle@a");
		AddMoveGroupOption("Quick", "move_m@quick");
		AddMoveGroupOption("Sad", "move_m@sad@a");
		AddMoveGroupOption("Sad2", "move_m@sad@b");
		AddMoveGroupOption("Sad3", "move_m@sad@c");
		AddMoveGroupOption("Sad4", "move_f@sad@a");
		AddMoveGroupOption("Sad5", "move_f@sad@b");
		AddMoveGroupOption("Shady", "move_m@shadyped@a");
		AddMoveGroupOption("Shocked", "move_m@shocked@a");
		AddMoveGroupOption("Arrogant", "move_f@arrogant@a");
		AddMoveGroupOption("Chubby", "move_f@chubby@a");
		AddMoveGroupOption("Handbag Walk", "move_f@handbag");
		AddMoveGroupOption("Heels", "move_f@heels@c");
		AddMoveGroupOption("move_p_m_one");
		AddMoveGroupOption("move_p_m_one_briefcase");
		AddMoveGroupOption("move_p_m_two");
		AddMoveGroupOption("move_p_m_zero");
		AddMoveGroupOption("move_p_m_zero_slow");
		AddMoveGroupOption("Ballistic", "anim_group_move_ballistic");

		AddBreak("Weapon Handling");
		AddTickol("Default", !wmgitIsValid, movementGroupResetW, movementGroupResetW);
		AddWMoveGroupOption("Lester's Cane", "move_lester_CaneUp");
		AddWMoveGroupOption("Crouched", "move_ped_crouched");
		AddWMoveGroupOption("Bucket", "move_ped_wpn_bucket");
		AddWMoveGroupOption("Mop", "move_ped_wpn_mop");
		AddWMoveGroupOption("Assault Rifle (Crouched)", "Wpn_AssaultRifle_WeaponHoldingCrouched");
		AddWMoveGroupOption("Garbageman", "missfbi4prepp1_garbageman");
		AddWMoveGroupOption("Prison Guard", "MOVE_M@PRISON_GAURD");
		AddWMoveGroupOption("Jerrycan (Generic)", "move_ped_wpn_jerrycan_generic");
		AddWMoveGroupOption("Golfer", "move_m@golfer@");
		AddWMoveGroupOption("Rucksack", "MOVE_P_M_ZERO_RUCKSACK");
		AddWMoveGroupOption("Clipboard", "MOVE_M@CLIPBOARD");
		AddWMoveGroupOption("Tennis (Male)", "weapons@tennis@male");
		AddWMoveGroupOption("Tennis Locomotion (Female)", "TENNIS_LOCOMOTION_FEMALE");
		AddWMoveGroupOption("Paparazzi (Standing)", "random@escape_paparazzi@standing@");
		AddWMoveGroupOption("Paparazzi (In Car)", "random@escape_paparazzi@incar@");
		AddWMoveGroupOption("Leaf Blower", "MOVE_M@LEAF_BLOWER");

		AddBreak("Weapon Animations (Doesn't Save)");
		const std::vector<std::pair<std::string, Hash>> vWeaponAnimsOrWhatever
		{
			{ "Default", 0xE4DF46D5 }, // default
			{ "Female", 0x6D155A1B }, // female
			{ "MP Freemode Female", 0xACB10C83 }, // mp_f_freemode
			{ "Ballistic", 0x5534A626 }, // ballistic
			{ "Unknown 1", 0xc531a409 }, // 0xc531a409
			{ "Unknown 2", 0x529e5780 }, // 0x529e5780
		};
		for (auto& wa : vWeaponAnimsOrWhatever)
		{
			bool weaponAnimPressed = false;
			AddOption(wa.first, weaponAnimPressed); 
			if (weaponAnimPressed)
			{
				WEAPON::SET_WEAPON_ANIMATION_OVERRIDE(g_Ped1, wa.second);
			}
		}


		if (movementGroupReset)
		{
			RESET_PED_MOVEMENT_CLIPSET(g_Ped1, 0x3E800000);
			WAIT(10);
			Vector3 Coord = GET_ENTITY_COORDS(g_Ped1, 1);
			SET_ENTITY_COORDS_NO_OFFSET(g_Ped1, Coord.x, Coord.y, Coord.z + 0.05f, 1, 1, 0);
			FREEZE_ENTITY_POSITION(g_Ped1, 0);
			if (mgitIsValid) 
			{
				g_pedListMovGroup.erase(mgit);
			}
		}
		if (movementGroupResetW)
		{
			RESET_PED_WEAPON_MOVEMENT_CLIPSET(g_Ped1);
			WAIT(10);
			Vector3 Coord = GET_ENTITY_COORDS(g_Ped1, 1);
			SET_ENTITY_COORDS_NO_OFFSET(g_Ped1, Coord.x, Coord.y, Coord.z + 0.05f, 1, 1, 0);
			FREEZE_ENTITY_POSITION(g_Ped1, 0);
			if (wmgitIsValid) 
			{
				g_pedListWMovGroup.erase(wmgit);
			}
		}
	}

	namespace FacialAnims
	{
		const std::vector<NamedFacialAnim> vFacialAnims
		{
			{ "Normal", "mood_normal_1" },
			{ "Aiming", "mood_aiming_1" },
			{ "Angry", "mood_angry_1" },
			{ "Happy", "mood_happy_1" },
			{ "Injured", "mood_injured_1" },
			{ "Stressed", "mood_stressed_1" },
			{ "Smug", "mood_smug_1" },
			{ "Sulk", "mood_sulk_1" },
			{ "Sleeping", "mood_sleeping_1" },
			{ "Drunk", "mood_drunk_1" },
			{ "Burning", "burning_1" },
			{ "Dead", "dead_1" }
		};

		void FacialMoodMenu()
		{
			GTAentity ped = g_Ped1;
			auto current = GetPedFacialMood(ped);

			AddTitle("Mood");

			bool clearPressed = false;
			AddTickol("Default", current.empty(), clearPressed, clearPressed); 
			if (clearPressed)
			{
				ped.RequestControl(400);
				clear_ped_facial_mood(ped);
			}

			for (auto& fa : vFacialAnims)
			{
				bool bFaPressed = false;
				AddTickol(fa.caption, current == fa.animName, bFaPressed, bFaPressed); if (bFaPressed)
				{
					ped.RequestControl(400);
					SetPedFacialMood(ped, fa.animName);
					current = fa.animName;
				}
			}

		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(ANIMATIONSUB,                     sub::PedAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_SETTINGS,            sub::AnimationSub_Settings)
REGISTER_SUBMENU(ANIMATIONSUB_FAVOURITES,          sub::AnimationFavouritesMenu)
REGISTER_SUBMENU(ANIMATIONSUB_CUSTOM,              sub::AnimationSub_Custom)
REGISTER_SUBMENU(ANIMATIONSUB_DEER,                sub::DeerAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_SHARK,               sub::SharkAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_MISSRAPPEL,          sub::MissionRappelAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_GESTSIT,             sub::GestureSitAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_SWAT,                sub::SwatAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_GUARDREAC,           sub::GuardReactAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_RANDARREST,          sub::RandomArrestAnimationMenu)
REGISTER_SUBMENU(ANIMATIONSUB_ALLPEDANIMS,         sub::AnimationMenu::AllPedAnimsMenu)
REGISTER_SUBMENU(ANIMATIONSUB_ALLPEDANIMS_INDICT,  sub::AnimationMenu::Sub_AllPedAnims_InDict)
REGISTER_SUBMENU(AnimationTaskScenarios,		   sub::AnimationTaskScenarios::AnimationTaskScenarios1)
REGISTER_SUBMENU(AnimationTaskScenarios2,		   sub::AnimationTaskScenarios::AnimationTaskScenarios2)
REGISTER_SUBMENU(FACIALMOOD,                       sub::FacialAnims::FacialMoodMenu)
REGISTER_SUBMENU(MOVEMENTGROUP,                    sub::MovementGroupMenu)