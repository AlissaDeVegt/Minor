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

	/// <summary>
	/// creates the unform buffers that will be used for the frames in flight
	/// </summary>
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

	/// <summary>
	/// fill the unform buffer of the current image with the needed info
	/// </summary>
	/// <param name="currentImage">the index of currentimage </param>
	/// <param name="device">the logical device</param>
	void Camera::setCamera(int currentImage, Device* device)
	{
		//every object will follow this
		UniformBufferObject ubo{};

		ubo.model = glm::mat4(1.0f); //because the models need to be individual
                    	           
		ubo.view = glm::lookAt(position, lookat, up); 

		ubo.proj = glm::perspective(glm::radians(45.0f), device->getRenderer()->getSwapchain()->getSwapChainExtent().width / (float)device->getRenderer()->getSwapchain()->getSwapChainExtent().height, 0.1f, viewdistance);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
	
	/// <summary>
	/// set view distance
	/// </summary>
	/// <param name="distance">view distance</param>
	void Camera::setViewDistance(float distance)
	{
		this->viewdistance = distance;
	}

	/// <summary>
	/// set the position of camera
	/// </summary>
	/// <param name="position">position of camera</param>
	void Camera::setPosition(glm::vec3 position)
	{
		this->position = position;
	}

	/// <summary>
	/// reset rotation to basic poinT
	/// </summary>
	void Camera::resetRotation()
	{
		lookat = glm::vec3{ 0.0f,-1.0f,0.0f };

	}

	/// <summary>
	/// reset position;
	/// </summary>
	void Camera::resetPosition()
	{
		position = glm::vec3{ 0.0f,0.0f,0.0f };
	}

	/// <summary>
	/// reset camera position and rotation
	/// </summary>
	void Camera::reset()
	{
		resetRotation();
		resetPosition();
	}

	/// <summary>
	/// move camera position by adding new position to old one
	/// </summary>
	/// <param name="x">how much move in x axis</param>
	/// <param name="y">how much move in y axis</param>
	/// <param name="z">how much move in z axis</param>
	void Camera::move(float x, float y, float z)
	{
		this->position = glm::vec3{position.x+x,position.y + y,position.z + z };
		this->lookat = glm::vec3{ lookat.x + x,lookat.y + y,lookat.z + z };
	}

	/// <summary>
	/// rotate using glm rotate function and lookup 
	/// this is a little bit buggy on the x axis.
	/// cant work for y axis which should tilt the camera
	/// </summary>
	/// <param name="rotation">a vec3 that contains the degrees of the rotation</param>
	void Camera::rotate(glm::vec3 rotation)
	{
		this->rotation = rotation;
		
		lookat = (glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(this->rotation.z), up)) * glm::vec3{ lookat.x - position.x, lookat.y - position.y, lookat.z - position.z }) + glm::vec3(position);

		glm::vec3 rotatearound = glm::cross(lookat,up);

		lookat = (glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(this->rotation.x), rotatearound)) * glm::vec3{ lookat.x , lookat.y , lookat.z }) ;

	}

}