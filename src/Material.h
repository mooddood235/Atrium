#pragma once

#include <glm/glm.hpp>

namespace Atrium {
	struct Material {
	public:
		Material();
		Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor);
	public:
		glm::vec3 albedoFactor;
		float roughnessFactor;
		float metallicFactor;
	};
}


