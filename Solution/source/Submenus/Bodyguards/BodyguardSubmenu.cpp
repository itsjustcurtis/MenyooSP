#include "BodyguardManagement.h"
#include "BodyguardSettings.h"
#include "BodyguardSpawn.h"
#include "../../Menu/submenu_enum.h"
#include "../../Scripting/GTAped.h"
#include "../../Submenus/PedComponentChanger.h"
#include "BodyguardMenu.h"
#include "../../Submenus/WeaponOptions.h"
#include "../../Scripting/Camera.h"
#include "../../Scripting/World.h"
#include "../..//Natives/natives.h"
#include "../../Util/StringManip.h"

namespace sub
{
    void ComponentChanger();
}

namespace sub::BodyguardMenu
{
    void SetEnt242() { g_Ped1= SelectedBodyguard->Handle.Handle(); }
    void BodyguardEntityOps()
    {
        // Determine the title dynamically
        std::string title = "Bodyguard";

        if (SelectedBodyguard)
        {
            if (SelectedBodyguard->Handle.Exists())
            {
                // Prefer a friendly name if provided
                if (!SelectedBodyguard->Name.empty())
                {
                    title = SelectedBodyguard->Name;
                }
                // Otherwise use a stored hash-name (if present)
                else if (!SelectedBodyguard->HashName.empty())
                {
                    title = SelectedBodyguard->HashName;
                }
                // Fallback: use the model hash as hex string
                else
                {
                    auto model = SelectedBodyguard->Handle.Model();
                    title = IntToHexString(model.hash, true);
                }
            }
            else
            {
                // Ped doesn't exist — show that in the title so it's obvious
                title = "Bodyguard (missing)";
            }
        }

        AddTitle(title);

        // Keep the rest of your existing logic unchanged
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

        AddOption("Wardrobe", null, SetEnt242, SUB::COMPONENTS);
        if (g_cam_componentChanger.Exists())
        {
            g_cam_componentChanger.SetActive(false);
            g_cam_componentChanger.Destroy();
            World::SetRenderingCamera(0);
        }
        AddOption("Voice Changer", null, SetEnt242, SUB::VOICECHANGER);
        AddOption("Weapons", null, nullFunc, SUB::BODYGUARD_WEAPONOPS);
        AddOption("Loadouts", null, SetEnt242, SUB::WEAPONOPS_LOADOUTS);
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
    void BodyguardWeaponLoadoutOps()
    {
        if (!SelectedBodyguard || !SelectedBodyguard->Handle.Exists())
            return;

        Ped ped = SelectedBodyguard->Handle.GetHandle();

        g_WeaponOpsPedOverride = ped;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = ped;

        if (g_WeaponOpsPedOverride != 0)
        {
            g_Ped1 = g_WeaponOpsPedOverride;
            g_Ped2 = g_WeaponOpsPlayerOverride;
        }
        else
        {
            g_Ped1 = PLAYER::PLAYER_PED_ID();
            g_Ped2 = PLAYER::PLAYER_ID();
        }

        WeaponsLoadouts_catind::Sub_Loadouts_InItem();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }

}

#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(BODYGUARD_ENTITYOPS,   sub::BodyguardMenu::BodyguardEntityOps)
REGISTER_SUBMENU(BODYGUARD_WEAPONOPS,   sub::BodyguardMenu::BodyguardWeaponOps)