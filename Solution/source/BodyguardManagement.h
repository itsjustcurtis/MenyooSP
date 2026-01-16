#pragma once

#include <vector>
#include <string>

#include "Scripting/GTAentity.h"
#include "Natives/types.h"

namespace sub::BodyguardMenu
{
    class BodyguardEntity
    {
    public:
        EntityType Type{};
        std::string Name;
        std::string HashName;
        GTAentity Handle;
    };

    extern std::vector<BodyguardEntity> BodyguardDb;

    bool operator==(const BodyguardEntity& a, const BodyguardEntity& b);
    bool operator!=(const BodyguardEntity& a, const BodyguardEntity& b);

    namespace BodyguardManagement
    {
        unsigned int GetNumberOfBodyguardsSpawned(const EntityType&);
        int GetBodyguardIndexInDb(const GTAentity&);
        int GetBodyguardIndexInDb(const BodyguardEntity&);
        void RemoveBodyguardFromDb(const BodyguardEntity&);
        void DeleteBodyguard(BodyguardEntity&);
        void AddBodyguardToDb(BodyguardEntity ent);

        // Draws an arrow above the specified entity to highlight it in the world
        void ShowArrowAboveEntity(const GTAentity& entity);
        // Give a weapon directly to a bodyguard entity (safe, ped-targeted native calls).
        void GiveWeaponToBodyguard(const BodyguardEntity& target, Hash weaponHash, int ammo = 250, bool equip = true);

        // Create a menu option that gives the specified weapon to the specified bodyguard when selected.
        // Mimics the AddOption_* helpers you already use for models.
        void AddOption_GiveWeaponToBodyguard(const std::string& label, Hash weaponHash, const BodyguardEntity& target);
    }
}
