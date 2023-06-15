#include "Mesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

using namespace Atrium;


Mesh::Mesh(const tinygltf::Node& gltfNode, Transform parentTransform, const tinygltf::Model& model) : Node(gltfNode, parentTransform) {
	if (gltfNode.mesh < 0) {
		std::cout << "Tried to create Mesh node from non-mesh gltf node '" << gltfNode.name << "'!" << std::endl;
		exit(-1);
	}
	type = NodeType::Mesh;

	const tinygltf::Primitive primitive = model.meshes[gltfNode.mesh].primitives[0];
	LoadAttributes(primitive, model);
	LoadIndices(primitive, model);	
}
void Mesh::LoadAttributes(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

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
			positions.resize(accessor.count);
			for (unsigned int i = 0; i < positions.size(); i++) positions[i] = *(glm::vec3*)(data + stride * i);
		}
		else if (attributeName == "NORMAL") {
			if (!(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == GL_FLOAT)) {
				std::cout << "LOAD GLTF ERROR: Only normal data that is Vec3s of floats is supported." << std::endl;
				exit(-1);
			}
			normals.resize(accessor.count);
			for (unsigned int i = 0; i < normals.size(); i++) normals[i] = *(glm::vec3*)(data + stride * i);
		}
	}
	if (positions.size() != normals.size()) {
		std::cout << "LOAD GLTF ERROR: Mismatching attribute count." << std::endl;
		exit(-1);
	}
	vertices.reserve(positions.size());

	for (unsigned int i = 0; i < positions.size(); i++) vertices.push_back(Vertex(positions[i], normals[i]));
}
void Mesh::LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
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
void Mesh::LoadMaterial(const tinygltf::Primitive& primitive, const tinygltf::Model& model){
	const tinygltf::Material gltfMaterial = model.materials[primitive.material];
	const tinygltf::PbrMetallicRoughness pbr = gltfMaterial.pbrMetallicRoughness;

	material = Material(
		glm::vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]),
		pbr.roughnessFactor,
		pbr.metallicFactor
	);
}
