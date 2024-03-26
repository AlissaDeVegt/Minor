#include "Window.h"

namespace Card {
    Window::Window(int width, int height, std::string windowname) :width{ width }, height{ height }, windowname{ windowname }
    {
        initWindow();
        initVulkan();
    }

    Window::~Window()
    {
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(vkinstance, surface, nullptr);
        vkDestroyInstance(vkinstance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }


    /// <summary>
    /// Initialising window
    /// </summary>
    void Window::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr);
    }

    /// <summary>
    /// Window update
    /// </summary>
    void Window::update(){
        glfwPollEvents();

    }

    //-----------------------------------------------------Vulkan-----------------------------------------------------

    /// <summary>
    /// Initialisation of vulkan and the needed components to make vulkan work.
    /// </summary>
    void Window::initVulkan()
    {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    /// <summary>
    /// Create instance of vulkan instance
    /// </summary>
    void Window::createInstance()
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
    }

    /// <summary>
    /// picks the first suitable graphics card
    /// </summary>
    void Window::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkinstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            CARD_ENGINE_ERROR("failed to find GPUs with Vulkan support!");
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
    }

    /// <summary>
    /// creation of a logical device which will describe which features to be used
    /// </summary>
    void Window::createLogicalDevice()
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

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    }

    /// <summary>
    /// creates a surface for vulkan.
    /// the surface is the connector between an actual window and the vkinstance
    /// </summary>
    void Window::createSurface()
    {
        if (glfwCreateWindowSurface(vkinstance, window, nullptr, &surface) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create window surface!");
        }
    }

    /// <summary>
    /// a check if all requested layers are available
    /// </summary>
    /// <returns>returns whether all requested layers are available</returns>
    bool Window::checkValidationLayerSupport()
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
    bool Window::checkDeviceExtensionSupport(VkPhysicalDevice device)
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
    bool Window::isdeviceSuitable(VkPhysicalDevice device)
    {
        //does the device have a Familyqueue for graphiccal commands
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        //the swap chain is currently adequate as long as there is one format and one present mode.
        if (extensionsSupported) {  
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    /// <summary>
    /// Vulkan works with sending commands to a queue.
    /// because there different types of queue we need to find the queues we need.
    /// </summary>
    /// <param name="device">the device from which the queue family is being looked for</param>
    /// <returns></returns>
    QueueFamilyIndices Window::findQueueFamilies(VkPhysicalDevice device)
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
    
    /// <summary>
    /// the function that will populate the SwapChainSupportDetails struct.
    /// </summary>
    /// <param name="device">the device from which details will be received</param>
    /// <returns>the details of the swapchain</returns>
    SwapChainSupportDetails Window::querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        //get basic capabilities min/max images, min/max height and width of images
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        //get surface formats, these can be multiple so resizing is needed. things like pixel format or color space.
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        //get the available presentmodes these can be multiple so resizing is needed.
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    /// <summary>
    /// chooses the best swapsurface format from list of available formats
    /// our first and most optimal one is with the srgb 
    /// because its the most standard color space for images
    /// </summary>
    /// <param name="availableFormats">list of available formats</param>
    /// <returns>best format of our requirements or the first</returns>
    VkSurfaceFormatKHR Window::chooseSwapsurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        //could add a point system to find the next best thing.
        return availableFormats[0];
    }
    /// <summary>
    /// chhose the chain swapmode.
    /// there are four possible in vulkan 
    /// </summary>
    /// <param name="availablePresentModes">the list of modes the swapchain has available</param>
    /// <returns>the chosen mode</returns>
    VkPresentModeKHR Window::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="capabilities"></param>
    /// <returns></returns>
    VkExtent2D Window::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}

