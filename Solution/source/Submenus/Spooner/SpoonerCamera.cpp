#include "SpoonerCamera.h"

#include "SpoonerMode.h"
#include "SpoonerSettings.h"
#include "MarkerManagement.h"
#include "SpoonerMarker.h"
#include "Submenus.h"
#include "..\PedComponentChanger.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\MenuConfig.h"
#include "..\..\Misc\FreeCam.h"
#include "..\..\Natives\natives2.h"
#include "..\..\Util\keyboard.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\GTAped.h"
#include "..\..\Scripting\GTAplayer.h"
#include "..\..\Scripting\World.h"
#include "..\..\Natives\types.h"
#include "..\..\macros.h"

#include <algorithm>

namespace sub::Spooner::SpoonerCamera
{
	Camera camera;

	struct Input
	{
		Vector3 translation{};
		Vector3 rotation{};
	};

	static void Stop(GTAplayer& player)
	{
		if (!camera.Exists()) return;

		player.SetControl(true, 0);
		camera.SetActive(false);
		camera.Destroy();
		World::SetRenderingCamera(0);
		camera = Camera();
		MenuConfig::FlushPendingSave(true);
	}

	static void EnsureActive(const GTAped& playerPed)
	{
		if (!camera.Exists())
		{
			const Vector3 playerPosition = playerPed.GetPosition();
			camera = World::CreateCamera(
				playerPosition + Vector3(0, 0, 2.8f),
				Vector3(0, 0, playerPed.GetRotation().z),
				MenuConfig::FreeCam::defaultFov);
			camera.SetActive(false);
		}

		if (camera.IsActive()) return;

		camera.SetActive(true);
		Camera::RenderScriptCams(true);
	}

	static void DisablePlayerControls(GTAplayer& player)
	{
		player.SetControl(false, 0);
		DISABLE_ALL_CONTROL_ACTIONS(0);
		DISABLE_ALL_CONTROL_ACTIONS(2);
		ENABLE_CONTROL_ACTION(2, INPUT_FRONTEND_PAUSE, true);
		ENABLE_CONTROL_ACTION(2, INPUT_FRONTEND_PAUSE_ALTERNATE, true);
	}

	static Input ReadControllerInput()
	{
		Input input;
		// movement and look speeds are shared with FreeCam
		const float movementSensitivity = MenuConfig::FreeCam::defaultSpeed * FreeCamMode::controllerSpeedScale;
		const float rotationSensitivity = MenuConfig::FreeCam::rotationSensitivityGamepad;

		input.translation.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movementSensitivity;
		input.translation.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movementSensitivity;
		input.rotation.x = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD) * rotationSensitivity;
		input.rotation.z = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR) * rotationSensitivity;
		input.rotation.y = -camera.GetRotation().y;
		return input;
	}

	static Input ReadKeyboardInput()
	{
		Input input;
		const SpoonerMode::EditingState& editingState = SpoonerMode::editingState;

		if (!editingState.BlocksCameraTranslation())
		{
			// movement speeds and the Left Ctrl slow-down are shared with FreeCam
			float movementSensitivity = IS_DISABLED_CONTROL_PRESSED(0, INPUT_DUCK)
				? MenuConfig::FreeCam::defaultSlowSpeed
				: MenuConfig::FreeCam::defaultSpeed;
			if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_SPRINT))
				movementSensitivity *= FreeCamMode::keyboardSprintMultiplier;

			input.translation.x = GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_LR) * movementSensitivity;
			input.translation.y = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_MOVE_UD) * movementSensitivity;
			input.translation.z = IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_RIGHT) ? movementSensitivity
				: IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_LEFT) ? -movementSensitivity
				: 0.0f;
		}

		if (!editingState.BlocksCameraRotation())
		{
			const float rotationSensitivity = MenuConfig::FreeCam::rotationSensitivityMouse;
			input.rotation.x = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD) * rotationSensitivity;
			input.rotation.z = -GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR) * rotationSensitivity;
		}
		input.rotation.y = -camera.GetRotation().y;
		return input;
	}

	// Returns -1 / 0 / +1 for scroll down / none / up
	static int ReadScroll()
	{
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP)) return 1;
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN)) return -1;
		return 0;
	}

	static void AdjustSpeed(int scroll)
	{
		const float currentSpeed = MenuConfig::FreeCam::defaultSpeed;
		const float newSpeed = std::clamp(currentSpeed + scroll * MenuConfig::FreeCam::speedAdjustStep,
			MenuConfig::FreeCam::minSpeed, MenuConfig::FreeCam::maxSpeed);
		if (newSpeed == currentSpeed) return;

		MenuConfig::FreeCam::defaultSpeed = newSpeed;
		MenuConfig::RequestSave();
		Game::Print::ShowNotification(oss_ << "Spooner Camera Speed: " << newSpeed, 1.0f);
	}

	static void AdjustFov(int scroll)
	{
		const float currentFov = camera.GetFieldOfView();
		const float newFov = std::clamp(currentFov + scroll * MenuConfig::FreeCam::fovAdjustStep,
			MenuConfig::FreeCam::minFov, MenuConfig::FreeCam::maxFov);
		if (newFov == currentFov) return;

		camera.SetFieldOfView(newFov);
		MenuConfig::FreeCam::defaultFov = newFov;
		MenuConfig::RequestSave();
		Game::Print::ShowNotification(oss_ << "Spooner Camera FOV: " << newFov, 1.0f);
	}

	static void HandleScrollAdjustments()
	{
		const int scroll = ReadScroll();
		if (scroll == 0) return;

		// Space + scroll: FOV (keyboard only)
		if (!Menu::usingControllerInput && IsKeyDown(VK_SPACE))
		{
			AdjustFov(scroll);
			return;
		}

		AdjustSpeed(scroll);
	}

	static void ApplyInput(const Input& input)
	{
		if (!input.translation.IsZero())
			camera.SetPosition(camera.GetOffsetInWorldCoords(input.translation));

		if (!input.rotation.IsZero())
			camera.SetRotation(camera.GetRotation() + input.rotation);
	}

	static void AddMarkerAtAim()
	{
		const Vector3 position = camera.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 160.0f, 3.0f);
		auto marker = MarkerManagement::AddMarker(position, Vector3(0, 0, camera.GetRotation().z));
		if (!marker) return;

		marker->m_position.z += marker->m_scale / 2.0f;
		SelectedMarker = marker;
		SpoonerMode::OpenMenu(SUB::SPOONER_MANAGEMARKERS_INMARKER);
	}

	static void HandleShortcuts()
	{
		if (Menu::activeSubmenu != SUB::CLOSED) return;

		Menu::add_IB(INPUT_VEH_EXIT, "Open main menu");
		if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_VEH_EXIT))
			SpoonerMode::OpenMenu(SUB::SPOONER_MAIN, 2);

		if (Menu::usingControllerInput)
		{
			Menu::add_IB(INPUT_FRONTEND_DOWN, "Place Marker");
			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_DOWN))
				AddMarkerAtAim();
			return;
		}

		Menu::add_IB(VirtualKey::M, "Place Marker");
		if (IsKeyJustUp(VirtualKey::M))
			AddMarkerAtAim();
	}

	static void DrawDistanceWarning(const GTAped& playerPed)
	{
		if (!playerPed.Exists() || !camera.Exists()) return;
		if (playerPed.GetPosition().DistanceTo(camera.GetPosition()) <= 350.0f) return;

		const RGBA warningColour(255, 200, 0, 255);
		Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true, warningColour);
		Game::Print::DrawString(oss_ << "WARNING: Your camera is too far from the player. You might experience texture loss or the environment might look low quality.", 0.5f, 0.72f);
		Game::Print::SetupDraw(GTAfont::Arial, Vector2(0.35f, 0.35f), true, false, true, warningColour);
		Game::Print::DrawString(oss_ << "This is expected - use the Freecam (available in \"Misc Options > FreeCam Settings\" menu) to move around the map freely.", 0.5f, 0.75f);
	}

	void Tick()
	{
		GTAplayer player = Game::Player();
		GTAped playerPed = Game::PlayerPed();

		if (!SpoonerMode::bEnabled)
		{
			Stop(player);
			return;
		}
		if (IS_PAUSE_MENU_ACTIVE()) return;

		HIDE_HUD_AND_RADAR_THIS_FRAME();

		// wardrobe front view owns the view (or is easing back to it); don't take it back or move the Spooner camera
		if (WardrobeCamera::IsBusy())
		{
			DisablePlayerControls(player);
			return;
		}

		EnsureActive(playerPed);
		DisablePlayerControls(player);

		const Input input = Menu::usingControllerInput
			? ReadControllerInput()
			: ReadKeyboardInput();

		HandleScrollAdjustments();
		ApplyInput(input);
		HandleShortcuts();
		DrawDistanceWarning(playerPed);
		MenuConfig::FlushPendingSave();
	}
}
