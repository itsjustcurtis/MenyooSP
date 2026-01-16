#include "BodyguardSettings.h"
#include "BodyguardManagement.h"
#include "Menu/Menu.h"
#include "Natives/natives.h"
#include "Natives/natives2.h"

#include "Util/keyboard.h"
#include "Scripting/Game.h"

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

        // Iterate over the DB
        for (UINT i = 0; i < BodyguardDb.size(); i++)
        {
            auto& bg = BodyguardDb[i];

            // Only skip if the ped no longer exists
            if (!bg.Handle.Exists())
                continue;

            bool bPressed = false;

            // Option label: Name or HashName
            std::string label = !bg.Name.empty() ? bg.Name : bg.HashName;

            // --- Important change: pass the submenu id so the menu system knows to route there ---
            // Use Menyoo's "null" and "nullFunc" as the example elsewhere in your codebase.
            AddOption(label, bPressed, nullFunc, SUB::BODYGUARD_ENTITYOPS);

            // If selected this frame: set selected pointer BEFORE the submenu actually opens
            if (bPressed)
            {
                SelectedBodyguard = &bg;
            }

            // Highlighted entry actions
            if (*Menu::currentopATM == Menu::printingop)
            {
                // Show arrow above the ped
                if (bg.Handle.Exists())
                    ENTITY::SET_ENTITY_HAS_GRAVITY(bg.Handle.GetHandle(), true); // optional, ensure ped visible
                sub::BodyguardMenu::BodyguardManagement::ShowArrowAboveEntity(bg.Handle);

                // Button prompt for deletion
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

        // Perform deletion after iteration to avoid invalidating the loop
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
