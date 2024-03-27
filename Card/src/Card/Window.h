#pragma once
#include "Base.h"
#include "Logger.h"

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN64
#include <GLFW/glfw3native.h>

#include <string>
#include <iostream>
#include <optional>
#include <set>


namespace Card {
	struct QueueFamilyIndices {
		//because its possible that there are queuefamilies that cant do both.
		std::optional<uint32_t> graphicsFamily; //do drawing command
		std::optional<uint32_t> presentFamily; //present images to surface

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class CARD_API Window
	{
	public:
		Window(int width, int height, std::string windowname);
		~Window();

		void initWindow();
		void update();
		//TODO add close window

		//---------------vulkan---------------
		void initVulkan();
		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface();
		void createSwapChain();
		void createImageViews();
		
		bool checkValidationLayerSupport();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		bool isdeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		const int width;
		const int height;

		std::string windowname;
		GLFWwindow* window;

		//----------------vulkan---------------
		VkInstance vkinstance;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkSwapchainKHR swapChain;
		VkSurfaceKHR surface;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		//list of validationlayers
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		//list of required device extensions
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		//----------------macro----------------
		#ifdef NDEBUG
				const bool enableValidationLayers = false;
		#else
				const bool enableValidationLayers = true;
		#endif

	};


};
