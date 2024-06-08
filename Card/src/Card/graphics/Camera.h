#pragma once
#include "../Base.h"
#include "../Logger.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <string>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>


namespace Card {

	class Device;

	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	class CARD_API Camera
	{
	public:
		Camera(Device* device);
		~Camera();

		VkBuffer getUniformBuffer(int i);

		void createUniformBuffers();
		void updateUniformBuffer(int currentImage , Device* device);
		void setCamera(int currentImage , Device* device);

		void setPosition(glm::vec3 position);
		void setLookAt(glm::vec3 lookat);
		glm::vec3 getLookAt();

	private:
		Device* device;
		glm::vec3 position =glm::vec3{0.0f,0.0f,0.0f};
		glm::vec3 lookat =glm::vec3{0.0f,0.0f,0.0f};
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;
	};

}
