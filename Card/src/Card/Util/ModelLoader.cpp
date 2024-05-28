#include "ModelLoader.h"


namespace Card {

	Model ModelLoader::readModelFile(std::string modelPath)
	{
		int vertexindex = 0 ;
		int textureindex = 0;
		std::string line;

		std::ifstream modelFile(modelPath.c_str());
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<glm::vec2> textcoord;
		std::vector<uint32_t> textindex;


		while (getline(modelFile, line)) {

			std::vector<std::string> data = Split::split(line, ' ');

			if (data[0] == "v") {
				Vertex vertex{};
				vertex.pos = {
					std::stof(data[1]),
					std::stof(data[2]), 
					std::stof(data[3]) 
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };
				vertices.push_back(vertex);

			}
			else if (data[0] == "vt") {
				glm::vec2 vertexcord = glm::vec2(std::stof(data[1]), 1.0f - std::stof(data[2]));

				textcoord.push_back(vertexcord);
			}
			else if (data[0] == "f") {
				for (int i = 1; i < 4; i++) {

					std::vector<std::string> split = Split::split(data[i], '/');;

					//TODO unique vertices?

					//indexes in obj files are always one higher than the actaul index in list
					indices.push_back(stoi(split[0])-1);
					textindex.push_back(stoi(split[1])-1);
				}
			}	
		}

		for (int i = 0; i < indices.size(); i++)
		{
			vertices[indices[i]].texCoord = textcoord[textindex[i]];
		}

		

		CARD_ENGINE_INFO("ModelLoaded");
		return Model(vertices,indices);
	}
}

