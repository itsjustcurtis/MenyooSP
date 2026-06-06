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

#include "SpoonerEntity.h"
#include "SpoonerMode.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Natives\natives.h"

namespace sub::Spooner::ImGuiSpooner
{
	struct PendingWrites
	{
		bool positionDirty = false;  Vector3 positionVal{};
		bool rotationDirty = false;  Vector3 rotationVal{};
	};

	struct SharedState
	{
		bool entityValid = false;
		Vector3 position{};
		Vector3 rotation{};

		// Active rendering camera
		Vector3 camCoord{};
		Vector3 camRot{};
		float   camFov = 50.0f;

		bool rotationMode = false;            // SpoonerMode::bEntityEditRotationMode
		bool gizmoEditModeActive = false;     // entityEditMode == eEntityEditMode::Gizmo
		bool cameraLocked = false;            // SpoonerMode::bGizmoCameraLocked
		bool localSpace = false;              // SpoonerMode::bGizmoLocalSpace

		// Render-thread interaction state.
		bool gizmoOver = false;
		bool gizmoUsing = false;

		PendingWrites pending;
	};

	static std::mutex g_Mutex;
	static SharedState g_Shared;

	static std::atomic<bool> g_Visible{ false };
	static std::atomic<bool> g_ShuttingDown{ false };
	static bool g_ImGuiInitialized = false;

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


	static void RunGizmo_NoLock(SharedState& s)
	{
		s.gizmoOver = false;
		s.gizmoUsing = false;

		if (!s.entityValid || !s.gizmoEditModeActive) return;

		ImGuiIO& io = ImGui::GetIO();

		float viewMat[16], projMat[16];
		BuildCameraMatricesFromCache(s.camCoord, s.camRot, s.camFov, io.DisplaySize.x, io.DisplaySize.y, viewMat, projMat);

		ImGuizmo::BeginFrame();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ImGuizmo::OPERATION op = s.rotationMode ? ImGuizmo::ROTATE : ImGuizmo::TRANSLATE;
		ImGuizmo::MODE gizmoMode = s.localSpace ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

		if (op == ImGuizmo::TRANSLATE)
		{
			float matrix[16];
			BuildTransformMatrix(s.position, s.rotation, Vector3(1.0f, 1.0f, 1.0f), matrix);

			float deltaMatrix[16]{};
			ImGuizmo::Manipulate(viewMat, projMat, op, gizmoMode, matrix, deltaMatrix, nullptr);

			if (ImGuizmo::IsUsing())
			{
				Vector3 newPos(
					s.position.x + deltaMatrix[12],
					s.position.y + deltaMatrix[13],
					s.position.z + deltaMatrix[14]
				);

				if (fabsf(newPos.x - s.position.x) > FLT_EPSILON ||
					fabsf(newPos.y - s.position.y) > FLT_EPSILON ||
					fabsf(newPos.z - s.position.z) > FLT_EPSILON)
				{
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
				BuildTransformMatrix(s.position, s.rotation, Vector3(1.0f, 1.0f, 1.0f), s_DragMatrix);
				s_LastEuler[0] = s.rotation.x;
				s_LastEuler[1] = s.rotation.y;
				s_LastEuler[2] = s.rotation.z;
			}

			float oldRot[3] = { s_LastEuler[0], s_LastEuler[1], s_LastEuler[2] };

			ImGuizmo::Manipulate(viewMat, projMat, op, gizmoMode, s_DragMatrix, nullptr, nullptr);

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
					s.pending.rotationDirty = true;
					s.pending.rotationVal = newRot;
				}

				s_LastEuler[0] = newRot.x;
				s_LastEuler[1] = newRot.y;
				s_LastEuler[2] = newRot.z;
			}
		}

		s.gizmoOver  = ImGuizmo::IsOver();
		s.gizmoUsing = ImGuizmo::IsUsing();
	}

	static void OnRender(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
	{
		if (g_ShuttingDown || !g_Visible)
		{
			D3D11Hook::SetMenuVisible(false);
			return;
		}

		if (!g_ImGuiInitialized)
		{
			HWND hWnd = D3D11Hook::GetWindowHandle();
			if (!hWnd) return;

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = nullptr;
			io.MouseDrawCursor = false;
			ImGui::StyleColorsDark();

			if (!ImGui_ImplWin32_Init(hWnd))
			{
				ImGui::DestroyContext();
				return;
			}
			if (!ImGui_ImplDX11_Init(device, context))
			{
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
				return;
			}
			g_ImGuiInitialized = true;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		{
			std::lock_guard<std::mutex> lock(g_Mutex);

			ImGui::GetIO().MouseDrawCursor = g_Shared.gizmoEditModeActive && g_Shared.cameraLocked;

			RunGizmo_NoLock(g_Shared);
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

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

	static void DrainPending_ScriptThread(SharedState& s)
	{
		SpoonerEntity& sel = selectedEntity;
		if (sel.handle.Exists())
		{
			GTAentity parentEntity(ENTITY::GET_ENTITY_ATTACHED_TO(sel.handle.Handle()));

			if (!sel.attachmentArgs.isAttached)
			{
				if (s.pending.positionDirty) sel.handle.SetPosition(s.pending.positionVal);
				if (s.pending.rotationDirty) sel.handle.SetRotation(s.pending.rotationVal);
			}
			else if (parentEntity.Exists())
			{
				if (s.pending.positionDirty) GetAttachmentOffset(sel, parentEntity, s.pending.positionVal);
				if (s.pending.rotationDirty)
				{
					float oldWorldM[16], newWorldM[16], oldLocalM[16];
					Vector3 curWorldRot = sel.handle.Rotation_get();
					BuildTransformMatrix(Vector3(), curWorldRot, Vector3(1.0f, 1.0f, 1.0f), oldWorldM);
					BuildTransformMatrix(Vector3(), s.pending.rotationVal, Vector3(1.0f, 1.0f, 1.0f), newWorldM);
					BuildTransformMatrix(Vector3(), sel.attachmentArgs.rotation, Vector3(1.0f, 1.0f, 1.0f), oldLocalM);

					float worldT[16], temp[16], newLocalM[16];
					Mat4Transpose(oldWorldM, worldT);
					Mat4Mul(oldLocalM, worldT, temp);
					Mat4Mul(temp, newWorldM, newLocalM);

					Vector3 pos, newLocalRot, scale;
					DecomposeTransformMatrix(newLocalM, pos, newLocalRot, scale);
					sel.attachmentArgs.rotation = newLocalRot;
				}

				if (s.pending.positionDirty || s.pending.rotationDirty)
				{
					sel.handle.AttachTo(parentEntity, sel.attachmentArgs.boneIndex, sel.handle.GetIsCollisionEnabled(), sel.attachmentArgs.offset, sel.attachmentArgs.rotation);
				}
			}
		}
		s.pending = PendingWrites{};
	}

	static void RefreshSnapshot_ScriptThread(SharedState& s)
	{
		int renderingCam = CAM::GET_RENDERING_CAM();
		if (renderingCam != 0 && CAM::DOES_CAM_EXIST(renderingCam))
		{
			s.camCoord = CAM::GET_CAM_COORD(renderingCam);
			s.camRot   = CAM::GET_CAM_ROT(renderingCam, 2);
			s.camFov   = CAM::GET_CAM_FOV(renderingCam);
		}
		else
		{
			s.camCoord = CAM::GET_GAMEPLAY_CAM_COORD();
			s.camRot   = CAM::GET_GAMEPLAY_CAM_ROT(2);
			s.camFov   = CAM::GET_GAMEPLAY_CAM_FOV();
		}

		s.rotationMode = SpoonerMode::bEntityEditRotationMode;

		const bool inGizmoNow = SpoonerMode::entityEditMode == SpoonerMode::eEntityEditMode::Gizmo;
		static bool s_wasInGizmo = false;
		if (inGizmoNow && !s_wasInGizmo)
			SpoonerMode::bGizmoCameraLocked = true;
		s_wasInGizmo = inGizmoNow;

		s.gizmoEditModeActive = inGizmoNow;
		s.cameraLocked = SpoonerMode::bGizmoCameraLocked;
		s.localSpace = SpoonerMode::bGizmoLocalSpace;

		SpoonerEntity& sel = selectedEntity;
		s.entityValid = (sel.handle.Handle() != 0) && sel.handle.Exists();
		if (!s.entityValid)
		{
			s.position = Vector3{};
			s.rotation = Vector3{};
			return;
		}

		s.position = sel.handle.GetPosition();
		s.rotation = sel.handle.Rotation_get();
	}

	void Tick()
	{
		bool suppressGameInput = false;
		{
			std::lock_guard<std::mutex> lock(g_Mutex);
			DrainPending_ScriptThread(g_Shared);
			RefreshSnapshot_ScriptThread(g_Shared);

			suppressGameInput = g_Visible && (
				(g_Shared.gizmoEditModeActive && g_Shared.cameraLocked) ||
				g_Shared.gizmoOver ||
				g_Shared.gizmoUsing);
		}

		if (suppressGameInput)
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
	}

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
		D3D11Hook::SetMenuVisible(visible);
	}

	bool IsVisible()
	{
		return g_Visible;
	}
}
