/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "SpoonerMode.h"

#include "ImGuiSpooner.h"
#include "SpoonerCursor.h"
#include "..\PedComponentChanger.h"
#include "..\..\Misc\FreeCam.h"
#include "..\..\macros.h"

#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Keybinds.h"
//#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Util\keyboard.h"
#include "..\..\Scripting\Camera.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\GTAprop.h"
#include "..\..\Scripting\GTAvehicle.h"
#include "..\..\Scripting\GTAped.h"
#include "..\..\Scripting\GTAplayer.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Natives\types.h" //RGBA
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\Model.h"
#include "..\..\Scripting\ModelNames.h"
#include "..\..\Util\StringManip.h"
#include "..\..\Scripting\enums.h"
#include "..\..\Scripting\Game.h"
#include "BlipCustoms.h"

#include "SpoonerSettings.h"
#include "EntityManagement.h"
#include "SpoonerEntity.h"
#include "Databases.h"
#include "SpoonerMarker.h"
#include "MarkerManagement.h"
#include "SpoonerLight.h"
#include "Submenus.h"
#include "..\\..\\Memory\\GTAmemory.h"

#include <utility>
#include <set>
#include <algorithm>
#include <math.h>
#include <Menu/Routine.h>

namespace sub::Spooner
{
	namespace SpoonerMode
	{
		bool bEnabled = false;
		bool hasWarned = false;
		EditingState editingState;
		SpoonerStats GetSpoonerStats()
		{
			SpoonerStats stats = { 0, 0, 0, 0 };
			stats.totalNumEntities = (UINT)Databases::EntityDb.size();
			for (auto& spoonerEntity : Databases::EntityDb)
			{
				switch (spoonerEntity.type)
				{
				case EntityType::PROP: stats.totalNumProps++; break;
				case EntityType::PED: stats.totalNumPeds++; break;
				case EntityType::VEHICLE: stats.totalNumVehicles++; break;
				}
			}
			return stats;
		}

		static bool IsHotkeyPressed()
		{
			if (SpoonerCursor::IsDragging()) return false;
			return Keybinds::WasPressedThisFrame("spooner_mode");
		}


		Vector3 SnapPos(Vector3 pos)
		{
			if (Settings::bGridSnapEnabled && Settings::gridSnapSize > 0.0f)
			{
				float g = Settings::gridSnapSize;
				pos.x = round(pos.x / g) * g;
				pos.y = round(pos.y / g) * g;
				if (!Settings::bSnapToGround)
					pos.z = round(pos.z / g) * g;
			}
			if (Settings::bSnapToGround)
			{
				float groundZ;
				if (GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z + 0.1f, &groundZ, false, false))
					pos.z = groundZ;
			}
			return pos;
		}
		Vector3 SnapRot(Vector3 rot)
		{
			if (Settings::bGridSnapEnabled && Settings::rotationSnapDegrees > 0.0f)
			{
				float r = Settings::rotationSnapDegrees;
				rot.x = round(rot.x / r) * r;
				rot.y = round(rot.y / r) * r;
				rot.z = round(rot.z / r) * r;
			}
			return rot;
		}
		float GetGroundOffset(const GTAmodel::ModelDimensions& dimensions, const Vector3& rotation)
		{
			if (fabs(rotation.x) > 150.0f || fabs(rotation.y) > 150.0f)
				return dimensions.Dim2.z;
			if (fabs(rotation.x) > 70.0f && fabs(rotation.y) > 70.0f)
				return (dimensions.Dim1.y + dimensions.Dim1.x) / 2.0f;
			if (fabs(rotation.x) > 70.0f)
				return dimensions.Dim1.y;
			if (fabs(rotation.y) > 70.0f)
				return dimensions.Dim1.x;
			return dimensions.Dim1.z;
		}

		void DrawSnappingGrid()
		{
			float gridSize = Settings::gridSnapSize;

			Vector3 origin = selectedEntity.handle.GetPosition();
			origin.x = round(origin.x / gridSize) * gridSize;
			origin.y = round(origin.y / gridSize) * gridSize;

			float z = round(origin.z / gridSize) * gridSize;
			const int cells = 10; // number of cells to draw in each direction (i.e setting this to 10 will draw a 20x20 grid)
			const RGBA color(255, 255, 255, 110);

			for (int i = -cells; i <= cells; i++)
			{
				float x = origin.x + i * gridSize;
				Vector3 start(x, origin.y - cells * gridSize, z);
				Vector3 end(x, origin.y + cells * gridSize, z);
				World::DrawLine(start, end, color);
			}

			for (int i = -cells; i <= cells; i++)
			{
				float y = origin.y + i * gridSize;
				Vector3 start(origin.x - cells * gridSize, y, z);
				Vector3 end(origin.x + cells * gridSize, y, z);
				World::DrawLine(start, end, color);
			}
		}

		ModelPreviewInfoStructure modelPreviewInfo = { EntityType::ALL, 0, 0, 0,{} };
		float previewYawOffset = 0.0f;

		void UpdatePreviewRotation()
		{
			if (modelPreviewInfo.entity.Exists() && Menu::activeSubmenu != SUB::CLOSED)
			{
				Menu::add_IB(INPUT_FRONTEND_RB, "");
				Menu::add_IB(INPUT_FRONTEND_LB, "Rotate Preview");

				bool lbPressed = IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LB);
				bool rbPressed = IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RB);
				bool dpadPressed = IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LEFT) ||
					IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RIGHT) ||
					IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_UP) ||
					IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_DOWN);

				if (!dpadPressed)
				{
					if (lbPressed && !rbPressed) previewYawOffset -= 2.0f;
					if (rbPressed && !lbPressed) previewYawOffset += 2.0f;
					if (previewYawOffset > 360.0f || previewYawOffset < -360.0f) previewYawOffset = fmod(previewYawOffset, 360.0f);
				}
			}
		}

		void SpawnModelPreview()
		{
			bool bOnTheLine = NETWORK_IS_IN_SESSION() != 0;
			auto& info = modelPreviewInfo;
			if (info.entityType == EntityType::ALL)
			{
				if (info.entity != 0)
				{
					if (bOnTheLine)
					{
						info.previousEntities.insert(info.entity);
					}
					else
					{
						info.entity.Delete(true);
						info.entity = 0;
					}
				}
				if (info.model.hash != 0)
				{
					if (info.model.IsLoaded())
						info.model.Unload();
					info.model = 0;
				}
				if (info.previousModel.hash != 0)
				{
					if (info.previousModel.IsLoaded())
						info.previousModel.Unload();
					info.previousModel = 0;
				}
			}
			else if (info.model != info.previousModel)
			{
				previewYawOffset = 0.0f;
				if (bOnTheLine)
				{
					info.previousEntities.insert(info.entity);
				}
				else
				{
					info.entity.Delete(true);
					info.entity = 0;
				}
				if (info.previousModel.IsLoaded())
					info.previousModel.Unload();
				info.previousModel = info.model;
				if (info.model.IsInCdImage())
					info.model.Load();
			}
			else
			{
				if (info.entity.Exists())
				{
					const ModelDimensions& dimensions = info.model.Dimensions();

					Vector3 spawnRot(0, 0, SpoonerCamera::camera.GetRotation().z + previewYawOffset);

					const float geGroundZ = GetGroundOffset(dimensions, spawnRot);
					Vector3 spawnPos(SpoonerCamera::camera.RaycastForCoord(Vector2(0.0f, 0.0f), info.entity, 120.0f, 23.0f + dimensions.Dim2.y) + Vector3(0, 0, geGroundZ));

					spawnPos = SnapPos(spawnPos);
					if (Settings::rotationSnapDegrees > 0.0f)
					{
						float r = Settings::rotationSnapDegrees;
						spawnRot.z = round(spawnRot.z / r) * r;
					}

					if (bOnTheLine)
						info.entity.RequestControlOnce();
					info.entity.SetRotation(spawnRot);
					info.entity.SetPosition(spawnPos);
					EntityManagement::ShowBoxAroundEntity(info.entity, false, RGBA::AllWhite());
				}
				else
				{
					if (info.model.IsLoaded())
					{
						switch (info.entityType)
						{
						case EntityType::PROP:
							info.entity = World::CreateProp(info.model, Vector3(), Vector3(), false, false);
							break;
						case EntityType::PED:
							info.entity = World::CreatePed(info.model, Vector3(), Vector3(), false);
							break;
						case EntityType::VEHICLE:
							info.entity = World::CreateVehicle(info.model, Vector3(), Vector3(), false);
							break;
						}
						info.entity.FreezePosition(true);
						info.entity.SetIsCollisionEnabled(false);
						info.entity.SetAlpha(120);
					}
				}
			}

			info.entityType = EntityType::ALL;

			for (auto it = info.previousEntities.begin(); it != info.previousEntities.end();)
			{
				GTAentity e = *it;
				if (e.RequestControlOnce())
				{
					if (e == info.entity)
						info.entity = 0;
					e.Delete(true);
					it = info.previousEntities.erase(it);
				}
				else ++it;
			}
		}

		void ResetSelectedEntity()
		{
			selectedEntity.handle = 0;
		}
		bool GetEntityPtr(GTAentity& inEntity, SpoonerEntity*& outEntity)
		{
			outEntity = new SpoonerEntity;

			outEntity->handle = inEntity;
			outEntity->type = (EntityType)inEntity.Type();
			const Model& outEntityModel = inEntity.Model();
			outEntity->hashName = outEntity->type == EntityType::PROP ? get_prop_model_label(outEntityModel)
				: (outEntity->type == EntityType::PED ? GetPedModelLabel(outEntityModel, true)
					: get_vehicle_model_label(outEntityModel, true));
			if (outEntity->hashName.length() == 0) outEntity->hashName = IntToHexString(outEntityModel.hash, true);
			outEntity->dynamic = !outEntity->handle.IsPositionFrozen();//outEntity->type == EntityType::PED || outEntity->type == EntityType::VEHICLE;
			//outEntity->lastAnimations.clear();
			//outEntity->currentScenario.clear();
			outEntity->isStill = false;

			auto idInDb = EntityManagement::GetEntityIndexInDb(*outEntity);
			if (idInDb >= 0)
			{
				delete outEntity;
				outEntity = &Databases::EntityDb[idInDb];
				return true; // Is in db
			}
			else
			{
				return false; // Is not in db
			}
		}
		SpoonerEntity GetEntityPtrValue(GTAentity& entity)
		{
			SpoonerEntity* eifoc = nullptr;
			bool isAlreadyInDb = SpoonerMode::GetEntityPtr(entity, eifoc);
			SpoonerEntity toReturn = *eifoc;
			if (!isAlreadyInDb)
				delete eifoc;
			return toReturn;
		}
		void SetAsSelectedEntity(GTAentity& entity)
		{
			SpoonerEntity* eifoc = nullptr;
			bool isAlreadyInDb = SpoonerMode::GetEntityPtr(entity, eifoc);
			selectedEntity = *eifoc;
			selectedEntity.handle.RequestControl();
			if (!isAlreadyInDb)
				delete eifoc;
		}

		void OpenMenu(int submenu, int selectedOption)
		{
			std::fill(std::begin(Menu::submenuHistory), std::end(Menu::submenuHistory), 0);
			std::fill(std::begin(Menu::optionSelectionHistory), std::end(Menu::optionSelectionHistory), 0);
			Menu::submenuHistory[0] = SUB::MAINMENU;
			Menu::optionSelectionHistory[0] = 1;
			Menu::menuHistoryIndex = 0;
			Menu::NewSetMenu(submenu);
			if (selectedOption < 1) selectedOption = 1;
			Menu::selectedOptionIndex = selectedOption;
			Menu::selectedOptionWithBreaks = selectedOption;
			*Menu::activeOptionIndex = selectedOption;
		}

		static void HandleTickHotkeys()
		{
			if (IsHotkeyPressed())
				Toggle();
		}

		static void TickModelPreview()
		{
			if (!bEnabled || !Settings::bShowModelPreviews || IS_PAUSE_MENU_ACTIVE()) return;
			if (SpoonerCursor::IsDragging()) return;

			SpawnModelPreview();
		}

		static void DrawSpoonerOverlays()
		{
			if (Settings::bShowBoxAroundSelectedEntity)
				EntityManagement::ShowBoxAroundEntity(selectedEntity.handle);

			if (Settings::bDrawGrid && Settings::bGridSnapEnabled && bEnabled && selectedEntity.handle.Exists())
				DrawSnappingGrid();
		}

		static void TickEntityTasks()
		{
			for (auto& entity : Databases::EntityDb)
			{
				if (entity.handle.Exists())
					entity.taskSequence.Tick(reinterpret_cast<void*>(&entity));
			}
		}

		static void DrawSpoonerWorldItems()
		{
			if (!Databases::MarkerDb.empty())
				MarkerManagement::DrawAll();
			if (!Databases::LightDb.empty())
				LightManagement::DrawAll();
			if (!Databases::BlipDb.empty())
			{
				auto sub = Menu::activeSubmenu;
				bool bInBlipSub =
					sub == SUB::SPOONER_BLIPS ||
					sub == SUB::SPOONER_BLIPS_ADD_SELECT ||
					sub == SUB::SPOONER_BLIPS_RADIALINBLIP ||
					sub == SUB::SPOONER_BLIPS_COORDINBLIP ||
					sub == SUB::SPOONER_BLIPS_ENTITYINBLIP ||
					sub == SUB::SPOONER_BLIPS_ATTACH ||
					sub == SUB::SPOONER_BLIPS_ENTITY_SELECT ||
					sub == SUB::SPOONER_BLIPS_ICONS;

				if (bInBlipSub)
					BlipCustoms::DrawAll();
				BlipCustoms::UpdateAttachedBlips();
			}
		}

		static void ApplyEntityScales()
		{
			auto applyScale = [](const Submenus::EntityScaleState& state)
			{
				if (state.handle == 0) return;
				GTAentity(state.handle).SetScale(state.scale);
			};

			applyScale(Submenus::_vehScale);
			applyScale(Submenus::_pedScale);
			applyScale(Submenus::_objScale);
		}

		void Tick()
		{
			HandleTickHotkeys();
			ImGuiSpooner::Tick();
			UpdatePreviewRotation();
			SpoonerCamera::Tick();
			TickModelPreview();
			SpoonerCursor::Tick();
			DrawSpoonerOverlays();
			TickEntityTasks();
			DrawSpoonerWorldItems();
			ApplyEntityScales();
		}

		void TurnOn()
		{
			if (!menuHasNotOpened)
			{
				WardrobeCamera::Disable(false);
				FreeCamMode::Stop();
				SpoonerMode::bEnabled = true;
				sub::Spooner::ImGuiSpooner::SetVisible(true);
				if (Menu::activeSubmenu != SUB::CLOSED)
					Game::Print::PrintBottomLeft("~b~Note:~s~ Spooner Mode instructions only appear when Menyoo is closed.");
			}
			else
			{
				Game::Print::ShowNotification("~r~Error:", "Menu not opened yet.");
			}
		}
		void TurnOff()
		{
			WardrobeCamera::Disable(false);
			SpoonerCursor::Reset();
			SpoonerMode::bEnabled = false;
			sub::Spooner::ImGuiSpooner::SetVisible(false);
			SpoonerMode::editingState.SetMode(SpoonerMode::eEditMode::Disabled);
			auto& info = modelPreviewInfo;
			for (auto it = info.previousEntities.begin(); it != info.previousEntities.end();)
			{
				GTAentity e = *it;
				e.RequestControl(600);
				if (e != info.entity)
					e.Delete(true);
				++it;
			}
			info.previousEntities.clear();
			if (info.entity != 0)
			{
				info.entityType = EntityType::ALL;
				SpoonerMode::SpawnModelPreview();
			}
		}
		void ProcessKeyboardManipulation(Vector3& position, Vector3& rotation)
		{
			if (!bEnabled) return;

			float& precision = editingState.transformMode == eTransformMode::Position ? editingState.precisionPos
			                 : editingState.transformMode == eTransformMode::Rotation ? editingState.precisionRot
			                 : editingState.precisionScale;

			static DWORD lastSensitivityChange = 0;
			if (Keybinds::WasPressedThisFrame("spooner_sensitivity_up") && GetTickCount() - lastSensitivityChange > 200)
			{
				if (precision < 10.0f) precision *= 10;
				lastSensitivityChange = GetTickCount();
				Game::Print::PrintBottomCentre("Sensitivity: ~b~" + std::to_string(precision), 3000);
			}
			if (Keybinds::WasPressedThisFrame("spooner_sensitivity_down") && GetTickCount() - lastSensitivityChange > 200)
			{
				if (precision > 0.0001f) precision /= 10;
				lastSensitivityChange = GetTickCount();
				Game::Print::PrintBottomCentre("Sensitivity: ~b~" + std::to_string(precision), 3000);
			}

			float step = precision;
			// if grid snap is enabled, override precision with the snap amount for the current transform mode
			if (Settings::bGridSnapEnabled)
			{
				float snapAmount = editingState.transformMode == eTransformMode::Rotation
					? Settings::rotationSnapDegrees
					: Settings::gridSnapSize;
				if (snapAmount > 0.0f) step = snapAmount;
			}

			auto& target = editingState.transformMode == eTransformMode::Rotation ? rotation : position;
			if (IsKeyDown(VirtualKey::W)) target.x += step;
			if (IsKeyDown(VirtualKey::S)) target.x -= step;
			if (IsKeyDown(VirtualKey::A)) target.y += step;
			if (IsKeyDown(VirtualKey::D)) target.y -= step;
			if (IsKeyDown(VirtualKey::E)) target.z += step;
			if (IsKeyDown(VirtualKey::Q)) target.z -= step;

			if (editingState.transformMode == eTransformMode::Rotation)
				rotation = SnapRot(rotation);
			else
				position = SnapPos(position);
		}

		void DrawEditingHUD()
		{
			if (!bEnabled && !hasWarned)
			{
				Game::Print::ShowNotification("Entity manipulation requires the Spooner Camera.", "~(b~Press F9:~w~ Enable Spooner Mode.",5);
				hasWarned = true;	
				return;
			}

			if (editingState.mode == eEditMode::Disabled)
			{
				Keybinds::AddBindIB("spooner_edit_mode", "Keyboard Controls");
			}
			else if (editingState.mode == eEditMode::Keyboard)
			{
				if (editingState.transformMode == eTransformMode::Rotation)
				{
					Keybinds::AddBindIB("spooner_sensitivity_down", "Sensitivity");
					Keybinds::AddBindIB("spooner_sensitivity_up", "Sensitivity");
					Menu::add_IB(VirtualKey::D, "Roll-");
					Menu::add_IB(VirtualKey::A, "Roll+");
					Menu::add_IB(VirtualKey::Q, "Yaw-");
					Menu::add_IB(VirtualKey::E, "Yaw+");
					Menu::add_IB(VirtualKey::S, "Pitch-");
					Menu::add_IB(VirtualKey::W, "Pitch+");
					Keybinds::AddBindIB("spooner_edit_transform", "Edit Position");
				}
				else
				{
					Keybinds::AddBindIB("spooner_sensitivity_down", "Sensitivity");
					Keybinds::AddBindIB("spooner_sensitivity_up", "Sensitivity");
					Menu::add_IB(VirtualKey::Q, "Z-");
					Menu::add_IB(VirtualKey::E, "Z+");
					Menu::add_IB(VirtualKey::D, "Y-");
					Menu::add_IB(VirtualKey::A, "Y+");
					Menu::add_IB(VirtualKey::S, "X-");
					Menu::add_IB(VirtualKey::W, "X+");
					Keybinds::AddBindIB("spooner_edit_transform", "Edit Rotation");
				}
				Keybinds::AddBindIB("spooner_edit_copy", "Copy");
					Keybinds::AddBindIB("spooner_edit_mode", "Gizmo Controls");
			}
			else if (editingState.mode == eEditMode::Gizmo)
			{
				std::string modeName;
				switch (editingState.transformMode)
				{
					case eTransformMode::Rotation: modeName = "Rotation"; break;
					case eTransformMode::Scale:  modeName = "Scale";    break;
					default:                              modeName = "Position"; break;
				}

				Menu::add_IB(INPUT_CURSOR_ACCEPT, "Grab axis handle (" + modeName + " Mode)");
					Keybinds::AddBindIB("spooner_edit_transform", "Cycle mode");
				Keybinds::AddBindIB("spooner_camera_lock", editingState.cameraLocked ? "Unlock camera" : "Lock camera");
				Keybinds::AddBindIB("spooner_local_space", editingState.localSpace ? "Edit in world space" : "Edit in local space");
				Keybinds::AddBindIB("spooner_edit_copy", "Copy");
				Keybinds::AddBindIB("spooner_edit_mode", "Disable Controls");
			}
		}

		void UpdateEntityEditingState(Vector3& position, Vector3& rotation)
		{
			// toggling between Disabled / Keyboard / Gizmo modes
			static bool lastBToggle = false;
			bool currentBToggle = Keybinds::WasPressedThisFrame("spooner_edit_mode");
			if (currentBToggle && !lastBToggle)
			{
				switch (editingState.mode)
				{
					case eEditMode::Disabled:
						editingState.SetMode(eEditMode::Keyboard);
						break;
					case eEditMode::Keyboard:
						editingState.SetMode(eEditMode::Gizmo);
						break;
					case eEditMode::Gizmo:
						editingState.SetMode(eEditMode::Disabled);
						break;
					}
			}
			lastBToggle = currentBToggle;

			// toggling between transform modes
			static bool lastRToggle = false;
			bool currentRToggle = Keybinds::WasPressedThisFrame("spooner_edit_transform");
			if (currentRToggle && !lastRToggle)
			{
				if (editingState.mode != eEditMode::Disabled)
				{
					// In keyboard mode, R just toggles between position and rotation editing (scale is not supported in keyboard mode)
					static const eTransformMode table[2][3] = {
						// Position, Rotation, Scale
						{ eTransformMode::Rotation, eTransformMode::Position, eTransformMode::Position }, // Keyboard editing mode (scale is not supported, it just redirects to position)
						{ eTransformMode::Rotation, eTransformMode::Scale,    eTransformMode::Position }  // Gizmo editing mode
					};
					editingState.transformMode = table[(int)editingState.mode - 1][(int)editingState.transformMode];
				}
			}
			lastRToggle = currentRToggle;

			// toggling camera lock
			if (editingState.mode != eEditMode::Disabled && Keybinds::WasPressedThisFrame("spooner_camera_lock"))
			{
				editingState.cameraLocked = !editingState.cameraLocked;
			}

			// toggling world / local space editing
			if (editingState.mode != eEditMode::Disabled && Keybinds::WasPressedThisFrame("spooner_local_space"))
			{
				editingState.localSpace = !editingState.localSpace;
			}

			// make a quick copy of an entity by clicking C in editing modes
			if (editingState.mode != eEditMode::Disabled && Keybinds::WasPressedThisFrame("spooner_edit_copy"))
			{
				if (selectedEntity.handle.Exists())
				{
					const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(selectedEntity, EntityManagement::GetEntityIndexInDb(selectedEntity) >= 0, true, Submenus::_copyEntTexterValue);
					selectedEntity = copiedEntity;
					Game::Print::ShowNotification("Entity copied.", 2.5f);
				}
			}

			if (editingState.mode == eEditMode::Keyboard)
			{
				// keyboard edit mode doesn't support scaling
				if (editingState.transformMode == eTransformMode::Scale)
					editingState.transformMode = eTransformMode::Position;
				ProcessKeyboardManipulation(position, rotation);
			}

			DrawEditingHUD();
		}

		void Toggle()
		{
			SpoonerMode::bEnabled ? SpoonerMode::TurnOff() : SpoonerMode::TurnOn();
		}
	}

}
