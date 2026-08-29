#include "BlipCustoms.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\enums.h"

#include "Databases.h"
#include "SpoonerBlips.h"

#include <algorithm>


namespace sub::Spooner
{
    namespace BlipCustoms
    {

        void DrawAll()
        {
            for (auto& blip : Databases::BlipDb)
            {
                Vector3 pos(blip.X, blip.Y, blip.Z);

                bool bSelectedInSub = blip.m_selectedInSub;
                blip.m_selectedInSub = false;

                RGBA baseColour = RGBA(255, 255, 0, 190);
                RGBA selectedColour = RGBA(baseColour.Inverse(false), 240);

                World::DrawMarker(
                    MarkerType::DebugSphere,
                    pos,
                    Vector3(),
                    Vector3(),
                    Vector3(0.25f, 0.25f, 0.25f),
                    bSelectedInSub ? selectedColour : baseColour,
                    bSelectedInSub,
                    false,
                    2,
                    false,
                    std::string(),
                    std::string(),
                    false
                );

                World::DrawLightWithRange(
                    pos,
                    bSelectedInSub ? RGBA(baseColour.Inverse(false), 150) : baseColour,
                    2.3f,
                    1.5f
                );
            }
        }

        SpoonerBlip* AddBlip(const std::string& name, const Vector3& position, const Vector3& rotation)
        {
            Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
            Databases::BlipDb.push_back(SpoonerBlip(name, position, rotation));

            SpoonerBlip& newBlip = Databases::BlipDb.back();

            newBlip.BlipType = SpoonerBlip::Type::Radial;

            newBlip.Create();

            return &newBlip;
        }

        SpoonerBlip* AddBlip(const Vector3& position, const Vector3& rotation)
        {
            Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
            Databases::BlipDb.push_back(SpoonerBlip("Radial Blip", position, rotation));

            SpoonerBlip& newBlip = Databases::BlipDb.back();

            newBlip.BlipType = SpoonerBlip::Type::Radial;

            newBlip.Create();

            return &newBlip;
        }

        SpoonerBlip* AddBlip(SpoonerBlip::Type type, const std::string& name)
        {
            Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
            Databases::BlipDb.push_back(SpoonerBlip(name, Vector3(), Vector3()));

            SpoonerBlip& newBlip = Databases::BlipDb.back();

            newBlip.BlipType = type;


            return &newBlip;
        }

        void GetAllBlipsInRange(std::vector<SpoonerBlip>& result, const Vector3& position, float radius)
        {
            result.clear();

            for (auto& blip : Databases::BlipDb)
            {
                Vector3 blipPos(blip.X, blip.Y, blip.Z);

                if (position.DistanceTo(blipPos) < radius)
                {
                    result.push_back(blip);
                }
            }
        }

        void ClearDb()
        {
            Databases::BlipDb.clear();
        }

        std::vector<SpoonerBlip>::iterator RemoveBlip(std::vector<SpoonerBlip>::iterator it)
        {
            it->Remove();
            return Databases::BlipDb.erase(it);
        }

        void RemoveBlip(SpoonerBlip& blip)
        {
            auto it = std::find(Databases::BlipDb.begin(), Databases::BlipDb.end(), blip);

            if (it != Databases::BlipDb.end())
            {
                RemoveBlip(it);
            }
        }

        void RemoveBlip(int indexInDb)
        {
            if (indexInDb >= 0 && indexInDb < Databases::BlipDb.size())
            {
                RemoveBlip(Databases::BlipDb.begin() + indexInDb);
            }
        }

        void RemoveAllBlips()
        {
            for (auto& blip : Databases::BlipDb)
            {
                blip.Remove();
            }

            Databases::BlipDb.clear();
        }

        void RefreshBlip(SpoonerBlip& blip)
        {
            blip.Remove();
            blip.Create();
        }

        void RefreshAllBlips()
        {
            for (auto& b : Databases::BlipDb)
            {
                b.Remove();
                b.Create();
            }
        }

        SpoonerBlip* CopyBlip(SpoonerBlip& blip)
        {
            SpoonerBlip newBlip = blip;

            Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
            Databases::BlipDb.push_back(newBlip);

            SpoonerBlip& copiedBlip = Databases::BlipDb.back();

            copiedBlip.Create();

            return &copiedBlip;
        }

        void UpdateAttachedBlips()
        {
            for (auto& blip : Databases::BlipDb)
            {
                if (blip.bAttached && blip.EntityHandle != 0 && ENTITY::DOES_ENTITY_EXIST(blip.EntityHandle)
                    && blip.BlipType != SpoonerBlip::Type::Entity)
                {
                    Vector3 worldPos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(
                        blip.EntityHandle,
                        blip.Offset.x,
                        blip.Offset.y,
                        blip.Offset.z
                    );

                    if (blip.X != worldPos.x || blip.Y != worldPos.y || blip.Z != worldPos.z)
                    {
                        blip.X = worldPos.x;
                        blip.Y = worldPos.y;
                        blip.Z = worldPos.z;

                        blip.Remove();
                        blip.Create();
                    }
                }

                if (blip.BlipType == SpoonerBlip::Type::Entity && blip.EntityHandle != 0 && ENTITY::DOES_ENTITY_EXIST(blip.EntityHandle))
                {
                    if (blip.bSyncRotation)
                    {
                        float heading = ENTITY::GET_ENTITY_HEADING(blip.EntityHandle);
                        HUD::SET_BLIP_ROTATION_WITH_FLOAT(blip.BlipHandle, heading);
                    }
                }
            }
        }

        void AttachBlipToEntity(SpoonerBlip& blip, int entity, const Vector3& offset)
        {
            blip.EntityHandle = entity;
            blip.Offset = offset;
            blip.bAttached = true;
        }

        void DetachBlip(SpoonerBlip& blip)
        {
            blip.bAttached = false;
            blip.EntityHandle = 0;
        }
    }
}