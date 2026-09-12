#include "D3D11Hook.h"
#include "Hook.h"
#include "gmAddress.h"
#include <dxgi.h>
#include <wrl/client.h>
#include <mutex>
#include <thread>
#include <chrono>

using Microsoft::WRL::ComPtr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace D3D11Hook
{
    namespace Internal
    {
        static gmAddress s_ResizeBuffersAddr;
        static gmAddress s_EndFrameAddr;
        static gmAddress s_WndProcAddr;

        static std::atomic<bool> g_Initialized = false;
        static std::atomic<bool> g_ShutdownRequested = false;
        static std::atomic<bool> g_RenderingFrame = false;
        static std::atomic<bool> g_MenuVisible = false;

        static RenderCallback g_RenderCallback = nullptr;

        static ComPtr<ID3D11Device> g_Device = nullptr;
        static ComPtr<ID3D11DeviceContext> g_Context = nullptr;
        static ComPtr<IDXGISwapChain> g_SwapChain = nullptr;

        static HWND g_hWnd = nullptr;

        static void(*g_OriginalEndFrame)() = nullptr;
        static LRESULT(*g_OriginalWndProc)(HWND, UINT, WPARAM, LPARAM) = nullptr;

        static std::mutex g_InitMutex;

        LRESULT HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            if (g_MenuVisible)
            {
                if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
                    return true;
            }

            return g_OriginalWndProc(hWnd, uMsg, wParam, lParam);
        }

        void HookedEndFrame()
        {
            if (g_MenuVisible && g_Device && g_Context && g_SwapChain && g_RenderCallback && !g_ShutdownRequested)
            {
                g_RenderingFrame = true;
                g_RenderCallback(g_Device.Get(), g_Context.Get(), g_SwapChain.Get());
                g_RenderingFrame = false;
            }

            g_OriginalEndFrame();
        }

        void WaitForGameReady()
        {
            gmAddress renderFlagAddr = gmAddress::Scan("88 1D ? ? ? ? 89 05 ? ? ? ? EB ? 8A 1D");
            if (renderFlagAddr)
            {
                bool* isGameRendering = renderFlagAddr.GetRef(2).To<bool*>();

                if (*isGameRendering)
                {
                    return;
                }

                while (!(*isGameRendering) && !g_ShutdownRequested)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        }

        void SearchForDevice()
        {
            s_ResizeBuffersAddr = gmAddress::Scan("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC 90 00 00 00 48 8B F1 48 8D 0D");
            s_EndFrameAddr = gmAddress::Scan("75 0C 44 38 35").GetAt(-0xC8);
            g_hWnd = FindWindowW(L"grcWindow", nullptr);

            if (s_ResizeBuffersAddr)
            {
                IDXGISwapChain** ppSwapChain = s_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();

                if (ppSwapChain && *ppSwapChain)
                {
                    g_SwapChain = ComPtr<IDXGISwapChain>(*ppSwapChain);

                    if (SUCCEEDED(g_SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(g_Device.GetAddressOf()))))
                    {
                        g_Device->GetImmediateContext(g_Context.GetAddressOf());
                    }
                }
            }

            s_WndProcAddr = gmAddress::Scan("48 8D 05 ? ? ? ? 33 C9 89 75 20").GetRef(3);
        }

        bool CreateHooks()
        {
            Hook::Init();

            if (s_EndFrameAddr)
            {
                if (!Hook::Create(s_EndFrameAddr, HookedEndFrame, &g_OriginalEndFrame, "EndFrame"))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            if (s_WndProcAddr)
            {
                Hook::Create(s_WndProcAddr, HookedWndProc, &g_OriginalWndProc, "WndProc");
            }

            return true;
        }
    }

    static std::atomic<bool> g_InitInProgress = false;

    bool Initialize(RenderCallback renderCallback)
    {
        if (Internal::g_Initialized)
        {
            return true;
        }

        if (g_InitInProgress)
        {
            return false;
        }

        std::unique_lock<std::mutex> lock(Internal::g_InitMutex, std::try_to_lock);
        if (!lock.owns_lock())
        {
            return false;
        }

        if (Internal::g_Initialized)
        {
            return true;
        }

        if (!renderCallback)
        {
            return false;
        }

        g_InitInProgress = true;
        Internal::g_RenderCallback = renderCallback;
        Internal::g_ShutdownRequested = false;

        lock.unlock();
        Internal::WaitForGameReady();

        lock.lock();

        if (Internal::g_ShutdownRequested)
        {
            g_InitInProgress = false;
            return false;
        }

        Internal::SearchForDevice();

        if (!Internal::CreateHooks())
        {
            Hook::Shutdown();
            g_InitInProgress = false;
            return false;
        }

        Internal::g_Initialized = true;
        g_InitInProgress = false;
        return true;
    }

    void Shutdown()
    {
        if (!Internal::g_Initialized)
        {
            return;
        }

        Internal::g_ShutdownRequested = true;
        Internal::g_MenuVisible = false;

        int waitCount = 0;
        while (Internal::g_RenderingFrame && waitCount < 100)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            waitCount++;
        }

        Hook::Remove(Internal::s_WndProcAddr, "WndProc");
        Hook::Remove(Internal::s_EndFrameAddr, "EndFrame");
        Hook::Shutdown();

        Internal::g_Context.Reset();
        Internal::g_Device.Reset();
        Internal::g_SwapChain.Reset();

        Internal::g_RenderCallback = nullptr;
        Internal::g_OriginalEndFrame = nullptr;
        Internal::g_OriginalWndProc = nullptr;
        Internal::s_EndFrameAddr = 0;
        Internal::s_ResizeBuffersAddr = 0;
        Internal::s_WndProcAddr = 0;
        Internal::g_hWnd = nullptr;
        Internal::g_Initialized = false;
    }

    bool IsInitialized()
    {
        return Internal::g_Initialized;
    }

    bool IsRenderingFrame()
    {
        return Internal::g_RenderingFrame;
    }

    ID3D11Device* GetDevice()
    {
        return Internal::g_Device.Get();
    }

    ID3D11DeviceContext* GetContext()
    {
        return Internal::g_Context.Get();
    }

    IDXGISwapChain* GetSwapChain()
    {
        return Internal::g_SwapChain.Get();
    }

    HWND GetWindowHandle()
    {
        return Internal::g_hWnd;
    }

    void SetMenuVisible(bool visible)
    {
        Internal::g_MenuVisible = visible;
    }

    bool IsMenuVisible()
    {
        return Internal::g_MenuVisible;
    }
}
