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
#include "..\Menu\Routine.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAprop.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\World.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\GTAblip.h"
#include "..\Util\GTAmath.h"
#include "..\Scripting\Raycast.h"
#include "..\Scripting\DxHookIMG.h"
#include "..\Util\ExePath.h"

#include "..\Misc\VehicleCruise.h"
#include "..\Misc\VehicleTow.h"
#include "..\Misc\VehicleFly.h"

#include <math.h>
#include <string>
#include <vector>

typedef unsigned __int8 UINT8;
typedef int INT, Entity, Ped, Vehicle, Object, ScrHandle;
typedef unsigned long DWORD, Hash;
typedef char *PCHAR;

class GTAvehicle;
class Vector2;

namespace sub
{
	
	void VehicleMenu();

	void PVMenu();

	namespace VehicleAutoDrive
	{
		void ToggleOnOff();
		void Tick();

		void AutoDriveMenu();
	}

	namespace VehicleSlam
	{
		void InitSub(GTAvehicle veh, float* val);

		void VehicleSlamMenu();
	}

	void VehicleWeaponsMenu();

	void VehicleMultipliersMenu();

	void VehicleMultiPlatNeonsMenu();

	namespace Speedo
	{
		struct SpeedoImage
		{
			std::string fileName;
			DxHookIMG::DxTexture id;
		};

		extern SpeedoImage currentSpeedoBG;
		extern SpeedoImage currentSpeedoNeedle;
		extern UINT8 speedoAlpha;
		extern Vector2 speedoPosition;

		enum eSpeedoMode : UINT8 { SPEEDOMODE_OFF, SPEEDOMODE_DIGITAL, SPEEDOMODE_ANALOGUE };
		extern UINT8 loopSpeedo;
		extern bool speedoMPH;

		void SetCurrentBgIdFromBgNameForConfig();
		void LoadSpeedoImages();

		void SpeedoTick();

		void SpeedoMainMenu();
		void SpeedoThemesLight();
		void SpeedoThemesDark();
	}

}



