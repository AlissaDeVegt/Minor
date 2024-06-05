#pragma once
#include "../Base.h"

#include "Vertex.h"

#include <vector>

namespace Card {
	class Swapchain;
	class Device;
	class CARD_API Model
	{
	public:
		Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices,Device* device, Swapchain* swapchain);
		~Model();

		VkImageView getImageview();
		VkSampler getSampler();

		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();
		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();

		Model* moveObject(glm::vec3 newPosition);
		Model* resetObject();

	private:
		void updateVertexBuffer();
		void createVertexBuffer();
		void createIndexBuffer();

		Device* device;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::vec3 position;
		glm::vec3 rotation;


		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
	};

}

