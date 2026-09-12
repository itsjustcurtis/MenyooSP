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

#include "GenericLoopedMode.h"

class GTAentity;
class Camera;

namespace GTA2Cam
{
	class Gta2Cam final : public GenericLoopedMode
	{
	private:
	public:
		const Camera& MainCam();

		void CreateMainCam(GTAentity myPed);

		void TurnOn() override;
		void TurnOff() override;

		void Tick() override;
		inline void DoGta2CamTick();

	};


	extern Gta2Cam g_gta2Cam;

	void ToggleOnOff();
}






