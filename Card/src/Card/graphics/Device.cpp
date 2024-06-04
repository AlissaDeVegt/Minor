#include "Device.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Card {
    Device::Device(Window* window)
	{
        this->window = window;
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createUniformBuffers();
	}

	Device::~Device()
	{
        vkDestroySampler(device, textureSampler, nullptr);
        vkDestroyImageView(device, textureImageView, nullptr);

        vkDestroyImageView(device, textureImageView, nullptr);

        vkDestroyImage(device, textureImage, nullptr);
        vkFreeMemory(device, textureImageMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);

        graphicsPipeline.destroyPipeline(device);

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(vkinstance, surface, nullptr);
        vkDestroyInstance(vkinstance, nullptr);
	}

    #pragma region -------------------------------getters--------------------------------------------

    VkPhysicalDevice Device::getPhysicalDevice()
    {
        return physicalDevice;
    }

    VkDevice Device::getDevice()
    {
        return device;
    }

    VkSurfaceKHR Device::getSurface()
    {
        return surface;
    }

    Window* Device::getWindow()
    {
        return window;
    }

    VkQueue Device::getGraphicsqueue()
    {
        return graphicsQueue;
    }

    int Device::getMax_Frames_In_Flight()
    {
        return MAX_FRAMES_IN_FLIGHT;
    }

    VkBuffer Device::getUniformBuffer(int i)
    {
        return uniformBuffers[i];
    }

    VkSampler Device::getTextureSampler()
    {
        return textureSampler;
    }

    VkImageView Device::getTextureImageView()
    {
        return textureImageView;
    }

    #pragma endregion


    void Device::drawFrame(Renderer* renderer)
    {
        vkWaitForFences(device, 1, &renderer->getSwapchain()->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, renderer->getSwapchain()->getSwapChain(), UINT64_MAX, renderer->getSwapchain()->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            renderer->getSwapchain()->recreateSwapChain();//todo create helper funct
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            CARD_ENGINE_ERROR("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &renderer->getSwapchain()->getInFlightFences()[currentFrame]);

        vkResetCommandBuffer(*renderer->getCommandBuffer(currentFrame), 0);

        recordCommandBuffer(*renderer->getCommandBuffer(currentFrame), imageIndex, renderer);

        updateUniformBuffer(currentFrame, renderer->getSwapchain());


        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { renderer->getSwapchain()->getImageAvailableSemaphores()[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = renderer->getCommandBuffer(currentFrame);

        VkSemaphore signalSemaphores[] = { renderer->getSwapchain()->getRenderFinishedSemaphores()[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, renderer->getSwapchain()->getInFlightFences()[currentFrame]) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { renderer->getSwapchain()->getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->getframebufferResized()) {
            window->setframebufferResized(false);
            renderer->getSwapchain()->recreateSwapChain(); //todo create helper function
        }
        else if (result != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Device::waitDevice()
    {
        vkDeviceWaitIdle(device);
    }

    void Device::afterSwapchainCreation(Renderer* renderer, Descriptor* descriptor )
    {
        this->descriptor = descriptor;
        graphicsPipeline = GraphicsPipeline("C:/dev/Minor/Card/src/Card/shaders/vert.spv", "C:/dev/Minor/Card/src/Card/shaders/frag.spv", device, renderer->getSwapchain()->getRenderPass(), descriptor->getLayout());

        renderer->continueSwapChainCreation();

        createTextureImage(renderer);
        createTextureImageView(renderer->getSwapchain());
        createTextureSampler();

        descriptor->createDescriptorSets();

        loadModel();
        createVertexBuffer(renderer,models[0]);
        createIndexBuffer(renderer, models[0]);
    }

    #pragma region  -------------------------------device setup------------------------------------------

    /// <summary>
    /// Create instance of vulkan instance
    /// </summary>
    void Device::createInstance()
    {
        //debug error
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            CARD_ENGINE_ERROR("validation layers requested, but not available!");
        }

        //creation of aplication info
        VkApplicationInfo appData{};
        appData.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appData.pApplicationName = "CardGame";
        appData.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appData.pEngineName = "No Engine";
        appData.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appData.apiVersion = VK_API_VERSION_1_0;

        //creation of vulkan instance info
        //Tells the Vulkan driver which global extensions and validation layers to use
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appData;

        //get the extenstion interface from glfw voor the window system.
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        //debugerror 
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &vkinstance);

        if (vkCreateInstance(&createInfo, nullptr, &vkinstance) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create instance!");
        }
        else {
            CARD_ENGINE_INFO("Succes in creating the instance");
        }
    }

    /// <summary>
    /// picks the first suitable graphics card
    /// </summary>
    void Device::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkinstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            CARD_ENGINE_ERROR("failed to find GPUs with Vulkan support!");
        }
        else {
            CARD_ENGINE_INFO("Succes in finding a Gpu with vulkan support");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkinstance, &deviceCount, devices.data());

        //check if any of the found devices meets the requirements
        for (const auto& device : devices) {
            if (isdeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            CARD_ENGINE_ERROR("failed to find a suitable GPU!");
        }
        else {
            CARD_ENGINE_INFO("Succes in finding a suitable GPU");
        }
    }

    /// <summary>
    /// creation of a logical device which will describe which features to be used
    /// </summary>
    void Device::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        //create info about the number of queues for a singe queue family
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        //creation of the logical device with pointers to the queue creation info and device features 
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        //specify extensions and validation layers device specific
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create logical device!");
        }
        else {
            CARD_ENGINE_INFO("Succes in creating logical device");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }

    /// <summary>
    /// creates a surface for vulkan.
    /// the surface is the connector between an actual window and the vkinstance
    /// aka GLFW
    /// </summary>
    void Device::createSurface()
    {
        window->createSurfaceWindow(vkinstance, &surface);
    }

#pragma endregion


    void Device::createTextureImage(Renderer* renderer)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            CARD_ENGINE_ERROR("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

        transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,renderer);
        copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),renderer);
        transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,renderer);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

        CARD_ENGINE_INFO("Succesfuly created TextureImage");
    }

    void Device::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);
    }

    void Device::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Renderer* renderer)
    {
        VkCommandBuffer commandBuffer = renderer->beginSingleTimeCommands(); //TODO renderer

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else {
            CARD_ENGINE_ERROR("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        renderer->endSingleTimeCommands(commandBuffer);
    }

    void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, Renderer* renderer)
    {
        VkCommandBuffer commandBuffer = renderer->beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        renderer->endSingleTimeCommands(commandBuffer);
    }

    void Device::createTextureSampler()
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
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create texture sampler!");
        }
        else
        {
            CARD_ENGINE_INFO("Succesfuly created TextureImageSampler");
        }

    }

    void Device::createTextureImageView(Swapchain* swapchain)
    {
        textureImageView = swapchain->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,device);
        CARD_ENGINE_INFO("Succesfuly created TextureImageView");
    }

    bool Device::hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    /// <summary>
    /// creates the vertex buffer  and staging buffer
    /// the official vertex buffer can not be map nor can be touched by the cpu 
    /// which is why the staging buffer is created 
    /// so the cpu can in a round about way touch the vertex buffer.
    /// </summary>
    void Device::createVertexBuffer(Renderer* renderer, Model model)
    {
        VkDeviceSize bufferSize = sizeof(model.getVertices()[0]) * model.getVertices().size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, model.getVertices().data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize,renderer);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

    }

    void Device::createIndexBuffer(Renderer* renderer, Model model)
    {
        VkDeviceSize bufferSize = sizeof(model.getIndices()[0]) * model.getIndices().size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, model.getIndices().data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize, renderer);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);

    }

    void Device::createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

            vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
        }
    }

    void Device::updateUniformBuffer(uint32_t currentImage, Swapchain* swapchain)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        //every object will follow this
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //Camera? 

        ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->getSwapChainExtent().width / (float)swapchain->getSwapChainExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void Device::loadModel()
    {
        models.push_back(ModelLoader::readModelFile(MODEL_PATH));
        models.push_back(ModelLoader::readModelFile(MODEL_PATH).moveObject(glm::vec3{ -1.0f, -1.0f, -1.0f } ));
        models.push_back(ModelLoader::readModelFile(MODEL_PATH).moveObject(glm::vec3{ 1.0f, 1.0f, 1.0f } ));

    }

    /// <summary>
    /// method for creating multiple(different) buffers
    /// </summary>
    /// <param name="size">size of buffer</param>
    /// <param name="usage">how the buffer is going to be used</param>
    /// <param name="properties">properties of the memmory</param>
    /// <param name="buffer">refrence to the buffer</param>
    /// <param name="bufferMemory">refrence to memory of buffer</param>
    void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);


    }

    /// <summary>
    /// copy one buffer to another which is done through command buffers
    /// </summary>
    /// <param name="srcBuffer">buffer from whch is copied</param>
    /// <param name="dstBuffer">buffer to which will be copied</param>
    /// <param name="size">size of buffer</param>
    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, Renderer* renderer)
    {
        VkCommandBuffer commandBuffer = renderer->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        renderer->endSingleTimeCommands(commandBuffer);
    }

    uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        CARD_ENGINE_ERROR("failed to find suitable memory type!");
    }

    /// <summary>
    /// record command buffer
    /// TODO change code to all
    /// </summary>
    /// <param name="commandBuffers"></param>
    /// <param name="imageIndex"></param>
    void Device::recordCommandBuffer(VkCommandBuffer commandBuffers, uint32_t imageIndex, Renderer* renderer)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


        if (vkBeginCommandBuffer(commandBuffers, &beginInfo) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass =  renderer->getSwapchain()->getRenderPass();
        renderPassInfo.framebuffer = renderer->getSwapchain()->getSwapChainFramebuffers()[imageIndex];

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = renderer->getSwapchain()->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getgraphicsPipeline());

        //becasue state dynamic viewport and scissor defined at write to buffer 
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float> (renderer->getSwapchain()->getSwapChainExtent().width);
        viewport.height = static_cast<float>(renderer->getSwapchain()->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffers, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = renderer->getSwapchain()->getSwapChainExtent();
        vkCmdSetScissor(commandBuffers, 0, 1, &scissor);

        descriptor->bind(commandBuffers, graphicsPipeline.getpipelineLayout(), currentFrame);
        
        for (Model model : models) { //render each model
            createVertexBuffer(renderer, model);
            createIndexBuffer(renderer, model);
            VkBuffer vertexBuffers[] = { vertexBuffer};
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffers, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffers, static_cast<uint32_t>(model.getIndices().size()), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers);

        if (vkEndCommandBuffer(commandBuffers) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to record command buffer!");
        }

    }

    /// <summary>
    /// a check if all requested layers are available
    /// </summary>
    /// <returns>returns whether all requested layers are available</returns>
    bool Device::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    /// <summary>
    /// Checks if device supports required extensions
    /// </summary>
    /// <param name="device">the device thats being checked on suitability</param>
    /// <returns>if the requiredextension is empty it will return true else false</returns>
    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    /// <summary>
    /// requirements a suitable device needs to have
    /// </summary>
    /// <param name="device">the device thats being checked on suitability</param>
    /// <returns>if its a suitable device</returns>
    bool Device::isdeviceSuitable(VkPhysicalDevice device)
    {
        //does the device have a Familyqueue for graphiccal commands
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        //the swap chain is currently adequate as long as there is one format and one present mode.
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = Swapchain::querySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    /// <summary>
    /// Vulkan works with sending commands to a queue.
    /// because there different types of queue we need to find the queues we need.
    /// </summary>
    /// <param name="device">the device from which the queue family is being looked for</param>
    /// <returns></returns>
    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        //get the queus from the devie
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        //find atleast one queue that supports VK_QUEUE_GRAPHICS_BIT because we want to perform graphic commands
        //find atleast one queue that supports SurfaceSupport
        //its possible to add logic that makes us prefer a device that supports drawing 
        // and presentation in the same queue for improved performance.
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

}