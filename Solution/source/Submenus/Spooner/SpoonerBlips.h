#pragma once

#include "..\..\Util\GTAmath.h"
#include <string>

namespace sub::Spooner
{

    class SpoonerBlip
    {
    public:

        int Handle = 0;

        enum class Type
        {
            Radial,
            Entity,
            Coord
        };

        Type BlipType = Type::Coord;

        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        Vector3 Rotation = Vector3();

        float Radius = 0.0f;

        int Entity = 0;

        std::string Name = "BLIP";

        SpoonerBlip() = default;

        SpoonerBlip(const std::string& name, const Vector3& position, const Vector3& rotation)
            : Handle(0), BlipType(Type::Coord),
            X(position.x), Y(position.y), Z(position.z),
            Rotation(rotation),
            Radius(0.0f), Entity(0), Name(name)
        {
        }

        bool operator==(const SpoonerBlip& other) const
        {
            return this == &other;
        }

        void Create();
        void Remove();
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

	}

	// Selected blip pointer
	extern SpoonerBlip* SelectedBlip;

}