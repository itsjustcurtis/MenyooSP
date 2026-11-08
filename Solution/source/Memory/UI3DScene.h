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

#include <cstdint>

namespace UI3DScene
{
	bool Init();
	void Shutdown();
	bool IsReady();

	bool PushPreset(uint32_t presetHash);
	bool IsPresetActive(uint32_t presetHash);

	bool AssignEntityToSlot(uint32_t presetHash, int slot, void* entity, const float pos[3], const float rot[3], float alpha = 1.0f);

	struct ElementPatch
	{
		float pos[3];
		float rot[3];
		float rect[4];
		uint32_t bgColour;
		float lightIntensity;
	};
	bool PatchPresetElement(uint32_t presetHash, int element, const ElementPatch& patch);
	bool ReadPresetElement(uint32_t presetHash, int element, ElementPatch& out, bool* enabled = nullptr, int* elementCount = nullptr);
	void RestorePresetPatches();

	void SetLightRigScale(float positionScale, float radiusScale, float coverRadius = 0.0f);

	void SetCameraFov(float fov);
	float GetOriginalCameraFov();

	bool SetPreviewEntity(void* entity);

	struct ExtraEntity
	{
		void* entity;
		bool isPed;
		uint8_t alpha;
	};
	void SetPreviewExtraEntities(const ExtraEntity* entities, int count);

	void SetPreviewEntityDrawn(bool drawn);

	void SetDefaultDrawParams(uint8_t alphaFade, uint8_t flag);
	void SetExtraDrawAlpha(uint8_t alphaFade);
}
