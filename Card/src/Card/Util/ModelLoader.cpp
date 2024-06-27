#include "ModelLoader.h"
#include "../graphics/Device.h"


namespace Card {

	/// <summary>
	/// reads a modelfile and creates a model from it.
	/// </summary>
	/// <param name="modelPath">path of the modelfile</param>
	/// <param name="texturePath">path of texture file which will be given to the model</param>
	/// <param name="device">a pointer to the device</param>
	/// <param name="camera">a pointer to the camera</param>
	/// <returns>a pointer of the newly made model</returns>
	Model* ModelLoader::readModelFile(std::string modelPath, std::string texturePath,Device* device, Camera* camera)
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

			std::vector<std::string> data = ModelLoader::split(line, ' ');

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

					std::vector<std::string> split = ModelLoader::split(data[i], '/');;

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
		return new Model(vertices,indices, device,texturePath,camera);
	}

	/// <summary>
	/// splits a string up in smaller strings
	/// </summary>
	/// <param name="line">the line that needs to be split</param>
	/// <param name="delimeter">the part on which it needs to be split</param>
	/// <returns>returns the list of split strings</returns>
	std::vector<std::string> ModelLoader::split(std::string line, char delimeter)
	{
		std::vector<std::string> data;

		int start, end;
		start = end = 0;

		//split the string
		while ((start = line.find_first_not_of(delimeter, end)) != std::string::npos) {

			end = line.find(delimeter, start);

			data.push_back(line.substr(start, end - start));
		}

		return data;
	}
}

