/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "StatManager.h"

namespace sub
{
	namespace SpStatManager
	{
		struct NamedCharStatList_t 
		{ 
			std::string title; 
			std::vector<CharStat_t> list; 
		};
	
		const std::array<NamedCharStatList_t, 5> vCharStatLists
		{ {
			{ "Cash",{
				{ "TOTAL_CASH", "Total Cash", StatDataType_t::INT, 0, static_cast<float>(INT_MAX) }
			} },
			{ "Abilities (ALPHA)",{
				{ "STAMINA", "Stamina", StatDataType_t::INT, 0, 100 },
				{ "STRENGTH", "Strength", StatDataType_t::INT, 0, 100 },
				{ "LUNG_CAPACITY", "Lung Capacity", StatDataType_t::INT, 0, 100 },
				{ "WHEELIE_ABILITY", "Wheelieing", StatDataType_t::INT, 0, 100 },
				{ "FLYING_ABILITY", "Flying", StatDataType_t::INT, 0, 100 },
				{ "SHOOTING_ABILITY", "Shooting", StatDataType_t::INT, 0, 100 },
				{ "STEALTH_ABILITY", "Stealth", StatDataType_t::INT, 0, 100 }
			} },
			{ "Special Ability",{
				{ "SPECIAL_ABILITY", "Amount Not Unlocked (ALPHA)", StatDataType_t::INT, 0, 100 },
				{ "SPECIAL_ABILITY_UNLOCKED", "Special Capacity", StatDataType_t::INT, 0, 100 }
			} },
			{ "K/D Ratio",{
				{ "KILLS", "Kill Count", StatDataType_t::INT, 0, static_cast<float>(INT_MAX) },
				{ "DEATHS", "Death Count", StatDataType_t::INT, 0, static_cast<float>(INT_MAX) }
			} },
			{ "Properties",{
				{ "PROP_BOUGHT_TRAF", "Arms Trafficking", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_CSCR", "Car Scrap Yard", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_WEED", "Weed Shop", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_TAXI", "Taxi Lot", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_CMSH", "Car Mod Shop", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_SOCO", "Sonar Collections", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_TOWI", "Towing Impound", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_GOLF", "Golf Club", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_CINV", "Vinewood Cinema", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_CIND", "Downtown Cinema", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_CINM", "Morningwood Cinema", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_BARTE", "Tequilala Bar", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_BARPI", "Pitchers Bar", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_BARHE", "Hen House Bar", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_BARHO", "Hookies Bar", StatDataType_t::BOOL, 0, 1 },
				{ "PROP_BOUGHT_STRIP", "Strip Club", StatDataType_t::BOOL, 0, 1 }
			} }
			} };

		std::pair<std::string, std::string> charNames[5] = { { "SP0_", "Michael" },{ "SP1_", "Franklin" },{ "SP2_", "Trevor" },{ "MP0_", "MP Char 1" },{ "MP1_", "MP Char 2" } };
		std::pair<std::string, std::string>* selectedCharName;
		const NamedCharStatList_t* selectedStatList;

		bool IsOnlineChar(const std::string& prefix)
		{
			return prefix == "MP0_" || prefix == "MP1_";
		}

		const std::array<NamedCharStatList_t, 8> vMPPropertyLists
		{ {
			{ "Apartments / Garages",{
				{ "PROPERTY_HOUSE", "Primary Apartment", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_1", "Property Slot 2", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_2", "Property Slot 3", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_3", "Property Slot 4", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_4", "Property Slot 5", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_5", "Property Slot 6", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_6", "Property Slot 7", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_7", "Property Slot 8", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_8", "Property Slot 9", StatDataType_t::INT, 0, 200 },
				{ "MULTI_PROPERTY_9", "Property Slot 10", StatDataType_t::INT, 0, 200 }
			} },
			{ "CEO",{
				{ "PROP_OFFICE", "Office", StatDataType_t::INT, 0, 200 },
				{ "PROP_OFFICE_VAR", "Office Style", StatDataType_t::INT, 0, 10 },
				{ "PROP_OFFICE_GAR1", "Office Garage 1", StatDataType_t::INT, 0, 10 },
				{ "PROP_OFFICE_GAR2", "Office Garage 2", StatDataType_t::INT, 0, 10 },
				{ "PROP_OFFICE_GAR3", "Office Garage 3", StatDataType_t::INT, 0, 10 },
				{ "PROP_OFFICE_MODSHOP", "Mod Shop", StatDataType_t::INT, 0, 10 },
				{ "PROP_OFFICE_ACCOMMODATION", "Accommodation", StatDataType_t::INT, 0, 10 }
			} },
			{ "MC / Businesses",{
				{ "PROP_CLUBHOUSE", "Clubhouse", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT0", "Business Slot 1", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT1", "Business Slot 2", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT2", "Business Slot 3", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT3", "Business Slot 4", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT4", "Business Slot 5", StatDataType_t::INT, 0, 200 },
				{ "PROP_FAC_SLOT5", "Business Slot 6", StatDataType_t::INT, 0, 200 }
			} },
			{ "Warehouses",{
				{ "PROP_WHOUSE_SLOT0", "Warehouse Slot 1", StatDataType_t::INT, 0, 200 },
				{ "PROP_WHOUSE_SLOT1", "Warehouse Slot 2", StatDataType_t::INT, 0, 200 },
				{ "PROP_WHOUSE_SLOT2", "Warehouse Slot 3", StatDataType_t::INT, 0, 200 },
				{ "PROP_WHOUSE_SLOT3", "Warehouse Slot 4", StatDataType_t::INT, 0, 200 },
				{ "PROP_WHOUSE_SLOT4", "Warehouse Slot 5", StatDataType_t::INT, 0, 200 },
				{ "PROP_IE_WAREHOUSE", "Vehicle Warehouse", StatDataType_t::INT, 0, 200 }
			} },
			{ "Military / Smuggling",{
				{ "PROP_DEFUNCBASE", "Bunker / Facility", StatDataType_t::INT, 0, 200 },
				{ "PROP_HANGAR", "Hangar", StatDataType_t::INT, 0, 10 }
			} },
			{ "Entertainment",{
				{ "PROP_NIGHTCLUB", "Nightclub", StatDataType_t::INT, 0, 200 },
				{ "PROP_CASINO_GAR1", "Casino Penthouse Garage", StatDataType_t::INT, 0, 10 },
				{ "PROP_ARCADE_GAR1", "Arcade Garage", StatDataType_t::INT, 0, 10 },
				{ "ARCADE_OWNED", "Arcade Owned", StatDataType_t::INT, 0, 1 }
			} },
			{ "Newer Properties",{
				{ "PROP_AUTO_SHOP", "Auto Shop", StatDataType_t::INT, 0, 200 },
				{ "PROP_FIXER_HQ", "Agency", StatDataType_t::INT, 0, 200 },
				{ "FIXER_HQ_OWNED", "Agency Owned", StatDataType_t::INT, 0, 1 },
				{ "MULTSTOREY_GAR_OWNED", "Multi-Storey Garage", StatDataType_t::INT, 0, 1 },
				{ "SALVAGE_YARD_OWNED", "Salvage Yard", StatDataType_t::INT, 0, 1 },
				{ "PROP_BAIL_OFFICE", "Bail Office", StatDataType_t::INT, 0, 200 },
				{ "BAIL_OFFICE_OWNED", "Bail Office Owned", StatDataType_t::INT, 0, 1 }
			} },
			{ "Mansions",{
				{ "MANSION_TH_OWNED", "Tongva Estate", StatDataType_t::INT, 0, 1 },
				{ "MANSION_AJ_OWNED", "Richman Villa", StatDataType_t::INT, 0, 1 },
				{ "MANSION_MD_OWNED", "Vinewood Residence", StatDataType_t::INT, 0, 1 }
			} }
		} };

		// Setters/Getters
		int StatGetInt(const std::string& name)
		{
			int tempp;
			STAT_GET_INT(GET_HASH_KEY(name), &tempp, -1);
			return tempp;
		}

		bool StatGetBool(const std::string& name)
		{
			int tempp;
			STAT_GET_BOOL(GET_HASH_KEY(name), &tempp, -1);
			return tempp != 0;
		}

		float StatGetFloat(const std::string& name)
		{
			float tempp;
			STAT_GET_FLOAT(GET_HASH_KEY(name), &tempp, -1);
			return tempp;
		}

		std::string StatGetString(const std::string& name)
		{
			return STAT_GET_STRING(GET_HASH_KEY(name), -1);
		}

		void SetAbilitySourceStats(const std::string& prefix, const std::string& statName, int value)
		{
			if (statName == "STAMINA")
			{
				STAT_SET_FLOAT(GET_HASH_KEY(prefix + "DIST_RUNNING"), value * 175.0f, true);
				STAT_SET_INT(GET_HASH_KEY(prefix + "TIME_SWIMMING"), value, true);
			}
			else if (statName == "STRENGTH")
				STAT_SET_INT(GET_HASH_KEY(prefix + "UNARMED_HITS"), value * 25, true);
			else if (statName == "LUNG_CAPACITY")
				STAT_INCREMENT(GET_HASH_KEY(prefix + "TIME_UNDERWATER"), value * 60.0f);
			else if (statName == "WHEELIE_ABILITY")
				STAT_SET_INT(GET_HASH_KEY(prefix + "NUMBER_NEAR_MISS"), value * 60, true);
			else if (statName == "FLYING_ABILITY")
			{
				STAT_SET_INT(GET_HASH_KEY(prefix + "TIME_DRIVING_PLANE"), value * 10, true);
				STAT_SET_INT(GET_HASH_KEY(prefix + "TIME_DRIVING_HELI"), value * 10, true);
				STAT_SET_INT(GET_HASH_KEY(prefix + "PLANE_LANDINGS"), value, true);
			}
			else if (statName == "SHOOTING_ABILITY")
			{
				STAT_SET_INT(GET_HASH_KEY(prefix + "HITS_MISSION"), value * 50, true);
				STAT_SET_INT(GET_HASH_KEY(prefix + "HITS_PEDS_VEHICLES"), value * 100, true);
			}
			else if (statName == "STEALTH_ABILITY")
			{
				STAT_SET_FLOAT(GET_HASH_KEY(prefix + "DIST_WALK_ST"), value * 50.0f, true);
				STAT_SET_INT(GET_HASH_KEY(prefix + "KILLS_STEALTH"), value * 2, true);
			}
			else if (statName == "SPECIAL_ABILITY_UNLOCKED")
				STAT_SET_INT(GET_HASH_KEY(prefix + "SPECIAL_ABILITY_UNLOCKED"), value, true);
		}

		bool IsAbilityStat(const std::string& statName)
		{
			return statName == "STAMINA" || statName == "STRENGTH" || statName == "LUNG_CAPACITY" ||
				statName == "WHEELIE_ABILITY" || statName == "FLYING_ABILITY" ||
				statName == "SHOOTING_ABILITY" || statName == "STEALTH_ABILITY" ||
				statName == "SPECIAL_ABILITY_UNLOCKED";
		}

		void StatSetInt(const std::string& name, int value)
		{
			addlog(ige::LogType::LOG_TRACE, "Setting Stat " + name + " to " + std::to_string(value));
			STAT_SET_INT(GET_HASH_KEY(name), value, 1);

			for (auto& ch : charNames)
			{
				if (name.find(ch.first) == 0)
				{
					std::string statPart = name.substr(ch.first.length());
					if (IsAbilityStat(statPart))
					{
						SetAbilitySourceStats(ch.first, statPart, value);
						if (value >= 100)
							STAT_SET_BOOL(GET_HASH_KEY(ch.first + statPart + "_MAXED"), true, true);
						else
							STAT_SET_BOOL(GET_HASH_KEY(ch.first + statPart + "_MAXED"), false, true);
						if (!STAT_SAVE_PENDING_OR_REQUESTED())
							STAT_SAVE(0, 0, 3, 0);
					}
					break;
				}
			}
		}

		void StatSetBool(const std::string& name, bool value)
		{
			addlog(ige::LogType::LOG_TRACE, "Setting Stat " + name + " to " + std::string(value ? "true" : "false"));
			TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("stats_controller");
			STAT_SET_BOOL(GET_HASH_KEY(name), value, 1);
		}

		void StatSetFloat(const std::string& name, float value)
		{
			addlog(ige::LogType::LOG_TRACE, "Setting Stat " + name + " to " + std::to_string(value));
			TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("stats_controller");
			STAT_SET_FLOAT(GET_HASH_KEY(name), value, 1);
		}

		void StatSetString(const std::string& name, const std::string& value)
		{
			addlog(ige::LogType::LOG_TRACE, "Setting Stat " + name + " to " + value);
			TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("stats_controller");
			STAT_SET_STRING(GET_HASH_KEY(name), value.c_str(), 1);
		}

		void AddOptionStats(const CharStat_t& stat)
		{
			bool bStatValue_plus = false, bStatValue_minus = false, bStatValue_input = false;

			const std::string& statName = selectedCharName->first + stat.name;

			switch (stat.type)
			{
			case StatDataType_t::BOOL:
			{
				bool statValue = StatGetBool(statName);
				AddTickol(stat.caption, statValue, bStatValue_input, bStatValue_input, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bStatValue_input)
				{
					addlog(ige::LogType::LOG_DEBUG, "Toggling Stat " + stat.caption + " from " + std::string(statValue ? "true" : "false") + " to " + std::string(!statValue ? "true" : "false"));
					statValue = !statValue;
					StatSetBool(statName, statValue);
				}
				break;
			}
			case StatDataType_t::INT:
			{
				int statValue = StatGetInt(statName);
				AddNumber(stat.caption, statValue, 0, bStatValue_input, bStatValue_plus, bStatValue_minus); 
				if (bStatValue_input)
				{
					std::string inputStr = Game::InputBox(std::string(), (int)std::to_string((int)stat.max).length() + 1, "Enter integer value:", std::to_string(statValue));
					if (inputStr.length() > 0)
					{
						try
						{
							addlog(ige::LogType::LOG_DEBUG, "Setting Stat " + stat.caption + " to " + std::to_string(statValue) + " via input");
							statValue = stoi(inputStr);
							StatSetInt(statName, statValue);
							addlog(ige::LogType::LOG_TRACE, "Stat " + stat.caption + " successfully set to " + std::to_string(StatGetInt(statName)) + " via input");
						}
						catch (...) 
						{ 
							Game::Print::PrintErrorInvalidInput(inputStr); 
							addlog(ige::LogType::LOG_ERROR, "Invalid Stat Integer for " + stat.caption + " Entered");
						}
					}
				}
				if (bStatValue_plus) 
				{ 
					if (statValue < stat.max) 
					{ 
						addlog(ige::LogType::LOG_DEBUG, "Increasing Stat " + stat.caption + " from " + std::to_string(statValue) + " to " + std::to_string(statValue + 1) + " via plus button");
						statValue += 1; 
						StatSetInt(statName, statValue);
						addlog(ige::LogType::LOG_TRACE, "Stat " + stat.caption + " successfully set to " + std::to_string(StatGetInt(statName)) + " via plus button");
					} 
				}
				if (bStatValue_minus) 
				{ 
					if (statValue > stat.min) 
					{ 
						addlog(ige::LogType::LOG_DEBUG, "Decreasing Stat " + stat.caption + " from " + std::to_string(statValue) + " to " + std::to_string(statValue - 1) + " via minus button");
						statValue -= 1; 
						StatSetInt(statName, statValue);
						addlog(ige::LogType::LOG_TRACE, "Stat " + stat.caption + " successfully set to " + std::to_string(StatGetInt(statName)) + " via minus button");
					} 
				}
				break;
			}
			case StatDataType_t::FLOAT:
			{
				float statValue = StatGetFloat(statName);
				AddNumber(stat.caption, statValue, 2, bStatValue_input, bStatValue_plus, bStatValue_minus); if (bStatValue_input)
				{
					std::string inputStr = Game::InputBox(std::string(), 13U, "Enter floating point value:", std::to_string(statValue));
					if (inputStr.length() > 0)
					{
						try
						{
							addlog(ige::LogType::LOG_DEBUG, "Setting Stat " + stat.caption + " to " + inputStr + " via input");
							statValue = stof(inputStr);
							StatSetFloat(statName, statValue);
						}
						catch (...) 
						{ 
							Game::Print::PrintErrorInvalidInput(inputStr);
							addlog(ige::LogType::LOG_ERROR, "Invalid Stat Float for " + stat.caption + " Entered");
						}
					}
				}
				if (bStatValue_plus) 
				{ 
					if (statValue < stat.max) 
					{ 
						addlog(ige::LogType::LOG_DEBUG, "Increasing Stat " + stat.caption + " from " + std::to_string(statValue) + " to " + std::to_string(statValue + 0.05f) + " via plus button");
						statValue += 0.05f;
						StatSetFloat(statName, statValue);
					}
				}
				if (bStatValue_minus)
				{
					if (statValue > stat.min)
					{
						addlog(ige::LogType::LOG_DEBUG, "Decreasing Stat " + stat.caption + " from " + std::to_string(statValue) + " to " + std::to_string(statValue - 0.05f) + " via minus button");
						statValue -= 0.05f;
						StatSetFloat(statName, statValue); 
					} 
				}
				break;
			}
			}
		}

		void SPStatsManagerMenu()
		{
			AddTitle("Stat Manager");

			AddBreak("---Story Mode---");
			for (int i = 0; i < 3; i++)
			{
				bool bGoToCharacterPressed = false;
				AddOption(charNames[i].second, bGoToCharacterPressed, nullFunc, SUB::SPSTATMANAGER_INCHAR); if (bGoToCharacterPressed)
				{
					selectedCharName = &charNames[i];
				}
			}

			if (NETWORK_IS_IN_SESSION())
			{
				AddBreak("---GTA Online---");
				for (int i = 3; i < 5; i++)
				{
					bool bGoToCharacterPressed = false;
					AddOption(charNames[i].second, bGoToCharacterPressed, nullFunc, SUB::SPSTATMANAGER_INCHAR); if (bGoToCharacterPressed)
					{
						selectedCharName = &charNames[i];
					}
				}
			}
			
			AddBreak("---Achievements---");
			bool unlockAllAchievements = false;
			AddOption("Unlock All Achievements", unlockAllAchievements); 
			if (unlockAllAchievements)
			{
				int numAchievements = 78;
				for (int i = 0; i < numAchievements; i++)
				{
					if (!HAS_ACHIEVEMENT_BEEN_PASSED(i))
					{
						GIVE_ACHIEVEMENT_TO_PLAYER(i);
					}
				}
			}

			auto unlockAchievement = [](int id, const char* description) 
			{
				bool unlockFlag = false;
				AddOption(std::to_string(id) + ". " + description, unlockFlag);
				if (unlockFlag && !HAS_ACHIEVEMENT_BEEN_PASSED(id))
				{
					GIVE_ACHIEVEMENT_TO_PLAYER(id);
				}
			};
			
			unlockAchievement(1, "Unlock 'Welcome to Los Santos'");
			unlockAchievement(2, "Unlock 'A Friendship Resurrected'");
			unlockAchievement(3, "Unlock 'A Fair Day's Pay'");
			unlockAchievement(4, "Unlock 'The Moment of Truth'");
			unlockAchievement(5, "Unlock 'To Live or Die in Los Santos'");
			unlockAchievement(6, "Unlock 'Diamond Hard'");
			unlockAchievement(7, "Unlock 'Subversive'");
			unlockAchievement(8, "Unlock 'Blitzed'");
			unlockAchievement(9, "Unlock 'Small Town, Big Job'");
			unlockAchievement(10, "Unlock 'The Government Gimps'");
			unlockAchievement(11, "Unlock 'The Big One!'");
			unlockAchievement(12, "Unlock 'Solid Gold, Baby!'");
			unlockAchievement(13, "Unlock 'Career Criminal'");
			unlockAchievement(14, "Unlock 'San Andreas Sightseer'");
			unlockAchievement(15, "Unlock 'All's Fare in Love and War'");
			unlockAchievement(16, "Unlock 'TP Industries Arms Race'");
			unlockAchievement(17, "Unlock 'Multi-Disciplined'");
			unlockAchievement(18, "Unlock 'From Beyond the Stars'");
			unlockAchievement(19, "Unlock 'A Mystery, Solved'");
			unlockAchievement(20, "Unlock 'Waste Management'");
			unlockAchievement(21, "Unlock 'Red Mist'");
			unlockAchievement(22, "Unlock 'Show Off'");
			unlockAchievement(23, "Unlock 'Kifflom!'");
			unlockAchievement(24, "Unlock 'Three Man Army'");
			unlockAchievement(25, "Unlock 'Out of Your Depth'");
			unlockAchievement(26, "Unlock 'Altruist Acolyte'");
			unlockAchievement(27, "Unlock 'A Lot of Cheddar'");
			unlockAchievement(28, "Unlock 'Trading Pure Alpha'");
			unlockAchievement(29, "Unlock 'Pimp My Sidearm'");
			unlockAchievement(30, "Unlock 'Wanted: Alive Or Alive'");
			unlockAchievement(31, "Unlock 'Los Santos Customs'");
			unlockAchievement(32, "Unlock 'Close Shave'");
			unlockAchievement(33, "Unlock 'Off the Plane'");
			unlockAchievement(34, "Unlock 'Three-Bit Gangster'");
			unlockAchievement(35, "Unlock 'Making Moves'");
			unlockAchievement(36, "Unlock 'Above the Law'");
			unlockAchievement(37, "Unlock 'Numero Uno'");
			unlockAchievement(38, "Unlock 'The Midnight Club'");
			unlockAchievement(39, "Unlock 'Unnatural Selection'");
			unlockAchievement(40, "Unlock 'Backseat Driver'");
			unlockAchievement(41, "Unlock 'Run Like The Wind'");
			unlockAchievement(42, "Unlock 'Clean Sweep'");
			unlockAchievement(43, "Unlock 'Decorated'");
			unlockAchievement(44, "Unlock 'Stick Up Kid'");
			unlockAchievement(45, "Unlock 'Enjoy Your Stay'");
			unlockAchievement(46, "Unlock 'Crew Cut'");
			unlockAchievement(47, "Unlock 'Full Refund'");
			unlockAchievement(48, "Unlock 'Dialling Digits'");
			unlockAchievement(49, "Unlock 'American Dream'");
			unlockAchievement(50, "Unlock 'A New Perspective'");
			unlockAchievement(51, "Unlock 'Be Prepared'");
			unlockAchievement(52, "Unlock 'In the Name of Science'");
			unlockAchievement(53, "Unlock 'Dead Presidents'");
			unlockAchievement(54, "Unlock 'Parole Day'");
			unlockAchievement(55, "Unlock 'Shot Caller'");
			unlockAchievement(56, "Unlock 'Four Way'");
			unlockAchievement(57, "Unlock 'Live a Little'");
			unlockAchievement(58, "Unlock 'Can't Touch This'");
			unlockAchievement(59, "Unlock 'Mastermind'");
			unlockAchievement(60, "Unlock 'Vinewood Visionary'");
			unlockAchievement(61, "Unlock 'Majestic'");
			unlockAchievement(62, "Unlock 'Humans of Los Santos'");
			unlockAchievement(63, "Unlock 'First Time Director'");
			unlockAchievement(64, "Unlock 'Animal Lover'");
			unlockAchievement(65, "Unlock 'Ensemble Piece'");
			unlockAchievement(66, "Unlock 'Cult Movie'");
			unlockAchievement(67, "Unlock 'Location Scout'");
			unlockAchievement(68, "Unlock 'Method Actor'");
			unlockAchievement(69, "Unlock 'Cryptozoologist'");
			unlockAchievement(70, "Unlock 'Getting Started'");
			unlockAchievement(71, "Unlock 'The Data Breaches'");
			unlockAchievement(72, "Unlock 'The Bogdan Problem'");
			unlockAchievement(73, "Unlock 'The Doomsday Scenario'");
			unlockAchievement(74, "Unlock 'A World Worth Saving'");
			unlockAchievement(75, "Unlock 'Orbital Obliteration'");
			unlockAchievement(76, "Unlock 'Elitist'");
			unlockAchievement(77, "Unlock 'Masterminds'");

		}
		void SPStatsInCharMenu()
		{
			AddTitle(selectedCharName->second);

			for (auto& statList : vCharStatLists)
			{
				if (statList.list.size() == 1)
				{
					AddOptionStats(statList.list.front());
				}
				else
				{
					bool bStatListPressed = false;
					AddOption(statList.title, bStatListPressed, nullFunc, SUB::SPSTATMANAGER_INCHAR_INLIST); if (bStatListPressed)
					{
						selectedStatList = &statList;
					}
				}
			}

			if (IsOnlineChar(selectedCharName->first))
			{
				AddBreak("---Property Ownership---");
				for (auto& statList : vMPPropertyLists)
				{
					bool bStatListPressed = false;
					AddOption(statList.title, bStatListPressed, nullFunc, SUB::SPSTATMANAGER_INCHAR_INLIST); if (bStatListPressed)
					{
						selectedStatList = &statList;
					}
				}
			}
		}
		void InCharInListMenu()
		{
			AddTitle(selectedStatList->title);

			for (auto& stat : selectedStatList->list)
			{
				AddOptionStats(stat);
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(SPSTATMANAGER,               	sub::SpStatManager::SPStatsManagerMenu)
REGISTER_SUBMENU(SPSTATMANAGER_INCHAR,        	sub::SpStatManager::SPStatsInCharMenu)
REGISTER_SUBMENU(SPSTATMANAGER_INCHAR_INLIST, 	sub::SpStatManager::InCharInListMenu)
