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

#include <string>
#include "../Menu/Routine.h"

typedef unsigned __int8 UINT8;
typedef int INT, Entity, Ped, Vehicle, Object, ScrHandle;
typedef unsigned long DWORD, Hash;
typedef float FLOAT;
typedef char *PCHAR;

class RGBA;
class GTAped;
namespace GTAmodel 
{
	class Model;
}

extern Ped g_WeaponOpsPedOverride;
extern Player g_WeaponOpsPlayerOverride;
extern Ped g_WeaponMenuPedOverride;

namespace sub 
{ 
	namespace PtfxSubs 
	{ 
		struct PtfxS; 
	}
}

namespace sub
{
    void Weaponops();


	void ForgeGun_();

	namespace GravityGun_catind
	{

		void Tick();
		bool& Enabled();
		float& ShootForce();

		void Sub_GravityGun();

	}

	namespace TriggerFxGun
	{
		void AddOptionGunFX(const sub::PtfxSubs::PtfxS& fx);

		void Sub_GunMain();
	}

	void AddKaboomGunOption(const std::string& text, Hash newHash, bool *extra_option_code = nullptr, bool gxt = false);
	void KaboomGun_();

	void AddbgunOption_(const std::string& text, Hash hash, bool *extra_option_code = nullptr);
	void BulletGun_();

	void AddPedGunOption(const std::string& text, GTAmodel::Model newModel, bool *extra_option_code = nullptr);
	void PedGun_();
	void PedGun_AllPeds();

	void AddObjectGunOption(const std::string& text, GTAmodel::Model newModel, bool *extra_option_code = nullptr, bool gxt = false);
	void ObjectGun_();

	void List_VehicleCats_Sub();

	namespace WeaponFavourites_catind
	{
		bool PopulateFavouritesInfo();
		bool IsWeaponAFavourite(Hash whash);
		bool AddWeaponToFavourites(Hash whash, const std::string& customName);
		bool RemoveWeaponFromFavourites(Hash whash);

		void Sub_WeaponFavourites();
	}

	namespace WeaponIndivs_catind
	{

		//extern INT& selectedCategory;
		//extern INT& selectedWeapon;


		// THESE SUBMENUS NEED THE AMMU NATION BANNER
		void Sub_CategoriesList();
		void Sub_InCategory();
		void Sub_InItem();
		void Sub_InItem_Mods();
		void Sub_Parachute();

	}

	namespace WeaponsLoadouts_catind
	{

		bool Create(GTAped ped, const std::string& filePath);
		bool Apply(GTAped ped, const std::string& filePath);

		void Sub_Loadouts();
		void Sub_Loadouts_InItem();
	}

	namespace LaserSight_catind
	{
		extern bool bEnabled;
		extern RGBA _colour;

		inline void DoSight();
		void Tick();

		void Sub_LaserSight();
	}

	void ObjectSpawner_objs();

}



