#include "Model.h"
#include "Device.h"
#include "Swapchain.h"
#include "Camera.h"

namespace Card {
	Model::Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Device* device,std::string path,Camera* camera)
	{
		this->indices = indices;
		this->vertices = vertices;
		this->device=device;

		createVertexBuffer();
		createIndexBuffer();

		createTextureImage(path);
		createTextureImageView();
		createTextureSampler();

		descriptor = new Descriptor(device,textureImageView,textureSampler,camera);
	}

	Model::~Model()
	{

		vkDestroyBuffer(device->getDevice(), indexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), indexBufferMemory, nullptr);

		vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);

		vkDestroySampler(device->getDevice(), textureSampler, nullptr);
		vkDestroyImageView(device->getDevice(), textureImageView, nullptr);

		vkDestroyImage(device->getDevice(), textureImage, nullptr);
		vkFreeMemory(device->getDevice(), textureImageMemory, nullptr);

		CARD_ENGINE_WARN("model destroyed");
	}

	glm::vec3 Model::getPosition()
	{
		return position;
	}

	glm::vec3 Model::getRotation()
	{
		return rotation;
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

	Descriptor* Model::getDescriptor()
	{
		return descriptor;
	}

	/// <summary>
	/// move model.
	/// </summary>
	/// <param name="posX"> x position </param>
	/// <param name="posY"> Y position </param>
	/// <param name="posZ"> z position </param>
	void Model::move(float posX, float posY, float posZ)
	{
		glm::vec3 newPosition = glm::vec3{ posX,posZ,posY };

		position = newPosition;

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x + posX;
			v->pos.y = v->pos.y + posZ;
			v->pos.z = v->pos.z + posY;
		}
	}

	/// <summary>
	/// change rotation by calling the rotate individual function.
	/// only do so when rotation is not 0.
	/// </summary>
	/// <param name="rotX">x rotation</param>
	/// <param name="rotY">y rotation</param>
	/// <param name="rotZ">z rotation</param>
	void Model::rotate(float rotX, float rotY, float rotZ)
	{
		glm::vec3 newrotation = this->rotation + glm::vec3{ rotX,rotY,rotZ };

		resetRotation(); 

		this->rotation = newrotation;


		if (this->rotation.x != 0) {
			rotateIndividual(glm::vec3{ 1.0f,0.0f,0.0f }, this->rotation.x);
		}		
		if (this->rotation.y != 0) {
			rotateIndividual(glm::vec3{ 0.0f,1.0f,0.0f }, this->rotation.y);
		}		
		if (this->rotation.z != 0) {
			rotateIndividual(glm::vec3{ 0.0f,0.0f,1.0f }, this->rotation.z);
		}
	}

	/// <summary>
	/// reset the rotation of the model.
	/// </summary>
	void Model::resetRotation()
	{
		if (rotation.z!=0)
		{
			resetRot(glm::vec3{ 0.0f,0.0f,-rotation.z }, -rotation.z);		
			this->rotation.z = 0;
		}
		if (rotation.y != 0)
		{
			resetRot(glm::vec3{ 0.0f,-rotation.y,0.0f }, -rotation.y);
			this->rotation.y = 0;
		}
		if (rotation.x != 0)
		{
			resetRot(glm::vec3{ -rotation.x,0.0f,0.0f }, -rotation.x);
			this->rotation.x = 0;
		}



	}

	/// <summary>
	/// set the size of model.
	/// </summary>
	/// <param name="size"> size of model in float</param>
	void Model::setSize(float size)
	{
		this->size = size;
		resetPosition();

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x * size;			

			v->pos.y = v->pos.y * size;			

			v->pos.z = v->pos.z * size;			
		}

		move(position.x, position.z, position.y);

	}

	/// <summary>
	/// reset size to original size.
	/// </summary>
	void Model::resetSize()
	{
		resetPosition();

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x / size;
			v->pos.y = v->pos.y / size;
			v->pos.z = v->pos.z / size;
		}

		move(position.x, position.z, position.y);

	}

	/// <summary>
	/// reset position to origin.
	/// </summary>
	void Model::resetPosition()
	{
		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x - position.x;
			v->pos.y = v->pos.y - position.y;
			v->pos.z = v->pos.z - position.z;
		}
	}

	/// <summary>
	/// rotate on axis individual.
	/// </summary>
	/// <param name="axis">the axis which will be normalised</param>
	/// <param name="decrees">degrees in which it needs to be  rotated in float</param>
	void Model::rotateIndividual(glm::vec3 axis, float decrees)
	{
		rotationQuat = glm::quat(cos(glm::radians(decrees / 2)), sin(glm::radians(decrees / 2)) * axis.x, sin(glm::radians(decrees / 2)) * axis.y, sin(glm::radians(decrees / 2)) * axis.z);
		glm::mat4 rotationmatrix = glm::toMat4(rotationQuat);

		resetSize();
		resetPosition();

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];

			if (decrees < 0) {
				v->pos = rotationmatrix / glm::vec4((v->pos), 1.0f);
			}
			else {
				v->pos = rotationmatrix * glm::vec4((v->pos), 1.0f);
			}
		}

		move(position.x, position.z, position.y);
		setSize(size);
	}

	/// <summary>
	/// reset rotation of individual axis.
	/// </summary>
	/// <param name="axis">the axis</param>
	/// <param name="decrees">the degrees</param>
	void Model::resetRot(glm::vec3 axis, float decrees)
	{
		glm::vec3 normal = glm::normalize(axis);
		rotationQuat = glm::quat(cos(glm::radians(decrees / 2)), sin(glm::radians(decrees / 2)) * normal.x, sin(glm::radians(decrees / 2)) * normal.y, sin(glm::radians(decrees / 2)) * normal.z);
	
		glm::mat4 rotationmatrix = glm::toMat4(rotationQuat);

		resetSize();
		resetPosition();

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];

			v->pos = glm::vec4((v->pos), 1.0f)* rotationmatrix;
		}

		move(position.x, position.z, position.y);
		setSize(size);
	}

	/// <summary>
	/// creates texture images from texture path.
	/// </summary>
	/// <param name="TEXTURE_PATH"> path to texture</param>
	void Model::createTextureImage(std::string TEXTURE_PATH)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			CARD_ENGINE_ERROR("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		device->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		void* data;
		vkMapMemory(device->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device->getDevice(), stagingBufferMemory);

		stbi_image_free(pixels);

		device->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		device->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		device->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);

		CARD_ENGINE_INFO("Succesfuly created TextureImage");
	}

	/// <summary>
	/// create texture sampler, basically an algorithm to calculate the pixel color based on the colors around it.
	/// </summary>
	void Model::createTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(device->getPhysicalDevice(), &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			CARD_ENGINE_ERROR("failed to create texture sampler!");
		}
		else
		{
			CARD_ENGINE_INFO("Succesfuly created TextureImageSampler");
		}

	}

	/// <summary>
	/// create texture image view, basically it tells the device how to read the file.
	/// </summary>
	void Model::createTextureImageView()
	{
		textureImageView = device->getRenderer()->getSwapchain()->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, device->getDevice());
		CARD_ENGINE_INFO("Successfully created TextureImageView");
		
	}

	/// <summary>
	/// update the data of the vertex buffer, without recreating the buffer.
	/// </summary>
	void Model::UpdateVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		void* data;
		vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device->getDevice(), stagingBufferMemory);

		device->copyBuffer(stagingBuffer, &vertexBuffer, bufferSize);

		vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
	}

	/// <summary>
	/// create vertex buffer which contains the information about the vertices.
	/// </summary>
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

	/// <summary>
	/// create index buffer which contains a buffer of which vertex needs to be drawn with which.
	/// </summary>
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