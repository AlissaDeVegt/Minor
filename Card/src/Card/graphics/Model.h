#pragma once
#include "../Base.h"

#include "Vertex.h"

#include <vector>

namespace Card {
	class CARD_API Model
	{
	public:
		Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
		~Model();

		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();

		Model moveObject(glm::vec3 newPosition);
		Model resetObject();

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		glm::vec3 position;
		glm::vec3 rotation;

	};

}

