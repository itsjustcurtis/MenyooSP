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

#include <utility>
#include <set>

typedef unsigned char UINT8, BYTE;
typedef unsigned short UINT16;

class Camera;

namespace sub::Spooner
{
	class SpoonerEntity;

	namespace SpoonerMode
	{
		extern BYTE bindsKeyboard;
		extern std::pair<UINT16, UINT16> bindsGamepad;

		extern bool bEnabled;
		extern bool bIsSomethingHeld;
		extern bool bHeldEntityHasCollision;

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
		};
		extern EditingState editingState;

		void ProcessKeyboardManipulation(Vector3& position, Vector3& rotation);
		void DrawEditingHUD();
		void UpdateEntityEditingState(Vector3& position, Vector3& rotation);
		extern Camera spoonerModeCamera;
		extern float spoonerModeCameraCamDistance;
		extern float spoonerModeCameraSpeed;

		struct SpoonerStats {
			int totalNumEntities;
			int totalNumProps;
			int totalNumPeds;
			int totalNumVehicles;
		};
		SpoonerStats GetSpoonerStats();

		bool IsHotkeyPressed();

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
		bool GetEntityPtr(GTAentity& inEntity, SpoonerEntity*& outEntity);
		SpoonerEntity GetEntityPtrValue(GTAentity& entity);
		inline void SetAsSelectedEntity(GTAentity& entity);
		Vector3 SnapPos(Vector3 pos);
		Vector3 SnapRot(Vector3 rot);
		void DrawSnappingGrid();

		inline void CamTick();
		void Tick();

		void TurnOn();
		void TurnOff();
		void Toggle();
	}

}
