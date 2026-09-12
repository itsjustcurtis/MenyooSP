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

#include "..\Natives\types.h"
#include "..\Util\GTAmath.h"

class OmniLight
{
public:
	Vector3 m_position;
	RgbS m_colour;
	float m_range;
	float m_intensity;

	OmniLight();
	OmniLight(const Vector3& pos, const RgbS& colour, float range, float intensity);

	void Draw() const;

	void SetPosition(const Vector3& value);
	Vector3 GetPosition() const;
	void SetColour(const RgbS& value);
	RgbS GetColour() const;
	void SetRange(float value);
	float GetRange() const;
	void SetIntensity(float value);
	float GetIntensity() const;
};

class SpotLight
{
public:
	Vector3 m_position;
	Vector3 m_direction;
	RgbS m_colour;
	float m_distance;
	float m_brightness;
	float m_roundness;
	float m_radius;
	float m_falloff;
	bool m_useShadow;
	int m_shadowId;

	SpotLight();
	SpotLight(const Vector3& pos, const Vector3& dir, const RgbS& colour, float distance, float brightness, float roundness, float radius, float falloff, bool useShadow = false, int shadowId = 0);

	void Draw() const;

	void SetPosition(const Vector3& value);
	Vector3 GetPosition() const;
	void SetDirection(const Vector3& value);
	Vector3 GetDirection() const;
	void SetColour(const RgbS& value);
	RgbS GetColour() const;
	void SetDistance(float value);
	float GetDistance() const;
	void SetBrightness(float value);
	float GetBrightness() const;
	void SetRoundness(float value);
	float GetRoundness() const;
	void SetRadius(float value);
	float GetRadius() const;
	void SetFalloff(float value);
	float GetFalloff() const;
	void SetUseShadow(bool value);
	bool GetUseShadow() const;
	void SetShadowId(int value);
	int GetShadowId() const;
};
