#include "BodyguardSpawn.h"
#include "BodyguardFunction.h"
#include "Scripting/Game.h"
#include "Util/FileLogger.h"
#include "Natives/natives.h"

#include "..\..\macros.h"

#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Natives\types.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\GTAprop.h"
#include "..\..\Scripting\GTAvehicle.h"
#include "..\..\Scripting\GTAped.h"
#include "..\..\Util\StringManip.h"
#include "..\..\Util\FileLogger.h"
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\Model.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\ModelNames.h"
#include "..\..\Scripting\enums.h"
#include "..\..\Scripting\Camera.h"

#include "Submenus/PedAnimation.h"
#include "Submenus/PedComponentChanger.h"
#include "Scripting/PTFX.h"
#include "Submenus/Spooner/Databases.h"
#include "Submenus/Spooner/EntityManagement.h"

#include <string>
#include <vector>
#include <utility>
#include <algorithm>

namespace sub::BodyguardMenu
{
	BodyguardEntity AddEntity_Bodyguard(text, model)
	{
		if (Databases::BodyguardDb.size() >= GTA_MAX_ENTITIES)
		{
			Game::Print::PrintBottomLeft("~r~Error:~s~ Max spawn count reached.");
			addlog(ige::LogType::LOG_WARNING, "Failed to spawn bodyguard, max spawn count reached", __FILENAME__);
			return BodyguardEntity();
		}

		if (!model.IsInCdImage() || !model.IsValid())
		{
			Game::Print::PrintError_InvalidModel();
			return BodyguardEntity();
		}

		if (!model.IsLoaded())
			model.Load();
		while (!model.IsLoaded()) WAIT(0);

		GTAped myPed = PLAYER_PED_ID();
		BodyguardEntity newEntity;
		const ModelDimensions& dimensions = model.Dimensions();
		bool bDynamic = false;
		bool bFreezePos = false;
		bool bCollision = true;

		if (unloadModel)
			model.Unload();

		if (!newEntity.Handle.IsValid())
		{
			Game::Print::PrintBottomLeft("~r~Error:~s~ Ped spawn failed.");
			return BodyguardEntity();
		}

		GTAped ep = newEntity.Handle;
		SET_NETWORK_ID_CAN_MIGRATE(PED_TO_NET(ep.Handle()), true);

		// === BODYGUARD BEHAVIOR START ===
		PED::SET_PED_AS_GROUP_MEMBER(ep.Handle(), PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));
		PED::SET_PED_NEVER_LEAVES_GROUP(ep.Handle(), true);
		ep.CanSwitchWeapons_set(true);
		SET_PED_COMBAT_ABILITY(ep.Handle(), 2);
		SET_PED_COMBAT_MOVEMENT(ep.Handle(), 2);
		// === BODYGUARD BEHAVIOR END ===

		// Standard Spooner flags
		ep.FreezePosition(bFreezePos);
		ep.Dynamic_set(bDynamic);
		ep.LodDistance_set(1000000);
		ep.MissionEntity_set(true);
		ep.SetInvincible(Settings::bSpawnInvincibleEntities);
		ep.SetExplosionProof(Settings::bSpawnInvincibleEntities);
		ep.SetMeleeProof(Settings::bSpawnInvincibleEntities);
		ep.BlockPermanentEvents_set(Settings::bSpawnStillPeds);

		// Animation flags
		SET_PED_CAN_PLAY_AMBIENT_ANIMS(ep.Handle(), true);
		SET_PED_CAN_PLAY_AMBIENT_BASE_ANIMS(ep.Handle(), true);
		SET_PED_CAN_PLAY_GESTURE_ANIMS(ep.Handle(), true);
		SET_PED_CAN_PLAY_VISEME_ANIMS(ep.Handle(), true, TRUE);
		SET_PED_IS_IGNORED_BY_AUTO_OPEN_DOORS(ep.Handle(), true);

		SET_PED_PATH_CAN_USE_CLIMBOVERS(ep.Handle(), true);
		SET_PED_PATH_CAN_USE_LADDERS(ep.Handle(), true);
		SET_PED_PATH_CAN_DROP_FROM_HEIGHT(ep.Handle(), true);

		model.LoadCollision(100);
		ep.IsCollisionEnabled_set(bCollision);
		model.Unload();

		newEntity.Type = (EntityType)ep.Type();

		if (name.length() > 0)
			newEntity.HashName = name;
		else
		{
			newEntity.HashName = get_ped_model_label(model, false);
			if (newEntity.HashName.length() == 0)
				newEntity.HashName = int_to_hexstring(model.hash, true);
		}

		newEntity.Dynamic = bDynamic;

		AddBodyguardToDb(newEntity);

		return newEntity;
	}
}