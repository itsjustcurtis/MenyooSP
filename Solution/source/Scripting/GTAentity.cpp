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
#include "GTAentity.h"

#include "..\macros.h"

#include "enums.h"
#include "..\Util\GTAmath.h"
#include "..\Natives\natives2.h"
#include "..\Memory\GTAmemory.h"
#include "GTAblip.h"
#include "Model.h"
#include "Raycast.h"
#include "..\Util\FileLogger.h"

namespace 
{
	inline constexpr UINT64 kEntityFreezeOffset      = 0x2E; // bit 1 = position frozen
	inline constexpr UINT64 kEntityRightVecOffset    = 0x60; // matrix row, right vector
	inline constexpr UINT64 kEntityForwardVecOffset  = 0x70; // matrix row, forward vector
	inline constexpr UINT64 kEntityUpVecOffset       = 0x80; // matrix row, up vector
}

namespace
{
	// atox fixes: run f(), catch access violations only
	template <typename F>
	bool RunGuarded(F&& f)
	{
		__try { f(); return true; }
		__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { return false; }
	}

	// atox fixes: manual attach for scaled entities
	struct ManualAttachment
	{
		int child;
		int parent;
		int bone;
		Vector3 offset;
		Vector3 rotation;
		Vector3 scale;
		bool prevFrozen;
		bool prevCollision;
	};
	std::vector<ManualAttachment> g_manualAttachments;

	ManualAttachment* FindManualAttachment(int child)
	{
		for (auto& a : g_manualAttachments)
			if (a.child == child) return &a;
		return nullptr;
	}

	// atox fixes: drop entry, restore old collision/freeze
	void ReleaseManualAttachment(int childHandle)
	{
		for (size_t i = 0; i < g_manualAttachments.size(); ++i)
		{
			if (g_manualAttachments[i].child != childHandle) continue;
			ManualAttachment a = g_manualAttachments[i];
			g_manualAttachments.erase(g_manualAttachments.begin() + i);
			GTAentity child(a.child);
			if (child.Exists())
			{
				child.SetIsCollisionEnabled(a.prevCollision);
				FREEZE_ENTITY_POSITION(a.child, a.prevFrozen);
			}
			return;
		}
	}

	bool IsScaledEntity(const GTAentity& e)
	{
		Vector3 s = e.GetScale();
		return std::fabs(s.x - 1.0f) > 0.01f || std::fabs(s.y - 1.0f) > 0.01f || std::fabs(s.z - 1.0f) > 0.01f;
	}

	// atox fixes: true = attached manually (scaled), false = use native attach
	bool TryAttachManuallyImpl(GTAentity child, GTAentity parent, int bone, const Vector3& offset, const Vector3& rotation)
	{
		const int ch = child.GetHandle();
		if (ch == parent.GetHandle()) return false;
		{
			// atox fixes: cycle check, skip the attach
			int cur = parent.GetHandle();
			for (int depth = 0; depth < 16; ++depth)
			{
				if (cur == ch) return true;
				ManualAttachment* p = FindManualAttachment(cur);
				if (!p) break;
				cur = p->parent;
			}
		}
		if (!IsScaledEntity(child))
		{
			ReleaseManualAttachment(ch); // not scaled anymore, native attach
			return false;
		}
		ManualAttachment* a = FindManualAttachment(ch);
		if (!a)
		{
			if (IS_ENTITY_ATTACHED(ch)) DETACH_ENTITY(ch, 1, 1); // atox fixes: drop native attach first
			ManualAttachment n{};
			n.child = ch;
			n.prevFrozen = child.IsPositionFrozen();
			n.prevCollision = child.GetIsCollisionEnabled();
			g_manualAttachments.push_back(n);
			a = &g_manualAttachments.back();
		}
		a->parent = parent.GetHandle();
		a->bone = bone;
		a->offset = offset;
		a->rotation = rotation;
		a->scale = child.GetScale();
		child.SetIsCollisionEnabled(false);
		FREEZE_ENTITY_POSITION(child.GetHandle(), true);
		return true;
	}

	bool g_manualAttachDisabled = false;

	// atox fixes: disable manual attach after an exception
	void HandleManualFailure(const char* msg)
	{
		addlog(ige::LogType::LOG_ERROR, msg);
		g_manualAttachDisabled = true;
		for (auto& a : g_manualAttachments)
		{
			int h = a.child;
			bool pc = a.prevCollision;
			bool pf = a.prevFrozen;
			RunGuarded([&]() {
				if (DOES_ENTITY_EXIST(h)) { SET_ENTITY_COLLISION(h, pc, false); FREEZE_ENTITY_POSITION(h, pf); }
			});
		}
		g_manualAttachments.clear();
	}

	bool TryAttachManually(GTAentity child, GTAentity parent, int bone, const Vector3& offset, const Vector3& rotation)
	{
		if (g_manualAttachDisabled) return false;
		bool result = false;
		if (!RunGuarded([&]() { result = TryAttachManuallyImpl(child, parent, bone, offset, rotation); }))
		{
			HandleManualFailure("Exception in TryAttachManually");
			return false;
		}
		return result;
	}
}

GTAentity& GTAentity::operator = (const GTAentity& value)
{
	this->mHandle = value.mHandle;
	return *this;
}
bool operator == (const GTAentity& left, const GTAentity& right)
{
	return left.mHandle == right.mHandle;
}
bool operator != (const GTAentity& left, const GTAentity& right)
{
	return left.mHandle != right.mHandle;
}
bool operator < (const GTAentity& left, const GTAentity& right)
{
	return left.mHandle < right.mHandle;
}
bool operator > (const GTAentity& left, const GTAentity& right)
{
	return left.mHandle > right.mHandle;
}

GTAentity::GTAentity()
	: mHandle(0)
{
}
GTAentity::GTAentity(int handle)
	: mHandle(handle)
{
}

int& GTAentity::Handle()
{
	return this->mHandle;
}
int GTAentity::GetHandle() const noexcept
{
	return this->mHandle;
}

UINT64 GTAentity::MemoryAddress() const
{
	return GTAmemory::GetEntityAddress(this->GetHandle());
}

int GTAentity::Type() const noexcept
{
	return GET_ENTITY_TYPE(this->mHandle);
}

GTAblip GTAentity::CurrentBlip() const
{
	return GET_BLIP_FROM_ENTITY(this->mHandle);
}

Vector3 GTAentity::ForwardVector() const
{
	auto addr = this->MemoryAddress();
	if (!addr) return Vector3();
	return GTAmemory::ReadVector3(addr + kEntityForwardVecOffset);
}

Vector3 GTAentity::RightVector() const
{
	auto addr = this->MemoryAddress();
	if (!addr) return Vector3();
	return GTAmemory::ReadVector3(addr + kEntityRightVecOffset);
}

Vector3 GTAentity::UpVector() const
{
	auto addr = this->MemoryAddress();
	if (!addr) return Vector3();
	return GTAmemory::ReadVector3(addr + kEntityUpVecOffset);
}

bool GTAentity::IsPositionFrozen() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(this->MemoryAddress() + kEntityFreezeOffset), 1);
	}
	else return false;
}

void GTAentity::FreezePosition(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		SetClearBit(*reinterpret_cast<int*>(this->MemoryAddress() + kEntityFreezeOffset), 1, value);
	}
	FREEZE_ENTITY_POSITION(this->mHandle, value);
}

void GTAentity::SetDynamic(bool value)
{
	SET_ENTITY_DYNAMIC(this->mHandle, value);
	if (value)
	{
		SET_ENTITY_VELOCITY(this->mHandle, 0.0f, 0.0f, 0.0f);
	}
}

float GTAentity::GetHeading() const noexcept
{
	return GET_ENTITY_HEADING(this->mHandle);
}

void GTAentity::SetHeading(float value)
{
	SET_ENTITY_HEADING(this->mHandle, value);
}

int GTAentity::GetHealth() const noexcept
{
	return GET_ENTITY_HEALTH(this->mHandle);
}

void GTAentity::SetHealth(int value)
{
	SET_ENTITY_HEALTH(this->mHandle, value, 0);
}

float GTAentity::GetVehicleEngine() const
{
	if (!IsVehicle())
		return -1.0f;

	return GET_VEHICLE_ENGINE_HEALTH(this->mHandle);
}

void GTAentity::SetVehicleEngine(float value)
{
	if (!IsVehicle())
		return;

	SET_VEHICLE_ENGINE_HEALTH(this->mHandle, value);
}

float GTAentity::HeightAboveGround() const noexcept
{
	return GET_ENTITY_HEIGHT_ABOVE_GROUND(this->mHandle);
}

float GTAentity::GetGroundZ() const
{
	Vector3 pos = this->GetPosition();
	GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, 1100.0f, &pos.z, 0, 0);
	return pos.z;
}

void GTAentity::PlaceOnGround()
{
	Vector3 pos = this->GetPosition();

	RaycastResult ray1 = RaycastResult::Raycast(pos, Vector3(0, 0, -1.0f), 10000.0f, IntersectOptions::Map);
	if (ray1.DidHitAnything())
	{
		this->SetPosition(ray1.HitCoords() + Vector3(0, 0, this->Dim1().z));
		return;
	}
	RaycastResult ray2 = RaycastResult::Raycast(pos, Vector3(0, 0, 1.0f), 10000.0f, IntersectOptions::Map);
	if (ray2.DidHitAnything())
	{
		this->SetPosition(ray2.HitCoords() + Vector3(0, 0, this->Dim1().z));
		return;
	}

	pos.z = this->GetGroundZ() + this->Dim1().z;
	SET_ENTITY_COORDS(this->mHandle, pos.x, pos.y, pos.z, 0, 0, 0, 1);
	PLACE_OBJECT_ON_GROUND_PROPERLY(this->mHandle);
}

bool GTAentity::IsAlive() const
{
	return !this->IsDead();
}

bool GTAentity::IsDead() const
{
	return IS_ENTITY_DEAD(this->mHandle, false) != 0;
}

bool GTAentity::IsInAir() const
{
	return IS_ENTITY_IN_AIR(this->mHandle) != 0;
}

bool GTAentity::IsInWater() const
{
	return IS_ENTITY_IN_WATER(this->mHandle) != 0;
}

bool GTAentity::IsSubmerged() const
{
	return GET_ENTITY_SUBMERGED_LEVEL(this->mHandle) != 0;
}

bool GTAentity::IsOccluded() const
{
	return IS_ENTITY_OCCLUDED(this->mHandle) != 0;
}

bool GTAentity::IsOnScreen() const
{
	return IS_ENTITY_ON_SCREEN(this->mHandle) != 0;
}

bool GTAentity::IsPed() const
{
	return IS_ENTITY_A_PED(this->mHandle) != 0;
}

bool GTAentity::IsVehicle() const
{
	return IS_ENTITY_A_VEHICLE(this->mHandle) != 0;
}

bool GTAentity::IsProp() const
{
	return IS_ENTITY_AN_OBJECT(this->mHandle) != 0;
}

void GTAentity::SetProofs(bool bulletProof, bool fireProof, bool explosionProof, bool collisionProof, bool meleeProof, bool unk1, bool unk2, bool drownProof)
{
	SET_ENTITY_PROOFS(this->mHandle, bulletProof, fireProof, explosionProof, collisionProof, meleeProof, unk1, unk2, drownProof);
}

bool GTAentity::IsOnFire() const
{
	return IS_ENTITY_ON_FIRE(this->mHandle) != 0;
}

void GTAentity::SetOnFire(bool value)
{
	value ? (void)START_ENTITY_FIRE(this->mHandle) : STOP_ENTITY_FIRE(this->mHandle);
}

bool GTAentity::MissionEntity_get() const
{
	return IS_ENTITY_A_MISSION_ENTITY(this->mHandle) != 0;
}

void GTAentity::SetMissionEntity(bool value)
{
	SET_ENTITY_AS_MISSION_ENTITY(this->mHandle, value, 1);
}

bool GTAentity::IsUpright() const
{
	return IS_ENTITY_UPRIGHT(this->mHandle, 0) != 0;
}

void GTAentity::SetUpright()
{
	SET_ENTITY_HEADING(this->mHandle, GetHeading() + 1.0f);
	SET_ENTITY_HEADING(this->mHandle, GetHeading() - 1.0f);
}

bool GTAentity::IsUpsideDown() const
{
	return IS_ENTITY_UPSIDEDOWN(this->mHandle) != 0;
}

bool GTAentity::IsVisible() const
{
	return IS_ENTITY_VISIBLE(this->mHandle) != 0;
}

void GTAentity::SetVisible(bool value)
{
	SET_ENTITY_VISIBLE(this->mHandle, value, false);
}

int GTAentity::GetMaxHealth() const noexcept
{
	return GET_ENTITY_MAX_HEALTH(this->mHandle);
}

void GTAentity::SetMaxHealth(int value)
{
	SET_ENTITY_MAX_HEALTH(this->mHandle, value);
}

void GTAentity::SetLandingGear(bool deployed)
{
	if (!IsVehicle())
		return;

	int state = deployed ? 0 : 3;  // 0 = deployed, 3 = retracted
	VEHICLE::CONTROL_LANDING_GEAR(this->GetHandle(), state);
}

int GTAentity::GetLandingGearState() const
{
	if (!IsVehicle())
		return -1;

	return VEHICLE::GET_LANDING_GEAR_STATE(this->GetHandle());
}

bool GTAentity::HasLandingGear() const
{
	if (!IsVehicle())
		return false;

	int state = VEHICLE::GET_LANDING_GEAR_STATE(this->mHandle);
	return state >= 0 && state <= 3;
}

GTAmodel::Model GTAentity::Model() const
{
	return GET_ENTITY_MODEL(this->mHandle);
}

GTAmodel::ModelDimensions GTAentity::ModelDimensions() const
{
	return this->Model().Dimensions();
}

void GTAentity::ModelDimensions(Vector3& dim1, Vector3& dim2) const
{
	this->Model().Dimensions(dim1, dim2);
}

Vector3 GTAentity::Dim1() const
{
	return this->Model().Dimensions().Dim1;
}

Vector3 GTAentity::Dim2() const
{
	return this->Model().Dimensions().Dim2;
}


Vector3 GTAentity::GetPosition() const
{
	return GET_ENTITY_COORDS(this->mHandle, 1);
}

void GTAentity::SetPosition(Vector3 value)
{
	SET_ENTITY_COORDS_NO_OFFSET(this->mHandle, value.x, value.y, value.z, 1, 1, 1);
}

Vector3 GTAentity::GetRotation() const
{
	return GET_ENTITY_ROTATION(this->mHandle, 2);
}

void GTAentity::SetRotation(Vector3 value)
{
	SET_ENTITY_ROTATION(this->mHandle, value.x, value.y, value.z, 2, 1);
}

Vector3 GTAentity::GetScale() const
{
	UINT64 ptr = GTAmemory::_entityAddressFunc(mHandle);
	if (!ptr) return Vector3(1.0f, 1.0f, 1.0f);

	// atox fixes: scale = vector lengths (the diagonal also holds rotation)
	Vector3 r = GTAmemory::ReadVector3(ptr + kEntityRightVecOffset);
	Vector3 f = GTAmemory::ReadVector3(ptr + kEntityForwardVecOffset);
	Vector3 u = GTAmemory::ReadVector3(ptr + kEntityUpVecOffset);
	return Vector3(r.Length(), f.Length(), u.Length());
}

void GTAentity::SetScale(Vector3 value)
{
	UINT64 ptr = GTAmemory::_entityAddressFunc(mHandle);
	if (!ptr) return;

	if (value.x < 0.01f) value.x = 0.01f;
	if (value.y < 0.01f) value.y = 0.01f;
	if (value.z < 0.01f) value.z = 0.01f;

	// keep direction, only change length
	auto apply = [](UINT64 m, float sx, float sy, float sz) {
		Vector3 r = GTAmemory::ReadVector3(m);
		Vector3 f = GTAmemory::ReadVector3(m + 0x10);
		Vector3 u = GTAmemory::ReadVector3(m + 0x20);
		float lr = r.Length(), lf = f.Length(), lu = u.Length();
		if (lr > 0.0001f) GTAmemory::WriteVector3(m, r * (sx / lr));
		if (lf > 0.0001f) GTAmemory::WriteVector3(m + 0x10, f * (sy / lf));
		if (lu > 0.0001f) GTAmemory::WriteVector3(m + 0x20, u * (sz / lu));
	};

	apply(ptr + kEntityRightVecOffset, value.x, value.y, value.z);

	if (IsVehicle())
	{
		UINT64 drawPtr = *reinterpret_cast<UINT64*>(ptr + 0x30);
		if (drawPtr) apply(drawPtr + 0x20, value.x, value.y, value.z);
	}

	// keep manual attach scale in sync
	if (ManualAttachment* a = FindManualAttachment(mHandle)) a->scale = value;
}

Vector3 GTAentity::GetDirection() const
{
	return Vector3::RotationToDirection(GET_ENTITY_ROTATION(this->mHandle, 2));
}

void GTAentity::SetDirection(Vector3 value)
{
	value = Vector3::DirectionToRotation(value);
	SET_ENTITY_ROTATION(this->mHandle, value.x, value.y, value.z, 2, 1);
}


Vector3 GTAentity::GetVelocity() const
{
	return GET_ENTITY_VELOCITY(this->mHandle);
}

void GTAentity::SetVelocity(Vector3 value)
{
	SET_ENTITY_VELOCITY(this->mHandle, value.x, value.y, value.z);
}

Vector3 GTAentity::GetRotationVelocity() const
{
	return GET_ENTITY_ROTATION_VELOCITY(this->mHandle);
}

float GTAentity::GetSpeed() const
{
	return GET_ENTITY_SPEED(this->mHandle);
}

void GTAentity::SetMaxSpeed(float value)
{
	SET_ENTITY_MAX_SPEED(this->mHandle, value);
}

Vector3 GTAentity::GetSpeedVector(bool relative)
{
	return GET_ENTITY_SPEED_VECTOR(this->mHandle, relative);
}

int GTAentity::GetAlpha() const noexcept
{
	return GET_ENTITY_ALPHA(this->mHandle);
}

void GTAentity::SetAlpha(int value)
{
	if (value == 255)
		RESET_ENTITY_ALPHA(this->mHandle);
	else
		SET_ENTITY_ALPHA(this->mHandle, value, 0);
}

void GTAentity::ResetAlpha()
{
	RESET_ENTITY_ALPHA(this->mHandle);
}

int GTAentity::GetLODDistance() const noexcept
{
	return GET_ENTITY_LOD_DIST(this->mHandle);
}

void GTAentity::SetLODDistance(int value)
{
	SET_ENTITY_LOD_DIST(this->mHandle, value);
}

bool GTAentity::GetHasGravity() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return !IsBitSet(*reinterpret_cast<int*>(memoryAddress + 26), 4);
	}
	else return true;
}

void GTAentity::SetHasGravity(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		SetClearBit(*reinterpret_cast<int*>(memoryAddress + 26), 4, !value);
	}
	SET_ENTITY_HAS_GRAVITY(this->mHandle, value);
}

Vector3 GTAentity::CollisionNormal() const
{
	return GET_COLLISION_NORMAL_OF_LAST_HIT_FOR_ENTITY(this->mHandle);
}

bool GTAentity::HasCollided() const
{
	return HAS_ENTITY_COLLIDED_WITH_ANYTHING(this->mHandle) != 0;
}

void GTAentity::SetIsRecordingCollisions(bool value)
{
	SET_ENTITY_RECORDS_COLLISIONS(this->mHandle, value);
}

void GTAentity::SetHasCollisionWithEntity(const GTAentity& ent, bool value)
{
	SET_ENTITY_NO_COLLISION_ENTITY(this->mHandle, ent.mHandle, value);
}

bool GTAentity::GetIsCollisionEnabled() const
{
	return !GET_ENTITY_COLLISION_DISABLED(this->mHandle);
}

void GTAentity::SetIsCollisionEnabled(bool value)
{
	SET_ENTITY_COLLISION(this->mHandle, value, false);
}

void GTAentity::ToggleLandingGear()
{
	if (!IS_ENTITY_A_VEHICLE(this->mHandle))
		return;

	int currentState = GET_LANDING_GEAR_STATE(this->mHandle);

	if (currentState == 0) // Deployed
	{
		// If in air, animate the retraction
		if (IS_ENTITY_IN_AIR(this->mHandle))
			CONTROL_LANDING_GEAR(this->mHandle, 1); // Retracting
		else
			CONTROL_LANDING_GEAR(this->mHandle, 3); // Snap to Retracted
	}
	else if (currentState == 3) // Retracted
	{
		// If in air, animate the deployment
		if (IS_ENTITY_IN_AIR(this->mHandle))
			CONTROL_LANDING_GEAR(this->mHandle, 2); // Deploying
		else
			CONTROL_LANDING_GEAR(this->mHandle, 0); // Snap to Deployed
	}
}
// Very real chance this won't work, but this is the ability to toggle landing gear which I've been begging for, for a while.

int GTAentity::NetID() const
{
	return NETWORK_GET_NETWORK_ID_FROM_ENTITY(this->mHandle);
}

bool GTAentity::IsInRangeOf(Vector3 position, float range) const
{
	return ((Vector3::Subtract(this->GetPosition(), position).Length()) < range);
}

bool GTAentity::IsInArea(Vector3 pos1, Vector3 pos2) const
{
	return IsInArea(pos1, pos2, 0);
}

bool GTAentity::IsInArea(Vector3 pos1, Vector3 pos2, float angle) const
{
	return IS_ENTITY_IN_ANGLED_AREA(this->mHandle, pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, angle, true, true, true) != 0;
}

bool GTAentity::IsNearEntity(GTAentity entity, Vector3 distance) const
{
	return IS_ENTITY_AT_ENTITY(this->mHandle, entity.Handle(), distance.x, distance.y, distance.z, 0, 1, 0) != 0;
}

bool GTAentity::IsTouching(GTAentity entity) const
{
	return IS_ENTITY_TOUCHING_ENTITY(this->mHandle, entity.Handle()) != 0;
}

bool GTAentity::HasBeenDamagedBy(GTAentity entity) const
{
	return HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(this->mHandle, entity.Handle(), 1) != 0;
}

Vector3 GTAentity::GetOffsetInWorldCoords(Vector3 offset) const
{
	return GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(this->mHandle, offset.x, offset.y, offset.z);
}

Vector3 GTAentity::GetOffsetInWorldCoords(float X, float Y, float Z) const
{
	return GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(this->mHandle, X, Y, Z);
}

Vector3 GTAentity::GetOffsetGivenWorldCoords(Vector3 coord) const
{
	return GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(this->mHandle, coord.x, coord.y, coord.z);
}

int GTAentity::GetBoneIndex(const std::string& boneLabel) const
{
	if (boneLabel.length() == 0)
		return 0;
	return GET_ENTITY_BONE_INDEX_BY_NAME(this->mHandle, boneLabel.c_str());
}

int GTAentity::GetBoneIndex(VBone::VBone value) const
{
	return GET_ENTITY_BONE_INDEX_BY_NAME(this->mHandle, VBone::vNames[value].c_str());
}

Vector3 GTAentity::GetBoneCoords(int boneIndex) const
{
	return GET_WORLD_POSITION_OF_ENTITY_BONE(this->mHandle, boneIndex);
}

Vector3 GTAentity::GetBoneCoords(const std::string& boneLabel) const
{
	return GET_WORLD_POSITION_OF_ENTITY_BONE(this->mHandle, this->GetBoneIndex(boneLabel));
}

int GTAentity::GetBoneCount() const
{
	return GTAmemory::GetEntityBoneCount(this->mHandle);
}

UINT64 GTAentity::GetBoneMatrixAddress(int boneIndex) const
{
	return GTAmemory::GetEntityBoneMatrixAddress(this->mHandle, boneIndex);
}

Vector3 GTAentity::GetOffsetFromBoneInWorldCoords(int boneIndex, const Vector3& offset) const
{
	if (this->Exists())
	{
		auto addr = GTAmemory::GetEntityBoneMatrixAddress(this->mHandle, boneIndex);
		if (addr)
		{
			float* Addr = reinterpret_cast<float*>(addr);
			const Vector3& right = Vector3(Addr[0], Addr[1], Addr[2]);
			const Vector3& front = Vector3(Addr[4], Addr[5], Addr[6]);
			const Vector3& up = Vector3(Addr[8], Addr[9], Addr[10]);
			const Vector3& boneOff = Vector3(Addr[12], Addr[13], Addr[14]);
			const Vector3& vehOffset = boneOff + right * offset.x + front * offset.y + up * offset.z;
			return this->GetOffsetInWorldCoords(vehOffset);
		}
	}
	return Vector3();
}

Vector3 GTAentity::GetOffsetFromBoneInWorldCoords(const std::string& boneLabel, const Vector3& offset) const
{
	return this->GetOffsetFromBoneInWorldCoords(this->GetBoneIndex(boneLabel), offset);
}

bool GTAentity::HasBone(const std::string& boneLabel) const
{
	return this->GetBoneIndex(boneLabel) != -1;
}

bool GTAentity::HasBone(VBone::VBone value) const
{
	return this->GetBoneIndex(VBone::vNames[value]) != -1;
}

bool GTAentity::IsAttached() const
{
	return IS_ENTITY_ATTACHED(this->mHandle) != 0 || FindManualAttachment(this->mHandle) != nullptr;
}

bool GTAentity::IsAttachedTo(const GTAentity& to) const
{
	if (IS_ENTITY_ATTACHED_TO_ENTITY(this->mHandle, to.mHandle) != 0) return true;
	ManualAttachment* a = FindManualAttachment(this->mHandle);
	return a && a->parent == to.mHandle;
}

void GTAentity::Detach()
{
	if (FindManualAttachment(this->mHandle))
	{
		ReleaseManualAttachment(this->mHandle);
		return;
	}
	DETACH_ENTITY(this->mHandle, 1, 1);
}

bool GTAentity::GetManualAttachParent(const GTAentity& child, GTAentity& parentOut)
{
	ManualAttachment* a = FindManualAttachment(child.GetHandle());
	if (!a) return false;
	parentOut = GTAentity(a->parent);
	return true;
}

static void UpdateManualAttachmentsImpl()
{
	for (size_t i = 0; i < g_manualAttachments.size(); )
	{
		ManualAttachment a = g_manualAttachments[i];
		GTAentity child(a.child), parent(a.parent);
		if (!child.Exists() || !parent.Exists())
		{
			ReleaseManualAttachment(a.child); // entry gone, no i++
			continue;
		}

		// atox fixes: natives only, no direct memory reads
		Vector3 parentRotation = parent.GetRotation();
		Mat3 pr = EulerToMatrix(parentRotation);
		Vector3 pos;
		if (a.bone > 0)
		{
			// bone world pos + offset rotated by parent (approx)
			Vector3 bp = parent.GetBoneCoords(a.bone);
			const Vector3& o = a.offset;
			pos = Vector3(bp.x + pr.m[0][0] * o.x + pr.m[0][1] * o.y + pr.m[0][2] * o.z,
			              bp.y + pr.m[1][0] * o.x + pr.m[1][1] * o.y + pr.m[1][2] * o.z,
			              bp.z + pr.m[2][0] * o.x + pr.m[2][1] * o.y + pr.m[2][2] * o.z);
		}
		else
		{
			pos = parent.GetOffsetInWorldCoords(a.offset);
		}

		Mat3 world = pr * EulerToMatrix(a.rotation);
		child.SetRotation(MatrixToEuler(world)); // rotation resets matrix scale, reapply after
		child.SetPosition(pos);
		child.SetScale(a.scale);
		++i;
	}
}

void GTAentity::UpdateManualAttachments()
{
	if (g_manualAttachments.empty()) return;
	if (!RunGuarded([]() { UpdateManualAttachmentsImpl(); }))
		HandleManualFailure("Exception in UpdateManualAttachments");
}

void GTAentity::AttachTo(GTAentity entity, int boneIndex, bool collision)
{
	this->AttachTo(entity.Handle(), boneIndex, collision, Vector3(0, 0, 0), Vector3(0, 0, 0));
}

void GTAentity::AttachTo(GTAentity entity, int boneIndex, bool collision, Vector3 position, Vector3 rotation)
{
	if (TryAttachManually(*this, entity, boneIndex, position, rotation)) return;
	ATTACH_ENTITY_TO_ENTITY(this->mHandle, entity.Handle(), boneIndex, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, false, false, collision, false, 2, true, 0);
}

void GTAentity::AttachTo(GTAentity entity, int boneIndex, Vector3 position, Vector3 rotation, bool b9, bool useSoftPinning, bool collision, bool isPed, int vertexIndex, bool fixedRot)
{
	if (TryAttachManually(*this, entity, boneIndex, position, rotation)) return;
	ATTACH_ENTITY_TO_ENTITY(this->mHandle, entity.Handle(), boneIndex, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, b9, useSoftPinning, collision, isPed, vertexIndex, fixedRot, 0);
}

void GTAentity::AttachPhysicallyTo(GTAentity entity, int boneIndexDoer, int boneIndexGetter, float forceToBreak)
{
	this->AttachPhysicallyTo(entity, boneIndexDoer, boneIndexGetter, forceToBreak, Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
}

void GTAentity::AttachPhysicallyTo(GTAentity entity, int boneIndexDoer, int boneIndexGetter, float forceToBreak, Vector3 position1, Vector3 position2, Vector3 rotation)
{
	ATTACH_ENTITY_TO_ENTITY_PHYSICALLY(this->mHandle, entity.Handle(), boneIndexDoer, boneIndexGetter, position1.x, position1.y, position1.z, position2.x, position2.y, position2.z, rotation.x, rotation.y, rotation.z, forceToBreak, 1, 1, 1, 1, 2);
}

GTAblip GTAentity::AddBlip()
{
	return ADD_BLIP_FOR_ENTITY(this->mHandle);
}

void GTAentity::SetMass(float mass)
{
	SET_OBJECT_PHYSICS_PARAMS(this->mHandle, mass, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
}
void GTAentity::Oscillate(const Vector3& position, float angleFreq, float dampRatio)
{
	//Zorg93 - bring entity to position without lerp function using applyforce
	this->ApplyForce(((position - this->GetPosition()) * (angleFreq * angleFreq)) -
		(2.0f * angleFreq * dampRatio * this->GetVelocity()) + Vector3(0.0f, 0.0f, 0.1f), ForceType::MaxForceRot2);
}

void OscillateEntity(GTAentity entity, const Vector3& position, float angleFreq, float dampRatio)
{
	entity.Oscillate(position, angleFreq, dampRatio);
}

void GTAentity::ApplyForce(Vector3 direction, ForceType forceType)
{
	this->ApplyForce(direction, Vector3(0, 0, 0), forceType);
}

void GTAentity::ApplyForce(Vector3 direction, Vector3 offset, ForceType forceType)
{
	APPLY_FORCE_TO_ENTITY(this->mHandle, static_cast<int>(forceType), direction.x, direction.y, direction.z, offset.x, offset.y, offset.z, false, false, true, true, false, true);
}

void GTAentity::ApplyForceRelative(Vector3 direction, ForceType forceType)
{
	this->ApplyForceRelative(direction, Vector3(0, 0, 0), forceType);
}

void GTAentity::ApplyForceRelative(Vector3 direction, Vector3 offset, ForceType forceType)
{
	APPLY_FORCE_TO_ENTITY(this->mHandle, static_cast<int>(forceType), direction.x, direction.y, direction.z, offset.x, offset.y, offset.z, false, true, true, true, false, true);
}

void GTAentity::ApplyForceCustom(Vector3 direction, Vector3 offset, ForceType forceType, bool unk1, bool isRel, bool ignoreUpVector, bool unk2, bool unk3, bool unk4)
{
	APPLY_FORCE_TO_ENTITY(this->mHandle, static_cast<int>(forceType), direction.x, direction.y, direction.z, offset.x, offset.y, offset.z, unk1, isRel, ignoreUpVector, unk2, unk3, unk4);
}


bool GTAentity::HasControl() const
{
	return NETWORK_HAS_CONTROL_OF_ENTITY(this->mHandle) != 0;
}

bool GTAentity::RequestControlOnce()
{
	if (!NETWORK_IS_IN_SESSION() || this->HasControl())
		return true;
	NETWORK::SET_NETWORK_ID_CAN_MIGRATE(this->NetID(), true);
	return NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(this->mHandle) != 0;
}

bool GTAentity::RequestControl()
{
	uint8_t tick = 0;

	while (!this->RequestControlOnce() && tick <= 12)
	{
		//WAIT(0); // Let's just not wait here ever again geez
		tick++;
	}
	return tick <= 12;
}

bool GTAentity::RequestControl(DWORD timeOut)
{
	for (timeOut += GetTickCount(); GetTickCount() < timeOut;)
	{
		if (this->RequestControlOnce())
			return true;
		WAIT(0);
	}
	return false;
}

void GTAentity::Delete(bool tele)
{
	if (!this->Exists()) return;

	this->RequestControl();

	GTAblip blip = this->CurrentBlip();
	if (blip.Exists()) blip.Remove();

	this->SetMissionEntity(false);

	if (tele) SET_ENTITY_COORDS_NO_OFFSET(this->mHandle, 32.2653f, 7683.5249f, 0.5696f, 0, 0, 0);

	auto type = static_cast<EntityType>(this->Type());
	auto handle = this->mHandle;
	this->mHandle = 0;

	switch (type)
	{
	case EntityType::PROP: DELETE_OBJECT(&handle); break;
	case EntityType::VEHICLE: DELETE_VEHICLE(&handle); break;
	case EntityType::PED: DELETE_PED(&handle); break;
	default: DELETE_ENTITY(&handle); break;
	}
}

bool GTAentity::Exists() const
{
	return DOES_ENTITY_EXIST(this->mHandle) != 0;
}

void GTAentity::NoLongerNeeded()
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(&this->mHandle);
	//delete this;
}

bool GTAentity::Equals(int value) const
{
	return (this->mHandle == value);
}

bool GTAentity::Equals(GTAentity const& value) const
{
	return (this->mHandle == value.mHandle);
}


bool GTAentity::IsBulletProof() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 4);
	}
	else return false;
}

void GTAentity::SetBulletProof(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 4, value);
	}
}

bool GTAentity::IsExplosionProof() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 11);
	}
	else return false;
}

void GTAentity::SetExplosionProof(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 11, value);
	}
}

bool GTAentity::IsFireProof() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 5);
	}
	else return false;
}

void GTAentity::SetFireProof(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 5, value);
	}
}

bool GTAentity::IsInvincible() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 8);
	}
	else return false;
}

void GTAentity::SetInvincible(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 8, value);
	}

	SET_ENTITY_INVINCIBLE(this->mHandle, value);
}

bool GTAentity::IsMeleeProof() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 7);
	}
	else return false;
}

void GTAentity::SetMeleeProof(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 7, value);
	}
}

bool GTAentity::IsOnlyDamagedByPlayer() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return IsBitSet(*reinterpret_cast<int*>(memoryAddress + 392), 9);
	}
	else return false;
}

void GTAentity::SetOnlyDamagedByPlayer(bool value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return SetClearBit(*reinterpret_cast<int*>(memoryAddress + 392), 9, value);
	}

	SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(this->mHandle, value);
}
