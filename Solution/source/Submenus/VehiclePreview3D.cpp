/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
// Live 3D previews (vehicles, Menyoo vehicle XMLs and Spooner maps) drawn through the game's UI3DSCENE system.
// The UI3D camera sits at the world origin, so the real entities are moved to the scene position every frame.
#include "VehiclePreview3D.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\Model.h"
#include "..\Memory\GTAmemory.h"
#include "..\Util\FileLogger.h"
#include "..\Menu\Menu.h"
#include "VehicleSpawner.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <Windows.h>

namespace VehiclePreview3D
{
	bool g_enabled = true;
	bool g_mapPreviews = true;
	float g_zoom = 1.85f;
	float g_spinSpeed = 30.0f;
	float g_boxHeight = 1.6f;
	float g_boxWidth = 1.6f;
	float g_lightIntensity = 1.5f;
	bool g_scaleLighting = true;

	namespace Debug
	{
		bool useOriginalPosition = false;
		bool brightBackground = false;
		bool moveEntityToScene = true;
		float offset[3] = { 0.0f, 0.0f, 0.0f };
		int drawAlpha = 255;
		bool drawAttachedPeds = true;
		bool drawAttachedObjects = true;
		bool drawAttachedVehicles = true;
		float lightSpread = 1.0f;
		float mapTilt = 30.0f;
	}

	namespace
	{
		// PAUSE_SINGLE_MIDDLE, borrowed from the pause menu while a preview is shown.
		constexpr uint32_t kPreset = 0xAD197067;
		constexpr int kSlot = 0;
		constexpr float kHideHeight = 40.0f;
		constexpr uint32_t kBgColour = 0xC8000000;
		constexpr uint32_t kDebugBgColour = 0xFFFF00FF;
		constexpr int kSettleFrames = 3;
		constexpr int kBoundsFrames = 30;
		constexpr DWORD kFileHoverDelayMs = 200;

		struct Source
		{
			bool isFile = false;
			Hash model = 0;
			std::string path;
			bool operator==(const Source& o) const { return isFile == o.isFile && (isFile ? path == o.path : model == o.model); }
			bool operator!=(const Source& o) const { return !(*this == o); }
		};

		Source g_current;
		bool g_haveCurrent = false;
		Source g_pending;
		DWORD g_pendingSince = 0;

		Vehicle g_vehicle = 0;
		std::vector<Entity> g_attachments;
		std::vector<int> g_attachmentTypes;
		bool g_rootIsDummy = false;
		float g_cameraFov = 0.0f;
		float g_tiltRad = 0.0f;
		Hash g_requestedModel = 0;
		float g_heading = 0.0f;
		int g_lastDrawFrame = -10;
		int g_spawnFrame = 0;
		bool g_patchActive = false;
		std::unique_ptr<sub::VehicleSaver::VehiclePreviewBuild> g_build;
		int g_lastGeneration = -1;
		float g_buildHeading = 0.0f;
		bool g_unavailable = false; // current file can't be previewed (too big / invalid): caller shows its image
		bool g_loggedBuilt = false;
		bool g_loggedFraming = false;

		// Combined bounding box of the root and its attachments, in the root's local space.
		float g_bbMin[3] = { -1.0f, -2.0f, -0.5f };
		float g_bbMax[3] = { 1.0f, 2.0f, 1.0f };
		int g_boundsUpdates = 0;

		void DeleteEntityNow(Entity e)
		{
			if (e && DOES_ENTITY_EXIST(e))
			{
				SET_ENTITY_AS_MISSION_ENTITY(e, true, true);
				DELETE_ENTITY(&e);
			}
		}

		// The render thread runs a frame behind and the chained attachment draws don't hold the references the world
		// pass takes, so attachments are only deleted a few frames after they stop being drawn.
		constexpr int kDeleteDelayFrames = 4;
		struct PendingDelete { Entity e; int frame; };
		std::vector<PendingDelete> g_pendingDeletes;

		void DeleteEntityDelayed(Entity e)
		{
			if (e)
				g_pendingDeletes.push_back({ e, GET_FRAME_COUNT() });
		}

		void ProcessPendingDeletes()
		{
			if (g_pendingDeletes.empty())
				return;
			const int frame = GET_FRAME_COUNT();
			size_t keep = 0;
			for (auto& p : g_pendingDeletes)
			{
				if (frame - p.frame >= kDeleteDelayFrames)
					DeleteEntityNow(p.e);
				else
					g_pendingDeletes[keep++] = p;
			}
			g_pendingDeletes.resize(keep);
		}

		void DeleteVehicle()
		{
			UI3DScene::SetPreviewEntity(nullptr);
			// The root is deleted immediately: the UI3D slot keeps a reference to it that only deletion clears, and an
			// unregistered root drawn through the slot crashes.
			if (g_build)
			{
				Vehicle buildRoot = g_build->Root();
				for (Entity e : g_build->TakeAllEntities())
				{
					if (e == buildRoot)
						DeleteEntityNow(e);
					else
						DeleteEntityDelayed(e);
				}
				g_build.reset();
			}
			for (Entity a : g_attachments)
				DeleteEntityDelayed(a);
			g_attachments.clear();
			g_attachmentTypes.clear();
			g_rootIsDummy = false;
			if (g_vehicle)
				DeleteEntityNow(g_vehicle);
			g_vehicle = 0;
			g_haveCurrent = false;
			g_unavailable = false;
		}

		Vector3 InitialPosition()
		{
			Vector3 p = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
			p.z += kHideHeight;
			return p;
		}

		void MakeInert(Vehicle v)
		{
			SET_ENTITY_AS_MISSION_ENTITY(v, true, true);
			FREEZE_ENTITY_POSITION(v, false);
			SET_ENTITY_COLLISION(v, false, false);
			SET_ENTITY_HAS_GRAVITY(v, false);
			SET_ENTITY_INVINCIBLE(v, true);
			if (IS_ENTITY_A_VEHICLE(v))
			{
				SET_VEHICLE_ENGINE_ON(v, false, true, true);
				SET_VEHICLE_RADIO_ENABLED(v, false);
				SET_VEHICLE_LOD_MULTIPLIER(v, 10.0f);
			}
			SET_ENTITY_LOD_DIST(v, 0xFFFF);
		}

		struct Mat { float r[4], f[4], u[4], p[4]; };
		constexpr uint32_t kEntityMatrixOffset = 0x60;

		bool ReadMatrix(Entity e, Mat& m)
		{
			auto addr = reinterpret_cast<const uint8_t*>(GTAmemory::GetEntityAddress(e));
			if (!addr)
				return false;
			memcpy(&m, addr + kEntityMatrixOffset, sizeof(m));
			return true;
		}

		void ModelDims(Entity e, float mn[3], float mx[3])
		{
			static std::unordered_map<Hash, std::pair<Vector3_t, Vector3_t>> cache;
			Hash model = GET_ENTITY_MODEL(e);
			auto it = cache.find(model);
			if (it == cache.end())
			{
				Vector3_t dmin{}, dmax{};
				GET_MODEL_DIMENSIONS(model, &dmin, &dmax);
				it = cache.emplace(model, std::make_pair(dmin, dmax)).first;
			}
			mn[0] = it->second.first.x; mn[1] = it->second.first.y; mn[2] = it->second.first.z;
			mx[0] = it->second.second.x; mx[1] = it->second.second.y; mx[2] = it->second.second.z;
		}

		void UpdateBounds()
		{
			const float kMaxReasonableOffset = g_rootIsDummy ? 3000.0f : 100.0f;
			Mat root;
			if (!g_vehicle || !ReadMatrix(g_vehicle, root))
				return;
			float mn[3] = { 1e9f, 1e9f, 1e9f }, mx[3] = { -1e9f, -1e9f, -1e9f };
			auto grow = [&](const float v[3])
			{
				for (int i = 0; i < 3; i++)
				{
					mn[i] = (std::min)(mn[i], v[i]);
					mx[i] = (std::max)(mx[i], v[i]);
				}
			};
			auto toRoot = [&](const float w[3], float out[3])
			{
				float d[3] = { w[0] - root.p[0], w[1] - root.p[1], w[2] - root.p[2] };
				out[0] = d[0] * root.r[0] + d[1] * root.r[1] + d[2] * root.r[2];
				out[1] = d[0] * root.f[0] + d[1] * root.f[1] + d[2] * root.f[2];
				out[2] = d[0] * root.u[0] + d[1] * root.u[1] + d[2] * root.u[2];
			};

			float dmin[3], dmax[3];
			if (!g_rootIsDummy)
			{
				ModelDims(g_vehicle, dmin, dmax);
				grow(dmin);
				grow(dmax);
			}

			struct Candidate { Mat m; float local[3]; Entity e; };
			static std::vector<Candidate> cands;
			cands.clear();
			for (Entity a : g_attachments)
			{
				Candidate cd;
				// A detached entity sits wherever it was spawned, not with the build.
				if (!a || !DOES_ENTITY_EXIST(a) || !IS_ENTITY_ATTACHED(a) || !ReadMatrix(a, cd.m))
					continue;
				toRoot(cd.m.p, cd.local);
				if (std::fabs(cd.local[0]) > kMaxReasonableOffset || std::fabs(cd.local[1]) > kMaxReasonableOffset || std::fabs(cd.local[2]) > kMaxReasonableOffset)
					continue;
				cd.e = a;
				cands.push_back(cd);
			}

			// Maps: a few stray pieces or SLOD shells kilometres across would push the camera past the far plane.
			// Positional outliers are left out of the framing and giant models only count by their position.
			constexpr float kGiantModelSpan = 200.0f;
			float keepRadius = 1e9f, median[3] = { 0.0f, 0.0f, 0.0f };
			if (g_rootIsDummy && cands.size() >= 4)
			{
				std::vector<float> axis(cands.size());
				for (int i = 0; i < 3; i++)
				{
					for (size_t k = 0; k < cands.size(); k++)
						axis[k] = cands[k].local[i];
					std::nth_element(axis.begin(), axis.begin() + axis.size() / 2, axis.end());
					median[i] = axis[axis.size() / 2];
				}
				std::vector<float> dists(cands.size());
				for (size_t k = 0; k < cands.size(); k++)
				{
					float d0 = cands[k].local[0] - median[0], d1 = cands[k].local[1] - median[1], d2 = cands[k].local[2] - median[2];
					dists[k] = std::sqrt(d0 * d0 + d1 * d1 + d2 * d2);
				}
				size_t p80 = (dists.size() * 8) / 10;
				std::nth_element(dists.begin(), dists.begin() + p80, dists.end());
				keepRadius = (std::max)(dists[p80] * 2.5f, 40.0f);
			}

			for (auto& cd : cands)
			{
				const Mat& m = cd.m;
				if (keepRadius < 1e8f)
				{
					float d0 = cd.local[0] - median[0], d1 = cd.local[1] - median[1], d2 = cd.local[2] - median[2];
					if (std::sqrt(d0 * d0 + d1 * d1 + d2 * d2) > keepRadius)
						continue;
				}
				ModelDims(cd.e, dmin, dmax);
				if (g_rootIsDummy && (dmax[0] - dmin[0] > kGiantModelSpan || dmax[1] - dmin[1] > kGiantModelSpan || dmax[2] - dmin[2] > kGiantModelSpan))
				{
					grow(cd.local);
					continue;
				}
				for (int c = 0; c < 8; c++)
				{
					float lx = (c & 1) ? dmax[0] : dmin[0];
					float ly = (c & 2) ? dmax[1] : dmin[1];
					float lz = (c & 4) ? dmax[2] : dmin[2];
					float w[3];
					for (int i = 0; i < 3; i++)
						w[i] = m.p[i] + m.r[i] * lx + m.f[i] * ly + m.u[i] * lz;
					float p[3];
					toRoot(w, p);
					grow(p);
				}
			}
			if (mn[0] > mx[0])
				return;
			memcpy(g_bbMin, mn, sizeof(mn));
			memcpy(g_bbMax, mx, sizeof(mx));
			g_boundsUpdates++;
		}

		void SyncAttachments(const std::vector<Entity>& src)
		{
			if (src.size() == g_attachments.size())
				return;
			for (size_t i = g_attachments.size(); i < src.size(); i++)
			{
				g_attachments.push_back(src[i]);
				g_attachmentTypes.push_back(DOES_ENTITY_EXIST(src[i]) ? GET_ENTITY_TYPE(src[i]) : 0);
			}
		}

		void OnRootCreated(Vehicle v, float heading)
		{
			g_vehicle = v;
			SET_ENTITY_HEADING(v, heading);
			MakeInert(v);
			g_spawnFrame = GET_FRAME_COUNT();
			g_boundsUpdates = 0;
			g_loggedBuilt = false;
			g_loggedFraming = false;
			const float defMin[3] = { -1.0f, -2.0f, -0.5f }, defMax[3] = { 1.0f, 2.0f, 1.0f };
			memcpy(g_bbMin, defMin, sizeof(defMin));
			memcpy(g_bbMax, defMax, sizeof(defMax));
			g_heading = 0.0f;
			UpdateBounds();
		}

		bool Build(const Source& src)
		{
			Vector3 pos = InitialPosition();
			float heading = 0.0f;

			g_current = src;
			g_haveCurrent = true;
			g_buildHeading = heading;

			if (src.isFile)
			{
				g_build.reset(new sub::VehicleSaver::VehiclePreviewBuild());
				if (!g_build->Begin(src.path, pos))
				{
					addlog(ige::LogType::LOG_DEBUG, "VehiclePreview3D: no preview for " + src.path + " (" + g_build->FailReason() + ")");
					g_build.reset(); // g_haveCurrent stays set so it isn't retried every frame
					g_unavailable = true;
					return false;
				}
				g_lastGeneration = -1;
				return false;
			}

			Vehicle v = CREATE_VEHICLE(src.model, pos.x, pos.y, pos.z, heading, false, false, false);
			SET_MODEL_AS_NO_LONGER_NEEDED(src.model);
			if (!v || !DOES_ENTITY_EXIST(v))
				return false;
			SET_VEHICLE_DIRT_LEVEL(v, 0.0f);
			OnRootCreated(v, heading);
			return true;
		}

		bool StepFileBuild()
		{
			if (!g_build)
				return g_vehicle && DOES_ENTITY_EXIST(g_vehicle);
			g_build->Step();
			if (g_build->Failed() && !g_vehicle)
			{
				addlog(ige::LogType::LOG_ERROR, "VehiclePreview3D: failed to build preview from " + g_current.path);
				g_build.reset();
				return false;
			}
			if (!g_vehicle && g_build->Root())
			{
				g_rootIsDummy = g_build->RootIsDummy();
				OnRootCreated(g_build->Root(), g_buildHeading);
			}
			SyncAttachments(g_build->Attachments());
			if (g_build->AttachGeneration() != g_lastGeneration)
			{
				g_lastGeneration = g_build->AttachGeneration();
				g_boundsUpdates = 0;
			}
			if (!g_loggedBuilt && g_build->Complete())
			{
				g_loggedBuilt = true;
				int peds = 0, vehs = 0, objs = 0;
				for (int t : g_attachmentTypes)
					(t == 1 ? peds : t == 2 ? vehs : objs)++;
				addlog(ige::LogType::LOG_DEBUG, "VehiclePreview3D: built " + g_current.path + " (" + std::to_string(g_attachments.size()) + " attachments: "
					+ std::to_string(peds) + " peds, " + std::to_string(vehs) + " vehicles, " + std::to_string(objs) + " objects)");
			}
			return g_vehicle && DOES_ENTITY_EXIST(g_vehicle);
		}

		bool EnsureBuilt(const Source& src)
		{
			if (g_haveCurrent && g_current == src)
				return src.isFile ? StepFileBuild() : (g_vehicle && DOES_ENTITY_EXIST(g_vehicle));

			if (src.isFile)
			{
				// Only build once the same file has stayed selected for a moment, so scrolling past files is cheap.
				if (g_pending != src)
				{
					g_pending = src;
					g_pendingSince = GetTickCount();
					DeleteVehicle();
					return false;
				}
				if (GetTickCount() - g_pendingSince < kFileHoverDelayMs)
					return false;
				DeleteVehicle();
				Build(src);
				return StepFileBuild();
			}

			g_pending = Source();
			DeleteVehicle();
			if (!IS_MODEL_A_VEHICLE(src.model))
				return false;
			REQUEST_MODEL(src.model);
			g_requestedModel = src.model;
			if (!HAS_MODEL_LOADED(src.model))
				return false;
			return Build(src);
		}

		void ToPresetRect(float cx, float cy, float w, float h, float out[4])
		{
			float aspect = GET_ASPECT_RATIO(false);
			float k = aspect > 0.0f ? (16.0f / 9.0f) / aspect : 1.0f;
			float left = cx - w * 0.5f;
			out[0] = (left + (k - 1.0f) * 0.5f) / k;
			out[1] = cy - h * 0.5f;
			out[2] = w / k;
			out[3] = h;
		}

		bool DrawSource(const Source& src, float centreX, float centreY, float width, float height)
		{
			if (!UI3DScene::IsReady() || IS_PAUSE_MENU_ACTIVE())
				return false;

			g_lastDrawFrame = GET_FRAME_COUNT();

			UI3DScene::ElementPatch orig{};
			bool haveOrig = UI3DScene::ReadPresetElement(kPreset, kSlot, orig);
			bool useOrig = Debug::useOriginalPosition && haveOrig;

			uint8_t* entity = nullptr;
			if (EnsureBuilt(src))
			{
				g_lastDrawFrame = GET_FRAME_COUNT();
				if (g_boundsUpdates < kBoundsFrames)
					UpdateBounds();
			}
			else if (g_unavailable)
			{
				if (g_patchActive)
				{
					UI3DScene::RestorePresetPatches();
					g_patchActive = false;
				}
				return false;
			}

			g_heading = std::fmod(g_heading + g_spinSpeed * GET_FRAME_TIME(), 360.0f);

			UI3DScene::ElementPatch patch{};
			float totalHeading = g_heading;
			if (useOrig)
			{
				g_tiltRad = 0.0f;
				memcpy(patch.pos, orig.pos, sizeof(patch.pos));
				memcpy(patch.rot, orig.rot, sizeof(patch.rot));
				totalHeading += patch.rot[2];
			}
			else
			{
				// The camera frames the preset's original position, so the view direction is normalize(origPos).
				float base[3] = { 0.0f, 3.0f, 0.0f };
				if (haveOrig)
					memcpy(base, orig.pos, sizeof(base));
				float len = std::sqrt(base[0] * base[0] + base[1] * base[1] + base[2] * base[2]);
				float dir[3] = { 0.0f, 1.0f, 0.0f };
				if (len > 0.1f)
					for (int i = 0; i < 3; i++) dir[i] = base[i] / len;

				float size[3] = { g_bbMax[0] - g_bbMin[0], g_bbMax[1] - g_bbMin[1], g_bbMax[2] - g_bbMin[2] };
				const float kFitMargin = g_rootIsDummy ? 1.1f : 1.25f;
				constexpr float kMaxReach = 900.0f;
				constexpr float kMaxFov = 110.0f;
				const float baseFov = UI3DScene::GetOriginalCameraFov();
				float boxAspect = (std::max)(height > 0.0f ? (width * GET_ASPECT_RATIO(false)) / height : 16.0f / 9.0f, 0.1f);
				float footprint = (std::max)(0.5f * std::sqrt(size[0] * size[0] + size[1] * size[1]), 0.5f);
				float halfHeight = (std::max)(0.5f * size[2], 0.5f);
				g_tiltRad = g_rootIsDummy ? Debug::mapTilt * 0.017453292f : 0.0f;
				const float ct = std::cos(g_tiltRad), st = std::fabs(std::sin(g_tiltRad));
				const float viewHalfHeight = halfHeight * ct + footprint * st;
				const float depthHalf = footprint * ct + halfHeight * st;
				const float mz = kFitMargin * (std::max)(g_zoom, 0.1f);
				const float K = (std::max)(viewHalfHeight, footprint / boxAspect);
				float tanV = std::tan(baseFov * 0.5f * 0.017453292f);
				float fov = baseFov;
				if ((K / tanV + depthHalf) * mz + depthHalf > kMaxReach)
				{
					float room = (kMaxReach - depthHalf) / mz - depthHalf;
					float needTan = room > 1.0f ? K / room : 1e9f;
					tanV = (std::min)(needTan, std::tan(kMaxFov * 0.5f * 0.017453292f));
					fov = 2.0f * std::atan(tanV) * 57.29578f;
				}
				g_cameraFov = fov;
				float dist = (std::max)(len, (K / tanV + depthHalf) * mz);
				dist = (std::min)(dist, kMaxReach * 0.85f);

				// Root position = target - R * localCentre (R = tilt * heading), so the build spins around its centre.
				float c[3] = { (g_bbMin[0] + g_bbMax[0]) * 0.5f, (g_bbMin[1] + g_bbMax[1]) * 0.5f, (g_bbMin[2] + g_bbMax[2]) * 0.5f };
				float h = totalHeading * 0.017453292f;
				float v[3] = { c[0] * std::cos(h) - c[1] * std::sin(h), c[0] * std::sin(h) + c[1] * std::cos(h), c[2] };
				const float t = -g_tiltRad;
				float off[3] = { v[0], v[1] * std::cos(t) - v[2] * std::sin(t), v[1] * std::sin(t) + v[2] * std::cos(t) };
				for (int i = 0; i < 3; i++)
					patch.pos[i] = dir[i] * dist - off[i];
			}
			for (int i = 0; i < 3; i++)
				patch.pos[i] += Debug::offset[i];
			ToPresetRect(centreX, centreY, width, height, patch.rect);
			patch.bgColour = Debug::brightBackground ? kDebugBgColour : kBgColour;

			// The light rig is sized for a ped; spread it out and brighten it for bigger builds.
			float rigScale = 1.0f, rigRadiusScale = 1.0f, rigIntensityBoost = 1.0f, rigCover = 0.0f;
			if (g_scaleLighting && !useOrig)
			{
				constexpr float kRigReferenceRadius = 1.5f;
				float sx = g_bbMax[0] - g_bbMin[0], sy = g_bbMax[1] - g_bbMin[1], sz = g_bbMax[2] - g_bbMin[2];
				float radius = 0.5f * std::sqrt(sx * sx + sy * sy + sz * sz);
				rigScale = (std::max)(1.0f, radius / kRigReferenceRadius);
				rigRadiusScale = rigScale;
				// Lights sit around the root, which can be far off the build's centre for maps.
				float c[3] = { (g_bbMin[0] + g_bbMax[0]) * 0.5f, (g_bbMin[1] + g_bbMax[1]) * 0.5f, (g_bbMin[2] + g_bbMax[2]) * 0.5f };
				float centreOffset = std::sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
				if (rigScale > 1.0f)
					rigCover = (radius + centreOffset) * 1.15f;
				rigIntensityBoost = std::clamp(std::sqrt(rigScale), 1.0f, 10.0f);
			}
			patch.lightIntensity = (std::max)(g_lightIntensity * rigIntensityBoost, 0.01f);
			if (!UI3DScene::PatchPresetElement(kPreset, kSlot, patch))
				return false;
			g_patchActive = true;
			UI3DScene::SetCameraFov(useOrig ? 0.0f : g_cameraFov);
			UI3DScene::SetLightRigScale(rigScale * Debug::lightSpread, rigRadiusScale, rigCover);
			if (!g_loggedFraming && g_vehicle && g_boundsUpdates >= kBoundsFrames && g_loglevel >= 3)
			{
				g_loggedFraming = true;
				char buf[256];
				sprintf_s(buf, "VehiclePreview3D: framing size=(%.1f, %.1f, %.1f) cameraDist=%.1f fov=%.1f lightScale=%.1f lightBoost=%.2f",
					g_bbMax[0] - g_bbMin[0], g_bbMax[1] - g_bbMin[1], g_bbMax[2] - g_bbMin[2],
					std::sqrt(patch.pos[0] * patch.pos[0] + patch.pos[1] * patch.pos[1] + patch.pos[2] * patch.pos[2]),
					useOrig ? UI3DScene::GetOriginalCameraFov() : g_cameraFov, rigScale * Debug::lightSpread, rigIntensityBoost);
				addlog(ige::LogType::LOG_DEBUG, buf);
			}

			const float slotRot[3] = { 0.0f, 0.0f, g_heading };
			const float sceneRot[3] = { patch.rot[0], patch.rot[1], totalHeading };
			const float zero[3] = { 0.0f, 0.0f, 0.0f };

			if (g_haveCurrent && g_vehicle && DOES_ENTITY_EXIST(g_vehicle))
			{
				if (Debug::moveEntityToScene)
				{
					// The scene position is a world position: the real entity (and its attachments) must be there.
					SET_ENTITY_COORDS_NO_OFFSET(g_vehicle, patch.pos[0], patch.pos[1], patch.pos[2], false, false, false);
					if (g_tiltRad != 0.0f)
					{
						float hh = totalHeading * 0.017453292f * 0.5f, th = -g_tiltRad * 0.5f;
						float sx = std::sin(th), cx = std::cos(th), sz = std::sin(hh), cz = std::cos(hh);
						SET_ENTITY_QUATERNION(g_vehicle, sx * cz, -sx * sz, cx * sz, cx * cz);
					}
					else
						SET_ENTITY_ROTATION(g_vehicle, sceneRot[0], sceneRot[1], sceneRot[2], 2, true);
					SET_ENTITY_VELOCITY(g_vehicle, 0.0f, 0.0f, 0.0f);
				}

				auto root = reinterpret_cast<uint8_t*>(GTAmemory::GetEntityAddress(g_vehicle));
				if (root && GET_FRAME_COUNT() - g_spawnFrame >= kSettleFrames)
				{
					// The UI draw bypasses the game's visibility checks, so invisible / alpha are applied here.
					int rootAlpha = !g_rootIsDummy && IS_ENTITY_VISIBLE(g_vehicle) ? std::clamp(GET_ENTITY_ALPHA(g_vehicle), 0, 255) : 0;
					const int drawAlpha = std::clamp(Debug::drawAlpha, 0, 255);
					UI3DScene::SetDefaultDrawParams(static_cast<uint8_t>((std::min)(drawAlpha, rootAlpha)), 0);
					UI3DScene::SetExtraDrawAlpha(static_cast<uint8_t>(drawAlpha));
					if (UI3DScene::SetPreviewEntity(root))
					{
						UI3DScene::SetPreviewEntityDrawn(rootAlpha > 0);
						entity = root;
						std::vector<UI3DScene::ExtraEntity> extras;
						extras.reserve(g_attachments.size());
						for (size_t i = 0; i < g_attachments.size(); i++)
						{
							Entity a = g_attachments[i];
							int type = i < g_attachmentTypes.size() ? g_attachmentTypes[i] : 0;
							if (!type || !DOES_ENTITY_EXIST(a) || !IS_ENTITY_ATTACHED(a))
								continue;
							if ((type == 1 && !Debug::drawAttachedPeds) || (type == 2 && !Debug::drawAttachedVehicles) || (type == 3 && !Debug::drawAttachedObjects))
								continue;
							int alpha = IS_ENTITY_VISIBLE(a) ? std::clamp(GET_ENTITY_ALPHA(a), 0, 255) : 0;
							if (alpha == 0)
								continue;
							auto p = reinterpret_cast<void*>(GTAmemory::GetEntityAddress(a));
							if (p)
								extras.push_back({ p, type == 1, static_cast<uint8_t>(alpha) });
						}
						UI3DScene::SetPreviewExtraEntities(extras.data(), static_cast<int>(extras.size()));
					}
				}
			}

			// pause menu is open
			if (!UI3DScene::PushPreset(kPreset) && !UI3DScene::IsPresetActive(kPreset))
				return true;
			if (!entity)
				return true;

			UI3DScene::AssignEntityToSlot(kPreset, kSlot, entity, zero, slotRot, 1.0f);
			return true;
		}
	}

	bool Draw(const GTAmodel::Model& model, float centreX, float centreY, float width, float height)
	{
		if (!g_enabled || !model.IsValid())
			return false;
		Source src;
		src.model = model.hash;
		return DrawSource(src, centreX, centreY, width, height);
	}

	bool DrawFile(const std::string& filePath, float centreX, float centreY, float width, float height)
	{
		Source src;
		src.isFile = true;
		src.path = filePath;
		return DrawSource(src, centreX, centreY, width, height);
	}

	void Tick()
	{
		ProcessPendingDeletes();
		if ((g_vehicle || g_requestedModel || g_patchActive || !g_pending.path.empty()) && GET_FRAME_COUNT() - g_lastDrawFrame > 2)
			Cleanup();
	}

	void Cleanup()
	{
		DeleteVehicle();
		if (g_requestedModel)
		{
			SET_MODEL_AS_NO_LONGER_NEEDED(g_requestedModel);
			g_requestedModel = 0;
		}
		UI3DScene::RestorePresetPatches();
		g_patchActive = false;
		g_pending = Source();
	}
}
