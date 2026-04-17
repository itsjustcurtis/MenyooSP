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
#include "Checkpoint.h"

#include "..\macros.h"

#include "..\Util\GTAmath.h"
//#include "..\Natives\types.h"
#include "..\Natives\natives2.h"
#include "..\Memory\GTAmemory.h"

Checkpoint::Checkpoint(int handle) : mHandle(handle)
{
}
Checkpoint::Checkpoint() : mHandle(0)
{
}

UINT64 Checkpoint::MemoryAddress() const
{
	return GTAmemory::GetCheckpointAddress(this->mHandle);
}

Vector3 Checkpoint::GetPosition() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		GTAmemory::ReadVector3(memoryAddress);
	}
	else return Vector3();
}
void Checkpoint::SetPosition(const Vector3& value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		GTAmemory::WriteVector3(memoryAddress, value);
	}
}

Vector3 Checkpoint::GetTargetPosition() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		GTAmemory::ReadVector3(memoryAddress + 16);
	}
	else return Vector3();
}
void Checkpoint::SetTargetPosition(const Vector3& value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		GTAmemory::WriteVector3(memoryAddress + 16, value);
	}
}

CheckpointIcon Checkpoint::GetIcon() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return CheckpointIcon(*(const int*)(memoryAddress + 56));
	}
	else return CheckpointIcon(0);
}
void Checkpoint::SetIcon(const CheckpointIcon& value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(int*)(memoryAddress + 56) = (int)value;
	}
}

BYTE Checkpoint::GetReserved() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return *(const BYTE*)(memoryAddress + 52);
	}
	else return 0;
}
void Checkpoint::SetReserved(BYTE value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(BYTE*)(memoryAddress + 52) = value;
	}
}

float Checkpoint::GetRadius() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return *(const float*)(memoryAddress + 60);
	}
	else return 0.0f;
}
void Checkpoint::SetRadius(float value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(float*)(memoryAddress + 60) = value;
	}
}

RGBA Checkpoint::GetColour() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return RGBA::FromArgb(*(const UINT32*)(memoryAddress + 80));
	}
	else return RGBA();
}
void Checkpoint::SetColour(const RGBA& value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(UINT32*)(memoryAddress + 80) = value.ToArgb();
	}
}
RGBA Checkpoint::GetIconColour() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return RGBA::FromArgb(*(const UINT32*)(memoryAddress + 84));
	}
	else return RGBA();
}
void Checkpoint::SetIconColour(const RGBA& value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(UINT32*)(memoryAddress + 84) = value.ToArgb();
	}
}

float Checkpoint::GetCylinderNearHeight() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return *(const float*)(memoryAddress + 68);
	}
	else return 0.0f;
}
void Checkpoint::SetCylinderNearHeight(float value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(float*)(memoryAddress + 68) = value;
	}
}

float Checkpoint::GetCylinderFarHeight() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return *(const float*)(memoryAddress + 72);
	}
	else return 0.0f;
}
void Checkpoint::SetCyclinderFarHeight(float value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(float*)(memoryAddress + 72) = value;
	}
}

float Checkpoint::GetCylinderRadius() const
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		return *(const float*)(memoryAddress + 76);
	}
	else return 0.0f;
}
void Checkpoint::SetCylinderRadius(float value)
{
	auto memoryAddress = this->MemoryAddress();
	if (memoryAddress)
	{
		*(float*)(memoryAddress + 76) = value;
	}
}

void Checkpoint::Delete()
{
	DELETE_CHECKPOINT(this->mHandle);
	this->mHandle = 0;
}
bool Checkpoint::Exists()
{
	return this->mHandle != 0 && this->MemoryAddress() != 0;
}

bool Checkpoint::Equals(const Checkpoint& obj)
{
	return this->mHandle == obj.mHandle;
}

bool operator == (const Checkpoint& left, const Checkpoint& right)
{
	return left.mHandle == right.mHandle;
}
bool operator != (const Checkpoint& left, const Checkpoint& right)
{
	return left.mHandle != right.mHandle;
}