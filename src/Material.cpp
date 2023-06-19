#include "Material.h"

using namespace Atrium;

Material::Material() {
	albedoFactor = glm::vec3(1.0f);
	roughnessFactor = 1.0f;
	metallicFactor = 0.0f;
	emissiveFactor = glm::vec3(0.0f);
}

Material::Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor, glm::vec3 emissiveFactor){
	this->albedoFactor = albedoFactor;
	this->roughnessFactor = roughnessFactor;
	this->metallicFactor = metallicFactor;
	this->emissiveFactor = emissiveFactor;
}
