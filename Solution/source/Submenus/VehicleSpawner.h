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
#include "..\Util\GTAmath.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\DxHookIMG.h"
#include "..\Util\ExePath.h"
#include "..\Scripting\ModelNames.h"
#include "..\Menu\FolderPreviewBmps.h"
#include "..\Util\StringManip.h"
#include "..\Util\keyboard.h"
#include "..\Util\FileLogger.h"

#include "VehicleModShop.h"
#include "WeaponOptions.h"
#include "Spooner\MenuOptions.h"
#include "Spooner\Databases.h"
#include "Spooner\EntityManagement.h"
#include "Spooner\FileManagement.h"
#include "Spooner\SpoonerEntity.h"

#include <Shlwapi.h> //PathIsDirectory
#pragma comment(lib, "Shlwapi.lib")
#include <string>
#include <vector>
#include <pugixml\src\pugixml.hpp>
#include <dirent\include\dirent.h>

typedef unsigned __int8 UINT8;
typedef int INT, Vehicle, Entity, Ped;
typedef unsigned long DWORD, Hash;
typedef char *PCHAR;

class GTAentity;
class GTAvehicle;
class GTAped;
namespace GTAmodel
{
	class Model;
}

namespace sub
{
	int SpawnVehicle(GTAmodel::Model model, GTAped ped, bool deleteOld = false, bool warpIntoVehicle = true);

	namespace VehicleSpawner
	{
		extern UINT8 spawnVehicleIndex;

		enum Indices
		{
			COMPACT, SEDAN, SUV, COUPE, MUSCLE, SPORTSCLASSIC, SPORT, SUPER,
			MOTORCYCLE, OFFROAD, INDUSTRIAL, UTILITY, VAN, BICYCLE, BOAT, HELICOPTER,
			PLANE, SERVICE, EMERGENCY, MILITARY, COMMERCIAL, TRAIN, OPENWHEEL, OTHER, DRIFT
		};

		void PopulateVehicleBmps();
		void AddVehicleCategoryOption(const std::string& text, UINT8 index, bool *extra_option_code = nullptr);

	}

	void SpawnVehicleMenu();
	void SpawnVehicleOptions();

	bool SpawnVehicleIsVehicleModelAFavourite(GTAmodel::Model vehModel);
	bool SpawnVehicleAddVehicleModelToFavourites(GTAmodel::Model vehModel, const std::string& customName);
	bool SpawnVehicleRemoveVehicleModelFromFavourites(GTAmodel::Model vehModel);

	void SpawnVehicleDLC();
	void SpawnVehicleDLCSelection();
	void SpawnVehicleAllCategoriesMenu();
	void SpawnVehicleFavouritesMenu();

	namespace VehicleSaver
	{
		extern UINT8 _persistentAttachmentsTexterIndex;
		extern UINT8 _driverVisibilityTexterIndex;

		void VehicleSaveToFile(std::string filePath, GTAvehicle ev);
		void VehicleReadFromFile(std::string filePath, GTAentity ped);

		void VehicleSaverMenu();
		void VehSaverInItemMenu();
		int saveCarVars();
		void saveColourVals();

	}

}
