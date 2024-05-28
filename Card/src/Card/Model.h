#pragma once
#include "Base.h"
#include <vector>
#include "Util/Vertex.h"
namespace Card {
	class CARD_API Model
	{
	public:
		Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
		~Model();

		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();

		void moveObject(glm::vec3 newPosition);
		void resetObject();

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::vec3 position;
		glm::vec3 rotation;

	};

}

