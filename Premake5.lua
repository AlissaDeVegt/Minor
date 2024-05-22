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
		"C:/dev/Minor/Card/vendor/glm",
		"C:/VulkanSDK/1.3.280.0/Include",
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/include",
		"Card/vendor/spdlog/include"
	}									  
										  
	links{								  
		"vulkan-1.lib",
		"glfw3_mt.lib"
	}									  
										  
	libdirs{							  
		"C:/VulkanSDK/1.3.280.0/Lib",
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/lib-vc2022"
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
		"C:/dev/Minor/Card/vendor/glm",
		"C:/VulkanSDK/1.3.280.0/Include",
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/include",
		"Card/vendor/spdlog/include",
		"Card/src/"
	}

	links{
		"Card",
		"vulkan-1.lib",
		"glfw3_mt.lib"

	}

	libdirs{
		"C:/VulkanSDK/1.3.280.0/Lib",
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/lib-vc2022"

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