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
    }

    includedirs {
        "Solution/source",
        "Solution/external",
    }

    links { "ScriptHookV" }

    libdirs { "Solution/external/ScriptHookV/SDK/lib" }

    defines {
        "WIN32",
        "_WINDOWS",
        "_USRDLL",
        "NativeTrainer_EXPORTS",
    }

    buildoptions { "/bigobj" }
    linkoptions { "/SUBSYSTEM:WINDOWS" }

    staticruntime "On"

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
