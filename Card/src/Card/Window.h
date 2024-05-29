#pragma once
#include "Base.h"
#include "Logger.h"
#include "GraphicsPipeline.h"
#include "Util/ModelLoader.h"
#include "Util/Vertex.h"

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


namespace Card {
	struct QueueFamilyIndices {
		//because its possible that there are queuefamilies that cant do both.
		std::optional<uint32_t> graphicsFamily; //do drawing command
		std::optional<uint32_t> presentFamily; //present images to surface

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
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
		void drawFrame();
		GLFWwindow* getGlfwWindow();

		//---------------vulkan---------------
		void initVulkan();
		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();
		void waitDevice();
		void cleanupSwapChain();
		void recreateSwapChain();
		void createTextureImage();
		void createVertexBuffer();
		void createIndexBuffer();
		void createDescriptorSetLayout();
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();
		void loadModel();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void createTextureSampler();
		void createTextureImageView();
		void createDepthResources();

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void recordCommandBuffer(VkCommandBuffer commandBuffers, uint32_t imageIndex);
		
		bool checkValidationLayerSupport();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		bool isdeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		const int width;
		const int height;

		const std::string MODEL_PATH = "C:/dev/Minor/Card/src/Card/models/vikingroom.obj";
		const std::string TEXTURE_PATH = "C:/dev/Minor/Card/src/Card/textures/viking_room.png";

		std::string windowname;
		GLFWwindow* window;

		//-----------wrapper classes-----------

		GraphicsPipeline graphicsPipeline;

		//----------------vulkan---------------
		VkInstance vkinstance;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkSwapchainKHR swapChain;
		VkSurfaceKHR surface;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		VkDescriptorPool descriptorPool;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;

		VkCommandPool commandPool;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;


		bool framebufferResized = false;

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
