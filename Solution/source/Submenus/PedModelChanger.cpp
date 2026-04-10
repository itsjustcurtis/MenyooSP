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
				Hash currWeaponHash = playerPed.Weapon_get();

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
					spe.lastAnimation.dict.clear();
					spe.lastAnimation.name.clear();
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
			switch (Menu::currentArray[Menu::currentArrayIndex])
			{
			case SUB::MODELCHANGER:
				AddModelChangerOption(text, model.hash, tickTrue);
				break;
			case SUB::PEDGUN_ALLPEDS:
				AddPedGunOption(text, model.hash, extraOptionCode);
				break;
			case SUB::SPOONER_SPAWN_PED:
				sub::Spooner::MenuOptions::AddOptionAddPed(text, model);
				break;
			case SUB::BODYGUARD_SPAWN:
				sub::BodyguardMenu::BodyguardManagement::AddOptionBodyGuardPed(text, model);
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
		bool modelChangerInput = false;
		rngped = { "", "" };

		g_Ped1 = PLAYER_PED_ID();
		AddTitle("Model Changer");
		AddOption("Randomize Ped Variation", modelChangerRandomPedVariation);
		AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);
		AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
		AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
		AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
		AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
		AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
		AddOption("Gang Female", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
		AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);
		AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
		AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
		AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
		AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
		AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
		AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
		AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);
		AddOption("~b~Input~s~ Model", modelChangerInput);

		if (modelChangerRandomPedVariation) 
		{
			addlog(ige::LogType::LOG_TRACE, "Random Ped Selected");
			SET_PED_RANDOM_COMPONENT_VARIATION(g_Ped1, 0);
			SET_PED_RANDOM_PROPS(g_Ped1);
			return;
		}

		if (modelChangerInput) 
		{
			std::string inputStr = Game::InputBox("", 64U, "Enter ped model name (e.g. IG_BENNY):");
			if (inputStr.length() > 0)
			{
				Model model = (inputStr);
				if (model.IsInCdImage())
				{
					ChangeModel(model);
				}
				else
				{
					Game::Print::PrintErrorInvalidModel(inputStr);
				}
				return;
			}
		}
	}

	GTAmodel::Model ModelChangerRandom(std::vector<std::pair<std::string, std::string>> pedModels)
	{
		addlog(ige::LogType::LOG_TRACE, "Getting Random Ped Model");		
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
}


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