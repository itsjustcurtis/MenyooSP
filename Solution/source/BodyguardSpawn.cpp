#include "BodyguardSpawn.h"
#include "BodyguardFunction.h"
#include "BodyguardManagement.h"

#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/GTAped.h"
#include "Scripting/Model.h"
#include "Natives/natives2.h"
#include "Natives/natives.h"
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
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SearchToLower, _searchStr, 126U, std::string(), _searchStr);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_searchStr);
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
						Bodyguards::BodyguardMenu::AddBodyguard_Ped(current.second, currentModel);
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

			bool bInputPressed = false;
			AddOption("INPUT MODEL", bInputPressed); if (bInputPressed)
			{
				BodyguardManagement::InputBodyguardIntoDb(EntityType::PED);
			}
		}

}
namespace sub::Bodyguards::BodyguardMenu
{
	void AddBodyguard_Ped(const std::string& text, const GTAmodel::Model& model)
	{


		GTAped player(PLAYER::PLAYER_PED_ID());
		Vector3 spawnPos = player.GetOffsetInWorldCoords(Vector3(2.0f, 0.0f, 0.0f));

		Ped ped = PED::CREATE_PED(26, model.hash, spawnPos.x, spawnPos.y, spawnPos.z, 0.0f, true, true);


		if (!ENTITY::DOES_ENTITY_EXIST(ped))
		{
			Game::Print::PrintBottomLeft("Ped creation failed.");
			return;
		}

		ENTITY::SET_ENTITY_HEALTH(ped, sub::BodyguardMenu::health, 0);
		ENTITY::SET_ENTITY_MAX_HEALTH(ped, sub::BodyguardMenu::health);
		PED::SET_PED_ARMOUR(ped, sub::BodyguardMenu::armor);
		ENTITY::SET_ENTITY_INVINCIBLE(ped, sub::BodyguardMenu::godmode);

		PED::SET_PED_AS_GROUP_MEMBER(ped, PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));
		PED::SET_PED_NEVER_LEAVES_GROUP(ped, true);
		PED::SET_PED_COMBAT_ABILITY(ped, 2);
		PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

		Game::Print::PrintBottomLeft("Bodyguard spawned");
	}
}
