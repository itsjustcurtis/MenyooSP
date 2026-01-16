#include "BodyguardManagement.h"

#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/GTAped.h"
#include "Scripting/GTAentity.h"
#include "Natives/natives.h"

#include <algorithm>
#include "Util/StringManip.h"
#include "Scripting/Model.h"

namespace sub::BodyguardMenu
{
	bool operator==(const BodyguardEntity& left, const BodyguardEntity& right)
	{
		return left.Handle == right.Handle;
	}
	bool operator!=(const BodyguardEntity& left, const BodyguardEntity& right)
	{
		return !(left == right);
	}

	std::vector<BodyguardEntity> BodyguardDb;
}

namespace sub::BodyguardMenu
{
	namespace BodyguardManagement
	{
		// <- changed UINT -> unsigned int
		unsigned int GetNumberOfBodyguardsSpawned(const EntityType& type)
		{
			switch (type)
			{
			case EntityType::ALL:
				return static_cast<unsigned int>(BodyguardDb.size());
			default:
				return static_cast<unsigned int>(std::count_if(
					BodyguardDb.begin(),
					BodyguardDb.end(),
					[type](const BodyguardEntity& item)
					{
						return item.Type == type;
					}));
			}
		}

		int GetBodyguardIndexInDb(const GTAentity& entity)
		{
			for (int i = 0; i < static_cast<int>(BodyguardDb.size()); ++i)
			{
				if (BodyguardDb[i].Handle == entity)
					return i;
			}
			return -1;
		}

		int GetBodyguardIndexInDb(const BodyguardEntity& ent)
		{
			return GetBodyguardIndexInDb(ent.Handle);
		}

		void AddBodyguardToDb(BodyguardEntity ent)
		{
			if (!ent.Handle.Exists())
				return;

			if (ent.HashName.empty())
				ent.HashName = int_to_hexstring(ent.Handle.Model().hash, true);

			BodyguardDb.push_back(std::move(ent));
		}

		void RemoveBodyguardFromDb(const BodyguardEntity& ent)
		{
			const auto it = std::find(BodyguardDb.begin(), BodyguardDb.end(), ent);
			if (it != BodyguardDb.end())
				BodyguardDb.erase(it);
		}

		void DeleteBodyguard(BodyguardEntity& ent)
		{
			if (!ent.Handle.Exists())
			{
				RemoveBodyguardFromDb(ent);
				return;
			}

			RemoveBodyguardFromDb(ent);
			ent.Handle.Detach();

			GTAentity playerPed = PLAYER::PLAYER_PED_ID();
			if (ent.Handle != playerPed)
				ent.Handle.Delete(true);
		}
				

			void ShowArrowAboveEntity(const GTAentity & entity)
			{
				if (!entity.Exists())
					return;

				Vector3 pos = entity.GetOffsetInWorldCoords({ 0.0f, 0.0f, 1.5f });
				GRAPHICS::DRAW_MARKER(
					2,              // type
					pos.x, pos.y, pos.z,
					0, 0, 0,        // direction
					0, 0, 0,        // rotation
					0.3f, 0.3f, 0.3f, // scale
					255, 0, 0, 200, // color
					false, false, 2,
					false, nullptr, nullptr, false
				);
			}

		
	}
}
