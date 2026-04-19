/*
* Copyright (C) 2015 crosire
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/
/*
* ALTERED SOURCE
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*/
#pragma once

#include "GTAentity.h"

#include <vector>
#include <string>
#include <map>

typedef unsigned long DWORD, Hash;
typedef unsigned char BYTE, UINT8;
typedef int INT, Ped, Vehicle, Entity;
typedef char *PCHAR;
typedef const char *LPCSTR;

enum VehicleSeat : int;
enum class CargobobHook : int;
enum class VehicleLockStatus: int;
enum class VehicleRoofState : int;
enum class VehicleDoor : int;
enum class VehicleWindow : int;
enum class VehicleNeonLight : int;
enum class VehicleClass : int;
namespace GTAmodel {
	class Model;
}
class RgbS;

extern const std::vector<std::string> vValues_HornNames;
extern const std::vector<std::string> vValues_ModSlotNames;
std::string GetModSlotName(Vehicle vehicle, INT modType, bool gxt);
std::string GetModTextLabel(Vehicle vehicle, INT modType, INT modValue, bool gxt);


class GTAvehicle : public GTAentity
{
public:
	GTAvehicle();
	GTAvehicle(int handle);
	GTAvehicle(const GTAentity& obj);

	GTAvehicle& operator = (const GTAvehicle& value);
	friend bool operator == (const GTAvehicle& left, const GTAvehicle& right);

	void Delete(bool tele = false) override;

	int NetID() const override;

	bool IsDamaged() const;
	void Fix();
	void Repair(bool checkIfDamaged = true);

	void SetEngineSound(const std::string& vehName);

	void SetFrictionOverride(float value);

	bool HasRoof() const;

	int MaxPassengers() const;

	GTAentity GetPedOnSeat(VehicleSeat seat) const;
	bool IsSeatFree(VehicleSeat seat) const;
	GTAentity GetDriver() const;
	std::vector<GTAentity> GetOccupants(bool bIncludeDriver = true) const;
	VehicleSeat FirstFreeSeat(VehicleSeat defaultReturn) const;

	std::string GetNumberPlateText() const;
	void SetNumberPlateText(const std::string& value);
	int GetNumberPlateTextIndex() const;
	void SetNumberPlateTextIndex(int value);

	bool IsConvertible() const;

	bool GetStolen() const;
	void SetStolen(bool value);

	bool GetDriveable() const;
	void SetDriveable(bool value);

	bool IsStopped() const;
	bool IsStoppedAtTrafficLights() const;
	void StopImmediately();

	float GetMaxBraking() const;
	float GetMaxTraction() const;

	bool IsOnAllWheels() const;

	void SetSpeed(float value);
	void SetForwardSpeed(float value);

	void SetMaxSpeed(float value);

	float GetDirtLevel() const;
	void SetDirtLevel(float value);
	float GetPaintFade() const;
	void SetPaintFade(float value);

	VehicleRoofState GetRoofState() const;
	void SetRoofState(VehicleRoofState value);

	float GetBodyHealth() const;
	void SetBodyHealth(float value);

	float GetEngineHealth() const;
	void SetEngineHealth(float value);

	float GetPetrolTankHealth() const;
	void SetPetrolTankHealth(float value);

	bool GetSirenActive() const;
	void SetSirenActive(bool value);

	bool GetLoudRadioActive() const;
	void SetLoudRadioActive(bool value);

	int GetPrimaryColour() const;
	void SetPrimaryColour(int value);

	int GetSecondaryColour() const;
	void SetSecondaryColour(int value);

	int GetRimColour() const;
	void SetRimColour(int value);

	int GetPearlescentColour() const;
	void SetPearlescentColour(int value);

	// For Benny's supported vehicles
	int GetInteriorColour() const;
	void SetInteriorColour(int value);
	int GetDashboardColour() const;
	void SetDashboardColour(int value);
	
	// b1604
	int GetHeadlightColour() const;
	void SetHeadlightColour(int value);

	int GetWheelType() const;
	void SetWheelType(int value);

	int GetWindowTint() const;
	void SetWindowTint(int value);
	;
	bool IsPrimaryColorCustom() const;
	bool IsSecondaryColorCustom() const;

	void SetWanted(bool value);

	void SetHasGravity(bool value) override;

	bool GetEngineRunning() const;
	void SetEngineRunning(bool value);

	void SetEnginePowerMultiplier(float value);
	void SetEngineTorqueMulitplier(float value);

	void SetEngineCanDegrade(bool value);

	bool GetLightsOn() const;
	void SetLightsOn(bool value);

	bool GetHighBeamsOn() const;

	void SetLightsMultiplier(float value);

	bool GetLeftHeadlightBroken() const;
	void SetLeftHeadlightBroken(bool value);
	bool GetRightHeadlightBroken() const;
	void SetRightHeadlightBroken(bool value);

	bool GetIsFrontBumperBrokenOff() const;
	bool GetIsRearBumperBrokenOff() const;

	void SetBrakeLightsOn(bool value);

	void SetHandBrakeOn(bool value);

	void SetLeftIndicatorLightOn(bool value);
	void SetRightIndicatorLightOn(bool value);

	void SetInteriorLightOn(bool value);

	bool GetTaxiLightOn() const;
	void SetTaxiLightOn(bool value);

	bool GetSearchLightOn() const;
	void SetSearchLightOn(bool value);

	bool GetNeedsToBeHotwired() const;
	void SetNeedsToBeHotwired(bool value);

	bool GetCanTyresBurst() const;
	void SetCanTyresBurst(bool value);

	bool GetCanTyresDrift() const;
	void SetCanTyresDrift(bool value);

	void SetCanBeVisiblyDamaged(bool value);

	bool GetPreviouslyOwnedByPlayer() const;
	void SetPreviouslyOwnedByPlayer(bool value);

	RgbS GetCustomPrimaryColour() const;
	void SetCustomPrimaryColour(RgbS value);
	void SetCustomPrimaryColour(BYTE r, BYTE g, BYTE b);

	RgbS GetCustomSecondaryColour() const;
	void SetCustomSecondaryColour(RgbS value);
	void SetCustomSecondaryColour(BYTE r, BYTE g, BYTE b);

	RgbS GetNeonLightsColour() const;
	void SetNeonLightsColour(RgbS value);
	void SetNeonLightsColour(BYTE r, BYTE g, BYTE b);

	RgbS GetTyreSmokeColour() const;
	void SetTyreSmokeColour(RgbS value);
	void SetTyreSmokeColour(BYTE r, BYTE g, BYTE b);

	int GetLivery() const;
	void SetLivery(int liveryIndex);
	int LiveryCount() const;

	bool GetAlarmActive() const;
	void SetAlarmActive(bool value);

	int GetCurrentGear() const;
	int GetHighGear() const;
	void SetHighGear(UINT8 value);
	float GetFuelLevel() const;
	void SetFuelLevel(float value);
	float GetCurrentRPM() const;
	void SetCurrentRPM(float value);
	float GetAcceleration() const;
	float GetWheelSpeed() const;
	float GetSteeringAngle(bool inDegrees=true) const;
	float GetSteeringScale() const;
	void SetSteeringScale(float value);

	void FixDoor(VehicleDoor door);
	void FixAllDoors();

	void SetRadioStation(int value);

	VehicleClass ClassType_get();

	VehicleLockStatus GetLockStatus() const;
	void SetLockStatus(const VehicleLockStatus& value);

	int GetMod(int modType) const;
	void SetMod(int modType, int modIndex, int variation);
	int GetModVariation(int modType) const;
	void SetModVariation(int modType, bool variation);
	void ToggleMod(int modType, bool toggle);
	bool IsToggleModOn(int modType) const;
	void RemoveMod(int modType);

	std::string GetModSlotName(int modType, bool getProperName) const;
	std::string GetModName(int modType, int modIndex, bool getProperName) const;

	void ClearCustomPrimaryColour();
	void ClearCustomSecondaryColour();

	void Explode(bool audible = true, bool visible = true);

	bool PlaceOnGroundProperly();
	void PlaceOnNextStreet();

	std::vector<VehicleDoor> Doors_get() const;
	bool HasBombBay_get() const;
	bool HasForks_get() const;
	bool GetHasSiren() const;
	bool HasTowArm_get() const;

	void OpenDoor(VehicleDoor door, bool loose, bool instantly, bool playSound = true);
	void CloseDoor(VehicleDoor door, bool instantly, bool playSound = true);
	void CloseAllDoors(bool instantly);
	void BreakDoor(VehicleDoor door, bool instant);
	void BreakAllDoors(bool instant);
	float GetDoorAngleRatio(VehicleDoor door) const;
	bool IsDoorOpen(VehicleDoor door) const;
	bool IsDoorFullyOpen(VehicleDoor door) const;
	bool IsDoorBroken(VehicleDoor door) const;
	void SetDoorBreakable(VehicleDoor door, bool isBreakable);

	void OpenBombBay();
	void CloseBombBay();

	bool IsWindowIntact(VehicleWindow window);
	void FixWindow(VehicleWindow window);
	void SmashWindow(VehicleWindow window);
	void RollUpWindow(VehicleWindow window);
	void RollDownWindow(VehicleWindow window);
	void RollDownWindows();
	void RemoveWindow(VehicleWindow window);

	bool IsNeonLightOn(VehicleNeonLight light) const;
	void SetNeonLightOn(VehicleNeonLight light, bool on);

	bool DoesExtraExist(int extraId) const;
	bool GetExtraOn(int extraId) const;
	void SetExtraOn(int extraId, bool value);

	void SoundHorn(int duration);

	void SetHeliYawPitchRollMult(float value);

	void DropCargobobHook(CargobobHook hookType);
	bool IsCargobobHookActive() const;
	bool IsCargobobHookActive(CargobobHook hookType) const;
	void RetractCargobobHook();
	void CargoBobMagnetGrabVehicle();
	void CargoBobMagnetReleaseVehicle();
	
	bool IsBoatAnchored();
	bool CanBoatAnchorHere();
	void AnchorBoat(bool anchored);

	bool IsTyreBursted(int wheel, bool completely = false) const;
	void BurstTyre(int wheel);
	void FixTyre(int wheel);
	bool IsInBurnout() const;
	void SetForceBurnout(bool value);

	void StartAlarm();

	void ApplyDamage(Vector3 loc, float damageAmount, float radius);

	GTAentity CreatePedOnSeat(VehicleSeat seat, GTAmodel::Model model);
	GTAentity CreateRandomPedOnSeat(VehicleSeat seat);
};



// Vehicle - Emblem
//inline bool get_vehicle_info_for_emblem_pos(Model vehicleModel, Vector3& x, Vector3& y, Vector3& z, float& scale);
void add_emblem_to_vehicle(GTAvehicle vehicle, GTAentity playerPed);

// Vehicle - clone
GTAvehicle clone_vehicle(GTAvehicle vehicle, GTAentity pedForEmblem = GTAentity());







