#include "BodyguardManagement.h"
#include "BodyguardSettings.h"
#include "BodyguardSpawn.h"
#include "Menu/submenu_enum.h"
#include "Scripting/GTAped.h"
#include "Submenus/PedComponentChanger.h"
#include "BodyguardMenu.h"
#include "Submenus/WeaponOptions.h"
#include "Scripting/Camera.h"
#include "Scripting/World.h"

namespace sub
{
    void ComponentChanger_();
}

namespace sub::BodyguardMenu
{
    void SetEnt242() { Static_241= SelectedBodyguard->Handle.Handle(); }
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
        AddOption("Loadouts", null, SetEnt242, SUB::WEAPONOPS_LOADOUTS);
        AddOption("Wardrobe", null, SetEnt242, SUB::COMPONENTS);
        if (g_cam_componentChanger.Exists())
        {
            g_cam_componentChanger.SetActive(false);
            g_cam_componentChanger.Destroy();
            World::RenderingCamera_set(0);
        }
        AddOption("Voice Changer", null, SetEnt242, SUB::VOICECHANGER);
    }
    void BodyguardWeaponOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        Ped ped = SelectedBodyguard->Handle.GetHandle();

        g_WeaponOpsPedOverride = ped;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = ped;


        WeaponIndivs_catind::Sub_CategoriesList();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }
}