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

		renderer->continueSwapChainCreation();
		scenebuilder->getCamera()->createUniformBuffers();
		renderer->createCommandBuffers();
		start();
		
		while (!glfwWindowShouldClose(window->getGlfwWindow()))
		{

			window->update();
			device->drawFrame(renderer);
			update();
			scenebuilder->updateModels();
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
