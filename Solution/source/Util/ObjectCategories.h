#pragma once

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <string>

namespace ObjectCategories
{
	enum Category
	{
		Props,
		Interiors,
		WeaponsAndPickups,
		FurnitureAndDecor,
		DoorsWindowsBuildings,
		BarriersFencesGates,
		SignsTrafficStreet,
		NatureLandscape,
		ConstructionIndustrial,
		UtilityElectrical,
		StorageCargo,
		FoodDrinkRetail,
		TrashDebris,
		Vehicles_Parts,
		RacingStunt,
		BeachLeisureSports,
		LootValuables,
		CharacterWearables,
		Missions,
		VfxLod,
		Specialized,
		Category_Count
	};

	enum DlcGroup
	{
		BaseGame,
		DLC_Heists,
		DLC_Executives,
		DLC_ImportExport,
		DLC_Bikers,
		DLC_Gunrunning,
		DLC_SmugglersRun,
		DLC_Doomsday,
		DLC_AfterHours,
		DLC_ArenaWar,
		DLC_DiamondCasino,
		DLC_DiamondCasinoHeist,
		DLC_CayoPerico,
		DLC_Lowriders,
		DLC_SpecialRaces,
		DLC_Nightclub,
		DLC_Mercenaries,
		DLC_Missions2023,
		DLC_Missions2024,
		DLC_Missions2025,
		DLC_Other,
		DLC_Count
	};

	inline std::string ToLower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return text;
	}

	inline bool StartsWith(const std::string& text, const char* prefix)
	{
		return text.rfind(prefix, 0) == 0;
	}

	inline bool StartsWithAny(const std::string& text, std::initializer_list<const char*> prefixes)
	{
		for (const char* prefix : prefixes)
		{
			if (StartsWith(text, prefix))
				return true;
		}
		return false;
	}

	inline bool ContainsAny(const std::string& text, std::initializer_list<const char*> tokens)
	{
		for (const char* token : tokens)
		{
			if (text.find(token) != std::string::npos)
				return true;
		}
		return false;
	}

	inline Category GetObjectCategory(const std::string& modelName)
	{
		const std::string name = ToLower(modelName);

		if (StartsWith(name, "w_") || ContainsAny(name, { "_ammo", "_weapon", "_gun", "_rifle", "_pistol", "_shotgun", "_smg", "_minigun", "_grenade", "_missile" }))
			return WeaponsAndPickups;

		if (StartsWithAny(name, { "v_", "lf_", "reh_int" }) || ContainsAny(name, { "_int_", "_shell", "_room", "_floor", "_ceiling", "_corridor", "_hall", "_mansion", "_apartment", "_garage", "_warehouse" }))
			return Interiors;

		if (StartsWithAny(name, { "ar_", "as_", "sr_", "stt_", "xs_", "lts_" }) || ContainsAny(name, { "_race", "_stunt", "_checkpoint", "_ramp", "_track", "_boost", "_target", "_arena", "_tube_" }))
			return RacingStunt;

		if (ContainsAny(name, { "barrier", "barier", "_bblock", "_bollard", "_fence", "_fnc", "_gate", "_railing", "_rail_", "_roadcone", "_cone", "_armco" }) ||
			StartsWithAny(name, { "prop_fnclink", "prop_fncwood", "prop_fncres", "prop_fnccorgm", "prop_sec", "prop_facgate", "prop_lrggate" }))
			return BarriersFencesGates;

		if (ContainsAny(name, { "_door", "_window", "_wall", "_roof", "_shutter", "_frame", "_pillar", "_stairs", "_lift", "_elevator", "_glass" }))
			return DoorsWindowsBuildings;

		if (StartsWithAny(name, { "prop_sign", "prop_billboard", "prop_streetlight", "prop_traffic", "prop_telegraph", "prop_forsale", "prop_parking" }) ||
			ContainsAny(name, { "_sign", "_traffic", "_streetlight", "_lamp", "_light", "_neon", "_billboard", "_poster", "_banner" }))
			return SignsTrafficStreet;

		if (StartsWithAny(name, { "prop_tree", "prop_bush", "prop_plant", "prop_palm", "prop_veg", "prop_weeds", "prop_rock", "prop_coral", "prop_cactus", "csx_", "cloudhat_", "reeds_", "root_" }) ||
			ContainsAny(name, { "_tree", "_bush", "_plant", "_palm", "_weed", "_grass", "_rock", "_coral", "_cactus", "_flower", "_ivy" }))
			return NatureLandscape;

		if (StartsWithAny(name, { "prop_scafold", "prop_tool", "prop_ind", "prop_const", "prop_conslift", "prop_pipes", "prop_pipe", "prop_oil", "prop_gas", "prop_storagetank" }) ||
			ContainsAny(name, { "_scaf", "_tool", "_crane", "_industrial", "_conveyor", "_tank", "_pipe", "_generator", "_pump", "_worklight" }))
			return ConstructionIndustrial;

		if (StartsWithAny(name, { "prop_elecbox", "prop_aircon", "prop_cctv", "prop_satdish", "prop_roofvent", "prop_monitor", "prop_tv", "vfx_" }) ||
			ContainsAny(name, { "_elec", "_aircon", "_cctv", "_camera", "_satdish", "_vent", "_screen", "_monitor", "_server", "_computer", "_laptop" }))
			return UtilityElectrical;

		if (StartsWithAny(name, { "prop_container", "prop_box", "prop_boxpile", "prop_crate", "prop_pallet", "prop_skip", "prop_dumpster", "prop_bin", "prop_mb" }) ||
			ContainsAny(name, { "_container", "_crate", "_box", "_pallet", "_cargo", "_sack", "_bag", "_case", "_briefcase", "_suitcase" }))
			return StorageCargo;

		if (StartsWithAny(name, { "prop_cash", "prop_gold", "prop_jewel" }) ||
			ContainsAny(name, { "_cash", "_money", "_gold", "_jewel", "_loot", "_safe" }))
			return LootValuables;

		if (StartsWithAny(name, { "prop_food", "prop_beer", "prop_vend", "prop_champ", "prop_bar" }) ||
			ContainsAny(name, { "_food", "_beer", "_wine", "_bottle", "_drink", "_coffee" }))
			return FoodDrinkRetail;

		if (StartsWithAny(name, { "prop_rub", "ng_proc", "frag_", "prop_frag", "prop_old" }) || ContainsAny(name, { "_rubbish", "_trash", "_debris", "_broken", "_frag", "_wreck", "_garbage" }))
			return TrashDebris;

		if (StartsWithAny(name, { "prop_air", "prop_byard", "prop_dock", "prop_rail", "prop_car", "prop_wheel", "prop_tyre", "port_", "sm_prop" }) ||
			ContainsAny(name, { "_vehicle", "_veh_", "_car_", "_truck", "_boat", "_yacht", "_plane", "_heli", "_wheel", "_tyre", "_dock", "_rail" }))
			return Vehicles_Parts;

		if (StartsWithAny(name, { "prop_chair", "prop_bench", "prop_table", "prop_couch", "prop_toilet", "prop_kitch", "prop_phone", "prop_speaker", "prop_parasol" }) ||
			ContainsAny(name, { "_chair", "_bench", "_table", "_couch", "_sofa", "_bed", "_stool", "_shelf", "_cabinet", "_desk", "_fridge", "_kitch" }))
			return FurnitureAndDecor;

		if (StartsWithAny(name, { "prop_beach", "prop_surf", "prop_boogieboard", "prop_skate", "prop_poolball", "prop_golf", "prop_yacht", "prop_snow" }) ||
			ContainsAny(name, { "_beach", "_surf", "_skate", "_golf", "_pool", "_snow", "_parasol", "_lounger" }))
			return BeachLeisureSports;

		if (StartsWithAny(name, { "prop_mask", "prop_tshirt", "prop_ped" }) || ContainsAny(name, { "_mask", "_shirt", "_helmet", "_hat", "_glasses", "_necklace", "_watch", "_wear" }))
			return CharacterWearables;

		if (StartsWithAny(name, { "des_", "p_", "prop_cs", "prop_ld", "miss_", "cs_" }) || ContainsAny(name, { "_heist", "_mission", "_script", "_cutscene" }))
			return Missions;

		if (StartsWithAny(name, { "vfx_", "dt_", "cs1_", "cs2_", "cs3_", "cs4_", "cs5_", "cs6_", "ch2_", "ch3_" }) || ContainsAny(name, { "_lod", "_slod", "_proxy", "_reflect", "_emissive" }))
			return VfxLod;

		if (StartsWith(name, "prop_"))
			return Props;

		return Specialized;
	}

	inline const char* GetObjectCategoryName(Category cat)
	{
		switch (cat)
		{
		case Props: return "Generic Props";
		case Interiors: return "Interiors/Shells";
		case WeaponsAndPickups: return "Weapons/Pickups";
		case FurnitureAndDecor: return "Furniture/Decor";
		case DoorsWindowsBuildings: return "Doors/Windows/Buildings";
		case BarriersFencesGates: return "Barriers/Fences/Gates";
		case SignsTrafficStreet: return "Signs/Traffic/Street";
		case NatureLandscape: return "Nature/Landscape";
		case ConstructionIndustrial: return "Construction/Industrial";
		case UtilityElectrical: return "Utility/Electrical";
		case StorageCargo: return "Storage/Cargo";
		case FoodDrinkRetail: return "Food/Drink/Retail";
		case TrashDebris: return "Trash/Debris";
		case Vehicles_Parts: return "Vehicles/Parts";
		case RacingStunt: return "Racing/Stunt";
		case BeachLeisureSports: return "Beach/Leisure/Sports";
		case LootValuables: return "Loot/Valuables";
		case CharacterWearables: return "Character/Wearables";
		case Missions: return "Mission/Scripted";
		case VfxLod: return "VFX/LOD/Proxy";
		case Specialized: return "Specialized/Other";
		default: return "Unknown";
		}
	}

	inline DlcGroup GetObjectDlcGroup(const std::string& modelName)
	{
		const std::string name = ToLower(modelName);

		if (StartsWith(name, "hei_")) return DLC_Heists;
		if (StartsWithAny(name, { "apa_", "ex_" })) return DLC_Executives;
		if (StartsWith(name, "imp_")) return DLC_ImportExport;
		if (StartsWith(name, "bkr_")) return DLC_Bikers;
		if (StartsWith(name, "gr_")) return DLC_Gunrunning;
		if (StartsWith(name, "sm_")) return DLC_SmugglersRun;
		if (StartsWithAny(name, { "xm_", "xm3_" })) return DLC_Doomsday;
		if (StartsWith(name, "ba_")) return DLC_AfterHours;
		if (StartsWithAny(name, { "ar_", "as_", "xs_" })) return DLC_ArenaWar;
		if (StartsWith(name, "vw_")) return DLC_DiamondCasino;
		if (StartsWith(name, "ch_")) return DLC_DiamondCasinoHeist;
		if (StartsWith(name, "h4_")) return DLC_CayoPerico;
		if (StartsWith(name, "lr_")) return DLC_Lowriders;
		if (StartsWithAny(name, { "sr_", "stt_" })) return DLC_SpecialRaces;
		if (StartsWith(name, "sum_")) return DLC_Nightclub;
		if (StartsWith(name, "sf_")) return DLC_Mercenaries;
		if (StartsWith(name, "m23_")) return DLC_Missions2023;
		if (StartsWith(name, "m24_")) return DLC_Missions2024;
		if (StartsWith(name, "m25_")) return DLC_Missions2025;
		if (StartsWithAny(name, { "tr_", "reh_", "lts_" })) return DLC_Other;

		return BaseGame;
	}

	inline const char* GetObjectDlcGroupName(DlcGroup group)
	{
		switch (group)
		{
		case BaseGame: return "Base Game/Core";
		case DLC_Heists: return "Heists";
		case DLC_Executives: return "Executives/Apartments";
		case DLC_ImportExport: return "Import/Export";
		case DLC_Bikers: return "Bikers";
		case DLC_Gunrunning: return "Gunrunning";
		case DLC_SmugglersRun: return "Smuggler's Run";
		case DLC_Doomsday: return "Doomsday/xm";
		case DLC_AfterHours: return "After Hours/Nightclub";
		case DLC_ArenaWar: return "Arena War";
		case DLC_DiamondCasino: return "Diamond Casino";
		case DLC_DiamondCasinoHeist: return "Casino Heist";
		case DLC_CayoPerico: return "Cayo Perico";
		case DLC_Lowriders: return "Lowriders";
		case DLC_SpecialRaces: return "Special/Stunt Races";
		case DLC_Nightclub: return "Nightclub/Business";
		case DLC_Mercenaries: return "Mercenaries/Sandy Shores";
		case DLC_Missions2023: return "Mission Props 2023";
		case DLC_Missions2024: return "Mission Props 2024";
		case DLC_Missions2025: return "Mission Props 2025";
		case DLC_Other: return "Other DLC";
		default: return "Unknown";
		}
	}

	enum StuntType
	{
		Stunt_None,
		Stunt_Ramps,
		Stunt_Tubes,
		Stunt_Loops,
		Stunt_Spirals,
		Stunt_Rings,
		Stunt_Checkpoints,
		Stunt_TrackPieces,
		Stunt_Wallrides,
		Stunt_BuildingBlocks,
		Stunt_BoostPads,
		Stunt_Barriers,
		Stunt_ArenaHazards,
		Stunt_StartFinish,
		Stunt_Pickups,
		Stunt_Targets,
		Stunt_Skatepark,
		Stunt_Count
	};

	inline StuntType GetStuntType(const std::string& modelName)
	{
		const std::string name = ToLower(modelName);

		if (ContainsAny(name, { "_ramp", "_jump", "_hloop", "_adj_loop", "_adj_flip" }) && !ContainsAny(name, { "_lod", "_slod" }))
			return Stunt_Ramps;
		if (ContainsAny(name, { "_tube", "_funnel", "_fn_" }))
			return Stunt_Tubes;
		if (ContainsAny(name, { "_loop", "multi_loop" }) && !ContainsAny(name, { "_tube", "_hloop" }))
			return Stunt_Loops;
		if (ContainsAny(name, { "spiral", "corkscrew", "helix" }))
			return Stunt_Spirals;
		if (ContainsAny(name, { "_ring", "_hoop", "_neon_gate", "inflategates" }) && !ContainsAny(name, { "firing", "mooring", "flooring", "string", "wiring", "steering" }))
			return Stunt_Rings;
		if (ContainsAny(name, { "checkpoint", "_cp_" }))
			return Stunt_Checkpoints;
		if (ContainsAny(name, { "track_straight", "track_bend", "track_slope", "track_chicane", "track_cross", "track_fork", "_dwslope", "_dwsh", "_dwuturn", "_dwlink", "_track_hill", "_track_bumps", "_track_turn", "_track_link", "_track_otake", "_track_cutout" }))
			return Stunt_TrackPieces;
		if (ContainsAny(name, { "wallride" }))
			return Stunt_Wallrides;
		if (ContainsAny(name, { "_bblock", "stunt_block", "_platform" }))
			return Stunt_BuildingBlocks;
		if (ContainsAny(name, { "boost", "speedup", "slowdown", "slow_down", "_speed" }) && ContainsAny(name, { "prop_ic", "prop_mk", "prop_mp", "stt_", "xs_", "ar_" }))
			return Stunt_BoostPads;
		if (ContainsAny(name, { "tyre_wall", "track_block", "track_stop", "_barrier" }) && StartsWithAny(name, { "stt_", "xs_", "ar_", "sr_" }))
			return Stunt_Barriers;
		if (ContainsAny(name, { "flipper", "landmine", "pit_fire", "_spikes", "_turret", "turntable", "pressure_plate", "wall_rising", "bollard_rising", "_wedge" }))
			return Stunt_ArenaHazards;
		if (ContainsAny(name, { "start_line", "finish_line", "startgate", "start_01", "race_gantry", "startline", "track_start" }))
			return Stunt_StartFinish;
		if (StartsWithAny(name, { "prop_ic_", "prop_mk_" }))
			return Stunt_Pickups;
		if (ContainsAny(name, { "_target" }) && StartsWithAny(name, { "sr_", "stt_" }))
			return Stunt_Targets;
		if (StartsWith(name, "prop_skate_"))
			return Stunt_Skatepark;

		return Stunt_None;
	}

	inline const char* GetStuntTypeName(StuntType type)
	{
		switch (type)
		{
		case Stunt_Ramps: return "Ramps/Jumps";
		case Stunt_Tubes: return "Tubes/Tunnels";
		case Stunt_Loops: return "Loops";
		case Stunt_Spirals: return "Spirals";
		case Stunt_Rings: return "Rings/Gates/Hoops";
		case Stunt_Checkpoints: return "Checkpoints";
		case Stunt_TrackPieces: return "Track Pieces";
		case Stunt_Wallrides: return "Wallrides";
		case Stunt_BuildingBlocks: return "Building Blocks";
		case Stunt_BoostPads: return "Boost/Speed Pads";
		case Stunt_Barriers: return "Barriers/Tyre Walls";
		case Stunt_ArenaHazards: return "Arena Hazards";
		case Stunt_StartFinish: return "Start/Finish Lines";
		case Stunt_Pickups: return "Pickups/Items";
		case Stunt_Targets: return "Targets";
		case Stunt_Skatepark: return "Skatepark";
		default: return "Not Stunt";
		}
	}

	inline bool IsLodProxy(const std::string& modelName)
	{
		const std::string name = ToLower(modelName);
		return ContainsAny(name, { "_lod", "_slod", "_proxy", "_reflect", "_shadow" });
	}

	enum SizeClass
	{
		Size_Unknown,
		Size_XXS,
		Size_XS,
		Size_S,
		Size_M,
		Size_L,
		Size_XL,
		Size_XXL,
		Size_Huge,
		Size_Count
	};

	inline SizeClass GetNamedSize(const std::string& modelName)
	{
		const std::string name = ToLower(modelName);

		if (ContainsAny(name, { "_xxs" })) return Size_XXS;
		if (ContainsAny(name, { "_xxl" })) return Size_XXL;
		if (ContainsAny(name, { "_huge" })) return Size_Huge;

		if (name.length() > 2)
		{
			if (ContainsAny(name, { "_xs" }) && !ContainsAny(name, { "_xs_0", "txs", "exs" })) return Size_XS;
			if (ContainsAny(name, { "_xl" }) && !ContainsAny(name, { "_xl_0", "txl" })) return Size_XL;

			if (ContainsAny(name, { "_sml" , "_small" })) return Size_S;
			if (ContainsAny(name, { "_mdm", "_medium" })) return Size_M;
			if (ContainsAny(name, { "_lrg", "_large" })) return Size_L;

			size_t len = name.length();
			if (len >= 2)
			{
				std::string last2 = name.substr(len - 2);
				if (last2 == "_s" || last2 == "sb") return Size_S;
				if (last2 == "_m" || last2 == "mb") return Size_M;
				if (last2 == "_l" || last2 == "lb") return Size_L;
			}
		}

		return Size_Unknown;
	}

	inline const char* GetSizeClassName(SizeClass size)
	{
		switch (size)
		{
		case Size_XXS: return "XXS";
		case Size_XS: return "XS";
		case Size_S: return "S";
		case Size_M: return "M";
		case Size_L: return "L";
		case Size_XL: return "XL";
		case Size_XXL: return "XXL";
		case Size_Huge: return "Huge";
		default: return "";
		}
	}
}
