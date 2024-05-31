#pragma once

#include "Base.h"
#include "Logger.h"

#include "graphics/Window.h"
#include "graphics/Device.h"
#include "graphics/Swapchain.h"
#include "graphics/Renderer.h"

namespace Card {
	class CARD_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		Window* window;

		virtual void update();
		virtual void start();
	private:
		Device* device;
		Swapchain* swapchain;
		Renderer* renderer;
	};

	Application* CreateApplication(); //client defined

}