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
    void ComponentChanger_();
}

namespace sub::BodyguardMenu
{
    void SetEnt242() { Static_241= SelectedBodyguard->Handle.Handle(); }
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
                    title = int_to_hexstring(model.hash, true);
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
            World::RenderingCamera_set(0);
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
            Static_241 = g_WeaponOpsPedOverride;
            Static_240 = g_WeaponOpsPlayerOverride;
        }
        else
        {
            Static_241 = PLAYER::PLAYER_PED_ID();
            Static_240 = PLAYER::PLAYER_ID();
        }

        WeaponsLoadouts_catind::Sub_Loadouts_InItem();

        g_WeaponOpsPedOverride = 0;
        g_WeaponOpsPlayerOverride = -1;
        g_WeaponMenuPedOverride = 0;
    }

}
