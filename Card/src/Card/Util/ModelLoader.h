#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "../graphics/Window.h"
#include "../graphics/Model.h"
#include "../graphics/Vertex.h"
#include "Split.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Card {
	class Device;

	class CARD_API ModelLoader
	{
	public:
		static Model* readModelFile(std::string modelPath,Device* device, Swapchain* swapchain);
		
	};
}

