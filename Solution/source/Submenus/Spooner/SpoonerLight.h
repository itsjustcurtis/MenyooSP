/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include "..\..\Scripting\GTAlight.h"
#include "..\..\Natives\types.h"

#include <string>
#include <vector>

typedef unsigned char UINT8;
typedef unsigned int UINT;

namespace pugi {
	class xml_node;
}

namespace sub::Spooner
{
	class SpoonerLight
	{
	public:
		enum class LightType : UINT8 { Omni = 0, Spot = 1 };
		static UINT iLightIdIter;

		UINT m_id;
		std::string m_name;
		LightType m_lightType;
		bool m_active;
		bool m_selectedInSub;

		RGBA m_colour;

		Vector3 m_position;

		// Omni
		float m_range;
		float m_intensity;

		// Spot
		Vector3 m_direction;
		float m_spotDistance;
		float m_spotBrightness;
		float m_spotRoundness;
		float m_spotRadius;
		float m_spotFalloff;
		bool m_useShadow;
		int m_shadowId;

		SpoonerLight();
		SpoonerLight(const Vector3& position, const Vector3& direction);

		void Draw() const;
	};

	namespace LightManagement
	{
		void DrawAll();
		SpoonerLight* Add(const SpoonerLight& light);
		void Remove(int indexInDb);
		void RemoveAll();
		void Clear();
		SpoonerLight* Copy(const SpoonerLight& light);

		bool LoadPresetsFromFile(const std::string& filePath);
		bool SavePresetToFile(const SpoonerLight& light);
		bool SaveAllPresetsToFile();
		void DrawPreviewMarkers();
		void RemoveOrphanedLightTasks(UINT lightId, const std::string& lightName);
		extern std::vector<SpoonerLight> PresetDb;
	}

	extern SpoonerLight* SelectedLight;
}
