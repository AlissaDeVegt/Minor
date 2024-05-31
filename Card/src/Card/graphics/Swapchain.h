#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "Device.h"
#include "Window.h"
#include "Renderer.h"

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

#define GLFW_EXPOSE_NATIVE_WIN64
#include <GLFW/glfw3native.h>

#include <string>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>
#include <vector>

namespace Card {
	class Device;
	class Renderer;

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class CARD_API Swapchain
	{
	public:
		Swapchain(Device* device);
		~Swapchain();

		VkRenderPass getRenderPass();
		VkSwapchainKHR getSwapChain();
		VkExtent2D getSwapChainExtent();
		std::vector<VkFramebuffer> getSwapChainFramebuffers();
		std::vector<VkSemaphore> getImageAvailableSemaphores();
		std::vector<VkSemaphore> getRenderFinishedSemaphores();
		std::vector<VkFence> getInFlightFences();
		int getMaxFramesInFlight();

		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createDepthResources(Renderer* renderer);
		void createSyncObjects();

		void cleanupSwapChain();
		void cleanupRenderPass();
		void recreateSwapChain(Renderer* renderer);

		static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice device);
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;
		Device* device;

		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkRenderPass renderPass;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

	};

}