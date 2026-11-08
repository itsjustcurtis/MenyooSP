/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
// Direct access to the game's UI3DDrawManager (the system behind the UI3DSCENE_* natives, which only accept peds).
// Two ped-only steps run for every slot entity and crash on anything else, so both call sites are hooked and skipped
// for the preview entity: the main-thread pre-render and the per-slot draw prep.
#include "UI3DScene.h"

#include "GTAmemory.h"
#include "Hooking.h"
#include "..\Util\FileLogger.h"

#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>

namespace UI3DScene
{
	namespace
	{
		struct Offsets
		{
			uint32_t presetArray;
			uint32_t presetCount;
			uint32_t slots;
			uint32_t activePreset;
		};
		constexpr Offsets kLegacy{ 0x808, 0x810, 0x820, 0xA00 };
		constexpr Offsets kEnhanced{ 0x7E8, 0x7F0, 0x800, 0x9E0 };

		constexpr int kSlotCount = 5;
		constexpr uint32_t kSlotStride = 0x60;
		constexpr uint32_t kSlotRotOffset = 0x20;

		constexpr uint32_t kPresetSize = 0x2A0;
		constexpr uint32_t kPresetElements = 0x10;
		constexpr uint32_t kElementStride = 0x80;
		constexpr uint32_t kPresetElementCount = 0x290;

		constexpr uint32_t kElemPos = 0x10;
		constexpr uint32_t kElemPos43 = 0x20;
		constexpr uint32_t kElemRot = 0x30;
		constexpr uint32_t kElemRect = 0x40;
		constexpr uint32_t kElemBgColour = 0x60;
		constexpr uint32_t kElemLightIntensity = 0x64;
		constexpr uint32_t kElemEnabled = 0x68;

		// The scene camera's vertical FOV is re-read every frame by the viewport setup; the far plane is fixed at 1000.
		constexpr uint32_t kCameraFovLegacy = 0x520;
		constexpr uint32_t kCameraFovEnhanced = 0x500;

		// Two light rigs (lerped by time of day) of 4 point lights placed at the element position + offset.
		constexpr uint32_t kLightRigLegacy = 0x530;
		constexpr uint32_t kLightRigEnhanced = 0x510;
		constexpr int kLightRigs = 2;
		constexpr uint32_t kLightRigStride = 176;
		constexpr int kLightsPerRig = 4;
		constexpr uint32_t kLightStride = 32;
		constexpr uint32_t kLightPos = 0x00;
		constexpr uint32_t kLightRadius = 0x14;

		typedef bool(__fastcall* PushPreset_t)(void* mgr, uint32_t* presetHash);
		typedef bool(__fastcall* AssignToSlot_t)(void* mgr, uint32_t* presetHash, uint32_t slot, void* entity, float* pos4, float alpha);
		typedef void(__fastcall* PreRender_t)(void* mgr, void* a2, void* a3, void* a4);
		typedef void(__fastcall* SlotDrawPrep_t)(void* entity);

		Offsets g_off{};
		void** g_mgrGlobal = nullptr;
		PushPreset_t g_pushPreset = nullptr;
		AssignToSlot_t g_assignToSlot = nullptr;
		CallHook<PreRender_t>* g_preRenderHook = nullptr;
		CallHook<SlotDrawPrep_t>* g_slotDrawPrepHook = nullptr;
		void* volatile g_excludedEntity = nullptr;
		bool g_ready = false;

		struct SavedElement
		{
			uint8_t* element;
			uint8_t data[kElementStride];
		};
		std::vector<SavedElement> g_savedElements;

		uint8_t g_savedLightRig[kLightRigs * kLightRigStride] = {};
		uint8_t* g_lightRigPatched = nullptr;

		float g_savedCameraFov = 0.0f;
		float* g_cameraFovPatched = nullptr;

		inline uint8_t* Mgr()
		{
			return g_mgrGlobal ? reinterpret_cast<uint8_t*>(*g_mgrGlobal) : nullptr;
		}

		inline float* CameraFovPtr(uint8_t* mgr)
		{
			return reinterpret_cast<float*>(mgr + (g_isEnhanced ? kCameraFovEnhanced : kCameraFovLegacy));
		}

		inline uint8_t* LightRigPtr(uint8_t* mgr)
		{
			return mgr + (g_isEnhanced ? kLightRigEnhanced : kLightRigLegacy);
		}

		inline void** SlotEntity(uint8_t* mgr, int slot)
		{
			return reinterpret_cast<void**>(mgr + g_off.slots + slot * kSlotStride);
		}

		uint8_t* FindPreset(uint8_t* mgr, uint32_t hash)
		{
			auto presets = *reinterpret_cast<uint8_t**>(mgr + g_off.presetArray);
			uint16_t count = *reinterpret_cast<uint16_t*>(mgr + g_off.presetCount);
			if (!presets)
				return nullptr;
			for (uint16_t i = 0; i < count; i++)
			{
				uint8_t* preset = presets + i * kPresetSize;
				if (*reinterpret_cast<uint32_t*>(preset) == hash)
					return preset;
			}
			return nullptr;
		}

		void __fastcall PreRenderStub(void* mgr, void* a2, void* a3, void* a4)
		{
			void* hidden[kSlotCount] = {};
			auto m = reinterpret_cast<uint8_t*>(mgr);
			if (m && g_excludedEntity)
			{
				for (int i = 0; i < kSlotCount; i++)
				{
					void** ref = SlotEntity(m, i);
					if (*ref == g_excludedEntity)
					{
						// Raw swap (no fwRegdRef add/remove): the entity cannot be deleted during this call.
						hidden[i] = *ref;
						*ref = nullptr;
					}
				}
			}

			g_preRenderHook->fn(mgr, a2, a3, a4);

			if (m)
			{
				for (int i = 0; i < kSlotCount; i++)
				{
					if (hidden[i])
						*SlotEntity(m, i) = hidden[i];
				}
			}
		}

		// The slot draw calls drawHandler->AddToDrawList(entity, nullptr); non-ped draw handlers read the params and
		// crash on null. The preview entity's draw handler gets a private vtable copy that substitutes params.
		constexpr uint32_t kEntityDrawHandler = 0x48;
		constexpr int kVtableCopyEntries = 48;
		typedef void*(__fastcall* AddToDrawList_t)(void* drawHandler, void* entity, void* params, void* a4);

		int g_addToDrawListIndex = 2;
		AddToDrawList_t g_origAddToDrawList = nullptr;
		void* g_vtableCopy[kVtableCopyEntries] = {}; // never freed: a destroyed handler may still reference it
		void** g_patchedHandler = nullptr;
		void* g_patchedHandlerVtable = nullptr;
		alignas(16) uint8_t g_defaultDrawParams[64] = { 0xFF, 0 };
		// Byte 1 = 1 makes objects draw directly instead of going into the world-phase instancing batch.
		alignas(16) uint8_t g_extraDrawParams[64] = { 0xFF, 1 };

		typedef void(__fastcall* DlAlign_t)(void* dlMgr, uint32_t alignment);
		typedef uint8_t*(__fastcall* DlAlloc_t)(uint32_t size, uint32_t a2);
		void** g_dlMgrGlobal = nullptr;
		DlAlign_t g_dlAlign = nullptr;
		DlAlloc_t g_dlAlloc = nullptr;

		// Enhanced only: the slot draw refreshes the draw handler's per-frame data right before AddToDrawList. Entities
		// the world pass didn't see (our chained attachments) are otherwise never refreshed and draw nothing.
		typedef void(__fastcall* PrepareDraw_t)(void* entity);
		PrepareDraw_t g_prepareDraw = nullptr;

		constexpr uint32_t kEntityMatrix = 0x60;
		constexpr uint32_t kCmdOverrideMatrix = 0xD3018;

		bool g_previewEntityDrawn = true;

		constexpr int kMaxExtraEntities = 2048;
		UI3DScene::ExtraEntity g_extraEntities[kMaxExtraEntities] = {};
		int g_extraEntityCount = 0;
		bool g_drawingExtras = false;

		void DlAlign16()
		{
			uint8_t* mgr = g_dlMgrGlobal ? reinterpret_cast<uint8_t*>(*g_dlMgrGlobal) : nullptr;
			if (!mgr)
				return;
			if (!g_isEnhanced)
			{
				g_dlAlign(mgr, 16);
				return;
			}
			// Enhanced inlines the alignment: pad with a 0x7F000 command up to the next 16 bytes.
			uint32_t& cursor = *reinterpret_cast<uint32_t*>(mgr + 0x1830);
			uint32_t mis = cursor & 0xF;
			if (!mis)
				return;
			uint8_t* page = *reinterpret_cast<uint8_t**>(mgr + 24ull * (static_cast<int32_t>(cursor) >> 24) + 0x810);
			uint32_t pad = 16 - mis;
			*reinterpret_cast<uint32_t*>(page + (cursor & 0xFFFFFF)) = (pad >> 2) | 0x7F000;
			cursor = (pad + (cursor & 0xFFFFFF)) | (cursor & 0xFF000000);
		}

		// Same world matrix override command the slot draw emits for its own entity. Without it, chained peds would all
		// be drawn at the root's matrix.
		void EmitOverrideMatrix(const uint8_t* entity)
		{
			if (!g_dlAlloc || !entity)
				return;
			DlAlign16();
			uint8_t* cmd = g_dlAlloc(96, 0);
			if (!cmd)
				return;
			float* m = reinterpret_cast<float*>(cmd + 0x10);
			memcpy(m, entity + kEntityMatrix, 64);
			m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;
			*reinterpret_cast<uint64_t*>(cmd + 0x50) = 0;
			cmd[0x58] = 1;
			auto& header = *reinterpret_cast<uint32_t*>(cmd);
			header = (header & 0xFFF00000) | kCmdOverrideMatrix;
		}

		void* __fastcall AddToDrawListStub(void* drawHandler, void* entity, void* params, void* a4)
		{
			const bool fromUiScene = (params == nullptr);
			if (!params)
				params = g_defaultDrawParams;
			void* result = nullptr;
			if (!fromUiScene || g_previewEntityDrawn)
				result = g_origAddToDrawList(drawHandler, entity, params, a4);

			// A slot only draws one entity, so attachments are drawn from inside the root's draw, in the same pass.
			if (fromUiScene && !g_drawingExtras && g_extraEntityCount > 0)
			{
				g_drawingExtras = true;
				for (int i = 0; i < g_extraEntityCount; i++)
				{
					auto e = reinterpret_cast<uint8_t*>(g_extraEntities[i].entity);
					if (!e || e == entity || g_extraEntities[i].alpha == 0)
						continue;
					auto dh = *reinterpret_cast<void***>(e + kEntityDrawHandler);
					if (!dh || !*dh)
						continue;
					auto fn = reinterpret_cast<AddToDrawList_t>(reinterpret_cast<void**>(*dh)[g_addToDrawListIndex]);
					if (fn == &AddToDrawListStub)
						continue;
					if (g_extraEntities[i].isPed && g_slotDrawPrepHook)
						g_slotDrawPrepHook->fn(e);
					if (g_prepareDraw)
						g_prepareDraw(e);
					EmitOverrideMatrix(e);
					alignas(16) uint8_t extraParams[sizeof(g_extraDrawParams)];
					memcpy(extraParams, g_extraDrawParams, sizeof(extraParams));
					extraParams[0] = (std::min)(g_extraEntities[i].alpha, g_extraDrawParams[0]);
					fn(dh, e, extraParams, nullptr);
				}
				g_drawingExtras = false;
			}
			return result;
		}

		void RestoreDrawHandler(uint8_t* entity)
		{
			// Only write if the entity still owns the handler we patched (it may have been recreated).
			if (g_patchedHandler && entity
				&& *reinterpret_cast<void***>(entity + kEntityDrawHandler) == g_patchedHandler
				&& *g_patchedHandler == g_vtableCopy)
			{
				*g_patchedHandler = g_patchedHandlerVtable;
			}
			g_patchedHandler = nullptr;
			g_patchedHandlerVtable = nullptr;
		}

		bool PatchDrawHandler(uint8_t* entity)
		{
			auto dh = *reinterpret_cast<void***>(entity + kEntityDrawHandler);
			if (!dh || !*dh)
				return false;
			if (*dh == g_vtableCopy)
			{
				g_patchedHandler = dh;
				return true;
			}

			g_patchedHandler = nullptr;
			auto vtable = reinterpret_cast<void**>(*dh);
			memcpy(g_vtableCopy, vtable, sizeof(g_vtableCopy));
			g_origAddToDrawList = reinterpret_cast<AddToDrawList_t>(vtable[g_addToDrawListIndex]);
			g_vtableCopy[g_addToDrawListIndex] = reinterpret_cast<void*>(&AddToDrawListStub);
			g_patchedHandlerVtable = vtable;
			*dh = g_vtableCopy;
			g_patchedHandler = dh;
			return true;
		}

		void __fastcall SlotDrawPrepStub(void* entity)
		{
			if (entity && entity == g_excludedEntity)
				return;
			g_slotDrawPrepHook->fn(entity);
		}

		template <typename T>
		CallHook<T>* HookCall(DWORD64 callInsn, T stub)
		{
			if (*reinterpret_cast<uint8_t*>(callInsn) != 0xE8)
				return nullptr;
			DWORD oldProtect;
			if (!VirtualProtect(reinterpret_cast<void*>(callInsn), 5, PAGE_EXECUTE_READWRITE, &oldProtect))
				return nullptr;
			auto hook = HookManager::SetCall(static_cast<uintptr_t>(callInsn), stub);
			VirtualProtect(reinterpret_cast<void*>(callInsn), 5, oldProtect, &oldProtect);
			return hook;
		}

		template <typename T>
		void Unhook(CallHook<T>*& hook)
		{
			if (!hook)
				return;
			DWORD oldProtect;
			void* site = reinterpret_cast<void*>(hook->address);
			VirtualProtect(site, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
			delete hook;
			VirtualProtect(site, 5, oldProtect, &oldProtect);
			hook = nullptr;
		}

		DWORD64 Scan(const char* name, const std::string& pattern)
		{
			DWORD64 addr = MemryScan::PatternScanner::FindPattern(pattern);
			if (!addr)
				addlog(ige::LogType::LOG_ERROR, std::string("UI3DScene: pattern not found: ") + name);
			return addr;
		}
	}

	bool Init()
	{
		if (g_ready)
			return true;

		const char* hookPattern;
		const char* drawPrepPattern;
		const char* pushPattern;
		const char* assignPattern;
		DWORD64 drawPrepCallOffset;
		if (g_isEnhanced)
		{
			g_off = kEnhanced;
			g_addToDrawListIndex = 3;
			hookPattern = "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? E8";
			drawPrepPattern = "F2 44 0F 10 84 29 10 08 00 00 48 89 F1 E8 ? ? ? ? E8 ? ? ? ? 34 01";
			drawPrepCallOffset = 0x0D;
			pushPattern = "44 0F B7 81 F0 07 00 00 4D 85 C0 74 ? 8B 12 48 8B 81 E8 07 00 00";
			assignPattern = "56 57 53 48 83 EC 40 0F 29 7C 24 30 0F 29 74 24 20 48 83 B9 E0 09 00 00 00 0F 84";
		}
		else
		{
			g_off = kLegacy;
			g_addToDrawListIndex = 2;
			hookPattern = "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 8B 15";
			drawPrepPattern = "49 8B 8C 0E 20 08 00 00 E8 ? ? ? ? F3 0F 10 05";
			drawPrepCallOffset = 0x08;
			pushPattern = "40 53 48 83 EC 20 8B 02 48 8D 54 24 38 48 8B D9 89 44 24 38 E8 ? ? ? ? 48 8B C8 33 C0 48 85";
			assignPattern = "4C 8B DC 48 83 EC 58 8B 02 0F 28 05 ? ? ? ? 49 8D 53 10 89 44 24 68 49 8D 43 E8";
		}

		DWORD64 hookSite = Scan("UI3D pre-render call", hookPattern);
		DWORD64 drawPrepSite = Scan("UI3D slot draw prep call", drawPrepPattern);
		DWORD64 push = Scan("UI3D PushPreset", pushPattern);
		DWORD64 assign = Scan("UI3D AssignToSlot", assignPattern);
		if (!hookSite || !drawPrepSite || !push || !assign)
			return false;

		auto rel32 = [](DWORD64 insn, int dispOffset, int insnLen) { return insn + insnLen + *reinterpret_cast<int32_t*>(insn + dispOffset); };
		if (g_isEnhanced)
		{
			DWORD64 a = Scan("UI3D draw-list alloc", "48 8B 05 ? ? ? ? 8B A8 30 18 00 00 89 EA 83 E2 0F 74 ? 89 E9 C1 F9 18 48 8D 0C 49 48 8B 8C C8 10 08 00 00 81 E5 FF FF FF 00 BB 10 00 00 00 29 D3 89 DA C1 EA 02 81 CA 00 F0 07 00 89 14 29 8B 88 30 18 00 00 89 CA 81 E2 FF FF FF 00 01 DA 81 E1 00 00 00 FF 09 D1 89 88 30 18 00 00 B9 60 00 00 00 31 D2 E8");
			if (a)
			{
				g_dlMgrGlobal = reinterpret_cast<void**>(rel32(a, 3, 7));
				g_dlAlloc = reinterpret_cast<DlAlloc_t>(rel32(a + 101, 1, 5));
			}
			g_prepareDraw = reinterpret_cast<PrepareDraw_t>(Scan("UI3D draw prepare", "48 89 CA 48 8B 49 48 48 85 C9 74 14 48 8B 05 ? ? ? ? 44 8B 80 E4 18 00 00 44 39 41 2C 75 01 C3"));
		}
		else
		{
			DWORD64 a = Scan("UI3D draw-list alloc", "48 8B 0D ? ? ? ? 8D 7B 10 8B D7 E8 ? ? ? ? 8D 4B 60 33 D2 E8 ? ? ? ? 48 85 C0");
			if (a)
			{
				g_dlMgrGlobal = reinterpret_cast<void**>(rel32(a, 3, 7));
				g_dlAlign = reinterpret_cast<DlAlign_t>(rel32(a + 12, 1, 5));
				g_dlAlloc = reinterpret_cast<DlAlloc_t>(rel32(a + 22, 1, 5));
			}
		}
		if (!g_dlAlloc || (!g_isEnhanced && !g_dlAlign))
			g_dlAlloc = nullptr;

		DWORD64 movInsn = hookSite + 0x0C;
		g_mgrGlobal = reinterpret_cast<void**>(movInsn + 7 + *reinterpret_cast<int32_t*>(movInsn + 3));
		g_pushPreset = reinterpret_cast<PushPreset_t>(push);
		g_assignToSlot = reinterpret_cast<AssignToSlot_t>(assign);

		g_preRenderHook = HookCall(hookSite + 0x13, &PreRenderStub);
		g_slotDrawPrepHook = HookCall(drawPrepSite + drawPrepCallOffset, &SlotDrawPrepStub);
		if (!g_preRenderHook || !g_slotDrawPrepHook)
		{
			// Both are required: a non-ped in a slot without either hook corrupts memory.
			Unhook(g_preRenderHook);
			Unhook(g_slotDrawPrepHook);
			addlog(ige::LogType::LOG_ERROR, "UI3DScene: failed to install hooks");
			return false;
		}

		g_ready = true;
		addlog(ige::LogType::LOG_INFO, "UI3DScene: ready");
		return true;
	}

	void Shutdown()
	{
		g_excludedEntity = nullptr;
		Unhook(g_preRenderHook);
		Unhook(g_slotDrawPrepHook);
		g_ready = false;
	}

	bool IsReady()
	{
		return g_ready && Mgr() != nullptr;
	}

	bool PushPreset(uint32_t presetHash)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return false;
		return g_pushPreset(mgr, &presetHash);
	}

	bool IsPresetActive(uint32_t presetHash)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return false;
		auto active = *reinterpret_cast<uint8_t**>(mgr + g_off.activePreset);
		return active && *reinterpret_cast<uint32_t*>(active) == presetHash;
	}

	bool AssignEntityToSlot(uint32_t presetHash, int slot, void* entity, const float pos[3], const float rot[3], float alpha)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr || slot < 0 || slot >= kSlotCount)
			return false;

		alignas(16) float pos4[4] = { pos[0], pos[1], pos[2], 0.0f };
		if (!g_assignToSlot(mgr, &presetHash, static_cast<uint32_t>(slot), entity, pos4, alpha))
			return false;

		// The game's assign always resets the rotation offset; write ours afterwards.
		auto rotOut = reinterpret_cast<float*>(mgr + g_off.slots + slot * kSlotStride + kSlotRotOffset);
		rotOut[0] = rot[0];
		rotOut[1] = rot[1];
		rotOut[2] = rot[2];
		rotOut[3] = 0.0f;
		return true;
	}

	bool PatchPresetElement(uint32_t presetHash, int element, const ElementPatch& patch)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return false;
		uint8_t* preset = FindPreset(mgr, presetHash);
		if (!preset || element < 0 || element >= *reinterpret_cast<int*>(preset + kPresetElementCount))
			return false;

		uint8_t* e = preset + kPresetElements + element * kElementStride;

		const uint8_t* orig = nullptr;
		for (auto& s : g_savedElements)
			if (s.element == e) { orig = s.data; break; }
		if (!orig)
		{
			SavedElement s{ e, {} };
			memcpy(s.data, e, kElementStride);
			g_savedElements.push_back(s);
			orig = g_savedElements.back().data;
		}

		auto f = [e](uint32_t off) { return reinterpret_cast<float*>(e + off); };
		memcpy(f(kElemPos), patch.pos, sizeof(float) * 3);
		memcpy(f(kElemPos43), patch.pos, sizeof(float) * 3);
		memcpy(f(kElemRot), patch.rot, sizeof(float) * 3);
		memcpy(f(kElemRect), patch.rect, sizeof(float) * 4);
		*reinterpret_cast<uint32_t*>(e + kElemBgColour) = patch.bgColour;
		if (patch.lightIntensity > 0.0f)
			*f(kElemLightIntensity) = *reinterpret_cast<const float*>(orig + kElemLightIntensity) * patch.lightIntensity;
		*(e + kElemEnabled) = 1;
		return true;
	}

	bool ReadPresetElement(uint32_t presetHash, int element, ElementPatch& out, bool* enabled, int* elementCount)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return false;
		uint8_t* preset = FindPreset(mgr, presetHash);
		if (!preset)
			return false;
		int count = *reinterpret_cast<int*>(preset + kPresetElementCount);
		if (elementCount)
			*elementCount = count;
		if (element < 0 || element >= count)
			return false;

		uint8_t* e = preset + kPresetElements + element * kElementStride;
		const uint8_t* src = e;
		for (auto& s : g_savedElements)
			if (s.element == e) { src = s.data; break; }

		memcpy(out.pos, src + kElemPos, sizeof(float) * 3);
		memcpy(out.rot, src + kElemRot, sizeof(float) * 3);
		memcpy(out.rect, src + kElemRect, sizeof(float) * 4);
		out.bgColour = *reinterpret_cast<const uint32_t*>(src + kElemBgColour);
		out.lightIntensity = *reinterpret_cast<const float*>(src + kElemLightIntensity);
		if (enabled)
			*enabled = src[kElemEnabled] != 0;
		return true;
	}

	float GetOriginalCameraFov()
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return 40.0f;
		float fov = g_cameraFovPatched == CameraFovPtr(mgr) ? g_savedCameraFov : *CameraFovPtr(mgr);
		return (fov > 1.0f && fov < 170.0f) ? fov : 40.0f;
	}

	void SetCameraFov(float fov)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return;
		float* p = CameraFovPtr(mgr);
		if (g_cameraFovPatched != p)
		{
			if (fov <= 0.0f)
				return;
			g_savedCameraFov = *p;
			g_cameraFovPatched = p;
		}
		*p = fov > 0.0f ? fov : g_savedCameraFov;
	}

	void SetLightRigScale(float positionScale, float radiusScale, float coverRadius)
	{
		uint8_t* mgr = Mgr();
		if (!g_ready || !mgr)
			return;
		uint8_t* rig = LightRigPtr(mgr);
		if (g_lightRigPatched != rig)
		{
			memcpy(g_savedLightRig, rig, sizeof(g_savedLightRig));
			g_lightRigPatched = rig;
		}
		for (int r = 0; r < kLightRigs; r++)
			for (int l = 0; l < kLightsPerRig; l++)
			{
				uint32_t o = r * kLightRigStride + l * kLightStride;
				auto src = reinterpret_cast<const float*>(g_savedLightRig + o);
				auto dst = reinterpret_cast<float*>(rig + o);
				float distSq = 0.0f;
				for (int i = 0; i < 3; i++)
				{
					dst[kLightPos / 4 + i] = src[kLightPos / 4 + i] * positionScale;
					distSq += dst[kLightPos / 4 + i] * dst[kLightPos / 4 + i];
				}
				// Oversized light volumes run past the far plane and stop rendering, so only reach across the build.
				dst[kLightRadius / 4] = coverRadius > 0.0f
					? (std::max)(src[kLightRadius / 4], std::sqrt(distSq) + coverRadius)
					: src[kLightRadius / 4] * radiusScale;
			}
	}

	void RestorePresetPatches()
	{
		for (auto& s : g_savedElements)
			memcpy(s.element, s.data, kElementStride);
		g_savedElements.clear();
		uint8_t* mgr = Mgr();
		if (mgr && g_lightRigPatched == LightRigPtr(mgr))
			memcpy(g_lightRigPatched, g_savedLightRig, sizeof(g_savedLightRig));
		g_lightRigPatched = nullptr;
		if (mgr && g_cameraFovPatched == CameraFovPtr(mgr))
			*g_cameraFovPatched = g_savedCameraFov;
		g_cameraFovPatched = nullptr;
	}

	bool SetPreviewEntity(void* entity)
	{
		auto e = reinterpret_cast<uint8_t*>(entity);
		auto previous = reinterpret_cast<uint8_t*>(g_excludedEntity);
		if (previous && previous != e)
			RestoreDrawHandler(previous);

		if (!e)
		{
			g_excludedEntity = nullptr;
			g_extraEntityCount = 0;
			g_previewEntityDrawn = true;
			return false;
		}
		g_excludedEntity = e;
		return g_ready && PatchDrawHandler(e);
	}

	void SetPreviewExtraEntities(const ExtraEntity* entities, int count)
	{
		if (count < 0)
			count = 0;
		if (count > kMaxExtraEntities)
			count = kMaxExtraEntities;
		for (int i = 0; i < count; i++)
			g_extraEntities[i] = entities[i];
		g_extraEntityCount = count;
	}

	void SetPreviewEntityDrawn(bool drawn)
	{
		g_previewEntityDrawn = drawn;
	}

	void SetDefaultDrawParams(uint8_t alphaFade, uint8_t flag)
	{
		g_defaultDrawParams[0] = alphaFade;
		g_defaultDrawParams[1] = flag;
	}

	void SetExtraDrawAlpha(uint8_t alphaFade)
	{
		g_extraDrawParams[0] = alphaFade;
	}
}
