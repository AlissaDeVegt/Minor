#include "Model.h"

namespace Card {
	Model::Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	{
		this->indices = indices;
		this->vertices = vertices;
	}
	Model::~Model()
	{

	}
	std::vector<Vertex> Model::getVertices()
	{
		return vertices;
	}
	std::vector<uint32_t> Model::getIndices()
	{
		return indices;
	}

	Model Model::moveObject(glm::vec3 newPosition)
	{
		position = newPosition;

		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x + position.x;
			v->pos.y = v->pos.y + position.y;
			v->pos.z = v->pos.z + position.z;
		}
		return *this;
	}
	Model Model::resetObject()
	{
		for (int i = 0; i < vertices.size(); i++) {
			Vertex* v = &vertices[i];
			v->pos.x = v->pos.x - position.x;
			v->pos.y = v->pos.y - position.y;
			v->pos.z = v->pos.z - position.z;
		}
		return *this;
	}
}