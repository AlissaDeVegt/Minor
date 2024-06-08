#pragma once

#include "Base.h"

namespace Card {
	class Camera;
	class Device;
	class Window;
	class Renderer;
	class SceneBuilder;


	class CARD_API Application
	{
	public:
		Application();
		~Application();

		void Run();

		Window* window;
		SceneBuilder* scenebuilder;

		virtual void start();
		virtual void update();

	private:
		Camera* camera;
		Device* device;
		Renderer* renderer;

	};

	Application* CreateApplication(); //client defined

}