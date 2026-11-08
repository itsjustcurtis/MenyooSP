#include "ImGuiSpooner.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#include <cfloat>
#include "D3D11Hook.h"

#include <d3d11.h>
#include <Windows.h>
#include <atomic>
#include <mutex>
#include <cmath>
#include <cstring>

#include "SpoonerEntity.h"
#include "SpoonerMode.h"
#include "SpoonerSettings.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Natives\natives.h"
#include "Submenus.h"

namespace sub::Spooner::ImGuiSpooner
{
// ═══════════════════════════════════════════════════════════════════
//  Shared State
// ═══════════════════════════════════════════════════════════════════

	// Gizmo writes
	struct PendingWrites
	{
		int entityHandle = 0;
		bool positionDirty = false;  Vector3 positionVal{};
		bool rotationDirty = false;  Vector3 rotationVal{};
		bool scaleDirty = false;     Vector3 scaleVal{1.0f, 1.0f, 1.0f};
	};

	struct RenderState
	{
		Vector3 camCoord{};
		Vector3 camRot{};
		float   camFov = 50.0f;
		SpoonerMode::EditingState editingState;
		bool gizmoOver = false;
		bool gizmoUsing = false;
		bool gridSnapEnabled = false;
		float gridSnapSize = 1.0f;
		float rotationSnapDegrees = 0.0f;
	};

	struct EntityCache
	{
		bool entityValid = false;
		int entityHandle = 0;
		Vector3 position{};
		Vector3 rotation{};
		Vector3 scale{1.0f, 1.0f, 1.0f};
	};

	struct SharedState
	{
		RenderState render;
		EntityCache cache;
		PendingWrites pending;
	};

	static std::mutex g_Mutex;
	static SharedState g_Shared;

	static std::atomic<bool> g_Visible{ false };
	static std::atomic<bool> g_ShuttingDown{ false };
	static std::atomic<bool> g_ImGuiInitialized{ false };

// ═══════════════════════════════════════════════════════════════════
//  Gizmo Math
// ═══════════════════════════════════════════════════════════════════

	static void BuildTransformMatrix(const Vector3& pos, const Vector3& rot, const Vector3& scale, float* matrix)
	{
		constexpr float DEG2RAD = 3.14159265358979323846f / 180.0f;
		float pitch = rot.x * DEG2RAD;
		float roll  = rot.y * DEG2RAD;
		float yaw   = rot.z * DEG2RAD;
		float cp = cosf(pitch), sp = sinf(pitch);
		float cr = cosf(roll),  sr = sinf(roll);
		float cy = cosf(yaw),   sy = sinf(yaw);

		float col0[3] = { cy*cr - sy*sp*sr, sy*cr + cy*sp*sr, -cp*sr };
		float col1[3] = { -sy*cp, cy*cp, sp };
		float col2[3] = { cy*sr + sy*sp*cr, sy*sr - cy*sp*cr, cp*cr };

		matrix[0]  = col0[0] * scale.x;
		matrix[1]  = col0[1] * scale.x;
		matrix[2]  = col0[2] * scale.x;
		matrix[3]  = 0.0f;

		matrix[4]  = col1[0] * scale.y;
		matrix[5]  = col1[1] * scale.y;
		matrix[6]  = col1[2] * scale.y;
		matrix[7]  = 0.0f;

		matrix[8]  = col2[0] * scale.z;
		matrix[9]  = col2[1] * scale.z;
		matrix[10] = col2[2] * scale.z;
		matrix[11] = 0.0f;

		matrix[12] = pos.x;
		matrix[13] = pos.y;
		matrix[14] = pos.z;
		matrix[15] = 1.0f;
	}

	static void DecomposeTransformMatrix(const float* matrix, Vector3& pos, Vector3& rot, Vector3& scale)
	{
		pos.x = matrix[12];
		pos.y = matrix[13];
		pos.z = matrix[14];

		scale.x = sqrtf(matrix[0]*matrix[0] + matrix[1]*matrix[1] + matrix[2]*matrix[2]);
		scale.y = sqrtf(matrix[4]*matrix[4] + matrix[5]*matrix[5] + matrix[6]*matrix[6]);
		scale.z = sqrtf(matrix[8]*matrix[8] + matrix[9]*matrix[9] + matrix[10]*matrix[10]);

		float invSx = (scale.x > 1e-8f) ? 1.0f / scale.x : 0.0f;
		float invSy = (scale.y > 1e-8f) ? 1.0f / scale.y : 0.0f;
		float invSz = (scale.z > 1e-8f) ? 1.0f / scale.z : 0.0f;

		float col0[3] = { matrix[0] * invSx, matrix[1] * invSx, matrix[2] * invSx };
		float col1[3] = { matrix[4] * invSy, matrix[5] * invSy, matrix[6] * invSy };
		float col2[3] = { matrix[8] * invSz, matrix[9] * invSz, matrix[10] * invSz };

		constexpr float RAD2DEG = 180.0f / 3.14159265358979323846f;

		float sp = col1[2];
		if (sp > 1.0f) sp = 1.0f;
		if (sp < -1.0f) sp = -1.0f;
		float pitch = asinf(sp);
		float cp = cosf(pitch);

		float yaw, roll;
		if (fabsf(cp) > 1e-5f)
		{
			yaw  = atan2f(-col1[0], col1[1]);
			roll = atan2f(-col0[2], col2[2]);
		}
		else
		{
			yaw  = atan2f(col0[1], col0[0]);
			roll = 0.0f;
		}

		rot.x = pitch * RAD2DEG;
		rot.y = roll  * RAD2DEG;
		rot.z = yaw   * RAD2DEG;
	}

	static void BuildCameraMatricesFromCache(const Vector3& camCoord, const Vector3& camRot,
		float camFov, float screenW, float screenH,
		float* outView, float* outProj)
	{
		constexpr float DEG2RAD = 3.14159265358979323846f / 180.0f;

		float h = camRot.z * DEG2RAD;
		float p = camRot.x * DEG2RAD;
		float r = camRot.y * DEG2RAD;

		float cosP = cosf(p), sinP = sinf(p);
		float cosH = cosf(h), sinH = sinf(h);
		float cosR = cosf(r), sinR = sinf(r);

		float rightX = cosH * cosR - sinH * sinP * sinR;
		float rightY = sinH * cosR + cosH * sinP * sinR;
		float rightZ = -cosP * sinR;

		float fwdX = -sinH * cosP;
		float fwdY = cosH * cosP;
		float fwdZ = sinP;

		float upX = cosH * sinR + sinH * sinP * cosR;
		float upY = sinH * sinR - cosH * sinP * cosR;
		float upZ = cosP * cosR;

		float eyeX = camCoord.x, eyeY = camCoord.y, eyeZ = camCoord.z;

		outView[0] = rightX;  outView[4] = rightY;  outView[8] = rightZ;
		outView[12] = -(rightX * eyeX + rightY * eyeY + rightZ * eyeZ);
		outView[1] = upX;     outView[5] = upY;     outView[9] = upZ;
		outView[13] = -(upX * eyeX + upY * eyeY + upZ * eyeZ);
		outView[2] = -fwdX;   outView[6] = -fwdY;   outView[10] = -fwdZ;
		outView[14] = (fwdX * eyeX + fwdY * eyeY + fwdZ * eyeZ);
		outView[3] = 0;       outView[7] = 0;       outView[11] = 0;
		outView[15] = 1;

		float aspect = screenH > 0 ? screenW / screenH : 16.0f / 9.0f;
		float fovRad = camFov * DEG2RAD;
		float f = 1.0f / tanf(fovRad * 0.5f);
		float nearZ = 0.1f;
		float farZ = 10000.0f;

		memset(outProj, 0, sizeof(float) * 16);
		outProj[0] = f / aspect;
		outProj[5] = f;
		outProj[10] = (farZ + nearZ) / (nearZ - farZ);
		outProj[11] = -1.0f;
		outProj[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
	}

	static float UnwrapAngle(float cur, float prev)
	{
		float diff = cur - prev;
		if (diff > 180.0f) cur -= 360.0f;
		if (diff < -180.0f) cur += 360.0f;
		return cur;
	}

	static void Mat4Mul(const float a[16], const float b[16], float out[16])
	{
		for (int row = 0; row < 4; row++)
			for (int col = 0; col < 4; col++) {
				out[row + col * 4] =
					a[row + 0 * 4] * b[0 + col * 4] +
					a[row + 1 * 4] * b[1 + col * 4] +
					a[row + 2 * 4] * b[2 + col * 4] +
					a[row + 3 * 4] * b[3 + col * 4];
			}
	}

	static void Mat4Transpose(const float in[16], float out[16])
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				out[i + j * 4] = in[j + i * 4];
			}
		}
	}

// ═══════════════════════════════════════════════════════════════════
//  Gizmo
// ═══════════════════════════════════════════════════════════════════

	static void RunGizmo_NoLock(SharedState& s)
	{
		s.render.gizmoOver = false;
		s.render.gizmoUsing = false;

		if (!s.cache.entityValid || s.render.editingState.mode != SpoonerMode::eEditMode::Gizmo) return;

		ImGuiIO& io = ImGui::GetIO();

		float viewMat[16], projMat[16];
		BuildCameraMatricesFromCache(s.render.camCoord, s.render.camRot, s.render.camFov, io.DisplaySize.x, io.DisplaySize.y, viewMat, projMat);

		ImGuizmo::BeginFrame();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		// the mouse drives the camera while unlocked, so the gizmo is display-only
		ImGuizmo::Enable(s.render.editingState.cameraLocked);

		ImGuizmo::OPERATION op;
		switch (s.render.editingState.transformMode)
		{
			case SpoonerMode::eTransformMode::Rotation: op = ImGuizmo::ROTATE; break;
			case SpoonerMode::eTransformMode::Scale:    op = ImGuizmo::SCALE;  break;
			default:                                          op = ImGuizmo::TRANSLATE; break;
		}
		ImGuizmo::MODE gizmoMode = s.render.editingState.localSpace ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

		if (op == ImGuizmo::TRANSLATE)
		{
			float matrix[16];
			BuildTransformMatrix(s.cache.position, s.cache.rotation, Vector3(1.0f, 1.0f, 1.0f), matrix);

			float deltaMatrix[16]{};
			float snapMatrix[3] = { s.render.gridSnapSize, s.render.gridSnapSize, s.render.gridSnapSize };

			ImGuizmo::Manipulate(viewMat, projMat, op, gizmoMode, matrix, deltaMatrix,
				(s.render.gridSnapEnabled && s.render.gridSnapSize > 0.0f) ? snapMatrix : nullptr);

			if (ImGuizmo::IsUsing())
			{
				Vector3 newPos(
					s.cache.position.x + deltaMatrix[12],
					s.cache.position.y + deltaMatrix[13],
					s.cache.position.z + deltaMatrix[14]
				);

				if (fabsf(newPos.x - s.cache.position.x) > FLT_EPSILON ||
					fabsf(newPos.y - s.cache.position.y) > FLT_EPSILON ||
					fabsf(newPos.z - s.cache.position.z) > FLT_EPSILON)
				{
					s.pending.entityHandle = s.cache.entityHandle;
					s.pending.positionDirty = true;
					s.pending.positionVal = newPos;
				}
			}
		}
		else if (op == ImGuizmo::ROTATE)
		{
			static float s_DragMatrix[16];
			static float s_LastEuler[3];

			if (!ImGuizmo::IsUsing())
			{
				BuildTransformMatrix(s.cache.position, s.cache.rotation, Vector3(1.0f, 1.0f, 1.0f), s_DragMatrix);
				s_LastEuler[0] = s.cache.rotation.x;
				s_LastEuler[1] = s.cache.rotation.y;
				s_LastEuler[2] = s.cache.rotation.z;
			}

			float oldRot[3] = { s_LastEuler[0], s_LastEuler[1], s_LastEuler[2] };
			float snapMatrix[3] = { s.render.rotationSnapDegrees, s.render.rotationSnapDegrees, s.render.rotationSnapDegrees };
			
			ImGuizmo::Manipulate(viewMat, projMat, op, gizmoMode, s_DragMatrix, nullptr,
				(s.render.gridSnapEnabled && s.render.rotationSnapDegrees > 0.0f) ? snapMatrix : nullptr);

			if (ImGuizmo::IsUsing())
			{
				Vector3 newPos, newRot, newScale;
				DecomposeTransformMatrix(s_DragMatrix, newPos, newRot, newScale);

				newRot.x = UnwrapAngle(newRot.x, oldRot[0]);
				newRot.y = UnwrapAngle(newRot.y, oldRot[1]);
				newRot.z = UnwrapAngle(newRot.z, oldRot[2]);

				if (fabsf(newRot.x - oldRot[0]) > FLT_EPSILON ||
					fabsf(newRot.y - oldRot[1]) > FLT_EPSILON ||
					fabsf(newRot.z - oldRot[2]) > FLT_EPSILON)
				{
					s.pending.entityHandle = s.cache.entityHandle;
					s.pending.rotationDirty = true;
					s.pending.rotationVal = newRot;
				}

				s_LastEuler[0] = newRot.x;
				s_LastEuler[1] = newRot.y;
				s_LastEuler[2] = newRot.z;
			}
		}
		else if (op == ImGuizmo::SCALE)
		{
			static float s_DragMatrix[16];

			if (!ImGuizmo::IsUsing())
				BuildTransformMatrix(s.cache.position, s.cache.rotation, s.cache.scale, s_DragMatrix);

			float deltaMatrix[16] = {0};
			ImGuizmo::Manipulate(viewMat, projMat, ImGuizmo::SCALE, ImGuizmo::LOCAL, s_DragMatrix, deltaMatrix, nullptr);

			if (ImGuizmo::IsUsing())
			{
				Vector3 deltaPos, deltaRot, deltaScale;
				DecomposeTransformMatrix(deltaMatrix, deltaPos, deltaRot, deltaScale);

				Vector3 newScale;
				newScale.x = s.cache.scale.x * deltaScale.x;
				newScale.y = s.cache.scale.y * deltaScale.y;
				newScale.z = s.cache.scale.z * deltaScale.z;

				if (fabsf(newScale.x - s.cache.scale.x) > FLT_EPSILON ||
					fabsf(newScale.y - s.cache.scale.y) > FLT_EPSILON ||
					fabsf(newScale.z - s.cache.scale.z) > FLT_EPSILON)
				{
					s.pending.entityHandle = s.cache.entityHandle;
					s.pending.scaleDirty = true;
					s.pending.scaleVal = newScale;
				}
			}
		}

		s.render.gizmoOver  = ImGuizmo::IsOver();
		s.render.gizmoUsing = ImGuizmo::IsUsing();
	}

// ═══════════════════════════════════════════════════════════════════
//  D3D11 Render Callback
// ═══════════════════════════════════════════════════════════════════

	static bool ImGui_Init(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		HWND hWnd = D3D11Hook::GetWindowHandle();
		if (!hWnd) return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
		ImGui::StyleColorsDark();

		if (!ImGui_ImplWin32_Init(hWnd)) { ImGui::DestroyContext(); return false; }
		if (!ImGui_ImplDX11_Init(device, context)) { ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext(); return false; }
		g_ImGuiInitialized = true;
		return true;
	}

	static void OnRender(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
	{
		if (g_ShuttingDown || !g_Visible)
		{
			if (g_ImGuiInitialized)
				ImGui::GetIO().MouseDrawCursor = false;
			D3D11Hook::SetMenuVisible(false);
			return;
		}

		if (!g_ImGuiInitialized && !ImGui_Init(device, context))
			return;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		{
			std::lock_guard<std::mutex> lock(g_Mutex);

			// draw cursor only when the gizmo is usable (camera locked)
			ImGui::GetIO().MouseDrawCursor = g_Shared.render.editingState.UsesGizmoCursor();

			RunGizmo_NoLock(g_Shared);
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

// ═══════════════════════════════════════════════════════════════════
//  Attachment Gizmo Math
// ═══════════════════════════════════════════════════════════════════

	static Vector3 WorldDeltaToBoneRelative(const Vector3& worldDelta, const Vector3& boneRotEuler)
	{
		float yaw = DegreeToRadian(boneRotEuler.z);
		float pitch = DegreeToRadian(boneRotEuler.y);
		float roll = DegreeToRadian(boneRotEuler.x);
		float cZ = cosf(yaw), sZ = sinf(yaw);
		float cX = cosf(roll), sX = sinf(roll);
		float cY = cosf(pitch), sY = sinf(pitch);
		Vector3 xAxis = Vector3(cZ * cY - sZ * sX * sY, sZ * cY + cZ * sX * sY, -cX * sY);
		Vector3 yAxis = Vector3(-sZ * cX, cZ * cX, sX);
		Vector3 zAxis = Vector3(cZ * sY + sZ * sX * cY, sZ * sY - cZ * sX * cY, cX * cY);
		return Vector3(Vector3::Dot(worldDelta, xAxis), Vector3::Dot(worldDelta, yAxis), Vector3::Dot(worldDelta, zAxis));
	}

	static void GetAttachmentOffset(SpoonerEntity& sel, const GTAentity& parentEntity, const Vector3& newWorldPos)
	{
		if (sel.attachmentArgs.boneIndex >= 0)
		{
			Vector3 worldDelta = newWorldPos - sel.handle.GetPosition();
			Vector3 boneRot = ENTITY::GET_ENTITY_BONE_ROTATION(parentEntity.GetHandle(), sel.attachmentArgs.boneIndex);
			sel.attachmentArgs.offset = sel.attachmentArgs.offset + WorldDeltaToBoneRelative(worldDelta, boneRot);
		}
		else
		{
			sel.attachmentArgs.offset = parentEntity.GetOffsetGivenWorldCoords(newWorldPos);
		}
	}

// ═══════════════════════════════════════════════════════════════════
//  Script Thread Ticks
// ═══════════════════════════════════════════════════════════════════

	static void ApplyPending_ScriptThread(PendingWrites pending)
	{
		SpoonerEntity& sel = selectedEntity;
		const bool targetsCurrentEntity = pending.entityHandle == 0 || pending.entityHandle == sel.handle.GetHandle();
		if (!targetsCurrentEntity)
			return;
		if (sel.handle.Exists())
		{
			GTAentity parentEntity(ENTITY::GET_ENTITY_ATTACHED_TO(sel.handle.Handle()));

			// Normal entity (not attached)
			if (!sel.attachmentArgs.isAttached)
			{
				if (pending.positionDirty) sel.handle.SetPosition(SpoonerMode::SnapPos(pending.positionVal));
				if (pending.rotationDirty) sel.handle.SetRotation(SpoonerMode::SnapRot(pending.rotationVal));
			}
			// Attached entity - converting to local offsets
			else if (parentEntity.Exists())
			{
				if (pending.positionDirty) GetAttachmentOffset(sel, parentEntity, pending.positionVal);
				if (pending.rotationDirty)
				{
					float oldWorldM[16], newWorldM[16], oldLocalM[16];
					Vector3 curWorldRot = sel.handle.GetRotation();
					BuildTransformMatrix(Vector3(), curWorldRot, Vector3(1.0f, 1.0f, 1.0f), oldWorldM);
					BuildTransformMatrix(Vector3(), pending.rotationVal, Vector3(1.0f, 1.0f, 1.0f), newWorldM);
					BuildTransformMatrix(Vector3(), sel.attachmentArgs.rotation, Vector3(1.0f, 1.0f, 1.0f), oldLocalM);

					float worldT[16], temp[16], newLocalM[16];
					Mat4Transpose(oldWorldM, worldT);
					Mat4Mul(oldLocalM, worldT, temp);
					Mat4Mul(temp, newWorldM, newLocalM);

					Vector3 pos, newLocalRot, scale;
					DecomposeTransformMatrix(newLocalM, pos, newLocalRot, scale);
					sel.attachmentArgs.rotation = newLocalRot;
				}

				if (pending.positionDirty || pending.rotationDirty)
				{
					sel.handle.AttachTo(parentEntity, sel.attachmentArgs.boneIndex, sel.handle.GetIsCollisionEnabled(), sel.attachmentArgs.offset, sel.attachmentArgs.rotation);
				}
			}

			if (pending.scaleDirty) {
				sel.handle.SetScale(pending.scaleVal);
				// syncing scale so that it doesn't reset every time we grab the gizmo
				Entity entHandle = sel.handle.GetHandle();
				Submenus::EntityScaleState& state = [&]() -> Submenus::EntityScaleState& {
					switch (static_cast<EntityType>(sel.handle.Type()))
					{
					case EntityType::VEHICLE: return Submenus::_vehScale;
					case EntityType::PED:    return Submenus::_pedScale;
					default:                 return Submenus::_objScale;
					}
				}();
				state.handle = entHandle;
				state.scale = pending.scaleVal;
			}
		}
	}

	// ── Snapshot ──────────────────────────────────────────────────

	static void RefreshSnapshot_ScriptThread(SharedState& s)
	{
		int renderingCam = CAM::GET_RENDERING_CAM();
		if (renderingCam != 0 && CAM::DOES_CAM_EXIST(renderingCam))
		{
			s.render.camCoord = CAM::GET_CAM_COORD(renderingCam);
			s.render.camRot   = CAM::GET_CAM_ROT(renderingCam, 2);
			s.render.camFov   = CAM::GET_CAM_FOV(renderingCam);
		}
		else
		{
			s.render.camCoord = CAM::GET_GAMEPLAY_CAM_COORD();
			s.render.camRot   = CAM::GET_GAMEPLAY_CAM_ROT(2);
			s.render.camFov   = CAM::GET_GAMEPLAY_CAM_FOV();
		}

		s.render.editingState = SpoonerMode::editingState;
		s.render.gridSnapEnabled = Settings::bGridSnapEnabled;
		s.render.gridSnapSize = Settings::gridSnapSize;
		s.render.rotationSnapDegrees = Settings::rotationSnapDegrees;

		SpoonerEntity& sel = selectedEntity;
		s.cache.entityHandle = sel.handle.Handle();
		s.cache.entityValid = (s.cache.entityHandle != 0) && sel.handle.Exists();
		if (!s.cache.entityValid)
		{
			s.cache.position = Vector3{};
			s.cache.rotation = Vector3{};
			s.cache.scale = Vector3{1.0f, 1.0f, 1.0f};
			return;
		}

		s.cache.position = sel.handle.GetPosition();
		s.cache.rotation = sel.handle.GetRotation();
		s.cache.scale = sel.handle.GetScale();
	}

// ═══════════════════════════════════════════════════════════════════
//  Main Tick
// ═══════════════════════════════════════════════════════════════════

	void Tick()
	{
		if (!g_Visible) return;

		PendingWrites pending;
		{
			std::lock_guard<std::mutex> lock(g_Mutex);
			pending = g_Shared.pending;
			g_Shared.pending = PendingWrites{};
		}

		ApplyPending_ScriptThread(pending);

		SharedState snapshot;
		RefreshSnapshot_ScriptThread(snapshot);

		bool capturedGizmoOver = false, capturedGizmoUsing = false;
		SpoonerMode::eEditMode capturedEditMode = SpoonerMode::eEditMode::Disabled;
		{
			std::lock_guard<std::mutex> lock(g_Mutex);

			// gizmo interaction flags are owned by the render thread
			snapshot.render.gizmoOver = g_Shared.render.gizmoOver;
			snapshot.render.gizmoUsing = g_Shared.render.gizmoUsing;
			g_Shared.render = snapshot.render;
			g_Shared.cache = snapshot.cache;

			capturedGizmoOver = g_Shared.render.gizmoOver;
			capturedGizmoUsing = g_Shared.render.gizmoUsing;
			capturedEditMode = g_Shared.render.editingState.mode;
		}

		// Disable player controls when using the gizmo
		if (capturedEditMode == SpoonerMode::eEditMode::Gizmo || capturedGizmoOver || capturedGizmoUsing)
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
	}

// ═══════════════════════════════════════════════════════════════════
//  Public API
// ═══════════════════════════════════════════════════════════════════

	bool Initialize()
	{
		if (D3D11Hook::IsInitialized())
			return true;

		g_ShuttingDown = false;
		return D3D11Hook::Initialize(OnRender);
	}

	void Shutdown()
	{
		g_ShuttingDown = true;
		g_Visible = false;
		D3D11Hook::SetMenuVisible(false);

		for (int i = 0; D3D11Hook::IsRenderingFrame() && i < 100; ++i)
			Sleep(10);

		if (g_ImGuiInitialized)
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			g_ImGuiInitialized = false;
		}

		D3D11Hook::Shutdown();
	}

	void SetVisible(bool visible)
	{
		g_Visible = visible;
		if (visible)
			D3D11Hook::SetMenuVisible(true);
		else
		{
			std::lock_guard<std::mutex> lock(g_Mutex);
			g_Shared.pending = PendingWrites{};
		}
	}

	bool IsVisible()
	{
		return g_Visible;
	}
}
