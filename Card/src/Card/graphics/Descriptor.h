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
		Descriptor(Device* device, uint32_t MAX_FRAMES_IN_FLIGHT);
		~Descriptor();
		void createDescriptorSetLayout();
		void createDescriptorPool();
		void createDescriptorSets();

	private:
		Device* device;
		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;

		int MAX_FRAMES_IN_FLIGHT;
	};
}


