/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "SpoonerLight.h"

#include "..\..\macros.h"

#include "..\..\Util\ExePath.h"
#include "..\..\Util\GTAmath.h"

#include "Databases.h"
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\enums.h"

#include <pugixml/src/pugixml.hpp>
#include <string>
#include <vector>

namespace sub::Spooner
{
	UINT SpoonerLight::iLightIdIter = 0;

	SpoonerLight* SelectedLight = nullptr;

	SpoonerLight::SpoonerLight()
		: m_id(++iLightIdIter), m_name("Light"), m_lightType(LightType::Omni),
		m_active(true), m_selectedInSub(false),
		m_colour(255, 255, 255, 255), m_position(),
		m_range(10.0f), m_intensity(1.0f),
		m_direction(0, 0, -1),
		m_spotDistance(20.0f), m_spotBrightness(1.0f), m_spotRoundness(0.0f),
		m_spotRadius(7.5f), m_spotFalloff(0.0f), m_useShadow(false), m_shadowId(0)
	{
	}

	SpoonerLight::SpoonerLight(const Vector3& position, const Vector3& direction)
		: m_id(++iLightIdIter), m_name("Light"), m_lightType(LightType::Omni),
		m_active(true), m_selectedInSub(false),
		m_colour(255, 255, 255, 255), m_position(position),
		m_range(10.0f), m_intensity(1.0f),
		m_direction(direction),
		m_spotDistance(20.0f), m_spotBrightness(1.0f), m_spotRoundness(0.0f),
		m_spotRadius(7.5f), m_spotFalloff(0.0f), m_useShadow(false), m_shadowId(0)
	{
	}

	void SpoonerLight::Draw() const
	{
		if (!m_active)
			return;

		RgbS rgbColour(m_colour);

		if (m_lightType == LightType::Omni)
		{
			OmniLight light(m_position, rgbColour, m_range, m_intensity);
			light.Draw();
		}
		else
		{
			SpotLight light(m_position, m_direction, rgbColour, m_spotDistance, m_spotBrightness, m_spotRoundness, m_spotRadius, m_spotFalloff, m_useShadow, m_shadowId);
			light.Draw();
		}
	}

	namespace LightManagement
	{
		std::vector<SpoonerLight> PresetDb;

		void DrawAll()
		{
			for (auto& light : Databases::LightDb)
			{
				light.Draw();
			}
		}

		void DrawPreviewMarkers()
		{
			for (auto& light : Databases::LightDb)
			{
				World::DrawMarker(MarkerType::DebugSphere, light.m_position, Vector3(), Vector3(), Vector3(0.15f, 0.15f, 0.15f), RGBA(light.m_colour, 200));
				if (light.m_lightType == SpoonerLight::LightType::Spot)
				{
					World::DrawLine(light.m_position, light.m_position + light.m_direction * light.m_spotDistance, RGBA(light.m_colour, 200));
				}
			}
		}

		SpoonerLight* Add(const SpoonerLight& light)
		{
			Databases::LightDb.push_back(light);
			return &Databases::LightDb.back();
		}

		void Remove(int indexInDb)
		{
			if (indexInDb >= 0 && indexInDb < static_cast<int>(Databases::LightDb.size()))
				Databases::LightDb.erase(Databases::LightDb.begin() + indexInDb);
		}

		void RemoveAll()
		{
			Databases::LightDb.clear();
		}

		void Clear()
		{
			Databases::LightDb.clear();
			SpoonerLight::iLightIdIter = 0;
		}

		SpoonerLight* Copy(const SpoonerLight& light)
		{
			SpoonerLight copy = light;
			copy.m_id = ++SpoonerLight::iLightIdIter;
			Databases::LightDb.push_back(copy);
			return &Databases::LightDb.back();
		}

		bool LoadPresetsFromFile(const std::string& filePath)
		{
			pugi::xml_document doc;
			if (doc.load_file(filePath.c_str()).status != pugi::status_ok)
				return false;

			PresetDb.clear();
			for (auto nodeLight = doc.child("SavedLights").child("Light"); nodeLight; nodeLight = nodeLight.next_sibling("Light"))
			{
				SpoonerLight light;
				light.m_name = nodeLight.child("Name").text().as_string();
				light.m_lightType = static_cast<SpoonerLight::LightType>(nodeLight.child("Type").text().as_uint());

				auto nodeColour = nodeLight.child("Colour");
				light.m_colour.R = nodeColour.attribute("R").as_int(255);
				light.m_colour.G = nodeColour.attribute("G").as_int(255);
				light.m_colour.B = nodeColour.attribute("B").as_int(255);
				light.m_colour.A = nodeColour.attribute("A").as_int(255);

				light.m_range = nodeLight.child("Range").text().as_float(10.0f);
				light.m_intensity = nodeLight.child("Intensity").text().as_float(1.0f);

				auto nodeDir = nodeLight.child("Direction");
				if (nodeDir)
				{
					light.m_direction.x = nodeDir.attribute("X").as_float(0);
					light.m_direction.y = nodeDir.attribute("Y").as_float(0);
					light.m_direction.z = nodeDir.attribute("Z").as_float(-1);
				}

				light.m_spotDistance = nodeLight.child("SpotDistance").text().as_float(20.0f);
				light.m_spotBrightness = nodeLight.child("SpotBrightness").text().as_float(1.0f);
				light.m_spotRoundness = nodeLight.child("SpotRoundness").text().as_float(0.0f);
				light.m_spotRadius = nodeLight.child("SpotRadius").text().as_float(1.0f);
				light.m_spotFalloff = nodeLight.child("SpotFalloff").text().as_float(0.0f);
				light.m_useShadow = nodeLight.child("UseShadow").text().as_bool(false);
				light.m_shadowId = nodeLight.child("ShadowId").text().as_int(0);

				PresetDb.push_back(light);
			}
			return true;
		}

		bool SavePresetToFile(const SpoonerLight& light)
		{
			std::string filePath = GetPathffA(Pathff::RootDir, true) + "FavouriteLights.xml";

			pugi::xml_document doc;
			if (doc.load_file(filePath.c_str()).status != pugi::status_ok)
			{
				auto decl = doc.append_child(pugi::node_declaration);
				decl.append_attribute("version") = "1.0";
				decl.append_attribute("encoding") = "ISO-8859-1";
				doc.append_child("SavedLights");
			}

			auto nodeLight = doc.child("SavedLights").append_child("Light");

			nodeLight.append_child("Name").text() = light.m_name.c_str();
			nodeLight.append_child("Type").text() = static_cast<UINT>(light.m_lightType);

			auto nodeColour = nodeLight.append_child("Colour");
			nodeColour.append_attribute("R") = light.m_colour.R;
			nodeColour.append_attribute("G") = light.m_colour.G;
			nodeColour.append_attribute("B") = light.m_colour.B;
			nodeColour.append_attribute("A") = light.m_colour.A;

			nodeLight.append_child("Range").text() = light.m_range;
			nodeLight.append_child("Intensity").text() = light.m_intensity;

			auto nodeDir = nodeLight.append_child("Direction");
			nodeDir.append_attribute("X") = light.m_direction.x;
			nodeDir.append_attribute("Y") = light.m_direction.y;
			nodeDir.append_attribute("Z") = light.m_direction.z;

			nodeLight.append_child("SpotDistance").text() = light.m_spotDistance;
			nodeLight.append_child("SpotBrightness").text() = light.m_spotBrightness;
			nodeLight.append_child("SpotRoundness").text() = light.m_spotRoundness;
			nodeLight.append_child("SpotRadius").text() = light.m_spotRadius;
			nodeLight.append_child("SpotFalloff").text() = light.m_spotFalloff;
			nodeLight.append_child("UseShadow").text() = light.m_useShadow;
			nodeLight.append_child("ShadowId").text() = light.m_shadowId;

			return doc.save_file(filePath.c_str());
		}

		bool SaveAllPresetsToFile()
		{
			std::string filePath = GetPathffA(Pathff::RootDir, true) + "FavouriteLights.xml";

			pugi::xml_document doc;
			auto decl = doc.append_child(pugi::node_declaration);
			decl.append_attribute("version") = "1.0";
			decl.append_attribute("encoding") = "ISO-8859-1";
			auto root = doc.append_child("SavedLights");

			for (auto& p : PresetDb)
			{
				auto nodeLight = root.append_child("Light");
				nodeLight.append_child("Name").text() = p.m_name.c_str();
				nodeLight.append_child("Type").text() = static_cast<UINT>(p.m_lightType);

				auto nodeColour = nodeLight.append_child("Colour");
				nodeColour.append_attribute("R") = p.m_colour.R;
				nodeColour.append_attribute("G") = p.m_colour.G;
				nodeColour.append_attribute("B") = p.m_colour.B;
				nodeColour.append_attribute("A") = p.m_colour.A;

				nodeLight.append_child("Range").text() = p.m_range;
				nodeLight.append_child("Intensity").text() = p.m_intensity;

				auto nodeDir = nodeLight.append_child("Direction");
				nodeDir.append_attribute("X") = p.m_direction.x;
				nodeDir.append_attribute("Y") = p.m_direction.y;
				nodeDir.append_attribute("Z") = p.m_direction.z;

				nodeLight.append_child("SpotDistance").text() = p.m_spotDistance;
				nodeLight.append_child("SpotBrightness").text() = p.m_spotBrightness;
				nodeLight.append_child("SpotRoundness").text() = p.m_spotRoundness;
				nodeLight.append_child("SpotRadius").text() = p.m_spotRadius;
				nodeLight.append_child("SpotFalloff").text() = p.m_spotFalloff;
				nodeLight.append_child("UseShadow").text() = p.m_useShadow;
				nodeLight.append_child("ShadowId").text() = p.m_shadowId;
			}

			return doc.save_file(filePath.c_str());
		}
	}
}
