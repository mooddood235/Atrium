#include "Vertex.h"

using namespace Atrium;

Vertex::Vertex() {
	this->position = glm::vec3(0.0f);
}

Vertex::Vertex(glm::vec3 position) {
	this->position = position;
}
