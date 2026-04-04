/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "SmashAbility.h"

#include "..\macros.h"

#include "..\Natives\natives2.h"
#include "..\Util\GTAmath.h"
#include "..\Scripting\enums.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAped.h"
#include "..\Memory\GTAmemory.h"
#include "..\Scripting\Game.h"

#include "..\Menu\Routine.h"

namespace SmashAbility
{
	void SmashAbility::TurnOn()
	{
		GenericLoopedMode::TurnOn();

		PrintSmashInstructions();
	}

	void SmashAbility::Tick()
	{
		if (bEnabled)
		{
			if (IsLevitatePressed())
				DoLevitate();
			if (IsLevitateReleased())
				DoSmash();
		}
	}

	void SmashAbility::DoLevitate()
	{
		GTAped myPed = PLAYER_PED_ID();
		const Vector3& myPos = myPed.GetPosition();

		if (!myPed.Task().IsPlayingAnimation("mini@strip_club@idles@dj@idle_01", "idle_01"))
			myPed.Task().PlayAnimation("mini@strip_club@idles@dj@idle_01", "idle_01", 1, 1, -1, AnimFlag::UpperBodySecondTask, 0, false);

		std::vector<Entity> entList;
		GTAmemory::GetEntityHandles(entList, myPos, 25.0f);
		for (GTAentity entity : entList)
		{
			if (entity == g_myVeh || entity == myPed.Handle())
				continue;
			const Vector3& entityPos = entity.GetPosition();
			OscillateEntity(entity, entityPos + Vector3(0, 0, 0.10f), 1.9f, 0.3f);
		}
	}
	void SmashAbility::DoSmash()
	{
		GTAped myPed = PLAYER_PED_ID();
		const auto& myPos = myPed.GetPosition();

		if (myPed.Task().IsPlayingAnimation("mini@strip_club@idles@dj@idle_01", "idle_01"))
			myPed.Task().ClearAnimation("mini@strip_club@idles@dj@idle_01", "idle_01");

		std::vector<Entity> entList;
		GTAmemory::GetEntityHandles(entList, myPos, 25.0f);
		for (GTAentity entity : entList)
		{
			if (entity.Handle() == g_myVeh || entity.Handle() == myPed.Handle())
				continue;
			const Vector3& entityPos = entity.GetPosition();
			entity.ApplyForce(Vector3(0, 0, -31.0f * entity.HeightAboveGround()), ForceType::MaxForceRot2);
		}
	}

	bool SmashAbility::IsLevitatePressed()
	{
		DISABLE_CONTROL_ACTION(0, INPUT_JUMP, true);
		return IS_DISABLED_CONTROL_PRESSED(0, INPUT_JUMP) != 0;
	}
	bool SmashAbility::IsLevitateReleased()
	{
		return IS_DISABLED_CONTROL_JUST_RELEASED(0, INPUT_JUMP) != 0;
	}

	void SmashAbility::PrintSmashInstructions()
	{
		Game::Print::PrintBottomLeft(oss_ << "Hold " << "~b~Jump" << "~s~ to use the ability.");
	}


	SmashAbility g_smashAbility;

	void ToggleOnOff()
	{
		g_smashAbility.Toggle();
	}

}


