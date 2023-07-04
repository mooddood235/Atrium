#pragma once

#include <glm/glm.hpp>
#include "Texture.h"

namespace Atrium {
	struct Material {
	public:
		Material();
		Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor, glm::vec3 emissiveFactor
		, float transmissionFactor, float ior, Texture albedoTexture);
	public:
		glm::vec3 albedoFactor;
		float roughnessFactor;
		float metallicFactor;
		glm::vec3 emissiveFactor;
		float transmissionFactor;
		float ior;

		Texture albedoTexture;
		uint64_t albedoTextureHandle;
	};
}


