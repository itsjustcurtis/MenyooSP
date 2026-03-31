/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "MiscMapMods.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAprop.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"

#include "..\Submenus\Teleport\TeleMethods.h"

#include <pugixml\src\pugixml.hpp>
#include <string>
#include <vector>

namespace sub
{
	namespace MapMods_catind
	{
		void mm_unloadAll();

		void mm_PlaceObject(std::vector<GTAentity>& handles, Hash hash, float X, float Y, float Z, float pitch, float roll, float yaw, bool noLongerNeeded = false)
		{
			Model model(hash);
			GTAprop e = World::CreateProp(hash, Vector3(X, Y, Z), Vector3(pitch, roll, yaw), false, false);
			e.FreezePosition(true);
			SET_NETWORK_ID_CAN_MIGRATE(OBJ_TO_NET(e.Handle()), TRUE);
			e.LodDistance_set(1000000);
			e.MissionEntity_set(true);
			model.Unload();
			if (noLongerNeeded)
				SET_OBJECT_AS_NO_LONGER_NEEDED(&e.Handle());

			handles.push_back(e.GetHandle());
		}

		void mm_PlaceRawObject(std::vector<GTAentity>& handles, Hash hash, float X, float Y, float Z, float pitch, float roll, float yaw,
			bool hasQuaternion, float qx, float qy, float qz, float qw)
		{
			Object tempEntity = CREATE_OBJECT(hash, X, Y, Z, 1, 1, 0);
			SET_ENTITY_COORDS(tempEntity, X, Y, Z, 1, 0, 0, 1);
			if (pitch != 0.0f || roll != 0.0f || yaw != 0.0f)
			{
				SET_ENTITY_ROTATION(tempEntity, pitch, roll, yaw, 2, 1);
			}
			if (hasQuaternion)
			{
				SET_ENTITY_QUATERNION(tempEntity, qx, qy, qz, qw);
			}
			FREEZE_ENTITY_POSITION(tempEntity, 1);
			handles.push_back(tempEntity);
		}

		class GTAmapmod
		{
		private:
			std::string name;
			std::string author;
			Vector3 location;
			pugi::xml_node xmlNode;
			std::vector<GTAentity> ohandles;
		public:
			GTAmapmod(const std::string& newName, const std::string& newAuthor, Vector3 newLocation, pugi::xml_node node)
				: name(newName), author(newAuthor), location(newLocation), xmlNode(node)
			{
			}
			void Tele()
			{
				teleport_net_ped(Game::PlayerPed(), location.x, location.y, location.z);
			}
			bool IsLoaded()
			{
				return !ohandles.empty();
			}
			void Load(bool showMsg = true)
			{
				mm_unloadAll();
				LoadFromXml();
				if (showMsg)
					Game::Print::PrintBottomCentre(name + " ~b~loaded~s~.");
			}
			void Unload(bool showMsg = true)
			{
				for (auto& obj : ohandles)
				{
					obj.Delete(1);
				}
				ohandles.clear();

				if (showMsg)
					Game::Print::PrintBottomCentre(name + " ~p~unloaded~s~.");
			}
			const std::string& Name()
			{
				return name;
			}

		private:
			void LoadFromXml()
			{
				// Handle loop-based mods
				pugi::xml_node loopNode = xmlNode.child("Loop");
				if (loopNode)
				{
					int count = loopNode.attribute("count").as_int();
					float zStart = loopNode.attribute("zStart").as_float();
					float zIncrement = loopNode.attribute("zIncrement").as_float();

					pugi::xml_node objNode = loopNode.child("Object");
					if (objNode)
					{
						Hash hash = (Hash)objNode.attribute("hash").as_llong();
						float x = objNode.attribute("x").as_float();
						float y = objNode.attribute("y").as_float();
						float pitch = objNode.attribute("pitch").as_float();
						float roll = objNode.attribute("roll").as_float();
						float yaw = objNode.attribute("yaw").as_float();
						bool noLongerNeeded = objNode.attribute("noLongerNeeded").as_bool(false);

						float tempZ = zStart;
						for (int i = 0; i < count; i++)
						{
							tempZ += zIncrement;
							mm_PlaceObject(ohandles, hash, x, y, tempZ, pitch, roll, yaw, noLongerNeeded);
						}
					}
				}

				// Handle regular objects
				pugi::xml_node objectsNode = xmlNode.child("Objects");
				if (objectsNode)
				{
					for (pugi::xml_node objNode = objectsNode.child("Object"); objNode; objNode = objNode.next_sibling("Object"))
					{
						Hash hash = (Hash)objNode.attribute("hash").as_llong();
						float x = objNode.attribute("x").as_float();
						float y = objNode.attribute("y").as_float();
						float z = objNode.attribute("z").as_float();
						float pitch = objNode.attribute("pitch").as_float();
						float roll = objNode.attribute("roll").as_float();
						float yaw = objNode.attribute("yaw").as_float();
						bool noLongerNeeded = objNode.attribute("noLongerNeeded").as_bool(false);
						bool raw = objNode.attribute("raw").as_bool(false);

						if (raw)
						{
							bool hasQuat = objNode.attribute("qx");
							float qx = objNode.attribute("qx").as_float();
							float qy = objNode.attribute("qy").as_float();
							float qz = objNode.attribute("qz").as_float();
							float qw = objNode.attribute("qw").as_float();
							mm_PlaceRawObject(ohandles, hash, x, y, z, pitch, roll, yaw, hasQuat, qx, qy, qz, qw);
						}
						else
						{
							mm_PlaceObject(ohandles, hash, x, y, z, pitch, roll, yaw, noLongerNeeded);
						}
					}
				}

				if (!author.empty())
					Game::Print::PrintBottomLeft("Hax by ~b~" + author);
			}
		};

		pugi::xml_document mapModsDoc;
		std::vector<GTAmapmod> vAllMapMods;
		GTAmapmod *currentMAPMODC;

		void mm_unloadAll()
		{
			for (GTAmapmod& mm : vAllMapMods)
				mm.Unload(false);
		}

		void LoadMapModsFromXml()
		{
			vAllMapMods.clear();

			std::string xmlPath = GetPathffA(Pathff::Main, true) + "MapMods.xml";
			if (mapModsDoc.load_file(xmlPath.c_str()).status != pugi::status_ok)
				return;

			pugi::xml_node root = mapModsDoc.document_element();
			for (pugi::xml_node modNode = root.child("MapMod"); modNode; modNode = modNode.next_sibling("MapMod"))
			{
				std::string name = modNode.attribute("name").as_string();
				std::string author = modNode.attribute("author").as_string();

				pugi::xml_node locNode = modNode.child("Location");
				Vector3 location;
				location.x = locNode.attribute("x").as_float();
				location.y = locNode.attribute("y").as_float();
				location.z = locNode.attribute("z").as_float();

				vAllMapMods.emplace_back(name, author, location, modNode);
			}
		}

		void __AddpointOption(GTAmapmod &mm)
		{
			bool pressed = false;
			AddOption(mm.Name(), pressed, nullFunc, SUB::MAPMODS2, false); if (pressed)
			{
				currentMAPMODC = &mm;
			}
		}

		void MapMods_()
		{
			if (vAllMapMods.empty())
				LoadMapModsFromXml();

			AddTitle("Map Mods");

			for (auto& mm : vAllMapMods)
				__AddpointOption(mm);

		}
		void MapMods2_()
		{

			bool cmmtele = 0, cmmload = 0, cmmunload = 0;

			AddTitle(currentMAPMODC->Name());

			AddOption("Teleport", cmmtele);
			AddLocal("Load", currentMAPMODC->IsLoaded(), cmmload, cmmunload);

			if (cmmtele) currentMAPMODC->Tele();

			if (cmmload) currentMAPMODC->Load();

			if (cmmunload) currentMAPMODC->Unload();

		}

	}

}

#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(MAPMODS,            sub::MapMods_catind::MapMods_)
REGISTER_SUBMENU(MAPMODS2,           sub::MapMods_catind::MapMods2_)
