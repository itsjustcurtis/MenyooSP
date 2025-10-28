#include "BodyguardManagement.h"

#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/GTAped.h"
#include "BodyguardMenu.h"
#include "Natives/natives.h"
#include <algorithm>
#include "Menu/Routine.h"
#include "Util/StringManip.h"

// Provide equality operators for BodyguardEntity and a single definition for BodyguardDb.
// std::find and other algorithms require operator== for the element type.
namespace sub::BodyguardMenu
{
	// compare by underlying Handle (GTAentity has operator==)
	bool operator==(const BodyguardEntity& left, const BodyguardEntity& right)
	{
		return left.Handle == right.Handle;
	}
	bool operator!=(const BodyguardEntity& left, const BodyguardEntity& right)
	{
		return left.Handle != right.Handle;
	}

	// define the storage for the DB (was incorrectly declared in header as "extern;")
	std::vector<BodyguardEntity> BodyguardDb;
}

namespace sub::BodyguardMenu
{
	namespace BodyguardManagement
	{
		UINT GetNumberOfBodyguardsSpawned(const EntityType& type)
		{
			switch (type)
			{
			case EntityType::ALL:
				return (UINT)BodyguardMenu::BodyguardDb.size();
				break;
			default:
				return (UINT)std::count_if(BodyguardMenu::BodyguardDb.begin(), BodyguardMenu::BodyguardDb.end(),
					[type](const BodyguardEntity& item) {
						return item.Type == type;
					});
				break;
			}
		}

		int GetBodyguardIndexInDb(const GTAentity& entity)
		{
			for (int i = 0; i < BodyguardMenu::BodyguardDb.size(); i++)
			{
				if (BodyguardMenu::BodyguardDb[i].Handle == entity)
					return i;
			}
			return -1;
		}
		int GetBodyguardIndexInDb(const BodyguardEntity& ent)
		{
			return GetBodyguardIndexInDb(ent.Handle);
		}
		void AddBodyguardToDb(BodyguardEntity ent, bool missionEnt)
		{
			if (ent.Handle.Exists())
			{
				if (ent.HashName.length() == 0)
					ent.HashName = int_to_hexstring(ent.Handle.Model().hash, true);
				if (missionEnt)
					ent.Handle.MissionEntity_set(true);
				BodyguardMenu::BodyguardDb.push_back(ent);
			}
		}
		void RemoveBodyguardFromDb(const BodyguardEntity& ent)
		{
			GTAentity handle = ent.Handle;

			const auto& eit = std::find(BodyguardMenu::BodyguardDb.begin(), BodyguardMenu::BodyguardDb.end(), ent);
			if (eit != BodyguardMenu::BodyguardDb.end())
			{
				BodyguardMenu::BodyguardDb.erase(eit);
			}

		}

		void ClearBodyguardDb()
		{
			BodyguardMenu::BodyguardDb.clear();
		}
		void DeleteBodyguard(BodyguardEntity& ent)
		{
			GTAentity handle = ent.Handle;
			for (auto& e : sub::BodyguardMenu::BodyguardDb)

				RemoveBodyguardFromDb(ent); // Remove this from db
			handle.Detach(); // Detach this
			//GTAblip(handle.CurrentBlip()).Remove();
			handle.Delete(handle != PLAYER::PLAYER_PED_ID() && handle != g_myVeh);
		}
	}
}
