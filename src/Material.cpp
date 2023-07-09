#include "Material.h"

using namespace Atrium;

Material::Material() {
	albedoFactor = glm::vec3(1.0f);
	roughnessFactor = 1.0f;
	metallicFactor = 0.0f;
	emissiveFactor = glm::vec3(0.0f);
	transmissionFactor = 0.0f;
	ior = 1.45f;

	albedoTexture = Texture();
	metallicRoughnessTexture = Texture();
	normalTexture = Texture();
}

Material::Material(glm::vec3 albedoFactor, float roughnessFactor, float metallicFactor, glm::vec3 emissiveFactor
	, float transmissionFactor, float ior, Texture albedoTexture, Texture metallicRoughnessTexture, Texture normalTexture) {
	this->albedoFactor = albedoFactor;
	this->roughnessFactor = roughnessFactor;
	this->metallicFactor = metallicFactor;
	this->emissiveFactor = emissiveFactor;
	this->transmissionFactor = transmissionFactor;
	this->ior = ior;
	
	this->albedoTexture = albedoTexture;
	this->metallicRoughnessTexture = metallicRoughnessTexture;
	this->normalTexture = normalTexture;
}
