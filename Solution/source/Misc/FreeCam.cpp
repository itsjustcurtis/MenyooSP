/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "FreeCam.h"

#include "..\macros.h"

#include "..\Menu\Menu.h"
#include "..\Menu\MenuConfig.h"
#include "..\Menu\Routine.h"
#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Util\keyboard.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\CustomHelpText.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GameplayCamera.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\World.h"
#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\Spooner\SpoonerMode.h"

#include <algorithm>

namespace FreeCamMode
{
	bool bEnabled = false;

	namespace
	{
		constexpr float controllerHastenMultiplier = 2.25f;

		struct EntityState
		{
			GTAentity entity;
			bool wasVisible = true;
			bool hadCollision = true;
			bool wasFrozen = false;
		};

		struct Input
		{
			Vector3 translation{}; // camera-local x/y/z
			Vector3 rotation{};    // pitch/yaw delta
		};

		struct State
		{
			bool active = false;
			Camera camera;

			bool heightLocked = false;
			float lockedHeight = 0.0f;

			EntityState ped;      // player ped, captured when flying starts or the ped changes
			EntityState vehicle;  // vehicle being flown, if any
		};

		State state;

		// ── Controlled entity ─────────────────────────────────────────

		EntityState Capture(GTAentity entity)
		{
			EntityState captured;
			captured.entity = entity;
			captured.wasVisible = entity.IsVisible();
			captured.hadCollision = entity.GetIsCollisionEnabled();
			captured.wasFrozen = entity.IsPositionFrozen();
			return captured;
		}

		void Restore(EntityState& saved, bool restoreVisibility = true)
		{
			GTAentity& entity = saved.entity;
			if (entity.Handle() == 0) return;

			if (entity.Exists())
			{
				entity.RequestControl();
				if (restoreVisibility)
					entity.SetVisible(saved.wasVisible);
				entity.SetIsCollisionEnabled(saved.hadCollision);
				entity.FreezePosition(saved.wasFrozen);
			}
		}

		GTAentity GetControlledEntity()
		{
			return state.vehicle.entity.Handle() != 0 ? state.vehicle.entity : state.ped.entity;
		}

		// Keep the saved state in sync with what we're flying (vehicle enter/exit, player model change)
		void SyncControlledEntity()
		{
			const GTAped playerPed = PLAYER_PED_ID();
			const int vehicleHandle = IS_PED_IN_ANY_VEHICLE(playerPed.GetHandle(), false)
				? GET_VEHICLE_PED_IS_IN(playerPed.GetHandle(), false)
				: 0;

			const GTAentity previous = GetControlledEntity();

			if (state.ped.entity.Handle() != playerPed.GetHandle())
			{
				Restore(state.ped);
				state.ped = Capture(playerPed);
			}

			if (state.vehicle.entity.Handle() != vehicleHandle)
			{
				Restore(state.vehicle);
				state.vehicle = vehicleHandle != 0 ? Capture(GTAentity(vehicleHandle)) : EntityState{};
				// flying the vehicle now: give the ped its collision/freeze back, but keep it hidden
				if (vehicleHandle != 0)
					Restore(state.ped, false);
			}

			GTAentity controlled = GetControlledEntity();
			if (controlled != previous && state.camera.Exists() && controlled.Exists())
				state.camera.AttachTo(controlled, Vector3());
		}

		void ApplyEntityOverrides()
		{
			GTAentity controlled = GetControlledEntity();
			if (!controlled.Exists()) return;

			controlled.RequestControl();
			controlled.FreezePosition(true);
			controlled.SetIsCollisionEnabled(false);
			controlled.SetVisible(false);

			GTAentity ped = state.ped.entity;
			if (ped.Exists())
				ped.SetVisible(false);
		}

		// ── Controls ───────────────────────────────────────────────────

		bool IsHotkeyPressed()
		{
			return Menu::usingControllerInput
				? IS_CONTROL_PRESSED(2, INPUT_FRONTEND_X) && IS_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LS)
				: IsKeyJustUp(BindNoClip);
		}

		void SetConflictingControls(bool enabled)
		{
			static constexpr ControllerInput controls[] = {
				INPUT_VEH_HORN, INPUT_LOOK_BEHIND, INPUT_VEH_LOOK_BEHIND, INPUT_SELECT_WEAPON,
				INPUT_VEH_ACCELERATE, INPUT_VEH_BRAKE, INPUT_VEH_RADIO_WHEEL,
			};
			for (const ControllerInput control : controls)
			{
				if (enabled)
					ENABLE_CONTROL_ACTION(2, control, TRUE);
				else
					DISABLE_CONTROL_ACTION(2, control, TRUE);
			}
			if (!enabled && Menu::usingControllerInput)
				DISABLE_CONTROL_ACTION(0, INPUT_VEH_HORN, TRUE);
			// Left Ctrl slows the camera down; don't let it also toggle the ped's stealth stance
			if (!enabled && !Menu::usingControllerInput)
				DISABLE_CONTROL_ACTION(0, INPUT_DUCK, TRUE);
		}

		Input ReadRotationInput(float sensitivity)
		{
			Input input;
			input.rotation.x = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD) * sensitivity;
			input.rotation.z = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR) * sensitivity;
			return input;
		}

		Input ReadControllerInput()
		{
			Input input = ReadRotationInput(MenuConfig::FreeCam::rotationSensitivityGamepad);

			float movement = MenuConfig::FreeCam::defaultSpeed * controllerSpeedScale;
			if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RB))
				movement *= controllerHastenMultiplier;

			input.translation.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movement;
			input.translation.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movement;
			input.translation.z = (GET_DISABLED_CONTROL_NORMAL(2, INPUT_FRONTEND_RT) - GET_DISABLED_CONTROL_NORMAL(2, INPUT_FRONTEND_LT)) * movement;
			return input;
		}

		Input ReadKeyboardInput()
		{
			Input input = ReadRotationInput(MenuConfig::FreeCam::rotationSensitivityMouse);

			float movement = IS_DISABLED_CONTROL_PRESSED(0, INPUT_DUCK)
				? MenuConfig::FreeCam::defaultSlowSpeed
				: MenuConfig::FreeCam::defaultSpeed;
			if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_SPRINT))
				movement *= keyboardSprintMultiplier;

			input.translation.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movement;
			input.translation.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movement;
			input.translation.z = IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_RIGHT) ? movement
				: IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_LEFT) ? -movement
				: 0.0f;
			return input;
		}

		// ── Movement ───────────────────────────────────────────────────

		void ResetControllerPedStance()
		{
			const GTAped ped = state.ped.entity;
			if (GetControlledEntity() != state.ped.entity || !ped.Exists()) return;

			if (GET_PED_STEALTH_MOVEMENT(ped.GetHandle()))
				SET_PED_STEALTH_MOVEMENT(ped.GetHandle(), false, 0);
			if (GET_PED_COMBAT_MOVEMENT(ped.GetHandle()))
				SET_PED_COMBAT_MOVEMENT(ped.GetHandle(), 0);
		}

		void ApplyRotation(const Input& input)
		{
			Camera& cam = state.camera;
			GTAentity controlled = GetControlledEntity();

			Vector3 nextRot = cam.GetRotation() + input.rotation;
			nextRot.y = 0.0f; // no roll
			controlled.SetRotation(Vector3(0.0f, 0.0f, nextRot.z));
			cam.SetRotation(nextRot);

			const GTAplayer player = PLAYER_ID();
			if (!player.IsFreeAiming() && !player.IsTargetingAnything())
				SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
		}

		void ApplyMovement(const Input& input)
		{
			GTAentity controlled = GetControlledEntity();

			if (state.heightLocked)
			{
				state.lockedHeight += input.translation.z;
				Vector3 newPos = state.camera.GetOffsetInWorldCoords(Vector3(input.translation.x, input.translation.y, 0.0f));
				newPos.z = state.lockedHeight;
				controlled.SetPosition(newPos);
				return;
			}

			if (!input.translation.IsZero())
				controlled.SetPosition(state.camera.GetOffsetInWorldCoords(input.translation));
		}

		// ── Keyboard adjustments (height lock, speed, FOV) ─────────────

		void ToggleHeightLock()
		{
			state.heightLocked = !state.heightLocked;
			if (state.heightLocked)
				state.lockedHeight = GetControlledEntity().GetPosition().z;
			Game::Print::ShowNotification(state.heightLocked ? "FreeCam Height Locked" : "FreeCam Height Unlocked", 1.0f);
		}

		// Returns -1 / 0 / +1 for scroll down / none / up
		int ReadScroll()
		{
			if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP)) return 1;
			if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN)) return -1;
			return 0;
		}

		void AdjustSpeed(int scroll)
		{
			const float currentSpeed = MenuConfig::FreeCam::defaultSpeed;
			const float newSpeed = std::clamp(currentSpeed + scroll * MenuConfig::FreeCam::speedAdjustStep,
				MenuConfig::FreeCam::minSpeed, MenuConfig::FreeCam::maxSpeed);
			if (newSpeed == currentSpeed) return;

			MenuConfig::FreeCam::defaultSpeed = newSpeed;
			MenuConfig::RequestSave();
			Game::Print::ShowNotification(oss_ << "FreeCam Speed: " << newSpeed, 1.0f);
		}

		void AdjustFov(int scroll)
		{
			const float currentFov = state.camera.GetFieldOfView();
			const float newFov = std::clamp(currentFov + scroll * MenuConfig::FreeCam::fovAdjustStep,
				MenuConfig::FreeCam::minFov, MenuConfig::FreeCam::maxFov);
			if (newFov == currentFov) return;

			state.camera.SetFieldOfView(newFov);
			MenuConfig::FreeCam::defaultFov = newFov;
			MenuConfig::RequestSave();
			Game::Print::ShowNotification(oss_ << "FreeCam FOV: " << newFov, 1.0f);
		}

		void HandleKeyboardAdjustments()
		{
			const int scroll = ReadScroll();

			// Space + scroll: FOV
			if (IsKeyDown(VK_SPACE))
			{
				if (scroll != 0)
					AdjustFov(scroll);
				return;
			}

			// Tab: height lock, scroll: speed
			if (IsKeyJustUp(VK_TAB))
				ToggleHeightLock();
			if (scroll != 0)
				AdjustSpeed(scroll);
		}

		void ShowHelp()
		{
			if (Menu::usingControllerInput)
			{
				Game::CustomHelpText::ShowTimedText(oss_ << "FreeCam:~n~~INPUT_MOVE_UD~ = " << Game::GetGXTEntry("ITEM_MOV_CAM")
					<< "~n~~INPUT_LOOK_LR~ = " << Game::GetGXTEntry("ITEM_MOVE") << "~n~~INPUT_FRONTEND_RT~/~INPUT_FRONTEND_LT~ = " << "Ascend/Descend" << "~n~~INPUT_FRONTEND_RB~ = " << "Hasten", 6000);
			}
			else
			{
				Game::CustomHelpText::ShowTimedText(oss_ << "FreeCam:~n~~INPUT_MOVE_UD~/~INPUT_MOVE_LR~ = " << Game::GetGXTEntry("ITEM_MOV_CAM")
					<< "~n~~INPUT_LOOK_LR~ = " << Game::GetGXTEntry("ITEM_MOVE") << "~n~~INPUT_PARACHUTE_BRAKE_RIGHT~/~INPUT_PARACHUTE_BRAKE_LEFT~ = " << "Ascend/Descend" << "~n~~INPUT_SPRINT~ = " << "Hasten" << "~n~~INPUT_DUCK~ = " << "Slow Down", 6000);
			}
		}
	}

	bool IsActive()
	{
		return state.active;
	}

	const Camera& GetCamera()
	{
		return state.camera;
	}

	void Start()
	{
		if (state.active || sub::Spooner::SpoonerMode::bEnabled) return;

		sub::WardrobeCamera::Disable(false);

		state = State{};
		state.active = true;
		SyncControlledEntity();

		GTAentity controlled = GetControlledEntity();
		if (!controlled.Exists())
		{
			Stop();
			return;
		}

		controlled.RequestControl();
		state.camera = World::CreateCamera();
		state.camera.SetPosition(GameplayCamera::GetPosition());
		state.camera.SetRotation(GameplayCamera::GetRotation());
		state.camera.AttachTo(controlled, Vector3());
		state.camera.SetFieldOfView(MenuConfig::FreeCam::defaultFov);
		state.camera.SetDepthOfFieldStrength(0.0f);
		World::SetRenderingCamera(state.camera);

		ShowHelp();
	}

	void Stop()
	{
		if (!state.active) return;
		state.active = false;

		sub::WardrobeCamera::Disable(false);

		Restore(state.vehicle);
		Restore(state.ped);
		SetConflictingControls(true);

		if (state.camera.Exists())
		{
			state.camera.SetActive(false);
			state.camera.Destroy();
			World::SetRenderingCamera(0);
		}

		MenuConfig::FlushPendingSave(true);
		state = State{};
	}

	void Toggle()
	{
		IsActive() ? Stop() : Start();
	}

	void Tick()
	{
		if (!bEnabled)
		{
			Stop();
			return;
		}

		// Spooner Mode owns the camera while it's on
		if (sub::Spooner::SpoonerMode::bEnabled)
		{
			Stop();
			return;
		}

		if (IsHotkeyPressed())
			Toggle();

		if (!state.active) return;

		SyncControlledEntity();
		if (!GetControlledEntity().Exists())
		{
			Stop();
			return;
		}

		SetConflictingControls(false);
		ApplyEntityOverrides();

		// wardrobe front view owns the view; don't move or rotate behind it
		if (!sub::WardrobeCamera::IsBusy())
		{
			Input input;
			if (Menu::usingControllerInput)
			{
				ResetControllerPedStance();
				input = ReadControllerInput();
			}
			else
			{
				HandleKeyboardAdjustments();
				input = ReadKeyboardInput();
			}

			ApplyRotation(input);
			ApplyMovement(input);
		}

		MenuConfig::FlushPendingSave();
	}
}
