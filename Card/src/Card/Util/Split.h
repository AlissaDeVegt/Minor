#pragma once
#include "../Base.h" 
#include "../Logger.h" 

namespace Card {

	class CARD_API Split
	{

	public:
		static std::vector<std::string> split(std::string line, char delimeter);

	};

}