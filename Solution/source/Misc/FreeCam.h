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

class Camera;

namespace FreeCamMode
{
	// Feature enabled in Misc Options (hotkey can toggle flying). Persisted as "freecam".
	extern bool bEnabled;

	// Currently flying
	bool IsActive();

	void Start();
	void Stop();
	void Toggle();

	const Camera& GetCamera();

	void Tick();
}
