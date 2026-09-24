#include "SpoonerCursor.h"

#include "SpoonerCamera.h"
#include "SpoonerMode.h"
#include "SpoonerSettings.h"
#include "SpoonerEntity.h"
#include "EntityManagement.h"
#include "Submenus.h"
#include "..\PedComponentChanger.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Keybinds.h"
#include "..\..\Natives\natives2.h"
#include "..\..\Natives\types.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\Model.h"
#include "..\..\Scripting\enums.h"

#include <algorithm>
#include <string>
#include <math.h>

namespace sub::Spooner::SpoonerCursor
{
	namespace
	{
		struct State
		{
			int hoveredEntityHandle = 0;
			std::string hoveredEntityName;
			bool dragging = false;
			int draggedEntityHandle = 0;
			bool draggedEntityHadCollision = true;
			float lastCameraYaw = 0.0f;
			float visibilityOpacity = 0.0f;
			float hoverOpacity = 0.0f;
			float textOpacity = 0.0f;
			DWORD lastUpdateTime = 0;
		};

			State state;

			constexpr float dotSize = 0.008f;
		constexpr float textY = 0.515f;
		constexpr float instructionY = 0.54f;
		const RGBA hoverColour(0, 255, 0, 255);
		const RGBA holdColour(255, 128, 0, 255);

		bool ShouldDraw()
		{
			return SpoonerMode::bEnabled &&
				!WardrobeCamera::IsBusy() &&
				SpoonerMode::editingState.mode == SpoonerMode::eEditMode::Disabled &&
				SpoonerCamera::camera.Exists();
		}

		GTAentity GetDraggedEntity()
		{
			return GTAentity(state.draggedEntityHandle);
		}

		GTAentity GetHoveredEntity()
		{
			return ShouldDraw()
				? SpoonerCamera::camera.RaycastForEntity(Vector2(0.0f, 0.0f), 0, 160.0f)
				: GTAentity();
		}

		GTAentity GetTargetEntity(const GTAentity& hoveredEntity)
		{
			return state.dragging ? GetDraggedEntity() : hoveredEntity;
		}

		std::string GetEntityName(GTAentity entity)
		{
			SpoonerEntity* entityInfo = nullptr;
			const bool isInDatabase = SpoonerMode::GetEntityPtr(entity, entityInfo);
			const std::string name = entityInfo != nullptr ? entityInfo->hashName : std::string();
			if (!isInDatabase)
				delete entityInfo;
			return name;
		}

		float AnimateOpacity(float current, float target, float deltaSeconds, float duration)
		{
			const float step = duration > 0.0f ? deltaSeconds / duration : 1.0f;
			return current < target
				? (std::min)(target, current + step)
				: (std::max)(target, current - step);
		}

		float WrapDegrees(float degrees)
		{
			degrees = fmod(degrees + 180.0f, 360.0f);
			return degrees < 0.0f ? degrees + 180.0f : degrees - 180.0f;
		}

		float ConsumeCameraYawDelta()
		{
			const float yaw = SpoonerCamera::camera.GetRotation().z;
			const float delta = WrapDegrees(yaw - state.lastCameraYaw);
			state.lastCameraYaw = yaw;
			return delta;
		}

		void ApplyHeldEntityYaw(GTAentity& entity, float yawDelta)
		{
			if (yawDelta == 0.0f) return;

			Vector3 rotation = entity.GetRotation();
			rotation.z += yawDelta;
			entity.SetRotation(rotation);
		}

		void StartTrackingDrag(GTAentity entity, bool hadCollision)
		{
			state.dragging = true;
			state.draggedEntityHandle = entity.Handle();
			state.draggedEntityHadCollision = hadCollision;
			entity.RequestControl();
			entity.SetIsCollisionEnabled(false);
		}

		void ClearDrag()
		{
			state.dragging = false;
			state.draggedEntityHandle = 0;
			state.draggedEntityHadCollision = true;
		}

		void RestoreDraggedEntityCollision()
		{
			GTAentity entity = GetDraggedEntity();
			if (!entity.Exists()) return;

			entity.RequestControl();
			entity.SetIsCollisionEnabled(state.draggedEntityHadCollision);
		}

		void BeginDrag(GTAentity entity)
		{
			if (state.dragging || !entity.Exists()) return;

			SpoonerMode::SetAsSelectedEntity(entity);
			state.lastCameraYaw = SpoonerCamera::camera.GetRotation().z;
			StartTrackingDrag(selectedEntity.handle, selectedEntity.handle.GetIsCollisionEnabled());
		}

		void RetargetDrag(GTAentity entity)
		{
			if (!state.dragging || !entity.Exists()) return;

			const bool hadCollision = state.draggedEntityHadCollision;
			RestoreDraggedEntityCollision();
			StartTrackingDrag(entity, hadCollision);
		}

		void UpdateDrag()
		{
			GTAentity entity = GetDraggedEntity();
			if (!entity.Exists()) return;

			entity.RequestControl();
			ApplyHeldEntityYaw(entity, ConsumeCameraYawDelta());

			const GTAmodel::ModelDimensions dimensions = entity.ModelDimensions();
			const float groundOffset = SpoonerMode::GetGroundOffset(dimensions, entity.GetRotation());
			const Vector3 position = SpoonerCamera::camera.RaycastForCoord(
				Vector2(0.0f, 0.0f), entity, 90.0f, 15.0f + dimensions.Dim2.y);
			entity.SetPosition(SpoonerMode::SnapPos(position + Vector3(0.0f, 0.0f, groundOffset)));
			if (Settings::bFreezeEntityWhenMovingIt)
				entity.FreezePosition(true);
		}

		void EndDrag()
		{
			if (!state.dragging) return;

			RestoreDraggedEntityCollision();
			ClearDrag();
		}

		void CopyTarget(GTAentity target, bool isInDb)
		{
			const SpoonerEntity copy = EntityManagement::CopyEntity(
				SpoonerMode::GetEntityPtrValue(target), isInDb, true, Submenus::_copyEntTexterValue);
			if (!copy.handle.Exists()) return;

			selectedEntity = copy;
			RetargetDrag(copy.handle);
		}

		void DeleteTarget(GTAentity target)
		{
			const bool isSelected = selectedEntity.handle == target;
			SpoonerEntity entity = SpoonerMode::GetEntityPtrValue(target);
			entity.handle.RequestControl(600);
			EntityManagement::DeleteEntity(entity);

			if (state.dragging)
				ClearDrag();
			if (isSelected)
				SpoonerMode::ResetSelectedEntity();
		}

		void AddTargetToDb(GTAentity target)
		{
			EntityManagement::AddEntityToDb(SpoonerMode::GetEntityPtrValue(target));
		}

		void AddInstructionalButtons(bool isInDb)
		{
			Menu::add_IB(INPUT_CURSOR_CANCEL, "Open property menu");
			if (!state.dragging)
				Menu::add_IB(INPUT_CURSOR_ACCEPT, "Move entity around (hold)");
			Keybinds::AddBindIB("spooner_edit_copy", "Copy (and add to DB)");
			Keybinds::AddBindIB("spooner_cursor_remove", "Delete");
			if (!isInDb)
				Keybinds::AddBindIB("spooner_cursor_add_to_db", "Add to Database");
		}

		void HandleShortcuts(GTAentity target)
		{
			if (Menu::activeSubmenu != SUB::CLOSED || !target.Exists()) return;

			const bool isInDb = EntityManagement::GetEntityIndexInDb(target) >= 0;
			AddInstructionalButtons(isInDb);

			if (Keybinds::WasPressedThisFrame("spooner_edit_copy"))
				CopyTarget(target, isInDb);
			else if (Keybinds::WasPressedThisFrame("spooner_cursor_remove"))
				DeleteTarget(target);
			else if (!isInDb && Keybinds::WasPressedThisFrame("spooner_cursor_add_to_db"))
				AddTargetToDb(target);
		}

		void HandleInput(const GTAentity& hoveredEntity)
		{
			if (!ShouldDraw())
			{
				EndDrag();
				return;
			}

			GTAentity target = GetTargetEntity(hoveredEntity);
			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_CURSOR_CANCEL) && target.Exists())
			{
				if (!state.dragging)
					SpoonerMode::SetAsSelectedEntity(target);
				SpoonerMode::OpenMenu(SUB::SPOONER_SELECTEDENTITYOPS);
				return;
			}

			if (!state.dragging && hoveredEntity.Exists() && IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
				BeginDrag(hoveredEntity);

			if (state.dragging)
			{
				if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_ACCEPT))
					UpdateDrag();
				else
					EndDrag();
			}

			HandleShortcuts(GetTargetEntity(hoveredEntity));
		}

		void UpdateState(DWORD now, const GTAentity& activeEntity)
		{
			const float deltaSeconds = state.lastUpdateTime == 0
				? 0.0f
				: static_cast<float>(now - state.lastUpdateTime) / 1000.0f;
			state.lastUpdateTime = now;

			const bool visible = ShouldDraw();
			const bool hasEntity = visible && activeEntity.Exists();
			state.visibilityOpacity = AnimateOpacity(state.visibilityOpacity, visible ? 1.0f : 0.0f, deltaSeconds, 0.20f);
			state.hoverOpacity = AnimateOpacity(state.hoverOpacity, hasEntity ? 1.0f : 0.0f, deltaSeconds, 0.16f);
			state.textOpacity = AnimateOpacity(state.textOpacity, hasEntity ? 1.0f : 0.0f, deltaSeconds, hasEntity ? 0.16f : 0.06f);

			const int activeHandle = hasEntity ? activeEntity.GetHandle() : 0;
			if (activeHandle != state.hoveredEntityHandle)
			{
				state.hoveredEntityHandle = activeHandle;
				state.hoveredEntityName = hasEntity ? GetEntityName(activeEntity) : std::string();
			}
		}

		void Draw()
		{
			if (state.visibilityOpacity <= 0.0f) return;
			if (!HAS_STREAMED_TEXTURE_DICT_LOADED("mpinventory"))
			{
				REQUEST_STREAMED_TEXTURE_DICT("mpinventory", false);
				return;
			}

			const RGBA& activeColour = state.dragging ? holdColour : hoverColour;
			const int whiteAlpha = static_cast<int>(255.0f * state.visibilityOpacity * (1.0f - state.hoverOpacity));
			const int activeAlpha = static_cast<int>(255.0f * state.visibilityOpacity * state.hoverOpacity);
			int screenWidth = 0, screenHeight = 0;
			GET_SCREEN_RESOLUTION(&screenWidth, &screenHeight);
			const float aspectRatio = screenHeight > 0 ? static_cast<float>(screenWidth) / screenHeight : 1.0f;
			const float dotWidth = dotSize / aspectRatio;
			if (whiteAlpha > 0)
				DRAW_SPRITE("mpinventory", "in_world_circle", 0.5f, 0.5f, dotWidth, dotSize, 0.0f, 255, 255, 255, whiteAlpha, false, 0);
			if (activeAlpha > 0)
				DRAW_SPRITE("mpinventory", "in_world_circle", 0.5f, 0.5f, dotWidth, dotSize, 0.0f, activeColour.R, activeColour.G, activeColour.B, activeAlpha, false, 0);

			if (state.hoveredEntityName.empty() || state.textOpacity <= 0.0f)
				return;

			const UINT8 textAlpha = static_cast<UINT8>(255.0f * state.visibilityOpacity * state.textOpacity);
			Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true, RGBA(255, 255, 255, textAlpha));
			Game::Print::drawstring(state.hoveredEntityName, 0.5f, textY);
			Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.22f, 0.22f), true, false, true, RGBA(195, 195, 195, static_cast<UINT8>(textAlpha * 0.75f)));
			Game::Print::drawstring("Right click to manage this entity", 0.5f, instructionY);
		}
	}

	void Tick()
	{
		const GTAentity hoveredEntity = state.dragging ? GetDraggedEntity() : GetHoveredEntity();
		HandleInput(hoveredEntity);
		UpdateState(GetTickCount(), GetTargetEntity(hoveredEntity));
		Draw();
	}

	void Reset()
	{
		EndDrag();
		state = State{};
	}

	bool IsDragging()
	{
		return state.dragging;
	}
}
