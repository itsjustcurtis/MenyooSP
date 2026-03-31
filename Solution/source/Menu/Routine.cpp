/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "Routine.h"

#include "..\macros.h"

#include "Menu.h"
#include "MenuConfig.h"

#include "..\Util\FileLogger.h"
#include "..\Util\ExePath.h"
#include "..\Util\keyboard.h"
#include "..\Scripting\enums.h"
#include "..\Natives\types.h" // RGBA/RgbS & types
#include "..\Natives\natives2.h"
#include "..\Scripting\DxHookIMG.h"
#include "..\Scripting\TimecycleModification.h"
#include "..\Scripting\Model.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\Camera.h"
#include "..\Scripting\GameplayCamera.h"
#include "..\Memory\GTAmemory.h"
#include "..\Scripting\CustomHelpText.h"
#include "..\Scripting\GTAplayer.h"
#include "..\Scripting\GTAentity.h"
#include "..\Scripting\GTAvehicle.h"
#include "..\Scripting\GTAped.h"
#include "..\Scripting\Game.h"
#include "..\Scripting\PTFX.h"
#include "..\Scripting\World.h"
#include "..\Scripting\WeaponIndivs.h"
#include "..\Scripting\Raycast.h"
#include "..\Scripting\ModelNames.h"
#include "..\Scripting\GTAblip.h"

#include "..\Misc\FlameThrower.h"
#include "..\Misc\FpsCounter.h"
#include "..\Misc\Gta2Cam.h"
#include "..\Misc\JumpAroundMode.h"
#include "..\Misc\MagnetGun.h"
#include "..\Misc\ManualRespawn.h"
#include "..\Misc\MeteorShower.h"
#include "..\Misc\RopeGun.h"
#include "..\Misc\SmashAbility.h"
#include "..\Misc\VehicleCruise.h"
#include "..\Misc\VehicleFly.h"
#include "..\Misc\VehicleTow.h"

#include "..\Submenus\PedAnimation.h"
#include "..\Submenus\WeaponOptions.h"
#include "..\Submenus\PedComponentChanger.h"
#include "..\Submenus\AnimalRiding.h"
#include "..\Submenus\VehicleSpawner.h"
#include "..\Menu\FolderPreviewBmps.h"
#include "..\Submenus\PedSpeech.h"
#include "..\Submenus\GhostRiderMode.h"
#include "..\Submenus\Spooner\SpoonerMode.h"
#include "..\Submenus\VehicleOptions.h"
#include "..\Submenus\VehicleModShop.h"
#include "..\Submenus\TimeOptions.h"
#include "..\Submenus\BreatheStuff.h"
#include "..\Submenus\MiscOptions.h"
#include "..\Submenus\Teleport\Yachts.h"
#include "..\Submenus\Teleport\TeleMethods.h"
#include "..\Submenus\PtfxSubs.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\EntityManagement.h"
#include "..\Submenus\CutscenePlayer.h"

#include <Windows.h>
#include <thread>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <time.h>

DWORD g_MenyooConfigTick = 0UL;
DWORD g_FaderTick = 0UL;
bool defaultPedSet = false;

void Menu::justopened()
{
	Game::Print::PrintBottomLeft(oss_ << "Menyoo PC v" << MENYOO_CURRENT_VER_ << " by ItsJustCurtis and MAFINS");

	SET_AUDIO_FLAG("IsDirectorModeActive", true);

	SET_THIS_SCRIPT_CAN_BE_PAUSED(0);
	SET_THIS_SCRIPT_CAN_REMOVE_BLIPS_CREATED_BY_ANY_SCRIPT(0); // lol poopoo dummy me this isn't a ysc

	if (!GTAmemory::GetIsEnhanced()) {
		if (
			IS_DLC_PRESENT(GET_HASH_KEY("mp2023_01_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mp2023_02_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mpchristmas3_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mpg9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mpSum2_G9EC")) or
			IS_DLC_PRESENT(GET_HASH_KEY("patch2023_01_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("patchday27g9ecng")) or
			IS_DLC_PRESENT(GET_HASH_KEY("patchday28g9ecng")) or
			IS_DLC_PRESENT(GET_HASH_KEY("patchdayg9ecng")) or
			IS_DLC_PRESENT(GET_HASH_KEY("patch2024_01_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mp2024_01_g9ec")) or
			IS_DLC_PRESENT(GET_HASH_KEY("mp2024_02_g9ec"))			//this hardcoding needs to get in the bin.
			)
		{
			//Game::Print::PrintBottomCentre("~r~Warning~s~: 9th Gen content detected, Game may crash. Read Menyoolog for fix instructions.");
				 //Rockstar seems to have fixed the invalid content crash in legacy, removing the on-screen warning, but will keep the log warning.
			ige::myLog << ige::LogType::LOG_WARNING << "Gen9 Content found in dlcpacks, this can cause instability when attempted to be loaded by Menyoo." << std::endl
				<< "			You can find these in your dlclist.xml by searching for \"g9\" and removing these lines or using a comment." << std::endl
				<< "				    		For example: <!--<Item>dlcpacks:/mpg9ec/</Item>-->" << std::endl << std::endl
				<< "				    Current known Gen9 Packs:" << std::endl
				<< "				    		patchdayg9ecng" << std::endl
				<< "				    		mpsum2_g9ec" << std::endl
				<< "				   	 	patchday27g9ecng" << std::endl
				<< "				   	 	mpchristmas3_g9ec" << std::endl
				<< "				   	 	patchday28g9ecng" << std::endl
				<< "				   	 	mp2023_01_g9ec" << std::endl
				<< "				   	 	mp2023_02_g9ec" << std::endl
				<< "				   	 	patch2024_01_g9ec" << std::endl
				<< "				   	 	mp2024_01_g9ec" << std::endl
				<< "				  	  	mp2024_02_g9ec" << std::endl << std::endl
				<< "				    Note: mp2024_02 also contains bugged content. If you continue to experience issues, removing this may help" << std::endl << std::endl;
		}
		else if (
			IS_DLC_PRESENT(GET_HASH_KEY("mp2024_02")) 		//f*cking rockstar cocked up some clothes, this warning is the only protection. 
			)
		{
			//Game::Print::PrintBottomCentre("~r~Warning~s~: DLCPack mp2024_02 present, Game may crash. Read Menyoolog for fix instructions.");
			ige::myLog << ige::LogType::LOG_WARNING << "mp2024_02 found in dlcpacks, certain bugged clothing can cause instability when attempted to be loaded by Menyoo." << std::endl
				<< "				    You can find this in your dlclist.xml by searching for \"mp2024_02\" and removing these lines or using a comment." << std::endl
				<< "				    		For example: <!--<Item>dlcpacks:/mp2024_02/</Item>-->" << std::endl << std::endl
				<< "				    Note: this issue can be ignored if bugged content has been fixed by a mod" << std::endl;
		}
	}

	sub::PopulateAllPaintIDs();

	g_menuNotOpenedYet = false;
}
inline void MenyooMain()
{	
	bool firstTick = true;
	addlog(ige::LogType::LOG_TRACE, "Loading Textures");
	DxHookIMG::LoadAllMenyooTexturesInit();
	addlog(ige::LogType::LOG_TRACE, "Populate Anims List");
	sub::AnimationSub_catind::PopulateAllPedAnimsList();
	addlog(ige::LogType::LOG_TRACE, "Populate Favourites");
	sub::WeaponFavourites_catind::PopulateFavouritesInfo();
	addlog(ige::LogType::LOG_TRACE, "Populate Decals");
	sub::PedDecals_catind::PopulateDecalsDict();
	addlog(ige::LogType::LOG_TRACE, "Populate Animals");
	sub::AnimalRiding_catind::PopulateAnimals();
	addlog(ige::LogType::LOG_TRACE, "Populate Vehicle Previews");
	sub::SpawnVehicle_catind::PopulateVehicleBmps();
	addlog(ige::LogType::LOG_TRACE, "Populate Folder Previews");
	sub::FolderPreviewBmps_catind::PopulateFolderBmps();
	addlog(ige::LogType::LOG_TRACE, "Populate Voice Data");
	sub::Speech_catind::PopulateVoiceData();
	addlog(ige::LogType::LOG_TRACE, "Populate Timecycle Names");
	TimecycleModification::PopulateTimecycleNames();
	addlog(ige::LogType::LOG_TRACE, "Populate Global Entity Arrays");
	PopulateGlobalEntityModelsArrays();
	addlog(ige::LogType::LOG_TRACE, "Populate Cutscene Labels");
	sub::CutscenePlayer_catind::PopulateCutsceneLabels();

	srand(GetTickCount());
	SET_RANDOM_SEED(GetTickCount());
	g_MenyooConfigTick = GetTickCount();
	g_FaderTick = GetTickCount();


	addlog(ige::LogType::LOG_TRACE, "Check Valid for Block Vehicles");
	if (!NETWORK_IS_SESSION_STARTED() && !IS_COMMANDLINE_END_USER_BENCHMARK() && !LANDING_SCREEN_STARTED_END_USER_BENCHMARK())
	{
		addlog(ige::LogType::LOG_TRACE, "Valid, Enabling Blocked Vehicles");
		if (GTAmemory::FindShopController())
			GeneralGlobalHax::EnableBlockedMpVehiclesInSp();
	}
	else
	{
		addlog(ige::LogType::LOG_ERROR, "Invalid, Unable to Unblock Vehicles");
	}

	addlog(ige::LogType::LOG_TRACE, "Creating Tick loop");
	for (;;)
	{
		if (firstTick)
			addlog(ige::LogType::LOG_TRACE, "First Tick - Textures");
		DxHookIMG::DxTexture::GlobalDrawOrderRef() = -9999;
		if (firstTick)
			addlog(ige::LogType::LOG_TRACE, "First Tick - Tick");
		Menu::Tick();
		if (firstTick)
			addlog(ige::LogType::LOG_TRACE, "First Tick - Load MenyooConfig");
		TickMenyooConfig();
		if (firstTick)
			addlog(ige::LogType::LOG_TRACE, "First Tick - Rainbow Fader");
		TickRainbowFader();
		WAIT(0);
		if (firstTick)
			addlog(ige::LogType::LOG_TRACE, "First Tick - looping");
		firstTick = false;
	}

}
void ThreadMenyooMain()
{
	addlog(ige::LogType::LOG_TRACE, "Launching MenyooMain");
	MenyooMain();
}

void TickMenyooConfig()
{
	static bool firstTick = true;
	if (firstTick)
		addlog(ige::LogType::LOG_TRACE, "First Tick - Run TickMenyooConfig");
	//if (GetTickCount() > g_MenyooConfigOnceTick + 9000U)
	if (GetTickCount() > g_MenyooConfigTick + 30000U)
	{
		if (MenuConfig::bSaveAtIntervals)
		{
			MenuConfig::SaveConfig();
		}
		g_MenyooConfigTick = GetTickCount();
	}
	firstTick = false;
}

void TickRainbowFader()
{
	static bool firstTick = true;
	if (firstTick)
		addlog(ige::LogType::LOG_TRACE, "First Tick - Run TickRainbowFader");
	if (GetTickCount() > g_FaderTick + 20U) {
		auto& colour = g_fadedRGB;
		if (colour.R > 0 && colour.B == 0)
		{
			colour.R--;
			colour.G++;
		}
		if (colour.G > 0 && colour.R == 0)
		{
			colour.G--;
			colour.B++;
		}
		if (colour.B > 0 && colour.G == 0)
		{
			colour.R++;
			colour.B--;
		}
		firstTick = false;

		g_FaderTick = GetTickCount();
	}
}

// Global state variables

INT16 BindNoClip = VirtualKey::F3;

RgbS g_fadedRGB(255, 0, 0);

UINT8 pauseClockH;
UINT8 pauseClockM;
Vehicle g_myVeh = 0;
GTAmodel::Model g_myVehModel;
Hash g_myWeap = 0U;
PTFX::sFxData triggerFXGunData = { "scr_fbi4", "scr_fbi4_trucks_crash" };
Hash kaboomGunHash = EXPLOSION::DIR_WATER_HYDRANT, bullet_gun_hash = WEAPON_FLARE;
GTAmodel::Model pedGunHash = PedHash::KillerWhale, objectGunHash = VEHICLE_BUS;
FLOAT currentTimescale = 1.0f;

INT g_Ped1;
INT g_Ped2;
INT g_Ped3;
INT g_Ped4;
const char* g_PlayerName;

INT bitMSPaintsRGBMode;

bool kaboomGunInvis = false;
bool kaboomGunRandBit = false;
bool pedGunRandBit = false;
bool objectGunRandBitO = false;
bool objectGunRandBitV = false;
bool bitNightVision = false;

FLOAT swimSpeedMult = 0;
FLOAT playerNoiseMult = 1.0f;
FLOAT selfSweatMult = 0.0f;
FLOAT g_playerVerticalElongationMultiplier = 1.0f;
FLOAT vehicleDamageAndDefense = 1.0f;
FLOAT vehicleSlam = 0.0f;
FLOAT currentTimecycleStrength = 1.0f;

INT accelMult = 0;
INT brakeMult = 0;
INT handlingMult = 0;

INT16 g_frozenRadioStation = -1;

bool bitVehicleGravity = false;
bool bitFreezeVehicle = false;
bool bitVehicleSlippyTires = false;

INT msCurrentPaintIndex = 0;

// String variables used in various submenus for search, storage, etc.
std::string dict;
std::string dict2;
std::string dict3;

// Spawn vehicle settings
std::string g_spawnVehiclePlateText = "MENYOO";
INT8 g_spawnVehiclePlateType = 5;
INT8 g_spawnVehiclePlateTexterValue = 0;

RgbS g_spawnVehicleNeonColor = { 0, 255, 0 };
bool g_spawnVehicleAutoSit = true;
bool g_warpNear = false;
bool g_addBlip = false;
bool g_spawnVehicleAutoUpgrade = true;
bool g_spawnVehicleInvincible = true;
bool g_spawnVehiclePersistent = false;
bool g_spawnVehicleDeleteOld = false;
bool g_spawnVehicleNeonToggle = false;
bool g_LSCCustoms = true;


INT16 g_spawnVehiclePrimaryColor = -3;
INT16 g_spawnVehicleSecondaryColor = -3;
bool g_spawnVehicleDrawBMPs = true;

FLOAT g_clearAreaRadius = 36.0f;
FLOAT g_rainFXIntensity = 0.0f;

Entity g_driveWaterObject;
INT8 spectatePlayer = -1;
UINT8 explostionWP = 0;

bool multiPlatNeons = false;
bool multiPlatNeonsRainbow = false;
RgbS g_multiPlatNeonsColor(0, 255, 0);
std::vector<GTAvehicle> g_multiPlatNeonsList;

bool checkSelfDeathModel = false;

UINT8 carJump = 0;
UINT8 autoKillEnemies = 0;
float weaponDamageIncrease = 1.0f;

UINT8 forceField = 0;
UINT8 selfFreezeWantedLevel = 0;
Entity bitInfiniteAmmoEnth = 0;

bool rainbowBoxes = false; 
bool forgeGun = false;
bool playerNoRagdoll = false;
bool playerSeatbelt = false;
bool playerUnlimitedAbility = false;
bool playerAutoClean = false;
bool playerWalkUnderwater = false;
bool explosiveRounds = false;
bool flamingRounds = false;
bool teleportGun = false;
bool kaboomGun = false;
bool triggerFXGun = false;
bool bulletGun = false;
bool pedGun = false;
bool objectGun = false; 
bool lightGun = false;
bool bulletTime = false; 
bool selfTriggerbot = false;
bool explosiveMelee = false; 
bool superJump = false;
bool selfRefillHealthInCover = false;
bool playerInvincibility = false;
bool noClip = false;
bool noClipToggle = false; 
bool superRun = false;
bool xyzhCoords = false; 
bool ignoredByEveryone = false; 
bool neverWanted = false;
bool superman = false;
bool supermanAuto = false;
bool vehiclePopulation = false; 
bool pedPopulation = false;
bool clearWeaponPickups = false; 
bool driveOnWater = false;
bool massacreMode = false;
bool playerBurn = false;
bool vehicleInvincibility = false;
bool vehicleHeavyMass = false;
bool raceBoost = false;
bool carHydraulics = false; 
bool superGrip = false;
bool superCarMode = false; 
bool unlimitedVehicleBoost = true;

bool vehicleWeaponLines = false; 
bool vehicleRPG = false; 
bool vehicleFireworks = false; 
bool vehicleGuns = false; 
bool vehicleSnowballs = false; 
bool vehicleBalls = false; 
bool vehicleWaterHydrant = false; 
bool vehicleLaserGreen = false; 
bool vehicleFlameLeak = false;
bool vehicleLaserRed = false; 
bool vehicleTurretsValkyrie = false; 
bool vehicleFlaregun = false; 
bool vehicleHeavySniper = false; 
bool vehicleTazerWeapon = false; 
bool vehicleMolotovWeapon = false; 
bool vehicleCombatPDW = false;
bool carColorChange = false; 
bool vehicleInvisibility = false; 
bool selfEngineOn = false; 
bool hideHUD = false; 
bool showFullHUD = false;
bool pauseClock = false; 
bool syncClock = false; 
bool tripleBullets = false; 
bool rapidFire = false; 
bool selfResurrectionGun = false; 
bool soulSwitchGun = false; 
bool selfDeleteGun = false; 
bool vehicleFixLoop = false; 
bool vehicleFlipLoop = false;
bool blackoutMode = false; 
bool simpleBlackoutMode = false; 
bool restrictedAreasAccess = false; 
bool hvSnipers = false; 
bool vehicleDisableSiren = false; 
bool fireworksDisplay = false;
bool bitInfiniteAmmo = false; 
bool selfInfiniteParachutes = false;

Entity targetSlotEntity = 0;

bool targetEntityLocked = false;
bool bitGravityGunDisabled = false;

float forgeDist = 6.0f;
float g_forgeGunPrecision = 0.2f;
float g_forgeGunShootForce = 300.0f;
bool objectSpawnForgeAssistance = false;

DWORD g_lastSpeedDisplayTime = 0;
DWORD g_lastFOVDisplayTime = 0;
float g_lastSpeedValue = 0.0f;
float g_lastFOVValue = 0.0f;

DWORD g_lastHeightLockMessageTime = 0;
const char* g_lastHeightLockMessage = nullptr;

bool g_unlockMaxIDs = false;
UINT8 max_shapeAndSkinIDs = 46;

// Game - HUD
void DisplayFullHUDThisFrame(bool bEnabled)
{
	DISPLAY_AMMO_THIS_FRAME(bEnabled);
	DISPLAY_CASH(bEnabled);

	std::list<HudComponent> comps
	{
		HudComponent::Cash,
		HudComponent::MpCash,
		HudComponent::MpRankBar,
		HudComponent::WantedStars,
		HudComponent::WeaponIcon,
		HudComponent::VehicleName,
		HudComponent::AreaName,
		HudComponent::StreetName,
		HudComponent::Reticle,
	};

	if (bEnabled)
	{
		for (auto& x : comps)
		{
			SHOW_HUD_COMPONENT_THIS_FRAME((int)x);
		}
	}
	else
	{
		for (auto& x : comps)
		{
			HIDE_HUD_COMPONENT_THIS_FRAME((int)x);
		}
	}
}

// World - Entities
void UpdateNearbyStuffArraysTick()
{
	nearbyPeds.clear();
	nearbyVehicles.clear();
	worldPeds.clear();
	worldVehicles.clear();
	worldObjects.clear();
	worldEntities.clear();

	GTAmemory::GetVehicleHandles(worldVehicles);
	GTAmemory::GetPedHandles(worldPeds);
	GTAmemory::GetPropHandles(worldObjects);
	GTAmemory::GetEntityHandles(worldEntities);

	Ped me = PLAYER_PED_ID();
	INT i, offsettedID, count = 100;

	std::vector<Ped> peds(count * 2 + 2);
	peds[0] = count;
	INT found = GET_PED_NEARBY_PEDS(me, (Any*)peds.data(), -1);
	for (i = 0; i < found; i++)
	{
		offsettedID = i * 2 + 2;
		if (!DOES_ENTITY_EXIST(peds[offsettedID])) continue;
		nearbyPeds.push_back(peds[offsettedID]);
	}

	std::vector<Vehicle> vehicles(count * 2 + 2);
	vehicles[0] = count;
	found = GET_PED_NEARBY_VEHICLES(me, (Any*)vehicles.data());
	for (i = 0; i < found; i++)
	{
		offsettedID = i * 2 + 2;
		if (!DOES_ENTITY_EXIST(vehicles[offsettedID])) continue;
		nearbyVehicles.push_back(vehicles[offsettedID]);
	}
}

// Game - HUD (teleport to wp command)
void SetPauseMenuTeleToWpCommand()
{
	if ((PauseMenuState)GET_PAUSE_MENU_STATE() == PauseMenuState::ViewingMap)
	{
		GTAentity myPed = PLAYER_PED_ID();
		if (IS_WAYPOINT_ACTIVE() && myPed.IsAlive())
		{
			(Menu::bitController ? DxHookIMG::teleToWpBoxIconGamepad : DxHookIMG::teleToWpBoxIconKeyboard).Draw(0, Vector2(0.5f, 0.04f), Vector2(0.0943f, 0.016f), 0.0f, RGBA::AllWhite());

			if (Menu::bitController ? IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RLEFT) : IsKeyJustUp(VirtualKey::T))
			{
				sub::TeleportLocations_catind::TeleMethods::ToWaypoint(myPed);
			}
		}
	}
}

// PTFX
void SetPTFXLopTick()
{
	using sub::Ptfx_catind::fxLops;

	if (GET_GAME_TIMER() > Menu::delayedTimer)
	{
		for (auto it = fxLops.begin(); it != fxLops.end();)
		{
			if (!it->entity.Exists())
			{
				it = fxLops.erase(it);
				continue;
			}

			switch ((EntityType)it->entity.Type())
			{
			case EntityType::PED:
				if (IS_PED_A_PLAYER(it->entity.Handle()) && it->entity.Handle() != PLAYER_PED_ID())
				{
					PTFX::TriggerPTFX(it->asset, it->fx, NULL, GET_PED_BONE_COORDS(it->entity.Handle(), Bone::SKEL_Head, 0.0f, 0.0f, 0.0f), it->entity.Rotation_get(), GET_RANDOM_FLOAT_IN_RANGE(0.76f, 1.4f));
				}
				else
				{
					PTFX::TriggerPTFX(it->asset, it->fx, it->entity, Vector3(), Vector3(), GET_RANDOM_FLOAT_IN_RANGE(0.76f, 1.4f), Bone::SKEL_Head);
				}
				break;
			default:
				PTFX::TriggerPTFX(it->asset, it->fx, it->entity, Vector3(), Vector3(), GET_RANDOM_FLOAT_IN_RANGE(0.76f, 1.4f));
				break;
			}
			++it;
		}
	}
}

// Time
void SetSyncClockTime()
{
	time_t now = time(0);
	tm t;
	localtime_s(&t, &now);
	SET_CLOCK_DATE(t.tm_mday, t.tm_mon, t.tm_year + 1900);
	SET_CLOCK_TIME(t.tm_hour, t.tm_min, t.tm_sec);
}

// Misc - massacre mode
void SetMassacreModeTick()
{
	float tempCoords1[3];
	Ped tempPed = PLAYER_PED_ID();

	for (GTAentity veh : nearbyVehicles)
	{
		if (veh.Equals(g_myVeh))
		{
			continue;
		}
		veh.RequestControlOnce();
		APPLY_FORCE_TO_ENTITY(veh.Handle(), 1, GET_RANDOM_FLOAT_IN_RANGE(1.0f, 9.0f), GET_RANDOM_FLOAT_IN_RANGE(1.0f, 9.0f), GET_RANDOM_FLOAT_IN_RANGE(1.0f, 6.0f), 5.0f, 13.0f, 6.5f, 1, 1, 1, 1, 0, 1);
	}

	for (GTAped ped : nearbyPeds)
	{
		ped.RequestControlOnce();
		ped.GiveNM(NMString::nm0286_handCuffsBehindBack);
		SET_PED_RAGDOLL_FORCE_FALL(ped.Handle());
		APPLY_FORCE_TO_ENTITY(ped.Handle(), 1, GET_RANDOM_FLOAT_IN_RANGE(1.0f, 9.0f), GET_RANDOM_FLOAT_IN_RANGE(1.0f, 9.0f), GET_RANDOM_FLOAT_IN_RANGE(1.0f, 6.0f), 5.0f, 13.0f, 6.5f, 1, 1, 1, 1, 0, 1);
	}

	if (rand() % 2)
	{
		Vector3(GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(tempPed, GET_RANDOM_FLOAT_IN_RANGE(25.0f, 50.0f), GET_RANDOM_FLOAT_IN_RANGE(25.0f, 50.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f))).ToArray(tempCoords1);
		ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], EXPLOSION::TRAIN, 0.2f, 0, 0, 0.05f, false);
		Vector3(GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(tempPed, GET_RANDOM_FLOAT_IN_RANGE(25.0f, 50.0f), GET_RANDOM_FLOAT_IN_RANGE(-25.0f, -50.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f))).ToArray(tempCoords1);
		ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], EXPLOSION::TRAIN, 0.2f, 0, 0, 0.05f, false);
	}
	else
	{
		Vector3(GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(tempPed, GET_RANDOM_FLOAT_IN_RANGE(-25.0f, -50.0f), GET_RANDOM_FLOAT_IN_RANGE(25.0f, 50.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f))).ToArray(tempCoords1);
		ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], EXPLOSION::TRAIN, 4.0f, 0, 0, 0.15f, false);
		Vector3(GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(tempPed, GET_RANDOM_FLOAT_IN_RANGE(-25.0f, -50.0f), GET_RANDOM_FLOAT_IN_RANGE(-25.0f, -50.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f))).ToArray(tempCoords1);
		ADD_EXPLOSION(tempCoords1[0], tempCoords1[1], tempCoords1[2], EXPLOSION::TRAIN, 4.0f, 0, 0, 0.15f, false);
	}
}

#include <random>
int GetRandomSpriteId()
{
	static std::vector<int> values = { 396, 303, 304, 397, 394, 462, 206, 161, 42, 3 };
	static size_t index = 0;
	static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

	// Shuffle the values once we've used them all
	if (index == 0)
	{
		std::shuffle(values.begin(), values.end(), rng);
	}

	int value = values[index++];
	if (index >= values.size())
	{
		index = 0; // Reset for the next shuffle cycle
	}

	return value;
}
// Misc
void SetBlackoutEMPMode()
{
	SET_ARTIFICIAL_LIGHTS_STATE(TRUE);

	for (auto& vehicle : nearbyVehicles)
	{
		if (vehicle == g_myVeh) continue;

		NETWORK_REQUEST_CONTROL_OF_ENTITY(vehicle);
		SET_VEHICLE_ENGINE_ON(vehicle, 0, 1, 0);
	}

	ScrHandle tempSeq;
	OPEN_SEQUENCE_TASK(&tempSeq);
	TASK_LEAVE_ANY_VEHICLE(0, 0, 0);
	TASK_CLEAR_LOOK_AT(0);
	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(0, 0);
	TASK_STAND_STILL(0, 300);
	TASK_START_SCENARIO_IN_PLACE(0, "WORLD_HUMAN_STAND_IMPATIENT", 800, 1);
	TASK_USE_MOBILE_PHONE_TIMED(0, 6000);
	TASK_WANDER_STANDARD(0, 0x471c4000, 0);
	CLOSE_SEQUENCE_TASK(tempSeq);

	for (auto& ped : nearbyPeds)
	{
		if (GET_SEQUENCE_PROGRESS(ped) < 0)
		{
			if (!IS_PED_IN_ANY_VEHICLE(ped, 0))
			{
				continue;
			}
			if (GET_ENTITY_SPEED(GET_VEHICLE_PED_IS_IN(ped, 0)) > 0.6f)
			{
				continue;
			}

			NETWORK_REQUEST_CONTROL_OF_ENTITY(ped);
			TASK_PERFORM_SEQUENCE(ped, tempSeq);
			SET_PED_KEEP_TASK(ped, 1);
			SET_PED_FLEE_ATTRIBUTES(ped, 0, 1);
			SET_PED_FLEE_ATTRIBUTES(ped, 1024, 1);
			SET_PED_FLEE_ATTRIBUTES(ped, 131072, 1);
		}
	}
	CLEAR_SEQUENCE_TASK(&tempSeq);
}
void SetBlackoutMode()
{
	SET_ARTIFICIAL_LIGHTS_STATE(TRUE);
	SET_ARTIFICIAL_VEHICLE_LIGHTS_STATE(FALSE);
}

// Playerped - ability
void SetSelfNearbyPedsCalm()
{
	for (auto& ped : nearbyPeds)
	{
		NETWORK_REQUEST_CONTROL_OF_ENTITY(ped);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, 1);
		SET_PED_FLEE_ATTRIBUTES(ped, 0, 0);
		SET_PED_COMBAT_ATTRIBUTES(ped, 17, 1);
	}
}
void NetworkSetEveryoneIgnorePlayer(Player player)
{
	SET_POLICE_IGNORE_PLAYER(player, 1);
	SET_EVERYONE_IGNORE_PLAYER(player, 1);
	SET_PLAYER_CAN_BE_HASSLED_BY_GANGS(player, 0);
	SET_IGNORE_LOW_PRIORITY_SHOCKING_EVENTS(player, 1);
}

// World
void SetExplosionAtCoords(GTAentity entity, Vector3 pos, UINT8 type, float radius, float camshake, bool sound, bool invis, GTAentity owner)
{
	const Vector3& Pos = (entity.Handle() == 0) ? pos : entity.GetOffsetInWorldCoords(pos);

	if (owner.Handle() != 0 && owner.IsPed())
	{
		ADD_OWNED_EXPLOSION(owner.Handle(), Pos.x, Pos.y, Pos.z, type, radius, sound, invis, camshake);
	}
	else
	{
		ADD_EXPLOSION(Pos.x, Pos.y, Pos.z, type, radius, sound, invis, camshake, false);
	}
}
// World-Misc
void StartFireworksAtCoords(const Vector3& pos, const Vector3& rot, float scale)
{
	if (!HAS_NAMED_PTFX_ASSET_LOADED("scr_indep_fireworks"))
	{
		REQUEST_NAMED_PTFX_ASSET("scr_indep_fireworks");
	}
	{
		std::vector<std::string> fw{ "scr_indep_firework_starburst", "scr_indep_firework_fountain", "scr_indep_firework_shotburst", "scr_indep_firework_trailburst" };
		USE_PARTICLE_FX_ASSET("scr_indep_fireworks");
		SET_PARTICLE_FX_NON_LOOPED_COLOUR(GET_RANDOM_FLOAT_IN_RANGE(0.0f, 1.0f), GET_RANDOM_FLOAT_IN_RANGE(0.0f, 1.0f), GET_RANDOM_FLOAT_IN_RANGE(0.0f, 1.0f));
		START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD(fw[rand() % 4].c_str(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, scale, 0, 0, 0, false);
	}
}

// Weapon - funguns
void SetTargetIntoSlot()
{
	GTAplayer player = PLAYER_ID();

	if (player.IsTargetingAnything() || player.IsFreeAiming())
	{
		if (rapidFire)
		{
			SetRapidFire();
		}
		if (selfTriggerbot)
		{
			SetPlayerTriggerbot(player);
		}
		if (soulSwitchGun)
		{
			SetSoulSwitchGun();
		}
		if (selfDeleteGun)
		{
			SetSelfDeleteGun();
		}
		if (selfResurrectionGun)
		{
			SetSelfResurrectionGun();
		}
		if (hvSnipers)
		{
			SetHVSnipers(true);
		}

		if (!targetEntityLocked)
		{
			targetSlotEntity = player.AimedEntity().GetHandle();
			if (targetSlotEntity == 0)
			{
				GTAentity aimedEntity = World::EntityFromAimCamRay();
				if (aimedEntity.Handle() != 0)
				{
					targetSlotEntity = aimedEntity.Handle();
				}
				else
				{
					targetSlotEntity = 0;
					return;
				}
			}

			if (IS_ENTITY_A_PED(targetSlotEntity))
			{
				if (IS_PED_SITTING_IN_ANY_VEHICLE(targetSlotEntity))
				{
					targetSlotEntity = GET_VEHICLE_PED_IS_IN(targetSlotEntity, 0);
				}
			}

			targetEntityLocked = true;
		}
	}
	else
	{
		if (hvSnipers)
		{
			SetHVSnipers(false);
		}
		targetEntityLocked = false;
		targetSlotEntity = 0;
	}
}

void SetPlayerTriggerbot(GTAplayer player)
{
	GTAentity playerPed = player.GetPed();

	if (player.IsTargetingAnything() || player.IsFreeAiming())
	{
		GTAentity target;
		if (GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player.Handle(), &target.Handle()))
		{
			if (target.IsPed() && target.IsAlive())
			{
				Hash weap = 0;
				GET_CURRENT_PED_WEAPON(playerPed.Handle(), &weap, true);

				if (IS_WEAPON_VALID(weap))
				{
					static const std::array<int, 1> triggerbotBoneList
					{
						{
							Bone::SKEL_Head,
						}
					};

					const Vector3& targetPos = GET_PED_BONE_COORDS(target.Handle(), triggerbotBoneList[rand() % triggerbotBoneList.size()], 0.0f, 0.0f, 0.0f);
					if (player.Handle() == PLAYER_ID())
					{
						SET_PED_SHOOTS_AT_COORD(playerPed.Handle(), targetPos.x, targetPos.y, targetPos.z, 0);
					}
					else
					{
						GTAentity gunObj = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed.Handle(), 0);
						const Vector3& launchPos = gunObj.GetOffsetInWorldCoords(0, gunObj.Dim1().y, 0);
						CLEAR_AREA_OF_PROJECTILES(launchPos.x, launchPos.y, launchPos.z, 4.0f, 0);
						World::ShootBullet(launchPos, targetPos, playerPed, weap, 5, -1, true, true);
					}
				}
			}
		}
	}
}

void SetRapidFire()
{
	if (GET_WEAPONTYPE_GROUP(g_myWeap) == WeaponGroupHash::Melee)
	{
		return;
	}

	DISABLE_CONTROL_ACTION(0, INPUT_ATTACK, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_ATTACK2, TRUE);

	if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_ATTACK))
	{
		Player playerPed = PLAYER_PED_ID();
		GTAentity gunObj = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed, 0);
		const Vector3& camDir = GameplayCamera::GetDirectionFromScreenCentre();
		const Vector3& camPos = GameplayCamera::GetPosition();
		const Vector3& launchPos = camPos + (camDir * (camPos.DistanceTo(gunObj.GetPosition()) + 0.4f));
		const Vector3& targPos = camPos + (camDir * 200.0f);

		CLEAR_AREA_OF_PROJECTILES(launchPos.x, launchPos.y, launchPos.z, 6.0f, 0);

		SET_CONTROL_SHAKE(0, 250, 125);
		World::ShootBullet(launchPos, targPos, playerPed, g_myWeap, 5, 24000.0f, true, true);
		World::ShootBullet(launchPos, targPos, playerPed, g_myWeap, 5, 24000.0f, true, true);
		STOP_CONTROL_SHAKE(0);
	}
}

void SetSoulSwitchGun()
{
	if (g_myWeap != WEAPON_COMBATPISTOL)
	{
		return;
	}

	DISABLE_CONTROL_ACTION(0, INPUT_ATTACK, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_ATTACK2, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_VEH_ATTACK, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_VEH_ATTACK2, TRUE);

	GTAplayer player = PLAYER_ID();
	GTAentity playerPed = PLAYER_PED_ID();
	ScrHandle eHandle;
	if (GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player.Handle(), &eHandle))
	{
		GTAentity soulSwitchEntity = eHandle;
		if (soulSwitchEntity.IsPed() && soulSwitchEntity.IsAlive() && (IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_ATTACK) || (IS_PED_IN_ANY_VEHICLE(playerPed.Handle(), false) && IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_VEH_ATTACK))))
		{
			if (IS_SPECIAL_ABILITY_ACTIVE(PLAYER_ID(), 0))
			{
				SPECIAL_ABILITY_DEACTIVATE_FAST(PLAYER_ID(), 0);
				WAIT(16);
			}

			Game::Sound::PlayFrontend("Knuckle_Crack_Hard_Cel", "MP_SNACKS_SOUNDSET");
			ANIMPOSTFX_PLAY("MinigameEndNeutral", 0, 0); // FocusIn
			SetBecomePed(soulSwitchEntity);

			SET_CONTROL_SHAKE(0, 4000, 210);
			STOP_CONTROL_SHAKE(0);

			if (g_Ped1 == playerPed.Handle())
			{
				g_Ped1 = PLAYER_PED_ID();
			}

			soulSwitchEntity.Handle() = 0;
			sub::Spooner::SpoonerEntity spe;
			spe.Handle = playerPed;
			if (!(sub::Spooner::EntityManagement::GetEntityIndexInDb(spe) >= 0))
			{
				playerPed.NoLongerNeeded();
			}

			WAIT(64);

			playerPed = PLAYER_PED_ID();

			SET_CURRENT_PED_WEAPON(playerPed.Handle(), WEAPON_COMBATPISTOL, true);
			SET_PED_CURRENT_WEAPON_VISIBLE(playerPed.Handle(), 1, 1, 1, 0);
		}
	}
}

void SetSelfDeleteGun()
{
	if (g_myWeap == WEAPON_SNSPISTOL)
	{
		if (IS_PED_SHOOTING(PLAYER_PED_ID()))
		{
			GTAentity targEntity = World::EntityFromAimCamRay();

			if (targEntity.Handle())
			{
				sub::Spooner::SpoonerEntity ent;
				ent.Handle = targEntity;
				sub::Spooner::EntityManagement::DeleteEntity(ent);
			}
		}
	}
}

void SetSelfResurrectionGun()
{
	if (g_myWeap == WEAPON_STUNGUN)
	{
		GTAentity myPed = PLAYER_PED_ID();
		if (IS_PED_SHOOTING(myPed.Handle()))
		{
			GTAped targPed = World::EntityFromAimCamRay();

			if (targPed.IsPed() && targPed.IsDead())
			{
				targPed.RequestControl();
				targPed.Health_set(200);
				RESURRECT_PED(targPed.Handle());
				REVIVE_INJURED_PED(targPed.Handle());
				targPed.MaxHealth_set(400);
				targPed.Health_set(200);
				SET_PED_GENERATES_DEAD_BODY_EVENTS(targPed.Handle(), false);
				SET_PED_CONFIG_FLAG(targPed.Handle(), ePedConfigFlags::IsInjured, 0);
				SET_PED_CONFIG_FLAG(targPed.Handle(), ePedConfigFlags::HasHurtStarted, 0);

				targPed.Task().ClearAllImmediately();
				TaskSequence seq;
				seq.AddTask().PlayAnimation("get_up@directional@movement@from_knees@standard", rand() % 3 == 1 ? "getup_r_90" : "getup_l_90", 8.0f, -8.0f, -1, AnimFlag::SecondTask, 0, false);
				seq.AddTask().WanderAround();
				seq.Close();
				seq.MakePedPerform(targPed);
				seq.Clear();
			}
		}
	}
}

void SetHVSnipers(bool set)
{
	if (set && g_myWeap == WEAPON_UNARMED)
	{
		return;
	}
	SET_SEETHROUGH(set);
}

void SetTeleportGun()
{
	GTAentity myPed = PLAYER_PED_ID();
	Hash weap;
	GET_CURRENT_PED_WEAPON(myPed.Handle(), &weap, true);
	if (weap == WEAPON_HEAVYPISTOL)
	{
		GTAentity ent = IS_PED_IN_ANY_VEHICLE(myPed.Handle(), false) ? g_myVeh : myPed;
		Vector3 targetPos;
		const Vector3& camPos = GameplayCamera::GetPosition();
		const Vector3& camDir = GameplayCamera::GetDirectionFromScreenCentre();
		auto ray = RaycastResult::Raycast(camPos, camDir, 15000.0f, IntersectOptions::Everything, myPed);
		if (ray.DidHitAnything())
		{
			if (ray.DidHitEntity())
			{
				const GTAentity& hitEntity = ray.HitEntity();
				if (hitEntity.IsVehicle() || !hitEntity.MissionEntity_get())
				{
					targetPos = hitEntity.GetPosition() + Vector3(0, 0, hitEntity.Dim2().z + ent.Dim1().z);
				}
				else
				{
					targetPos = ray.HitCoords();
				}
			}
			else 
			{
				targetPos = ray.HitCoords();	
			}

			if (!targetPos.IsZero())
			{
				ent.RequestControl();
				ent.SetPosition(targetPos);
				Game::Sound::PlayFrontend("Knuckle_Crack_Hard_Cel", "MP_SNACKS_SOUNDSET");
				ANIMPOSTFX_PLAY("ExplosionJosh3", 0, 0);
			}
		}
	}
}

void SetBulletGun()
{
	GTAentity playerPed = PLAYER_PED_ID();

	GTAentity gunObj = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed.Handle(), 0);
	const Vector3& camDir = GameplayCamera::GetDirectionFromScreenCentre();
	const Vector3& camPos = GameplayCamera::GetPosition();
	const Vector3& launchPos = camPos + (camDir * (camPos.DistanceTo(gunObj.GetPosition()) + 0.4f));
	const Vector3& targPos = camPos + (camDir * 200.0f);

	CLEAR_AREA_OF_PROJECTILES(launchPos.x, launchPos.y, launchPos.z, 6.0f, 0);
	World::ShootBullet(launchPos, targPos, playerPed, bullet_gun_hash, 5, 2500.0f, true, true);
}

void SetPedGun()
{
	if (pedGunRandBit) 
	{ 
		pedGunHash = GET_HASH_KEY(g_pedModels[rand() % (INT)g_pedModels.size()].first); pedGunHash.Load(200); 
	}

	if (pedGunHash.IsLoaded())
	{
		GTAentity myPed = PLAYER_PED_ID();
		const Vector3& launchPos = GetCoordsFromCam(GameplayCamera::GetPosition().DistanceTo(myPed.GetPosition()) + pedGunHash.Dim2().y + 0.5f);
		const Vector3& Rot = GameplayCamera::GetRotation();

		GTAentity spawnedPed = CREATE_PED(PedType::Human, pedGunHash.hash, launchPos.x, launchPos.y, launchPos.z, Rot.z, 1, 1);
		spawnedPed.SetRotation(Rot);
		spawnedPed.ApplyForceRelative(Vector3(0, 300.0f, 0), Vector3(0, -0.65f, 0), ForceType::MaxForceRot);
		spawnedPed.NoLongerNeeded();
	}
	if (pedGunRandBit)
	{
		pedGunHash.Unload();
	}
}

void SetObjectGun()
{
	Entity tempPed = PLAYER_PED_ID();
	Entity tempEntity;

	if (objectGunRandBitO)
	{ 
		objectGunHash = GET_HASH_KEY(objectModels[rand() % (INT)objectModels.size()]);
	}
	else if (objectGunRandBitV)
	{ 
		objectGunHash = g_vehHashes[rand() % (INT)g_vehHashes.size()];
	}

	if (objectGunHash.IsInCdImage())
	{
		if (objectGunRandBitO || objectGunRandBitV)
		{
			objectGunHash.Load(160);
		}

		const Vector3& launchPos = GetCoordsFromCam(GameplayCamera::GetPosition().DistanceTo(GET_ENTITY_COORDS(tempPed, 1)) + objectGunHash.Dim2().y + 1.355f);
		const Vector3& Rot = GET_GAMEPLAY_CAM_ROT(2);

		if (objectGunHash.IsVehicle())
		{
			tempEntity = CREATE_VEHICLE(objectGunHash.hash, launchPos.x, launchPos.y, launchPos.z, GET_ENTITY_HEADING(tempPed), 1, 1, 0);
		}
		else 
		{
			tempEntity = CREATE_OBJECT(objectGunHash.hash, launchPos.x, launchPos.y, launchPos.z, 1, 1, 1);
		}

		SET_ENTITY_ROTATION(tempEntity, Rot.x, Rot.y, Rot.z, 2, 1);
		APPLY_FORCE_TO_ENTITY(tempEntity, 1, 0.0f, 350.0f, 0.0f, 0.0f, -0.65f, 0.0f, 0, 1, 1, 1, 0, 1);

		if (objectGunRandBitO || objectGunRandBitV) 
		{
			objectGunHash.Unload();
		}

		if (DOES_ENTITY_EXIST(tempEntity))
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&tempEntity);
		}
	}
}

void SetLightGun()
{
	GTAentity myPed = PLAYER_PED_ID();

	if (IS_PED_SHOOTING(myPed.Handle()))
	{
		const auto& colour = RgbS::Random();

		Game::Sound::GameSound soundEffect("EPSILONISM_04_SOUNDSET", "IDLE_BEEP");
		soundEffect.Play(myPed);

		const Vector3& camPos = GameplayCamera::GetPosition();
		const Vector3& camDir = GameplayCamera::GetDirectionFromScreenCentre();
		float launchDist;

		GTAentity myWeaponEntity = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(myPed.Handle(), 0);
		if (myWeaponEntity.Exists())
		{
			launchDist = camPos.DistanceTo(myWeaponEntity.GetPosition()) + 0.26f;
		}
		else
		{
			launchDist = camPos.DistanceTo(myPed.GetPosition()) + 0.44f;
		}

		for (float i = launchDist; i < 260.0f; i += 0.02f)
		{
			World::DrawLightWithRange(camPos + (camDir * i), colour, 1.0f, 3.0f);
		}
	}
}

void SetTripleBullets()
{
	if (GET_WEAPONTYPE_GROUP(g_myWeap) == WeaponGroupHash::Melee)
	{
		return;
	}

	Player playerPed = PLAYER_PED_ID();
	GTAentity gunObj = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(playerPed, 0);
	const Vector3& launchPos = gunObj.GetOffsetInWorldCoords(0, gunObj.Dim1().y, 0);
	Vector3 targPos[]
	{
		{ -2, 2, 2 },
		{ -1.5, 1, 1 },
		{ -1, 0, 0 },
		{ 1, 0, 0 },
		{ 1.5, 1, 1 },
		{ 2, 2, 2 }
	};

	CLEAR_AREA_OF_PROJECTILES(launchPos.x, launchPos.y, launchPos.z, 6.0f, 0);
	float maxDist = GET_MAX_RANGE_OF_CURRENT_PED_WEAPON(playerPed);

	for (auto& pos : targPos)
	{
		World::ShootBullet(launchPos, GameplayCamera::RaycastForCoord(Vector2(0.0f, 0.0f), playerPed, maxDist, maxDist) + pos, playerPed, g_myWeap, 5, 0xbf800000, true, true);
	}

}

void SetForgeGunDist(float& distance)
{
	DISABLE_CONTROL_ACTION(2, INPUT_LOOK_BEHIND, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_WEAPON_WHEEL_NEXT, TRUE);
	DISABLE_CONTROL_ACTION(2, INPUT_WEAPON_WHEEL_PREV, TRUE);
	if (Menu::bitController)
	{
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RS)) 
		{
			distance += 0.166f;
		}
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LS))
		{
			if (distance > 3.0f) 
			{
				distance -= 0.166f;
			}

			auto ped = PLAYER_PED_ID();
			if (GET_PED_STEALTH_MOVEMENT(ped)) 
			{
				SET_PED_STEALTH_MOVEMENT(ped, 0, 0);
			}
			if (GET_PED_COMBAT_MOVEMENT(ped)) 
			{
				SET_PED_COMBAT_MOVEMENT(ped, 0);
			}
		}
	}
	else
	{
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP)) 
		{
			distance += 0.32f;
		}
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
		{
			if (distance > 3.0f) distance -= 0.32f;
		}
	}

	ENABLE_CONTROL_ACTION(2, INPUT_WEAPON_WHEEL_NEXT, TRUE);
	ENABLE_CONTROL_ACTION(2, INPUT_WEAPON_WHEEL_PREV, TRUE);
}

inline void SetForgeGunRotationHotKeys()
{
	Vector3 Rot = GET_ENTITY_ROTATION(targetSlotEntity, 2);
	FLOAT& precision = g_forgeGunPrecision;

	if (!Menu::bitController)
	{
		if (IsKeyDown(VK_OEM_4)) 
		{
			Rot.x -= precision;
		}
		if (IsKeyDown(VK_OEM_6)) 
		{
			Rot.x += precision;
		}
		if (IsKeyDown(VK_OEM_1)) 
		{
			Rot.y -= precision;
		}
		if (IsKeyDown(VK_OEM_7)) 
		{
			Rot.y += precision;
		}
		if (IsKeyDown(VK_OEM_COMMA)) 
		{
			Rot.z -= precision;
		}
		if (IsKeyDown(VK_OEM_PERIOD)) 
		{
			Rot.z += precision;
		}

	}
	else
	{
		DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_WHEEL_NEXT, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_WHEEL_PREV, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_WHEEL_LR, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_MAP, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_VEH_SELECT_NEXT_WEAPON, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_VEH_FLY_SELECT_NEXT_WEAPON, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_FRANKLIN, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MICHAEL, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_TREVOR, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, TRUE);
		DISABLE_CONTROL_ACTION(0, INPUT_CHARACTER_WHEEL, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_CANCEL, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_SELECT, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_UP, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_DOWN, TRUE);


		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_DOWN)) 
		{
			Rot.x -= precision;
		}
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_UP)) 
		{
			Rot.x += precision;
		}
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LEFT)) 
		{
			Rot.y -= precision;
		}
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RIGHT)) 
		{
			Rot.y += precision;
		}
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LB)) 
		{
			Rot.z -= precision;
		}
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RB)) 
		{
			Rot.z += precision;
		}
	}

	SET_ENTITY_ROTATION(targetSlotEntity, Rot.x, Rot.y, Rot.z, 2, 1);
}

void SetForgeGun()
{
	Ped tempPed = PLAYER_ID();
	if (g_myWeap == WEAPON_PISTOL && (IS_PLAYER_FREE_AIMING(tempPed) || IS_PLAYER_TARGETTING_ANYTHING(tempPed)))
	{
		if (!DOES_ENTITY_EXIST(targetSlotEntity) || bitGravityGunDisabled)
		{
			return;
		}

		if (forgeDist == 0) forgeDist = GameplayCamera::GetPosition().DistanceTo(GET_ENTITY_COORDS(targetSlotEntity, 1));
		SetForgeGunDist(forgeDist);
		SetForgeGunRotationHotKeys();

		Vector3 Coord, dim2;
		dim2 = GTAentity(targetSlotEntity).Dim2();
		float heading = GET_ENTITY_HEADING(targetSlotEntity);
		float playerHeading = GET_ENTITY_HEADING(PLAYER_PED_ID());
		if (heading > (playerHeading - 50) && heading < (playerHeading + 50))
		{
			Coord = GetCoordsFromCam(forgeDist + abs(dim2.y));
		}
		else
		{
			Coord = GetCoordsFromCam(forgeDist + abs(dim2.x));
		}

		GTAentity(targetSlotEntity).RequestControl();
		SET_ENTITY_COORDS_NO_OFFSET(targetSlotEntity, Coord.x, Coord.y, Coord.z, 0, 0, 0);
		FREEZE_ENTITY_POSITION(targetSlotEntity, objectSpawnForgeAssistance);

		if (IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_ATTACK))
		{
			FREEZE_ENTITY_POSITION(targetSlotEntity, 0);
			PLAY_SOUND_FROM_ENTITY(-1, "Foot_Swish", targetSlotEntity, "docks_heist_finale_2a_sounds", 0, 0);
			PLAY_SOUND_FROM_ENTITY(-1, "SUSPENSION_SCRIPT_FORCE", targetSlotEntity, 0, 0, 0);
			PLAY_SOUND_FROM_ENTITY(-1, "Chopper_Destroyed", PLAYER_PED_ID(), "FBI_HEIST_FIGHT_CHOPPER_SOUNDS", 0, 0);
			dim2 = GET_GAMEPLAY_CAM_ROT(2);
			SET_ENTITY_ROTATION(targetSlotEntity, dim2.x, dim2.y, dim2.z, 2, 1);
			APPLY_FORCE_TO_ENTITY(targetSlotEntity, 1, 0.0f, g_forgeGunShootForce, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);
			PTFX::TriggerPTFX("scr_carsteal4", "scr_carsteal4_wheel_burnout", 0, Coord, Vector3(), 0.66f);

			targetSlotEntity = 0;
			forgeDist = 0;
			bitGravityGunDisabled = true;
			targetEntityLocked = false;
		}
	}
	else
	{
		forgeDist = 0;
		bitGravityGunDisabled = false;
	}

}

void SetExplosionAtBulletHit(Ped ped, Hash type, bool invisible)
{
	Vector3_t Pos;
	if (!GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &Pos) && ped == PLAYER_PED_ID())
	{
		const Vector3& camDir = GameplayCamera::Direction_get();
		const Vector3& camCoord = GameplayCamera::GetPosition();
		Vector3 hitCoord = (camDir * 1000.0f) + camCoord;

		RaycastResult ray = RaycastResult::Raycast(camCoord, hitCoord, IntersectOptions::Everything);
		if (ray.DidHitAnything())
		{
			Pos = hitCoord.ToTypeStruct();
		}
		else
		{
			hitCoord = (camDir * 100.0f) + camCoord;
			Pos = hitCoord.ToTypeStruct();
		}
	}


	if (kaboomGun && ped == PLAYER_PED_ID() && kaboomGunRandBit)
	{
		ADD_EXPLOSION(Pos.x, Pos.y, Pos.z, GET_RANDOM_INT_IN_RANGE(0, 40), 5.0, 1, invisible, 0.5, false);
	}
	else if (type < 70)
	{
		ADD_EXPLOSION(Pos.x, Pos.y, Pos.z, type, 5.0, 1, invisible, 0.5, false);
	}
	else
	{
		Entity ent;
		if (IS_MODEL_A_VEHICLE(type))
		{
			ent = CREATE_VEHICLE(type, Pos.x, Pos.y, Pos.z + 0.16f, GET_ENTITY_HEADING(ped), 1, 1, 0);
		}
		else
		{
			ent = CREATE_PED(PedType::Human, type, Pos.x, Pos.y, Pos.z + 0.16f, GET_ENTITY_HEADING(ped), 1, 1);
		}

		SET_ENTITY_AS_NO_LONGER_NEEDED(&ent);
	}
}

void SetTriggerFXAtBulletHit(Ped ped, const std::string& fxAsset, const std::string& fxName, const Vector3& Rot, float scale)
{
	Vector3_t Pos;
	if (!GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &Pos) && ped == PLAYER_PED_ID())
	{
		const Vector3& camDir = GameplayCamera::Direction_get();
		const Vector3& camCoord = GameplayCamera::GetPosition();
		Vector3 hitCoord = (camDir * 1000.0f) + camCoord;

		RaycastResult ray = RaycastResult::Raycast(camCoord, hitCoord, IntersectOptions::Everything);
		if (ray.DidHitAnything())
		{
			Pos = hitCoord.ToTypeStruct();
		}
		else
		{
			hitCoord = (camDir * 100.0f) + camCoord;
			Pos = hitCoord.ToTypeStruct();
		}
	}

	PTFX::TriggerPTFX(fxAsset, fxName, 0, Pos, Rot, scale);
}

void SetBecomePed(GTAped ped)
{
	GTAped oldPed = PLAYER_PED_ID();

	std::vector<s_Weapon_Components_Tint> weaponsBackup;
	oldPed.StoreWeaponsInArray(weaponsBackup);

	GTAvehicle vehicle(0);
	VehicleSeat currVehSeat;
	if (ped.IsInVehicle())
	{
		vehicle = ped.CurrentVehicle();
		currVehSeat = ped.GetCurrentVehicleSeat();
	}

	float camPitchRelative = GameplayCamera::GetRelativePitch();

	if (ped.IsPlayer())
	{
		return;
	}

	ped.RequestControl();
	SetPedInvincibleOff(oldPed.Handle());
	CHANGE_PLAYER_PED(PLAYER_ID(), ped.Handle(), true, true);

	GameplayCamera::RelativeHeading_set(0.0f);
	GameplayCamera::RelativePitch_set(camPitchRelative);

	ped = PLAYER_PED_ID();

	if (vehicle.Exists())
	{
		vehicle.RequestControl();
		ped.SetIntoVehicle(vehicle, currVehSeat);
	}

	ped.GiveWeaponsFromArray(weaponsBackup);

	SET_PED_INFINITE_AMMO_CLIP(ped.Handle(), bitInfiniteAmmo);

}

void SetPedInvincibleOn(Ped ped)
{
	SET_ENTITY_INVINCIBLE(ped, 1);
	SET_PED_DIES_IN_WATER(ped, 0);
	SET_ENTITY_PROOFS(ped, 1, 0, 1, 1, 1, 1, 1, 1);
}
void SetPedInvincibleOff(Ped ped)
{
	SET_ENTITY_INVINCIBLE(ped, 0);
	SET_PED_DIES_IN_WATER(ped, 1);
	SET_ENTITY_PROOFS(ped, 0, 0, 0, 0, 0, 0, 0, 0);
}
void SetPedNoRagdollOn(Ped ped)
{
	SET_PED_CAN_RAGDOLL(ped, 2);
	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, 2);
}
void SetPedNoRagdollOff(Ped ped)
{
	SET_PED_CAN_RAGDOLL(ped, 0);
	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, 0);
}
void SetPedSeatbeltOn(Ped ped)
{
	SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 1); // state cantFollOff
	SET_PED_CONFIG_FLAG(ped, ePedConfigFlags::WillFlyThruWindscreen, false);
}
void SetPedSeatbeltOff(Ped ped)
{
	SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 0); // state canFallOff
	SET_PED_CONFIG_FLAG(ped, ePedConfigFlags::WillFlyThruWindscreen, true);
}

bool bitNoclipAlreadyInvisible = true;
bool bitNoclipAlreadyCollision = true;
bool bitNoclipShowHelp = true;
Camera g_cam_noClip;
bool g_freecamHeightLocked = false;  
float g_freecamLockedHeight = 0.0f;
float g_freecamSpeed = MenuConfig::FreeCam::defaultSpeed; // Modify the default value

void SetNoclipOff1()
{
	GTAentity myPed = PLAYER_PED_ID();
	GTAentity ent = IS_PED_IN_ANY_VEHICLE(myPed.Handle(), false) ? GET_VEHICLE_PED_IS_IN(myPed.Handle(), false) : myPed;

	ent.RequestControl();
	ent.SetVisible(!bitNoclipAlreadyInvisible);
	ent.SetIsCollisionEnabled(bitNoclipAlreadyCollision);
	ent.FreezePosition(false);
	ENABLE_CONTROL_ACTION(2, INPUT_VEH_HORN, TRUE);
	ENABLE_CONTROL_ACTION(2, INPUT_LOOK_BEHIND, TRUE);
	ENABLE_CONTROL_ACTION(2, INPUT_VEH_LOOK_BEHIND, TRUE);
	ENABLE_CONTROL_ACTION(2, INPUT_SELECT_WEAPON, TRUE);
	bitNoclipShowHelp = true;
}
void SetNoclipOff2()
{
	auto& cam = g_cam_noClip;
	if (cam.Exists())
	{
		cam.SetActive(false);
		cam.Destroy();
		World::RenderingCamera_set(0);
	}
}
void SetNoclip()
{
	if (sub::Spooner::SpoonerMode::bEnabled)
	{
		return;
	}

	auto& cam = g_cam_noClip;
	GTAentity myPed = PLAYER_PED_ID();
	GTAplayer myPlayer = PLAYER_ID();
	GTAentity ent = IS_PED_IN_ANY_VEHICLE(myPed.Handle(), false) ? GET_VEHICLE_PED_IS_IN(myPed.Handle(), false) : myPed;

	if (ent.Exists())
	{
		if (Menu::bitController ? (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_X) && IS_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LS)) : IsKeyJustUp(BindNoClip))
		{
			noClipToggle = !noClipToggle;
			if (!noClipToggle)
			{
				SetNoclipOff1();
			}
			else
			{
				if (bitNoclipShowHelp)
				{
					bitNoclipShowHelp = false;
					if (Menu::bitController)
					{
						Game::CustomHelpText::ShowTimedText(oss_ << "FreeCam:~n~~INPUT_MOVE_UD~ = " << Game::GetGXTEntry("ITEM_MOV_CAM")
							<< "~n~~INPUT_LOOK_LR~ = " << Game::GetGXTEntry("ITEM_MOVE") << "~n~~INPUT_FRONTEND_RT~/~INPUT_FRONTEND_LT~ = " << "Ascend/Descend" << "~n~~INPUT_FRONTEND_RB~ = " << "Hasten", 6000);
					}
					else 
					{
						Game::CustomHelpText::ShowTimedText(oss_ << "FreeCam:~n~~INPUT_MOVE_UD~/~INPUT_MOVE_LR~ = " << Game::GetGXTEntry("ITEM_MOV_CAM")
							<< "~n~~INPUT_LOOK_LR~ = " << Game::GetGXTEntry("ITEM_MOVE") << "~n~~INPUT_PARACHUTE_BRAKE_RIGHT~/~INPUT_PARACHUTE_BRAKE_LEFT~ = " << "Ascend/Descend" << "~n~~INPUT_SPRINT~ = " << "Hasten", 6000);
					}
					bitNoclipShowHelp = false;
				}
				bitNoclipAlreadyInvisible = !ent.IsVisible();
				bitNoclipAlreadyCollision = ent.GetIsCollisionEnabled();
			}
		}

		if (!noClipToggle)
		{
			SetNoclipOff2();
			return;
		}

		DISABLE_CONTROL_ACTION(2, INPUT_VEH_HORN, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_LOOK_BEHIND, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_VEH_LOOK_BEHIND, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_SELECT_WEAPON, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_VEH_ACCELERATE, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_VEH_BRAKE, TRUE);
		DISABLE_CONTROL_ACTION(2, INPUT_VEH_RADIO_WHEEL, TRUE);

		const Vector3& entPos = ent.GetPosition();
		const Vector3& camOffset = Vector3();

		if (!cam.Exists())
		{
			ent.RequestControl();
			cam = World::CreateCamera();
			cam.SetPosition(GameplayCamera::GetPosition());
			cam.SetRotation(GameplayCamera::GetRotation());
			cam.AttachTo(ent, camOffset);
			cam.SetFieldOfView(MenuConfig::FreeCam::defaultFov); // Use configured FOV
			cam.DepthOfFieldStrength_set(0.0f);
			World::RenderingCamera_set(cam);
		}

		ent.RequestControl();
		ent.FreezePosition(true);
		ent.SetIsCollisionEnabled(false);
		ent.SetVisible(false);
		myPed.SetVisible(false);

		Vector3 nextRot = cam.Rotation_get() - Vector3(GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_UD), 0, GET_DISABLED_CONTROL_NORMAL(0, INPUT_LOOK_LR)) * (Menu::bitController ? 2.5f : 11.0f);
		nextRot.y = 0.0f; // No roll
		ent.SetRotation(Vector3(0, 0, nextRot.z));
		cam.SetRotation(nextRot);
		if (!myPlayer.IsFreeAiming() && !myPlayer.IsTargetingAnything())
		{
			SET_GAMEPLAY_CAM_RELATIVE_HEADING(0.0f);
		}

		if (Menu::bitController)
		{
			DISABLE_CONTROL_ACTION(0, INPUT_VEH_HORN, TRUE);

			if (ent == myPed)
			{
				if (GET_PED_STEALTH_MOVEMENT(myPed.Handle()))
				{
					SET_PED_STEALTH_MOVEMENT(myPed.Handle(), false, 0);
				}
				if (GET_PED_COMBAT_MOVEMENT(myPed.Handle()))
				{
					SET_PED_COMBAT_MOVEMENT(myPed.Handle(), 0);
				}
			}

			float noclipPrecisionLevel = IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_RB) ? 1.8f : 0.8f;
			Vector3 offset;
			offset.x = GET_CONTROL_NORMAL(0, INPUT_MOVE_LR) * noclipPrecisionLevel;
			offset.y = -GET_CONTROL_NORMAL(0, INPUT_MOVE_UD) * noclipPrecisionLevel;
			offset.z = (GET_DISABLED_CONTROL_NORMAL(2, INPUT_FRONTEND_RT) - GET_DISABLED_CONTROL_NORMAL(2, INPUT_FRONTEND_LT)) * noclipPrecisionLevel;
			if (!offset.IsZero())
			{
				ent.SetPosition(cam.GetOffsetInWorldCoords(offset - camOffset));
			}

		}
		else
		{
			// TAB to toggle height lock
			if (!IsKeyDown(VK_SPACE))
			{
				if (IsKeyJustUp(VK_TAB))
				{
					g_freecamHeightLocked = !g_freecamHeightLocked;
					if (g_freecamHeightLocked)
					{
						g_freecamLockedHeight = ent.GetPosition().z;
						g_lastHeightLockMessage = "Height Locked";
					}
					else
					{
						g_lastHeightLockMessage = "Height Unlocked";
					}
					g_lastHeightLockMessageTime = GetTickCount();
				}

				// Mouse wheel to adjust speed
				if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
				{
					g_freecamSpeed = min(g_freecamSpeed + MenuConfig::FreeCam::speedAdjustStep, MenuConfig::FreeCam::maxSpeed);
					MenuConfig::FreeCam::defaultSpeed = g_freecamSpeed;
					MenuConfig::SaveConfig();
					g_lastSpeedValue = g_freecamSpeed;
					g_lastSpeedDisplayTime = GetTickCount();
				}
				if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
				{
					g_freecamSpeed = max(g_freecamSpeed - MenuConfig::FreeCam::speedAdjustStep, MenuConfig::FreeCam::minSpeed);
					MenuConfig::FreeCam::defaultSpeed = g_freecamSpeed;
					MenuConfig::SaveConfig();
					g_lastSpeedValue = g_freecamSpeed;
					g_lastSpeedDisplayTime = GetTickCount();
				}

				if (GetTickCount() - g_lastSpeedDisplayTime < 1000)
				{
					Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.4f, 0.4f), true, false, false);
					Game::Print::DrawString(oss_ << "FreeCam Speed: " << g_lastSpeedValue, 0.5f, 0.95f);
				}
			}

			float currentSpeed = IS_DISABLED_CONTROL_PRESSED(2, INPUT_VEH_ATTACK2) ? MenuConfig::FreeCam::defaultSlowSpeed : g_freecamSpeed;
			float noclipPrecisionLevel = IS_DISABLED_CONTROL_PRESSED(0, INPUT_SPRINT) ? currentSpeed * 2.0f : currentSpeed;

			Vector3 offset;
			offset.x = GET_CONTROL_NORMAL(0, INPUT_MOVE_LR) * noclipPrecisionLevel;
			offset.y = -GET_CONTROL_NORMAL(0, INPUT_MOVE_UD) * noclipPrecisionLevel;

			if (g_freecamHeightLocked)
			{
				float zOffset = IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_RIGHT) ? noclipPrecisionLevel : IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_LEFT) ? -noclipPrecisionLevel : 0.0f;
				if (zOffset != 0.0f)
				{
					g_freecamLockedHeight += zOffset;
				}

				Vector3 newPos = cam.GetOffsetInWorldCoords(offset - camOffset);
				newPos.z = g_freecamLockedHeight;
				ent.SetPosition(newPos);
			}
			else
			{
				offset.z = IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_RIGHT) ? noclipPrecisionLevel : IS_DISABLED_CONTROL_PRESSED(2, INPUT_PARACHUTE_BRAKE_LEFT) ? -noclipPrecisionLevel : 0.0f;
				if (!offset.IsZero())
				{
					ent.SetPosition(cam.GetOffsetInWorldCoords(offset - camOffset));
				}
			}

			// Space + scroll wheel to control camera FOV
			if (IsKeyDown(VK_SPACE))
			{
				float currentFov = cam.FieldOfView_get();
				if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_UP))
				{
					currentFov = min(currentFov + MenuConfig::FreeCam::fovAdjustStep, MenuConfig::FreeCam::maxFov);
					cam.SetFieldOfView(currentFov);
					MenuConfig::FreeCam::defaultFov = currentFov;
					MenuConfig::SaveConfig();
					g_lastFOVValue = currentFov;
					g_lastFOVDisplayTime = GetTickCount();
				}
				if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_CURSOR_SCROLL_DOWN))
				{
					currentFov = max(currentFov - MenuConfig::FreeCam::fovAdjustStep, MenuConfig::FreeCam::minFov);
					cam.SetFieldOfView(currentFov);
					MenuConfig::FreeCam::defaultFov = currentFov;
					MenuConfig::SaveConfig();
					g_lastFOVValue = currentFov;
					g_lastFOVDisplayTime = GetTickCount();
				}

				if (GetTickCount() - g_lastFOVDisplayTime < 1000)
				{
					Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.4f, 0.4f), true, false, false);
					Game::Print::DrawString(oss_ << "Camera FOV: " << g_lastFOVValue, 0.5f, 0.95f);
				}
			}
		}
	}

	// Height lock status display
	if (g_lastHeightLockMessage != nullptr && GetTickCount() - g_lastHeightLockMessageTime < 1000)
	{
		Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.4f, 0.4f), true, false, false);
		Game::Print::drawstring(g_lastHeightLockMessage, 0.5f, 0.95f);
	}
}

void SetLocalButtonSuperRun()
{
	auto ped = PLAYER_PED_ID();
	bool isInAir = IS_ENTITY_IN_AIR(ped) != 0;

	if (!isInAir)
	{
		if (IS_CONTROL_PRESSED(0, INPUT_SPRINT))
		{
			APPLY_FORCE_TO_ENTITY(ped, 1, 0.0, 3.4, 0.0, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
		}

		else if (IS_CONTROL_JUST_RELEASED(0, INPUT_SPRINT))
		{
			FREEZE_ENTITY_POSITION(ped, 1);
			FREEZE_ENTITY_POSITION(ped, 0);
		}
	}

}

void SetSelfRefillHealthWhenInCover()
{
	if (GET_GAME_TIMER() >= Menu::delayedTimer - 100)
	{
		GTAped playerPed = PLAYER_PED_ID();
		auto health = playerPed.GetHealth();
		auto maxHealth = playerPed.GetMaxHealth();
		if (playerPed.IsInCover() && !playerPed.IsAimingFromCover() && health < maxHealth)
		{
			playerPed.Health_set(health + 4);
		}
	}
}

void xyzhDrawFloat(float text, float x_coord, float y_coord)
{
	Game::Print::SetupDraw(font_xyzh, Vector2(0.35f, 0.35f), false, false, true);
	Game::Print::drawfloat(text, 4, x_coord, y_coord);
}
void XYZH()
{
	Vector3 Pos = GET_ENTITY_COORDS(PLAYER_PED_ID(), 1);

	Game::Print::SetupDraw(font_xyzh, Vector2(0.35f, 0.36f), false, false, true);
	Game::Print::drawstring("Coords:", 0.90f, 0.066f);
	xyzhDrawFloat(Pos.x, 0.90f, 0.10f);
	xyzhDrawFloat(Pos.y, 0.90f, 0.13f);
	xyzhDrawFloat(Pos.z, 0.90f, 0.16f);
	xyzhDrawFloat(GET_ENTITY_HEADING(PLAYER_PED_ID()), 0.90f, 0.19f);
}

void SetLocalSupermanManual()
{
	if (IS_PAUSE_MENU_ACTIVE() || IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0))
	{
		return;
	}

	DISABLE_CONTROL_ACTION(2, INPUT_PARACHUTE_DEPLOY, TRUE);

	Ped playerPed = PLAYER_PED_ID();
	bool isInParaFreeFall = IS_PED_IN_PARACHUTE_FREE_FALL(playerPed) != 0;

	if (isInParaFreeFall)
	{
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RB) || get_key_pressed(VK_ADD))
		{
			APPLY_FORCE_TO_ENTITY(playerPed, 1, 0.0, 45.0, 0.0, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
		}

		DISABLE_CONTROL_ACTION(2, INPUT_PARACHUTE_DEPLOY, TRUE);
		if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RDOWN) || get_key_pressed(VK_SUBTRACT))
		{
			FREEZE_ENTITY_POSITION(playerPed, true);
		}
		else if (IS_CONTROL_JUST_RELEASED(2, INPUT_FRONTEND_RDOWN) || IsKeyJustUp(VK_SUBTRACT))
		{
			FREEZE_ENTITY_POSITION(playerPed, false);
		}
	}

	if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT) || IsKeyDown(VK_NUMPAD7))
	{
		if (!isInParaFreeFall) 
		{
			TASK_PARACHUTE(playerPed, true, false);
		}
		APPLY_FORCE_TO_ENTITY(playerPed, 1, 0.0, 0.0, 13.0, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
	}

	if (IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LT) || IsKeyDown(VK_NUMPAD1))
	{
		if (!isInParaFreeFall) 
		{
			TASK_PARACHUTE(playerPed, true, false);
		}
		APPLY_FORCE_TO_ENTITY(playerPed, 1, 0.0, 0.0, -13.0, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
	}

}
void SetPedSupermanAuto(Ped ped)
{
	if (IS_PED_IN_PARACHUTE_FREE_FALL(ped))
	{
		if (!NETWORK_HAS_CONTROL_OF_ENTITY(ped))
		{
			NETWORK_REQUEST_CONTROL_OF_ENTITY(ped);
		}

		APPLY_FORCE_TO_ENTITY(ped, 1, 0.0, 70.0f, 70.0f, 0.0, 0.0, 0.0, 0, 1, 0, 0, 0, 1);

		if (ped == PLAYER_PED_ID())
		{
			bool isBrakePressed, isBrakeReleased = false;
			if (Menu::bitController)
			{
				DISABLE_CONTROL_ACTION(2, INPUT_PARACHUTE_DEPLOY, TRUE);
				isBrakePressed = IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RDOWN) != 0;
				if (!isBrakePressed) 
				{
					isBrakeReleased = IS_CONTROL_JUST_RELEASED(2, INPUT_FRONTEND_RDOWN) != 0;
				}
			}
			else
			{
				isBrakePressed = IsKeyDown(VK_ADD);
				if (!isBrakePressed) 
				{
					isBrakeReleased = IsKeyJustUp(VK_ADD);
				}
			}
			if (isBrakePressed)
			{
				FREEZE_ENTITY_POSITION(ped, true);
			}
			else if (isBrakeReleased)
			{
				FREEZE_ENTITY_POSITION(ped, false);
			}
		}

	}
}

void SetVehicleNosPTFXThisFrame(GTAvehicle vehicle)
{
	WeaponS ptfx1 = { "scr_rcbarry1", "scr_alien_teleport" };
	if (!HAS_NAMED_PTFX_ASSET_LOADED(ptfx1.label.c_str()))
	{
		REQUEST_NAMED_PTFX_ASSET(ptfx1.label.c_str());
	}
	else
	{
		Vector3 dim1, dim2;
		vehicle.ModelDimensions(dim1, dim2);
		USE_PARTICLE_FX_ASSET(ptfx1.label.c_str());
		SET_PARTICLE_FX_NON_LOOPED_COLOUR(float(titlebox.R) / 255.0f, float(titlebox.G) / 255.0f, float(titlebox.B) / 255.0f);
		START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_ENTITY(ptfx1.name.c_str(), vehicle.Handle(), dim1.x - 0.2f, 0.4 - dim2.y, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0, 0, 0);
		USE_PARTICLE_FX_ASSET(ptfx1.label.c_str());
		SET_PARTICLE_FX_NON_LOOPED_COLOUR(float(titlebox.R) / 255.0f, float(titlebox.G) / 255.0f, float(titlebox.B) / 255.0f);
		START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_ENTITY(ptfx1.name.c_str(), vehicle.Handle(), 0.2f - dim2.x, 0.4 - dim2.y, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0, 0, 0);
	}

	PTFX::NonLoopedPTFX muzzleFlash("scr_carsteal4", "scr_carsteal5_car_muzzle_flash");
	if (!muzzleFlash.IsAssetLoaded())
	{
		muzzleFlash.LoadAsset();
	}
	else
	{
		const Vector3& otherWayRot = vehicle.Rotation_get() + Vector3(0, 0, -90.0f);
		for (auto& exh : { VBone::exhaust, VBone::exhaust_2 })
		{
			muzzleFlash.Start(vehicle.GetBoneCoords(vehicle.GetBoneIndex(exh)), 1.0f, otherWayRot);
		}
	}
}

void SetSuperCarModeSelf()
{
	if (IS_VEHICLE_ON_ALL_WHEELS(g_myVeh))
	{
		if (IS_CONTROL_PRESSED(2, INPUT_VEH_ACCELERATE))
		{
			SET_VEHICLE_BOOST_ACTIVE(g_myVeh, 1);
			SET_VEHICLE_BOOST_ACTIVE(g_myVeh, 0);
			SET_VEHICLE_FORWARD_SPEED(g_myVeh, GET_ENTITY_SPEED_VECTOR(g_myVeh, true).y + 0.46f);
			SetVehicleNosPTFXThisFrame(GTAvehicle(g_myVeh));
		}
		if (IS_CONTROL_JUST_PRESSED(2, INPUT_VEH_BRAKE))
		{
			SET_VEHICLE_FORWARD_SPEED(g_myVeh, 0.0f);
		}
		else if (IS_CONTROL_PRESSED(2, INPUT_VEH_BRAKE))
		{
			APPLY_FORCE_TO_ENTITY(g_myVeh, 1, 0.0, -0.4, 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
		}
	}
}

void SetLocalCarJump()
{
	if (!IS_MP_TEXT_CHAT_TYPING())
	{
		Model& model = g_myVehModel;

		if (model.IsPlane() || model.IsHeli())
		{
			return;
		}

		GTAvehicle vehicle = g_myVeh;
		bool bPressed;

		switch (carJump)
		{
		case 1: // Tap
			if (vehicle.Model().IsBicycle())
			{
				bPressed = IS_CONTROL_JUST_PRESSED(2, INPUT_VEH_JUMP);
			}
			else
			{
				bPressed = Menu::bitController ? IS_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RDOWN) : IS_CONTROL_JUST_PRESSED(2, INPUT_VEH_HANDBRAKE);
			}
			if (bPressed)
			{
				vehicle.ApplyForceCustom(Vector3(0, 0, 3.6f), Vector3(0, 1.2f, 0), ForceType::MaxForceRot, true, false, true, true, true, true);
				vehicle.ApplyForceCustom(Vector3(0, 0, 3.6f), Vector3(0, -1.0f, 0), ForceType::MaxForceRot, true, false, true, true, true, true);
			}
			break;
		case 2: // Hold
			if (vehicle.Model().IsBicycle())
			{
				bPressed = IS_CONTROL_PRESSED(2, INPUT_VEH_JUMP);
			}
			else
			{
				bPressed = Menu::bitController ? IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RDOWN) : IS_CONTROL_PRESSED(2, INPUT_VEH_HANDBRAKE);
			}
			if (bPressed)
			{
				vehicle.ApplyForceCustom(Vector3(0, 0, 0.6f), Vector3(0, 0.06f, 0), ForceType::MaxForceRot, true, false, true, true, true, true);
			}
			break;
		}
	}
}

void SetLocalCarHydraulics()
{
	GTAvehicle vehicle = g_myVeh;

	if ((Menu::bitController ? IS_DISABLED_CONTROL_PRESSED(2, INPUT_FRONTEND_LS) : get_key_pressed(VirtualKey::LeftShift)) && vehicle.IsOnAllWheels())
	{
		Vector2 normal;
		if (Menu::bitController)
		{
			DISABLE_CONTROL_ACTION(2, INPUT_VEH_HORN, true);
			normal.x = GET_CONTROL_NORMAL(2, INPUT_SCRIPT_LEFT_AXIS_X);
			normal.y = GET_CONTROL_NORMAL(2, INPUT_SCRIPT_LEFT_AXIS_Y);
		}
		else
		{
			if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_RIGHT) || IsKeyDown('D'))
			{
				normal.x = 1.0f;
			}
			else if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_LEFT) || IsKeyDown('A'))
			{
				normal.x = -1.0f;
			}
			else if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_UP) || IsKeyDown('W'))
			{
				normal.y = 1.0f;
			}
			else if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_DOWN) || IsKeyDown('S'))
			{
				normal.y = -1.0f;
			}
		}

		if (normal.x > 0.4f || normal.x < -0.4f)
		{
			vehicle.ApplyForceCustom(Vector3(0, 0, abs(normal.x) * 0.294f), Vector3((normal.x > 0 ? 1 : -1) * 0.98f, 0, 0), ForceType::MaxForceRot, true, true, true, true, false, true);
		}
		if (normal.y > 0.4f || normal.y < -0.4f)
		{
			vehicle.ApplyForceCustom(Vector3(0, 0, abs(normal.y) * 0.164f), Vector3(0, (normal.y > 0 ? 1 : -1) * 3.05f, 0), ForceType::MaxForceRot, true, true, true, true, false, true);
		}
	}
}
void SetLocalForcefield()
{
	GTAentity myPed = PLAYER_PED_ID();
	const Vector3& myPos = myPed.GetPosition();
	switch (forceField)
	{
	case 1: //push out
		for (GTAentity ent : worldEntities)
		{
			if (ent.Handle() != myPed.Handle() && ent.Handle() != g_myVeh)
			{
				const Vector3& entPos = ent.GetPosition();
				if (myPos.DistanceTo(entPos) < 10.0f)
				{
					ent.ApplyForce(entPos - myPos, ForceType::MaxForceRot2);
				}
			}
		}
		break;
	case 2://explode
		myPed.SetExplosionProof(true);
		World::AddExplosion(myPos, EXPLOSION::BLIMP, 5.0f, 0.0f, false, false);
		break;
	}
}

void SetExplosionWP(UINT8 mode)
{
	if (!IS_WAYPOINT_ACTIVE())
	{
		return;
	}

	float camshake = 0.0f;
	bool visible = true;

	switch (mode)
	{
	case 0: // off
		return;
		break;
	case 1: default: // visible + shaky
		camshake = 3.0f;
		break;
	case 2: // visible
		visible = true;
		break;
	case 3: // invisible
		visible = false;
		break;
	}

	Vector3 pos = GET_BLIP_INFO_ID_COORD(GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint));
	ADD_EXPLOSION(pos.x, pos.y, pos.z, 16, 36.0f, 1, !visible, camshake, false);
	GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, 600.0f, &pos.z, 0, 0);
	ADD_EXPLOSION(pos.x, pos.y, pos.z + 5.0f, 29, 36.0f, 1, !visible, camshake, false);
	ADD_EXPLOSION(pos.x, pos.y, pos.z + 20.0f, 29, 36.0f, 1, !visible, camshake, false);

}

float g_multiPlatNeonsIntensity = 3.5f;
void DrawVehicleAmbientLightNeons(const GTAvehicle& vehicle, const RgbS& colour)
{
	Vector3 dim1, dim2;
	vehicle.ModelDimensions(dim1, dim2);

	World::DrawLightWithRange(vehicle.GetOffsetInWorldCoords(0.0f, dim1.y - 0.3f, -0.4f), colour, 5.0f, g_multiPlatNeonsIntensity);
	World::DrawLightWithRange(vehicle.GetOffsetInWorldCoords(0.0f, 0.3f - dim2.y, -0.4f), colour, 5.0f, g_multiPlatNeonsIntensity);
	World::DrawLightWithRange(vehicle.GetOffsetInWorldCoords(dim1.x - 0.3f, 0.64f, -0.4f), colour, 5.0f, g_multiPlatNeonsIntensity);
	World::DrawLightWithRange(vehicle.GetOffsetInWorldCoords(0.3f - dim2.x, 0.64f, -0.4f), colour, 5.0f, g_multiPlatNeonsIntensity);

}

void SetMultiPlatNeons()
{
	bool already = false;
	auto& theList = g_multiPlatNeonsList;
	auto& colour = g_multiPlatNeonsColor;

	if (multiPlatNeonsRainbow) // rainbow loop
	{
		colour = g_fadedRGB;
	}

	if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0))
	{
		for (auto& veh : theList)
		{
			if (veh.Equals(g_myVeh)) { already = true; break; }
		}
		if (!already)
		{
			theList.push_back((g_myVeh));
		}
	}

	for (auto it = theList.begin(); it != theList.end();)
	{
		if (!it->Exists() || it->IsDead())
		{
			it = theList.erase(it);
			continue;
		}
		DrawVehicleAmbientLightNeons(*it, g_multiPlatNeonsColor);
		++it;
	}

}

void DriveOnWater(GTAped ped, Entity& waterobject)
{
	if (ped.IsInVehicle())
	{
		ped = ped.CurrentVehicle();
	}

	if (!DOES_ENTITY_EXIST(waterobject))
	{
		Model objModel = 0xC42C019A; // prop_ld_ferris_wheel
		objModel.LoadAndWait();
		const Vector3& Pos = ped.GetOffsetInWorldCoords(0, 4.0f, 0);
		float whh = 0.0f;
		if (GET_WATER_HEIGHT_NO_WAVES(Pos.x, Pos.y, Pos.z, &whh))
		{
			ped.RequestControl();
			SET_ENTITY_COORDS(ped.Handle(), Pos.x, Pos.y, whh, 0, 0, 0, 1);
		}
		waterobject = CREATE_OBJECT(objModel.hash, Pos.x, Pos.y, whh - 4.0f, 1, 1, 1);
		SET_NETWORK_ID_CAN_MIGRATE(OBJ_TO_NET(waterobject), ped != PLAYER_PED_ID());
		SET_ENTITY_COORDS_NO_OFFSET(waterobject, Pos.x, Pos.y, whh, 0, 0, 0);
		SET_ENTITY_ROTATION(waterobject, 0, 90, 0, 2, 1);
		FREEZE_ENTITY_POSITION(waterobject, true);
		Game::Print::PrintBottomCentre("~b~Note:~s~ Enable again if water level is incorrect/changes.");
		WAIT(65);
		return;
	}

	const Vector3& myPos = ped.GetPosition();
	const Vector3& Pos = GET_ENTITY_COORDS(waterobject, 1);

	if (ped.IsInWater())
	{
		float whh = 0.0f;
		if (GET_WATER_HEIGHT_NO_WAVES(Pos.x, Pos.y, Pos.z, &whh))
		{
			SET_ENTITY_COORDS_NO_OFFSET(waterobject, Pos.x, Pos.y, whh, 0, 0, 0);
		}
	}

	if (!NETWORK_HAS_CONTROL_OF_ENTITY(waterobject))
	{
		NETWORK_REQUEST_CONTROL_OF_ENTITY(waterobject);
	}
	SET_ENTITY_COORDS_NO_OFFSET(waterobject, myPos.x, myPos.y, Pos.z, 1, 1, 1);
	SET_ENTITY_ROTATION(waterobject, 180.0f, 90.0f, 180.0f, 2, 1);
	SET_ENTITY_VISIBLE(waterobject, false, false);
	FREEZE_ENTITY_POSITION(waterobject, true);


}

void SetPedBurnMode(GTAped ped, bool enable)
{
	auto isOnFire = ped.IsOnFire();

	if (enable && !isOnFire && !ped.IsInWater())
	{
		ped.SetOnFire(enable);
	}
	else if (!enable && isOnFire)
	{
		ped.SetOnFire(enable);
	}

}

inline void SetHandlingMultiplier()
{
	if (handlingMult == 0)
	{
		return;
	}
	if (!Menu::bitController)
	{
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_RIGHT) || IsKeyDown('D'))
		{
			APPLY_FORCE_TO_ENTITY(g_myVeh, 1, handlingMult / 220, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
		}
		if (IS_DISABLED_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_LEFT) || IsKeyDown('A'))
		{
			APPLY_FORCE_TO_ENTITY(g_myVeh, 1, (0 - handlingMult) / 220, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0,1);
		}
	}
	else
	{
		FLOAT handling_mult69_7_control_normal = GET_DISABLED_CONTROL_NORMAL(2, INPUT_SCRIPT_LEFT_AXIS_X);
		if (handling_mult69_7_control_normal > 0.5f || handling_mult69_7_control_normal < -0.5f)
		{
			APPLY_FORCE_TO_ENTITY(g_myVeh, 1, (handlingMult * handling_mult69_7_control_normal) / 220, 0.0, 0.0, 0.0, 0.0f, 0.0, 0, 1, 1, 1, 0, 1);
		}
	}
}

void SetVehicleInvincibleOn(Vehicle vehicle)
{
	SET_VEHICLE_CAN_BREAK(vehicle, 0);
	SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(vehicle, 0);
	SET_VEHICLE_TYRES_CAN_BURST(vehicle, 0);
	SET_ENTITY_CAN_BE_DAMAGED(vehicle, 0);
	SET_ENTITY_INVINCIBLE(vehicle, 1);
	SET_ENTITY_PROOFS(vehicle, 1, 0, 1, 1, 1, 1, 1, 1);
}

void SetVehicleInvincibleOff(Vehicle vehicle)
{
	SET_VEHICLE_CAN_BREAK(vehicle, 1);
	SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(vehicle, 1);
	SET_ENTITY_CAN_BE_DAMAGED(vehicle, 1);
	SET_ENTITY_INVINCIBLE(vehicle, 0);
	SET_ENTITY_PROOFS(vehicle, 0, 0, 0, 0, 0, 0, 0, 0);
	SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(vehicle, 0);
}

void SetVehicleFlip(GTAvehicle vehicle)
{
	FLOAT roll = GET_ENTITY_ROLL(vehicle.Handle());
	if (vehicle.IsUpsideDown() && (roll > 160 || roll < -160))
	{
		const Model& model = vehicle.Model();
		if (!vehicle.IsInAir() && !vehicle.IsInWater() && !model.IsPlane() && !model.IsHeli())
		{
			vehicle.RequestControlOnce();
			vehicle.SetRotation(Vector3(0, 0, vehicle.Rotation_get().z));
		}
	}
}

void SetVehicleRainbowMode(GTAvehicle vehicle, bool useFader)
{
	vehicle.RequestControlOnce();
	if (useFader)
	{
		vehicle.CustomPrimaryColour_set(g_fadedRGB);
		vehicle.CustomSecondaryColour_set(g_fadedRGB);
	}
	else
	{
		if (vehicle.IsPrimaryColorCustom())
		{
			vehicle.ClearCustomPrimaryColour();
		}
		if (vehicle.IsSecondaryColorCustom())
		{
			vehicle.ClearCustomSecondaryColour();
		}
		vehicle.PrimaryColour_set(rand() % 160);
		vehicle.SecondaryColour_set(rand() % 160);
	}
}

void SetVehicleHeavyMass(GTAvehicle vehicle)
{
	if (!vehicle.Exists())
	{
		return;
	}

	float speed = vehicle.GetSpeed();
	if (speed < 0.5f)
	{
		return;
	}

	vehicle.RequestControlOnce();

	SetVehicleInvincibleOn(vehicle.Handle());
	vehicle.Repair(true); // Only if it needs a repair obv
	vehicle.SetFrictionOverride(100.0f);
	if (!vehicle.IsSeatFree(VehicleSeat::SEAT_DRIVER))
	{
		SetPedSeatbeltOn(vehicle.GetPedOnSeat(VehicleSeat::SEAT_DRIVER).Handle());
	}

	float pushForce = speed * 3.5f; // More speed === More bleed
	auto& vehicleArray = vehicle == g_myVeh ? nearbyVehicles : worldVehicles;

	for (auto& veh : vehicleArray)
	{
		if (vehicle != veh && vehicle.IsTouching(veh))
		{
			GTAvehicle(veh).ApplyForce(vehicle.CollisionNormal() * pushForce);
			vehicle.SetForwardSpeed(speed);
		}
	}

}

Vector3 vehicleWeaponsOriginR;
Vector3 vehicleWeaponsTargetR;
Vector3 vehicleWeaponsOriginL;
Vector3 vehicleWeaponsTargetL;

void StoreVehicleWeaponPos(const GTAentity& vehicle)
{
	Vector3_t dim1, dim2;
	vehicle.Model().Dimensions(dim1, dim2);

	if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_LOOK_BEHIND))
	{
		vehicleWeaponsOriginR = vehicle.GetOffsetInWorldCoords(dim1.x - 0.22f, 0.5f - dim2.y, 0.5f);
		vehicleWeaponsTargetR = vehicle.GetOffsetInWorldCoords(dim1.x - 0.22f, -(dim1.y + 350.0f), 0.5f);

		vehicleWeaponsOriginL = vehicle.GetOffsetInWorldCoords(0.22f - dim2.x, 0.5f - dim2.y, 0.5f);
		vehicleWeaponsTargetL = vehicle.GetOffsetInWorldCoords(0.22f - dim2.x, -(dim2.y + 350.0f), 0.5f);
	}
	else
	{
		vehicleWeaponsOriginR = vehicle.GetOffsetInWorldCoords(dim1.x - 0.22f, dim1.y - 0.5f, 0.5f);
		vehicleWeaponsTargetR = vehicle.GetOffsetInWorldCoords(dim1.x - 0.22f, dim1.y + 350.0f, 0.5f);

		vehicleWeaponsOriginL = vehicle.GetOffsetInWorldCoords(0.22f - dim2.x, dim1.y - 0.5f, 0.5f);
		vehicleWeaponsTargetL = vehicle.GetOffsetInWorldCoords(0.22f - dim2.x, dim1.y + 350.0f, 0.5f);
	}
}

void SetVehicleWeaponFire(Hash whash, float speed = 2000.0f)
{
	const auto& owner = Game::PlayerPed();
	World::ShootBullet(vehicleWeaponsOriginR, vehicleWeaponsTargetR, owner, whash, 200, speed, true, true);
	World::ShootBullet(vehicleWeaponsOriginL, vehicleWeaponsTargetL, owner, whash, 200, speed, true, true);
}

void SetVehicleWeaponExplosion(const EXPLOSION::EXPLOSION& type)
{
	Vector3 targPosR;
	Vector3 targPosL;
	GTAvehicle vehicle(g_myVeh);

	if (IS_DISABLED_CONTROL_PRESSED(0, INPUT_LOOK_BEHIND))
	{
		targPosR = vehicle.GetOffsetInWorldCoords(1.24f, -40.0f, 0.0f);
		targPosL = vehicle.GetOffsetInWorldCoords(-1.24f, -40.0f, 0.0f);
	}
	else
	{
		targPosR = vehicle.GetOffsetInWorldCoords(1.24f, 40.0f, 0.0f);
		targPosL = vehicle.GetOffsetInWorldCoords(-1.24f, 40.0f, 0.0f);
	}

	World::AddExplosion(targPosR, type, 5.0f, 0.3f, true, true);
	World::AddExplosion(targPosL, type, 5.0f, 0.3f, true, true);
}

void SetVehicleWeaponLines()
{
	World::DrawLine(vehicleWeaponsOriginR, vehicleWeaponsTargetR, RGBA(titlebox.R, titlebox.G, titlebox.B, 255));
	World::DrawLine(vehicleWeaponsOriginL, vehicleWeaponsTargetL, RGBA(titlebox.R, titlebox.G, titlebox.B, 255));
}

void SetVehicleWeapons()
{
	StoreVehicleWeaponPos(g_myVeh);
	if (vehicleWeaponLines)
	{
		SetVehicleWeaponLines();
	}

	if (Menu::bitController ? IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LS) : IsKeyDown(VirtualKey::Add))
	{
		if (vehicleRPG
			|| vehicleFireworks
			|| vehicleGuns
			|| vehicleSnowballs
			|| vehicleBalls
			|| vehicleWaterHydrant
			|| vehicleFlameLeak
			|| vehicleLaserGreen
			|| vehicleLaserRed
			|| vehicleTurretsValkyrie
			|| vehicleFlaregun
			|| vehicleHeavySniper
			|| vehicleTazerWeapon
			|| vehicleMolotovWeapon
			|| vehicleCombatPDW)
			CLEAR_AREA_OF_PROJECTILES(vehicleWeaponsOriginR.x, vehicleWeaponsOriginR.y, vehicleWeaponsOriginR.z, 8.0f, 0);

		// RPG
		if (vehicleRPG)
		{
			SetVehicleWeaponFire(WEAPON_VEHICLE_ROCKET);
		}

		// Fireworks
		if (vehicleFireworks)
		{
			SetVehicleWeaponFire(WEAPON_FIREWORK);
		}

		// Guns
		if (vehicleGuns)
		{
			SetVehicleWeaponFire(WEAPON_ASSAULTRIFLE);
		}

		// Snowballs
		if (vehicleSnowballs)
		{
			SetVehicleWeaponFire(WEAPON_SNOWBALL, 2970.0f);
		}

		// Balls
		if (vehicleBalls)
		{
			SetVehicleWeaponFire(WEAPON_BALL, 2970.0f);
		}

		// Water hydrant explosions
		if (vehicleWaterHydrant)
		{
			SetVehicleWeaponExplosion(EXPLOSION::DIR_WATER_HYDRANT);
		}

		// Flame Explosions
		if (vehicleFlameLeak)
		{
			SetVehicleWeaponExplosion(EXPLOSION::DIR_FLAME_EXPLODE);
		}

		// Green laser
		if (vehicleLaserGreen)
		{
			SetVehicleWeaponFire(VEHICLE_WEAPON_PLAYER_LASER);
		}

		// Red laser
		if (vehicleLaserRed)
		{
			SetVehicleWeaponFire(VEHICLE_WEAPON_ENEMY_LASER);
		}

		// Valkyrie turrets
		if (vehicleTurretsValkyrie)
		{
			SetVehicleWeaponFire(VEHICLE_WEAPON_NOSE_TURRET_VALKYRIE);
		}

		// Flare gun/flare
		if (vehicleFlaregun)
		{
			SetVehicleWeaponFire(WEAPON_FLARE, 2970.0f);
		}

		// Heavy sniper
		if (vehicleHeavySniper)
		{
			SetVehicleWeaponFire(WEAPON_HEAVYSNIPER);
		}

		// Tazer
		if (vehicleTazerWeapon)
		{
			SetVehicleWeaponFire(WEAPON_STUNGUN, 2970.0f);
		}

		// Molotov
		if (vehicleMolotovWeapon)
		{
			SetVehicleWeaponFire(WEAPON_MOLOTOV, 2970.0f);
		}

		// Combat pdw
		if (vehicleCombatPDW)
		{
			SetVehicleWeaponFire(WEAPON_COMBATPDW);
		}
	}
}

void SetSelfVehicleBoost()
{
	GTAvehicle vehicle(g_myVeh);

	if (!vehicle.Exists())
	{
		return;
	}

	vehicle.RequestControlOnce();
	SET_VEHICLE_BOOST_ACTIVE(vehicle.Handle(), true);
	APPLY_FORCE_TO_ENTITY(vehicle.Handle(), 1, 0.0f, 1.7f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);
	SET_VEHICLE_BOOST_ACTIVE(vehicle.Handle(), false);

	SetVehicleNosPTFXThisFrame(vehicle);
}

inline void SetSelfVehicleNativeBoost()
{
	{
		const GTAentity& myPed = Game::PlayerPed();
		if (IS_PED_SITTING_IN_ANY_VEHICLE(myPed.GetHandle()) && DOES_ENTITY_EXIST(g_myVeh) && GET_HAS_ROCKET_BOOST(g_myVeh))
		{
			if (IS_CONTROL_PRESSED(2, INPUT_VEH_HORN))
			{
				float* boostCharge = GeneralGlobalHax::GetVehicleBoostChargePtr();
				if (boostCharge != nullptr)
				{
					*boostCharge = 1.24f; // 2.5f full 1.25f full after b944
				}
				GeneralGlobalHax::SetVehicleBoostState(1);
			}
			else
			{
				float* boostCharge = GeneralGlobalHax::GetVehicleBoostChargePtr();
				if (boostCharge != nullptr)
				{
					*boostCharge = 1.24f;
				}
				GeneralGlobalHax::SetVehicleBoostState(0);
			}
		}
	}
}

GTAvehicle pvSubVehicleID;
void SetPVOpsVehicleTextWorld2Screen()
{
	if (pvSubVehicleID.Exists())
	{
		GTAblip carblip = pvSubVehicleID.CurrentBlip();
		GTAped playerPed = PLAYER_PED_ID();

		if (IS_PED_SITTING_IN_VEHICLE(playerPed.Handle(), pvSubVehicleID.Handle()))
		{
			if (carblip.Exists())
			{
				carblip.SetAlpha(0);
			}
			return;
		}

		if (carblip.Exists())
		{
			carblip.SetAlpha(255);
		}

		const Vector3& carpos = pvSubVehicleID.GetPosition();

		if (carpos.DistanceTo(playerPed.GetPosition()) < 40.0f)
		{
			Vector2 newScreenPos;
			if (GET_SCREEN_COORD_FROM_WORLD_COORD(carpos.x, carpos.y, carpos.z, &newScreenPos.x, &newScreenPos.y))
			{
				Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.64f, 0.64f), true, false, false);
				Game::Print::drawstring(pvSubVehicleID.Model().VehicleDisplayName(true) + " - PV", newScreenPos.x, newScreenPos.y);
			}
		}
	}
}

std::map<Vehicle, float> g_multListRPM;
std::map<Vehicle, float> g_multListTorque;
std::map<Vehicle, float> g_multListMaxSpeed;
std::map<Vehicle, float> g_multListHeadLights;

inline void VehicleTorqueMultiplier()
{
	for (auto it = g_multListTorque.begin(); it != g_multListTorque.end();)
	{
		if (DOES_ENTITY_EXIST(it->first))
		{
			SET_VEHICLE_CHEAT_POWER_INCREASE(it->first, it->second);
			++it;
		}
		else
		{
			it = g_multListTorque.erase(it);
		}
	}
}

inline void VehicleMaxSpeedMultiplier()
{
	for (auto it = g_multListMaxSpeed.begin(); it != g_multListMaxSpeed.end();)
	{
		if (DOES_ENTITY_EXIST(it->first))
		{
			SET_ENTITY_MAX_SPEED(it->first, it->second);
			++it;
		}
		else
		{
			it = g_multListMaxSpeed.erase(it);
		}
	}
}

std::map<Vehicle, std::string> g_vehListEngineSounds;
std::string GetVehicleEngineSoundName(const GTAvehicle& vehicle)
{
	auto it = g_vehListEngineSounds.find(vehicle.GetHandle());
	if (it != g_vehListEngineSounds.end())
	{
		return it->second;
	}
	else
	{
		return std::string();
	}
}

void SetVehicleEngineSoundName(GTAvehicle vehicle, const std::string& name)
{
	g_vehListEngineSounds[vehicle.GetHandle()] = name;
	vehicle.EngineSound_set(name);
}

std::unordered_set<Vehicle> g_vehWheelsInvisForRussian;
bool are_vehicle_wheels_invisible(const GTAvehicle& vehicle)
{
	return (g_vehWheelsInvisForRussian.find(vehicle.GetHandle()) != g_vehWheelsInvisForRussian.end());
}

void SetVehicleWheelsInvisible(GTAvehicle vehicle, bool enable)
{
	if (enable)
	{
		if (g_vehWheelsInvisForRussian.find(vehicle.Handle()) == g_vehWheelsInvisForRussian.end())
		{
			g_vehWheelsInvisForRussian.insert(vehicle.Handle());
		}

		vehicle.RequestControl(800);
		vehicle.SetForwardSpeed(DBL_MAX * DBL_MAX);
		WAIT(100);
		SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), DBL_MAX * DBL_MAX);
		MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), DBL_MAX * DBL_MAX);
		vehicle.ApplyForceRelative(Vector3(0, 0, -DBL_MAX * DBL_MAX));
		WAIT(100);
		if (g_multListRPM.count(vehicle.Handle()))
		{
			MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), g_multListRPM[vehicle.Handle()]);
		}
		else
		{
			MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), 1.0f);
		}
		if (g_multListTorque.count(vehicle.Handle()))
		{
			SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), g_multListTorque[vehicle.Handle()]);
		}
		else
		{
			SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), 1.0f);
		}
	}
	else
	{
		if (g_vehWheelsInvisForRussian.find(vehicle.Handle()) != g_vehWheelsInvisForRussian.end())
		{
			g_vehWheelsInvisForRussian.erase(vehicle.Handle());
		}

		vehicle.RequestControl(800);
		for (UINT i = 0; i <= 8; i++)
		{
			vehicle.FixTyre(i);
		}
		vehicle.Repair(false);

		SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), 0.0f);
		MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), 0.0f);
		WAIT(100);
		if (g_multListRPM.count(vehicle.Handle()))
		{
			MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), g_multListRPM[vehicle.Handle()]);
		}
		else
		{
			MODIFY_VEHICLE_TOP_SPEED(vehicle.Handle(), 1.0f);
		}
		if (g_multListTorque.count(vehicle.Handle()))
		{
			SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), g_multListTorque[vehicle.Handle()]);
		}
		else
		{
			SET_VEHICLE_CHEAT_POWER_INCREASE(vehicle.Handle(), 1.0f);
		}
	}
}

std::map<Ped, std::string> g_pedListMovGroup;
std::map<Ped, std::string> g_pedListWMovGroup;
std::map<Ped, std::string> g_pedListFacialMood;

std::string GetPedFacialMood(GTAentity ped)
{
	auto it = g_pedListFacialMood.find(ped.Handle());
	if (it == g_pedListFacialMood.end())
	{
		return std::string();
	}
	else 
	{
		return it->second;
	}
}

void SetPedFacialMood(GTAentity ped, const std::string& animName)
{
	auto& m = g_pedListFacialMood[ped.Handle()];
	m = animName;
	SET_FACIAL_IDLE_ANIM_OVERRIDE(ped.Handle(), animName.c_str(), 0);
}

void clear_ped_facial_mood(GTAentity ped)
{
	auto it = g_pedListFacialMood.find(ped.Handle());
	if (it != g_pedListFacialMood.end())
	{
		g_pedListFacialMood.erase(it);
	}
	CLEAR_FACIAL_IDLE_ANIM_OVERRIDE(ped.Handle());
}

void SetWalkUnderwater(Entity PlayerPed)
{
	if (IS_ENTITY_IN_WATER(PlayerPed))
	{
		SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::IsSwimming, false);
		SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::WasSwimming, false);
		SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::_0xD8072639, false);

		Vector3 PlayerPos = GET_ENTITY_COORDS(PlayerPed, 0);
		DRAW_LIGHT_WITH_RANGEEX(PlayerPos.x, PlayerPos.y, (PlayerPos.z + 1.5f), 255, 255, 251, 100.0f, 1.5f, 0.0f);
		DRAW_LIGHT_WITH_RANGEEX(PlayerPos.x, PlayerPos.y, (PlayerPos.z + 50.0f), 255, 255, 251, 200.0f, 1.0f, 0.0f);

		if (IS_PED_JUMPING(PlayerPed)) // small pushup so jump feel more natural ( like when not underwater )
		{
			APPLY_FORCE_TO_ENTITY(PlayerPed, true, 0, 0, 0.7f, 0, 0, 0, true, true, true, true, false, true);
		}

		if (GET_ENTITY_HEIGHT_ABOVE_GROUND(PlayerPed) > 1) //Do falling down
		{
			SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::IsStanding, false);
			SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::WasStanding, false);
			SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::OpenDoorArmIK, false);
			SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::EdgeDetected, false);
			SET_PED_CONFIG_FLAG(PlayerPed, ePedConfigFlags::IsInTheAir, true);
			APPLY_FORCE_TO_ENTITY(PlayerPed, true, 0, 0, -0.7f, 0, 0, 0, true, true, true, true, false, true);
		}

		if (GET_IS_TASK_ACTIVE(PlayerPed, 281) || IS_PED_SWIMMING(PlayerPed) || IS_PED_SWIMMING_UNDER_WATER(PlayerPed)) // Stop Swimming
		{
			CLEAR_PED_TASKS_IMMEDIATELY(PlayerPed);
		}
	}
}

std::array<int, 3> GetHSVFromRGB(int r, int g, int b)
{
	std::array<int, 3> hsv;
	//setup
	float R = (r / 255.0f);
	float G = (g / 255.0f);
	float B = (b / 255.0f);

	float M = max(R, max(G, B));
	float m = min(R, min(G, B));
	float C = M - m;

	int H = 0;
	int	S = 0;
	//Hue
	if (C != 0)
	{
		if (M == R)
		{
			H = 60 * ((G - B) / C);
		}
		else if (M == G)
		{
			H = 60 * (((B - R) / C) + 2);
		}
		else if (M == B)
		{
			H = 60 * (((R - G) / C) + 4);
		}
		H %= 360;
		if (H < 0)
		{
			H += 360;
		}
	}
	//Saturation
	if (M != 0)
	{
		S = static_cast<int>((C / M) * 100);
	}

	hsv[0] = H;
	hsv[1] = S;
	hsv[2] = static_cast<int>(M * 100);

	return hsv;
}

void GetHSVFromRGB(RgbS colour)
{
	int r, g, b;
	r = colour.R;
	g = colour.G;
	b = colour.B;;
	GetHSVFromRGB(r, g, b);
}

float NormalizeHSV(int h, int s, int v)
{
	float normalOut = sqrt(pow(h, 2) + pow(s, 2) + pow(v / 2, 2));
	return normalOut;
}

static void TickSubsystems()
{
	sub::Spooner::SpoonerMode::Tick();
	sub::GhostRiderMode_catind::Tick();
	sub::VehicleAutoDrive_catind::Tick();
	sub::GravityGun_catind::Tick();
	MagnetGun::g_magnetGun.Tick();
	g_spSnow.Tick();
	VehicleTow::g_vehicleTow.Tick();
	VehicleCruise::g_vehicleCruise.Tick();
	VehicleFly::g_vehicleFly.Tick();
	sub::WaterHack_catind::Tick();
	sub::LaserSight_catind::Tick();
	MeteorShower::g_meteorShower.Tick();
	SmashAbility::g_smashAbility.Tick();
	RopeGun::g_ropeGun.Tick();
	sub::AnimalRiding_catind::Tick();
	GTA2Cam::g_gta2Cam.Tick();
	SetPTFXLopTick();
}

static void TickWorldState()
{
	if (restrictedAreasAccess)
	{
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("am_armybase");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("restrictedareas");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_armybase");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_lossantosintl");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_prison");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("re_prisonvanbreak");
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("am_doors");
	}

	if (!IS_PLAYER_SWITCH_IN_PROGRESS())
	{
		if (pauseClock)
		{
			NETWORK_OVERRIDE_CLOCK_TIME(pauseClockH, pauseClockM, 0);
		}
		if (syncClock)
		{
			SetSyncClockTime();
		}
		if (sub::Clock_catind::loop_clock)
		{
			sub::Clock_catind::DisplayClock();
		}

		if (hideHUD)
		{
			HIDE_HUD_AND_RADAR_THIS_FRAME();
		}
		if (showFullHUD)
		{
			DisplayFullHUDThisFrame(true);
		}

		if (massacreMode)
		{
			SetMassacreModeTick();
		}
		if (blackoutMode)
		{
			SetBlackoutEMPMode();
		}
		if (simpleBlackoutMode)
		{
			SetBlackoutMode();
		}
		if (JumpAroundMode::bEnabled)
		{
			JumpAroundMode::Tick();
		}
	}

	if (neverWanted)
	{
		SET_MAX_WANTED_LEVEL(0);
		SET_WANTED_LEVEL_MULTIPLIER(0.0f);
	}

	if (vehiclePopulation)
	{
		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
		SET_VEHICLE_POPULATION_BUDGET(0);
	}

	if (pedPopulation)
	{
		SET_PED_POPULATION_BUDGET(0);
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
	}

	if (g_rainFXIntensity > 0.0f)
	{
		SET_RAIN(g_rainFXIntensity);
	}

	if (g_frozenRadioStation != -1)
	{
		if (GET_PLAYER_RADIO_STATION_INDEX() != g_frozenRadioStation)
		{
			SET_RADIO_TO_STATION_NAME(GET_RADIO_STATION_NAME(g_frozenRadioStation));
		}
	}

	if (multiPlatNeons)
	{
		SetMultiPlatNeons();
	}

	if (explostionWP != 0)
	{
		SetExplosionWP(explostionWP);
	}
}

static void TickPlayerState(int player, int iped, GTAplayer& player2)
{
	if (ignoredByEveryone)
	{
		NetworkSetEveryoneIgnorePlayer(player);
		SetSelfNearbyPedsCalm();
	}

	if (selfFreezeWantedLevel)
	{
		SET_PLAYER_WANTED_LEVEL(player, selfFreezeWantedLevel, 0);
		SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
	}

	if (playerUnlimitedAbility)
	{
		if (!IS_SPECIAL_ABILITY_ENABLED(player, 0))
		{
			ENABLE_SPECIAL_ABILITY(player, TRUE, 0);
		}
		SET_SPECIAL_ABILITY_MULTIPLIER(FLT_MAX);
		SPECIAL_ABILITY_FILL_METER(player, TRUE, 0);
	}

	if (playerAutoClean)
	{
		sub::PedDamageTextures_catind::ClearAllBloodDamage(iped);
		sub::PedDamageTextures_catind::ClearAllVisibleDamage(iped);
	}

	if (fireworksDisplay)
	{
		StartFireworksAtCoords(GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player2.GetPed().Handle(), GET_RANDOM_FLOAT_IN_RANGE(-10.0f, 10.0f), GET_RANDOM_FLOAT_IN_RANGE(-6.0f, 27.0f), GET_RANDOM_FLOAT_IN_RANGE(-9.0f, 3.5f)), Vector3(0, 0, GET_RANDOM_FLOAT_IN_RANGE(-90.0f, 90.0f)), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 2.45f));
		if (rand() % (INT)2)
		{
			SetExplosionAtCoords(iped, Vector3(GET_RANDOM_FLOAT_IN_RANGE(9.0f, 25.0f), GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f)), EXPLOSION::DIR_WATER_HYDRANT, 8.0f, 0.0f, true, false, 0);
		}
		else
		{
			SetExplosionAtCoords(iped, Vector3(GET_RANDOM_FLOAT_IN_RANGE(-9.0f, -25.0f), GET_RANDOM_FLOAT_IN_RANGE(5.0f, 25.0f), GET_RANDOM_FLOAT_IN_RANGE(0.4f, 20.0f)), EXPLOSION::DIR_WATER_HYDRANT, 8.0f, 0.0f, true, false, 0);
		}
	}

	if (clearWeaponPickups)
	{
		static const Hash weaponPickups[] = {
			PICKUP_WEAPON_BULLPUPSHOTGUN, PICKUP_WEAPON_ASSAULTSMG, PICKUP_VEHICLE_WEAPON_ASSAULTSMG,
			PICKUP_WEAPON_PISTOL50, PICKUP_WEAPON_ASSAULTRIFLE, PICKUP_WEAPON_CARBINERIFLE,
			PICKUP_WEAPON_ADVANCEDRIFLE, PICKUP_WEAPON_MG, PICKUP_WEAPON_COMBATMG,
			PICKUP_WEAPON_SNIPERRIFLE, PICKUP_WEAPON_HEAVYSNIPER, PICKUP_WEAPON_MICROSMG,
			PICKUP_WEAPON_SMG, PICKUP_WEAPON_RPG, PICKUP_WEAPON_MINIGUN,
			PICKUP_WEAPON_PUMPSHOTGUN, PICKUP_WEAPON_SAWNOFFSHOTGUN, PICKUP_WEAPON_ASSAULTSHOTGUN,
			PICKUP_WEAPON_GRENADE, PICKUP_WEAPON_MOLOTOV, PICKUP_WEAPON_SMOKEGRENADE,
			PICKUP_WEAPON_STICKYBOMB, PICKUP_WEAPON_PISTOL, PICKUP_WEAPON_COMBATPISTOL,
			PICKUP_WEAPON_APPISTOL, PICKUP_WEAPON_GRENADELAUNCHER, PICKUP_WEAPON_STUNGUN,
			PICKUP_WEAPON_FIREEXTINGUISHER, PICKUP_WEAPON_PETROLCAN, PICKUP_WEAPON_KNIFE,
			PICKUP_WEAPON_NIGHTSTICK, PICKUP_WEAPON_HAMMER, PICKUP_WEAPON_BAT,
			PICKUP_WEAPON_GOLFCLUB, PICKUP_WEAPON_CROWBAR, PICKUP_WEAPON_BULLPUPRIFLE,
			PICKUP_WEAPON_BOTTLE, PICKUP_WEAPON_SNSPISTOL, PICKUP_WEAPON_GUSENBERG,
			PICKUP_WEAPON_HEAVYPISTOL, PICKUP_WEAPON_SPECIALCARBINE, PICKUP_WEAPON_DAGGER,
			PICKUP_WEAPON_VINTAGEPISTOL, PICKUP_WEAPON_FIREWORK, PICKUP_WEAPON_MUSKET,
			PICKUP_WEAPON_HEAVYSHOTGUN, PICKUP_WEAPON_MARKSMANRIFLE, PICKUP_WEAPON_PROXMINE,
			PICKUP_WEAPON_HOMINGLAUNCHER, PICKUP_WEAPON_FLAREGUN,
			PICKUP_WEAPON_PISTOL_MK2, PICKUP_WEAPON_SMG_MK2, PICKUP_WEAPON_ASSAULTRIFLE_MK2,
			PICKUP_WEAPON_CARBINERIFLE_MK2, PICKUP_WEAPON_COMBATMG_MK2, PICKUP_WEAPON_HEAVYSNIPER_MK2,
		};
		for (Hash pickup : weaponPickups)
		{
			REMOVE_ALL_PICKUPS_OF_TYPE(pickup);
		}
	}
}

static void TickPlayerAbilities()
{
	Ped myPed = PLAYER_PED_ID();
	Player myPlayer = PLAYER_ID();

	if (playerWalkUnderwater)
	{
		SetWalkUnderwater(myPed);
	}

	if (playerInvincibility)
	{
		if (!GET_PLAYER_INVINCIBLE(myPlayer))
		{
			SET_PLAYER_INVINCIBLE(myPlayer, 1);
		}
		SetPedInvincibleOn(myPed);
	}

	if (playerNoRagdoll)
	{
		SetPedNoRagdollOn(myPed);
	}

	if (playerSeatbelt)
	{
		SetPedSeatbeltOn(myPed);
	}

	if (superJump)
	{
		SET_SUPER_JUMP_THIS_FRAME(myPlayer);
	}

	if (noClip)
	{
		SetNoclip();
	}

	if (superRun)
	{
		SetLocalButtonSuperRun();
	}

	if (selfRefillHealthInCover)
	{
		SetSelfRefillHealthWhenInCover();
	}

	if (superman)
	{
		SetLocalSupermanManual();
	}

	if (supermanAuto)
	{
		SetPedSupermanAuto(myPed);
	}

	if (forceField)
	{
		SetLocalForcefield();
	}

	if (driveOnWater)
	{
		DriveOnWater(myPed, g_driveWaterObject);
	}

	if (playerBurn)
	{
		SetPedBurnMode(myPed, true);
	}

	if (selfSweatMult > 0.0f)
	{
		SET_PED_SWEAT(myPed, selfSweatMult);
		if (selfSweatMult > 4.0f)
		{
			SET_PED_WETNESS_ENABLED_THIS_FRAME(myPed);
			SET_PED_WETNESS_HEIGHT(myPed, selfSweatMult / 6);
		}
	}

	if (swimSpeedMult)
	{
		SET_SWIM_MULTIPLIER_FOR_PLAYER(myPlayer, swimSpeedMult);
		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(myPlayer, swimSpeedMult);
	}

	if (g_playerVerticalElongationMultiplier != 1.0f)
	{
		GeneralGlobalHax::SetPlayerHeight(g_playerVerticalElongationMultiplier);
	}

	if (playerNoiseMult != 1)
	{
		SET_PLAYER_NOISE_MULTIPLIER(myPlayer, playerNoiseMult);
	}
}

static void TickWeaponEffects()
{
	Ped myPed = PLAYER_PED_ID();
	Player myPlayer = PLAYER_ID();

	SetTargetIntoSlot();

	if (forgeGun)
	{
		SetForgeGun();
	}

	if (sub::BreatheStuff_catind::loop_player_breatheStuff != sub::BreatheStuff_catind::BreathePtfxType::None)
	{
		sub::BreatheStuff_catind::set_self_breathe_ptfx(sub::BreatheStuff_catind::loop_player_breatheStuff);
	}

	if (explosiveRounds)
	{
		SET_EXPLOSIVE_AMMO_THIS_FRAME(myPlayer);
	}

	if (explosiveMelee)
	{
		SET_EXPLOSIVE_MELEE_THIS_FRAME(myPlayer);
	}

	if (flamingRounds)
	{
		SET_FIRE_AMMO_THIS_FRAME(myPlayer);
	}

	if (bitInfiniteAmmo && (bitInfiniteAmmoEnth != myPed || GET_TIME_SINCE_LAST_DEATH() < 10000))
	{
		bitInfiniteAmmoEnth = myPed;
		SET_PED_INFINITE_AMMO_CLIP(bitInfiniteAmmoEnth, true);
	}

	if (selfInfiniteParachutes)
	{
		GivePedParachute(myPed);
	}

	if (weaponDamageIncrease != 1.0f)
	{
		SET_PLAYER_WEAPON_DAMAGE_MODIFIER(myPlayer, weaponDamageIncrease);
		SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(myPlayer, weaponDamageIncrease, true);
	}

	if (IS_PED_SHOOTING(myPed))
	{
		if (kaboomGun)
		{
			SetExplosionAtBulletHit(myPed, kaboomGunHash, kaboomGunInvis);
		}
		if (triggerFXGun)
		{
			SetTriggerFXAtBulletHit(myPed, triggerFXGunData.asset, triggerFXGunData.effect, Vector3::RandomXYZ() * 180.0f, GET_RANDOM_FLOAT_IN_RANGE(0.63f, 1.40f));
		}
		if (bulletGun)
		{
			SetBulletGun();
		}
		if (teleportGun)
		{
			SetTeleportGun();
		}
		if (pedGun)
		{
			SetPedGun();
		}
		if (objectGun)
		{
			SetObjectGun();
		}
		if (bulletTime)
		{
			SET_TIME_SCALE(0.2f);
		}
		if (tripleBullets)
		{
			SetTripleBullets();
		}
	}
	if (GET_GAME_TIMER() >= Menu::delayedTimer && bulletTime)
	{
		SET_TIME_SCALE(currentTimescale);
	}
}

static void TickVehicleEffects(bool gameIsPaused)
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0))
	{
		bitVehicleGravity = bitFreezeVehicle = bitVehicleSlippyTires = false;
		return;
	}

	// Acceleration and brake multipliers
	if (!g_myVehModel.IsHeli())
	{
		if (g_myVehModel.IsPlane() || IS_VEHICLE_ON_ALL_WHEELS(g_myVeh))
		{
			if (GET_PED_IN_VEHICLE_SEAT(g_myVeh, VehicleSeat::SEAT_DRIVER, 0) == PLAYER_PED_ID())
			{
				if (IS_CONTROL_PRESSED(2, INPUT_VEH_ACCELERATE) && accelMult != 0)
				{
					APPLY_FORCE_TO_ENTITY(g_myVeh, 1, 0.0, (float)(accelMult) / 69.0f, 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
				}
				if (brakeMult != 0 && IS_CONTROL_PRESSED(2, INPUT_VEH_BRAKE))
				{
					APPLY_FORCE_TO_ENTITY(g_myVeh, 0, 0.0, (float)(0 - brakeMult), 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
				}
				if (GET_ENTITY_SPEED_VECTOR(g_myVeh, true).y > 2)
				{
					SetHandlingMultiplier();
				}
			}
		}
	}

	if (vehicleDamageAndDefense != 1.0f)
	{
		SET_PLAYER_VEHICLE_DAMAGE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
		SET_PLAYER_VEHICLE_DEFENSE_MODIFIER(PLAYER_ID(), vehicleDamageAndDefense);
	}

	if (selfEngineOn)
	{
		GTAvehicle veh = g_myVeh;
		if (veh.Exists())
		{
			if (!veh.GetEngineRunning())
			{
				veh.SetEngineRunning(true);	
			}
			if (!veh.GetLightsOn())
			{
				veh.SetLightsOn(true);
			}
		}
	}

	if (vehicleInvincibility)
	{
		SetVehicleInvincibleOn(g_myVeh);
	}

	if (vehicleFixLoop)
	{
		GTAvehicle veh = g_myVeh;
		if (veh.IsDamaged())
		{
			static int vehicleOpsFixCarTexterValue = 0;
			auto& fixCarTexterVal = vehicleOpsFixCarTexterValue;
			std::array<bool, (int)VehicleWindow::Last> windowsIntact;
			if (fixCarTexterVal == 1)
			{
				for (int i = 0; i < windowsIntact.size(); i++)
				{
					windowsIntact[i] = veh.IsWindowIntact((VehicleWindow)i);
				}
			}

			SET_VEHICLE_FIXED(veh.Handle());
			SET_VEHICLE_DIRT_LEVEL(veh.Handle(), 0.0f);
			SET_VEHICLE_ENGINE_HEALTH(veh.Handle(), 2000.0f);
			SET_VEHICLE_PETROL_TANK_HEALTH(veh.Handle(), 2000.0f);
			SET_VEHICLE_BODY_HEALTH(veh.Handle(), 2000.0f);
			SET_VEHICLE_UNDRIVEABLE(veh.Handle(), false);

			if (fixCarTexterVal == 1)
			{
				for (int i = 0; i < windowsIntact.size(); i++)
				{
					if (!windowsIntact[i])
					{
						veh.RollDownWindow((VehicleWindow)i);
					}
				}
			}
		}
	}

	if (vehicleFlipLoop)
	{
		SetVehicleFlip(g_myVeh);
	}

	if (vehicleInvisibility)
	{
		inull = IS_ENTITY_VISIBLE(PLAYER_PED_ID());
		SET_ENTITY_VISIBLE(g_myVeh, false, false);
		SET_ENTITY_VISIBLE(PLAYER_PED_ID(), inull, false);
		vehicleInvisibility = false;
	}

	if (carColorChange)
	{
		SetVehicleRainbowMode(g_myVeh, true);
	}

	if (vehicleDisableSiren)
	{
		if (GTAvehicle(g_myVeh).GetHasSiren())
		{
			SET_VEHICLE_HAS_MUTED_SIRENS(g_myVeh, TRUE);
		}
	}

	if (vehicleSlam)
	{
		if (vehicleSlam <= -0.35f || IS_VEHICLE_ON_ALL_WHEELS(g_myVeh))
		{
			APPLY_FORCE_TO_ENTITY(g_myVeh, 1, 0.0, 0.0, vehicleSlam, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);
		}
	}

	if (vehicleHeavyMass)
	{
		SetVehicleHeavyMass(g_myVeh);
	}

	// Vehicle controls (only when game is not paused)
	if (!gameIsPaused)
	{
		if (raceBoost && IS_CONTROL_PRESSED(2, INPUT_VEH_HORN))
		{
			SetSelfVehicleBoost();
		}

		if (carJump != 0)
		{
			SetLocalCarJump();
		}

		if (carHydraulics)
		{
			SetLocalCarHydraulics();
		}

		if (superGrip)
		{
			SET_VEHICLE_ON_GROUND_PROPERLY(g_myVeh, 0.0f);
		}

		if (superCarMode)
		{
			SetSuperCarModeSelf();
		}

		SetVehicleWeapons();

		if (sub::Speedo_catind::loop_speedo != sub::Speedo_catind::SPEEDOMODE_OFF)
		{
			sub::Speedo_catind::SpeedoTick();
		}
	}
}

// Main loop

void Menu::loops()
{
	bool gameIsPaused = IS_PAUSE_MENU_ACTIVE() != 0;

	// Apply default outfit on first load
	if (!GET_IS_LOADING_SCREEN_ACTIVE() && !defaultPedSet)
	{
		sub::ComponentChanger_Outfit_catind::Apply(PLAYER_PED_ID(), "menyooStuff/defaultPed.xml", true, false, false, false, false, false);
		sub::ComponentChanger_Outfit_catind::Apply(PLAYER_PED_ID(), "menyooStuff/defaultPed.xml", false, true, true, true, true, true);
		defaultPedSet = true;
	}

	Game::CustomHelpText::Tick();
	UpdateNearbyStuffArraysTick();

	if (gameIsPaused)
	{
		SetPauseMenuTeleToWpCommand();
	}

	TickSubsystems();
	MenuInput::UpdateDeltaCursorNormal();

	// Cache current vehicle and weapon
	if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), 0))
	{
		g_myVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);
		g_myVehModel = GET_ENTITY_MODEL(g_myVeh);
	}
	else
	{
		g_myVeh = 0;
	}

	if (!IS_PLAYER_DEAD(PLAYER_ID()))
	{
		GET_CURRENT_PED_WEAPON(PLAYER_PED_ID(), &g_myWeap, 1);
	}

	if (rainbowBoxes)
	{
		titlebox = { g_fadedRGB.R, g_fadedRGB.G, g_fadedRGB.B, titlebox.A };
	}

	if (checkSelfDeathModel)
	{
		ManualRespawn::CheckSelfDealthModel();
	}

	// Tick all categories
	TickWorldState();

	if (GET_GAME_TIMER() >= delayedTimer)
	{
		int player = PLAYER_ID();
		GTAplayer player2;
		player2.Handle() = player;
		int iped = PLAYER_PED_ID();
		TickPlayerState(player, iped, player2);
	}

	TickWeaponEffects();
	TickPlayerAbilities();

	// HUD overlays
	if (xyzhCoords)
	{
		XYZH();
	}
	if (FPSCounter::bDisplayFps)
	{
		FPSCounter::DisplayFps();
	}

	TickVehicleEffects(gameIsPaused);
	SetPVOpsVehicleTextWorld2Screen();
}

// Secondary thread

static void TickSpectatePlayer()
{
	if (spectatePlayer >= 0 && spectatePlayer < GAME_PLAYERCOUNT)
	{
		if (!NETWORK_IS_PLAYER_ACTIVE(spectatePlayer))
		{
			NETWORK_SET_IN_SPECTATOR_MODE_EXTENDED(false, spectatePlayer, 1);
			int p = GET_PLAYER_PED(spectatePlayer);
			if (DOES_ENTITY_EXIST(p))
			{
				NETWORK_SET_IN_SPECTATOR_MODE(false, p);
			}
			NETWORK_SET_ACTIVITY_SPECTATOR(false);
			spectatePlayer = -1;
		}
		else
		{
			NETWORK_SET_IN_SPECTATOR_MODE(true, GET_PLAYER_PED(spectatePlayer));
		}
	}
}

void ThreadMenuLoops2()
{
	for (;;)
	{
		WAIT(0);

		VehicleTorqueMultiplier();
		VehicleMaxSpeedMultiplier();

		FlameThrower::Tick();

		if (sub::TVChannelStuff_catind::loop_basictv)
		{
			sub::TVChannelStuff_catind::DrawTvWhereItsSupposedToBe();
		}

		TickSpectatePlayer();

		if (lightGun)
		{
			SetLightGun();
		}

		switch (autoKillEnemies)
		{
		case 1:
			World::KillNearbyPeds(PLAYER_PED_ID(), FLT_MAX, PedRelationship::Hate);
			World::KillNearbyPeds(PLAYER_PED_ID(), FLT_MAX, PedRelationship::Dislike);
			break;
		case 2:
			World::KillMyEnemies();
			break;
		}

		sub::TeleportLocations_catind::Yachts::Tick();
		ManualRespawn::g_manualRespawn.Tick();

		if (unlimitedVehicleBoost)
		{
			SetSelfVehicleNativeBoost();
		}
	}
}