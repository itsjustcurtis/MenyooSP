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

#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\Model.h"
#include "SpoonerCamera.h"

#include <utility>
#include <set>

typedef unsigned char UINT8, BYTE;
typedef unsigned short UINT16;

namespace sub::Spooner
{
	class SpoonerEntity;

	namespace SpoonerMode
	{
		extern BYTE bindsKeyboard;
		extern std::pair<UINT16, UINT16> bindsGamepad;

		extern bool bEnabled;

		enum class eEditMode : UINT8 { Disabled, Keyboard, Gizmo };
		enum class eTransformMode : UINT8 { Position, Rotation, Scale };

		struct EditingState {
			eEditMode mode = eEditMode::Disabled;
			eTransformMode transformMode = eTransformMode::Position;
			bool localSpace = false;
			bool cameraLocked = false;
			float precisionPos = 0.1f;
			float precisionRot = 1.0f;
			float precisionScale = 0.1f;

			void SetMode(eEditMode newMode)
			{
				if (mode == newMode) return;
				mode = newMode;
				cameraLocked = false;
			}
			bool BlocksCameraTranslation() const
			{
				return mode == eEditMode::Keyboard || (mode == eEditMode::Gizmo && cameraLocked);
			}
			bool BlocksCameraRotation() const
			{
				return mode == eEditMode::Gizmo && cameraLocked;
			}
			bool UsesGizmoCursor() const
			{
				return mode == eEditMode::Gizmo && cameraLocked;
			}
		};
		extern EditingState editingState;

		void ProcessKeyboardManipulation(Vector3& position, Vector3& rotation);
		void DrawEditingHUD();
		void UpdateEntityEditingState(Vector3& position, Vector3& rotation);
		struct SpoonerStats {
			int totalNumEntities;
			int totalNumProps;
			int totalNumPeds;
			int totalNumVehicles;
		};
		SpoonerStats GetSpoonerStats();

		struct ModelPreviewInfoStructure
		{
			EntityType entityType;
			Model previousModel,
				model;
			GTAentity entity;
			std::set<GTAentity> previousEntities;
		};
		extern ModelPreviewInfoStructure modelPreviewInfo;
		extern float previewYawOffset;
		void SpawnModelPreview();

		void ResetSelectedEntity();
		void OpenMenu(int submenu, int selectedOption = 1);
		bool GetEntityPtr(GTAentity& inEntity, SpoonerEntity*& outEntity);
		SpoonerEntity GetEntityPtrValue(GTAentity& entity);
		void SetAsSelectedEntity(GTAentity& entity);
		Vector3 SnapPos(Vector3 pos);
		Vector3 SnapRot(Vector3 rot);
		float GetGroundOffset(const GTAmodel::ModelDimensions& dimensions, const Vector3& rotation);
		void DrawSnappingGrid();

		void Tick();

		void TurnOn();
		void TurnOff();
		void Toggle();
	}

}
