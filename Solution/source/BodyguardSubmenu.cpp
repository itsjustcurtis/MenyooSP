#include "BodyguardManagement.h"
#include "BodyguardSettings.h"
#include "BodyguardSpawn.h"
#include "Menu/submenu_enum.h"
#include "Scripting/GTAped.h"
#include "Submenus/PedComponentChanger.h"
#include "BodyguardMenu.h"
#include "Submenus/WeaponOptions.h"

namespace sub
{
    void ComponentChanger_();
}

namespace sub::BodyguardMenu
{
    void BodyguardEntityOps()
    {
        AddTitle("Bodyguard");

        if (!SelectedBodyguard)
        {
            AddOption("No bodyguard selected");
            return;
        }

        if (!SelectedBodyguard->Handle.Exists())
        {
            AddOption("Bodyguard no longer exists");
            return;
        }

        AddOption("Weapons", null, nullFunc, SUB::BODYGUARD_WEAPONOPS);
        AddOption("Wardrobe", null, nullFunc, SUB::BODYGUARD_WARDROBE);
    }

    void BodyguardWeaponOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        Ped ped = SelectedBodyguard->Handle.GetHandle();

        g_WeaponOpsPedOverride = ped;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = ped;

        sub::Weaponops();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }

    void BodyguardWardrobe()
    {
        // TBA
    }
}

