#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "Device.h"
#include "Swapchain.h"

namespace Card{
	class Device;
	class Swapchain;

	class CARD_API Renderer
	{
	public:
		Renderer(Device* device);
		~Renderer();

		Swapchain* getSwapchain();

		VkCommandBuffer* getCommandBuffer(int  number);

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);


	private:
		void createCommandPool();
		void createCommandBuffers();

		Device* device;
		Swapchain* swapchain;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	};

}

