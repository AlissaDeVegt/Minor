#pragma once
#include "../Base.h"
#include "../Logger.h"
#include "../Window.h"
#include "../Model.h"
#include "Vertex.h"
#include "Split.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Card {

	class CARD_API ModelLoader
	{
	public:
		static Model readModelFile(std::string modelPath);
		//todo Model instead of floating vertices
	};
}

