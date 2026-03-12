#include "BlipCustoms.h"
#include "SpoonerBlips.h"
#include "Databases.h"

#include <string>
#include <vector>

namespace sub::Spooner
{
	SpoonerBlip* SelectedBlip = nullptr;
	namespace BlipCustoms
	{
		void DrawAll()
		{
			for (auto& blip : Databases::BlipDb)
			{
				// drawing will be implemented later
			}
		}

		SpoonerBlip* AddBlip(const std::string& name, const Vector3& position, const Vector3& rotation)
		{
			Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
			Databases::BlipDb.push_back(SpoonerBlip(name, position, rotation));
			return &Databases::BlipDb.back();
		}

		SpoonerBlip* AddBlip(const Vector3& position, const Vector3& rotation)
		{
			Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
			Databases::BlipDb.push_back(SpoonerBlip("BLIP", position, rotation));
			return &Databases::BlipDb.back();
		}

		void GetAllBlipsInRange(std::vector<SpoonerBlip>& result, const Vector3& position, float radius)
		{
			result.clear();

			for (auto& blip : Databases::BlipDb)
			{
				// SpoonerBlip stores coordinates in X, Y, Z — construct a Vector3 for distance checks
				Vector3 blipPos(blip.X, blip.Y, blip.Z);
				if (position.DistanceTo(blipPos) < radius)
				{
					result.push_back(blip);
				}
			}
		}

		void ClearDb()
		{
			Databases::BlipDb.clear();
		}

		inline std::vector<SpoonerBlip>::iterator RemoveBlip(std::vector<SpoonerBlip>::iterator it)
		{
			return Databases::BlipDb.erase(it);
		}

		void RemoveBlip(SpoonerBlip& blip)
		{
			auto it = std::find(Databases::BlipDb.begin(), Databases::BlipDb.end(), blip);

			if (it != Databases::BlipDb.end())
			{
				RemoveBlip(it);
			}
		}

		void RemoveBlip(int indexInDb)
		{
			if (indexInDb >= 0 && indexInDb < Databases::BlipDb.size())
			{
				RemoveBlip(Databases::BlipDb.begin() + indexInDb);
			}
		}

		void RemoveAllBlips()
		{
			BlipCustoms::ClearDb();
		}

		SpoonerBlip* CopyBlip(SpoonerBlip& blip)
		{
			SpoonerBlip newBlip = blip;

			Databases::BlipDb.reserve(Databases::BlipDb.size() + 1);
			Databases::BlipDb.push_back(newBlip);

			return &Databases::BlipDb.back();
		}
	}
}