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

		void continueSwapChainCreation();
		void createCommandBuffers();

	private:

		Device* device;
		Swapchain* swapchain;

		std::vector<VkCommandBuffer> commandBuffers;

	};

}

