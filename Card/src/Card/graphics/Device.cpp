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
        createCommandPool();
        createDescriptorSetLayout();

	}

	Device::~Device()
	{

        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        vkDestroyCommandPool(device, commandPool, nullptr);

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


    VkCommandPool Device::getCommandPool()
    {
        return commandPool;
    }

    VkDescriptorSetLayout Device::getDescriptorSetLayout()
    {
        return descriptorSetLayout;
    }

    Renderer* Device::getRenderer()
    {
        return renderer;
    }

    #pragma endregion

    /// <summary>
    /// draw a frame 
    /// </summary>
    /// <param name="renderer"> pointer to a renderer</param>
    void Device::drawFrame(Renderer* renderer)
    {
        vkWaitForFences(device, 1, &renderer->getSwapchain()->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, renderer->getSwapchain()->getSwapChain(), UINT64_MAX, renderer->getSwapchain()->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            renderer->getSwapchain()->recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            CARD_ENGINE_ERROR("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &renderer->getSwapchain()->getInFlightFences()[currentFrame]);
        vkResetCommandBuffer(*renderer->getCommandBuffer(currentFrame), 0);
        recordCommandBuffer(*renderer->getCommandBuffer(currentFrame), imageIndex, renderer);

        scenebuilder->getCamera()->setCamera(currentFrame, this);

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
            renderer->getSwapchain()->recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % renderer->getSwapchain()->getMaxFramesInFlight();
    }

    /// <summary>
    /// record the command buffer to start drawing models 
    /// </summary>
    /// <param name="commandBuffers"></param>
    /// <param name="imageIndex"></param>
    /// <param name="renderer"></param>
    void Device::recordCommandBuffer(VkCommandBuffer commandBuffers, uint32_t imageIndex, Renderer* renderer)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers, &beginInfo) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderer->getSwapchain()->getRenderPass();
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

        for (Model* model : scenebuilder->getModels()) {
            model->getDescriptor()->bind(commandBuffers, graphicsPipeline.getpipelineLayout(), currentFrame);
            VkBuffer buffers[] = { model->getVertexBuffer() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(commandBuffers, 0, 1, buffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers, model->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffers, static_cast<uint32_t>(model->getIndices().size()), 1, 0, 0, 0);

        }

        vkCmdEndRenderPass(commandBuffers);

        if (vkEndCommandBuffer(commandBuffers) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to record command buffer!");
        }
    }

    /// <summary>
    /// make device wait a bit.
    /// </summary>
    void Device::waitDevice()
    {
        vkDeviceWaitIdle(device);
    }

    /// <summary>
    /// perform all actions that are needed after swapchain creation.
    /// </summary>
    /// <param name="renderer">pointer to the renderer</param>
    /// <param name="scenebuilder">pointer to the scenebuilder</param>
    void Device::afterSwapchainCreation(Renderer* renderer, SceneBuilder* scenebuilder)
    {
        this->renderer = renderer;
        this->scenebuilder = scenebuilder;
        graphicsPipeline = GraphicsPipeline("../Card/src/Card/shaders/vert.spv", "../Card/src/Card/shaders/frag.spv", device, renderer->getSwapchain()->getRenderPass(), &descriptorSetLayout);
    }

    /// <summary>
    /// create an image for the device
    /// </summary>
    /// <param name="width">width of image</param>
    /// <param name="height">height of image</param>
    /// <param name="format">format of the image</param>
    /// <param name="tiling">how the image will be tiled</param>
    /// <param name="usage">vk flags of how the image will be used</param>
    /// <param name="properties">memory properies</param>
    /// <param name="image">reference to image</param>
    /// <param name="imageMemory">reference to memory location of the image</param>
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

    /// <summary>
    /// change the image layout
    /// </summary>
    /// <param name="image">image</param>
    /// <param name="format">format</param>
    /// <param name="oldLayout">old layout</param>
    /// <param name="newLayout">new layout</param>
    void Device::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(); 

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

        endSingleTimeCommands(commandBuffer);
    }

    /// <summary>
    /// copy the buffer to image using command
    /// </summary>
    /// <param name="buffer"> the buffer that will be copied from</param>
    /// <param name="image">the image that will be copied to</param>
    /// <param name="width">width of image</param>
    /// <param name="height">height of image</param>
    void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

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

        endSingleTimeCommands(commandBuffer);
    }

    /// <summary>
    /// method for creating multiple(different) buffers
    /// </summary>
    /// <param name="size">size of buffer</param>
    /// <param name="usage">how the buffer is going to be used</param>
    /// <param name="properties">properties of the memmory</param>
    /// <param name="buffer">refrence to the buffer</param>
    /// <param name="bufferMemory">refrence to memory of buffer</param>
    void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, buffer) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(device, *buffer, *bufferMemory, 0);


    }

    /// <summary>
    /// copy one buffer to another which is done through command buffers
    /// </summary>
    /// <param name="srcBuffer">buffer from whch is copied</param>
    /// <param name="dstBuffer">buffer to which will be copied</param>
    /// <param name="size">size of buffer</param>
    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer* dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, *dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

#pragma region -------------------------------------commands----------------------------------------

    /// <summary>
    /// allows single time commands to start with a commandbuffer;
    /// </summary>
    /// <returns>commandbuffers</returns>
    VkCommandBuffer Device::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    /// <summary>
    /// ends the single time command 
    /// </summary>
    /// <param name="commandBuffer">the command buffer in which th single time commands are kept</param>
    void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    /// <summary>
    /// create a commandpool which will hold the command buffers.
    /// </summary>
    void Device::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create command pool!");
        }
        else {
            CARD_ENGINE_INFO("succesfully created commandpool");
        }
    }

#pragma endregion

#pragma region  -----------------------------------device setup--------------------------------------

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

    /// <summary>
    /// create a descriptor set layout that will explain how the descriptorsetlayout
    /// </summary>
    void Device::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create descriptor set layout!");
        }
        else {
            CARD_ENGINE_INFO("succeeded in creating descriptor set");
        }
    }

#pragma endregion

#pragma region --------------------------------------checks-----------------------------------------

    /// <summary>
    /// check if format has stencil component
    /// </summary>
    /// <param name="format">the format</param>
    /// <returns>true if has a stencil component and false if not</returns>
    bool Device::hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    /// <summary>
    /// find a suitable type of memory the physical device has.
    /// </summary>
    /// <param name="typeFilter"></param>
    /// <param name="properties">properties the memory must have</param>
    /// <returns>return index of memory type</returns>
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
    /// <returns>returns family queu indices</returns>
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

#pragma endregion

}