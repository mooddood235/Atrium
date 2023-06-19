#pragma once

#include <glm/glm.hpp>

namespace Atrium {
	struct Material {
	public:
		Material();
		Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor, glm::vec3 emissiveFactor);
	public:
		glm::vec3 albedoFactor;
		float roughnessFactor;
		float metallicFactor;
		glm::vec3 emissiveFactor;
	};
}


