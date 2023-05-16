#pragma once
#include <glm/glm.hpp>

namespace Atrium {
	class Vertex{
	public:
		Vertex();
		Vertex(glm::vec3 position);
	public:
		glm::vec3 position;
	};
}


