/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "VehicleSpawner.h"

namespace sub
{
	Blip blip_g;
	int SpawnVehicle(GTAmodel::Model model, GTAped ped, bool deleteOld, bool warpIntoVehicle)
	{
		Vehicle newcar = 0;
		GTAentity selfPed = PLAYER_PED_ID();
		Vector3 pedsCoords = selfPed.GetPosition();
		Vector3 oldVelocity;
		Vector3 Pos1, Pos2;
		Vehicle oldcar = 0;
		bool oldcarBool = false;
		int oldRadioStation = GET_PLAYER_RADIO_STATION_INDEX();
		bool oldCarOn = true;
		if (ped.IsInVehicle())
		{
			oldcar = ped.CurrentVehicle().GetHandle();
			oldVelocity = GET_ENTITY_VELOCITY(oldcar);
			oldCarOn = GET_IS_VEHICLE_ENGINE_RUNNING(oldcar) != 0;
			oldcarBool = true;
		}

		if (model.Load(3000))
		{
			if (oldcarBool)
			{
				float spacing1 = Model(GET_ENTITY_MODEL(oldcar)).Dim1().y + model.Dim2().y + 1.0f;
				if(deleteOld)
				{
					Pos1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(oldcar, 0, 0, 0.5f);
				}
				else
				{
					Pos1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(oldcar, 0, spacing1, 0.5f);
				}
			}
			else
			{
				Pos1 = ped.GetOffsetInWorldCoords(Vector3());
			}

			PTFX::TriggerPTFX("proj_xmas_firework", "scr_firework_xmas_burst_rgw", 0, Pos1, Vector3(), 1.0f);
			newcar = CREATE_VEHICLE(model.hash, Pos1.x, Pos1.y, Pos1.z, ped.GetHeading(), 1, 1, 0);

			SET_ENTITY_ALPHA(newcar, 0, false);
			SET_ENTITY_COLLISION(newcar, false, true);
			SET_VEHICLE_DIRT_LEVEL(newcar, 0.0f);

			if (oldcarBool && DOES_ENTITY_EXIST(oldcar))
			{
				GTAvehicle(newcar).RequestControl();
				SET_VEHICLE_ENGINE_ON(newcar, oldCarOn, true, 0);
				SET_ENTITY_COLLISION(newcar, true, true);
				SET_VEHICLE_FORWARD_SPEED(newcar, abs(oldVelocity.y));
				SET_ENTITY_VELOCITY(newcar, oldVelocity.x, oldVelocity.y, oldVelocity.z);
				if (deleteOld)
				{ 
					SET_ENTITY_COLLISION(oldcar, false, true);
					SET_ENTITY_ALPHA(oldcar, 0, false);
				}
				RESET_ENTITY_ALPHA(newcar);
				if (warpIntoVehicle)
				{
					int maxi = GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(newcar);// - 2;
					for (INT i = -1; i <= maxi; i++)
					{
						Ped tempPed = GET_PED_IN_VEHICLE_SEAT(oldcar, i, 0);
						if (DOES_ENTITY_EXIST(tempPed))
						{
							if (GTAentity(tempPed).RequestControl())
							{
								SET_PED_INTO_VEHICLE(tempPed, newcar, i);
							}
						}
					}
				}

				if (deleteOld)
				{
					WAIT(0);
					int maxi = GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(oldcar);// - 2;
					for (INT i = -1; i <= maxi; i++)
					{
						Ped tempPed = GET_PED_IN_VEHICLE_SEAT(oldcar, i, 0);
						if (DOES_ENTITY_EXIST(tempPed))
						{
							if (GTAentity(tempPed).RequestControl())
							{
								TASK_LEAVE_ANY_VEHICLE(tempPed, 0, 0);
								CLEAR_PED_TASKS_IMMEDIATELY(tempPed);
							}
						}
					}
					WAIT(0);
					GTAvehicle(oldcar).RequestControl();
					SET_ENTITY_COORDS(oldcar, 32.2653f, 7683.5249f, 0.5696f, 0, 0, 0, 1);
					DELETE_VEHICLE(&oldcar);
				}
			}
			else
			{
				if (warpIntoVehicle)
				{
					SET_PED_INTO_VEHICLE(ped.Handle(), newcar, (int)GTAvehicle(newcar).FirstFreeSeat(SEAT_DRIVER));
				}
				SET_ENTITY_COLLISION(newcar, true, true);
				RESET_ENTITY_ALPHA(newcar);
			}

			GTAvehicle(newcar).SetRadioStation(oldRadioStation);
			GTAvehicle(newcar).CloseAllDoors(true);

			model.Unload();
			SET_VEHICLE_IS_STOLEN(newcar, false);
		}
		
		return newcar;
	}

	namespace VehicleSpawner
	{
		UINT8 spawnVehicleIndex;

		struct VehBmpSprite
		{
			Hash modelHash;
			DxHookIMG::DxTexture tid;
			std::string dict, imgName;

			friend bool operator == (const VehBmpSprite& left, const Hash right)
			{
				return left.modelHash == right;
			}

			VehBmpSprite() : modelHash(0)
			{
			}
			VehBmpSprite(Hash modelHashP) : modelHash(modelHashP)
			{
			}
			VehBmpSprite(Hash modelHashP, std::string dictP, std::string imgNameP) : modelHash(modelHashP), dict(dictP), imgName(imgNameP)
			{
			}

			bool LoadFile(const std::string& path)
			{
				tid.Load(path);
				return tid.ID() != 0;
			}
		};
		std::vector<VehBmpSprite> vehicleBMPs {
			{ VEHICLE_AIRBUS, "candc_default", "airbus" },
			{ VEHICLE_BARRACKS, "candc_default", "barracks" },
			{ VEHICLE_BOXVILLE4, "candc_default", "boxville4" },
			{ VEHICLE_BUS, "candc_default", "bus" },
			{ VEHICLE_BUZZARD, "candc_default", "buzzard" },
			{ VEHICLE_CARGOBOB, "candc_default", "cargobob" },
			{ VEHICLE_COACH, "candc_default", "coach" },
			{ VEHICLE_CRUSADER, "candc_default", "crusader" },
			{ VEHICLE_DUBSTA3, "candc_default", "dubsta3" },
			{ VEHICLE_DUKES2, "candc_default", "dukes" },
			{ VEHICLE_DUMP, "candc_default", "dump" },
			{ VEHICLE_HYDRA, "candc_default", "hydra" },
			{ VEHICLE_INSURGENT, "candc_default", "insurgent" },
			{ VEHICLE_INSURGENT2, "candc_default", "insurgent2" },
			{ VEHICLE_JOURNEY, "candc_default", "journey" },
			{ VEHICLE_MARSHALL, "candc_default", "marshall" },
			{ VEHICLE_MESA, "candc_default", "mesa" },
			{ VEHICLE_MESA3, "candc_default", "mesa3" },
			{ VEHICLE_MONSTER, "candc_default", "monster" },
			{ VEHICLE_MULE, "candc_default", "mule" },
			{ VEHICLE_MULE3, "candc_default", "mule3" },
			{ VEHICLE_PBUS, "candc_default", "pbus" },
			{ VEHICLE_RENTALBUS, "candc_default", "rentbus" },
			{ VEHICLE_RHINO, "candc_default", "rhino" },
			{ VEHICLE_SAVAGE, "candc_default", "savage" },
			{ VEHICLE_TECHNICAL, "candc_default", "technical" },
			{ VEHICLE_VALKYRIE, "candc_default", "valkyrie" },

			{ VEHICLE_DINGHY3, "dock_default", "dinghy3" },
			{ VEHICLE_JETMAX, "dock_default", "jetmax" },
			{ VEHICLE_MARQUIS, "dock_default", "marquis" },
			{ VEHICLE_SEASHARK, "dock_default", "seashark" },
			{ VEHICLE_SPEEDER, "dock_default", "speeder" },
			{ VEHICLE_SQUALO, "dock_default", "squalo" },
			{ VEHICLE_SUBMERSIBLE2, "dock_default", "sub2" },
			{ VEHICLE_SUNTRAP, "dock_default", "suntrap" },
			{ VEHICLE_TROPIC, "dock_default", "tropic" },

			{ VEHICLE_ANNIHILATOR, "elt_default", "annihl" },
			{ VEHICLE_CUBAN800, "elt_default", "cuban800" },
			{ VEHICLE_DODO, "elt_default", "dodo" },
			{ VEHICLE_DUSTER, "elt_default", "duster" },
			{ VEHICLE_FROGGER, "elt_default", "frogger" },
			{ VEHICLE_LUXOR, "elt_default", "luxor" },
			{ VEHICLE_MAMMATUS, "elt_default", "mammatus" },
			{ VEHICLE_MAVERICK, "elt_default", "maverick" },
			{ VEHICLE_SHAMAL, "elt_default", "shamal" },
			{ VEHICLE_STUNT, "elt_default", "stunt" },
			{ VEHICLE_TITAN, "elt_default", "titan" },
			{ VEHICLE_VELUM, "elt_default", "velum" },
			{ VEHICLE_VELUM2, "elt_default", "velum2" },

			{ VEHICLE_VESTRA, "elt_dlc_business", "vestra" },

			{ VEHICLE_LUXOR2, "elt_dlc_luxe", "luxor2" },
			{ VEHICLE_SWIFT2, "elt_dlc_luxe", "swift2" },

			{ VEHICLE_BESRA, "elt_dlc_pilot", "besra" },
			{ VEHICLE_MILJET, "elt_dlc_pilot", "miljet" },
			{ VEHICLE_SWIFT, "elt_dlc_pilot", "swift" },

			{ VEHICLE_ADDER, "lgm_default", "adder" },
			{ VEHICLE_BANSHEE, "lgm_default", "banshee" },
			{ VEHICLE_BULLET, "lgm_default", "bullet" },
			{ VEHICLE_CARBONIZZARE, "lgm_default", "carboniz" },
			{ VEHICLE_CARBONRS, "lgm_default", "carbon" },
			{ VEHICLE_CHEETAH, "lgm_default", "cheetah" },
			{ VEHICLE_COGCABRIO, "lgm_default", "cogcabri" },
			{ VEHICLE_COMET2, "lgm_default", "comet2" },
			{ VEHICLE_COQUETTE, "lgm_default", "coquette" },
			{ VEHICLE_ELEGY2, "lgm_default", "elegy2" },
			{ VEHICLE_ENTITYXF, "lgm_default", "entityxf" },
			{ VEHICLE_EXEMPLAR, "lgm_default", "exemplar" },
			{ VEHICLE_FELTZER2, "lgm_default", "feltzer" },
			{ VEHICLE_HOTKNIFE, "lgm_default", "hotknife" },
			{ VEHICLE_JB700, "lgm_default", "jb700" },
			{ VEHICLE_KHAMELION, "lgm_default", "khamel" },
			{ VEHICLE_MONROE, "lgm_default", "monroe" },
			{ VEHICLE_NEMESIS, "sssa_dlc_heist", "nemesis" },
			{ VEHICLE_NINEF, "lgm_default", "ninef" },
			{ VEHICLE_NINEF2, "lgm_default", "ninef2" },
			{ VEHICLE_RAPIDGT, "lgm_default", "rapidgt" },
			{ VEHICLE_RAPIDGT2, "lgm_default", "rapidgt2" },
			{ VEHICLE_STINGER, "lgm_default", "stinger" },
			{ VEHICLE_STINGERGT, "lgm_default", "stingerg" },
			{ VEHICLE_VOLTIC, "lgm_default", "voltic_tless" },
			{ VEHICLE_ZTYPE, "lgm_default", "ztype" },

			{ VEHICLE_ALPHA, "lgm_dlc_business", "alpha" },
			{ VEHICLE_JESTER, "lgm_dlc_business", "jester" },
			{ VEHICLE_TURISMOR, "lgm_dlc_business", "turismor" },

			{ VEHICLE_COQUETTE2, "lgm_dlc_pilot", "coquette2" },

			{ VEHICLE_HUNTLEY, "lgm_dlc_business2", "huntley" },
			{ VEHICLE_MASSACRO, "lgm_dlc_business2", "massacro" },
			{ VEHICLE_THRUST, "lgm_dlc_business2", "thrust" },
			{ VEHICLE_ZENTORNO, "lgm_dlc_business2", "zentorno" },

			{ VEHICLE_FUROREGT, "lgm_dlc_lts_creator", "furore" },

			{ VEHICLE_FELTZER3, "lgm_dlc_luxe", "feltzer3" },
			{ VEHICLE_OSIRIS, "lgm_dlc_luxe", "osiris" },
			{ VEHICLE_VIRGO, "lgm_dlc_luxe", "virgo" },
			{ VEHICLE_WINDSOR, "lgm_dlc_luxe", "windsor" },
			{ VEHICLE_BRAWLER, "lgm_dlc_luxe", "brawler" },
			{ VEHICLE_VINDICATOR, "lgm_dlc_luxe", "vindicator" },
			{ VEHICLE_COQUETTE3, "lgm_dlc_luxe", "coquette3" },
			{ VEHICLE_T20, "lgm_dlc_luxe", "t20" },

			{ VEHICLE_TORO, "dock_default", "toro" },

			{ VEHICLE_BMX, "pandm_default", "bmx" },
			{ VEHICLE_CRUISER, "pandm_default", "cruiser" },
			{ VEHICLE_SCORCHER, "pandm_default", "scorcher" },
			{ VEHICLE_TRIBIKE, "pandm_default", "tribike" },
			{ VEHICLE_TRIBIKE2, "pandm_default", "tribike2" },
			{ VEHICLE_TRIBIKE3, "pandm_default", "tribike3" },

			{ VEHICLE_AKUMA, "sssa_default", "akuma" },
			{ VEHICLE_BALLER, "sssa_default", "baller2" },
			{ VEHICLE_BALLER2, "sssa_default", "baller2" },
			{ VEHICLE_BATI, "sssa_default", "bati" },
			{ VEHICLE_BATI2, "sssa_default", "bati2" },
			{ VEHICLE_BFINJECTION, "sssa_default", "bfinject" },
			{ VEHICLE_BIFTA, "sssa_default", "bifta" },
			{ VEHICLE_BISON, "sssa_default", "bison" },
			{ VEHICLE_BLAZER, "sssa_default", "blazer" },
			{ VEHICLE_BLAZER2, "candc_casinoheist", "blazer2" },
			{ VEHICLE_BODHI2, "sssa_default", "bodhi2" },
			{ VEHICLE_CAVALCADE, "sssa_default", "cavcade" },
			{ VEHICLE_DILETTANTE, "sssa_default", "dilettan" },
			{ VEHICLE_DOUBLE, "sssa_default", "double" },
			{ VEHICLE_DUNE, "sssa_default", "dune" },
			{ VEHICLE_FAGGIO2, "sssa_default", "faggio" },
			{ VEHICLE_FELON, "sssa_default", "felon" },
			{ VEHICLE_FELON2, "sssa_default", "felon2" },
			{ VEHICLE_FUGITIVE, "sssa_default", "fugitive" },
			{ VEHICLE_GAUNTLET, "sssa_default", "gauntlet" },
			{ VEHICLE_HEXER, "sssa_default", "hexer" },
			{ VEHICLE_INFERNUS, "sssa_default", "infernus" },
			{ VEHICLE_ISSI2, "sssa_default", "issi2" },
			{ VEHICLE_KALAHARI, "sssa_default", "kalahari" },
			{ VEHICLE_ORACLE, "sssa_default", "oracle" },
			{ VEHICLE_PARADISE, "sssa_default", "paradise" },
			{ VEHICLE_PCJ, "sssa_default", "pcj" },
			{ VEHICLE_REBEL, "sssa_default", "rebel" },
			{ VEHICLE_ROCOTO, "sssa_default", "rocoto" },
			{ VEHICLE_RUFFIAN, "sssa_default", "ruffian" },
			{ VEHICLE_SADLER, "sssa_default", "sadler" },
			{ VEHICLE_SANCHEZ, "sssa_default", "sanchez" },
			{ VEHICLE_SANCHEZ2, "sssa_default", "sanchez2" },
			{ VEHICLE_SANDKING, "sssa_default", "sandking" },
			{ VEHICLE_SANDKING2, "sssa_default", "sandkin2" },
			{ VEHICLE_SCHWARZER, "sssa_default", "schwarze" },
			{ VEHICLE_STRETCH, "sssa_default", "stretch" },
			{ VEHICLE_SUPERD, "lgm_default", "superd" },
			{ VEHICLE_SURANO, "lgm_default", "surano_convertable" },
			{ VEHICLE_VACCA, "lgm_default", "vacca" },
			{ VEHICLE_VADER, "sssa_default", "vader" },
			{ VEHICLE_VIGERO, "sssa_default", "vigero" },
			{ VEHICLE_ZION, "sssa_default", "zion" },
			{ VEHICLE_ZION2, "sssa_default", "zion2" },

			{ VEHICLE_ASEA, "sssa_dlc_business", "asea" },
			{ VEHICLE_ASTEROPE, "sssa_dlc_business", "astrope" },
			{ VEHICLE_BOBCATXL, "sssa_dlc_business", "bobcatxl" },
			{ VEHICLE_CAVALCADE2, "sssa_dlc_business", "cavcade2" },
			{ VEHICLE_INGOT, "sssa_dlc_business", "ingot" },
			{ VEHICLE_INTRUDER, "sssa_dlc_business", "intruder" },
			{ VEHICLE_MINIVAN, "sssa_dlc_business", "minivan" },
			{ VEHICLE_PREMIER, "sssa_dlc_business", "premier" },
			{ VEHICLE_RADI, "sssa_dlc_business", "radi" },
			{ VEHICLE_RANCHERXL, "sssa_dlc_business", "rancherx" },
			{ VEHICLE_STANIER, "sssa_dlc_business", "stanier" },
			{ VEHICLE_STRATUM, "sssa_dlc_business", "stratum" },
			{ VEHICLE_WASHINGTON, "sssa_dlc_business", "washingt" },
			{ VEHICLE_DOMINATOR, "sssa_dlc_business2", "dominato" },
			{ VEHICLE_F620, "sssa_dlc_business2", "f620" },
			{ VEHICLE_FUSILADE, "sssa_dlc_business2", "fusilade" },
			{ VEHICLE_PENUMBRA, "sssa_dlc_business2", "penumbra" },
			{ VEHICLE_SENTINEL, "sssa_dlc_business2", "sentinel" },

			{ VEHICLE_JESTER2, "sssa_dlc_christmas_2", "jester2" },
			{ VEHICLE_MASSACRO2, "sssa_dlc_christmas_2", "massacro2" },
			{ VEHICLE_RATLOADER2, "sssa_dlc_christmas_2", "rloader2" },
			{ VEHICLE_SLAMVAN, "sssa_dlc_christmas_2", "slamvan" },

			{ VEHICLE_ENDURO, "sssa_dlc_heist", "enduro" },
			{ VEHICLE_GBURRITO2, "sssa_dlc_heist", "gburrito2" },
			{ VEHICLE_GRESLEY, "sssa_dlc_heist", "gresley" },
			{ VEHICLE_JACKAL, "sssa_dlc_heist", "jackal" },
			{ VEHICLE_KURUMA, "sssa_dlc_heist", "kuruma" },
			{ VEHICLE_KURUMA2, "sssa_dlc_heist", "kuruma2" },
			{ VEHICLE_LANDSTALKER, "sssa_dlc_heist", "landstalker" },
			{ VEHICLE_RUMPO, "sssa_dlc_heist", "rumpo" },
			{ VEHICLE_SCHAFTER2, "sssa_dlc_heist", "schafter2" },
			{ VEHICLE_SCHAFTER5, "sssa_dlc_heist", "schafter2" },
			{ VEHICLE_SEMINOLE, "sssa_dlc_heist", "seminole" },
			{ VEHICLE_SURGE, "sssa_dlc_heist", "surge" },

			{ VEHICLE_BLADE, "sssa_dlc_hipster", "blade" },
			{ VEHICLE_BLAZER3, "sssa_dlc_hipster", "blazer3" },
			{ VEHICLE_BUFFALO, "sssa_dlc_hipster", "buffalo" },
			{ VEHICLE_BUFFALO2, "sssa_dlc_hipster", "buffalo2" },
			{ VEHICLE_GLENDALE, "sssa_dlc_hipster", "glendale" },
			{ VEHICLE_PANTO, "sssa_dlc_hipster", "panto" },
			{ VEHICLE_PICADOR, "sssa_dlc_hipster", "picador" },
			{ VEHICLE_PIGALLE, "sssa_dlc_hipster", "pigalle" },
			{ VEHICLE_REBEL2, "sssa_dlc_hipster", "rebel2" },
			{ VEHICLE_REGINA, "sssa_dlc_hipster", "regina" },
			{ VEHICLE_RHAPSODY, "sssa_dlc_hipster", "rhapsody" },
			{ VEHICLE_SURFER, "sssa_dlc_hipster", "surfer" },
			{ VEHICLE_TAILGATER, "sssa_dlc_hipster", "tailgater" },
			{ VEHICLE_WARRENER, "sssa_dlc_hipster", "warrener" },
			{ VEHICLE_YOUGA, "sssa_dlc_hipster", "youga" },

			{ VEHICLE_SOVEREIGN, "sssa_dlc_independence", "sovereign" },

			{ VEHICLE_HAKUCHOU, "sssa_dlc_lts_creator", "hakuchou" },
			{ VEHICLE_INNOVATION, "sssa_dlc_lts_creator", "innovation" },

			{ VEHICLE_BLISTA2, "sssa_dlc_mp_to_sp", "blista2" },
			{ VEHICLE_BUFFALO3, "sssa_dlc_mp_to_sp", "buffalo3" },
			{ VEHICLE_DOMINATOR2, "sssa_dlc_mp_to_sp", "dominator2" },
			{ VEHICLE_GAUNTLET2, "sssa_dlc_mp_to_sp", "gauntlet2" },
			{ VEHICLE_STALION, "sssa_dlc_mp_to_sp", "stallion" },
			{ VEHICLE_STALION2, "sssa_dlc_mp_to_sp", "stalion2" },

			{ VEHICLE_RATLOADER, "sssa_dlc_valentines", "rloader" },
			{ VEHICLE_BTYPE, "lgm_dlc_valentines", "roosevelt" },

			{ VEHICLE_CASCO, "lgm_dlc_heist", "casco" },
			{ VEHICLE_LECTRO, "lgm_dlc_heist", "lectro" },

			{ VEHICLE_BUCCANEER2, "lsc_default", "buccaneer2_b" },
			{ VEHICLE_BUCCANEER, "lsc_default", "buccaneer2" },
			{ VEHICLE_CHINO2, "lsc_default", "chino2_b" },
			{ VEHICLE_CHINO, "lgm_dlc_luxe", "chino" },
			{ VEHICLE_FACTION, "lsc_default", "faction2_a" },
			{ VEHICLE_FACTION2, "lsc_default", "faction2_b" },
			{ VEHICLE_MOONBEAM, "lsc_default", "moonbeam2_a" },
			{ VEHICLE_MOONBEAM2, "lsc_default", "moonbeam2_b" },
			{ VEHICLE_PRIMO, "sssa_dlc_hipster", "primo" },
			{ VEHICLE_PRIMO2, "lsc_default", "primo2_b" },
			{ VEHICLE_VOODOO2, "lsc_default", "voodoo_a" },
			{ VEHICLE_VOODOO, "lsc_default", "voodoo_b" },

			{ VEHICLE_BTYPE2, "sssa_dlc_halloween", "btype2" },
			{ VEHICLE_LURCHER, "sssa_dlc_halloween", "lurcher" },

			{ VEHICLE_SUPERVOLITO, "elt_dlc_apartments", "svolito" },
			{ VEHICLE_SUPERVOLITO2, "elt_dlc_apartments", "svolito2" },
			{ VEHICLE_LIMO2, "candc_apartments", "limo2" },
			{ VEHICLE_BALLER3, "lgm_dlc_apartments", "baller3" },
			{ VEHICLE_BALLER4, "lgm_dlc_apartments", "baller4" },
			{ VEHICLE_BALLER5, "lgm_dlc_apartments", "baller3" },
			{ VEHICLE_BALLER6, "lgm_dlc_apartments", "baller4" },
			{ VEHICLE_COG55, "lgm_dlc_apartments", "cog55" },
			{ VEHICLE_COG552, "lgm_dlc_apartments", "cog55" },
			{ VEHICLE_COGNOSCENTI, "lgm_dlc_apartments", "cognosc" },
			{ VEHICLE_COGNOSCENTI2, "lgm_dlc_apartments", "cognosc" },
			{ VEHICLE_MAMBA, "lgm_dlc_apartments", "mamba" },
			{ VEHICLE_NIGHTSHADE, "lgm_dlc_apartments", "niteshad" },
			{ VEHICLE_SCHAFTER3, "lgm_dlc_apartments", "schafter3" },
			{ VEHICLE_SCHAFTER4, "lgm_dlc_apartments", "schafter4" },
			{ VEHICLE_VERLIERER2, "lgm_dlc_apartments", "verlier" },
			{ VEHICLE_TAMPA, "sssa_dlc_christmas_3", "tampa" },

			{ VEHICLE_BANSHEE2, "lsc_jan2016", "banshee2" },
			{ VEHICLE_SULTANRS, "lsc_jan2016", "sultan2" },
			{ VEHICLE_BTYPE3, "lgm_dlc_valentines2", "roosevelt2" },

			{ VEHICLE_FACTION3, "lsc_lowrider2", "faction3_b" },
			{ VEHICLE_MINIVAN2, "lsc_lowrider2", "minivan2_b" },
			{ VEHICLE_SABREGT, "lsc_lowrider2", "sabregt2_a" },
			{ VEHICLE_SABREGT2, "lsc_lowrider2", "sabregt2_b" },
			{ VEHICLE_SLAMVAN3, "lsc_lowrider2", "slamvan3_b" },
			{ VEHICLE_TORNADO, "lsc_lowrider2", "tornado5_a" },
			{ VEHICLE_TORNADO5, "lsc_lowrider2", "tornado5_b" },
			{ VEHICLE_VIRGO2, "lsc_lowrider2", "virgo2_b" },
			{ VEHICLE_VIRGO3, "lsc_lowrider2", "virgo2_a" },

			{ VEHICLE_BESTIAGTS, "lgm_dlc_executive1", "bestiagts" },
			{ VEHICLE_FMJ, "lgm_dlc_executive1", "fmj" },
			{ VEHICLE_PFISTER811, "lgm_dlc_executive1", "pfister811" },
			{ VEHICLE_PROTOTIPO, "lgm_dlc_executive1", "prototipo" },
			{ VEHICLE_REAPER, "lgm_dlc_executive1", "reaper" },
			{ VEHICLE_SEVEN70, "lgm_dlc_executive1", "seven70" },
			{ VEHICLE_WINDSOR2, "lgm_dlc_executive1", "windsor2" },
			{ VEHICLE_XLS, "lgm_dlc_executive1", "xls" },
			{ VEHICLE_XLS2, "lgm_dlc_executive1", "xls" },
			{ VEHICLE_RUMPO3, "sssa_dlc_executive_1", "rumpo3" },
			{ VEHICLE_BRICKADE, "candc_executive1", "brickade" },
			{ VEHICLE_CARGOBOB2, "candc_executive1", "cargobob2" },
			{ VEHICLE_NIMBUS, "elt_dlc_executive1", "nimbus" },
			{ VEHICLE_VOLATUS, "elt_dlc_executive1", "volatus" },
			{ VEHICLE_TUG, "dock_dlc_executive1", "tug" },

			{ VEHICLE_LE7B, "lgm_dlc_stunt", "le7b" },
			{ VEHICLE_LYNX, "lgm_dlc_stunt", "lynx" },
			{ VEHICLE_SHEAVA, "lgm_dlc_stunt", "sheava" },
			{ VEHICLE_TYRUS, "lgm_dlc_stunt", "tyrus" },
			{ VEHICLE_BF400, "sssa_dlc_stunt", "bf400" },
			{ VEHICLE_BRIOSO, "sssa_dlc_stunt", "brioso" },
			{ VEHICLE_CLIFFHANGER, "sssa_dlc_stunt", "cliffhanger" },
			{ VEHICLE_CONTENDER, "sssa_dlc_stunt", "contender" },
			{ VEHICLE_GARGOYLE, "sssa_dlc_stunt", "gargoyle" },
			{ VEHICLE_OMNIS, "sssa_dlc_stunt", "omnis" },
			{ VEHICLE_RALLYTRUCK, "sssa_dlc_stunt", "rallytruck" },
			{ VEHICLE_TAMPA2, "sssa_dlc_stunt", "tampa2" },
			{ VEHICLE_TROPHYTRUCK, "sssa_dlc_stunt", "trophy" },
			{ VEHICLE_TROPHYTRUCK2, "sssa_dlc_stunt", "trophy2" },
			{ VEHICLE_TROPOS, "sssa_dlc_stunt", "tropos" },

			{ VEHICLE_HAKUCHOU2, "lgm_dlc_biker", "hakuchou2" },
			{ VEHICLE_RAPTOR, "lgm_dlc_biker", "raptor" },
			{ VEHICLE_SHOTARO, "lgm_dlc_biker", "shotaro" },
			{ VEHICLE_AVARUS, "sssa_dlc_biker", "avarus" },
			{ VEHICLE_BAGGER, "sssa_dlc_biker", "bagger" },
			{ VEHICLE_BLAZER4, "sssa_dlc_biker", "blazer4" },
			{ VEHICLE_CHIMERA, "sssa_dlc_biker", "chimera" },
			{ VEHICLE_DAEMON2, "sssa_dlc_biker", "daemon2" },
			{ VEHICLE_DEFILER, "sssa_dlc_biker", "defiler" },
			{ VEHICLE_ESSKEY, "sssa_dlc_biker", "esskey" },
			{ VEHICLE_FAGGIO3, "sssa_dlc_biker", "faggio3" },
			{ VEHICLE_FAGGIO, "sssa_dlc_biker", "faggion" },
			{ VEHICLE_MANCHEZ, "sssa_dlc_biker", "manchez" },
			{ VEHICLE_NIGHTBLADE, "sssa_dlc_biker", "nightblade" },
			{ VEHICLE_RATBIKE, "sssa_dlc_biker", "ratbike" },
			{ VEHICLE_SANCTUS, "sssa_dlc_biker", "sanctus" },
			{ VEHICLE_TORNADO6, "sssa_dlc_biker", "tornado6" },
			{ VEHICLE_VORTEX, "sssa_dlc_biker", "vortex" },
			{ VEHICLE_WOLFSBANE, "sssa_dlc_biker", "wolfsbane" },
			{ VEHICLE_YOUGA2, "sssa_dlc_biker", "youga2" },
			{ VEHICLE_ZOMBIEA, "sssa_dlc_biker", "zombiea" },
			{ VEHICLE_ZOMBIEB, "sssa_dlc_biker", "zombieb" },

			{ VEHICLE_BLAZER5, "candc_importexport", "blazer5" },
			{ VEHICLE_BOXVILLE5, "candc_importexport", "boxville5" },
			{ VEHICLE_DUNE5, "candc_importexport", "dune5" },
			{ VEHICLE_PHANTOM2, "candc_importexport", "phantom2" },
			{ VEHICLE_RUINER2, "candc_importexport", "ruiner2" },
			{ VEHICLE_TECHNICAL2, "candc_importexport", "technical2" },
			{ VEHICLE_VOLTIC2, "candc_importexport", "voltic2" },
			{ VEHICLE_WASTELANDER, "candc_importexport", "wastlndr" },
			{ VEHICLE_PENETRATOR, "lgm_dlc_importexport", "penetrator" },
			{ VEHICLE_TEMPESTA, "lgm_dlc_importexport", "tempesta" },
			{ VEHICLE_COMET3, "lsc_dlc_import_export", "comet3_b" },
			{ VEHICLE_DIABLOUS, "lsc_dlc_import_export", "diablous2_a" },
			{ VEHICLE_DIABLOUS2, "lsc_dlc_import_export", "diablous2_b" },
			{ VEHICLE_ELEGY, "lsc_dlc_import_export", "elegy_b" },
			{ VEHICLE_FCR, "lsc_dlc_import_export", "fcr2_a" },
			{ VEHICLE_FCR2, "lsc_dlc_import_export", "fcr2_b" },
			{ VEHICLE_ITALIGTB, "lsc_dlc_import_export", "italigtb2_a" },
			{ VEHICLE_ITALIGTB2, "lsc_dlc_import_export", "italigtb2_b" },
			{ VEHICLE_NERO, "lsc_dlc_import_export", "nero2_a" },
			{ VEHICLE_NERO2, "lsc_dlc_import_export", "nero2_b" },
			{ VEHICLE_SPECTER, "lsc_dlc_import_export", "specter2_a" },
			{ VEHICLE_SPECTER2, "lsc_dlc_import_export", "specter2_b" },

			{ VEHICLE_GP1, "lgm_dlc_specialraces", "gp1" },
			{ VEHICLE_INFERNUS2, "lgm_dlc_specialraces", "infernus2" },
			{ VEHICLE_RUSTON, "lgm_dlc_specialraces", "ruston" },
			{ VEHICLE_TURISMO2, "lgm_dlc_specialraces", "turismo2" },

			{ VEHICLE_APC, "candc_gunrunning", "apc" },
			{ VEHICLE_ARDENT, "candc_gunrunning", "ardent" },
			{ VEHICLE_CADDY3, "foreclosures_bunker", "transportation_1" },
			{ VEHICLE_CHEETAH2, "lgm_dlc_gunrunning", "cheetah2" },
			{ VEHICLE_DUNE3, "candc_gunrunning", "dune3" },
			{ VEHICLE_HALFTRACK, "candc_gunrunning", "halftrack" },
			{ VEHICLE_HAULER2 , "candc_truck", "cab_1" },
			{ VEHICLE_NIGHTSHARK, "candc_gunrunning", "nightshark" },
			{ VEHICLE_OPPRESSOR, "candc_gunrunning", "oppressor" },
			{ VEHICLE_TAMPA3, "candc_gunrunning", "tampa3" },
			{ VEHICLE_TORERO, "lgm_dlc_gunrunning", "torero" },
			{ VEHICLE_TRAILERSMALL2, "candc_gunrunning", "trsmall2" },
			{ VEHICLE_VAGNER, "lgm_dlc_gunrunning", "vagner" },
			{ VEHICLE_XA21, "lgm_dlc_gunrunning", "xa21" },

			{ VEHICLE_HAULER2, "candc_truck", "cab_0" },
			{ VEHICLE_BOMBUSHKA, "candc_smuggler","bombushka" },
			{ VEHICLE_HUNTER, "candc_smuggler","hunter" },
			{ VEHICLE_LAZER , "candc_smuggler","lazer" },
			{ VEHICLE_MOGUL , "candc_smuggler","mogul" },
			{ VEHICLE_MOLOTOK  , "candc_smuggler","molotok" },
			{ VEHICLE_NOKOTA, "candc_smuggler","nokota" },
			{ VEHICLE_PYRO  , "candc_smuggler","pyro" },
			{ VEHICLE_ROGUE , "candc_smuggler","rogue" },
			{ VEHICLE_STARLING , "candc_smuggler","starling" },
			{ VEHICLE_TULA  , "candc_smuggler","tula" },
			{ VEHICLE_VIGILANTE, "candc_smuggler","vigilante" },
			{ VEHICLE_CYCLONE  , "lgm_dlc_smuggler" , "cyclone" },
			{ VEHICLE_RAPIDGT3 , "lgm_dlc_smuggler" , "rapidgt3" },
			{ VEHICLE_VISIONE  , "lgm_dlc_smuggler" , "visione" },
			{ VEHICLE_RETINUE , "sssa_dlc_smuggler", "retinue" },
			{ VEHICLE_ALPHAZ1  , "elt_dlc_smuggler" , "alphaz1" },
			{ VEHICLE_HAVOK , "elt_dlc_smuggler" , "havok" },
			{ VEHICLE_HOWARD, "elt_dlc_smuggler" , "howard" },
			{ VEHICLE_MICROLIGHT   , "elt_dlc_smuggler" , "microlight" },
			{ VEHICLE_SEABREEZE, "elt_dlc_smuggler" , "seabreeze" },

			// b1290
			{ VEHICLE_AKULA, "candc_xmas2017", "akula" },
			{ VEHICLE_BARRAGE, "candc_xmas2017", "barrage" },
			{ VEHICLE_CHERNOBOG, "candc_xmas2017", "chernobog" },
			{ VEHICLE_DELUXO, "candc_xmas2017", "deluxo" },
			{ VEHICLE_KHANJALI, "candc_xmas2017", "khanjali" },
			{ VEHICLE_RIOT2, "candc_xmas2017", "riot2" },
			{ VEHICLE_STROMBERG, "candc_xmas2017", "stromberg" },
			{ VEHICLE_THRUSTER, "candc_xmas2017", "thruster" },
			{ VEHICLE_VOLATOL, "candc_xmas2017", "volatol" },
			{ VEHICLE_HERMES, "sssa_dlc_xmas2017", "hermes" },
			{ VEHICLE_KAMACHO, "sssa_dlc_xmas2017", "kamacho" },
			{ VEHICLE_RIATA, "sssa_dlc_xmas2017", "riata" },
			{ VEHICLE_SENTINEL3, "sssa_dlc_xmas2017", "sentinel3" },
			{ VEHICLE_STREITER, "sssa_dlc_xmas2017", "streiter" },
			{ VEHICLE_YOSEMITE, "sssa_dlc_xmas2017", "yosemite" },
			{ VEHICLE_AUTARCH, "lgm_dlc_xmas2017", "autarch" },
			{ VEHICLE_COMET4, "lgm_dlc_xmas2017", "comet4" },
			{ VEHICLE_COMET5, "lgm_dlc_xmas2017", "comet5" },
			{ VEHICLE_GT500, "lgm_dlc_xmas2017", "gt500" },
			{ VEHICLE_HUSTLER, "lgm_dlc_xmas2017", "hustler" },
			{ VEHICLE_NEON, "lgm_dlc_xmas2017", "neon" },
			{ VEHICLE_PARIAH, "lgm_dlc_xmas2017", "pariah" },
			{ VEHICLE_RAIDEN, "lgm_dlc_xmas2017", "raiden" },
			{ VEHICLE_REVOLTER, "lgm_dlc_xmas2017", "revolter" },
			{ VEHICLE_SAVESTRA, "lgm_dlc_xmas2017", "savestra" },
			{ VEHICLE_SC1, "lgm_dlc_xmas2017", "sc1" },
			{ VEHICLE_VISERIS, "lgm_dlc_xmas2017", "viseris" },
			{ VEHICLE_Z190, "lgm_dlc_xmas2017", "z190" },

			// b1365 Southern san andreas autos series
			{ VEHICLE_CARACARA, "candc_assault", "caracara" },
			{ VEHICLE_ENTITY2, "lgm_dlc_assault", "entity2" },
			{ VEHICLE_FLASHGT, "lgm_dlc_assault", "flashgt" },
			{ VEHICLE_GB200, "lgm_dlc_assault", "gb200" },
			{ VEHICLE_JESTER3, "lgm_dlc_assault", "jester3" },
			{ VEHICLE_TAIPAN, "lgm_dlc_assault", "taipan" },
			{ VEHICLE_TEZERACT, "lgm_dlc_assault", "tezeract" },
			{ VEHICLE_TYRANT, "lgm_dlc_assault", "tyrant" },
			{ VEHICLE_SEASPARROW, "elt_dlc_assault", "sparrow" },
			{ VEHICLE_CHEBUREK, "sssa_dlc_assault", "cheburek" },
			{ VEHICLE_DOMINATOR3, "sssa_dlc_assault", "dominator3" },
			{ VEHICLE_ELLIE, "sssa_dlc_assault", "ellie" },
			{ VEHICLE_FAGALOA, "sssa_dlc_assault", "fagaloa" },
			{ VEHICLE_HOTRING, "sssa_dlc_assault", "hotring" },
			{ VEHICLE_ISSI3, "sssa_dlc_assault", "issi3" },
			//{ VEHICLE_ISSI3, "mba_vehicles", "issi3" },
			{ VEHICLE_MICHELLI, "sssa_dlc_assault", "michelli" },

			// b1493 After hours
			{ VEHICLE_PBUS2, "sssa_dlc_battle", "pbus2" },
			{ VEHICLE_PATRIOT, "sssa_dlc_battle", "patriot" },
			{ VEHICLE_PATRIOT2, "sssa_dlc_battle", "patriot2" },
			{ VEHICLE_FUTO, "sssa_dlc_battle", "futo" },
			{ VEHICLE_TERBYTE, "candc_hacker", "banner0" },
			{ VEHICLE_POUNDER2, "candc_battle", "pounder2" },
			{ VEHICLE_MULE4, "candc_battle", "mule4" },
			{ VEHICLE_MENACER, "candc_battle", "menacer" },
			{ VEHICLE_OPPRESSOR2, "candc_battle", "oppressor2" },
			{ VEHICLE_SCRAMJET, "candc_battle", "scramjet" },
			{ VEHICLE_STRIKEFORCE, "candc_battle", "strikeforce" },
			{ VEHICLE_SWINGER, "lgm_dlc_battle", "swinger" },
			{ VEHICLE_STAFFORD, "lgm_dlc_battle", "stafford" },
			{ VEHICLE_FREECRAWLER, "lgm_dlc_battle", "freecrawler" },
			{ VEHICLE_BLIMP3, "elt_dlc_battle", "blimp3" },

			// b1604 Arena battle
			{ VEHICLE_BLISTA3, "sssa_dlc_arena", "blista3" },
			{ VEHICLE_BRUISER, "mba_vehicles", "bruiser_c_1" },
			{ VEHICLE_BRUISER2, "mba_vehicles", "bruiser_c_2" },
			{ VEHICLE_BRUISER3, "mba_vehicles", "bruiser_c_3" },
			{ VEHICLE_BRUTUS, "mba_vehicles", "brutus1" },
			{ VEHICLE_BRUTUS2, "mba_vehicles", "brutus2" },
			{ VEHICLE_BRUTUS3, "mba_vehicles", "brutus3" },
			{ VEHICLE_CERBERUS, "mba_vehicles", "cerberus1" },
			{ VEHICLE_CERBERUS2, "mba_vehicles", "cerberus2" },
			{ VEHICLE_CERBERUS3, "mba_vehicles", "cerberus3" },
			{ VEHICLE_CLIQUE, "lgm_dlc_arena", "clique" },
			{ VEHICLE_DEATHBIKE, "mba_vehicles", "deathbike_c_1" },
			{ VEHICLE_DEATHBIKE2, "mba_vehicles", "deathbike_c_2" },
			{ VEHICLE_DEATHBIKE3, "mba_vehicles", "deathbike_c_3" },
			{ VEHICLE_DEVESTE, "lgm_dlc_arena", "deveste" },
			{ VEHICLE_DEVIANT, "lgm_dlc_arena", "deviant" },
			{ VEHICLE_DOMINATOR4, "mba_vehicles", "dominato_c_1" },
			{ VEHICLE_DOMINATOR5, "mba_vehicles", "dominato_c_2" },
			{ VEHICLE_DOMINATOR6, "mba_vehicles", "dominato_c_3" },
			{ VEHICLE_IMPALER, "mba_vehicles", "impaler" },
			{ VEHICLE_IMPALER2, "mba_vehicles", "impaler_c_1" },
			{ VEHICLE_IMPALER3, "mba_vehicles", "impaler_c_2" },
			{ VEHICLE_IMPALER4, "mba_vehicles", "impaler_c_3" },
			{ VEHICLE_IMPERATOR, "mba_vehicles", "imperator1" },
			{ VEHICLE_IMPERATOR2, "mba_vehicles", "imperator2" },
			{ VEHICLE_IMPERATOR3, "mba_vehicles", "imperator3" },
			{ VEHICLE_ISSI4, "mba_vehicles", "issi3_c_1" },
			{ VEHICLE_ISSI5, "mba_vehicles", "issi3_c_2" },
			{ VEHICLE_ISSI6, "mba_vehicles", "issi3_c_3" },
			{ VEHICLE_ITALIGTO, "lgm_dlc_arena", "italigto" },
			{ VEHICLE_MONSTER3, "mba_vehicles", "monster_c_1" },
			{ VEHICLE_MONSTER4, "mba_vehicles", "monster_c_2" },
			{ VEHICLE_MONSTER5, "mba_vehicles", "monster_c_3" },
			{ VEHICLE_RCBANDITO, "sssa_dlc_arena", "rcbandito" },
			{ VEHICLE_SCARAB, "mba_vehicles", "scarab1" },
			{ VEHICLE_SCARAB2, "mba_vehicles", "scarab2" },
			{ VEHICLE_SCARAB3, "mba_vehicles", "scarab3" },
			{ VEHICLE_SCHLAGEN, "lgm_dlc_arena", "schlagen" },
			{ VEHICLE_SLAMVAN4, "mba_vehicles", "slamvan_c_1" },
			{ VEHICLE_SLAMVAN5, "mba_vehicles", "slamvan_c_2" },
			{ VEHICLE_SLAMVAN6, "mba_vehicles", "slamvan_c_3" },
			{ VEHICLE_TOROS, "lgm_dlc_arena", "toros" },
			{ VEHICLE_TULIP, "sssa_dlc_arena", "tulip" },
			{ VEHICLE_VAMOS, "sssa_dlc_arena", "vamos" },
			{ VEHICLE_ZR380, "mba_vehicles", "zr3801" },
			{ VEHICLE_ZR3802, "mba_vehicles", "zr3802" },
			{ VEHICLE_ZR3803, "mba_vehicles", "zr3803" },
			
			//The Diamond Casino and Resort
			{ VEHICLE_DRAFTER, "lgm_dlc_vinewood", "drafter" },
			{ VEHICLE_EMERUS, "lgm_dlc_vinewood", "emerus" },
			{ VEHICLE_JUGULAR, "lgm_dlc_vinewood", "jugular" },
			{ VEHICLE_KRIEGER, "lgm_dlc_vinewood", "krieger" },
			{ VEHICLE_LOCUST, "lgm_dlc_vinewood", "locust" },
			{ VEHICLE_NEO, "lgm_dlc_vinewood", "neo" },
			{ VEHICLE_NOVAK, "lgm_dlc_vinewood", "novak" },
			{ VEHICLE_PARAGON, "lgm_dlc_vinewood", "paragon" },
			{ VEHICLE_PARAGON2, "lgm_dlc_vinewood", "paragon" },
			{ VEHICLE_RROCKET, "lgm_dlc_vinewood", "rrocket" },
			{ VEHICLE_S80, "lgm_dlc_vinewood", "s80" },
			{ VEHICLE_THRAX, "lgm_dlc_vinewood", "thrax" },
			{ VEHICLE_ZORRUSSO, "lgm_dlc_vinewood", "zorrusso" },
			{ VEHICLE_CARACARA2, "sssa_dlc_vinewood", "caracara2" },
			{ VEHICLE_DYNASTY, "sssa_dlc_vinewood", "dynasty" },
			{ VEHICLE_GAUNTLET3, "sssa_dlc_vinewood", "gauntlet3" },
			{ VEHICLE_GAUNTLET4, "sssa_dlc_vinewood", "gauntlet4" },
			{ VEHICLE_HELLION, "sssa_dlc_vinewood", "hellion" },
			{ VEHICLE_ISSI7, "sssa_dlc_vinewood", "issi7" },
			{ VEHICLE_NEBULA, "sssa_dlc_vinewood", "nebula" },
			{ VEHICLE_PEYOTE2, "sssa_dlc_vinewood", "peyote2" },
			{ VEHICLE_ZION3, "sssa_dlc_vinewood", "zion3" },

			//The Diamond Casino Heist
			{ VEHICLE_JB7002, "candc_casinoheist", "jb7002" },
			{ VEHICLE_MINITANK, "candc_casinoheist", "minitank" },
			{ VEHICLE_ZHABA, "candc_casinoheist", "zhaba" },
			{ VEHICLE_FORMULA, "lgm_dlc_casinoheist", "formula" },
			{ VEHICLE_FORMULA2, "lgm_dlc_casinoheist", "formula2" },
			{ VEHICLE_FURIA, "lgm_dlc_casinoheist", "furia" },
			{ VEHICLE_IMORGON, "lgm_dlc_casinoheist", "imorgon" },
			{ VEHICLE_KOMODA, "lgm_dlc_casinoheist", "komoda" },
			{ VEHICLE_REBLA, "lgm_dlc_casinoheist", "rebla" },
			{ VEHICLE_STRYDER, "lgm_dlc_casinoheist", "stryder" },
			{ VEHICLE_VSTR, "lgm_dlc_casinoheist", "vstr" },
			{ VEHICLE_ASBO, "sssa_dlc_casinoheist", "asbo" },
			{ VEHICLE_EVERON, "sssa_dlc_casinoheist", "everon" },
			{ VEHICLE_KANJO, "sssa_dlc_casinoheist", "kanjo" },
			{ VEHICLE_OUTLAW, "sssa_dlc_casinoheist", "outlaw" },
			{ VEHICLE_RETINUE2, "sssa_dlc_casinoheist", "retinue2" },
			{ VEHICLE_SUGOI, "sssa_dlc_casinoheist", "sugoi" },
			{ VEHICLE_SULTAN2, "sssa_dlc_casinoheist", "sultan2" },
			{ VEHICLE_VAGRANT, "sssa_dlc_casinoheist", "vagrant" },
			{ VEHICLE_YOSEMITE2, "sssa_dlc_casinoheist", "yosemite2" },

			//Los Santos Summer Special DLC
			{ VEHICLE_COQUETTE4, "lgm_dlc_summer2020", "coquette4" },
			{ VEHICLE_OPENWHEEL1, "lgm_dlc_summer2020", "openwheel1" },
			{ VEHICLE_OPENWHEEL2, "lgm_dlc_summer2020", "openwheel2" },
			{ VEHICLE_TIGON, "lgm_dlc_summer2020", "tigon" },
			{ VEHICLE_GAUNTLET5, "lsc_dlc_summer2020", "gauntlet3_b" },
			{ VEHICLE_GLENDALE2, "lsc_dlc_summer2020", "glendale_b" },
			{ VEHICLE_MANANA2, "lsc_dlc_summer2020", "manana_b" },
			{ VEHICLE_PEYOTE3, "lsc_dlc_summer2020", "peyote_b" },
			{ VEHICLE_YOSEMITE3, "lsc_dlc_summer2020", "yosemite_b" },
			{ VEHICLE_YOUGA3, "lsc_dlc_summer2020", "youga2_b" },
			{ VEHICLE_CLUB, "sssa_dlc_summer2020", "club" },
			{ VEHICLE_DUKES3, "sssa_dlc_summer2020", "dukes3" },
			{ VEHICLE_LANDSTALKER2, "sssa_dlc_summer2020", "landstlkr2" },
			{ VEHICLE_PENUMBRA2, "sssa_dlc_summer2020", "penumbra2" },
			{ VEHICLE_SEMINOLE2, "sssa_dlc_summer2020", "seminole2" },

			//The Cayo Perico Heist
			{ VEHICLE_KOSATKA, "candc_sub", "banner1" },
			{ VEHICLE_ALKONOST, "candc_heist4", "alkonost" },
			{ VEHICLE_ANNIHILATOR2, "candc_heist4", "annihlator2" },
			{ VEHICLE_AVISA, "candc_heist4", "avisa" },
			{ VEHICLE_DINGHY5, "candc_heist4", "dinghy5" },
			{ VEHICLE_MANCHEZ2, "candc_heist4", "manchez2" },
			{ VEHICLE_PATROLBOAT, "candc_heist4", "patrolboat" },
			{ VEHICLE_SEASPARROW2, "candc_heist4", "sparrow3" },
			{ VEHICLE_SEASPARROW3, "candc_heist4", "sparrow3" },
			{ VEHICLE_SQUADDIE, "candc_heist4", "squaddie" },
			{ VEHICLE_TOREADOR, "candc_heist4", "toreador" },
			{ VEHICLE_VERUS, "candc_heist4", "verus" },
			{ VEHICLE_VETIR, "candc_heist4", "vetir" },
			{ VEHICLE_WINKY, "candc_heist4", "winky" },
			{ VEHICLE_LONGFIN, "dock_dlc_heist4", "longfin" },
			{ VEHICLE_ITALIRSX, "lgm_dlc_heist4", "italirsx" },
			{ VEHICLE_BRIOSO2, "sssa_dlc_heist4", "brioso2" },
			{ VEHICLE_SLAMTRUCK, "sssa_dlc_heist4", "slamtruck" },
			{ VEHICLE_VETO, "sssa_dlc_heist4", "veto" },
			{ VEHICLE_VETO2, "sssa_dlc_heist4", "veto2" },
			{ VEHICLE_WEEVIL, "sssa_dlc_heist4", "weevil" },

			//Los Santos Tuner
			{ VEHICLE_COMET6, "lgm_dlc_tuner", "comet6" },
			{ VEHICLE_CYPHER, "lgm_dlc_tuner", "cypher" },
			{ VEHICLE_EUROS, "lgm_dlc_tuner", "euros" },
			{ VEHICLE_GROWLER, "lgm_dlc_tuner", "growler" },
			{ VEHICLE_JESTER4, "lgm_dlc_tuner", "jester4" },
			{ VEHICLE_TAILGATER2, "lgm_dlc_tuner", "tailgater2" },
			{ VEHICLE_VECTRE, "lgm_dlc_tuner", "vectre" },
			{ VEHICLE_ZR350, "lgm_dlc_tuner", "zr350" },
			{ VEHICLE_CALICO, "sssa_dlc_tuner", "calico" },
			{ VEHICLE_DOMINATOR7, "sssa_dlc_tuner", "dominator7" },
			{ VEHICLE_DOMINATOR8, "sssa_dlc_tuner", "dominator8" },
			{ VEHICLE_FUTO2, "sssa_dlc_tuner", "futo2" },
			{ VEHICLE_PREVION, "sssa_dlc_tuner", "previon" },
			{ VEHICLE_REMUS, "sssa_dlc_tuner", "remus" },
			{ VEHICLE_RT3000, "sssa_dlc_tuner", "rt3000" },
			{ VEHICLE_SULTAN3, "sssa_dlc_tuner", "sultan3" },
			{ VEHICLE_WARRENER2, "sssa_dlc_tuner", "warrener2" },

			//The Contract
			{ VEHICLE_ASTRON, "lgm_dlc_security", "astron" },
			{ VEHICLE_BALLER7, "lgm_dlc_security", "baller7" },
			{ VEHICLE_CHAMPION, "lgm_dlc_security", "champion" },
			{ VEHICLE_CINQUEMILA, "lgm_dlc_security", "cinquemila" },
			{ VEHICLE_COMET7, "lgm_dlc_security", "comet7" },
			{ VEHICLE_DEITY, "lgm_dlc_security", "deity" },
			{ VEHICLE_IGNUS, "lgm_dlc_security", "ignus" },
			{ VEHICLE_JUBILEE, "lgm_dlc_security", "jubilee" },
			{ VEHICLE_REEVER, "lgm_dlc_security", "reever" },
			{ VEHICLE_SHINOBI, "lgm_dlc_security", "shinobi" },
			{ VEHICLE_ZENO, "lgm_dlc_security", "zeno" },
			{ VEHICLE_BUFFALO4, "sssa_dlc_security", "buffalo4" },
			{ VEHICLE_GRANGER2, "sssa_dlc_security", "granger2" },
			{ VEHICLE_IWAGEN, "sssa_dlc_security", "iwagen" },
			{ VEHICLE_PATRIOT3, "sssa_dlc_security", "patriot3" },
			{ VEHICLE_MULE5, "candc_default", "mule3" },

			//The Criminal Enterprises
			{ VEHICLE_CONADA, "elt_dlc_sum2", "conada" },
			{ VEHICLE_BRIOSO3, "lsc_dlc_sum2", "brioso2_b" },
			{ VEHICLE_SENTINEL4, "lsc_dlc_sum2", "sentinel3_b" },
			{ VEHICLE_TENF, "lsc_dlc_sum2", "tenf" },
			{ VEHICLE_TENF2, "lsc_dlc_sum2", "tenf_b" },
			{ VEHICLE_WEEVIL2, "lsc_dlc_sum2", "weevil_b" },
			{ VEHICLE_CORSITA, "lgm_dlc_sum2", "corsita" },
			{ VEHICLE_LM87, "lgm_dlc_sum2", "lm87" },
			{ VEHICLE_OMNISEGT, "lgm_dlc_sum2", "omnisegt" },
			{ VEHICLE_SM722, "lgm_dlc_sum2", "sm722" },
			{ VEHICLE_TORERO2, "lgm_dlc_sum2", "torero2" },
			{ VEHICLE_DRAUGUR, "sssa_dlc_sum2", "draugur" },
			{ VEHICLE_GREENWOOD, "sssa_dlc_sum2", "greenwood" },
			{ VEHICLE_KANJOSJ, "sssa_dlc_sum2", "kanjosj" },
			{ VEHICLE_POSTLUDE, "sssa_dlc_sum2", "postlude" },
			{ VEHICLE_RHINEHART, "sssa_dlc_sum2", "rhinehart" },
			{ VEHICLE_RUINER4, "sssa_dlc_sum2", "ruiner4" },
			{ VEHICLE_VIGERO2, "sssa_dlc_sum2", "vigero2" },

			//Los Santos Drug Wars
			{ VEHICLE_BRICKADE2, "candc_xmas2022", "brickade2" },
			{ VEHICLE_TAXI, "candc_xmas2022", "taxi" },
			{ VEHICLE_BROADWAY, "lgm_dlc_xmas2022", "broadway" },
			{ VEHICLE_ENTITY3, "lgm_dlc_xmas2022", "entity3" },
			{ VEHICLE_PANTHERE, "lgm_dlc_xmas2022", "panthere" },
			{ VEHICLE_POWERSURGE, "lgm_dlc_xmas2022", "powersurge" },
			{ VEHICLE_R300, "lgm_dlc_xmas2022", "r300" },
			{ VEHICLE_VIRTUE, "lgm_dlc_xmas2022", "virtue" },
			{ VEHICLE_BOOR, "sssa_dlc_xmas2022", "boor" },
			{ VEHICLE_EUDORA, "sssa_dlc_xmas2022", "eudora" },
			{ VEHICLE_EVERON2, "sssa_dlc_xmas2022", "everon2" },
			{ VEHICLE_ISSI8, "sssa_dlc_xmas2022", "issi8" },
			{ VEHICLE_JOURNEY2, "sssa_dlc_xmas2022", "journey2" },
			{ VEHICLE_SURFER3, "sssa_dlc_xmas2022", "surfer3" },
			{ VEHICLE_TAHOMA, "sssa_dlc_xmas2022", "tahoma" },
			{ VEHICLE_TULIP2, "sssa_dlc_xmas2022", "tulip2" },
			{ VEHICLE_MANCHEZ3, "candc_heist4", "manchez2" },

			//San Andreas Mercenaries
			{ VEHICLE_CONADA2, "candc_2023_01", "conada2" },
			{ VEHICLE_RAIJU, "candc_2023_01", "raiju" },
			{ VEHICLE_STREAMER216, "candc_2023_01", "streamer216" },
			{ VEHICLE_BUFFALO5, "lgm_dlc_2023_01", "buffalo5" },
			{ VEHICLE_COUREUR, "lgm_dlc_2023_01", "coureur" },
			{ VEHICLE_STINGERTT, "lgm_dlc_2023_01", "stingertt" },
			{ VEHICLE_INDUCTOR, "pandm_dlc_2023_01", "inductor" },
			{ VEHICLE_INDUCTOR2, "pandm_dlc_2023_01", "inductor2" },
			{ VEHICLE_BRIGHAM, "sssa_dlc_2023_01", "brigham" },
			{ VEHICLE_CLIQUE2, "sssa_dlc_2023_01", "clique2" },
			{ VEHICLE_GAUNTLET6, "sssa_dlc_2023_01", "gauntlet6" },
			{ VEHICLE_L35, "sssa_dlc_2023_01", "l35" },
			{ VEHICLE_MONSTROCITI, "sssa_dlc_2023_01", "monstrociti" },
			{ VEHICLE_RATEL, "sssa_dlc_2023_01", "ratel" },

			//Chop Shop DLC
			{ VEHICLE_BENSON2, "candc_2023_2", "benson2" },
			{ VEHICLE_BOXVILLE6, "candc_2023_2", "boxville6" },
			{ VEHICLE_POLGAUNTLET, "candc_2023_2", "polgauntlet" },
			{ VEHICLE_POLICE4, "candc_2023_2", "police4" },
			{ VEHICLE_POLICE5, "candc_2023_2", "police5" },
			{ VEHICLE_PRANGER, "candc_2023_2", "pranger" },
			{ VEHICLE_RIOT, "candc_2023_2", "riot" },
			{ VEHICLE_ALEUTIAN, "lgm_dlc_2023_2", "aleutian" },
			{ VEHICLE_BALLER8, "lgm_dlc_2023_2", "baller8" },
			{ VEHICLE_TURISMO3, "lgm_dlc_2023_2", "turismo3" },
			{ VEHICLE_ASTEROPE2, "sssa_dlc_2023_2", "asterope2" },
			{ VEHICLE_CAVALCADE3, "sssa_dlc_2023_2", "cavalcade3" },
			{ VEHICLE_DOMINATOR9, "sssa_dlc_2023_2", "dominator9" },
			{ VEHICLE_DORADO, "sssa_dlc_2023_2", "dorado" },
			{ VEHICLE_FR36, "sssa_dlc_2023_2", "fr36" },
			{ VEHICLE_IMPALER5, "sssa_dlc_2023_2", "impaler5" },
			{ VEHICLE_IMPALER6, "sssa_dlc_2023_2", "impaler6" },
			{ VEHICLE_TERMINUS, "sssa_dlc_2023_2", "terminus" },
			{ VEHICLE_VIGERO3, "sssa_dlc_2023_2", "vigero3" },
			{ VEHICLE_VIVANITE, "sssa_dlc_2023_2", "vivanite" },
			{ VEHICLE_DRIFTFR36, "sssa_dlc_2023_2", "fr36" },
			{ VEHICLE_DRIFTEUROS, "lgm_dlc_tuner", "euros" },
			{ VEHICLE_DRIFTJESTER, "lgm_dlc_tuner", "jester4" },
			{ VEHICLE_DRIFTZR350, "lgm_dlc_tuner", "zr350" },
			{ VEHICLE_DRIFTFUTO, "sssa_dlc_tuner", "futo2" },
			{ VEHICLE_DRIFTREMUS, "sssa_dlc_tuner", "remus" },
			{ VEHICLE_DRIFTTAMPA, "sssa_dlc_stunt", "tampa2" },
			{ VEHICLE_PHANTOM4, "candc_importexport", "phantom2" },
			{ VEHICLE_DRIFTYOSEMITE, "sssa_dlc_casinoheist", "yosemite2" },

			//Bottom Dollar Bounties DLC
			{ VEHICLE_POLDOMINATOR10, "candc_dlc_2024_1", "poldom10" },
			{ VEHICLE_POLDORADO, "candc_dlc_2024_1", "poldorado" },
			{ VEHICLE_POLGREENWOOD, "candc_dlc_2024_1", "polgreenw" },
			{ VEHICLE_POLIMPALER5, "candc_dlc_2024_1", "polimpaler5" },
			{ VEHICLE_POLIMPALER6, "candc_dlc_2024_1", "polimpaler6" },
			{ VEHICLE_COQUETTE5, "lgm_dlc_2024_1", "coquette5" },
			{ VEHICLE_ENVISAGE, "lgm_dlc_2024_1", "envisage" },
			{ VEHICLE_EUROSX32, "lgm_dlc_2024_1", "eurosx32" },
			{ VEHICLE_NIOBE, "lgm_dlc_2024_1", "niobe" },
			{ VEHICLE_PARAGON3, "lgm_dlc_2024_1", "paragon3" },
			{ VEHICLE_PIPISTRELLO, "lgm_dlc_2024_1", "pipistrello" },
			{ VEHICLE_CASTIGATOR, "sssa_dlc_2024_1", "castigator" },
			{ VEHICLE_DOMINATOR10, "sssa_dlc_2024_1", "dominator10" },
			{ VEHICLE_PIZZABOY, "sssa_dlc_2024_1", "pizzaboy" },
			{ VEHICLE_VORSCHLAGHAMMER, "sssa_dlc_2024_1", "vorschlag" },
			{ VEHICLE_YOSEMITE1500, "sssa_dlc_2024_1", "yosemite4" },
			{ VEHICLE_DRIFTVORSCHLAG, "sssa_dlc_2024_1", "vorschlag" },
			{ VEHICLE_DRIFTCYPHER, "lgm_dlc_tuner", "cypher" },
			{ VEHICLE_DRIFTNEBULA, "sssa_dlc_vinewood", "nebula" },
			{ VEHICLE_DRIFTSENTINEL, "lsc_dlc_sum2", "sentinel3_b" },

			//Agents of Sabotage
			{ VEHICLE_BANSHEE3, "lgm_dlc_2024_2", "banshee3" },
			{ VEHICLE_CARGOBOB5, "candc_dlc_2024_2", "cargobob5" },
			{ VEHICLE_CHAVOSV6, "sssa_dlc_2024_2", "chavosv6" },
			{ VEHICLE_COQUETTE6, "lgm_dlc_2024_2", "coquette6" },
			{ VEHICLE_DRIFTCHEBUREK, "sssa_dlc_assault", "cheburek" },
			{ VEHICLE_DRIFTFUTO2, "sssa_dlc_battle", "futo" },
			{ VEHICLE_DRIFTJESTER3, "lgm_dlc_assault", "jester3" },
			{ VEHICLE_DUSTER2, "elt_dlc_2024_2", "duster2" },
			{ VEHICLE_FIREBOLT, "sssa_dlc_2024_2", "firebolt" },
			{ VEHICLE_JESTER5, "lgm_dlc_2024_2", "jester5" },
			{ VEHICLE_POLCARACARA, "candc_dlc_2024_2", "polcaracar" },
			{ VEHICLE_POLCOQUETTE4, "candc_dlc_2024_2", "polcoqutt4" },
			{ VEHICLE_POLFACTION2, "candc_dlc_2024_2", "polfation2" },
			{ VEHICLE_POLTERMINUS, "candc_dlc_2024_2", "polterminu" },
			{ VEHICLE_URANUS, "sssa_dlc_2024_2", "uranus" },
			{ VEHICLE_TITAN2, "candc_dlc_2024_2", "titan2" },
			{ VEHICLE_TACO, "candc_dlc_2024_2", "taco" },
		};
		void PopulateVehicleBmps()
		{
			std::vector<std::string> fileNames;
			std::string path = GetPathffA(Pathff::Graphics, true) + "Vehicle Previews\\";
			get_all_filenames_with_extension(path, ".png", fileNames, false);
			for (auto& f : fileNames)
			{
				Hash fhash = GET_HASH_KEY(f);
				auto it = std::find(vehicleBMPs.begin(), vehicleBMPs.end(), fhash);
				if (it == vehicleBMPs.end())
				{
					VehBmpSprite bmp(fhash);
					if (bmp.LoadFile(path + f + ".png"))
					{
						vehicleBMPs.push_back(bmp);
					}
				}
				else
				{
					if (!it->tid.Exists())
					{
						it->LoadFile(path + f + ".png");
					}
				}
			}
		}
		void DrawVehicleBmp(const GTAmodel::Model& vehModel)
		{
			Vector2 res = { 0.1f, 0.0889f };

			FLOAT x_coord = 0.324f + menuPos.x;
			FLOAT y_coord = OptionY + 0.044f + menuPos.y;

			if (menuPos.x > 0.45f)
			{
				x_coord = menuPos.x - 0.003f;
			}

			DRAW_RECT(x_coord, y_coord, res.x + 0.003f, res.y + 0.003f, 0, 0, 0, 212, false);

			auto vit = std::find(vehicleBMPs.begin(), vehicleBMPs.end(), vehModel.hash);
			if (vit != vehicleBMPs.end()) //if found
			{
				if (vit->tid.Exists())//if dxtexture png
				{
					vit->tid.Draw(0, Vector2(x_coord, y_coord), Vector2(res.x, res.y / 2 + 0.005f), 0.0f, RGBA::AllWhite());
				}
				else//if in-game sprite
				{
					if (!HAS_STREAMED_TEXTURE_DICT_LOADED(vit->dict.c_str()))
					{
						REQUEST_STREAMED_TEXTURE_DICT(vit->dict.c_str(), false);
					}
					DRAW_SPRITE(vit->dict.c_str(), vit->imgName.c_str(), x_coord, y_coord, res.x, res.y, 0.0f, 255, 255, 255, 255, false, 0);
				}
			}
			else
			{
				Game::Print::SetupDraw(0, Vector2(0, 0.185f), true, false, false);
				Game::Print::drawstring("No preview available", x_coord, y_coord - 0.0043f);
			}
		}
		void DrawVehicleModelName(const GTAmodel::Model& vehModel)
		{
			FLOAT x_coord = menuPos.x + 0.25f;
			FLOAT y_coord = OptionY + menuPos.y;

			Game::Print::SetupDraw(font_selection, Vector2(0.0f, (font_options == 0? 0.33f:0.4f)), false, true, false, selectedtext,{0, x_coord});
			Game::Print::drawstring("ModelName: " + vehModel.VehicleModelName(), 0, y_coord);
		}

		void AddVehicleCategoryOption(const std::string& text, UINT8 index, bool *extra_option_code)
		{
			std::vector<Model>* tempvecp;
			switch (index)
			{
				case OPENWHEEL:		tempvecp = &g_vehHashes_OPENWHEEL; break;
				case SUPER:			tempvecp = &g_vehHashes_SUPER; break;
				case SPORT:			tempvecp = &g_vehHashes_SPORT; break;
				case SPORTSCLASSIC: tempvecp = &g_vehHashes_SPORTSCLASSIC; break;
				case COUPE:			tempvecp = &g_vehHashes_COUPE; break;
				case MUSCLE:		tempvecp = &g_vehHashes_MUSCLE; break;
				case OFFROAD:		tempvecp = &g_vehHashes_OFFROAD; break;
				case SUV:			tempvecp = &g_vehHashes_SUV; break;
				case SEDAN:			tempvecp = &g_vehHashes_SEDAN; break;
				case COMPACT:		tempvecp = &g_vehHashes_COMPACT; break;
				case VAN:			tempvecp = &g_vehHashes_VAN; break;
				case SERVICE:		tempvecp = &g_vehHashes_SERVICE; break;
				case TRAIN:			tempvecp = &g_vehHashes_TRAIN; break;
				case EMERGENCY:		tempvecp = &g_vehHashes_EMERGENCY; break;
				case MOTORCYCLE:	tempvecp = &g_vehHashes_MOTORCYCLE; break;
				case BICYCLE:		tempvecp = &g_vehHashes_BICYCLE; break;
				case PLANE:			tempvecp = &g_vehHashes_PLANE; break;
				case HELICOPTER:	tempvecp = &g_vehHashes_HELICOPTER; break;
				case BOAT:			tempvecp = &g_vehHashes_BOAT; break;
				case INDUSTRIAL:	tempvecp = &g_vehHashes_INDUSTRIAL; break;
				case COMMERCIAL:	tempvecp = &g_vehHashes_COMMERCIAL; break;
				case UTILITY:		tempvecp = &g_vehHashes_UTILITY; break;
				case MILITARY:		tempvecp = &g_vehHashes_MILITARY; break;
				case OTHER:			tempvecp = &g_vehHashes_OTHER; break;
				case DRIFT:			tempvecp = &g_vehHashes_DRIFT; break;
			}
			if (tempvecp->empty())
			{
				return;
			}

			bool pressed = false;
			AddOption(text, pressed, nullFunc, -1, true); if (pressed)
			{
				spawnVehicleIndex = index;
				dict = std::string(text);
				Menu::SetSub_delayed = SUB::SPAWNVEHICLE_ALLCATS;

				if (extra_option_code != nullptr)
				{
					*extra_option_code = true;
				}
			}
		}

		void AddVehicleSpawnOption(const std::string& text, const GTAmodel::Model& vehModel, Ped ped)
		{
			Vector3 toBeNodeCoordinates = GTAped(ped).GetPosition();
			bool pressed = false;
			AddOption(text, pressed, nullFunc, -1, false, false); 
			if (pressed)
			{
				Vehicle vehicle = SpawnVehicle(vehModel, ped, g_spawnVehicleDeleteOld, g_spawnVehicleAutoSit);
				SetVehicleMaxUpgrades(vehicle, g_spawnVehicleAutoUpgrade, g_spawnVehicleInvincible,
					g_spawnVehiclePlateType, g_spawnVehiclePlateTexterValue == 0 ? g_spawnVehiclePlateText : "", g_spawnVehicleNeonToggle,
					g_spawnVehicleNeonColor.R, g_spawnVehicleNeonColor.G, g_spawnVehicleNeonColor.B,
					g_spawnVehiclePrimaryColor, g_spawnVehicleSecondaryColor);

				if (g_addBlip)
				{
					blip_g = ADD_BLIP_FOR_ENTITY(vehicle);
					HUD::SET_BLIP_SPRITE(blip_g, GetRandomSpriteId());
					Game::Print::PrintBottomLeft("Added a blip.");
				}
				if (g_warpNear)
				{
					Vector3_t outPos;

					for (int i = 1; i < 40; i++)
					{
						GET_NTH_CLOSEST_VEHICLE_NODE(toBeNodeCoordinates.x, toBeNodeCoordinates.y, toBeNodeCoordinates.z, i, &outPos, 1, 0x40400000, 0);

						if (!IS_POINT_OBSCURED_BY_A_MISSION_ENTITY(outPos.x, outPos.y, outPos.z, 5.0f, 5.0f, 5.0f, 0))
						{
							SET_ENTITY_COORDS(vehicle, outPos.x, outPos.y, outPos.z, 0, 0, 0, 0);
						}
					}
				}
				if (!NETWORK_IS_IN_SESSION() && !g_spawnVehiclePersistent)
				{
					SET_VEHICLE_AS_NO_LONGER_NEEDED(&vehicle);
				}
			}
		}
	}

	void SpawnVehicleMenu()
	{
		using namespace VehicleSpawner;
		dict2.clear();
		dict3.clear();
		bool spawnRandom = 0;
		bool spawnVehicleInput = 0;

		AddTitle("Vehicles");

		AddOption("Spawn Settings", null, nullFunc, SUB::SPAWNVEHICLE_OPTIONS);
		AddOption("Saved Vehicles", null, nullFunc, SUB::VEHICLE_SAVER);
		AddOption("Favourites", null, nullFunc, SUB::SPAWNVEHICLE_FAVOURITES);
		AddOption("Funny Vehicles (Old)", null, nullFunc, SUB::FUNNYVEHICLES);

		AddBreak("---Cars---");
		AddVehicleCategoryOption("Open Wheel", OPENWHEEL);
		AddVehicleCategoryOption("Super", SUPER);
		AddVehicleCategoryOption("Sports", SPORT);
		AddVehicleCategoryOption("Sports Classics", SPORTSCLASSIC);
		AddVehicleCategoryOption("Coupes", COUPE);
		AddVehicleCategoryOption("Muscle", MUSCLE);
		AddVehicleCategoryOption("Offroad", OFFROAD);
		AddVehicleCategoryOption("SUVs", SUV);
		AddVehicleCategoryOption("Sedans", SEDAN);
		AddVehicleCategoryOption("Compacts", COMPACT);
		AddVehicleCategoryOption("Drift", DRIFT);

		AddBreak("---Industrial---");
		AddVehicleCategoryOption("Vans", VAN);
		AddVehicleCategoryOption("Services", SERVICE);
		AddVehicleCategoryOption("Industrial", INDUSTRIAL);
		AddVehicleCategoryOption("Commercial", COMMERCIAL);
		AddVehicleCategoryOption("Utility", UTILITY);
		AddVehicleCategoryOption("Trains", TRAIN);

		AddBreak("---Others---");
		AddVehicleCategoryOption("Emergency", EMERGENCY);
		AddVehicleCategoryOption("Military", MILITARY);
		AddVehicleCategoryOption("Motorcycles", MOTORCYCLE);
		AddVehicleCategoryOption("Bicycles", BICYCLE);
		AddVehicleCategoryOption("Planes", PLANE);
		AddVehicleCategoryOption("Helicopters", HELICOPTER);
		AddVehicleCategoryOption("Boats", BOAT);
		AddVehicleCategoryOption("Others", OTHER);

		AddOption("Random Vehicle", spawnRandom);
		AddOption("~b~Input~s~ Model", spawnVehicleInput);

		if (spawnRandom || spawnVehicleInput)
		{
			Model model;
			Ped ped = g_Ped1;

			if (spawnRandom)
			{
				if (g_vehHashes.empty())
				{
					return;
				}
				model = g_vehHashes[GET_RANDOM_INT_IN_RANGE(0, (int)g_vehHashes.size())];
				
			}
			else if (spawnVehicleInput)
			{
				std::string inputStr = Game::InputBox("", 64U, "Enter vehicle model name (e.g. adder):");
				if (inputStr.length() == 0)
				{
					return;
				}
				model = GET_HASH_KEY(inputStr);
			}

			if (model.IsInCdImage() && model.IsVehicle())
			{
				Vehicle spawnedVehicle = SpawnVehicle(model, ped, g_spawnVehicleDeleteOld, g_spawnVehicleAutoSit);

				SetVehicleMaxUpgrades(spawnedVehicle, g_spawnVehicleAutoUpgrade, g_spawnVehicleInvincible,
					g_spawnVehiclePlateType, g_spawnVehiclePlateTexterValue == 0 ? g_spawnVehiclePlateText : "", g_spawnVehicleNeonToggle,
					g_spawnVehicleNeonColor.R, g_spawnVehicleNeonColor.G, g_spawnVehicleNeonColor.B,
					g_spawnVehiclePrimaryColor, g_spawnVehicleSecondaryColor);

				if (!NETWORK_IS_IN_SESSION() && !g_spawnVehiclePersistent)
					SET_VEHICLE_AS_NO_LONGER_NEEDED(&spawnedVehicle);
			}
			else Game::Print::PrintErrorInvalidModel(model.VehicleModelName());
		}
	}

	void SpawnVehicleOptions()
	{
		bool vehPlatePlus = 0;
		bool vehPlateMinus = 0; 
		bool vehPlateTextSet = 0; 
		bool vehPlateTextPlus = 0; 
		bool vehPlateTextMinus = 0;
		bool setMSPaintIndex10 = 0; 
		bool setMSPaintIndex11 = 0; 
		bool setRGBCarcolIndex9 = 0;

		auto& plateTexter_value = g_spawnVehiclePlateTexterValue;
		std::vector<std::string> plateTexter{ g_spawnVehiclePlateText, "Random" };
		std::vector<std::string> plateTypeNames{ "CMOD_PLA_0", "CMOD_PLA_4", "CMOD_PLA_3", "CMOD_PLA_1", "CMOD_PLA_2", "Yankton", "CMOD_PLA_6", "CMOD_PLA_7", "CMOD_PLA_8", "CMOD_PLA_9", "CMOD_PLA_10", "CMOD_PLA_11", "CMOD_PLA_12" }; // BOW1, YOBLA, YOBLU, BOW2, BOW3, YANKTON, ECOLA, LASVENTURAS, LIBERTYCITY, LSCARMEET, LSPOUNDERS, SPRUNK 

		AddTitle("Spawn Settings");
		AddToggle("Delete Old Vehicle", g_spawnVehicleDeleteOld);
		AddToggle("Auto-Sit In Vehicle", g_spawnVehicleAutoSit);
		AddToggle("Add Blip For Spawned Vehicles", g_addBlip);
		AddToggle("Spawn At Nearest Node", g_warpNear);
		AddToggle("Spawn Pre-Upgraded", g_spawnVehicleAutoUpgrade);
		AddToggle("Spawn Invincible", g_spawnVehicleInvincible);
		AddToggle("Spawn Persistent", g_spawnVehiclePersistent);
		AddOption("Primary Paint", setMSPaintIndex10, nullFunc, SUB::MSPAINTS2); // Primary Paint
		AddOption("Secondary Paint", setMSPaintIndex11, nullFunc, SUB::MSPAINTS2); // Secondary Paint
		AddBreak("---Neons---");
		AddToggle("Toggle", g_spawnVehicleNeonToggle);
		AddOption("RGB Colour", setRGBCarcolIndex9, nullFunc, SUB::MSPAINTS_RGB); 
		if (*Menu::currentopATM == Menu::printingop) 
		{
			AddPresetColourOptionsPreview(g_spawnVehicleNeonColor);
		}

		AddBreak("---Plate---");
		AddTexter("Type", g_spawnVehiclePlateType, plateTypeNames, null, vehPlatePlus, vehPlateMinus);
		AddTexter("Text", plateTexter_value, plateTexter, vehPlateTextSet, vehPlateTextPlus, vehPlateTextMinus);

		AddBreak("---Previews---");
		AddToggle("Enable Previews", g_spawnVehicleDrawBMPs);
		bool reloadPreviewsPressed = false;
		AddOption("Reload Previews", reloadPreviewsPressed); 

		if (reloadPreviewsPressed)
		{
			VehicleSpawner::PopulateVehicleBmps();
			FolderPreviewBmps_catind::PopulateFolderBmps();
		}

		if (setMSPaintIndex10) 
		{
			msCurrentPaintIndex = 10;
		}
		if (setMSPaintIndex11) 
		{
			msCurrentPaintIndex = 11;
		}
		if (setRGBCarcolIndex9) 
		{
			bitMSPaintsRGBMode = 9;
		}

		if (vehPlatePlus) 
		{ 
			if (g_spawnVehiclePlateType < (INT)(plateTypeNames.size() - 1)) 
			{
				g_spawnVehiclePlateType++; 
				return;
			}
		}
		if (vehPlateMinus)
		{ 
			if (g_spawnVehiclePlateType > 0)
			{
				g_spawnVehiclePlateType--; 
				return; 
			}
		}

		if (vehPlateTextPlus) 
		{ 
			if (plateTexter_value < 1) 
			{
				plateTexter_value++; 
				return; 
			}
		}
		if (vehPlateTextMinus) 
		{ 
			if (plateTexter_value > 0) 
			{
				plateTexter_value--; 
				return; 
			}
		}

		if (vehPlateTextSet)
		{
			switch (plateTexter_value)
			{
			case 0:
				std::string inputStr = Game::InputBox("", 9U, "CMOD_MOD_18_D", g_spawnVehiclePlateText);
				if (inputStr.length() > 0 && inputStr.length() <= 8)
				{
					g_spawnVehiclePlateText = inputStr;
				}
				else
				{
					Game::Print::PrintErrorInvalidInput(inputStr);
				}
				break;
			}
			return;
		}

	}

	bool SpawnVehicleIsVehicleModelAFavourite(GTAmodel::Model vehModel)
	{
		std::string xmlAddedVehicleModels = "AddedVehicleModels.xml";
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()).status != pugi::status_ok)
		{
			return false;
		}
		pugi::xml_node nodeRoot = doc.child("AddedVehicleModels");
		const std::string& vehModelName = vehModel.VehicleDisplayName(false);
		return nodeRoot.find_child_by_attribute("modelName", vehModelName.c_str()) || nodeRoot.find_child_by_attribute("modelHash", IntToHexString(vehModel.hash, true).c_str());
	}
	bool SpawnVehicleAddVehicleModelToFavourites(GTAmodel::Model vehModel, const std::string& customName)
	{
		if (customName.empty())
		{
			return false;
		}
		std::string xmlAddedVehicleModels = "AddedVehicleModels.xml";
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()).status != pugi::status_ok)
		{
			doc.reset();
			auto nodeDecleration = doc.append_child(pugi::node_declaration);
			nodeDecleration.append_attribute("version") = "1.0";
			nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
			auto nodeRoot = doc.append_child("AddedVehicleModels");
			doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str());
		}
		pugi::xml_node nodeRoot = doc.child("AddedVehicleModels");

		const std::string& vehModelName = vehModel.VehicleDisplayName(false);
		auto nodeOldLoc = nodeRoot.find_child_by_attribute("modelHash", IntToHexString(vehModel.hash, true).c_str());
		if (!nodeOldLoc) // If null
		{
			nodeOldLoc = nodeRoot.find_child_by_attribute("modelName", vehModelName.c_str());
		}
		if (nodeOldLoc) // If not null
		{
			nodeOldLoc.parent().remove_child(nodeOldLoc);
		}
		auto nodeNewLoc = nodeRoot.append_child("VehModel");
		nodeNewLoc.append_attribute("modelName") = vehModelName.c_str();
		nodeNewLoc.append_attribute("modelHash") = IntToHexString(vehModel.hash, true).c_str();
		nodeNewLoc.append_attribute("customName") = customName.c_str();
		return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()));
	}
	bool SpawnVehicleRemoveVehicleModelFromFavourites(GTAmodel::Model vehModel)
	{
		std::string xmlAddedVehicleModels = "AddedVehicleModels.xml";
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()).status != pugi::status_ok)
		{
			return false;
		}
		pugi::xml_node nodeRoot = doc.child("AddedVehicleModels");

		const std::string& vehModelName = vehModel.VehicleDisplayName(false);
		auto nodeOldLoc = nodeRoot.find_child_by_attribute("modelHash", IntToHexString(vehModel.hash, true).c_str());
		if (!nodeOldLoc) // If null
		{
			nodeOldLoc = nodeRoot.find_child_by_attribute("modelName", vehModelName.c_str());
		}
		if (nodeOldLoc) // If not null
		{
			nodeOldLoc.parent().remove_child(nodeOldLoc);
		}
		return (doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()));
	}

	void SpawnVehicleAllCategoriesMenu()
	{
		using namespace VehicleSpawner;
		const std::vector<Model> *tempVehicleCopy;

		AddTitle(dict);

		switch (spawnVehicleIndex)
		{
			case OPENWHEEL:		tempVehicleCopy = &g_vehHashes_OPENWHEEL; break;
			case SUPER:			tempVehicleCopy = &g_vehHashes_SUPER; break;
			case SPORT:			tempVehicleCopy = &g_vehHashes_SPORT; break;
			case SPORTSCLASSIC: tempVehicleCopy = &g_vehHashes_SPORTSCLASSIC; break;
			case COUPE:			tempVehicleCopy = &g_vehHashes_COUPE; break;
			case MUSCLE:		tempVehicleCopy = &g_vehHashes_MUSCLE; break;
			case OFFROAD:		tempVehicleCopy = &g_vehHashes_OFFROAD; break;
			case SUV:			tempVehicleCopy = &g_vehHashes_SUV; break;
			case SEDAN:			tempVehicleCopy = &g_vehHashes_SEDAN; break;
			case COMPACT:		tempVehicleCopy = &g_vehHashes_COMPACT; break;
			case VAN:			tempVehicleCopy = &g_vehHashes_VAN; break;
			case SERVICE:		tempVehicleCopy = &g_vehHashes_SERVICE; break;
			case TRAIN:			tempVehicleCopy = &g_vehHashes_TRAIN; break;
			case EMERGENCY:		tempVehicleCopy = &g_vehHashes_EMERGENCY; break;
			case MOTORCYCLE:	tempVehicleCopy = &g_vehHashes_MOTORCYCLE; break;
			case BICYCLE:		tempVehicleCopy = &g_vehHashes_BICYCLE; break;
			case PLANE:			tempVehicleCopy = &g_vehHashes_PLANE; break;
			case HELICOPTER:	tempVehicleCopy = &g_vehHashes_HELICOPTER; break;
			case BOAT:			tempVehicleCopy = &g_vehHashes_BOAT; break;
			case INDUSTRIAL:	tempVehicleCopy = &g_vehHashes_INDUSTRIAL; break;
			case COMMERCIAL:	tempVehicleCopy = &g_vehHashes_COMMERCIAL; break;
			case UTILITY:		tempVehicleCopy = &g_vehHashes_UTILITY; break;
			case MILITARY:		tempVehicleCopy = &g_vehHashes_MILITARY; break;
			case OTHER:			tempVehicleCopy = &g_vehHashes_OTHER; break;
			case DRIFT:			tempVehicleCopy = &g_vehHashes_DRIFT; break;
		}

		if (tempVehicleCopy->empty())
		{
			Menu::SetPreviousMenu();
			return;
		}

		for (auto& vehModel : *tempVehicleCopy)
		{
			if (!vehModel.IsInCdImage())
			{
				continue;

			}
			bool setEngineSoundPressed = false;
			bool spawnVehicleForAllPlayersPressed = false;

			switch (Menu::currentArray[Menu::currentArrayIndex])
			{
			case SUB::SPOONER_SPAWN_VEHICLE:
				sub::Spooner::MenuOptions::AddOption_AddVehicle(vehModel.VehicleDisplayName(true), vehModel);
				break;
			case SUB::SPAWNVEHICLE:
				AddVehicleSpawnOption(vehModel.VehicleDisplayName(true), vehModel, g_Ped1);
				break;

			}
			/// one submenu back >>>
			switch (Menu::currentArray[Menu::currentArrayIndex - 1]) /// -1
			{
			case SUB::OBJECTGUN:
				AddObjectGunOption(vehModel.VehicleDisplayName(true), vehModel, nullptr, false);
				break;
			case SUB::KABOOMGUN:
				AddKaboomGunOption(vehModel.VehicleDisplayName(true), vehModel.hash, nullptr, false);
				break;
			case SUB::MSENGINESOUND:
				AddTickol(vehModel.VehicleDisplayName(true), GetVehicleEngineSoundName(g_Ped4) == vehModel.VehicleDisplayName(false), setEngineSoundPressed, setEngineSoundPressed); 
				if (setEngineSoundPressed)
				{
					GTAvehicle veh12 = g_Ped4;
					veh12.RequestControl(200);
					SetVehicleEngineSoundName(veh12, vehModel.VehicleDisplayName(false));
				}
				break;
			}

			if (Menu::printingop == *Menu::currentopATM)
			{
				if (g_spawnVehicleDrawBMPs)
				{
					DrawVehicleBmp(vehModel);
				}
				
				DrawVehicleModelName(vehModel);

				bool bIsAFav = SpawnVehicleIsVehicleModelAFavourite(vehModel);
				if (Menu::bitController)
				{
					Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");

					if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
					{
						!bIsAFav ? SpawnVehicleAddVehicleModelToFavourites(vehModel, Game::InputBox("", 28U, "Enter custom name:", vehModel.VehicleDisplayName(true))) : SpawnVehicleRemoveVehicleModelFromFavourites(vehModel);
					}
				}
				else
				{
					Menu::add_IB(VirtualKey::B, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");

					if (IsKeyJustUp(VirtualKey::B))
					{
						!bIsAFav ? SpawnVehicleAddVehicleModelToFavourites(vehModel, Game::InputBox("", 28U, "Enter custom name:", vehModel.VehicleDisplayName(true))) : SpawnVehicleRemoveVehicleModelFromFavourites(vehModel);
					}
				}
			}
		}

	}
	void SpawnVehicleFavouritesMenu()
	{
		auto& searchStr = dict2;
		using namespace VehicleSpawner;

		GTAped myPed = g_Ped1;
		GTAvehicle myVehicle = myPed.CurrentVehicle();
		bool isInVehicle = myVehicle.Exists();

		AddTitle("Favourites");

		std::string xmlAddedVehicleModels = "AddedVehicleModels.xml";
		pugi::xml_document doc;
		if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str()).status != pugi::status_ok)
		{
			doc.reset();
			auto nodeDecleration = doc.append_child(pugi::node_declaration);
			nodeDecleration.append_attribute("version") = "1.0";
			nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
			auto nodeRoot = doc.append_child("AddedVehicleModels");
			doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str());
			return;
		}
		pugi::xml_node nodeRoot = doc.child("AddedVehicleModels");

		bool bInputAdd = false;
		AddOption("Add New Vehicle Model", bInputAdd); if (bInputAdd)
		{
			std::string inputStrModel = Game::InputBox("", 28U, "Enter model name (e.g. adder):");
			if (inputStrModel.length() > 0)
			{
				WAIT(500);
				Model ism = GET_HASH_KEY(inputStrModel);
				if (ism.IsInCdImage())
				{
					std::string inputStrName = Game::InputBox("", 28U, "Enter custom name:", ism.VehicleDisplayName(true));
					if (SpawnVehicleAddVehicleModelToFavourites(ism, inputStrName))
					{
						Game::Print::PrintBottomLeft("Model ~b~added~s~.");
					}
					else
					{
						Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to add model.");
					}
				}
			}
			else
			{
				Game::Print::PrintErrorInvalidModel(inputStrModel);
			}
		}

		if (isInVehicle)
		{
			const Model& myVehicleModel = myVehicle.Model();
			bool bIsCurrentModelAFav = SpawnVehicleIsVehicleModelAFavourite(myVehicleModel);
			bool bAddCurrentModelToFav = false, bRemoveCurrentModelFromFav = false;
			AddTickol("Current Vehicle's Model", bIsCurrentModelAFav, bAddCurrentModelToFav, bRemoveCurrentModelFromFav, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			if (bAddCurrentModelToFav)
			{
				std::string inputStrName = Game::InputBox("", 28U, "Enter custom name:", myVehicleModel.VehicleDisplayName(true));
				if (inputStrName.length() > 0)
				{
					if (SpawnVehicleAddVehicleModelToFavourites(myVehicleModel, inputStrName))
					{
						Game::Print::PrintBottomLeft("Model ~b~added~s~.");
					}
					else
					{
						Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to add model.");
					}
				}
				else 
				{
					Game::Print::PrintErrorInvalidInput(inputStrName);
				}
			}
			else if (bRemoveCurrentModelFromFav)
			{
				if (SpawnVehicleRemoveVehicleModelFromFavourites(myVehicleModel))
				{
					Game::Print::PrintBottomLeft("Model ~b~removed~s~.");
				}
				else 
				{
					Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to remove model.");
				}
			}
		}

		if (nodeRoot.first_child())
		{
			AddBreak("---Added Models---");

			bool bSearchPressed = false;
			AddOption(searchStr.empty() ? "SEARCH" : searchStr, bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
			{
				searchStr = Game::InputBox(searchStr, 126U, "SEARCH", boost::to_lower_copy(searchStr));
				boost::to_upper(searchStr);
			}

			for (auto nodeLocToLoad = nodeRoot.first_child(); nodeLocToLoad; nodeLocToLoad = nodeLocToLoad.next_sibling())
			{
				std::string vehModelName = nodeLocToLoad.attribute("modelName").as_string();
				Model vehModel = nodeLocToLoad.attribute("modelHash").as_uint(0);
				std::string vehCustomName = nodeLocToLoad.attribute("customName").as_string();

				if (!searchStr.empty()) 
				{ 
					if (boost::to_upper_copy(vehModelName).find(searchStr) == std::string::npos) 
					continue; 
				}

				if (vehModel.hash == 0)
				{
					vehModel = GET_HASH_KEY(vehModelName);
				}

				std::string vehDisplayName = vehCustomName.length() ? vehCustomName : vehModel.VehicleDisplayName(true);

				if (!vehModel.IsInCdImage())
				{
					AddOption(vehDisplayName + " (Invalid)", null);
				}
				else
				{
					bool setEngineSoundPressed = false;

					switch (Menu::currentArray[Menu::currentArrayIndex])
					{
					case SUB::SPOONER_SPAWN_VEHICLE:
						sub::Spooner::MenuOptions::AddOption_AddVehicle(vehDisplayName, vehModel);
						break;
					case SUB::SPAWNVEHICLE:
						AddVehicleSpawnOption(vehDisplayName, vehModel, g_Ped1);
						break;
					}
					/// one submenu back >>>
					switch (Menu::currentArray[Menu::currentArrayIndex - 1]) /// -1
					{
					case SUB::OBJECTGUN:
						AddObjectGunOption(vehDisplayName, vehModel, &null, false);
						break;
					case SUB::KABOOMGUN:
						AddKaboomGunOption(vehDisplayName, vehModel.hash, &null, false);
						break;
					case SUB::MSENGINESOUND:
						AddTickol(vehDisplayName, GetVehicleEngineSoundName(g_Ped4) == vehModel.VehicleDisplayName(false), setEngineSoundPressed, setEngineSoundPressed); if (setEngineSoundPressed)
						{
							GTAvehicle veh12 = g_Ped4;
							veh12.RequestControl(200);
							SetVehicleEngineSoundName(veh12, vehModel.VehicleDisplayName(false));
						}
						break;
					}
				}

				if (Menu::printingop == *Menu::currentopATM)
				{
					if (g_spawnVehicleDrawBMPs)
					{
						DrawVehicleBmp(vehModel);
					}

					DrawVehicleModelName(vehModel);

					if (Menu::bitController)
					{
						Menu::add_IB(INPUT_SCRIPT_RLEFT, "Remove");

						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
						{
							nodeLocToLoad.parent().remove_child(nodeLocToLoad);
							doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str());
							if (*Menu::currentopATM >= Menu::totalop)
							{
								Menu::Up();
							}
							return; // Yeah
						}
					}
					else
					{
						Menu::add_IB(VirtualKey::B, "Remove");

						if (IsKeyJustUp(VirtualKey::B))
						{
							nodeLocToLoad.parent().remove_child(nodeLocToLoad);
							doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlAddedVehicleModels).c_str());
							if (*Menu::currentopATM >= Menu::totalop)
							{
								Menu::Up();
							}
							return; // Yeah
						}
					}
				}
			}
		}
	}
	//keeping this code, but purely for future reference, access in the menu has been blocked
	const std::vector<std::string> CAPTIONS_CUNNINGSTUNTS{ "Nagasaki BF400", "Grotti Brioso R/A", "Vapid Desert Raid", "Emperor ETR1", "Western Gargoyle", "Obey Omnis", "Annis RE-7B", "Vapid Trophy Truck", "Lampadati Tropos Rallye", "Progen Tyrus", "Western Cliffhanger", "Ocelot Lynx", "Declasse Drift Tampa", "MTL Dune", "Vapid Contender" };

	const std::vector<std::string> CAPTIONS_BIKERSUPDATE{ "LCC Avarus", "Nagasaki Chimera", "Shitzu Defiler", "Pegassi Faggio Mod", "Pegassi Faggio Sport", "Shitzu Hakuchou Drag Bike", "Maibatsu Manchez", "Western Nightblade", "Western Rat Bike", "Nagasaki Street Blazer", "Western Wolfsbane", "Western Zombie Bobber", "Western Zombie Chopper", "Western Daemon (Custom)", "BF Raptor", "Pegassi Vortex", "LCC Sanctus", "Declasse Tornado Rat Rod", "Nagasaki Shotaro", "Pegassi Esskey", "Bravado Youga Classic" };

	const std::vector<std::string> CAPTIONS_IMPORTEXPORT{ "Brute Armored Boxville", "Nagasaki Blazer Aqua", "Principe Diabolus", "Principe Diabolus (Custom)", "Annis Elegy Retro (Custom)", "Ocelot Penetrator", "JoBuilt Phantom Wedge", "BF Ramp Buggy", "Coil Rocket Voltic", "Imponte Ruiner 2000", "Karin Technical Aqua", "Pegassi Tempesta", "MTL Wastelander", "Truffade Nero", "Truffade Nero Custom", "Pfister Comet Retro Custom", "Pegassi FCR 1000", "Pegassi FCR 1000 Custom", "Progen Itali GTB", "Progen Itali GTB Custom", "Dewbauchee Specter", "Dewbauchee Specter Custom" };

	const std::vector<std::string> CAPTIONS_CUNNINGSTUNTS2{ "Progen GP1", "Hijak Ruston", "Pegassi Infernus Classic", "Grotti Turismo Classic", "Imponte Duke O'Death" };

	const std::vector<std::string> CAPTIONS_GUNRUNNING{ "Vom Feuer Anti-Aircraft Trailer", "HVY APC Tank", "Nagasaki Caddy (Bunker)", "BF Dune FAV", "Bravado Half-track", "JoBuilt Hauler Custom", "Mobile Operations Center (Trailer)", "Pegassi Oppressor", "JoBuilt Phantom Custom", "Declasse Weaponized Tampa", "Dewbauchee Vagner", "Grotti Cheetah Classic", "Karin Technical Custom", "Pegassi Torero", "HVY Insurgent Pick-up Custom", "HVY Nightshark", "Ocelot XA-21", "Ocelot Ardent" };

	const std::vector<std::string> CAPTIONS_SMUGGLERSRUN{ "Buckingham Alpha-Z1", "Nagasaki Havok", "LF-22 Starling", "Vapid Retinue", "Western Company Rogue", "Mammoth Tula", "Nagasaki Ultralight", "V-65 Molotok", "Grotti Visione", "Dewbauchee Rapid GT Classic", "RM-10 Bombushka", "Buckingham Howard NX-25", "Mammoth Mogul", "Buckingham Pyro", "Grotti Vigilante", "Western Company Seabreeze", "P-45 Nokota", "Coil Cyclone", "FH-1 Hunter", "JoBuilt P-996 LAZER" };

	const std::vector<std::string> CAPTIONS_DOOMSDAYHEIST{ "Akula", "Mammoth Avenger", "HVY Barrage", "HVY Chernobog", "Pfister Comet Safari", "Imponte Deluxo", "Ocelot Pariah", "Coil Raiden", "RCV", "Vapid Riata", "Übermacht SC1", "Ocelot Stromberg", "Mammoth Thruster (Jetpack)", "TM-02 Khanjali Tank", "Volatol", "Declasse Yosemite", "Übermacht Sentinel Classic", "Benefactor Streiter", "Albany Hermes", "Overflod Autarch", "Annis Savestra", "ampadati Viseris", "Grotti GT500", "Übermacht Revolter", "Pfister Neon", "Canis Kamacho", "Vapid Hustler", "Karin 190z", "Pfister Comet SR" };

	const std::vector<std::string> CAPTIONS_SOUTHERNSASPORTSERIES{ "Overflod Entity XXR", "Vulcar Fagaloa", "Vapid GB200", "Declasse Hotring Sabre", "Cheval Taipan", "Vapid Ellie", "Pegassi Tezeract", "Vapid Caracara", "Vapid Flash GT", "Weeny Issi Classic", "Sea Sparrow", "Vapid Dominator GTX", "Overflod Tyrant", "RUNE Cheburek", "Lampadati Michelli GT" };

	const std::vector<std::string> CAPTIONS_AFTERHOURS{ "Festival Bus", "Dinka Jester Classic", "Maibatsu Mule Custom", "Mammoth Patriot Stretch", "MTL Pounder Custom", "Vapid Speedo Custom", "Ocelot Swinger", "Blimp", "Enus Stafford", "B-11 Strikeforce", "Benefactor Terrorbyte", "Pegassi Oppressor Mk II", "Declasse Scramjet", "HVY Menacer", "Canis Freecrawler" };

	const std::vector<std::string> CAPTIONS_ARENAWAR{ "Benefactor Bruiser (Arena)", "Declasse Brutus (Arena)", "MTL Cerberus (Arena)", "Western Deathbike (Arena)", "Vapid Dominator (Arena)", "Declasse Impaler", "Declasse Impaler (Arena)", "Vapid Imperator (Arena)", "Weeny Issi (Arena)", "Bravado Sasquatch (Arena)", "HVY Scarab (Arena)", "Vapid Slamvan (Arena)", "Annis ZR380 (Arena)", "Pegassi Toros", "Vapid Clique", "Grotti Itali GTO", "Declasse Tulip", "Benefactor Schlagen GT", "RC Bandito", "Schyster Deviant", "Declasse Vamos", "Principe Deveste Eight" };

	const std::vector<std::string> CAPTIONS_DIAMONDCASINO{ "Obey 8F Drafter", "Vapid Caracara 4x4", "Weeny Issi Sport", "Enus Paragon R", "Enus Paragon R (Armored)", "Annis S80RR", "Truffade Thrax", "Vysser Neo", "Bravado Gauntlet Classic", "Progen Emerus", "Vulcar Nebula Turbo", "Ocelot Locust", "Bravado Gauntlet Hellfire", "Benefactor Krieger", "Übermacht Zion Classic", "Annis Hellion", "Ocelot Jugular", "Weeny Dynasty", "Pegassi Zorrusso", "Western Rampant Rocket Tricycle", "Lampadati Novak", "Vapid Peyote Gasser" };

	const std::vector<std::string> CAPTIONS_LSSUMMERSPECIAL{ "Imponte Beater Dukes", "Benefactor BR8 (Formula 1 Car)", "BF Club", "Invetero Coquette D10", "Declasse DR1 (IndyCar)", "Bravado Gauntlet Classic Custom", "Benefactor Glendale Custom", "Dundreary Landstalker XL", "Albany Manana Custom", "Maibatsu Penumbra FF", "Vapid Peyote Custom", "Canis Seminole Frontier", "Lampadati Tigon", "Declasse Yosemite Rancher", "Bravado Youga Classic 4x4" };

	const std::vector<std::string> CAPTIONS_CAYOHEIST{ "Western Company Annihilator Stealth", "Kraken Submersibles Avisa", "RUNE Kosatka (Submarine HQ)", "Kurtz 31 Patrol Boat", "Shitzu Longfin", "RO-86 Alkonost", "Sparrow", "Pegassi Toreador", "Vapid Winky", "Dinka Veto Classic (Go-Kart)", "Dinka Veto Modern (Go-Kart)", "Grotti Itali RSX", "BF Weevil", "Grotti Brioso 300", "Maibatsu Manchez Scout", "Vapid Slamtruck", "Vetir", "Mammoth Squaddie", "Dinka Verus", "Nagasaki Weaponized Dinghy" };

	const std::vector<std::string> CAPTIONS_LSTUNERS{ "Karin Calico GTF", "Vapid Dominator GTT", "Annis Euros", "Karin Futo GTX", "Dinka Jester RR", "Annis Remus", "Dinka RT3000", "Obey Tailgater S", "Vulcar Warrener HKR", "Annis ZR350", "Pfister Comet S2", "Vapid Dominator ASP", "Emperor Vectre", "Pfister Growler", "Karin Sultan RS Classic", "Übermacht Cypher", "Karin Previon" };

	const std::vector<std::string> CAPTIONS_THECONTRACT{ "Pfister Astron", "Bravado Buffalo STX", "Dewbauchee Champion", "Lampadati Cinquemila", "Enus Deity", "Pegassi Ignus", "Enus Jubilee", "Vapid Youga Custom", "Overflod Zeno", "Gallivanter Baller ST", "Mammoth Patriot Mil-Spec", "Pfister Comet S2 Cabrio", "Nagasaki Shinobi", "Obey I-Wagen", "Declasse Granger 3600LX", "Western Reever" };

	const std::vector<std::string> CAPTIONS_CRIMINALENTERPRISES{ "Obey 10F", "Obey 10F Widebody", "Grotti Brioso 300 Widebody", "Buckingham Conada", "Lampadati Corsita", "Declasse Draugur", "Bravado Greenwood", "Dinka Kanjo SJ", "Benefactor LM87", "Obey Omnis e-GT (Imani Tech)", "Dinka Postlude", "Übermacht Rhinehart", "Imponte Ruiner ZZ-8", "Übermacht Sentinel Classic Widebody", "Benefactor SM722", "Pegassi Torero XO", "Declasse Vigero ZX", "BF Weevil Custom", "Benefactor SM722", "Declasse Draugur", "Imponte Ruiner ZZ-8", "Grotti Brioso 300 Widebody", "Declasse Vigero ZX", "Dinka Postlude", "Dinka Kanjo SJ", "Obey 10F", "Übermacht Rhinehart", "BF Weevil Custom", "Obey 10F Widebody", "Übermacht Sentinel Classic Widebody" };

	const std::vector<std::string> CAPTIONS_THECHOPSHOP{ "Vapid Aleutian", "Karin Asterope GZ", "Vapid Dominator GT", "Fathom FR36 (Drift Car)", "Declasse Impaler LX", "Brute Police Riot", "Vapid Police Stanier LE Cruiser", "Grotti Turismo Omaggio", "Vapid Unmarked Cruiser", "Declasse Vigero ZX Convertible (HSW Upgrade)", "Karin Vivanite", "Declasse Park Ranger", "Brute Boxville (LSDS)", "Bravado Dorado", "Albany Cavalcade XL", "Bravado Police Gauntlet Interceptor", "Declasse Impaler SZ", "Canis Terminus", "Gallivanter Baller ST-D", "Vapid Benson (Cluckin' Bell)" };

	const std::vector<std::string> CAPTIONS_BOTTOMDOLLAR{ "Impaler LX Cruiser", "Dominator FX Cruiser", "Castigator", "Pipistrello", "Übermacht Niobe", "Paragon S", "Envisage", "Euros X32", "Coquette D1", "Dorado Cruiser", "Burrito (Bail Enforcement)", "Impaler SZ Cruiser", "Greenwood Cruiser", "Yosemite 1500", "Dominator FX", "Vorschlaghammer", "Pizza Boy" };


	const std::vector<std::string> Captions_financeFelony = {
	"Benefactor XLS",              // XLS
	"Benefactor XLS (Armored)",    // XLS2
	"Dewbauchee Seven-70",         // Seven-70
	"Vapid FMJ",                   // FMJ
	"Pegassi Reaper",              // Reaper
	"Grotti Bestia GTS",           // Bestia GTS
	"Enus Windsor Drop",           // Windsor2
	"Buckingham Volatus",          // Volatus
	"Buckingham Nimbus",           // Nimbus
	"Nagasaki Tug",                // Tug
	"MTL Brickade",                // Brickade
	"Bravado Rumpo Custom",        // Rumpo3
	"Ocelot Lynx",                 // Lynx
	"Progen T20",                  // T20
	"Western Company Cargobob",    // Cargobob2
	"Western Company Cargobob3",   // Cargobob3
	"Pegassi Zentorno",            // Zentorno
	"Benefactor Schafter V12",     // Schafter3
	"Benefactor Schafter V12 (Armored)", // Schafter4
	};
	const std::vector<std::string> VALUES_CUNNINGSTUNTS{ "bf400", "brioso", "trophytruck2", "sheava", "gargoyle", "omnis", "le7b", "trophytruck", "tropos", "tyrus", "cliffhanger", "lynx", "tampa2", "rallytruck", "contender" };

	const std::vector<std::string> VALUES_BIKERSUPDATE{ "avarus", "chimera", "defiler", "faggio3", "faggio", "hakuchou2", "manchez", "nightblade", "ratbike", "blazer4", "wolfsbane", "zombiea", "zombieb", "daemon2", "raptor", "vortex", "sanctus", "tornado6", "shotaro", "esskey", "youga2" };

	const std::vector<std::string> VALUES_IMPORTEXPORT{ "boxville5", "blazer5", "diablous", "diablous2", "elegy", "penetrator", "phantom2", "dune5", "voltic2", "ruiner2", "technical2", "tempesta", "wastelander", "nero", "nero2", "comet3", "fcr", "fcr2", "italigtb", "italigtb2", "specter", "specter2" };

	const std::vector<std::string> VALUES_CUNNINGSTUNTS2{ "gp1", "ruston", "infernus2", "turismo2", "dukes2" };

	const std::vector<std::string> VALUES_GUNRUNNING{ "trailersmall2", "apc", "caddy3", "dune3", "halftrack", "hauler2", "trailerlarge", "oppressor", "phantom3", "tampa3", "vagner", "cheetah2", "technical3", "torero", "insurgent3", "nightshark", "xa21", "ardent" };

	const std::vector<std::string> VALUES_SMUGGLERSRUN{ "alphaz1", "havok", "starling", "retinue", "rogue", "tula", "microlight", "molotok", "visione", "rapidgt3", "bombushka", "howard", "mogul", "pyro", "vigilante", "seabreeze", "nokota", "cyclone", "hunter", "lazer" };

	const std::vector<std::string> VALUES_DOOMSDAYHEIST{ "akula", "avenger", "barrage", "chernobog", "comet4", "deluxo", "pariah", "raiden", "riot2", "riata", "sc1", "stromberg", "thruster", "khanjali", "volatol", "yosemite", "sentinel3", "streiter", "hermes", "autarch", "savestra", "viseris", "gt500", "revolter", "neon", "kamacho", "hustler", "z190", "comet5" };

	const std::vector<std::string> VALUES_SOUTHERNSASPORTSERIES{ "entity2", "fagaloa", "GB200", "hotring", "taipan", "ellie", "Tezeract", "caracara", "flashgt", "issi3", "seasparrow", "dominator3", "tyrant", "cheburek", "michelli" };

	const std::vector<std::string> VALUES_AFTERHOURS{ "pbus2", "jester3", "mule4", "mule4", "pounder2", "speedo4", "swinger", "blimp3", "stafford", "strikeforce", "terbyte", "oppressor2", "scramjet", "menacer", "freecrawler" };

	const std::vector<std::string> VALUES_ARENAWAR{ "bruiser", "brutus", "cerberus", "deathbike", "dominator4", "impaler", "impaler2", "imperator", "issi4", "monster3", "scarab", "slamvan4", "zr380", "toros", "clique", "italigto", "tulip", "schlagen", "rcbandito", "deviant", "vamos", "deveste" };

	const std::vector<std::string> VALUES_DIAMONDCASINO{ "drafter", "caracara2", "issi7", "paragon", "paragon2", "s80", "thrax", "neo", "gauntlet3", "emerus", "nebula", "locust", "gauntlet4", "krieger", "zion3", "hellion", "jugular", "dynasty", "zorrusso", "rrocket", "novak", "peyote2" };

	const std::vector<std::string> VALUES_LSSUMMERSPECIAL{ "dukes3", "openwheel1", "club", "coquette4", "openwheel2", "gauntlet5", "glendale2", "landstalker2", "manana2", "penumbra2", "peyote3", "seminole2", "tigon", "yosemite3", "youga3" };

	const std::vector<std::string> VALUES_CAYOHEIST{ "annihilator2", "avisa", "kosatka", "patrolboat", "longfin", "alkonost", "seasparrow3", "toreador", "winky", "veto", "veto2", "italirsx", "weevil", "brioso2", "manchez2", "slamtruck", "vetir", "squaddie", "verus", "dinghy5" };

	const std::vector<std::string> VALUES_LSTUNERS{ "calico", "dominator8", "drifteuros", "futo2", "driftjester", "driftremus", "rt3000", "tailgater2", "warrener2", "zr350", "comet6", "dominator7", "vectre", "growler", "sultan3", "cypher", "previon" };

	const std::vector<std::string> VALUES_THECONTRACT{ "astron", "buffalo4", "champion", "cinquemila", "Deity", "Ignus", "jubilee", "youga4", "zeno", "baller7", "patriot3", "comet7", "shinobi", "iwagen", "granger2", "reever" };

	const std::vector<std::string> VALUES_CRIMINALENTERPRISES{ "tenf", "tenf2", "brioso3", "conada", "corsita", "draugur", "greenwood", "kanjosj", "lm87", "omnisegt", "postlude", "rhinehart", "ruiner4", "sentinel4", "sm722", "torero2", "vigero2", "weevil2", "sm722", "draugur", "ruiner4", "brioso3", "vigero2", "postlude", "kanjosj", "tenf", "rhinehart", "weevil2", "tenf2", "sentinel4" };

	const std::vector<std::string> VALUES_THECHOPSHOP{ "aleutian", "asterope2", "dominator9", "fr36", "impaler6", "riot", "police", "turismo3", "police4", "vigero3", "vivanite", "pranger", "boxville6", "dorado", "cavalcade3", "polgauntlet", "impaler5", "terminus", "baller8", "benson2" };

	const std::vector<std::string> VALUES_BOTTOMDOLLAR{ "polimpaler6", "poldominator10", "castigator", "pipistrello", "niobe", "paragon3", "envisage", "eurosx32", "coquette5", "poldorado", "policet3", "polimpaler5", "polgreenwood", "yosemite1500", "dominator10", "vorschlaghammer", "pizzaboy" };
	const std::vector<std::string> Values_financeFelony = {
	"xls",          // XLS
	"xls2",         // XLS2 (Armored)
	"seven70",      // Seven-70
	"fmj",          // FMJ
	"reaper",       // Reaper
	"bestiaGTS",    // Bestia GTS
	"windsor2",     // Windsor Drop
	"volatus",      // Volatus
	"nimbus",       // Nimbus
	"tug",          // Tug
	"brickade",     // Brickade
	"rumpo3",       // Rumpo Custom
	"lynx",         // Lynx
	"t20",          // T20
	"cargobob2",    // Cargobob2
	"cargobob3",    // Cargobob3
	"zentorno",     // Zentorno
	"schafter3",    // Schafter V12
	"schafter4",    // Schafter V12 (Armored)
	};
	const std::vector<std::string> Captions_heistsDLC = {
	"Karin Kuruma",
	"Karin Kuruma (Armored)",
	"Armored Boxville",
	"Insurgent",
	"Insurgent Pickup",
	"Technical",
	"Casco",
	"Velum 5-Seater",
	"Hydra",
	"Savage",
	"Valkyrie",
	"Lectro",
	"Dinghy 4-Seater"
	};
	const std::vector<std::string> Values_heistsDLC = {
		"kuruma",
		"kuruma2",
		"boxville4",
		"insurgent",
		"insurgent2",
		"technical",
		"casco",
		"velum2",
		"hydra",
		"savage",
		"valkyrie",
		"lectro",
		"dinghy3"
	};

	const std::vector<std::string> CAPTIONS_AGENTS_OF_SABOTAGE = {
		"Bravado Banshee GTS",
		"Dinka Jester RR Widebody",
		"Invetero Coquette D5",
		"Futo",
		"Vapid Firebolt ASP",
		"Dinka Chavos V6",
		"Vapid Uranus",
		"Brute Taco Van",
		"Buckingham DH7 Iron Mule",
		"Canis Terminus Patrol",
		"Invetero Coquette D10 Pursuit",
		"Cheburek",
		"Vapid Caracara Pursuit",
		"Eberhard Titan 250D",
		"Jester Classic"
	};

	const std::vector<std::string> VALUES_AGENTS_OF_SABOTAGE = {
	"banshee3",
	"jester4",
	"coquette5",
	"driftfuto2",
	"firebolt",
	"chavosv6",
	"uranus",
	"taco",
	"cargobob5",
	"polterminus",//ok
	"polcoquette4",
	"driftcheburek",
	"polcaracara",
	"titan2",
	"driftjester3"
	};

	const std::vector<std::string> CAPTIONS_BEACH_BUM = {//done
	"BF Bifta",
	"Canis Kalahari",
	"Bravado Paradise",
	"Dinka Speeder"
	};
	const std::vector<std::string> VALUES_BEACH_BUM = {//done
		"bifta",
		"kalahari",
		"paradise",
		"speeder"
	};

	const std::vector<std::string> CAPTIONS_LOW_RIDERS = {//done
	"Declasse Buccaneer",
	"Declasse Primo",
	"Imponte Chino",
	"Willard Voodoo",
	"Declasse Moonbeam",
	"Declasse Faction"
	};
	const std::vector<std::string> VALUES_LOW_RIDERS = {//done
		"buccaneer",
		"primo",
		"chino",
		"voodoo",
		"moonbeam",
		"faction"
	};
	const std::vector<std::string> CAPTIONS_HIGH_LIFE_ILL_GOT_ONE = {//dome
	"Enus Huntley S",
	"Dewbauchee Massacro",
	"Pegassi Zentorno",
	"Pegassi Osiris",
	"Benefactor Stirling GT",
	"Progen T20"
	};
	const std::vector<std::string> VALUES_HIGH_LIFE_ILL_GOT_ONE = {//done
		"huntley",
		"massacro",
		"zentorno",
		"osiris",
		"stirling",
		"t20"
	};

	const std::vector<std::string> CAPTIONS_ILL_GOT_TWO = {//done
	"Pegassi Osiris",
	"Enus Windsor Drop",
	"Benefactor Schafter LWB",
	"Grotti Turismo R",
	"Pegassi ZType",
	"Coil Brawler",
	"Progen T20",
	"Invetero Coquette BlackFin",
	"Vapid Chino",
	"Dinka Vindicator",
	"Lampadati Toro"
	};
	const std::vector<std::string> VALUES_ILL_GOT_TWO = {//done
		"osiris",
		"windsor2",
		"schafter3",
		"turismor",
		"ztype",
		"brawler",
		"t20",
		"coquette3",
		"chino",
		"vindicator",
		"toro"
	};

	const std::vector<std::string> CAPTIONS_VALENTINES = {//done
	"Albany Roosevelt"
	};
	const std::vector<std::string> VALUES_VALENTINES = {//done
		"btype"
	};

	const std::vector<std::string> CAPTIONS_HIGH_LIFE = {//done
	"Pegassi Zentorno",
	"Dewbauchee Massacro",
	"Enus Huntley S",
	"Dinka Thrust"
	};
	const std::vector<std::string> VALUES_HIGH_LIFE = {//done
		"zentorno",
		"massacro",
		"huntley",
		"thrust"
	};

	const std::vector<std::string> CAPTIONS_NOT_HIPSTER = {//done
	"Benefactor Glendale",
	"Benefactor Panto",
	"Bravado Rhapsody",
	"Vapid Blade",
	"Vapid Slamvan",
	"Vulcar Warrener",
	"Lampadati Pigalle"
	};
	const std::vector<std::string> VALUES_NOT_HIPSTER = {//done
		"glendale",
		"panto",
		"rhapsody",
		"blade",
		"slamvan",
		"warrener",
		"pigalle"
	};

	const std::vector<std::string> CAPTIONS_INDEPENDENCE_DAY = {//dine
	"Western Sovereign",
	"Vapid Liberator"
	};
	const std::vector<std::string> VALUES_INDEPENDENCE_DAY = {//done
		"sovereign",
		"monster"
	};

	const std::vector<std::string> CAPTIONS_FLIGHT_SCHOOL = {//done
	"Western Besra",
	"Buckingham Miljet",
	"Invetero Coquette Classic",
	"Swift"
	};
	const std::vector<std::string> VALUES_FLIGHT_SCHOOL = {//done
		"besra",
		"miljet",
		"coquette2",
		"swift"
	};

	const std::vector<std::string> CAPTIONS_LAST_TEAM_STANDING = {//done
	"Lampadati Furore GT",
	"Shitzu Hakuchou",
	"LCC Innovation"
	};
	const std::vector<std::string> VALUES_LAST_TEAM_STANDING = {//done
		"furoregt",
		"hakuchou",
		"innovation"
	};

	const std::vector<std::string> CAPTIONS_FESTIVE_2014 = {//done
	"Bravado Rat-Truck",
	"Vapid Slamvan Custom",
	"Dinka Jester Racecar",
	"Dewbauchee Massacro Racecar"
	};
	const std::vector<std::string> VALUES_FESTIVE_2014 = {//done
		"ratloader2",
		"slamvan2",
		"jester2",
		"massacro2"
	};

	const std::vector<std::string> CAPTIONS_EXECUTIVES_CRIMINALS = {//done
	"Bravado Banshee 900R",
	"Albany Roosevelt Valor",
	"Benefactor Schafter LWB",
	"Benefactor Schafter V12",
	"Benefactor Schafter V12 (Armored)",
	"Enus Cognoscenti",
	"Enus Cognoscenti (Armored)",
	"Enus Cognoscenti 55",
	"Enus Cognoscenti 55 (Armored)",
	"Declasse Mamba",
	"Imponte Nightshade",
	"Bravado Verlierer"
	};
	const std::vector<std::string> VALUES_EXECUTIVES_CRIMINALS = {//done
		"banshee2",
		"btype3",
		"schafter5",
		"schafter3",
		"schafter4",
		"cognoscenti",
		"cog552",
		"cog55",
		"cog552",
		"mamba",
		"nightshade",
		"verlierer2"
	};

	struct VehicleDlcCategory {
		std::string name;
		std::vector<std::string> captions;
		std::vector<std::string> values;
	};

	const std::vector<VehicleDlcCategory> VehicleDlcCategories = {
		{ "Beach Bum", CAPTIONS_BEACH_BUM, VALUES_BEACH_BUM },
		{ "Executives and Other Criminals", CAPTIONS_EXECUTIVES_CRIMINALS, VALUES_EXECUTIVES_CRIMINALS},
		{ "Festive Surprise 2014", CAPTIONS_FESTIVE_2014, VALUES_FESTIVE_2014},
		{ "Last Team Standing", CAPTIONS_LAST_TEAM_STANDING, VALUES_LAST_TEAM_STANDING},
		{ "SA Flight School", CAPTIONS_FLIGHT_SCHOOL, VALUES_FLIGHT_SCHOOL},
		{ "Independence Day", CAPTIONS_INDEPENDENCE_DAY, VALUES_INDEPENDENCE_DAY},
		{ "I'm Not a Hipster", CAPTIONS_NOT_HIPSTER, VALUES_NOT_HIPSTER},
		{ "High Life", CAPTIONS_HIGH_LIFE, VALUES_HIGH_LIFE},
		{ "Valentine's Day Massacre", CAPTIONS_VALENTINES, VALUES_VALENTINES},
		{ "Ill-Gotten Gains Part II", CAPTIONS_ILL_GOT_TWO, VALUES_ILL_GOT_TWO},
		{ "Ill-Gotten Gains Part I", CAPTIONS_HIGH_LIFE_ILL_GOT_ONE, VALUES_HIGH_LIFE_ILL_GOT_ONE},
		{ "Lowriders", CAPTIONS_LOW_RIDERS, VALUES_LOW_RIDERS},
		{ "Finance & Felony", Captions_financeFelony, Values_financeFelony },
		{ "Heists", Captions_heistsDLC, Values_heistsDLC },
		{ "Cunning Stunts", CAPTIONS_CUNNINGSTUNTS, VALUES_CUNNINGSTUNTS },
		{ "Bikers", CAPTIONS_BIKERSUPDATE, VALUES_BIKERSUPDATE },
		{ "Import/Export", CAPTIONS_IMPORTEXPORT, VALUES_IMPORTEXPORT },
		{ "Cunning Stunts 2", CAPTIONS_CUNNINGSTUNTS2, VALUES_CUNNINGSTUNTS2 },
		{ "Smugglers Run", CAPTIONS_SMUGGLERSRUN, VALUES_SMUGGLERSRUN },
		{ "Doomsday Heist", CAPTIONS_DOOMSDAYHEIST, VALUES_DOOMSDAYHEIST },
		{ "Southern SA Series", CAPTIONS_SOUTHERNSASPORTSERIES, VALUES_SOUTHERNSASPORTSERIES },
		{ "After Hours", CAPTIONS_AFTERHOURS, VALUES_AFTERHOURS },
		{ "Arena War", CAPTIONS_ARENAWAR, VALUES_ARENAWAR },
		{ "Diamond Casino", CAPTIONS_DIAMONDCASINO, VALUES_DIAMONDCASINO },
		{ "LS Summer Special", CAPTIONS_LSSUMMERSPECIAL, VALUES_LSSUMMERSPECIAL },
		{ "Cayo Heist", CAPTIONS_CAYOHEIST, VALUES_CAYOHEIST },
		{ "LS Tuners", CAPTIONS_LSTUNERS, VALUES_LSTUNERS },
		{ "The Contract", CAPTIONS_THECONTRACT, VALUES_THECONTRACT },
		{ "Criminal Enterprises", CAPTIONS_CRIMINALENTERPRISES, VALUES_CRIMINALENTERPRISES },
		{ "The Chop Shop", CAPTIONS_THECHOPSHOP, VALUES_THECHOPSHOP },
		{ "Bottom Dollar", CAPTIONS_BOTTOMDOLLAR, VALUES_BOTTOMDOLLAR },
		{ "Agents Of Sabotage", CAPTIONS_AGENTS_OF_SABOTAGE, VALUES_AGENTS_OF_SABOTAGE },
	};

	int vehDLCCategoryID = 0;
	int vehDlcIdToSpawn = 0;
	void SpawnVehicleDLC()
	{
		bool spawnVehicle = false;

		if (vehDLCCategoryID < 0 || vehDLCCategoryID >= VehicleDlcCategories.size())
		{
			Game::Print::PrintBottomLeft("Invalid Category");
			return;
		}

		const auto& selectedCategory = VehicleDlcCategories[vehDLCCategoryID];

		AddTitle(selectedCategory.name);

		for (size_t i = 0; i < selectedCategory.captions.size(); ++i)
		{
			AddOption(selectedCategory.captions[i], spawnVehicle);

			if (Menu::printingop == *Menu::currentopATM)
			{
				vehDlcIdToSpawn = static_cast<int>(i);
			}

			if (Menu::printingop == *Menu::currentopATM)
			{

				if (g_spawnVehicleDrawBMPs)
				{
					VehicleSpawner::DrawVehicleBmp(selectedCategory.values[vehDlcIdToSpawn]);
				}

				const bool bIsAFav = SpawnVehicleIsVehicleModelAFavourite(selectedCategory.values[vehDlcIdToSpawn]);

				if (Menu::bitController)
				{
					Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsAFav ? "Add to" : "Remove from") + std::string(" favourites"));

					if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
					{
						if (!bIsAFav)
						{
							SpawnVehicleAddVehicleModelToFavourites(
								selectedCategory.values[vehDlcIdToSpawn],
								Game::InputBox("", 28U, "Enter custom name:",
									GTAmodel::Model(selectedCategory.values[vehDlcIdToSpawn]).VehicleDisplayName(true)));
						}
						else
						{
							SpawnVehicleRemoveVehicleModelFromFavourites(selectedCategory.values[vehDlcIdToSpawn]);
						}
					}
				}
				else
				{
					Menu::add_IB(VirtualKey::B, (!bIsAFav ? "Add to" : "Remove from") + std::string(" favourites"));

					if (IsKeyJustUp(VirtualKey::B))
					{
						if (!bIsAFav)
						{
							SpawnVehicleAddVehicleModelToFavourites(
								selectedCategory.values[vehDlcIdToSpawn],
								Game::InputBox("", 28U, "Enter custom name:",
									GTAmodel::Model(selectedCategory.values[vehDlcIdToSpawn]).VehicleDisplayName(true)));
						}
						else
						{
							SpawnVehicleRemoveVehicleModelFromFavourites(selectedCategory.values[vehDlcIdToSpawn]);
						}
					}
				}
			}
		}

		if (spawnVehicle)
		{
			SpawnVehicle(selectedCategory.values[vehDlcIdToSpawn], g_Ped1, g_spawnVehicleDeleteOld, g_spawnVehicleAutoSit);
		}
	}

	void SpawnVehicleDLCSelection()
	{
		AddTitle("DLC Vehicles");
		for (size_t i = 0; i < VehicleDlcCategories.size(); ++i)
		{
			AddOption(VehicleDlcCategories[i].name, null, nullFunc, SUB::SPAWNVEHICLEDLC);

			if (Menu::printingop == *Menu::currentopATM)
			{
				vehDLCCategoryID = static_cast<int>(i);
			}
		}
	}
	// Vehicle saver

	namespace VehicleSaver
	{
		UINT8 _persistentAttachmentsTexterIndex = 0;
		UINT8 _driverVisibilityTexterIndex = 0;

		void VehicleSaveToFile(std::string filePath, GTAvehicle ev)
		{
			if (!ev.IsVehicle())
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Invalid vehicle.");
				return;
			}

			GTAentity myPed = PLAYER_PED_ID();
			GTAentity driverPed = ev.GetPedOnSeat(VehicleSeat::SEAT_DRIVER);
			bool driverPedExists = driverPed.Exists();

			bool bAddAttachmentsToSpoonerDb = false;
			bool bStartTaskSeqsOnLoad = true;

			pugi::xml_document oldXml;
			if (oldXml.load_file((const char*)filePath.c_str()).status == pugi::status_ok)
			{
				auto nodeOldRoot = oldXml.child("Vehicle");
				bAddAttachmentsToSpoonerDb = nodeOldRoot.child("SpoonerAttachments").attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase").as_bool(bAddAttachmentsToSpoonerDb);
				bStartTaskSeqsOnLoad = nodeOldRoot.child("SpoonerAttachments").attribute("StartTaskSequencesOnLoad").as_bool(bStartTaskSeqsOnLoad);
			}

			pugi::xml_document doc;

			auto nodeDecleration = doc.append_child(pugi::node_declaration);
			nodeDecleration.append_attribute("version") = "1.0";
			nodeDecleration.append_attribute("encoding") = "ISO-8859-1";

			auto nodeVehicle = doc.append_child("Vehicle"); // Root
			nodeVehicle.append_attribute("menyoo_ver") = MENYOO_CURRENT_VER_;

			const Model& eModel = ev.Model();
			nodeVehicle.append_child("ModelHash").text() = IntToHexString(eModel.hash, true).c_str();
			nodeVehicle.append_child("InitialHandle").text() = ev.Handle();

			auto nodeVehicleStuff = nodeVehicle.append_child("VehicleProperties");

			// Colours
			auto nodeVehicleColours = nodeVehicleStuff.append_child("Colours");
			int mod1a, mod1b, mod1c;
			GET_VEHICLE_MOD_COLOR_1(ev.Handle(), &mod1a, &mod1b, &mod1c);
			int mod2a, mod2b;
			GET_VEHICLE_MOD_COLOR_2(ev.Handle(), &mod2a, &mod2b);
			bool isPrimaryColourCustom = ev.IsPrimaryColorCustom();
			bool isSecondaryColourCustom = ev.IsSecondaryColorCustom();
			RgbS cust1 = ev.GetCustomPrimaryColour();
			RgbS cust2 = ev.GetCustomSecondaryColour();
			RgbS tyreSmokeRgb = ev.GetTyreSmokeColour();
			nodeVehicleColours.append_child("Primary").text() = ev.GetPrimaryColour();
			nodeVehicleColours.append_child("Secondary").text() = ev.GetSecondaryColour();
			nodeVehicleColours.append_child("Pearl").text() = ev.GetPearlescentColour();
			nodeVehicleColours.append_child("Rim").text() = ev.GetRimColour();
			nodeVehicleColours.append_child("Mod1_a").text() = mod1a;
			nodeVehicleColours.append_child("Mod1_b").text() = mod1b;
			nodeVehicleColours.append_child("Mod1_c").text() = mod1c;
			nodeVehicleColours.append_child("Mod2_a").text() = mod2a;
			nodeVehicleColours.append_child("Mod2_b").text() = mod2b;
			nodeVehicleColours.append_child("IsPrimaryColourCustom").text() = isPrimaryColourCustom;
			if (isPrimaryColourCustom)
			{
				nodeVehicleColours.append_child("Cust1_R").text() = cust1.R;
				nodeVehicleColours.append_child("Cust1_G").text() = cust1.G;
				nodeVehicleColours.append_child("Cust1_B").text() = cust1.B;
			}
			nodeVehicleColours.append_child("IsSecondaryColourCustom").text() = isSecondaryColourCustom;
			if (isSecondaryColourCustom)
			{
				nodeVehicleColours.append_child("Cust2_R").text() = cust2.R;
				nodeVehicleColours.append_child("Cust2_G").text() = cust2.G;
				nodeVehicleColours.append_child("Cust2_B").text() = cust2.B;
			}
			nodeVehicleColours.append_child("tyreSmoke_R").text() = tyreSmokeRgb.R;
			nodeVehicleColours.append_child("tyreSmoke_G").text() = tyreSmokeRgb.G;
			nodeVehicleColours.append_child("tyreSmoke_B").text() = tyreSmokeRgb.B;
			nodeVehicleColours.append_child("LrInterior").text() = ev.GetInteriorColour();
			nodeVehicleColours.append_child("LrDashboard").text() = ev.GetDashboardColour();
			nodeVehicleColours.append_child("LrXenonHeadlights").text() = ev.GetHeadlightColour();

			// Other stuff
			nodeVehicleStuff.append_child("Livery").text() = ev.GetLivery(); // Livery should be applied before paint is applied
			nodeVehicleStuff.append_child("NumberPlateText").text() = ev.GetNumberPlateText().c_str();
			nodeVehicleStuff.append_child("NumberPlateIndex").text() = ev.GetNumberPlateTextIndex();
			nodeVehicleStuff.append_child("WheelType").text() = ev.GetWheelType();
			nodeVehicleStuff.append_child("WheelsInvisible").text() = AreVehicleWheelsInvisible(ev);
			nodeVehicleStuff.append_child("EngineSoundName").text() = GetVehicleEngineSoundName(ev).c_str();
			nodeVehicleStuff.append_child("WindowTint").text() = ev.GetWindowTint();
			nodeVehicleStuff.append_child("BulletProofTyres").text() = !ev.GetCanTyresBurst();
			nodeVehicleStuff.append_child("DirtLevel").text() = ev.GetDirtLevel();
			nodeVehicleStuff.append_child("PaintFade").text() = ev.GetPaintFade();
			nodeVehicleStuff.append_child("RoofState").text() = (int)ev.GetRoofState();
			nodeVehicleStuff.append_child("SirenActive").text() = ev.GetSirenActive();
			nodeVehicleStuff.append_child("EngineOn").text() = ev.GetEngineRunning();
			nodeVehicleStuff.append_child("EngineHealth").text() = ev.GetEngineHealth();
			nodeVehicleStuff.append_child("LightsOn").text() = ev.GetLightsOn();
			nodeVehicleStuff.append_child("IsRadioLoud").text() = CAN_VEHICLE_RECEIVE_CB_RADIO(ev.Handle());// != 0;
			nodeVehicleStuff.append_child("LockStatus").text() = (int)ev.GetLockStatus();

			// Neons
			auto nodeVehicleNeons = nodeVehicleStuff.append_child("Neons");
			RgbS neonLightsRgb = ev.GetNeonLightsColour();
			nodeVehicleNeons.append_child("Left").text() = ev.IsNeonLightOn(VehicleNeonLight::Left);
			nodeVehicleNeons.append_child("Right").text() = ev.IsNeonLightOn(VehicleNeonLight::Right);
			nodeVehicleNeons.append_child("Front").text() = ev.IsNeonLightOn(VehicleNeonLight::Front);
			nodeVehicleNeons.append_child("Back").text() = ev.IsNeonLightOn(VehicleNeonLight::Back);
			nodeVehicleNeons.append_child("R").text() = neonLightsRgb.R;
			nodeVehicleNeons.append_child("G").text() = neonLightsRgb.G;
			nodeVehicleNeons.append_child("B").text() = neonLightsRgb.B;

			// Extras (modExtras)
			auto nodeVehicleModExtras = nodeVehicleStuff.append_child("ModExtras");
			for (UINT8 i = 0; i < 60; i++)
			{
				if (ev.DoesExtraExist(i)) nodeVehicleModExtras.append_child(("_" + std::to_string(i)).c_str()).text() = ev.GetExtraOn(i);
			}

			// Mods (customisations)
			auto nodeVehicleMods = nodeVehicleStuff.append_child("Mods");
			for (UINT i = 0; i < vValues_ModSlotNames.size(); i++)
			{
				bool isToggleable = (i >= 17 && i <= 22);
				if (isToggleable) 
				{
					nodeVehicleMods.append_child(("_" + std::to_string(i)).c_str()).text() = ev.IsToggleModOn(i);
				}
				else
				{
					nodeVehicleMods.append_child(("_" + std::to_string(i)).c_str()).text() = (std::to_string(ev.GetMod(i)) + "," + std::to_string(ev.GetModVariation(i))).c_str();
				}
			}

			// Doors
			auto nodeVehicleDoorsOpen = nodeVehicleStuff.append_child("DoorsOpen");
			nodeVehicleDoorsOpen.append_child("BackLeftDoor").text() = ev.IsDoorOpen(VehicleDoor::BackLeftDoor);
			nodeVehicleDoorsOpen.append_child("BackRightDoor").text() = ev.IsDoorOpen(VehicleDoor::BackRightDoor);
			nodeVehicleDoorsOpen.append_child("FrontLeftDoor").text() = ev.IsDoorOpen(VehicleDoor::FrontLeftDoor);
			nodeVehicleDoorsOpen.append_child("FrontRightDoor").text() = ev.IsDoorOpen(VehicleDoor::FrontRightDoor);
			nodeVehicleDoorsOpen.append_child("Hood").text() = ev.IsDoorOpen(VehicleDoor::Hood);
			nodeVehicleDoorsOpen.append_child("Trunk").text() = ev.IsDoorOpen(VehicleDoor::Trunk);
			nodeVehicleDoorsOpen.append_child("Trunk2").text() = ev.IsDoorOpen(VehicleDoor::Trunk2);
			auto nodeVehicleDoorsBroken = nodeVehicleStuff.append_child("DoorsBroken");
			nodeVehicleDoorsBroken.append_child("BackLeftDoor").text() = ev.IsDoorBroken(VehicleDoor::BackLeftDoor);
			nodeVehicleDoorsBroken.append_child("BackRightDoor").text() = ev.IsDoorBroken(VehicleDoor::BackRightDoor);
			nodeVehicleDoorsBroken.append_child("FrontLeftDoor").text() = ev.IsDoorBroken(VehicleDoor::FrontLeftDoor);
			nodeVehicleDoorsBroken.append_child("FrontRightDoor").text() = ev.IsDoorBroken(VehicleDoor::FrontRightDoor);
			nodeVehicleDoorsBroken.append_child("Hood").text() = ev.IsDoorBroken(VehicleDoor::Hood);
			nodeVehicleDoorsBroken.append_child("Trunk").text() = ev.IsDoorBroken(VehicleDoor::Trunk);
			nodeVehicleDoorsBroken.append_child("Trunk2").text() = ev.IsDoorBroken(VehicleDoor::Trunk2);

			// Tyres Bursted
			auto nodeVehicleTyresBursted = nodeVehicleStuff.append_child("TyresBursted");
			nodeVehicleTyresBursted.append_child("FrontLeft").text() = ev.IsTyreBursted(0);
			nodeVehicleTyresBursted.append_child("FrontRight").text() = ev.IsTyreBursted(1);
			nodeVehicleTyresBursted.append_child("_2").text() = ev.IsTyreBursted(2);
			nodeVehicleTyresBursted.append_child("_3").text() = ev.IsTyreBursted(3);
			nodeVehicleTyresBursted.append_child("BackLeft").text() = ev.IsTyreBursted(4);
			nodeVehicleTyresBursted.append_child("BackRight").text() = ev.IsTyreBursted(5);
			nodeVehicleTyresBursted.append_child("_6").text() = ev.IsTyreBursted(6);
			nodeVehicleTyresBursted.append_child("_7").text() = ev.IsTyreBursted(7);
			nodeVehicleTyresBursted.append_child("_8").text() = ev.IsTyreBursted(8);

			// Multipliers
			if (g_multListRPM.count(ev.Handle())) 
			{
				nodeVehicleStuff.append_child("RpmMultiplier").text() = g_multListRPM[ev.Handle()];
			}
			if (g_multListTorque.count(ev.Handle())) 
			{
				nodeVehicleStuff.append_child("TorqueMultiplier").text() = g_multListTorque[ev.Handle()];
			}
			if (g_multListMaxSpeed.count(ev.Handle())) 
			{
				nodeVehicleStuff.append_child("MaxSpeed").text() = g_multListMaxSpeed[ev.Handle()];
			}
			if (g_multListHeadLights.count(ev.Handle())) 
			{
				nodeVehicleStuff.append_child("HeadlightIntensity").text() = g_multListHeadLights[ev.Handle()];
			}

			nodeVehicle.append_child("OpacityLevel").text() = ev.GetAlpha();
			nodeVehicle.append_child("LodDistance").text() = ev.GetLODDistance();
			nodeVehicle.append_child("IsVisible").text() = ev.IsVisible();
			if (driverPedExists)
			{
				nodeVehicle.append_child("IsDriverVisible").text() = driverPed.IsVisible();
			}
			nodeVehicle.append_child("MaxHealth").text() = ev.GetMaxHealth();
			nodeVehicle.append_child("Health").text() = ev.GetHealth();

			nodeVehicle.append_child("HasGravity").text() = ev.GetHasGravity();
			nodeVehicle.append_child("IsOnFire").text() = ev.IsOnFire();
			nodeVehicle.append_child("IsInvincible").text() = ev.IsInvincible();
			nodeVehicle.append_child("IsBulletProof").text() = ev.IsBulletProof();
			//nodeVehicle.append_child("IsCollisionProof").text() = false;
			nodeVehicle.append_child("IsExplosionProof").text() = ev.IsExplosionProof();
			nodeVehicle.append_child("IsFireProof").text() = ev.IsFireProof();
			nodeVehicle.append_child("IsMeleeProof").text() = ev.IsMeleeProof();
			nodeVehicle.append_child("IsOnlyDamagedByPlayer").text() = ev.IsOnlyDamagedByPlayer();

			// Attachments
			auto nodeAttachments = nodeVehicle.append_child("SpoonerAttachments");
			nodeAttachments.append_attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase") = bAddAttachmentsToSpoonerDb;
			nodeAttachments.append_attribute("StartTaskSequencesOnLoad") = bStartTaskSeqsOnLoad;
			std::vector<GTAentity>vSpoonerAttachmentsSaved;
			vSpoonerAttachmentsSaved.push_back(ev);
			for (int bei = 0; bei < vSpoonerAttachmentsSaved.size(); bei++)
			{
				for (auto& e : sub::Spooner::Databases::EntityDb)
				{
					if (e.attachmentArgs.isAttached)
					{
						GTAentity att;
						if (sub::Spooner::EntityManagement::GetEntityThisEntityIsAttachedTo(e.handle, att))
						{
							if (att.Handle() == vSpoonerAttachmentsSaved[bei].Handle()) // baseEntToCheck is vSpoonerAttachmentsSaved[bei]
							{
								auto nodeAttachment = nodeAttachments.append_child("Attachment");
								sub::Spooner::FileManagement::AddEntityToXmlNode(e, nodeAttachment);
								std::string attachedToHandleStr = nodeAttachment.child("Attachment").child("AttachedTo").text().as_string();
								if (attachedToHandleStr == "PLAYER") 
								{
									nodeAttachment.child("Attachment").child("AttachedTo").text() = myPed.Handle();
								}
								else if (attachedToHandleStr == "VEHICLE") 
								{
									nodeAttachment.child("Attachment").child("AttachedTo").text() = g_myVeh;
								}
								vSpoonerAttachmentsSaved.push_back(e.handle);
							}
						}
					}
				}
			}

			if (doc.save_file((const char*)filePath.c_str()))
			{
				Game::Print::PrintBottomLeft("File ~b~saved~s~.");
				addlog(ige::LogType::LOG_INFO,  "Vehicle saved - " + eModel.VehicleDisplayName(false) + " in " + filePath);
			}
			else
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to save file.");
				addlog(ige::LogType::LOG_ERROR,  "Unable to save vehicle.");
			}
		}

		void VehicleReadFromFile(std::string filePath, GTAentity ped)
		{
			pugi::xml_document doc;
			if (doc.load_file((const char*)filePath.c_str()).status != pugi::status_ok)
			{
				addlog(ige::LogType::LOG_ERROR,  "Unable to load vehicle file " + filePath);
				Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to load file.");
			}

			GTAentity myPed = PLAYER_PED_ID();
			GTAentity myVehicle = g_myVeh;

			auto nodeVehicle = doc.child("Vehicle"); // Root

			Model eModel = nodeVehicle.child("ModelHash").text().as_uint();
			if (!eModel.IsInCdImage())
			{
				Game::Print::PrintErrorInvalidModel(std::to_string(eModel.hash));
				return;
			}
			eModel.Load();

			GTAvehicle ev = SpawnVehicle(eModel, ped.Handle(), g_spawnVehicleDeleteOld, g_spawnVehicleAutoSit); // spawn vehicle to commence customisation
			if (ped.Handle() == myPed.Handle())
			{
				g_myVeh = ev.Handle();
				g_myVehModel = eModel;
				myVehicle = g_myVeh;
			}
			SET_VEHICLE_MOD_KIT(ev.Handle(), 0);
			SetVehicleMaxUpgrades(ev.Handle(), false, g_spawnVehicleInvincible, g_spawnVehiclePlateType);

			WAIT(100);

			auto nodeVehicleInitialHandle = nodeVehicle.child("InitialHandle");
			ScrHandle evInitHandle = nodeVehicleInitialHandle.text().as_int();

			ev.SetHasGravity(nodeVehicle.child("HasGravity").text().as_bool(true));

			auto nodeVehicleStuff = nodeVehicle.child("VehicleProperties");

			ev.SetLivery(nodeVehicleStuff.child("Livery").text().as_int()); // Livery should be applied before paint is applied
																			 // Colours
			auto nodeVehicleColours = nodeVehicleStuff.child("Colours");
			int mod1a = nodeVehicleColours.child("Mod1_a").text().as_int();
			int mod1b = nodeVehicleColours.child("Mod1_b").text().as_int();
			int mod1c = nodeVehicleColours.child("Mod1_c").text().as_int();
			SET_VEHICLE_MOD_COLOR_1(ev.Handle(), mod1a, mod1b, mod1c);
			int mod2a = nodeVehicleColours.child("Mod2_a").text().as_int();
			int mod2b = nodeVehicleColours.child("Mod2_b").text().as_int();
			SET_VEHICLE_MOD_COLOR_2(ev.Handle(), mod2a, mod2b);
			ev.SetPrimaryColour(nodeVehicleColours.child("Primary").text().as_int());
			ev.SetSecondaryColour(nodeVehicleColours.child("Secondary").text().as_int());
			ev.SetPearlescentColour(nodeVehicleColours.child("Pearl").text().as_int());
			ev.SetRimColour(nodeVehicleColours.child("Rim").text().as_int());
			bool isPrimaryColourCustom = nodeVehicleColours.child("IsPrimaryColourCustom").text().as_bool();
			bool isSecondaryColourCustom = nodeVehicleColours.child("IsSecondaryColourCustom").text().as_bool();
			if (isPrimaryColourCustom)
			{
				RgbS cust1;
				cust1.R = nodeVehicleColours.child("Cust1_R").text().as_int();
				cust1.G = nodeVehicleColours.child("Cust1_G").text().as_int();
				cust1.B = nodeVehicleColours.child("Cust1_B").text().as_int();
				ev.SetCustomPrimaryColour(cust1);
			}
			if (isSecondaryColourCustom)
			{
				RgbS cust2;
				cust2.R = nodeVehicleColours.child("Cust2_R").text().as_int();
				cust2.G = nodeVehicleColours.child("Cust2_G").text().as_int();
				cust2.B = nodeVehicleColours.child("Cust2_B").text().as_int();
				ev.SetCustomSecondaryColour(cust2);
			}
			RgbS tyreSmokeRgb;
			tyreSmokeRgb.R = nodeVehicleColours.child("tyreSmoke_R").text().as_int();
			tyreSmokeRgb.G = nodeVehicleColours.child("tyreSmoke_G").text().as_int();
			tyreSmokeRgb.B = nodeVehicleColours.child("tyreSmoke_B").text().as_int();
			ev.SetTyreSmokeColour(tyreSmokeRgb);
			ev.SetInteriorColour(nodeVehicleColours.child("LrInterior").text().as_int());
			ev.SetDashboardColour(nodeVehicleColours.child("LrDashboard").text().as_int());
			ev.SetHeadlightColour(nodeVehicleColours.child("LrXenonHeadlights").text().as_int());

			// Other stuff
			ev.SetNumberPlateText(nodeVehicleStuff.child("NumberPlateText").text().as_string());
			ev.SetNumberPlateTextIndex(nodeVehicleStuff.child("NumberPlateIndex").text().as_int());
			ev.SetWheelType(nodeVehicleStuff.child("WheelType").text().as_int());
			ev.SetWindowTint(nodeVehicleStuff.child("WindowTint").text().as_int());
			ev.SetCanTyresBurst(!nodeVehicleStuff.child("BulletProofTyres").text().as_bool());
			ev.SetDirtLevel(nodeVehicleStuff.child("DirtLevel").text().as_float());
			ev.SetPaintFade(nodeVehicleStuff.child("PaintFade").text().as_float());
			ev.SetRoofState((VehicleRoofState)nodeVehicleStuff.child("RoofState").text().as_int());
			ev.SetSirenActive(nodeVehicleStuff.child("SirenActive").text().as_bool());
			if (nodeVehicleStuff.child("EngineOn")) 
			{
				ev.SetEngineRunning(nodeVehicleStuff.child("EngineOn").text().as_bool());
			}
			if (nodeVehicleStuff.child("EngineHealth")) 
			{
				ev.SetEngineHealth(nodeVehicleStuff.child("EngineHealth").text().as_int());
			}
			if (nodeVehicleStuff.child("LightsOn")) 
			{
				ev.SetLightsOn(nodeVehicleStuff.child("LightsOn").text().as_bool());
			}
			if (nodeVehicleStuff.child("IsRadioLoud").text().as_int(0))
			{
				SET_VEHICLE_RADIO_LOUD(ev.Handle(), nodeVehicleStuff.child("IsRadioLoud").text().as_int());
				SET_VEHICLE_RADIO_ENABLED(ev.Handle(), true);
			}
			ev.SetLockStatus((VehicleLockStatus)nodeVehicleStuff.child("LockStatus").text().as_int());

			// Neons
			auto nodeVehicleNeons = nodeVehicleStuff.child("Neons");
			RgbS neonLightsRgb;
			ev.SetNeonLightOn(VehicleNeonLight::Left, nodeVehicleNeons.child("Left").text().as_bool());
			ev.SetNeonLightOn(VehicleNeonLight::Right, nodeVehicleNeons.child("Right").text().as_bool());
			ev.SetNeonLightOn(VehicleNeonLight::Front, nodeVehicleNeons.child("Front").text().as_bool());
			ev.SetNeonLightOn(VehicleNeonLight::Back, nodeVehicleNeons.child("Back").text().as_bool());
			neonLightsRgb.R = nodeVehicleNeons.child("R").text().as_int();
			neonLightsRgb.G = nodeVehicleNeons.child("G").text().as_int();
			neonLightsRgb.B = nodeVehicleNeons.child("B").text().as_int();
			ev.SetNeonLightsColour(neonLightsRgb);

			// Extras (modExtras)
			auto nodeVehicleModExtras = nodeVehicleStuff.child("ModExtras");
			for (auto nodeVehicleModExtrasObject = nodeVehicleModExtras.first_child(); nodeVehicleModExtrasObject; nodeVehicleModExtrasObject = nodeVehicleModExtrasObject.next_sibling())
			{
				ev.SetExtraOn(stoi(std::string(nodeVehicleModExtrasObject.name()).substr(1)), nodeVehicleModExtrasObject.text().as_bool());
			}

			// Mods (customisations)
			auto nodeVehicleMods = nodeVehicleStuff.child("Mods");
			for (auto nodeVehicleModsObject = nodeVehicleMods.first_child(); nodeVehicleModsObject; nodeVehicleModsObject = nodeVehicleModsObject.next_sibling())
			{
				int modType = stoi(std::string(nodeVehicleModsObject.name()).substr(1));
				std::string modValueStr = nodeVehicleModsObject.text().as_string();
				if (modValueStr.find(",") == std::string::npos) // isToggleable
				{
					ev.ToggleMod(modType, nodeVehicleModsObject.text().as_bool());
				}
				else
				{
					ev.SetMod(modType, stoi(modValueStr.substr(0, modValueStr.find(","))), stoi(modValueStr.substr(modValueStr.find(",") + 1)));
				}
			}

			// Doors
			auto nodeVehicleDoorsOpen = nodeVehicleStuff.child("DoorsOpen");
			if (nodeVehicleDoorsOpen)
			{
				nodeVehicleDoorsOpen.child("BackLeftDoor").text().as_bool() ? ev.OpenDoor(VehicleDoor::BackLeftDoor, false, true) : ev.CloseDoor(VehicleDoor::BackLeftDoor, true);
				nodeVehicleDoorsOpen.child("BackRightDoor").text().as_bool() ? ev.OpenDoor(VehicleDoor::BackRightDoor, false, true) : ev.CloseDoor(VehicleDoor::BackRightDoor, true);
				nodeVehicleDoorsOpen.child("FrontLeftDoor").text().as_bool() ? ev.OpenDoor(VehicleDoor::FrontLeftDoor, false, true) : ev.CloseDoor(VehicleDoor::FrontLeftDoor, true);
				nodeVehicleDoorsOpen.child("FrontRightDoor").text().as_bool() ? ev.OpenDoor(VehicleDoor::FrontRightDoor, false, true) : ev.CloseDoor(VehicleDoor::FrontRightDoor, true);
				nodeVehicleDoorsOpen.child("Hood").text().as_bool() ? ev.OpenDoor(VehicleDoor::Hood, false, true) : ev.CloseDoor(VehicleDoor::Hood, true);
				nodeVehicleDoorsOpen.child("Trunk").text().as_bool() ? ev.OpenDoor(VehicleDoor::Trunk, false, true) : ev.CloseDoor(VehicleDoor::Trunk, true);
				nodeVehicleDoorsOpen.child("Trunk2").text().as_bool() ? ev.OpenDoor(VehicleDoor::Trunk2, false, true) : ev.CloseDoor(VehicleDoor::Trunk2, true);
			}
			auto nodeVehicleDoorsBroken = nodeVehicleStuff.child("DoorsBroken");
			if (nodeVehicleDoorsBroken)
			{
				if (nodeVehicleDoorsBroken.child("BackLeftDoor").text().as_bool()) ev.BreakDoor(VehicleDoor::BackLeftDoor, true);
				if (nodeVehicleDoorsBroken.child("BackRightDoor").text().as_bool()) ev.BreakDoor(VehicleDoor::BackRightDoor, true);
				if (nodeVehicleDoorsBroken.child("FrontLeftDoor").text().as_bool()) ev.BreakDoor(VehicleDoor::FrontLeftDoor, true);
				if (nodeVehicleDoorsBroken.child("FrontRightDoor").text().as_bool()) ev.BreakDoor(VehicleDoor::FrontRightDoor, true);
				if (nodeVehicleDoorsBroken.child("Hood").text().as_bool()) ev.BreakDoor(VehicleDoor::Hood, true);
				if (nodeVehicleDoorsBroken.child("Trunk").text().as_bool()) ev.BreakDoor(VehicleDoor::Trunk, true);
				if (nodeVehicleDoorsBroken.child("Trunk2").text().as_bool()) ev.BreakDoor(VehicleDoor::Trunk2, true);
			}

			// Tyres
			auto nodeVehicleTyresBursted = nodeVehicleStuff.child("TyresBursted");
			if (nodeVehicleTyresBursted)
			{
				if (nodeVehicleTyresBursted.child("FrontLeft").text().as_bool()) ev.BurstTyre(0);
				if (nodeVehicleTyresBursted.child("FrontRight").text().as_bool()) ev.BurstTyre(1);
				if (nodeVehicleTyresBursted.child("_2").text().as_bool()) ev.BurstTyre(2);
				if (nodeVehicleTyresBursted.child("_3").text().as_bool()) ev.BurstTyre(3);
				if (nodeVehicleTyresBursted.child("BackLeft").text().as_bool()) ev.BurstTyre(4);
				if (nodeVehicleTyresBursted.child("BackRight").text().as_bool()) ev.BurstTyre(5);
				if (nodeVehicleTyresBursted.child("_6").text().as_bool()) ev.BurstTyre(6);
				if (nodeVehicleTyresBursted.child("_7").text().as_bool()) ev.BurstTyre(7);
				if (nodeVehicleTyresBursted.child("_8").text().as_bool()) ev.BurstTyre(8);
			}

			if (nodeVehicleStuff.child("WheelsInvisible").text().as_bool()) 
			{
				SetVehicleWheelsInvisible(ev, true);
			}
			std::string engSoundName = nodeVehicleStuff.child("EngineSoundName").text().as_string();
			if (engSoundName.length()) 
			{
				SetVehicleEngineSoundName(ev, engSoundName);
			}

			// Multipliers
			auto nodeVehicleRpmMultiplier = nodeVehicleStuff.child("RpmMultiplier");
			auto nodeVehicleTorqueMultiplier = nodeVehicleStuff.child("TorqueMultiplier");
			auto nodeVehicleMaxSpeed = nodeVehicleStuff.child("MaxSpeed");
			auto nodeVehicleHeadlightIntensity = nodeVehicleStuff.child("HeadlightIntensity");
			if (nodeVehicleRpmMultiplier)
			{
				MODIFY_VEHICLE_TOP_SPEED(ev.Handle(), nodeVehicleRpmMultiplier.text().as_float());
				g_multListRPM[ev.Handle()] = nodeVehicleRpmMultiplier.text().as_float();
			}
			if (nodeVehicleTorqueMultiplier)
			{
				SET_VEHICLE_CHEAT_POWER_INCREASE(ev.Handle(), nodeVehicleTorqueMultiplier.text().as_float());
				g_multListTorque[ev.Handle()] = nodeVehicleTorqueMultiplier.text().as_float();
			}
			if (nodeVehicleMaxSpeed)
			{
				SET_ENTITY_MAX_SPEED(ev.Handle(), nodeVehicleMaxSpeed.text().as_float());
				g_multListMaxSpeed[ev.Handle()] = nodeVehicleMaxSpeed.text().as_float();
			}
			if (nodeVehicleHeadlightIntensity)
			{
				SET_VEHICLE_LIGHT_MULTIPLIER(ev.Handle(), nodeVehicleHeadlightIntensity.text().as_float());
				g_multListHeadLights[ev.Handle()] = nodeVehicleHeadlightIntensity.text().as_float();
			}

			int opacityLevel = nodeVehicle.child("OpacityLevel").text().as_int();
			if (opacityLevel < 255) ev.SetAlpha(opacityLevel);
			ev.SetLODDistance(nodeVehicle.child("LodDistance").text().as_int());

			bool bDriverIsVisible = ped.IsVisible();
			bool bVehicleIsVisible = nodeVehicle.child("IsVisible").text().as_bool();
			if (!bVehicleIsVisible) ev.SetVisible(false);
			if (nodeVehicle.child("IsDriverVisible"))
			{
				bool bSetDriverVisible = nodeVehicle.child("IsDriverVisible").text().as_bool();
				ped.RequestControl();
				switch (_driverVisibilityTexterIndex)
				{
				case 0:
					if (!bVehicleIsVisible || (!bDriverIsVisible && bSetDriverVisible || bDriverIsVisible && !bSetDriverVisible))
						ped.SetVisible(bSetDriverVisible);
					break; // FileDecides
				case 1:
					if (!bVehicleIsVisible)
						ped.SetVisible(bDriverIsVisible);
					break; // Retain
				case 2:
					ped.SetVisible(false);
					break; // ForceOff
				case 3:
					ped.SetVisible(true);
					break; // ForceOn
				}
			}
			if (nodeVehicle.child("MaxHealth")) 
			{
				ev.SetMaxHealth(nodeVehicle.child("MaxHealth").text().as_int());
			}
			if (nodeVehicle.child("Health")) 
			{
				ev.SetHealth(nodeVehicle.child("Health").text().as_int());
			}

			ev.SetOnFire(nodeVehicle.child("IsOnFire").text().as_bool());
			ev.SetInvincible(nodeVehicle.child("IsInvincible").text().as_bool());
			ev.SetBulletProof(nodeVehicle.child("IsBulletProof").text().as_bool());
			ev.SetIsCollisionEnabled(true);
			ev.SetExplosionProof(nodeVehicle.child("IsExplosionProof").text().as_bool());
			ev.SetFireProof(nodeVehicle.child("IsFireProof").text().as_bool());
			ev.SetMeleeProof(nodeVehicle.child("IsMeleeProof").text().as_bool());
			ev.SetOnlyDamagedByPlayer(nodeVehicle.child("IsOnlyDamagedByPlayer").text().as_bool());

			// Attachments
			std::unordered_set<Hash> vModelHashes;
			std::vector<sub::Spooner::SpoonerEntityWithInitHandle> vSpawnedAttachments;
			auto nodeAttachments = nodeVehicle.child("SpoonerAttachments");
			bool bAddAttachmentsToSpoonerDb = nodeAttachments.attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase").as_bool(false);
			bool bStartTaskSeqsOnLoad = nodeAttachments.attribute("StartTaskSequencesOnLoad").as_bool(true);
			switch (_persistentAttachmentsTexterIndex)
			{
				case 0: break; // FileDecides
				case 1: bAddAttachmentsToSpoonerDb = false; break; // ForceOff
				case 2: bAddAttachmentsToSpoonerDb = true; break; // ForceOn
			}

			for (auto nodeAttachment = nodeAttachments.first_child(); nodeAttachment; nodeAttachment = nodeAttachment.next_sibling())
			{
				auto e = sub::Spooner::FileManagement::SpawnEntityFromXmlNode(nodeAttachment, vModelHashes);
				vSpawnedAttachments.push_back(e);
			}

			WAIT(100);

			if (!nodeVehicleInitialHandle)
			{
				for (auto& e : vSpawnedAttachments)
				{
					sub::Spooner::EntityManagement::AttachEntity(e.e, ev, e.e.attachmentArgs.boneIndex, e.e.attachmentArgs.offset, e.e.attachmentArgs.rotation);
				}
			}
			else
			{
				sub::Spooner::SpoonerEntityWithInitHandle evwih;
				evwih.e.handle = ev;
				evwih.e.dynamic = true;
				evwih.e.type = EntityType::VEHICLE;
				evwih.e.hashName = get_vehicle_model_label(evwih.e.handle.Model(), true);
				evwih.initHandle = evInitHandle;
				vSpawnedAttachments.push_back(evwih); // ADD EV

				for (auto& e : vSpawnedAttachments)
				{
					if (e.e.attachmentArgs.isAttached)
					{
						for (auto& b : vSpawnedAttachments)
						{
							if (e.attachedToHandle == b.initHandle)
							{
								sub::Spooner::EntityManagement::AttachEntity(e.e, b.e.handle, e.e.attachmentArgs.boneIndex, e.e.attachmentArgs.offset, e.e.attachmentArgs.rotation);
								break;
							}

						}
					}
				}
			}

			if (bAddAttachmentsToSpoonerDb)
			{
				for (auto& e : vSpawnedAttachments)
				{
					if (!e.e.taskSequence.empty())
					{
						auto& vTskPtrs = e.e.taskSequence.AllTasks();
						for (auto& u : vSpawnedAttachments)
						{
							for (auto& tskPtr : vTskPtrs)
							{
								tskPtr->LoadTargetingDressing(u.initHandle, u.e.handle.Handle());
							}
						}
						if (bStartTaskSeqsOnLoad) e.e.taskSequence.Start();
					}
					sub::Spooner::Databases::EntityDb.push_back(e.e);
				}
			}

			else
			{
				if (NETWORK_IS_IN_SESSION() && !vSpawnedAttachments.empty())
				{
					if (vSpawnedAttachments.back().e.handle.Handle() == ev.Handle()) 
					{
						vSpawnedAttachments.back().e.handle = 0;
					}
				}
				for (auto& e : vSpawnedAttachments)
				{
					if (!g_spawnVehiclePersistent) 
					{
						e.e.handle.NoLongerNeeded();
					}
				}
			}

			for (auto& amh : vModelHashes)
			{
				Model(amh).Unload();
			}
			eModel.Unload();

			addlog(ige::LogType::LOG_INFO,  "Loaded vehicle file " + filePath);
			std::ostringstream ss;
			ss << "Spawned vehicle from file with " << (vSpawnedAttachments.size() - 1) << " attachments. ";
			if (bAddAttachmentsToSpoonerDb)
			{
				ss << "The attachments have been added to the Spooner Database.";
			}
			Game::Print::PrintBottomLeft(ss);
		}

		void saveColourVals()
		{
			std::ofstream outfile;
			auto& _dir = dict3;
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(g_Ped4, &r, &g, &b);
			std::array<int, 3> hsv = GetHSVFromRGB(r, g, b);
			float normalisedcolour = NormalizeHSV(hsv[0], hsv[1], hsv[2]);

			std::vector<std::pair<std::string, std::pair<std::string, float>>> ColourNames
			{
				{"3850041493", {"black", 0}},
				{"807368168", {"grey", 35}},
				{"607306136", {"silver", 53}},
				{"4066575219", {"white", 71}},
				{"1325014621", {"beige", 72}},
				{"2201497177", {"brown", 110}},
				{"1585269136", {"red", 122}},
				{"2639756769", {"orange", 129}},
				{"3440150791", {"yellow", 136}},
				{"306110198", {"green", 171}},
				{"1345033317", {"graphite", 214}},
				{"3826758445", {"blue", 240}},
				{"2019367074", {"pink", 352}},
			};

			float minDiff = static_cast<float>(INT_MAX);
			std::string nearestColour;
			std::string nearestAudio;
			for (const auto& colour : ColourNames)
			{
				float diff = std::abs(colour.second.second - normalisedcolour);
				if (diff < minDiff)
				{
					minDiff = diff;
					nearestColour = colour.second.first;
					nearestAudio = colour.first;
				}
			}
			int prim, sec;
			std::string finish;
			GET_VEHICLE_COLOURS(g_Ped4, &prim, &sec);
			switch (prim)
			{
			case 0:
				finish = "1";
				break;
			case 111:
				finish = "2";
				break;
			case 12:
				finish = "3";
				break;
			case 15:
				finish = "4";
				break;
			case 21:
				finish = "5";
				break;
			case 117:
				finish = "6";
				break;
			case 120:
				finish = "7";
				break;
			case 158:
				finish = "8";
				break;
			case 159:
				finish = "9";
				break;
			case 2: default:
				finish = "normal";
				break;
			}

			std::string customname = Game::InputBox("noname", 64, "Enter Colour Name");

			outfile.open(dict3 + "\\Custom colours.txt", std::ios_base::app); // append instead of overwrite
			outfile << "Check https://gtamods.com/wiki/Carcols.ymt for all colour names, audio hashes and IDs.\n";
			outfile << "<Item>\n";
			outfile << "      <color value = \"0xFF" + ((IntToHexString(r, false)) + IntToHexString(g, false) + IntToHexString(b, false) + "\"/>\n");
			outfile << "      <metallicID>EVehicleModelColorMetallic_" + finish + "</metallicID>\n";
			outfile << "      <audioColor>POLICE_SCANNER_COLOUR_" + nearestColour + "</audioColor>\n";
			outfile << "      <audioPrefix>none</audioPrefix>\n";
			outfile << "      <audioColorHash value=\"" + nearestAudio + "\"/>\n";
			outfile << "      <audioPrefixHash value=\"0\"/>\n";
			outfile << "      <colorName> " + std::to_string(static_cast<int>(normalisedcolour)) + " " + customname + " </colorName>\n";
			outfile << "</Item>\n";
			outfile << "\n";
			outfile.close();
			Game::Print::PrintBottomLeft("Saved Current Colour: " + customname + "\n" + "HSV: " + std::to_string(hsv[0]) + ", " + std::to_string(hsv[1]) + ", " + std::to_string(hsv[2]));
		}

		int saveCarVars(GTAvehicle vehicle)
		{
			std::ofstream outfile;
			const Model& eModel = vehicle.Model();
			std::string spawnname = GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(eModel.hash);
			int color1 = vehicle.GetPrimaryColour(), 
				color2 = vehicle.GetSecondaryColour(),
				color3 = vehicle.GetPearlescentColour(),
				color4 = vehicle.GetRimColour(),
				color5 = vehicle.GetInteriorColour(),
				color6 = vehicle.GetDashboardColour();


			outfile.open(dict3 + "\\Carvar colours.txt", std::ios_base::app); // append instead of overwrite
			outfile << spawnname << "\n";
			outfile << std::to_string(color1) << "\n";
			outfile << std::to_string(color2) << "\n";
			outfile << std::to_string(color3) << "\n";
			outfile << std::to_string(color4) << "\n";
			outfile << std::to_string(color5) << "\n";
			outfile << std::to_string(color6) << "\n";
			outfile << "\n";
			outfile.close();
			Game::Print::PrintBottomLeft("Saved Vehicle Carvariations");
			return 0;
		}

		void VehicleSaverMenu()
		{
			auto& _searchStr = dict2;
			auto& _name = dict;
			auto& _dir = dict3;

			auto ped = g_Ped1;
			auto vehicle = GET_VEHICLE_PED_IS_USING(ped);
			bool isPedInVeh = IS_PED_IN_ANY_VEHICLE(ped, 0) || IS_PED_SITTING_IN_ANY_VEHICLE(ped);

			bool save2 = false, bCreateFolderPressed = false, savecarvar = false, ms_paints_savergb = false;
			std::vector<std::string> vfilnames;

			AddTitle("Saved Vehicles");

			bool persistentAttachments_plus = 0, persistentAttachments_minus = 0;
			AddTexter("AddAttachmentsToSpoonerDB", _persistentAttachmentsTexterIndex, std::vector<std::string>{"FileDecides", "ForceOff", "ForceOn"}, null, persistentAttachments_plus, persistentAttachments_minus);
			if (persistentAttachments_plus) 
			{
				 if (_persistentAttachmentsTexterIndex < 2)
				 {
				 	_persistentAttachmentsTexterIndex++; 
				 }
			}

			if (persistentAttachments_minus) 
			{ 
				if (_persistentAttachmentsTexterIndex > 0)
				{
					_persistentAttachmentsTexterIndex--; 
				}
			}

			bool driverVisi_plus = 0, driverVisi_minus = 0;
			AddTexter("Driver Visibility", _driverVisibilityTexterIndex, std::vector<std::string>{"FileDecides", "Retain" "ForceOff", "ForceOn"}, null, driverVisi_plus, driverVisi_minus);
			if (driverVisi_plus) 
			{
				if (_driverVisibilityTexterIndex < 3) 
				{
					_driverVisibilityTexterIndex++; 
				}
			}
			if (driverVisi_minus) 
			{ 
				if (_driverVisibilityTexterIndex > 0)
				{
					_driverVisibilityTexterIndex--; 
				}
			}


			AddOption("Save Current Vehicle", save2);

			AddOption("Store CarVariations", savecarvar);
			AddOption("Save Colour Profile", ms_paints_savergb);

			AddOption("Create New Folder", bCreateFolderPressed);

			if (_dir.empty()) 
			{
				_dir = GetPathffA(Pathff::Vehicle, false);
			}
			DIR* dir_point = opendir(_dir.c_str());
			dirent* entry = readdir(dir_point);
			while (entry)
			{
				vfilnames.push_back(entry->d_name);
				entry = readdir(dir_point);
			}
			closedir(dir_point);

			AddBreak("---Found Files---");

			bool bFolderBackPressed = false;
			AddOption("..", bFolderBackPressed); if (bFolderBackPressed)
			{
				_dir = _dir.substr(0, _dir.rfind("\\"));
				Menu::currentop = 6;
			}

			if (!vfilnames.empty())
			{
				bool bSearchPressed = false;
				AddOption(_searchStr.empty() ? "SEARCH" : _searchStr, bSearchPressed, nullFunc, -1, true);
				if (bSearchPressed)
				{
					_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", boost::to_lower_copy(_searchStr));
					boost::to_upper(_searchStr);
				}

				for (auto& filname : vfilnames)
				{
					if (filname.front() == '.' || filname.front() == ',')
					{
						continue;
					}
					if (!_searchStr.empty()) 
					{
						if (boost::to_upper_copy(filname).find(_searchStr) == std::string::npos) 
						{
							continue;
						}
					}

					bool isFolder = PathIsDirectoryA((_dir + "\\" + filname).c_str()) != 0;
					bool isXml = filname.length() > 4 && filname.rfind(".xml") == filname.length() - 4;
					TICKOL icon = TICKOL::NONE;
					if (isFolder) 
					{
						icon = TICKOL::ARROWRIGHT;
					}
					else if (isXml) 
					{
						icon = TICKOL::TICK2;
					}
					bool bFilePressed = false;

					if (isFolder)
					{
						AddTickol(filname + " >>>", true, bFilePressed, bFilePressed, icon, TICKOL::NONE); 
						if (bFilePressed)
						{
							_dir = _dir + "\\" + filname;
							Menu::currentop = 6;
						}

						if (Menu::printingop == *Menu::currentopATM && !bFilePressed)
						{
							if (FolderPreviewBmps_catind::bFolderBmpsEnabled)
							{
								FolderPreviewBmps_catind::DrawBmp(_dir + "\\" + filname);
							}
						}
					}
					else if (isXml)
					{
						AddTickol(filname, true, bFilePressed, bFilePressed, icon, TICKOL::NONE); 
						if (bFilePressed)
						{
							_name = filname.substr(0, filname.rfind('.'));
							Menu::SetSub_delayed = SUB::VEHICLE_SAVER_INITEM;
							return;
						}
					}
				}
			}


			if (save2)
			{
				if (isPedInVeh)
				{
					std::string inputStr = Game::InputBox("", 28U, "Enter file name:");
					if (inputStr.length() > 0)
					{
						VehicleSaveToFile(_dir + "\\" + inputStr + ".xml", vehicle);
					}
					else
					{
						Game::Print::PrintErrorInvalidInput(inputStr);
					}
				}
			}

			if (ms_paints_savergb)
			{
				saveColourVals();
			}

			if (savecarvar)
			{
				if (isPedInVeh)
				{
					saveCarVars(vehicle);
				}
			}

			if (bCreateFolderPressed)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter folder name:");
				if (inputStr.length() > 0)
				{
					if (CreateDirectoryA((_dir + "\\" + inputStr).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
					{
						_dir = _dir + "\\" + inputStr;
						Menu::currentop = 6;
						Game::Print::PrintBottomLeft("Folder ~b~created~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Failed~s~ to create folder.");
					}
				}
				else
				{
					Game::Print::PrintErrorInvalidInput(inputStr);
				}
				return;
			}
		}

		void VehSaverInItemMenu()
		{
			auto& _name = dict;
			auto& _dir = dict3;
			std::string filePath = _dir + "\\" + _name + ".xml";

			auto& ped = g_Ped1;
			auto vehicle = GET_VEHICLE_PED_IS_USING(ped);
			bool isPedInVeh = IS_PED_IN_ANY_VEHICLE(ped, 0) || IS_PED_SITTING_IN_ANY_VEHICLE(ped);

			bool bLoad = 0, bRename = 0, bOverwrite = 0, bDelete = 0;

			AddTitle(dict);
			AddOption("Spawn", bLoad);
			AddOption("Rename File", bRename);
			if (isPedInVeh) 
			{
				AddOption("Overwrite File", bOverwrite);
			}
			AddOption("Delete File", bDelete);

			if (bLoad)
			{
				VehicleReadFromFile(filePath, ped);
			}

			if (bRename)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter new name:", _name);
				if (inputStr.length() > 0)
				{
					std::string oldPath = _dir + "\\" + _name + ".xml";
					std::string newPath = _dir + "\\" + inputStr + ".xml";
					if (rename(oldPath.c_str(), (newPath).c_str()) == 0)
					{
						Game::Print::PrintBottomLeft("File ~b~renamed~s~.");
						_name = inputStr;
					}
					else 
					{
						Game::Print::PrintBottomCentre("~r~Error~s~ renaming file.");
					}
				}
				else 
				{
					Game::Print::PrintErrorInvalidInput(inputStr);
				}
			}

			if (bOverwrite)
			{
				if (isPedInVeh)
				{
					VehicleSaveToFile(filePath, vehicle);
				}
			}

			if (bDelete)
			{
				if (remove(filePath.c_str()) == 0) 
				{
					Game::Print::PrintBottomLeft("File ~b~deleted~s~.");
				}
				else 
				{
					Game::Print::PrintBottomCentre("~r~Error~s~ deleting file.");
				}
				Menu::SetPreviousMenu();
				Menu::Up();
				return;
			}

			//=============================================================================
			pugi::xml_document doc;
			if (doc.load_file((const char*)filePath.c_str()).status == pugi::status_ok)
			{
				AddBreak("---Attributes---");
				auto nodeVehicle = doc.child("Vehicle"); // Root

				auto nodeDriverVisible = nodeVehicle.child("IsDriverVisible");
				if (nodeDriverVisible)
				{
					bool bToggleDriverVisiblePressed = false;
					AddTickol("Driver Visibility", nodeDriverVisible.text().as_bool(), bToggleDriverVisiblePressed, bToggleDriverVisiblePressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); 
					if (bToggleDriverVisiblePressed)
					{
						nodeDriverVisible.text() = !nodeDriverVisible.text().as_bool();
						doc.save_file((const char*)filePath.c_str());
					}
				}

				auto nodeAddAttachmentsToSpoonerDb = nodeVehicle.child("SpoonerAttachments").attribute("SetAttachmentsPersistentAndAddToSpoonerDatabase");
				bool bAddAttachemntsToSpoonerDb = nodeAddAttachmentsToSpoonerDb.as_bool();
				bool bToggleAddAttachmentsToSpoonerDbPressed = false;
				AddTickol("Persistent Attachments (AddToSpoonerDb)", bAddAttachemntsToSpoonerDb, bToggleAddAttachmentsToSpoonerDbPressed, bToggleAddAttachmentsToSpoonerDbPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); 
				if (bToggleAddAttachmentsToSpoonerDbPressed)
				{
					nodeAddAttachmentsToSpoonerDb = !nodeAddAttachmentsToSpoonerDb.as_bool();
					bAddAttachemntsToSpoonerDb = !bAddAttachemntsToSpoonerDb;
					doc.save_file((const char*)filePath.c_str());
				}

				if (bAddAttachemntsToSpoonerDb)
				{
					auto nodeStartTaskSeqOnLoad = nodeVehicle.child("SpoonerAttachments").attribute("StartTaskSequencesOnLoad");
					if (nodeStartTaskSeqOnLoad)
					{
						bool bToggleStartTaskSeqOnLoadPressed = false;
						AddTickol("Start Task Sequences Immediately", nodeStartTaskSeqOnLoad.as_bool(), bToggleStartTaskSeqOnLoadPressed, bToggleStartTaskSeqOnLoadPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); 
						if (bToggleStartTaskSeqOnLoadPressed)
						{
							nodeStartTaskSeqOnLoad = !nodeStartTaskSeqOnLoad.as_bool();
							doc.save_file((const char*)filePath.c_str());
						}
					}
				}
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(SPAWNVEHICLE,               	sub::SpawnVehicleMenu)
REGISTER_SUBMENU(SPAWNVEHICLE_OPTIONS,       	sub::SpawnVehicleOptions)
REGISTER_SUBMENU(SPAWNVEHICLE_ALLCATS,       	sub::SpawnVehicleAllCategoriesMenu)
REGISTER_SUBMENU(SPAWNVEHICLEDLC,            	sub::SpawnVehicleDLC)
REGISTER_SUBMENU(SPAWNVEHICLE_DLC_SELECTION, 	sub::SpawnVehicleDLCSelection)
REGISTER_SUBMENU(SPAWNVEHICLE_FAVOURITES,    	sub::SpawnVehicleFavouritesMenu)
REGISTER_SUBMENU(VEHICLE_SAVER,       			sub::VehicleSaver::VehicleSaverMenu)
REGISTER_SUBMENU(VEHICLE_SAVER_INITEM, 			sub::VehicleSaver::VehSaverInItemMenu)