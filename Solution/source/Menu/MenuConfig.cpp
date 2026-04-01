/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "MenuConfig.h"

#include "..\macros.h"

#include "Menu.h"
#include "Routine.h"
#include "Language.h"

#include "..\Util\ExePath.h"
#include "..\Natives\types.h" // RGBA/RgbS
#include "..\Util\GTAmath.h"
#include "..\Util\FileLogger.h"

#include "..\Submenus\Spooner\SpoonerMode.h"
#include "..\Submenus\Spooner\SpoonerSettings.h"
#include "..\Misc\ManualRespawn.h"
#include "..\Misc\FpsCounter.h"
#include "..\Submenus\MiscOptions.h"
#include "..\Misc\JumpAroundMode.h"
#include "..\Memory\GTAmemory.h"
#include "..\Submenus\AnimalRiding.h"
#include "..\Submenus\WeaponOptions.h"
#include "..\Misc\MagnetGun.h"
#include "..\Misc\RopeGun.h"
#include "..\Misc\SmashAbility.h"
#include "..\Misc\VehicleTow.h"
#include "..\Misc\VehicleFly.h"
#include "..\Submenus\TimeOptions.h"
#include "..\Submenus\VehicleOptions.h"

#include <Windows.h>
#include <simpleini\SimpleIni.h>

CSimpleIniA MenuConfig::iniFile;
bool MenuConfig::bSaveAtIntervals = true;

// Initialize the default camera parameters
namespace MenuConfig {
    namespace FreeCam {
        float defaultSpeed = 0.5f;
        float defaultFov = 50.0f;
		float defaultSlowSpeed = 0.2f;
        float speedAdjustStep = 0.1f;
        float fovAdjustStep = 1.0f;
        float minSpeed = 0.1f;
        float maxSpeed = 10.0f;
        float minFov = 30.0f;
        float maxFov = 120.0f;
    }
}

void MenuConfig::ConfigInit()
{
	MenuConfig::iniFile.SetUnicode(true);
	MenuConfig::iniFile.SetMultiKey(false);
	MenuConfig::iniFile.SetMultiLine(false);
	SetFileAttributesW(GetPathffW(Pathff::Main, false).c_str(), GetFileAttributes(GetPathffW(Pathff::Main, false).c_str()) & ~FILE_ATTRIBUTE_READONLY);

	if (MenuConfig::iniFile.LoadFile((GetPathffA(Pathff::Main, true) + "menyooConfig.ini").c_str()) < 0)
		addlog(ige::LogType::LOG_ERROR,  "Failed to load menyooConfig from " + GetPathffA(Pathff::Main, true) + "menyooConfig.ini.");
	else {
		ConfigRead();
		addlog(ige::LogType::LOG_INFO, "Logging level " + std::to_string(g_loglevel) + " active. Edit loglevel in menyooconfig.ini to change.");
		addlog(ige::LogType::LOG_INFO, "Config loaded from " + GetPathffA(Pathff::Main, true) + "menyooConfig.ini.");
	}
}

void MenuConfig::ConfigRead()
{
	auto& ini = MenuConfig::iniFile;


	std::string section_settings = "settings";/////////

	MenuConfig::bSaveAtIntervals = ini.GetBoolValue(section_settings.c_str(), "sync_with_config_at_intervals", MenuConfig::bSaveAtIntervals);
	checkSelfDeathModel = ini.GetBoolValue(section_settings.c_str(), "DeathModelReset", checkSelfDeathModel);
	menubinds = ini.GetLongValue(section_settings.c_str(), "open_key", menubinds);
	menubindsGamepad.first = ini.GetLongValue(section_settings.c_str(), "open_button_for_gamepad_1", menubindsGamepad.first);
	menubindsGamepad.second = ini.GetLongValue(section_settings.c_str(), "open_button_for_gamepad_2", menubindsGamepad.second);
	respawnbinds = ini.GetLongValue(section_settings.c_str(), "manual_respawn_button", respawnbinds);
	stopanimbinds = ini.GetLongValue(section_settings.c_str(), "stop_animation_key", stopanimbinds);
	menuPos.x = ini.GetDoubleValue(section_settings.c_str(), "menuPosX", (menuPos.x + 0.0598f) * 100); menuPos.x = menuPos.x / 100 - 0.0598f;
	menuPos.y = ini.GetDoubleValue(section_settings.c_str(), "menuPosY", (menuPos.y + 0.074f) * 100); menuPos.y = menuPos.y / 100 - 0.074f;
	Menu::bit_glare_test = ini.GetBoolValue(section_settings.c_str(), "Titlebox_Globe", Menu::bit_glare_test);
	Menu::bit_centre_title = ini.GetBoolValue(section_settings.c_str(), "centre_title", Menu::bit_centre_title);
	Menu::bit_centre_options = ini.GetBoolValue(section_settings.c_str(), "centre_options", Menu::bit_centre_options);
	Menu::bit_centre_breaks = ini.GetBoolValue(section_settings.c_str(), "centre_breaks", Menu::bit_centre_breaks);
	Language::configLangName = ini.GetValue(section_settings.c_str(), "language", Language::configLangName.c_str());
	Language::Init();
	g_loglevel = ini.GetLongValue(section_settings.c_str(), "log level", g_loglevel);


	std::string section_general = "general";/////////


	BindNoClip = ini.GetLongValue(section_general.c_str(), "FreeCamButton", BindNoClip);


	std::string section_colours = "colours";/////////


	Menu::gradients = ini.GetBoolValue(section_colours.c_str(), "gradients", Menu::gradients);
	rainbowBoxes = ini.GetBoolValue(section_colours.c_str(), "rainbow_mode", rainbowBoxes);
	Menu::thinLineOverScrect = ini.GetBoolValue(section_colours.c_str(), "thin_line_over_footer", Menu::thinLineOverScrect);

	titlebox.R = ini.GetLongValue(section_colours.c_str(), "titlebox_R", titlebox.R);
	titlebox.G = ini.GetLongValue(section_colours.c_str(), "titlebox_G", titlebox.G);
	titlebox.B = ini.GetLongValue(section_colours.c_str(), "titlebox_B", titlebox.B);
	titlebox.A = ini.GetLongValue(section_colours.c_str(), "titlebox_A", titlebox.A);

	BG.R = ini.GetLongValue(section_colours.c_str(), "BG_R", BG.R);
	BG.G = ini.GetLongValue(section_colours.c_str(), "BG_G", BG.G);
	BG.B = ini.GetLongValue(section_colours.c_str(), "BG_B", BG.B);
	BG.A = ini.GetLongValue(section_colours.c_str(), "BG_A", BG.A);

	titletext.R = ini.GetLongValue(section_colours.c_str(), "titletext_R", titletext.R);
	titletext.G = ini.GetLongValue(section_colours.c_str(), "titletext_G", titletext.G);
	titletext.B = ini.GetLongValue(section_colours.c_str(), "titletext_B", titletext.B);
	titletext.A = ini.GetLongValue(section_colours.c_str(), "titletext_A", titletext.A);

	optiontext.R = ini.GetLongValue(section_colours.c_str(), "optiontext_R", optiontext.R);
	optiontext.G = ini.GetLongValue(section_colours.c_str(), "optiontext_G", optiontext.G);
	optiontext.B = ini.GetLongValue(section_colours.c_str(), "optiontext_B", optiontext.B);
	optiontext.A = ini.GetLongValue(section_colours.c_str(), "optiontext_A", optiontext.A);

	selectedtext.R = ini.GetLongValue(section_colours.c_str(), "selectedtext_R", selectedtext.R);
	selectedtext.G = ini.GetLongValue(section_colours.c_str(), "selectedtext_G", selectedtext.G);
	selectedtext.B = ini.GetLongValue(section_colours.c_str(), "selectedtext_B", selectedtext.B);
	selectedtext.A = ini.GetLongValue(section_colours.c_str(), "selectedtext_A", selectedtext.A);

	optionbreaks.R = ini.GetLongValue(section_colours.c_str(), "optionbreaks_R", optionbreaks.R);
	optionbreaks.G = ini.GetLongValue(section_colours.c_str(), "optionbreaks_G", optionbreaks.G);
	optionbreaks.B = ini.GetLongValue(section_colours.c_str(), "optionbreaks_B", optionbreaks.B);
	optionbreaks.A = ini.GetLongValue(section_colours.c_str(), "optionbreaks_A", optionbreaks.A);

	optioncount.R = ini.GetLongValue(section_colours.c_str(), "optioncount_R", optioncount.R);
	optioncount.G = ini.GetLongValue(section_colours.c_str(), "optioncount_G", optioncount.G);
	optioncount.B = ini.GetLongValue(section_colours.c_str(), "optioncount_B", optioncount.B);
	optioncount.A = ini.GetLongValue(section_colours.c_str(), "optioncount_A", optioncount.A);

	selectionhi.R = ini.GetLongValue(section_colours.c_str(), "selectionhi_R", selectionhi.R);
	selectionhi.G = ini.GetLongValue(section_colours.c_str(), "selectionhi_G", selectionhi.G);
	selectionhi.B = ini.GetLongValue(section_colours.c_str(), "selectionhi_B", selectionhi.B);
	selectionhi.A = ini.GetLongValue(section_colours.c_str(), "selectionhi_A", selectionhi.A);

	_globalPedTrackers_Col.R = ini.GetLongValue(section_colours.c_str(), "pedTrackers_R", _globalPedTrackers_Col.R);
	_globalPedTrackers_Col.G = ini.GetLongValue(section_colours.c_str(), "pedTrackers_G", _globalPedTrackers_Col.G);
	_globalPedTrackers_Col.B = ini.GetLongValue(section_colours.c_str(), "pedTrackers_B", _globalPedTrackers_Col.B);
	_globalPedTrackers_Col.A = ini.GetLongValue(section_colours.c_str(), "pedTrackers_A", _globalPedTrackers_Col.A);


	std::string section_fonts = "fonts";/////////


	font_title = ini.GetLongValue(section_fonts.c_str(), "title", font_title);
	font_options = ini.GetLongValue(section_fonts.c_str(), "options", font_options);
	font_selection = ini.GetLongValue(section_fonts.c_str(), "selection", font_selection);
	font_breaks = ini.GetLongValue(section_fonts.c_str(), "breaks", font_breaks);
	font_xyzh = ini.GetLongValue(section_fonts.c_str(), "font_xyzh", font_xyzh);
	font_speedo = ini.GetLongValue(section_fonts.c_str(), "font_speedo", font_speedo);


	std::string section_spooner = "object-spooner";/////////

	sub::Spooner::SpoonerMode::bindsKeyboard = ini.GetLongValue(section_spooner.c_str(), "SpoonerModeHotkey", sub::Spooner::SpoonerMode::bindsKeyboard);
	sub::Spooner::SpoonerMode::bindsGamepad.first = ini.GetLongValue(section_spooner.c_str(), "SpoonerModeGamepadBind_1", sub::Spooner::SpoonerMode::bindsGamepad.first);
	sub::Spooner::SpoonerMode::bindsGamepad.second = ini.GetLongValue(section_spooner.c_str(), "SpoonerModeGamepadBind_2", sub::Spooner::SpoonerMode::bindsGamepad.second);
	sub::Spooner::Settings::CameraMovementSensitivityKeyboard = (float)ini.GetDoubleValue(section_spooner.c_str(), "CameraMovementSensitivityKeyboard", sub::Spooner::Settings::CameraMovementSensitivityKeyboard);
	sub::Spooner::Settings::CameraRotationSensitivityMouse = (float)ini.GetDoubleValue(section_spooner.c_str(), "CameraRotationSensitivityMouse", sub::Spooner::Settings::CameraRotationSensitivityMouse);
	sub::Spooner::Settings::CameraMovementSensitivityGamepad = (float)ini.GetDoubleValue(section_spooner.c_str(), "CameraMovementSensitivityGamepad", sub::Spooner::Settings::CameraMovementSensitivityGamepad);
	sub::Spooner::Settings::CameraRotationSensitivityGamepad = (float)ini.GetDoubleValue(section_spooner.c_str(), "CameraRotationSensitivityGamepad", sub::Spooner::Settings::CameraRotationSensitivityGamepad);
	sub::Spooner::Settings::bShowModelPreviews = ini.GetBoolValue(section_spooner.c_str(), "ShowModelPreviews", sub::Spooner::Settings::bShowModelPreviews);
	sub::Spooner::Settings::bShowBoxAroundSelectedEntity = ini.GetBoolValue(section_spooner.c_str(), "ShowBoxAroundSelectedEntity", sub::Spooner::Settings::bShowBoxAroundSelectedEntity);
	sub::Spooner::Settings::bSpawnDynamicProps = ini.GetBoolValue(section_spooner.c_str(), "SpawnDynamicProps", sub::Spooner::Settings::bSpawnDynamicProps);
	sub::Spooner::Settings::bSpawnDynamicPeds = ini.GetBoolValue(section_spooner.c_str(), "SpawnDynamicPeds", sub::Spooner::Settings::bSpawnDynamicPeds);
	sub::Spooner::Settings::bSpawnDynamicVehicles = ini.GetBoolValue(section_spooner.c_str(), "SpawnDynamicVehicles", sub::Spooner::Settings::bSpawnDynamicVehicles);
	sub::Spooner::Settings::bFreezeEntityWhenMovingIt = ini.GetBoolValue(section_spooner.c_str(), "FreezeEntityWhenMovingIt", sub::Spooner::Settings::bFreezeEntityWhenMovingIt);
	sub::Spooner::Settings::bSpawnInvincibleEntities = ini.GetBoolValue(section_spooner.c_str(), "SpawnInvincibleEntities", sub::Spooner::Settings::bSpawnInvincibleEntities);
	sub::Spooner::Settings::bSpawnStillPeds = ini.GetBoolValue(section_spooner.c_str(), "SpawnStillPeds", sub::Spooner::Settings::bSpawnStillPeds);
	sub::Spooner::Settings::bAddToDbAsMissionEntities = ini.GetBoolValue(section_spooner.c_str(), "AddToDbAsMissionEntities", sub::Spooner::Settings::bAddToDbAsMissionEntities);
	sub::Spooner::Settings::bTeleportToReferenceWhenLoadingFile = ini.GetBoolValue(section_spooner.c_str(), "TeleportToReferenceWhenLoadingFile", sub::Spooner::Settings::bTeleportToReferenceWhenLoadingFile);
	sub::Spooner::Settings::bKeepPositionWhenAttaching = ini.GetBoolValue(section_spooner.c_str(), "KeepPositionWhenAttaching", sub::Spooner::Settings::bKeepPositionWhenAttaching);
	sub::Spooner::Settings::SpoonerModeMode = (sub::Spooner::eSpoonerModeMode)ini.GetLongValue(section_spooner.c_str(), "SpoonerModeMethod", (UINT8)sub::Spooner::Settings::SpoonerModeMode);


	std::string section_haxValues = "hax-values";/////////


	 //loop_hide_hud = ini.GetBoolValue(section_haxValues.c_str(), "hide_hud", loop_hide_hud);
	showFullHUD = ini.GetBoolValue(section_haxValues.c_str(), "show_full_hud", showFullHUD);
	ManualRespawn::g_manualRespawn.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "manual_respawn", ManualRespawn::g_manualRespawn.Enabled());
	noClip = ini.GetBoolValue(section_haxValues.c_str(), "freecam", noClip);
	xyzhCoords = ini.GetBoolValue(section_haxValues.c_str(), "display_xyzh_coords", xyzhCoords);
	FPSCounter::bDisplayFps = ini.GetBoolValue(section_haxValues.c_str(), "display_fps", FPSCounter::bDisplayFps);
	sub::TVChannelStuff_catind::loop_basictv = ini.GetBoolValue(section_haxValues.c_str(), "basic_tv_player", sub::TVChannelStuff_catind::loop_basictv);
	syncClock = ini.GetBoolValue(section_haxValues.c_str(), "sync_clock", syncClock);
	pauseClock = ini.GetBoolValue(section_haxValues.c_str(), "pause_clock", pauseClock);
	pauseClockH = ini.GetLongValue(section_haxValues.c_str(), "pause_clock_hour", pauseClockH);
	pauseClockM = ini.GetLongValue(section_haxValues.c_str(), "pause_clock_minute", pauseClockM);
	pedPopulation = ini.GetBoolValue(section_haxValues.c_str(), "decreased_ped_population", pedPopulation);
	vehiclePopulation = ini.GetBoolValue(section_haxValues.c_str(), "decreased_veh_population", vehiclePopulation);
	clearWeaponPickups = ini.GetBoolValue(section_haxValues.c_str(), "decreased_weapon_pickups", clearWeaponPickups);
	g_clearAreaRadius = ini.GetDoubleValue(section_haxValues.c_str(), "clear_area_radius", g_clearAreaRadius);
	restrictedAreasAccess = ini.GetBoolValue(section_haxValues.c_str(), "restricted_area_access", restrictedAreasAccess);
	fireworksDisplay = ini.GetBoolValue(section_haxValues.c_str(), "fireworks_ahoy", fireworksDisplay);
	blackoutMode = ini.GetBoolValue(section_haxValues.c_str(), "emp_mode", blackoutMode);
	simpleBlackoutMode = ini.GetBoolValue(section_haxValues.c_str(), "simple_blackout_mode", simpleBlackoutMode);
	massacreMode = ini.GetBoolValue(section_haxValues.c_str(), "massacre_mode", massacreMode);
	JumpAroundMode::bEnabled = ini.GetBoolValue(section_haxValues.c_str(), "jump_around_mode", JumpAroundMode::bEnabled);
	g_frozenRadioStation = (INT16)ini.GetLongValue(section_haxValues.c_str(), "frozen_radio_station", g_frozenRadioStation);
	g_spSnow.ToggleSnow(ini.GetBoolValue(section_haxValues.c_str(), "snow_on_terrain", g_spSnow.IsSnow()));
	sub::AnimalRiding_catind::Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "animal_riding_mode", sub::AnimalRiding_catind::Enabled());

	forgeGun = ini.GetBoolValue(section_haxValues.c_str(), "forge_gun", forgeGun);
	g_forgeGunShootForce = ini.GetDoubleValue(section_haxValues.c_str(), "forge_gun_shoot_force", g_forgeGunShootForce);
	sub::GravityGun_catind::Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "gravity_gun", sub::GravityGun_catind::Enabled());
	sub::GravityGun_catind::ShootForce() = ini.GetDoubleValue(section_haxValues.c_str(), "gravity_gun_shoot_force", sub::GravityGun_catind::ShootForce());
	MagnetGun::g_magnetGun.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "magnet_gun", MagnetGun::g_magnetGun.Enabled());
	teleportGun = ini.GetBoolValue(section_haxValues.c_str(), "teleport_gun", teleportGun);
	lightGun = ini.GetBoolValue(section_haxValues.c_str(), "light_gun", lightGun);
	RopeGun::g_ropeGun.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "rope_gun", RopeGun::g_ropeGun.Enabled());
	bulletTime = ini.GetBoolValue(section_haxValues.c_str(), "bullet_time", bulletTime);
	weaponDamageIncrease = ini.GetDoubleValue(section_haxValues.c_str(), "weapon_damage_multiplier", weaponDamageIncrease);
	explosiveRounds = ini.GetBoolValue(section_haxValues.c_str(), "explosive_bullets", explosiveRounds);
	flamingRounds = ini.GetBoolValue(section_haxValues.c_str(), "flaming_bullets", flamingRounds);
	explosiveMelee = ini.GetBoolValue(section_haxValues.c_str(), "explosive_melee", explosiveMelee);
	tripleBullets = ini.GetBoolValue(section_haxValues.c_str(), "triple_bullets", tripleBullets);
	selfTriggerbot = ini.GetBoolValue(section_haxValues.c_str(), "self_triggerbot", selfTriggerbot);
	rapidFire = ini.GetBoolValue(section_haxValues.c_str(), "rapid_fire", rapidFire);
	soulSwitchGun = ini.GetBoolValue(section_haxValues.c_str(), "soul_switch_gun", soulSwitchGun);
	selfResurrectionGun = ini.GetBoolValue(section_haxValues.c_str(), "self_revival_gun", selfResurrectionGun);
	selfDeleteGun = ini.GetBoolValue(section_haxValues.c_str(), "self_delete_gun", selfDeleteGun);
	bitInfiniteAmmo = ini.GetBoolValue(section_haxValues.c_str(), "self_infinite_ammo_clip", bitInfiniteAmmo);
	selfInfiniteParachutes = ini.GetBoolValue(section_haxValues.c_str(), "infinite_parachutes", selfInfiniteParachutes);
	autoKillEnemies = ini.GetLongValue(section_haxValues.c_str(), "auto_kill_enemies", autoKillEnemies);

	sub::LaserSight_catind::bEnabled = ini.GetBoolValue(section_haxValues.c_str(), "laser_sight_toggle", sub::LaserSight_catind::bEnabled);
	sub::LaserSight_catind::_colour.R = ini.GetLongValue(section_haxValues.c_str(), "laser_sight_R", sub::LaserSight_catind::_colour.R);
	sub::LaserSight_catind::_colour.G = ini.GetLongValue(section_haxValues.c_str(), "laser_sight_G", sub::LaserSight_catind::_colour.G);
	sub::LaserSight_catind::_colour.B = ini.GetLongValue(section_haxValues.c_str(), "laser_sight_B", sub::LaserSight_catind::_colour.B);
	sub::LaserSight_catind::_colour.A = ini.GetLongValue(section_haxValues.c_str(), "laser_sight_A", sub::LaserSight_catind::_colour.A);

	selfRefillHealthInCover = ini.GetBoolValue(section_haxValues.c_str(), "player_refill_health_when_in_cover", selfRefillHealthInCover);
	playerInvincibility = ini.GetBoolValue(section_haxValues.c_str(), "player_invincibility", playerInvincibility);
	playerNoRagdoll = ini.GetBoolValue(section_haxValues.c_str(), "player_no_ragdoll", playerNoRagdoll);
	playerSeatbelt = ini.GetBoolValue(section_haxValues.c_str(), "player_seatbelt", playerSeatbelt);
	playerUnlimitedAbility = ini.GetBoolValue(section_haxValues.c_str(), "player_unlimited_special_ab", playerUnlimitedAbility);
	playerAutoClean = ini.GetBoolValue(section_haxValues.c_str(), "player_auto_clean", playerAutoClean);

	superJump = ini.GetBoolValue(section_haxValues.c_str(), "player_super_jump", superJump);
	superRun = ini.GetBoolValue(section_haxValues.c_str(), "player_super_run", superRun);
	superman = ini.GetBoolValue(section_haxValues.c_str(), "player_superman_manual", superman);
	supermanAuto = ini.GetBoolValue(section_haxValues.c_str(), "player_superman_auto", supermanAuto);
	forceField = ini.GetLongValue(section_haxValues.c_str(), "player_forcefield", forceField);
	SmashAbility::g_smashAbility.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "player_smash_ability", SmashAbility::g_smashAbility.Enabled());
	ignoredByEveryone = ini.GetBoolValue(section_haxValues.c_str(), "player_ignored_by_everyone", ignoredByEveryone);
	neverWanted = ini.GetBoolValue(section_haxValues.c_str(), "player_never_wanted", neverWanted);
	playerBurn = ini.GetBoolValue(section_haxValues.c_str(), "player_burn_mode", playerBurn);

	vehicleInvincibility = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_invincibility", vehicleInvincibility);
	vehicleHeavyMass = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_increased_mass", vehicleHeavyMass);
	raceBoost = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_horn_boost", raceBoost);
	unlimitedVehicleBoost = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_unlim_native_boost", unlimitedVehicleBoost);
	carJump = ini.GetLongValue(section_haxValues.c_str(), "vehicle_jump", carJump);
	carHydraulics = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_hydraulics", carHydraulics);
	superGrip = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_glue_to_ground", superGrip);
	superCarMode = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_SuprKar_mode", superCarMode);
	carColorChange = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_rainbow_mode", carColorChange);
	vehicleFixLoop = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_auto_fix", vehicleFixLoop);
	vehicleFlipLoop = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_auto_flip", vehicleFlipLoop);
	selfEngineOn = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_keep_engine_running", selfEngineOn);
	VehicleTow::g_vehicleTow.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_tow_mode", VehicleTow::g_vehicleTow.Enabled());
	VehicleFly::g_vehicleFly.Enabled() = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_fly_mode", VehicleFly::g_vehicleFly.Enabled());
	multiPlatNeons = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_multiplat_neons", multiPlatNeons);
	multiPlatNeonsRainbow = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_multiplat_neons_rainbow", multiPlatNeonsRainbow);
	g_multiPlatNeonsColor.R = ini.GetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_R", g_multiPlatNeonsColor.R);
	g_multiPlatNeonsColor.G = ini.GetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_G", g_multiPlatNeonsColor.G);
	g_multiPlatNeonsColor.B = ini.GetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_B", g_multiPlatNeonsColor.B);
	accelMult = (float)ini.GetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_acceleration", accelMult);
	brakeMult = (float)ini.GetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_brakes", brakeMult);
	handlingMult = (float)ini.GetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_handling", handlingMult);

	g_addBlip = ini.GetBoolValue(section_haxValues.c_str(), "add_blip_to_vehicle", g_addBlip);
	g_warpNear = ini.GetBoolValue(section_haxValues.c_str(), "warp_vehicle_nearby", g_warpNear);
	g_spawnVehicleDrawBMPs = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_enable_previews", g_spawnVehicleDrawBMPs);
	g_spawnVehiclePlateText = ini.GetValue(section_haxValues.c_str(), "vehicle_spawner_plate_text", g_spawnVehiclePlateText.c_str());
	g_spawnVehiclePlateTexterValue = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_plate_text_texter_value", g_spawnVehiclePlateTexterValue);
	g_spawnVehiclePlateType = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_plate_type", g_spawnVehiclePlateType);
	g_spawnVehicleAutoSit = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_auto_sit", g_spawnVehicleAutoSit);
	g_spawnVehicleAutoUpgrade = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_auto_upgrade", g_spawnVehicleAutoUpgrade);
	g_spawnVehicleInvincible = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_invincible", g_spawnVehicleInvincible);
	g_spawnVehiclePersistent = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_persistent", g_spawnVehiclePersistent);
	g_spawnVehicleDeleteOld = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_delete_old", g_spawnVehicleDeleteOld);
	g_spawnVehicleNeonToggle = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_spawner_neons_on", g_spawnVehicleNeonToggle);
	g_spawnVehicleNeonColor.R = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_R", g_spawnVehicleNeonColor.R);
	g_spawnVehicleNeonColor.G = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_G", g_spawnVehicleNeonColor.G);
	g_spawnVehicleNeonColor.B = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_B", g_spawnVehicleNeonColor.B);
	g_spawnVehiclePrimaryColor = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_primary_colour", g_spawnVehiclePrimaryColor);
	g_spawnVehicleSecondaryColor = ini.GetLongValue(section_haxValues.c_str(), "vehicle_spawner_secondary_colour", g_spawnVehicleSecondaryColor);
	g_LSCCustoms = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_menyoo_customs_lsc", g_LSCCustoms);
	g_vehiclePVOpsName = ini.GetBoolValue(section_haxValues.c_str(), "vehicle_PV_Options_Name", g_vehiclePVOpsName);

	sub::Clock_catind::loop_clock = ini.GetDoubleValue(section_haxValues.c_str(), "clock", sub::Clock_catind::loop_clock);
	sub::Clock_catind::_analogueClockPos.x = (float)ini.GetDoubleValue(section_haxValues.c_str(), "clock_screen_pos_x", sub::Clock_catind::_analogueClockPos.x);
	sub::Clock_catind::_analogueClockPos.y = (float)ini.GetDoubleValue(section_haxValues.c_str(), "clock_screen_pos_y", sub::Clock_catind::_analogueClockPos.y);

	sub::Speedo_catind::loop_speedo = ini.GetLongValue(section_haxValues.c_str(), "speedo", sub::Speedo_catind::loop_speedo);
	sub::Speedo_catind::bit_speedo_mph = ini.GetBoolValue(section_haxValues.c_str(), "speedo_mph_instead_of_kmph", sub::Speedo_catind::bit_speedo_mph);
	sub::Speedo_catind::_speedoPosition.x = (float)ini.GetDoubleValue(section_haxValues.c_str(), "speedo_screen_pos_x", sub::Speedo_catind::_speedoPosition.x);
	sub::Speedo_catind::_speedoPosition.y = (float)ini.GetDoubleValue(section_haxValues.c_str(), "speedo_screen_pos_y", sub::Speedo_catind::_speedoPosition.y);
	sub::Speedo_catind::_currentSpeedoBg.fileName = ini.GetValue(section_haxValues.c_str(), "speedo_bg_name", sub::Speedo_catind::_currentSpeedoBg.fileName.c_str());
	g_unlockMaxIDs = ini.GetBoolValue(section_haxValues.c_str(), "unlock_max_head_ids", g_unlockMaxIDs);
	sub::Speedo_catind::SetCurrentBgIdFromBgNameForConfig();

    // Read camera configuration
    std::string section_freecam = "free-camera";
    FreeCam::defaultSpeed = (float)ini.GetDoubleValue(section_freecam.c_str(), "default_speed", FreeCam::defaultSpeed);
    FreeCam::defaultFov = (float)ini.GetDoubleValue(section_freecam.c_str(), "default_fov", FreeCam::defaultFov);
	FreeCam::defaultSlowSpeed = (float)ini.GetDoubleValue(section_freecam.c_str(), "default_slow_speed", FreeCam::defaultSlowSpeed);
    FreeCam::speedAdjustStep = (float)ini.GetDoubleValue(section_freecam.c_str(), "speed_adjust_step", FreeCam::speedAdjustStep); 
    FreeCam::fovAdjustStep = (float)ini.GetDoubleValue(section_freecam.c_str(), "fov_adjust_step", FreeCam::fovAdjustStep);
    FreeCam::minSpeed = (float)ini.GetDoubleValue(section_freecam.c_str(), "min_speed", FreeCam::minSpeed);
    FreeCam::maxSpeed = (float)ini.GetDoubleValue(section_freecam.c_str(), "max_speed", FreeCam::maxSpeed);
    FreeCam::minFov = (float)ini.GetDoubleValue(section_freecam.c_str(), "min_fov", FreeCam::minFov);
    FreeCam::maxFov = (float)ini.GetDoubleValue(section_freecam.c_str(), "max_fov", FreeCam::maxFov);
}

void MenuConfig::SaveConfig()
{
	auto& ini = MenuConfig::iniFile;


	std::string section_settings = "settings";/////////


	ini.SetBoolValue(section_settings.c_str(), "sync_with_config_at_intervals", MenuConfig::bSaveAtIntervals);
	ini.SetBoolValue(section_settings.c_str(), "DeathModelReset", checkSelfDeathModel);
	ini.SetLongValue(section_settings.c_str(), "open_key", menubinds);
	ini.SetLongValue(section_settings.c_str(), "open_button_for_gamepad_1", menubindsGamepad.first);
	ini.SetLongValue(section_settings.c_str(), "open_button_for_gamepad_2", menubindsGamepad.second);
	ini.SetLongValue(section_settings.c_str(), "manual_respawn_button", respawnbinds);
	ini.SetLongValue(section_settings.c_str(), "stop_animation_key", stopanimbinds);
	ini.SetDoubleValue(section_settings.c_str(), "menuPosX", (menuPos.x + 0.0598f) * 100);
	ini.SetDoubleValue(section_settings.c_str(), "menuPosY", (menuPos.y + 0.074f) * 100);
	ini.SetBoolValue(section_settings.c_str(), "Titlebox_Globe", Menu::bit_glare_test);
	ini.SetBoolValue(section_settings.c_str(), "centre_title", Menu::bit_centre_title);
	ini.SetBoolValue(section_settings.c_str(), "centre_options", Menu::bit_centre_options);
	ini.SetBoolValue(section_settings.c_str(), "centre_breaks", Menu::bit_centre_breaks);
	ini.SetValue(section_settings.c_str(), "language", Language::configLangName.c_str());
	ini.SetLongValue(section_settings.c_str(), "log level", g_loglevel);


	std::string section_general = "general";/////////


	ini.SetLongValue(section_general.c_str(), "FreeCamButton", BindNoClip);


	std::string section_colours = "colours";/////////


	ini.SetBoolValue(section_colours.c_str(), "gradients", Menu::gradients);
	ini.SetBoolValue(section_colours.c_str(), "rainbow_mode", rainbowBoxes);
	ini.SetBoolValue(section_colours.c_str(), "thin_line_over_footer", Menu::thinLineOverScrect);

	ini.SetLongValue(section_colours.c_str(), "titlebox_R", titlebox.R);
	ini.SetLongValue(section_colours.c_str(), "titlebox_G", titlebox.G);
	ini.SetLongValue(section_colours.c_str(), "titlebox_B", titlebox.B);
	ini.SetLongValue(section_colours.c_str(), "titlebox_A", titlebox.A);

	ini.SetLongValue(section_colours.c_str(), "BG_R", BG.R);
	ini.SetLongValue(section_colours.c_str(), "BG_G", BG.G);
	ini.SetLongValue(section_colours.c_str(), "BG_B", BG.B);
	ini.SetLongValue(section_colours.c_str(), "BG_A", BG.A);

	ini.SetLongValue(section_colours.c_str(), "titletext_R", titletext.R);
	ini.SetLongValue(section_colours.c_str(), "titletext_G", titletext.G);
	ini.SetLongValue(section_colours.c_str(), "titletext_B", titletext.B);
	ini.SetLongValue(section_colours.c_str(), "titletext_A", titletext.A);

	ini.SetLongValue(section_colours.c_str(), "optiontext_R", optiontext.R);
	ini.SetLongValue(section_colours.c_str(), "optiontext_G", optiontext.G);
	ini.SetLongValue(section_colours.c_str(), "optiontext_B", optiontext.B);
	ini.SetLongValue(section_colours.c_str(), "optiontext_A", optiontext.A);

	ini.SetLongValue(section_colours.c_str(), "selectedtext_R", selectedtext.R);
	ini.SetLongValue(section_colours.c_str(), "selectedtext_G", selectedtext.G);
	ini.SetLongValue(section_colours.c_str(), "selectedtext_B", selectedtext.B);
	ini.SetLongValue(section_colours.c_str(), "selectedtext_A", selectedtext.A);

	ini.SetLongValue(section_colours.c_str(), "optionbreaks_R", optionbreaks.R);
	ini.SetLongValue(section_colours.c_str(), "optionbreaks_G", optionbreaks.G);
	ini.SetLongValue(section_colours.c_str(), "optionbreaks_B", optionbreaks.B);
	ini.SetLongValue(section_colours.c_str(), "optionbreaks_A", optionbreaks.A);

	ini.SetLongValue(section_colours.c_str(), "optioncount_R", optioncount.R);
	ini.SetLongValue(section_colours.c_str(), "optioncount_G", optioncount.G);
	ini.SetLongValue(section_colours.c_str(), "optioncount_B", optioncount.B);
	ini.SetLongValue(section_colours.c_str(), "optioncount_A", optioncount.A);

	ini.SetLongValue(section_colours.c_str(), "selectionhi_R", selectionhi.R);
	ini.SetLongValue(section_colours.c_str(), "selectionhi_G", selectionhi.G);
	ini.SetLongValue(section_colours.c_str(), "selectionhi_B", selectionhi.B);
	ini.SetLongValue(section_colours.c_str(), "selectionhi_A", selectionhi.A);

	ini.SetLongValue(section_colours.c_str(), "pedTrackers_R", _globalPedTrackers_Col.R);
	ini.SetLongValue(section_colours.c_str(), "pedTrackers_G", _globalPedTrackers_Col.G);
	ini.SetLongValue(section_colours.c_str(), "pedTrackers_B", _globalPedTrackers_Col.B);
	ini.SetLongValue(section_colours.c_str(), "pedTrackers_A", _globalPedTrackers_Col.A);


	std::string section_fonts = "fonts";/////////


	ini.SetLongValue(section_fonts.c_str(), "title", font_title);
	ini.SetLongValue(section_fonts.c_str(), "options", font_options);
	ini.SetLongValue(section_fonts.c_str(), "selection", font_selection);
	ini.SetLongValue(section_fonts.c_str(), "breaks", font_breaks);
	ini.SetLongValue(section_fonts.c_str(), "font_xyzh", font_xyzh);
	ini.SetLongValue(section_fonts.c_str(), "font_speedo", font_speedo);


	std::string section_spooner = "object-spooner";/////////

	ini.SetLongValue(section_spooner.c_str(), "SpoonerModeHotkey", sub::Spooner::SpoonerMode::bindsKeyboard);
	ini.SetLongValue(section_spooner.c_str(), "SpoonerModeGamepadBind_1", sub::Spooner::SpoonerMode::bindsGamepad.first);
	ini.SetLongValue(section_spooner.c_str(), "SpoonerModeGamepadBind_2", sub::Spooner::SpoonerMode::bindsGamepad.second);
	ini.SetDoubleValue(section_spooner.c_str(), "CameraMovementSensitivityKeyboard", sub::Spooner::Settings::CameraMovementSensitivityKeyboard);
	ini.SetDoubleValue(section_spooner.c_str(), "CameraRotationSensitivityMouse", sub::Spooner::Settings::CameraRotationSensitivityMouse);
	ini.SetDoubleValue(section_spooner.c_str(), "CameraMovementSensitivityGamepad", sub::Spooner::Settings::CameraMovementSensitivityGamepad);
	ini.SetDoubleValue(section_spooner.c_str(), "CameraRotationSensitivityGamepad", sub::Spooner::Settings::CameraRotationSensitivityGamepad);
	ini.SetBoolValue(section_spooner.c_str(), "ShowModelPreviews", sub::Spooner::Settings::bShowModelPreviews);
	ini.SetBoolValue(section_spooner.c_str(), "ShowBoxAroundSelectedEntity", sub::Spooner::Settings::bShowBoxAroundSelectedEntity);
	ini.SetBoolValue(section_spooner.c_str(), "SpawnDynamicProps", sub::Spooner::Settings::bSpawnDynamicProps);
	ini.SetBoolValue(section_spooner.c_str(), "SpawnDynamicPeds", sub::Spooner::Settings::bSpawnDynamicPeds);
	ini.SetBoolValue(section_spooner.c_str(), "SpawnDynamicVehicles", sub::Spooner::Settings::bSpawnDynamicVehicles);
	ini.SetBoolValue(section_spooner.c_str(), "FreezeEntityWhenMovingIt", sub::Spooner::Settings::bFreezeEntityWhenMovingIt);
	ini.SetBoolValue(section_spooner.c_str(), "SpawnInvincibleEntities", sub::Spooner::Settings::bSpawnInvincibleEntities);
	ini.SetBoolValue(section_spooner.c_str(), "SpawnStillPeds", sub::Spooner::Settings::bSpawnStillPeds);
	ini.SetBoolValue(section_spooner.c_str(), "AddToDbAsMissionEntities", sub::Spooner::Settings::bAddToDbAsMissionEntities);
	ini.SetBoolValue(section_spooner.c_str(), "TeleportToReferenceWhenLoadingFile", sub::Spooner::Settings::bTeleportToReferenceWhenLoadingFile);
	ini.SetBoolValue(section_spooner.c_str(), "KeepPositionWhenAttaching", sub::Spooner::Settings::bKeepPositionWhenAttaching);
	ini.SetLongValue(section_spooner.c_str(), "SpoonerModeMethod", (UINT8)sub::Spooner::Settings::SpoonerModeMode);


	std::string section_haxValues = "hax-values";/////////


										   //ini.SetBoolValue(section_haxValues.c_str(), "hide_hud", loop_hide_hud);
	ini.SetBoolValue(section_haxValues.c_str(), "show_full_hud", showFullHUD);
	ini.SetBoolValue(section_haxValues.c_str(), "manual_respawn", ManualRespawn::g_manualRespawn.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "freecam", noClip);
	ini.SetBoolValue(section_haxValues.c_str(), "display_xyzh_coords", xyzhCoords);
	ini.SetBoolValue(section_haxValues.c_str(), "display_fps", FPSCounter::bDisplayFps);
	ini.SetBoolValue(section_haxValues.c_str(), "basic_tv_player", sub::TVChannelStuff_catind::loop_basictv);
	ini.SetBoolValue(section_haxValues.c_str(), "sync_clock", syncClock);
	ini.SetBoolValue(section_haxValues.c_str(), "pause_clock", pauseClock);
	ini.SetLongValue(section_haxValues.c_str(), "pause_clock_hour", pauseClockH);
	ini.SetLongValue(section_haxValues.c_str(), "pause_clock_minute", pauseClockM);
	ini.SetBoolValue(section_haxValues.c_str(), "decreased_ped_population", pedPopulation);
	ini.SetBoolValue(section_haxValues.c_str(), "decreased_veh_population", vehiclePopulation);
	ini.SetBoolValue(section_haxValues.c_str(), "decreased_weapon_pickups", clearWeaponPickups);
	ini.SetDoubleValue(section_haxValues.c_str(), "clear_area_radius", g_clearAreaRadius);
	ini.SetBoolValue(section_haxValues.c_str(), "restricted_area_access", restrictedAreasAccess);
	ini.SetBoolValue(section_haxValues.c_str(), "fireworks_ahoy", fireworksDisplay);
	ini.SetBoolValue(section_haxValues.c_str(), "emp_mode", blackoutMode);
	ini.SetBoolValue(section_haxValues.c_str(), "simple_blackout_mode", simpleBlackoutMode);
	ini.SetBoolValue(section_haxValues.c_str(), "massacre_mode", massacreMode);
	ini.SetBoolValue(section_haxValues.c_str(), "jump_around_mode", JumpAroundMode::bEnabled);
	ini.SetLongValue(section_haxValues.c_str(), "frozen_radio_station", g_frozenRadioStation);
	ini.SetBoolValue(section_haxValues.c_str(), "snow_on_terrain", g_spSnow.IsSnow());
	ini.SetBoolValue(section_haxValues.c_str(), "animal_riding_mode", sub::AnimalRiding_catind::Enabled());

	ini.SetBoolValue(section_haxValues.c_str(), "forge_gun", forgeGun);
	ini.SetDoubleValue(section_haxValues.c_str(), "forge_gun_shoot_force", g_forgeGunShootForce);
	ini.SetBoolValue(section_haxValues.c_str(), "gravity_gun", sub::GravityGun_catind::Enabled());
	ini.SetDoubleValue(section_haxValues.c_str(), "gravity_gun_shoot_force", sub::GravityGun_catind::ShootForce());
	ini.SetBoolValue(section_haxValues.c_str(), "magnet_gun", MagnetGun::g_magnetGun.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "teleport_gun", teleportGun);
	ini.SetBoolValue(section_haxValues.c_str(), "light_gun", lightGun);
	ini.SetBoolValue(section_haxValues.c_str(), "rope_gun", RopeGun::g_ropeGun.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "bullet_time", bulletTime);
	ini.SetDoubleValue(section_haxValues.c_str(), "weapon_damage_multiplier", weaponDamageIncrease);
	ini.SetBoolValue(section_haxValues.c_str(), "explosive_bullets", explosiveRounds);
	ini.SetBoolValue(section_haxValues.c_str(), "flaming_bullets", flamingRounds);
	ini.SetBoolValue(section_haxValues.c_str(), "explosive_melee", explosiveMelee);
	ini.SetBoolValue(section_haxValues.c_str(), "triple_bullets", tripleBullets);
	ini.SetBoolValue(section_haxValues.c_str(), "self_triggerbot", selfTriggerbot);
	ini.SetBoolValue(section_haxValues.c_str(), "rapid_fire", rapidFire);
	ini.SetBoolValue(section_haxValues.c_str(), "soul_switch_gun", soulSwitchGun);
	ini.SetBoolValue(section_haxValues.c_str(), "self_revival_gun", selfResurrectionGun);
	ini.SetBoolValue(section_haxValues.c_str(), "self_delete_gun", selfDeleteGun);
	ini.SetBoolValue(section_haxValues.c_str(), "self_infinite_ammo_clip", bitInfiniteAmmo);
	ini.SetBoolValue(section_haxValues.c_str(), "infinite_parachutes", selfInfiniteParachutes);
	ini.SetLongValue(section_haxValues.c_str(), "auto_kill_enemies", autoKillEnemies);

	ini.SetBoolValue(section_haxValues.c_str(), "laser_sight_toggle", sub::LaserSight_catind::bEnabled);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_R", sub::LaserSight_catind::_colour.R);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_G", sub::LaserSight_catind::_colour.G);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_B", sub::LaserSight_catind::_colour.B);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_A", sub::LaserSight_catind::_colour.A);

	ini.SetBoolValue(section_haxValues.c_str(), "player_refill_health_when_in_cover", selfRefillHealthInCover);
	ini.SetBoolValue(section_haxValues.c_str(), "player_invincibility", playerInvincibility);
	ini.SetBoolValue(section_haxValues.c_str(), "player_no_ragdoll", playerNoRagdoll);
	ini.SetBoolValue(section_haxValues.c_str(), "player_seatbelt", playerSeatbelt);
	ini.SetBoolValue(section_haxValues.c_str(), "player_unlimited_special_ab", playerUnlimitedAbility);
	ini.SetBoolValue(section_haxValues.c_str(), "player_auto_clean", playerAutoClean);
	ini.SetBoolValue(section_haxValues.c_str(), "player_super_jump", superJump);
	ini.SetBoolValue(section_haxValues.c_str(), "player_super_run", superRun);
	ini.SetBoolValue(section_haxValues.c_str(), "player_superman_manual", superman);
	ini.SetBoolValue(section_haxValues.c_str(), "player_superman_auto", supermanAuto);
	ini.SetLongValue(section_haxValues.c_str(), "player_forcefield", forceField);
	ini.SetBoolValue(section_haxValues.c_str(), "player_smash_ability", SmashAbility::g_smashAbility.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "player_ignored_by_everyone", ignoredByEveryone);
	ini.SetBoolValue(section_haxValues.c_str(), "player_never_wanted", neverWanted);
	ini.SetBoolValue(section_haxValues.c_str(), "player_burn_mode", playerBurn);

	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_invincibility", vehicleInvincibility);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_increased_mass", vehicleHeavyMass);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_horn_boost", raceBoost);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_unlim_native_boost", unlimitedVehicleBoost);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_jump", carJump);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_hydraulics", carHydraulics);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_glue_to_ground", superGrip);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_SuprKar_mode", superCarMode);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_rainbow_mode", carColorChange);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_auto_fix", vehicleFixLoop);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_auto_flip", vehicleFlipLoop);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_keep_engine_running", selfEngineOn);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_tow_mode", VehicleTow::g_vehicleTow.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_fly_mode", VehicleFly::g_vehicleFly.Enabled());
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_multiplat_neons", multiPlatNeons);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_multiplat_neons_rainbow", multiPlatNeonsRainbow);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_R", g_multiPlatNeonsColor.R);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_G", g_multiPlatNeonsColor.G);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_multiplat_neons_B", g_multiPlatNeonsColor.B);
	ini.SetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_acceleration", accelMult);
	ini.SetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_brakes", brakeMult);
	ini.SetDoubleValue(section_haxValues.c_str(), "vehicle_multiplier_handling", handlingMult);

	ini.SetBoolValue(section_haxValues.c_str(), "add_blip_to_vehicle", g_addBlip);
	ini.SetBoolValue(section_haxValues.c_str(), "warp_vehicle_nearby", g_warpNear);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_enable_previews", g_spawnVehicleDrawBMPs);
	ini.SetValue(section_haxValues.c_str(), "vehicle_spawner_plate_text", g_spawnVehiclePlateText.c_str());
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_plate_text_texter_value", g_spawnVehiclePlateTexterValue);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_plate_type", g_spawnVehiclePlateType);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_auto_sit", g_spawnVehicleAutoSit);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_auto_upgrade", g_spawnVehicleAutoUpgrade);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_invincible", g_spawnVehicleInvincible);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_persistent", g_spawnVehiclePersistent);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_delete_old", g_spawnVehicleDeleteOld);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_spawner_neons_on", g_spawnVehicleNeonToggle);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_R", g_spawnVehicleNeonColor.R);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_G", g_spawnVehicleNeonColor.G);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_neons_B", g_spawnVehicleNeonColor.B);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_primary_colour", g_spawnVehiclePrimaryColor);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_spawner_secondary_colour", g_spawnVehicleSecondaryColor);
	ini.SetBoolValue(section_haxValues.c_str(), "vehicle_menyoo_customs_lsc", g_LSCCustoms);
	ini.SetLongValue(section_haxValues.c_str(), "vehicle_PV_Options_Name", g_vehiclePVOpsName);

	ini.SetDoubleValue(section_haxValues.c_str(), "clock", sub::Clock_catind::loop_clock);
	ini.SetDoubleValue(section_haxValues.c_str(), "clock_screen_pos_x", sub::Clock_catind::_analogueClockPos.x);
	ini.SetDoubleValue(section_haxValues.c_str(), "clock_screen_pos_y", sub::Clock_catind::_analogueClockPos.y);

	ini.SetLongValue(section_haxValues.c_str(), "speedo", sub::Speedo_catind::loop_speedo);
	ini.SetBoolValue(section_haxValues.c_str(), "speedo_mph_instead_of_kmph", sub::Speedo_catind::bit_speedo_mph);
	ini.SetDoubleValue(section_haxValues.c_str(), "speedo_screen_pos_x", sub::Speedo_catind::_speedoPosition.x);
	ini.SetDoubleValue(section_haxValues.c_str(), "speedo_screen_pos_y", sub::Speedo_catind::_speedoPosition.y);
	ini.SetValue(section_haxValues.c_str(), "speedo_bg_name", sub::Speedo_catind::_currentSpeedoBg.fileName.c_str());
	ini.SetBoolValue(section_haxValues.c_str(), "unlock_max_head_ids", g_unlockMaxIDs);

    // Save camera configuration
    std::string section_freecam = "free-camera";
    ini.SetDoubleValue(section_freecam.c_str(), "default_speed", FreeCam::defaultSpeed);
    ini.SetDoubleValue(section_freecam.c_str(), "default_fov", FreeCam::defaultFov); 
	ini.SetDoubleValue(section_freecam.c_str(), "right_click_slow_speed", FreeCam::defaultSlowSpeed);
    ini.SetDoubleValue(section_freecam.c_str(), "speed_adjust_step", FreeCam::speedAdjustStep);
    ini.SetDoubleValue(section_freecam.c_str(), "fov_adjust_step", FreeCam::fovAdjustStep);
    ini.SetDoubleValue(section_freecam.c_str(), "min_speed", FreeCam::minSpeed);
    ini.SetDoubleValue(section_freecam.c_str(), "max_speed", FreeCam::maxSpeed);
    ini.SetDoubleValue(section_freecam.c_str(), "min_fov", FreeCam::minFov);
    ini.SetDoubleValue(section_freecam.c_str(), "max_fov", FreeCam::maxFov);

	ini.SaveFile((GetPathffA(Pathff::Main, true) + "menyooConfig.ini").c_str());
}

void MenuConfig::ConfigResetHaxValues()
{
	auto& ini = MenuConfig::iniFile;

	std::string section_haxValues = "hax-values";/////////

	CSimpleIniA::TNamesDepend keys;
	ini.GetAllKeys(section_haxValues.c_str(), keys);

	for (auto& key : keys)
	{
		if (std::string(ini.GetValue(section_haxValues.c_str(), key.pItem)).find(".") == std::string::npos)
			ini.SetLongValue(section_haxValues.c_str(), key.pItem, 0);
	}

	ini.SetLongValue(section_haxValues.c_str(), "frozen_radio_station", -1);

	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_R", sub::LaserSight_catind::_colour.R);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_G", sub::LaserSight_catind::_colour.G);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_B", sub::LaserSight_catind::_colour.B);
	ini.SetLongValue(section_haxValues.c_str(), "laser_sight_A", sub::LaserSight_catind::_colour.A);

	ini.SetValue(section_haxValues.c_str(), "speedo_bg_name", sub::Speedo_catind::_currentSpeedoBg.fileName.c_str());
	ini.SetBoolValue(section_haxValues.c_str(), "unlock_max_head_ids", false);

	MenuConfig::ConfigRead();

}


