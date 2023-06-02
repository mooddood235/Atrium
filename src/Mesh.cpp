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
		if (attributeName == "NORMAL") {
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
	vertices.resize(positions.size());

	for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] = Vertex(positions[i], normals[i]);
}
void Mesh::LoadIndices(const tinygltf::Primitive& primitive, const tinygltf::Model& model) {
	const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
	const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
	const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

	size_t totalByteOffset = accessor.byteOffset + bufferView.byteOffset;
	int stride = accessor.ByteStride(bufferView);

	const unsigned char* data = buffer.data.data() + totalByteOffset;

	if (!(accessor.type == TINYGLTF_TYPE_SCALAR && accessor.componentType == GL_UNSIGNED_SHORT)) {
		std::cout << "Only indices that are scaler unsigned shorts are supported." << std::endl;
		exit(-1);
	}
	indices.resize(accessor.count);
	for (unsigned int i = 0; i < indices.size(); i++) indices[i] = *(unsigned short*)(data + stride * i);
}