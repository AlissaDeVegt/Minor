#include "Renderer.h"
namespace Card {

	Renderer::Renderer(Device* device)
	{
		this->device = device;
		this->swapchain = new Swapchain(device);

		createCommandPool();
		createCommandBuffers();
	}

	Renderer::~Renderer()
	{

		vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);

	}

	Swapchain* Renderer::getSwapchain()
	{
		return swapchain;
	}

	VkCommandBuffer* Renderer::getCommandBuffer(int number)
	{
		return &commandBuffers[number];
	}

	void Renderer::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies(device->getPhysicalDevice());

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
			CARD_ENGINE_ERROR("failed to create command pool!");
		}
		else {
			CARD_ENGINE_INFO("succesfully created commandpool");
		}
	}

	void Renderer::createCommandBuffers()
	{
		commandBuffers.resize(swapchain->getMaxFramesInFlight());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			CARD_ENGINE_ERROR("failed to allocate command buffers!");
		}
		else {
			CARD_ENGINE_INFO("succesfully allocated command buffers!");
		}
	}

	VkCommandBuffer Renderer::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void Renderer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(device->getGraphicsqueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->getGraphicsqueue());

		vkFreeCommandBuffers(device->getDevice(), commandPool, 1, &commandBuffer);
	}
}