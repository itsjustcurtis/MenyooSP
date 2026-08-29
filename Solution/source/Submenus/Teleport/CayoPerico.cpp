#include "CayoPerico.h"

#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"

#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\GTAped.h"

#include "TeleMethods.h"
#include "TeleLocation.h"

namespace sub::TeleportLocations_catind
{
	namespace CayoPerico
	{
		bool bCayoPericoEnabled = false;

		const struct CayoPericoLocation
		{
			Vector3 pos;
			const std::string name;
		};

		// we are not using IPLs to load the island, but I'm keeping this for future reference
		const std::vector<const char*> vCayoPericoIPLs
		{
			"h4_aa_guns",
			"h4_aa_guns_lod",
			"h4_beach",
			"h4_beach_lod",
			"h4_beach_slod",
			"h4_beach_props",
			"h4_beach_props_lod",
			"h4_beach_props_slod",
			"h4_beach_party",
			"h4_beach_props_party",
			"h4_beach_party_lod",
			"h4_beach_bar_props",
			"h4_island_padlock_props",
			"h4_islandx_barrack_hatch",
			"h4_islandx_barrack_props",
			"h4_islandx_barrack_props_lod",
			"h4_islandx_barrack_props_slod",
			"h4_islandxcanal_props",
			"h4_islandxcanal_props_lod",
			"h4_islandxcanal_props_slod",
			"h4_islandx_mansion_entrance_fence",
			"h4_islandx_mansion_guardfence",
			"h4_islandx_mansion_lights",
			"h4_islandx_mansion_lod",
			"h4_islandx_mansion_slod",
			"h4_islandx_mansion_b",
			"h4_islandx_mansion_b_side_fence",
			"h4_islandx_mansion_b_lod",
			"h4_islandx_mansion_b_slod",
			"h4_islandx_mansion_props",
			"h4_islandx_mansion_props_lod",
			"h4_islandx_mansion_props_slod",
			"h4_islandx_mansion_vault",
			"h4_islandx_mansion_vault_lod",
			"h4_islandx_mansion_lockup_01",
			"h4_islandx_mansion_lockup_01_lod",
			"h4_islandx_mansion_lockup_02",
			"h4_islandx_mansion_lockup_02_lod",
			"h4_islandx_mansion_lockup_03",
			"h4_islandx_mansion_lockup_03_lod",
			"h4_islandx_mansion_office",
			"h4_islandx_mansion_office_lod",
			"h4_islandx",
			"h4_islandx_props",
			"h4_islandx_props_lod",
			"h4_islandx_terrain_01",
			"h4_islandx_terrain_02",
			"h4_islandx_terrain_03",
			"h4_islandx_terrain_04",
			"h4_islandx_terrain_05",
			"h4_islandx_terrain_06",
			"h4_islandx_terrain_props_05_a",
			"h4_islandx_terrain_props_05_b",
			"h4_islandx_terrain_props_05_c",
			"h4_islandx_terrain_props_05_d",
			"h4_islandx_terrain_props_05_e",
			"h4_islandx_terrain_props_05_f",
			"h4_islandx_terrain_props_06_a",
			"h4_islandx_terrain_props_06_b",
			"h4_islandx_terrain_props_06_c",
			"h4_islandx_terrain_01_lod",
			"h4_islandx_terrain_02_lod",
			"h4_islandx_terrain_03_lod",
			"h4_islandx_terrain_04_lod",
			"h4_islandx_terrain_05_lod",
			"h4_islandx_terrain_06_lod",
			"h4_islandx_terrain_props_05_a_lod",
			"h4_islandx_terrain_props_05_b_lod",
			"h4_islandx_terrain_props_05_c_lod",
			"h4_islandx_terrain_props_05_d_lod",
			"h4_islandx_terrain_props_05_e_lod",
			"h4_islandx_terrain_props_05_f_lod",
			"h4_islandx_terrain_props_06_a_lod",
			"h4_islandx_terrain_props_06_b_lod",
			"h4_islandx_terrain_props_06_c_lod",
			"h4_islandx_terrain_01_slod",
			"h4_islandx_terrain_02_slod",
			"h4_islandx_terrain_04_slod",
			"h4_islandx_terrain_05_slod",
			"h4_islandx_terrain_06_slod",
			"h4_islandx_terrain_props_05_d_slod",
			"h4_islandx_terrain_props_05_e_slod",
			"h4_islandx_terrain_props_05_f_slod",
			"h4_islandx_terrain_props_06_a_slod",
			"h4_islandx_terrain_props_06_b_slod",
			"h4_islandx_terrain_props_06_c_slod",
			"h4_mph4_airstrip",
			"h4_mph4_airstrip_interior_0_airstrip_hanger",
			"h4_mph4_beach",
			"h4_mph4_dock",
			"h4_mph4_island",
			"h4_mph4_island_long_0",
			"h4_mph4_island_strm_0",
			"h4_mph4_island_lod",
			"h4_mph4_island_placement",
			"h4_mph4_island_ne_placement",
			"h4_mph4_island_nw_placement",
			"h4_mph4_island_se_placement",
			"h4_mph4_island_sw_placement",
			"h4_mph4_mansion",
			"h4_mph4_mansion_strm_0",
			"h4_mph4_mansion_b",
			"h4_mph4_mansion_b_strm_0",
			"h4_mph4_wtowers",
			"h4_mph4_terrain_01",
			"h4_mph4_terrain_01_grass_0",
			"h4_mph4_terrain_01_grass_1",
			"h4_mph4_terrain_01_long_0",
			"h4_mph4_terrain_02",
			"h4_mph4_terrain_02_grass_0",
			"h4_mph4_terrain_02_grass_1",
			"h4_mph4_terrain_02_grass_2",
			"h4_mph4_terrain_02_grass_3",
			"h4_mph4_terrain_03",
			"h4_mph4_terrain_04",
			"h4_mph4_terrain_04_grass_0",
			"h4_mph4_terrain_04_grass_1",
			"h4_mph4_terrain_05",
			"h4_mph4_terrain_05_grass_0",
			"h4_mph4_terrain_06",
			"h4_mph4_terrain_06_strm_0",
			"h4_mph4_terrain_lod",
			"h4_mph4_terrain_occ_00",
			"h4_mph4_terrain_occ_01",
			"h4_mph4_terrain_occ_02",
			"h4_mph4_terrain_occ_03",
			"h4_mph4_terrain_occ_04",
			"h4_mph4_terrain_occ_05",
			"h4_mph4_terrain_occ_06",
			"h4_mph4_terrain_occ_07",
			"h4_mph4_terrain_occ_08",
			"h4_mph4_terrain_occ_09",
			"h4_ne_ipl_00",
			"h4_ne_ipl_01",
			"h4_ne_ipl_02",
			"h4_ne_ipl_03",
			"h4_ne_ipl_04",
			"h4_ne_ipl_05",
			"h4_ne_ipl_06",
			"h4_ne_ipl_07",
			"h4_ne_ipl_08",
			"h4_ne_ipl_09",
			"h4_ne_ipl_00_lod",
			"h4_ne_ipl_01_lod",
			"h4_ne_ipl_02_lod",
			"h4_ne_ipl_03_lod",
			"h4_ne_ipl_04_lod",
			"h4_ne_ipl_05_lod",
			"h4_ne_ipl_06_lod",
			"h4_ne_ipl_07_lod",
			"h4_ne_ipl_08_lod",
			"h4_ne_ipl_09_lod",
			"h4_ne_ipl_00_slod",
			"h4_ne_ipl_01_slod",
			"h4_ne_ipl_02_slod",
			"h4_ne_ipl_03_slod",
			"h4_ne_ipl_04_slod",
			"h4_ne_ipl_05_slod",
			"h4_ne_ipl_06_slod",
			"h4_ne_ipl_07_slod",
			"h4_ne_ipl_08_slod",
			"h4_ne_ipl_09_slod",
			"h4_nw_ipl_00",
			"h4_nw_ipl_01",
			"h4_nw_ipl_02",
			"h4_nw_ipl_03",
			"h4_nw_ipl_04",
			"h4_nw_ipl_05",
			"h4_nw_ipl_06",
			"h4_nw_ipl_07",
			"h4_nw_ipl_08",
			"h4_nw_ipl_09",
			"h4_nw_ipl_00_lod",
			"h4_nw_ipl_01_lod",
			"h4_nw_ipl_02_lod",
			"h4_nw_ipl_03_lod",
			"h4_nw_ipl_04_lod",
			"h4_nw_ipl_05_lod",
			"h4_nw_ipl_06_lod",
			"h4_nw_ipl_07_lod",
			"h4_nw_ipl_08_lod",
			"h4_nw_ipl_09_lod",
			"h4_nw_ipl_00_slod",
			"h4_nw_ipl_01_slod",
			"h4_nw_ipl_02_slod",
			"h4_nw_ipl_03_slod",
			"h4_nw_ipl_04_slod",
			"h4_nw_ipl_05_slod",
			"h4_nw_ipl_06_slod",
			"h4_nw_ipl_07_slod",
			"h4_nw_ipl_08_slod",
			"h4_nw_ipl_09_slod",
			"h4_se_ipl_00",
			"h4_se_ipl_01",
			"h4_se_ipl_02",
			"h4_se_ipl_03",
			"h4_se_ipl_04",
			"h4_se_ipl_05",
			"h4_se_ipl_06",
			"h4_se_ipl_07",
			"h4_se_ipl_08",
			"h4_se_ipl_09",
			"h4_se_ipl_00_lod",
			"h4_se_ipl_01_lod",
			"h4_se_ipl_02_lod",
			"h4_se_ipl_03_lod",
			"h4_se_ipl_04_lod",
			"h4_se_ipl_05_lod",
			"h4_se_ipl_06_lod",
			"h4_se_ipl_07_lod",
			"h4_se_ipl_08_lod",
			"h4_se_ipl_09_lod",
			"h4_se_ipl_00_slod",
			"h4_se_ipl_01_slod",
			"h4_se_ipl_02_slod",
			"h4_se_ipl_03_slod",
			"h4_se_ipl_04_slod",
			"h4_se_ipl_05_slod",
			"h4_se_ipl_06_slod",
			"h4_se_ipl_07_slod",
			"h4_se_ipl_08_slod",
			"h4_se_ipl_09_slod",
			"h4_sw_ipl_00",
			"h4_sw_ipl_01",
			"h4_sw_ipl_02",
			"h4_sw_ipl_03",
			"h4_sw_ipl_04",
			"h4_sw_ipl_05",
			"h4_sw_ipl_06",
			"h4_sw_ipl_07",
			"h4_sw_ipl_08",
			"h4_sw_ipl_09",
			"h4_sw_ipl_00_lod",
			"h4_sw_ipl_01_lod",
			"h4_sw_ipl_02_lod",
			"h4_sw_ipl_03_lod",
			"h4_sw_ipl_04_lod",
			"h4_sw_ipl_05_lod",
			"h4_sw_ipl_06_lod",
			"h4_sw_ipl_07_lod",
			"h4_sw_ipl_08_lod",
			"h4_sw_ipl_09_lod",
			"h4_sw_ipl_00_slod",
			"h4_sw_ipl_01_slod",
			"h4_sw_ipl_02_slod",
			"h4_sw_ipl_03_slod",
			"h4_sw_ipl_04_slod",
			"h4_sw_ipl_05_slod",
			"h4_sw_ipl_06_slod",
			"h4_sw_ipl_07_slod",
			"h4_sw_ipl_08_slod",
			"h4_sw_ipl_09_slod",
			"h4_ch2_mansion_final",
			"h4_islandx_checkpoint",
			"h4_islandx_checkpoint_lod",
			"h4_islandx_checkpoint_props",
			"h4_islandx_checkpoint_props_lod",
			"h4_islandx_checkpoint_props_slod",
			"h4_islandairstrip",
			"h4_islandairstrip_lod",
			"h4_islandairstrip_slod",
			"h4_islandairstrip_props",
			"h4_islandairstrip_props_lod",
			"h4_islandairstrip_props_slod",
			"h4_islandairstrip_propsb",
			"h4_islandairstrip_propsb_lod",
			"h4_islandairstrip_propsb_slod",
			"h4_islandairstrip_hangar_props",
			"h4_islandairstrip_hangar_props_lod",
			"h4_islandairstrip_hangar_props_slod",
			"h4_islandx_maindock",
			"h4_islandx_maindock_lod",
			"h4_islandx_maindock_slod",
			"h4_islandx_maindock_props",
			"h4_islandx_maindock_props_lod",
			"h4_islandx_maindock_props_slod",
			"h4_islandx_maindock_props_2",
			"h4_islandx_maindock_props_2_lod",
			"h4_islandx_maindock_props_2_slod",
			"h4_islandxdock",
			"h4_islandxdock_lod",
			"h4_islandxdock_slod",
			"h4_islandxdock_props",
			"h4_islandxdock_props_lod",
			"h4_islandxdock_props_slod",
			"h4_islandxdock_props_2",
			"h4_islandxdock_props_2_lod",
			"h4_islandxdock_props_2_slod",
			"h4_islandxdock_water_hatch",
			"h4_islandxtower",
			"h4_islandxtower_lod",
			"h4_islandxtower_slod",
			"h4_islandxtower_veg",
			"h4_islandxtower_veg_lod",
			"h4_islandxtower_veg_slod",
			"island_lodlights",
			"island_distantlights",
			"h4_islandx_mansion",
			"h4_mansion_gate_closed",
			"h4_islandairstrip_doorsopen",
			"h4_islandairstrip_doorsopen_lod",
		};

		const std::vector<CayoPericoLocation> vCayoPericoLocations
		{
			{ Vector3(5266.11377f, -5427.58838f, 141.047821f), "Communications Tower" },
			{ Vector3(5579.22510f, -5666.74561f,  15.389485f), "Lighthouse" },
			{ Vector3(5362.74707f, -5437.04150f,  66.176415f), "Watchtower" },
			{ Vector3(5320.99707f, -5251.63232f,  32.581440f), "The Farm" },
			{ Vector3(5143.31543f, -5144.11475f,   2.173501f), "Warehouses" },
			{ Vector3(4903.53027f, -5180.59717f,   2.468283f), "Main Dock" },
			{ Vector3(5099.01074f, -4619.31348f,   2.481808f), "Secondary Dock" },
			{ Vector3(4506.87842f, -4455.74170f,   4.316392f), "Airfield" },
			{ Vector3(4899.58252f, -4915.92920f,   3.377625f), "Beach Party" },
			{ Vector3(4969.79736f, -5698.11963f,  19.883307f), "Main Compound" },
		};

		const std::vector<CayoPericoLocation> vCayoPericoInteriors
		{
			{ Vector3(4427.90088f, -4450.84912f,   7.236789f), "Airfield Office" },
			{ Vector3(5007.07031f, -5751.53613f,  28.845320f), "Compound Office" },
			{ Vector3(4991.99756f, -5738.28662f,  14.840494f), "Compound Vault" },
		};

		static void _EnableIsland()
		{
			SET_INSTANCE_PRIORITY_MODE(1);
			ON_ENTER_MP();
			WAIT(0);

			// for (auto& ipl : vCayoPericoIPLs)
			// {
			// 	REQUEST_IPL(ipl);
			// }

			SET_ZONE_ENABLED(GET_ZONE_FROM_NAME_ID("PrLog"), FALSE); // make sure that north yankton is disabled as it's in the same zone as cayo perico
			SET_AUDIO_FLAG("DisableFlightMusic", TRUE);
			SET_AUDIO_FLAG("PlayerOnDLCHeist4Island", TRUE);
			SET_ISLAND_ENABLED("HeistIsland", TRUE); 

			SET_INSTANCE_PRIORITY_MODE(0);
		}
		static void _DisableIsland()
		{
			SET_INSTANCE_PRIORITY_MODE(1);
			ON_ENTER_SP();
			WAIT(0);
			
			// for (auto& ipl : vCayoPericoIPLs)
			// {
			// 	if (IS_IPL_ACTIVE(ipl)) REMOVE_IPL(ipl);
			// }

			SET_AUDIO_FLAG("DisableFlightMusic", FALSE);
			SET_AUDIO_FLAG("PlayerOnDLCHeist4Island", FALSE);
			SET_ISLAND_ENABLED("HeistIsland", FALSE); 

			SET_INSTANCE_PRIORITY_MODE(0);
		}

		static void _TeleportToLocation(const CayoPericoLocation& loc)
		{
			if (!bCayoPericoEnabled)
			{
				bCayoPericoEnabled = true;
				_EnableIsland();
			}
			TeleportNetPed(g_Ped1, loc.pos.x, loc.pos.y, loc.pos.z);
		}

		void Sub_CayoPerico()
		{
			AddTitle("Cayo Perico");
			AddToggle("Cayo Perico Enabled", bCayoPericoEnabled, _EnableIsland, _DisableIsland);
			AddBreak("---Locations---");
			for (auto& loc : vCayoPericoLocations)
			{
				bool bPressed = false;
				AddOption(loc.name, bPressed); if (bPressed)
					_TeleportToLocation(loc);
			}
			AddBreak("---Interiors---");
			for (auto& loc : vCayoPericoInteriors)
			{
				bool bPressed = false;
				AddOption(loc.name, bPressed); if (bPressed)
					_TeleportToLocation(loc);
			}
		}

		void Tick()
		{
			if (bCayoPericoEnabled)
			{
				// SET_RADAR_AS_EXTERIOR_THIS_FRAME();
				// SET_RADAR_AS_INTERIOR_THIS_FRAME(GET_HASH_KEY("h4_fake_islandx"), 4700.0f, -5145.0f, 0, 0);
				SET_USE_ISLAND_MAP(TRUE);
			}
		}
	}
}

#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(TELEPORTOPS_CAYOPERICO, sub::TeleportLocations_catind::CayoPerico::Sub_CayoPerico)
