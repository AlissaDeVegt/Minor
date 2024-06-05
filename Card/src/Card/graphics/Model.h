#pragma once
#include "../Base.h"

#include "Vertex.h"
#include "Descriptor.h"
#include "stb_image.h"

#include <vector>

namespace Card {

	class Device;
	class CARD_API Model
	{
	public:
		Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices,Device* device, std::string path);
		~Model();
;

		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();
		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();
		Descriptor* getDescriptor();

		Model* moveObject(glm::vec3 newPosition);
		Model* resetObject();

	private:
		void createTextureImage(std::string TEXTURE_PATH);
		void createTextureSampler();
		void createTextureImageView();
		void updateVertexBuffer();
		void createVertexBuffer();
		void createIndexBuffer();

		Device* device;
		Descriptor* descriptor;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::vec3 position;
		glm::vec3 rotation;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

		
	};

}

