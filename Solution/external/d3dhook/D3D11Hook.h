#pragma once

#include <d3d11.h>
#include <functional>
#include <atomic>

namespace D3D11Hook
{
    using RenderCallback = std::function<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*)>;
    bool Initialize(RenderCallback renderCallback);
    void Shutdown();
    bool IsInitialized();
    bool IsRenderingFrame();
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetContext();
    IDXGISwapChain* GetSwapChain();
    HWND GetWindowHandle();
    void SetMenuVisible(bool visible);
    bool IsMenuVisible();
}
