#include "Vertex.h"

using namespace Atrium;

Vertex::Vertex() {
	this->position = glm::vec3(0.0f);
	this->normal = glm::vec3(0.0f);
	this->tangent = glm::vec3(0.0f);
	this->bitangent = glm::vec3(0.0f);
	this->uv = glm::vec2(0.0f);
}

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec4 tangent, glm::vec2 uv) {
	this->position = position;
	this->normal = normal;
	this->tangent = tangent;
	bitangent = glm::normalize(glm::cross(normal, glm::vec3(tangent)) * tangent.w);
	this->uv = uv;
}
