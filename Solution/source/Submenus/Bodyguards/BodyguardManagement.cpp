#include "BodyguardManagement.h"

#include "../../Menu/Menu.h"
#include "../../Scripting/Game.h"
#include "../../Scripting/GTAped.h"
#include "../../Scripting/GTAentity.h"
#include "../../Scripting/GTAblip.h"
#include "../../Natives/natives.h"

#include <algorithm>
#include "../../Util/StringManip.h"
#include "../../Scripting/Model.h"
#include "../../Scripting/World.h"

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
			auto it = std::remove_if(
				BodyguardDb.begin(),
				BodyguardDb.end(),
				[&](const BodyguardEntity& e)
				{
					return e.Handle.GetHandle() == ent.Handle.GetHandle();
				}
			);

			if (it != BodyguardDb.end())
				BodyguardDb.erase(it, BodyguardDb.end());
		}

		void DeleteBodyguard(BodyguardEntity& ent)
		{
			if (!ent.Handle.Exists())
				return;

			Ped ped = ent.Handle.GetHandle();

			ent.Handle.RequestControl();

			GTAblip blip = ent.Handle.CurrentBlip();
			if (blip.Exists())
				blip.Remove();

			ent.Handle.Detach();

			ent.Handle.MissionEntity_set(false);

			if (ped && ENTITY::DOES_ENTITY_EXIST(ped))
			{
				PED::DELETE_PED(&ped);
			}

			ent.Handle = GTAped();

			RemoveBodyguardFromDb(ent);
		}
				
		void ShowArrowAboveEntity(const GTAentity& ent, RGBA colour)
		{
			if (ent.Exists())
			{
				const auto& soe_pos = ent.Position_get();
				const auto& soe_md = ent.ModelDimensions();
				const auto& markerPos = soe_pos + Vector3(0, 0, (std::max)(soe_md.Dim1.z, soe_md.Dim2.z) + 0.20f); // May not be at the right position if the entity is tilted
				World::DrawMarker(MarkerType::UpsideDownCone, markerPos, Vector3(), Vector3(), Vector3(0.45f, 0.45f, 0.50f), RGBA(190, 0, 0, 190));
			}

		
		}
	}
}
