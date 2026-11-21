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

#include <string>

namespace GTAmodel
{
	class Model;
}

namespace VehiclePreview3D
{
	extern bool g_enabled;
	extern bool g_mapPreviews;
	extern float g_zoom;
	extern float g_spinSpeed;
	extern float g_boxHeight;
	extern float g_boxWidth;
	extern float g_lightIntensity;
	extern bool g_scaleLighting;

	namespace Debug
	{
		extern bool useOriginalPosition;
		extern bool brightBackground;
		extern bool moveEntityToScene;
		extern float offset[3];
		extern int drawAlpha;
		extern bool drawAttachedPeds;
		extern bool drawAttachedObjects;
		extern bool drawAttachedVehicles;
		extern float lightSpread;
		extern float mapTilt;
	}

	bool Draw(const GTAmodel::Model& model, float centreX, float centreY, float width, float height);
	bool DrawFile(const std::string& filePath, float centreX, float centreY, float width, float height);

	void Tick();
	void Cleanup();
}
