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

typedef unsigned long DWORD, Hash;

#include <string>
#include <vector>

class GTAped;

namespace sub::Spooner
{
	namespace RelationshipManagement
	{
		//struct RelationshipGroup_t{ std::string name; Hash handle; };
		extern bool rgCreated;
		extern std::string rgSpoonerFriends;
		extern std::string rgSpoonerEnemies;
		extern std::string rgSpoonerNeutral;
		extern std::string rgMe;

		extern std::vector<std::string> relationshipGroups;

		//bool GetPedRelationshipGroup(const GTAped& ped, std::string& outGrp)
		bool GetPedRelationshipGroup(const GTAped& ped, Hash& outHash);
		void SetPedRelationshipGroup(GTAped ped, Hash group);
	}

}



