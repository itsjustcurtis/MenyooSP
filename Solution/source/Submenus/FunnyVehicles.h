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
#include "..\Scripting\Game.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\GTAprop.h"

#include "VehicleSpawner.h"
#include "VehicleModShop.h"

#include <string>

typedef unsigned __int8 UINT8;
typedef int INT, Entity, Ped, Vehicle, Object, ScrHandle;
typedef unsigned long DWORD, Hash;
typedef float FLOAT;
typedef char *PCHAR;

class Vector3;
class GTAvehicle;
namespace GTAmodel
{
	class Model;
}

namespace sub
{
	void AttachPedToVehicle(GTAmodel::Model model, GTAvehicle vehicle, const Vector3& offset, const Vector3& rotation, bool invis = false, bool piggyback = false);
	void AttachObjectToVehicle(GTAmodel::Model model, GTAvehicle vehicle, float X, float Y, float Z, float Pitch, float Roll, float Yaw, bool invis, int boneIndex = -1, bool dynamic = false, bool collisionEnabled = false, bool destroyVar = true);
	void AttachVehicleToVehicle(GTAmodel::Model model, GTAvehicle vehicle, int primColour, int secColour, float X, float Y, float Z, float Pitch, float Roll, float Yaw, bool invis, int boneIndex = -1, bool collisionEnabled = false);

	namespace FunnyVehicles
	{

		void Sub_FunnyVehicles();

	}

}



