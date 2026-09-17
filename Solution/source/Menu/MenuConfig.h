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

#include <simpleini\SimpleIni.h>

namespace MenuConfig
{
	extern CSimpleIniA iniFile;
	extern bool bSaveAtIntervals;
	extern bool bShowNotificationBackground;

// Camera configuration parameters
namespace FreeCam {
	extern float defaultSpeed; // Default movement speed
	extern float defaultFov; // Default FOV value
	extern float defaultSlowSpeed; // Movement speed while Left Ctrl is held
	extern float rotationSensitivityMouse; // Mouse look sensitivity
	extern float rotationSensitivityGamepad; // Gamepad look sensitivity
	extern float speedAdjustStep; // Speed ​​adjustment step
	extern float fovAdjustStep; // FOV adjustment step
	extern float minSpeed; // Minimum speed
	extern float maxSpeed; // Maximum speed
	extern float minFov; // Minimum FOV
	extern float maxFov; // Maximum FOV

	namespace Defaults {
		constexpr float speed = 0.5f;
		constexpr float fov = 50.0f;
		constexpr float slowSpeed = 0.2f;
		constexpr float rotationSensitivityMouse = 7.0f;
		constexpr float rotationSensitivityGamepad = 2.5f;
	}

	void ResetToDefaults();
    }

	void ConfigInit();
	void ConfigRead();
	void SaveConfig();
	// Debounced save: RequestSave marks the config dirty, FlushPendingSave writes it once changes settle (or immediately when forced)
	void RequestSave();
	void FlushPendingSave(bool force = false);
	void ConfigResetHaxValues();
}



