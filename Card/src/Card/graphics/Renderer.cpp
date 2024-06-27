#include "Renderer.h"
namespace Card {

	Renderer::Renderer(Device* device)
	{
		this->device = device;
		this->swapchain = new Swapchain(device,this);

	}

	Renderer::~Renderer()
	{

	}

	Swapchain* Renderer::getSwapchain()
	{
		return swapchain;
	}

	VkCommandBuffer* Renderer::getCommandBuffer(int number)
	{
		return &commandBuffers[number];
	}

	/// <summary>
	/// create commandbuffers
	/// </summary>
	void Renderer::createCommandBuffers()
	{
		commandBuffers.resize(swapchain->getMaxFramesInFlight());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device->getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			CARD_ENGINE_ERROR("failed to allocate command buffers!");
		}
		else {
			CARD_ENGINE_INFO("succesfully allocated command buffers!");
		}
	}

	/// <summary>
	/// continue swapchain creation
	/// </summary>
	void Renderer::continueSwapChainCreation()
	{
		swapchain->createDepthResources();
		swapchain->createFramebuffers();
		swapchain->createSyncObjects();
	}
}