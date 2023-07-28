#pragma once

#include <glm/glm.hpp>
#include "../Texture/Texture.h"

namespace Atrium {
	struct Material {
	public:
		Material();
		Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor, glm::vec3 emissiveFactor
		, float transmissionFactor, float ior,
			Texture albedoTexture,
			Texture metallicRoughnessTexture,
			Texture emissionTexture,
			Texture normalTexture);
	public:
		glm::vec3 albedoFactor;
		float roughnessFactor;
		float metallicFactor;
		glm::vec3 emissiveFactor;
		float transmissionFactor;
		float ior;

		Texture albedoTexture;
		Texture metallicRoughnessTexture;
		Texture emissionTexture;
		Texture normalTexture;
	};
}


