#pragma once

#include "Base.h"
#include "Window.h"
#include "Logger.h"

namespace Card {
	class CARD_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		Window* window;

		virtual void update();
	};

	Application* CreateApplication(); //client defined

}