#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "../graphics/Window.h"
#include "../graphics/Model.h"
#include "../graphics/Vertex.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Card {
	class Device;
	class Camera;

	class CARD_API ModelLoader
	{
	public:
		static Model* readModelFile(std::string modelPath, std::string texturePath,Device* device,Camera* camera);
		static std::vector<std::string> split(std::string line, char delimeter);
		
	};
}

