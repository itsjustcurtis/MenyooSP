workspace "Menyoo"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    location "Solution"
    startproject "Menyoo"

project "Menyoo"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "Unicode"
    targetextension ".asi"

    targetdir "Solution/source/%{cfg.buildcfg == 'Release' and '_Build/bin/Release' or 'bin/Debug'}"
    objdir    "Solution/source/%{cfg.buildcfg == 'Release' and '_Build/tmp/Release' or 'tmp/Debug'}"

    files {
        "Solution/source/**.h",
        "Solution/source/**.cpp",
        "Solution/source/Menyoo.rc",
        "Solution/external/pugixml/src/pugixml.cpp",
        "Solution/external/pugixml/src/pugiconfig.hpp",
        "Solution/external/pugixml/src/pugixml.hpp",
        "Solution/external/imgui/imgui.cpp",
        "Solution/external/imgui/imgui_draw.cpp",
        "Solution/external/imgui/imgui_impl_dx11.cpp",
        "Solution/external/imgui/imgui_impl_win32.cpp",
        "Solution/external/imgui/ImGizmo3D.cpp",
        "Solution/external/imgui/**.h",
        "Solution/external/d3dhook/D3D11Hook.cpp",
        "Solution/external/d3dhook/**.h",
        "Solution/external/MinHook/include/MinHook.h",
        "Solution/external/MinHook/src/**.c",
        "Solution/external/MinHook/src/**.h",
    }

    includedirs {
        "Solution/source",
        "Solution/external",
        "Solution/external/imgui",
        "Solution/external/d3dhook",
        "Solution/external/MinHook/include",
    }

    links { "ScriptHookV", "d3d11", "d3dcompiler", "dxgi", "Psapi" }

    libdirs {
        "Solution/external/ScriptHookV/SDK/lib",
    }

    defines {
        "WIN32",
        "_WINDOWS",
        "_USRDLL",
        "NativeTrainer_EXPORTS",
    }

    buildoptions { "/bigobj" }
    linkoptions { "/SUBSYSTEM:WINDOWS" }

    staticruntime "Off"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        defines { "_DEBUG" }
        linkoptions { "/INCREMENTAL" }

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Speed"
        defines { "NDEBUG" }
        multiprocessorcompile "On"
        floatingpoint "Fast"
        buildoptions { "/GL", "/Gy", "/Oi" }
        linkoptions {
            "/LTCG",
            "/OPT:REF",
            "/OPT:ICF",
            "/INCREMENTAL:NO",
            "/MAP",
        }
