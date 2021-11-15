#!lua

workspace "TauUtils"
    configurations {
        "Debug",
        "Release",
    }
    platforms { "Win64" }

    cdialect "C11"
    cppdialect "C++17"
    floatingpoint "Fast"
    floatingpointexceptions "off"
    rtti "off"
    clr "off"
    functionlevellinking "on"
    intrinsics "on"
    largeaddressaware "on"
    vectorextensions "AVX2"
    stringpooling "on"
    staticruntime "on"
    flags { "LinkTimeOptimization" }

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"
        _arch = "x64"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "_HAS_EXCEPTIONS=0"
        }
        characterset "MBCS"

    filter "configurations:Debug"
        optimize "Debug"
        inlining "Explicit"
        runtime "Debug"

    filter "configurations:Release"
        optimize "Speed"
        inlining "Auto"
        runtime "Release"

    filter {}

    targetdir "%{wks.location}/bin/%{cfg.shortname}-%{_arch}"

    project "TauUtils"
        kind "StaticLib"
        language "C++"
        toolset "clang"
        location "TauUtils"

        files { 
            "%{prj.location}/**.h", 
            "%{prj.location}/**.hpp", 
            "%{prj.location}/**.inl", 
            "%{prj.location}/src/**.c", 
            "%{prj.location}/src/**.cpp" 
        }

        includedirs {
            "%{prj.location}/include"
        }

    project "Testing"
        kind "ConsoleApp"
        language "C++"
        toolset "clang"
        location "Testing"

        files { 
            "%{prj.location}/**.h", 
            "%{prj.location}/**.hpp", 
            "%{prj.location}/src/**.c", 
            "%{prj.location}/src/**.cpp" 
        }

        includedirs {
            "%{prj.location}/include",
            "%{wks.location}/TauUtils/include"
        }

        libdirs {
            "%{cfg.outdir}"
        }

        links {
            "TauUtils.lib"
        }

