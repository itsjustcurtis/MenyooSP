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

#include "..\macros.h"

#include "..\Menu\Menu.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\Game.h"
#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"

#include <Windows.h>
#include <string>
#include <vector>

typedef unsigned long DWORD;
typedef char *PCHAR;

namespace sub
{
	namespace CutscenePlayer
	{
		void PopulateCutsceneLabels();
		void EndCutscene();
		void CutsceneListMenu();
	}
}