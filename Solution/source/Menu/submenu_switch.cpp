/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "submenu_switch.h"

#include "..\macros.h"

#include "Menu.h"

#include "..\Submenus\MainMenu.h"
#include "..\Submenus\Settings.h"
#include "..\Submenus\MiscOptions.h"
#include "..\Submenus\WeaponOptions.h"
#include "..\Submenus\Players.h"
#include "..\Submenus\PlayerOptions.h"
#include "..\Submenus\WeatherOptions.h"
#include "..\Submenus\TimeOptions.h"

#include "..\Submenus\PtfxSubs.h"
#include "..\Submenus\PedAnimation.h"
#include "..\Submenus\PedModelChanger.h"
#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\PedSpeech.h"
#include "..\Submenus\BreatheStuff.h"
#include "..\Submenus\GhostRiderMode.h"

#include "..\Submenus\VehicleOptions.h"
#include "..\Submenus\VehicleSpawner.h"
#include "..\Submenus\FunnyVehicles.h"
#include "..\Submenus\VehicleModShop.h"

#include "..\Submenus\Teleport\Teleport_Submenus.h"
#include "..\Submenus\Teleport\OfficeGarages.h"
#include "..\Submenus\Teleport\IeVehicleWarehouses.h"
#include "..\Submenus\Teleport\BikerInteriors.h"
#include "..\Submenus\Teleport\GunRunningInteriors.h"
#include "..\Submenus\Teleport\Hangars.h"
#include "..\Submenus\Teleport\Facilities.h"
#include "..\Submenus\Teleport\Nightclubs.h"
#include "..\Submenus\Teleport\ArenaWar.h"
#include "..\Submenus\Teleport\Yachts.h"

#include "..\Submenus\MiscMapMods.h"
#include "..\Submenus\CutscenePlayer.h"
#include "..\Submenus\AnimalRiding.h"
#include "..\Submenus\StatManager.h"

#include "..\Submenus\Spooner\Submenus.h"
#include "..\Submenus\Spooner\Submenus_TaskSequence.h"
#include "..\Submenus\Bodyguards\BodyguardMenu.h"
#include "..\Submenus\Bodyguards\BodyguardSettings.h"
#include "..\Submenus\Bodyguards\BodyguardManagement.h"
#include "..\Submenus\Bodyguards\BodyguardSpawn.h"
#include "..\Submenus\Bodyguards\BodyguardSubmenu.h"

using namespace sub;

std::unordered_map<int, SubmenuHandler>& SubMenuRegistry::GetMap()
{
	static std::unordered_map<int, SubmenuHandler> map;
	return map;
}

void SubMenuRegistry::Register(int sub, SubmenuHandler handler)
{
	GetMap()[sub] = handler;
}

void SubMenuRegistry::Dispatch(int sub)
{
	auto& map = GetMap();
	auto it = map.find(sub);
	if (it != map.end())
	{
		it->second();
	}
}

bool SubMenuRegistry::IsRegistered(int sub)
{
	auto& map = GetMap();
	return map.find(sub) != map.end();
}

// Submenu registration
// Main & Settings
REGISTER_SUBMENU(MAINMENU,            MainMenu)
REGISTER_SUBMENU(SETTINGS,            Settings)
REGISTER_SUBMENU(SETTINGS_MENUPOS,    SettingsMenuPos_)
REGISTER_SUBMENU(SETTINGS_THEMES,     SettingsThemes_catind::Sub_Main)
REGISTER_SUBMENU(SETTINGS_COLOURS,    SettingsColours)
REGISTER_SUBMENU(SETTINGS_COLOURS2,   SettingsColours2)
REGISTER_SUBMENU(SETTINGS_FONTS,      SettingsFonts)
REGISTER_SUBMENU(SETTINGS_FONTS2,     SettingsFonts2)
REGISTER_SUBMENU(SETTINGS_LANGUAGE,   SettingsLanguage)
REGISTER_SUBMENU(TIMECYCLES,          Timecycles_)

// Players
REGISTER_SUBMENU(PLAYERSSUB,           PlayersSub_)
REGISTER_SUBMENU(PLAYERSSUBAMENU,      PlayersSubAMenu_)
REGISTER_SUBMENU(CLONECOMPANIONSUB,    CloneCompanionSub)
REGISTER_SUBMENU(PEDEXPLOSIONSUB,      PedExplosionSub)
REGISTER_SUBMENU(ATTACHFUNNYOBJECTSUB, AttachFunnyObjectSub)
REGISTER_SUBMENU(CLEARAREA,            ClearAreaSub)
REGISTER_SUBMENU(PLAYEROPS,            PlayerOps_)

// PTFX
REGISTER_SUBMENU(PTFXSUB,        Ptfx_catind::PTFXSub)
REGISTER_SUBMENU(PTFX_FAVORITES, Ptfx_catind::PTFXFavourites)

// Animations
REGISTER_SUBMENU(ANIMATIONSUB,                     AnimationSub_)
REGISTER_SUBMENU(ANIMATIONSUB_SETTINGS,            AnimationSub_Settings)
REGISTER_SUBMENU(ANIMATIONSUB_FAVOURITES,          AnimationSub_Favourites)
REGISTER_SUBMENU(ANIMATIONSUB_CUSTOM,              AnimationSub_Custom)
REGISTER_SUBMENU(ANIMATIONSUB_DEER,                AnimationSub_Deer)
REGISTER_SUBMENU(ANIMATIONSUB_SHARK,               AnimationSub_Shark)
REGISTER_SUBMENU(ANIMATIONSUB_MISSRAPPEL,          AnimationSub_MissRappel)
REGISTER_SUBMENU(ANIMATIONSUB_GESTSIT,             AnimationSub_GestSit)
REGISTER_SUBMENU(ANIMATIONSUB_SWAT,                AnimationSub_Swat)
REGISTER_SUBMENU(ANIMATIONSUB_GUARDREAC,           AnimationSub_GuardReac)
REGISTER_SUBMENU(ANIMATIONSUB_RANDARREST,          AnimationSub_RandArrest)
REGISTER_SUBMENU(ANIMATIONSUB_ALLPEDANIMS,         AnimationSub_catind::Sub_AllPedAnims)
REGISTER_SUBMENU(ANIMATIONSUB_ALLPEDANIMS_INDICT,  AnimationSub_catind::Sub_AllPedAnims_InDict)
REGISTER_SUBMENU(ANIMATIONSUB_TASKSCENARIOS,       AnimationSub_TaskScenarios::AnimationSub_TaskScenarios)
REGISTER_SUBMENU(ANIMATIONSUB_TASKSCENARIOS2,      AnimationSub_TaskScenarios::AnimationSub_TaskScenarios2)
REGISTER_SUBMENU(FACIALMOOD,                       FacialAnims_catind::Sub_FacialMood)
REGISTER_SUBMENU(MOVEMENTGROUP,                    MovementGroup_)

// Model Changer
REGISTER_SUBMENU(MODELCHANGER,                     ModelChanger_)
REGISTER_SUBMENU(MODELCHANGER_FAVOURITES,          PedFavourites_catind::Sub_PedFavourites)
REGISTER_SUBMENU(MODELCHANGER_PLAYER,              ModelChanger_Player)
REGISTER_SUBMENU(MODELCHANGER_ANIMAL,              ModelChanger_Animal)
REGISTER_SUBMENU(MODELCHANGER_AMBFEMALES,          ModelChanger_AmbientFemale)
REGISTER_SUBMENU(MODELCHANGER_AMBMALES,            ModelChanger_AmbientMale)
REGISTER_SUBMENU(MODELCHANGER_CS,                  ModelChanger_Cutscene)
REGISTER_SUBMENU(MODELCHANGER_GANGFEMALES,         ModelChanger_GangFemale)
REGISTER_SUBMENU(MODELCHANGER_GANGMALES,           ModelChanger_GangMale)
REGISTER_SUBMENU(MODELCHANGER_STORY,               ModelChanger_Story)
REGISTER_SUBMENU(MODELCHANGER_MP,                  ModelChanger_Multiplayer)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOFEMALES,     ModelChanger_ScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_SCENARIOMALES,       ModelChanger_ScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOFEMALES,  ModelChanger_Story_ScenarioFemale)
REGISTER_SUBMENU(MODELCHANGER_ST_SCENARIOMALES,    ModelChanger_Story_ScenarioMale)
REGISTER_SUBMENU(MODELCHANGER_OTHERS,              ModelChanger_Others)

// Components / Appearance
REGISTER_SUBMENU(COMPONENTS,           ComponentChanger_)
REGISTER_SUBMENU(COMPONENTS2,          ComponentChanger2_)
REGISTER_SUBMENU(COMPONENTSPROPS,      ComponentChangerProps_)
REGISTER_SUBMENU(COMPONENTSPROPS2,     ComponentChangerProps2_)
REGISTER_SUBMENU(COMPONENTS_OUTFITS,   ComponentChanger_Outfits)
REGISTER_SUBMENU(COMPONENTS_OUTFITS2,  ComponentChanger_Outfits2)
REGISTER_SUBMENU(PEDDECALS_TYPES,      PedDecals_catind::Sub_Decals_Types)
REGISTER_SUBMENU(PEDDECALS_ZONES,      PedDecals_catind::Sub_Decals_Zones)
REGISTER_SUBMENU(PEDDECALS_INZONE,     PedDecals_catind::Sub_Decals_InZone)

// Vehicle Options
REGISTER_SUBMENU(VEHICLEOPS,           VehicleOps)
REGISTER_SUBMENU(PVOPS,                PVOpsSub_)
REGISTER_SUBMENU(VEHICLEWEAPONS,       VehicleWeapons_)
REGISTER_SUBMENU(VEHICLEMULTIPLIERS,   VehicleMultipliers_)
REGISTER_SUBMENU(VEHICLESPEEDOS,       Speedo_catind::Sub_Main)
REGISTER_SUBMENU(VEHICLESPEEDOS_LIGHT, Speedo_catind::Sub_Themes_Light)
REGISTER_SUBMENU(VEHICLESPEEDOS_DARK,  Speedo_catind::Sub_Themes_Dark)
REGISTER_SUBMENU(MULTIPLATNEONS,       VehicleMultiPlatNeons_Sub)
REGISTER_SUBMENU(VEHICLESLAM,          VehicleSlam_catind::Sub_VehicleSlam)

// Vehicle Spawner
REGISTER_SUBMENU(SPAWNVEHICLE,               SpawnVehicle_)
REGISTER_SUBMENU(SPAWNVEHICLE_OPTIONS,       SpawnVehicle_Options)
REGISTER_SUBMENU(SPAWNVEHICLE_ALLCATS,       SpawnVehicle_AllCatsSub)
REGISTER_SUBMENU(SPAWNVEHICLEDLC,            SpawnVehicle_DLC)
REGISTER_SUBMENU(SPAWNVEHICLE_DLC_SELECTION, SpawnVehicle_DLC_Selection)
REGISTER_SUBMENU(SPAWNVEHICLE_FAVOURITES,    SpawnVehicle_Favourites)
REGISTER_SUBMENU(FUNNYVEHICLES,              FunnyVehicles_catind::Sub_FunnyVehicles)

// Mod Shop
REGISTER_SUBMENU(MODSHOP,              ModShop_)
REGISTER_SUBMENU(MS_BENNYS,            Bennys_catind::Sub_BennysMain)
REGISTER_SUBMENU(MS_EMBLEM,            MSEmblem_)
REGISTER_SUBMENU(MSWHEELS,             MSWheels_)
REGISTER_SUBMENU(MSWHEELS2,            MSWheels2_)
REGISTER_SUBMENU(MSWHEELS3,            MSWheels3_)
REGISTER_SUBMENU(MS_TYRESBURST,        MSTyresBurst_)
REGISTER_SUBMENU(MSPAINTS,             MSPaints_)
REGISTER_SUBMENU(MSPAINTS2,            MSPaints2_)
REGISTER_SUBMENU(MSPAINTS2_CHROME,     MSPaints_catind::Sub_Chrome)
REGISTER_SUBMENU(MSPAINTS2_MATTE,      MSPaints_catind::Sub_Matte)
REGISTER_SUBMENU(MSPAINTS2_METAL,      MSPaints_catind::Sub_Metal)
REGISTER_SUBMENU(MSPAINTS2_CHAMELEON,  MSPaints_catind::Sub_Chameleon)
REGISTER_SUBMENU(MSPAINTS2_PEARL,      MSPaints_catind::Sub_Pearl)
REGISTER_SUBMENU(MSPAINTS2_METALLIC,   MSPaints_catind::Sub_Metallic)
REGISTER_SUBMENU(MSPAINTS2_NORMAL,     MSPaints_catind::Sub_Normal)
REGISTER_SUBMENU(MSPAINTS2_UTIL,       MSPaints_catind::Sub_Util)
REGISTER_SUBMENU(MSPAINTS2_WORN,       MSPaints_catind::Sub_Worn)
REGISTER_SUBMENU(MSPAINTS2_SHARED,     MSPaints_catind::Sub_Shared)
REGISTER_SUBMENU(MSPAINTS_RGB,         MSPaints_RGB)
REGISTER_SUBMENU(MSCATALL,             MSCatall_)
REGISTER_SUBMENU(MSWINDOWS,            MSWindows_catind::MSWindows_)
REGISTER_SUBMENU(MSENGINESOUND,        MSEngineSound_)
REGISTER_SUBMENU(MSLIGHTS,             MSLights_)
REGISTER_SUBMENU(MSDOORS,              MSDoors_)
REGISTER_SUBMENU(MSNEONS,              MSNeons_)
REGISTER_SUBMENU(MSEXTRA,              MSExtra_)

// Teleport
REGISTER_SUBMENU(TELEPORTOPS,                           TeleportLocations_catind::Submenus::Sub_TeleportMain)
REGISTER_SUBMENU(TELEPORTOPS_CUSTOMCOORDS,              TeleportLocations_catind::Submenus::Sub_CustomCoords)
REGISTER_SUBMENU(TELEPORTOPS_SELECTEDCATEGORY,          TeleportLocations_catind::Submenus::Sub_SelectedCategory)
REGISTER_SUBMENU(TELEPORTOPS_BLIPLIST,                  TeleportLocations_catind::Submenus::Sub_BlipList)
REGISTER_SUBMENU(TELEPORTOPS_SAVEDLOCATIONS,            TeleportLocations_catind::Submenus::Sub_SavedLocations)
REGISTER_SUBMENU(TELEPORTOPS_OFFICEGARAGES,             TeleportLocations_catind::OfficeGarages::Sub_OfficeGarages)
REGISTER_SUBMENU(TELEPORTOPS_OFFICEGARAGES_INLOC,       TeleportLocations_catind::OfficeGarages::Sub_OfficeGarages_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_IEVEHICLEWAREHOUSES,       TeleportLocations_catind::IeVehicleWarehouses::Sub_IeVehicleWarehouses)
REGISTER_SUBMENU(TELEPORTOPS_IEVEHICLEWAREHOUSES_INLOC, TeleportLocations_catind::IeVehicleWarehouses::Sub_IeVehicleWarehouses_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_BIKERCLUBHOUSES,           TeleportLocations_catind::BikerInteriors::Clubhouses::Sub_Clubhouses)
REGISTER_SUBMENU(TELEPORTOPS_BIKERCLUBHOUSES_INLOC,     TeleportLocations_catind::BikerInteriors::Clubhouses::Sub_Clubhouses_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_BUSINESSES,                TeleportLocations_catind::BikerInteriors::Businesses::Sub_Businesses)
REGISTER_SUBMENU(TELEPORTOPS_BUSINESSES_INLOC,          TeleportLocations_catind::BikerInteriors::Businesses::Sub_Businesses_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_BUNKERS,                   TeleportLocations_catind::GunRunningInteriors::Bunkers::Sub_Bunkers)
REGISTER_SUBMENU(TELEPORTOPS_BUNKERS_INLOC,             TeleportLocations_catind::GunRunningInteriors::Bunkers::Sub_Bunkers_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_MOC,                       TeleportLocations_catind::GunRunningInteriors::Moc::Sub_Moc)
REGISTER_SUBMENU(TELEPORTOPS_MOC_INLOC,                 TeleportLocations_catind::GunRunningInteriors::Moc::Sub_Moc_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_HANGARS,                   TeleportLocations_catind::Hangars::Sub_Hangars)
REGISTER_SUBMENU(TELEPORTOPS_HANGARS_INLOC,             TeleportLocations_catind::Hangars::Sub_Hangars_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_HANGARS_INOPTION,          TeleportLocations_catind::Hangars::Sub_Hangars_InOption)
REGISTER_SUBMENU(TELEPORTOPS_FACILITIES,                TeleportLocations_catind::Facilities::Sub_Facilities)
REGISTER_SUBMENU(TELEPORTOPS_FACILITIES_INLOC,          TeleportLocations_catind::Facilities::Sub_Facilities_InLoc)
REGISTER_SUBMENU(TELEPORTOPS_FACILITIES_INOPTION,       TeleportLocations_catind::Facilities::Sub_Facilities_InOption)
REGISTER_SUBMENU(TELEPORTOPS_NIGHTCLUBS,                TeleportLocations_catind::Nightclubs::Sub_Nightclubs)
REGISTER_SUBMENU(TELEPORTOPS_ARENAWAR,                  TeleportLocations_catind::ArenaWar::Sub_ArenaWar)
REGISTER_SUBMENU(TELEPORTOPS_YACHTS,                    TeleportLocations_catind::Yachts::Sub_Yachts)
REGISTER_SUBMENU(TELEPORTOPS_YACHTS_INGRP,              TeleportLocations_catind::Yachts::Sub_Yachts_InGrp)

// Weapons
REGISTER_SUBMENU(WEAPONOPS,                    Weaponops)
REGISTER_SUBMENU(WEAPONOPS_WEAPONFAVOURITES,   WeaponFavourites_catind::Sub_WeaponFavourites)
REGISTER_SUBMENU(WEAPONOPS_INDIVS_CATEGORIES,  WeaponIndivs_catind::Sub_CategoriesList)
REGISTER_SUBMENU(WEAPONOPS_INDIVS_CATEGORY,    WeaponIndivs_catind::Sub_InCategory)
REGISTER_SUBMENU(WEAPONOPS_INDIVS_ITEM,        WeaponIndivs_catind::Sub_InItem)
REGISTER_SUBMENU(WEAPONOPS_INDIVS_ITEM_MODS,   WeaponIndivs_catind::Sub_InItem_Mods)
REGISTER_SUBMENU(WEAPONOPS_LOADOUTS,           WeaponsLoadouts_catind::Sub_Loadouts)
REGISTER_SUBMENU(WEAPONOPS_LOADOUTS_INITEM,    WeaponsLoadouts_catind::Sub_Loadouts_InItem)
REGISTER_SUBMENU(WEAPONOPS_PARACHUTE,          WeaponIndivs_catind::Sub_Parachute)
REGISTER_SUBMENU(WEAPONOPS_LASERSIGHT,         LaserSight_catind::Sub_LaserSight)
REGISTER_SUBMENU(FORGEGUN,                     ForgeGun_)
REGISTER_SUBMENU(GRAVITYGUN,                   GravityGun_catind::Sub_GravityGun)
REGISTER_SUBMENU(KABOOMGUN,                    KaboomGun_)
REGISTER_SUBMENU(TRIGGERFXGUN,                 TriggerFxGun_catind::Sub_GunMain)
REGISTER_SUBMENU(BULLETGUN,                    BulletGun_)
REGISTER_SUBMENU(PEDGUN,                       PedGun_)
REGISTER_SUBMENU(PEDGUN_ALLPEDS,               PedGun_AllPeds)
REGISTER_SUBMENU(OBJECTGUN,                    ObjectGun_)
REGISTER_SUBMENU(LIST_VEHICLECATS,             List_VehicleCats_Sub)

// World
REGISTER_SUBMENU(WEATHEROPS, WeatherOps_)
REGISTER_SUBMENU(CLOUDOPS,   WeatherClouds_catind::sub_CloudOps)
REGISTER_SUBMENU(WATERHACK,  WaterHack_catind::Sub_WaterHack)
REGISTER_SUBMENU(TIMEOPS,    TimeOps_)

// Misc
REGISTER_SUBMENU(MISCOPS,            MiscOps)
REGISTER_SUBMENU(MAPMODS,            MapMods_catind::MapMods_)
REGISTER_SUBMENU(MAPMODS2,           MapMods_catind::MapMods2_)
REGISTER_SUBMENU(OBJECTSPAWNER_OBJS, ObjectSpawner_objs)

// Ped features
REGISTER_SUBMENU(VOICECHANGER,            Speech_catind::Sub_VoiceChanger)
REGISTER_SUBMENU(SPEECHPLAYER,            Speech_catind::Sub_AmbientSpeechPlayer)
REGISTER_SUBMENU(SPEECHPLAYER_INVOICE,    Speech_catind::Sub_AmbientSpeechPlayer_InVoice)
REGISTER_SUBMENU(BREATHESTUFF,            BreatheStuff_catind::BreatheStuffSub_)
REGISTER_SUBMENU(GHOSTRIDERMODE,          GhostRiderMode_catind::Sub_GhostRiderMode)
REGISTER_SUBMENU(PEDFLAGMANAGER_NAMEDLIST, PedConfigFlagManager_catind::Sub_FlagList)
REGISTER_SUBMENU(PEDFLAGMANAGER_CUSTOM,    PedConfigFlagManager_catind::Sub_CustomFlagSetter)

// Ped damage / head features
REGISTER_SUBMENU(PEDDAMAGET_CATEGORYLIST,              PedDamageTextures_catind::Sub_CategoryList)
REGISTER_SUBMENU(PEDDAMAGET_BONESELECTION,             PedDamageTextures_catind::Sub_BoneSelection)
REGISTER_SUBMENU(PEDDAMAGET_BLOOD,                     PedDamageTextures_catind::Sub_Blood)
REGISTER_SUBMENU(PEDDAMAGET_DAMAGEDECALS,              PedDamageTextures_catind::Sub_DamageDecals)
REGISTER_SUBMENU(PEDDAMAGET_DAMAGEPACKS,               PedDamageTextures_catind::Sub_DamagePacks)
REGISTER_SUBMENU(PED_HEADFEATURES_MAIN,                PedHeadFeatures_catind::Sub_Main)
REGISTER_SUBMENU(PED_HEADFEATURES_HEADOVERLAYS,        PedHeadFeatures_catind::Sub_HeadOverlays)
REGISTER_SUBMENU(PED_HEADFEATURES_HEADOVERLAYS_INITEM, PedHeadFeatures_catind::Sub_HeadOverlays_InItem)
REGISTER_SUBMENU(PED_HEADFEATURES_FACEFEATURES,        PedHeadFeatures_catind::Sub_FaceFeatures)
REGISTER_SUBMENU(PED_HEADFEATURES_SKINTONE,            PedHeadFeatures_catind::Sub_SkinTone)

// Vehicle saver / misc vehicle
REGISTER_SUBMENU(VEHICLE_SAVER,        VehicleSaver_catind::Sub_VehSaver)
REGISTER_SUBMENU(VEHICLE_SAVER_INITEM, VehicleSaver_catind::Sub_VehSaver_InItem)
REGISTER_SUBMENU(RADIOSUB,             RadioSub_)
REGISTER_SUBMENU(AUTODRIVESUB,         VehicleAutoDrive_catind::Sub_AutoDrive)

// Misc utilities
REGISTER_SUBMENU(CUTSCENEPLAYER,       CutscenePlayer_catind::Sub_CutsceneList)
REGISTER_SUBMENU(ENTITYALPHALEVEL,     EntityAlphaLevelSub_)
REGISTER_SUBMENU(ANIMALRIDING,         AnimalRiding_catind::Sub_AnimalRiding)
REGISTER_SUBMENU(TVCHANNELSTUFF_TV,    TVChannelStuff_catind::Sub_TV)
REGISTER_SUBMENU(CLOCK,                Clock_catind::Sub_Clock)
REGISTER_SUBMENU(HUDOPTIONS,           HudOptions_catind::Sub_HudOptions)
REGISTER_SUBMENU(GAMECAMOPTIONS,       GameCamOptions_catind::Sub_GameCamOptions)

// Stat Manager
REGISTER_SUBMENU(SPSTATMANAGER,               SpStatManager_catind::Sub_SpStatManager)
REGISTER_SUBMENU(SPSTATMANAGER_INCHAR,        SpStatManager_catind::Sub_InChar)
REGISTER_SUBMENU(SPSTATMANAGER_INCHAR_INLIST, SpStatManager_catind::Sub_InChar_InList)

// Spooner
REGISTER_SUBMENU(SPOONER_MAIN,                                        Spooner::Submenus::Sub_SpoonerMain)
REGISTER_SUBMENU(SPOONER_SPAWN_CATEGORIES,                            Spooner::Submenus::Sub_SpawnCategories)
REGISTER_SUBMENU(SPOONER_SPAWN_PROP,                                  Spooner::Submenus::Sub_SpawnProp)
REGISTER_SUBMENU(SPOONER_SPAWN_PROP_FAVOURITES,                       Spooner::Submenus::Sub_SpawnProp_Favourites)
REGISTER_SUBMENU(SPOONER_SPAWN_PED,                                   Spooner::Submenus::Sub_SpawnPed)
REGISTER_SUBMENU(SPOONER_SPAWN_VEHICLE,                               Spooner::Submenus::Sub_SpawnVehicle)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS,                               Spooner::Submenus::Sub_ManageMarkers)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_REMOVAL,                       Spooner::Submenus::Sub_ManageMarkers_Removal)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER,                      Spooner::Submenus::Sub_ManageMarkers_InMarker)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER_DEST2MARKER,          Spooner::Submenus::Sub_ManageMarkers_InMarker_Dest2Marker)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER_ATTACH,               Spooner::Submenus::Sub_ManageMarkers_InMarker_Attach)
REGISTER_SUBMENU(SPOONER_MANAGEDB,                                    Spooner::Submenus::Sub_ManageEntities)
REGISTER_SUBMENU(SPOONER_MANAGEDB_REMOVAL,                            Spooner::Submenus::Sub_ManageEntities_Removal)
REGISTER_SUBMENU(SPOONER_SAVEFILES,                                   Spooner::Submenus::Sub_SaveFiles)
REGISTER_SUBMENU(SPOONER_SAVEFILES_LOAD,                              Spooner::Submenus::Sub_SaveFiles_Load)
REGISTER_SUBMENU(SPOONER_SAVEFILES_LOAD_LEGACYSP00N,                  Spooner::Submenus::Sub_SaveFiles_Load_LegacySP00N)
REGISTER_SUBMENU(SPOONER_VECTOR3_MANUALPLACEMENT,                     Spooner::Submenus::Sub_Vector3_ManualPlacement)
REGISTER_SUBMENU(SPOONER_QUICKMANUALPLACEMENT,                        Spooner::Submenus::Sub_QuickManualPlacement)
REGISTER_SUBMENU(SPOONER_GROUPSPOON,                                  Spooner::Submenus::Sub_GroupSpoon)
REGISTER_SUBMENU(SPOONER_GROUPSPOON_SELECTENTITIES,                   Spooner::Submenus::Sub_GroupSpoon_SelectEntities)
REGISTER_SUBMENU(SPOONER_GROUPSPOON_ATTACHTO,                         Spooner::Submenus::Sub_GroupSpoon_AttachTo)
REGISTER_SUBMENU(SPOONER_SETTINGS,                                    Spooner::Submenus::Sub_Settings)
REGISTER_SUBMENU(SPOONER_SELECTEDENTITYOPS,                           Spooner::Submenus::Sub_SelectedEntityOps)
REGISTER_SUBMENU(SPOONER_PEDOPS,                                      Spooner::Submenus::Sub_PedOps)
REGISTER_SUBMENU(SPOONER_PEDOPS_WEAPON,                               Spooner::Submenus::Sub_PedOps_Weapon)
REGISTER_SUBMENU(SPOONER_PEDOPS_WEAPON_INCATEGORY,                    Spooner::Submenus::Sub_PedOps_Weapon_InCategory)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKLIST,                       Spooner::Submenus::Sub_TaskSequence_TaskList)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_ADDTASK,                        Spooner::Submenus::Sub_TaskSequence_AddTask)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_INTASK,                         Spooner::Submenus::Sub_TaskSequence_InTask)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKSUB_SCENARIOACTION_LIST,    Spooner::Submenus::Sub_TaskSequence::ScenarioAction_list)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKSUB_PLAYANIMATION_SETTINGS, Spooner::Submenus::Sub_TaskSequence::PlayAnimation_settings)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKSUB_PLAYANIMATION_ALLPEDANIMS,        Spooner::Submenus::Sub_TaskSequence::PlayAnimation_allPedAnims)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKSUB_PLAYANIMATION_ALLPEDANIMS_INDICT, Spooner::Submenus::Sub_TaskSequence::PlayAnimation_allPedAnims_inDict)
REGISTER_SUBMENU(SPOONER_TASKSEQUENCE_TASKSUB_PLAYSPEECHWITHVOICE_INVOICE,      Spooner::Submenus::Sub_TaskSequence::PlaySpeechWithVoice_inVoice)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS,                               Spooner::Submenus::Sub_AttachmentOps)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS_ATTACHTO,                      Spooner::Submenus::Sub_AttachmentOps_AttachTo)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS_SELECTBONE,                    Spooner::Submenus::Sub_AttachmentOps_SelectBone)
REGISTER_SUBMENU(SPOONER_MANUALPLACEMENT,                             Spooner::Submenus::Sub_ManualPlacement)
REGISTER_SUBMENU(SPOONER_SIZEMANIPULATION,                            Spooner::Submenus::Sub_SizeManipulation)

// Bodyguards
REGISTER_SUBMENU(BODYGUARDMAINMENU,  BodyguardMainMenu)
REGISTER_SUBMENU(BODYGUARD_SPAWN,    BodyguardMenu::BodyguardSpawn)
REGISTER_SUBMENU(BODYGUARD_LIST,     BodyguardMenu::BodyguardList)
REGISTER_SUBMENU(BODYGUARD_SETTINGS, BodyguardMenu::BodyguardOps_)
REGISTER_SUBMENU(BODYGUARD_ENTITYOPS, BodyguardMenu::BodyguardEntityOps)
REGISTER_SUBMENU(BODYGUARD_WEAPONOPS, BodyguardMenu::BodyguardWeaponOps)

// Menu dispatch

void Menu::submenu_switch()
{
	SubMenuRegistry::Dispatch(currentsub);
}
