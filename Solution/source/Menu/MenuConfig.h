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

// Camera configuration parameters
namespace FreeCam {
	extern float defaultSpeed; // Default movement speed
	extern float defaultFov; // Default FOV value
	extern float defaultSlowSpeed; // Speed ​​multiplier when right mouse button is held
	extern float speedAdjustStep; // Speed ​​adjustment step
	extern float fovAdjustStep; // FOV adjustment step
	extern float minSpeed; // Minimum speed
	extern float maxSpeed; // Maximum speed
	extern float minFov; // Minimum FOV
	extern float maxFov; // Maximum FOV
    }

	void ConfigInit();
	void ConfigRead();
	void ConfigSave();
	void ConfigResetHaxValues();
}



