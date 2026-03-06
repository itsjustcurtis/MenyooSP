#pragma once

#include <string>
#include <vector>

class Vector3;

namespace sub::Spooner
{
	class SpoonerBlip;

	namespace BlipCustoms
	{
		void DrawAll();

		SpoonerBlip* AddBlip(const std::string& name, const Vector3& position, const Vector3& rotation);
		SpoonerBlip* AddBlip(const Vector3& position, const Vector3& rotation);

		void GetAllBlipsInRange(std::vector<SpoonerBlip>& result, const Vector3& position, float radius);

		void ClearDb();
		inline std::vector<SpoonerBlip>::iterator RemoveBlip(std::vector<SpoonerBlip>::iterator it);
		void RemoveBlip(SpoonerBlip& blip);
		void RemoveBlip(int indexInDb);
		void RemoveAllBlips();
		//void RemoveAllBlipsInRange(const Vector3& position, float radius);

		SpoonerBlip* CopyBlip(SpoonerBlip& blip);
	}

}