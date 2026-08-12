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
#include "..\..\macros.h"

#include "..\..\Menu\Menu.h"
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
#include <math.h>
#include <Menu/Routine.h>

namespace sub::Spooner
{
	namespace SpoonerMode
	{
		BYTE bindsKeyboard = VirtualKey::F9;
		std::pair<UINT16, UINT16> bindsGamepad = { INPUT_FRONTEND_RB, INPUT_FRONTEND_RIGHT };

		bool bEnabled = false;
		bool bIsSomethingHeld = false;
		bool bHeldEntityHasCollision = true;
		EditingState editingState;
		Camera spoonerModeCamera;
		float spoonerModeCameraCamDistance = 5.0f;
		float spoonerModeCameraSpeed = 1.0f;
		DWORD lastSpoonerSpeedDisplayTime = 0;
		float lastSpoonerSpeedValue = 1.0f;
		eSpoonerModeMode& spoonerModeMode = Settings::spoonerModeMode;

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

		bool IsHotkeyPressed()
		{
		bool bInSpoonerMenu = std::find(std::begin(Menu::submenuHistory), std::end(Menu::submenuHistory), SUB::SPOONER_MAIN) != std::end(Menu::submenuHistory);

			if (!bInSpoonerMenu || !bIsSomethingHeld)
			{
				UINT8 index1 = bindsGamepad.first < 50 ? 0 : 2;
				UINT8 index2 = bindsGamepad.second < 50 ? 0 : 2;
				return Menu::bitController ? (IS_DISABLED_CONTROL_PRESSED(index1, bindsGamepad.first) && IS_DISABLED_CONTROL_JUST_PRESSED(index2, bindsGamepad.second)) : IsKeyJustUp(bindsKeyboard);
			}
			return false;
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

					Vector3 spawnRot(0, 0, spoonerModeCamera.GetRotation().z + previewYawOffset);

					const Vector3& geSep = info.entity.GetPosition();
					//auto& geGroundRay = RaycastResult::Raycast(geSep, Vector3::WorldDown(), max(max(dimensions.Dim1.x, dimensions.Dim2.x), max(max(dimensions.Dim1.y, dimensions.Dim2.y), max(dimensions.Dim1.z, dimensions.Dim2.z))) + 2.0f, IntersectOptions::Everything, info.entity);
					float geGroundZ = dimensions.Dim1.z;
					//if (geGroundRay.DidHitAnything()){
					//float oldYaw = spawnRot.z;
					//geGroundZ = geGroundRay.HitCoords().DistanceTo(geSep);
					//Vector3 spawnRot;
					//spawnRot = Vector3::DirectionToRotation(geGroundRay.SurfaceNormal());
					//spawnRot.x += 90.0f;
					//spawnRot.z = oldYaw;
					//}
					if (abs(spawnRot.x) > 150.0f || abs(spawnRot.y) > 150.0f) geGroundZ = dimensions.Dim2.z;
					else if (abs(spawnRot.x) > 70.0f && abs(spawnRot.y) > 70.0f) geGroundZ = (dimensions.Dim1.y + dimensions.Dim1.x) / 2;
					else if (abs(spawnRot.x) > 70.0f) geGroundZ = dimensions.Dim1.y;
					else if (abs(spawnRot.y) > 70.0f) geGroundZ = dimensions.Dim1.x;
					Vector3 spawnPos(spoonerModeCamera.RaycastForCoord(Vector2(0.0f, 0.0f), info.entity, 120.0f, 23.0f + dimensions.Dim2.y) + Vector3(0, 0, geGroundZ));

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
		inline void SetAsSelectedEntity(GTAentity& entity)
		{
			SpoonerEntity* eifoc = nullptr;
			bool isAlreadyInDb = SpoonerMode::GetEntityPtr(entity, eifoc);
			selectedEntity = *eifoc;
			selectedEntity.handle.RequestControl();
			if (!isAlreadyInDb)
				delete eifoc;
		}

		inline void CamTick()
		{
			if (IS_PAUSE_MENU_ACTIVE())
				return;

			GTAplayer myPlayer = Game::Player();
			GTAped myPed = Game::PlayerPed();

			Camera& freeCam = SpoonerMode::spoonerModeCamera;
			float& freeCamCamDistance = SpoonerMode::spoonerModeCameraCamDistance;

			if (SpoonerMode::bEnabled)
			{
				HIDE_HUD_AND_RADAR_THIS_FRAME();

				if (!freeCam.Exists())
				{
					const Vector3& myPos = myPed.GetPosition();
					freeCam = World::CreateCamera(myPos + Vector3(0, 0, 2.8f), Vector3(0, 0, myPed.Rotation_get().z), 73.f);
					freeCam.SetActive(false);
				}
				if (!freeCam.IsActive())
				{
					freeCam.SetActive(true);
					Camera::RenderScriptCams(true);
				}
				myPlayer.SetControl(false, 0);
				DISABLE_ALL_CONTROL_ACTIONS(0);
				DISABLE_ALL_CONTROL_ACTIONS(2);
				ENABLE_CONTROL_ACTION(2, INPUT_FRONTEND_PAUSE, true);
				ENABLE_CONTROL_ACTION(2, INPUT_FRONTEND_PAUSE_ALTERNATE, true);

				Vector3 nextOffset;
				Vector3 nextRot;

				const Vector3& coordInFrontOfCam = freeCam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 160.0f, 3.0f);
				GTAentity entityInFrontOfCam = freeCam.RaycastForEntity(Vector2(0.0f, 0.0f), 0, 160.0f);

				if (Menu::bitController) // If controller
				{
					float movementSensitivity = Settings::cameraMovementSensitivityGamepad;
					//if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LS)) movementSensitivity += 1.36f * movementSensitivity;
					if (!bIsSomethingHeld)
						movementSensitivity *= spoonerModeCameraSpeed;

					nextOffset.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movementSensitivity;
					nextOffset.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movementSensitivity;

					float rotationSensitivity = Settings::cameraRotationSensitivityGamepad;
					nextRot.z = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR) * rotationSensitivity;
					nextRot.x = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD) * rotationSensitivity;
					nextRot.y = !IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RB) ? (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LB) ? -2.0f : 0.0f) : 2.0f;

					if (!bIsSomethingHeld || spoonerModeMode == eSpoonerModeMode::GroundEase)
					{
						if (!bIsSomethingHeld)
						{
							nextRot.y = -freeCam.GetRotation().y; // Roll should be 0 when no entity is held

							// Mouse wheel to adjust speed
							if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
							{
								spoonerModeCameraSpeed = min(spoonerModeCameraSpeed + 0.1f, 10.0f);
								lastSpoonerSpeedValue = spoonerModeCameraSpeed;
								lastSpoonerSpeedDisplayTime = GetTickCount();
							}
							if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
							{
								spoonerModeCameraSpeed = max(spoonerModeCameraSpeed - 0.1f, 0.1f);
								lastSpoonerSpeedValue = spoonerModeCameraSpeed;
								lastSpoonerSpeedDisplayTime = GetTickCount();
							}
						}
						if (nextOffset.x || nextOffset.y)
							freeCam.SetPosition(freeCam.GetOffsetInWorldCoords(nextOffset.x, nextOffset.y, 0));

						if (!bIsSomethingHeld && Settings::bShowModelPreviews)
							SpoonerMode::SpawnModelPreview();
					}
					if (!bIsSomethingHeld && GetTickCount() - lastSpoonerSpeedDisplayTime < 1000)
					{
						Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.4f, 0.4f), true, false, false);
						Game::Print::DrawString(oss_ << "Spooner Camera Speed: " << lastSpoonerSpeedValue, 0.5f, 0.95f);
					}
					if (!nextRot.IsZero())
					{
						Vector3 nextRotFinal = freeCam.GetRotation() + nextRot;
						//float fcrXfinal = fmod(nextRotFinal.x, 360.0f); // What if -10/350/710?
//if (fcrXfinal > -10.0f && fcrXfinal < 0.0f)
//	nextRotFinal.x = -10.0f;
//else if (fcrXfinal >= 0.0f && fcrXfinal < 10.0f)
//	nextRotFinal.x = 10.0f;
						switch (spoonerModeMode)
						{
						case eSpoonerModeMode::GroundEase:
							nextRotFinal.y = 0.0f;
							break;
						case eSpoonerModeMode::Precision:
							break;
						}
						freeCam.SetRotation(nextRotFinal);
					}

					if (Menu::activeSubmenu == SUB::CLOSED)
					{
						Menu::add_IB(INPUT_VEH_EXIT, "Open main menu");
						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_VEH_EXIT))
						{
							memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
							memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
							Menu::submenuHistory[0] = SUB::MAINMENU;
							Menu::optionSelectionHistory[0] = 1;
							Menu::menuHistoryIndex = 0;
							Menu::NewSetMenu(SUB::SPOONER_MAIN);
							Menu::selectedOptionIndex = 2;
						}

						if (!bIsSomethingHeld)
						{
							Menu::add_IB(INPUT_FRONTEND_DOWN, "Place Marker");
							if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_DOWN))
							{
								auto newMarkerPtr = MarkerManagement::AddMarker(coordInFrontOfCam, Vector3(0, 0, freeCam.GetRotation().z));
								if (newMarkerPtr != nullptr)
								{
									newMarkerPtr->m_position.z += (newMarkerPtr->m_scale / 2);
									SelectedMarker = newMarkerPtr;
									memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
									memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
									Menu::submenuHistory[0] = SUB::MAINMENU;
									Menu::optionSelectionHistory[0] = 1;
									Menu::menuHistoryIndex = 0;
									Menu::NewSetMenu(SUB::SPOONER_MANAGEMARKERS_INMARKER);
								}
							}
						}
					}

					if (entityInFrontOfCam.Exists() || bIsSomethingHeld)
					{
						DRAW_RECT(0.5f, 0.5f, 0.02f, 0.002f, 0, 255, 0, 255, false);
						DRAW_RECT(0.5f, 0.5f, 0.001f, 0.03f, 0, 255, 0, 255, false);

						GTAentity* currentEntPtr = bIsSomethingHeld ? &selectedEntity.handle : &entityInFrontOfCam;
						GTAentity& currentEnt = *currentEntPtr;

						auto indexInDb = EntityManagement::GetEntityIndexInDb(currentEnt);
						bool isInDb = indexInDb >= 0;

						if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LT))
						{
							if (!bIsSomethingHeld)
							{
								bIsSomethingHeld = true;
								SpoonerMode::SetAsSelectedEntity(currentEnt);

								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									freeCam.PointAt(selectedEntity.handle);
									freeCam.StopPointing();
									bHeldEntityHasCollision = selectedEntity.handle.GetIsCollisionEnabled();
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), false, false);
									break;
								case eSpoonerModeMode::Precision:
									bHeldEntityHasCollision = selectedEntity.handle.GetIsCollisionEnabled();
									freeCam.SetRotation(selectedEntity.handle.Rotation_get());
									break;
								}
							}

							DRAW_RECT(0.5f, 0.5f, 0.004f, 0.008f, 255, 128, 0, 255, false);

							selectedEntity.handle.RequestControl();
							Vector3 rotSelected = selectedEntity.handle.Rotation_get();
							Vector3 rotFreeCam = freeCam.GetRotation();
							switch (spoonerModeMode)
							{
							case eSpoonerModeMode::GroundEase:
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RS))
									rotSelected.x -= 2.0f; // Decrease pitch RS
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LS))
									rotSelected.x += 2.0f; // Increase pitch LS
								rotSelected.y += nextRot.y; // Increase/Decrease pitch using RB/LB
								rotSelected.z += nextRot.z; // Relative yaw kept constant
								selectedEntity.handle.SetRotation(rotSelected);
								break;
							case eSpoonerModeMode::Precision:
								selectedEntity.handle.SetRotation(rotFreeCam);
								break;
							}
							rotSelected = selectedEntity.handle.Rotation_get(); // To get -180 to 180 values

							const ModelDimensions& mdSelectedEntity = selectedEntity.handle.ModelDimensions();
							switch (spoonerModeMode)
							{
							case eSpoonerModeMode::GroundEase:
							{
								//Vector3& geSep = selectedEntity.handle.Position_get();
								//auto& geGroundRay = RaycastResult::Raycast(geSep, Vector3::WorldDown(), max(max(mdSelectedEntity.Dim1.x, mdSelectedEntity.Dim2.x), max(max(mdSelectedEntity.Dim1.y, mdSelectedEntity.Dim2.y), max(mdSelectedEntity.Dim1.z, mdSelectedEntity.Dim2.z))) + 2.0f, IntersectOptions::Everything, selectedEntity.handle);
								float geGroundZ = mdSelectedEntity.Dim1.z;
								//if (geGroundRay.DidHitAnything())
								//{
								//geGroundZ = geGroundRay.HitCoords().DistanceTo(geSep);
								//}
								if (abs(rotSelected.x) > 150.0f || abs(rotSelected.y) > 150.0f)
									geGroundZ = mdSelectedEntity.Dim2.z;
								else if (abs(rotSelected.x) > 70.0f && abs(rotSelected.y) > 70.0f)
									geGroundZ = (mdSelectedEntity.Dim1.y + mdSelectedEntity.Dim1.x) / 2;
								else if (abs(rotSelected.x) > 70.0f)
									geGroundZ = mdSelectedEntity.Dim1.y;
								else if (abs(rotSelected.y) > 70.0f)
									geGroundZ = mdSelectedEntity.Dim1.x;
								selectedEntity.handle.SetPosition(SnapPos(spoonerModeCamera.RaycastForCoord(Vector2(0.0f, 0.0f), selectedEntity.handle, 90.0f, 15.0f + mdSelectedEntity.Dim2.y) + Vector3(0, 0, geGroundZ)));
								break;
							}
							case eSpoonerModeMode::Precision:
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RS))
									freeCamCamDistance -= 0.1f; // Zoom in RS
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LS))
									freeCamCamDistance += 0.1f; // Zoom out LS
								Vector3 attachmentOffset = { 0.0f, -mdSelectedEntity.Dim2.y - freeCamCamDistance, 0.0f };
								freeCam.AttachTo(selectedEntity.handle, attachmentOffset);
								selectedEntity.handle.SetPosition(SnapPos(selectedEntity.handle.GetOffsetInWorldCoords(nextOffset)));
								if (Settings::bFreezeEntityWhenMovingIt)
									selectedEntity.handle.FreezePosition(Settings::bFreezeEntityWhenMovingIt);
								break;
							}

							if (Menu::activeSubmenu == SUB::CLOSED)
							{
								Menu::add_IB(INPUT_FRONTEND_RT, "Open property menu");
								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									Menu::add_IB(INPUT_FRONTEND_RS, "");
									Menu::add_IB(INPUT_FRONTEND_LS, "Adjust pitch rotation");
									break;
								case eSpoonerModeMode::Precision:
									Menu::add_IB(INPUT_FRONTEND_RS, "");
									Menu::add_IB(INPUT_FRONTEND_LS, "Zoom camera in/out");
									break;
								}
								Menu::add_IB(INPUT_FRONTEND_RB, "");
								Menu::add_IB(INPUT_FRONTEND_LB, "Adjust roll rotation");
								Menu::add_IB(INPUT_FRONTEND_RIGHT, "Copy (and add to DB)");
								Menu::add_IB(INPUT_FRONTEND_LEFT, "Delete");
								if (!isInDb)
								{
									Menu::add_IB(INPUT_FRONTEND_UP, "Add to Database");
									if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_UP))
									{
										EntityManagement::AddEntityToDb(selectedEntity);
									}
								}

								if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RIGHT))
								{
									const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(selectedEntity, isInDb, true, Submenus::_copyEntTexterValue);
									//EntityManagement::AddEntityToDb(copiedEntity);
									selectedEntity = copiedEntity;
								}
								if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LEFT))
								{
									selectedEntity.handle.RequestControl(600);
									EntityManagement::DeleteEntity(selectedEntity);
									bIsSomethingHeld = false;
								}
							}
						}
						else
						{
							if (bIsSomethingHeld)
							{
								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), bHeldEntityHasCollision, true);
									freeCam.Detach(); // Just in case
									break;
								case eSpoonerModeMode::Precision:
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), bHeldEntityHasCollision, true);
									freeCam.Detach();
									break;
								}
							}
							bIsSomethingHeld = false;

							if (Menu::activeSubmenu == SUB::CLOSED)
							{
								Menu::add_IB(INPUT_FRONTEND_RT, "Open property menu");
								Menu::add_IB(INPUT_FRONTEND_LT, "Move entity around (hold)");
								Menu::add_IB(INPUT_FRONTEND_RIGHT, "Copy (and add to DB)");
								Menu::add_IB(INPUT_FRONTEND_LEFT, "Delete");
								if (!isInDb)
								{
									Menu::add_IB(INPUT_FRONTEND_UP, "Add to Database");
									if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_UP))
									{
										EntityManagement::AddEntityToDb(GetEntityPtrValue(currentEnt));
									}
								}

								if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RIGHT))
								{
									const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(GetEntityPtrValue(currentEnt), isInDb, true, Submenus::_copyEntTexterValue);
									//EntityManagement::AddEntityToDb(copiedEntity);
									selectedEntity = copiedEntity;
								}
								else if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LEFT))
								{
									auto entPtrVal = GetEntityPtrValue(currentEnt);
									entPtrVal.handle.RequestControl(600);
									EntityManagement::DeleteEntity(entPtrVal);
									bIsSomethingHeld = false;
								}
							}
						}
						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RT))
						{
							if (!bIsSomethingHeld)
							{
								SpoonerMode::SetAsSelectedEntity(currentEnt);
							}
							memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
							memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
							Menu::submenuHistory[0] = SUB::MAINMENU;
							Menu::optionSelectionHistory[0] = 1;
							Menu::menuHistoryIndex = 0;
							Menu::NewSetMenu(SUB::SPOONER_SELECTEDENTITYOPS);
						}
					}
					else
					{
						DRAW_RECT(0.5f, 0.5f, 0.02f, 0.002f, 255, 255, 255, 255, false);
						DRAW_RECT(0.5f, 0.5f, 0.001f, 0.03f, 255, 255, 255, 255, false);
					}
				}
				else // If keyboard + mouse
				{
					float movementSensitivity = Settings::cameraMovementSensitivityKeyboard;
					if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_SPRINT))
						movementSensitivity = 4.0f * movementSensitivity;
					if (!bIsSomethingHeld)
						movementSensitivity *= spoonerModeCameraSpeed;

					if (editingState.mode != eEditMode::Keyboard && !(editingState.mode == eEditMode::Gizmo && editingState.cameraLocked))
					{
						nextOffset.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movementSensitivity;
						nextOffset.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movementSensitivity;
						nextOffset.z = IsKeyDown(VirtualKey::X) ? movementSensitivity / 2 : IsKeyDown(VirtualKey::Z) ? -movementSensitivity / 2 : 0.0f;
					}

					// blocks camera rotation while we are using the gizmo to edit entity pos / rot
					if (!editingState.cameraLocked || editingState.mode != eEditMode::Gizmo)
					{
						float rotationSensitivity = Settings::cameraRotationSensitivityMouse;
						nextRot.z = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR) * rotationSensitivity;
						nextRot.x = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD) * rotationSensitivity;
						nextRot.y = !IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_RIGHT) ? (IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_LEFT) ? -2.0f : 0.0f) : 2.0f;
					}

					if (!bIsSomethingHeld || spoonerModeMode == eSpoonerModeMode::GroundEase)
					{
						if (!bIsSomethingHeld)
						{
							nextRot.y = -freeCam.GetRotation().y; // Roll should be 0 when no entity is held

							// Mouse wheel to adjust speed
							if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
							{
								spoonerModeCameraSpeed = min(spoonerModeCameraSpeed + 0.1f, 10.0f);
								lastSpoonerSpeedValue = spoonerModeCameraSpeed;
								lastSpoonerSpeedDisplayTime = GetTickCount();
							}
							if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
							{
								spoonerModeCameraSpeed = max(spoonerModeCameraSpeed - 0.1f, 0.1f);
								lastSpoonerSpeedValue = spoonerModeCameraSpeed;
								lastSpoonerSpeedDisplayTime = GetTickCount();
							}
						}
						if (!nextOffset.IsZero())
							freeCam.SetPosition(freeCam.GetOffsetInWorldCoords(nextOffset));

						if (!bIsSomethingHeld && Settings::bShowModelPreviews)
							SpoonerMode::SpawnModelPreview();
					}
					if (!bIsSomethingHeld && GetTickCount() - lastSpoonerSpeedDisplayTime < 1000)
					{
						Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.4f, 0.4f), true, false, false);
						Game::Print::DrawString(oss_ << "Spooner Camera Speed: " << lastSpoonerSpeedValue, 0.5f, 0.95f);
					}
					if (!nextRot.IsZero())
					{
						Vector3 nextRotFinal = freeCam.GetRotation() + nextRot;
						//float fcrXfinal = fmod(nextRotFinal.x, 360.0f); // What if -10/350/710?
//if (fcrXfinal > -10.0f && fcrXfinal < 0.0f)
//	nextRotFinal.x = -10.0f;
//else if (fcrXfinal >= 0.0f && fcrXfinal < 10.0f)
//	nextRotFinal.x = 10.0f;
						switch (spoonerModeMode)
						{
						case eSpoonerModeMode::GroundEase:
							nextRotFinal.y = 0.0f;
							break;
						case eSpoonerModeMode::Precision:
							break;
						}
						freeCam.SetRotation(nextRotFinal);
					}

					if (Menu::activeSubmenu == SUB::CLOSED)
					{
						Menu::add_IB(INPUT_VEH_EXIT, "Open main menu");
						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_VEH_EXIT))
						{
							memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
							memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
							Menu::submenuHistory[0] = SUB::MAINMENU;
							Menu::optionSelectionHistory[0] = 1;
							Menu::menuHistoryIndex = 0;
							Menu::NewSetMenu(SUB::SPOONER_MAIN);
							Menu::selectedOptionIndex = 2;
						}

						if (!bIsSomethingHeld)
						{
							Menu::add_IB(VirtualKey::M, "Place Marker");
							if (IsKeyJustUp(VirtualKey::M))
							{
								auto newMarkerPtr = MarkerManagement::AddMarker(coordInFrontOfCam, Vector3(0, 0, freeCam.GetRotation().z));
								if (newMarkerPtr != nullptr)
								{
									newMarkerPtr->m_position.z += (newMarkerPtr->m_scale / 2);
									SelectedMarker = newMarkerPtr;
									memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
									memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
									Menu::submenuHistory[0] = SUB::MAINMENU;
									Menu::optionSelectionHistory[0] = 1;
									Menu::menuHistoryIndex = 0;
									Menu::NewSetMenu(SUB::SPOONER_MANAGEMARKERS_INMARKER);
								}
							}
						}
					}

					// does not draw the cursor when inside gizmo entity editing mode.
					if (editingState.mode != eEditMode::Gizmo && (entityInFrontOfCam.Exists() || bIsSomethingHeld))
					{
						DRAW_RECT(0.5f, 0.5f, 0.02f, 0.002f, 0, 255, 0, 255, false);
						DRAW_RECT(0.5f, 0.5f, 0.001f, 0.03f, 0, 255, 0, 255, false);

						GTAentity* currentEntPtr = bIsSomethingHeld ? &selectedEntity.handle : &entityInFrontOfCam;
						GTAentity& currentEnt = *currentEntPtr;

						auto indexInDb = EntityManagement::GetEntityIndexInDb(currentEnt);
						bool isInDb = indexInDb >= 0;

						if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
						{
							if (!bIsSomethingHeld)
							{
								bIsSomethingHeld = true;
								SpoonerMode::SetAsSelectedEntity(currentEnt);

								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									freeCam.PointAt(selectedEntity.handle);
									freeCam.StopPointing();
									bHeldEntityHasCollision = selectedEntity.handle.GetIsCollisionEnabled();
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), false, false);
									break;
								case eSpoonerModeMode::Precision:
									bHeldEntityHasCollision = selectedEntity.handle.GetIsCollisionEnabled();
									freeCam.SetRotation(selectedEntity.handle.Rotation_get());
									break;
								}
							}

							DRAW_RECT(0.5f, 0.5f, 0.004f, 0.008f, 255, 128, 0, 255, false);

							selectedEntity.handle.RequestControl();
							Vector3 rotSelected = selectedEntity.handle.Rotation_get();
							Vector3 rotFreeCam = freeCam.GetRotation();
							switch (spoonerModeMode)
							{
							case eSpoonerModeMode::GroundEase:
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
									rotSelected.x -= 2.0f; // Decrease pitch ScrollDown
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
									rotSelected.x += 2.0f; // Increase pitch ScrollUp
								rotSelected.y += nextRot.y; // Increase/Decrease pitch using RB/LB
								rotSelected.z += nextRot.z; // Relative yaw kept constant
								selectedEntity.handle.SetRotation(rotSelected);
								break;
							case eSpoonerModeMode::Precision:
								selectedEntity.handle.SetRotation(rotFreeCam);
								break;
							}
							rotSelected = selectedEntity.handle.Rotation_get(); // To get -180 to 180 values

							const ModelDimensions& mdSelectedEntity = selectedEntity.handle.ModelDimensions();
							switch (spoonerModeMode)
							{
							case eSpoonerModeMode::GroundEase:
							{
								//Vector3& geSep = selectedEntity.handle.Position_get();
								//auto& geGroundRay = RaycastResult::Raycast(geSep, Vector3::WorldDown(), max(max(mdSelectedEntity.Dim1.x, mdSelectedEntity.Dim2.x), max(max(mdSelectedEntity.Dim1.y, mdSelectedEntity.Dim2.y), max(mdSelectedEntity.Dim1.z, mdSelectedEntity.Dim2.z))) + 2.0f, IntersectOptions::Everything, selectedEntity.handle);
								float geGroundZ = mdSelectedEntity.Dim1.z;
								//if (geGroundRay.DidHitAnything())
								//{
								//geGroundZ = geGroundRay.HitCoords().DistanceTo(geSep);
								//}
								if (abs(rotSelected.x) > 150.0f || abs(rotSelected.y) > 150.0f)
									geGroundZ = mdSelectedEntity.Dim2.z;
								else if (abs(rotSelected.x) > 70.0f && abs(rotSelected.y) > 70.0f)
									geGroundZ = (mdSelectedEntity.Dim1.y + mdSelectedEntity.Dim1.x) / 2;
								else if (abs(rotSelected.x) > 70.0f)
									geGroundZ = mdSelectedEntity.Dim1.y;
								else if (abs(rotSelected.y) > 70.0f)
									geGroundZ = mdSelectedEntity.Dim1.x;
								selectedEntity.handle.SetPosition(SnapPos(spoonerModeCamera.RaycastForCoord(Vector2(0.0f, 0.0f), selectedEntity.handle, 90.0f, 15.0f + mdSelectedEntity.Dim2.y) + Vector3(0, 0, geGroundZ)));
								break;
							}
							case eSpoonerModeMode::Precision:
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
									freeCamCamDistance -= 0.23f; // Zoom in RS
								if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
									freeCamCamDistance += 0.23f; // Zoom out LS
								Vector3 attachmentOffset = { 0.0f, -mdSelectedEntity.Dim2.y - freeCamCamDistance, 0.0f };
								freeCam.AttachTo(selectedEntity.handle, attachmentOffset);
								selectedEntity.handle.SetPosition(SnapPos(selectedEntity.handle.GetOffsetInWorldCoords(nextOffset)));
								if (Settings::bFreezeEntityWhenMovingIt)
									selectedEntity.handle.FreezePosition(Settings::bFreezeEntityWhenMovingIt);
								break;
							}

							if (Menu::activeSubmenu == SUB::CLOSED)
							{
								Menu::add_IB(INPUT_CURSOR_CANCEL, "Open property menu");
								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									Menu::add_IB(INPUT_CURSOR_SCROLL_DOWN, "");
									Menu::add_IB(INPUT_CURSOR_SCROLL_UP, "Adjust pitch rotation");
									break;
								case eSpoonerModeMode::Precision:
									Menu::add_IB(INPUT_CURSOR_SCROLL_DOWN, "");
									Menu::add_IB(INPUT_CURSOR_SCROLL_UP, "Zoom camera in/out");
									Menu::add_IB(VirtualKey::Z, "");
									Menu::add_IB(VirtualKey::X, "Ascend/Descend");
									break;
								}
								Menu::add_IB(INPUT_PARACHUTE_BRAKE_RIGHT, "");
								Menu::add_IB(INPUT_PARACHUTE_BRAKE_LEFT, "Adjust roll rotation");
								Menu::add_IB(INPUT_LOOK_BEHIND, "Copy (and add to DB)");
								Menu::add_IB(INPUT_CREATOR_DELETE, "Delete");
								if (!isInDb)
								{
									Menu::add_IB(INPUT_FRONTEND_UP, "Add to Database");
									if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_UP))
									{
										EntityManagement::AddEntityToDb(selectedEntity);
									}
								}

								if (IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_LOOK_BEHIND))
								{
									const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(selectedEntity, isInDb, true, Submenus::_copyEntTexterValue);
									//EntityManagement::AddEntityToDb(copiedEntity);
									selectedEntity = copiedEntity;
								}
								if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_CREATOR_DELETE))
								{
									selectedEntity.handle.RequestControl(600);
									EntityManagement::DeleteEntity(selectedEntity);
									bIsSomethingHeld = false;
								}
							}
						}
						else
						{
							if (bIsSomethingHeld)
							{
								switch (spoonerModeMode)
								{
								case eSpoonerModeMode::GroundEase:
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), bHeldEntityHasCollision, true);
									freeCam.Detach(); // Just in case
									break;
								case eSpoonerModeMode::Precision:
									SET_ENTITY_COLLISION(selectedEntity.handle.Handle(), bHeldEntityHasCollision, true);
									freeCam.Detach();
									break;
								}
							}
							bIsSomethingHeld = false;

							if (Menu::activeSubmenu == SUB::CLOSED)
							{
								Menu::add_IB(INPUT_CURSOR_CANCEL, "Open property menu");
								Menu::add_IB(INPUT_CURSOR_ACCEPT, "Move entity around (hold)");
								Menu::add_IB(INPUT_LOOK_BEHIND, "Copy (and add to DB)");
								Menu::add_IB(INPUT_CREATOR_DELETE, "Delete");
								if (!isInDb)
								{
									Menu::add_IB(INPUT_FRONTEND_UP, "Add to Database");
									if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_UP))
									{
										EntityManagement::AddEntityToDb(GetEntityPtrValue(currentEnt));
									}
								}

								if (IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_LOOK_BEHIND))
								{
									const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(GetEntityPtrValue(currentEnt), isInDb, true, Submenus::_copyEntTexterValue);
									//EntityManagement::AddEntityToDb(copiedEntity);
									selectedEntity = copiedEntity;
								}
								else if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_CREATOR_DELETE))
								{
									auto entPtrVal = GetEntityPtrValue(currentEnt);
									entPtrVal.handle.RequestControl(600);
									EntityManagement::DeleteEntity(entPtrVal);
									bIsSomethingHeld = false;
								}
							}
						}
						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_CURSOR_CANCEL))
						{
							if (!bIsSomethingHeld)
							{
								SpoonerMode::SetAsSelectedEntity(currentEnt);
							}
							memset(Menu::submenuHistory, 0, sizeof(Menu::submenuHistory));
							memset(Menu::optionSelectionHistory, 0, sizeof(Menu::optionSelectionHistory));
							Menu::submenuHistory[0] = SUB::MAINMENU;
							Menu::optionSelectionHistory[0] = 1;
							Menu::menuHistoryIndex = 0;
							Menu::NewSetMenu(SUB::SPOONER_SELECTEDENTITYOPS);
						}
					}
					// does not draw the cursor when inside gizmo entity editing mode.
					else if (editingState.mode != eEditMode::Gizmo)
					{
						DRAW_RECT(0.5f, 0.5f, 0.02f, 0.002f, 255, 255, 255, 255, false);
						DRAW_RECT(0.5f, 0.5f, 0.001f, 0.03f, 255, 255, 255, 255, false);
					}
				}
				if (!bIsSomethingHeld && myPed.Exists() && freeCam.Exists())
				{
					float distToPlayer = myPed.GetPosition().DistanceTo(freeCam.GetPosition());
					if (distToPlayer > 350.0f)
					{
						Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true, RGBA(255, 200, 0, 255));
						Game::Print::DrawString(oss_ << "WARNING: Your camera is too far from the player. You might experience texture loss or the environment might look low quality.", 0.5f, 0.72f);
						Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true, RGBA(255, 200, 0, 255));
						Game::Print::DrawString(oss_ << "This is expected - use the Freecam (available in \"Misc Options\" menu) to move around the map freely.", 0.5f, 0.75f);
					}
				}
			}
			else
			{
				if (freeCam.Handle() != 0)
				{
					myPlayer.SetControl(true, 0);

					bIsSomethingHeld = false;
					bHeldEntityHasCollision = true;

					freeCam.SetActive(false);
					freeCam.Destroy();
					World::SetRenderingCamera(0);
					freeCam = Camera();
				}
			}
		}

		void Tick()
		{
			if (SpoonerMode::IsHotkeyPressed())
				SpoonerMode::Toggle();

			sub::Spooner::ImGuiSpooner::Tick();

			UpdatePreviewRotation();
			CamTick();

			if (Settings::bShowBoxAroundSelectedEntity)
				EntityManagement::ShowBoxAroundEntity(selectedEntity.handle);

			// Snapping grid; visible only when using Spooner Camera
			if (Settings::bDrawGrid && Settings::bGridSnapEnabled && SpoonerMode::bEnabled && selectedEntity.handle.Exists())
				DrawSnappingGrid();

			for (auto& ent : Databases::EntityDb)
			{
				if (ent.handle.Exists())
					ent.taskSequence.Tick(reinterpret_cast<void*>(&ent)); //ent.taskSequence.Tick((GTAped)ent.handle);
			}

			if (!Databases::MarkerDb.empty())
				MarkerManagement::DrawAll();

			if (!Databases::LightDb.empty())
				LightManagement::DrawAll();

			auto applyScaleTick = [](const Submenus::EntityScaleState& s)
			{
				if (s.handle == 0) return;
				GTAentity ent(s.handle);
				ent.SetScale(s.scale);
			};
			applyScaleTick(Submenus::_vehScale);
			applyScaleTick(Submenus::_pedScale);
			applyScaleTick(Submenus::_objScale);
		}

		void TurnOn()
		{
			if (!g_menuNotOpenedYet)
			{
				SpoonerMode::bEnabled = true;
				sub::Spooner::ImGuiSpooner::SetVisible(true);
				if (Menu::activeSubmenu != SUB::CLOSED)
					Game::Print::PrintBottomLeft("~b~Note:~s~ Spooner Mode instructions only appear when Menyoo is closed.");
			}
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Menu not opened yet.");
			}
		}
		void TurnOff()
		{
			SpoonerMode::bEnabled = false;
			sub::Spooner::ImGuiSpooner::SetVisible(false);
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
			if (IsKeyJustUp(VirtualKey::OEMPlus) && GetTickCount() - lastSensitivityChange > 200)
			{
				if (precision < 10.0f) precision *= 10;
				lastSensitivityChange = GetTickCount();
				Game::Print::PrintBottomCentre("Sensitivity: ~b~" + std::to_string(precision), 3000);
			}
			if (IsKeyJustUp(VirtualKey::OEMMinus) && GetTickCount() - lastSensitivityChange > 200)
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
			constexpr float HUD_LINE_HEIGHT = 0.025f;
			const Vector2 HUD_FONT_SIZE(0.35f, 0.35f);
			constexpr float hudX = 0.02f;
			float hudY = 0.8f;

			auto drawText = [&](const std::string& text, RGBA colour = {255, 255, 255, 255})
			{
				Game::Print::SetupDraw(GTAfont::Arial, HUD_FONT_SIZE, false, false, true, colour);
				Game::Print::drawstring(text, hudX, hudY);
				hudY += HUD_LINE_HEIGHT;
			};

			if (!bEnabled)
			{
				drawText("~r~Entity manipulation requires the Spooner Camera.");
				drawText("~b~Press F9:~w~ Enable Spooner Mode.");
				return;
			}

			if (editingState.mode == eEditMode::Disabled)
			{
				drawText("~r~Entity manipulation DISABLED.");
				drawText("~b~Press B:~w~ Enable keyboard controls or gizmo editing mode.");
			}
			else if (editingState.mode == eEditMode::Keyboard)
			{
				if (editingState.transformMode == eTransformMode::Rotation)
				{
					drawText("~y~Rotation Mode:");
					drawText("~b~W/S: ~w~Pitch+ / Pitch-");
					drawText("~b~A/D: ~w~Yaw+ / Yaw-");
					drawText("~b~E/Q: ~w~Roll+ / Roll-");
					drawText("~b~=/-: ~w~+/- Sensitivity");
					drawText("~b~R: ~w~Edit position");
				}
				else
				{
					drawText("~y~Position Mode:");
					drawText("~b~W/S: ~w~X+ / X-");
					drawText("~b~A/D: ~w~Y+ / Y-");
					drawText("~b~E/Q: ~w~Z+ / Z-");
					drawText("~b~=/-: ~w~+/- Sensitivity");
					drawText("~b~R: ~w~Edit rotation");
				}
				drawText("~b~ALT: ~w~Copy entity");
				drawText("~b~B: ~w~Switch to gizmo / disable controls.");
			}
			else if (editingState.mode == eEditMode::Gizmo)
			{
				std::string modeName;
				switch (editingState.transformMode)
				{
					case eTransformMode::Rotation: modeName = "Rotation"; break;
					case eTransformMode::Scale:    modeName = "Scale";    break;
					default:                             modeName = "Position"; break;
				}
				drawText("~y~Gizmo Mode ~s~(" + modeName + " Mode):");
				drawText("~b~Left Click:~w~ Grab axis handle");
				drawText("~b~R:~w~ Cycle mode");
				drawText(editingState.cameraLocked ? "~b~C:~w~ Unlock camera" : "~b~C:~w~ Lock camera");
				drawText(editingState.localSpace ? "~b~L:~w~ Edit in world space" : "~b~L:~w~ Edit in local space");
				drawText("~b~ALT:~w~ Copy entity");
				drawText("~b~B:~w~ Disable gizmo mode");
			}
		}

		void UpdateEntityEditingState(Vector3& position, Vector3& rotation)
		{
			// toggling between Disabled / Keyboard / Gizmo modes
			static bool lastBToggle = false;
			bool currentBToggle = IsKeyJustUp(VirtualKey::B);
			if (currentBToggle && !lastBToggle)
			{
				switch (editingState.mode)
				{
					case eEditMode::Disabled:
						editingState.mode = eEditMode::Keyboard;
						break;
					case eEditMode::Keyboard:
						editingState.mode = eEditMode::Gizmo;
						break;
					case eEditMode::Gizmo:
						editingState.mode = eEditMode::Disabled;
						break;
					}
				editingState.cameraLocked = false;
			}
			lastBToggle = currentBToggle;

			// toggling between transform modes
			static bool lastRToggle = false;
			bool currentRToggle = IsKeyJustUp(VirtualKey::R);
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
			if (editingState.mode != eEditMode::Disabled && IsKeyJustUp(VirtualKey::C))
			{
				editingState.cameraLocked = !editingState.cameraLocked;
			}

			// toggling world / local space editing
			if (editingState.mode != eEditMode::Disabled && IsKeyJustUp(VirtualKey::L))
			{
				editingState.localSpace = !editingState.localSpace;
			}

			// make a quick copy of an entity by clicking ALT in editing modes
			if (editingState.mode != eEditMode::Disabled && IsKeyJustUp(VirtualKey::Menu))
			{
				if (selectedEntity.handle.Exists())
				{
					const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(selectedEntity, EntityManagement::GetEntityIndexInDb(selectedEntity) >= 0, true, Submenus::_copyEntTexterValue);
					selectedEntity = copiedEntity;
					Game::Print::PrintBottomCentre("Entity copied.", 2500);
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
