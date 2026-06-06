/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "PedModelChanger.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\Model.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\ModelNames.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\WeaponIndivs.h"

#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\WeaponOptions.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\Databases.h"
#include "..\Submenus\Spooner\EntityManagement.h"
#include "..\Submenus\Spooner\MenuOptions.h"
#include "..\Submenus\Bodyguards\BodyguardSpawn.h"

#include <string>
#include <vector>
#include <pugixml\src\pugixml.hpp>

namespace sub
{
	namespace PedFavourites
	{
		std::string xmlFavouritePeds = "FavouritePeds.xml";
		std::string searchStr = std::string();

		void ClearSearchStr() 
		{ 
			searchStr.clear(); 
		}

		bool IsPedAFavourite(GTAmodel::Model model)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				return false;
			}
			pugi::xml_node nodeRoot = doc.document_element();
			return nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str()) != NULL;
		}
		bool AddPedToFavourites(GTAmodel::Model model, const std::string& customName)
		{
			if (customName.empty())
			{
				return false;
			}
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				doc.reset();
				auto nodeDecleration = doc.append_child(pugi::node_declaration);
				nodeDecleration.append_attribute("version") = "1.0";
				nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
				auto nodeRoot = doc.append_child("FavouriteWeapons");
				doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str());
			}
			pugi::xml_node nodeRoot = doc.document_element();
			auto nodeOldLoc = nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str());
			if (nodeOldLoc) // If not null
			{
				nodeOldLoc.parent().remove_child(nodeOldLoc);
			}
			auto nodeNewLoc = nodeRoot.append_child("Ped");
			nodeNewLoc.append_attribute("hash") = IntToHexString(model.hash, true).c_str();
			nodeNewLoc.append_attribute("customName") = customName.c_str();
			return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()));
		}

		bool RemovePedFromFavourites(GTAmodel::Model model)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				return false;
			}
			pugi::xml_node nodeRoot = doc.document_element();
			auto nodeOldLoc = nodeRoot.find_child_by_attribute("hash", IntToHexString(model.hash, true).c_str());
			if (nodeOldLoc) // If not null
			{
				nodeOldLoc.parent().remove_child(nodeOldLoc);
			}
			return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()));
		}

		void ShowInstructionalButton(GTAmodel::Model model)
		{
			bool bIsAFav = IsPedAFavourite(model);
			if (Menu::bitController)
			{
				Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
				{
					!bIsAFav ? AddPedToFavourites(model, Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(model, true))) : RemovePedFromFavourites(model);
				}
			}
			else
			{
				Menu::add_IB(VirtualKey::B, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");
				if (IsKeyJustUp(VirtualKey::B))
				{
					!bIsAFav ? AddPedToFavourites(model, Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(model, true))) : RemovePedFromFavourites(model);
				}
			}
		}

		void PedFavouritesMenu()
		{
			Menu::OnSubBack = ClearSearchStr;
			AddTitle("Favourites");

			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str()).status != pugi::status_ok)
			{
				doc.reset();
				auto nodeDeclaration = doc.append_child(pugi::node_declaration);
				nodeDeclaration.append_attribute("version") = "1.0";
				nodeDeclaration.append_attribute("encoding") = "ISO-8859-1";
				auto nodeRoot = doc.append_child("FavouritePeds");
				doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouritePeds).c_str());
				return;
			}
			pugi::xml_node nodeRoot = doc.document_element();

			bool inputAdd = false;
			AddOption("Add New Ped Model", inputAdd); if (inputAdd)
			{
				std::string hashNameStr = Game::InputBox("", 40U, "Enter model name (e.g. IG_BENNY):");
				if (hashNameStr.length())
				{
					Model hashNameHash = GET_HASH_KEY(hashNameStr);
					if (hashNameHash.IsInCdImage())
					{
						WAIT(500);
						std::string customNameStr = Game::InputBox("", 28U, "Enter custom name:", GetPedModelLabel(hashNameHash, true));
						if (customNameStr.length())
						{
							if (AddPedToFavourites(hashNameHash, customNameStr))
							{
								Game::Print::PrintBottomLeft("Ped model ~b~added~s~.");
							}
							else 
							{
								Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to add ped model.");
							}
						}
						else 
						{
							Game::Print::PrintErrorInvalidInput(customNameStr);
						}
					}
					else 
					{
						Game::Print::PrintErrorInvalidModel(hashNameStr);
					}
				}
				else 
				{
					Game::Print::PrintErrorInvalidInput(hashNameStr);
				}
			}

			if (nodeRoot.first_child())
			{
				AddBreak("---Added Ped Models---");

				bool bSearchPressed = false;
				AddOption(searchStr.empty() ? "SEARCH" : searchStr, bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
				{
					searchStr = Game::InputBox(searchStr, 126U, "SEARCH", boost::to_lower_copy(searchStr));
					boost::to_upper(searchStr);
				}

				for (auto nodeLocToLoad = nodeRoot.first_child(); nodeLocToLoad; nodeLocToLoad = nodeLocToLoad.next_sibling())
				{
					const std::string& customName = nodeLocToLoad.attribute("customName").as_string();
					Model model = nodeLocToLoad.attribute("hash").as_uint();

					if (!searchStr.empty()) 
					{
						if (boost::to_upper_copy(customName).find(searchStr) == std::string::npos) 
						{
							continue;
						}
					}
					AddModelOption(customName, model);
				}
			}
		}
	}

	void ChangeModel(GTAmodel::Model model)
	{
		if (model.IsInCdImage())
		{
			if (model.Load(4000))
			{
				GTAped playerPed = PLAYER_PED_ID();
				int oldPlayerPed = playerPed.Handle();

				if (sub::PedDamageTextures::vPedsAndDamagePacks.count(playerPed.Handle()))
				{
					sub::PedDamageTextures::vPedsAndDamagePacks.erase(playerPed.Handle());
				}
				if (sub::PedDecals::vPedsAndDecals.count(playerPed.Handle()))
				{
					sub::PedDecals::vPedsAndDecals.erase(playerPed.Handle());
				}

				std::vector<s_Weapon_Components_Tint> weaponsBackup;
				playerPed.StoreWeaponsInArray(weaponsBackup);
				Hash currWeaponHash = playerPed.GetWeapon();

				GTAentity att;
				auto spi = sub::Spooner::EntityManagement::GetEntityIndexInDb(playerPed);
				if (spi >= 0)
				{
					auto& spe = sub::Spooner::Databases::EntityDb[spi];
					sub::Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(spe.handle, att);
				}

				bool wasInVehicle = playerPed.IsInVehicle();
				GTAvehicle vehicle;
				VehicleSeat currentVehSeat;
				if (wasInVehicle)
				{
					vehicle = playerPed.CurrentVehicle();
					currentVehSeat = playerPed.GetCurrentVehicleSeat();
				}

				bool hasCollision = playerPed.GetIsCollisionEnabled();
				SET_PLAYER_MODEL(PLAYER_ID(), model.hash);

				playerPed = PLAYER_PED_ID();
				playerPed.SetIsCollisionEnabled(hasCollision);

				SET_PED_DEFAULT_COMPONENT_VARIATION(playerPed.Handle());
				model.Unload();


				if (wasInVehicle)
				{
					playerPed.SetIntoVehicle(vehicle, currentVehSeat);
				}

				if (playerPed.PedType() == PedType::Animal && !HAS_ANIM_DICT_LOADED("creatures@rottweiler@melee@streamed_core@"))
				{
					REQUEST_ANIM_DICT("creatures@rottweiler@melee@streamed_core@");
					REQUEST_ANIM_DICT("creatures@cougar@melee@streamed_core@");
				}

				playerPed.GiveWeaponsFromArray(weaponsBackup);
				if (IS_WEAPON_VALID(currWeaponHash))
				{
					playerPed.SetWeapon(currWeaponHash);
				}

				SET_PED_INFINITE_AMMO_CLIP(playerPed.Handle(), bitInfiniteAmmo);

				if (spi >= 0)
				{
					auto& spe = sub::Spooner::Databases::EntityDb[spi];
					GTAentity oldPlayerPed = spe.handle;
					spe.handle = playerPed;
					spe.hashName = GetPedModelLabel(model, true);
					if (spe.hashName.length() == 0)
					{
						IntToHexString(model.hash, true);
					}
					spe.ClearLastAnimations();
					spe.currentScenario.clear();
					if (att.Exists() && spe.attachmentArgs.isAttached)
					{
						spe.handle.AttachTo(att, spe.attachmentArgs.boneIndex, spe.handle.GetIsCollisionEnabled(), spe.attachmentArgs.offset, spe.attachmentArgs.rotation);
					}
					spe.taskSequence.Reset();
					if (sub::Spooner::selectedEntity.handle.Equals(oldPlayerPed))
					{
						sub::Spooner::selectedEntity = spe;
					}
				}
			}
		}
	}

	void AddModelChangerOption(const std::string& text, const GTAmodel::Model& model, int tickTrue)
	{
		const GTAped& ped = Game::PlayerPed();
		const Model& pedModel = ped.Model();

		bool pressed = false;
		AddTickol(text, model.Equals(pedModel), pressed, pressed, static_cast<TICKOL>(tickTrue)); if (pressed)
		{
			PTFX::TriggerPTFX("scr_solomon3", "scr_trev4_747_blood_impact", 0, ped.GetOffsetInWorldCoords(0.37, -0.32f, -1.32f), Vector3(90.0f, 0, 0), 0.7f);
			ChangeModel(model);
			addlog(ige::LogType::LOG_TRACE, "Changed model to: " + text);
		}
	}

	void AddModelOption(const std::string& text, const GTAmodel::Model& model, bool* extraOptionCode, int tickTrue)
	{
		if (model.IsInCdImage())
		{
			int context = 0;
			for (int ci = Menu::currentArrayIndex; ci >= 0; ci--)
			{
				if (Menu::currentArray[ci] == SUB::SPOONER_SPAWN_PED) { context = 1; break; }
				if (Menu::currentArray[ci] == SUB::BODYGUARD_SPAWN) { context = 2; break; }
				if (Menu::currentArray[ci] == SUB::PEDGUN_ALLPEDS) { context = 3; break; }
			}

			switch (context)
			{
			case 1:
				sub::Spooner::MenuOptions::AddOptionAddPed(text, model);
				break;
			case 2:
				sub::BodyguardMenu::BodyguardManagement::AddOptionBodyGuardPed(text, model);
				break;
			case 3:
				AddPedGunOption(text, model.hash, extraOptionCode);
				break;
			default:
				AddModelChangerOption(text, model.hash, tickTrue);
				break;
			}

			if (*Menu::currentopATM == Menu::printingop)
			{
				PedFavourites::ShowInstructionalButton(model);
			}
		}
	}

	std::pair<std::string, std::string> rngped;

	void ModelChangerMenu()
	{
		bool modelChangerRandomPedVariation = false;
		rngped = { "", "" };
		dict3.clear();

		g_Ped1 = PLAYER_PED_ID();
		AddTitle("Model Changer");
		AddOption("~b~Search~s~ Peds", null, nullFunc, SUB::MODELCHANGER_SEARCH);
		AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);
		AddOption("Randomize Ped Variation", modelChangerRandomPedVariation);

		AddBreak("---Categories---");
		AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
		AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
		AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
		AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
		AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);

		AddBreak("---Ambient---");
		AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
		AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
		AddOption("Gang Female", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
		AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);

		AddBreak("---Scenario---");
		AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
		AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
		AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
		AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
		AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);

		if (modelChangerRandomPedVariation)
		{
			addlog(ige::LogType::LOG_TRACE, "Random Ped Selected");
			SET_PED_RANDOM_COMPONENT_VARIATION(g_Ped1, 0);
			SET_PED_RANDOM_PROPS(g_Ped1);
			return;
		}
	}

	GTAmodel::Model ModelChangerRandom(std::vector<std::pair<std::string, std::string>> pedModels)
	{
		addlog(ige::LogType::LOG_TRACE, "Getting Random Ped Model");
		if (pedModels.empty()) return 0;
		rngped = pedModels[std::rand() % pedModels.size()];
		addlog(ige::LogType::LOG_TRACE, "Got Random Ped Model: " + rngped.first + ", " + rngped.second);
		return rngped.first;
	}

	void ModelChangerPlayer()
	{
		AddTitle("Player");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_Player);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Player)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAnimal()
	{
		AddTitle("Animals");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_Animal);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Animal)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAmbientFemale()
	{
		AddTitle("Ambient Females");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_AmbientFemale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_AmbientFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerAmbientMale()
	{
		AddTitle("Ambient Males");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_AmbientMale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_AmbientMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerCutscene()
	{
		AddTitle("Cutscene Models");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_Cutscene);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Cutscene)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerGangFemale()
	{
		AddTitle("Gang Females");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_GangFemale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_GangFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerGangMale()
	{
		AddTitle("Gang Males");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_GangMale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_GangMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStory()
	{
		AddTitle("Story Models");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_Story);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Story)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerMultiplayer()
	{
		AddTitle("Multiplayer Models");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_Multiplayer);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Multiplayer)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerScenarioFemale()
	{
		AddTitle("Scenario Females");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_ScenarioFemale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_ScenarioFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerScenarioMale()
	{
		AddTitle("Scenario Males");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_ScenarioMale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_ScenarioMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStoryScenarioFemale()
	{
		AddTitle("Story Scenario Females");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_StoryScenarioFemale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_StoryScenarioFemale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	void ModelChangerStoryScenarioMale()
	{
		AddTitle("Story Scenario Males");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "") 
		{
			ModelChangerRandom(g_pedModels_StoryScenarioMale);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_StoryScenarioMale)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}
	
	void ModelChangerOthers()
	{
		AddTitle("Others");
		if (rngped.first == Game::PlayerPed().Model() || rngped.first == "")
		{
			ModelChangerRandom(g_pedModels_Others);
		}
		AddModelOption("Random", rngped.first, nullptr, 0);
		for (auto& pmn : g_pedModels_Others)
		{
			AddModelOption(pmn.second, (pmn.first));
		}
	}

	namespace PedSearch
	{
		static int sortIndex = 0;
		static int categoryFilter = 0;
		static std::vector<std::pair<std::string, std::string>> results;
		static std::vector<std::string> categoryOptions;
		static bool dirty = true;
		static std::string lastSearch;
		static int lastSort = -1, lastCategory = -1;

		static const std::vector<std::string> sortOptions = { "Name (A-Z)", "Name (Z-A)", "Model Name" };

		struct CategoryEntry {
			const char* name;
			std::vector<std::pair<std::string, std::string>>* vec;
		};

		static const CategoryEntry categories[] = {
			{ "Player", &g_pedModels_Player },
			{ "Animals", &g_pedModels_Animal },
			{ "Ambient Females", &g_pedModels_AmbientFemale },
			{ "Ambient Males", &g_pedModels_AmbientMale },
			{ "Cutscene", &g_pedModels_Cutscene },
			{ "Gang Females", &g_pedModels_GangFemale },
			{ "Gang Males", &g_pedModels_GangMale },
			{ "Story", &g_pedModels_Story },
			{ "Multiplayer", &g_pedModels_Multiplayer },
			{ "Scenario Females", &g_pedModels_ScenarioFemale },
			{ "Scenario Males", &g_pedModels_ScenarioMale },
			{ "Story Scenario F", &g_pedModels_StoryScenarioFemale },
			{ "Story Scenario M", &g_pedModels_StoryScenarioMale },
			{ "Others", &g_pedModels_Others },
		};
		static const int numCategories = 14;

		void BuildCategoryOptions()
		{
			categoryOptions.clear();
			categoryOptions.push_back("All (" + std::to_string(g_pedModels.size()) + ")");
			for (int i = 0; i < numCategories; i++)
			{
				categoryOptions.push_back(std::string(categories[i].name) + " (" + std::to_string(categories[i].vec->size()) + ")");
			}
		}

		void RebuildResults(const std::string& searchStr)
		{
			results.clear();
			std::string searchUpper = boost::to_upper_copy(searchStr);

			auto* sourceVec = &g_pedModels;
			if (categoryFilter > 0 && categoryFilter <= numCategories)
				sourceVec = categories[categoryFilter - 1].vec;

			for (auto& ped : *sourceVec)
			{
				if (!searchUpper.empty())
				{
					std::string nameUpper = boost::to_upper_copy(ped.second);
					std::string modelUpper = boost::to_upper_copy(ped.first);
					if (nameUpper.find(searchUpper) == std::string::npos &&
						modelUpper.find(searchUpper) == std::string::npos)
						continue;
				}
				results.push_back(ped);
			}

			switch (sortIndex)
			{
			case 0:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.second) < boost::to_lower_copy(b.second);
				});
				break;
			case 1:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.second) > boost::to_lower_copy(b.second);
				});
				break;
			case 2:
				std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
					return boost::to_lower_copy(a.first) < boost::to_lower_copy(b.first);
				});
				break;
			}
		}
	}

	void ModelChangerSearchMenu()
	{
		using namespace PedSearch;
		auto& searchStr = dict3;

		if (categoryOptions.empty()) BuildCategoryOptions();

		AddTitle("Ped Search");

		bool bSearchPressed = false;
		AddOption(searchStr.empty() ? "~b~SEARCH~s~" : ("~b~" + searchStr + "~s~"), bSearchPressed, nullFunc, -1, true);
		if (bSearchPressed)
		{
			searchStr = Game::InputBox(searchStr, 64U, "Search peds:", boost::to_lower_copy(searchStr));
			boost::to_upper(searchStr);
		}

		bool sortRight = false, sortLeft = false;
		AddTexter("Sort", sortIndex, sortOptions, null, sortRight, sortLeft);
		if (sortRight && sortIndex < (int)(sortOptions.size() - 1)) sortIndex++;
		if (sortLeft && sortIndex > 0) sortIndex--;

		bool catRight = false, catLeft = false;
		AddTexter("Category", categoryFilter, categoryOptions, null, catRight, catLeft);
		if (catRight && categoryFilter < (int)(categoryOptions.size() - 1)) categoryFilter++;
		if (catLeft && categoryFilter > 0) categoryFilter--;

		if (dirty || searchStr != lastSearch || sortIndex != lastSort || categoryFilter != lastCategory)
		{
			RebuildResults(searchStr);
			lastSearch = searchStr;
			lastSort = sortIndex;
			lastCategory = categoryFilter;
			dirty = false;
		}

		AddBreak("---Results: " + std::to_string(results.size()) + "---");

		for (auto& ped : results)
		{
			AddModelOption(ped.second, (ped.first));
		}
	}

} // namespace sub


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(MODELCHANGER,                     sub::ModelChangerMenu)
REGISTER_SUBMENU(MODELCHANGER_FAVOURITES,          sub::PedFavourites::PedFavouritesMenu)
REGISTER_SUBMENU(MODELCHANGER_PLAYER,              sub::ModelChangerPlayer)
REGISTER_SUBMENU(MODELCHANGER_ANIMAL,              sub::ModelChangerAnimal)
REGISTER_SUBMENU(MODELCHANGER_AMBFEMALES,          sub::ModelChangerAmbientFemale)
REGISTER_SUBMENU(MODELCHANGER_AMBMALES,            sub::ModelChangerAmbientMale)
REGISTER_SUBMENU(MODELCHANGER_CS,                  sub::ModelChangerCutscene)
REGISTER_SUBMENU(MODELCHANGER_GANGFEMALES,         sub::ModelChangerGangFemale)
REGISTER_SUBMENU(MODELCHANGER_GANGMALES,           sub::ModelChangerGangMale)
REGISTER_SUBMENU(MODELCHANGER_STORY,               sub::ModelChangerStory)
REGISTER_SUBMENU(MODELCHANGER_MP,                  sub::ModelChangerMultiplayer)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOFEMALES,     sub::ModelChangerScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOMALES,       sub::ModelChangerScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOFEMALES,  sub::ModelChangerStoryScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOMALES,    sub::ModelChangerStoryScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_OTHERS,              sub::ModelChangerOthers)
REGISTER_SUBMENU(MODELCHANGER_SEARCH,              sub::ModelChangerSearchMenu)