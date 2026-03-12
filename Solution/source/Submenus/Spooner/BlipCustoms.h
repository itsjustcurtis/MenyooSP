#pragma once

#include "SpoonerBlips.h"
#include "..\..\Util\GTAmath.h"

#include <string>
#include <vector>

namespace sub::Spooner
{
    namespace BlipCustoms
    {
        void DrawAll();

        SpoonerBlip* AddBlip(const std::string& name, const Vector3& position, const Vector3& rotation);
        SpoonerBlip* AddBlip(const Vector3& position, const Vector3& rotation);

        void GetAllBlipsInRange(std::vector<SpoonerBlip>& result, const Vector3& position, float radius);

        void ClearDb();
        std::vector<SpoonerBlip>::iterator RemoveBlip(std::vector<SpoonerBlip>::iterator it);
        void RemoveBlip(SpoonerBlip& blip);
        void RemoveBlip(int indexInDb);
        void RemoveAllBlips();

        SpoonerBlip* CopyBlip(SpoonerBlip& blip);
    }
}