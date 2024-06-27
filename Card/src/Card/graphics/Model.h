#pragma once
#include "../Base.h"

#include "Vertex.h"
#include "Descriptor.h"
#include "stb_image.h"


#include <gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp >

#include <vector>

namespace Card {

	class Device;
	class Camera;
	class CARD_API Model
	{
	public:
		Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices,Device* device, std::string path,Camera* camera);
		~Model();
;
		glm::vec3 getPosition();
		glm::vec3 getRotation();
		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();
		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();
		Descriptor* getDescriptor();

		void resetPosition();
		void move(float posX, float posY, float posZ);
		void rotate(float rotX, float rotY, float rotZ);
		void resetRotation();
		void setSize(float size);
		void resetSize();
		void UpdateVertexBuffer();

	private:
		void rotateIndividual(glm::vec3 axis, float decrees);
		void resetRot(glm::vec3 axis, float decrees);
		void createTextureImage(std::string TEXTURE_PATH);
		void createTextureSampler();
		void createTextureImageView();
		void createVertexBuffer();
		void createIndexBuffer();

		Device* device;
		Descriptor* descriptor;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::vec3 position;
		float size = 1;
		glm::vec3 rotation = glm::vec3{0.0f,0.0f,0.0f};
		glm::vec3 up = glm::vec3{0.0f,0.0f,1.0f};
		glm::vec3 lookat = glm::vec3{ 0.0f,-1.0f,0.0f };
		glm::quat rotationQuat{ 0.0f,0.0f,0.0f,0.0f };

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

