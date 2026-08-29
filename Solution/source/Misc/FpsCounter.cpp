/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "FpsCounter.h"

#include "..\macros.h"

#include "..\Scripting\enums.h"
#include "..\Scripting\Game.h"
#include "..\Util\GTAmath.h"

#include <Windows.h> // GetTickCount

namespace FPSCounter
{
	FpsCounter::FpsCounter()
		: fpsValue(0), frameCounter(0), timer(GetTickCount())
	{
	}

	DWORD FpsCounter::Get()
	{
		this->Tick();
		return fpsValue;
	}

	void FpsCounter::Tick()
	{
		frameCounter++;

		if (GetTickCount() - timer > 500) //0.5sec
		{
			// store frame count to fpsValue
			fpsValue = frameCounter * 2;

			// reset
			frameCounter = 0;
			timer = GetTickCount();
		}
	}

	FpsCounter g_fpsCounter;

	bool bDisplayFps = false;
}