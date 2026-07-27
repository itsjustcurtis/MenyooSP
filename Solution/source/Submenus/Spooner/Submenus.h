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

#include <tuple>
#include <string>
#include <vector>

#include "..\..\Util\GTAmath.h"
#include "SpoonerEntity.h"

typedef unsigned char UINT8, BYTE;
typedef unsigned int UINT;
typedef unsigned long DWORD, Hash;

class GTAentity;

namespace sub
{
	namespace Spooner::Submenus
	{
		extern std::string& _searchStr;
		extern std::tuple<GTAentity, Vector3*, Vector3*> SpoonerVector3ManualEditingPtrs;
		extern UINT8 _copyEntTexterValue;
		struct EntityScaleState {
			int handle = 0;
			Vector3 scale{ 1.0f, 1.0f, 1.0f };
		};
		extern EntityScaleState _vehScale, _pedScale, _objScale;

		namespace MultiSelect
		{
			extern std::vector<SpoonerEntity> g_selectedEntities;
			void Add(const SpoonerEntity& entity);
			void Remove(int index);
			void Remove(GTAentity handle);
			bool IsSelected(GTAentity handle);
			void Clear();
			void DestroyPivot();
			void CreatePivot();
		}

		void HandleKeyboardPlacementInput(Vector3& position, Vector3& rotation);

		void SetEnt241();
		void SetEnt12();

		void Sub_SpoonerMain();
		void Sub_Settings();
		void Sub_SaveFiles();
		void Sub_AutoSave();
		void Sub_SaveFiles_Load();
		void Sub_SaveFiles_Load_LegacySP00N();
		void Sub_ManageEntities();
		void Sub_ManageEntities_Removal();
		//void Sub_ManageEntities_Removal_FromDb();
		void Sub_SelectedEntityOps();
		void Sub_AttachmentOps();
		void Sub_AttachmentOps_AttachTo();
		void Sub_AttachmentOps_SelectBone();
		void Sub_ManualEditing();
		void Sub_Snapping();
		void Sub_Vector3_ManualEditing();
		void Sub_MultiSelect();

		void Sub_PedOps();
		void Sub_PedOps_Weapon();
		void Sub_PedOps_Weapon_InCategory();

		void Sub_ManageMarkers();
		void Sub_ManageMarkers_Removal();
		void Sub_ManageMarkers_InMarker();
		void Sub_ManageMarkers_InMarker_Dest2Marker();
		void Sub_ManageMarkers_InMarker_Attach();

		void Sub_ManageLights();
		void Sub_ManageLights_Removal();
		void Sub_ManageLights_InLight();
		void Sub_ManageLights_Presets();
		void Sub_ManageLights_Colour();

		void Sub_SpawnCategories();
		void Sub_SpawnProp();
		void Sub_SpawnProp_Favourites();
		void Sub_SpawnProp_Favourites_Search();
		void Sub_SpawnPed();
		void Sub_SpawnVehicle();
	}


	void EntityAlphaLevelSub_();

	void PedExplosionSub();

	void AttachFunnyObjectSub();

}



