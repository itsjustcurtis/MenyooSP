#include "ImGuiSpooner.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGizmo3D.h"
#include "D3D11Hook.h"

#include <d3d11.h>
#include <Windows.h>
#include <atomic>
#include <mutex>
#include <cmath>

#include "..\Natives\natives.h"
#include "..\Submenus\Spooner\SpoonerEntity.h"
#include "..\Submenus\Spooner\SpoonerMode.h"
#include "..\Scripting\GTAentity.h"
#include "..\Util\GTAmath.h"

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

	static void RunGizmo_NoLock(SharedState& s)
	{
		// Reset interaction flags every frame
		s.gizmoOver = false;
		s.gizmoUsing = false;

		if (!s.entityValid || !s.gizmoEditModeActive) return;

		ImGuiIO& io = ImGui::GetIO();

		float viewMat[16], projMat[16];
		BuildCameraMatricesFromCache(s.camCoord, s.camRot, s.camFov, io.DisplaySize.x, io.DisplaySize.y, viewMat, projMat);

		ImGizmo3D::BeginFrame();
		ImGizmo3D::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGizmo3D::SetMouseOverGui(false);

		const ImGizmo3D::Operation op = s.rotationMode ? ImGizmo3D::ROTATE : ImGizmo3D::TRANSLATE;

		float position[3] = { s.position.x, s.position.y, s.position.z };
		float rotation[3] = { s.rotation.x, s.rotation.y, s.rotation.z };
		float scale[3] = { 1.0f, 1.0f, 1.0f };

		const ImGizmo3D::Mode gizmoMode = s.localSpace ? ImGizmo3D::Local : ImGizmo3D::World;

		if (ImGizmo3D::Manipulate(viewMat, projMat, op, position, rotation, scale, nullptr, gizmoMode))
		{
			if (op == ImGizmo3D::TRANSLATE)
			{
				s.pending.positionDirty = true;
				s.pending.positionVal = Vector3(position[0], position[1], position[2]);
			}
			else
			{
				s.pending.rotationDirty = true;
				s.pending.rotationVal = Vector3(rotation[0], rotation[1], rotation[2]);
			}
		}

		s.gizmoOver  = ImGizmo3D::IsOver();
		s.gizmoUsing = ImGizmo3D::IsUsing();
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

	static void DrainPending_ScriptThread(SharedState& s)
	{
		SpoonerEntity& sel = selectedEntity;
		if (sel.handle.Exists())
		{
			if (s.pending.positionDirty)
			{
				sel.handle.SetPosition(s.pending.positionVal);
			}
			if (s.pending.rotationDirty)
			{
				sel.handle.SetRotation(s.pending.rotationVal);
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
