#include "BodyguardMenu.h"

#include "Menu\Menu.h"
#include "Scripting\Game.h"
#include "Natives\natives2.h"
#include "Submenus/Spooner/Submenus.h"
#include "Submenus/PedAnimation.h"
#include "Submenus/PedModelChanger.h"
#include "Submenus/PedSpeech.h"
#include "Submenus/WeaponOptions.h"

#include <functional>
#include "Menu/Routine.h"
#include "Misc/MeteorShower.h"
#include "Scripting/GTAvehicle.h"
#include "Scripting/Model.h"
#include "Submenus/Spooner/MenuOptions.h"
#include "Scripting/ModelNames.h"
#include "Util/StringManip.h"
#include "Util\ExePath.h"
#include "Submenus/Spooner/EntityManagement.h"
#include "Submenus/Spooner/SpoonerEntity.h"

#include "BodyguardManagement.h"
#include "BodyguardSpawn.h"
using namespace sub::BodyguardMenu;


using namespace sub::Spooner::Submenus;
using namespace Game::Print;


namespace sub::BodyguardMenu
{
		static bool g_fd_plus = false;
		static bool g_fd_minus = false;
		static bool prevGodMode = false;
		int armor = 200;
		int health = 200;
		bool godmode = false;
	

	void BodyguardMainMenu()
	{
		AddTitle("Bodyguards");

		AddOption("Spawn Bodyguard", null, nullFunc, SUB::BODYGUARD_SPAWN);
		AddOption("Bodyguard List", null, nullFunc, SUB::BODYGUARD_LIST);

		static int health = 200;
		static bool bHealth_plus = false, bHealth_minus = false, bHealth_input = false;
		AddNumber("Default Health", health, 0, bHealth_input, bHealth_plus, bHealth_minus);
		if (bHealth_plus && health < INT_MAX) ++health;
		if (bHealth_minus && health > 0) --health;
		if (bHealth_input)
		{
			std::string inputStr = Game::InputBox("", 5U, "", std::to_string(health));
			if (!inputStr.empty())
			{
				try { health = std::stoi(inputStr); }
				catch (...) { PrintError_InvalidInput(); }
			}
		}

		static int armor = 200;
		static bool bArmor_plus = false, bArmor_minus = false, bArmor_input = false;
		AddNumber("Default Armor", armor, 0, bArmor_input, bArmor_plus, bArmor_minus);
		if (bArmor_plus && armor < INT_MAX) ++armor;
		if (bArmor_minus && armor > 0) --armor;
		if (bArmor_input)
		{
			std::string inputStr = Game::InputBox("", 5U, "", std::to_string(armor));
			if (!inputStr.empty())
			{
				try { armor = std::stoi(inputStr); }
				catch (...) { PrintError_InvalidInput(); }
			}
		}

		static bool godmode = false;
		AddToggle("Godmode", godmode);
	}
}