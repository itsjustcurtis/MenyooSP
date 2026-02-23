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
        void ShowArrowAboveEntity(const GTAentity& ent, RGBA colour = {255, 0, 0, 190});
    }
}
