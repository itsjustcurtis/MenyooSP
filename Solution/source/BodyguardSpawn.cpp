#include "Natives/types.h"
#include "Natives/natives2.h"
#include "Natives/natives.h"

#include "BodyguardSpawn.h"
#include "BodyguardFunction.h"
#include "BodyguardManagement.h"

#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/GTAped.h"
#include "Scripting/Model.h"
#include "macros.h"
#include <vector>
#include <tuple>
#include <string>

#include "Submenus/Spooner/SpoonerMode.h"
#include "Scripting/ModelNames.h"
#include "Menu/Routine.h"
#include "Submenus/Spooner/MenuOptions.h"
#include "Submenus/Spooner/Submenus.h"
#include "Submenus/PedModelChanger.h"
#include "Util/StringManip.h"
#include "Submenus/Spooner/EntityManagement.h"

namespace sub::BodyguardMenu
{
	extern int health;
	extern int armor;
	extern bool godmode;
	std::string  _searchStr = std::string();
	void BodyguardSpawn() 
	{

		
			AddTitle("Spawn Ped");

			AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);

			bool bSearchPressed = false;
			AddOption(_searchStr.empty() ? "SEARCH" : boost::to_upper_copy(_searchStr), bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
			{
				_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", _searchStr);
				boost::to_lower(_searchStr);
			}

			if (!_searchStr.empty())
			{
				for (auto& current : g_pedModels)
				{
					if (current.first.find(_searchStr) == std::string::npos && current.second.find(_searchStr) == std::string::npos)
						continue;

					Model currentModel = GET_HASH_KEY(current.first);

					if (currentModel.IsInCdImage())
					{
						sub::BodyguardMenu::BodyguardManagement::AddOption_BodyguardPed(current.second, currentModel);
						if (*Menu::currentopATM == Menu::printingop) PedFavourites_catind::ShowInstructionalButton(currentModel);

					}
				}
			}
			else
			{
				AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
				AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
				AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
				AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
				AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
				AddOption("Gang Females", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
				AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);
				AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
				AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
				AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
				AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
				AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
				AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
				AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);
			}
	}

}
namespace sub::BodyguardMenu::BodyguardManagement
{
	std::vector<Ped> s_bodyguards;
	void AddOption_BodyguardPed(const std::string& text, const GTAmodel::Model& model)
	{
		bool bPressed = false;
		AddOption(text, bPressed);

		if (!bPressed) return;

		if (s_bodyguards.size() >= MAX_BODYGUARDS)
		{
			Game::Print::PrintBottomLeft("Maximum of 7 bodyguards reached.");
			return;
		}

		if (!model.IsInCdImage())
			return;

		if (!model.Load(4000))
		{
			Game::Print::PrintBottomLeft("Failed to load model.");
			model.Unload();
			return;
		}

		int pedType = 26;

		GTAped player(PLAYER::PLAYER_PED_ID());
		Vector3 pos = player.GetOffsetInWorldCoords(Vector3(2.f, 0.f, 0.f));

		Ped ped = PED::CREATE_PED(
			pedType,
			model.hash,
			pos.x, pos.y, pos.z,
			0.f,
			true, true
		);

		ENTITY::SET_ENTITY_MAX_HEALTH(ped, sub::BodyguardMenu::health);
		ENTITY::SET_ENTITY_HEALTH(ped, sub::BodyguardMenu::health, 0);
		PED::SET_PED_ARMOUR(ped, sub::BodyguardMenu::armor);
		if (sub::BodyguardMenu::godmode)
			set_ped_invincible_on(ped);
		else
			set_ped_invincible_off(ped);


		PED::SET_PED_AS_GROUP_MEMBER(ped, PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));
		PED::SET_PED_NEVER_LEAVES_GROUP(ped, true);
		PED::SET_PED_COMBAT_ABILITY(ped, 2);
		PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

		BodyguardEntity ent{};
		ent.Handle = GTAentity(ped);
		ent.Type = EntityType::PED;
		ent.Name = text;
		ent.HashName = int_to_hexstring(model.hash, true);

		sub::BodyguardMenu::BodyguardManagement::AddBodyguardToDb(ent);

		s_bodyguards.push_back(ped);

		Game::Print::PrintBottomLeft("Bodyguard spawned");
		model.Unload();
	}

}

