#include "Model.h"
#include "Device.h"
#include "Swapchain.h"

namespace Card {
	Model::Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Device* device)
	{
		this->indices = indices;
		this->vertices = vertices;
		this->device=device;
		createVertexBuffer();
		createIndexBuffer();

	}
	Model::~Model()
	{

		vkDestroyBuffer(device->getDevice(), indexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), indexBufferMemory, nullptr);

		vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);


		CARD_ENGINE_WARN("model destroyed");
	}

	std::vector<Vertex> Model::getVertices()
	{
		return vertices;
	}
	std::vector<uint32_t> Model::getIndices()
	{
		return indices;
	}

	VkBuffer Model::getVertexBuffer()
	{
		return vertexBuffer;
	}

	VkBuffer Model::getIndexBuffer()
	{
		return indexBuffer;
	}

	Model* Model::moveObject(glm::vec3 newPosition)
	{
		position = newPosition;

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x + position.x;
			v->pos.y = v->pos.y + position.y;
			v->pos.z = v->pos.z + position.z;
		}

		updateVertexBuffer();

		return this;
	}
	Model* Model::resetObject()
	{
		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x - position.x;
			v->pos.y = v->pos.y - position.y;
			v->pos.z = v->pos.z - position.z;
		}
		return this;
	}

	void Model::updateVertexBuffer()
	{
		vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);

		createVertexBuffer();
	}

	void Model::createVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		void* data;
		vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device->getDevice(), stagingBufferMemory);

		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

		device->copyBuffer(stagingBuffer, &vertexBuffer, bufferSize);

		vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
	}

	void Model::createIndexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		void* data;
		vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device->getDevice(), stagingBufferMemory);

		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

		device->copyBuffer(stagingBuffer, &indexBuffer, bufferSize);

		vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
	}
}