/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

/////////////////////////////
// Player Options -> Wardrobe
/////////////////////////////

#include "PedComponentChanger.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\Routine.h"

#include "..\Memory\GTAmemory.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\GameplayCamera.h"
#include "..\Scripting\World.h"
#include "Spooner\SpoonerMode.h"
#include "..\Scripting\Game.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"

#include "..\Menu\FolderPreviewBmps.h"
#include "..\Submenus\PedModelChanger.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\Databases.h"
#include "..\Submenus\Spooner\EntityManagement.h"
#include "..\Submenus\Spooner\FileManagement.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <string>
#include <vector>
#include <map>
//#include <utility>
#include <array>
#include <pugixml\src\pugixml.hpp>
#include <dirent\include\dirent.h>

namespace sub
{
	// Wraps an int value around [minVal, maxVal] on increment/decrement
	static int cycleInt(int current, bool increment, int minVal, int maxVal)
	{
		if (increment)
			return (current < maxVal) ? current + 1 : minVal;
		else
			return (current > minVal) ? current - 1 : maxVal;
	}

	// Wraps a float value around [minVal, maxVal] with a given step
	static float cycleFloat(float current, bool increment, float minVal, float maxVal, float step)
	{
		if (increment)
			return (current < maxVal) ? current + step : current;
		else
			return (current > minVal) ? current - step : current;
	}
	// Component changer

	Camera g_cam_componentChanger;

	void AddPedComponentOption(const std::string& text, int index)
	{
		bool pressed = false;
		AddOption(text, pressed, nullFunc, SUB::COMPONENTS2, true, true); if (pressed)
		{
			g_Ped4 = index;
		}
	}
	void AddPedPropOption(const std::string& text, int index)
	{
		bool pressed = false;
		AddOption(text, pressed, nullFunc, SUB::COMPONENTSPROPS2, true, true); if (pressed)
		{
			g_Ped4 = index;
		}
	}

	void SyncCollectionToGlobalId(GTAmemory::DrawableCollectionData& data, int globalId)
	{
		for (int c = 0; c < (int)data.collections.size(); c++)
		{
			for (int l = 0; l < (int)data.collections[c].localToGlobal.size(); l++)
			{
				if (data.collections[c].localToGlobal[l] == globalId)
				{
					data.currentCollectionIdx = c;
					data.currentLocalIdx = l;
					return;
				}
			}
		}
	}

	// Returns false when option is not confirmed, true when confirmed.
	static bool PromptConfirm(int& state, const std::string& message)
	{
		if (state == 0) {
			Game::Print::PrintBottomCentre(message);
			state = 1;
			return false;
		}
		state = 0;
		return true;
	}

	void ComponentChanger()
	{
		dict2.clear();
		dict3.clear();

		bool bRandomComponents = 0, frontView = 0, bDefaultComponents = 0;

		GTAped thisPed = g_Ped1;

		if (g_cam_componentChanger.Exists())
		{
			g_cam_componentChanger.AttachTo(thisPed, Vector3(0.0f, 2.6f + thisPed.Dim1().y, 0.5f));
			g_cam_componentChanger.PointAt(thisPed);
		}

		AddTitle("Wardrobe");
		AddLocal("Front View", g_cam_componentChanger.Exists(), frontView, frontView);
		AddOption("Outfits", null, nullFunc, SUB::COMPONENTS_OUTFITS);
		AddOption("Default Outfits (Beta)", null, nullFunc, SUB::COMPONENTS_OUTFITS_DEFAULT);
		AddOption("Decal Overlays", null, PedDecals::OpenSubDecals, -1, true);
		AddOption("Damage Overlays", null, nullFunc, SUB::PEDDAMAGET_CATEGORYLIST);
		AddOption("Head Features", null, nullFunc, SUB::PED_HEADFEATURES_MAIN);
		AddOption("Accessories", null, nullFunc, SUB::COMPONENTSPROPS);

		const std::vector<std::string> components
		{
			"Head ~c~[head]",
			"Beard/Mask ~c~[berd]",
			"Hair ~c~[hair]",
			"Torso ~c~[uppr]",
			"Legs ~c~[lowr]",
			"Hands/Back ~c~[hand]",
			"Shoes ~c~[feet]",
			"Teeth/Scarf/Necklace/Bracelets ~c~[teef]",
			"Accessory/Tops ~c~[accs]",
			"Task/Armour ~c~[task]",
			"Emblem ~c~[decl]",
			"Tops2 (Outer) ~c~[jbib]"
		};

		AddBreak("---Components---");

		for ( int i = 0; i < PV_COMP_MAX; i++)
		{
			if(GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(g_Ped1, i) > 0) AddPedComponentOption(components[i], i);
		}

		AddBreak("---Utilities---");
		AddOption("Random Components", bRandomComponents);
		AddOption("Default Components", bDefaultComponents);

		static int confirmRandom = 0, confirmDefault = 0;
		static UINT16 lastSub = 0;
		if (lastSub != Menu::currentsub)
			confirmRandom = 0, confirmDefault = 0;
		lastSub = Menu::currentsub;

		if (bRandomComponents) {
			if (PromptConfirm(confirmRandom, "~r~Randomize ~w~all components? Press again to confirm.")) {
				thisPed.RequestControlOnce();
				SET_PED_RANDOM_COMPONENT_VARIATION(thisPed.GetHandle(), 0);
			}
			return;
		}

		if (bDefaultComponents) {
			if (PromptConfirm(confirmDefault, "~r~Clear ~w~all components? Press again to confirm.")) {
				thisPed.RequestControlOnce();
				SET_PED_DEFAULT_COMPONENT_VARIATION(thisPed.GetHandle());
			}
			return;
		}

		if (frontView) {
			if (g_cam_componentChanger.Exists())
			{
				g_cam_componentChanger.SetActive(false);
				g_cam_componentChanger.Destroy();
				if (sub::Spooner::SpoonerMode::bEnabled && sub::Spooner::SpoonerMode::spoonerModeCamera.Exists())
					World::SetRenderingCamera(sub::Spooner::SpoonerMode::spoonerModeCamera);
				else
					World::SetRenderingCamera(0);
			}
			else
			{
				Camera gmCam = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
				g_cam_componentChanger = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);

				g_cam_componentChanger.SetFieldOfView(40.0f);
				g_cam_componentChanger.AttachTo(thisPed, Vector3(0.0f, 1.5f + thisPed.Dim1().y, 0.5f));
				g_cam_componentChanger.PointAt(thisPed);

				gmCam.SetPosition(World::GetRenderingCamera().Handle() == 0 ? GameplayCamera::GetPosition() : World::GetRenderingCamera().GetPosition());
				gmCam.SetRotation(World::GetRenderingCamera().Handle() == 0 ? GameplayCamera::GetRotation() : World::GetRenderingCamera().GetRotation());

				gmCam.InterpTo(g_cam_componentChanger, 1000, true, true);
				while (gmCam.IsInterpolating())
					WAIT(0);
				gmCam.Destroy();
				World::SetRenderingCamera(g_cam_componentChanger);
			}
			return;
		}
	}
	void ComponentChanger2()
	{
		int globalDrawableId = GET_PED_DRAWABLE_VARIATION(g_Ped1, g_Ped4);
		int textureId = GET_PED_TEXTURE_VARIATION(g_Ped1, g_Ped4);
		int prevGlobalDrawableId = globalDrawableId;
		int prevTextureId = textureId;

		int maxGlobalDrawableId = GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(g_Ped1, g_Ped4) - 1;
		int maxTextureId = GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, globalDrawableId);

		AddTitle("Set Variation");

		if (maxGlobalDrawableId >= 0)
			AddNumberStepper("Type", globalDrawableId, 0, 1.0, 0, maxGlobalDrawableId, false, true);

		if (globalDrawableId != prevGlobalDrawableId)
		{
			textureId = 0;
			maxTextureId = GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, globalDrawableId);
		}

		if (maxTextureId > 0)
			AddNumberStepper("Texture", textureId, 0, 1.0, 0, maxTextureId - 1, false, true);

		// Collection data section (legacy only)
		bool collectionModified = false;
		if (!g_isEnhanced && maxGlobalDrawableId >= 0)
		{
			static GTAmemory::DrawableCollectionData s_cache[PV_COMP_MAX];
			static Hash s_cachedModel[PV_COMP_MAX] = {};
			Hash modelHash = GET_ENTITY_MODEL(g_Ped1);

			if (s_cachedModel[g_Ped4] != modelHash)
			{
				s_cache[g_Ped4] = GTAmemory::BuildDrawableCollectionData(g_Ped1, g_Ped4);
				s_cachedModel[g_Ped4] = modelHash;
			}

			auto& data = s_cache[g_Ped4];

			if (!data.collections.empty())
			{
				AddBreak("---Collection Data---");

				if (data.currentCollectionIdx < 0)
					data.currentCollectionIdx = 0;
				if (data.currentCollectionIdx >= (int)data.collections.size())
					data.currentCollectionIdx = 0;

				std::vector<std::string> names;
				for (auto& c : data.collections)
					names.push_back(c.name);

				int prevCollectionIdx = data.currentCollectionIdx;
				data.currentCollectionIdx = AddTexterCycler("Collection", data.currentCollectionIdx, names);
				bool collectionChanged = (data.currentCollectionIdx != prevCollectionIdx);

				if (collectionChanged)
					data.currentLocalIdx = 0;

				auto& col = data.collections[data.currentCollectionIdx];
				if (data.currentLocalIdx < 0 || data.currentLocalIdx > col.maxLocalId)
					data.currentLocalIdx = 0;

				int prevLocalDrawableId = data.currentLocalIdx;

				AddNumberStepper("Local ID", data.currentLocalIdx, 0, 1.0, 0, col.maxLocalId, false, true);
				bool localDrawableIdChanged = collectionChanged || (data.currentLocalIdx != prevLocalDrawableId);

				if (localDrawableIdChanged)
				{
					globalDrawableId = col.localToGlobal[data.currentLocalIdx];
					textureId = 0;
					maxTextureId = GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, globalDrawableId);
					collectionModified = true;
				}
				else if (globalDrawableId != prevGlobalDrawableId)
				{
					SyncCollectionToGlobalId(data, globalDrawableId);
					collectionModified = true;
				}
			}
		}

		if (collectionModified || globalDrawableId != prevGlobalDrawableId || textureId != prevTextureId)
		{
			if (g_Ped4 == PV_COMP_ACCS && !GET_PED_CONFIG_FLAG(g_Ped1, ePedConfigFlags::DisableTakeOffScubaGear, true))
			{
				SET_PED_CONFIG_FLAG(g_Ped1, ePedConfigFlags::DisableTakeOffScubaGear, true);
			}

			SET_PED_COMPONENT_VARIATION(g_Ped1, g_Ped4, globalDrawableId, textureId, 0);

			bool goingForward = (globalDrawableId > prevGlobalDrawableId);
			while (!HasPedSpecificDrawable(globalDrawableId))
			{
				if (goingForward)
				{
					if (globalDrawableId < maxGlobalDrawableId) globalDrawableId++;
					else globalDrawableId = 0;
				}
				else
				{
					if (globalDrawableId > 0) globalDrawableId--;
					else globalDrawableId = maxGlobalDrawableId;
				}
				textureId = 0;
				SET_PED_COMPONENT_VARIATION(g_Ped1, g_Ped4, globalDrawableId, textureId, 0);
			}
		}
	}

    bool HasPedSpecificDrawable(int compon_drawable_new)
    {
        bool compon_drawable_correct = false;
        int drawableCurrent = GET_PED_DRAWABLE_VARIATION(g_Ped1, g_Ped4);
        if (compon_drawable_new == drawableCurrent)
        {
            compon_drawable_correct = true;
        }
        return compon_drawable_correct;
    }
    void ComponentChangerProps_()
	{
		GTAped thisPed = g_Ped1;

		if (g_cam_componentChanger.Exists())
		{
			g_cam_componentChanger.AttachTo(thisPed, Bone::Head, Vector3(0.0f, 0.6f, 0.0f));
			g_cam_componentChanger.PointAt(thisPed, Bone::Head);
		}

		bool bRandomProps = false, bDefaultProps = false;
		const std::vector<std::string> propNames
		{
			"Hats ~c~[p_head]",
			"Glasses ~c~[p_eyes]",
			"Ear Pieces ~c~[p_ears]",
			"Unknown 3",
			"Unknown 4",
			"Unknown 5",
			"Watches ~c~[p_lwrist]",
			"Bangles ~c~[p_rwrist]",
			"Unknown 8",
			"Unknown 9"
		};

		AddTitle("Accessories");

		for (int i = 0; i < static_cast<int>(propNames.size()); ++i)
		{
			if (GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(g_Ped1, i) > 0)
				AddPedPropOption(propNames[i], i);
		}

		AddBreak("---Utilities---");
		AddOption("Random Accessories", bRandomProps);
		AddTickol("Default Accessories", true, bDefaultProps, bDefaultProps, TICKOL::CROSS);

		static int confirmRandomProps = 0, confirmDefaultProps = 0;
		static UINT16 lastPropsSub = 0;
		if (lastPropsSub != Menu::currentsub)
			confirmRandomProps = 0, confirmDefaultProps = 0;
		lastPropsSub = Menu::currentsub;

		if (bRandomProps)
		{
			if (PromptConfirm(confirmRandomProps, "~r~Randomize ~w~all accessories? Press again to confirm.")) {
				thisPed.RequestControlOnce();
				SET_PED_RANDOM_PROPS(thisPed.Handle());
			}
			return;
		}

		if (bDefaultProps)
		{
			if (PromptConfirm(confirmDefaultProps, "~r~Clear ~w~all accessories? Press again to confirm.")) {
				thisPed.RequestControlOnce();
				CLEAR_ALL_PED_PROPS(thisPed.Handle(), 0);
			}
			return;
		}
	}

	void ComponentChangerProps2()
	{
		int propTypeCurrent = GET_PED_PROP_INDEX(g_Ped1, g_Ped4, 0);
		int propTextureCurrent = GET_PED_PROP_TEXTURE_INDEX(g_Ped1, g_Ped4);
		int propTypeOld = propTypeCurrent;
		int propTextureOld = propTextureCurrent;

		int maxGlobalPropId = GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(g_Ped1, g_Ped4) - 1;
		int maxTextureId = propTypeCurrent >= 0 ? GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, propTypeCurrent) : 0;

		AddTitle("Set Variation");

		if (maxGlobalPropId >= 0)
		{
			AddNumberStepper("Type", propTypeCurrent, 0, 1.0, -1, maxGlobalPropId, false, true);
		}

		if (propTypeCurrent != propTypeOld)
		{
			propTextureCurrent = 0;
			maxTextureId = propTypeCurrent >= 0 ? GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, propTypeCurrent) : 0;
		}
		if (maxTextureId > 0)
		{
			AddNumberStepper("Texture", propTextureCurrent, 0, 1.0, 0, maxTextureId - 1, false, true);
		}

		// Collection data section (legacy only)
		bool collectionModified = false;
		if (!g_isEnhanced && maxGlobalPropId >= 0)
		{
			static GTAmemory::DrawableCollectionData s_cache[PV_COMP_MAX];
			static Hash s_cachedModel[PV_COMP_MAX] = {};
			Hash modelHash = GET_ENTITY_MODEL(g_Ped1);

			if (s_cachedModel[g_Ped4] != modelHash)
			{
				s_cache[g_Ped4] = GTAmemory::BuildPropCollectionData(g_Ped1, g_Ped4);
				s_cachedModel[g_Ped4] = modelHash;
			}

			auto& data = s_cache[g_Ped4];

			if (!data.collections.empty())
			{
				AddBreak("---Collection Data---");

				if (data.currentCollectionIdx < 0)
					data.currentCollectionIdx = 0;
				if (data.currentCollectionIdx >= (int)data.collections.size())
					data.currentCollectionIdx = 0;

				std::vector<std::string> names;
				for (auto& c : data.collections)
					names.push_back(c.name);

				int prevCollectionIdx = data.currentCollectionIdx;
				data.currentCollectionIdx = AddTexterCycler("Collection", data.currentCollectionIdx, names);
				bool collectionChanged = (data.currentCollectionIdx != prevCollectionIdx);

				if (collectionChanged)
					data.currentLocalIdx = 0;

				auto& col = data.collections[data.currentCollectionIdx];
				if (data.currentLocalIdx < 0 || data.currentLocalIdx > col.maxLocalId)
					data.currentLocalIdx = 0;

				int prevLocalPropId = data.currentLocalIdx;
				AddNumberStepper("Local ID", data.currentLocalIdx, 0, 1.0, 0, col.maxLocalId, false, true);
				bool localPropIdChanged = collectionChanged || (data.currentLocalIdx != prevLocalPropId);

				if (localPropIdChanged)
				{
					propTypeCurrent = col.localToGlobal[data.currentLocalIdx];
					propTextureCurrent = 0;
					maxTextureId = GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(g_Ped1, g_Ped4, propTypeCurrent);
					collectionModified = true;
				}
				else if (propTypeCurrent != propTypeOld && propTypeCurrent >= 0)
				{
					SyncCollectionToGlobalId(data, propTypeCurrent);
					collectionModified = true;
				}
			}
		}

		if (collectionModified || propTypeCurrent != propTypeOld || propTextureCurrent != propTextureOld)
		{
			if (propTypeCurrent == -1)
			{
				CLEAR_PED_PROP(g_Ped1, g_Ped4, 0);
			}
			else
			{
				SET_PED_PROP_INDEX(g_Ped1, g_Ped4, propTypeCurrent, propTextureCurrent, NETWORK_IS_GAME_IN_PROGRESS(), 0);

				bool goingForward = (propTypeCurrent > propTypeOld);
				while (!HasPedSpecificPropType(propTypeCurrent))
				{
					if (goingForward)
					{
						if (propTypeCurrent < maxGlobalPropId) propTypeCurrent++;
						else propTypeCurrent = -1;
					}
					else
					{
						if (propTypeCurrent > -1) propTypeCurrent--;
						else propTypeCurrent = maxGlobalPropId;
					}
					propTextureCurrent = 0;
					SET_PED_PROP_INDEX(g_Ped1, g_Ped4, propTypeCurrent, propTextureCurrent, NETWORK_IS_GAME_IN_PROGRESS(), 0);
				}
			}
		}
	}

	bool HasPedSpecificPropType(int propTypeNew)
	{
		bool propTypeCorrect = false;
		int propTypeCurrent = GET_PED_PROP_INDEX(g_Ped1, g_Ped4, 0);
		if (propTypeNew == propTypeCurrent)
		{
			propTypeCorrect = true;
		}
		return propTypeCorrect;
	}

	// Decals, tattoos & badges

	namespace PedDecals
	{
		std::map<Ped, std::vector<PedDecalValue>> vPedsAndDecals;

		bool g_tattooPreviewMode = false;
		const NamedPedDecal* g_previewTattoo = nullptr;

		void ClearPreviewTattoo()
		{
			if (g_previewTattoo && DOES_ENTITY_EXIST(g_Ped1))
			{
				g_previewTattoo->Remove(g_Ped1);
				g_previewTattoo = nullptr;
			}
		}

		bool NamedPedDecal::IsOnPed(GTAentity ped) const
		{
			auto it = vPedsAndDecals.find(ped.Handle());
			if (it == vPedsAndDecals.end())
			{
				return false;
			}

			for (auto& decal : it->second)
			{
				if (decal.collection == this->collection && decal.value == this->value)
				{
					return true;
				}
			}
			return false;
		}

		void NamedPedDecal::Apply(GTAentity ped) const
		{
			if (ped.Exists())
			{
				ped.RequestControl(200);
				PED::ADD_PED_DECORATION_FROM_HASHES(ped.Handle(), this->collection, this->value);
				vPedsAndDecals[ped.Handle()].push_back({ this->collection, this->value });
			}
			else
			{
				vPedsAndDecals.erase(ped.Handle());
			}
		}

		void NamedPedDecal::Remove(GTAentity ped) const
		{
			if (ped.Exists())
			{
				auto& decals = vPedsAndDecals[ped.Handle()];
				for (auto it = decals.begin(); it != decals.end();)
				{
					if (it->collection == this->collection && it->value == this->value)
					{
						it = decals.erase(it);
					}
					else ++it;
				}

				ped.RequestControl(200);
				CLEAR_PED_DECORATIONS(ped.Handle());
				for (auto& decal : decals)
				{
					ADD_PED_DECORATION_FROM_HASHES(ped.Handle(), decal.collection, decal.value);
				}
			}
			else
			{
				vPedsAndDecals.erase(ped.Handle());
			}
		}

		std::map<Hash, std::map<std::string, std::map<std::string, std::vector<NamedPedDecal>>>> vAllDecals; // PedHash[Type][Zone]
		void PopulateDecalsDict()
		{
			vAllDecals.clear();
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + "PedDecalOverlays.xml").c_str()).status != pugi::status_ok)
			{
				addlog(ige::LogType::LOG_ERROR, "Unable to open PedDecalOverlays.xml");
				return;
			}

			auto nodeRoot = doc.document_element();
			for (auto nodePed = nodeRoot.child("Ped"); nodePed; nodePed = nodePed.next_sibling("Ped"))
			{
				auto& dictType = vAllDecals[nodePed.attribute("hash").as_uint()];
				for (auto nodeType = nodePed.first_child(); nodeType; nodeType = nodeType.next_sibling())
				{
					auto& dictZone = dictType[nodeType.name()];
					for (auto nodeZone = nodeType.first_child(); nodeZone; nodeZone = nodeZone.next_sibling())
					{
						auto& listDecals = dictZone[nodeZone.name()];
						for (auto nodeDecal = nodeZone.child("OVERLAY"); nodeDecal; nodeDecal = nodeDecal.next_sibling("OVERLAY"))
						{
							NamedPedDecal decal;
							decal.collection = GET_HASH_KEY(nodeDecal.attribute("collection").as_string());
							decal.value = GET_HASH_KEY(nodeDecal.attribute("name").as_string());
							decal.caption = nodeDecal.attribute("caption").as_string();
							listDecals.push_back(decal);
						}
					}
				}
			}
		}

		std::string GetDecalCaption(Hash collection, Hash value)
		{
			for (auto& [pedHash, dictType] : vAllDecals)
				for (auto& [type, dictZone] : dictType)
					for (auto& [zone, listDecals] : dictZone)
						for (auto& decal : listDecals)
							if (decal.collection == collection && decal.value == value)
								return decal.caption;
			return {};
		}

		std::pair<std::string, std::map<std::string, std::vector<NamedPedDecal>>>* selectedType;
		std::pair<std::string, std::vector<NamedPedDecal>>* selectedZone;

		void Sub_Decals_Types()
		{
			GTAped ped = g_Ped1;
			const auto& pedModel = ped.Model();

			const auto& vPed = vAllDecals.find(pedModel.hash);
			if (vPed == vAllDecals.end())
			{
				Menu::SetPreviousMenu();
				return;
			}

			AddTitle("Decal Overlays");

			for (auto& type : vPed->second)
			{
				bool bTypePressed = false;
				AddOption(type.first, bTypePressed, nullFunc, SUB::PEDDECALS_ZONES);
				if (bTypePressed)
				{
					selectedType = (std::pair<std::string, std::map<std::string, std::vector<NamedPedDecal>>>*)&type;
				}
			}

			bool bClearAllPressed = false;
			AddTickol("CLEAR ALL", true, bClearAllPressed, bClearAllPressed, TICKOL::CROSS);
			if (bClearAllPressed)
			{
				ped.RequestControl(600);
				CLEAR_PED_DECORATIONS(ped.Handle());
				vPedsAndDecals.erase(ped.Handle());
			}
		}

		void Sub_Decals_Zones()
		{
			AddTitle(selectedType->first);

			for (auto& zone : selectedType->second)
			{
				bool bZonePressed = false;
				AddOption(zone.first, bZonePressed, nullFunc, SUB::PEDDECALS_INZONE);
				if (bZonePressed)
				{
					selectedZone = (std::pair<std::string, std::vector<NamedPedDecal>>*)&zone;
				}
			}
		}
		void Sub_Decals_InZone()
		{
			GTAentity ped = g_Ped1;

			bool bShortcutDecalPreviewPressed = false;

			if (Menu::OnSubBack == nullptr)
			{
				Menu::OnSubBack = []
				{
					PedDecals::ClearPreviewTattoo();
				};
			}

			AddTitle(selectedZone->first);

			for (const auto& decal : selectedZone->second)
			{
				bool isHovered = (*Menu::currentopATM == Menu::printingop + 1);
				bool bDecalPressedApply = false, bDecalPressedRemove = false;
				bool bIsOnPed = decal.IsOnPed(ped);

				AddTickol(decal.caption, bIsOnPed, bDecalPressedApply, bDecalPressedRemove, TICKOL::TATTOOTHING);

				if (g_tattooPreviewMode && isHovered)
				{
					
					if (g_previewTattoo != &decal)
					{
						ClearPreviewTattoo();
						if (!bIsOnPed) {
							decal.Apply(ped);
							g_previewTattoo = &decal;
						}
					}
				}

				if (bDecalPressedApply)
				{
					decal.Apply(ped);
				
				}
				// permanently adding a decal while it's being previewed
				else if (bDecalPressedRemove && g_previewTattoo == &decal)
				{
					ClearPreviewTattoo();
					decal.Apply(ped);
				}
				else if (bDecalPressedRemove)
				{
					decal.Remove(ped);
				}
			}

			Menu::add_IB(VirtualKey::B, g_tattooPreviewMode ? "Preview: ON " : "Preview: OFF ");
			bShortcutDecalPreviewPressed = IsKeyJustUp(VirtualKey::B);
			if (bShortcutDecalPreviewPressed)
			{
				g_tattooPreviewMode = !g_tattooPreviewMode;
				
				if (!g_tattooPreviewMode)
				{
					ClearPreviewTattoo();
				}
			}

		}
		void OpenSubDecals()
		{
			GTAentity ped = g_Ped1;
			bool allowed = vAllDecals.find(ped.Model().hash) != vAllDecals.end();

			if (vAllDecals.find(ped.Model().hash) != vAllDecals.end())
			{
				Menu::SetSub_delayed = SUB::PEDDECALS_TYPES;
			}
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ No decal overlays available for this ped model.");
			}
		}
	}

	// Damage/blood textures

	namespace PedDamageTextures
	{
		auto& selectedPedHandle = g_Ped1;
		int boneToUse = 0;

		std::map<Ped, std::vector<std::string>> vPedsAndDamagePacks;

		void ClearAllBloodDamage(GTAped ped)
		{
			ped.ClearBloodDamage();
		}

		void ClearAllVisibleDamage(GTAped ped)
		{
			ped.ResetVisibleDamage();
			const auto& it = vPedsAndDamagePacks.find(ped.Handle());
			if (it != vPedsAndDamagePacks.end())
			{
				vPedsAndDamagePacks.erase(it);
			}
		}

		void ClearAll241BloodDamage()
		{
			ClearAllBloodDamage(selectedPedHandle);
		}

		void ClearAll241VisibleDamage()
		{
			ClearAllVisibleDamage(selectedPedHandle);
		}

#pragma region blood data
		using PedBloodDecals::vBloodDecals;
#pragma endregion
#pragma region damage decal data
		using PedDamageDecals::vDamageDecals;
#pragma endregion
#pragma region damage packs
		using PedDamagePacks::vDamagePacks;
#pragma endregion

		void Sub_CategoryList()
		{
			AddTitle("Damage Overlays");
			AddOption("Damage Packs", null, nullFunc, SUB::PEDDAMAGET_DAMAGEPACKS);
			AddBreak("---Wash Up---");
			AddTickol("Clear Blood Damage", true, ClearAll241BloodDamage, ClearAll241BloodDamage, TICKOL::CROSS);
			AddTickol("Clear All Visible Damage", true, ClearAll241VisibleDamage, ClearAll241VisibleDamage, TICKOL::CROSS);
		}

		void Sub_BoneSelection()
		{
			AddTitle("Select Bone");

			for (const auto& bn : Bone::vBoneNames)
			{
				bool bPressed = false;
				AddTickol(bn.name, boneToUse == bn.boneid, bPressed, bPressed, TICKOL::SKULL_DM, TICKOL::NONE, true);
				if (bPressed)
				{
					boneToUse = bn.boneid;
				}
			}
		}

		void Sub_Blood()
		{
			GTAped thisPed = selectedPedHandle;
			auto& thisBone = boneToUse;

			AddTitle("Blood Decals");

			for (const auto& bdn : vBloodDecals)
			{
				bool bPressed = false;
				AddOption(bdn, bPressed);
				if (bPressed)
				{
					thisPed.ApplyBlood(bdn, thisBone, Vector3());
				}
			}
		}

		void Sub_DamageDecals()
		{
			GTAped thisPed = selectedPedHandle;
			auto& thisBone = boneToUse;

			AddTitle("Damage Decals");

			for (const auto& ddn : vDamageDecals)
			{
				bool bPressed = false;
				AddOption(ddn, bPressed);
				if (bPressed)
				{
					thisPed.ApplyDamageDecal(ddn, thisBone, Vector3(), 1.0f, 1.0f, 1, true);
				}
			}
		}
		void Sub_DamagePacks()
		{
			GTAped thisPed = selectedPedHandle;

			auto& dmgPacksApplied = vPedsAndDamagePacks[thisPed.Handle()];

			AddTitle("Damage Packs");

			for (const auto& dpn : vDamagePacks)
			{
				bool bPressedApply = false, bPressedRemove = false;
				AddTickol(dpn, std::find(dmgPacksApplied.begin(), dmgPacksApplied.end(), dpn) != dmgPacksApplied.end(), bPressedApply, bPressedRemove, TICKOL::MAKEUPTHING);
				if (bPressedApply)
				{
					thisPed.ApplyDamagePack(dpn, 1.0f, 1.0f);
					dmgPacksApplied.push_back(dpn);
				}

				if (bPressedRemove)
				{
					thisPed.ResetVisibleDamage();
					for (auto dpntrit = dmgPacksApplied.begin(); dpntrit != dmgPacksApplied.end();)
					{
						if (dpntrit->compare(dpn) == 0)
						{
							dpntrit = dmgPacksApplied.erase(dpntrit);
							continue;
						}
						thisPed.ApplyDamagePack(*dpntrit, 1.0f, 1.0f);
						++dpntrit;
					}
				}
			}
		}
	}

	// Head features (freemode m/f)

	namespace PedHeadFeatures_catind
	{

		std::map<Ped, sPedHeadFeatures> vPedHeads;
		std::map<Ped, sPedHeadFeatures>::mapped_type* pedHead;

#pragma region arrays
		const std::vector<std::pair<std::string, std::vector<std::string>>> vCaptions_headOverlays
		{
			{ /*"Skin Rash"*/"FACE_F_SUND",{ "Uneven", "Sandpaper", "Patchy", "Rough", "Leathery", "Textured", "Coarse", "Rugged", "Creased", "Cracked", "Gritty" } },
			{ /*"Beard"*/"FACE_F_BEARD",{ "HAIR_BEARD1", "HAIR_BEARD2", "HAIR_BEARD3", "HAIR_BEARD4", "HAIR_BEARD5", "HAIR_BEARD6", "HAIR_BEARD7", "HAIR_BEARD8", "HAIR_BEARD9", "HAIR_BEARD10", "HAIR_BEARD11", "HAIR_BEARD12", "HAIR_BEARD13", "HAIR_BEARD14", "HAIR_BEARD15", "HAIR_BEARD16", "HAIR_BEARD17", "HAIR_BEARD18", "HAIR_BEARD19" } }, // Beard HAIR_OPTION_0
			{ /*"Eyebrows"*/"FACE_F_EYEBR",{ "CC_EYEBRW_0", "CC_EYEBRW_1", "CC_EYEBRW_2", "CC_EYEBRW_3", "CC_EYEBRW_4", "CC_EYEBRW_5", "CC_EYEBRW_6", "CC_EYEBRW_7", "CC_EYEBRW_8", "CC_EYEBRW_9", "CC_EYEBRW_10", "CC_EYEBRW_11", "CC_EYEBRW_12", "CC_EYEBRW_13", "CC_EYEBRW_14", "CC_EYEBRW_15", "CC_EYEBRW_16", "CC_EYEBRW_17", "CC_EYEBRW_18", "CC_EYEBRW_19", "CC_EYEBRW_20", "CC_EYEBRW_21", "CC_EYEBRW_22", "CC_EYEBRW_23", "CC_EYEBRW_24", "CC_EYEBRW_25", "CC_EYEBRW_26", "CC_EYEBRW_27", "CC_EYEBRW_28", "CC_EYEBRW_29", "CC_EYEBRW_30", "CC_EYEBRW_31", "CC_EYEBRW_32", "CC_EYEBRW_33" } },
			{ /*"Wrinkles"*/"FACE_F_SKINA",{ "Crow's Feet", "First Signs", "Middle Aged", "Worry Lines", "Depression", "Distinguished", "Aged", "Weathered", "Wrinkled", "Sagging", "Tough Life", "Vintage", "Retired", "Junkie", "Geriatric" } },
			{ /*"Makeup & Face Paint"*/"HAIR_OPTION_2",{ "CC_MKUP_0", "CC_MKUP_1", "CC_MKUP_2", "CC_MKUP_3", "CC_MKUP_4", "CC_MKUP_5", "CC_MKUP_6", "CC_MKUP_7", "CC_MKUP_8", "CC_MKUP_9", "CC_MKUP_10", "CC_MKUP_11", "CC_MKUP_12", "CC_MKUP_13", "CC_MKUP_14", "CC_MKUP_15", "CC_MKUP_16", "CC_MKUP_17", "CC_MKUP_18", "CC_MKUP_19", "CC_MKUP_20", "CC_MKUP_21", "CC_MKUP_22", "CC_MKUP_23", "CC_MKUP_24", "CC_MKUP_25", "CC_MKUP_26", "CC_MKUP_27", "CC_MKUP_28", "CC_MKUP_29", "CC_MKUP_30", "CC_MKUP_31", "CC_MKUP_32", "CC_MKUP_33", "CC_MKUP_34", "CC_MKUP_35", "CC_MKUP_36", "CC_MKUP_37", "CC_MKUP_38", "CC_MKUP_39", "CC_MKUP_40", "CC_MKUP_41" } }, // Makeup & face paint HAIR_OPTION_2
			{ /*"Blush"*/"FACE_F_BLUSH",{ "CC_BLUSH_0", "CC_BLUSH_1", "CC_BLUSH_2", "CC_BLUSH_3", "CC_BLUSH_4", "CC_BLUSH_5", "CC_BLUSH_6" } },
			{ /*"Pigment 1 - Complexion"*/"FACE_F_SKC",{ "Rosy Cheeks", "Stubble Rash", "Hot Flush", "Sunburn", "Bruised", "Alchoholic", "Patchy", "Totem", "Blood Vessels", "Damaged", "Pale", "Ghostly" } },
			{ /*"Pigment 2 - Blemishes"*/"FACE_F_SKINB",{ "Measles", "Pimples", "Spots", "Break Out", "Blackheads", "Build Up", "Pustules", "Zits", "Full Acne", "Acne", "Cheek Rash", "Face Rash", "Picker", "Puberty", "Eyesore", "Chin Rash", "Two Face", "T Zone", "Greasy", "Marked", "Acne Scarring", "Full Acne Scarring", "Cold Sores", "Impetigo" } },
			{ /*"Lipstick"*/"FACE_F_LIPST",{ "CC_LIPSTICK_0", "CC_LIPSTICK_1", "CC_LIPSTICK_2", "CC_LIPSTICK_3", "CC_LIPSTICK_4", "CC_LIPSTICK_5", "CC_LIPSTICK_6", "CC_LIPSTICK_7", "CC_LIPSTICK_8", "CC_LIPSTICK_9" } },
			{ /*"Spots"*/"FACE_F_MOLE",{ "Cherub", "All Over", "Irregular", "Dot Dash", "Over the Bridge", "Baby Doll", "Pixie", "Sun Kissed", "Beauty Marks", "Line Up", "Modelesque", "Occasional", "Speckled", "Rain Drops", "Double Dip", "One Sided", "Pairs", "Growth" } },
			{ "Chest Hair",{ "CC_BODY_1_0", "CC_BODY_1_1", "CC_BODY_1_2", "CC_BODY_1_3", "CC_BODY_1_4", "CC_BODY_1_5", "CC_BODY_1_6", "CC_BODY_1_7", "CC_BODY_1_8", "CC_BODY_1_9", "CC_BODY_1_10", "CC_BODY_1_11", "CC_BODY_1_12", "CC_BODY_1_13", "CC_BODY_1_14", "CC_BODY_1_15", "CC_BODY_1_16", "CC_BODY_1_17" } },
			{ "Chest Blemishes",{} },
			{ "Chest Blemishes 2",{} },
		};
		const std::vector<std::string> vCaptions_facialFeatures
		{
			{ "Nose Width" },
			{ "Nose Bottom Height" },
			{ "Nose Tip Length" },
			{ "Nose Bridge Depth" },
			{ "Nose Tip Height" },
			{ "Nose Broken" },
			{ "Brow Height" },
			{ "Brow Depth" },
			{ "Cheekbone Height" },
			{ "Cheekbone Width" },
			{ "Cheek Depth" },
			{ "Eye Size" },
			{ "Lip Thickness" },
			{ "Jaw Width" },
			{ "Jaw Shape" },
			{ "Chin Height" },
			{ "Chin Depth" },
			{ "Chin Width" },
			{ "Chin Indent" },
			{ "Neck Width" }
		};
#pragma endregion

		namespace PedFaceGen
		{
			// [gender][skin]: [0=Male/1=Female][0=White,1=Black,2=Hispanic,3=Asian,4=Arab,5=Pakistani]
			const std::vector<int> parentIdsByGenderSkin[2][6] = {
				{ // Male
					{0, 1, 12, 13, 42, 43, 44, 45},		// White
					{2, 3, 14, 15},						// Black
					{4, 5, 16},                         // Hispanic
					{6, 7, 17, 18},						// Asian
					{10, 11, 20},                       // Arab
					{8, 9, 19}                          // Pakistani
				},
				{ // Female
					{21, 22, 33, 34},					// White
					{23, 24, 35, 36},					// Black
					{25, 26, 37},                       // Hispanic
					{27, 28, 38, 39},					// Asian
					{31, 32, 41},                       // Arab
					{29, 30, 40}                        // Pakistani
				}
			};

			sFaceGenData settings;
		}

		inline int getMaxShapeAndSkinIds()
		{
			return g_unlockMaxIDs ? 255 : 46;
		}
		
		UINT8 GetPedHeadOverlayColourType(const PedHeadOverlay& overlayIndex)
		{
			switch (overlayIndex)
			{
			case PedHeadOverlay::Eyebrows:
			case PedHeadOverlay::Beard:
			case PedHeadOverlay::ChestHair:
			case PedHeadOverlay::Makeup:
				return 1;
			case PedHeadOverlay::Blush:
			case PedHeadOverlay::Lipstick:
				return 2;
			default:
				return 0;
			}
		}

		bool DoesPedModelSupportHeadFeatures(const GTAmodel::Model& pedModel)
		{
			return pedModel.hash == PedHash::FreemodeMale01 || pedModel.hash == PedHash::FreemodeFemale01;
		}

		void UpdatePedHeadBlendData(GTAped& ped, const PedHeadBlendData& blendData, bool bUnused)
		{
			ped.SetHeadBlendData(blendData);
		}

		void Sub_Main()
		{
			GTAped ped = g_Ped1;
			Model pedModel = ped.Model();

			if (g_cam_componentChanger.Exists())
			{
				g_cam_componentChanger.AttachTo(ped, Bone::Head, Vector3(0.0f, 0.645f, 0.0f));
				g_cam_componentChanger.PointAt(ped, Bone::Head);
			}

			if (!ped.Exists() || !DoesPedModelSupportHeadFeatures(pedModel.hash))
			{
				auto pit = vPedHeads.find(ped.Handle());
				if (pit != vPedHeads.end())
				{
					vPedHeads.erase(pit);
				}
				Menu::SetPreviousMenu();
				Game::Print::PrintBottomLeft("~r~Error:~s~ Either the ped died or it isn't an MP freemode model.");
				return;
			}

			pedHead = &vPedHeads[ped.Handle()];

			int maxIds = getMaxShapeAndSkinIds();

			auto headBlend = ped.GetHeadBlendData();
			if (headBlend.shapeFirstID < 0 || headBlend.shapeFirstID > maxIds || headBlend.shapeSecondID < 0 || headBlend.shapeSecondID > maxIds
				|| headBlend.shapeThirdID < 0 || headBlend.shapeThirdID > maxIds || headBlend.skinFirstID < 0 || headBlend.skinFirstID > maxIds
				|| headBlend.skinSecondID < 0 || headBlend.skinSecondID > maxIds || headBlend.skinThirdID < 0 || headBlend.skinThirdID > maxIds
				)
			{
				headBlend.shapeFirstID = 0;
				headBlend.shapeSecondID = 0;
				headBlend.shapeThirdID = 0;
				headBlend.skinFirstID = 1;
				headBlend.skinSecondID = 1;
				headBlend.skinThirdID = 1;
				headBlend.shapeMix = 0.0f;
				headBlend.skinMix = 0.0f;
				headBlend.thirdMix = 0.0f;
				headBlend.isParent = false;
				ped.SetHeadBlendData(headBlend);
			}

			int maxHairColours = GET_NUM_PED_HAIR_TINTS() - 1;
			int maxEyeColours = 32;

			bool hairColourPlus = false, hairColourMinus = false;
			bool hairStreaksPlus = false, hairStreaksMinus = false;
			bool eyeColourPlus = false, eyeColourMinus = false;

			AddTitle("Head Features");

			AddOption("Overlays", null, nullFunc, SUB::PED_HEADFEATURES_HEADOVERLAYS);
			AddOption("Facial Features", null, nullFunc, SUB::PED_HEADFEATURES_FACEFEATURES);
			AddOption("Shape & Skin Tone", null, nullFunc, SUB::PED_HEADFEATURES_SKINTONE);
			AddOption("Face Generator", null, nullFunc, SUB::PED_HEADFEATURES_FACEGENERATOR);

			AddBreak("---Hair---");
			AddNumber("Hair Colour", pedHead->hairColour, 0, null, hairColourPlus, hairColourMinus);
			AddNumber("Hair Streaks Colour", pedHead->hairColourStreaks, 0, null, hairStreaksPlus, hairStreaksMinus);

			AddBreak("---Eyes---");
			AddNumber(Game::GetGXTEntry("FACE_APP_EYE", "Eye Colour"), pedHead->eyeColour, 0, null, eyeColourPlus, eyeColourMinus);

			if (hairColourPlus || hairColourMinus)
			{
				pedHead->hairColour = static_cast<int>(cycleFloat(static_cast<float>(pedHead->hairColour), hairColourPlus, 0.0f, static_cast<float>(maxHairColours), 1.0f));
				SET_PED_HAIR_TINT(ped.Handle(), pedHead->hairColour, pedHead->hairColourStreaks);
			}

			if (hairStreaksPlus || hairStreaksMinus)
			{
				pedHead->hairColourStreaks = static_cast<int>(cycleFloat(static_cast<float>(pedHead->hairColourStreaks), hairStreaksPlus, 0.0f, static_cast<float>(maxHairColours), 1.0f));
				SET_PED_HAIR_TINT(ped.Handle(), pedHead->hairColour, pedHead->hairColourStreaks);
			}

			if (eyeColourPlus || eyeColourMinus)
			{
				pedHead->eyeColour = static_cast<int>(cycleFloat(static_cast<float>(pedHead->eyeColour), eyeColourPlus, 0.0f, static_cast<float>(maxEyeColours), 1.0f));
				SET_HEAD_BLEND_EYE_COLOR(ped.Handle(), SYSTEM::ROUND((float)pedHead->eyeColour));
			}

		}
		void Sub_HeadOverlays()
		{
			auto& overlayIndex = g_Ped4;
			AddTitle("Overlays");

			for (UINT i = 0; i < vCaptions_headOverlays.size(); i++)
			{
				bool bOverlayNamePressed = false;
				AddOption(vCaptions_headOverlays[i].first, bOverlayNamePressed, nullFunc, -1, true, true);
				if (bOverlayNamePressed)
				{
					overlayIndex = i;
					Menu::SetSub_delayed = SUB::PED_HEADFEATURES_HEADOVERLAYS_INITEM;
				}
			}
		}

		void ApplyHeadOverlayTint(GTAped ped, int overlayIndex, int colourType, int primary, int secondary)
		{
			if (primary < 0 || colourType == 0)
			{
				SET_PED_HEAD_OVERLAY_TINT(ped.Handle(), overlayIndex, 0, 0, 0);
			}
			else
			{
				SET_PED_HEAD_OVERLAY_TINT(ped.Handle(), overlayIndex, colourType, primary, secondary);
			}
		}

		void Sub_HeadOverlays_InItem()
		{
			auto& overlayIndex = g_Ped4;
			GTAped ped = g_Ped1;

			auto colourType = GetPedHeadOverlayColourType((PedHeadOverlay)overlayIndex);
			bool bColoursAvailable = (colourType != 0);

			auto& currentOverlayData = pedHead->overlayData[overlayIndex];
			int overlayValue = GET_PED_HEAD_OVERLAY(ped.Handle(), overlayIndex);
			int maxOverlays = GET_PED_HEAD_OVERLAY_NUM(overlayIndex) - 1;
			int maxColours = 64;

			bool overlayPlus = false, overlayMinus = false;
			bool opacityPlus = false, opacityMinus = false;

			AddTitle(vCaptions_headOverlays[overlayIndex].first);

			// VARIATION
			AddTexter("Variation", overlayValue, vCaptions_headOverlays[overlayIndex].second, null, overlayPlus, overlayMinus);
			if (overlayPlus)
			{
				if (overlayValue < maxOverlays)
				{
					overlayValue++;
				}
				else
				{
					overlayValue = overlayValue == 255 ? 0 : 255;
				}

				SET_PED_HEAD_OVERLAY(ped.Handle(), overlayIndex, overlayValue, currentOverlayData.opacity);
				ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
			}

			if (overlayMinus)
			{
				if (overlayValue > 0)
				{
					overlayValue = overlayValue > maxOverlays ? maxOverlays : overlayValue - 1;
				}
				else
				{
					overlayValue = 255;
				}

				SET_PED_HEAD_OVERLAY(ped.Handle(), overlayIndex, overlayValue, currentOverlayData.opacity);
				ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
			}

			// OPACITY
			AddNumber(Game::GetGXTEntry("FACE_OPAC", "Opacity"), currentOverlayData.opacity, 3, null, opacityPlus, opacityMinus);
			if (opacityPlus)
			{
				if (currentOverlayData.opacity < 1.0f)
				{
					currentOverlayData.opacity += 0.01f;
					SET_PED_HEAD_OVERLAY(ped.Handle(), overlayIndex, overlayValue, currentOverlayData.opacity);
				}
			}

			if (opacityMinus)
			{
				if (currentOverlayData.opacity > 0.0f)
				{
					currentOverlayData.opacity -= 0.01f;
					SET_PED_HEAD_OVERLAY(ped.Handle(), overlayIndex, overlayValue, currentOverlayData.opacity);
				}
			}

			if (bColoursAvailable)
			{
				bool colourPlus = false, colourMinus = false;
				bool colourSecondaryPlus = false, colourSecondaryMinus = false;

				// PRIMARY COLOUR
				AddNumber(Game::GetGXTEntry("CMOD_COL0_0", "Primary Colour"), currentOverlayData.colour, 0, null, colourPlus, colourMinus);
				if (colourPlus)
				{
					if (currentOverlayData.colour < maxColours)
					{
						currentOverlayData.colour++;
					}
					else
					{
						currentOverlayData.colour = -1;
					}

					ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
				}

				if (colourMinus)
				{
					if (currentOverlayData.colour > -1)
					{
						currentOverlayData.colour--;
					}
					else
					{
						currentOverlayData.colour = maxColours;
					}

					ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
				}

				// SECONDARY COLOUR
				if (currentOverlayData.colour > -1)
				{
					AddNumber(Game::GetGXTEntry("CMOD_COL0_1", "Secondary Colour"), currentOverlayData.colourSecondary, 0, null, colourSecondaryPlus, colourSecondaryMinus);
				}

				if (colourSecondaryPlus)
				{
					if (currentOverlayData.colourSecondary < maxColours)
					{
						currentOverlayData.colourSecondary++;
					}

					ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
				}

				if (colourSecondaryMinus)
				{
					if (currentOverlayData.colourSecondary > -1)
					{
						currentOverlayData.colourSecondary--;	
					}

					ApplyHeadOverlayTint(ped, overlayIndex, colourType, currentOverlayData.colour, currentOverlayData.colourSecondary);
				}
			}
		}

		void Sub_FaceFeatures()
		{
			GTAped ped = g_Ped1;

			AddTitle("Facial Features");

			for (int i = 0; i < vCaptions_facialFeatures.size(); i++)
			{
				auto& featureValue = pedHead->facialFeatureData[i]; // Use data from memory or native func if possible later

				bool featurePlus = false, featureMinus = false;
				AddNumber(vCaptions_facialFeatures[i], featureValue, 2, null, featurePlus, featureMinus);
				if (featurePlus)
				{
					if (featureValue < 1.0f)
					{
						featureValue += 0.05f;
					}
					SET_PED_MICRO_MORPH(ped.Handle(), i, featureValue);
				}

				if (featureMinus)
				{
					if (featureValue > -1.0f)
					{
						featureValue -= 0.05f;
					}
					SET_PED_MICRO_MORPH(ped.Handle(), i, featureValue);
				}
			}
		}

		void Sub_SkinTone() // HEAD_BLEND
		{
			GTAped ped = g_Ped1;
			//auto& blendData = _pedHead->blendData;
			PedHeadBlendData blendData;
			GET_PED_HEAD_BLEND_DATA(ped.Handle(), (Any*)&blendData);
			std::vector<std::string> idNames;
			float maxMix = 1.0f;
			float minMix = 0.0f;
			float mixStep = 0.01f;

			AddTitle("Shape & Skin Tone");
			AddToggle("Unlock ID Limits", g_unlockMaxIDs);

			int maxIds = getMaxShapeAndSkinIds();

			// add a texter for a blend ID, handle cycling, and update if changed
			auto addBlendIdTexter = [&](const char* label, int& idValue, bool isShape)
			{
				bool plus = false, minus = false;
				AddTexter(label, idValue, idNames, null, plus, minus);
				if (plus || minus)
				{
					idValue = cycleInt(idValue, plus, 0, maxIds);
					UpdatePedHeadBlendData(ped, blendData, isShape);
				}
			};

			// Shape IDs
			addBlendIdTexter("Shape Inherited From Father", blendData.shapeFirstID, true);
			addBlendIdTexter("Shape Inherited From Mother", blendData.shapeSecondID, true);
			addBlendIdTexter("Shape Inherited From Ancestor", blendData.shapeThirdID, true);

			// Skin IDs
			addBlendIdTexter("Tone Inherited From Father", blendData.skinFirstID, true);
			addBlendIdTexter("Tone Inherited From Mother", blendData.skinSecondID, true);
			addBlendIdTexter("Tone Inherited From Ancestor", blendData.skinThirdID, true);

			// Mixes
			AddBreak("---Adjustment---");

			auto addMixSlider = [&](const char* label, float& mixValue)
			{
				bool plus = false, minus = false;
				AddNumber(label, mixValue, 2, null, plus, minus);
				if (plus || minus)
				{
					mixValue = cycleFloat(mixValue, plus, minMix, maxMix, mixStep);
					UpdatePedHeadBlendData(ped, blendData, false);
				}
			};

			addMixSlider("Shape", blendData.shapeMix);
			addMixSlider("Tone", blendData.skinMix);
			addMixSlider("Ancestor (Shape & Tone)", blendData.thirdMix);
		}

		void Sub_FaceGenerator()
		{
			GTAped ped = g_Ped1;

			std::vector<std::string> genderOpts = { "Any", "Male", "Female" };
			std::vector<std::string> skinOpts = { "Any", "White", "Black", "Hispanic", "Asian", "Arab", "Pakistani" };

			bool bRandFace = false, bRandShapes = false, bRandSkins = false, 
				bRandAllFeatures = false, bResetAllFeatures = false, bRandEverything = false;

			// set parent gender filter based on current ped model
			if (PedFaceGen::settings.lastPedModel != ped.Model().hash)
			{
				PedFaceGen::settings.lastPedModel = ped.Model().hash;
				PedFaceGen::settings.parentGenderFilter = (ped.Model().hash == PedHash::FreemodeMale01) ? 1 : (ped.Model().hash == PedHash::FreemodeFemale01) ? 2 : 0;
			}

			auto buildCandidateList = [&]() -> std::vector<int>
			{
				std::vector<int> candidates;
				int skinIdx = PedFaceGen::settings.skinColorFilter - 1; // -1 = Any (all columns)
				int genderStart = (PedFaceGen::settings.parentGenderFilter == 0) ? 0 : PedFaceGen::settings.parentGenderFilter - 1;
				int genderEnd   = (PedFaceGen::settings.parentGenderFilter == 0) ? 1 : PedFaceGen::settings.parentGenderFilter - 1;

				for (int g = genderStart; g <= genderEnd; g++)
				{
					if (skinIdx < 0)
					{
						for (int s = 0; s < 6; s++)
							candidates.insert(candidates.end(), PedFaceGen::parentIdsByGenderSkin[g][s].begin(), PedFaceGen::parentIdsByGenderSkin[g][s].end());
					}
					else
					{
						candidates.insert(candidates.end(), PedFaceGen::parentIdsByGenderSkin[g][skinIdx].begin(), PedFaceGen::parentIdsByGenderSkin[g][skinIdx].end());
					}
				}
				return candidates;
			};

			// randomly picks a parent ID from candidate list
			auto pickRandomId = [](const std::vector<int>& candidates) -> int
			{
				if (candidates.empty()) return 0;
				return candidates[GET_RANDOM_INT_IN_RANGE(0, static_cast<int>(candidates.size()))];
			};

			auto randomizeMix = []() -> float
			{
				return GET_RANDOM_FLOAT_IN_RANGE(0.0f, 1.0f);
			};

			// --- UI ---
			AddTitle("Face Generator");

			// --- Parents ---
			AddTickol("Use Third Parent", PedFaceGen::settings.useThirdParent, PedFaceGen::settings.useThirdParent, PedFaceGen::settings.useThirdParent, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			PedFaceGen::settings.parentGenderFilter = AddTexterCycler("Parent Filter", PedFaceGen::settings.parentGenderFilter, genderOpts);
			PedFaceGen::settings.skinColorFilter = AddTexterCycler("Skin Colour", PedFaceGen::settings.skinColorFilter, skinOpts);
			
			// Show non-rockstar parents only if parent / skin color filter is set to "Any" (we don't know the genders/races of modded-parents)
			bool showNonRockstar = (PedFaceGen::settings.parentGenderFilter == 0) && (PedFaceGen::settings.skinColorFilter == 0);
			if (showNonRockstar)
				AddNumberStepper("Non-Rockstar Parent Max ID", PedFaceGen::settings.nonRockstarMax, 0, 1.0, 46.0, 255.0);

			// --- Randomize ---
			AddBreak("---Randomize---");
			AddOption("Randomize Face", bRandFace);
			AddOption("Randomize Face Shapes", bRandShapes);
			AddOption("Randomize Face Textures", bRandSkins);
			AddOption("Randomize Everything", bRandEverything);

			if (bRandFace || bRandShapes || bRandSkins || bRandEverything)
			{
				auto candidates = buildCandidateList();
				if (showNonRockstar)
					for (int id = 46; id <= PedFaceGen::settings.nonRockstarMax; id++)
						candidates.push_back(id);
				if (candidates.empty()) candidates.push_back(0);

				PedHeadBlendData bd;
				GET_PED_HEAD_BLEND_DATA(ped.Handle(), (Any*)&bd);

				if (bRandFace || bRandEverything)
				{
					bd.shapeFirstID = pickRandomId(candidates);
					bd.shapeSecondID = pickRandomId(candidates);
					bd.skinFirstID = pickRandomId(candidates);
					bd.skinSecondID = pickRandomId(candidates);
					bd.shapeMix = randomizeMix();
					bd.skinMix = randomizeMix();
					if (PedFaceGen::settings.useThirdParent)
					{
						bd.shapeThirdID = pickRandomId(candidates);
						bd.skinThirdID = pickRandomId(candidates);
						bd.thirdMix = randomizeMix();
					}
				}
				else if (bRandShapes)
				{
					bd.shapeFirstID = pickRandomId(candidates);
					bd.shapeSecondID = pickRandomId(candidates);
					bd.shapeMix = randomizeMix();
					if (PedFaceGen::settings.useThirdParent)
					{
						bd.shapeThirdID = pickRandomId(candidates);
						bd.thirdMix = randomizeMix();
					}
				}
				else if (bRandSkins)
				{
					bd.skinFirstID = pickRandomId(candidates);
					bd.skinSecondID = pickRandomId(candidates);
					bd.skinMix = randomizeMix();
					if (PedFaceGen::settings.useThirdParent)
					{
						bd.skinThirdID = pickRandomId(candidates);
					}
				}
				UpdatePedHeadBlendData(ped, bd, false);

				if (bRandEverything)
				{
					for (int i = 0; i < 20; i++)
					{
						float val = GET_RANDOM_FLOAT_IN_RANGE(-1.0f, 1.0f);
						pedHead->facialFeatureData[i] = val;
						SET_PED_MICRO_MORPH(ped.Handle(), i, val);
					}
				}
				return;
			}

			// --- Facial Features ---
			AddBreak("---Facial Features---");
			AddOption("Randomize Facial Features", bRandAllFeatures);
			if (bRandAllFeatures)
			{
				for (int i = 0; i < 20; i++)
				{
					float val = GET_RANDOM_FLOAT_IN_RANGE(-1.0f, 1.0f);
					pedHead->facialFeatureData[i] = val;
					SET_PED_MICRO_MORPH(ped.Handle(), i, val);
				}
				return;
			}

			struct FeatureGroup { const char* label; int start; int end; };
			const FeatureGroup groups[] =
			{
				{ "Nose",           0,  5 },
				{ "Brows",          6,  7 },
				{ "Cheeks & Eyes",  8,  11 },
				{ "Lips",           12, 12 },
				{ "Jaw",            13, 14 },
				{ "Chin",           15, 18 },
				{ "Neck",           19, 19 },
			};

			for (const auto& grp : groups)
			{
				bool bPressed = false;
				AddOption(std::string("Randomize ") + grp.label, bPressed);
				if (bPressed)
				{
					for (int i = grp.start; i <= grp.end; i++)
					{
						float val = GET_RANDOM_FLOAT_IN_RANGE(-1.0f, 1.0f);
						pedHead->facialFeatureData[i] = val;
						SET_PED_MICRO_MORPH(ped.Handle(), i, val);
					}
					return;
				}
			}
			AddBreak("---Reset---");
			AddOption("Reset All Facial Features", bResetAllFeatures);
			if (bResetAllFeatures)
			{
				for (int i = 0; i < 20; i++)
				{
					pedHead->facialFeatureData[i] = 0.0f;
					SET_PED_MICRO_MORPH(ped.Handle(), i, 0.0f);
				}
				return;
			}
			for (const auto& grp : groups)
			{
				bool bPressed = false;
				AddOption(std::string("Reset ") + grp.label, bPressed);
				if (bPressed)
				{
					for (int i = grp.start; i <= grp.end; i++)
					{
						pedHead->facialFeatureData[i] = 0.0f;
						SET_PED_MICRO_MORPH(ped.Handle(), i, 0.0f);
					}
					return;
				}
			}
		}
	}

	// Outfits (saver)

	namespace ComponentChangerOutfit
	{
		UINT8 persistentAttachmentsTexterIndex = 0;
		bool legacyXMLFormat = false;

		bool Create(GTAentity ped, std::string filePath, bool legacyXMLFormat)
		{
			sub::Spooner::SpoonerEntity eped;
			eped.handle = ped;
			eped.type = EntityType::PED;
			eped.dynamic = true;

			bool bClearDecalOverlays = true;
			bool bAddAttachmentsToSpoonerDb = false;
			bool bStartTaskSeqsOnLoad = true;

			pugi::xml_document oldXml;
			if (oldXml.load_file((const char*)filePath.c_str()).status == pugi::status_ok)
			{
				auto nodeOldRoot = oldXml.child("OutfitPedData");
				bClearDecalOverlays = nodeOldRoot.child("ClearDecalOverlays").text().as_bool(bClearDecalOverlays);
				bAddAttachmentsToSpoonerDb = nodeOldRoot.child("SpoonerAttachments").attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase").as_bool(bAddAttachmentsToSpoonerDb);
				bStartTaskSeqsOnLoad = nodeOldRoot.child("SpoonerAttachments").attribute("StartTaskSequencesOnLoad").as_bool(bStartTaskSeqsOnLoad);
			}

			pugi::xml_document doc;

			auto nodeDecleration = doc.append_child(pugi::node_declaration);
			nodeDecleration.append_attribute("version") = "1.0";
			nodeDecleration.append_attribute("encoding") = "ISO-8859-1";

			auto nodeEntity = doc.append_child("OutfitPedData"); // Root
			nodeEntity.append_child("ClearDecalOverlays").text() = bClearDecalOverlays;
			sub::Spooner::FileManagement::AddEntityToXmlNode(eped, nodeEntity, legacyXMLFormat);

			// Attachments
			auto nodeAttachments = nodeEntity.append_child("SpoonerAttachments");
			nodeAttachments.append_attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase") = bAddAttachmentsToSpoonerDb;
			nodeAttachments.append_attribute("StartTaskSequencesOnLoad") = bStartTaskSeqsOnLoad;
			for (auto& e : sub::Spooner::Databases::EntityDb)
			{
				if (e.attachmentArgs.isAttached)
				{
					GTAentity att;
					if (sub::Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(e.handle, att))
					{
						if (att.Handle() == ped.Handle())
						{
							auto nodeAttachment = nodeAttachments.append_child("Attachment");
							sub::Spooner::FileManagement::AddEntityToXmlNode(e, nodeAttachment, legacyXMLFormat);
						}
					}
				}
			}

			return doc.save_file((const char*)filePath.c_str());
		}

		bool Apply(GTAped ep, const std::string& filePath, bool applyModelAndHead, bool applyProps, bool applyComps, bool applyDecals, bool applyDamageTextures, bool applyAttachedEntities)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)filePath.c_str()).status != pugi::status_ok)
			{
				return false;
			}

			bool bNetworkIsGameInProgress = NETWORK::NETWORK_IS_GAME_IN_PROGRESS() != 0;
			auto nodeEntity = doc.child("OutfitPedData"); // Root
			ep.RequestControl(400);

			Model eModel = nodeEntity.child("ModelHash").text().as_uint();
			auto nodePedStuff = nodeEntity.child("PedProperties");

			if (applyModelAndHead)
			{
				if (ep.Handle() == PLAYER_PED_ID())
				{
					bool bWas241 = (g_Ped1 == ep.Handle());
					ChangeModel(eModel);
					ep = PLAYER_PED_ID();
					if (bWas241) g_Ped1 = ep.Handle();
				}

				if (nodePedStuff.child("HasShortHeight").text().as_bool()) SET_PED_CONFIG_FLAG(ep.Handle(), ePedConfigFlags::_Shrink, 1);

				sub::Spooner::FileManagement::LoadPedHeadFeaturesFromXml(ep, nodePedStuff.child("HeadFeatures"), eModel);

				auto nodeFacialMood = nodePedStuff.child("FacialMood");
				if (nodeFacialMood)
				{
					SetPedFacialMood(ep, nodeFacialMood.text().as_string());
				}

				int opacityLevel = nodeEntity.child("OpacityLevel").text().as_int(255);
				if (opacityLevel < 255)
				{
					ep.SetAlpha(opacityLevel);
				}
				ep.SetVisible(nodeEntity.child("IsVisible").text().as_bool());
			}

			if (nodeEntity.child("ClearDecalOverlays").text().as_bool(true)) 
			{
				CLEAR_PED_DECORATIONS(ep.Handle());	
			}
			auto& decalsApplied = sub::PedDecals::vPedsAndDecals[ep.Handle()];
			decalsApplied.clear();
			if (applyDecals)
			{
				sub::Spooner::FileManagement::LoadPedDecalsFromXml(ep, nodePedStuff.child("TattooLogoDecals"));
			}

			if (applyComps)
			{
				sub::Spooner::FileManagement::LoadPedCompsFromXml(ep, nodePedStuff.child("PedComps"));
			}

			if (applyProps)
			{
				CLEAR_ALL_PED_PROPS(ep.Handle(), 0);
				sub::Spooner::FileManagement::LoadPedPropsFromXml(ep, nodePedStuff.child("PedProps"), bNetworkIsGameInProgress != 0);
			}

			sub::PedDamageTextures::ClearAllBloodDamage(ep);
			sub::PedDamageTextures::ClearAllVisibleDamage(ep);
			if (applyDamageTextures)
			{
				auto& dmgPacksApplied = sub::PedDamageTextures::vPedsAndDamagePacks[ep.Handle()];
				dmgPacksApplied.clear();
				sub::Spooner::FileManagement::LoadPedDamagePacksFromXml(ep, nodePedStuff.child("DamagePacks"));
			}

			if (applyAttachedEntities)
			{
				std::unordered_set<Hash> vModelHashes;
				std::vector<sub::Spooner::SpoonerEntityWithInitHandle> vSpawnedAttachments;
				auto nodeAttachments = nodeEntity.child("SpoonerAttachments");
				bool bAddAttachmentsToSpoonerDb = nodeAttachments.attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase").as_bool(false);
				bool bStartTaskSeqsOnLoad = nodeAttachments.attribute("StartTaskSequencesOnLoad").as_bool(true);
				switch (persistentAttachmentsTexterIndex)
				{
					case 0: break; // FileDecides
					case 1: bAddAttachmentsToSpoonerDb = false; break; // ForceOff
					case 2: bAddAttachmentsToSpoonerDb = true; break; // ForceOn
				}
				for (auto nodeAttachment = nodeAttachments.first_child(); nodeAttachment; nodeAttachment = nodeAttachment.next_sibling())
				{
					auto e = sub::Spooner::FileManagement::SpawnEntityFromXmlNode(nodeAttachment, vModelHashes);
					sub::Spooner::EntityManagement::AttachEntity(e.e, ep, e.e.attachmentArgs.boneIndex, e.e.attachmentArgs.offset, e.e.attachmentArgs.rotation);
					vSpawnedAttachments.push_back(e);
					if (bAddAttachmentsToSpoonerDb)
					{
						if (!e.e.taskSequence.empty())
						{
							auto& vTskPtrs = e.e.taskSequence.AllTasks();
							for (auto& u : vSpawnedAttachments)
							{
								for (auto& tskPtr : vTskPtrs)
								{
									tskPtr->LoadTargetingDressing(u.initHandle, u.e.handle.Handle());
								}
							}
							if (bStartTaskSeqsOnLoad) e.e.taskSequence.Start();
						}
						sub::Spooner::Databases::EntityDb.push_back(e.e);
					}
					else
					{
						e.e.handle.NoLongerNeeded();
					}
				}
				for (auto& amh : vModelHashes)
				{
					Model(amh).Unload();
				}
			}
			return true;
		}

	}

	void ComponentChanger_Outfits()
	{
		using ComponentChangerOutfit::persistentAttachmentsTexterIndex;
		std::string& name = dict;
		std::string& searchStr = dict2;
		std::string& dir = dict3;

		bool savePressed = false, createFolderPressed = false;
		std::vector<std::string> fileNames;

		AddTitle("Outfits");

		bool attachmentsPlus = false, attachmentsMinus = false;
		AddTexter("AddAttachmentsToSpoonerDB", persistentAttachmentsTexterIndex, std::vector<std::string>{ "FileDecides", "ForceOff", "ForceOn" }, null, attachmentsPlus, attachmentsMinus);
		if (attachmentsPlus) { if (persistentAttachmentsTexterIndex < 2) persistentAttachmentsTexterIndex++; }
		if (attachmentsMinus) { if (persistentAttachmentsTexterIndex > 0) persistentAttachmentsTexterIndex--; }

		ComponentChangerOutfit::legacyXMLFormat = AddTexterCycler("XML Format", ComponentChangerOutfit::legacyXMLFormat, { "New XML format", "Legacy XML format" }) == 1;

		AddOption("Save Outfit To File", savePressed);

		AddOption("Create New Folder", createFolderPressed);

		if (dir.empty())
		{
			dir = GetPathffA(Pathff::Outfit, false);
		}
		
		DIR* dirPoint = opendir(dir.c_str());
		dirent* entry = readdir(dirPoint);
		while (entry)
		{
			fileNames.push_back(entry->d_name);
			entry = readdir(dirPoint);
		}

		closedir(dirPoint);
		AddBreak("---Found Files---");

		bool bFolderBackPressed = false;
		AddOption("..", bFolderBackPressed);
		if (bFolderBackPressed)
		{
			dir = dir.substr(0, dir.rfind("\\"));
			Menu::currentop = 5;
		}

		if (!fileNames.empty())
		{
			bool bSearchPressed = false;
			AddOption(searchStr.empty() ? "SEARCH" : searchStr, bSearchPressed, nullFunc, -1, true);
			if (bSearchPressed)
			{
				searchStr = Game::InputBox(searchStr, 126U, "SEARCH", boost::to_lower_copy(searchStr));
				boost::to_upper(searchStr);
			}

			for (auto& fileName : fileNames)
			{
				if (fileName.front() == '.' || fileName.front() == ',') continue;
				if (!searchStr.empty()) { if (boost::to_upper_copy(fileName).find(searchStr) == std::string::npos) continue; }

				bool isFolder = PathIsDirectoryA((dir + "\\" + fileName).c_str()) != 0;
				bool isXml = fileName.length() > 4 && fileName.rfind(".xml") == fileName.length() - 4;
				TICKOL icon = TICKOL::NONE;
				if (isFolder) icon = TICKOL::ARROWRIGHT;
				else if (isXml) icon = TICKOL::TICK2;
				bool bFilePressed = false;

				if (isFolder)
				{
					AddTickol(fileName + " >>>", true, bFilePressed, bFilePressed, icon, TICKOL::NONE);
					if (bFilePressed)
					{
						dir = dir + "\\" + fileName;
						Menu::currentop = 5;
					}

					if (Menu::printingop == *Menu::currentopATM && !bFilePressed)
					{
						if (FolderPreviewBmps_catind::bFolderBmpsEnabled)
						{
							FolderPreviewBmps_catind::DrawBmp(dir + "\\" + fileName);
						}
					}
				}

				else if (isXml)
				{
					AddTickol(fileName, true, bFilePressed, bFilePressed, icon, TICKOL::NONE);
					if (bFilePressed)
					{
						name = fileName.substr(0, fileName.rfind('.'));
						Menu::SetSub_delayed = SUB::COMPONENTS_OUTFITS2;
						return;
					}
				}
			}
		}

		if (savePressed)
		{
			std::string inputStr = Game::InputBox("", 28U, "FMMC_KEY_TIP9");
			if (inputStr.length() > 0)
			{
				if (!IsSafePath(inputStr))
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Invalid characters in name.");
				}
				else
				{
					ComponentChangerOutfit::Create(g_Ped1, dir + "\\" + inputStr + ".xml", ComponentChangerOutfit::legacyXMLFormat);
					Game::Print::PrintBottomLeft("File ~b~created~s~.");
				}
			}
			else Game::Print::PrintErrorInvalidInput(inputStr);
			return;
		}

		if (createFolderPressed)
		{
			std::string inputStr = Game::InputBox("", 28U, "Enter folder name:");
			if (inputStr.length() > 0)
			{
				if (!IsSafePath(inputStr))
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Invalid characters in name.");
				}
				else if (CreateDirectoryA((dir + "\\" + inputStr).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
				{
					dir = dir + "\\" + inputStr;
					Menu::currentop = 5;
					Game::Print::PrintBottomLeft("Folder ~b~created~s~.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Failed~s~ to create folder.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to create folder " + inputStr + " failed");
				}
			}
			else Game::Print::PrintErrorInvalidInput(inputStr);
			return;
		}
	}
	void ComponentChanger_Outfits2()
	{
		std::string& name = dict;
		std::string& dir = dict3;
		std::string filePath = dir + "\\" + name + ".xml";

		bool outfits2_apply = false, outfits2_applyAllFeatures = false, outfits2_applyModel = false, outfits2_overwrite = false, outfits2_applySetDefault = false, outfits2_rename = false, outfits2_delete = false;

		AddTitle(name);
		AddOption("Apply", outfits2_apply);
		AddOption("Apply Clothing & Attachments", outfits2_applyAllFeatures);
		AddOption((std::string)"Apply " + (g_Ped1 == PLAYER_PED_ID() ? "Ped Model" : "Head Features"), outfits2_applyModel);
		AddOption("Apply and Set as Default", outfits2_applySetDefault);
		ComponentChangerOutfit::legacyXMLFormat = AddTexterCycler("XML Format", ComponentChangerOutfit::legacyXMLFormat, { "New XML format", "Legacy XML format" }) == 1;
		AddOption("Rename File", outfits2_rename);
		AddOption("Overwrite File", outfits2_overwrite);
		AddOption("Delete File", outfits2_delete);

		if (outfits2_apply)
		{
			outfits2_applyModel = true;
			outfits2_applyAllFeatures = true;
		}

		if (outfits2_applyModel)
		{
			bool s1isme = g_Ped1 == PLAYER_PED_ID();
			ComponentChangerOutfit::Apply(g_Ped1, filePath, true, false, false, false, false, false);
			if (s1isme) g_Ped1 = PLAYER_PED_ID();
		}

		if (outfits2_applyAllFeatures)
		{
			ComponentChangerOutfit::Apply(g_Ped1, filePath, false, true, true, true, true, true);
		}

		if (outfits2_applySetDefault)
		{
			ComponentChangerOutfit::Apply(PLAYER_PED_ID(), filePath, true, false, false, false, false, false);
			ComponentChangerOutfit::Apply(PLAYER_PED_ID(), filePath, false, true, true, true, true, true);
			if (ComponentChangerOutfit::Create(PLAYER_PED_ID(), "menyooStuff/defaultPed.xml", ComponentChangerOutfit::legacyXMLFormat))
			{
				Game::Print::PrintBottomLeft("Set as ~b~Default~s~, Outfit will be auto loaded on next game launch.");
			}
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to create file.");
				addlog(ige::LogType::LOG_ERROR, "Attempt to create file menyooStuff/defaultPed.xml failed");
			}
		}

		if (outfits2_overwrite)
		{
			if (ComponentChangerOutfit::Create(g_Ped1, filePath, ComponentChangerOutfit::legacyXMLFormat))
				Game::Print::PrintBottomLeft("File ~b~overwritten~s~.");
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to overwrite file.");
				addlog(ige::LogType::LOG_ERROR, "Attempt to overwrite file " + filePath + " failed");
			}
		}

		if (outfits2_rename)
		{
			std::string newName = Game::InputBox("", 28U, "FMMC_KEY_TIP9", name);
			if (newName.length() > 0)
			{
				if (!IsSafePath(newName))
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Invalid characters in name.");
				}
				else if (rename(filePath.c_str(), (dir + "\\" + newName + ".xml").c_str()) == 0)
				{
					name = newName;
					Game::Print::PrintBottomLeft("File ~b~renamed~s~.");
				}
				else Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to rename file.");
				addlog(ige::LogType::LOG_ERROR, "Attempt to rename file " + name + " to " + newName + "failed");
			}
			else Game::Print::PrintErrorInvalidInput(newName);
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::RenameOutfitFile, std::string(), 28U, "FMMC_KEY_TIP9", _name);
			//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_name);
			//OnscreenKeyboard::State::arg2._ptr = reinterpret_cast<void*>(&dir);
		}

		if (outfits2_delete)
		{
			if (remove(filePath.c_str()) == 0)
			{
				Game::Print::PrintBottomLeft("File ~b~deleted~s~.");
			}
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to delete file.");
				addlog(ige::LogType::LOG_ERROR, "Attempt to delete file " + filePath + " failed");
			}
			Menu::SetPreviousMenu();
			Menu::Up();
			return;
		}

		pugi::xml_document doc;
		if (doc.load_file((const char*)filePath.c_str()).status == pugi::status_ok)
		{
			AddBreak("---Attributes---");
			auto nodeEntity = doc.child("OutfitPedData"); // Root
			auto nodePedStuff = nodeEntity.child("PedProperties");

			auto nodeClearDecalOverlays = nodeEntity.child("ClearDecalOverlays");
			bool bToggleClearDecalOverlaysPressed = false;
			AddTickol("Clear Previous Decals", nodeClearDecalOverlays.text().as_bool(true), bToggleClearDecalOverlaysPressed, bToggleClearDecalOverlaysPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			if (bToggleClearDecalOverlaysPressed)
			{
				if (!nodeClearDecalOverlays) 
				{
					nodeClearDecalOverlays = nodeEntity.append_child("ClearDecalOverlays");
				}
				nodeClearDecalOverlays.text() = !nodeClearDecalOverlays.text().as_bool(true);
				doc.save_file((const char*)filePath.c_str());
			}

			auto nodeShortHeighted = nodePedStuff.child("HasShortHeight");
			if (nodeShortHeighted)
			{
				bool bToggleShortHeightedPressed = false;
				AddTickol("Short Height", nodeShortHeighted.text().as_bool(), bToggleShortHeightedPressed, bToggleShortHeightedPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
				if (bToggleShortHeightedPressed)
				{
					nodeShortHeighted.text() = !nodeShortHeighted.text().as_bool();
					doc.save_file((const char*)filePath.c_str());
				}
			}

			auto nodeAddAttachmentsToSpoonerDb = nodeEntity.child("SpoonerAttachments").attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase");
			bool bAddAttachemntsToSpoonerDb = nodeAddAttachmentsToSpoonerDb.as_bool();
			if (nodeAddAttachmentsToSpoonerDb)
			{
				bool bToggleAddAttachmentsToSpoonerDbPressed = false;
				AddTickol("Persistent Attachments (AddToSpoonerDb)", bAddAttachemntsToSpoonerDb, bToggleAddAttachmentsToSpoonerDbPressed, bToggleAddAttachmentsToSpoonerDbPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
				if (bToggleAddAttachmentsToSpoonerDbPressed)
				{
					nodeAddAttachmentsToSpoonerDb = !nodeAddAttachmentsToSpoonerDb.as_bool();
					bAddAttachemntsToSpoonerDb = !bAddAttachemntsToSpoonerDb;
					doc.save_file((const char*)filePath.c_str());
				}
			}

			if (bAddAttachemntsToSpoonerDb)
			{
				auto nodeStartTaskSeqOnLoad = nodeEntity.child("SpoonerAttachments").attribute("StartTaskSequencesOnLoad");
				if (nodeStartTaskSeqOnLoad)
				{
					bool bToggleStartTaskSeqOnLoadPressed = false;
					AddTickol("Start Task Sequences Immediately", nodeStartTaskSeqOnLoad.as_bool(), bToggleStartTaskSeqOnLoadPressed, bToggleStartTaskSeqOnLoadPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
					if (bToggleStartTaskSeqOnLoadPressed)
					{
						nodeStartTaskSeqOnLoad = !nodeStartTaskSeqOnLoad.as_bool();
						doc.save_file((const char*)filePath.c_str());
					}
				}
			}
		}
	}

}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"

namespace sub
{
	struct DefaultOutfitCacheEntry {
		int index;
		Hash hash;
		int price;
		int totalProps;
		int totalComponents;
		std::string gxt;
		std::string name;
	};

	static std::vector<DefaultOutfitCacheEntry> g_defaultOutfitCache;
	static int g_defaultOutfitCacheCharType = -1;
	static std::string g_defaultOutfitSearch = "";

	void ComponentChanger_DefaultOutfits()
	{
		GTAped ped = g_Ped1;
		Hash modelHash = ped.Model().hash;
		int charType = -1;
		// Only Freemode Characters seem to work.  It should work for Story Mode Characters but nothing comes up.  They don't have many anyway.
		if (modelHash == GET_HASH_KEY("Player_Zero")) { charType = 0; }
		else if (modelHash == GET_HASH_KEY("Player_One")) { charType = 1; }
		else if (modelHash == GET_HASH_KEY("Player_Two")) { charType = 2; }
		else if (modelHash == GET_HASH_KEY("MP_M_Freemode_01")) { charType = 3; }
		else if (modelHash == GET_HASH_KEY("MP_F_Freemode_01")) { charType = 4; }

		AddTitle("Default Outfits");

		if (charType != g_defaultOutfitCacheCharType)
		{
			g_defaultOutfitCache.clear();
			g_defaultOutfitCacheCharType = charType;
			g_defaultOutfitSearch = "";
		}

		if (g_defaultOutfitCache.empty() && charType >= 0)
		{
			int numOutfits = FILES::SETUP_SHOP_PED_OUTFIT_QUERY(charType, false);
			if (numOutfits > 0)
			{
				for (int i = 0; i < numOutfits; ++i)
				{
					int blob[32]; // 128 bytes
					memset(blob, 0, sizeof(blob));
					FILES::GET_SHOP_PED_QUERY_OUTFIT(i, (Any*)blob);
					
					DefaultOutfitCacheEntry entry;
					entry.index = i;
					entry.hash = (Hash)blob[2]; // byte 8
					entry.price = blob[4]; // byte 16
					entry.totalProps = blob[6]; // byte 24
					entry.totalComponents = blob[8]; // byte 32
					
					const char* gxt = (const char*)blob + 56; // 56 bytes offset
					if (gxt) entry.gxt = gxt;
						
					std::string displayName = entry.gxt;
					if (!entry.gxt.empty())
					{
						const char* resolved = DOES_TEXT_LABEL_EXIST((char*)entry.gxt.c_str()) ? GET_FILENAME_FOR_AUDIO_CONVERSATION((char*)entry.gxt.c_str()) : nullptr;
						if (resolved && strlen(resolved) > 0 && strcmp(resolved, "NULL") != 0)
						{
							displayName = resolved;
						}
					}
					entry.name = displayName;
					g_defaultOutfitCache.push_back(entry);
				}
			}
		}

		if (charType < 0)
		{
			bool dummy = false;
			AddOption("No outfits found - unsupported ped model", dummy);
			return;
		}

		bool bSearchPressed = false;
		AddOption(g_defaultOutfitSearch.empty() ? "SEARCH" : g_defaultOutfitSearch, bSearchPressed, nullFunc, -1, true);
		if (bSearchPressed)
		{
			g_defaultOutfitSearch = Game::InputBox(g_defaultOutfitSearch, 126U, "SEARCH", boost::to_lower_copy(g_defaultOutfitSearch));
			boost::to_lower(g_defaultOutfitSearch);
		}

		bool refreshPressed = false;
		//AddOption("Refresh Cache", refreshPressed); //I don't think this is really needed, cache refreshes on launch anyway
		if (refreshPressed)
		{
			g_defaultOutfitCache.clear();
			g_defaultOutfitCacheCharType = -1;
		}

		for (const auto& outfit : g_defaultOutfitCache)
		{
			if (!g_defaultOutfitSearch.empty())
			{
				std::string lowerName = boost::to_lower_copy(outfit.name);
				std::string lowerGxt = boost::to_lower_copy(outfit.gxt);
				if (lowerName.find(g_defaultOutfitSearch) == std::string::npos &&
					lowerGxt.find(g_defaultOutfitSearch) == std::string::npos)
				{
					continue;
				}
			}

			bool outfitPressed = false;
			AddOption(outfit.name, outfitPressed);
			if (outfitPressed)
			{
				ped.RequestControl(200);
				
				// Reset to default
				SET_PED_DEFAULT_COMPONENT_VARIATION(ped.Handle());
				
				for (int i = 0; i <= 10; ++i)
				{
					CLEAR_PED_PROP(ped.Handle(), i, 0);
				}
				
				int clearComps[] = { 1, 5, 7, 8, 9, 10 };
				for (int c : clearComps)
				{
					SET_PED_COMPONENT_VARIATION(ped.Handle(), c, 0, 0, 0);
				}
				
				addlog(ige::LogType::LOG_DEBUG, "Applied Default Outfit: " + outfit.name + " | Hash: " + std::to_string(outfit.hash) + " | Comps: " + std::to_string(outfit.totalComponents) + " | Props: " + std::to_string(outfit.totalProps));
				
				// Apply components
				for (int ci = 0; ci < outfit.totalComponents; ++ci)
				{
					int vb[8]; // 32 bytes
					memset(vb, 0, sizeof(vb));
					FILES::GET_SHOP_PED_OUTFIT_COMPONENT_VARIANT(outfit.hash, ci, (Any*)vb);
					Hash compHash = (Hash)vb[0];
					int compType = vb[4]; // byte 16
					
					int cb[20]; // 80 bytes
					memset(cb, 0, sizeof(cb));
					FILES::GET_SHOP_PED_COMPONENT(compHash, (Any*)cb);
					int drawable = cb[6]; // byte 24
					int texture = cb[8];  // byte 32
					SET_PED_COMPONENT_VARIATION(ped.Handle(), compType, drawable, texture, 0);
				}
				
				// Apply props
				for (int pi = 0; pi < outfit.totalProps; ++pi)
				{
					int vb[8];
					memset(vb, 0, sizeof(vb));
					FILES::GET_SHOP_PED_OUTFIT_PROP_VARIANT(outfit.hash, pi, (Any*)vb);
					Hash propHash = (Hash)vb[0];
					int anchorPoint = vb[4]; // byte 16
					
					int pb[20];
					memset(pb, 0, sizeof(pb));
					FILES::GET_SHOP_PED_PROP(propHash, (Any*)pb);
					int drawable = pb[6]; // byte 24
					int texture = pb[8];  // byte 32
					SET_PED_PROP_INDEX(ped.Handle(), anchorPoint, drawable, texture, true, 0);
				}
			}
		}
	}
}
REGISTER_SUBMENU(COMPONENTS, sub::ComponentChanger)
REGISTER_SUBMENU(COMPONENTS2, sub::ComponentChanger2)
REGISTER_SUBMENU(COMPONENTSPROPS, sub::ComponentChangerProps_)
REGISTER_SUBMENU(COMPONENTSPROPS2, sub::ComponentChangerProps2)
REGISTER_SUBMENU(COMPONENTS_OUTFITS, sub::ComponentChanger_Outfits)
REGISTER_SUBMENU(COMPONENTS_OUTFITS2, sub::ComponentChanger_Outfits2)
REGISTER_SUBMENU(COMPONENTS_OUTFITS_DEFAULT, sub::ComponentChanger_DefaultOutfits)
REGISTER_SUBMENU(PEDDECALS_TYPES, sub::PedDecals::Sub_Decals_Types)
REGISTER_SUBMENU(PEDDECALS_ZONES, sub::PedDecals::Sub_Decals_Zones)
REGISTER_SUBMENU(PEDDECALS_INZONE, sub::PedDecals::Sub_Decals_InZone)
REGISTER_SUBMENU(PEDDAMAGET_CATEGORYLIST, sub::PedDamageTextures::Sub_CategoryList)
REGISTER_SUBMENU(PEDDAMAGET_BONESELECTION, sub::PedDamageTextures::Sub_BoneSelection)
REGISTER_SUBMENU(PEDDAMAGET_BLOOD, sub::PedDamageTextures::Sub_Blood)
REGISTER_SUBMENU(PEDDAMAGET_DAMAGEDECALS, sub::PedDamageTextures::Sub_DamageDecals)
REGISTER_SUBMENU(PEDDAMAGET_DAMAGEPACKS, sub::PedDamageTextures::Sub_DamagePacks)
REGISTER_SUBMENU(PED_HEADFEATURES_MAIN, sub::PedHeadFeatures_catind::Sub_Main)
REGISTER_SUBMENU(PED_HEADFEATURES_HEADOVERLAYS, sub::PedHeadFeatures_catind::Sub_HeadOverlays)
REGISTER_SUBMENU(PED_HEADFEATURES_HEADOVERLAYS_INITEM, sub::PedHeadFeatures_catind::Sub_HeadOverlays_InItem)
REGISTER_SUBMENU(PED_HEADFEATURES_FACEFEATURES, sub::PedHeadFeatures_catind::Sub_FaceFeatures)
REGISTER_SUBMENU(PED_HEADFEATURES_SKINTONE, sub::PedHeadFeatures_catind::Sub_SkinTone)
REGISTER_SUBMENU(PED_HEADFEATURES_FACEGENERATOR, sub::PedHeadFeatures_catind::Sub_FaceGenerator)
