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

#include <vector>
#include <array>
#include <map>
#include <string>
//#include <utility>

using UINT8 = unsigned char;
using DWORD = unsigned long;
using Hash = unsigned long;
using PCHAR = char*;

class GTAentity;

struct WeaponS { std::string label, name; };

class s_Weapon_Components_Tint
{
public:
	Hash weaponHash;
	std::vector<std::pair<Hash, int>> componentHashes;
	int tint;
};

struct NamedWeaponComponent
{
	std::string name; Hash hash;
};

class WeaponAndComponents
{
public:
	Hash weaponHash;
	std::vector<NamedWeaponComponent> components;
	const std::vector<std::string>* tintCaptions;
	template<typename WH> WeaponAndComponents(WH pWeaponHash, std::vector<NamedWeaponComponent> pComponents, const std::vector<std::string>* pTintCaptions = nullptr)
		: weaponHash(pWeaponHash), tintCaptions(pTintCaptions)
	{
		this->components = pComponents;
	}
};

namespace WeaponIndivs
{
	enum WEAPONTYPE{ WEAPE_CURRENTLYHELD = -3, WEAPE_MELEE = 0, WEAPE_PISTOLS, WEAPE_SMGS, WEAPE_MGS, WEAPE_ASSAULTRIFLES, WEAPE_SHOTGUNS, WEAPE_SNIPERS, WEAPE_HEAVY, WEAPE_THROWABLES };
	extern const std::vector<std::string> vCategoryNames;
	extern const std::vector<std::string> vCaptions_Tints;
	extern const std::vector<std::string> vCaptions_TintsMk2;
	extern const std::vector<std::string> vCaptions_ChuteTints;
	extern std::vector<WeaponAndComponents> wc_MELEE;
	extern std::vector<WeaponAndComponents> wc_PISTOLS;
	extern std::vector<WeaponAndComponents> wc_SMGS;
	extern std::vector<WeaponAndComponents> wc_MGS;
	extern std::vector<WeaponAndComponents> wc_ASSAULTRIFLES;
	extern std::vector<WeaponAndComponents> wc_SHOTGUNS;
	extern std::vector<WeaponAndComponents> wc_SNIPERS;
	extern std::vector<WeaponAndComponents> wc_HEAVY;
	extern std::vector<WeaponAndComponents> wc_THROWABLES;
	extern std::vector<WeaponAndComponents> wc_ADDON;
	extern std::map<Hash, std::string> vWeaponLabels;
	extern std::array<std::vector<WeaponAndComponents>*, 10> vAllWeapons;
	const WeaponAndComponents* get_weapon(Hash weaponHash);
	bool get_weapon_components(Hash weaponHash, std::vector<NamedWeaponComponent>& result);
	UINT8 get_weapon_components_count(Hash weaponHash);
	bool is_weapon_tintable(Hash weaponHash);
}


// Weapon - labels
std::string GetWeaponLabel(Hash hash, bool gxt);

// Weapon - give
void GivePedParachute(GTAentity ped);
void GivePedMaxAmmo(GTAentity ped);
void GiveAllWeaponsToPed(GTAentity ped, bool bInfAmmo = false);


