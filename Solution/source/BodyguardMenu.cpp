#include "BodyguardMenu.h"

#include "Menu\Menu.h"
#include "Scripting\Game.h"
#include "Natives\natives2.h"
#include "Submenus/Spooner/Submenus.h"
#include "Submenus/PedAnimation.h"
#include "Submenus/PedModelChanger.h"
#include "Submenus/PedSpeech.h"

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

namespace sub::BodyguardMenu
{
    int armor = 200;
    int health = 200;
    bool godmode = false;
}
namespace sub
{
    void BodyguardMainMenu()
    {
        AddTitle("Bodyguards");

        AddOption("Spawn Bodyguard", null, nullFunc, SUB::BODYGUARD_SPAWN);
        AddOption("Bodyguard List", null, nullFunc, SUB::BODYGUARD_LIST);

        static bool bHealth_plus = false, bHealth_minus = false, bHealth_input = false;
        AddNumber("Default Health", sub::BodyguardMenu::health, 0, bHealth_input, bHealth_plus, bHealth_minus);
        if (bHealth_plus && sub::BodyguardMenu::health < INT_MAX) ++sub::BodyguardMenu::health;
        if (bHealth_minus && sub::BodyguardMenu::health > 0) --sub::BodyguardMenu::health;
        if (bHealth_input)
        {
            std::string inputStr = Game::InputBox("", 5U, "", std::to_string(sub::BodyguardMenu::health));
            if (!inputStr.empty())
            {
                try { sub::BodyguardMenu::health = std::stoi(inputStr); }
                catch (...) { PrintError_InvalidInput(); }
            }
        }

        static bool bArmor_plus = false, bArmor_minus = false, bArmor_input = false;
        AddNumber("Default Armor", sub::BodyguardMenu::armor, 0, bArmor_input, bArmor_plus, bArmor_minus);
        if (bArmor_plus && sub::BodyguardMenu::armor < INT_MAX) ++sub::BodyguardMenu::armor;
        if (bArmor_minus && sub::BodyguardMenu::armor > 0) --sub::BodyguardMenu::armor;
        if (bArmor_input)
        {
            std::string inputStr = Game::InputBox("", 5U, "", std::to_string(sub::BodyguardMenu::armor));
            if (!inputStr.empty())
            {
                try { sub::BodyguardMenu::armor = std::stoi(inputStr); }
                catch (...) { PrintError_InvalidInput(); }
            }
        }

        AddToggle("Godmode", sub::BodyguardMenu::godmode);
    }
}