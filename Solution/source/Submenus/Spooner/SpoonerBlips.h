#pragma once

#include "..\..\Util\GTAmath.h"
#include <string>
#include "../../Natives/natives.h"
#include "../../Scripting/GTAblip.h"

namespace sub::Spooner
{
    class SpoonerBlipPosition
    {
    public:
        struct Attachment_t
        {
            int attachedTo;     // Entity handle
            Vector3 offset;

            Attachment_t()
                : attachedTo(0), offset(Vector3(0, 0, 0))
            {
            }

            Attachment_t& operator = (const Attachment_t& right)
            {
                this->attachedTo = right.attachedTo;
                this->offset = right.offset;
                return *this;
            }

            bool IsAttached() const
            {
                return attachedTo != 0;
            }
        };

        Vector3 m_position;
        Attachment_t m_attachmentArgs;

        SpoonerBlipPosition()
            : m_position(Vector3(0, 0, 0))
        {
        }

        SpoonerBlipPosition& operator = (const SpoonerBlipPosition& right)
        {
            this->m_position = right.m_position;
            this->m_attachmentArgs = right.m_attachmentArgs;
            return *this;
        }
    };

    class SpoonerBlip
    {
    public:

        enum class Type
        {
            Radial,
            Entity,
            Coord
        };

        enum class RadialShape
        {
            Circle,
            Square
        };

        Type BlipType = Type::Coord;
        RadialShape Shape = RadialShape::Circle;

        int EntityHandle = 0;
        std::string m_name;
        bool m_selectedInSub;

        float RadialSize = 60.0f;
        float AreaWidth = 60.0f;
        float AreaHeight = 60.0f;
        float Heading = 0.0f;
        float Scale = 0.80f;
        int Colour = 0;
        int Alpha = 255;
        int Icon = BlipIcon::Standard;
        int Priority = 2;

        bool bShowRoute = false;
        int RouteColour = BlipColour::White;
        bool bShowCone = false;
        int ConeColour = 3;
        bool bShortRange = false;
        bool bSelectableOnMap = true;
        bool bSyncRotation = false;

        float X;
        float Y;
        float Z;

        Vector3 Rotation = Vector3();
        Vector3 Offset = Vector3(0, 0, 0);
        bool bAttached = false;

        Blip BlipHandle = 0;

        std::string Name = "Blip";
        std::string label = "";

        SpoonerBlip() = default;

        SpoonerBlip(const std::string& name, const Vector3& position, const Vector3& rotation)
            : Name(name),
            X(position.x),
            Y(position.y),
            Z(position.z),
            Rotation(rotation)
        {
        }

        bool operator==(const SpoonerBlip& other) const
        {
            return this == &other;
        }

        void Create()
        {
            if (BlipType == Type::Radial)
            {
                if (Shape == RadialShape::Circle)
                    BlipHandle = HUD::ADD_BLIP_FOR_RADIUS(X, Y, Z, RadialSize);
                else
                    BlipHandle = HUD::ADD_BLIP_FOR_AREA(X, Y, Z, AreaWidth, AreaHeight);

                HUD::SET_BLIP_ROTATION_WITH_FLOAT(BlipHandle, Heading);
                HUD::SET_BLIP_ALPHA(BlipHandle, Alpha);
                HUD::SET_BLIP_COLOUR(BlipHandle, Colour);

                if (!label.empty())
                    GTAblip(BlipHandle).SetBlipName(label);
                else
                    GTAblip(BlipHandle).SetBlipName("Area");
            }
            else if (BlipType == Type::Entity)
            {
                BlipHandle = HUD::ADD_BLIP_FOR_ENTITY(EntityHandle);

                HUD::SET_BLIP_SPRITE(BlipHandle, Icon);
                HUD::SET_BLIP_SCALE(BlipHandle, Scale);
                HUD::SET_BLIP_ALPHA(BlipHandle, Alpha);
                HUD::SET_BLIP_COLOUR(BlipHandle, Colour);
                HUD::SET_BLIP_ROUTE(BlipHandle, bShowRoute);
                if (bShowRoute)
                    HUD::SET_BLIP_ROUTE_COLOUR(BlipHandle, RouteColour);
                HUD::SET_BLIP_SHOW_CONE(BlipHandle, bShowCone, ConeColour);
                HUD::SET_BLIP_AS_SHORT_RANGE(BlipHandle, bShortRange);
                HUD::SET_BLIP_PRIORITY(BlipHandle, Priority);
                HUD::SET_BLIP_DISPLAY(BlipHandle, bSelectableOnMap ? 2 : 8);

                if (!label.empty())
                    GTAblip(BlipHandle).SetBlipName(label);
                else
                    GTAblip(BlipHandle).SetBlipName(Name);
            }
            else
            {
                BlipHandle = HUD::ADD_BLIP_FOR_COORD(X, Y, Z);

                HUD::SET_BLIP_SPRITE(BlipHandle, Icon);
                HUD::SET_BLIP_SCALE(BlipHandle, Scale);
                HUD::SET_BLIP_ALPHA(BlipHandle, Alpha);
                HUD::SET_BLIP_COLOUR(BlipHandle, Colour);
                HUD::SET_BLIP_ROUTE(BlipHandle, bShowRoute);
                HUD::SET_BLIP_AS_SHORT_RANGE(BlipHandle, bShortRange);
                HUD::SET_BLIP_PRIORITY(BlipHandle, Priority);
                HUD::SET_BLIP_DISPLAY(BlipHandle, bSelectableOnMap ? 2 : 8);

                if (!label.empty())
                    GTAblip(BlipHandle).SetBlipName(label);
                else
                    GTAblip(BlipHandle).SetBlipName(BlipIcon::vNames.at(Icon));
            }
        }

        void Remove()
        {
            if (BlipHandle != 0)
            {
                HUD::REMOVE_BLIP(&BlipHandle);
                BlipHandle = 0;
            }
        }

        void Update();
    };

    namespace Submenus
    {
        void Sub_Blip_Management();
        void Sub_Blip_Select();

        void Sub_Blip_Radial();
        void Sub_Blip_Entity();
        void Sub_Blip_Coord();

        void Sub_Blip_RadialInBlip();
        void Sub_Blip_EntityInBlip();
        void Sub_Blip_CoordInBlip();

        void Sub_Blip_Attach();
        void Sub_Blip_Entity_Select();
    }

    extern SpoonerBlip* SelectedBlip;
}