#pragma once

#include "Submenus/Spooner/SpoonerBlips.h"
#include "../../Scripting/GTAentity.h"
#include "../../Natives/natives.h"

namespace sub::Spooner
{
    struct BlipMapping
    {
        int icon;
        bool syncRotation;
    };

    BlipMapping GetBlipMappingForEntity(GTAentity& ent);
}