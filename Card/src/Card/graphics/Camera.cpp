#include "Camera.h"
#include "Device.h"

namespace Card {
	Camera::Camera(Device* device)
	{
		this->device = device;

	}

	Camera::~Camera()
	{
		for (size_t i = 0; i < uniformBuffers.size(); i++) {
			vkDestroyBuffer(device->getDevice(), uniformBuffers[i], nullptr);
			vkFreeMemory(device->getDevice(), uniformBuffersMemory[i], nullptr);
		}

	}

	VkBuffer Camera::getUniformBuffer(int i)
	{
		return uniformBuffers[i];
	}

	void Camera::createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(device->getRenderer()->getSwapchain()->getMaxFramesInFlight());
		uniformBuffersMemory.resize(device->getRenderer()->getSwapchain()->getMaxFramesInFlight());
		uniformBuffersMapped.resize(device->getRenderer()->getSwapchain()->getMaxFramesInFlight());

		for (size_t i = 0; i < device->getRenderer()->getSwapchain()->getMaxFramesInFlight(); i++) {
			device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformBuffers[i], &uniformBuffersMemory[i]);

			vkMapMemory(device->getDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void Camera::updateUniformBuffer(int currentImage,Device* device)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		//every object will follow this
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //Camera? 

		ubo.proj = glm::perspective(glm::radians(45.0f), device->getRenderer()->getSwapchain()->getSwapChainExtent().width / (float)device->getRenderer()->getSwapchain()->getSwapChainExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void Camera::setCamera(int currentImage, Device* device)
	{
		//every object will follow this
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		//		                         	           
		ubo.view = glm::lookAt(position, lookat, glm::vec3(0.0f, 0.0f, 1.0f)); //Camera? 

		ubo.proj = glm::perspective(glm::radians(45.0f), device->getRenderer()->getSwapchain()->getSwapChainExtent().width / (float)device->getRenderer()->getSwapchain()->getSwapChainExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
	
	void Camera::setPosition(glm::vec3 position)
	{
		this->position = position;
	}

	void Camera::setLookAt(glm::vec3 lookat)
	{
		this->lookat = lookat;
	}

	glm::vec3 Camera::getLookAt()
	{
		return lookat;
	}
}