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
#include <unordered_set>
#include <map>

#define TRUE 1
#define FALSE 0

typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed char INT8;
typedef unsigned char UINT8, BYTE;
typedef int INT, Entity, Ped, Vehicle, Player;
typedef unsigned long DWORD, Hash;
typedef float FLOAT;
typedef char *PCHAR;
typedef const char *LPCSTR;

class RgbS;
class Vector3;
class Camera;

namespace GTAmodel 
{
	class Model;
}
class GTAplayer;
class GTAentity;
class GTAvehicle;
class GTAped;

namespace PTFX 
{
	class sFxData;
}

void ThreadMenyooMain();
void ThreadMenuLoops2();
void TickMenyooConfig();
void TickRainbowFader();

extern INT16 BindNoClip;
extern RgbS g_fadedRGB;
extern UINT8 pauseClockH;
extern UINT8 pauseClockM;
extern Vehicle g_myVeh;
extern GTAmodel::Model g_myVehModel;
extern Hash g_myWeap;
extern PTFX::sFxData triggerFXGunData;
extern Hash kaboomGunHash;
extern Hash bullet_gun_hash;
extern GTAmodel::Model pedGunHash; 
extern GTAmodel::Model objectGunHash;
extern FLOAT currentTimescale;

extern INT g_Ped1; 
extern INT g_Ped2;
extern INT g_Ped3;
extern INT g_Ped4;

extern const char* g_PlayerName;
extern INT bitMSPaintsRGBMode;
extern bool kaboomGunInvis;
extern bool kaboomGunRandBit;
extern bool pedGunRandBit;
extern bool objectGunRandBitO; 
extern bool objectGunRandBitV;
extern bool bitNightVision;

extern FLOAT swimSpeedMult;
extern FLOAT playerNoiseMult;
extern FLOAT selfSweatMult;
extern FLOAT g_playerVerticalElongationMultiplier;
extern FLOAT vehicleDamageAndDefense;
extern FLOAT vehicleSlam;
extern FLOAT currentTimecycleStrength;

extern INT accelMult;
extern INT brakeMult;
extern INT handlingMult;

extern INT16 g_frozenRadioStation;
extern bool bitVehicleGravity;
extern bool bitFreezeVehicle;
extern bool bitVehicleSlippyTires;

extern std::array<int, 3> GetHSVFromRGB(int r, int g, int b);
extern float NormalizeHSV(int h, int s, int v);

extern INT msCurrentPaintIndex;

// String variables used in various submenus for search, storage, etc.
extern INT16 BindNoClip;
extern std::string dict;
extern std::string dict2;
extern std::string dict3;

extern std::string g_spawnVehiclePlateText;
extern INT8 g_spawnVehiclePlateType;
extern INT8 g_spawnVehiclePlateTexterValue;
extern RgbS g_spawnVehicleNeonColor;
extern bool g_spawnVehicleAutoSit;
extern bool g_warpNear;
extern bool g_addBlip;
extern bool g_spawnVehicleAutoUpgrade; 
extern bool g_spawnVehicleInvincible;
extern bool g_spawnVehiclePersistent;
extern bool g_spawnVehicleDeleteOld;
extern bool g_spawnVehicleNeonToggle;
extern bool g_LSCCustoms;
extern bool g_vehiclePVOpsName;


extern INT16 g_spawnVehiclePrimaryColor;
extern INT16 g_spawnVehicleSecondaryColor;
extern bool g_spawnVehicleDrawBMPs;
extern FLOAT g_clearAreaRadius;
extern FLOAT g_rainFXIntensity;

extern Entity g_driveWaterObject;

extern INT8 spectatePlayer;
extern UINT8 explostionWP;

extern bool multiPlatNeons;
extern bool multiPlatNeonsRainbow;

extern RgbS g_multiPlatNeonsColor;
extern std::vector<GTAvehicle> g_multiPlatNeonsList;

extern bool checkSelfDeathModel;

extern UINT8 carJump;
extern UINT8 autoKillEnemies;
extern float weaponDamageIncrease;

extern UINT8 forceField;
extern UINT8 selfFreezeWantedLevel;
extern Entity bitInfiniteAmmoEnth;

extern bool rainbowBoxes;
extern bool forgeGun;
extern bool playerNoRagdoll;
extern bool playerSeatbelt;
extern bool playerUnlimitedAbility;
extern bool playerAutoClean;
extern bool playerWalkUnderwater;
extern bool explosiveRounds;
extern bool flamingRounds;
extern bool teleportGun;
extern bool kaboomGun;
extern bool triggerFXGun;
extern bool bulletGun;
extern bool pedGun;
extern bool objectGun;
extern bool lightGun;
extern bool bulletTime;
extern bool selfTriggerbot;
extern bool explosiveMelee;
extern bool superJump;
extern bool selfRefillHealthInCover;
extern bool playerInvincibility;
extern bool noClip;
extern bool noClipToggle;
extern bool superRun;
extern bool xyzhCoords;
extern bool ignoredByEveryone;
extern bool neverWanted;
extern bool superman;
extern bool supermanAuto;
extern bool vehiclePopulation;
extern bool pedPopulation;
extern bool clearWeaponPickups;
extern bool driveOnWater;
extern bool massacreMode;
extern bool playerBurn;
extern bool vehicleInvincibility;
extern bool vehicleHeavyMass;
extern bool raceBoost;
extern bool carHydraulics;
extern bool superGrip;
extern bool superCarMode;
extern bool unlimitedVehicleBoost;
extern bool vehicleWeaponLines;
extern bool vehicleRPG;
extern bool vehicleFireworks;
extern bool vehicleGuns;
extern bool vehicleSnowballs;
extern bool vehicleBalls;
extern bool vehicleWaterHydrant;
extern bool vehicleLaserGreen;
extern bool vehicleFlameLeak;
extern bool vehicleLaserRed;
extern bool vehicleTurretsValkyrie;
extern bool vehicleFlaregun;
extern bool vehicleHeavySniper;
extern bool vehicleTazerWeapon;
extern bool vehicleMolotovWeapon;
extern bool vehicleCombatPDW;
extern bool carColorChange;
extern bool vehicleInvisibility;
extern bool selfEngineOn;
extern bool hideHUD;
extern bool showFullHUD;
extern bool pauseClock;
extern bool syncClock;
extern bool tripleBullets;
extern bool rapidFire;
extern bool selfResurrectionGun;
extern bool soulSwitchGun;
extern bool selfDeleteGun;
extern bool vehicleFixLoop;
extern bool vehicleFlipLoop;
extern bool blackoutMode;
extern bool simpleBlackoutMode;
extern bool restrictedAreasAccess;
extern bool hvSnipers;
extern bool vehicleDisableSiren;
extern bool fireworksDisplay;
extern bool bitInfiniteAmmo;
extern bool selfInfiniteParachutes;

extern Entity targetSlotEntity;
extern bool targetEntityLocked;

extern bool bitGravityGunDisabled;

extern float forgeDist;
extern float g_forgeGunPrecision;
extern float g_forgeGunShootForce;
extern bool objectSpawnForgeAssistance;

extern bool g_unlockMaxIDs;
extern UINT8 max_shapeAndSkinIDs;

void DisplayFullHUDThisFrame(bool bEnabled);
void UpdateNearbyStuffArraysTick();
void SetPauseMenuTeleToWpCommand();

void SetPTFXLopTick();
void SetSyncClockTime();
void SetMassacreModeTick();
void SetBlackoutEMPMode();
void SetBlackoutMode();
void SetSelfNearbyPedsCalm();
void NetworkSetEveryoneIgnorePlayer(Player player);
void SetExplosionAtCoords(GTAentity entity, Vector3 pos, UINT8 type, float radius, float camshake, bool sound, bool invis, GTAentity owner);
void StartFireworksAtCoords(const Vector3& pos, const Vector3& rot, float scale);
void SetTargetIntoSlot();
void SetPlayerTriggerbot(GTAplayer player);
void SetRapidFire();
void SetSoulSwitchGun();
void SetSelfDeleteGun();
void SetSelfResurrectionGun();
void SetHVSnipers(bool set);
void SetTeleportGun();
void SetBulletGun();
void SetPedGun();
void SetObjectGun();
void SetLightGun();
void SetTripleBullets();
void SetForgeGunDist(float& distance);
inline void SetForgeGunRotationHotKeys();
void SetForgeGun();
void SetExplosionAtBulletHit(Ped ped, Hash type, bool invisible);
void SetTriggerFXAtBulletHit(Ped ped, const std::string& fxAsset, const std::string& fxName, const Vector3& Rot, float scale);
void SetBecomePed(GTAped ped);
void SetPedInvincibleOn(Ped ped);
void SetPedInvincibleOff(Ped ped);
void SetPedNoRagdollOn(Ped ped);
void SetPedNoRagdollOff(Ped ped);
void SetPedSeatbeltOn(Ped ped);
void SetPedSeatbeltOff(Ped ped);

extern bool bitNoclipAlreadyInvisible, bitNoclipAlreadyCollision, bitNoclipShowHelp;
extern Camera g_cam_noClip;
void SetNoclipOff1();
void SetNoclipOff2();
void SetNoclip();
void SetLocalButtonSuperRun();
void SetSelfRefillHealthWhenInCover();
void xyzhDrawFloat(float text, float x_coord, float y_coord);
void XYZH();
void SetLocalSupermanManual();
void SetPedSupermanAuto(Ped ped);
void SetVehicleNosPTFXThisFrame(GTAvehicle vehicle);
void SetSuperCarModeSelf();
void SetLocalCarJump();
void SetLocalCarHydraulics();
void SetLocalForcefield();

extern float g_multiPlatNeonsIntensity;
void DrawVehicleAmbientLightNeons(const GTAvehicle& vehicle, const RgbS& colour);
void SetMultiPlatNeons();
void DriveOnWater(GTAped ped, Entity& waterobject);
void SetPedBurnMode(GTAped ped, bool enable);
void SetVehicleInvincibleOn(Vehicle vehicle);
void SetVehicleInvincibleOff(Vehicle vehicle);
void SetVehicleFlip(GTAvehicle vehicle);
void SetVehicleRainbowMode(GTAvehicle vehicle, bool useFader);
void SetVehicleHeavyMass(GTAvehicle vehicle);
void SetSelfVehicleBoost();

extern GTAvehicle pvSubVehicleID;
void SetPVOpsVehicleTextWorld2Screen();

extern std::map<Vehicle, float> g_multListRPM;
extern std::map<Vehicle, float> g_multListTorque;
extern std::map<Vehicle, float> g_multListMaxSpeed;
extern std::map<Vehicle, float> g_multListHeadLights;
inline void VehicleTorqueMultiplier();
inline void VehicleMaxSpeedMultiplier();

extern std::map<Vehicle, std::string> g_vehListEngineSounds;
std::string GetVehicleEngineSoundName(const GTAvehicle& vehicle);
void SetVehicleEngineSoundName(GTAvehicle vehicle, const std::string& name);

extern std::unordered_set<Vehicle> g_vehWheelsInvisForRussian;
bool AreVehicleWheelsInvisible(const GTAvehicle& vehicle);
void SetVehicleWheelsInvisible(GTAvehicle vehicle, bool enable);

extern std::map<Ped, std::string> g_pedListMovGroup;
extern std::map<Ped, std::string> g_pedListWMovGroup;
extern std::map<Ped, std::string> g_pedListFacialMood;
extern std::string GetPedFacialMood(GTAentity ped);
void SetPedFacialMood(GTAentity ped, const std::string& animName);
void clear_ped_facial_mood(GTAentity ped);

enum class WeaponTargetType
{
    TargetPlayer,
    TargetPed
};

inline WeaponTargetType g_WeaponTargetType = WeaponTargetType::TargetPlayer;
inline Ped g_WeaponTargetPed = 0;
struct ScopedWeaponTargetOverride
{
    WeaponTargetType oldType;
    Ped oldPed;

    ScopedWeaponTargetOverride(Ped ped)
    {
        oldType = g_WeaponTargetType;
        oldPed = g_WeaponTargetPed;

        g_WeaponTargetType = WeaponTargetType::TargetPed;
        g_WeaponTargetPed = ped;
    }

    ~ScopedWeaponTargetOverride()
    {
        g_WeaponTargetType = oldType;
        g_WeaponTargetPed = oldPed;
    }
};

extern int GetRandomSpriteId();