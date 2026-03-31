#include "BodyguardSettings.h"
#include "BodyguardManagement.h"
#include "../../Menu/Menu.h"
#include "../../Natives/natives.h"
#include "../../Natives/natives2.h"

#include "../../Util/keyboard.h"
#include "../../Scripting/Game.h"
#include "BodyguardMenu.h"

namespace sub::BodyguardMenu
{
    // Currently selected bodyguard in the menu
    BodyguardEntity* SelectedBodyguard = nullptr;

    void BodyguardList()
    {
        AddTitle("Bodyguard List");

        if (BodyguardDb.empty())
        {
            AddOption("No bodyguards spawned");
            return;
        }

        BodyguardEntity* pBodyguardToDelete = nullptr;

        for (UINT i = 0; i < BodyguardDb.size(); i++)
        {
            auto& bg = BodyguardDb[i];

            if (!bg.Handle.Exists())
                continue;

            bool bPressed = false;

            std::string label = !bg.Name.empty() ? bg.Name : bg.HashName;

            AddOption(label, bPressed, nullFunc, SUB::BODYGUARD_ENTITYOPS);

            if (bPressed)
            {
                SelectedBodyguard = &bg;
            }

            if (*Menu::currentopATM == Menu::printingop)
            {
                if (bg.Handle.Exists())
                    ENTITY::SET_ENTITY_HAS_GRAVITY(bg.Handle.GetHandle(), true);
                sub::BodyguardMenu::BodyguardManagement::ShowArrowAboveEntity(bg.Handle);

                bool bDeletePressed = false;
                if (Menu::bit_controller)
                {
                    Menu::add_IB(INPUT_SCRIPT_RLEFT, "Delete Bodyguard");
                    bDeletePressed = IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT) != 0;
                }
                else
                {
                    Menu::add_IB(VirtualKey::B, "Delete Bodyguard");
                    bDeletePressed = IsKeyJustUp(VirtualKey::B);
                }

                if (bDeletePressed)
                {
                    pBodyguardToDelete = &bg;
                }
            }
        }

        if (pBodyguardToDelete)
        {
            sub::BodyguardMenu::BodyguardManagement::DeleteBodyguard(*pBodyguardToDelete);
        }
    }

    void BodyguardOps_()
    {
        AddTitle("Bodyguard Settings");
    }
}
