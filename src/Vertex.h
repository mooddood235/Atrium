#pragma once
#include <glm/glm.hpp>

namespace Atrium {
	class Vertex{
	public:
		Vertex();
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec4 tangent, glm::vec2 uv);
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 uv;
	};
}


