#include "Window.h"

namespace Card {
    Window::Window(int width, int height, std::string windowname) :width{ width }, height{ height }, windowname{ windowname }
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    /// <summary>
    /// set size of the window
    /// </summary>
    /// <param name="width">width of window</param>
    /// <param name="height">height of window</param>
    void Window::setSize(int width, int height)
    {
        this->width = width;
        this->height = height;

        glfwSetWindowSize(window, width, height);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    /// <summary>
    /// Initialising window
    /// </summary>
    void Window::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    /// <summary>
    /// Window update
    /// </summary>
    void Window::update(){
        glfwPollEvents();
    }

    /// <summary>
    /// create a surface between the window and vulkan
    /// </summary>
    /// <param name="vkinstance"></param>
    /// <param name="surface"></param>
    void Window::createSurfaceWindow(VkInstance vkinstance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(vkinstance, window, nullptr, surface) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create window surface!");
        }
        else {
            CARD_ENGINE_INFO("Succes in creating surface");
        }
    }

    /// <summary>
    /// if window frame changes
    /// </summary>
    /// <param name="window"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    //--------------------------set&get-------------------------------------------

    bool Window::getframebufferResized()
    {
        return framebufferResized;
    }

    void Window::setframebufferResized(bool framebufferResized)
    {
        this->framebufferResized = framebufferResized;
    }

    GLFWwindow* Window::getGlfwWindow()
    {
        return window;
    }

}

