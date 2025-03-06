#pragma once
#include "../Base.h"
#include "../Logger.h"

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Card {
	class Device;
	class Camera;

	class Descriptor
	{
	public:
		Descriptor();
		Descriptor(Device* device,VkImageView imageview, VkSampler sampler, Camera* camera);
		~Descriptor();

		void createDescriptorPool(int MAX_FRAMES_IN_FLIGHT);
		void createDescriptorSets(VkImageView imageview, VkSampler sampler, Camera* camera);
		void bind(VkCommandBuffer commandBuffers, VkPipelineLayout layout, int currentImage);

	private:

		Device* device;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		int MAX_FRAMES_IN_FLIGHT;
	};
}


