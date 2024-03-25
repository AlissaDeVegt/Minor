workspace "Minor"
	architecture "x64"
	startproject "Kaartspel"
	
	configurations{
		"Debug",
		"Release",
		"Dist" 
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Card"

	location "Card"
	kind "SharedLib"
	language "c++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"Card/vendor/spdlog/include",
		"Card/vendor/GLFW/glfw-3.4/include/GLFW"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"CARD_PLATFORM_WINDOWS",
			"CARD_BUILD_DLL"
		}

		postbuildcommands{

		("{COPY} %{cfg.buildtarget.relpath} ../bin/".. outputdir .. "/Kaartspel" )

		}
	filter "configurations:Debug"
		defines "CARD_DEBUG"
		symbols "On"	
		
		filter "configurations:Release"
		defines "CARD_RELEASE"
		symbols "On"
		
		filter "configurations:Dist"
		defines "CARD_DIST"
		symbols "On"
	
project "Kaartspel"
	location "Kaartspel"
	kind "ConsoleApp"
	language "c++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"Card/vendor/spdlog/include",
		"Card/vendor/GLFW/glfw-3.4/include/GLFW",
		"Card/src/"
	}

	links{
		"Card"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines{
			"CARD_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "CARD_DEBUG"
		symbols "On"	
		
		filter "configurations:Release"
		defines "CARD_RELEASE"
		symbols "On"
		
		filter "configurations:Dist"
		defines "CARD_DIST"
		symbols "On"