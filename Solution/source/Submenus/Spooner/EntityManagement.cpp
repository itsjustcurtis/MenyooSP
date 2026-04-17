/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "EntityManagement.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Natives\types.h" //RGBA/RgbS
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

#include "..\PedAnimation.h"
#include "..\PedComponentChanger.h"
#include "..\PtfxSubs.h"
#include "Databases.h"
#include "SpoonerEntity.h"
#include "SpoonerSettings.h"
#include "BlipManagement.h"
#include "SpoonerMode.h"
#include "RelationshipManagement.h"

#include <string>
#include <vector>
#include <utility>
#include <algorithm>

namespace sub::Spooner
{
	namespace EntityManagement
	{
		UINT GetNumberOfEntitiesSpawned(const EntityType& type)
		{
			switch (type)
			{
			case EntityType::ALL:
				return (UINT)Databases::EntityDb.size();
				break;
			default:
				return (UINT)std::count_if(Databases::EntityDb.begin(), Databases::EntityDb.end(),
					[type](const SpoonerEntity& item) {
					return item.type == type;
				});
				break;
			}
		}

		int GetEntityIndexInDb(const GTAentity& entity)
		{
			for (int i = 0; i < Databases::EntityDb.size(); i++)
			{
				if (Databases::EntityDb[i].handle == entity)
					return i;
			}
			return -1;
		}
		int GetEntityIndexInDb(const SpoonerEntity& ent)
		{
			return GetEntityIndexInDb(ent.handle);
		}
		void AddEntityToDb(SpoonerEntity ent, bool missionEnt)
		{
			if (ent.handle.Exists())
			{
				if (ent.hashName.length() == 0)
					ent.hashName = IntToHexString(ent.handle.Model().hash, true);
				if (missionEnt)
					ent.handle.SetMissionEntity(true);
				Databases::EntityDb.push_back(ent);
			}
		}
		void RemoveEntityFromDb(const SpoonerEntity& ent)
		{
			GTAentity handle = ent.handle;

			const auto& eit = std::find(Databases::EntityDb.begin(), Databases::EntityDb.end(), ent);
			if (eit != Databases::EntityDb.end())
			{
				eit->taskSequence.Reset(true);
				Databases::EntityDb.erase(eit);
			}

			for (auto rit = Databases::RelationshipDb.begin(); rit != Databases::RelationshipDb.end();)
			{
				if (rit->first.Equals(handle))
				{
					rit = Databases::RelationshipDb.erase(rit);
					break;
				}
				else ++rit;
			}
		}

		void ClearDb()
		{
			Databases::EntityDb.clear();
			Databases::RelationshipDb.clear();
			BlipManagement::ClearAllRefCoordBlips();
		}
		void DeleteEntity(SpoonerEntity& ent)
		{
			GTAentity handle = ent.handle;
			for (auto& e : Databases::EntityDb)
			{
				GTAentity attTo;
				if (e.attachmentArgs.isAttached && GetEntityThisEntityIsAttachedTo(e.handle, attTo) && attTo.Equals(handle))
				{
					DetachEntity(e); // Detach other attachments (to clear variables)
				}
			}
			RemoveEntityFromDb(ent); // Remove this from db
			handle.Detach(); // Detach this
			//GTAblip(handle.CurrentBlip()).Remove();
			handle.Delete(handle != PLAYER_PED_ID() && handle != g_myVeh);
		}
		void DeleteAllEntitiesInDb()
		{
			GTAentity myPed = PLAYER_PED_ID();
			for (auto& e : Databases::EntityDb)
			{
				if (e.handle != myPed &&  e.handle != g_myVeh)
				{
					//GTAblip(e.handle.CurrentBlip()).Remove();
					e.handle.Delete(true);
				}
			}
			ClearDb();
		}
		void DeleteAllEntitiesOfTypeInDb(const EntityType& targetType)
		{
			GTAentity myPed = PLAYER_PED_ID();
			for (auto it = Databases::EntityDb.begin(); it != Databases::EntityDb.end();)
			{
				if (it->type == targetType)
				{
					for (auto& e : Databases::EntityDb)
					{
						GTAentity attTo;
						if (e.attachmentArgs.isAttached && GetEntityThisEntityIsAttachedTo(e.handle, attTo) && attTo.Equals(it->handle))
						{
							DetachEntity(e); // Detach other attachments (to clear variables)
						}
					}
					it->handle.Detach(); // Detach this
					//GTAblip(it->handle.CurrentBlip()).Remove();
					it->handle.Delete(it->handle != myPed && it->handle != g_myVeh);
					it = Databases::EntityDb.erase(it);
				}
				else ++it;
			}
			if (targetType == EntityType::PED) Databases::RelationshipDb.clear();
		}
		inline void DeleteAllPropsInDb()
		{
			DeleteAllEntitiesOfTypeInDb(EntityType::PROP);
		}
		inline void DeleteAllPedsInDb()
		{
			DeleteAllEntitiesOfTypeInDb(EntityType::PED);
		}
		inline void DeleteAllVehiclesInDb()
		{
			DeleteAllEntitiesOfTypeInDb(EntityType::VEHICLE);
		}
		void DeleteInvalidEntitiesInDb()
		{
			for (auto it = Databases::EntityDb.begin(); it != Databases::EntityDb.end();)
			{
				if (!it->handle.Exists())
				{
					//it->handle.Delete(false);
					it = Databases::EntityDb.erase(it);
				}
				else ++it;
			}
			for (auto rit = Databases::RelationshipDb.begin(); rit != Databases::RelationshipDb.end();)
			{
				if (!rit->first.Exists())
				{
					rit = Databases::RelationshipDb.erase(rit);
				}
				else ++rit;
			}
		}

		void DeleteAllEntitiesInWorld()
		{
			GTAentity myPed = PLAYER_PED_ID();
			for (GTAentity e : worldEntities)
			{
				if (e != myPed && e != g_myVeh)
					e.Delete();
			}
			ClearDb();

			WAIT(0);
			UpdateNearbyStuffArraysTick();
		}
		void DeleteAllPropsInWorld()
		{
			for (GTAentity e : worldObjects)
			{
				e.Delete();
			}

			/*std::vector<SpoonerEntity> newdb;
			for (auto& dbe : Databases::EntityDb)
			{
			switch (dbe.type)
			{
			case EntityType::PROP: break;
			default: newdb.push_back(dbe); break;
			}
			}
			Databases::EntityDb = newdb;*/
			DeleteAllPropsInDb();

			WAIT(0);
			UpdateNearbyStuffArraysTick();
		}
		void DeleteAllPedsInWorld()
		{
			GTAentity myPed = PLAYER_PED_ID();
			for (GTAentity e : worldPeds)
			{
				if (e != myPed)
					e.Delete();
			}

			/*std::vector<SpoonerEntity> newdb;
			for (auto& dbe : Databases::EntityDb)
			{
			switch (dbe.type)
			{
			case EntityType::PED: break;
			default: newdb.push_back(dbe); break;
			}
			}
			Databases::EntityDb = newdb;
			Databases::RelationshipDb.clear();*/
			DeleteAllPedsInDb();

			WAIT(0);
			UpdateNearbyStuffArraysTick();
		}
		void DeleteAllVehiclesInWorld()
		{
			for (GTAprop e : worldVehicles)
			{
				e.Delete(e != g_myVeh);
			}

			/*std::vector<SpoonerEntity> newdb;
			for (auto& dbe : Databases::EntityDb)
			{
			switch (dbe.type)
			{
			case EntityType::VEHICLE: break;
			default: newdb.push_back(dbe); break;
			}
			}
			Databases::EntityDb = newdb;*/
			DeleteAllVehiclesInDb();

			WAIT(0);
			UpdateNearbyStuffArraysTick();
		}

		SpoonerEntity AddProp(const GTAmodel::Model& model, const std::string& name, bool unloadModel)
		{
			if (Databases::EntityDb.size() >= GTA_MAX_ENTITIES)
			{
				Game::Print::PrintBottomLeft("~r~Error:~s~ Max spawn count reached.");
				addlog(ige::LogType::LOG_WARNING, "Failed to add prop, max spawn count " + std::to_string(GTA_MAX_ENTITIES) + " Reached");
				return SpoonerEntity();
			}
			if (!model.IsInCdImage())
			{
				const std::string& mdlnme = get_prop_model_label(model);
				std::string suggestedLocation = "NO SUGGESTED LOCATIONS";
				if (!mdlnme.empty())
				{
					for (auto& prfx : std::vector<std::pair<std::string, std::string>>
					{
						{ "v_club", "Nightclubs" },
						{ "bah", "Bahama Mama's" },
						{ "cc", "Comedy club (Split Sides West)" },
						{ "vu", "Vanilla Unicorn" },
						{ "rock", "Tequila-la" },
						{ "v_res", "Residential - Vanilla Unicorn" },
						{ "d", "Strawberry Ave - Franklin's house" },
						{ "fa", "Strawberry Ave - Franklin's house" },
						{ "fh", "Vinewood Hills - Franklin's house" },
						{ "j", "FIB janitor's apartment" },
						{ "lest", "Lester's place" },
						{ "m", "Michael's place" },
						{ "mp", "Multiplayer apartments" },
						{ "r", "Madrazo's ranch" },
						{ "son", "Jimmy's room" },
						{ "mson", "Jimmy's room" },
						{ "tre", "Floyd's apartment" },
						{ "tt", "Trevor's trailer" },
						{ "v_med", "medical facilities" },
						{ "lab", "Humane Labs" },
						{ "cor", "Morgue" },
						{ "hosp", "Hospital" },
						{ "p", "Psychiatrist's office (Michael's shrink)" },
						{ "v_ret", "Retail outlets" },
						{ "247", "Convenience stores" },
						{ "csr", "Car showroom (Simeon)" },
						{ "fh", "Farmhouse" },
						{ "gc", "Gun club (Ammunation)" },
						{ "ml", "Meth lab" },
						{ "ps", "Ponsonby's clothing" },
						{ "ta", "tattoo parlour" },
						{ "v_serv", "Services" },
						{ "bs", "Barber shops" },
						{ "ct", "Control towers" },
						{ "v_corp", "Corporate" },
						{ "face", "Lifeinvader Building" }

					})
					{
						if (mdlnme.size() > prfx.first.size())
						{
							if (mdlnme.compare(0, prfx.first.size(), prfx.first) == 0)
								suggestedLocation = prfx.second;
						}
					}
				}
				Game::Print::PrintBottomCentre("~r~Error:~s~ Invalid model.\n Check suggested locations: [" + suggestedLocation + "]");
				return SpoonerEntity();
			}

			GTAped myPed = PLAYER_PED_ID();
			SpoonerEntity newEntity;
			const ModelDimensions& dimensions = model.Dimensions();
			bool bDynamic = Settings::bSpawnDynamicProps;
			bool bFreezePos = !bDynamic;
			bool bCollision = true;

			auto& spoocam = SpoonerMode::spoonerModeCamera;

			if (!spoocam.IsActive())
			{
				GTAentity myPedOrVehicle = myPed.IsInVehicle() ? (GTAentity)myPed.CurrentVehicle() : (GTAentity)myPed;

				newEntity.handle = World::CreateProp(model, myPedOrVehicle.GetOffsetInWorldCoords(0, myPedOrVehicle.Dim1().y + 2.6f + dimensions.Dim2.y, 0), myPedOrVehicle.Rotation_get(), bDynamic, false);
				if (unloadModel)
					model.Unload();
				if (!myPedOrVehicle.IsInAir())
					PLACE_OBJECT_ON_GROUND_PROPERLY(newEntity.handle.Handle());
			}
			else
			{
				Vector3 spawnPos = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 120.0f, 30.0f + dimensions.Dim2.y);
				spawnPos.z += dimensions.Dim1.z;

				newEntity.handle = World::CreateProp(model, spawnPos, Vector3(0, 0, spoocam.GetRotation().z), bDynamic, false);
				if (unloadModel)
					model.Unload();
			}

			SET_NETWORK_ID_CAN_MIGRATE(OBJ_TO_NET(newEntity.handle.Handle()), true);
			newEntity.handle.SetDynamic(false);
			newEntity.handle.FreezePosition(true);
			newEntity.handle.FreezePosition(bFreezePos);
			newEntity.handle.SetDynamic(bDynamic);
			newEntity.handle.SetLODDistance(1000000);
			newEntity.handle.SetMissionEntity(true);
			newEntity.handle.SetInvincible(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetExplosionProof(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetMeleeProof(Settings::bSpawnInvincibleEntities);
			model.LoadCollision(100);
			newEntity.handle.SetIsCollisionEnabled(bCollision);
			model.Unload();

			newEntity.type = (EntityType)newEntity.handle.Type();

			if (name.length() > 0)
				newEntity.hashName = name;
			else
			{
				newEntity.hashName = get_prop_model_label(model);
				if (newEntity.hashName.length() == 0) newEntity.hashName = IntToHexString(model.hash, true);
			}

			newEntity.dynamic = bDynamic;

			AddEntityToDb(newEntity);

			return newEntity;
		}
		SpoonerEntity AddPed(const GTAmodel::Model& model, const std::string& name, bool unloadModel)
		{
			if (Databases::EntityDb.size() >= GTA_MAX_ENTITIES)
			{
				Game::Print::PrintBottomLeft("~r~Error:~s~ Max spawn count reached.");
				addlog(ige::LogType::LOG_WARNING, "Failed to spawn ped, max spawn count " + std::to_string(GTA_MAX_ENTITIES) + " Reached");
				return SpoonerEntity();
			}
			if (!model.IsInCdImage())
			{
				Game::Print::PrintErrorInvalidModel(name);
				addlog(ige::LogType::LOG_ERROR, "Failed to spawn ped, invalid model: " + name);
				return SpoonerEntity();
			}

			GTAped myPed = PLAYER_PED_ID();
			SpoonerEntity newEntity;
			const ModelDimensions& dimensions = model.Dimensions();
			bool bDynamic = Settings::bSpawnDynamicPeds;
			bool bFreezePos = !bDynamic;
			bool bCollision = true;

			auto& spoocam = SpoonerMode::spoonerModeCamera;

			if (!spoocam.IsActive())
			{
				GTAentity myPedOrVehicle = myPed.IsInVehicle() ? (GTAentity)myPed.CurrentVehicle() : (GTAentity)myPed;

				newEntity.handle = World::CreatePed(model, myPedOrVehicle.GetOffsetInWorldCoords(0, myPedOrVehicle.Dim1().y + 2.6f + dimensions.Dim2.y, 0), myPedOrVehicle.Rotation_get(), myPedOrVehicle.HeightAboveGround() < 3.0f);
				if (unloadModel)
					model.Unload();
			}
			else
			{
				Vector3 spawnPos = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 120.0f, 30.0f + dimensions.Dim2.y);
				spawnPos.z += dimensions.Dim1.z;

				newEntity.handle = World::CreatePed(model, spawnPos, Vector3(0, 0, spoocam.GetRotation().z), false);
				if (unloadModel)
					model.Unload();
			}

			GTAped ep = newEntity.handle;
			SET_NETWORK_ID_CAN_MIGRATE(PED_TO_NET(newEntity.handle.Handle()), true);
			newEntity.handle.FreezePosition(bFreezePos);
			newEntity.handle.SetDynamic(bDynamic);
			newEntity.handle.SetLODDistance(1000000);
			newEntity.handle.SetMissionEntity(true);
			newEntity.handle.SetInvincible(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetExplosionProof(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetMeleeProof(Settings::bSpawnInvincibleEntities);
			newEntity.isStill = Settings::bSpawnStillPeds;
			ep.SetBlockPermanentEvent(Settings::bSpawnStillPeds);

			SET_PED_CAN_PLAY_AMBIENT_ANIMS(ep.Handle(), true);
			SET_PED_CAN_PLAY_AMBIENT_BASE_ANIMS(ep.Handle(), true);
			SET_PED_CAN_PLAY_GESTURE_ANIMS(ep.Handle(), true);
			SET_PED_CAN_PLAY_VISEME_ANIMS(ep.Handle(), true, TRUE);
			SET_PED_IS_IGNORED_BY_AUTO_OPEN_DOORS(ep.Handle(), true);

			SET_PED_PATH_CAN_USE_CLIMBOVERS(ep.Handle(), true);
			SET_PED_PATH_CAN_USE_LADDERS(ep.Handle(), true);
			SET_PED_PATH_CAN_DROP_FROM_HEIGHT(ep.Handle(), true);
			//SET_PED_PATH_PREFER_TO_AVOID_WATER(ep.Handle(), true);
			//SET_PED_PATH_AVOID_FIRE(ep.Handle(), true);
			SET_PED_COMBAT_ABILITY(ep.Handle(), 2);
			SET_PED_COMBAT_MOVEMENT(ep.Handle(), 2);
			ep.SetCanSwitchWeapons(false);
			model.LoadCollision(100);
			newEntity.handle.SetIsCollisionEnabled(bCollision);
			model.Unload();

			newEntity.type = (EntityType)newEntity.handle.Type();

			if (name.length() > 0)
				newEntity.hashName = name;
			else
			{
				newEntity.hashName = GetPedModelLabel(model, false);
				if (newEntity.hashName.length() == 0)
					newEntity.hashName = IntToHexString(model.hash, true);
			}

			newEntity.dynamic = bDynamic;

			AddEntityToDb(newEntity);

			return newEntity;
		}
		SpoonerEntity AddVehicle(const GTAmodel::Model& model, const std::string& name, bool unloadModel)
		{
			if (Databases::EntityDb.size() >= GTA_MAX_ENTITIES)
			{
				Game::Print::PrintBottomLeft("~r~Error:~s~ Max spawn count reached.");
				addlog(ige::LogType::LOG_WARNING, "Failed to add vehicle, max spawn count " + std::to_string(GTA_MAX_ENTITIES) + " Reached");
				return SpoonerEntity();
			}
			if (!model.IsInCdImage())
			{
				Game::Print::PrintErrorInvalidModel(name);
				addlog(ige::LogType::LOG_ERROR, "Failed to spawn vehicle, invalid model: " + name);
				return SpoonerEntity();
			}

			GTAped myPed = PLAYER_PED_ID();
			SpoonerEntity newEntity;
			const ModelDimensions& dimensions = model.Dimensions();
			bool bDynamic = Settings::bSpawnDynamicVehicles;
			bool bFreezePos = !bDynamic;
			bool bCollision = true;

			auto& spoocam = SpoonerMode::spoonerModeCamera;

			if (!spoocam.IsActive())
			{
				GTAentity myPedOrVehicle = myPed.IsInVehicle() ? (GTAentity)myPed.CurrentVehicle() : (GTAentity)myPed;

				newEntity.handle = World::CreateVehicle(model, myPedOrVehicle.GetOffsetInWorldCoords(0, myPedOrVehicle.Dim1().y + 3.6f + dimensions.Dim2.y, 0), myPedOrVehicle.Rotation_get(), false);
				if (unloadModel)
					model.Unload();
				if (!myPedOrVehicle.IsInAir())
					SET_VEHICLE_ON_GROUND_PROPERLY(newEntity.handle.Handle(), 0.0f);
			}
			else
			{
				Vector3 spawnPos = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 120.0f, 30.0f + dimensions.Dim2.y);
				spawnPos.z += dimensions.Dim1.z;

				newEntity.handle = World::CreateVehicle(model, spawnPos, Vector3(0, 0, spoocam.GetRotation().z), false);
				if (unloadModel) model.Unload();
			}

			SET_NETWORK_ID_CAN_MIGRATE(VEH_TO_NET(newEntity.handle.Handle()), true);
			SET_VEHICLE_MOD_KIT(newEntity.handle.Handle(), 0);
			SET_VEHICLE_DIRT_LEVEL(newEntity.handle.Handle(), 0.0f);
			SET_VEHICLE_ENVEFF_SCALE(newEntity.handle.Handle(), 0.3f);
			GTAvehicle(newEntity.handle).CloseAllDoors(true);
			newEntity.handle.FreezePosition(bFreezePos);
			newEntity.handle.SetDynamic(bDynamic);
			newEntity.handle.SetLODDistance(1000000);
			newEntity.handle.SetMissionEntity(true);
			newEntity.handle.SetInvincible(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetExplosionProof(Settings::bSpawnInvincibleEntities);
			newEntity.handle.SetMeleeProof(Settings::bSpawnInvincibleEntities);
			model.LoadCollision(100);
			newEntity.handle.SetIsCollisionEnabled(bCollision);
			model.Unload();

			newEntity.type = (EntityType)newEntity.handle.Type();

			if (name.length() > 0)
				newEntity.hashName = name;
			else
			{
				newEntity.hashName = get_vehicle_model_label(model, false);
				if (newEntity.hashName.length() == 0)
					newEntity.hashName = IntToHexString(model.hash, true);
			}

			newEntity.dynamic = bDynamic;

			AddEntityToDb(newEntity);

			return newEntity;
		}
		SpoonerEntity AddEntityOfType(const EntityType& type, const GTAmodel::Model& model, const std::string& name)
		{
			switch (type)
			{
			case EntityType::PROP: return AddProp(model, name); break;
			case EntityType::PED: return AddPed(model, name); break;
			case EntityType::VEHICLE: return AddVehicle(model, name); break;
			}
			return SpoonerEntity();
		}

		SpoonerEntity CopyEntity(SpoonerEntity orig, bool isInDb, bool addToDb, UINT8 copyAttachments, bool unloadModel, UINT8 currAtir)
		{
			if (!orig.handle.Exists())
				return SpoonerEntity();

			bool bOrigWasMissionEntity = orig.handle.MissionEntity_get();
			if (!bOrigWasMissionEntity)
			{
				orig.handle.SetMissionEntity(true);
				WAIT(20);
			}
			SpoonerEntity newEntity(orig);
			bool bDynamic = orig.dynamic;
			bool bFreezePos = orig.handle.IsPositionFrozen();
			newEntity.handle.SetDynamic(false);
			newEntity.handle.FreezePosition(true);

			GTAped myPed = PLAYER_PED_ID();
			bool isThisMyPed = orig.handle.Handle() == myPed.Handle();

			bool bTaskSeqIsActive = false;
			if (isInDb)
			{
				bool bTaskSeqIsEmpty = orig.taskSequence.empty();
				bTaskSeqIsActive = orig.taskSequence.IsActive();
				newEntity.taskSequence.AllTasks().clear();
				newEntity.taskSequence.Reset();
				auto& oldTaskSeqTasks = orig.taskSequence.AllTasks();

				if (!bTaskSeqIsEmpty)
				{
					for (STSTask* tskPtr : oldTaskSeqTasks)
					{
						if (tskPtr != nullptr)
						{
							STSTask* newtskPtr = newEntity.taskSequence.AddTask(tskPtr->type);
							if (newtskPtr != nullptr)
							{
								newtskPtr->Assign(tskPtr);
							}
						}
					}
					if (bTaskSeqIsActive) newEntity.taskSequence.Start();
				}
			}

			EntityType entType = (EntityType)orig.handle.Type();
			if (entType == EntityType::PROP)
			{
				newEntity.handle = World::CreateProp(orig.handle.Model(), orig.handle.GetPosition(), orig.handle.Rotation_get(), bDynamic, false);
				SET_NETWORK_ID_CAN_MIGRATE(OBJ_TO_NET(newEntity.handle.Handle()), true);
				GTAprop eo = newEntity.handle;

				if (orig.textureVariation != -1)
					SET_OBJECT_TINT_INDEX(eo.Handle(), orig.textureVariation);
			}
			else if (entType == EntityType::PED)
			{
				//std::vector<s_Weapon_Components_Tint> weaponsBackup;
				GTAped ep;
				GTAped origPed = orig.handle;

				//newEntity.handle = World::CreatePed(orig.handle.Model(), orig.handle.Position_get(), orig.handle.Rotation_get(), false);
				newEntity.handle = origPed.Clone(origPed.GetHeading(), true, true);
				ep = newEntity.handle;

				const auto& movGrpStr = GetPedMovementClipSet(orig.handle);
				if (!movGrpStr.empty())
					SetPedMovementClipSet(ep, movGrpStr);
				const auto& wMovGrpStr = GetPedWeaponMovementClipSet(orig.handle);
				if (!wMovGrpStr.empty())
					SetPedWeaponMovementClipSet(ep, wMovGrpStr);

				ep.SetPosition(origPed.GetPosition());
				ep.SetRotation(origPed.Rotation_get());
				sub::PedHeadFeatures_catind::vPedHeads[ep.Handle()] = sub::PedHeadFeatures_catind::vPedHeads[origPed.Handle()];
				sub::PedDamageTextures::vPedsAndDamagePacks[ep.Handle()] = sub::PedDamageTextures::vPedsAndDamagePacks[origPed.Handle()];
				sub::PedDecals::vPedsAndDecals[ep.Handle()] = sub::PedDecals::vPedsAndDecals[origPed.Handle()];
				//GTAped(orig.handle).StoreWeaponsInArray(weaponsBackup);
				//GTAped(newEntity.handle).GiveWeaponsFromArray(weaponsBackup);
				SET_NETWORK_ID_CAN_MIGRATE(ep.NetID(), true);
				ep.SetBlockPermanentEvent(orig.isStill);
				SET_PED_CONFIG_FLAG(ep.Handle(), ePedConfigFlags::_Shrink, GET_PED_CONFIG_FLAG(origPed.Handle(), ePedConfigFlags::_Shrink, false));

				SET_PED_CAN_PLAY_AMBIENT_ANIMS(ep.Handle(), true);
				SET_PED_CAN_PLAY_AMBIENT_BASE_ANIMS(ep.Handle(), true);
				SET_PED_CAN_PLAY_GESTURE_ANIMS(ep.Handle(), true);
				SET_PED_CAN_PLAY_VISEME_ANIMS(ep.Handle(), true, TRUE);
				SET_PED_IS_IGNORED_BY_AUTO_OPEN_DOORS(ep.Handle(), true);

				if (!bTaskSeqIsActive && IS_PED_USING_SCENARIO(orig.handle.Handle(), orig.lastAnimation.name.c_str()))
				{
					WAIT(40);
					ep.Task().StartScenario(orig.lastAnimation.name, -1, false);
				}
				if (!bTaskSeqIsActive && IS_ENTITY_PLAYING_ANIM(orig.handle.Handle(), orig.lastAnimation.dict.c_str(), orig.lastAnimation.name.c_str(), 3))
				{
					ep.Task().PlayAnimation(orig.lastAnimation.dict, orig.lastAnimation.name);
				}

				const auto& facialMoodStr = GetPedFacialMood(orig.handle);
				if (!facialMoodStr.empty())
					SetPedFacialMood(ep, facialMoodStr);

				ep.SetArmour(origPed.GetArmour());
				ep.SetWeapon(origPed.GetWeapon());
				ep.SetCanSwitchWeapons(false);
				SET_PED_PATH_CAN_USE_CLIMBOVERS(ep.Handle(), true);
				SET_PED_PATH_CAN_USE_LADDERS(ep.Handle(), true);
				SET_PED_PATH_CAN_DROP_FROM_HEIGHT(ep.Handle(), true);
				//SET_PED_PATH_PREFER_TO_AVOID_WATER(ep.Handle(), true);
				//SET_PED_PATH_AVOID_FIRE(ep.Handle(), true);
				SET_PED_COMBAT_ABILITY(ep.Handle(), 2);
				SET_PED_COMBAT_MOVEMENT(ep.Handle(), 2);
				Hash oldRelationshipGrp;
				RelationshipManagement::GetPedRelationshipGroup(orig.handle, oldRelationshipGrp);
				RelationshipManagement::SetPedRelationshipGroup(ep, oldRelationshipGrp);
			}
			else if (entType == EntityType::VEHICLE)
			{
				//newEntity.handle = World::CreateVehicle(orig.handle.Model(), orig.handle.Position_get(), orig.handle.Rotation_get(), false);
				newEntity.handle = clone_vehicle(orig.handle);
				newEntity.handle.SetPosition(orig.handle.GetPosition());
				newEntity.handle.SetRotation(orig.handle.Rotation_get());
				SET_NETWORK_ID_CAN_MIGRATE(VEH_TO_NET(newEntity.handle.Handle()), true);
			}

			newEntity.handle.FreezePosition(bFreezePos);
			newEntity.handle.SetDynamic(bDynamic);
			newEntity.handle.SetLODDistance(1000000);
			newEntity.handle.SetMissionEntity(true);
			newEntity.handle.SetVisible(orig.handle.IsVisible());
			newEntity.handle.SetInvincible(orig.handle.IsInvincible());
			newEntity.handle.SetBulletProof(orig.handle.IsBulletProof());
			newEntity.handle.SetIsCollisionEnabled(orig.handle.GetIsCollisionEnabled());
			newEntity.handle.SetExplosionProof(orig.handle.IsExplosionProof());
			newEntity.handle.SetFireProof(orig.handle.IsFireProof());
			newEntity.handle.SetMeleeProof(orig.handle.IsMeleeProof());
			newEntity.handle.SetOnlyDamagedByPlayer(orig.handle.IsOnlyDamagedByPlayer());
			newEntity.handle.SetOnFire(orig.handle.IsOnFire());
			newEntity.handle.SetAlpha(orig.handle.GetAlpha());
			newEntity.handle.SetHealth(orig.handle.GetHealth());

			orig.handle.SetMissionEntity(bOrigWasMissionEntity);

			if (orig.attachmentArgs.isAttached && !currAtir)
			{
				GTAentity attTo;
				if (GetEntityThisEntityIsAttachedTo(orig.handle, attTo))
				{
					EntityManagement::AttachEntity(newEntity, attTo, orig.attachmentArgs.boneIndex, orig.attachmentArgs.offset, orig.attachmentArgs.rotation);
				}
				// else set both isAttached to false??
			}

			// fx lops
			for (auto& ptfxlop : sub::PtfxSubs::fxLoops)
			{
				if (ptfxlop.entity == orig.handle)
				{
					sub::PtfxSubs::PtfxlopS newPtfxLop;
					newPtfxLop.entity = newEntity.handle;
					newPtfxLop.asset = ptfxlop.asset.c_str();
					newPtfxLop.fx = ptfxlop.fx.c_str();
					sub::PtfxSubs::fxLoops.push_back(newPtfxLop);
					break;
				}
			}

			if (isInDb && copyAttachments)
			{
				std::set<Hash> atirModelHashes;
				GTAentity attTo;
				for (auto& e : Databases::EntityDb)
				{
					if (GetEntityThisEntityIsAttachedTo(e.handle, attTo) && attTo == orig.handle)
					{
						atirModelHashes.insert(e.handle.Model().hash);
						auto newAtt = CopyEntity(e, true, false, copyAttachments, false, currAtir + 1);
						EntityManagement::AttachEntity(newAtt, newEntity.handle, e.attachmentArgs.boneIndex, e.attachmentArgs.offset, e.attachmentArgs.rotation);
						if (addToDb) Databases::EntityDb.push_back(newAtt);
					}
				}
				if (unloadModel)
				{
					for (Model amh : atirModelHashes)
						amh.Unload();
				}
			}

			//bug?
			orig.handle.FreezePosition(bFreezePos);
			orig.handle.SetDynamic(bDynamic);

			if (unloadModel)
				newEntity.handle.Model().Unload();
			if (addToDb)
				Databases::EntityDb.push_back(newEntity);
			return newEntity;
		}

		void DetachEntity(SpoonerEntity& ent)
		{
			bool isOnTheLine = NETWORK_IS_IN_SESSION() != 0;
			bool bHasCollision = ent.handle.GetIsCollisionEnabled();
			if (isOnTheLine)
				ent.handle.RequestControl();
			ent.handle.Detach();
			ent.attachmentArgs.isAttached = false;
			ent.attachmentArgs.boneIndex = 0;
			ent.attachmentArgs.offset.clear();
			ent.attachmentArgs.rotation.clear();
			ent.handle.SetIsCollisionEnabled(bHasCollision);

			GTAentity attTo;
			for (auto& e : Databases::EntityDb)
			{
				if (GetEntityThisEntityIsAttachedTo(e.handle, attTo) && attTo == ent.handle)
				{
					AttachEntity(e, attTo, e.attachmentArgs.boneIndex, e.attachmentArgs.offset, e.attachmentArgs.rotation);
				}
			}
		}
		bool GetEntityThisEntityIsAttachedTo(GTAentity& from, GTAentity& to)
		{
			/*for (auto& e : _worldEntities)
			{
			if (IS_ENTITY_ATTACHED_TO_ENTITY(from.handle.Handle(), to.Handle()))
			{
			to = e;
			return true;
			}
			}
			return false;*/
			if (from.IsAttached())
			{
				to = GET_ENTITY_ATTACHED_TO(from.Handle());
				return true;
			}
			to = 0;
			return false;
		}
		
		void AttachEntity(SpoonerEntity& ent, GTAentity to, int boneIndex, const Vector3& offset, const Vector3& rotation)
		{
			if (ent.handle.Handle() != to.Handle())
			{
				bool isOnTheLine = NETWORK_IS_IN_SESSION() != 0;
				bool bHasCollision = ent.handle.GetIsCollisionEnabled();
				bool wheelsAreInvis = AreVehicleWheelsInvisible(ent.handle);
				if (isOnTheLine)
				{
					ent.handle.RequestControl();
				}
				ent.handle.AttachTo(to, boneIndex, bHasCollision, offset, rotation);
				ent.attachmentArgs.isAttached = true;
				ent.attachmentArgs.boneIndex = boneIndex;
				ent.attachmentArgs.offset = offset;
				ent.attachmentArgs.rotation = rotation;
				ent.handle.SetDynamic(ent.dynamic);
				SET_ENTITY_LIGHTS(ent.handle.Handle(), 0);
				if (wheelsAreInvis)
					SetVehicleWheelsInvisible(ent.handle, true);
			}
		}

		void AttachEntityInit(SpoonerEntity& ent, GTAentity to, bool bAttachWithRelativePosRot)
		{
			if (to.IsAttachedTo(ent.handle))
			{
				SpoonerEntity to1;
				to1.handle = to;
				auto to1ind = EntityManagement::GetEntityIndexInDb(to1);
				if (to1ind >= 0)
				{
					bool isOnTheLine = NETWORK_IS_IN_SESSION() != 0;
					if (isOnTheLine)
						to1.handle.RequestControl();
					auto& to1r = Databases::EntityDb[to1ind];
					DetachEntity(to1r);
				}
			}
			DetachEntity(ent);
			if (bAttachWithRelativePosRot)
			{
				AttachEntity(ent, to, 0, to.GetOffsetGivenWorldCoords(ent.handle.GetPosition()), ent.handle.Rotation_get() - to.Rotation_get());
			}
			else
			{
				AttachEntity(ent, to, 0, Vector3(), Vector3());
			}
		}

		// Keyboard input
		SpoonerEntity InputEntityIntoDb(const EntityType& type)
		{
			std::string inputStr = Game::InputBox("", 64U, "Input model name:");
			if (inputStr.length() > 0)
			{
				return AddEntityOfType(type, GET_HASH_KEY(inputStr), inputStr);
			}
			return SpoonerEntity();
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerInputEntityIntoDb, std::string(), 64U, "Input model name:");
			//OnscreenKeyboard::State::arg1._int = static_cast<int>(type);
		}

		void ShowBoxAroundEntity(const GTAentity& ent, bool showPoly, RgbS colour)
		{
			if (ent.Exists())
			{
				const auto& entMd = ent.ModelDimensions();

				// I've used the opposite Z dimensions for reasons
				const auto& boxUpperLeftRear = ent.GetOffsetInWorldCoords(-entMd.Dim2.x, -entMd.Dim2.y, entMd.Dim2.z);
				const auto& boxUpperRightRear = ent.GetOffsetInWorldCoords(entMd.Dim1.x, -entMd.Dim2.y, entMd.Dim2.z);
				const auto& boxLowerLeftRear = ent.GetOffsetInWorldCoords(-entMd.Dim2.x, -entMd.Dim2.y, -entMd.Dim1.z);
				const auto& boxLowerRightRear = ent.GetOffsetInWorldCoords(entMd.Dim1.x, -entMd.Dim2.y, -entMd.Dim1.z);

				const auto& boxUpperLeftFront = ent.GetOffsetInWorldCoords(-entMd.Dim2.x, entMd.Dim1.y, entMd.Dim2.z);
				const auto& boxUpperRightFront = ent.GetOffsetInWorldCoords(entMd.Dim1.x, entMd.Dim1.y, entMd.Dim2.z);
				const auto& boxLowerLeftFront = ent.GetOffsetInWorldCoords(-entMd.Dim2.x, entMd.Dim1.y, -entMd.Dim1.z);
				const auto& boxLowerRightFront = ent.GetOffsetInWorldCoords(entMd.Dim1.x, entMd.Dim1.y, -entMd.Dim1.z);

				const RGBA& lineColour = colour.ToRGBA(250);
				const RGBA& polyColour = colour.ToRGBA(100);

				World::DrawLine(boxUpperLeftRear, boxUpperRightRear, lineColour);
				World::DrawLine(boxLowerLeftRear, boxLowerRightRear, lineColour);
				World::DrawLine(boxUpperLeftRear, boxLowerLeftRear, lineColour);
				World::DrawLine(boxUpperRightRear, boxLowerRightRear, lineColour);

				World::DrawLine(boxUpperLeftFront, boxUpperRightFront, lineColour);
				World::DrawLine(boxLowerLeftFront, boxLowerRightFront, lineColour);
				World::DrawLine(boxUpperLeftFront, boxLowerLeftFront, lineColour);
				World::DrawLine(boxUpperRightFront, boxLowerRightFront, lineColour);

				World::DrawLine(boxUpperLeftRear, boxUpperLeftFront, lineColour);
				World::DrawLine(boxUpperRightRear, boxUpperRightFront, lineColour);
				World::DrawLine(boxLowerLeftRear, boxLowerLeftFront, lineColour);
				World::DrawLine(boxLowerRightRear, boxLowerRightFront, lineColour);

				if (showPoly)
				{
					World::DrawPoly(boxUpperRightRear, boxLowerRightRear, boxUpperLeftRear, polyColour);
					World::DrawPoly(boxUpperLeftRear, boxLowerLeftRear, boxLowerRightRear, polyColour);

					World::DrawPoly(boxUpperRightFront, boxLowerRightFront, boxUpperLeftFront, polyColour);
					World::DrawPoly(boxUpperLeftFront, boxLowerLeftFront, boxLowerRightFront, polyColour);

					World::DrawPoly(boxUpperLeftFront, boxLowerLeftFront, boxUpperLeftRear, polyColour);
					World::DrawPoly(boxUpperLeftRear, boxLowerLeftFront, boxLowerLeftRear, polyColour);

					World::DrawPoly(boxUpperRightFront, boxLowerRightFront, boxUpperRightRear, polyColour);
					World::DrawPoly(boxUpperRightRear, boxLowerRightFront, boxLowerRightRear, polyColour);

					World::DrawPoly(boxUpperRightFront, boxUpperRightRear, boxUpperLeftRear, polyColour);
					World::DrawPoly(boxUpperLeftFront, boxUpperRightFront, boxUpperLeftRear, polyColour);

					World::DrawPoly(boxLowerRightFront, boxLowerRightRear, boxLowerLeftRear, polyColour);
					World::DrawPoly(boxLowerLeftFront, boxLowerRightFront, boxLowerLeftRear, polyColour);
				}
			}
		}
		void ShowArrowAboveEntity(const GTAentity& ent, RGBA colour)
		{
			if (ent.Exists())
			{
				const auto& soePos = ent.GetPosition();
				const auto& soeMd = ent.ModelDimensions();
				const auto& markerPos = soePos + Vector3(0, 0, (std::max)(soeMd.Dim1.z, soeMd.Dim2.z) + 1.4f); // May not be at the right position if the entity is tilted
				World::DrawMarker(MarkerType::UpsideDownCone, markerPos, Vector3(), Vector3(), Vector3(1, 1, 2), colour);
			}
		}
		void DrawRadiusDisplayingMarker(const Vector3& pos, float radius, RGBA colour)
		{
			World::DrawMarker(MarkerType::DebugSphere, pos, Vector3(), Vector3(), Vector3(radius, radius, radius), colour);
		}
	}

}



