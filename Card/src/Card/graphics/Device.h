#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "../Util/ModelLoader.h"
#include "Vertex.h"
#include "GraphicsPipeline.h"
#include "Window.h"
#include "Model.h"
#include "Swapchain.h"
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

namespace Card {
	
	class Swapchain;
	class Renderer;

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

	class CARD_API Device
	{
	public: 

		Device(Window* window);
		~Device();

		VkPhysicalDevice getPhysicalDevice();
		VkDevice getDevice();
		VkSurfaceKHR getSurface();
		Window* getWindow();
		VkQueue getGraphicsqueue();
		int getMax_Frames_In_Flight();
		VkBuffer getUniformBuffer(int i);
		VkSampler getTextureSampler();
		VkImageView getTextureImageView();

		void drawFrame(Renderer* renderer);
		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSurface();
		
		void waitDevice();

		void afterSwapchainCreation(Renderer* renderer);

		void createTextureImage(Renderer* renderer);
		void createVertexBuffer(Renderer* renderer,Model model);
		void createIndexBuffer(Renderer* renderer,Model model);
		void createDescriptorSetLayout();//todo remove
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage, Swapchain* swapchain);
		void createDescriptorPool(); //todo remove
		void createDescriptorSets();//todo remove
		void loadModel();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Renderer* renderer);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, Renderer* renderer);

		void createTextureSampler();
		void createTextureImageView(Swapchain* swapchain);

		bool hasStencilComponent(VkFormat format);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, Renderer* renderer);

		void recordCommandBuffer(VkCommandBuffer commandBuffers, uint32_t imageIndex, Renderer* renderer);

		bool checkValidationLayerSupport();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		bool isdeviceSuitable(VkPhysicalDevice device);
	    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		const int MAX_FRAMES_IN_FLIGHT = 2; //todo remove
		uint32_t currentFrame = 0;

		const std::string MODEL_PATH = "C:/dev/Minor/Card/src/Card/models/vikingroom.obj";
		const std::string TEXTURE_PATH = "C:/dev/Minor/Card/src/Card/textures/viking_room.png";

		//-----------wrapper classes-----------

		GraphicsPipeline graphicsPipeline;
		Window* window;

		//----------------vulkan---------------
		VkInstance vkinstance;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkSurfaceKHR surface;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;


		VkQueue graphicsQueue;
		VkQueue presentQueue;



		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;



		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
		std::vector<Model> models;


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
}
