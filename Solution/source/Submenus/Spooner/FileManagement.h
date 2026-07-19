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

#include <string>
#include <unordered_set>
#include <vector>

typedef int ScrHandle, Entity, Ped, Vehicle;
typedef unsigned long DWORD, Hash;

class GTAped;
namespace GTAmodel { class Model; }

namespace pugi {
	class xml_node;
}

namespace sub::Spooner
{
	class SpoonerEntity;
	class SpoonerEntityWithInitHandle;
	class SpoonerMarker;
	class SpoonerMarkerWithInitHandle;
	class SpoonerLight;

	namespace FileManagement
	{
		extern std::string _oldAudioAlias;

		//bool Exists(const std::string& fileName, std::string extension = ".xml");
		//bool Rename(const std::string& oldName, const std::string& newName, std::string extension = ".xml");
		//bool Delete(const std::string& fileName, std::string extension = ".xml");

		void AddEntityToXmlNode(SpoonerEntity& e, pugi::xml_node& nodeEntity, bool legacyXMLFormat = false);
		SpoonerEntityWithInitHandle SpawnEntityFromXmlNode(pugi::xml_node& nodeEntity, std::unordered_set<Hash>& vModelHashes);

		// Shared ped sub-loaders (backward-compatible with old _N and new named format)
		void LoadPedCompsFromXml(GTAped ep, const pugi::xml_node& nodePedComps);
		void LoadPedPropsFromXml(GTAped ep, const pugi::xml_node& nodePedProps, bool bNetworkIsGameInProgress);
		void LoadPedHeadFeaturesFromXml(GTAped ep, const pugi::xml_node& nodePedHeadFeatures, const GTAmodel::Model& eModel);
		void LoadPedDecalsFromXml(GTAped ep, const pugi::xml_node& nodePedDecals);
		void LoadPedDamagePacksFromXml(GTAped ep, const pugi::xml_node& nodePedDamagePacks);

		void AddMarkerToXmlNode(SpoonerMarker& m, pugi::xml_node& nodeMarker);
		SpoonerMarkerWithInitHandle SpawnMarkerFromXmlNode(pugi::xml_node& nodeMarker);

		void AddLightToXmlNode(SpoonerLight& l, pugi::xml_node& nodeLight);
		void SpawnLightFromXmlNode(pugi::xml_node& nodeLight);

		bool SaveDbToFile(const std::string& filePath, bool bForceReferenceCoords);
		bool SaveWorldToFile(const std::string& filePath, std::vector<Entity>& vEntityHandles, std::vector<SpoonerMarker>& vMarkers);
		bool LoadPlacementsFromFile(const std::string& filePath);

		bool LoadPlacementsFromSP00NFile(const std::string& filePath);

		bool TeleportToReference(const std::string& filePath);
	}

}



