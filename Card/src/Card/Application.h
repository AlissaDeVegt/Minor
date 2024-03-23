#pragma once

#include "Base.h"
namespace Card {
	class CARD_API Application
	{
	public:
		Application();
		~Application();

		void Run();

	};

	Application* CreateApplication(); //client defined
}