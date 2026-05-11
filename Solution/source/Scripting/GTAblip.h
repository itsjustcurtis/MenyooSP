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

#include "BlipEnums.h"

#include <string>

typedef unsigned short UINT16;
typedef unsigned char uint8_t;
class Vector3;

class GTAblip final
{
public:
	friend bool operator == (const GTAblip& left, const GTAblip& right);
	friend bool operator != (const GTAblip& left, const GTAblip& right);
	GTAblip& operator = (const GTAblip& right);

	GTAblip();
	GTAblip(int handle);

	int& Handle();

	uint8_t Alpha() const;
	void SetAlpha(uint8_t value);

	int Colour() const;
	void SetColour(int value);

	bool IsFlashing() const;
	void SetFlashing(bool value);

	void SetFriendly(bool value);

	bool IsOnMinimap() const;

	bool IsShortRange() const;
	void SetShortRange(bool value);

	Vector3 GetPosition() const;
	void SetPosition(const Vector3& value);

	void SetRotation(float value);

	void SetScale(float value);

	void ShowCone(bool toggle, int hudColorIndex, float coneWidth = 1.0f, int coneColour = BlipColour::Blue);

	void SetSelectableOnMap(bool selectable);

	void SetRotationWithFloat(float heading);

	void AddBlipForArea(float x, float y, float z, float width, float height);

	void SyncRotationWithEntity(int entityHandle);

	void ShowRoute(bool value);

	int Icon() const;
	void SetIcon(int value);

	std::string IconName() const;

	void SetBlipName(const std::string& value);

	int Type() const;

	void HideNumber();
	void ShowNumber(int number);

	bool Exists() const;
	void Remove();

private:
	int mHandle;
	int mConeColour = 3;
};
