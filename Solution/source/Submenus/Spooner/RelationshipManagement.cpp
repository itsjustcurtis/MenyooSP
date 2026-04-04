/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "RelationshipManagement.h"

#include "..\..\macros.h"

//#include "..\..\Menu\Menu.h"
//#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\GTAped.h"
#include "..\..\Scripting\World.h"
#include "..\..\Scripting\enums.h"

#include "Databases.h"

#include <string>
#include <vector>
#include <utility>

namespace sub::Spooner
{
	namespace RelationshipManagement
	{
		//struct RelationshipGroup_t{ std::string name; Hash handle; };
		bool rgCreated = false;
		std::string rgSpoonerFriends = { "SPOONER_FRIENDS" };
		std::string rgSpoonerEnemies = { "SPOONER_ENEMIES" };
		std::string rgSpoonerNeutral = { "SPOONER_NEUTRAL" };
		std::string rgMe = { "PLAYER", };

		std::vector<std::string> relationshipGroups{ rgSpoonerFriends, rgSpoonerEnemies, rgSpoonerNeutral };

		/*bool GetPedRelationshipGroup(const GTAped& ped, std::string& outGrp)
		{
			for (auto& r : Databases::RelationshipDb)
			{
				if (ped.Equals(r.first))
				{
					outGrp = r.second;
					return true;
				}
			}
			return false;
		}*/
		bool GetPedRelationshipGroup(const GTAped& ped, Hash& outHash)
		{
			outHash = ped.RelationshipGroup_get();
			return (outHash == GET_HASH_KEY(rgSpoonerFriends) || outHash == GET_HASH_KEY(rgSpoonerEnemies) || outHash == GET_HASH_KEY(rgSpoonerNeutral));
		}
		void SetPedRelationshipGroup(GTAped ped, Hash group)
		{
			//using namespace sub::Spooner::RelationshipManagement;

			if (NETWORK_HAS_PLAYER_STARTED_TRANSITION(PLAYER_ID()))
				rgCreated = false;

			if (!rgCreated)
			{
				World::AddRelationshipGroup(rgSpoonerFriends);
				World::AddRelationshipGroup(rgSpoonerEnemies);
				World::AddRelationshipGroup(rgSpoonerNeutral);
				//World::AddRelationshipGroup(rgMe);

				World::SetRelationshipBetweenGroups(PedRelationship::Neutral, rgSpoonerFriends, rgSpoonerNeutral);
				World::SetRelationshipBetweenGroups(PedRelationship::Neutral, rgSpoonerEnemies, rgSpoonerNeutral);
				World::SetRelationshipBetweenGroups(PedRelationship::Hate, rgSpoonerFriends, rgSpoonerEnemies);
				World::SetRelationshipBetweenGroups(PedRelationship::Companion, rgSpoonerFriends, rgMe);
				rgCreated = true;
			}

			ped.RelationshipGroup_set(group);

			for (auto& r : Databases::RelationshipDb)
			{
				if (ped.Equals(r.first))
				{
					r.second = group;
					return;
				}
			}
			Databases::RelationshipDb.push_back(std::make_pair(ped, group));
		}
	}

}



