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

    void Window::createSurfaceWindow(VkInstance vkinstance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(vkinstance, window, nullptr, surface) != VK_SUCCESS) {
            CARD_ENGINE_ERROR("failed to create window surface!");
        }
        else {
            CARD_ENGINE_INFO("Succes in creating surface");
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

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

