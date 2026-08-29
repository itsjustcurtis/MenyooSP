/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "MenuOptions.h"

#include "..\..\macros.h"

#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\GTAped.h"

#include "SpoonerMode.h"
#include "EntityManagement.h"
#include "RelationshipManagement.h"
#include "SpoonerEntity.h"
#include "../../Scripting/Game.h"

namespace sub::Spooner
{
	namespace MenuOptions
	{
		void AddOption_AddProp(const std::string& text, const GTAmodel::Model& model)
		{
			bool pressed = false;
			AddOption(text, pressed);

			if (*Menu::currentopATM == Menu::printingop)
			{
				SpoonerMode::modelPreviewInfo.entityType = EntityType::PROP;
				SpoonerMode::modelPreviewInfo.model = model;
			}

			if (pressed)
			{
				EntityManagement::AddProp(model, text);
			}
		}
		void AddOptionAddPed(const std::string& text, const GTAmodel::Model& model)
		{
			bool pressed = false;
			AddOption(text, pressed);

			if (*Menu::currentopATM == Menu::printingop)
			{
				SpoonerMode::modelPreviewInfo.entityType = EntityType::PED;
				SpoonerMode::modelPreviewInfo.model = model;
			}

			if (pressed)
			{
				EntityManagement::AddPed(model, text);
			}
		}
		void AddOption_AddVehicle(const std::string& text, const GTAmodel::Model& model)
		{
			bool pressed = false;
			AddOption(text, pressed);

			if (*Menu::currentopATM == Menu::printingop)
			{
				SpoonerMode::modelPreviewInfo.entityType = EntityType::VEHICLE;
				SpoonerMode::modelPreviewInfo.model = model;
			}

			if (pressed)
			{
				EntityManagement::AddVehicle(model, text);
			}
		}

		void AddOption_RelationshipTextScroller()
		{
			std::vector<std::string> relationshipStringVec{ "NONE" };
			for (UINT8 i = 0; i < RelationshipManagement::relationshipGroups.size(); i++)
			{
				if (GTAped(selectedEntity.handle).GetRelationshipGroup() == GET_HASH_KEY(RelationshipManagement::relationshipGroups[i]))
					relationshipStringVec[0] = RelationshipManagement::relationshipGroups[i];
			}

			bool relationshipStringPlus = false, relationshipStringMinus = false;
			AddTexter("Relationship", 0, relationshipStringVec, null, relationshipStringPlus, relationshipStringMinus);
			if (relationshipStringPlus)
			{
				Hash currHash;
				if (RelationshipManagement::GetPedRelationshipGroup(selectedEntity.handle, currHash))
				{
					for (INT8 i = 0; i < RelationshipManagement::relationshipGroups.size(); i++)
					{
						if (GTAped(selectedEntity.handle).GetRelationshipGroup() == GET_HASH_KEY(RelationshipManagement::relationshipGroups[i]))
						{
							i++;
							if (i >= RelationshipManagement::relationshipGroups.size())
								break;
							RelationshipManagement::SetPedRelationshipGroup(selectedEntity.handle, GET_HASH_KEY(RelationshipManagement::relationshipGroups[i]));
							break;
						}
					}
				}
				else
				{
					RelationshipManagement::SetPedRelationshipGroup(selectedEntity.handle, GET_HASH_KEY(RelationshipManagement::relationshipGroups[0]));
				}
			}
			if (relationshipStringMinus)
			{
				for (INT8 i = 0; i < RelationshipManagement::relationshipGroups.size(); i++)
				{
					if (GTAped(selectedEntity.handle).GetRelationshipGroup() == GET_HASH_KEY(RelationshipManagement::relationshipGroups[i]))
					{
						i--;
						if (i < 0)
							break;
						RelationshipManagement::SetPedRelationshipGroup(selectedEntity.handle, GET_HASH_KEY(RelationshipManagement::relationshipGroups[i]));
						break;
					}
				}
			}
		}

		void AddOption_AddPedWithCallback(const std::string& text, const GTAmodel::Model& model, const std::function<void(Ped, const std::string&)>& callback)
		{
			bool pressed = false;
			AddOption(text, pressed);

			if (*Menu::currentopATM == Menu::printingop)
			{
				SpoonerMode::modelPreviewInfo.entityType = EntityType::PED;
				SpoonerMode::modelPreviewInfo.model = model;
			}

			if (pressed)
			{
				Hash modelHash = *(Hash*)&model;
				std::string modelName = text;

				if (!STREAMING::HAS_MODEL_LOADED(modelHash))
				{
					STREAMING::REQUEST_MODEL(modelHash);
					while (!STREAMING::HAS_MODEL_LOADED(modelHash))
					{
						WAIT(0);
					}
				}

				Vector3 pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER_PED_ID(), 0.0f, 5.0f, 0.0f);
				Ped pedHandle = PED::CREATE_PED(26, modelHash, pos.x, pos.y, pos.z, 0.0f, true, true);

				if (pedHandle != 0)
				{
					callback(pedHandle, modelName);
				}
				else
				{
					Game::Print::PrintBottomLeft("Failed to spawn ped.");
				}
			}
		}



	}

}



