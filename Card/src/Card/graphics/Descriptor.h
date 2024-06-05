#pragma once
#include "../Base.h"
#include "../Logger.h"

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Card {
	class Device;

	class Descriptor
	{
	public:
		Descriptor();
		Descriptor(Device* device);
		~Descriptor();

		void createDescriptorPool(int MAX_FRAMES_IN_FLIGHT);
		void createDescriptorSets();
		void bind(VkCommandBuffer commandBuffers, VkPipelineLayout layout, int currentImage);
		VkDescriptorSetLayout* getLayout();

	private:
		void createDescriptorSetLayout();

		Device* device;
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;

		int MAX_FRAMES_IN_FLIGHT;
	};
}


