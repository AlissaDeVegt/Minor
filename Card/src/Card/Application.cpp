#include "Application.h"

#include "Logger.h"
#include "SceneBuilder.h"

#include "graphics/Window.h"
#include "graphics/Device.h"
#include "graphics/Swapchain.h"
#include "graphics/Renderer.h"
#include "graphics/Descriptor.h"
#include "graphics/Camera.h"


namespace Card{

	Application::Application()
	{
	}
	
	Application::~Application()
	{
	}
	
	/// <summary>
	/// the main loop of the game AND game engine
	/// </summary>
	void Application::Run()
	{
		window = new Window(850, 400, "Cards");
		device = new Device(window);		
		renderer = new Renderer(device);
		scenebuilder = new SceneBuilder(device);
		device->afterSwapchainCreation(renderer, scenebuilder);
		start();
		
		while (!glfwWindowShouldClose(window->getGlfwWindow()))
		{
			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_D) == GLFW_PRESS) {
				scenebuilder->moveCamera(-0.001f, 0.0f, 0.0f);

			}
			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_A) == GLFW_PRESS) {
				scenebuilder->moveCamera(0.001f, 0.0f, 0.0f);

			}

			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_W) == GLFW_PRESS) {
				scenebuilder->moveCamera(0.00f, 0.001f, 0.0f);

			}

			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_S) == GLFW_PRESS) {
				scenebuilder->moveCamera(0.0f, -0.001f, 0.0f);
			}


			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
				scenebuilder->moveCamera(0.00f, 0.000f, 0.001f);

			}

			if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
				scenebuilder->moveCamera(0.0f, 0.0f, -0.001f);
			}

			window->update();
			device->drawFrame(renderer);
			update();
		}

		device->waitDevice();
	}




	/// <summary>
	/// Class that gets overwritten by the client.
	/// code for game loop;
	/// </summary>
	void Application::update()
	{

	}

	/// <summary>
	/// Class that gets overwritten by the client.
	/// usefull for code that needs to be run before the start of the game loop
	/// </summary>
	void Application::start()
	{

	}

}
