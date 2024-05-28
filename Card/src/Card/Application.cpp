#include "Application.h"

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

		while (!glfwWindowShouldClose(window->getGlfwWindow()))
		{
			window->update();
			this->update();

		}

		window->waitDevice();
	}

	/// <summary>
	/// Class that gets overwritten by the client for the updates of game code.
	/// </summary>
	void Application::update()
	{

	}

}
