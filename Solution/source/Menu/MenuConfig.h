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

    // 相机配置参数 
    namespace FreeCam {
        extern float defaultSpeed; // 默认移动速度
        extern float defaultFov;   // 默认FOV值
        extern float defaultSlowSpeed; // Speed multiplier when right mouse button is held
        extern float speedAdjustStep; // 速度调节步进
        extern float fovAdjustStep;   // FOV调节步进
        extern float minSpeed;     // 最小速度
        extern float maxSpeed;     // 最大速度 
        extern float minFov;      // 最小FOV
        extern float maxFov;      // 最大FOV
    }

	void ConfigInit();
	void ConfigRead();
	void ConfigSave();
	void ConfigResetHaxValues();
}



