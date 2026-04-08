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
#include <vector>
#include <map>
#include <array>

typedef unsigned char UINT8;
typedef unsigned int UINT;
typedef unsigned long DWORD, Hash;
typedef int Ped;

class Camera;
class PedHeadBlendData;
class GTAentity;
class GTAped;
namespace GTAmodel 
{
	class Model;
}

enum class PedHeadOverlay : int;

namespace sub
{
	// Component changer

	extern Camera g_cam_componentChanger;
	void AddPedComponentOption(const std::string& text, int index);
	void AddPedPropOption(const std::string& text, int index);

	void ComponentChanger();
	void ComponentChanger2();
	bool HasPedSpecificDrawable(int drawableNew);
	void ComponentChangerProps();
	void ComponentChangerProps2();
	bool HasPedSpecificPropType(int propTypeNew);

	// Decals - tattoos & badges

	namespace PedDecals
	{
		struct PedDecalValue
		{
			Hash collection, value;
			PedDecalValue(Hash newCollection, Hash newValue) : collection(newCollection), value(newValue)
			{
			}
		};

		extern std::map<Ped, std::vector<PedDecalValue>> vPedsAndDecals;

		struct NamedPedDecal
		{
			Hash collection, value;
			std::string caption;
			bool IsOnPed(GTAentity ped) const;
			void Apply(GTAentity ped) const;
			void Remove(GTAentity ped) const;

		};

		extern std::map<Hash, std::map<std::string, std::map<std::string, std::vector<NamedPedDecal>>>> vAllDecals; // PedHash[Type][Zone]
		void PopulateDecalsDict();

		extern std::pair<std::string, std::map<std::string, std::vector<NamedPedDecal>>>* selectedType;
		extern std::pair<std::string, std::vector<NamedPedDecal>>* selectedZone;

		extern bool g_tattooPreviewMode;
		extern const NamedPedDecal* g_previewTattoo;

		void Sub_Decals_Types();
		void Sub_Decals_Zones();
		void Sub_Decals_InZone();
		void OpenSubDecals();

	}

	// Damage/blood textures

	namespace PedDamageTextures
	{
		extern std::map<Ped, std::vector<std::string>> vPedsAndDamagePacks;

		void ClearAllBloodDamage(GTAped ped);
		void ClearAllVisibleDamage(GTAped ped);
		void ClearAll241BloodDamage();
		void ClearAll241VisibleDamage();

		void Sub_CategoryList();
		void Sub_BoneSelection();
		void Sub_Blood();
		void Sub_DamageDecals();
		void Sub_DamagePacks();
	}

	// Head features (freemode m/f)

	namespace PedHeadFeatures_catind
	{
		struct sPedHeadOverlayData
		{
			float opacity;
			int colour;
			int colourSecondary;

			sPedHeadOverlayData()
			{
				this->opacity = 1.0f;
				this->colour = -1;
				this->colourSecondary = -1;
			}
		};
		struct sPedHeadFeatures
		{
			std::array<sPedHeadOverlayData, 13> overlayData;
			std::array<float, 20>  facialFeatureData;
			int hairColour;
			int hairColourStreaks;
			int eyeColour;

			sPedHeadFeatures()
			{
				this->facialFeatureData.fill(0);
				this->hairColour = 0;
				this->hairColourStreaks = 0;
				this->eyeColour = 1;
			}
		};

		extern std::map<Ped, sPedHeadFeatures> vPedHeads;
		extern std::map<Ped, sPedHeadFeatures>::mapped_type* pedHead;

		extern const std::vector<std::pair<std::string, std::vector<std::string>>> vCaptions_headOverlays;
		extern const std::vector<std::string> vCaptions_facialFeatures;

		//extern UINT8 max_shapeAndSkinIDs;
		UINT8 GetPedHeadOverlayColourType(const PedHeadOverlay& overlayIndex);
		bool DoesPedModelSupportHeadFeatures(const GTAmodel::Model& pedModel);
		void UpdatePedHeadBlendData(GTAped& ped, const PedHeadBlendData& blendData, bool bUnused);

		void Sub_Main();
		void Sub_HeadOverlays();
		void ApplyHeadOverlayTint(GTAped ped, int overlayIndex, int colourType, int primary, int secondary);
		void Sub_HeadOverlays_InItem();
		void Sub_FaceFeatures();
		void Sub_SkinTone();

	}

	// Outfits (saver)

	namespace ComponentChangerOutfit
	{
		extern UINT8 persistentAttachmentsTexterIndex;
		bool Create(GTAentity ped, std::string filePath);
		bool Apply(GTAped ep, const std::string& filePath, bool applyModelAndHead, bool applyProps, bool applyComps, bool applyDecals, bool applyDamageTextures, bool applyAttachedEntities);

	}

	void ComponentChanger_Outfits();
	void ComponentChanger_Outfits2();
}


