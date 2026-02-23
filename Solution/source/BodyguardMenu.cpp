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

#include "Scripting/GTAblip.h"
using namespace sub::BodyguardMenu;

namespace sub::BodyguardMenu
{
    int armor = 200;
    int health = 200;
    bool godmode = true;
    int blipIcon = 1; // 1 = Standard, 280 = Friend, 480 = VIP
}

static constexpr int BLIP_COLOUR_BLUELIGHT = 3;

void sub::BodyguardMenu::RemoveBodyguardBlip(Ped ped)
{
    if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
        return;

    Blip blip = GET_BLIP_FROM_ENTITY(ped);
    if (!blip)
        return;

    GTAblip gtaBlip(blip);
    if (gtaBlip.Exists())
        gtaBlip.Remove();
}

void sub::BodyguardMenu::ApplyBodyguardBlip(Ped ped, int icon)
{
    if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
        return;

    RemoveBodyguardBlip(ped);

    Blip blip = ADD_BLIP_FOR_ENTITY(ped);
    if (!blip)
        return;

    SET_BLIP_SPRITE(blip, icon);
    SET_BLIP_SCALE(blip, 0.80f);
    SET_BLIP_COLOUR(blip, BLIP_COLOUR_BLUELIGHT);
    SET_BLIP_AS_FRIENDLY(blip, true);
}

void sub::BodyguardMenu::RefreshAllBodyguardBlips()
{
    for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
    {
        auto& bg = sub::BodyguardMenu::BodyguardDb[i];
        if (!bg.Handle.Exists())
            continue;

        Ped ped = bg.Handle.GetHandle();
        if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
            continue;

        int hp = ENTITY::GET_ENTITY_HEALTH(ped);
        if (hp <= 0)
            ApplyBodyguardBlip(ped, 274); // dead blip
        else
            ApplyBodyguardBlip(ped, sub::BodyguardMenu::blipIcon);
    }
}

void sub::BodyguardMenu::UpdateBodyguardBlipsOnDeath()
{
    for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
    {
        auto& bg = sub::BodyguardMenu::BodyguardDb[i];
        if (!bg.Handle.Exists())
            continue;

        Ped ped = bg.Handle.GetHandle();
        if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
            continue;

        int hp = ENTITY::GET_ENTITY_HEALTH(ped);
        if (hp <= 0)
        {
            Blip blip = GET_BLIP_FROM_ENTITY(ped);
            if (!blip || GET_BLIP_SPRITE(blip) != 274)
                ApplyBodyguardBlip(ped, 274);
            RefreshAllBodyguardBlips();
        }
    }
    void sub::BodyguardMenu::RefreshAllBodyguardBlips();
    {
        for (auto& bg : BodyguardDb)
        {
            if (!bg.Handle.Exists())
                continue;

            Ped ped = bg.Handle.GetHandle();
            if (!ped || !ENTITY::DOES_ENTITY_EXIST(ped))
                continue;

            Blip blip = GET_BLIP_FROM_ENTITY(ped);
            if (blip)
            {
                SET_BLIP_SCALE(blip, 0.8f);
                SET_BLIP_AS_SHORT_RANGE(blip, true);
            }
        }
    }
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
                catch (...) { Game::Print::PrintError_InvalidInput(inputStr); }
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
                catch (...) { Game::Print::PrintError_InvalidInput(inputStr); }
            }
        }

        bool oldGodmode = sub::BodyguardMenu::godmode;
        AddToggle("Godmode", sub::BodyguardMenu::godmode);

        if (oldGodmode != sub::BodyguardMenu::godmode)
        {
            for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
            {
                auto& bg = sub::BodyguardMenu::BodyguardDb[i];
                if (!bg.Handle.Exists()) continue;

                Ped ped = bg.Handle.GetHandle();
                if (sub::BodyguardMenu::godmode) set_ped_invincible_on(ped);
                else set_ped_invincible_off(ped);
            }
        }

        static int blipIndex = 0;
        static const std::vector<std::pair<int, std::string>> blipOptions =
        {
            { 1,   "Standard" },
            { 280, "Friend"   },
            { 480, "VIP"      }
        };

        // show labels using AddTexter
        static bool bBlipInput = false;
        bool icon_plus = false, icon_minus = false;

        // current label from our restricted blipOptions array
        AddTexter("Bodyguard Blip", 0, { blipOptions[blipIndex].second }, bBlipInput, icon_plus, icon_minus);

        // scroll through our 3-choice array
        if (icon_plus)
        {
            blipIndex = (blipIndex + 1) % blipOptions.size();
            sub::BodyguardMenu::blipIcon = blipOptions[blipIndex].first;
            sub::BodyguardMenu::RefreshAllBodyguardBlips();
        }

        if (icon_minus)
        {
            blipIndex = (blipIndex == 0 ? (int)blipOptions.size() - 1 : blipIndex - 1);
            sub::BodyguardMenu::blipIcon = blipOptions[blipIndex].first;
            sub::BodyguardMenu::RefreshAllBodyguardBlips();
        }
        bool bTeleportBodyguards = false;
        AddOption("Bring Bodyguards To Self", bTeleportBodyguards);

        if (bTeleportBodyguards)
        {
            Ped playerPed = PLAYER_PED_ID();
            if (ENTITY::DOES_ENTITY_EXIST(playerPed))
            {
                Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, true);
                Vector3 forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(playerPed);

                float baseDist = 3.0f;
                float spacing = 0.75f;
                int placed = 0;

                for (unsigned int i = 0; i < sub::BodyguardMenu::BodyguardDb.size(); ++i)
                {
                    auto& bg = sub::BodyguardMenu::BodyguardDb[i];
                    if (!bg.Handle.Exists())
                        continue;

                    Ped ped = bg.Handle.GetHandle();

                    Vector3 targetPos =
                        playerPos +
                        (forward * baseDist) +
                        Vector3(0.0f, 0.0f, 0.2f) +
                        (forward * (spacing * placed));

                    ENTITY::SET_ENTITY_COORDS_NO_OFFSET(
                        ped,
                        targetPos.x,
                        targetPos.y,
                        targetPos.z,
                        false, false, false
                    );

                    placed++;
                }

                Game::Print::PrintBottomLeft("Bodyguards teleported.");
            }
        }
    }
}