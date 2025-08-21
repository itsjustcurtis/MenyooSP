#pragma once
#include <vector>
#include <filesystem>

#include "Submenus/Settings.h"
#include "Submenus/Spooner/SpoonerSettings.h"

typedef unsigned __int8 UINT8;
typedef unsigned int UINT;
enum class BodyguardType : UINT8;
class Vector3;
class GTABodyguard;
namespace GTAmodel {
	class Model;
}

//namespace BodyguardMenu
//{
	//class BodyguardPed;
	//void BodyguardList();
	//void BodyguardGodmode();

	//namespace BodyguardManagement
	//{
// Code
	//}

//}

namespace sub::BodyguardMenu
{
	// Full class definition BEFORE the vector
	class BodyguardEntity
	{
	public:
		BodyguardEntity();
		~BodyguardEntity();

		// data members
		int id;
		GTAentity entityHandle;
		std::string name;

		EntityType Type;
		std::string HashName;
		GTAentity Handle;
	};

	extern; std::vector<BodyguardEntity> BodyguardDb;

	namespace BodyguardManagement
	{
		UINT GetNumberOfBodyguardsSpawned(const EntityType& type);

		int GetBodyguardIndexInDb(const GTAentity& entity);
		int GetBodyguardIndexInDb(const BodyguardEntity& ent);
		void AddBodyguardToDb(BodyguardEntity ent, bool missionEnt = sub::Spooner::Settings::bAddToDbAsMissionEntities);
		void RemoveBodyguardFromDb(const BodyguardEntity& ent);

		void ClearBodyguardDb();
		void DeleteBodyguard(BodyguardEntity& ent);
		BodyguardEntity AddPed(const GTAmodel::Model& model, const std::string& name, bool unloadModel = true);

		//BodyguardEntity CopyEntity(BodyguardEntity orig, bool isInDb, bool addToDb = true, UINT8 copyAttachments = 0, bool unloadModel = true, UINT8 currAtir = 0);

		// Keyboard input
		BodyguardEntity InputBodyguardIntoDb(const EntityType& type);

		void ShowBoxAroundEntity(const GTAentity& ent, bool showPoly = true, RgbS colour = { 0, 255, 255 });
		void ShowArrowAboveEntity(const GTAentity& ent, RGBA colour = { 0, 255, 255, 200 });
		void DrawRadiusDisplayingMarker(const Vector3& pos, float radius, RGBA colour = { 0, 255, 0, 130 });
	}

}