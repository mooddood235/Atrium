#include "Primitive.h"
#include <iostream>
#include <glad/glad.h>

using namespace Atrium;

Primitive::Primitive(const tinygltf::Primitive& primitive, const tinygltf::Model& model){
	LoadAttributes(primitive, model);
	LoadIndices(primitive, model);
	LoadMaterial(primitive, model);
}
void Primitive::LoadAttributes(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> tangents;
	std::vector<glm::vec2> uvs;

	for (const auto& x : primitive.attributes) {
		std::string attributeName = x.first;

		const tinygltf::Accessor& accessor = model.accessors[x.second];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		size_t totalByteOffset = accessor.byteOffset + bufferView.byteOffset;
		int stride = accessor.ByteStride(bufferView);

		const unsigned char* data = buffer.data.data() + totalByteOffset;

		if (attributeName == "POSITION") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only position data that is Vec3s of floats is supported." << std::endl;
				exit(-1);
			}
			positions.reserve(accessor.count);
			for (unsigned int i = 0; i < accessor.count; i++) positions.push_back(*(glm::vec3*)(data + stride * i));
		}
		else if (attributeName == "NORMAL") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only normal data that is Vec3s of floats is supported." << std::endl;
				exit(-1);
			}
			normals.reserve(accessor.count);
			for (unsigned int i = 0; i < accessor.count; i++) normals.push_back(*(glm::vec3*)(data + stride * i));
		}
		else if (attributeName == "TANGENT") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC4 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only tangent data that is Vec4s of floats is supported." << std::endl;
				exit(-1);
			}
			tangents.reserve(accessor.count);
			for (unsigned int i = 0; i < accessor.count; i++) tangents.push_back(*(glm::vec4*)(data + stride * i));
		}
		else if (attributeName == "TEXCOORD_0") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC2 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only UV data that is Vec2s of floats is supported." << std::endl;
				exit(-1);
			}
			uvs.reserve(accessor.count);
			for (unsigned int i = 0; i < accessor.count; i++) {
				glm::vec2 uv = *(glm::vec2*)(data + stride * i);
				uv.y = 1.0 - uv.y; // Flip uv
				uvs.push_back(uv);
			}
		}
	}
	vertices.reserve(positions.size());

	for (unsigned int i = 0; i < positions.size(); i++) {
		glm::vec3 normal = i < normals.size() ? normals[i] : glm::vec3(0.0f);
		glm::vec4 tangent = i < tangents.size() ? tangents[i] : glm::vec4(0.0f);
		glm::vec2 uv = i < uvs.size() ? uvs[i] : glm::vec2(0.0f);
		vertices.push_back(Vertex(positions[i], normal, tangent, uv));
	}
}
void Primitive::LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
	const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
	const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

	size_t totalByteOffset = accessor.byteOffset + bufferView.byteOffset;
	int stride = accessor.ByteStride(bufferView);

	const unsigned char* data = buffer.data.data() + totalByteOffset;

	if (!(accessor.type == TINYGLTF_TYPE_SCALAR
		&& (accessor.componentType == GL_UNSIGNED_SHORT || accessor.componentType == GL_UNSIGNED_INT))) {

		std::cout << "Only indices that are scaler unsigned shorts/ints are supported." << std::endl;
		exit(-1);
	}
	indices.reserve(accessor.count);
	for (unsigned int i = 0; i < accessor.count; i++) {
		if (accessor.componentType == GL_UNSIGNED_SHORT) indices.push_back(*(unsigned short*)(data + stride * i));
		else  indices.push_back(*(unsigned int*)(data + stride * i));
	}
}
void Primitive::LoadMaterial(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	if (primitive.material < 0) {
		material = Material();
		return;
	}

	const tinygltf::Material gltfMaterial = model.materials[primitive.material];
	const tinygltf::PbrMetallicRoughness pbr = gltfMaterial.pbrMetallicRoughness;

	material = Material(
		glm::vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]),
		pbr.roughnessFactor,
		pbr.metallicFactor,
		glm::vec3(gltfMaterial.emissiveFactor[0], gltfMaterial.emissiveFactor[1], gltfMaterial.emissiveFactor[2]),
		0.0f,
		1.45f,
		GetTexture(pbr.baseColorTexture, model, true),
		GetTexture(pbr.metallicRoughnessTexture, model, false),
		GetTexture(gltfMaterial.emissiveTexture, model, true),
		GetNormalTexture(gltfMaterial.normalTexture, model)
	);
	auto extension = gltfMaterial.extensions.find("KHR_materials_emissive_strength");
	if (extension != gltfMaterial.extensions.end()) {
		material.emissiveFactor *= extension->second.Get("emissiveStrength").GetNumberAsDouble();
	}
	extension = gltfMaterial.extensions.find("KHR_materials_transmission");
	if (extension != gltfMaterial.extensions.end()) {
		material.transmissionFactor = extension->second.Get("transmissionFactor").GetNumberAsDouble();
	}
	extension = gltfMaterial.extensions.find("KHR_materials_ior");
	if (extension != gltfMaterial.extensions.end()) {
		material.ior = extension->second.Get("ior").GetNumberAsDouble();
	}
}
Texture Primitive::GetTexture(const tinygltf::TextureInfo& textureInfo, const tinygltf::Model& model, bool isSRGB) {
	if (textureInfo.index < 0) return Texture();
	std::string pathFromDirectory = model.images[model.textures[textureInfo.index].source].uri;
	return Texture(model.directory + "/" + pathFromDirectory, isSRGB);
}
Texture Primitive::GetNormalTexture(const tinygltf::NormalTextureInfo& textureInfo, const tinygltf::Model& model) {
	if (textureInfo.index < 0) return Texture();
	std::string pathFromDirectory = model.images[model.textures[textureInfo.index].source].uri;
	return Texture(model.directory + "/" + pathFromDirectory, false);
}
