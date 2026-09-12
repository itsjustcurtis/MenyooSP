/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "GTAlight.h"

#include "..\macros.h"
#include "..\Scripting\World.h"

// Omni Light

OmniLight::OmniLight()
	: m_position(), m_colour(255, 255, 255), m_range(10.0f), m_intensity(1.0f)
{
}
OmniLight::OmniLight(const Vector3& pos, const RgbS& colour, float range, float intensity)
	: m_position(pos), m_colour(colour), m_range(range), m_intensity(intensity)
{
}

void OmniLight::Draw() const
{
	World::DrawLightWithRange(m_position, m_colour, m_range, m_intensity);
}

void OmniLight::SetPosition(const Vector3& value) { m_position = value; }
Vector3 OmniLight::GetPosition() const { return m_position; }
void OmniLight::SetColour(const RgbS& value) { m_colour = value; }
RgbS OmniLight::GetColour() const { return m_colour; }
void OmniLight::SetRange(float value) { m_range = value; }
float OmniLight::GetRange() const { return m_range; }
void OmniLight::SetIntensity(float value) { m_intensity = value; }
float OmniLight::GetIntensity() const { return m_intensity; }

// Spot Light

SpotLight::SpotLight()
	: m_position(), m_direction(0, 0, -1), m_colour(255, 255, 255),
	m_distance(20.0f), m_brightness(1.0f), m_roundness(0.0f),
	m_radius(1.0f), m_falloff(0.0f), m_useShadow(false), m_shadowId(0)
{
}
SpotLight::SpotLight(const Vector3& pos, const Vector3& dir, const RgbS& colour, float distance, float brightness, float roundness, float radius, float falloff, bool useShadow, int shadowId)
	: m_position(pos), m_direction(dir), m_colour(colour),
	m_distance(distance), m_brightness(brightness), m_roundness(roundness),
	m_radius(radius), m_falloff(falloff), m_useShadow(useShadow), m_shadowId(shadowId)
{
}

void SpotLight::Draw() const
{
	if (m_useShadow)
		World::DrawSpotLightWithShadow(m_position, m_direction, m_colour, m_distance, m_brightness, m_roundness, m_radius, m_falloff, static_cast<float>(m_shadowId));
	else
		World::DrawSpotLight(m_position, m_direction, m_colour, m_distance, m_brightness, m_roundness, m_radius, m_falloff);
}

void SpotLight::SetPosition(const Vector3& value) { m_position = value; }
Vector3 SpotLight::GetPosition() const { return m_position; }
void SpotLight::SetDirection(const Vector3& value) { m_direction = value; }
Vector3 SpotLight::GetDirection() const { return m_direction; }
void SpotLight::SetColour(const RgbS& value) { m_colour = value; }
RgbS SpotLight::GetColour() const { return m_colour; }
void SpotLight::SetDistance(float value) { m_distance = value; }
float SpotLight::GetDistance() const { return m_distance; }
void SpotLight::SetBrightness(float value) { m_brightness = value; }
float SpotLight::GetBrightness() const { return m_brightness; }
void SpotLight::SetRoundness(float value) { m_roundness = value; }
float SpotLight::GetRoundness() const { return m_roundness; }
void SpotLight::SetRadius(float value) { m_radius = value; }
float SpotLight::GetRadius() const { return m_radius; }
void SpotLight::SetFalloff(float value) { m_falloff = value; }
float SpotLight::GetFalloff() const { return m_falloff; }
void SpotLight::SetUseShadow(bool value) { m_useShadow = value; }
bool SpotLight::GetUseShadow() const { return m_useShadow; }
void SpotLight::SetShadowId(int value) { m_shadowId = value; }
int SpotLight::GetShadowId() const { return m_shadowId; }
