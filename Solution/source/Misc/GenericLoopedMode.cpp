/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "GenericLoopedMode.h"

// Not used
void GenericLoopedMode::TickAll()
{

}


GenericLoopedMode::GenericLoopedMode()
	: bEnabled(false)
{
}

bool& GenericLoopedMode::Enabled()
{
	return bEnabled;
}

void GenericLoopedMode::Toggle()
{
	if (bEnabled)
	{
		TurnOff();
	}
	else
	{
		TurnOn();
	}
}
void GenericLoopedMode::TurnOn()
{
	bEnabled = true;
}
void GenericLoopedMode::TurnOff()
{
	bEnabled = false;
}




