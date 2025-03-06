workspace "Minor" --change name to the file directory
	architecture "x64"
	startproject "Strategie" --change name to game project
	
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
		"Card/vendor/glm",
		"Card/vendor/stb",
		"C:/VulkanSDK/1.3.296.0/Include",-- change location include based own location
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/include",
		"Card/vendor/spdlog/include"
	}									  
										  
	links{								  
		"vulkan-1.lib",
		"glfw3_mt.lib"
	}									  
										  
	libdirs{							  
		"C:/VulkanSDK/1.3.296.0/Lib",-- change location include based own location
		"Card/vendor/GLFW/glfw-3.4.bin.WIN64/lib-vc2022"
	}
	

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines{
			"CARD_PLATFORM_WINDOWS",
			"CARD_BUILD_DLL"
		}

		

		postbuildcommands{

		("{COPY} %{cfg.buildtarget.relpath} ../bin/".. outputdir .. "/Strategie" ) -- change /kaartspel to the game directory name

		}
		
	filter "configurations:Debug"
		defines "CARD_DEBUG"
		staticruntime "off"
		runtime "Debug"
		symbols "On"	
		
	filter "configurations:Release"
		defines "CARD_RELEASE"
		staticruntime "off"
		runtime "Release"
		symbols "On"
		
	filter "configurations:Dist"
		defines "CARD_DIST"
		staticruntime "off"
		runtime "Release"
		symbols "On"
	



project "Strategie"  -- example game change to the name of startproject
	location "Strategie" -- example game change to the name of startproject
	kind "ConsoleApp"
	language "c++"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{ 
		"Card/vendor/glm",
		"Card/vendor/stb",
		"C:/VulkanSDK/1.3.296.0/Include", -- change location include based own location
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
		"C:/VulkanSDK/1.3.296.0/Lib", -- change location include based own location
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
		staticruntime "off"
		runtime "Debug"
		symbols "On"	
		
	filter "configurations:Release"
		defines "CARD_RELEASE"
		staticruntime "off"
		runtime "Release"
		symbols "On"
		
	filter "configurations:Dist"
		defines "CARD_DIST"
		staticruntime "off"
		runtime "Release"
		symbols "On"