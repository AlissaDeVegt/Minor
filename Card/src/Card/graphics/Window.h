#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "../Util/ModelLoader.h"
#include "GraphicsPipeline.h"

#include "Vertex.h"

#define VK_USE_PLATFORM_WIN64_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

#define GLFW_EXPOSE_NATIVE_WIN64
#include <GLFW/glfw3native.h>


#include <string>
#include <iostream>
#include <optional>
#include <set>
#include <fstream>


namespace Card {

	class CARD_API Window
	{
	public:
		Window(int width, int height, std::string windowname);
		~Window();

		void setSize(int width, int height);

		void update();
		void createSurfaceWindow(VkInstance vkinstance, VkSurfaceKHR* surface);

		static void framebufferResizeCallback(GLFWwindow * window, int width, int height);

		bool getframebufferResized();
		void setframebufferResized(bool framebufferResized);

		GLFWwindow* getGlfwWindow();

	private:
		void initWindow();
		int width;
		int height;

		std::string windowname;
		GLFWwindow* window;

		bool framebufferResized = false;

	};


};
