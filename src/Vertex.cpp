#include "Vertex.h"

using namespace Atrium;

Vertex::Vertex() {
	this->position = glm::vec3(0.0f);
	this->normal = glm::vec3(0.0f);
	this->uv = glm::vec2(0.0f);
}

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) {
	this->position = position;
	this->normal = normal;
	this->uv = uv;
}
