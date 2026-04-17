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

//#include "Euphoria.h"
#include "GTAentity.h"
#include "Tasks.h"

#include <string>
#include <vector>

typedef unsigned long DWORD, Hash;
typedef int BOOL;

class GTAvehicle;
class Vector3;
enum class ParachuteState : int;
enum class PedSubTask : int;
enum class NMString : int;
enum VehicleSeat : int;
namespace FiringPattern {
	enum FiringPattern : DWORD;
}
class s_Weapon_Components_Tint;
//class GTAentity;
//class Tasks;

enum class Gender
{
	Male,
	Female
};
enum class FormationType
{
	Default = 0,
	Circle1 = 1,
	Circle2 = 2,
	Line = 3
};
enum class PedHeadOverlay : int
{
	SkinRash = 0,
	Beard = 1,
	Eyebrows = 2,
	Wrinkles = 3,
	Makeup = 4,
	Blush = 5,
	Pigment1 = 6,
	Pigment2 = 7,
	Lipstick = 8,
	Spots = 9,
	ChestHair = 10,
	Chest1 = 11,
	Chest2 = 12,
};
enum class PedFacialFeature
{
	NoseWidth = 0,
	NoseBottomHeight = 1,
	NoseTipLength = 2,
	NoseBridgeDepth = 3,
	NoseTipHeight = 4,
	NoseBroken = 5,
	BrowHeight = 6,
	BrowDepth = 7,
	CheekboneHeight = 8,
	CheekboneWidth = 9,
	CheekDepth = 10,
	EyeSize = 11,
	LipThickness = 12,
	JawWidth = 13,
	JawShape = 14,
	ChinHeight = 15,
	ChinDepth = 16,
	ChinWidth = 17,
	ChinIndent = 18,
	NeckWidth = 19,
};

class PedHeadBlendData
{
public:
	int shapeFirstID;
private:
	DWORD _padding0;
public:
	int shapeSecondID;
private:
	DWORD _padding1;
public:
	int shapeThirdID;
private:
	DWORD _padding2;
public:
	int skinFirstID;
private:
	DWORD _padding3;
public:
	int skinSecondID;
private:
	DWORD _padding4;
public:
	int skinThirdID;
private:
	DWORD _padding5;
public:
	float shapeMix;
private:
	DWORD _padding6;
public:
	float skinMix;
private:
	DWORD _padding7;
public:
	float thirdMix;
private:
	DWORD _padding8;
public:
	BOOL isParent;
private:
	DWORD _padding9;
};

class PedGroup
{
public:
	PedGroup();
	PedGroup(int handle);
	//~PedGroup();

	PedGroup operator =(const PedGroup& right);
	bool operator ==(const PedGroup& right) const;
	bool operator !=(const PedGroup& right) const;

	static PedGroup CreateNewGroup();

	int& Handle();
	int GetHandle() const;

	GTAentity GetLeader() const;
	void SetLeader(GTAentity ped);

	int MemberCount() const;

	void SetSeparationRange(float range);

	void SetFormationSpacing(float spacing);
	void ResetFormationSpacing();

	void SetFormationType(FormationType value);

	void Add(GTAentity ped, bool leader, bool teleportWithLeader = false);
	void Remove(GTAentity ped);

	bool Exists() const;
	static bool Exists(PedGroup pedGroup);

	GTAentity GetMember(int index) const;

	bool Contains(GTAentity ped) const;

	void ToVector(std::vector<GTAentity>& result, bool includeLeader) const;

private:
	int _handle;
};

namespace PedBloodDecals {
	extern const std::vector<std::string> vBloodDecals;
}
namespace PedDamageDecals {
	extern const std::vector<std::string> vDamageDecals;
}
namespace PedDamagePacks {
	extern const std::vector<std::string> vDamagePacks;
}

class GTAped : public GTAentity
{
public:
	GTAped();
	GTAped(int handle);
	GTAped(GTAentity handle);

	GTAped& operator = (const GTAped& value);
	friend bool operator == (const GTAped& left, const GTAped& right);

	void Delete(bool tele = false) override;

	int NetID() const override;

	void GiveNM(const NMString& messageid);

	GTAentity Clone(float heading, bool createNetH, bool createPedH);

	bool IsSubTaskActive(const PedSubTask& taskType);

	PedHeadBlendData GetHeadBlendData() const;
	void SetHeadBlendData(const PedHeadBlendData& blendData);

	void SetVoiceName(const std::string& value);

	int PedType() const;

	void PlaySpeechWithVoice(const std::string& speechName, const std::string& voiceName, const std::string& speechParam, bool unk = 0);

	Hash GetWeapon() const;
	void SetWeapon(Hash weaponHash);
	void RemoveAllWeapons();

	Vector3 LastWeaponImpactCoord() const;

	ParachuteState GetParachuteState() const;
	
	int GetAccuracy() const;
	void SetAccuracy(int value);

	void SetAlwaysDiesWhenInjured(bool value);

	void SetAlwaysKeepTask(bool value);

	int GetArmour() const;
	void SetArmour(int value);

	void SetBlockPermanentEvent(bool value);

	bool GetCanRagdoll() const;
	void SetCanRagdoll(bool value);

	void SetCanSwitchWeapons(bool value);

	void SetCanSufferCriticalHits(bool value);

	bool GetCanFlyThroughWindscreen() const;
	void SetCanFlyThroughWindscreen(bool value);

	void SetCanBeKnockedOffBike(int state);

	void SetCanBeDraggedOutOfVehicle(bool value);

	void SetCanBeTargetted(bool value);

	void SetCanPlayGestures(bool value);

	bool IsInGroup() const;
	PedGroup GetCurrentPedGroup() const;
	void SetNeverLeavesGroup(bool value);
	void SetTeleportsWithGroupLeader(bool value, const PedGroup& grp = 0);

	void SetDiesInstantlyInWater(bool value);
	void SetDrownsInWater(bool value);
	void SetDrownsInSinkingVehicle(bool value);

	void SetDrivingSpeed(float value);
	void SetMaxDrivingSpeed(float value);

	void SetDrivingStyle(int value);

	//NaturalMotion::Euphoria Euphoria_get() const;

	void SetFiringPattern(FiringPattern::FiringPattern value);

	Gender GetGender() const;

	bool IsAimingFromCover() const;

	bool IsBeingJacked() const;

	bool IsBeingStealthKilled() const;
	bool IsPerformingStealthKill() const;

	bool IsBeingStunned() const;

	bool IsDoingDriveBy() const;

	bool IsDucking() const;
	void SetDucking(bool value);

	void SetAsEnemy(bool value);

	bool IsHuman() const;

	bool IsIdle() const;

	bool IsProne() const;

	bool IsGettingUp() const;

	bool IsGettingIntoAVehicle() const;

	bool IsGoingIntoCover() const;

	bool IsInjured() const;

	bool IsInBoat() const;

	bool IsInCombat() const;
	bool IsInCombatWith(const GTAped& target) const;

	bool IsInCoverFacingLeft() const;

	bool IsInFlyingVehicle() const;

	bool IsInHeli() const;
	bool IsInPlane() const;
	bool IsInPoliceVehicle() const;
	bool IsInSub() const;
	bool IsInTrain() const;
	bool IsOnBike() const;
	bool IsOnFoot() const;

	bool IsInMeleeCombat() const;

	bool IsJacking() const;

	bool IsPlayer() const;

	void SetPriorityTargetForEnemies(bool value);

	bool IsRagdoll() const;

	bool IsWalking() const;
	bool IsRunning() const;
	bool IsSprinting() const;
	bool IsJumping() const;

	bool IsShooting() const;
	bool IsReloading() const;

	bool IsStopped() const;

	bool IsSwimming() const;
	bool IsSwimmingUnderWater() const;

	bool IsTryingToEnterALockedVehicle() const;

	int GetMoney() const;
	void SetMoney(int value);

	Hash GetRelationshipGroup() const;
	void SetRelationshipGroup(Hash grpHash);
	void SetRelationshipGroup(const std::string& grpName);

	void SetShootRate(int value);

	void SetHasGravity(bool value) override;

	Tasks Task();
	int GetTaskSequenceProgress() const;
	bool IsPerformingAnyTaskSequence() const;

	bool WasKilledByStealth() const;
	bool WasKilledByTakedown() const;

	void SetWetnessHeight(float value);

	bool IsInVehicle() const;
	bool IsInVehicle(GTAvehicle vehicle) const;
	GTAvehicle CurrentVehicle() const;

	VehicleSeat GetCurrentVehicleSeat();

	int GetRelationshipWithPed(GTAentity otherPed) const;

	void SetIntoVehicle(GTAvehicle vehicle, VehicleSeat seat);

	bool IsInCover() const;
	bool IsInCover(bool expectUseWeapon) const;

	GTAentity GetJacker() const;
	GTAentity GetJackTarget() const;
	GTAentity GetSourceOfDeath() const;
	void Kill();
	void ExplodeHead(Hash weaponHash);

	void ResetVisibleDamage();
	void ClearBloodDamage();
	void ApplyDamage(int damageAmount);

	Vector3 GetBoneCoord(int boneID) const;
	Vector3 GetBoneCoord(int boneID, const Vector3& offset) const;
	int GetBoneIndex(int boneID) const;

	void ApplyBlood(const std::string& bloodDecalName, int boneID);
	void ApplyBlood(const std::string& bloodDecalName, int boneID, const Vector3& rotation);

	void ApplyDamageDecal(const std::string& damageDecalName, int boneID, const Vector3& rotation, float damageScale, float multiplier, int state, bool b1);
	void ApplyDamagePack(const std::string& damagePackName, float damageAmount, float multiplier);

	void StoreWeaponsInArray(std::vector<s_Weapon_Components_Tint>& result);
	void GiveWeaponsFromArray(const std::vector<s_Weapon_Components_Tint>& value);

private:
	Tasks _tasks;
	//NaturalMotion::Euphoria _euphoria;

};



