// Auto-generated from root-cause gist. Do not edit manually.
#include "MansionInteriors.h"

#include "..\..\macros.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Routine.h"
#include "..\..\Natives\natives2.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Scripting\GTAped.h"
#include "TeleLocation.h"
#include "TeleMethods.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

namespace sub::TeleportLocations_catind
{
	namespace MansionInteriors
	{
		struct MansionInteriorOption
		{
			std::string name;
			std::string value;
			int maxTints;
		};

		// All estates have defined IPLs for:
		// - loading the mansion and terrain
		// - unloading SP underlying terrain
		// - loading the shell (generic is without interior visible from outside, private is with interior)
		// - loading the railings (for private shell)
		// - loading the shutters (for private shell)
		// - loading the custom IPLs

		// The Tongva Estate IPLs
		const std::vector<std::string> vTheTongvaEstateLoad{
			"m25_2_ch1_09_mansion_interior_a", "m25_2_ch1_09_mansion_interior_b", "m25_2_ch1_09_mansion_interior_c", "hei_ch1_09_mansion_shared", "hei_ch1_09_mansion_shared_distantlights", "hei_ch1_09_mansion_shared_lodlights", "hei_ch1_09_mansion_private_distantlights", "hei_ch1_09_mansion_private_lodlights", "hei_ch1_09_mansion_player_bounds", "hei_ch1_09_mansion_firepit_distantlights", "hei_ch1_09_mansion_firepit_lodlights", "m25_2_knoway_sign", "m25_2_legacy_fixes", "m25_2_payphones", "m25_2_mansion_props"};
		const std::vector<std::string> vTheTongvaEstateRemove{
			"hei_ch1_09_mansion_original", "hei_ch1_09_mansion_original_distantlights", "hei_ch1_09_mansion_original_lodlights", "hei_ch1_09_props_original", "hei_ch1_09_props_original_distantlights", "hei_ch1_09_props_original_lodlights"};
		const std::vector<std::string> vTheTongvaEstateShell{
			"hei_ch1_09_mansion_generic", "hei_ch1_09_mansion_private"};
		const std::vector<std::string> vTheTongvaEstateRailing{
			"hei_ch1_09_mansion_railings_m", "hei_ch1_09_mansion_railings_p"};
		const std::vector<std::string> vTheTongvaEstateShutters{
			"hei_ch1_09_mansion_shutters"};
		const std::vector<std::string> vTheTongvaEstateCustom{
			"hei_ch1_09_mansion_furniture", "hei_ch1_09_mansion_firepit", "m25_2_tongva_mansion_gym", "m25_2_tongva_dog_house"};

		// The Vinewood Residence IPLs
		const std::vector<std::string> vTheVinewoodResidenceLoad{
			"m25_2_ch2_04_mansion_interior_a", "m25_2_ch2_04_mansion_interior_b", "m25_2_ch2_04_mansion_interior_c", "apa_ch2_04_mansion_shared", "apa_ch2_04_mansion_shared_distantlights", "apa_ch2_04_mansion_shared_lodlights", "apa_ch2_04_mansion_private_distantlights", "apa_ch2_04_mansion_private_lodlights", "apa_ch2_04_mansion_player_bounds", "apa_ch2_04_mansion_firepit_distantlights", "apa_ch2_04_mansion_firepit_lodlights", "m25_2_knoway_sign", "m25_2_legacy_fixes", "m25_2_payphones", "m25_2_mansion_props"};
		const std::vector<std::string> vTheVinewoodResidenceRemove{
			"apa_ch2_04_mansion_original", "apa_ch2_04_props_original", "apa_ch2_04_mansion_grass"};
		const std::vector<std::string> vTheVinewoodResidenceShell{
			"apa_ch2_04_mansion_generic", "apa_ch2_04_mansion_private"};
		const std::vector<std::string> vTheVinewoodResidenceRailing{
			"apa_ch2_04_mansion_railings_m", "apa_ch2_04_mansion_railings_p"};
		const std::vector<std::string> vTheVinewoodResidenceShutters{
			"apa_ch2_04_mansion_shutters"};
		const std::vector<std::string> vTheVinewoodResidenceCustom{
			"apa_ch2_04_mansion_furniture", "apa_ch2_04_mansion_firepit", "m25_2_east_mansion_gym", "m25_2_east_dog_house"};

		// Richman Villa IPLs
		const std::vector<std::string> vRichmanVillaLoad{
			"m25_2_ch1_06e_mansion_interior_a", "m25_2_ch1_06e_mansion_interior_b", "m25_2_ch1_06e_mansion_interior_c", "m25_2_ch1_06e_mansion_interior_d", "hei_ch1_06e_mansion_shared", "hei_ch1_06e_mansion_shared_distantlights", "hei_ch1_06e_mansion_shared_lodlights", "hei_ch1_06f_mansion_shared", "hei_ch1_06e_mansion_private_distantlights", "hei_ch1_06e_mansion_private_lodlights", "hei_ch1_roads_mansion", "hei_ch1_06e_mansion_player_bounds", "hei_ch1_06e_mansion_firepit_distantlights", "hei_ch1_06e_mansion_firepit_lodlights", "m25_2_knoway_sign", "m25_2_legacy_fixes", "m25_2_payphones", "m25_2_mansion_props"};
		const std::vector<std::string> vRichmanVillaRemove{
			"hei_ch1_06e_mansion_original", "hei_ch1_06f_mansion_original", "hei_ch1_06e_props_original", "hei_ch1_roads_original"};
		const std::vector<std::string> vRichmanVillaShell{
			"hei_ch1_06e_mansion_generic", "hei_ch1_06e_mansion_private"};
		const std::vector<std::string> vRichmanVillaRailing{
			"hei_ch1_06e_mansion_railings_m", "hei_ch1_06e_mansion_railings_p"};
		const std::vector<std::string> vRichmanVillaShutters{
			"hei_ch1_06e_mansion_shutters"};
		const std::vector<std::string> vRichmanVillaCustom{
			"hei_ch1_06e_mansion_furniture", "hei_ch1_06e_mansion_firepit", "m25_2_mansion_gym", "m25_2_dog_house"};

		const std::vector<MansionInteriorOption> vblockersOptions{
			{"None", "", 0},
			{"Arcade", "SET_ARCADE_BLOCKER", 0},
			{"Loft Arcade", "SET_LOFT_ARCADE_BLOCKER", 0},
			{"Loft Podium", "SET_LOFT_PODIUM_BLOCKER", 0},
			{"Podium", "SET_PODIUM_BLOCKER", 0},
			{"Regency Arcade", "SET_REG_ARCADE_BLOCKER", 0},
			{"Regency Podium", "SET_REG_PODIUM_BLOCKER", 0}};

		const std::vector<MansionInteriorOption> vseasonalOptions{
			{"None", "", 0},
			{"Afterparty", "SET_AFTERPARTY", 0},
			{"Birthday", "SET_BIRTHDAY", 0},
			{"Halloween", "SET_HALLOWEEN", 0},
			{"Lunar", "SET_LUNAR", 0},
			{"Xmas", "SET_XMAS", 0}};

		const std::vector<MansionInteriorOption> vstyleOptions{
			{"None", "", 0},
			{"San Andreas Coastal", "SET_STYLE_CALI", 0},
			{"Vinewood Regency", "SET_STYLE_HOLLY", 0},
			{"Los Santos Loft", "SET_STYLE_LOFT", 0}};

		const std::vector<MansionInteriorOption> vpetsOptions{
			{"None", "", 0},
			{"Cat", "SET_PET_CAT", 0},
			{"Dog", "SET_PET_DOG", 0}};

		const std::vector<MansionInteriorOption> vwallpaperOptions{
			{"None", "", 0},
			{"Coastal", "SET_WALLPAPER_COASTAL", 0},
			{"Deco", "SET_WALLPAPER_DECO", 0},
			{"Pop Art", "SET_WALLPAPER_POPART", 0},
			{"Rustic", "SET_WALLPAPER_RUSTIC", 0},
			{"Safari", "SET_WALLPAPER_SAFARI", 0},
			{"Subtle", "SET_WALLPAPER_SUBTLE", 0}};

		const std::vector<MansionInteriorOption> vassistantOptions{
			{"None", "", 0},
			{"OG", "SET_AI_TABLETS_01", 0},
			{"Haviland", "SET_AI_TABLETS_02", 0},
			{"Angel", "SET_AI_TABLETS_03", 0}};

		const std::vector<MansionInteriorOption> vartOptions{
			{"None", "", 0},
			{"San Andreas Coastal", "SET_ART_COASTAL", 0},
			{"Los Santos Loft", "SET_ART_LOFT", 0},
			{"Vinewood Regency", "SET_ART_REGENCY", 0}};

		const std::vector<MansionInteriorOption> vmichaelOptions{
			{"None", "", 0},
			{"Card", "SET_MICHAEL_CARD", 0},
			{"Poster", "SET_MICHAEL_POSTER", 0}};

		const std::vector<MansionInteriorOption> velevatorOptions{
			{"None", "", 0},
			{"San Andreas Coastal", "SET_ELEV_CALI", 0},
			{"Los Santos Loft", "SET_ELEV_LOFT", 0},
			{"Vinewood Regency", "SET_ELEV_HOLLY", 0}};

		const std::vector<MansionInteriorOption> vtrophiesOptions{
			{"None", "", 0},
			{"Loft Shelving", "SET_LOFT_SHELVING_PLANTER", 0},
			{"Loft Planter", "SET_LOFT_TROPHY_PLANTER", 0},
			{"Regency Shelving", "SET_REG_SHELVING_PLANTER", 0},
			{"Regency Planter", "SET_REG_TROPHY_PLANTER", 0},
			{"Shelving Planter", "SET_SHELVING_PLANTER", 0},
			{"Planter", "SET_TROPHY_PLANTER", 0},
			{"Shelves", "SET_TROPHY_SHELVES", 0}};

		const std::vector<MansionInteriorOption> vLowerVaultOptions{
			{"None", "", 0},
			{"Vault 00", "SET_BASE_VAULT_00", 1},
			{"Vault 01", "SET_BASE_VAULT_01", 1},
			{"Vault 02", "SET_BASE_VAULT_02", 1},
			{"Vault 03", "SET_BASE_VAULT_03", 1},
			{"Vault 04", "SET_BASE_VAULT_04", 1},
			{"Vault 05", "SET_BASE_VAULT_05", 1},
			{"Vault 06", "SET_BASE_VAULT_06", 1},
			{"Vault 07", "SET_BASE_VAULT_07", 1},
			{"Vault 08", "SET_BASE_VAULT_08", 1},
			{"Vault 09", "SET_BASE_VAULT_09", 1}};

		const std::vector<MansionInteriorOption> vLowerExtrasOptions{
			{"None", "", 0},
			{"ACCESS_BLOCKER", "SET_ACCESS_BLOCKER", 1},
			{"ARMORY_BLOCKER", "SET_ARMORY_BLOCKER", 1},
			{"AI_TABLETS", "SET_BASE_AI_TABLETS", 1},
			{"AI_TABLETS_01", "SET_BASE_AI_TABLETS_01", 1},
			{"AI_TABLETS_02", "SET_BASE_AI_TABLETS_02", 1},
			{"AI_TABLETS_03", "SET_BASE_AI_TABLETS_03", 1},
			{"HALLOWEEN", "SET_BASE_HALLOWEEN", 1},
			{"NEW_YEAR", "SET_BASE_NEW_YEAR", 1},
			{"XMAS", "SET_BASE_XMAS", 1},
			{"ELEV_STD", "SET_ELEV_STD", 1},
			{"MOD_BLOCKER", "SET_MOD_BLOCKER", 1},
			{"VAULT_DOOR_CLOSED", "SET_VAULT_DOOR_CLOSED", 1},
			{"VAULT_DOOR_OPEN", "SET_VAULT_DOOR_OPEN", 1}};

		const std::vector<MansionInteriorOption> vGarageOptions{
			{"None", "", 0},
			{"AI_TABLETS", "SET_GAR_AI_TABLETS", 1},
			{"AI_TABLETS_01", "SET_GAR_AI_TABLETS_01", 1},
			{"AI_TABLETS_02", "SET_GAR_AI_TABLETS_02", 1},
			{"AI_TABLETS_03", "SET_GAR_AI_TABLETS_03", 1},
			{"MOD_BLOCKER", "SET_GAR_MOD_BLOCKER", 1},
			{"PODIUM_BLOCKER", "SET_GAR_PODIUM_BLOCKER", 1}};

		struct MansionLocation
		{
			const std::string name;
			Vector3 posMain;
			Vector3 posLower;
			Vector3 posGarage;
			std::vector<std::string> iplToLoad;
			std::vector<std::string> iplToRemove;
			std::vector<std::string> iplShell;
			std::vector<std::string> iplRailing;
			std::vector<std::string> iplShutters;
			std::vector<std::string> iplCustom;
		};

		const std::vector<MansionLocation> vLocations{
			{"Richman Villa", {-1666.36f, 478.92f, 128.22f}, {-1649.63f, 480.97f, 117.36f}, {-1679.87f, 493.59f, 56.45f}, vRichmanVillaLoad, vRichmanVillaRemove, vRichmanVillaShell, vRichmanVillaRailing, vRichmanVillaShutters, vRichmanVillaCustom},
			{"The Vinewood Residence", {539.7012f, 749.089f, 201.36f}, {547.4955f, 734.136f, 190.5f}, {548.6964f, 766.88f, 186.07f}, vTheVinewoodResidenceLoad, vTheVinewoodResidenceRemove, vTheVinewoodResidenceShell, vTheVinewoodResidenceRailing, vTheVinewoodResidenceShutters, vTheVinewoodResidenceCustom},
			{"The Tongva Estate", {-2586.065f, 1909.995f, 166.37f}, {-2587.495f, 1893.193f, 155.51f}, {-2568.933f, 1920.202f, 151.08f}, vTheTongvaEstateLoad, vTheTongvaEstateRemove, vTheTongvaEstateShell, vTheTongvaEstateRailing, vTheTongvaEstateShutters, vTheTongvaEstateCustom}};
		struct MansionInfoStructure
		{
			MansionLocation const *location = nullptr;
			int shellType = 1;
			int railingsType = 0;
			struct MansionInteriorOptionIndex
			{
				int index = 0;
			};
			MansionInteriorOptionIndex blockersOption;
			MansionInteriorOptionIndex seasonalOption;
			MansionInteriorOptionIndex styleOption{1};
			MansionInteriorOptionIndex petsOption;
			MansionInteriorOptionIndex wallpaperOption{1};
			MansionInteriorOptionIndex assistantOption;
			MansionInteriorOptionIndex artOption;
			MansionInteriorOptionIndex michaelOption;
			MansionInteriorOptionIndex elevatorOption;
			MansionInteriorOptionIndex trophiesOption;
			MansionInteriorOptionIndex lowerVaultOption;

			MansionInteriorOptionIndex lowerExtrasOption;

			MansionInteriorOptionIndex garageOption;
			int mansionTint = 0;
			bool shuttersLoaded = false;
			bool needsBuild = true;
		};
		MansionInfoStructure currentMansionInfo{};

		struct MansionInteriorOptionArray
		{
			std::string name;
			MansionInfoStructure::MansionInteriorOptionIndex *ptr;
			const std::vector<MansionInteriorOption> *arr;
			int usePos;
		};
		std::map<MansionLocation const *, MansionInfoStructure> s_SavedSettings;
		std::vector<MansionInteriorOptionArray> vOptionArrays{
			{"Blockers", &currentMansionInfo.blockersOption, &vblockersOptions, 0},
			{"Seasonal", &currentMansionInfo.seasonalOption, &vseasonalOptions, 0},
			{"Style (Decor)", &currentMansionInfo.styleOption, &vstyleOptions, 0}, // UpdateMansionProp depends on this being third element in the array, do not change the order
			{"Wallpaper (Patterns)", &currentMansionInfo.wallpaperOption, &vwallpaperOptions, 0},
			{"Assistant", &currentMansionInfo.assistantOption, &vassistantOptions, 0},
			{"Elevator", &currentMansionInfo.elevatorOption, &velevatorOptions, 0},
			{"Art", &currentMansionInfo.artOption, &vartOptions, 0},
			{"Pets", &currentMansionInfo.petsOption, &vpetsOptions, 0},
			{"Michael", &currentMansionInfo.michaelOption, &vmichaelOptions, 0},
			{"Trophies", &currentMansionInfo.trophiesOption, &vtrophiesOptions, 0},
			{"Lower Vault", &currentMansionInfo.lowerVaultOption, &vLowerVaultOptions, 1},
			{"Lower Extras", &currentMansionInfo.lowerExtrasOption, &vLowerExtrasOptions, 1},
			{"Garage", &currentMansionInfo.garageOption, &vGarageOptions, 2},
		};
		MansionInteriorOptionArray *selectedOptionArray = nullptr;

		void HandleStyleTint(int interior, const std::string &styleSet, int tint)
		{
			DEACTIVATE_INTERIOR_ENTITY_SET(interior, "SET_STYLE_CALI_TINT");
			DEACTIVATE_INTERIOR_ENTITY_SET(interior, "SET_STYLE_LOFT_TINT");
			DEACTIVATE_INTERIOR_ENTITY_SET(interior, "SET_STYLE_REG_TINT");

			const char *tintSet = nullptr;
			if (styleSet == "SET_STYLE_CALI")
				tintSet = "SET_STYLE_CALI_TINT";
			else if (styleSet == "SET_STYLE_LOFT")
				tintSet = "SET_STYLE_LOFT_TINT";
			else if (styleSet == "SET_STYLE_HOLLY")
				tintSet = "SET_STYLE_REG_TINT";
			else
				return;

			ACTIVATE_INTERIOR_ENTITY_SET(interior, tintSet);
			for (DWORD timeOut = GetTickCount() + 250; GetTickCount() < timeOut;)
			{
				if (IS_INTERIOR_ENTITY_SET_ACTIVE(interior, tintSet))
					break;
				WAIT(0);
			}
			SET_INTERIOR_ENTITY_SET_TINT_INDEX(interior, tintSet, tint);
		}
		void CreateMansion(MansionInfoStructure &info)
		{
			if (info.location == nullptr)
				return;
			auto &loc = *info.location;

			SET_INSTANCE_PRIORITY_MODE(true);
			ON_ENTER_MP();

			// remove SP IPLs
			for (auto &ipl : loc.iplToRemove)
				if (IS_IPL_ACTIVE(ipl.c_str()))
					REMOVE_IPL(ipl.c_str());

			// load MP IPLs
			for (auto &ipl : loc.iplToLoad)
				if (!IS_IPL_ACTIVE(ipl.c_str()))
					REQUEST_IPL(ipl.c_str());

			// load shell IPL
			if (!loc.iplShell.empty() && info.shellType < loc.iplShell.size())
			{
				const auto &selectedShell = loc.iplShell[info.shellType];
				if (!IS_IPL_ACTIVE(selectedShell.c_str()))
				{
					for (auto &s : loc.iplShell)
						if (IS_IPL_ACTIVE(s.c_str()))
							REMOVE_IPL(s.c_str());
					REQUEST_IPL(selectedShell.c_str());
				}
			}
			// load IPLs for private shell
			if (info.shellType == 1)
			{
				if (info.railingsType < loc.iplRailing.size())
				{
					const auto &selectedRailing = loc.iplRailing[info.railingsType];
					if (!IS_IPL_ACTIVE(selectedRailing.c_str()))
					{
						for (auto &r : loc.iplRailing)
							if (IS_IPL_ACTIVE(r.c_str()))
								REMOVE_IPL(r.c_str());
						REQUEST_IPL(selectedRailing.c_str());
					}
				}
				for (auto &s : loc.iplShutters)
				{
					if (info.shuttersLoaded)
					{
						if (!IS_IPL_ACTIVE(s.c_str()))
							REQUEST_IPL(s.c_str());
					}
					else
					{
						if (IS_IPL_ACTIVE(s.c_str()))
							REMOVE_IPL(s.c_str());
					}
				}
				for (auto &c : loc.iplCustom)
					if (!IS_IPL_ACTIVE(c.c_str()))
						REQUEST_IPL(c.c_str());
			}
			// remove all railings, shutters, and custom IPLs if not using private shell
			else
			{
				for (auto &r : loc.iplRailing)
					if (IS_IPL_ACTIVE(r.c_str()))
						REMOVE_IPL(r.c_str());
				for (auto &s : loc.iplShutters)
					if (IS_IPL_ACTIVE(s.c_str()))
						REMOVE_IPL(s.c_str());
				for (auto &c : loc.iplCustom)
					if (IS_IPL_ACTIVE(c.c_str()))
						REMOVE_IPL(c.c_str());
			}

			struct
			{
				Vector3 pos;
				int usePos;
			} levels[] = {
				{loc.posMain, 0},
				{loc.posLower, 1},
				{loc.posGarage, 2},
			};
			// update interiors for each level (main, lower, garage)
			for (auto &lvl : levels)
			{
				int interior = GET_INTERIOR_AT_COORDS(lvl.pos.x, lvl.pos.y, lvl.pos.z);
				if (!IS_VALID_INTERIOR(interior))
					continue;
				if (IS_INTERIOR_DISABLED(interior))
				{
					PIN_INTERIOR_IN_MEMORY(interior);
					SET_INTERIOR_ACTIVE(interior, true);
					DISABLE_INTERIOR(interior, false);
				}

				for (auto &oa : vOptionArrays)
				{
					if (oa.usePos != lvl.usePos)
						continue;
					if (oa.ptr == nullptr)
						continue;

					for (auto &p : *oa.arr)
						if (!p.value.empty())
							DEACTIVATE_INTERIOR_ENTITY_SET(interior, p.value.c_str());

					auto &selected = oa.arr->at(oa.ptr->index);
					if (!selected.value.empty())
					{
						ACTIVATE_INTERIOR_ENTITY_SET(interior, selected.value.c_str());
					}
				}

				if (lvl.usePos == 0)
					HandleStyleTint(interior, vstyleOptions[info.styleOption.index].value, info.mansionTint);

				REFRESH_INTERIOR(interior);
			}

			SET_INSTANCE_PRIORITY_MODE(false);
		}
		void UpdateMansionProp(MansionInfoStructure &info, const MansionInteriorOptionArray &arr)
		{
			if (info.location == nullptr || arr.ptr == nullptr || arr.arr == nullptr)
				return;
			auto &loc = *info.location;
			Vector3 pos;
			if (arr.usePos == 1)
				pos = loc.posLower;
			else if (arr.usePos == 2)
				pos = loc.posGarage;
			else
				pos = loc.posMain;

			int interior = GET_INTERIOR_AT_COORDS(pos.x, pos.y, pos.z);
			if (!IS_VALID_INTERIOR(interior))
				return;
			if (IS_INTERIOR_DISABLED(interior))
			{
				PIN_INTERIOR_IN_MEMORY(interior);
				SET_INTERIOR_ACTIVE(interior, true);
				DISABLE_INTERIOR(interior, false);
			}

			for (auto &p : *arr.arr)
				if (!p.value.empty())
					DEACTIVATE_INTERIOR_ENTITY_SET(interior, p.value.c_str());

			auto &selected = arr.arr->at(arr.ptr->index);
			if (!selected.value.empty())
			{
				ACTIVATE_INTERIOR_ENTITY_SET(interior, selected.value.c_str());
			}

			if (&arr == &vOptionArrays[2])
				HandleStyleTint(interior, selected.value, info.mansionTint);
			REFRESH_INTERIOR(interior);
		}
		void Sub_MansionInteriors()
		{
			AddTitle("Mansions");

			for (auto &loc : vLocations)
			{
				bool bPressed = false;
				AddOption(loc.name, bPressed, nullFunc, SUB::TELEPORTOPS_MANSIONS_INLOC);
				if (bPressed)
				{
					if (currentMansionInfo.location != nullptr)
						s_SavedSettings[currentMansionInfo.location] = currentMansionInfo;
					auto it = s_SavedSettings.find(&loc);
					currentMansionInfo = it != s_SavedSettings.end() ? it->second : MansionInfoStructure{};
					currentMansionInfo.location = &loc;
				}
			}

			AddBreak("---Quick Teleport---");
			for (auto &loc : vLocations)
			{
				struct
				{
					std::string label;
					Vector3 pos;
				} zones[] = {
					{"Main Interior", loc.posMain},
					{"Lower Level", loc.posLower},
					{"Garage", loc.posGarage},
				};
				for (auto &z : zones)
				{
					std::string label = loc.name + " - " + z.label;
					bool bTelePressed = false;
					AddOption(label, bTelePressed);
					if (bTelePressed)
					{
						SET_INSTANCE_PRIORITY_MODE(true);
						ON_ENTER_MP();
						int interior = GET_INTERIOR_AT_COORDS(z.pos.x, z.pos.y, z.pos.z);
						if (IS_VALID_INTERIOR(interior) && IS_INTERIOR_DISABLED(interior))
						{
							PIN_INTERIOR_IN_MEMORY(interior);
							SET_INTERIOR_ACTIVE(interior, true);
							DISABLE_INTERIOR(interior, false);
							REFRESH_INTERIOR(interior);
						}
						SET_INSTANCE_PRIORITY_MODE(false);
						TeleportNetPed(PLAYER_PED_ID(), z.pos.x, z.pos.y, z.pos.z);
					}
				}
			}
		}
		void Sub_MansionInteriors_InLoc()
		{
			if (currentMansionInfo.location == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}
			if (currentMansionInfo.needsBuild)
			{
				DO_SCREEN_FADE_OUT(50);
				CreateMansion(currentMansionInfo);
				DO_SCREEN_FADE_IN(200);
				currentMansionInfo.needsBuild = false;
			}
			GTAped ped = g_activePedHandle;
			AddTitle(currentMansionInfo.location->name);

			{
				bool bPlus = false, bMinus = false;
				AddTexter("Shell", 0, std::vector<std::string>{currentMansionInfo.shellType == 0 ? "Generic" : "Private"}, null, bPlus, bMinus);
				if (bPlus && currentMansionInfo.shellType < 1)
				{
					currentMansionInfo.shellType++;
					DO_SCREEN_FADE_OUT(50);
					CreateMansion(currentMansionInfo);
					DO_SCREEN_FADE_IN(200);
				}
				if (bMinus && currentMansionInfo.shellType > 0)
				{
					currentMansionInfo.shellType--;
					DO_SCREEN_FADE_OUT(50);
					CreateMansion(currentMansionInfo);
					DO_SCREEN_FADE_IN(200);
				}
			}
			if (currentMansionInfo.shellType == 1)
			{
				bool bPlus = false, bMinus = false;
				AddTexter("Railings", 0, std::vector<std::string>{currentMansionInfo.railingsType == 0 ? "M" : "P"}, null, bPlus, bMinus);
				if (bPlus && currentMansionInfo.railingsType < 1)
				{
					currentMansionInfo.railingsType++;
					DO_SCREEN_FADE_OUT(50);
					CreateMansion(currentMansionInfo);
					DO_SCREEN_FADE_IN(200);
				}
				if (bMinus && currentMansionInfo.railingsType > 0)
				{
					currentMansionInfo.railingsType--;
					DO_SCREEN_FADE_OUT(50);
					CreateMansion(currentMansionInfo);
					DO_SCREEN_FADE_IN(200);
				}
				{
					bool bShutOn = false, bShutOff = false;
					AddTickol("Exterior Shutters", currentMansionInfo.shuttersLoaded, bShutOn, bShutOff, TICKOL::BOXTICK, TICKOL::BOXBLANK);
					if (bShutOn || bShutOff)
					{
						currentMansionInfo.shuttersLoaded = bShutOn;
						DO_SCREEN_FADE_OUT(50);
						CreateMansion(currentMansionInfo);
						DO_SCREEN_FADE_IN(200);
					}
				}
			}

			for (auto &o : vOptionArrays)
			{
				if (!o.name.empty() && o.ptr != nullptr)
				{
					bool bOption_plus = false, bOption_minus = false, bOption_pressed = false;
					AddTexter(o.name, 0, std::vector<std::string>{o.arr->at(o.ptr->index).name}, bOption_pressed, bOption_plus, bOption_minus);
					if (bOption_plus)
					{
						if (o.ptr->index < o.arr->size() - 1)
						{
							(o.ptr->index)++;
							UpdateMansionProp(currentMansionInfo, o);
						}
					}
					if (bOption_minus)
					{
						if (o.ptr->index > 0)
						{
							(o.ptr->index)--;
							UpdateMansionProp(currentMansionInfo, o);
						}
					}
					if (bOption_pressed)
					{
						selectedOptionArray = &o;
						Menu::pendingSubmenu = SUB::TELEPORTOPS_MANSIONS_INOPTION;
					}
				}
			}
			if (currentMansionInfo.styleOption.index > 0)
			{
				int oldTint = currentMansionInfo.mansionTint;
				AddNumberStepper("Tint", currentMansionInfo.mansionTint, 0, 1.0, 0, 3);
				if (currentMansionInfo.mansionTint != oldTint)
				{
					Vector3 pos = currentMansionInfo.location->posMain;
					int interior = GET_INTERIOR_AT_COORDS(pos.x, pos.y, pos.z);
					if (IS_VALID_INTERIOR(interior))
					{
						HandleStyleTint(interior, vstyleOptions[currentMansionInfo.styleOption.index].value, currentMansionInfo.mansionTint);
						REFRESH_INTERIOR(interior);
					}
				}
			}

			AddBreak("---Actions---");
			auto &loc = *currentMansionInfo.location;
			const char *teleLabels[] = {"Main Interior", "Lower Level", "Garage"};
			Vector3 telePos[] = {loc.posMain, loc.posLower, loc.posGarage};
			for (int i = 0; i < 3; i++)
			{
				bool bPressed = false;
				AddOption(teleLabels[i], bPressed);
				if (bPressed)
					TeleportNetPed(ped, telePos[i]);
			}
		}
		void Sub_MansionInteriors_InOption()
		{
			if (currentMansionInfo.location == nullptr || selectedOptionArray == nullptr || selectedOptionArray->ptr == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}
			AddTitle(selectedOptionArray->name.empty() ? "Option" : selectedOptionArray->name);

			for (UINT i = 0; i < selectedOptionArray->arr->size(); i++)
			{
				auto &o = selectedOptionArray->arr->at(i);
				auto &ptr = selectedOptionArray->ptr;
				bool isSelected = ptr->index == i;
				bool bOpPressed = false;
				AddTickol(o.name, isSelected, bOpPressed, bOpPressed);
				if (bOpPressed)
				{
					ptr->index = i;
					UpdateMansionProp(currentMansionInfo, *selectedOptionArray);
				}
			}
		}
	}
}

#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(TELEPORTOPS_MANSIONS, sub::TeleportLocations_catind::MansionInteriors::Sub_MansionInteriors)
REGISTER_SUBMENU(TELEPORTOPS_MANSIONS_INLOC, sub::TeleportLocations_catind::MansionInteriors::Sub_MansionInteriors_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_MANSIONS_INOPTION, sub::TeleportLocations_catind::MansionInteriors::Sub_MansionInteriors_InOption)
