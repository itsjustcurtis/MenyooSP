#include "BodyguardManagement.h"
#include "BodyguardSettings.h"
#include "BodyguardSpawn.h"
#include "Menu/submenu_enum.h"
#include "Scripting/GTAped.h"
#include "Submenus/PedComponentChanger.h"
#include "BodyguardMenu.h"
#include "Submenus/WeaponOptions.h"

// Scoped override for Menyoo's global current ped (Static_241)
struct ScopedPedOverride
{
    int oldPed;

    ScopedPedOverride(const GTAentity& newPed)
    {
        oldPed = Static_241;
        Static_241 = newPed.GetHandle();
    }

    ~ScopedPedOverride()
    {
        Static_241 = oldPed;
    }
};

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

		//Melee Focus will be added later.
        AddOption("Weapons", null, nullFunc, SUB::BODYGUARD_WEAPONOPS);
        AddOption("Wardrobe", null, nullFunc, SUB::BODYGUARD_WARDROBE);
    }

    void BodyguardWeaponOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        g_WeaponOpsPedOverride = SelectedBodyguard->Handle.GetHandle();
        g_WeaponOpsPlayerOverride = -1;

        sub::Weaponops();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
    }
}
